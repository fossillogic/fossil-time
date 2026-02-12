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
FOSSIL_SUITE(cpp_span_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_span_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_span_suite) {
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

using fossil::time::Span;

// Test: Span::clear zeros all fields and mask
FOSSIL_TEST(cpp_test_span_clear) {
    Span span;
    span.raw = make_span(1, 2, 3, 4, 5, 6, 7, 0xFFFFFFFF);
    span.clear();
    ASSUME_ITS_EQUAL_I64(span.raw.days, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.hours, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.minutes, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.seconds, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.milliseconds, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.microseconds, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.nanoseconds, 0);
    ASSUME_ITS_EQUAL_I64(span.raw.precision_mask, 0);
}

// Test: Span::validate detects valid and invalid spans
FOSSIL_TEST(cpp_test_span_validate) {
    Span span;
    span.raw = make_span(0, 23, 59, 59, 0, 0, 0,
        FOSSIL_TIME_SPAN_PRECISION_HOURS | FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    ASSUME_ITS_TRUE(span.validate());
    span.raw.hours = 24;
    ASSUME_ITS_FALSE(span.validate());
    span.raw.hours = 0; span.raw.minutes = 60;
    ASSUME_ITS_FALSE(span.validate());
    span.raw.minutes = 0; span.raw.seconds = 60;
    ASSUME_ITS_FALSE(span.validate());
}

// Test: Span::normalize carries overflow up the SI ladder
FOSSIL_TEST(cpp_test_span_normalize) {
    Span span;
    span.raw = make_span(0, 23, 59, 59, 999, 999, 1001,
        FOSSIL_TIME_SPAN_PRECISION_HOURS | FOSSIL_TIME_SPAN_PRECISION_MINUTES |
        FOSSIL_TIME_SPAN_PRECISION_SECONDS | FOSSIL_TIME_SPAN_PRECISION_MILLI |
        FOSSIL_TIME_SPAN_PRECISION_MICRO | FOSSIL_TIME_SPAN_PRECISION_NANO);
    span.normalize();
    ASSUME_ITS_EQUAL_I32(span.raw.nanoseconds, 1);
    ASSUME_ITS_EQUAL_I32(span.raw.microseconds, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.milliseconds, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.seconds, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.minutes, 0);
    ASSUME_ITS_EQUAL_I32(span.raw.hours, 0);
    ASSUME_ITS_EQUAL_I64(span.raw.days, 1);
}

// Test: Span::from_unit sets correct field and mask
FOSSIL_TEST(cpp_test_span_from_unit) {
    Span span;
    span.from_unit(5, "days");
    ASSUME_ITS_EQUAL_I64(span.raw.days, 5);
    ASSUME_ITS_EQUAL_I64(span.raw.precision_mask, FOSSIL_TIME_SPAN_PRECISION_DAYS);

    span.from_unit(12, "hours");
    ASSUME_ITS_EQUAL_I32(span.raw.hours, 12);
    ASSUME_ITS_EQUAL_I64(span.raw.precision_mask, FOSSIL_TIME_SPAN_PRECISION_HOURS);

    span.from_unit(42, "ms");
    ASSUME_ITS_EQUAL_I32(span.raw.milliseconds, 42);
    ASSUME_ITS_EQUAL_I64(span.raw.precision_mask, FOSSIL_TIME_SPAN_PRECISION_MILLI);

    span.from_unit(7, "ys");
    ASSUME_ITS_EQUAL_I32(span.raw.yoctoseconds, 7);
    ASSUME_ITS_EQUAL_I64(span.raw.precision_mask, FOSSIL_TIME_SPAN_PRECISION_YOCTO);
}

// Test: Span::from_ai sets correct values for hints
FOSSIL_TEST(cpp_test_span_from_ai) {
    Span span;
    span.from_ai("moment");
    ASSUME_ITS_EQUAL_I32(span.raw.milliseconds, 250);
    ASSUME_ITS_EQUAL_I64(span.raw.precision_mask, FOSSIL_TIME_SPAN_PRECISION_MILLI);

    span.from_ai("short");
    ASSUME_ITS_EQUAL_I32(span.raw.seconds, 1);
    ASSUME_ITS_EQUAL_I64(span.raw.precision_mask, FOSSIL_TIME_SPAN_PRECISION_SECONDS);

    span.from_ai("frame");
    ASSUME_ITS_EQUAL_I32(span.raw.milliseconds, 16);
    ASSUME_ITS_EQUAL_I64(span.raw.precision_mask, FOSSIL_TIME_SPAN_PRECISION_MILLI);

    span.from_ai("human_tick");
    ASSUME_ITS_EQUAL_I32(span.raw.milliseconds, 100);
    ASSUME_ITS_EQUAL_I64(span.raw.precision_mask, FOSSIL_TIME_SPAN_PRECISION_MILLI);
}

// Test: Span::add and Span::sub
FOSSIL_TEST(cpp_test_span_add_sub) {
    Span a, b;
    a.raw = make_span(1, 2, 3, 4, 5, 6, 7,
        FOSSIL_TIME_SPAN_PRECISION_DAYS | FOSSIL_TIME_SPAN_PRECISION_HOURS |
        FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS |
        FOSSIL_TIME_SPAN_PRECISION_MILLI | FOSSIL_TIME_SPAN_PRECISION_MICRO | FOSSIL_TIME_SPAN_PRECISION_NANO);
    b.raw = make_span(2, 3, 4, 5, 6, 7, 8, a.raw.precision_mask);
    Span result = Span::add(a, b);
    ASSUME_ITS_EQUAL_I64(result.raw.days, 3);
    ASSUME_ITS_EQUAL_I32(result.raw.hours, 5);
    ASSUME_ITS_EQUAL_I32(result.raw.minutes, 7);
    ASSUME_ITS_EQUAL_I32(result.raw.seconds, 9);
    ASSUME_ITS_EQUAL_I32(result.raw.milliseconds, 11);
    ASSUME_ITS_EQUAL_I32(result.raw.microseconds, 13);
    ASSUME_ITS_EQUAL_I32(result.raw.nanoseconds, 15);

    result = Span::sub(a, b);
    ASSUME_ITS_EQUAL_I64(result.raw.days, -1);
    ASSUME_ITS_EQUAL_I32(result.raw.hours, -1);
    ASSUME_ITS_EQUAL_I32(result.raw.minutes, -1);
    ASSUME_ITS_EQUAL_I32(result.raw.seconds, -1);
    ASSUME_ITS_EQUAL_I32(result.raw.milliseconds, -1);
    ASSUME_ITS_EQUAL_I32(result.raw.microseconds, -1);
    ASSUME_ITS_EQUAL_I32(result.raw.nanoseconds, -1);
}

// Test: Span::to_seconds aggregates fields
FOSSIL_TEST(cpp_test_span_to_seconds) {
    Span span;
    span.raw = make_span(1, 1, 1, 1, 0, 0, 0,
        FOSSIL_TIME_SPAN_PRECISION_DAYS | FOSSIL_TIME_SPAN_PRECISION_HOURS |
        FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    int64_t expected = 1 * 86400 + 1 * 3600 + 1 * 60 + 1;
    ASSUME_ITS_EQUAL_I64(span.to_seconds(), expected);

    span.raw = make_span(0, 0, 0, 42, 0, 0, 0, FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    ASSUME_ITS_EQUAL_I64(span.to_seconds(), 42);
}

// Test: Span::to_nanoseconds aggregates fields
FOSSIL_TEST(cpp_test_span_to_nanoseconds) {
    Span span;
    span.raw = make_span(0, 0, 0, 1, 2, 3, 4,
        FOSSIL_TIME_SPAN_PRECISION_SECONDS | FOSSIL_TIME_SPAN_PRECISION_MILLI |
        FOSSIL_TIME_SPAN_PRECISION_MICRO | FOSSIL_TIME_SPAN_PRECISION_NANO);
    int64_t expected = 1 * 1000000000LL + 2 * 1000000LL + 3 * 1000LL + 4;
    ASSUME_ITS_EQUAL_I64(span.to_nanoseconds(), expected);
}

// Test: Span::format for "short" and "human"
FOSSIL_TEST(cpp_test_span_format) {
    Span span;
    span.raw = make_span(1, 2, 3, 4, 0, 0, 0,
        FOSSIL_TIME_SPAN_PRECISION_DAYS | FOSSIL_TIME_SPAN_PRECISION_HOURS |
        FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    char buf[64];
    span.format(buf, sizeof(buf), "short");
    ASSUME_ITS_TRUE(strstr(buf, "1d") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "2h") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "3m") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "4s") != NULL);

    span.format(buf, sizeof(buf), "human");
    ASSUME_ITS_TRUE(strstr(buf, "1 days") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "2 hours") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "3 minutes") != NULL);
    ASSUME_ITS_TRUE(strstr(buf, "4 seconds") != NULL);

    int rc = span.format(buf, sizeof(buf), "unknown");
    ASSUME_ITS_TRUE(rc < 0);
}

