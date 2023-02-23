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
    double Duration()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_startTimepoint).time_since_epoch();
        auto end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimepoint).time_since_epoch();
        std::chrono::duration<double, std::micro> duration = end - start;
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
        m_samples = std::array<double, Samples>();
    }
    explicit TimerAverage(bool printOnInvalidation = true)
        : m_printOnInvalidation(printOnInvalidation), m_msg("Timer")
    {
        m_samples = std::array<double, Samples>();
    }
    ~TimerAverage()
    {

        if(m_printOnInvalidation)
        {
            std::size_t sz = std::min(m_currentIdx, Samples);
            double acc = 0;
            for(std::size_t i = 0; i < sz; i++)
                acc += m_samples[i];
            std::cout << m_msg << ": " << acc / sz << "us\n";
        }
    }
    void StartMeasure()
    {
        m_startTimepoint = std::chrono::high_resolution_clock::now();
    }
    double StopMeasure()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_startTimepoint).time_since_epoch();
        auto end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimepoint).time_since_epoch();
        std::chrono::duration<double, std::micro> duration = end - start;
        double res = duration.count();
        if(m_currentIdx != Samples)
            m_samples[m_currentIdx++] = res;
        return res;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
    bool m_printOnInvalidation;
    std::string m_msg;
    std::size_t m_currentIdx = 0;
    std::array<double, Samples> m_samples;
};


#endif //UTILS_HPP
