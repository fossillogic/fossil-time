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
#ifndef FOSSIL_TIME_SPAN_H
#define FOSSIL_TIME_SPAN_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================
 * Fossil Time — Span (Duration)
 * ====================================================== */

/*
 * Represents a duration, not anchored to a calendar.
 * All fields are optional and governed by precision_mask.
 */
typedef struct fossil_time_span_t {

    /* -------- Large units -------- */
    int64_t days;

    /* -------- Clock units -------- */
    int32_t hours;
    int32_t minutes;
    int32_t seconds;

    /* -------- Sub-second (SI ladder) -------- */
    int32_t milliseconds;
    int32_t microseconds;
    int32_t nanoseconds;
    int32_t picoseconds;
    int32_t femtoseconds;
    int32_t attoseconds;
    int32_t zeptoseconds;
    int32_t yoctoseconds;

    /* -------- Precision -------- */
    uint64_t precision_mask;

} fossil_time_span_t;

/* ======================================================
 * Precision Mask Bits
 * ====================================================== */

#define FOSSIL_TIME_SPAN_PRECISION_DAYS     (1ULL << 0)
#define FOSSIL_TIME_SPAN_PRECISION_HOURS    (1ULL << 1)
#define FOSSIL_TIME_SPAN_PRECISION_MINUTES  (1ULL << 2)
#define FOSSIL_TIME_SPAN_PRECISION_SECONDS  (1ULL << 3)

#define FOSSIL_TIME_SPAN_PRECISION_MILLI    (1ULL << 4)
#define FOSSIL_TIME_SPAN_PRECISION_MICRO    (1ULL << 5)
#define FOSSIL_TIME_SPAN_PRECISION_NANO     (1ULL << 6)
#define FOSSIL_TIME_SPAN_PRECISION_PICO     (1ULL << 7)
#define FOSSIL_TIME_SPAN_PRECISION_FEMTO    (1ULL << 8)
#define FOSSIL_TIME_SPAN_PRECISION_ATTO     (1ULL << 9)
#define FOSSIL_TIME_SPAN_PRECISION_ZEPTO    (1ULL << 10)
#define FOSSIL_TIME_SPAN_PRECISION_YOCTO    (1ULL << 11)

/* ======================================================
 * C API — Core
 * ====================================================== */

/* 
 * Zero or reset all fields of the span to zero, including the precision mask.
 * This function is useful for initializing or reusing a span structure.
 */
void fossil_time_span_clear(fossil_time_span_t *span);

/*
 * Validate the contents of the span.
 * Returns nonzero if the span is valid (e.g., all fields are within expected ranges),
 * or zero if the span contains invalid values.
 */
int fossil_time_span_validate(const fossil_time_span_t *span);

/*
 * Normalize the span fields, carrying overflow from lower units to higher units
 * (e.g., 1000 milliseconds becomes 1 second), and updating the precision mask accordingly.
 * This ensures the span is in canonical form.
 */
void fossil_time_span_normalize(fossil_time_span_t *span);

/* ======================================================
 * C API — Construction
 * ====================================================== */

/*
 * Create a time span from a value and a unit string.
 *
 * This function sets the appropriate field in the span structure based on the
 * provided unit identifier and value. The precision mask is updated to reflect
 * the chosen unit. All other fields are set to zero.
 *
 * Parameters:
 *   span    - Pointer to the span structure to initialize.
 *   value   - The numeric value for the specified unit.
 *   unit_id - String identifier for the unit. Supported values include:
 *             "days", "hours", "minutes", "seconds",
 *             "ms" (milliseconds), "us" (microseconds), "ns" (nanoseconds),
 *             "ps" (picoseconds), "fs" (femtoseconds), "as" (attoseconds),
 *             "zs" (zeptoseconds), "ys" (yoctoseconds).
 *
 * Example:
 *   fossil_time_span_from_unit(&span, 5, "seconds");
 */
void fossil_time_span_from_unit(
    fossil_time_span_t *span,
    int64_t value,
    const char *unit_id
);

/*
 * Create a time span using an AI-friendly or semantic hint.
 *
 * This function initializes the span structure based on a human-readable or
 * AI-oriented hint string. The hint_id can represent common durations or
 * semantic concepts, such as "moment", "short", "long", "human_tick", or "frame".
 * The actual mapping of hints to durations is implementation-defined.
 *
 * Parameters:
 *   span    - Pointer to the span structure to initialize.
 *   hint_id - String identifier for the semantic duration hint.
 *
 * Example:
 *   fossil_time_span_from_ai(&span, "moment");
 */
void fossil_time_span_from_ai(
    fossil_time_span_t *span,
    const char *hint_id
);

/* ======================================================
 * C API — Arithmetic
 * ====================================================== */

/**
 * Add two time spans together.
 *
 * This function adds the corresponding fields of two fossil_time_span_t structures
 * and stores the result in the 'result' structure. The precision mask of the result
 * is the bitwise OR of the input precision masks. The result is not automatically
 * normalized; call fossil_time_span_normalize() if canonical form is required.
 *
 * Parameters:
 *   result - Pointer to the span structure to store the sum.
 *   a      - Pointer to the first operand span.
 *   b      - Pointer to the second operand span.
 */
void fossil_time_span_add(
    fossil_time_span_t *result,
    const fossil_time_span_t *a,
    const fossil_time_span_t *b
);

/**
 * Subtract one time span from another.
 *
 * This function subtracts the fields of span 'b' from span 'a' and stores the result
 * in the 'result' structure. The precision mask of the result is the bitwise OR of
 * the input precision masks. The result is not automatically normalized; call
 * fossil_time_span_normalize() if canonical form is required.
 *
 * Parameters:
 *   result - Pointer to the span structure to store the difference.
 *   a      - Pointer to the minuend span.
 *   b      - Pointer to the subtrahend span.
 */
