/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "log.h"
#include "thread.h"
#include "gui.h"
#include "gui/dispatcher.h"
#include "hal.h"

#define DISPATCHER_THREAD_NAME    "gui_disp"
#define DISPATCHER_THREAD_PRIO    5
#define DISPATCHER_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)

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
    gui_t *gui = gui_get_ctx();
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

    flush_event->direction = gui->refresh_mode;
    if ((area->y2 >= (HAL_DISPLAY_ROWS_VISIBLE - 1) && gui->refresh_mode == GUI_SCROLL_DIRECTION_DOWN) ||
        (area->y1 == 0 && gui->refresh_mode == GUI_SCROLL_DIRECTION_UP)) {

        LOG_INFO("[gui]: disabling full refresh\n");
        lv_disp_set_direction(gui->display, 0);
        gui->refresh_mode = GUI_SCROLL_DIRECTION_NONE;
    }

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
    gui_t *gui = gui_get_ctx();
    gui_flush_event_t *flush_event =
        (gui_flush_event_t*)event;


    if (flush_event->direction == GUI_SCROLL_DIRECTION_DOWN && flush_event->y1 == 0) {
            /* Next full render starts here */
            gui->offset = (gui->offset + HAL_DISPLAY_ROWS_VISIBLE) % HAL_DISPLAY_ROWS_TOTAL;
    }
    else if (flush_event->direction == GUI_SCROLL_DIRECTION_UP && flush_event->y2 == (HAL_DISPLAY_ROWS_VISIBLE - 1)) {
            gui->offset = ((gui->offset + HAL_DISPLAY_ROWS_TOTAL) - HAL_DISPLAY_ROWS_VISIBLE) % HAL_DISPLAY_ROWS_TOTAL;
    }

    /* Calculate shifts based on the current render offset */
    uint16_t y1_shift = (flush_event->y1 + gui->offset) % HAL_DISPLAY_ROWS_TOTAL;
    uint16_t y2_shift = (flush_event->y2 + gui->offset) % HAL_DISPLAY_ROWS_TOTAL;

    /* Use two transactions when the render is split over the screen end */
    if (y2_shift < y1_shift) {
        /* Requires 2 transmissions */
        hal_display_flush(hal_display_get_context(),
                          flush_event->x1,
                          flush_event->x2,
                          y1_shift,
                          HAL_DISPLAY_ROWS_TOTAL - 1,
                          flush_event->map);
        size_t pix_offset = (flush_event->x2 - flush_event->x1 + 1) * (HAL_DISPLAY_ROWS_TOTAL - y1_shift);
        hal_display_flush(hal_display_get_context(),
                          flush_event->x1,
                          flush_event->x2,
                          0,
                          y2_shift,
                          flush_event->map + pix_offset);
    }
    else {
        hal_display_flush(hal_display_get_context(),
                          flush_event->x1,
                          flush_event->x2,
                          y1_shift,
                          y2_shift,
                          flush_event->map);
    }

    if (flush_event->direction == GUI_SCROLL_DIRECTION_DOWN) {
        uint16_t new_shift = (gui->offset + flush_event->y2 + 1) % HAL_DISPLAY_ROWS_TOTAL;
        uint16_t scroll = (HAL_DISPLAY_ROWS_TOTAL - new_shift + HAL_DISPLAY_ROWS_VISIBLE) % HAL_DISPLAY_ROWS_TOTAL;
        hal_display_scroll(scroll);
    }
    else if (flush_event->direction == GUI_SCROLL_DIRECTION_UP) {
        uint16_t scroll = (HAL_DISPLAY_ROWS_TOTAL - y1_shift) % HAL_DISPLAY_ROWS_TOTAL;
        hal_display_scroll(scroll);
    }

    flush_event->used = 0;
    lv_disp_flush_ready(flush_event->drv);
}

static void *_gui_dispatcher(void *arg)
{
    event_queue_init(&_queue);
    event_loop(&_queue);
    return NULL;
}
