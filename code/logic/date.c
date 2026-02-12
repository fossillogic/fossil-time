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
#include <ctype.h>
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

// format logic

int fossil_time_date_format(
    const fossil_time_date_t *dt,
    char *buffer,
    size_t buffer_size,
    const char *format_id
) {
    if (!dt || !buffer || buffer_size == 0) return 0;

    size_t n = 0;

    /* Date part */
    if (dt->precision_mask & FOSSIL_TIME_PRECISION_YEAR)
        n += snprintf(buffer + n, buffer_size - n, "%04d", dt->year);
    if (dt->precision_mask & FOSSIL_TIME_PRECISION_MONTH)
        n += snprintf(buffer + n, buffer_size - n, "-%02d", dt->month);
    if (dt->precision_mask & FOSSIL_TIME_PRECISION_DAY)
        n += snprintf(buffer + n, buffer_size - n, "-%02d", dt->day);

    /* Time part */
    if (dt->precision_mask & (FOSSIL_TIME_PRECISION_HOUR |
                              FOSSIL_TIME_PRECISION_MINUTE |
                              FOSSIL_TIME_PRECISION_SECOND)) {
        int h = (dt->precision_mask & FOSSIL_TIME_PRECISION_HOUR) ? dt->hour : 0;
        int m = (dt->precision_mask & FOSSIL_TIME_PRECISION_MINUTE) ? dt->minute : 0;
        int s = (dt->precision_mask & FOSSIL_TIME_PRECISION_SECOND) ? dt->second : 0;
        n += snprintf(buffer + n, buffer_size - n, "T%02d:%02d:%02d", h, m, s);

        /* Sub-seconds */
        char subsec[32];
        int pos = 0;
        if (dt->precision_mask & FOSSIL_TIME_PRECISION_MILLI)
            pos += snprintf(subsec + pos, sizeof(subsec) - pos, "%03d", dt->millisecond);
        if (dt->precision_mask & FOSSIL_TIME_PRECISION_MICRO)
            pos += snprintf(subsec + pos, sizeof(subsec) - pos, "%03d", dt->microsecond);
        if (dt->precision_mask & FOSSIL_TIME_PRECISION_NANO)
            pos += snprintf(subsec + pos, sizeof(subsec) - pos, "%03d", dt->nanosecond);
        if (dt->precision_mask & FOSSIL_TIME_PRECISION_PICO)
            pos += snprintf(subsec + pos, sizeof(subsec) - pos, "%03d", dt->picosecond);
        if (dt->precision_mask & FOSSIL_TIME_PRECISION_FEMTO)
            pos += snprintf(subsec + pos, sizeof(subsec) - pos, "%03d", dt->femtosecond);
        if (dt->precision_mask & FOSSIL_TIME_PRECISION_ATTO)
            pos += snprintf(subsec + pos, sizeof(subsec) - pos, "%03d", dt->attosecond);
        if (dt->precision_mask & FOSSIL_TIME_PRECISION_ZEPTO)
            pos += snprintf(subsec + pos, sizeof(subsec) - pos, "%03d", dt->zeptosecond);
        if (dt->precision_mask & FOSSIL_TIME_PRECISION_YOCTO)
            pos += snprintf(subsec + pos, sizeof(subsec) - pos, "%03d", dt->yoctosecond);

        /* Trim trailing zeros */
        while (pos > 0 && subsec[pos - 1] == '0') pos--;
        if (pos > 0) n += snprintf(buffer + n, buffer_size - n, ".%.*s", pos, subsec);

        /* UTC marker */
        n += snprintf(buffer + n, buffer_size - n, "Z");
    }

    /* Log format fallback */
    if (!strcmp(format_id, "log") && n == 0) {
        n = snprintf(buffer, buffer_size, "%04d%02d%02d-%02d%02d%02d",
                     dt->year, dt->month, dt->day,
                     dt->hour, dt->minute, dt->second);
    }

    return (int)n;
}

