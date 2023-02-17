#include "MeshCell.hpp"

LineCell::LineCell()
    : a(-1), b(-1)
{
}

LineCell::LineCell(std::size_t a, std::size_t b)
    : a(-1), b(-1)
{
    this->insert(a);
    this->insert(b);
}

LineCell::LineCell(const LineCell& l) = default;

LineCell::~LineCell() = default;

void LineCell::insert(std::size_t i)
{
    if(this->a == i || this->b == i)
        return;

    if(this->a == -1)
        this->a = i;
    else if(this->b == -1)
        this->b = i;

    if(this->a > this->b)
        std::swap(this->a, this->b);
}

std::size_t LineCell::size() const
{
    std::size_t sz = 0;
    if(this->a != -1)
        sz++;
    if(this->b != -1)
        sz++;
    return sz;
}

bool LineCell::operator<(const LineCell& rhs) const
{
    return (this->a != rhs.a) ? this->a < rhs.a : this->b < rhs.b;
}

bool LineCell::operator==(const LineCell& rhs) const
{
    return this->a == rhs.a && this->b == rhs.b;
}

//bool LineCell::operator!=(const LineCell& rhs) const
//{
//	return !this->operator==(rhs);
//}

TriangleCell::TriangleCell()
    : a(-1), b(-1), c(-1)
{
}

TriangleCell::TriangleCell(std::size_t a, std::size_t b, std::size_t c)
    : a(-1), b(-1), c(-1)
{
    this->insert(a);
    this->insert(b);
    this->insert(c);
}

TriangleCell::TriangleCell(const TriangleCell& t) = default;

TriangleCell::~TriangleCell() = default;

void TriangleCell::insert(std::size_t i)
{
    if(this->a == i || this->b == i || this->c == i)
        return;

    if(this->a == -1)
        this->a = i;
    else if(this->b == -1)
        this->b = i;
    else if(this->c == -1)
        this->c = i;

    if(this->a > this->b)
        std::swap(this->a, this->b);
    if(this->b > this->c)
        std::swap(this->b, this->c);
    if(this->a > this->b)
        std::swap(this->a, this->b);
}

std::size_t TriangleCell::size() const
{
    std::size_t sz = 0;
    if(this->a != -1)
        sz++;
    if(this->b != -1)
        sz++;
    if(this->c != -1)
        sz++;
    return sz;
}

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

std::size_t TriangleCell::oppositePoint(const LineCell& l) const
{
    if(l.size() < 2 || this->size() < 3)
        return -1;

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

std::ostream& operator<<(std::ostream& out, const LineCell& l)
{
    out << "{" << l.a << "," << l.b << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const TriangleCell& t)
{
    out << "{" << t.a << "," << t.b << "," << t.c << "}";
    return out;
}

std::size_t hash_value(const LineCell& l)
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
