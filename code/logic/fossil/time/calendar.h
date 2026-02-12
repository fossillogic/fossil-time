/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#ifndef FOSSIL_TIME_CALENDAR_H
#define FOSSIL_TIME_CALENDAR_H

#include <stdint.h>
#include "date.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================
 * Fossil Time — Calendar Logic
 * ====================================================== */

/*
 * Calendar utilities operate purely on provided dates.
 * No system time, no locale guessing, no global state.
 */

/* ======================================================
 * C API — Core calendar math
 * ====================================================== */

/**
 * @brief Determines if the specified year is a leap year.
 *
 * Leap years are years divisible by 4, except for years that are divisible by 100,
 * unless they are also divisible by 400.
 *
 * @param year The year to check (e.g., 2024).
 * @return 1 if the year is a leap year, 0 otherwise.
 */
int fossil_time_calendar_is_leap_year(
    int32_t year
);

/**
 * @brief Returns the number of days in a given month for a specific year.
 *
 * Accounts for leap years when the month is February.
 *
 * @param year  The year (e.g., 2024).
 * @param month The month (1 = January, 12 = December).
 * @return The number of days in the specified month (28-31), or 0 if the month is invalid.
 */
int fossil_time_calendar_days_in_month(
    int32_t year,
    int8_t month
);

/**
 * @brief Computes and updates derived calendar fields for a given date structure.
 *
 * This function takes a pointer to a fossil_time_date_t structure and calculates
 * any additional or derived fields that are not directly set by the user, such as
 * day of the week, day of the year, or leap year status. It updates the provided
 * structure in-place with these computed values.
 *
 * @param dt Pointer to a fossil_time_date_t structure whose derived fields will be computed and updated.
 *
 * @note The input structure must be properly initialized with valid date values
 *       before calling this function. The function does not perform validation
 *       of the input fields.
 */
void fossil_time_calendar_compute_derived(
    fossil_time_date_t *dt
);

/* ======================================================
 * C API — Queries (string IDs)
 * ====================================================== */

/**
 * @brief Query calendar properties of a given date using a string identifier.
 *
 * This function checks if the provided date matches a specific calendar property,
 * such as being a particular weekday, weekend, or a named day (e.g., "monday", "friday").
 *
 * @param dt Pointer to a fossil_time_date_t structure representing the date to query.
 * @param query_id String identifier for the property to check (e.g., "weekday", "weekend", "monday").
 * @return 1 if the date matches the query, 0 otherwise.
 *
 * @note The function does not validate the input date structure.
 */
int fossil_time_calendar_is(
    const fossil_time_date_t *dt,
    const char *query_id
);

/**
 * @brief Retrieve a calendar classification for a given date using a string identifier.
 *
 * This function writes a classification string (such as season, quarter, or half-year)
 * into the provided buffer, based on the specified class_id.
 *
 * @param dt          Pointer to a fossil_time_date_t structure representing the date.
 * @param class_id    String identifier for the classification (e.g., "season", "quarter", "half").
 * @param buffer      Output buffer to receive the classification string.
 * @param buffer_size Size of the output buffer.
 * @return 0 on success, non-zero on error (e.g., unknown class_id or insufficient buffer size).
 *
 * @note Example outputs: "spring", "Q1", "H2".
 */
int fossil_time_calendar_get(
    const fossil_time_date_t *dt,
    const char *class_id,
    char *buffer,
    size_t buffer_size
);

/**
 * @brief Determine if a given date is a holiday in a specified region.
 *
 * This function checks if the provided date corresponds to a recognized holiday
 * for the specified region or holiday set.
 *
 * @param dt        Pointer to a fossil_time_date_t structure representing the date.
 * @param region_id String identifier for the region or holiday set (e.g., "us", "us_federal", "eu", "jp").
 * @return 1 if the date is a holiday in the specified region, 0 otherwise.
 */
int fossil_time_calendar_is_holiday(
    const fossil_time_date_t *dt,
    const char *region_id
);

/**
 * @brief Retrieve the holiday name for a given date and region, if applicable.
 *
 * If the specified date is a holiday in the given region, this function writes
 * the holiday's string identifier (e.g., "new_years_day", "christmas") into the provided buffer.
 *
 * @param dt          Pointer to a fossil_time_date_t structure representing the date.
 * @param region_id   String identifier for the region or holiday set.
 * @param buffer      Output buffer to receive the holiday name.
 * @param buffer_size Size of the output buffer.
 * @return 0 on success (holiday found and name written), non-zero if not a holiday or on error.
 */
int fossil_time_calendar_get_holiday(
    const fossil_time_date_t *dt,
    const char *region_id,
    char *buffer,
    size_t buffer_size
);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ======================================================
 * C++ Wrapper — Thin, Inline
 * ====================================================== */

