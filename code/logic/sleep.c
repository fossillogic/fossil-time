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
#include "fossil/time/sleep.h"
#include <string.h>

#if defined(_WIN32)
#  include <windows.h>
#else
#  include <time.h>
#  include <unistd.h>
#endif

/* ======================================================
 * Internal helpers
 * ====================================================== */

static void sleep_nanoseconds_internal(uint64_t ns) {
#if defined(_WIN32)
    /* Windows sleep granularity is milliseconds */
    DWORD ms = (DWORD)(ns / 1000000ULL);
    if (ms == 0 && ns > 0)
        ms = 1;
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec  = (time_t)(ns / 1000000000ULL);
    ts.tv_nsec = (long)(ns % 1000000000ULL);
    nanosleep(&ts, NULL);
#endif
}

static uint64_t unit_to_nanoseconds(
    uint64_t value,
    const char *unit_id
) {
    if (!unit_id)
        return 0;

    if (!strcmp(unit_id, "sec"))
        return value * 1000000000ULL;
    if (!strcmp(unit_id, "ms"))
        return value * 1000000ULL;
    if (!strcmp(unit_id, "us"))
        return value * 1000ULL;
    if (!strcmp(unit_id, "ns"))
        return value;

    /* Sub-nanosecond units collapse to nanoseconds */
    if (!strcmp(unit_id, "ps"))
        return value / 1000ULL;
    if (!strcmp(unit_id, "fs"))
        return value / 1000000ULL;
    if (!strcmp(unit_id, "as"))
        return value / 1000000000ULL;
    if (!strcmp(unit_id, "zs"))
        return value / 1000000000000ULL;
    if (!strcmp(unit_id, "ys"))
        return value / 1000000000000000ULL;

    return 0;
}

static uint64_t hint_to_nanoseconds(const char *hint_id) {
    if (!hint_id)
        return 0;

    if (!strcmp(hint_id, "yield"))
        return 0;

    if (!strcmp(hint_id, "moment"))
        return 1000000ULL;        /* 1 ms */

    if (!strcmp(hint_id, "short"))
        return 10000000ULL;       /* 10 ms */

    if (!strcmp(hint_id, "human_tick"))
        return 16666666ULL;       /* ~60 Hz */

    if (!strcmp(hint_id, "frame"))
        return 16666666ULL;       /* ~60 FPS */

    if (!strcmp(hint_id, "long"))
        return 1000000000ULL;     /* 1 s */

    return 0;
}

/* ======================================================
 * C API — Sleep
 * ====================================================== */

void fossil_time_sleep(
    uint64_t value,
    const char *unit_id
) {
    uint64_t ns = unit_to_nanoseconds(value, unit_id);
    if (ns > 0)
        sleep_nanoseconds_internal(ns);
}

void fossil_time_sleep_seconds(
    uint64_t seconds
) {
    fossil_time_sleep(seconds, "sec");
}

void fossil_time_sleep_milliseconds(
    uint64_t milliseconds
) {
    fossil_time_sleep(milliseconds, "ms");
}

void fossil_time_sleep_microseconds(
    uint64_t microseconds
) {
    fossil_time_sleep(microseconds, "us");
}

void fossil_time_sleep_nanoseconds(
    uint64_t nanoseconds
) {
    sleep_nanoseconds_internal(nanoseconds);
}

void fossil_time_sleep_ai(
    const char *hint_id
) {
    uint64_t ns = hint_to_nanoseconds(hint_id);

    if (ns > 0)
        sleep_nanoseconds_internal(ns);
}
