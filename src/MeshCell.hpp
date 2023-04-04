#ifndef CONVEX_TRIANGULATIONS_MESHCELLS_HPP
#define CONVEX_TRIANGULATIONS_MESHCELLS_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <iostream>

class EdgeCell
{
public:
    std::size_t a, b;

    EdgeCell();
    explicit EdgeCell(std::size_t a, std::size_t b);
    EdgeCell(const EdgeCell& l);
    ~EdgeCell();

    [[nodiscard]] bool undefined() const;
    bool operator<(const EdgeCell& rhs) const;
    bool operator==(const EdgeCell& rhs) const;

    friend std::size_t hash_value(const EdgeCell& l);
};

class TriangleCell
{
public:
    std::size_t a, b, c;

    TriangleCell();
    explicit TriangleCell(std::size_t a, std::size_t b, std::size_t c);
    TriangleCell(const TriangleCell& t);
    ~TriangleCell();

    [[nodiscard]] std::size_t oppositePoint(const EdgeCell& l) const;
    [[nodiscard]] bool undefined() const;

    bool operator<(const TriangleCell& rhs) const;
    bool operator==(const TriangleCell& rhs) const;
    bool operator!=(const TriangleCell& rhs) const;

    friend std::size_t hash_value(const TriangleCell& l);
};

std::ostream& operator<<(std::ostream& out, const EdgeCell& t);

std::ostream& operator<<(std::ostream& out, const TriangleCell& t);

template<>
struct std::hash<EdgeCell>
{
    std::size_t operator()(const EdgeCell& l) const
    {
        return boost::hash<EdgeCell>()(l);
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