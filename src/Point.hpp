#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <initializer_list>
#include <sstream>
#include <string>

template<typename T, std::size_t D>
class Point : public std::array<T, D>
{
public:
	using ValueType = T;
	Point();
	explicit Point(ValueType x);
	Point(std::initializer_list<ValueType>&& v);
	~Point();
	Point<ValueType, D>& operator+=(const Point<ValueType, D>& rhs);
	Point<ValueType, D>& operator-=(const Point<ValueType, D>& rhs);
	Point<ValueType, D>& operator*=(const ValueType& rhs);
	const Point<ValueType, D>& operator+(const Point<ValueType, D>& rhs) const;
	const Point<ValueType, D>& operator-(const Point<ValueType, D>& rhs) const;
	const Point<ValueType, D>& operator*(const ValueType& rhs) const;
	std::string toString() const;
	std::string wtk() const;
	T distanceSq(const Point<ValueType, D>& to) const;
	double distance(const Point<ValueType, D>& to) const;
};

template<typename T, std::size_t D>
Point<T, D>::Point() = default;

template<typename T, std::size_t D>
inline Point<T, D>::Point(T x)
{
	this->fill(x);
}

template<typename T, std::size_t D>
Point<T, D>::Point(std::initializer_list<T>&& v)
{
	std::size_t i = 0;
	for (const T& elem : v)
	{
		(*this)[i++] = elem;
		if (i >= D)
			break;
	}
}

template<typename T, std::size_t D>
Point<T, D>::~Point() = default;

template<typename T, std::size_t D>
Point<T, D>& Point<T, D>::operator+=(const Point<T, D>& rhs)
{
	std::transform(this->begin(), this->end(), rhs.begin(),
		this->begin(), std::plus<T>());
	return *this;
}

template<typename T, std::size_t D>
Point<T, D>& Point<T, D>::operator-=(const Point<T, D>& rhs)
{
	std::transform(this->begin(), this->end(), rhs.begin(),
		this->begin(), std::minus<T>());
	return *this;
}

template<typename T, std::size_t D>
Point<T, D>& Point<T, D>::operator*=(const T& rhs)
{
	std::transform(this->begin(), this->end(), this->begin(),
		[&](const T& x) -> T
		{
			return x * rhs;
		});
	return *this;
}

template<typename T, std::size_t D>
inline const Point<T, D>& Point<T, D>::operator+(const Point<T, D>& rhs) const
{
	return Point<T, D>(*this) += rhs;
}

template<typename T, std::size_t D>
inline const Point<T, D>& Point<T, D>::operator-(const Point<T, D>& rhs) const
{
	return Point<T, D>(*this) -= rhs;
}

template<typename T, std::size_t D>
inline std::string Point<T, D>::toString() const
{
	std::ostringstream oss;
	oss << "{";
	for (auto it = this->begin(); it != this->end(); it++)
	{
		oss << (*it >= 0 &&  it != this->begin() ? " " : "");
		oss << *it;
		oss << (it != std::prev(this->end()) ? "," : "}");
	}
	return oss.str();
}

template<typename T, std::size_t D>
inline std::string Point<T, D>::wtk() const
{
	std::ostringstream oss;
	for (auto it = this->begin(); it != this->end(); it++)
	{
		oss << *it;
		oss << (it != std::prev(this->end()) ? " " : "");
	}
	return oss.str();
}

template<typename T, std::size_t D>
inline T Point<T, D>::distanceSq(const Point<ValueType, D>& to) const
{
	T res = 0;
	for (std::size_t i = 0; i < D; i++)
	{
		T diff = (*this)[i] - to[i];
		res += diff * diff;
	}
	return res;
}

template<typename T, std::size_t D>
inline double Point<T, D>::distance(const Point<ValueType, D>& to) const
{
	return std::sqrt(this->distanceSq(to));
}

template<typename T, std::size_t D>
inline const Point<T, D>& Point<T, D>::operator*(const T& rhs) const
{
	return Point<T, D>(*this) *= rhs;
}

template<typename T, std::size_t D>
const Point<T, D>& operator*(const T& lhs, const Point<T, D>& rhs)
{
	return Point<T, D>(*rhs) *= lhs;
}

template<typename T, std::size_t D>
std::ostream& operator<<(std::ostream& stream, const Point<T, D>& pt)
{
	return stream << pt.toString();
}

template<typename T, std::size_t D>
struct std::hash<Point<T, D>>
{
	std::size_t operator()(const Point<T, D>& pt) const noexcept
	{
		std::size_t seed = 0;
		std::hash<T> hash{};
		for (const T& c : pt)
			seed ^= hash(c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed; // or use boost::hash_combine
	}
};