#ifdef __cplusplus

namespace fossil {

    namespace time {

        class Calendar {
        public:
            Calendar() = delete; /* static-only utility */

            /**
             * @brief Determines if the specified year is a leap year.
             *
             * Leap years are years divisible by 4, except for years that are divisible by 100,
             * unless they are also divisible by 400.
             *
             * @param year The year to check (e.g., 2024).
             * @return true if the year is a leap year, false otherwise.
             */
            static inline bool is_leap_year(int32_t year) {
                return fossil_time_calendar_is_leap_year(year) != 0;
            }

            /**
             * @brief Returns the number of days in a given month for a specific year.
             *
             * Accounts for leap years when the month is February.
             *
             * @param year  The year (e.g., 2024).
             * @param month The month (1 = January, 12 = December).
             * @return The number of days in the specified month (28-31), or 0 if the month is invalid.
             */
            static inline int days_in_month(int32_t year, int8_t month) {
                return fossil_time_calendar_days_in_month(year, month);
            }

            /**
             * @brief Computes and updates derived calendar fields for a given Date object.
             *
             * This function calculates additional or derived fields for the provided Date,
             * such as day of the week, day of the year, or leap year status. It updates
             * the Date object in-place with these computed values.
             *
             * @param dt Reference to a Date object whose derived fields will be computed and updated.
             *
             * @note The input Date must be properly initialized with valid date values
             *       before calling this function. The function does not perform validation
             *       of the input fields.
             */
            static inline void compute_derived(Date &dt) {
                fossil_time_calendar_compute_derived(&dt.raw);
            }

            /**
             * @brief Query calendar properties of a given Date using a string identifier.
             *
             * Checks if the provided Date matches a specific calendar property,
             * such as being a particular weekday, weekend, or a named day (e.g., "monday", "friday").
             *
             * @param dt       Reference to a Date object representing the date to query.
             * @param query_id String identifier for the property to check (e.g., "weekday", "weekend", "monday").
             * @return true if the date matches the query, false otherwise.
             *
             * @note The function does not validate the input Date object.
             */
            static inline bool is(const Date &dt, const char *query_id) {
                return fossil_time_calendar_is(&dt.raw, query_id) != 0;
            }

            /**
             * @brief Retrieve a calendar classification for a given Date using a string identifier.
             *
             * Writes a classification string (such as season, quarter, or half-year)
             * into the provided buffer, based on the specified class_id.
             *
             * @param dt          Reference to a Date object representing the date.
             * @param class_id    String identifier for the classification (e.g., "season", "quarter", "half").
             * @param buffer      Output buffer to receive the classification string.
             * @param buffer_size Size of the output buffer.
             * @return 0 on success, non-zero on error (e.g., unknown class_id or insufficient buffer size).
             *
             * @note Example outputs: "spring", "Q1", "H2".
             */
            static inline int get(
                const Date &dt,
                const char *class_id,
                char *buffer,
                size_t buffer_size
            ) {
                return fossil_time_calendar_get(
                    &dt.raw, class_id, buffer, buffer_size
                );
            }

            /**
             * @brief Determine if a given Date is a holiday in a specified region.
             *
             * Checks if the provided Date corresponds to a recognized holiday
             * for the specified region or holiday set.
             *
             * @param dt        Reference to a Date object representing the date.
             * @param region_id String identifier for the region or holiday set (e.g., "us", "us_federal", "eu", "jp").
             * @return true if the date is a holiday in the specified region, false otherwise.
             */
            static inline bool is_holiday(
                const Date &dt,
                const char *region_id
            ) {
                return fossil_time_calendar_is_holiday(
                    &dt.raw, region_id
                ) != 0;
            }

            /**
             * @brief Retrieve the holiday name for a given Date and region, if applicable.
             *
             * If the specified Date is a holiday in the given region, this function writes
             * the holiday's string identifier (e.g., "new_years_day", "christmas") into the provided buffer.
             *
             * @param dt          Reference to a Date object representing the date.
             * @param region_id   String identifier for the region or holiday set.
             * @param buffer      Output buffer to receive the holiday name.
             * @param buffer_size Size of the output buffer.
             * @return 0 on success (holiday found and name written), non-zero if not a holiday or on error.
             */
            static inline int get_holiday(
                const Date &dt,
                const char *region_id,
                char *buffer,
                size_t buffer_size
            ) {
                return fossil_time_calendar_get_holiday(
                    &dt.raw, region_id, buffer, buffer_size
                );
            }
        };

    } /* namespace time */

} /* namespace fossil */

#endif

#endif /* FOSSIL_TIME_CALENDAR_H */
