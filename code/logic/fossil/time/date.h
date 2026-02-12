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

/**
 * @brief Populate the fossil_time_date_t structure with the current system time.
 *
 * This function fills the provided fossil_time_date_t structure with the current
 * date and time as reported by the system clock. All available fields, including
 * sub-second precision (such as milliseconds, microseconds, etc.), are populated
 * if supported by the underlying platform. The precision_mask is updated to reflect
 * which fields are valid. This function is intended to provide the most precise
 * representation of the current moment possible.
 *
 * @param dt Pointer to the fossil_time_date_t structure to populate with the current time.
 */
void fossil_time_date_now(fossil_time_date_t *dt);

/**
 * @brief Validate the contents of a fossil_time_date_t structure.
 *
 * This function checks that all fields present in the precision_mask of the given
 * fossil_time_date_t structure are within their valid ranges. For example, it ensures
 * that the month is between 1 and 12, the day is valid for the specified month and year,
 * and sub-second fields are within their respective bounds. If any field marked as present
 * in the precision_mask is out of range or inconsistent, the function returns 0 (invalid).
 * Otherwise, it returns a nonzero value indicating validity.
 *
 * @param dt Pointer to the fossil_time_date_t structure to validate.
 * @return Nonzero if all present fields are valid; 0 if any field is invalid.
 */
int fossil_time_date_validate(const fossil_time_date_t *dt);

/**
 * @brief Normalize a fossil_time_date_t structure.
 *
 * This function adjusts the fields of the fossil_time_date_t structure to ensure they
 * are within canonical ranges. For example, if the seconds field exceeds 60 (to account
 * for leap seconds), it rolls over the excess into the minutes field. It also fills in
 * derived fields such as weekday and yearday, and updates the precision_mask as needed
 * to reflect any changes. The normalization process ensures the structure represents a
 * valid and canonical date/time.
 *
 * @param dt Pointer to the fossil_time_date_t structure to normalize.
 */
void fossil_time_date_normalize(fossil_time_date_t *dt);

/**
 * @brief Compare two fossil_time_date_t structures.
 *
 * This function performs a field-wise comparison of two fossil_time_date_t structures,
 * considering only the highest available precision present in both structures. It returns
 * -1 if the first structure (a) represents an earlier date/time than the second (b),
 * 0 if they are equal at the available precision, and 1 if a is later than b.
 *
 * @param a Pointer to the first fossil_time_date_t structure.
 * @param b Pointer to the second fossil_time_date_t structure.
 * @return -1 if a < b, 0 if a == b, 1 if a > b, based on available precision.
 */
int fossil_time_date_compare(
    const fossil_time_date_t *a,
    const fossil_time_date_t *b
);

/**
 * @brief Compute the difference in seconds between two fossil_time_date_t structures.
 *
 * This function calculates the signed difference in seconds between two date/time
 * structures (a - b), using all available precision as indicated by their precision_mask
 * fields. Sub-second fields (such as milliseconds, microseconds, etc.) are included in
 * the calculation if present. The result may be negative if a is earlier than b.
 *
 * @param a Pointer to the first fossil_time_date_t structure.
 * @param b Pointer to the second fossil_time_date_t structure.
 * @return The signed difference in seconds (a - b), including sub-second precision.
 */
int64_t fossil_time_date_diff_seconds(
    const fossil_time_date_t *a,
    const fossil_time_date_t *b
);

/**
 * @brief Convert a fossil_time_date_t structure to Unix time (seconds since epoch).
 *
 * This function converts the date and time represented by the fossil_time_date_t structure
 * to the number of seconds since the Unix epoch (1970-01-01T00:00:00Z). Only fields present
 * in the precision_mask are used; missing fields are assumed to be zero or their minimal
 * valid value. The conversion accounts for the timezone offset if specified.
 *
 * @param dt Pointer to the fossil_time_date_t structure to convert.
 * @return The number of seconds since the Unix epoch.
 */
int64_t fossil_time_date_to_unix_seconds(
    const fossil_time_date_t *dt
);

/**
 * @brief Convert a fossil_time_date_t structure to Unix time in nanoseconds.
 *
 * This function converts the date and time represented by the fossil_time_date_t structure
 * to the number of nanoseconds since the Unix epoch (1970-01-01T00:00:00Z). Sub-second
 * fields (such as nanoseconds, microseconds, etc.) are included in the calculation if
 * present in the precision_mask. The conversion also accounts for the timezone offset.
 *
 * @param dt Pointer to the fossil_time_date_t structure to convert.
 * @return The number of nanoseconds since the Unix epoch.
 */
