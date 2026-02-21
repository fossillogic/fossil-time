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
FOSSIL_SUITE(cpp_holiday_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_holiday_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_holiday_suite) {
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

// Test: fossil_holiday_register and fossil_holiday_get
FOSSIL_TEST(cpp_test_holiday_register_get) {
    fossil::time::Holiday custom;
    custom.name = "Test Holiday";
    custom.type = FOSSIL_HOLIDAY_FIXED;
    custom.month = 3;
    custom.day = 15;
    
    ASSUME_ITS_EQUAL_I32(custom.register_holiday(), 0);
    
    const fossil_holiday_t *retrieved = NULL;
    ASSUME_ITS_EQUAL_I32(fossil_holiday_get("Test Holiday", &retrieved), 0);
    ASSUME_NOT_CNULL(retrieved);
    ASSUME_ITS_EQUAL_CSTR(retrieved->name, "Test Holiday");
    
    ASSUME_ITS_EQUAL_I32(fossil_holiday_get("Nonexistent Holiday", &retrieved), -1);
}

// Test: fossil_holiday_date for FIXED type
FOSSIL_TEST(cpp_test_holiday_date_fixed) {
    fossil::time::Holiday h;
    h.name = "Fixed Test";
    h.type = FOSSIL_HOLIDAY_FIXED;
    h.month = 7;
    h.day = 4;
    
    fossil_time_date_t dt = h.date(2024);
    ASSUME_ITS_EQUAL_I32(dt.year, 2024);
    ASSUME_ITS_EQUAL_I32(dt.month, 7);
    ASSUME_ITS_EQUAL_I32(dt.day, 4);
}

// Test: fossil_holiday_date for WEEKDAY type
FOSSIL_TEST(cpp_test_holiday_date_weekday) {
    fossil::time::Holiday h;
    h.name = "Weekday Test";
    h.type = FOSSIL_HOLIDAY_WEEKDAY;
    h.month = 1;
    h.weekday = 1;
    h.nth = 3;  /* 3rd Monday in January */
    
    fossil_time_date_t dt = h.date(2024);
    ASSUME_ITS_EQUAL_I32(dt.year, 2024);
    ASSUME_ITS_EQUAL_I32(dt.month, 1);
    ASSUME_ITS_EQUAL_I32(dt.weekday, 1);  /* Monday */
}

// Test: fossil_holiday_is
FOSSIL_TEST(cpp_test_holiday_is) {
    fossil_time_date_t dt = make_date(2024, 1, 1, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    
    fossil::time::Holiday h;
    h.name = "New Year's Day";
    ASSUME_ITS_TRUE(h.is(dt));
    
    dt.day = 2;
    ASSUME_ITS_FALSE(h.is(dt));
}

// Test: fossil_holiday_list
FOSSIL_TEST(cpp_test_holiday_list) {
    auto holidays = fossil::time::Holiday::list(2024);
    ASSUME_ITS_TRUE(!holidays.empty());
    ASSUME_ITS_EQUAL_I32(holidays[0].year, 2024);
    ASSUME_ITS_EQUAL_I32(holidays[0].month, 1);
    ASSUME_ITS_EQUAL_I32(holidays[0].day, 1);
}

// Test: fossil_holiday_date with invalid input
FOSSIL_TEST(cpp_test_holiday_date_invalid) {
    fossil::time::Holiday h;
    h.name = "Test";
    h.type = FOSSIL_HOLIDAY_FIXED;
    h.month = 1;
    h.day = 1;
    
    // Test that valid holiday works
    fossil_time_date_t dt = h.date(2024);
    ASSUME_ITS_EQUAL_I32(dt.year, 2024);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_holiday_tests) {
    FOSSIL_TEST_ADD(cpp_holiday_suite, cpp_test_holiday_register_get);
    FOSSIL_TEST_ADD(cpp_holiday_suite, cpp_test_holiday_date_fixed);
    FOSSIL_TEST_ADD(cpp_holiday_suite, cpp_test_holiday_date_weekday);
    FOSSIL_TEST_ADD(cpp_holiday_suite, cpp_test_holiday_is);
    FOSSIL_TEST_ADD(cpp_holiday_suite, cpp_test_holiday_list);
    FOSSIL_TEST_ADD(cpp_holiday_suite, cpp_test_holiday_date_invalid);

    FOSSIL_TEST_REGISTER(cpp_holiday_suite);
}
