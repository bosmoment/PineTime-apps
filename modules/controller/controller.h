/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_CONTROLLER
#define APP_CONTROLLER

#include <stdint.h>
#include "event.h"
#include "gui.h"
#include "widget.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    event_queue_t queue;
    gui_screen_t cur_screen;
} controller_t;

typedef enum {
    CONTROLLER_EVENT_WIDGET_LEAVE, /* Leave widget and return to menu */
    CONTROLLER_EVENT_WIDGET_HOME, /* Return to home screen */
} controller_event_widget_t;

int controller_event_submit_input_event(widget_t *widget,
                                        controller_event_widget_t event);

int controller_thread_create(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_CONTROLLER */
