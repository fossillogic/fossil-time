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
#include "fossil/time/calendar.h"
#include <string.h>
#include <stdio.h>

/* ======================================================
 * Internal helpers
 * ====================================================== */

static int is_leap(int32_t year) {
    return ((year % 4 == 0 && year % 100 != 0) ||
            (year % 400 == 0));
}

static int days_in_month_internal(int32_t year, int8_t month) {
    static const int days[] = {
        31,28,31,30,31,30,31,31,30,31,30,31
    };

    if (month < 1 || month > 12)
        return 0;

    if (month == 2 && is_leap(year))
        return 29;

    return days[month - 1];
}

/* Zeller’s congruence (Gregorian) */
static int compute_weekday(
    int32_t year,
    int8_t month,
    int8_t day
) {
    if (month < 3) {
        month += 12;
        year--;
    }

    int k = year % 100;
    int j = year / 100;

    int h = (day + (13 * (month + 1)) / 5 +
             k + (k / 4) + (j / 4) + (5 * j)) % 7;

    /* Convert to Sunday = 0 */
    return (h + 6) % 7;
}

static int compute_yearday(
    int32_t year,
    int8_t month,
    int8_t day
) {
    int yd = 0;
    for (int m = 1; m < month; ++m)
        yd += days_in_month_internal(year, m);
    return yd + day;
}

/* ======================================================
 * C API — Core calendar math
 * ====================================================== */

int fossil_time_calendar_is_leap_year(
    int32_t year
) {
    return is_leap(year);
}

int fossil_time_calendar_days_in_month(
    int32_t year,
    int8_t month
) {
    return days_in_month_internal(year, month);
}

void fossil_time_calendar_compute_derived(
    fossil_time_date_t *dt
) {
    if (!dt)
        return;

    /* Weekday */
    if ((dt->precision_mask & FOSSIL_TIME_PRECISION_YEAR) &&
        (dt->precision_mask & FOSSIL_TIME_PRECISION_MONTH) &&
        (dt->precision_mask & FOSSIL_TIME_PRECISION_DAY)) {

        dt->weekday = compute_weekday(
            dt->year, dt->month, dt->day
        );

        dt->yearday = compute_yearday(
            dt->year, dt->month, dt->day
        );
    } else {
        dt->weekday = -1;
        dt->yearday = -1;
    }
}

/* ======================================================
 * C API — Queries (string IDs)
 * ====================================================== */

int fossil_time_calendar_is(
    const fossil_time_date_t *dt,
    const char *query_id
) {
    if (!dt || !query_id)
        return 0;

    if (!strcmp(query_id, "weekday"))
        return (dt->weekday >= 1 && dt->weekday <= 5);

    if (!strcmp(query_id, "weekend"))
        return (dt->weekday == 0 || dt->weekday == 6);

    if (!strcmp(query_id, "sunday"))    return dt->weekday == 0;
    if (!strcmp(query_id, "monday"))    return dt->weekday == 1;
    if (!strcmp(query_id, "tuesday"))   return dt->weekday == 2;
    if (!strcmp(query_id, "wednesday")) return dt->weekday == 3;
    if (!strcmp(query_id, "thursday"))  return dt->weekday == 4;
    if (!strcmp(query_id, "friday"))    return dt->weekday == 5;
    if (!strcmp(query_id, "saturday"))  return dt->weekday == 6;

    return 0;
}

int fossil_time_calendar_get(
    const fossil_time_date_t *dt,
    const char *class_id,
    char *buffer,
    size_t buffer_size
) {
    if (!dt || !class_id || !buffer || buffer_size == 0)
        return -1;

    if (!strcmp(class_id, "quarter")) {
        int q = (dt->month - 1) / 3 + 1;
        return snprintf(buffer, buffer_size, "Q%d", q);
    }

    if (!strcmp(class_id, "half")) {
        return snprintf(
            buffer, buffer_size,
            (dt->month <= 6) ? "H1" : "H2"
        );
    }

    if (!strcmp(class_id, "season")) {
        const char *season = "winter";

        if (dt->month >= 3 && dt->month <= 5)
            season = "spring";
        else if (dt->month >= 6 && dt->month <= 8)
            season = "summer";
        else if (dt->month >= 9 && dt->month <= 11)
            season = "autumn";

        return snprintf(buffer, buffer_size, "%s", season);
    }

    return -1;
}

