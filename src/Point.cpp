#include "Point.hpp"

#include <sstream>
#include <cmath>
#include <stdexcept>

Point::Point()
	: x(0), y(0)
{
}

Point::Point(Point::ValueType x)
	: x(x), y(x)
{
}

Point::Point(Point::ValueType x, Point::ValueType y)
	: x(x), y(y)
{
}

Point::Point(std::initializer_list<Point::ValueType>&& v)
	: x(0), y(0)
{
	std::size_t i = 0;
	for (const auto& elem : v)
		this->at(i++) = elem;
}

Point::~Point() = default;

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

inline Point::ValueType Point::distanceSq(const Point& to) const
{
	return (this->x - to.x) * (this->x - to.x) + (this->y - to.y) * (this->y - to.y);
}

double Point::distance(const Point& to) const
{
	return std::sqrt(this->distanceSq(to));
}

std::size_t hash_value(const Point& pt)
{
	std::size_t seed = 0;
	std::hash<Point::ValueType> hash{};
	seed ^= hash(pt.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= hash(pt.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	return seed; // or use boost::hash_combine
}