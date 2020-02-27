/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "thread.h"
#include "gui.h"
#include "gui/dispatcher.h"
#include "hal.h"

#define DISPATCHER_THREAD_NAME    "gui_disp"
#define DISPATCHER_THREAD_PRIO    5
#define DISPATCHER_STACKSIZE     (THREAD_STACKSIZE_SMALL)

static void *_gui_dispatcher(void *arg);
static char _dispatch_stack[DISPATCHER_STACKSIZE];

static void _gui_dispatcher_display_flush(event_t *event);

static event_queue_t _queue;

/* Buffer flush events */
static gui_flush_event_t ev[2] = {
    {
        .super = { .handler = _gui_dispatcher_display_flush, },
    },
    {
        .super = { .handler = _gui_dispatcher_display_flush, },
    }
};

/* Called from the main gui thread */
void gui_dispatcher_display_flush_cb(struct _disp_drv_t * disp_drv,
                              const lv_area_t * area, lv_color_t * color_p)
{
    gui_flush_event_t *flush_event = NULL;
    for (size_t i = 0; i < ARRAY_SIZE(ev); i++) {
        if (ev[i].used == 0) {
            flush_event = &ev[i];
            break;
        }
    }
    assert(flush_event);

    flush_event->used = 1;
    flush_event->x1 = area->x1;
    flush_event->x2 = area->x2;
    flush_event->y1 = area->y1;
    flush_event->y2 = area->y2;
    flush_event->map = (uint16_t*)color_p;
    flush_event->drv = disp_drv;

    event_post(&_queue, (event_t*)flush_event);
    /* Dispatch event */
}

int gui_dispatcher_thread_create(gui_t *gui)
{
    int res = thread_create(_dispatch_stack, DISPATCHER_STACKSIZE,
                            DISPATCHER_THREAD_PRIO,
                            THREAD_CREATE_STACKTEST, _gui_dispatcher,
                            NULL, DISPATCHER_THREAD_NAME);
    return res;
}

static void _gui_dispatcher_display_flush(event_t *event)
{
    gui_flush_event_t *flush_event =
        (gui_flush_event_t*)event;

    hal_display_flush(hal_display_get_context(),
                      flush_event->x1, flush_event->x2,
                      flush_event->y1, flush_event->y2,
                      flush_event->map);
    flush_event->used = 0;
    lv_disp_flush_ready(flush_event->drv);
}

static void *_gui_dispatcher(void *arg)
{
    event_queue_init(&_queue);
    event_loop(&_queue);
    return NULL;
}
