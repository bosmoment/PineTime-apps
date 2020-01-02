/*
 * Copyright (C) 2019 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_BLEMAN_TIMESYNC_H
#define APP_BLEMAN_TIMESYNC_H

#include <stdint.h>
#include "event.h"
#include "event/timeout.h"
#include "ts_event.h"
#include "bleman.h"
#include "host/ble_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((packed)) {
    uint16_t year;
    uint8_t month;
    uint8_t dayofmonth;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t millis;
    uint8_t reason;
} bleman_timesync_ble_cts_t;

typedef struct {
    event_t ev;
    event_t start_chrs_ev;
    event_t start_read_ev;
    event_timeout_t timeout_ev;
    bleman_t *bleman;
    struct ble_gatt_svc time_svc;
    struct ble_gatt_chr time_chr;
} bleman_timesync_t;

void bleman_timesync_init(bleman_t *bleman, bleman_timesync_t *sync);
void bleman_timesync_start_events(bleman_timesync_t *sync);

void bleman_timesync_stop_events(bleman_timesync_t *sync);

#ifdef __cplusplus
}
#endif

#endif /* APP_BLEMAN_TIMESYNC_H */


