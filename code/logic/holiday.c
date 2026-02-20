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
#include "fossil/time/holiday.h"
#include <string.h>

/* ======================================================
 * Internal Holiday Registry
 * ====================================================== */

#define FOSSIL_MAX_HOLIDAYS 128

static fossil_holiday_t g_holidays[FOSSIL_MAX_HOLIDAYS];
static size_t g_holiday_count = 0;

/* ======================================================
 * Utilities
 * ====================================================== */

static int is_leap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static int days_in_month(int y, int m) {
    static const int d[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (m == 2) return d[1] + is_leap(y);
    return d[m-1];
}

/* Zeller-style weekday: 0=Sun..6=Sat */
static int weekday_of(int y, int m, int d) {
    if (m < 3) { m += 12; y--; }
    int K = y % 100;
    int J = y / 100;
    int h = (d + (13*(m+1))/5 + K + K/4 + J/4 + 5*J) % 7;
    return (h + 6) % 7;
}

static void set_date(fossil_time_date_t *dt, int y, int m, int d) {
    memset(dt, 0, sizeof(*dt));
    dt->year  = y;
    dt->month = m;
    dt->day   = d;
    dt->weekday = weekday_of(y,m,d);
    dt->precision_mask =
        FOSSIL_TIME_PRECISION_YEAR |
        FOSSIL_TIME_PRECISION_MONTH |
        FOSSIL_TIME_PRECISION_DAY;
}

/* nth weekday of month */
static void nth_weekday(int year, int month, int weekday, int nth,
                        fossil_time_date_t *out)
{
    int d = 1;
    int w = weekday_of(year,month,d);

    int delta = (weekday - w + 7) % 7;
    d += delta + (nth-1)*7;

    set_date(out,year,month,d);
}

/* last weekday of month */
static void last_weekday(int year,int month,int weekday,
                         fossil_time_date_t *out)
{
    int d = days_in_month(year,month);
    int w = weekday_of(year,month,d);
    int delta = (w - weekday + 7) % 7;
    d -= delta;
    set_date(out,year,month,d);
}

/* observed rule: if Sat→Fri, Sun→Mon */
static void apply_observed(fossil_time_date_t *dt)
{
    if (dt->weekday == 6) { /* Sat */
        dt->day -= 1;
    }
    else if (dt->weekday == 0) { /* Sun */
        dt->day += 1;
    }
    dt->weekday = weekday_of(dt->year,dt->month,dt->day);
}

/* ======================================================
 * Registry API
 * ====================================================== */

int fossil_holiday_register(const fossil_holiday_t *h)
{
    if (!h || g_holiday_count >= FOSSIL_MAX_HOLIDAYS) return -1;
    g_holidays[g_holiday_count++] = *h;
    return 0;
}

int fossil_holiday_get(const char *name,
                       const fossil_holiday_t **out)
{
    for (size_t i=0;i<g_holiday_count;i++) {
        if (strcmp(g_holidays[i].name,name)==0) {
            if (out) *out = &g_holidays[i];
            return 0;
        }
    }
    return -1;
}

/* ======================================================
 * Compute Holiday Date
 * ====================================================== */

int fossil_holiday_date(const fossil_holiday_t *h,
                        int year,
                        fossil_time_date_t *out)
{
    if (!h || !out) return -1;

    switch (h->type) {

        case FOSSIL_HOLIDAY_FIXED:
            set_date(out,year,h->month,h->day);
            apply_observed(out);
            return 0;

        case FOSSIL_HOLIDAY_WEEKDAY:
            nth_weekday(year,h->month,h->weekday,h->nth,out);
            return 0;

        case FOSSIL_HOLIDAY_RELATIVE: {
            const fossil_holiday_t *base;
            if (fossil_holiday_get(h->relative_to,&base)) return -1;
            fossil_time_date_t dt;
            fossil_holiday_date(base,year,&dt);

            dt.day += h->offset_days;

            /* normalize day */
            while (dt.day > days_in_month(year,dt.month)) {
                dt.day -= days_in_month(year,dt.month);
                dt.month++;
            }
            while (dt.day < 1) {
                dt.month--;
                dt.day += days_in_month(year,dt.month);
            }
            dt.weekday = weekday_of(year,dt.month,dt.day);
            *out = dt;
            return 0;
        }

        default:
            return -1;
    }
}

/* ======================================================
 * Query Functions
 * ====================================================== */

int fossil_holiday_is(const fossil_time_date_t *date,
                      const char **out_name)
{
    fossil_time_date_t tmp;

    for (size_t i=0;i<g_holiday_count;i++) {
        fossil_holiday_date(&g_holidays[i],date->year,&tmp);
        if (tmp.month==date->month && tmp.day==date->day) {
            if (out_name) *out_name = g_holidays[i].name;
            return 1;
        }
    }
    return 0;
}

int fossil_holiday_list(int year,
                        fossil_time_date_t *out_dates,
                        size_t max,
                        size_t *out_count)
{
    size_t n=0;
    for (size_t i=0;i<g_holiday_count && n<max;i++) {
        fossil_holiday_date(&g_holidays[i],year,&out_dates[n++]);
    }
    if (out_count) *out_count = n;
    return 0;
}

/* ======================================================
 * Default U.S. Federal Holidays
 * ====================================================== */

static void register_us_holidays(void)
{
    if (g_holiday_count) return;

    fossil_holiday_t h[] = {

        {"New Year's Day",      FOSSIL_HOLIDAY_FIXED,   1,1,0,0,0,NULL},
        {"MLK Day",             FOSSIL_HOLIDAY_WEEKDAY, 1,0,1,3,0,NULL}, /* 3rd Mon Jan */
        {"Presidents Day",      FOSSIL_HOLIDAY_WEEKDAY, 2,0,1,3,0,NULL},
        {"Memorial Day",        FOSSIL_HOLIDAY_WEEKDAY, 5,0,1,5,0,NULL}, /* last Mon */
        {"Juneteenth",          FOSSIL_HOLIDAY_FIXED,   6,19,0,0,0,NULL},
        {"Independence Day",    FOSSIL_HOLIDAY_FIXED,   7,4,0,0,0,NULL},
        {"Labor Day",           FOSSIL_HOLIDAY_WEEKDAY, 9,0,1,1,0,NULL},
        {"Columbus Day",        FOSSIL_HOLIDAY_WEEKDAY,10,0,1,2,0,NULL},
        {"Veterans Day",        FOSSIL_HOLIDAY_FIXED,  11,11,0,0,0,NULL},
        {"Thanksgiving",        FOSSIL_HOLIDAY_WEEKDAY,11,0,4,4,0,NULL},
        {"Christmas",           FOSSIL_HOLIDAY_FIXED,  12,25,0,0,0,NULL}
    };

    for (size_t i=0;i<sizeof(h)/sizeof(*h);i++)
        fossil_holiday_register(&h[i]);
}

/* Auto-init on first call */
__attribute__((constructor))
static void fossil_holiday_init(void)
{
    register_us_holidays();
}
