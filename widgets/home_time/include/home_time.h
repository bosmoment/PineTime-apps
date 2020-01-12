/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef WIDGET_HOME_TIME_H
#define WIDGET_HOME_TIME_H

#include "lvgl.h"
#include "widget.h"
#include "controller.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _home_time_widget {
    widget_t widget;
    control_event_handler_t handler;
    lv_obj_t *screen;
    lv_obj_t *lv_time;
    lv_obj_t *lv_date;
    lv_obj_t *lv_second_meter;
    lv_obj_t *lv_ble;
    lv_obj_t *lv_power;
    bleman_ble_state_t ble_state;
    /* Shared storage between gui and control */
    controller_time_spec_t time;
    unsigned percentage;
    bool charging;
    bool powered;
} home_time_widget_t;


#ifdef __cplusplus
}
#endif


#endif /* WIDGET_HOME_TIME_H */

