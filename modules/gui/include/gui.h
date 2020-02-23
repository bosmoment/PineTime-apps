/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _APP_GUI_H
#define _APP_GUI_H

#include <stdint.h>
#include "hal_input.h"
#include "lvgl.h"
#include "event.h"
#include "event/timeout.h"
#include "ts_event.h"
#include "widget.h"
#include "thread.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_GUI_LVGL_LOOP_TIME
#define CONFIG_GUI_LVGL_LOOP_TIME       (10 * US_PER_MS)
#endif

/**
 * @brief Screen timeout configuration setting
 *
 * The display will automatically turn off after this many milliseconds
 * of inactivity
 */
#ifndef CONFIG_GUI_SCREEN_TIMEOUT
#define CONFIG_GUI_SCREEN_TIMEOUT       (6U * MS_PER_SEC)
#endif

#define GUI_MSG_QUEUE_SIZE              8

#define GUI_MSG_SWITCH_WIDGET           0x0001

typedef enum {
    GUI_SCREEN_NONE,
    GUI_SCREEN_OFF,
    GUI_SCREEN_TIME,
    GUI_SCREEN_MENU,
} gui_screen_t;

typedef enum {
    GUI_INPUT_EVENT_SCREEN_PRESS, /* Generic whole area screen press */
    GUI_INPUT_EVENT_LEAVE,        /* Leave current GUI context */
} gui_input_event_t;

typedef enum {
    GUI_GESTURE_UP,
    GUI_GESTURE_DOWN,
    GUI_GESTURE_LEFT,
    GUI_GESTURE_RIGHT,
} gui_event_t;

typedef struct {
    gui_screen_t type;
    lv_obj_t *(*create)(void);
} gui_screen_map_t;

typedef struct {
    event_t super;
    void *drv;
    uint16_t x1;
    uint16_t x2;
    uint16_t y1;
    uint16_t y2;
    uint16_t *map;
    bool used;
} gui_flush_event_t;

typedef struct {
    lv_disp_drv_t disp_drv;
    lv_indev_drv_t indev_drv;
    lv_disp_buf_t disp_buf;
    lv_disp_t *display;
    hal_input_coord_t coord;
    unsigned send_press;
    msg_t msg_queue[GUI_MSG_QUEUE_SIZE];
    event_t button_press;
    event_t screen_timeout;
    event_timeout_t screen_timeout_ev;
    event_queue_t queue;
    widget_t *active_widget;
    xtimer_t lvgl_loop; /* timer loop for lvgl */
    xtimer_t button_debounce;
    bool button_enabled;
    kernel_pid_t pid;
    bool display_on;
} gui_t;

typedef struct {
    ts_event_t super;
    widget_t *widget;
} gui_event_widget_switch_t;

gui_t *gui_get_ctx(void);

int gui_event_submit_switch_widget(widget_t *widget);
#ifdef __cplusplus
}
#endif

#endif /* _APP_GUI_H */
