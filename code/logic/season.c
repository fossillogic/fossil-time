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
#include "fossil/time/season.h"

/* ======================================================
 * Internal helpers
 * ====================================================== */

static fossil_season_t invert_hemisphere(fossil_season_t s)
{
    switch (s) {
        case FOSSIL_SEASON_SPRING: return FOSSIL_SEASON_AUTUMN;
        case FOSSIL_SEASON_SUMMER: return FOSSIL_SEASON_WINTER;
        case FOSSIL_SEASON_AUTUMN: return FOSSIL_SEASON_SPRING;
        case FOSSIL_SEASON_WINTER: return FOSSIL_SEASON_SUMMER;
        default: return FOSSIL_SEASON_UNKNOWN;
    }
}

/* ======================================================
 * Meteorological seasons (exact month ranges)
 * ====================================================== */

static fossil_season_t season_meteorological(int month)
{
    if (month >= 3 && month <= 5)  return FOSSIL_SEASON_SPRING;
    if (month >= 6 && month <= 8)  return FOSSIL_SEASON_SUMMER;
    if (month >= 9 && month <= 11) return FOSSIL_SEASON_AUTUMN;
    return FOSSIL_SEASON_WINTER;
}

/* ======================================================
 * Astronomical seasons (approximate boundaries)
 *
 * These are stable approximations good for civil use.
 * Precision: ±1 day worst case.
 *
 * Spring  ≈ Mar 20
 * Summer  ≈ Jun 21
 * Autumn  ≈ Sep 22
 * Winter  ≈ Dec 21
 * ====================================================== */

static fossil_season_t season_astronomical(int month, int day)
{
    /* Winter: Dec 21 → Mar 19 */
    if ((month == 12 && day >= 21) ||
        (month == 1) ||
        (month == 2) ||
        (month == 3 && day < 20))
        return FOSSIL_SEASON_WINTER;

    /* Spring: Mar 20 → Jun 20 */
    if ((month == 3 && day >= 20) ||
        (month == 4) ||
        (month == 5) ||
        (month == 6 && day < 21))
        return FOSSIL_SEASON_SPRING;

    /* Summer: Jun 21 → Sep 21 */
    if ((month == 6 && day >= 21) ||
        (month == 7) ||
        (month == 8) ||
        (month == 9 && day < 22))
        return FOSSIL_SEASON_SUMMER;

    /* Autumn: Sep 22 → Dec 20 */
    return FOSSIL_SEASON_AUTUMN;
}

/* ======================================================
 * Public API
 * ====================================================== */

fossil_season_t fossil_season_of(
    const fossil_time_date_t *date,
    fossil_hemisphere_t hemisphere,
    fossil_season_system_t system
) {
    if (!date)
        return FOSSIL_SEASON_UNKNOWN;

    if (!(date->precision_mask & FOSSIL_TIME_PRECISION_MONTH))
        return FOSSIL_SEASON_UNKNOWN;

    int month = date->month;
    int day   = (date->precision_mask & FOSSIL_TIME_PRECISION_DAY) ? date->day : 1;

    fossil_season_t s;

    if (system == FOSSIL_SEASON_ASTRONOMICAL)
        s = season_astronomical(month, day);
    else
        s = season_meteorological(month);

    if (hemisphere == FOSSIL_HEMISPHERE_SOUTHERN)
        s = invert_hemisphere(s);

    return s;
}

const char* fossil_season_name(fossil_season_t season)
{
    switch (season) {
        case FOSSIL_SEASON_SPRING: return "Spring";
        case FOSSIL_SEASON_SUMMER: return "Summer";
        case FOSSIL_SEASON_AUTUMN: return "Autumn";
        case FOSSIL_SEASON_WINTER: return "Winter";
        default: return "Unknown";
    }
}

int fossil_season_is(
    const fossil_time_date_t *date,
    fossil_season_t season,
    fossil_hemisphere_t hemisphere,
    fossil_season_system_t system
) {
    if (!date)
        return 0;

    fossil_season_t actual = fossil_season_of(date, hemisphere, system);
    return actual == season;
}
