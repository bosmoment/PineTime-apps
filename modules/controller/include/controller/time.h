/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_CONTROLLER_TIME_H
#define APP_CONTROLLER_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#define  CONTROLLER_TIME_PRESCALER  127

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t dayofmonth;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t fracs;
} controller_time_spec_t;

void controller_time_init(void);

const char *controller_time_month_get_short_name(controller_time_spec_t *time);
const char *controller_time_month_get_long_name(controller_time_spec_t *time);

#ifdef __cplusplus
}
#endif

#endif /* APP_CONTROLLER_TIME_H */
