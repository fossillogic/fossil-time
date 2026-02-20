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
#ifndef FOSSIL_TIME_SEASON_H
#define FOSSIL_TIME_SEASON_H

#include "fossil_time_date.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================
 * Fossil Time — Season API
 * ====================================================== */

/**
 * @brief Season enumeration.
 */
typedef enum fossil_season_t {
    FOSSIL_SEASON_UNKNOWN = 0,
    FOSSIL_SEASON_SPRING,
    FOSSIL_SEASON_SUMMER,
    FOSSIL_SEASON_AUTUMN,
    FOSSIL_SEASON_WINTER
} fossil_season_t;

/**
 * @brief Hemisphere enumeration.
 */
typedef enum fossil_hemisphere_t {
    FOSSIL_HEMISPHERE_NORTHERN = 0,
    FOSSIL_HEMISPHERE_SOUTHERN = 1
} fossil_hemisphere_t;

/**
 * @brief Season system.
 *
 * Meteorological:
 *   Spring = Mar–May
 *   Summer = Jun–Aug
 *   Autumn = Sep–Nov
 *   Winter = Dec–Feb
 *
 * Astronomical:
 *   Based on equinox/solstice approximations.
 */
typedef enum fossil_season_system_t {
    FOSSIL_SEASON_METEOROLOGICAL = 0,
    FOSSIL_SEASON_ASTRONOMICAL   = 1
} fossil_season_system_t;

/* ======================================================
 * C API — Core
 * ====================================================== */

/**
 * @brief Determine the season for a given date.
 *
 * @param date        Date to evaluate.
 * @param hemisphere  Hemisphere of the observer.
 * @param system      Season system to use.
 * @return Season enumeration value.
 */
fossil_season_t fossil_season_of(
    const fossil_time_date_t *date,
    fossil_hemisphere_t hemisphere,
    fossil_season_system_t system
);

/**
 * @brief Get a human-readable name for a season.
 *
 * @param season Season enumeration.
 * @return Constant string name.
 */
const char* fossil_season_name(fossil_season_t season);

/**
 * @brief Check if a date falls within a specified season.
 *
 * @param date        Date to check.
 * @param season      Season to test.
 * @param hemisphere  Hemisphere of the observer.
 * @param system      Season system to use.
 * @return Nonzero if the date is within the season.
 */
int fossil_season_is(
    const fossil_time_date_t *date,
    fossil_season_t season,
    fossil_hemisphere_t hemisphere,
    fossil_season_system_t system
);

#ifdef __cplusplus
} /* extern "C" */

/* ======================================================
 * C++ Interface
 * ====================================================== */

#include <string>

namespace fossil {
namespace time {

class Season {
public:
    fossil_season_t value;

    Season() : value(FOSSIL_SEASON_UNKNOWN) {}
    Season(fossil_season_t s) : value(s) {}

    static Season of(
        const Date &date,
        fossil_hemisphere_t hemi = FOSSIL_HEMISPHERE_NORTHERN,
        fossil_season_system_t sys = FOSSIL_SEASON_METEOROLOGICAL
    ) {
        return Season(
            fossil_season_of(&date.raw, hemi, sys)
        );
    }

    std::string name() const {
        return fossil_season_name(value);
    }

    bool is(
        const Date &date,
        fossil_hemisphere_t hemi = FOSSIL_HEMISPHERE_NORTHERN,
        fossil_season_system_t sys = FOSSIL_SEASON_METEOROLOGICAL
    ) const {
        return fossil_season_is(&date.raw, value, hemi, sys) != 0;
    }
};

} /* namespace time */
} /* namespace fossil */

#endif

#endif /* FOSSIL_TIME_SEASON_H */
