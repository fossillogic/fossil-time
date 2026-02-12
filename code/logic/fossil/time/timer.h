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
#ifndef FOSSIL_TIME_TIMER_H
#define FOSSIL_TIME_TIMER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================
 * Fossil Time — Monotonic Timer
 * ====================================================== */

/*
 * Timer represents a monotonic measurement point.
 * It is NOT wall-clock time and must never jump backwards.
 */
typedef struct fossil_time_timer_t {
    uint64_t start_ns;
} fossil_time_timer_t;

/* ======================================================
 * C API — Core
 * ====================================================== */

/**
 * @brief Initialize or reset the timer to the current monotonic time.
 *
 * This function sets the timer's start point to the current monotonic
 * time in nanoseconds. Use this to begin or restart timing.
 *
 * @param timer Pointer to a fossil_time_timer_t structure to initialize.
 */
void fossil_time_timer_start(
    fossil_time_timer_t *timer
);

/**
 * @brief Get the elapsed time in nanoseconds since the timer was started.
 *
 * Returns the difference between the current monotonic time and the timer's
 * start point, in nanoseconds.
 *
 * @param timer Pointer to a fossil_time_timer_t structure.
 * @return Elapsed time in nanoseconds.
 */
uint64_t fossil_time_timer_elapsed_ns(
    const fossil_time_timer_t *timer
);

/**
 * @brief Get the elapsed time in microseconds since the timer was started.
 *
 * Returns the elapsed time in microseconds, derived from the monotonic clock.
 *
 * @param timer Pointer to a fossil_time_timer_t structure.
 * @return Elapsed time in microseconds.
 */
uint64_t fossil_time_timer_elapsed_us(
    const fossil_time_timer_t *timer
);

/**
 * @brief Get the elapsed time in milliseconds since the timer was started.
 *
 * Returns the elapsed time in milliseconds, derived from the monotonic clock.
 *
 * @param timer Pointer to a fossil_time_timer_t structure.
 * @return Elapsed time in milliseconds.
 */
uint64_t fossil_time_timer_elapsed_ms(
    const fossil_time_timer_t *timer
);

/**
 * @brief Get the elapsed time in seconds since the timer was started.
 *
 * Returns the elapsed time as a double-precision floating point value
 * in seconds, derived from the monotonic clock.
 *
 * @param timer Pointer to a fossil_time_timer_t structure.
 * @return Elapsed time in seconds.
 */
double fossil_time_timer_elapsed_sec(
    const fossil_time_timer_t *timer
);

/* ======================================================
 * C API — Convenience
 * ====================================================== */

/**
 * @brief Measure elapsed time and reset the timer's start point.
 *
 * Returns the elapsed time in nanoseconds since the last start or lap,
 * and resets the timer's start point to the current time.
 * Useful for measuring intervals in loops or frames.
 *
 * @param timer Pointer to a fossil_time_timer_t structure.
 * @return Elapsed time in nanoseconds.
 */
uint64_t fossil_time_timer_lap_ns(
    fossil_time_timer_t *timer
);

/* ======================================================
 * C API — AI / Hint-Based Timing
 * ====================================================== */

/**
 * @brief Interpret a timing hint and return a duration in nanoseconds.
 *
 * Converts a string-based timing hint (e.g., "frame", "tick", "yield",
 * "human_short", "human_long") into a recommended duration in nanoseconds.
 * Useful for AI, scheduling, or human-centric timing.
 *
 * @param hint_id String identifier for the timing hint.
 * @return Duration in nanoseconds corresponding to the hint.
 */
uint64_t fossil_time_timer_hint_ns(
    const char *hint_id
);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ======================================================
 * C++ Wrapper — Thin, Inline, ABI-Safe
 * ====================================================== */

#ifdef __cplusplus
namespace fossil {
namespace time {

/**
 * @brief C++ wrapper class for fossil_time_timer_t.
 *
 * Provides a thin, inline, ABI-safe interface for monotonic timing
 * using the Fossil Time C API. This class allows convenient usage
 * of timer functionality in C++ code, including starting, measuring
 * elapsed time in various units, performing lap measurements, and
 * interpreting timing hints.
 */
class Timer {
public:
    /**
     * @brief The underlying C timer structure.
     */
    fossil_time_timer_t raw;

    /**
     * @brief Default constructor.
     *
     * Constructs a Timer object. The timer is not started automatically;
     * call start() to begin timing.
     */
    Timer() { }

    /**
     * @brief Start or reset the timer to the current monotonic time.
     *
     * Sets the timer's start point to the current monotonic time in nanoseconds.
     * Use this to begin or restart timing.
     */
    inline void start() {
        fossil_time_timer_start(&raw);
    }

    /**
     * @brief Get the elapsed time in nanoseconds since the timer was started.
     *
     * Returns the difference between the current monotonic time and the timer's
     * start point, in nanoseconds.
     *
     * @return Elapsed time in nanoseconds.
     */
    inline uint64_t elapsed_ns() const {
        return fossil_time_timer_elapsed_ns(&raw);
    }

    /**
     * @brief Get the elapsed time in microseconds since the timer was started.
     *
     * Returns the elapsed time in microseconds, derived from the monotonic clock.
     *
     * @return Elapsed time in microseconds.
     */
    inline uint64_t elapsed_us() const {
        return fossil_time_timer_elapsed_us(&raw);
    }

    /**
     * @brief Get the elapsed time in milliseconds since the timer was started.
     *
     * Returns the elapsed time in milliseconds, derived from the monotonic clock.
     *
     * @return Elapsed time in milliseconds.
     */
    inline uint64_t elapsed_ms() const {
        return fossil_time_timer_elapsed_ms(&raw);
    }

    /**
     * @brief Get the elapsed time in seconds since the timer was started.
     *
     * Returns the elapsed time as a double-precision floating point value
     * in seconds, derived from the monotonic clock.
     *
     * @return Elapsed time in seconds.
     */
    inline double elapsed_sec() const {
        return fossil_time_timer_elapsed_sec(&raw);
    }

    /**
     * @brief Measure elapsed time and reset the timer's start point.
     *
     * Returns the elapsed time in nanoseconds since the last start or lap,
     * and resets the timer's start point to the current time.
     * Useful for measuring intervals in loops or frames.
     *
     * @return Elapsed time in nanoseconds.
     */
    inline uint64_t lap_ns() {
        return fossil_time_timer_lap_ns(&raw);
    }

    /**
     * @brief Interpret a timing hint and return a duration in nanoseconds.
     *
     * Converts a string-based timing hint (e.g., "frame", "tick", "yield",
     * "human_short", "human_long") into a recommended duration in nanoseconds.
     * Useful for AI, scheduling, or human-centric timing.
     *
     * @param hint_id String identifier for the timing hint.
     * @return Duration in nanoseconds corresponding to the hint.
     */
    static inline uint64_t hint_ns(const char *hint_id) {
        return fossil_time_timer_hint_ns(hint_id);
    }
};

} /* namespace time */
} /* namespace fossil */
#endif

#endif /* FOSSIL_TIME_TIMER_H */
