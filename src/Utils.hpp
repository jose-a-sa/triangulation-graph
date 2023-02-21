#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <chrono>
#include <list>

template<typename T>
class ListUtil
{
public:
    using list_iterator = typename std::list<T>::iterator;
    static list_iterator circularErase(std::list<T>& lst, list_iterator b, list_iterator e)
    {
        if(b == e)
            return e;

        list_iterator b_c = b;
        while(b_c != e)
        {
            if(b_c++ == lst.end())
            {
                auto dist = std::distance(b, e) - 1;
                lst.splice(lst.end(), lst, e, b);
                return lst.erase(lst.begin(), std::next(lst.begin(), dist));
            }
        }

        return lst.erase(b, e);
    }
};

class Timer
{
public:
    Timer(bool printOnInvalidation, std::string msg)
        : printOnInvalidation_(printOnInvalidation), msg_(std::move(msg))
    {
        startTimepoint_ = std::chrono::high_resolution_clock::now();
    }
    explicit Timer(bool printOnInvalidation = true)
        : Timer(printOnInvalidation, "Timer")
    {
    }
    ~Timer()
    {
        if(printOnInvalidation_)
            std::cout << msg_ << ": " << Duration() << "us\n";
    }
    double Duration()
    {
        auto endTimepoint_ = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint_).time_since_epoch();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint_).time_since_epoch();
        std::chrono::duration<double, std::micro> duration = end - start;
        return duration.count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint_;
    bool printOnInvalidation_;
    std::string msg_;
};





#endif //UTILS_HPP
