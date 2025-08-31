#include "ThreadPool.h"
#include <iostream>
#include <limits>

// 非模板成员函数的实现必须放在.cpp文件
ThreadPool::ThreadPool(size_t coreThreads, size_t maxThreads,
    size_t maxQueueSize, RejectionPolicy policy)
    : running(true), coreSize(coreThreads), maxSize(maxThreads),
    maxQueue(maxQueueSize), policy(policy),
    activeCount(0), totalTaskCount(0) {
    if (coreThreads < 1 || maxThreads < coreThreads || maxQueueSize < 1) {
        throw std::invalid_argument("Invalid thread pool parameters");
    }
    for (size_t i = 0; i < coreThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::workerThread() {
    while (running) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return !running || !priorityQueue.empty(); });
            if (!running) return;
            if (priorityQueue.empty()) continue;
            task = priorityQueue.top();
            priorityQueue.pop();
            totalTaskCount--;
        }
        if (task.cancelFlag && task.cancelFlag->get_future().wait_for(std::chrono::seconds(0))
            == std::future_status::ready) {
            continue;
        }
        activeCount++;
        try {
            if (task.function) task.function();
        }
        catch (...) {}
        activeCount--;
    }
}

void ThreadPool::resize(size_t newSize) {
    if (newSize < coreSize || newSize > maxSize) return;
    std::unique_lock<std::mutex> lock(queueMutex);
    while (workers.size() < newSize) {
        if (addWorker()) {
            workers.emplace_back(&ThreadPool::workerThread, this);
        }
    }
    while (workers.size() > newSize) {
        stopWorker();
    }
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        running = false;
    }
    condition.notify_all();
    for (std::thread& worker : workers) {
        if (worker.joinable()) worker.join();
    }
}

size_t ThreadPool::getActiveCount() const {
    return activeCount;
}

size_t ThreadPool::getQueueSize() const {
    return totalTaskCount;
}

bool ThreadPool::isShutdown() const {
    return !running;
}

void ThreadPool::stopWorker() {
    priorityQueue.emplace(Task{ std::numeric_limits<int>::max() });
    condition.notify_one();
}

bool ThreadPool::addWorker() {
    return workers.size() < maxSize;
}