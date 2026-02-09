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
#ifndef FOSSIL_TIME_SPAN_H
#define FOSSIL_TIME_SPAN_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int64_t days;
    int hours;
    int minutes;
    int seconds;
    int nanoseconds;
} fossil_time_span_t;

fossil_time_span_t fossil_time_span_from_seconds(int64_t seconds);
int64_t fossil_time_span_to_seconds(const fossil_time_span_t *span);
void fossil_time_span_add(fossil_time_date_t *dt, const fossil_time_span_t *span);
void fossil_time_span_normalize(fossil_time_span_t *span);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
namespace fossil {
namespace time {

class Span {
public:
    int64_t days;
    int hours;
    int minutes;
    int seconds;
    int nanoseconds;

    Span();
    static Span from_seconds(int64_t seconds);
    int64_t to_seconds() const;

    void add_to(Date &dt) const;
    void normalize();
};

} // namespace time
} // namespace fossil
#endif

#endif
