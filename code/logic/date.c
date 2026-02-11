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
#include <strings.h>
#include <stdint.h>
#include <stdlib.h>
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
 * Internal: civil date to epoch seconds (UTC)
 * No libc time, no TZ, fully deterministic
 * ====================================================== */

static int64_t fossil_time_days_from_civil(int64_t y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int64_t era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = (unsigned)(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (int64_t)doe - 719468;
}

static int64_t fossil_time_tm_to_epoch_utc(const struct tm *tm) {
    int64_t days = fossil_time_days_from_civil(
        tm->tm_year + 1900,
        (unsigned)(tm->tm_mon + 1),
        (unsigned)tm->tm_mday
    );

    return days * 86400
         + tm->tm_hour * 3600
         + tm->tm_min * 60
         + tm->tm_sec;
}

/* ======================================================
 * Internal: portable UTC timegm replacement
 * ====================================================== */

static time_t fossil_time_internal_timegm(struct tm *tm) {

#if defined(_WIN32)

    /* Windows provides _mkgmtime */
    return _mkgmtime(tm);

#else

    /* POSIX fallback: temporarily force UTC */
    char *old_tz = getenv("TZ");
    time_t result;

    setenv("TZ", "UTC", 1);
    tzset();

    result = mktime(tm);

    if (old_tz)
        setenv("TZ", old_tz, 1);
    else
        unsetenv("TZ");

    tzset();
    return result;

#endif
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

    time_t t = fossil_time_internal_timegm(&tm);
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

    int64_t seconds = fossil_time_tm_to_epoch_utc(&tm);
    return seconds - (dt->tz_offset_min * 60);
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

// search logic

static int fossil_time_date_get_field(
    const fossil_time_date_t *dt,
    const char *field,
    int *out
) {
    if (!strcmp(field, "year") || !strcmp(field, "y")) {
        *out = dt->year; return 1;
    }
    if (!strcmp(field, "month") || !strcmp(field, "mon") || !strcmp(field, "m")) {
        *out = dt->month; return 1;
    }
    if (!strcmp(field, "day") || !strcmp(field, "d")) {
        *out = dt->day; return 1;
    }
    if (!strcmp(field, "hour") || !strcmp(field, "h")) {
        *out = dt->hour; return 1;
    }
    if (!strcmp(field, "minute") || !strcmp(field, "min")) {
        *out = dt->minute; return 1;
    }
    if (!strcmp(field, "second") || !strcmp(field, "sec") || !strcmp(field, "s")) {
        *out = dt->second; return 1;
    }
    if (!strcmp(field, "weekday") || !strcmp(field, "wday") || !strcmp(field, "dow")) {
        *out = dt->weekday; return 1;
    }
    if (!strcmp(field, "yearday") || !strcmp(field, "yday")) {
        *out = dt->yearday; return 1;
    }
    if (!strcmp(field, "millisecond") || !strcmp(field, "ms")) {
        *out = dt->millisecond; return 1;
    }
    if (!strcmp(field, "microsecond") || !strcmp(field, "us")) {
        *out = dt->microsecond; return 1;
    }
    if (!strcmp(field, "nanosecond") || !strcmp(field, "ns")) {
        *out = dt->nanosecond; return 1;
    }
    if (!strcmp(field, "tz_offset") || !strcmp(field, "tz") || !strcmp(field, "offset")) {
        *out = dt->tz_offset_min; return 1;
    }
    return 0;
}

static int fossil_cmp(int lhs, const char *op, int rhs) {
    if (!strcmp(op, "=")  || !strcmp(op, "==") || !strcmp(op, "is") || !strcmp(op, "equals"))
        return lhs == rhs;

    if (!strcmp(op, "!=") || !strcmp(op, "is not") || !strcmp(op, "<>") || !strcmp(op, "not equals"))
        return lhs != rhs;

    if (!strcmp(op, "<")  || !strcmp(op, "before") || !strcmp(op, "lt") || !strcmp(op, "less"))
        return lhs < rhs;

    if (!strcmp(op, ">")  || !strcmp(op, "after") || !strcmp(op, "gt") || !strcmp(op, "greater"))
        return lhs > rhs;

    if (!strcmp(op, "<=") || !strcmp(op, "on or before") || !strcmp(op, "le") || !strcmp(op, "lte") || !strcmp(op, "less or equal"))
        return lhs <= rhs;

    if (!strcmp(op, ">=") || !strcmp(op, "on or after") || !strcmp(op, "ge") || !strcmp(op, "gte") || !strcmp(op, "greater or equal"))
        return lhs >= rhs;

    return 0;
}

int fossil_time_date_search(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    const char *query
) {
    if (!dt || !query) {
        return 0;
    }

    /* ---- simple keywords ---- */

    if (now && (!strcmp(query, "today") || !strcmp(query, "this day"))) {
        return dt->year  == now->year &&
               dt->month == now->month &&
               dt->day   == now->day;
    }

    if (!strcmp(query, "weekend") || !strcmp(query, "is weekend")) {
        return dt->weekday == 0 || dt->weekday == 6;
    }

    if (!strcmp(query, "weekday") || !strcmp(query, "is weekday")) {
        return dt->weekday >= 1 && dt->weekday <= 5;
    }

    if (now && (!strcmp(query, "past") || !strcmp(query, "in the past"))) {
        return fossil_time_date_compare(dt, now) < 0;
    }

    if (now && (!strcmp(query, "future") || !strcmp(query, "in the future"))) {
        return fossil_time_date_compare(dt, now) > 0;
    }

    if (!strcmp(query, "leap year")) {
        return is_leap(dt->year);
    }

    if (!strcmp(query, "first of month")) {
        return dt->day == 1;
    }

    if (!strcmp(query, "last of month")) {
        return dt->day == days_in_month(dt->year, dt->month);
    }

    /* ---- expression form: field op value ---- */

    char field[16];
    char op[16];
    int value;

    if (sscanf(query, "%15s %15s %d", field, op, &value) == 3) {
        int lhs;
        if (fossil_time_date_get_field(dt, field, &lhs)) {
            return fossil_cmp(lhs, op, value);
        }
    }

    /* ---- relative expressions ---- */

    if (now && (!strcmp(query, "before today") || !strcmp(query, "before now"))) {
        return fossil_time_date_compare(dt, now) < 0;
    }

    if (now && (!strcmp(query, "after today") || !strcmp(query, "after now"))) {
        return fossil_time_date_compare(dt, now) > 0;
    }

    /* ---- range expressions: e.g. year in 2020..2025 ---- */
    {
        int end;
        if (sscanf(query, "%15s in %d..%d", field, &value, &end) == 3) {
            int lhs;
            if (fossil_time_date_get_field(dt, field, &lhs)) {
                return lhs >= value && lhs <= end;
            }
        }
    }

    /* ---- day of week by name ---- */
    static const char *weekdays[] = {
        "sunday", "monday", "tuesday", "wednesday",
        "thursday", "friday", "saturday"
    };
    for (int i = 0; i < 7; ++i) {
        if (!strcasecmp(query, weekdays[i])) {
            return dt->weekday == i;
        }
    }

    return 0;
}
