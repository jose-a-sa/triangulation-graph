#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <chrono>
#include <array>

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
    std::double_t Duration()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_startTimepoint).time_since_epoch();
        auto end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimepoint).time_since_epoch();
        std::chrono::duration<std::double_t, std::micro> duration = end - start;
        return duration.count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
    bool m_printOnInvalidation;
    std::string m_msg;
};

template<std::size_t Samples>
class TimerAverage
{
public:
    TimerAverage(bool printOnInvalidation, std::string msg)
        : m_printOnInvalidation(printOnInvalidation), m_msg(std::move(msg))
    {
        m_samples = std::array<std::double_t, Samples>();
    }
    explicit TimerAverage(bool printOnInvalidation = true)
        : m_printOnInvalidation(printOnInvalidation), m_msg("Timer")
    {
        m_samples = std::array<std::double_t, Samples>();
    }
    ~TimerAverage()
    {
        if(m_printOnInvalidation)
            std::cout << m_msg << ": " << Duration() << "us\n";
    }
    void StartMeasure()
    {
        m_startTimepoint = std::chrono::high_resolution_clock::now();
    }
    std::double_t StopMeasure()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_startTimepoint).time_since_epoch();
        auto end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimepoint).time_since_epoch();
        std::chrono::duration<std::double_t, std::micro> duration = end - start;
        std::double_t res = duration.count();
        if(m_currentIdx != Samples)
            m_samples[m_currentIdx++] = res;
        return res;
    }
    std::double_t Duration()
    {
        std::size_t sz = std::min(m_currentIdx, Samples);
        std::double_t acc = 0;
        for(std::size_t i = 0; i < sz; i++)
            acc += m_samples[i];
        return acc / static_cast<std::double_t>(sz);
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
    bool m_printOnInvalidation;
    std::string m_msg;
    std::size_t m_currentIdx = 0;
    std::array<std::double_t, Samples> m_samples;
};


#endif //UTILS_HPP
