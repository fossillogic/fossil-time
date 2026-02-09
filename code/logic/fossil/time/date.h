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
 * Fossil Time — Maximal Date/Time Structure
 * ====================================================== */

/*
 * This structure represents a lossless, maximally-expressive
 * datetime with explicit precision and human + scientific units.
 *
 * All fields are optional unless marked present in precision_mask.
 */
typedef struct fossil_time_date_t {

    /* -------- Calendar -------- */
    int32_t year;        /* e.g. 2026 */
    int8_t  month;       /* 1–12 */
    int8_t  day;         /* 1–31 */

    /* -------- Clock -------- */
    int8_t  hour;        /* 0–23 */
    int8_t  minute;      /* 0–59 */
    int8_t  second;      /* 0–60 (leap second safe) */

    /* -------- Sub-second (SI ladder) -------- */
    int16_t millisecond; /* 0–999 */
    int16_t microsecond; /* 0–999 */
    int16_t nanosecond;  /* 0–999 */
    int16_t picosecond;  /* 0–999 */
    int16_t femtosecond; /* 0–999 */
    int16_t attosecond;  /* 0–999 */
    int16_t zeptosecond; /* 0–999 */
    int16_t yoctosecond; /* 0–999 */

    /* -------- Derived / cached -------- */
    int8_t  weekday;     /* 0–6 (Sun=0), -1 if unknown */
    int16_t yearday;     /* 1–366, -1 if unknown */

    /* -------- Timezone -------- */
    int16_t tz_offset_min; /* minutes east of UTC */

    /* -------- Precision -------- */
    /*
     * Bitmask describing which fields are valid.
     * Enables AI reasoning, partial dates, and fuzzy time.
     */
    uint64_t precision_mask;

} fossil_time_date_t;

/* ======================================================
 * Precision Mask Bits
 * ====================================================== */

#define FOSSIL_TIME_PRECISION_YEAR     (1ULL << 0)
#define FOSSIL_TIME_PRECISION_MONTH    (1ULL << 1)
#define FOSSIL_TIME_PRECISION_DAY      (1ULL << 2)
#define FOSSIL_TIME_PRECISION_HOUR     (1ULL << 3)
#define FOSSIL_TIME_PRECISION_MINUTE   (1ULL << 4)
#define FOSSIL_TIME_PRECISION_SECOND   (1ULL << 5)

#define FOSSIL_TIME_PRECISION_MILLI    (1ULL << 6)
#define FOSSIL_TIME_PRECISION_MICRO    (1ULL << 7)
#define FOSSIL_TIME_PRECISION_NANO     (1ULL << 8)
#define FOSSIL_TIME_PRECISION_PICO     (1ULL << 9)
#define FOSSIL_TIME_PRECISION_FEMTO    (1ULL << 10)
#define FOSSIL_TIME_PRECISION_ATTO     (1ULL << 11)
#define FOSSIL_TIME_PRECISION_ZEPTO    (1ULL << 12)
#define FOSSIL_TIME_PRECISION_YOCTO    (1ULL << 13)

/* ======================================================
 * C API — Core
 * ====================================================== */

/* Populate with current system time */
void fossil_time_date_now(fossil_time_date_t *dt);

/* Validation & normalization */
int  fossil_time_date_validate(const fossil_time_date_t *dt);
void fossil_time_date_normalize(fossil_time_date_t *dt);

/* Comparison */
int  fossil_time_date_compare(
    const fossil_time_date_t *a,
    const fossil_time_date_t *b
);

/* Difference */
int64_t fossil_time_date_diff_seconds(
    const fossil_time_date_t *a,
    const fossil_time_date_t *b
);

/* ======================================================
 * C API — Conversion
 * ====================================================== */

int64_t fossil_time_date_to_unix_seconds(
    const fossil_time_date_t *dt
);

int64_t fossil_time_date_to_unix_nanoseconds(
    const fossil_time_date_t *dt
);

void fossil_time_date_from_unix_seconds(
    int64_t seconds,
    fossil_time_date_t *dt
);

/* ======================================================
 * C API — Formatting (string IDs)
 * ====================================================== */

/*
 * format_id examples:
 *   "iso"
 *   "iso_nano"
 *   "human"
 *   "short"
 *   "log"
 *   "ai"
 */
int fossil_time_date_format(
    const fossil_time_date_t *dt,
    char *buffer,
    size_t buffer_size,
    const char *format_id
);

/* Context-aware formatting ("Today at 3pm") */
int fossil_time_date_format_smart(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    char *buffer,
    size_t buffer_size
);

/* Relative formatting ("2 minutes ago") */
int fossil_time_date_format_relative(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    char *buffer,
    size_t buffer_size
);

/* ======================================================
 * C API — Search / DSL
 * ====================================================== */

/*
 * Query examples:
 *   "today"
 *   "yesterday"
 *   "> 2025-01-01"
 *   "within 5 seconds"
 *   "same month"
 *   "weekend"
 */
int fossil_time_date_search(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    const char *query
);

#ifdef __cplusplus
} /* extern "C" */

/* ======================================================
 * C++ Interface
 * ====================================================== */

namespace fossil {

    namespace time {
    
        class Date {
        public:
            fossil_time_date_t raw;
        
            Date();
        
            void now();
        
            bool validate() const;
            void normalize();
        
            int compare(const Date &other) const;
            int64_t diff_seconds(const Date &other) const;
        
            int64_t to_unix_seconds() const;
            int64_t to_unix_nanoseconds() const;
        
            void from_unix_seconds(int64_t seconds);
        
            int format(
                char *buffer,
                size_t buffer_size,
                const char *format_id
            ) const;
        
            int format_smart(
                const Date &now,
                char *buffer,
                size_t buffer_size
            ) const;
        
            int format_relative(
                const Date &now,
                char *buffer,
                size_t buffer_size
            ) const;
        
            bool search(
                const Date &now,
                const char *query
            ) const;
        };
    
    } /* namespace time */

} /* namespace fossil */

#endif

#endif /* FOSSIL_TIME_DATE_H */
