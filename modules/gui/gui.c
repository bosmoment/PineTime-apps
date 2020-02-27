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
#include "msg.h"

#define LVGL_THREAD_NAME    "lvgl"
#define LVGL_THREAD_PRIO    6
#define LVGL_STACKSIZE     (THREAD_STACKSIZE_LARGE)

#define GUI_BUF_SIZE             (LV_HOR_RES_MAX * 6)

#define GUI_THREAD_FLAG_INPUT_EV       (1 << 3)
#define GUI_THREAD_FLAG_LVGL_HANDLE    (1 << 5)
#define GUI_THREAD_FLAG_IDLE           (1 << 6)
#define GUI_THREAD_FLAG_WAKE           (1 << 7)

static void *_lvgl_thread(void* arg);
static char _stack[LVGL_STACKSIZE];

static gui_t _gui;

static void _gui_lvgl_update(gui_t *gui);

static lv_color_t _buf1[GUI_BUF_SIZE];
static lv_color_t _buf2[GUI_BUF_SIZE];

extern lv_obj_t *screen_time_create(void);
extern lv_obj_t *screen_menu_create(void);

extern void gui_dispatcher_display_flush_cb(struct _disp_drv_t * disp_drv,
                              const lv_area_t * area, lv_color_t * color_p);


gui_t *gui_get_ctx(void)
{
    return &_gui;
}

static void _gui_widget_send_event(gui_t *gui, gui_event_t ev)
{
    widget_t *widget = gui->active_widget;
    if (widget && widget->spec->gui_event) {
        widget->spec->gui_event(widget, ev);
    }
}

static bool _input_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    gui_t *gui = &_gui;
    data->point.x = gui->coord.x;
    data->point.y = gui->coord.y;
    data->state = LV_INDEV_STATE_REL;
    if (gui->send_press) {
        LOG_INFO("sending click at %u:%u\n", gui->coord.x, gui->coord.y);
        gui->send_press--;
        data->state = LV_INDEV_STATE_PR;
    }
    return false;
}

static void _gui_read_input(gui_t *gui)
{
    int res = hal_input_get_measurement(hal_input_get_context(), &gui->coord);
    if (res < 0 || !gui->display_on) {
        return;
    }
    if (gui->coord.gesture == CST816S_GESTURE_SINGLE_CLICK) {
        LOG_INFO("Single click at %u:%u\n", gui->coord.x, gui->coord.y);
        /* Send short press */
        gui->send_press = 1;
    }
    gui_event_t ev = GUI_EVENT_NONE;
    switch (gui->coord.gesture) {
        case CST816S_GESTURE_SLIDE_UP:
            ev = GUI_EVENT_GESTURE_UP;
            break;
        case CST816S_GESTURE_SLIDE_DOWN:
            ev = GUI_EVENT_GESTURE_DOWN;
            break;
        case CST816S_GESTURE_SLIDE_LEFT:
            ev = GUI_EVENT_GESTURE_LEFT;
            break;
        case CST816S_GESTURE_SLIDE_RIGHT:
            ev = GUI_EVENT_GESTURE_RIGHT;
            break;
        default:
            break;
    }
    if (!(ev == GUI_EVENT_NONE)) {
        /* Manually signal activity to LVGL */
        lv_disp_trig_activity(gui->display);
        _gui_widget_send_event(gui, ev);
    }
}

