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
#ifndef FOSSIL_TIME_CALENDAR_H
#define FOSSIL_TIME_CALENDAR_H

#include <stdint.h>
#include "date.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================
 * Fossil Time — Calendar Logic
 * ====================================================== */

/*
 * Calendar utilities operate purely on provided dates.
 * No system time, no locale guessing, no global state.
 */

/* ======================================================
 * C API — Core calendar math
 * ====================================================== */

int fossil_time_calendar_is_leap_year(
    int32_t year
);

int fossil_time_calendar_days_in_month(
    int32_t year,
    int8_t month
);

/*
 * Compute derived fields on the date.
 * Does NOT modify higher-precision fields.
 */
void fossil_time_calendar_compute_derived(
    fossil_time_date_t *dt
);

/* ======================================================
 * C API — Queries (string IDs)
 * ====================================================== */

/*
 * Query examples:
 *   "weekday"
 *   "weekend"
 *   "monday"
 *   "friday"
 */
int fossil_time_calendar_is(
    const fossil_time_date_t *dt,
    const char *query_id
);

/*
 * Get calendar classification via string ID.
 *
 * class_id examples:
 *   "season"
 *   "quarter"
 *   "half"
 *
 * Result examples:
 *   "spring", "summer"
 *   "Q1", "Q4"
 *   "H1", "H2"
 */
int fossil_time_calendar_get(
    const fossil_time_date_t *dt,
    const char *class_id,
    char *buffer,
    size_t buffer_size
);

/*
 * Holiday lookup via string IDs.
 *
 * region_id examples:
 *   "us"
 *   "us_federal"
 *   "eu"
 *   "jp"
 *
 * Returns:
 *   1 if holiday
 *   0 if not
 */
int fossil_time_calendar_is_holiday(
    const fossil_time_date_t *dt,
    const char *region_id
);

/*
 * If holiday, write holiday name (string ID).
 *
 * Example outputs:
 *   "new_years_day"
 *   "independence_day"
 *   "christmas"
 */
int fossil_time_calendar_get_holiday(
    const fossil_time_date_t *dt,
    const char *region_id,
    char *buffer,
    size_t buffer_size
);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ======================================================
 * C++ Wrapper — Thin, Inline
 * ====================================================== */

#ifdef __cplusplus
namespace fossil {
namespace time {

class Calendar {
public:
    Calendar() = delete; /* static-only utility */

    static inline bool is_leap_year(int32_t year) {
        return fossil_time_calendar_is_leap_year(year) != 0;
    }

    static inline int days_in_month(int32_t year, int8_t month) {
        return fossil_time_calendar_days_in_month(year, month);
    }

    static inline void compute_derived(Date &dt) {
        fossil_time_calendar_compute_derived(&dt.raw);
    }

    static inline bool is(const Date &dt, const char *query_id) {
        return fossil_time_calendar_is(&dt.raw, query_id) != 0;
    }

    static inline int get(
        const Date &dt,
        const char *class_id,
        char *buffer,
        size_t buffer_size
    ) {
        return fossil_time_calendar_get(
            &dt.raw, class_id, buffer, buffer_size
        );
    }

    static inline bool is_holiday(
        const Date &dt,
        const char *region_id
    ) {
        return fossil_time_calendar_is_holiday(
            &dt.raw, region_id
        ) != 0;
    }

    static inline int get_holiday(
        const Date &dt,
        const char *region_id,
        char *buffer,
        size_t buffer_size
    ) {
        return fossil_time_calendar_get_holiday(
            &dt.raw, region_id, buffer, buffer_size
        );
    }
};

} /* namespace time */
} /* namespace fossil */
#endif

#endif /* FOSSIL_TIME_CALENDAR_H */
