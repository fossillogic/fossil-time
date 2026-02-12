/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * performance, cross-platform applications and libraries. The code contained
 * This file is part of the Fossil Logic project, which aims to develop high-
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include <fossil/pizza/framework.h>

#include "fossil/time/framework.h"

#include <string.h>
#include <stdint.h>
#include <time.h>

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_SUITE(cpp_sleep_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_sleep_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_sleep_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Helper: get current time in nanoseconds (monotonic if possible)
static uint64_t now_ns(void) {
#if defined(_WIN32)
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (uint64_t)((counter.QuadPart * 1000000000ULL) / freq.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#endif
}

// Helper: check that elapsed time is at least expected_ns (with some tolerance)
static void assume_sleep_elapsed(uint64_t start, uint64_t end, uint64_t expected_ns) {
    // Allow for some scheduling jitter (10%)
    uint64_t min_ns = expected_ns * 9 / 10;
    ASSUME_ITS_TRUE(end > start);
    ASSUME_ITS_TRUE((end - start) >= min_ns);
}

using fossil::time::Sleep;

// Test: Sleep::for_unit with various units
FOSSIL_TEST(cpp_test_sleep_units) {
    struct {
        uint64_t value;
        const char *unit;
        uint64_t expected_ns;
    } cases[] = {
        {1, "sec", 1000000000ULL},
        {10, "ms", 10000000ULL},
        {100, "us", 100000ULL},
        {500, "ns", 500ULL},
        {2000, "ps", 2ULL},
        {3000000, "fs", 3ULL},
        {4000000000ULL, "as", 4ULL},
        {5000000000000ULL, "zs", 5ULL},
        {6000000000000000ULL, "ys", 6ULL},
        {0, "sec", 0ULL},
        {1, "unknown", 0ULL}
    };

    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        uint64_t start = now_ns();
        Sleep::for_unit(cases[i].value, cases[i].unit);
        uint64_t end = now_ns();
        if (cases[i].expected_ns > 0)
            assume_sleep_elapsed(start, end, cases[i].expected_ns);
        else
            ASSUME_ITS_TRUE((end - start) < 1000000ULL); // Should not sleep
    }
}

// Test: Sleep::seconds
FOSSIL_TEST(cpp_test_sleep_seconds) {
    uint64_t start = now_ns();
    Sleep::seconds(1);
    uint64_t end = now_ns();
    assume_sleep_elapsed(start, end, 1000000000ULL);
}

// Test: Sleep::milliseconds
FOSSIL_TEST(cpp_test_sleep_milliseconds) {
    uint64_t start = now_ns();
    Sleep::milliseconds(50);
    uint64_t end = now_ns();
    assume_sleep_elapsed(start, end, 50000000ULL);
}

// Test: Sleep::microseconds
FOSSIL_TEST(cpp_test_sleep_microseconds) {
    uint64_t start = now_ns();
    Sleep::microseconds(1000);
    uint64_t end = now_ns();
    assume_sleep_elapsed(start, end, 1000000ULL);
}

// Test: Sleep::nanoseconds
FOSSIL_TEST(cpp_test_sleep_nanoseconds) {
    uint64_t start = now_ns();
    Sleep::nanoseconds(100000);
    uint64_t end = now_ns();
    assume_sleep_elapsed(start, end, 100000ULL);
}

// Test: Sleep::ai with hints
FOSSIL_TEST(cpp_test_sleep_ai_hints) {
    struct {
        const char *hint;
        uint64_t expected_ns;
    } cases[] = {
        {"moment", 1000000ULL},
        {"short", 10000000ULL},
        {"human_tick", 16666666ULL},
        {"frame", 16666666ULL},
        {"long", 1000000000ULL},
        {"yield", 0ULL},
        {"unknown", 0ULL},
        {NULL, 0ULL}
    };

    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        uint64_t start = now_ns();
        Sleep::ai(cases[i].hint);
        uint64_t end = now_ns();
        if (cases[i].expected_ns > 0)
            assume_sleep_elapsed(start, end, cases[i].expected_ns);
        else
            ASSUME_ITS_TRUE((end - start) < 1000000ULL); // Should not sleep
    }
}

// Test: Sleep::for_unit with zero and NULL
FOSSIL_TEST(cpp_test_sleep_zero_and_null) {
    uint64_t start = now_ns();
    Sleep::for_unit(0, "sec");
    Sleep::for_unit(1, NULL);
    Sleep::for_unit(0, NULL);
    uint64_t end = now_ns();
    ASSUME_ITS_TRUE((end - start) < 1000000ULL); // Should not sleep
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_sleep_tests) {
    FOSSIL_TEST_ADD(cpp_sleep_suite, cpp_test_sleep_units);
    FOSSIL_TEST_ADD(cpp_sleep_suite, cpp_test_sleep_seconds);
    FOSSIL_TEST_ADD(cpp_sleep_suite, cpp_test_sleep_milliseconds);
    FOSSIL_TEST_ADD(cpp_sleep_suite, cpp_test_sleep_microseconds);
    FOSSIL_TEST_ADD(cpp_sleep_suite, cpp_test_sleep_nanoseconds);
    FOSSIL_TEST_ADD(cpp_sleep_suite, cpp_test_sleep_ai_hints);
    FOSSIL_TEST_ADD(cpp_sleep_suite, cpp_test_sleep_zero_and_null);

    FOSSIL_TEST_REGISTER(cpp_sleep_suite);
}
