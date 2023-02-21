#ifndef CONVEX_TRIANGULATIONS_POINT_HPP
#define CONVEX_TRIANGULATIONS_POINT_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <utility>
#include <initializer_list>
#include <string>
#include <random>

class Point
{
public:
    using ValueType = double;
    ValueType x, y;
    Point();
    explicit Point(ValueType x, ValueType y);
    Point(std::initializer_list<ValueType>&& v);
    ~Point();
    bool operator<(const Point& rhs) const;
    bool operator==(const Point& rhs) const;
    Point operator+(const Point& rhs) const;
    Point operator-(const Point& rhs) const;
    Point& operator+=(const Point& rhs);
    Point& operator-=(const Point& rhs);
    ValueType& at(std::size_t i);
    std::string toString() const;
    double distance(const Point& to) const;
    friend std::size_t hash_value(const Point& pt);
    static double dot(const Point& lhs, const Point& rhs);
    static double cross(const Point& lhs, const Point& rhs);
    static double cross(const Point& p1, const Point& p2, const Point& p3);
    static std::pair<Point, double> circumcircle(const Point& p1, const Point& p2, const Point& p3);
};

template<>
struct std::hash<Point>
{
    std::size_t operator()(const Point& pt) const noexcept
    {
        return boost::hash<Point>()(pt); // or use boost::hash_combine
    }
};

class PointFactory
{
public:
    static std::vector<Point> randomSample(std::size_t n = 1, double a = 0.0, double b = 1.0)
    {
        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_real_distribution<> dist(a, b);

        std::vector<Point> res;
        res.reserve(n);
        for(std::size_t i = 0; i < n; i++)
        {
            res.emplace_back(dist(eng), dist(eng));
        }
        return res;
    }
};

#endif //CONVEX_TRIANGULATIONS_POINT_HPP