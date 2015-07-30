// Timing utilities
// Yang Zhang <y@yzhang.net>, 2014
// See LICENSE file for copyright notice

#include "date_time.h"

#include <time.h>
#include <sys/time.h>



static void make_int(char* str, int val, int digits) {
    char* p = str + digits;
    for (int i = 0; i < digits; i++) {
        int d = val % 10;
        val /= 10;
        p--;
        *p = '0' + d;
    }
}



void Time::now(time_t* seconds_since_epoch, timeval* timeofday) {
    if (seconds_since_epoch != nullptr) {
        *seconds_since_epoch = time(nullptr);
    }
    if (timeofday != nullptr) {
        gettimeofday(timeofday, nullptr);
    }
}

// inspired by the TPC-C benchmark from Evan Jones
// strftime is slow because it ends up consulting timezone info
// also, snprintf is slow
std::string Time::format(time_t seconds_since_epoch, timeval timeofday) {
    // YYYY-MM-DD HH:MM:SS.uuuuuu, 27 bytes
    const int now_cstr_size = 27;
    char now_cstr[now_cstr_size];
    struct tm local_calendar;
    localtime_r(&seconds_since_epoch, &local_calendar);
    make_int(now_cstr, local_calendar.tm_year + 1900, 4);
    now_cstr[4] = '-';
    make_int(now_cstr + 5, local_calendar.tm_mon + 1, 2);
    now_cstr[7] = '-';
    make_int(now_cstr + 8, local_calendar.tm_mday, 2);
    now_cstr[10] = ' ';
    make_int(now_cstr + 11, local_calendar.tm_hour, 2);
    now_cstr[13] = ':';
    make_int(now_cstr + 14, local_calendar.tm_min, 2);
    now_cstr[16] = ':';
    make_int(now_cstr + 17, local_calendar.tm_sec, 2);
    now_cstr[19] = '.';
    make_int(now_cstr + 20, timeofday.tv_usec, 6);
    now_cstr[26] = '\0';
    return std::string(now_cstr);
}

std::string Time::now_string() {
    time_t seconds_since_epoch;
    timeval tv;
    Time::now(&seconds_since_epoch, &tv);
    return Time::format(seconds_since_epoch, tv);
}

