/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _APP_BLEMAN_BAS_H
#define _APP_BLEMAN_BAS_H

#include <stdint.h>
#include "event.h"
#include "event/timeout.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Controller battery measurement interval in milliseconds
 */
#ifndef BLEMAN_BATTERY_UPDATE_INTERVAL
#define BLEMAN_BATTERY_UPDATE_INTERVAL     30 * MS_PER_SEC
#endif

typedef struct {
    event_timeout_t evt;        /**< Event timeout for battery notifications  */
    event_t ev;                 /**< Event for battery notifications */
    uint8_t last_percentage;    /**< Last submitted notification percentage */
    uint16_t handle;            /**< notification handle */
} bleman_bas_t;

/**
 * @brief handler for bleman battery service requests
 */
int bleman_bas_handler(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt, void *arg);

/**
 * @brief Enable or disable bleman battery service notifications
 */
void bleman_bas_notify(bleman_bas_t *bas, bool enable);

#ifdef __cplusplus
}
#endif

#endif /* APP_CONTROLLER_BATTERY_H */
