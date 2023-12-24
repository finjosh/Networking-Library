#ifndef UpdateLimiter_H
#define UpdateLimiter_H

#pragma once

#include <thread>
#include <chrono>

class UpdateLimiter
{
public:
    // @param MaxUpdates max updates per second
    UpdateLimiter(std::intmax_t MaxUpdates) : // initialize the object keeping the pace
        MaxUpdates(MaxUpdates),
        time_between_frames{(double)1/(double)MaxUpdates},
        tp{std::chrono::steady_clock::now()}
    {}

    // @param MaxUpdates max updates per second
    void updateLimit(std::intmax_t MaxUpdates)
    {
        this->MaxUpdates = MaxUpdates;
        time_between_frames = std::chrono::duration<double>((double)1/(double)MaxUpdates);
    }

    std::intmax_t getUpdateLimit()
    {
        return this->MaxUpdates;
    }

    void wait() {
        // add to time point
        tp += time_between_frames;

        // and sleep until that time point
        std::this_thread::sleep_until(tp);
    }

private:
    std::intmax_t MaxUpdates = 1;

    // a duration with a length of 1/MaxFPS seconds
    std::chrono::duration<double> time_between_frames;

    // the time point added to in every loop
    std::chrono::time_point<std::chrono::steady_clock, decltype(time_between_frames)> tp;
};

#endif
