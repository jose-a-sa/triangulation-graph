#ifndef CONVEX_TRIANGULATIONS_POINT_HPP
#define CONVEX_TRIANGULATIONS_POINT_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <utility>
#include <initializer_list>
#include <string>

class Point
{
public:
    using ValueType = double;
    ValueType x, y;
    Point();
    Point(const Point& pt);
    Point(Point&& pt) noexcept;
    explicit Point(ValueType x, ValueType y);
    Point(std::initializer_list<ValueType>&& v);
    ~Point();
    Point& operator=(const Point& pt);
    Point& operator=(Point&& pt);
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
    static double circumcircleRadius(const Point& p1, const Point& p2, const Point& p3);
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

#endif //CONVEX_TRIANGULATIONS_POINT_HPP