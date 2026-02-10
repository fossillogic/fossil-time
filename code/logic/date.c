/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/time/date.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

/* ======================================================
 * Helpers
 * ====================================================== */

static int is_leap(int32_t year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

static int days_in_month(int32_t year, int8_t month) {
    static const int days[] = {
        31,28,31,30,31,30,31,31,30,31,30,31
    };
    if (month == 2 && is_leap(year)) return 29;
    if (month < 1 || month > 12) return 0;
    return days[month - 1];
}

/* ======================================================
 * Core API
 * ====================================================== */

void fossil_time_date_now(fossil_time_date_t *dt) {
    struct timespec ts;
    struct tm tm;

    memset(dt, 0, sizeof(*dt));

#if defined(_WIN32)
    timespec_get(&ts, TIME_UTC);
    gmtime_s(&tm, &ts.tv_sec);
#else
    clock_gettime(CLOCK_REALTIME, &ts);
    gmtime_r(&ts.tv_sec, &tm);
#endif

    dt->year   = tm.tm_year + 1900;
    dt->month  = tm.tm_mon + 1;
    dt->day    = tm.tm_mday;
    dt->hour   = tm.tm_hour;
    dt->minute = tm.tm_min;
    dt->second = tm.tm_sec;

    dt->millisecond = ts.tv_nsec / 1000000;
    dt->microsecond = (ts.tv_nsec / 1000) % 1000;
    dt->nanosecond  = ts.tv_nsec % 1000;

    dt->weekday = tm.tm_wday;
    dt->yearday = tm.tm_yday + 1;
    dt->tz_offset_min = 0;

    dt->precision_mask =
        FOSSIL_TIME_PRECISION_YEAR   |
        FOSSIL_TIME_PRECISION_MONTH  |
        FOSSIL_TIME_PRECISION_DAY    |
        FOSSIL_TIME_PRECISION_HOUR   |
        FOSSIL_TIME_PRECISION_MINUTE |
        FOSSIL_TIME_PRECISION_SECOND |
        FOSSIL_TIME_PRECISION_MILLI  |
        FOSSIL_TIME_PRECISION_MICRO  |
        FOSSIL_TIME_PRECISION_NANO;
}

int fossil_time_date_validate(const fossil_time_date_t *dt) {
    if ((dt->precision_mask & FOSSIL_TIME_PRECISION_MONTH) &&
        (dt->month < 1 || dt->month > 12))
        return 0;

    if ((dt->precision_mask & FOSSIL_TIME_PRECISION_DAY) &&
        (dt->day < 1 || dt->day > days_in_month(dt->year, dt->month)))
        return 0;

    if ((dt->precision_mask & FOSSIL_TIME_PRECISION_HOUR) &&
        (dt->hour < 0 || dt->hour > 23))
        return 0;

    if ((dt->precision_mask & FOSSIL_TIME_PRECISION_MINUTE) &&
        (dt->minute < 0 || dt->minute > 59))
        return 0;

    if ((dt->precision_mask & FOSSIL_TIME_PRECISION_SECOND) &&
        (dt->second < 0 || dt->second > 60))
        return 0;

    return 1;
}

void fossil_time_date_normalize(fossil_time_date_t *dt) {
    /* Conservative normalize: recompute derived only */
    struct tm tm = {0};

    tm.tm_year = dt->year - 1900;
    tm.tm_mon  = dt->month - 1;
    tm.tm_mday = dt->day;
    tm.tm_hour = dt->hour;
    tm.tm_min  = dt->minute;
    tm.tm_sec  = dt->second;

    time_t t = timegm(&tm);
    gmtime_r(&t, &tm);

    dt->weekday = tm.tm_wday;
    dt->yearday = tm.tm_yday + 1;
}

int fossil_time_date_compare(
    const fossil_time_date_t *a,
    const fossil_time_date_t *b
) {
#define CMP(field) if (a->field != b->field) return (a->field < b->field ? -1 : 1)

    CMP(year);
    CMP(month);
    CMP(day);
    CMP(hour);
    CMP(minute);
    CMP(second);
    CMP(millisecond);
    CMP(microsecond);
    CMP(nanosecond);

#undef CMP
    return 0;
}

int64_t fossil_time_date_diff_seconds(
    const fossil_time_date_t *a,
    const fossil_time_date_t *b
) {
    return fossil_time_date_to_unix_seconds(a) -
           fossil_time_date_to_unix_seconds(b);
}

int64_t fossil_time_date_to_unix_seconds(
    const fossil_time_date_t *dt
) {
    struct tm tm = {0};

    tm.tm_year = dt->year - 1900;
    tm.tm_mon  = dt->month - 1;
    tm.tm_mday = dt->day;
    tm.tm_hour = dt->hour;
    tm.tm_min  = dt->minute;
    tm.tm_sec  = dt->second;

    return (int64_t)timegm(&tm) - (dt->tz_offset_min * 60);
}

int64_t fossil_time_date_to_unix_nanoseconds(
    const fossil_time_date_t *dt
) {
    int64_t sec = fossil_time_date_to_unix_seconds(dt);
    int64_t ns  = 0;

    if (dt->precision_mask & FOSSIL_TIME_PRECISION_MILLI)
        ns += dt->millisecond * 1000000LL;
    if (dt->precision_mask & FOSSIL_TIME_PRECISION_MICRO)
        ns += dt->microsecond * 1000LL;
    if (dt->precision_mask & FOSSIL_TIME_PRECISION_NANO)
        ns += dt->nanosecond;

    return sec * 1000000000LL + ns;
}

void fossil_time_date_from_unix_seconds(
    int64_t seconds,
    fossil_time_date_t *dt
) {
    struct tm tm;
    time_t t = (time_t)seconds;

    memset(dt, 0, sizeof(*dt));
    gmtime_r(&t, &tm);

    dt->year   = tm.tm_year + 1900;
    dt->month  = tm.tm_mon + 1;
    dt->day    = tm.tm_mday;
    dt->hour   = tm.tm_hour;
    dt->minute = tm.tm_min;
    dt->second = tm.tm_sec;
    dt->weekday = tm.tm_wday;
    dt->yearday = tm.tm_yday + 1;
    dt->tz_offset_min = 0;

    dt->precision_mask =
        FOSSIL_TIME_PRECISION_YEAR |
        FOSSIL_TIME_PRECISION_MONTH |
        FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR |
        FOSSIL_TIME_PRECISION_MINUTE |
        FOSSIL_TIME_PRECISION_SECOND;
}

int fossil_time_date_format(
    const fossil_time_date_t *dt,
    char *buffer,
    size_t buffer_size,
    const char *format_id
) {
    if (!strcmp(format_id, "iso")) {
        return snprintf(
            buffer, buffer_size,
            "%04d-%02d-%02dT%02d:%02d:%02dZ",
            dt->year, dt->month, dt->day,
            dt->hour, dt->minute, dt->second
        );
    }

    if (!strcmp(format_id, "log")) {
        return snprintf(
            buffer, buffer_size,
            "%04d%02d%02d-%02d%02d%02d",
            dt->year, dt->month, dt->day,
            dt->hour, dt->minute, dt->second
        );
    }

    /* Fallback */
    return snprintf(buffer, buffer_size, "invalid_date");
}

int fossil_time_date_format_smart(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    char *buffer,
    size_t buffer_size
) {
    int64_t diff = fossil_time_date_diff_seconds(dt, now);

    if (diff == 0)
        return snprintf(buffer, buffer_size, "now");

    if (diff > 0)
        return snprintf(buffer, buffer_size, "in %lld seconds", (long long)diff);

    return snprintf(buffer, buffer_size, "%lld seconds ago", (long long)-diff);
}

int fossil_time_date_format_relative(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    char *buffer,
    size_t buffer_size
) {
    return fossil_time_date_format_smart(dt, now, buffer, buffer_size);
}

int fossil_time_date_search(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    const char *query
) {
    if (!strcmp(query, "today") && now) {
        return dt->year  == now->year &&
               dt->month == now->month &&
               dt->day   == now->day;
    }

    if (!strcmp(query, "weekend")) {
        return (dt->weekday == 0 || dt->weekday == 6);
    }

    return 0;
}
