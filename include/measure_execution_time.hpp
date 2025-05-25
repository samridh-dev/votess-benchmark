#ifndef MEASURE_EXECUTION_TIME_HPP
#define MEASURE_EXECUTION_TIME_HPP

#include <chrono>
template <typename func>
double 
measure_execution_time(func f) 
{
        auto beg = std::chrono::high_resolution_clock::now();
        f();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - beg).count();
}

#endif // MEASURE_EXECUTION_TIME_HPP
