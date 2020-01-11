/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include "log.h"
#include "irq.h"
#include "lvgl.h"
#include "board.h"
#include "xtimer.h"
#include "hal.h"
#include "hal_input.h"
#include "gui.h"
#include "gui/dispatcher.h"
#include "gui/theme.h"
#include "ts_event.h"
#include "widget.h"
#include "event/timeout.h"

#define LVGL_THREAD_NAME    "lvgl"
#define LVGL_THREAD_PRIO    6
#define LVGL_STACKSIZE     (THREAD_STACKSIZE_LARGE)

#define GUI_BUF_SIZE             (LV_HOR_RES_MAX * 6)

#define GUI_THREAD_FLAG_LVGL_HANDLE    (1 << 5)
#define GUI_THREAD_FLAG_IDLE           (1 << 6)
#define GUI_THREAD_FLAG_WAKE           (1 << 7)

static void *_lvgl_thread(void* arg);
static char _stack[LVGL_STACKSIZE];

static gui_t _gui;

static unsigned press_hist[2];
static hal_input_coord_t _coord;

static lv_color_t _buf1[GUI_BUF_SIZE];
static lv_color_t _buf2[GUI_BUF_SIZE];

extern lv_obj_t *screen_time_create(void);
extern lv_obj_t *screen_menu_create(void);

static void _gui_event_switch_widget_draw(event_t *event);

static gui_event_widget_switch_t ev_sw = {
    .super = { .super = { .handler = _gui_event_switch_widget_draw} }
};

gui_t *gui_get_ctx(void)
{
    return &_gui;
}

static bool _input_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    hal_input_coord_t coord;
    int res = hal_input_get_measurement(hal_input_get_context(), &coord);
    unsigned press = res == 1 ? 1 : 0;
    for (size_t i = 0; i < ARRAY_SIZE(press_hist) - 1; i++) {
        press &= press_hist[i];
        press_hist[i] = press_hist[i+1];
    }
    press_hist[ARRAY_SIZE(press_hist) - 1] = res;
    if (press) {
        memcpy(&_coord, &coord, sizeof(coord));
    }
    data->point.x = _coord.x;
    data->point.y = _coord.y;
    data->state = press == 1 ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    return false;
}

/* Switch the screen displayed */
static void _switch_widget_draw(gui_t *gui, widget_t *widget)
{
    LOG_INFO("[GUI]: switching screen to \"%s\"\n", widget->spec->name);
    if (widget == gui->active_widget) {
        return;
    }
    widget_draw(widget);
    lv_scr_load(widget->spec->container(widget));
    if (gui->active_widget) {
        widget_close(gui->active_widget);
    }
    gui->active_widget = widget;
}

static void _gui_event_switch_widget_draw(event_t *event)
{
    gui_event_widget_switch_t *sw = (gui_event_widget_switch_t *)event;
    _switch_widget_draw(gui_get_ctx(), sw->widget);
    ts_event_clear(&sw->super);
}

int gui_event_submit_switch_widget(widget_t *widget)
{
    gui_t *gui = gui_get_ctx();
    if (ts_event_claim(&ev_sw.super) == -EBUSY) {
        return -EBUSY;
    }
    LOG_INFO("[GUI]: Submitting widget switch event\n");
    ev_sw.widget = widget;
    event_post(&gui->queue, &ev_sw.super.super);
    return 0;
}

int lvgl_thread_create(void)
{
    gui_t *gui = &_gui;
    lv_disp_buf_init(&gui->disp_buf, _buf1, _buf2, GUI_BUF_SIZE);
    lv_disp_drv_init(&gui->disp_drv);            /*Basic initialization*/
    lv_indev_drv_init(&gui->indev_drv);

    gui_dispatcher_thread_create(gui);
    gui->disp_drv.buffer = &gui->disp_buf;
    gui->indev_drv.type = LV_INDEV_TYPE_POINTER;
    gui->indev_drv.read_cb = _input_read_cb;

    gui->display = lv_disp_drv_register(&gui->disp_drv);
    lv_indev_drv_register(&gui->indev_drv);

    int res = thread_create(_stack, LVGL_STACKSIZE, LVGL_THREAD_PRIO,
                            THREAD_CREATE_STACKTEST, _lvgl_thread,
                            gui, LVGL_THREAD_NAME);
    return res;
}

static void _gui_lvgl_trigger(void *arg)
{
    gui_t *gui = arg;
    thread_flags_set((thread_t*)sched_threads[gui->pid], GUI_THREAD_FLAG_LVGL_HANDLE);
    xtimer_set(&gui->lvgl_loop, CONFIG_GUI_LVGL_LOOP_TIME);
}

static void _gui_button_irq(void *arg)
{
    gui_t *gui = (gui_t*)arg;
    /* Button pressed */
    puts("[gui] BUTTON!");
    event_post(&gui->queue, &gui->button_press);
}

static void _gui_button_event(event_t *event)
{
    gui_t *gui = container_of(event, gui_t, button_press);
    LOG_INFO("[gui] Screen on\n");
    hal_display_on();
    event_timeout_clear(&gui->screen_timeout_ev);
    event_timeout_set(&gui->screen_timeout_ev, CONFIG_GUI_SCREEN_TIMEOUT);
}

static void _gui_screen_timeout(event_t *event)
{
    LOG_INFO("[gui] Screen off after timeout\n");
    gui_t *gui = container_of(event, gui_t, screen_timeout);
    (void)gui;
    hal_display_off();
}

static void *_lvgl_thread(void* arg)
{
    gui_t *gui = (gui_t*)arg;
    gui->pid = thread_getpid();

    gui->lvgl_loop.callback = _gui_lvgl_trigger;
    gui->lvgl_loop.arg = gui;

    lv_theme_t *th = gui_theme_init(10, NULL);
    lv_theme_set_current(th);

    event_queue_claim(&gui->queue);

    /* Button events */
    gui->button_press.handler = _gui_button_event;
    gui->screen_timeout.handler = _gui_screen_timeout;
    event_timeout_init(&gui->screen_timeout_ev, &gui->queue, &gui->screen_timeout);

    hal_set_button_cb(_gui_button_irq, gui);

    event_timeout_set(&gui->screen_timeout_ev, CONFIG_GUI_SCREEN_TIMEOUT);
    /* Bootstrap lvgl loop events */
    xtimer_set(&gui->lvgl_loop, CONFIG_GUI_LVGL_LOOP_TIME);
    while(1)
    {

        thread_flags_t flag = thread_flags_wait_any(
            GUI_THREAD_FLAG_IDLE |
            GUI_THREAD_FLAG_WAKE |
            GUI_THREAD_FLAG_LVGL_HANDLE |
            THREAD_FLAG_EVENT
            );
        if (flag & THREAD_FLAG_EVENT) {
            event_t *ev = event_get(&gui->queue);
            if (ev) {
                ev->handler(ev);
            }
            else {
                LOG_ERROR("[GUI]: No event while flag set\n");
            }
        }
        if (flag & GUI_THREAD_FLAG_LVGL_HANDLE) {
            lv_task_handler();
            if (widget_is_dirty(gui->active_widget)) {
                LOG_DEBUG("[GUI]: widget is dirty, updating\n");
                widget_update_draw(gui->active_widget);
            }

        }
        if (flag & GUI_THREAD_FLAG_IDLE) {
            /* idle handling */
        }
    }
    assert(false);
    /* should be never reached */
    return NULL;
}
