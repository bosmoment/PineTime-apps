/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef WIDGET_HOME_TERMINAL_H
#define WIDGET_HOME_TERMINAL_H

#include "lvgl.h"
#include "widget.h"
#include "controller.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _home_terminal_widget {
    widget_t widget;
    control_event_handler_t handler;
    lv_obj_t *screen;
    lv_obj_t *lv_string;
    bleman_ble_state_t ble_state;
    /* Shared storage between gui and control */
    controller_time_spec_t time;
    uint32_t millivolts;
    uint32_t quartertemp;
    bool charging;
    bool powered;
} home_terminal_widget_t;


#ifdef __cplusplus
}
#endif


#endif /* WIDGET_HOME_TERMINAL_H */

