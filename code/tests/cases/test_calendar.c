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
FOSSIL_SUITE(c_calendar_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_calendar_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_calendar_suite) {
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

// Test: fossil_time_calendar_is_leap_year
FOSSIL_TEST(c_test_calendar_is_leap_year) {
    ASSUME_ITS_TRUE(fossil_time_calendar_is_leap_year(2020));
    ASSUME_ITS_TRUE(fossil_time_calendar_is_leap_year(2000));
    ASSUME_ITS_FALSE(fossil_time_calendar_is_leap_year(1900));
    ASSUME_ITS_FALSE(fossil_time_calendar_is_leap_year(2023));
}

// Test: fossil_time_calendar_days_in_month
FOSSIL_TEST(c_test_calendar_days_in_month) {
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_days_in_month(2024, 2), 29); // Leap year
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_days_in_month(2023, 2), 28);
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_days_in_month(2024, 4), 30);
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_days_in_month(2024, 1), 31);
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_days_in_month(2024, 0), 0); // Invalid month
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_days_in_month(2024, 13), 0); // Invalid month
}

// Test: fossil_time_calendar_compute_derived
FOSSIL_TEST(c_test_calendar_compute_derived) {
    fossil_time_date_t dt = make_date(2024, 12, 31, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    fossil_time_calendar_compute_derived(&dt);
    ASSUME_ITS_EQUAL_I32(dt.weekday, 2); // 2024-12-31 is Tuesday
    ASSUME_ITS_EQUAL_I32(dt.yearday, 366); // Leap year, last day

    dt.precision_mask = 0; // Not enough precision
    fossil_time_calendar_compute_derived(&dt);
    ASSUME_ITS_EQUAL_I32(dt.weekday, -1);
    ASSUME_ITS_EQUAL_I32(dt.yearday, -1);
}

// Test: fossil_time_calendar_is (weekday/weekend/named days)
FOSSIL_TEST(c_test_calendar_is_queries) {
    fossil_time_date_t dt = make_date(2024, 6, 3, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    fossil_time_calendar_compute_derived(&dt); // 2024-06-03 is Monday (weekday 1)
    ASSUME_ITS_TRUE(fossil_time_calendar_is(&dt, "weekday"));
    ASSUME_ITS_FALSE(fossil_time_calendar_is(&dt, "weekend"));
    ASSUME_ITS_TRUE(fossil_time_calendar_is(&dt, "monday"));
    ASSUME_ITS_FALSE(fossil_time_calendar_is(&dt, "sunday"));

    dt.weekday = 0; // Sunday
    ASSUME_ITS_TRUE(fossil_time_calendar_is(&dt, "sunday"));
    ASSUME_ITS_TRUE(fossil_time_calendar_is(&dt, "weekend"));
    ASSUME_ITS_FALSE(fossil_time_calendar_is(&dt, "weekday"));
}

// Test: fossil_time_calendar_get (season, quarter, half)
FOSSIL_TEST(c_test_calendar_get_classifications) {
    fossil_time_date_t dt = make_date(2024, 4, 15, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    char buf[32];

    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get(&dt, "season", buf, sizeof(buf)), strlen("spring"));
    ASSUME_ITS_EQUAL_CSTR(buf, "spring");

    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get(&dt, "quarter", buf, sizeof(buf)), strlen("Q2"));
    ASSUME_ITS_EQUAL_CSTR(buf, "Q2");

    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get(&dt, "half", buf, sizeof(buf)), strlen("H1"));
    ASSUME_ITS_EQUAL_CSTR(buf, "H1");

    dt.month = 10;
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get(&dt, "season", buf, sizeof(buf)), strlen("autumn"));
    ASSUME_ITS_EQUAL_CSTR(buf, "autumn");

    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get(&dt, "quarter", buf, sizeof(buf)), strlen("Q4"));
    ASSUME_ITS_EQUAL_CSTR(buf, "Q4");

    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get(&dt, "half", buf, sizeof(buf)), strlen("H2"));
    ASSUME_ITS_EQUAL_CSTR(buf, "H2");

    // Unknown class_id
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get(&dt, "unknown", buf, sizeof(buf)), -1);
}

// Test: fossil_time_calendar_is_holiday and get_holiday (US baseline)
FOSSIL_TEST(c_test_calendar_holidays_us) {
    fossil_time_date_t dt = make_date(2024, 1, 1, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    char buf[32];

    ASSUME_ITS_TRUE(fossil_time_calendar_is_holiday(&dt, "us"));
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get_holiday(&dt, "us", buf, sizeof(buf)), strlen("new_years_day"));
    ASSUME_ITS_EQUAL_CSTR(buf, "new_years_day");

    dt.month = 7; dt.day = 4;
    ASSUME_ITS_TRUE(fossil_time_calendar_is_holiday(&dt, "us_federal"));
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get_holiday(&dt, "us_federal", buf, sizeof(buf)), strlen("independence_day"));
    ASSUME_ITS_EQUAL_CSTR(buf, "independence_day");

    dt.month = 12; dt.day = 25;
    ASSUME_ITS_TRUE(fossil_time_calendar_is_holiday(&dt, "us"));
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get_holiday(&dt, "us", buf, sizeof(buf)), strlen("christmas"));
    ASSUME_ITS_EQUAL_CSTR(buf, "christmas");

    dt.month = 2; dt.day = 14;
    ASSUME_ITS_FALSE(fossil_time_calendar_is_holiday(&dt, "us"));
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get_holiday(&dt, "us", buf, sizeof(buf)), -1);

    // Unknown region
    ASSUME_ITS_FALSE(fossil_time_calendar_is_holiday(&dt, "eu"));
    ASSUME_ITS_EQUAL_I32(fossil_time_calendar_get_holiday(&dt, "eu", buf, sizeof(buf)), -1);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_calendar_tests) {
    FOSSIL_TEST_ADD(c_calendar_suite, c_test_calendar_is_leap_year);
    FOSSIL_TEST_ADD(c_calendar_suite, c_test_calendar_days_in_month);
    FOSSIL_TEST_ADD(c_calendar_suite, c_test_calendar_compute_derived);
    FOSSIL_TEST_ADD(c_calendar_suite, c_test_calendar_is_queries);
    FOSSIL_TEST_ADD(c_calendar_suite, c_test_calendar_get_classifications);
    FOSSIL_TEST_ADD(c_calendar_suite, c_test_calendar_holidays_us);

    FOSSIL_TEST_REGISTER(c_calendar_suite);
}
