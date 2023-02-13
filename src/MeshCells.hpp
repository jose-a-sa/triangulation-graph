#pragma once
#include <boost/assert.hpp>
#include <boost/functional/hash.hpp>
#include <set>
#include <utility>

struct LineCell
{
    std::size_t a, b;
    explicit LineCell() = default;
    explicit LineCell(std::size_t aa, std::size_t bb)
        : a(aa < bb ? aa : bb), b(aa < bb ? bb : aa) {}
    LineCell(const LineCell &l)
        : a(l.a),  b(l.b) {};
    LineCell(const std::set<std::size_t> &st)
    {
        BOOST_ASSERT_MSG(st.size() >= 2,
                         "std::set<std::size_t> initializer is not size 2, in LineCell::LineCell");
        auto it = st.begin();
        a = *(it++);
        b = *(it++);
    }
    ~LineCell() = default;
    const bool operator<(const LineCell &rhs) const
    {
        return (this->a != rhs.a) ? this->a < rhs.a : this->b < rhs.b;
    }
    const bool operator==(const LineCell &rhs) const
    {
        return this->a == rhs.a && this->b == rhs.b;
    }
    const bool operator!=(const LineCell &rhs) const
    {
        return !this->operator==(rhs);
    }
};

struct TriangleCell
{
    std::size_t a, b, c;
    explicit TriangleCell() = default;
    explicit TriangleCell(std::size_t aa, std::size_t bb, std::size_t cc)
    {
        if (aa > bb)
            std::swap(aa, bb);
        if (bb > cc)
            std::swap(bb, cc);
        if (aa > bb)
            std::swap(aa, bb);

        a = aa, b = bb, c = cc;
    }
    TriangleCell(const TriangleCell &t)
        : TriangleCell::TriangleCell(t.a, t.b, t.c) {}
    TriangleCell(const std::set<std::size_t> &st)
    {
        BOOST_ASSERT_MSG(st.size() >= 3,
                         "std::set<std::size_t> initializer is not size 2, in LineCell::LineCell");
        auto it = st.begin();
        a = *(it++);
        b = *(it++);
        c = *(it++);
    }
    ~TriangleCell() = default;
    const bool operator<(const TriangleCell &rhs) const
    {
        if (this->a != rhs.a)
            return this->a < rhs.a;
        if (this->b != rhs.b)
            return this->b < rhs.c;

        return this->c < rhs.c;
    }
    const bool operator==(const TriangleCell &rhs) const
    {
        return this->a == rhs.a && this->b == rhs.b && this->c == rhs.c;
    }
    const bool operator!=(const TriangleCell &rhs) const
    {
        return !this->operator==(rhs);
    }

    std::set<size_t> difference(const LineCell &l) const
    {
        std::set<size_t> res;
        if (this->a != l.a && this->a != l.b)
            res.insert(this->a);
        if (this->b != l.a && this->b != l.b)
            res.insert(this->b);
        if (this->c != l.a && this->c != l.b)
            res.insert(this->c);
        return res;
    }

    const bool contains(const LineCell &l) const
    {
        if (this->a == l.a)
            return this->b == l.b || this->c == l.b;
        return this->b == l.a && this->c == l.b;
    }
};

template <>
struct std::hash<LineCell>
{
    std::size_t operator()(const LineCell &l) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, l.a);
        boost::hash_combine(seed, l.b);
        return seed;
    }
};

std::ostream &operator<<(std::ostream &out, const LineCell &l)
{
    return out << "{" << l.a << "," << l.b << "}";
};

template <>
struct std::hash<TriangleCell>
{
    std::size_t operator()(const TriangleCell &t) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, t.a);
        boost::hash_combine(seed, t.b);
        boost::hash_combine(seed, t.c);
        return seed;
    }
};

std::ostream &operator<<(std::ostream &out, const TriangleCell &t)
{
    return out << "{" << t.a << "," << t.b << "," << t.c << "}";
};