#pragma once
#include <Windows.h>

inline double perfFrequency()
{
    static double frequency = [] {
        LARGE_INTEGER query; QueryPerformanceFrequency(&query);
        return (double)query.QuadPart;
        }();
    return frequency;
}

struct ScopedTimer
{
    LARGE_INTEGER startTicks;
    double& accumulatorMs;

    ScopedTimer(double& accumulator) : accumulatorMs(accumulator)
    {
        QueryPerformanceCounter(&startTicks);
    }
    ~ScopedTimer()
    {
        LARGE_INTEGER endTicks;
        QueryPerformanceCounter(&endTicks);
        accumulatorMs += (double)(endTicks.QuadPart - startTicks.QuadPart) * 1000.0 / perfFrequency();
    }
};