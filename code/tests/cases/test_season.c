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
FOSSIL_SUITE(c_season_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_season_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_season_suite) {
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

// Test: fossil_season_of with meteorological system (Northern hemisphere)
FOSSIL_TEST(c_test_season_of_meteorological_northern) {
    fossil_time_date_t dt = make_date(2024, 3, 15, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_SPRING
    );
    
    dt.month = 6;
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_SUMMER
    );
    
    dt.month = 9;
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_AUTUMN
    );
    
    dt.month = 12;
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_WINTER
    );
}

// Test: fossil_season_of with meteorological system (Southern hemisphere)
FOSSIL_TEST(c_test_season_of_meteorological_southern) {
    fossil_time_date_t dt = make_date(2024, 3, 15, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_AUTUMN
    );
    
    dt.month = 6;
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_WINTER
    );
    
    dt.month = 12;
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_SUMMER
    );
}

// Test: fossil_season_of with astronomical system (Northern hemisphere)
FOSSIL_TEST(c_test_season_of_astronomical_northern) {
    fossil_time_date_t dt = make_date(2024, 3, 20, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL),
        FOSSIL_SEASON_SPRING
    );
    
    dt.month = 6; dt.day = 21;
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL),
        FOSSIL_SEASON_SUMMER
    );
    
    dt.month = 9; dt.day = 22;
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL),
        FOSSIL_SEASON_AUTUMN
    );
    
    dt.month = 12; dt.day = 21;
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL),
        FOSSIL_SEASON_WINTER
    );
}

// Test: fossil_season_of with null date
FOSSIL_TEST(c_test_season_of_null_date) {
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(NULL, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_UNKNOWN
    );
}

// Test: fossil_season_of with insufficient precision
FOSSIL_TEST(c_test_season_of_no_month_precision) {
    fossil_time_date_t dt = make_date(2024, 3, 15, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR);
    
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_UNKNOWN
    );
}

// Test: fossil_season_name for all seasons
FOSSIL_TEST(c_test_season_name) {
    ASSUME_ITS_EQUAL_CSTR(fossil_season_name(FOSSIL_SEASON_SPRING), "Spring");
    ASSUME_ITS_EQUAL_CSTR(fossil_season_name(FOSSIL_SEASON_SUMMER), "Summer");
    ASSUME_ITS_EQUAL_CSTR(fossil_season_name(FOSSIL_SEASON_AUTUMN), "Autumn");
    ASSUME_ITS_EQUAL_CSTR(fossil_season_name(FOSSIL_SEASON_WINTER), "Winter");
    ASSUME_ITS_EQUAL_CSTR(fossil_season_name(FOSSIL_SEASON_UNKNOWN), "Unknown");
}

// Test: fossil_season_is for matching seasons
FOSSIL_TEST(c_test_season_is_match) {
    fossil_time_date_t dt = make_date(2024, 6, 15, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    
    ASSUME_ITS_TRUE(
        fossil_season_is(&dt, FOSSIL_SEASON_SUMMER, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
    
    ASSUME_ITS_FALSE(
        fossil_season_is(&dt, FOSSIL_SEASON_WINTER, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
}

// Test: fossil_season_is with southern hemisphere
FOSSIL_TEST(c_test_season_is_southern_hemisphere) {
    fossil_time_date_t dt = make_date(2024, 6, 15, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    
    ASSUME_ITS_TRUE(
        fossil_season_is(&dt, FOSSIL_SEASON_WINTER, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
    
    ASSUME_ITS_FALSE(
        fossil_season_is(&dt, FOSSIL_SEASON_SUMMER, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
}

// Test: fossil_season_is with null date
FOSSIL_TEST(c_test_season_is_null_date) {
    ASSUME_ITS_FALSE(
        fossil_season_is(NULL, FOSSIL_SEASON_SPRING, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
}

// Test: fossil_season_of astronomical boundary dates
FOSSIL_TEST(c_test_season_of_astronomical_boundaries) {
    fossil_time_date_t dt = make_date(2024, 3, 19, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR | FOSSIL_TIME_PRECISION_MONTH | FOSSIL_TIME_PRECISION_DAY);
    
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL),
        FOSSIL_SEASON_WINTER
    );
    
    dt.day = 20;
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL),
        FOSSIL_SEASON_SPRING
    );
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_season_tests) {
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_of_meteorological_northern);
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_of_meteorological_southern);
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_of_astronomical_northern);
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_of_null_date);
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_of_no_month_precision);
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_name);
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_is_match);
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_is_southern_hemisphere);
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_is_null_date);
    FOSSIL_TEST_ADD(c_season_suite, c_test_season_of_astronomical_boundaries);

    FOSSIL_TEST_REGISTER(c_season_suite);
}
