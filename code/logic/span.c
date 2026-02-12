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
#include "fossil/time/span.h"
#include <string.h>
#include <stdio.h>

/* ======================================================
 * Internal helpers
 * ====================================================== */

static void fossil_time_span_zero_fields(fossil_time_span_t *span) {
    memset(span, 0, sizeof(*span));
}

static int unit_equals(const char *a, const char *b) {
    return a && b && strcmp(a, b) == 0;
}

/* ======================================================
 * Core
 * ====================================================== */

void fossil_time_span_clear(fossil_time_span_t *span) {
    if (!span) return;
    fossil_time_span_zero_fields(span);
}

int fossil_time_span_validate(const fossil_time_span_t *span) {
    if (!span) return 0;

    /* Basic sanity checks — conservative by design */
    if (span->hours   < 0 || span->hours   >= 24) return 0;
    if (span->minutes < 0 || span->minutes >= 60) return 0;
    if (span->seconds < 0 || span->seconds >= 60) return 0;

    return 1;
}

void fossil_time_span_normalize(fossil_time_span_t *span) {
    if (!span) return;

    /* Carry SI ladder upward */
    span->microseconds += span->nanoseconds / 1000;
    span->nanoseconds  %= 1000;

    span->milliseconds += span->microseconds / 1000;
    span->microseconds %= 1000;

    span->seconds += span->milliseconds / 1000;
    span->milliseconds %= 1000;

    span->minutes += span->seconds / 60;
    span->seconds %= 60;

    span->hours += span->minutes / 60;
    span->minutes %= 60;

    span->days += span->hours / 24;
    span->hours %= 24;
}

/* ======================================================
 * Construction
 * ====================================================== */

void fossil_time_span_from_unit(
    fossil_time_span_t *span,
    int64_t value,
    const char *unit_id
) {
    if (!span || !unit_id) return;

    fossil_time_span_clear(span);

    if (unit_equals(unit_id, "days")) {
        span->days = value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_DAYS;
    } else if (unit_equals(unit_id, "hours")) {
        span->hours = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_HOURS;
    } else if (unit_equals(unit_id, "minutes")) {
        span->minutes = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_MINUTES;
    } else if (unit_equals(unit_id, "seconds")) {
        span->seconds = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_SECONDS;
    } else if (unit_equals(unit_id, "ms")) {
        span->milliseconds = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_MILLI;
    } else if (unit_equals(unit_id, "us")) {
        span->microseconds = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_MICRO;
    } else if (unit_equals(unit_id, "ns")) {
        span->nanoseconds = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_NANO;
    } else if (unit_equals(unit_id, "ps")) {
        span->picoseconds = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_PICO;
    } else if (unit_equals(unit_id, "fs")) {
        span->femtoseconds = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_FEMTO;
    } else if (unit_equals(unit_id, "as")) {
        span->attoseconds = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_ATTO;
    } else if (unit_equals(unit_id, "zs")) {
        span->zeptoseconds = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_ZEPTO;
    } else if (unit_equals(unit_id, "ys")) {
        span->yoctoseconds = (int32_t)value;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_YOCTO;
    }
}

void fossil_time_span_from_ai(
    fossil_time_span_t *span,
    const char *hint_id
) {
    if (!span || !hint_id) return;

    fossil_time_span_clear(span);

    if (unit_equals(hint_id, "moment")) {
        span->milliseconds = 250;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_MILLI;
    } else if (unit_equals(hint_id, "short")) {
        span->seconds = 1;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_SECONDS;
    } else if (unit_equals(hint_id, "long")) {
        span->seconds = 10;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_SECONDS;
    } else if (unit_equals(hint_id, "frame")) {
        span->milliseconds = 16;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_MILLI;
    } else if (unit_equals(hint_id, "human_tick")) {
        span->milliseconds = 100;
        span->precision_mask = FOSSIL_TIME_SPAN_PRECISION_MILLI;
    }
}

