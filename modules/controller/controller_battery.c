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

uint16_t controller_battery_get_voltage(controller_battery_t *cbatt)
{
    mutex_lock(&cbatt->lock);
    uint16_t voltage = cbatt->average_millivolts;
    mutex_unlock(&cbatt->lock);
    return voltage;
}


static uint16_t _battery_ewma(controller_battery_t *cbatt, uint16_t measurement)
{
    /* Exponentially weighted moving average */
    if (cbatt->average_millivolts == 0) {
        return measurement;
    }
    else {
        return (CONTROLLER_BATTERY_ALPHA * measurement +
            (10 - CONTROLLER_BATTERY_ALPHA) * cbatt->average_millivolts) / 10;
    }
}

static void _battery_measure_event(event_t *event)
{
    controller_battery_t *cbatt = container_of(event, controller_battery_t, ev);
    uint16_t measurement = hal_battery_read_voltage();
    uint16_t new_average = _battery_ewma(cbatt, measurement);

    mutex_lock(&cbatt->lock);
    cbatt->last_millivolts = measurement;
    cbatt->average_millivolts = new_average;
    mutex_unlock(&cbatt->lock);

    event_timeout_set(&cbatt->evt, CONTROLLER_BATTERY_INTERVAL * US_PER_MS);
}

void controller_battery_init(controller_t *controller, controller_battery_t *cbatt)
{
    cbatt->ev.handler = _battery_measure_event;
    event_timeout_init(&cbatt->evt, &controller->queue, &cbatt->ev);
    _battery_measure_event(&cbatt->ev);
}