// Test: Span::format handles buffer too small
FOSSIL_TEST(cpp_test_span_format_buffer_small) {
    Span span;
    span.raw = make_span(1, 2, 3, 4, 0, 0, 0,
        FOSSIL_TIME_SPAN_PRECISION_DAYS | FOSSIL_TIME_SPAN_PRECISION_HOURS |
        FOSSIL_TIME_SPAN_PRECISION_MINUTES | FOSSIL_TIME_SPAN_PRECISION_SECONDS);
    char buf[4];
    int rc = span.format(buf, sizeof(buf), "short");
    ASSUME_ITS_TRUE(rc > 0);
    buf[sizeof(buf)-1] = 0;
    ASSUME_ITS_TRUE(buf[0] != 0);
}

// Test: Span::format handles nulls and zero buffer
FOSSIL_TEST(cpp_test_span_format_nulls) {
    Span span;
    span.raw = make_span(1, 0, 0, 0, 0, 0, 0, FOSSIL_TIME_SPAN_PRECISION_DAYS);
    char buf[8];
    int rc = span.format(buf, sizeof(buf), "short");
    ASSUME_ITS_TRUE(rc > 0);

    rc = fossil_time_span_format(NULL, buf, sizeof(buf), "short");
    ASSUME_ITS_EQUAL_I32(rc, -1);
    rc = fossil_time_span_format(&span.raw, NULL, 8, "short");
    ASSUME_ITS_EQUAL_I32(rc, -1);
    rc = fossil_time_span_format(&span.raw, buf, 0, "short");
    ASSUME_ITS_EQUAL_I32(rc, -1);
    rc = fossil_time_span_format(&span.raw, buf, sizeof(buf), NULL);
    ASSUME_ITS_EQUAL_I32(rc, -1);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_span_tests) {
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_clear);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_validate);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_normalize);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_from_unit);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_from_ai);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_add_sub);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_to_seconds);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_to_nanoseconds);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_format);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_format_buffer_small);
    FOSSIL_TEST_ADD(cpp_span_suite, cpp_test_span_format_nulls);

    FOSSIL_TEST_REGISTER(cpp_span_suite);
}
