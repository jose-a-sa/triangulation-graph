#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <initializer_list>
#include <sstream>
#include <string>

template <typename T, std::size_t D>
class Point : public std::array<T, D>
{
public:
    using ValueType = T;
    Point();
    Point(ValueType x);
    Point(std::initializer_list<ValueType> &&v);
    ~Point();
    Point<ValueType, D> &operator+=(const Point<ValueType, D> &rhs);
    Point<ValueType, D> &operator-=(const Point<ValueType, D> &rhs);
    Point<ValueType, D> &operator*=(const ValueType &rhs);
    const Point<ValueType, D> &operator+(const Point<ValueType, D> &rhs) const;
    const Point<ValueType, D> &operator-(const Point<ValueType, D> &rhs) const;
    const Point<ValueType, D> &operator*(const ValueType &rhs) const;
    const std::string toString() const;
};

template <typename T, std::size_t D>
inline Point<T, D>::Point()
{
}

template <typename T, std::size_t D>
inline Point<T, D>::Point(ValueType x)
{
    this->fill(x);
}

template <typename T, std::size_t D>
inline Point<T, D>::~Point()
{
}

template <typename T, std::size_t D>
inline Point<T, D>::Point(std::initializer_list<T> &&v)
{
    std::size_t i = 0;
    for (const T &elem : v)
    {
        (*this)[i++] = elem;
        if (i >= D)
            break;
    }
}

template <typename T, std::size_t D>
inline Point<T, D> &Point<T, D>::operator+=(const Point<T, D> &rhs)
{
    std::transform(this->begin(), this->end(), rhs.begin(),
                   this->begin(), std::plus<T>());
    return *this;
}

template <typename T, std::size_t D>
inline Point<T, D> &Point<T, D>::operator-=(const Point<T, D> &rhs)
{
    std::transform(this->begin(), this->end(), rhs.begin(),
                   this->begin(), std::minus<T>());
    return *this;
}

template <typename T, std::size_t D>
inline Point<T, D> &Point<T, D>::operator*=(const T &rhs)
{
    std::transform(
        this->begin(), this->end(), this->begin(),
        [&](const T &x) -> T
        {
            return x * rhs;
        });
    return *this;
}

template <typename T, std::size_t D>
inline const Point<T, D> &Point<T, D>::operator+(const Point<T, D> &rhs) const
{
    return Point<T, D>(*this) += rhs;
}

template <typename T, std::size_t D>
inline const Point<T, D> &Point<T, D>::operator-(const Point<T, D> &rhs) const
{
    return Point<T, D>(*this) -= rhs;
}

template <typename T, std::size_t D>
inline const std::string Point<T, D>::toString() const
{
    std::ostringstream res;
    res << "{";
    typename Point<T, D>::const_iterator it;
    for (it = this->cbegin(); it < this->cend() - 1; it++)
    {
        res << " " << (*it) << ",";
    }
    res << " " << (*it) << "}";
    return res.str();
}

template <typename T, std::size_t D>
inline const Point<T, D> &Point<T, D>::operator*(const T &rhs) const
{
    return Point<T, D>(*this) *= rhs;
}

template <typename T, std::size_t D>
const Point<T, D> &operator*(const T &lhs, const Point<T, D> &rhs)
{
    return Point<T, D>(*rhs) *= lhs;
}

template <typename T, std::size_t D>
struct std::hash<Point<T, D>>
{
    std::size_t operator()(Point<T, D> const &pt) const noexcept
    {
        std::size_t seed = 0;
        std::hash<T> hasher{};
        for (const T &c : pt)
            seed ^= hasher(c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed; // or use boost::hash_combine
    }
};

template <typename T, std::size_t D>
std::ostream &operator<<(std::ostream &stream, const Point<T, D> &pt)
{
    return stream << pt.toString();
}
