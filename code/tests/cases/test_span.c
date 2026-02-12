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
FOSSIL_SUITE(c_span_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_span_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_span_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Helper: create a fossil_time_span_t with given fields and mask
static fossil_time_span_t make_span(
    int64_t days, int32_t hours, int32_t minutes, int32_t seconds,
    int32_t ms, int32_t us, int32_t ns, uint64_t mask
) {
    fossil_time_span_t span;
    memset(&span, 0, sizeof(span));
    span.days = days;
    span.hours = hours;
    span.minutes = minutes;
    span.seconds = seconds;
    span.milliseconds = ms;
    span.microseconds = us;
    span.nanoseconds = ns;
    span.precision_mask = mask;
    return span;
}

// Test: fossil_time_span_clear zeros all fields and mask
FOSSIL_TEST(c_test_span_clear) {
    fossil_time_span_t span = make_span(1, 2, 3, 4, 5, 6, 7, 0xFFFFFFFF);
    fossil_time_span_clear(&span);
    ASSUME_ITS_EQUAL_I64(span.days, 0);
    ASSUME_ITS_EQUAL_I32(span.hours, 0);
    ASSUME_ITS_EQUAL_I32(span.minutes, 0);
    ASSUME_ITS_EQUAL_I32(span.seconds, 0);
    ASSUME_ITS_EQUAL_I32(span.milliseconds, 0);
    ASSUME_ITS_EQUAL_I32(span.microseconds, 0);
    ASSUME_ITS_EQUAL_I32(span.nanoseconds, 0);
    ASSUME_ITS_EQUAL_I64(span.precision_mask, 0);
}

// Test: fossil_time_span_validate detects valid and invalid spans
FOSSIL_TEST(c_test_span_validate) {
    fossil_time_span_t span = make_span(0, 23, 59, 59, 0, 0, 0,
        FOSSIL_TIME_SPAN_PRECISION_HOURS | FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    ASSUME_ITS_TRUE(fossil_time_span_validate(&span));
    span.hours = 24;
    ASSUME_ITS_FALSE(fossil_time_span_validate(&span));
    span.hours = 0; span.minutes = 60;
    ASSUME_ITS_FALSE(fossil_time_span_validate(&span));
    span.minutes = 0; span.seconds = 60;
    ASSUME_ITS_FALSE(fossil_time_span_validate(&span));
}

// Test: fossil_time_span_normalize carries overflow up the SI ladder
FOSSIL_TEST(c_test_span_normalize) {
    fossil_time_span_t span = make_span(0, 23, 59, 59, 999, 999, 1001,
        FOSSIL_TIME_SPAN_PRECISION_HOURS | FOSSIL_TIME_SPAN_PRECISION_MINUTES |
        FOSSIL_TIME_SPAN_PRECISION_SECONDS | FOSSIL_TIME_SPAN_PRECISION_MILLI |
        FOSSIL_TIME_SPAN_PRECISION_MICRO | FOSSIL_TIME_SPAN_PRECISION_NANO);
    fossil_time_span_normalize(&span);
    // 1001 ns = 1 us + 1 ns, 999 us + 1 us = 1000 us = 1 ms, etc.
    ASSUME_ITS_EQUAL_I32(span.nanoseconds, 1);
    ASSUME_ITS_EQUAL_I32(span.microseconds, 0);
    ASSUME_ITS_EQUAL_I32(span.milliseconds, 0);
    ASSUME_ITS_EQUAL_I32(span.seconds, 0);
    ASSUME_ITS_EQUAL_I32(span.minutes, 0);
    ASSUME_ITS_EQUAL_I32(span.hours, 0);
    ASSUME_ITS_EQUAL_I64(span.days, 1);
}

// Test: fossil_time_span_from_unit sets correct field and mask
FOSSIL_TEST(c_test_span_from_unit) {
    fossil_time_span_t span;
    fossil_time_span_from_unit(&span, 5, "days");
    ASSUME_ITS_EQUAL_I64(span.days, 5);
    ASSUME_ITS_EQUAL_I64(span.precision_mask, FOSSIL_TIME_SPAN_PRECISION_DAYS);

    fossil_time_span_from_unit(&span, 12, "hours");
    ASSUME_ITS_EQUAL_I32(span.hours, 12);
    ASSUME_ITS_EQUAL_I64(span.precision_mask, FOSSIL_TIME_SPAN_PRECISION_HOURS);

    fossil_time_span_from_unit(&span, 42, "ms");
    ASSUME_ITS_EQUAL_I32(span.milliseconds, 42);
    ASSUME_ITS_EQUAL_I64(span.precision_mask, FOSSIL_TIME_SPAN_PRECISION_MILLI);

    fossil_time_span_from_unit(&span, 7, "ys");
    ASSUME_ITS_EQUAL_I32(span.yoctoseconds, 7);
    ASSUME_ITS_EQUAL_I64(span.precision_mask, FOSSIL_TIME_SPAN_PRECISION_YOCTO);
}

// Test: fossil_time_span_from_ai sets correct values for hints
FOSSIL_TEST(c_test_span_from_ai) {
    fossil_time_span_t span;
    fossil_time_span_from_ai(&span, "moment");
    ASSUME_ITS_EQUAL_I32(span.milliseconds, 250);
    ASSUME_ITS_EQUAL_I64(span.precision_mask, FOSSIL_TIME_SPAN_PRECISION_MILLI);

    fossil_time_span_from_ai(&span, "short");
    ASSUME_ITS_EQUAL_I32(span.seconds, 1);
    ASSUME_ITS_EQUAL_I64(span.precision_mask, FOSSIL_TIME_SPAN_PRECISION_SECONDS);

    fossil_time_span_from_ai(&span, "frame");
    ASSUME_ITS_EQUAL_I32(span.milliseconds, 16);
    ASSUME_ITS_EQUAL_I64(span.precision_mask, FOSSIL_TIME_SPAN_PRECISION_MILLI);

    fossil_time_span_from_ai(&span, "human_tick");
    ASSUME_ITS_EQUAL_I32(span.milliseconds, 100);
    ASSUME_ITS_EQUAL_I64(span.precision_mask, FOSSIL_TIME_SPAN_PRECISION_MILLI);
}

// Test: fossil_time_span_add and fossil_time_span_sub
FOSSIL_TEST(c_test_span_add_sub) {
    fossil_time_span_t a = make_span(1, 2, 3, 4, 5, 6, 7,
        FOSSIL_TIME_SPAN_PRECISION_DAYS | FOSSIL_TIME_SPAN_PRECISION_HOURS |
        FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS |
        FOSSIL_TIME_SPAN_PRECISION_MILLI | FOSSIL_TIME_SPAN_PRECISION_MICRO | FOSSIL_TIME_SPAN_PRECISION_NANO);
    fossil_time_span_t b = make_span(2, 3, 4, 5, 6, 7, 8,
        a.precision_mask);
    fossil_time_span_t result;
    fossil_time_span_add(&result, &a, &b);
    ASSUME_ITS_EQUAL_I64(result.days, 3);
    ASSUME_ITS_EQUAL_I32(result.hours, 5);
    ASSUME_ITS_EQUAL_I32(result.minutes, 7);
    ASSUME_ITS_EQUAL_I32(result.seconds, 9);
    ASSUME_ITS_EQUAL_I32(result.milliseconds, 11);
    ASSUME_ITS_EQUAL_I32(result.microseconds, 13);
    ASSUME_ITS_EQUAL_I32(result.nanoseconds, 15);

    fossil_time_span_sub(&result, &a, &b);
    ASSUME_ITS_EQUAL_I64(result.days, -1);
    ASSUME_ITS_EQUAL_I32(result.hours, -1);
    ASSUME_ITS_EQUAL_I32(result.minutes, -1);
    ASSUME_ITS_EQUAL_I32(result.seconds, -1);
    ASSUME_ITS_EQUAL_I32(result.milliseconds, -1);
    ASSUME_ITS_EQUAL_I32(result.microseconds, -1);
    ASSUME_ITS_EQUAL_I32(result.nanoseconds, -1);
}

// Test: fossil_time_span_to_seconds aggregates fields
FOSSIL_TEST(c_test_span_to_seconds) {
    fossil_time_span_t span = make_span(1, 1, 1, 1, 0, 0, 0,
        FOSSIL_TIME_SPAN_PRECISION_DAYS | FOSSIL_TIME_SPAN_PRECISION_HOURS |
        FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    int64_t expected = 1 * 86400 + 1 * 3600 + 1 * 60 + 1;
    ASSUME_ITS_EQUAL_I64(fossil_time_span_to_seconds(&span), expected);

    // Only seconds
    span = make_span(0, 0, 0, 42, 0, 0, 0, FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    ASSUME_ITS_EQUAL_I64(fossil_time_span_to_seconds(&span), 42);
}

// Test: fossil_time_span_to_nanoseconds aggregates fields
FOSSIL_TEST(c_test_span_to_nanoseconds) {
    fossil_time_span_t span = make_span(0, 0, 0, 1, 2, 3, 4,
        FOSSIL_TIME_SPAN_PRECISION_SECONDS | FOSSIL_TIME_SPAN_PRECISION_MILLI |
        FOSSIL_TIME_SPAN_PRECISION_MICRO | FOSSIL_TIME_SPAN_PRECISION_NANO);
    int64_t expected = 1 * 1000000000LL + 2 * 1000000LL + 3 * 1000LL + 4;
    ASSUME_ITS_EQUAL_I64(fossil_time_span_to_nanoseconds(&span), expected);
}

// Test: fossil_time_span_format for "short" and "human"
FOSSIL_TEST(c_test_span_format) {
    fossil_time_span_t span = make_span(1, 2, 3, 4, 0, 0, 0,
        FOSSIL_TIME_SPAN_PRECISION_DAYS | FOSSIL_TIME_SPAN_PRECISION_HOURS |
        FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    char buf[64];
    fossil_time_span_format(&span, buf, sizeof(buf), "short");
    ASSUME_ITS_TRUE(strstr(buf, "1d") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "2h") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "3m") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "4s") != NULL);

    fossil_time_span_format(&span, buf, sizeof(buf), "human");
    ASSUME_ITS_TRUE(strstr(buf, "1 days") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "2 hours") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "3 minutes") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "4 seconds") != NULL);

    // Unknown format
    int rc = fossil_time_span_format(&span, buf, sizeof(buf), "unknown");
    ASSUME_ITS_TRUE(rc < 0);
}

