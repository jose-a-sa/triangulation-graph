#ifndef CONVEX_TRIANGULATIONS_POINT_HPP
#define CONVEX_TRIANGULATIONS_POINT_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <initializer_list>
#include <string>

class Point
{
public:
	using ValueType = double;
	ValueType x, y;
	Point();
	explicit Point(ValueType x);
	explicit Point(ValueType x, ValueType y);
	Point(std::initializer_list<ValueType>&& v);
	~Point();
	ValueType& at(std::size_t i);
	std::string toString() const;
	ValueType distanceSq(const Point& to) const;
	double distance(const Point& to) const;
	friend std::size_t hash_value(const Point& pt);
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