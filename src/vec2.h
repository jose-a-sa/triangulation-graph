#ifndef VEC2_H_
#define VEC2_H_

#include <cmath>
#include <fmt/format.h>

// clang-format off
struct Vec2
{
    using float_type = double;
    
    float_type x;
    float_type y;

    constexpr Vec2() : x(0), y(0) {};
    constexpr Vec2(float_type x, float_type y) : x(x), y(y) {};
    constexpr Vec2(std::pair<double,double> p) : x(p.first), y(p.second) {};

    constexpr float_type dot(Vec2 const& other) const { return x * other.x + y * other.y; };
    constexpr float_type norm2() const { return x * x + y * y; };
    
    constexpr float_type distance2(Vec2 const& other) const { Vec2 const temp(x - other.x,  y - other.y); return temp.norm2(); };
    float_type distance(Vec2 const& other) const { return std::sqrt(distance2(other)); };

    constexpr float_type cross(Vec2 const& rhs) const { return x * rhs.y - y * rhs.x; };
    static constexpr float_type cross(Vec2 const& p1, Vec2 const& p2, Vec2 const& p3) { return (p2 - p1).cross(p3 - p1); };

    std::pair<Vec2, float_type> circumcircle(Vec2 const& p2, Vec2 const& p3) const
    { 
        Vec2 p = p2 - *this;
        Vec2 q = p3 - *this;
        float_type p_norm2 = p.norm2();
        float_type q_norm2 = q.norm2();
        float_type den = 2.0 * p.cross(q);

        Vec2 cc((q.y * p_norm2 - p.y * q_norm2) / den, (p.x * q_norm2 - q.x * p_norm2) / den);
        return std::make_pair(*this + cc, std::sqrt(cc.norm2()));
    } ;

    constexpr bool operator<(Vec2 const& rhs) const { return x != rhs.x ? x < rhs.x : y < rhs.y; };
    constexpr bool operator==(Vec2 const& rhs) const { return x == rhs.x && y == rhs.y; };

    constexpr Vec2 operator+(Vec2 const& rhs) const { return Vec2(x + rhs.x, y + rhs.y); };
    constexpr Vec2 operator-(Vec2 const& rhs) const { return Vec2(x - rhs.x, y - rhs.y); };
    constexpr Vec2& operator+=(Vec2 const& rhs) { x += rhs.x; y += rhs.y; return *this; };
    constexpr Vec2& operator-=(Vec2 const& rhs) { x -= rhs.x; y -= rhs.y; return *this; };
};
// clang-format on

template<>
struct fmt::formatter<Vec2>
{
    constexpr const char* parse(format_parse_context& ctx) const { return ctx.begin(); }
    template<typename FormatContext>
    typename FormatContext::iterator format(Vec2 const& v, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({},{})", v.x, v.y);
    }
};


#include <random>
#include <vector>

class Vec2Factory
{
public:
    using float_type = Vec2::float_type;

    Vec2Factory()
        : eng_{}
    {
        init();
    };

    void init(std::uint64_t seed = std::random_device{}()) { eng_.seed(seed); }

    std::vector<Vec2> random_sample(size_t n = 1, float_type a = 0.0, float_type b = 1.0)
    {
        std::uniform_real_distribution<float_type> dist(a, b);

        std::vector<Vec2> res;
        res.reserve(n);
        for(size_t i = 0; i < n; i++)
            res.emplace_back(dist(eng_), dist(eng_));

        return res;
    }

    std::vector<Vec2> make_simplex(float_type a = 1.0) { return {{0, 0}, {a, 0}, {0, a}}; }

private:
    std::mt19937_64 eng_;
};


#endif // VEC2_H_