/* ======================================================
 * C API — Holidays (minimal baseline)
 * ====================================================== */

int fossil_time_calendar_is_holiday(
    const fossil_time_date_t *dt,
    const char *region_id
) {
    char tmp[32];
    return fossil_time_calendar_get_holiday(
        dt, region_id, tmp, sizeof(tmp)
    ) == 0;
}

int fossil_time_calendar_get_holiday(
    const fossil_time_date_t *dt,
    const char *region_id,
    char *buffer,
    size_t buffer_size
) {
    if (!dt || !region_id || !buffer || buffer_size == 0)
        return -1;

    /* Minimal US federal baseline */
    if (!strcmp(region_id, "us") ||
        !strcmp(region_id, "us_federal")) {

        /* New Year's Day */
        if (dt->month == 1 && dt->day == 1) {
            snprintf(buffer, buffer_size, "new_years_day");
            return strlen("new_years_day");
        }

        /* MLK Jr. Day (3rd Monday in January) */
        if (dt->month == 1 && dt->weekday == 1 && dt->day >= 15 && dt->day <= 21) {
            snprintf(buffer, buffer_size, "mlk_day");
            return strlen("mlk_day");
        }

        /* Presidents' Day (3rd Monday in February) */
        if (dt->month == 2 && dt->weekday == 1 && dt->day >= 15 && dt->day <= 21) {
            snprintf(buffer, buffer_size, "presidents_day");
            return strlen("presidents_day");
        }

        /* Memorial Day (last Monday in May) */
        if (dt->month == 5 && dt->weekday == 1 && dt->day >= 25) {
            snprintf(buffer, buffer_size, "memorial_day");
            return strlen("memorial_day");
        }

        /* Independence Day */
        if (dt->month == 7 && dt->day == 4) {
            snprintf(buffer, buffer_size, "independence_day");
            return strlen("independence_day");
        }

        /* Labor Day (1st Monday in September) */
        if (dt->month == 9 && dt->weekday == 1 && dt->day <= 7) {
            snprintf(buffer, buffer_size, "labor_day");
            return strlen("labor_day");
        }

        /* Columbus Day (2nd Monday in October) */
        if (dt->month == 10 && dt->weekday == 1 && dt->day >= 8 && dt->day <= 14) {
            snprintf(buffer, buffer_size, "columbus_day");
            return strlen("columbus_day");
        }

        /* Veterans Day */
        if (dt->month == 11 && dt->day == 11) {
            snprintf(buffer, buffer_size, "veterans_day");
            return strlen("veterans_day");
        }

        /* Thanksgiving (4th Thursday in November) */
        if (dt->month == 11 && dt->weekday == 4 && dt->day >= 22 && dt->day <= 28) {
            snprintf(buffer, buffer_size, "thanksgiving");
            return strlen("thanksgiving");
        }

        /* Christmas */
        if (dt->month == 12 && dt->day == 25) {
            snprintf(buffer, buffer_size, "christmas");
            return strlen("christmas");
        }
    }

    /* Minimal UK baseline */
    if (!strcmp(region_id, "uk") ||
        !strcmp(region_id, "gb")) {

        /* New Year's Day */
        if (dt->month == 1 && dt->day == 1) {
            snprintf(buffer, buffer_size, "new_years_day");
            return strlen("new_years_day");
        }

        /* Christmas Day */
        if (dt->month == 12 && dt->day == 25) {
            snprintf(buffer, buffer_size, "christmas");
            return strlen("christmas");
        }

        /* Boxing Day */
        if (dt->month == 12 && dt->day == 26) {
            snprintf(buffer, buffer_size, "boxing_day");
            return strlen("boxing_day");
        }
    }

    /* Minimal Canada baseline */
    if (!strcmp(region_id, "ca") ||
        !strcmp(region_id, "canada")) {

        /* New Year's Day */
        if (dt->month == 1 && dt->day == 1) {
            snprintf(buffer, buffer_size, "new_years_day");
            return strlen("new_years_day");
        }

        /* Canada Day */
        if (dt->month == 7 && dt->day == 1) {
            snprintf(buffer, buffer_size, "canada_day");
            return strlen("canada_day");
        }

        /* Christmas Day */
        if (dt->month == 12 && dt->day == 25) {
            snprintf(buffer, buffer_size, "christmas");
            return strlen("christmas");
        }
    }

    return -1;
}
