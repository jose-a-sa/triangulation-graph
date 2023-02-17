#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <chrono>
#include <random>
#include "Point.hpp"

class Timer
{
public:
    Timer(bool printOnInvalidation = true)
    {
        printOnInvalidation_ = printOnInvalidation;
        startTimepoint_ = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        if(printOnInvalidation_)
            std::cout << "Timer :" << Duration() << "us";
    }

    double Duration()
    {
        auto endTimepoint_ = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint_).time_since_epoch();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint_).time_since_epoch();
        std::chrono::duration<double, std::micro> duration = end - start;
        return duration.count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint_;
    bool printOnInvalidation_;
};

class RandomPoint
{
public:
    RandomPoint(double a = 0.0L, double b = 1.0L)
    {
        std::random_device rd;
        eng_ = std::mt19937(rd());
        dist_ = std::uniform_real_distribution<>(a, b);
    }

    ~RandomPoint() = default;

    std::vector<Point> generate(std::size_t n = 1)
    {
        std::vector<Point> res;
        res.reserve(n);
        for(std::size_t i = 0; i < n; i++)
        {
            res.emplace_back(dist_(eng_), dist_(eng_));
        }
        return res;
    }

private:
    std::mt19937 eng_;
    std::uniform_real_distribution<> dist_;
};

#endif //UTILS_HPP
