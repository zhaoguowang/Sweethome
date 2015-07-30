#pragma once

// Timing utilities
// Yang Zhang <y@yzhang.net>, 2014
// See LICENSE file for copyright notice


#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
// #include <stdint.h>
#include <string>




class Time {

#define CPU_FREQ 2294.672 //MHZ

public:
    static void now(time_t* seconds_since_epoch, timeval* timeofday);
    static std::string format(time_t seconds_since_epoch, timeval timeofday);
    static std::string now_string();

    static uint64_t read_tsc() {
        uint32_t hi, lo;
        __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));  // NOLINT
        return (static_cast<uint64_t>(hi) << 32) | static_cast<uint64_t>(lo);
    }

    static float cycles_to_ms(uint64_t cycles) {
        return cycles/CPU_FREQ/1000;
    }
};

