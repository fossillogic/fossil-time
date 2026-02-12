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
FOSSIL_SUITE(cpp_calendar_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_calendar_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_calendar_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

using fossil::time::Calendar;
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

// Test: Calendar::is_leap_year
FOSSIL_TEST(cpp_test_calendar_is_leap_year) {
    ASSUME_ITS_TRUE(Calendar::is_leap_year(2020));
    ASSUME_ITS_TRUE(Calendar::is_leap_year(2000));
    ASSUME_ITS_FALSE(Calendar::is_leap_year(1900));
    ASSUME_ITS_FALSE(Calendar::is_leap_year(2023));
}

// Test: Calendar::days_in_month
FOSSIL_TEST(cpp_test_calendar_days_in_month) {
    ASSUME_ITS_EQUAL_I32(Calendar::days_in_month(2024, 2), 29); // Leap year
    ASSUME_ITS_EQUAL_I32(Calendar::days_in_month(2023, 2), 28);
    ASSUME_ITS_EQUAL_I32(Calendar::days_in_month(2024, 4), 30);
    ASSUME_ITS_EQUAL_I32(Calendar::days_in_month(2024, 1), 31);
    ASSUME_ITS_EQUAL_I32(Calendar::days_in_month(2024, 0), 0); // Invalid month
    ASSUME_ITS_EQUAL_I32(Calendar::days_in_month(2024, 13), 0); // Invalid month
}

// Test: Calendar::compute_derived
FOSSIL_TEST(cpp_test_calendar_compute_derived) {
    Date dt = make_cpp_date(2024, 12, 31, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    Calendar::compute_derived(dt);
    ASSUME_ITS_EQUAL_I32(dt.raw.weekday, 2); // 2024-12-31 is Tuesday
    ASSUME_ITS_EQUAL_I32(dt.raw.yearday, 366); // Leap year, last day

    dt.raw.precision_mask = 0; // Not enough precision
    Calendar::compute_derived(dt);
    ASSUME_ITS_EQUAL_I32(dt.raw.weekday, -1);
    ASSUME_ITS_EQUAL_I32(dt.raw.yearday, -1);
}

// Test: Calendar::is (weekday/weekend/named days)
FOSSIL_TEST(cpp_test_calendar_is_queries) {
    Date dt = make_cpp_date(2024, 6, 3, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    Calendar::compute_derived(dt); // 2024-06-03 is Monday (weekday 1)
    ASSUME_ITS_TRUE(Calendar::is(dt, "weekday"));
    ASSUME_ITS_FALSE(Calendar::is(dt, "weekend"));
    ASSUME_ITS_TRUE(Calendar::is(dt, "monday"));
    ASSUME_ITS_FALSE(Calendar::is(dt, "sunday"));

    dt.raw.weekday = 0; // Sunday
    ASSUME_ITS_TRUE(Calendar::is(dt, "sunday"));
    ASSUME_ITS_TRUE(Calendar::is(dt, "weekend"));
    ASSUME_ITS_FALSE(Calendar::is(dt, "weekday"));
}

// Test: Calendar::get (season, quarter, half)
FOSSIL_TEST(cpp_test_calendar_get_classifications) {
    Date dt = make_cpp_date(2024, 4, 15, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    char buf[32];

    ASSUME_ITS_EQUAL_I32(Calendar::get(dt, "season", buf, sizeof(buf)), strlen("spring"));
    ASSUME_ITS_EQUAL_CSTR(buf, "spring");

    ASSUME_ITS_EQUAL_I32(Calendar::get(dt, "quarter", buf, sizeof(buf)), strlen("Q2"));
    ASSUME_ITS_EQUAL_CSTR(buf, "Q2");

    ASSUME_ITS_EQUAL_I32(Calendar::get(dt, "half", buf, sizeof(buf)), strlen("H1"));
    ASSUME_ITS_EQUAL_CSTR(buf, "H1");

    dt.raw.month = 10;
    ASSUME_ITS_EQUAL_I32(Calendar::get(dt, "season", buf, sizeof(buf)), strlen("autumn"));
    ASSUME_ITS_EQUAL_CSTR(buf, "autumn");

    ASSUME_ITS_EQUAL_I32(Calendar::get(dt, "quarter", buf, sizeof(buf)), strlen("Q4"));
    ASSUME_ITS_EQUAL_CSTR(buf, "Q4");

    ASSUME_ITS_EQUAL_I32(Calendar::get(dt, "half", buf, sizeof(buf)), strlen("H2"));
    ASSUME_ITS_EQUAL_CSTR(buf, "H2");

    // Unknown class_id
    ASSUME_ITS_EQUAL_I32(Calendar::get(dt, "unknown", buf, sizeof(buf)), -1);
}

// Test: Calendar::is_holiday and get_holiday (US baseline)
FOSSIL_TEST(cpp_test_calendar_holidays_us) {
    Date dt = make_cpp_date(2024, 1, 1, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    char buf[32];

    ASSUME_ITS_TRUE(Calendar::is_holiday(dt, "us"));
    ASSUME_ITS_EQUAL_I32(Calendar::get_holiday(dt, "us", buf, sizeof(buf)), strlen("new_years_day"));
    ASSUME_ITS_EQUAL_CSTR(buf, "new_years_day");

    dt.raw.month = 7; dt.raw.day = 4;
    ASSUME_ITS_TRUE(Calendar::is_holiday(dt, "us_federal"));
    ASSUME_ITS_EQUAL_I32(Calendar::get_holiday(dt, "us_federal", buf, sizeof(buf)), strlen("independence_day"));
    ASSUME_ITS_EQUAL_CSTR(buf, "independence_day");

    dt.raw.month = 12; dt.raw.day = 25;
    ASSUME_ITS_TRUE(Calendar::is_holiday(dt, "us"));
    ASSUME_ITS_EQUAL_I32(Calendar::get_holiday(dt, "us", buf, sizeof(buf)), strlen("christmas"));
    ASSUME_ITS_EQUAL_CSTR(buf, "christmas");

    dt.raw.month = 2; dt.raw.day = 14;
    ASSUME_ITS_FALSE(Calendar::is_holiday(dt, "us"));
    ASSUME_ITS_EQUAL_I32(Calendar::get_holiday(dt, "us", buf, sizeof(buf)), -1);

    // Unknown region
    ASSUME_ITS_FALSE(Calendar::is_holiday(dt, "eu"));
    ASSUME_ITS_EQUAL_I32(Calendar::get_holiday(dt, "eu", buf, sizeof(buf)), -1);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_calendar_tests) {
    FOSSIL_TEST_ADD(cpp_calendar_suite, cpp_test_calendar_is_leap_year);
    FOSSIL_TEST_ADD(cpp_calendar_suite, cpp_test_calendar_days_in_month);
    FOSSIL_TEST_ADD(cpp_calendar_suite, cpp_test_calendar_compute_derived);
    FOSSIL_TEST_ADD(cpp_calendar_suite, cpp_test_calendar_is_queries);
    FOSSIL_TEST_ADD(cpp_calendar_suite, cpp_test_calendar_get_classifications);
    FOSSIL_TEST_ADD(cpp_calendar_suite, cpp_test_calendar_holidays_us);

    FOSSIL_TEST_REGISTER(cpp_calendar_suite);
}
