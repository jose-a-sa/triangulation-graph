#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <chrono>
#include <list>

class Timer
{
public:
    Timer(bool printOnInvalidation, std::string msg)
        : m_printOnInvalidation(printOnInvalidation), m_msg(std::move(msg))
    {
        m_startTimepoint = std::chrono::high_resolution_clock::now();
    }
    explicit Timer(bool printOnInvalidation = true)
        : Timer(printOnInvalidation, "Timer")
    {
    }
    ~Timer()
    {
        if(m_printOnInvalidation)
            std::cout << m_msg << ": " << Duration() << "us\n";
    }
    double Duration()
    {
        auto endTimepoint_ = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint_).time_since_epoch();
        std::chrono::duration<double, std::micro> duration = end - start;
        return duration.count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
    bool m_printOnInvalidation;
    std::string m_msg;
};


#endif //UTILS_HPP
