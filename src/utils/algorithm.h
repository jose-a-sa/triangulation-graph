#ifndef CONVEX_TRIANGULATIONS_ALGOS_HPP
#define CONVEX_TRIANGULATIONS_ALGOS_HPP

#include <cmath>

template<class Iterator, class Sentinel, class Projector>
constexpr Iterator min_element_by(Iterator first, Sentinel last, Projector proj)
{
    if(first == last)
        return last;

    Iterator smallest  = first;
    auto     min_value = proj(*first);
    while(std::isnan(min_value) && smallest != last)
    {
        ++smallest;
        min_value = proj(*smallest);
    }
    if(smallest == last)
        return last;

    first = smallest;
    while(++first != last)
    {
        auto const value = proj(*first);
        if(std::isnan(value))
            continue;

        if(value < min_value)
        {
            min_value = value;
            smallest  = first;
        }
    }

    return smallest;
}

template<class Iterator, class Sentinel, class Projector>
constexpr Iterator max_element_by(Iterator first, Sentinel last, Projector proj)
{
    if(first == last)
        return last;

    Iterator smallest  = first;
    auto     max_value = proj(*first);
    while(std::isnan(max_value) && smallest != last)
    {
        ++smallest;
        max_value = proj(*smallest);
    }
    if(smallest == last)
        return last;

    first = smallest;
    while(++first != last)
    {
        auto const value = proj(*first);
        if(std::isnan(value))
            continue;

        if(max_value < value)
        {
            max_value = value;
            smallest  = first;
        }
    }

    return smallest;
}

#endif // CONVEX_TRIANGULATIONS_ALGOS_HPP
