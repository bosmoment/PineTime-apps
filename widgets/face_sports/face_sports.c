/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <stdint.h>
#include "face_sports.h"
#include "hal.h"
#include "log.h"
#include "lvgl.h"
#include "gui.h"
#include "gui/theme.h"
#include "controller.h"
#include "kernel_defines.h"
#include "bleman.h"
#include "fonts/noto_sans_numeric_80.h"

static const widget_spec_t face_sports_spec;

face_sports_widget_t face_sports_widget = {
    .widget = {.spec = &face_sports_spec }
};

static inline face_sports_widget_t *_from_widget(widget_t *widget)
{
    return container_of(widget, face_sports_widget_t, widget);
}

static inline face_sports_widget_t *active_widget(void)
{
    return &face_sports_widget;
}

static void _face_sports_pressed(lv_obj_t *obj, lv_event_t event)
{
    face_sports_widget_t *fn = active_widget();
    switch (event) {
        case LV_EVENT_CLICKED:
            controller_action_submit_input_action(&fn->widget,
                                                CONTROLLER_ACTION_WIDGET_MENU, NULL);
        default:
            break;
    }
}

lv_obj_t *face_sports_create(face_sports_widget_t *fn)
{
    lv_obj_t *scr = lv_obj_create(NULL, NULL);
    lv_obj_set_click(scr, true);
    lv_obj_set_event_cb(scr, _face_sports_pressed);
    return scr;
}

static int face_sports_update_screen(widget_t *widget)
{
    if (widget_get_gui_lock(widget) == 0) {
        return 0;
    }
    LOG_DEBUG("[face_sports]: updating drawing\n");
    widget_release_gui_lock(widget);
    return 1;
}

static int face_sports_init(widget_t *widget)
{
    face_sports_widget_t *fn = _from_widget(widget);
    widget_init_local(widget);
    fn->handler.events = CONTROLLER_EVENT_FLAG(CONTROLLER_EVENT_TICK);
    fn->handler.widget = widget;
    controller_add_control_handler(controller_get(), &fn->handler);
    return 0;
}

static int face_sports_launch(widget_t *widget)
{
    face_sports_widget_t *fn = _from_widget(widget);
    (void)fn;
    return 0;
}

static int face_sports_draw(widget_t *widget, lv_obj_t *parent)
{
    LOG_INFO("[face_sports]: drawing\n");
    face_sports_widget_t *fn = _from_widget(widget);
    fn->screen = face_sports_create(fn);
    return 0;
}

static lv_obj_t *face_sports_get_container(widget_t *widget)
{
    face_sports_widget_t *fn = _from_widget(widget);
    return fn->screen;
}

static int face_sports_close(widget_t *widget)
{
    face_sports_widget_t *fn = _from_widget(widget);
    lv_obj_del(fn->screen);
    fn->screen = NULL;
    return 0;
}

static int face_sports_event(widget_t *widget, controller_event_t event)
{
    face_sports_widget_t *fn = _from_widget(widget);
    (void)fn;
    widget_get_control_lock(widget);
    if (event == CONTROLLER_EVENT_TICK) {
    }
    widget_release_control_lock(widget);
    return 0;
}

static const widget_spec_t face_sports_spec = {
    .name = "notifications",
    .init = face_sports_init,
    .launch = face_sports_launch,
    .draw = face_sports_draw,
    .container = face_sports_get_container,
    .close = face_sports_close,
    .event = face_sports_event,
    .gui_event = widget_face_gui_event,
    .update_draw = face_sports_update_screen,
};
