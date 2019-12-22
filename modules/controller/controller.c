/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include <errno.h>
#include "controller.h"
#include "log.h"
#include "xtimer.h"
#include "gui.h"
#include "ts_event.h"

#define CONTROLLER_THREAD_NAME    "controller"
#define CONTROLLER_THREAD_PRIO    6
#define CONTROLLER_STACKSIZE      (THREAD_STACKSIZE_DEFAULT)

static void *_control_thread(void* arg);
static char _stack[CONTROLLER_STACKSIZE];

static controller_t _control;

static void _handle_input_event(event_t *event);

typedef struct {
    ts_event_t super;
    widget_t *widget;
    controller_event_widget_t event;
} controller_widget_event_t;

static controller_widget_event_t ev_widget = {
    .super = { .super = { .handler = _handle_input_event } }
};

static void _handle_input_event(event_t *event)
{
    controller_widget_event_t *ev = (controller_widget_event_t*)event;
    switch(ev->event) {
        case CONTROLLER_EVENT_WIDGET_LEAVE:
            gui_event_submit_switch_widget(widget_get_menu());
            break;
        case CONTROLLER_EVENT_WIDGET_HOME:
            gui_event_submit_switch_widget(widget_get_home());
            break;
        default:
            break;
    }
    ts_event_clear(&ev->super);
}

int controller_event_submit_input_event(widget_t *widget, controller_event_widget_t event)
{
    if (ts_event_claim(&ev_widget.super) == -EBUSY) {
        return -EBUSY;
    }
    LOG_INFO("Submitting event\n");
    ev_widget.widget = widget;
    ev_widget.event = event;
    event_post(&_control.queue, &ev_widget.super.super);
    return 0;
}

int controller_thread_create(void)
{
    int res = thread_create(_stack, CONTROLLER_STACKSIZE, CONTROLLER_THREAD_PRIO,
                            THREAD_CREATE_STACKTEST, _control_thread,
                            &_control, CONTROLLER_THREAD_NAME);
    return res;
}

static void *_control_thread(void* arg)
{
    event_queue_init(&_control.queue);
    gui_event_submit_switch_widget(widget_get_home());
    while(1)
    {
        thread_flags_t flags = thread_flags_wait_any(
            THREAD_FLAG_EVENT
            );
        if (flags & THREAD_FLAG_EVENT) {
            event_t *event = event_get(&_control.queue);
            if (event) {
                event->handler(event);
            }
        }
    }
    assert(false);
    /* should be never reached */
    return NULL;
}
