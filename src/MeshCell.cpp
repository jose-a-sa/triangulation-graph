#include "MeshCell.hpp"

EdgeCell::EdgeCell()
    : a(-1), b(-1)
{
}

EdgeCell::EdgeCell(std::size_t a, std::size_t b)
    : a(a), b(b)
{
    if(this->a > this->b)
        std::swap(this->a, this->b);
}

EdgeCell::EdgeCell(const EdgeCell& l) = default;

EdgeCell::~EdgeCell() = default;

bool EdgeCell::operator<(const EdgeCell& rhs) const
{
    return (this->a != rhs.a) ? this->a < rhs.a : this->b < rhs.b;
}

bool EdgeCell::operator==(const EdgeCell& rhs) const
{
    return this->a == rhs.a && this->b == rhs.b;
}

bool EdgeCell::undefined() const
{
    return this->a >= this->b || this->b == -1 || this->a == -1;
}

TriangleCell::TriangleCell()
    : a(-1), b(-1), c(-1)
{
}

TriangleCell::TriangleCell(std::size_t a, std::size_t b, std::size_t c)
    : a(a), b(b), c(c)
{
    if(this->a > this->b)
        std::swap(this->a, this->b);
    if(this->b > this->c)
        std::swap(this->b, this->c);
    if(this->a > this->b)
        std::swap(this->a, this->b);
}

TriangleCell::TriangleCell(const TriangleCell& t) = default;

TriangleCell::~TriangleCell() = default;

bool TriangleCell::operator<(const TriangleCell& rhs) const
{
    if(this->a != rhs.a)
        return this->a < rhs.a;
    if(this->b != rhs.b)
        return this->b < rhs.b;

    return this->c < rhs.c;
}

bool TriangleCell::operator==(const TriangleCell& rhs) const
{
    return this->a == rhs.a && this->b == rhs.b && this->c == rhs.c;
}

bool TriangleCell::operator!=(const TriangleCell& rhs) const
{
    return !this->operator==(rhs);
}

std::size_t TriangleCell::oppositePoint(const EdgeCell& l) const
{
    if(this->a == l.a)
    {
        if(this->b == l.b)
            return this->c;
        if(this->c == l.b)
            return this->b;
    }
    else if(this->b == l.a && this->c == l.b)
        return this->a;

    return -1;
}

std::ostream& operator<<(std::ostream& out, const EdgeCell& l)
{
    out << "{" << l.a << "," << l.b << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const TriangleCell& t)
{
    out << "{" << t.a << "," << t.b << "," << t.c << "}";
    return out;
}

std::size_t hash_value(const EdgeCell& l)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, l.a);
    boost::hash_combine(seed, l.b);
    return seed;
}

std::size_t hash_value(const TriangleCell& t)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, t.a);
    boost::hash_combine(seed, t.b);
    boost::hash_combine(seed, t.c);
    return seed;
}

bool TriangleCell::undefined() const
{
    return this->a >= this->b || this->b >= this->c || this->c == -1 || this->b == -1 || this->a == -1;
}
