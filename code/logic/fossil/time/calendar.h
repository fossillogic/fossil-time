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

#include "date.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FOSSIL_TIME_SEASON_UNKNOWN,
    FOSSIL_TIME_SEASON_SPRING,
    FOSSIL_TIME_SEASON_SUMMER,
    FOSSIL_TIME_SEASON_AUTUMN,
    FOSSIL_TIME_SEASON_WINTER
} fossil_time_season_t;

int fossil_time_calendar_is_leap_year(int year);
int fossil_time_calendar_days_in_month(int year, int month);
int fossil_time_calendar_is_weekend(const fossil_time_date_t *dt);
int fossil_time_calendar_get_quarter(const fossil_time_date_t *dt);
fossil_time_season_t fossil_time_calendar_get_season(const fossil_time_date_t *dt, int northern_hemisphere);
int fossil_time_calendar_get_holiday(const fossil_time_date_t *dt);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
namespace fossil {
namespace time {

class Calendar {
public:
    static bool is_leap_year(int year);
    static int days_in_month(int year, int month);
    static bool is_weekend(const Date &dt);
    static int get_quarter(const Date &dt);
    static fossil_time_season_t get_season(const Date &dt, bool northern_hemisphere=true);
    static int get_holiday(const Date &dt);
};

} // namespace time
} // namespace fossil
#endif

#endif
