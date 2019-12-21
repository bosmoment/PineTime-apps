/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_GUI
#define APP_GUI

#include <stdint.h>
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    event_t super;
    void *drv;
    uint16_t x1;
    uint16_t x2;
    uint16_t y1;
    uint16_t y2;
    uint16_t *map;
    bool used;
} gui_flush_event_t;


#ifdef __cplusplus
}
#endif

#endif /* APP_HAL */
