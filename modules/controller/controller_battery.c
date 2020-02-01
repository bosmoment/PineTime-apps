/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <errno.h>

#include "log.h"

#include "controller.h"
#include "controller/battery.h"

static void _battery_ewma(controller_battery_t *cbatt, uint16_t measurement)
{
    /* Exponentially weighted moving average */
    if (cbatt->average_millivolts == 0) {
        cbatt->average_millivolts = measurement;
    }
    else {
        cbatt->average_millivolts = (CONTROLLER_BATTERY_ALPHA * measurement +
            (10 - CONTROLLER_BATTERY_ALPHA) * cbatt->average_millivolts) / 10;
    }
}

static void _battery_measure_event(event_t *event)
{
    controller_battery_t *cbatt = container_of(event, controller_battery_t, ev);
    uint16_t measurement = hal_battery_read_voltage();

    cbatt->last_millivolts = measurement;
    _battery_ewma(cbatt, measurement);
    event_timeout_set(&cbatt->evt, CONTROLLER_BATTERY_INTERVAL * US_PER_MS);
}

void controller_battery_init(controller_t *controller, controller_battery_t *cbatt)
{
    cbatt->ev.handler = _battery_measure_event;
    event_timeout_init(&cbatt->evt, &controller->queue, &cbatt->ev);
    _battery_measure_event(&cbatt->ev);
}
