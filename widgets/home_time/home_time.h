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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _home_time_widget {
    widget_t widget;
    lv_obj_t *screen;
} home_time_widget_t;


#ifdef __cplusplus
}
#endif


#endif /* WIDGET_HOME_TIME_H */

