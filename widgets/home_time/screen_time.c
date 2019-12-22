/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "home_time.h"
#include "log.h"
#include "lvgl.h"
#include "gui.h"
#include "controller.h"
#include "kernel_defines.h"

static const widget_spec_t home_time_spec;

/* Widget context */
home_time_widget_t home_time_widget = {
    .widget = {.spec = &home_time_spec }
};

static inline home_time_widget_t *_from_widget(widget_t *widget)
{
    return container_of(widget, home_time_widget_t, widget);
}

static inline home_time_widget_t *active_widget(void)
{
    return &home_time_widget;
}

static void _screen_time_pressed(lv_obj_t *obj, lv_event_t event)
{
    home_time_widget_t *ht = active_widget();
    switch (event) {
        case LV_EVENT_PRESSED:
            LOG_INFO("Screen press event\n");
            controller_event_submit_input_event(&ht->widget,
                                                CONTROLLER_EVENT_WIDGET_LEAVE);
        default:
            break;
    }
}

lv_obj_t *screen_time_create(void)
{
    lv_obj_t *scr = lv_obj_create(NULL, NULL);

    lv_obj_t * label1 = lv_label_create(scr, NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);
    lv_label_set_text(label1, "21:36");
    lv_obj_set_width(label1, 200);
    lv_obj_set_height(label1, 200);
    lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(label1, scr, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_click(scr, true);

    lv_obj_set_event_cb(scr, _screen_time_pressed);
    lv_obj_set_event_cb(label1, _screen_time_pressed);

    return scr;
}

int home_time_init(widget_t *widget)
{
    (void)widget;
    return 0;
}

int home_time_launch(widget_t *widget)
{
    home_time_widget_t *htwidget = _from_widget(widget);
    (void)htwidget;
    return 0;
}

int home_time_draw(widget_t *widget, lv_obj_t *parent)
{
    LOG_INFO("drawing time widget\n");
    home_time_widget_t *htwidget = _from_widget(widget);
    htwidget->screen = screen_time_create();
    return 0;
}

lv_obj_t *home_time_get_container(widget_t *widget)
{
    home_time_widget_t *htwidget = _from_widget(widget);
    return htwidget->screen;
}

int home_time_close(widget_t *widget)
{
    home_time_widget_t *htwidget = _from_widget(widget);
    lv_obj_del(htwidget->screen);
    htwidget->screen = NULL;
    return 0;
}

static const widget_spec_t home_time_spec = {
    .name = "time",
    .init = home_time_init,
    .draw = home_time_draw,
    .container = home_time_get_container,
    .close = home_time_close,
};
