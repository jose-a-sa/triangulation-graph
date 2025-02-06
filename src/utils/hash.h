#ifndef HASH_H
#define HASH_H

#include <cstdint>
#include <utility>

struct HashUtils
{
    static constexpr uint64_t combine(uint64_t seed, uint64_t hash)
    {
        return seed ^ ( hash + 0x9e3779b9 + (seed<<6) + (seed>>2) );
    }

    template<class Iter, class HashFunction>
    static constexpr uint64_t combine_unordered_range(Iter first, Iter last, HashFunction hasher)
    {
        uint64_t r = 0;
        for( ; first != last; ++first )
            r += mixin(0x9e3779b9 + hasher(*first));
        return r;
    }

    template<class T1, class T2, class HashFunction>
    static constexpr uint64_t combine_unordered_pair(std::pair<T1, T2> const& p, HashFunction hasher)
    {
        return mixin(0x9e3779b9 + hasher(p.first)) ^ mixin(0x9e3779b9 + hasher(p.second));
    }
    
private:
    static constexpr uint64_t m = 0xe9846af9b1a615d;
    static constexpr uint64_t mixin(uint64_t x)
    {
        x ^= x >> 32;
        x *= m;
        x ^= x >> 32;
        x *= m;
        x ^= x >> 28;
        return x;
    }
};

#endif // HASH_H