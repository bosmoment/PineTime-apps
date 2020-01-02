/*
 * Copyright (C) 2019 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_BLEMAN_H
#define APP_BLEMAN_H

#include <stdint.h>
#include "event.h"
#include "ts_event.h"
#include "widget.h"
#include "thread.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_BLEMAN_DEVICE_NAME
#define CONFIG_BLEMAN_DEVICE_NAME   "RIOT PineTime Smart Watch"
#endif

#ifndef CONFIG_BLEMAN_MANUF_NAME
#define CONFIG_BLEMAN_MANUF_NAME     "BosMoment"
#endif

#ifndef CONFIG_BLEMAN_MODEL_NUM
#define CONFIG_BLEMAN_MODEL_NUM     "1A"
#endif

#ifndef CONFIG_BLEMAN_FW_VERSION
#define CONFIG_BLEMAN_FW_VERSION    "1.0.0-alpha1"
#endif

#ifndef CONFIG_BLEMAN_HW_VERSION
#define CONFIG_BLEMAN_HW_VERSION    "1"
#endif

#ifndef CONFIG_BLEMAN_SERIAL_LEN
#define CONFIG_BLEMAN_SERIAL_LEN    24
#endif


typedef struct _bleman {
    char serial[CONFIG_BLEMAN_SERIAL_LEN];
    event_queue_t eq;
    uint16_t conn_handle;

} bleman_t;

int bleman_thread_create(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_BLEMAN_H */