int fossil_time_date_format_smart(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    char *buffer,
    size_t buffer_size
) {
    if (!dt || !now || !buffer || buffer_size == 0)
        return 0;

    int64_t diff = fossil_time_date_diff_seconds(dt, now);

    if (diff == 0)
        return snprintf(buffer, buffer_size, "now");

    int future = diff > 0;
    int64_t abs_diff = future ? diff : -diff;

    const char *unit = "seconds";
    int64_t value = abs_diff;

    if (abs_diff >= 31536000) {          /* years */
        unit = "years";
        value = abs_diff / 31536000;
    }
    else if (abs_diff >= 2592000) {      /* months (30d approx) */
        unit = "months";
        value = abs_diff / 2592000;
    }
    else if (abs_diff >= 604800) {       /* weeks */
        unit = "weeks";
        value = abs_diff / 604800;
    }
    else if (abs_diff >= 86400) {        /* days */
        unit = "days";
        value = abs_diff / 86400;
    }
    else if (abs_diff >= 3600) {         /* hours */
        unit = "hours";
        value = abs_diff / 3600;
    }
    else if (abs_diff >= 60) {           /* minutes */
        unit = "minutes";
        value = abs_diff / 60;
    }

    /* Singular correction */
    if (value == 1) {
        if (!strcmp(unit, "seconds")) unit = "second";
        else if (!strcmp(unit, "minutes")) unit = "minute";
        else if (!strcmp(unit, "hours")) unit = "hour";
        else if (!strcmp(unit, "days")) unit = "day";
        else if (!strcmp(unit, "weeks")) unit = "week";
        else if (!strcmp(unit, "months")) unit = "month";
        else if (!strcmp(unit, "years")) unit = "year";
    }

    if (future)
        return snprintf(buffer, buffer_size,
                        "in %lld %s",
                        (long long)value, unit);

    return snprintf(buffer, buffer_size,
                    "%lld %s ago",
                    (long long)value, unit);
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

static int fossil_match_char_class(char c, const char *pattern, int *consumed) {
    int i = 1; /* skip '[' */
    int match = 0;

    while (pattern[i] && pattern[i] != ']') {
        if (pattern[i+1] == '-' && pattern[i+2] && pattern[i+2] != ']') {
            char start = pattern[i];
            char end   = pattern[i+2];
            if (c >= start && c <= end)
                match = 1;
            i += 3;
        } else {
            if (c == pattern[i])
                match = 1;
            i++;
        }
    }

    if (pattern[i] == ']')
        i++;

    *consumed = i;
    return match;
}

static int fossil_pattern_match(const char *str, const char *pattern) {
    if (!pattern || !str)
        return 0;

    while (*pattern) {
        if (*pattern == '*') {
            pattern++;
            if (!*pattern)
                return 1;

            while (*str) {
                if (fossil_pattern_match(str, pattern))
                    return 1;
                str++;
            }
            return 0;
        }
        else if (*pattern == '?') {
            if (!*str)
                return 0;
            str++;
            pattern++;
        }
        else if (*pattern == '[') {
            int consumed = 0;
            if (!*str)
                return 0;

            if (!fossil_match_char_class(*str, pattern, &consumed))
                return 0;

            str++;
            pattern += consumed;
        }
        else {
            if (tolower((unsigned char)*str) !=
                tolower((unsigned char)*pattern))
                return 0;

            str++;
            pattern++;
        }
    }

    return *str == '\0';
}

static int fossil_parse_int(const char *s, int *out) {
    if (!s || !*s)
        return 0;

    int neg = 0;
    if (*s == '-') {
        neg = 1;
        s++;
    }

    if (!isdigit((unsigned char)*s))
        return 0;

    int value = 0;
    while (*s) {
        if (!isdigit((unsigned char)*s))
            return 0;
        value = value * 10 + (*s - '0');
        s++;
    }

    *out = neg ? -value : value;
    return 1;
}

static int fossil_extract_operator(
    const char *query,
    char *field,
    char *op,
    char *value
) {
    /* try symbolic first */
    if (sscanf(query, "%31s %31[<>=!] %31s", field, op, value) == 3)
        return 1;

    /* English operators */

    if (sscanf(query, "%31s is not %31s", field, value) == 2) {
        strcpy(op, "!=");
        return 1;
    }

    if (sscanf(query, "%31s is %31s", field, value) == 2) {
        strcpy(op, "=");
        return 1;
    }

    if (sscanf(query, "%31s equals %31s", field, value) == 2) {
        strcpy(op, "=");
        return 1;
    }

    if (sscanf(query, "%31s before %31s", field, value) == 2) {
        strcpy(op, "<");
        return 1;
    }

    if (sscanf(query, "%31s after %31s", field, value) == 2) {
        strcpy(op, ">");
        return 1;
    }

    if (sscanf(query, "%31s on or before %31s", field, value) == 2) {
        strcpy(op, "<=");
        return 1;
    }

    if (sscanf(query, "%31s on or after %31s", field, value) == 2) {
        strcpy(op, ">=");
        return 1;
    }

    return 0;
}

static int fossil_time_date_get_field(
    const fossil_time_date_t *dt,
    const char *field,
    int *out
) {
    if (!dt || !field || !out)
        return 0;

    /* year */
    if (fossil_pattern_match("year", field) ||
        fossil_pattern_match("y", field))
    {
        *out = dt->year;
        return 1;
    }

    /* month */
    if (fossil_pattern_match("month", field) ||
        fossil_pattern_match("mon", field) ||
        fossil_pattern_match("m", field))
    {
        *out = dt->month;
        return 1;
    }

    /* day */
    if (fossil_pattern_match("day", field) ||
        fossil_pattern_match("d", field))
    {
        *out = dt->day;
        return 1;
    }

    /* hour */
    if (fossil_pattern_match("hour", field) ||
        fossil_pattern_match("h", field))
    {
        *out = dt->hour;
        return 1;
    }

    /* minute */
    if (fossil_pattern_match("minute", field) ||
        fossil_pattern_match("min", field))
    {
        *out = dt->minute;
        return 1;
    }

    /* second */
    if (fossil_pattern_match("second", field) ||
        fossil_pattern_match("sec", field) ||
        fossil_pattern_match("s", field))
    {
        *out = dt->second;
        return 1;
    }

    /* weekday */
    if (fossil_pattern_match("weekday", field) ||
        fossil_pattern_match("wday", field) ||
        fossil_pattern_match("dow", field))
    {
        *out = dt->weekday;
        return 1;
    }

    /* yearday */
    if (fossil_pattern_match("yearday", field) ||
        fossil_pattern_match("yday", field))
    {
        *out = dt->yearday;
        return 1;
    }

    /* millisecond */
    if (fossil_pattern_match("millisecond", field) ||
        fossil_pattern_match("ms", field))
    {
        *out = dt->millisecond;
        return 1;
    }

    /* microsecond */
    if (fossil_pattern_match("microsecond", field) ||
        fossil_pattern_match("us", field))
    {
        *out = dt->microsecond;
        return 1;
    }

    /* nanosecond */
    if (fossil_pattern_match("nanosecond", field) ||
        fossil_pattern_match("ns", field))
    {
        *out = dt->nanosecond;
        return 1;
    }

    /* timezone offset */
    if (fossil_pattern_match("tz_offset", field) ||
        fossil_pattern_match("tz", field) ||
        fossil_pattern_match("offset", field))
    {
        *out = dt->tz_offset_min;
        return 1;
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
    if (!dt || !query || !*query)
        return 0;

    /* =========================================================
     * SIMPLE KEYWORDS (zero parsing)
     * ========================================================= */

    if (now && (!strcmp(query, "today") ||
                !strcmp(query, "this day")))
    {
        return dt->year  == now->year &&
               dt->month == now->month &&
               dt->day   == now->day;
    }

    if (!strcmp(query, "weekend") ||
        !strcmp(query, "is weekend"))
    {
        return dt->weekday == 0 || dt->weekday == 6;
    }

    if (!strcmp(query, "weekday") ||
        !strcmp(query, "is weekday"))
    {
        return dt->weekday >= 1 && dt->weekday <= 5;
    }

    if (!strcmp(query, "leap year"))
        return is_leap(dt->year);

    if (!strcmp(query, "first of month"))
        return dt->day == 1;

    if (!strcmp(query, "last of month"))
        return dt->day == days_in_month(dt->year, dt->month);

    /* =========================================================
     * MINIMAL RELATIVE KEYWORDS
     * ========================================================= */

    if (now) {
        if (!strcmp(query, "past") ||
            !strcmp(query, "in the past"))
            return fossil_time_date_compare(dt, now) < 0;

        if (!strcmp(query, "future") ||
            !strcmp(query, "in the future"))
            return fossil_time_date_compare(dt, now) > 0;

        if (!strcmp(query, "before today") ||
            !strcmp(query, "before now"))
            return fossil_time_date_compare(dt, now) < 0;

        if (!strcmp(query, "after today") ||
            !strcmp(query, "after now"))
            return fossil_time_date_compare(dt, now) > 0;
    }

    /* =========================================================
     * ADVANCED: FIELD COMPARISON (symbolic + English)
     * ========================================================= */

    {
        char field[32] = {0};
        char op[32] = {0};
        char value_str[32] = {0};

        if (fossil_extract_operator(query, field, op, value_str)) {

            int lhs;
            if (!fossil_time_date_get_field(dt, field, &lhs))
                return 0;

            int rhs;
            if (!fossil_parse_int(value_str, &rhs))
                return 0;

            return fossil_cmp(lhs, op, rhs);
        }
    }

    /* =========================================================
     * ADVANCED: RANGE (year in 2020..2025)
     * ========================================================= */

    {
        char field[32];
        int start, end;

        if (sscanf(query, "%31s in %d..%d", field, &start, &end) == 3) {
            int lhs;
            if (fossil_time_date_get_field(dt, field, &lhs)) {
                return lhs >= start && lhs <= end;
            }
            return 0;
        }
    }

    /* =========================================================
     * ADVANCED: WEEKDAY NAME
     * ========================================================= */

    static const char *weekdays[] = {
        "sunday", "monday", "tuesday", "wednesday",
        "thursday", "friday", "saturday"
    };

    for (int i = 0; i < 7; ++i) {
        if (!strcasecmp(query, weekdays[i]))
            return dt->weekday == i;
    }

    return 0;
}
