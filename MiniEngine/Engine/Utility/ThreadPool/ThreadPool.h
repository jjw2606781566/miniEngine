#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <atomic>
#include <string>

enum class RejectionPolicy {
    ThrowException,
    DiscardNew,
    DiscardOldest
};

class ThreadPool {
public:
    ThreadPool(size_t coreThreads,
        size_t maxThreads,
        size_t maxQueueSize = 1000,
        RejectionPolicy policy = RejectionPolicy::ThrowException);

    ~ThreadPool();

    // 模板函数定义必须留在头文件
    template<class F, class... Args>
    auto enqueue(int priority, F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        std::shared_ptr<std::promise<void>> cancelFlag =
            std::make_shared<std::promise<void>>();

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            if (!running) {
                throw std::runtime_error("Enqueue on stopped thread pool");
            }

            if (totalTaskCount >= maxQueue) {
                switch (policy) {
                case RejectionPolicy::ThrowException:
                    throw std::runtime_error("Task queue full");
                case RejectionPolicy::DiscardNew:
                    return res;
                case RejectionPolicy::DiscardOldest:
                    if (!priorityQueue.empty()) {
                        priorityQueue.pop();
                        totalTaskCount--;
                    }
                    break;
                }
            }

            priorityQueue.emplace(Task{ priority, cancelFlag, [task]() { (*task)(); } });
            totalTaskCount++;
        }

        condition.notify_one();
        return res;
    }

    void resize(size_t newSize);
    void shutdown();

    size_t getActiveCount() const;
    size_t getQueueSize() const;
    bool isShutdown() const;

private:
    struct Task {
        std::function<void()> function;
        int priority;
        std::shared_ptr<std::promise<void>> cancelFlag;

        Task(int p = 0, std::shared_ptr<std::promise<void>> c = nullptr,
            std::function<void()> f = {})
            : priority(p), cancelFlag(c), function(f) {
        }
        bool operator<(const Task& other) const {
            return priority < other.priority;
        }
    };

    void workerThread();
    bool addWorker();
    void stopWorker();

    std::atomic_bool running;
    std::atomic_size_t coreSize;
    std::atomic_size_t maxSize;
    std::atomic_size_t maxQueue;
    RejectionPolicy policy;

    std::vector<std::thread> workers;
    std::priority_queue<Task> priorityQueue;

    mutable std::mutex queueMutex;
    std::condition_variable condition;

    std::atomic_size_t activeCount;
    std::atomic_size_t totalTaskCount;
};