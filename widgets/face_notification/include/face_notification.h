/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef WIDGET_FACE_NOTIFICATION_H
#define WIDGET_FACE_NOTIFICATION_H

#include "lvgl.h"
#include "widget.h"
#include "controller.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _face_notif_widget {
    widget_t widget;
    control_event_handler_t handler;
    lv_obj_t *screen;
} face_notification_widget_t;

#ifdef __cplusplus
}
#endif

#endif /* WIDGET_FACE_NOTIFICATION_H */


