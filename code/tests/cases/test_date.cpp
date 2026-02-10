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
FOSSIL_SUITE(cpp_date_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_date_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_date_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

using fossil::time::Date;

// Helper: create a Date with given fields and mask
static Date make_cpp_date(
    int year, int month, int day, int hour, int min, int sec,
    int ms, int us, int ns, uint64_t mask
) {
    Date d;
    memset(&d.raw, 0, sizeof(d.raw));
    d.raw.year = year;
    d.raw.month = month;
    d.raw.day = day;
    d.raw.hour = hour;
    d.raw.minute = min;
    d.raw.second = sec;
    d.raw.millisecond = ms;
    d.raw.microsecond = us;
    d.raw.nanosecond = ns;
    d.raw.precision_mask = mask;
    return d;
}

// Test: Default constructor initializes to zero
FOSSIL_TEST(cpp_test_date_default_ctor) {
    Date d;
    ASSUME_ITS_EQUAL_I32(d.raw.year, 0);
    ASSUME_ITS_EQUAL_I32(d.raw.month, 0);
    ASSUME_ITS_EQUAL_I32(d.raw.day, 0);
    ASSUME_ITS_EQUAL_I32(d.raw.precision_mask, 0);
}

// Test: now() populates fields and precision_mask
FOSSIL_TEST(cpp_test_date_now_method) {
    Date d;
    d.now();
    ASSUME_ITS_TRUE(d.raw.year > 1970 && d.raw.year < 3000);
    ASSUME_ITS_TRUE(d.raw.month >= 1 && d.raw.month <= 12);
    ASSUME_ITS_TRUE(d.raw.day >= 1 && d.raw.day <= 31);
    uint64_t mask = FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH |
                    FOSSIL_TIME_PRECISION_DAY | FOSSIL_TIME_PRECISION_HOUR |
                    FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND;
    ASSUME_ITS_TRUE((d.raw.precision_mask & mask) == mask);
}

// Test: validate() returns true for valid, false for invalid
FOSSIL_TEST(cpp_test_date_validate_method) {
    Date d = make_cpp_date(2024, 2, 29, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    ASSUME_ITS_TRUE(d.validate());
    d.raw.day = 30;
    ASSUME_ITS_FALSE(d.validate());
}

// Test: normalize() sets weekday and yearday
FOSSIL_TEST(cpp_test_date_normalize_method) {
    Date d = make_cpp_date(2024, 6, 1, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    d.normalize();
    ASSUME_ITS_EQUAL_I32(d.raw.weekday, 6);
    ASSUME_ITS_EQUAL_I32(d.raw.yearday, 153);
}

// Test: compare() returns -1, 0, 1 as expected
FOSSIL_TEST(cpp_test_date_compare_method) {
    Date a = make_cpp_date(2024, 6, 1, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY | FOSSIL_TIME_PRECISION_HOUR);
    Date b = make_cpp_date(2024, 6, 1, 13, 0, 0, 0, 0, 0, a.raw.precision_mask);
    ASSUME_ITS_EQUAL_I32(a.compare(b), -1);
    ASSUME_ITS_EQUAL_I32(b.compare(a), 1);
    ASSUME_ITS_EQUAL_I32(a.compare(a), 0);
}

// Test: diff_seconds() returns correct difference
FOSSIL_TEST(cpp_test_date_diff_seconds_method) {
    Date a = make_cpp_date(2024, 6, 1, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY | FOSSIL_TIME_PRECISION_HOUR);
    Date b = make_cpp_date(2024, 6, 1, 11, 0, 0, 0, 0, 0, a.raw.precision_mask);
    int64_t diff = a.diff_seconds(b);
    ASSUME_ITS_EQUAL_I32(diff, 3600);
}

// Test: to_unix_seconds() and from_unix_seconds()
FOSSIL_TEST(cpp_test_date_to_from_unix_seconds_method) {
    Date d = make_cpp_date(2024, 6, 1, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    d.raw.precision_mask |= FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND;
    int64_t unix_sec = d.to_unix_seconds();
    Date d2;
    d2.from_unix_seconds(unix_sec);
    ASSUME_ITS_EQUAL_I32(d2.raw.year, 2024);
    ASSUME_ITS_EQUAL_I32(d2.raw.month, 6);
    ASSUME_ITS_EQUAL_I32(d2.raw.day, 1);
    ASSUME_ITS_EQUAL_I32(d2.raw.hour, 0);
    ASSUME_ITS_EQUAL_I32(d2.raw.minute, 0);
    ASSUME_ITS_EQUAL_I32(d2.raw.second, 0);
}

// Test: to_unix_nanoseconds()
FOSSIL_TEST(cpp_test_date_to_unix_nanoseconds_method) {
    Date d = make_cpp_date(1970, 1, 1, 0, 0, 1, 123, 456, 789,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND |
        FOSSIL_TIME_PRECISION_MILLI | FOSSIL_TIME_PRECISION_MICRO | FOSSIL_TIME_PRECISION_NANO);
    int64_t ns = d.to_unix_nanoseconds();
    int64_t expected = 1 * 1000000000LL + 123 * 1000000LL + 456 * 1000LL + 789;
    ASSUME_ITS_EQUAL_I64(ns, expected);
}

// Test: format() with various formats
FOSSIL_TEST(cpp_test_date_format_method) {
    Date d = make_cpp_date(2024, 6, 1, 12, 34, 56, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND);
    char buf[64];
    int n = d.format(buf, sizeof(buf), "iso");
    ASSUME_ITS_EQUAL_CSTR(buf, "2024-06-01T12:34:56Z");
    n = d.format(buf, sizeof(buf), "log");
    ASSUME_ITS_EQUAL_CSTR(buf, "20240601-123456");
    n = d.format(buf, sizeof(buf), "unknown");
    ASSUME_ITS_EQUAL_CSTR(buf, "invalid_date");
}

// Test: format_smart() and format_relative()
FOSSIL_TEST(cpp_test_date_format_smart_relative_method) {
    Date now = make_cpp_date(2024, 6, 1, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND);
    Date d = now;
    char buf[64];
    int n = d.format_smart(now, buf, sizeof(buf));
    ASSUME_ITS_EQUAL_CSTR(buf, "now");
    d.raw.second += 10;
    d.normalize();
    n = d.format_smart(now, buf, sizeof(buf));
    ASSUME_ITS_TRUE(strstr(buf, "in 10 seconds") != NULL);
    d = now;
    d.raw.second -= 5;
    d.normalize();
    n = d.format_relative(now, buf, sizeof(buf));
    ASSUME_ITS_TRUE(strstr(buf, "5 seconds ago") != NULL);
}

// Test: search() with queries
FOSSIL_TEST(cpp_test_date_search_method) {
    Date now = make_cpp_date(2024, 6, 1, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND);
    Date d = now;
    d.normalize();
    ASSUME_ITS_TRUE(d.search(now, "today"));
    d.raw.weekday = 6; // Saturday
    ASSUME_ITS_TRUE(d.search(now, "weekend"));
    d.raw.weekday = 2; // Tuesday
    ASSUME_ITS_FALSE(d.search(now, "weekend"));
}

// Test: round-trip conversion with sub-second precision
FOSSIL_TEST(cpp_test_date_roundtrip_subsecond) {
    Date d = make_cpp_date(2024, 6, 1, 12, 34, 56, 789, 123, 456,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND |
        FOSSIL_TIME_PRECISION_MILLI | FOSSIL_TIME_PRECISION_MICRO | FOSSIL_TIME_PRECISION_NANO);
    int64_t ns = d.to_unix_nanoseconds();
    Date d2;
    d2.from_unix_seconds(d.to_unix_seconds());
    d2.normalize();
    ASSUME_ITS_EQUAL_I32(d2.raw.year, 2024);
    ASSUME_ITS_EQUAL_I32(d2.raw.month, 6);
    ASSUME_ITS_EQUAL_I32(d2.raw.day, 1);
}

// Test: format() with "human" and "short" formats
FOSSIL_TEST(cpp_test_date_format_human_short) {
    Date d = make_cpp_date(2024, 6, 1, 12, 34, 56, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND);
    char buf[64];
    int n = d.format(buf, sizeof(buf), "human");
    ASSUME_ITS_TRUE(n > 0);
    n = d.format(buf, sizeof(buf), "short");
    ASSUME_ITS_TRUE(n > 0);
}

// Test: search() with "within" queries
FOSSIL_TEST(cpp_test_date_search_within) {
    Date now = make_cpp_date(2024, 6, 1, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND);
    Date d = now;
    d.raw.second += 3;
    d.normalize();
    ASSUME_ITS_TRUE(d.search(now, "within 5 seconds"));
    ASSUME_ITS_FALSE(d.search(now, "within 1 second"));
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_date_tests) {
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_default_ctor);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_now_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_validate_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_normalize_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_compare_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_diff_seconds_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_to_from_unix_seconds_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_to_unix_nanoseconds_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_format_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_format_smart_relative_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_search_method);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_roundtrip_subsecond);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_format_human_short);
    FOSSIL_TEST_ADD(cpp_date_suite, cpp_test_date_search_within);

    FOSSIL_TEST_REGISTER(cpp_date_suite);
}
