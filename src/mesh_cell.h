#ifndef CONVEX_TRIANGULATIONS_MESHCELLS_HPP
#define CONVEX_TRIANGULATIONS_MESHCELLS_HPP

#include <cstddef>
#include <fmt/format.h>

class EdgeCell
{
public:
    size_t a{};
    size_t b{};

    explicit constexpr EdgeCell(size_t i = -1ull, size_t j = -1ull)
        : a(std::min(i, j)),
          b(std::max(i, j))
    {}

    constexpr bool undefined() const { return b == -1ull || a == -1ull; };

    constexpr bool operator<(const EdgeCell& rhs) const { return (a != b) ? a < rhs.a : b < rhs.b; }
    constexpr bool operator==(const EdgeCell& rhs) const { return a == rhs.a && b == rhs.b; }
};

template<>
struct fmt::formatter<EdgeCell>
{
    constexpr const char* parse(format_parse_context& ctx) const { return ctx.begin(); }
    template<typename FormatContext>
    typename FormatContext::iterator format(EdgeCell const& cell, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), (cell.b == -1ull) ? "({},_)" : "({},{})", cell.a, cell.b);
    }
};


class TriangleCell
{
public:
    size_t a;
    size_t b;
    size_t c;

    explicit constexpr TriangleCell(size_t i = -1ul, size_t j = -1ul, size_t k = -1ul)
        : a(i),
          b(j),
          c(k)
    {
        if(a > b)
            std::swap(a, b);
        if(b > c)
            std::swap(b, c);
        if(a > b)
            std::swap(a, b);
    }

    constexpr bool undefined() const { return c == -1ull || b == -1ull || a == -1ull; };

    constexpr size_t opposite_point(EdgeCell const& l) const
    {
        if(l.undefined())
            return -1ull;

        if(a == l.a)
        {
            if(b == l.b)
                return c;
            if(c == l.b)
                return b;
        }
        else if(b == l.a && c == l.b)
            return a;

        return -1ull;
    };

    constexpr bool operator<(const TriangleCell& rhs) const
    {
        return a != rhs.a ? a < rhs.a : (b != rhs.b ? b < rhs.b : c < rhs.c);
    };
    constexpr bool operator==(const TriangleCell& rhs) const { return a == rhs.a && b == rhs.b && c == rhs.c; };
};

template<>
struct fmt::formatter<TriangleCell>
{
    constexpr const char* parse(format_parse_context& ctx) const { return ctx.begin(); }
    template<typename FormatContext>
    typename FormatContext::iterator format(TriangleCell const& cell, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({},{},{})", cell.a, cell.b, cell.c);
    }
};


#endif // CONVEX_TRIANGULATIONS_MESHCELLS_HPP