int64_t fossil_time_date_to_unix_nanoseconds(
    const fossil_time_date_t *dt
);

/**
 * @brief Populate a fossil_time_date_t structure from Unix time (seconds since epoch).
 *
 * This function fills the fossil_time_date_t structure with the corresponding date and
 * time for the given number of seconds since the Unix epoch (1970-01-01T00:00:00Z).
 * All fields are set with maximal available precision, and the precision_mask is updated
 * accordingly. The timezone offset is set to zero (UTC) unless otherwise specified.
 *
 * @param seconds The number of seconds since the Unix epoch.
 * @param dt Pointer to the fossil_time_date_t structure to populate.
 */
void fossil_time_date_from_unix_seconds(
    int64_t seconds,
    fossil_time_date_t *dt
);

/**
 * @brief Format a fossil_time_date_t structure as a string using a named format.
 *
 * This function formats the given fossil_time_date_t structure as a string according
 * to the specified format_id. Supported format_id values include:
 *   - "iso": ISO 8601 format (e.g., "2024-06-01T12:34:56Z")
 *   - "iso_nano": ISO 8601 with nanoseconds
 *   - "human": Human-readable format (e.g., "June 1, 2024, 12:34 PM")
 *   - "short": Short form (e.g., "6/1/24 12:34")
 *   - "log": Log-friendly format (e.g., "20240601-123456")
 *   - "ai": AI/semantic-friendly format
 * The formatted string is written to the provided buffer, which is guaranteed to be
 * null-terminated and not to exceed buffer_size bytes.
 *
 * @param dt         Pointer to the fossil_time_date_t structure to format.
 * @param buffer     Output buffer for the formatted string.
 * @param buffer_size Size of the output buffer in bytes.
 * @param format_id  String identifier for the desired format.
 * @return Number of characters written (excluding null terminator), or negative on error.
 */
int fossil_time_date_format(
    const fossil_time_date_t *dt,
    char *buffer,
    size_t buffer_size,
    const char *format_id
);

/**
 * @brief Format a fossil_time_date_t structure in a context-aware, human-friendly way.
 *
 * This function produces a context-sensitive, human-readable string representation of
 * the given fossil_time_date_t structure, relative to the provided "now" date/time.
 * Output examples include "Today at 3:00 PM", "Yesterday", or "June 1, 2024", depending
 * on the temporal relationship between dt and now. The result is written to the provided
 * buffer, which is null-terminated and does not exceed buffer_size bytes.
 *
 * @param dt          Pointer to the fossil_time_date_t structure to format.
 * @param now         Pointer to the current date/time for context.
 * @param buffer      Output buffer for the formatted string.
 * @param buffer_size Size of the output buffer in bytes.
 * @return Number of characters written (excluding null terminator), or negative on error.
 */
int fossil_time_date_format_smart(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    char *buffer,
    size_t buffer_size
);

/**
 * @brief Format the difference between two fossil_time_date_t structures as a relative string.
 *
 * This function produces a human-friendly, relative description of the temporal relationship
 * between the given date/time (dt) and a reference date/time (now). Example outputs include
 * "2 minutes ago", "in 5 days", or "just now". The result is written to the provided buffer,
 * which is null-terminated and does not exceed buffer_size bytes.
 *
 * @param dt          Pointer to the fossil_time_date_t structure to format.
 * @param now         Pointer to the fossil_time_date_t structure to use as the reference.
 * @param buffer      Output buffer for the formatted string.
 * @param buffer_size Size of the output buffer in bytes.
 * @return Number of characters written (excluding null terminator), or negative on error.
 */
int fossil_time_date_format_relative(
    const fossil_time_date_t *dt,
    const fossil_time_date_t *now,
    char *buffer,
    size_t buffer_size
);