// Test: fossil_time_span_format handles buffer too small
FOSSIL_TEST(c_test_span_format_buffer_small) {
    fossil_time_span_t span = make_span(1, 2, 3, 4, 0, 0, 0,
        FOSSIL_TIME_SPAN_PRECISION_DAYS | FOSSIL_TIME_SPAN_PRECISION_HOURS |
        FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    char buf[4];
    int rc = fossil_time_span_format(&span, buf, sizeof(buf), "short");
    ASSUME_ITS_TRUE(rc > 0); // snprintf returns number of chars that would have been written
    buf[sizeof(buf)-1] = 0;
    ASSUME_ITS_TRUE(buf[0] != 0); // At least something written
}

// Test: fossil_time_span_format handles nulls and zero buffer
FOSSIL_TEST(c_test_span_format_nulls) {
    fossil_time_span_t span = make_span(1, 0, 0, 0, 0, 0, 0, FOSSIL_TIME_SPAN_PRECISION_DAYS);
    char buf[8];
    int rc = fossil_time_span_format(NULL, buf, sizeof(buf), "short");
    ASSUME_ITS_EQUAL_I32(rc, -1);
    rc = fossil_time_span_format(&span, NULL, 8, "short");
    ASSUME_ITS_EQUAL_I32(rc, -1);
    rc = fossil_time_span_format(&span, buf, 0, "short");
    ASSUME_ITS_EQUAL_I32(rc, -1);
    rc = fossil_time_span_format(&span, buf, sizeof(buf), NULL);
    ASSUME_ITS_EQUAL_I32(rc, -1);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_span_tests) {
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_clear);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_validate);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_normalize);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_from_unit);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_from_ai);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_add_sub);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_to_seconds);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_to_nanoseconds);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_format);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_format_buffer_small);
    FOSSIL_TEST_ADD(c_span_suite, c_test_span_format_nulls);

    FOSSIL_TEST_REGISTER(c_span_suite);
}