void fossil_time_span_sub(
    fossil_time_span_t *result,
    const fossil_time_span_t *a,
    const fossil_time_span_t *b
);

/* ======================================================
 * C API — Conversion
 * ====================================================== */

/**
 * Convert a time span to total seconds.
 *
 * This function computes the total number of seconds represented by the span,
 * aggregating all fields (days, hours, minutes, seconds, and sub-second units)
 * into a single integer value in seconds. Sub-second units are truncated.
 * The calculation only includes fields indicated by the precision mask.
 *
 * Parameters:
 *   span - Pointer to the span structure to convert.
 *
 * Returns:
 *   The total number of seconds as a 64-bit integer.
 */
int64_t fossil_time_span_to_seconds(
    const fossil_time_span_t *span
);

/**
 * Convert a time span to total nanoseconds.
 *
 * This function computes the total number of nanoseconds represented by the span,
 * aggregating all fields (days, hours, minutes, seconds, and sub-second units)
 * into a single integer value in nanoseconds. The calculation only includes
 * fields indicated by the precision mask.
 *
 * Parameters:
 *   span - Pointer to the span structure to convert.
 *
 * Returns:
 *   The total number of nanoseconds as a 64-bit integer.
 */
int64_t fossil_time_span_to_nanoseconds(
    const fossil_time_span_t *span
);

/* ======================================================
 * C API — Formatting
 * ====================================================== */

/**
 * Format a time span as a string.
 *
 * This function converts the given fossil_time_span_t structure into a
 * human-readable string representation, according to the specified format_id.
 * The formatted string is written to the provided buffer, which must be at
 * least buffer_size bytes in length. The output is always null-terminated
 * unless buffer_size is zero.
 *
 * Supported format_id values include:
 *   "short"   - Compact notation (e.g., "5s", "2h 30m").
 *   "human"   - Human-friendly words (e.g., "5 seconds", "2 hours, 30 minutes").
 *   "precise" - High-precision decimal (e.g., "5.000000000 s").
 *   "ai"      - AI/semantic-friendly format (implementation-defined).
 *
 * Parameters:
 *   span        - Pointer to the span structure to format.
 *   buffer      - Pointer to the character buffer to receive the formatted string.
 *   buffer_size - Size of the buffer in bytes.
 *   format_id   - String identifier for the desired output format.
 *
 * Returns:
 *   The number of characters written (excluding the null terminator), or a negative
 *   value if an error occurred (e.g., invalid format_id or insufficient buffer).
 */
int fossil_time_span_format(
    const fossil_time_span_t *span,
    char *buffer,
    size_t buffer_size,
    const char *format_id
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

class Span {
public:
    fossil_time_span_t raw;

    /**
     * Default constructor.
     * Initializes the Span object without modifying its contents.
     */
    Span() { }

    /**
     * Clear all fields of the span, including the precision mask.
     * Useful for initializing or resetting the span.
     */
    inline void clear() {
        fossil_time_span_clear(&raw);
    }

    /**
     * Validate the contents of the span.
     * Returns true if all fields are within expected ranges.
     */
    inline bool validate() const {
        return fossil_time_span_validate(&raw) != 0;
    }

    /**
     * Normalize the span fields, carrying overflow from lower units to higher units.
     * Updates the precision mask accordingly.
     */
    inline void normalize() {
        fossil_time_span_normalize(&raw);
    }

    /**
     * Set the span using a value and unit string.
     * Only the specified unit field is set; others are zeroed.
     * Updates the precision mask.
     */
    inline void from_unit(int64_t value, const char *unit_id) {
        fossil_time_span_from_unit(&raw, value, unit_id);
    }

    /**
     * Set the span using a semantic or AI-friendly hint string.
     * The mapping of hints to durations is implementation-defined.
     */
    inline void from_ai(const char *hint_id) {
        fossil_time_span_from_ai(&raw, hint_id);
    }

    /**
     * Convert the span to total seconds.
     * Aggregates all fields into a single integer value in seconds.
     */
    inline int64_t to_seconds() const {
        return fossil_time_span_to_seconds(&raw);
    }

    /**
     * Convert the span to total nanoseconds.
     * Aggregates all fields into a single integer value in nanoseconds.
     */
    inline int64_t to_nanoseconds() const {
        return fossil_time_span_to_nanoseconds(&raw);
    }

    /**
     * Format the span as a string according to the specified format.
     * Writes the formatted string to the provided buffer.
     * Returns the number of characters written, or negative on error.
     */
    inline int format(
        char *buffer,
        size_t buffer_size,
        const char *format_id
    ) const {
        return fossil_time_span_format(&raw, buffer, buffer_size, format_id);
    }

    /**
     * Add two spans together.
     * Returns a new Span representing the sum.
     * The result is not automatically normalized.
     */
    static inline Span add(const Span &a, const Span &b) {
        Span out;
        fossil_time_span_add(&out.raw, &a.raw, &b.raw);
        return out;
    }

    /**
     * Subtract one span from another.
     * Returns a new Span representing the difference.
     * The result is not automatically normalized.
     */
    static inline Span sub(const Span &a, const Span &b) {
        Span out;
        fossil_time_span_sub(&out.raw, &a.raw, &b.raw);
        return out;
    }
};

} /* namespace time */
} /* namespace fossil */
#endif

#endif /* FOSSIL_TIME_SPAN_H */
