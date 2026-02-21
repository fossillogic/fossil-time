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
#ifndef FOSSIL_TIME_HOLIDAY_H
#define FOSSIL_TIME_HOLIDAY_H

#include "date.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================
 * Fossil Time — Holiday API
 * ====================================================== */

/**
 * @brief Holiday type enumeration.
 */
typedef enum fossil_holiday_type_t {
    FOSSIL_HOLIDAY_FIXED,    /* Fixed date every year (e.g., Jan 1) */
    FOSSIL_HOLIDAY_WEEKDAY,  /* e.g., 3rd Monday in Feb */
    FOSSIL_HOLIDAY_RELATIVE, /* Relative to other holidays or Easter */
    FOSSIL_HOLIDAY_CUSTOM    /* Computed via callback or formula */
} fossil_holiday_type_t;

/**
 * @brief Holiday descriptor.
 */
typedef struct fossil_holiday_t {
    const char* name;                  /* e.g., "New Year's Day" */
    fossil_holiday_type_t type;        /* Holiday type */
    int month;                          /* 1–12 (for fixed) */
    int day;                            /* 1–31 (for fixed) */
    int weekday;                        /* 0–6 (for weekday-based) */
    int nth;                             /* e.g., 3rd Monday = 3 */
    int offset_days;                    /* Offset relative to other holiday */
    const char* relative_to;            /* Name of holiday to be relative to */
} fossil_holiday_t;

/* ======================================================
 * C API — Core
 * ====================================================== */

/**
 * @brief Register a custom holiday.
 *
 * @param holiday Pointer to a fossil_holiday_t descriptor.
 * @return 0 on success, non-zero on error.
 */
int fossil_holiday_register(const fossil_holiday_t *holiday);

/**
 * @brief Get a holiday by name.
 *
 * @param name       Name of the holiday.
 * @param out_holiday Output pointer to stored holiday descriptor.
 * @return 0 on success, non-zero if not found.
 */
int fossil_holiday_get(const char *name, const fossil_holiday_t **out_holiday);

/**
 * @brief Compute the date of a holiday for a given year.
 *
 * Fills a fossil_time_date_t structure with the computed date of the holiday.
 *
 * @param holiday    Pointer to the holiday descriptor.
 * @param year       Year for which to compute the holiday.
 * @param out_date   Pointer to fossil_time_date_t to fill.
 * @return 0 on success, non-zero on error.
 */
int fossil_holiday_date(const fossil_holiday_t *holiday, int year, fossil_time_date_t *out_date);

/**
 * @brief Check if a given date matches a registered holiday.
 *
 * @param date       Pointer to the date to check.
 * @param out_name   Output pointer to holiday name if matched, NULL if none.
 * @return Nonzero if the date is a holiday, 0 otherwise.
 */
int fossil_holiday_is(const fossil_time_date_t *date, const char **out_name);

/**
 * @brief List all holidays in a given year.
 *
 * Fills an array of fossil_time_date_t pointers with the dates of all holidays.
 * Caller must allocate the array and provide max_entries.
 *
 * @param year          Year for which to list holidays.
 * @param out_dates     Array to fill with holiday dates.
 * @param max_entries   Maximum number of entries to fill.
 * @param out_count     Number of holidays actually filled.
 * @return 0 on success, non-zero on error.
 */
int fossil_holiday_list(int year, fossil_time_date_t *out_dates, size_t max_entries, size_t *out_count);

#ifdef __cplusplus
} /* extern "C" */

/* ======================================================
 * C++ Interface
 * ====================================================== */
#include <vector>
#include <string>

namespace fossil::time {

    class Holiday {
    public:
        std::string name;
        fossil_holiday_type_t type;
        int month;
        int day;
        int weekday;
        int nth;
        int offset_days;
        std::string relative_to;
    
        /**
         * @brief Default constructor for Holiday.
         * 
         * Initializes a Holiday object with default values. The type is set to
         * FOSSIL_HOLIDAY_CUSTOM, month and day are 0, weekday is -1 (invalid),
         * nth and offset_days are 0, and relative_to is empty.
         */
        Holiday() : type(FOSSIL_HOLIDAY_CUSTOM), month(0), day(0),
                    weekday(-1), nth(0), offset_days(0) {}
    
        /**
         * @brief Register this holiday with the holiday registry.
         * 
         * Converts the C++ Holiday object to a C struct (fossil_holiday_t) and
         * registers it with the fossil holiday system. This makes the holiday
         * available for date computations and queries.
         * 
         * @return 0 on success, non-zero on error.
         */
        inline int register_holiday() const {
            fossil_holiday_t h;
            h.name = name.c_str();
            h.type = type;
            h.month = month;
            h.day = day;
            h.weekday = weekday;
            h.nth = nth;
            h.offset_days = offset_days;
            h.relative_to = relative_to.empty() ? NULL : relative_to.c_str();
            return fossil_holiday_register(&h);
        }
    
        /**
         * @brief Compute the date of this holiday for a given year.
         * 
         * Calculates when this holiday falls in the specified year, taking into
         * account its type (fixed, weekday-based, relative, or custom). Returns
         * a fossil_time_date_t structure containing the computed date.
         * 
         * @param year The year for which to compute the holiday date.
         * @return A fossil_time_date_t structure with the computed holiday date.
         */
        inline fossil_time_date_t date(int year) const {
            fossil_time_date_t dt;
            fossil_holiday_t h;
            h.name = name.c_str();
            h.type = type;
            h.month = month;
            h.day = day;
            h.weekday = weekday;
            h.nth = nth;
            h.offset_days = offset_days;
            h.relative_to = relative_to.empty() ? NULL : relative_to.c_str();
            fossil_holiday_date(&h, year, &dt);
            return dt;
        }
    
        /**
         * @brief Check if a given date matches this holiday.
         * 
         * Determines whether the provided date corresponds to this specific holiday.
         * Compares the computed holiday date for the given year against the
         * provided date.
         * 
         * @param date The date to check against this holiday.
         * @return true if the date matches this holiday, false otherwise.
         */
        inline bool is(const fossil_time_date_t &date) const {
            const char* out_name = nullptr;
            if (fossil_holiday_is(&date, &out_name)) {
                return name == out_name;
            }
            return false;
        }
    
        /**
         * @brief List all registered holidays in a given year.
         * 
         * Retrieves a vector of all holiday dates for the specified year from
         * the fossil holiday registry. The number of holidays returned depends
         * on how many holidays are registered in the system.
         * 
         * @param year The year for which to list holidays.
         * @param max_entries Maximum number of holiday entries to retrieve (default 128).
         * @return A vector of fossil_time_date_t structures containing all holidays in the year.
         */
        static std::vector<fossil_time_date_t> list(int year, size_t max_entries = 128) {
            std::vector<fossil_time_date_t> vec(max_entries);
            size_t count = 0;
            fossil_holiday_list(year, vec.data(), max_entries, &count);
            vec.resize(count);
            return vec;
        }
    };

} /* namespace fossil */

#endif

#endif /* FOSSIL_TIME_HOLIDAY_H */
