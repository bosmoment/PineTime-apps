/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_CONTROLLER_BATTERY_H
#define APP_CONTROLLER_BATTERY_H

#include <stdint.h>
#include "event.h"
#include "event/timeout.h"
#include "xtimer.h"
#include "controller.h"
#include "controller/structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Controller battery measurement interval in milliseconds
 */
#ifndef CONTROLLER_BATTERY_INTERVAL
#define CONTROLLER_BATTERY_INTERVAL     10 * MS_PER_SEC
#endif

/**
 * @brief Controller battery voltage EWMA alpha value multiplied by ten
 *
 * Try to keep this an integer value
 */
#ifndef CONTROLLER_BATTERY_ALPHA
#define CONTROLLER_BATTERY_ALPHA        1
#endif

static inline uint16_t controller_battery_get_voltage(controller_battery_t *cbatt)
{
    return cbatt->average_millivolts;
}

void controller_battery_init(controller_t *controller, controller_battery_t *cbatt);

#ifdef __cplusplus
}
#endif

#endif /* APP_CONTROLLER_BATTERY_H */

