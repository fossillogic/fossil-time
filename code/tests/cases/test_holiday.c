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
FOSSIL_SUITE(c_holiday_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_holiday_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_holiday_suite) {
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
FOSSIL_TEST(c_test_holiday_register_get) {
    fossil_holiday_t custom = {
        "Test Holiday",
        FOSSIL_HOLIDAY_FIXED,
        3, 15, 0, 0, 0, NULL
    };
    
    ASSUME_ITS_EQUAL_I32(fossil_holiday_register(&custom), 0);
    
    const fossil_holiday_t *retrieved = NULL;
    ASSUME_ITS_EQUAL_I32(fossil_holiday_get("Test Holiday", &retrieved), 0);
    ASSUME_NOT_CNULL(retrieved);
    ASSUME_ITS_EQUAL_CSTR(retrieved->name, "Test Holiday");
    
    ASSUME_ITS_EQUAL_I32(fossil_holiday_get("Nonexistent Holiday", &retrieved), -1);
}

// Test: fossil_holiday_date for FIXED type
FOSSIL_TEST(c_test_holiday_date_fixed) {
    fossil_holiday_t h = {
        "Fixed Test",
        FOSSIL_HOLIDAY_FIXED,
        7, 4, 0, 0, 0, NULL
    };
    
    fossil_time_date_t dt;
    ASSUME_ITS_EQUAL_I32(fossil_holiday_date(&h, 2024, &dt), 0);
    ASSUME_ITS_EQUAL_I32(dt.year, 2024);
    ASSUME_ITS_EQUAL_I32(dt.month, 7);
    ASSUME_ITS_EQUAL_I32(dt.day, 4);
}

// Test: fossil_holiday_date for WEEKDAY type
FOSSIL_TEST(c_test_holiday_date_weekday) {
    fossil_holiday_t h = {
        "Weekday Test",
        FOSSIL_HOLIDAY_WEEKDAY,
        1, 0, 1, 3, 0, NULL  /* 3rd Monday in January */
    };
    
    fossil_time_date_t dt;
    ASSUME_ITS_EQUAL_I32(fossil_holiday_date(&h, 2024, &dt), 0);
    ASSUME_ITS_EQUAL_I32(dt.year, 2024);
    ASSUME_ITS_EQUAL_I32(dt.month, 1);
    ASSUME_ITS_EQUAL_I32(dt.weekday, 1);  /* Monday */
}

// Test: fossil_holiday_is
FOSSIL_TEST(c_test_holiday_is) {
    fossil_time_date_t dt = make_date(2024, 1, 1, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    
    const char *holiday_name = NULL;
    ASSUME_ITS_EQUAL_I32(fossil_holiday_is(&dt, &holiday_name), 1);
    ASSUME_NOT_CNULL(holiday_name);
    
    dt.day = 2;
    ASSUME_ITS_EQUAL_I32(fossil_holiday_is(&dt, NULL), 0);
}

// Test: fossil_holiday_list
FOSSIL_TEST(c_test_holiday_list) {
    fossil_time_date_t dates[32];
    size_t count = 0;
    
    ASSUME_ITS_EQUAL_I32(fossil_holiday_list(2024, dates, 32, &count), 0);
    ASSUME_ITS_TRUE(count > 0 && count <= 32);
    ASSUME_ITS_EQUAL_I32(dates[0].year, 2024);
    ASSUME_ITS_EQUAL_I32(dates[0].month, 1);
    ASSUME_ITS_EQUAL_I32(dates[0].day, 1);
}

// Test: fossil_holiday_date with invalid input
FOSSIL_TEST(c_test_holiday_date_invalid) {
    fossil_time_date_t dt;
    
    ASSUME_ITS_EQUAL_I32(fossil_holiday_date(NULL, 2024, &dt), -1);
    
    fossil_holiday_t h = {"Test", FOSSIL_HOLIDAY_FIXED, 1, 1, 0, 0, 0, NULL};
    ASSUME_ITS_EQUAL_I32(fossil_holiday_date(&h, 2024, NULL), -1);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_holiday_tests) {
    FOSSIL_TEST_ADD(c_holiday_suite, c_test_holiday_register_get);
    FOSSIL_TEST_ADD(c_holiday_suite, c_test_holiday_date_fixed);
    FOSSIL_TEST_ADD(c_holiday_suite, c_test_holiday_date_weekday);
    FOSSIL_TEST_ADD(c_holiday_suite, c_test_holiday_is);
    FOSSIL_TEST_ADD(c_holiday_suite, c_test_holiday_list);
    FOSSIL_TEST_ADD(c_holiday_suite, c_test_holiday_date_invalid);

    FOSSIL_TEST_REGISTER(c_holiday_suite);
}
