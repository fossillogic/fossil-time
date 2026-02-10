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
FOSSIL_SUITE(c_date_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_date_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_date_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Helper: create a fossil_time_date_t with given fields and mask
static fossil_time_date_t make_date(
    int year, int month, int day, int hour, int min, int sec,
    int ms, int us, int ns, uint64_t mask
) {
    fossil_time_date_t dt;
    memset(&dt, 0, sizeof(dt));
    dt.year = year;
    dt.month = month;
    dt.day = day;
    dt.hour = hour;
    dt.minute = min;
    dt.second = sec;
    dt.millisecond = ms;
    dt.microsecond = us;
    dt.nanosecond = ns;
    dt.precision_mask = mask;
    return dt;
}

// Test: Precision mask bits are unique and non-overlapping
FOSSIL_TEST(c_test_precision_mask_bits) {
    uint64_t mask = 0;
    mask |= FOSSIL_TIME_PRECISION_YEAR;
    mask |= FOSSIL_TIME_PRECISION_MONTH;
    mask |= FOSSIL_TIME_PRECISION_DAY;
    mask |= FOSSIL_TIME_PRECISION_HOUR;
    mask |= FOSSIL_TIME_PRECISION_MINUTE;
    mask |= FOSSIL_TIME_PRECISION_SECOND;
    mask |= FOSSIL_TIME_PRECISION_MILLI;
    mask |= FOSSIL_TIME_PRECISION_MICRO;
    mask |= FOSSIL_TIME_PRECISION_NANO;
    mask |= FOSSIL_TIME_PRECISION_PICO;
    mask |= FOSSIL_TIME_PRECISION_FEMTO;
    mask |= FOSSIL_TIME_PRECISION_ATTO;
    mask |= FOSSIL_TIME_PRECISION_ZEPTO;
    mask |= FOSSIL_TIME_PRECISION_YOCTO;
    // Should be 14 bits set
    ASSUME_ITS_EQUAL_I32(fossil_sys_bitwise_count(mask), 14);
}

// Test: fossil_time_date_now populates all fields and sets precision_mask
FOSSIL_TEST(c_test_date_now) {
    fossil_time_date_t dt;
    fossil_time_date_now(&dt);
    // At least year, month, day, hour, minute, second should be set
    ASSUME_ITS_TRUE(dt.year > 1970 && dt.year < 3000);
    ASSUME_ITS_TRUE(dt.month >= 1 && dt.month <= 12);
    ASSUME_ITS_TRUE(dt.day >= 1 && dt.day <= 31);
    ASSUME_ITS_TRUE(dt.hour >= 0 && dt.hour <= 23);
    ASSUME_ITS_TRUE(dt.minute >= 0 && dt.minute <= 59);
    ASSUME_ITS_TRUE(dt.second >= 0 && dt.second <= 60);
    // Precision mask should have at least year..second
    uint64_t mask = FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH |
                    FOSSIL_TIME_PRECISION_DAY | FOSSIL_TIME_PRECISION_HOUR |
                    FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND;
    ASSUME_ITS_TRUE((dt.precision_mask & mask) == mask);
}

// Test: fossil_time_date_validate detects valid and invalid dates
FOSSIL_TEST(c_test_date_validate) {
    fossil_time_date_t dt = make_date(2024, 2, 29, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    ASSUME_ITS_TRUE(fossil_time_date_validate(&dt)); // Leap year

    dt.month = 2; dt.day = 30;
    ASSUME_ITS_FALSE(fossil_time_date_validate(&dt)); // Invalid day

    dt.month = 13; dt.day = 1;
    ASSUME_ITS_FALSE(fossil_time_date_validate(&dt)); // Invalid month

    dt = make_date(2023, 4, 31, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    ASSUME_ITS_FALSE(fossil_time_date_validate(&dt)); // April has 30 days

    dt = make_date(2024, 1, 1, 25, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY | FOSSIL_TIME_PRECISION_HOUR);
    ASSUME_ITS_FALSE(fossil_time_date_validate(&dt)); // Invalid hour
}

// Test: fossil_time_date_normalize sets weekday and yearday
FOSSIL_TEST(c_test_date_normalize) {
    fossil_time_date_t dt = make_date(2024, 6, 1, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    fossil_time_date_normalize(&dt);
    // 2024-06-01 is a Saturday (weekday 6), yearday 153
    ASSUME_ITS_EQUAL_I32(dt.weekday, 6);
    ASSUME_ITS_EQUAL_I32(dt.yearday, 153);
}

// Test: fossil_time_date_compare
FOSSIL_TEST(c_test_date_compare) {
    fossil_time_date_t a = make_date(2024, 6, 1, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY | FOSSIL_TIME_PRECISION_HOUR);
    fossil_time_date_t b = make_date(2024, 6, 1, 13, 0, 0, 0, 0, 0,
        a.precision_mask);
    ASSUME_ITS_EQUAL_I32(fossil_time_date_compare(&a, &b), -1);
    ASSUME_ITS_EQUAL_I32(fossil_time_date_compare(&b, &a), 1);
    ASSUME_ITS_EQUAL_I32(fossil_time_date_compare(&a, &a), 0);
}

// Test: fossil_time_date_diff_seconds
FOSSIL_TEST(c_test_date_diff_seconds) {
    fossil_time_date_t a = make_date(2024, 6, 1, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY | FOSSIL_TIME_PRECISION_HOUR);
    fossil_time_date_t b = make_date(2024, 6, 1, 11, 0, 0, 0, 0, 0,
        a.precision_mask);
    int64_t diff = fossil_time_date_diff_seconds(&a, &b);
    ASSUME_ITS_EQUAL_I32(diff, 3600);
}

// Test: fossil_time_date_to_unix_seconds and from_unix_seconds
FOSSIL_TEST(c_test_date_to_from_unix_seconds) {
    fossil_time_date_t dt = make_date(2024, 6, 1, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    dt.precision_mask |= FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND;
    int64_t unix_sec = fossil_time_date_to_unix_seconds(&dt);

    fossil_time_date_t dt2;
    fossil_time_date_from_unix_seconds(unix_sec, &dt2);
    ASSUME_ITS_EQUAL_I32(dt2.year, 2024);
    ASSUME_ITS_EQUAL_I32(dt2.month, 6);
    ASSUME_ITS_EQUAL_I32(dt2.day, 1);
    ASSUME_ITS_EQUAL_I32(dt2.hour, 0);
    ASSUME_ITS_EQUAL_I32(dt2.minute, 0);
    ASSUME_ITS_EQUAL_I32(dt2.second, 0);
}

// Test: fossil_time_date_to_unix_nanoseconds
FOSSIL_TEST(c_test_date_to_unix_nanoseconds) {
    fossil_time_date_t dt = make_date(1970, 1, 1, 0, 0, 1, 123, 456, 789,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND |
        FOSSIL_TIME_PRECISION_MILLI | FOSSIL_TIME_PRECISION_MICRO | FOSSIL_TIME_PRECISION_NANO);
    int64_t ns = fossil_time_date_to_unix_nanoseconds(&dt);
    int64_t expected = 1 * 1000000000LL + 123 * 1000000LL + 456 * 1000LL + 789;
    ASSUME_ITS_EQUAL_I64(ns, expected);
}

// Test: fossil_time_date_format
FOSSIL_TEST(c_test_date_format) {
    fossil_time_date_t dt = make_date(2024, 6, 1, 12, 34, 56, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND);
    char buf[64];
    int n = fossil_time_date_format(&dt, buf, sizeof(buf), "iso");
    ASSUME_ITS_EQUAL_CSTR(buf, "2024-06-01T12:34:56Z");
    n = fossil_time_date_format(&dt, buf, sizeof(buf), "log");
    ASSUME_ITS_EQUAL_CSTR(buf, "20240601-123456");
    n = fossil_time_date_format(&dt, buf, sizeof(buf), "unknown");
    ASSUME_ITS_EQUAL_CSTR(buf, "invalid_date");
}

// Test: fossil_time_date_format_smart and format_relative
FOSSIL_TEST(c_test_date_format_smart_relative) {
    fossil_time_date_t now = make_date(2024, 6, 1, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND);
    fossil_time_date_t dt = now;
    char buf[64];
    int n = fossil_time_date_format_smart(&dt, &now, buf, sizeof(buf));
    ASSUME_ITS_EQUAL_CSTR(buf, "now");
    dt.second += 10;
    fossil_time_date_normalize(&dt);
    n = fossil_time_date_format_smart(&dt, &now, buf, sizeof(buf));
    ASSUME_ITS_TRUE(strstr(buf, "in 10 seconds") != NULL);
    dt = now;
    dt.second -= 5;
    fossil_time_date_normalize(&dt);
    n = fossil_time_date_format_relative(&dt, &now, buf, sizeof(buf));
    ASSUME_ITS_TRUE(strstr(buf, "5 seconds ago") != NULL);
}

// Test: fossil_time_date_search
FOSSIL_TEST(c_test_date_search) {
    fossil_time_date_t now = make_date(2024, 6, 1, 12, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY |
        FOSSIL_TIME_PRECISION_HOUR | FOSSIL_TIME_PRECISION_MINUTE | FOSSIL_TIME_PRECISION_SECOND);
    fossil_time_date_t dt = now;
    fossil_time_date_normalize(&dt);
    ASSUME_ITS_TRUE(fossil_time_date_search(&dt, &now, "today"));
    dt.weekday = 6; // Saturday
    ASSUME_ITS_TRUE(fossil_time_date_search(&dt, &now, "weekend"));
    dt.weekday = 2; // Tuesday
    ASSUME_ITS_FALSE(fossil_time_date_search(&dt, &now, "weekend"));
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_date_tests) {
    FOSSIL_TEST_ADD(c_date_suite, c_test_precision_mask_bits);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_now);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_validate);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_normalize);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_compare);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_diff_seconds);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_to_from_unix_seconds);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_to_unix_nanoseconds);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_format);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_format_smart_relative);
    FOSSIL_TEST_ADD(c_date_suite, c_test_date_search);

    FOSSIL_TEST_REGISTER(c_date_suite);
}
