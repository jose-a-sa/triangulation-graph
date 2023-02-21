#ifndef CONVEX_TRIANGULATIONS_MESHCELLS_HPP
#define CONVEX_TRIANGULATIONS_MESHCELLS_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <iostream>

class LineCell
{
public:
    std::size_t a, b;

    LineCell();
    explicit LineCell(std::size_t a, std::size_t b);
    LineCell(const LineCell& l);
    ~LineCell();

    bool operator<(const LineCell& rhs) const;
    bool operator==(const LineCell& rhs) const;

    friend std::size_t hash_value(const LineCell& l);
};

class TriangleCell
{
public:
    std::size_t a, b, c;

    TriangleCell();
    explicit TriangleCell(std::size_t a, std::size_t b, std::size_t c);
    TriangleCell(const TriangleCell& t);
    ~TriangleCell();

    std::size_t oppositePoint(const LineCell& l) const;

    bool operator<(const TriangleCell& rhs) const;
    bool operator==(const TriangleCell& rhs) const;
    bool operator!=(const TriangleCell& rhs) const;

    friend std::size_t hash_value(const TriangleCell& l);
};

std::ostream& operator<<(std::ostream& out, const LineCell& t);

std::ostream& operator<<(std::ostream& out, const TriangleCell& t);

template<>
struct std::hash<LineCell>
{
    std::size_t operator()(const LineCell& l) const
    {
        return boost::hash<LineCell>()(l);
    }
};

template<>
struct std::hash<TriangleCell>
{
    std::size_t operator()(const TriangleCell& t) const
    {
        return boost::hash<TriangleCell>()(t);
    }
};


#endif //CONVEX_TRIANGULATIONS_MESHCELLS_HPP