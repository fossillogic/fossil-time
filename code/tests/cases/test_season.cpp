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
FOSSIL_SUITE(cpp_season_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_season_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_season_suite) {
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
FOSSIL_TEST(cpp_test_season_of_meteorological_northern) {
    fossil::time::Date date(2024, 3, 15);
    
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL).value,
        FOSSIL_SEASON_SPRING
    );
    
    date = fossil::time::Date(2024, 6, 15);
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL).value,
        FOSSIL_SEASON_SUMMER
    );
    
    date = fossil::time::Date(2024, 9, 15);
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL).value,
        FOSSIL_SEASON_AUTUMN
    );
    
    date = fossil::time::Date(2024, 12, 15);
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL).value,
        FOSSIL_SEASON_WINTER
    );
}

// Test: fossil_season_of with meteorological system (Southern hemisphere)
FOSSIL_TEST(cpp_test_season_of_meteorological_southern) {
    fossil::time::Date date(2024, 3, 15);
    
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL).value,
        FOSSIL_SEASON_AUTUMN
    );
    
    date = fossil::time::Date(2024, 6, 15);
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL).value,
        FOSSIL_SEASON_WINTER
    );
    
    date = fossil::time::Date(2024, 12, 15);
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL).value,
        FOSSIL_SEASON_SUMMER
    );
}

// Test: fossil_season_of with astronomical system (Northern hemisphere)
FOSSIL_TEST(cpp_test_season_of_astronomical_northern) {
    fossil::time::Date date(2024, 3, 20);
    
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL).value,
        FOSSIL_SEASON_SPRING
    );
    
    date = fossil::time::Date(2024, 6, 21);
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL).value,
        FOSSIL_SEASON_SUMMER
    );
    
    date = fossil::time::Date(2024, 9, 22);
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL).value,
        FOSSIL_SEASON_AUTUMN
    );
    
    date = fossil::time::Date(2024, 12, 21);
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL).value,
        FOSSIL_SEASON_WINTER
    );
}

// Test: fossil_season_of with null date
FOSSIL_TEST(cpp_test_season_of_null_date) {
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(NULL, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_UNKNOWN
    );
}

// Test: fossil_season_of with insufficient precision
FOSSIL_TEST(cpp_test_season_of_no_month_precision) {
    fossil_time_date_t dt = make_date(2024, 3, 15, 0, 0, 0, 0, 0, 0,
        FOSSIL_TIME_PRECISION_YEAR);
    
    ASSUME_ITS_EQUAL_I32(
        fossil_season_of(&dt, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL),
        FOSSIL_SEASON_UNKNOWN
    );
}

// Test: fossil_season_name for all seasons
FOSSIL_TEST(cpp_test_season_name) {
    ASSUME_ITS_EQUAL_CSTR(fossil::time::Season(FOSSIL_SEASON_SPRING).name().c_str(), "Spring");
    ASSUME_ITS_EQUAL_CSTR(fossil::time::Season(FOSSIL_SEASON_SUMMER).name().c_str(), "Summer");
    ASSUME_ITS_EQUAL_CSTR(fossil::time::Season(FOSSIL_SEASON_AUTUMN).name().c_str(), "Autumn");
    ASSUME_ITS_EQUAL_CSTR(fossil::time::Season(FOSSIL_SEASON_WINTER).name().c_str(), "Winter");
    ASSUME_ITS_EQUAL_CSTR(fossil::time::Season(FOSSIL_SEASON_UNKNOWN).name().c_str(), "Unknown");
}

// Test: fossil_season_is for matching seasons
FOSSIL_TEST(cpp_test_season_is_match) {
    fossil::time::Date date(2024, 6, 15);
    fossil::time::Season summer(FOSSIL_SEASON_SUMMER);
    fossil::time::Season winter(FOSSIL_SEASON_WINTER);
    
    ASSUME_ITS_TRUE(
        summer.is(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
    
    ASSUME_ITS_FALSE(
        winter.is(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
}

// Test: fossil_season_is with southern hemisphere
FOSSIL_TEST(cpp_test_season_is_southern_hemisphere) {
    fossil::time::Date date(2024, 6, 15);
    fossil::time::Season winter(FOSSIL_SEASON_WINTER);
    fossil::time::Season summer(FOSSIL_SEASON_SUMMER);
    
    ASSUME_ITS_TRUE(
        winter.is(date, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
    
    ASSUME_ITS_FALSE(
        summer.is(date, FOSSIL_HEMISPHERE_SOUTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
}

// Test: fossil_season_is with null date
FOSSIL_TEST(cpp_test_season_is_null_date) {
    ASSUME_ITS_FALSE(
        fossil_season_is(NULL, FOSSIL_SEASON_SPRING, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_METEOROLOGICAL)
    );
}

// Test: fossil_season_of astronomical boundary dates
FOSSIL_TEST(cpp_test_season_of_astronomical_boundaries) {
    fossil::time::Date date(2024, 3, 19);
    
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL).value,
        FOSSIL_SEASON_WINTER
    );
    
    date = fossil::time::Date(2024, 3, 20);
    ASSUME_ITS_EQUAL_I32(
        fossil::time::Season::of(date, FOSSIL_HEMISPHERE_NORTHERN, FOSSIL_SEASON_ASTRONOMICAL).value,
        FOSSIL_SEASON_SPRING
    );
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_season_tests) {
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_of_meteorological_northern);
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_of_meteorological_southern);
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_of_astronomical_northern);
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_of_null_date);
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_of_no_month_precision);
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_name);
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_is_match);
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_is_southern_hemisphere);
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_is_null_date);
    FOSSIL_TEST_ADD(cpp_season_suite, cpp_test_season_of_astronomical_boundaries);

    FOSSIL_TEST_REGISTER(cpp_season_suite);
}