/**
 * @brief Evaluate a search or query expression against a fossil_time_date_t structure.
 *
 * Evaluates whether the given fossil_time_date_t structure satisfies the specified
 * query expression. The query may be one of the following:
 *
 * Simple keywords:
 *   "today", "this day", "weekend", "weekday",
 *   "leap year", "first of month", "last of month"
 *
 * Relative expressions (require @p now):
 *   "past", "future", "before today", "after now",
 *   "in the past", "in the future"
 *
 * Field comparison expressions using symbolic or English operators:
 *   "year = 2025"
 *   "month >= 6"
 *   "weekday is not 0"
 *   "day before 15"
 *   "hour on or after 12"
 *
 * Range expressions:
 *   "year in 2020..2025"
 *
 * Day-of-week names:
 *   "monday", "tuesday", ..., "sunday"
 *
 * Symbolic operators supported:
 *   =  ==  !=  <  <=  >  >=
 *
 * English operators supported:
 *   is, is not, equals,
 *   before, after,
 *   on or before, on or after
 *
 * The optional @p now parameter provides contextual reference for relative
 * expressions such as "today", "past", and "future". If @p now is NULL,
 * only absolute field-based and context-free queries are evaluated.
 *
 * Invalid, malformed, or unsupported queries return 0.
 *
 * @param dt    Pointer to the fossil_time_date_t structure to evaluate.
 * @param now   Pointer to the current date/time for contextual queries (may be NULL).
 * @param query Query string describing the search or filter criteria.
 * @return Nonzero if the structure matches the query; 0 otherwise.
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

            /* ======================================================
             * Construction
             * ====================================================== */

            /**
             * @brief Default constructor. Initializes an empty Date object.
             * The underlying fossil_time_date_t structure is default-initialized.
             */
            Date() {
                raw.year           = 0;
                raw.month          = 0;
                raw.day            = 0;
        
                raw.hour           = 0;
                raw.minute         = 0;
                raw.second         = 0;
        
                raw.millisecond    = 0;
                raw.microsecond    = 0;
                raw.nanosecond     = 0;
                raw.picosecond     = 0;
                raw.femtosecond    = 0;
                raw.attosecond     = 0;
                raw.zeptosecond    = 0;
                raw.yoctosecond    = 0;
        
                raw.weekday        = -1;
                raw.yearday        = -1;
        
                raw.tz_offset_min  = 0;
        
                raw.precision_mask = 0;
            }

            /* ======================================================
             * Core
             * ====================================================== */

            /**
             * @brief Populate this Date object with the current system time.
             * Calls fossil_time_date_now to fill the raw structure with the
             * most precise current date and time available.
             */
            inline void now() {
                fossil_time_date_now(&raw);
            }

            /**
             * @brief Validate the contents of this Date object.
             * Checks that all fields present in the precision_mask are within
             * their valid ranges. Returns true if valid, false otherwise.
             */
            inline bool validate() const {
                return fossil_time_date_validate(&raw) != 0;
            }

            /**
             * @brief Normalize the fields of this Date object.
             * Adjusts all fields to canonical ranges, fills derived fields,
             * and updates the precision_mask as needed.
             */
            inline void normalize() {
                fossil_time_date_normalize(&raw);
            }

            /* ======================================================
             * Comparison
             * ====================================================== */

            /**
             * @brief Compare this Date object to another.
             * Performs a field-wise comparison using the highest available
             * precision present in both objects.
             * @param other The Date object to compare against.
             * @return -1 if this < other, 0 if equal, 1 if this > other.
             */
            inline int compare(const Date &other) const {
                return fossil_time_date_compare(&raw, &other.raw);
            }

            /**
             * @brief Compute the difference in seconds between this Date and another.
             * Uses all available precision, including sub-second fields, as indicated
             * by the precision_mask.
             * @param other The Date object to compare against.
             * @return Signed difference in seconds (this - other).
             */
            inline int64_t diff_seconds(const Date &other) const {
                return fossil_time_date_diff_seconds(&raw, &other.raw);
            }

            /* ======================================================
             * Conversion
             * ====================================================== */

            /**
             * @brief Convert this Date object to Unix time (seconds since epoch).
             * Only fields present in the precision_mask are used; missing fields
             * are assumed to be zero or their minimal valid value.
             * @return Number of seconds since the Unix epoch.
             */
            inline int64_t to_unix_seconds() const {
                return fossil_time_date_to_unix_seconds(&raw);
            }

            /**
             * @brief Convert this Date object to Unix time in nanoseconds.
             * Includes sub-second fields if present in the precision_mask.
             * @return Number of nanoseconds since the Unix epoch.
             */
            inline int64_t to_unix_nanoseconds() const {
                return fossil_time_date_to_unix_nanoseconds(&raw);
            }

            /**
             * @brief Populate this Date object from Unix time (seconds since epoch).
             * Fills all fields with maximal available precision and updates the
             * precision_mask accordingly.
             * @param seconds Number of seconds since the Unix epoch.
             */
            inline void from_unix_seconds(int64_t seconds) {
                fossil_time_date_from_unix_seconds(seconds, &raw);
            }

            /* ======================================================
             * Formatting
             * ====================================================== */

            /**
             * @brief Format this Date object as a string using a named format.
             * Supported formats include "iso", "iso_nano", "human", "short", "log", "ai".
             * The formatted string is written to the provided buffer.
             * @param buffer Output buffer for the formatted string.
             * @param buffer_size Size of the output buffer in bytes.
             * @param format_id String identifier for the desired format.
             * @return Number of characters written (excluding null terminator), or negative on error.
             */
            inline int format(
            char *buffer,
            size_t buffer_size,
            const char *format_id
            ) const {
                return fossil_time_date_format(&raw, buffer, buffer_size, format_id);
            }

            /**
             * @brief Format this Date object in a context-aware, human-friendly way.
             * Produces a context-sensitive string relative to the provided "now" date/time.
             * @param now Reference Date object for context.
             * @param buffer Output buffer for the formatted string.
             * @param buffer_size Size of the output buffer in bytes.
             * @return Number of characters written (excluding null terminator), or negative on error.
             */
            inline int format_smart(
            const Date &now,
            char *buffer,
            size_t buffer_size
            ) const {
                return fossil_time_date_format_smart(
                    &raw, &now.raw, buffer, buffer_size
                );
            }

            /**
             * @brief Format the difference between this Date and another as a relative string.
             * Produces a human-friendly, relative description (e.g., "2 minutes ago").
             * @param now Reference Date object for comparison.
             * @param buffer Output buffer for the formatted string.
             * @param buffer_size Size of the output buffer in bytes.
             * @return Number of characters written (excluding null terminator), or negative on error.
             */
            inline int format_relative(
            const Date &now,
            char *buffer,
            size_t buffer_size
            ) const {
                return fossil_time_date_format_relative(
                    &raw, &now.raw, buffer, buffer_size
                );
            }

            /* ======================================================
             * Search / DSL
             * ====================================================== */

            /**
             * @brief Evaluate a search or query expression against a fossil_time_date_t structure.
             *
             * Evaluates whether the given fossil_time_date_t structure satisfies the specified
             * query expression. The query may be one of the following:
             *
             * Simple keywords:
             *   "today", "this day", "weekend", "weekday",
             *   "leap year", "first of month", "last of month"
             *
             * Relative expressions (require @p now):
             *   "past", "future", "before today", "after now",
             *   "in the past", "in the future"
             *
             * Field comparison expressions using symbolic or English operators:
             *   "year = 2025"
             *   "month >= 6"
             *   "weekday is not 0"
             *   "day before 15"
             *   "hour on or after 12"
             *
             * Range expressions:
             *   "year in 2020..2025"
             *
             * Day-of-week names:
             *   "monday", "tuesday", ..., "sunday"
             *
             * Symbolic operators supported:
             *   =  ==  !=  <  <=  >  >=
             *
             * English operators supported:
             *   is, is not, equals,
             *   before, after,
             *   on or before, on or after
             *
             * The optional @p now parameter provides contextual reference for relative
             * expressions such as "today", "past", and "future". If @p now is NULL,
             * only absolute field-based and context-free queries are evaluated.
             *
             * Invalid, malformed, or unsupported queries return 0.
             *
             * @param dt    Pointer to the fossil_time_date_t structure to evaluate.
             * @param now   Pointer to the current date/time for contextual queries (may be NULL).
             * @param query Query string describing the search or filter criteria.
             * @return Nonzero if the structure matches the query; 0 otherwise.
             */
            inline bool search(
            const Date &now,
            const char *query
            ) const {
                return fossil_time_date_search(&raw, &now.raw, query) != 0;
            }
        };
    
    } /* namespace time */

} /* namespace fossil */

#endif

#endif /* FOSSIL_TIME_DATE_H */
