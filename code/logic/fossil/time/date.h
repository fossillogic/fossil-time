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
#ifndef FOSSIL_TIME_DATE_H
#define FOSSIL_TIME_DATE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================
 * C STRUCT
 * ====================================================== */
typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int nanosecond;
} fossil_time_date_t;

/* ======================================================
 * C FUNCTIONS
 * ====================================================== */
void fossil_time_date_now(fossil_time_date_t *dt);

int fossil_time_date_validate(const fossil_time_date_t *dt);
void fossil_time_date_normalize(fossil_time_date_t *dt);

int64_t fossil_time_date_to_unix(const fossil_time_date_t *dt);
void fossil_time_date_from_unix(int64_t ts, fossil_time_date_t *dt);

int fossil_time_date_diff_seconds(const fossil_time_date_t *a, const fossil_time_date_t *b);

/* Format using string IDs (human, iso, short, etc.) */
int fossil_time_date_format(
    const fossil_time_date_t *dt,
    char *buffer,
    size_t buffer_size,
    const char *format_id
);

/* AI-enhanced smart formatting */
int fossil_time_date_format_smart(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    char *buffer,
    size_t buffer_size
);

/* Relative time formatting: "3 minutes ago", "yesterday" */
int fossil_time_date_format_relative(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    char *buffer,
    size_t buffer_size
);

/* Search/query DSL: string-based datetime search */
int fossil_time_date_search(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    const char *query
);

#ifdef __cplusplus
} // extern "C"
#endif

/* ======================================================
 * C++ INTERFACE
 * ====================================================== */
#ifdef __cplusplus
namespace fossil {
namespace time {

class Date {
public:
    int year, month, day;
    int hour, minute, second;
    int nanosecond;

    Date();

    void now();
    bool validate() const;
    void normalize();

    int64_t to_unix() const;
    void from_unix(int64_t ts);

    int diff_seconds(const Date &other) const;

    int format(char *buffer, size_t buffer_size, const char *format_id) const;
    int format_smart(const Date &now, char *buffer, size_t buffer_size) const;
    int format_relative(const Date &now, char *buffer, size_t buffer_size) const;

    /* Search/query DSL: string-based datetime search */
    bool search(const Date &now, const char *query) const;
};

} // namespace time
} // namespace fossil
#endif

#endif
