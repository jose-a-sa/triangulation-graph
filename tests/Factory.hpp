//
// Created by Jose Sa on 21/02/2023.
//

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "Point.hpp"

class PointFactory
{
public:
    static std::shared_ptr<std::vector<Point>>
    randomSample(std::size_t n = 1, std::double_t a = 0.0, std::double_t b = 1.0)
    {
        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_real_distribution<> dist(a, b);

        auto res = std::make_shared<std::vector<Point>>();
        res->reserve(n);
        for(std::size_t i = 0; i < n; i++)
        {
            res->emplace_back(dist(eng), dist(eng));
        }
        return res;
    }

    static std::shared_ptr<std::vector<Point>> makeSimplex(std::double_t a = 1.0)
    {
        auto res = std::make_shared<std::vector<Point>>();
        res->reserve(3);
        res->push_back(Point(0, 0));
        res->push_back(Point(a, 0));
        res->push_back(Point(0, a));
        return res;
    }
};

#endif //FACTORY_HPP
