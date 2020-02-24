/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _APP_BLEMAN_HRS_H
#define _APP_BLEMAN_HRS_H

#include <stdint.h>
#include "event.h"
#include "event/timeout.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Controller battery measurement interval in milliseconds
 */
#ifndef BLEMAN_HEART_RATE_UPDATE_INTERVAL
#define BLEMAN_HEART_RATE_UPDATE_INTERVAL     1 * MS_PER_SEC
#endif

typedef struct {
    event_timeout_t evt;        /**< Event timeout for HRS notifications  */
    event_t ev;                 /**< Event for HRS notifications */
    uint16_t bpm;         /**< Last submitted heart rate */
    int step;
    uint16_t handle;            /**< notification handle */
} bleman_hrs_t;

/**
 * @brief handler for bleman heart rate service requests
 */
int bleman_hrs_handler(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt, void *arg);

/**
 * @brief Enable or disable bleman heart rate service notifications
 */
void bleman_hrs_notify(bleman_hrs_t *hrs, bool enable);

#ifdef __cplusplus
}
#endif

#endif /* APP_BLEMAN_BAS_H */

