#include "Point.hpp"

#include <sstream>
#include <cmath>
#include <stdexcept>

Point::Point()
    : x(0), y(0)
{
}

Point::Point(Point::ValueType x, Point::ValueType y)
    : x(x), y(y)
{
}

Point::Point(std::initializer_list<Point::ValueType>&& v)
    : x(0), y(0)
{
    if(v.size() != 2)
        throw std::out_of_range("Initializer list must be dimension 2");

    std::size_t i = 0;
    for(const auto& elem : v)
        this->at(i++) = elem;
}

Point::Point(const Point& pt) = default;

Point::Point(Point&& pt) noexcept = default;

Point::~Point() = default;

Point& Point::operator=(const Point& pt)
{
    x = pt.x;
    y = pt.y;
    return *this;
}

Point& Point::operator=(Point&& pt)
{
    x = pt.x;
    y = pt.y;
    return *this;
}

bool Point::operator<(const Point& rhs) const
{
    if(this->x != rhs.x)
        return this->x < rhs.x;
    return this->y < rhs.y;
}

bool Point::operator==(const Point& rhs) const
{
    return this->x == rhs.x && this->y == rhs.y;
}

Point::ValueType& Point::at(std::size_t i)
{
	if (i >= 2)
		throw std::out_of_range("Point has only 2 elements");

    return i == 0 ? this->x : this->y;
}

std::string Point::toString() const
{
    std::ostringstream oss;
    oss << "{" << this->x << "," << this->y << "}";
    return oss.str();
}

double Point::distance(const Point& to) const
{
    return std::sqrt((this->x - to.x) * (this->x - to.x) + (this->y - to.y) * (this->y - to.y));
}

Point Point::operator+(const Point& rhs) const
{
    Point cp(*this);
    cp += rhs;
    return cp;
}

Point Point::operator-(const Point& rhs) const
{
    Point cp(*this);
    cp -= rhs;
    return cp;
}

Point& Point::operator+=(const Point& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

Point& Point::operator-=(const Point& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

std::size_t hash_value(const Point& pt)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, pt.x);
    boost::hash_combine(seed, pt.y);
    return seed; // or use boost::hash_combine
}

double Point::dot(const Point& lhs, const Point& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

double Point::cross(const Point& lhs, const Point& rhs)
{
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

double Point::cross(const Point& p1, const Point& p2, const Point& p3)
{
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

std::pair<Point, double> Point::circumcircle(const Point& p1, const Point& p2, const Point& p3)
{
    Point p = p2 - p1, q = p3 - p1;
    double pnorm2 = Point::dot(p, p);
    double qnorm2 = Point::dot(q, q);
    double den = 2.0 * Point::cross(p, q);
    Point CC = {(q.y * pnorm2 - p.y * qnorm2) / den, (p.x * qnorm2 - q.x * pnorm2) / den};
    double radius = std::sqrt(Point::dot(CC, CC));
    CC += p1;
    return {CC, radius};
}
