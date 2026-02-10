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
#include "fossil/time/timer.h"
#include <string.h>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <time.h>
#endif

/* ======================================================
 * Internal: monotonic clock (nanoseconds)
 * ====================================================== */

static uint64_t fossil_time_monotonic_now_ns(void) {

#if defined(_WIN32)

    static LARGE_INTEGER freq;
    static int initialized = 0;

    LARGE_INTEGER counter;

    if (!initialized) {
        QueryPerformanceFrequency(&freq);
        initialized = 1;
    }

    QueryPerformanceCounter(&counter);

    return (uint64_t)(
        (counter.QuadPart * 1000000000ULL) / freq.QuadPart
    );

#else

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint64_t)ts.tv_sec * 1000000000ULL +
           (uint64_t)ts.tv_nsec;

#endif
}

/* ======================================================
 * C API — Core
 * ====================================================== */

void fossil_time_timer_start(
    fossil_time_timer_t *timer
) {
    if (!timer) return;
    timer->start_ns = fossil_time_monotonic_now_ns();
}

uint64_t fossil_time_timer_elapsed_ns(
    const fossil_time_timer_t *timer
) {
    if (!timer) return 0;

    uint64_t now = fossil_time_monotonic_now_ns();
    return now - timer->start_ns;
}

uint64_t fossil_time_timer_elapsed_us(
    const fossil_time_timer_t *timer
) {
    return fossil_time_timer_elapsed_ns(timer) / 1000ULL;
}

uint64_t fossil_time_timer_elapsed_ms(
    const fossil_time_timer_t *timer
) {
    return fossil_time_timer_elapsed_ns(timer) / 1000000ULL;
}

double fossil_time_timer_elapsed_sec(
    const fossil_time_timer_t *timer
) {
    return (double)fossil_time_timer_elapsed_ns(timer) / 1e9;
}

/* ======================================================
 * C API — Convenience
 * ====================================================== */

uint64_t fossil_time_timer_lap_ns(
    fossil_time_timer_t *timer
) {
    if (!timer) return 0;

    uint64_t now = fossil_time_monotonic_now_ns();
    uint64_t elapsed = now - timer->start_ns;

    timer->start_ns = now;
    return elapsed;
}

/* ======================================================
 * C API — AI / Hint-Based Timing
 * ====================================================== */

uint64_t fossil_time_timer_hint_ns(
    const char *hint_id
) {
    if (!hint_id) return 0;

    /* Frame / scheduling oriented */
    if (strcmp(hint_id, "frame") == 0)
        return 16ULL * 1000000ULL;      /* ~60 FPS */

    if (strcmp(hint_id, "tick") == 0)
        return 1ULL * 1000000ULL;       /* 1 ms */

    if (strcmp(hint_id, "yield") == 0)
        return 0ULL;                    /* cooperative yield */

    /* Human-centric */
    if (strcmp(hint_id, "human_short") == 0)
        return 100ULL * 1000000ULL;     /* 100 ms */

    if (strcmp(hint_id, "human_long") == 0)
        return 500ULL * 1000000ULL;     /* 500 ms */

    if (strcmp(hint_id, "moment") == 0)
        return 250ULL * 1000000ULL;

    /* Unknown hint */
    return 0;
}
