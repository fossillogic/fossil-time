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
#ifndef FOSSIL_TIME_SLEEP_H
#define FOSSIL_TIME_SLEEP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================
 * C API — Sleep
 * ====================================================== */

/*
 * Sleep for a duration specified by unit string.
 *
 * unit_id examples:
 *   "sec"
 *   "ms"
 *   "us"
 *   "ns"
 *   "ps"
 *   "fs"
 *   "as"
 *   "zs"
 *   "ys"
 *
 * Value is interpreted in the given unit.
 */
void fossil_time_sleep(
    uint64_t value,
    const char *unit_id
);

/* Explicit helpers (no guessing, no strings) */
void fossil_time_sleep_seconds(uint64_t seconds);
void fossil_time_sleep_milliseconds(uint64_t milliseconds);
void fossil_time_sleep_microseconds(uint64_t microseconds);
void fossil_time_sleep_nanoseconds(uint64_t nanoseconds);

/*
 * AI-friendly sleep:
 * Examples:
 *   "a moment"
 *   "short"
 *   "human_tick"
 *   "frame"
 *   "yield"
 */
void fossil_time_sleep_ai(
    const char *hint_id
);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ======================================================
 * C++ Wrapper — Thin Only
 * ====================================================== */

#ifdef __cplusplus
namespace fossil {
namespace time {

class Sleep {
public:
    Sleep() = delete; /* static-only utility */

    static inline void for_unit(
        uint64_t value,
        const char *unit_id
    ) {
        fossil_time_sleep(value, unit_id);
    }

    static inline void seconds(uint64_t v) {
        fossil_time_sleep_seconds(v);
    }

    static inline void milliseconds(uint64_t v) {
        fossil_time_sleep_milliseconds(v);
    }

    static inline void microseconds(uint64_t v) {
        fossil_time_sleep_microseconds(v);
    }

    static inline void nanoseconds(uint64_t v) {
        fossil_time_sleep_nanoseconds(v);
    }

    static inline void ai(const char *hint_id) {
        fossil_time_sleep_ai(hint_id);
    }
};

} /* namespace time */
} /* namespace fossil */
#endif

#endif /* FOSSIL_TIME_SLEEP_H */
