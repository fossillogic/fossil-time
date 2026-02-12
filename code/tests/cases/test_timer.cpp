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

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_SUITE(cpp_timer_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_timer_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_timer_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Helper: sleep for at least the given number of nanoseconds (busy-wait)
static void busy_wait_ns(uint64_t ns) {
    fossil_time_timer_t t;
    fossil_time_timer_start(&t);
    while (fossil_time_timer_elapsed_ns(&t) < ns) { /* spin */ }
}

using fossil::time::Timer;

FOSSIL_TEST(cpp_test_timer_start_and_elapsed_ns) {
    Timer timer;
    timer.start();
    busy_wait_ns(1000000); // 1 ms
    uint64_t elapsed = timer.elapsed_ns();
    ASSUME_ITS_TRUE(elapsed >= 1000000 && elapsed < 10000000); // 1ms..10ms
}

FOSSIL_TEST(cpp_test_timer_elapsed_us_and_ms) {
    Timer timer;
    timer.start();
    busy_wait_ns(2500000); // 2.5 ms
    uint64_t us = timer.elapsed_us();
    uint64_t ms = timer.elapsed_ms();
    ASSUME_ITS_TRUE(us >= 2500 && us < 20000);
    ASSUME_ITS_TRUE(ms >= 2 && ms < 20);
}

FOSSIL_TEST(cpp_test_timer_elapsed_sec) {
    Timer timer;
    timer.start();
    busy_wait_ns(5000000); // 5 ms
    double sec = timer.elapsed_sec();
    ASSUME_ITS_TRUE(sec >= 0.005 && sec < 0.1);
}

FOSSIL_TEST(cpp_test_timer_lap_ns) {
    Timer timer;
    timer.start();
    busy_wait_ns(1000000); // 1 ms
    uint64_t lap1 = timer.lap_ns();
    busy_wait_ns(2000000); // 2 ms
    uint64_t lap2 = timer.lap_ns();
    ASSUME_ITS_TRUE(lap1 >= 1000000 && lap1 < 10000000);
    ASSUME_ITS_TRUE(lap2 >= 2000000 && lap2 < 20000000);
}

FOSSIL_TEST(cpp_test_timer_null_safety) {
    ASSUME_ITS_EQUAL_U64(fossil_time_timer_elapsed_ns(NULL), 0);
    ASSUME_ITS_EQUAL_U64(fossil_time_timer_elapsed_us(NULL), 0);
    ASSUME_ITS_EQUAL_U64(fossil_time_timer_elapsed_ms(NULL), 0);
    ASSUME_ITS_EQUAL_U64(fossil_time_timer_lap_ns(NULL), 0);
}

FOSSIL_TEST(cpp_test_timer_hint_ns) {
    ASSUME_ITS_EQUAL_U64(Timer::hint_ns("frame"), 16000000ULL);
    ASSUME_ITS_EQUAL_U64(Timer::hint_ns("tick"), 1000000ULL);
    ASSUME_ITS_EQUAL_U64(Timer::hint_ns("yield"), 0ULL);
    ASSUME_ITS_EQUAL_U64(Timer::hint_ns("human_short"), 100000000ULL);
    ASSUME_ITS_EQUAL_U64(Timer::hint_ns("human_long"), 500000000ULL);
    ASSUME_ITS_EQUAL_U64(Timer::hint_ns("moment"), 250000000ULL);
    ASSUME_ITS_EQUAL_U64(Timer::hint_ns("unknown"), 0ULL);
    ASSUME_ITS_EQUAL_U64(Timer::hint_ns(NULL), 0ULL);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_timer_tests) {
    FOSSIL_TEST_ADD(cpp_timer_suite, cpp_test_timer_start_and_elapsed_ns);
    FOSSIL_TEST_ADD(cpp_timer_suite, cpp_test_timer_elapsed_us_and_ms);
    FOSSIL_TEST_ADD(cpp_timer_suite, cpp_test_timer_elapsed_sec);
    FOSSIL_TEST_ADD(cpp_timer_suite, cpp_test_timer_lap_ns);
    FOSSIL_TEST_ADD(cpp_timer_suite, cpp_test_timer_null_safety);
    FOSSIL_TEST_ADD(cpp_timer_suite, cpp_test_timer_hint_ns);

    FOSSIL_TEST_REGISTER(cpp_timer_suite);
}
