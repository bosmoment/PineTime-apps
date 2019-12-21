/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdio.h>

#include "log.h"
#include "lvgl.h"
#include "board.h"
#include "xtimer.h"
#include "hal.h"
#include "gui.h"

#define LVGL_THREAD_NAME    "lvgl"
#define LVGL_THREAD_PRIO    4
#define LVGL_STACKSIZE     (THREAD_STACKSIZE_LARGE)

#define DISPATCHER_THREAD_NAME    "ph_disp"
#define DISPATCHER_THREAD_PRIO    2
#define DISPATCHER_STACKSIZE     (THREAD_STACKSIZE_SMALL)

#define GUI_BUF_SIZE             (LV_HOR_RES_MAX * 6)

static void *_lvgl_thread(void* arg);
static void *_lvgl_dispatcher(void *arg);
static char _stack[LVGL_STACKSIZE];
static char _dispatch_stack[DISPATCHER_STACKSIZE];

static void dispatch_display_flush(event_t *event);

static lv_disp_drv_t _disp_drv;
static lv_disp_buf_t _disp_buf;

static lv_color_t _buf1[GUI_BUF_SIZE];
static lv_color_t _buf2[GUI_BUF_SIZE];

static gui_flush_event_t ev[2] = {
    {
        .super = { .handler = dispatch_display_flush, },
    },
    {
        .super = { .handler = dispatch_display_flush, },
    }
};

extern lv_obj_t *screen_time_create(void);
extern lv_obj_t *screen_menu_create(void);

static event_queue_t _queue;

typedef enum {
    GUI_SCREEN_TIME,
    GUI_SCREEN_MENU,
} gui_screen_t;

typedef struct {
    gui_screen_t type;
    lv_obj_t *(*create)(void);
} gui_screen_map_t;

static const gui_screen_map_t _screen_map[] = {
    { GUI_SCREEN_TIME, screen_time_create },
    { GUI_SCREEN_MENU, screen_menu_create },
};

static void _display_flush_cb(struct _disp_drv_t * disp_drv,
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

int lvgl_thread_create(void)
{
    (void)_buf2;
    lv_disp_buf_init(&_disp_buf, _buf1, _buf2, GUI_BUF_SIZE);
    lv_disp_drv_init(&_disp_drv);            /*Basic initialization*/

    _disp_drv.flush_cb = _display_flush_cb;
    _disp_drv.buffer = &_disp_buf;
    lv_disp_t *display = lv_disp_drv_register(&_disp_drv);

    int res = thread_create(_dispatch_stack, DISPATCHER_STACKSIZE,
                            DISPATCHER_THREAD_PRIO,
                            THREAD_CREATE_STACKTEST, _lvgl_dispatcher,
                            (void *)_lvgl_dispatcher, DISPATCHER_THREAD_NAME);

    res = thread_create(_stack, LVGL_STACKSIZE, LVGL_THREAD_PRIO,
                            THREAD_CREATE_STACKTEST, _lvgl_thread,
                            (void *)display, LVGL_THREAD_NAME);
    return res;
}

static lv_obj_t *_switch_screen(lv_obj_t *active, gui_screen_t screen)
{
    lv_obj_t *next = _screen_map[screen].create();
    lv_scr_load(next);
    if (active) {
        lv_obj_del(active);
    }
    return next;
}

static void *_lvgl_thread(void* arg)
{
    lv_disp_t *display = (lv_disp_t*)arg;
    (void)display;

    lv_theme_t *th = lv_theme_night_init(10, NULL);
    lv_theme_set_current(th);

    lv_obj_t *cur_screen = NULL;

    gui_screen_t selected = GUI_SCREEN_TIME;

    cur_screen = _switch_screen(cur_screen, selected);
    (void)cur_screen;

    xtimer_ticks32_t last_wake = xtimer_now();
    uint32_t count = 0;
    while(1)
    {
        lv_tick_inc(10);
        count++;
        if (count > 200) {
            if (selected == GUI_SCREEN_TIME) {
                selected = GUI_SCREEN_MENU;
            }
            else {
                selected = GUI_SCREEN_TIME;
            }
            count = 0;
            cur_screen = _switch_screen(cur_screen, selected);
        }
        lv_task_handler();
        xtimer_periodic_wakeup(&last_wake, 10 * US_PER_MS);
    }
    assert(false);
    /* should be never reached */
    return NULL;
}

static void dispatch_display_flush(event_t *event)
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

static void *_lvgl_dispatcher(void *arg)
{
    event_queue_init(&_queue);
    event_loop(&_queue);
    return NULL;
}
