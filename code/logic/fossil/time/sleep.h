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
 * Sleep for a duration specified by a unit string.
 *
 * @param value    The numeric value representing the amount of time to sleep.
 * @param unit_id  A string representing the time unit. Supported units include:
 *                 "sec" (seconds), "ms" (milliseconds), "us" (microseconds),
 *                 "ns" (nanoseconds), "ps" (picoseconds), "fs" (femtoseconds),
 *                 "as" (attoseconds), "zs" (zeptoseconds), "ys" (yoctoseconds).
 *
 * The function interprets the value according to the specified unit and
 * suspends execution for the corresponding duration.
 */
void fossil_time_sleep(
    uint64_t value,
    const char *unit_id
);

/*
 * Sleep for a specified number of seconds.
 *
 * @param seconds  The number of seconds to sleep.
 *
 * This function provides an explicit, type-safe way to sleep for whole seconds.
 */
void fossil_time_sleep_seconds(uint64_t seconds);

/*
 * Sleep for a specified number of milliseconds.
 *
 * @param milliseconds  The number of milliseconds to sleep.
 *
 * This function provides an explicit, type-safe way to sleep for millisecond durations.
 */
void fossil_time_sleep_milliseconds(uint64_t milliseconds);

/*
 * Sleep for a specified number of microseconds.
 *
 * @param microseconds  The number of microseconds to sleep.
 *
 * This function provides an explicit, type-safe way to sleep for microsecond durations.
 */
void fossil_time_sleep_microseconds(uint64_t microseconds);

/*
 * Sleep for a specified number of nanoseconds.
 *
 * @param nanoseconds  The number of nanoseconds to sleep.
 *
 * This function provides an explicit, type-safe way to sleep for nanosecond durations.
 */
void fossil_time_sleep_nanoseconds(uint64_t nanoseconds);

/*
 * Sleep for a duration specified by a human-friendly hint string.
 *
 * @param hint_id  A string describing the desired sleep duration in a
 *                 human-readable or AI-friendly way. Examples include:
 *                 "a moment", "short", "human_tick", "frame", "yield".
 *
 * The function interprets the hint and sleeps for an appropriate duration.
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

    /**
     * @brief Sleep for a duration specified by a unit string.
     *
     * @param value    The numeric value representing the amount of time to sleep.
     * @param unit_id  A string representing the time unit. Supported units include:
     *                 "sec" (seconds), "ms" (milliseconds), "us" (microseconds),
     *                 "ns" (nanoseconds), "ps" (picoseconds), "fs" (femtoseconds),
     *                 "as" (attoseconds), "zs" (zeptoseconds), "ys" (yoctoseconds).
     *
     * This function interprets the value according to the specified unit and
     * suspends execution for the corresponding duration.
     */
    static inline void for_unit(
        uint64_t value,
        const char *unit_id
    ) {
        fossil_time_sleep(value, unit_id);
    }

    /**
     * @brief Sleep for a specified number of seconds.
     *
     * @param v  The number of seconds to sleep.
     *
     * Provides an explicit, type-safe way to sleep for whole seconds.
     */
    static inline void seconds(uint64_t v) {
        fossil_time_sleep_seconds(v);
    }

    /**
     * @brief Sleep for a specified number of milliseconds.
     *
     * @param v  The number of milliseconds to sleep.
     *
     * Provides an explicit, type-safe way to sleep for millisecond durations.
     */
    static inline void milliseconds(uint64_t v) {
        fossil_time_sleep_milliseconds(v);
    }

    /**
     * @brief Sleep for a specified number of microseconds.
     *
     * @param v  The number of microseconds to sleep.
     *
     * Provides an explicit, type-safe way to sleep for microsecond durations.
     */
    static inline void microseconds(uint64_t v) {
        fossil_time_sleep_microseconds(v);
    }

    /**
     * @brief Sleep for a specified number of nanoseconds.
     *
     * @param v  The number of nanoseconds to sleep.
     *
     * Provides an explicit, type-safe way to sleep for nanosecond durations.
     */
    static inline void nanoseconds(uint64_t v) {
        fossil_time_sleep_nanoseconds(v);
    }

    /**
     * @brief Sleep for a duration specified by a human-friendly hint string.
     *
     * @param hint_id  A string describing the desired sleep duration in a
     *                 human-readable or AI-friendly way. Examples include:
     *                 "a moment", "short", "human_tick", "frame", "yield".
     *
     * Interprets the hint and sleeps for an appropriate duration.
     */
    static inline void ai(const char *hint_id) {
        fossil_time_sleep_ai(hint_id);
    }
};

} /* namespace time */
} /* namespace fossil */
#endif

#endif /* FOSSIL_TIME_SLEEP_H */
