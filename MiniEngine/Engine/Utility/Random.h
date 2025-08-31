#pragma once

#include <random>

class Random
{
public:
    static void Init()
    {
        sRandomEngine.seed(std::random_device()());
    }

    ///返回0~1的float数
    static float Float()
    {
        return static_cast<float>(sDistribution(sRandomEngine))
            / static_cast<float>((std::numeric_limits<uint32_t>::max)());
        // 注意：max 被括号包裹，防止被替换成宏
    }

private:
    static std::mt19937 sRandomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> sDistribution;
};
