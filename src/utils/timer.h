#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <fmt/core.h>

template<class Duration>
class Timer
{
    struct EmptyCallback
    {
        void operator()() const noexcept {}
    };
public:
    template<size_t Iterations = 1, class StepCallback, class RestorerCallback = EmptyCallback>
    static void MeasureRepeated(std::string_view msg, StepCallback step, RestorerCallback restorer = EmptyCallback{})
    {
        double sum = 0.0;
        double sum2 = 0.0;
        for(size_t i = 0; i < Iterations; ++i)
        {
            auto const start_time = std::chrono::high_resolution_clock::now();
            step();
            auto const end_time = std::chrono::high_resolution_clock::now();

            auto const duration = std::chrono::duration_cast<Duration>(end_time - start_time).count();
            sum += static_cast<double>(duration);
            sum2 += static_cast<double>(duration*duration);

            if(i < Iterations - 1)
                restorer();
        }
        double const mean = Iterations==0 ? 0. : sum / Iterations;
        double const variance = Iterations < 2 ? 0. : (sum2 - mean * sum) / (Iterations - 1);
        fmt::println("{}: mean={:.1f}, std={:.1f}", msg, mean, std::sqrt(variance));
    }
};

#endif // TIMER_H
