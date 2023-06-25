//
// Created by jiangshanfeng on 2023/6/25.
//

#pragma once

#include <chrono>

using namespace std::chrono;

class TimerClock {
public:
    TimerClock() {
        _start = update();
    }

    ~TimerClock() {
        // double count = duration_cast<microseconds>(high_resolution_clock::now() - _start).count()
        // cout << "total cost " << count << " us" << endl;
    }

    time_point<high_resolution_clock> update() {
        return _now = high_resolution_clock::now();
    }

    long long getTimerMicroSec() {
        return duration_cast<microseconds>(high_resolution_clock::now() - _now).count();
    }

private:
    time_point<high_resolution_clock> _start;
    time_point<high_resolution_clock> _now;
};