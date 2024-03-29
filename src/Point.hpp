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
    using ValueType = std::double_t;
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
    [[nodiscard]] std::double_t distance(const Point& to) const;
    friend std::size_t hash_value(const Point& pt);
    static std::double_t dot(const Point& lhs, const Point& rhs);
    static std::double_t cross(const Point& lhs, const Point& rhs);
    static std::double_t cross(const Point& p1, const Point& p2, const Point& p3);
    static std::pair<Point, std::double_t> circumcircle(const Point& p1, const Point& p2, const Point& p3);
};

template<>
struct std::hash<Point>
{
    std::size_t operator()(const Point& pt) const noexcept
    {
        return boost::hash<Point>()(pt); // or use boost::hash_combine
    }
};

#endif //CONVEX_TRIANGULATIONS_POINT_HPP