/* ======================================================
 * Arithmetic
 * ====================================================== */

void fossil_time_span_add(
    fossil_time_span_t *result,
    const fossil_time_span_t *a,
    const fossil_time_span_t *b
) {
    if (!result || !a || !b) return;

    *result = *a;

    result->days          += b->days;
    result->hours         += b->hours;
    result->minutes       += b->minutes;
    result->seconds       += b->seconds;
    result->milliseconds  += b->milliseconds;
    result->microseconds  += b->microseconds;
    result->nanoseconds   += b->nanoseconds;
    result->picoseconds   += b->picoseconds;
    result->femtoseconds  += b->femtoseconds;
    result->attoseconds   += b->attoseconds;
    result->zeptoseconds  += b->zeptoseconds;
    result->yoctoseconds  += b->yoctoseconds;

    result->precision_mask |= b->precision_mask;
}

void fossil_time_span_sub(
    fossil_time_span_t *result,
    const fossil_time_span_t *a,
    const fossil_time_span_t *b
) {
    if (!result || !a || !b) return;

    *result = *a;

    result->days          -= b->days;
    result->hours         -= b->hours;
    result->minutes       -= b->minutes;
    result->seconds       -= b->seconds;
    result->milliseconds  -= b->milliseconds;
    result->microseconds  -= b->microseconds;
    result->nanoseconds   -= b->nanoseconds;
    result->picoseconds   -= b->picoseconds;
    result->femtoseconds  -= b->femtoseconds;
    result->attoseconds   -= b->attoseconds;
    result->zeptoseconds  -= b->zeptoseconds;
    result->yoctoseconds  -= b->yoctoseconds;

    result->precision_mask |= b->precision_mask;
}

/* ======================================================
 * Conversion
 * ====================================================== */

int64_t fossil_time_span_to_seconds(const fossil_time_span_t *span) {
    if (!span) return 0;

    int64_t total = 0;

    if (span->precision_mask & FOSSIL_TIME_SPAN_PRECISION_DAYS)
        total += span->days * 86400;

    if (span->precision_mask & FOSSIL_TIME_SPAN_PRECISION_HOURS)
        total += span->hours * 3600;

    if (span->precision_mask & FOSSIL_TIME_SPAN_PRECISION_MINUTES)
        total += span->minutes * 60;

    if (span->precision_mask & FOSSIL_TIME_SPAN_PRECISION_SECONDS)
        total += span->seconds;

    return total;
}

int64_t fossil_time_span_to_nanoseconds(const fossil_time_span_t *span) {
    if (!span) return 0;

    int64_t total = fossil_time_span_to_seconds(span);
    total *= 1000000000LL;

    if (span->precision_mask & FOSSIL_TIME_SPAN_PRECISION_MILLI)
        total += (int64_t)span->milliseconds * 1000000LL;

    if (span->precision_mask & FOSSIL_TIME_SPAN_PRECISION_MICRO)
        total += (int64_t)span->microseconds * 1000LL;

    if (span->precision_mask & FOSSIL_TIME_SPAN_PRECISION_NANO)
        total += span->nanoseconds;

    return total;
}

/* ======================================================
 * Formatting
 * ====================================================== */

int fossil_time_span_format(
    const fossil_time_span_t *span,
    char *buffer,
    size_t buffer_size,
    const char *format_id
) {
    if (!span || !buffer || buffer_size == 0 || !format_id)
        return -1;

    if (unit_equals(format_id, "short")) {
        return snprintf(buffer, buffer_size,
            "%lldd %dh %dm %ds",
            (long long)span->days,
            span->hours,
            span->minutes,
            span->seconds
        );
    }

    if (unit_equals(format_id, "human")) {
        return snprintf(buffer, buffer_size,
            "%lld days, %d hours, %d minutes, %d seconds",
            (long long)span->days,
            span->hours,
            span->minutes,
            span->seconds
        );
    }

    return -1;
}
