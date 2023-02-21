#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <chrono>
#include <memory>
#include <random>
#include <list>
#include "Point.hpp"
#include "MeshTriangulation.hpp"

class ListUtil
{
public:
    using list_iterator = std::list<std::size_t>::iterator;
    static std::list<std::size_t>::iterator
    eraseCircular(std::list<std::size_t>& lst, std::list<std::size_t>::iterator b, std::list<std::size_t>::iterator e)
    {
        if(b == e)
            return e;

        auto b_c = b;
        while(b_c != e)
        {
            if(b_c++ == lst.end())
            {
                long dist = std::distance(b, e) - 1;
                lst.splice(lst.end(), lst, e, b);
                return lst.erase(lst.begin(), std::next(lst.begin(), dist));
            }
        }

        return lst.erase(b, e);
    }
};

class Timer
{
public:
    Timer(bool printOnInvalidation, const std::string& msg)
        : printOnInvalidation_(printOnInvalidation), msg_(msg)
    {
        startTimepoint_ = std::chrono::high_resolution_clock::now();
    }
    explicit Timer(bool printOnInvalidation = true)
        : Timer(true, "Timer")
    {
    }
    ~Timer()
    {
        if(printOnInvalidation_)
            std::cout << msg_ << " : " << Duration() << "us\n";
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
    std::string msg_;
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

class MeshFactory
{
public:
    static std::unique_ptr<MeshTriangulation> makeMesh(const std::vector<Point>& coord)
    {
        std::unique_ptr<MeshTriangulation> mesh =
            std::make_unique<MeshTriangulation>(coord);
        mesh->triangulate();
        return mesh;
    }

    static std::unique_ptr<MeshTriangulation> makeMeshSingle(double x)
    {
        return MeshFactory::makeMesh(
            {Point(0, 0), Point(x, 0), Point(0, x)}
        );
    }
};

#endif //UTILS_HPP