static void _input_cb(cst816s_t *dev, void *arg)
{
    gui_t *gui = arg;
    thread_flags_set((thread_t*)thread_get(gui->pid), GUI_THREAD_FLAG_INPUT_EV);
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

static void _gui_update_widget_draw(gui_t *gui)
{
    if (widget_is_dirty(gui->active_widget)) {
        LOG_DEBUG("[GUI]: widget is dirty, updating\n");
        widget_update_draw(gui->active_widget);
    }
}


int gui_event_submit_switch_widget(widget_t *widget)
{
    gui_t *gui = gui_get_ctx();
    msg_t msg;
    msg.type = GUI_MSG_SWITCH_WIDGET;
    msg.content.ptr = widget;
    return msg_send(&msg, gui->pid);
}

int lvgl_thread_create(void)
{
    gui_t *gui = &_gui;

    if (hal_input_init(_input_cb, gui) == 0) {
        LOG_INFO("[cst816s]: OK!\n");
    }
    else {
        LOG_ERROR("[cst816s]: Device initialization failed\n");
    }

    lv_disp_drv_t disp_drv;
    lv_disp_buf_init(&gui->disp_buf, _buf1, _buf2, GUI_BUF_SIZE);
    lv_disp_drv_init(&disp_drv);            /*Basic initialization*/

    gui_dispatcher_thread_create(gui);
    disp_drv.buffer = &gui->disp_buf;
    disp_drv.flush_cb = gui_dispatcher_display_flush_cb;

    gui->display = lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = _input_read_cb;
    lv_indev_drv_register(&indev_drv);

    int res = thread_create(_stack, LVGL_STACKSIZE, LVGL_THREAD_PRIO,
                            THREAD_CREATE_STACKTEST, _lvgl_thread,
                            gui, LVGL_THREAD_NAME);
    return res;
}

static void _gui_lvgl_trigger(void *arg)
{
    gui_t *gui = arg;
    thread_flags_set((thread_t*)sched_threads[gui->pid], GUI_THREAD_FLAG_LVGL_HANDLE);
}

static void _gui_screen_on(gui_t *gui)
{
    if (gui->display_on == false) {
        gui->display_on = true;
        _gui_lvgl_update(gui);
        hal_display_on();
    }
}

static void _gui_enable_button(void *arg)
{
    gui_t *gui = arg;
    gui->button_enabled = true;
}

static void _gui_button_irq(void *arg)
{
    gui_t *gui = (gui_t*)arg;
    /* Button pressed */
    if (gui->button_enabled) {
        gui->button_enabled = false;
        xtimer_set(&gui->button_debounce, 100 * US_PER_MS);
        event_post(&gui->queue, &gui->button_press);
    }
}

static void _gui_button_event(event_t *event)
{
    gui_t *gui = container_of(event, gui_t, button_press);
    _gui_screen_on(gui);
    LOG_INFO("[gui] Button press event\n");
    event_timeout_clear(&gui->screen_timeout_ev);
    event_timeout_set(&gui->screen_timeout_ev,
            CONFIG_GUI_SCREEN_TIMEOUT * US_PER_MS);
}

static void _gui_screen_timeout(event_t *event)
{
    gui_t *gui = container_of(event, gui_t, screen_timeout);
    uint32_t inactive_time = lv_disp_get_inactive_time(NULL);
    if (inactive_time  >= CONFIG_GUI_SCREEN_TIMEOUT) {
        LOG_INFO("[gui] Screen off after timeout\n");
        /* Turn screen off */
        gui->display_on = false;
        hal_display_off();
    }
    else {
        /* Touch event within the screen timeout, resubmit event with new timeout */
        uint32_t timeout = CONFIG_GUI_SCREEN_TIMEOUT - inactive_time;
        event_timeout_set(&gui->screen_timeout_ev,
                          timeout * US_PER_MS);
    }
}

static void _gui_lvgl_update(gui_t *gui)
{
    if (gui->display_on) {
        xtimer_set(&gui->lvgl_loop, CONFIG_GUI_LVGL_LOOP_TIME);
        _gui_update_widget_draw(gui);
        lv_task_handler();
    }
}

/* Message handling */
static void _gui_handle_msg(gui_t *gui, msg_t *msg)
{
    switch(msg->type) {
        case GUI_MSG_SWITCH_WIDGET:
            _switch_widget_draw(gui, msg->content.ptr);
            break;
        default:
            LOG_ERROR("[gui]: Unhandled msg of type %"PRIu16"\n", msg->type);
    }
}

static void *_lvgl_thread(void* arg)
{
    gui_t *gui = (gui_t*)arg;
    msg_init_queue(gui->msg_queue, GUI_MSG_QUEUE_SIZE);
    gui->pid = thread_getpid();

    gui->lvgl_loop.callback = _gui_lvgl_trigger;
    gui->lvgl_loop.arg = gui;

    gui->button_debounce.callback = _gui_enable_button;
    gui->button_debounce.arg = gui;
    gui->button_enabled = true;

    lv_theme_t *th = gui_theme_init(10, NULL);
    lv_theme_set_current(th);

    event_queue_claim(&gui->queue);

    /* Button events */
    gui->button_press.handler = _gui_button_event;
    gui->screen_timeout.handler = _gui_screen_timeout;
    event_timeout_init(&gui->screen_timeout_ev, &gui->queue, &gui->screen_timeout);

    /* Configure the button */
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
            GUI_THREAD_FLAG_INPUT_EV |
            THREAD_FLAG_EVENT |
            THREAD_FLAG_MSG_WAITING
            );
        /* External thread to GUI messages */
        if (flag & THREAD_FLAG_MSG_WAITING) {
            msg_t msg;
            while (msg_try_receive(&msg) == 1) {
                _gui_handle_msg(gui, &msg);
            }
        }
        /* Internal state machine based events */
        if (flag & THREAD_FLAG_EVENT) {
            event_t *ev = NULL;
            /* Handle all events */
            while ((ev = event_get(&gui->queue))) {
                ev->handler(ev);
            }
        }
        if (flag & GUI_THREAD_FLAG_LVGL_HANDLE) {
            _gui_lvgl_update(gui);
        }
        if (flag & GUI_THREAD_FLAG_INPUT_EV) {
            puts("Input EV flag");
            _gui_read_input(gui);
        }
        if (flag & GUI_THREAD_FLAG_IDLE) {
            /* idle handling */
        }
    }
    assert(false);
    /* should be never reached */
    return NULL;
}
