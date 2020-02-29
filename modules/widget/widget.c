/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "widget.h"
#include "widget_conf.h"
#include "mutex.h"

widget_t *widget_get_home(void)
{
    return CONFIG_WIDGET_HOME;
}

widget_t *widget_get_menu(void)
{
    return CONFIG_WIDGET_MENU;
}

int widget_launch(widget_t *widget)
{
    return widget->spec->launch(widget);
}

int widget_draw(widget_t *widget)
{
    return widget->spec->draw(widget, NULL);
}

int widget_update_draw(widget_t *widget)
{
    return widget->spec->update_draw(widget);
}

int widget_close(widget_t *widget)
{
    return widget->spec->close(widget);
}

void widget_init(widget_t *widget)
{
    widget->spec->init(widget);
}

void widget_init_local(widget_t *widget)
{
    mutex_init(&widget->update);
}

void widget_init_installed(void)
{
    widget_init(CONFIG_WIDGET_MENU);
    for (size_t i = 0; i < ARRAY_SIZE(widgets_installed); i++) {
        widget_init(widgets_installed[i]);
    }
    for (size_t i = 0; i < ARRAY_SIZE(widget_faces_installed); i++) {
        widget_init(widget_faces_installed[i]);
    }
}

int widget_get_gui_lock(widget_t *widget)
{
    int res = mutex_trylock(&widget->update);
    if (res) {
        widget->dirty = 0;
    }
    return res;
}

void widget_release_gui_lock(widget_t *widget)
{
    mutex_unlock(&widget->update);
}

void widget_get_control_lock(widget_t *widget)
{
    mutex_lock(&widget->update);
}

void widget_release_control_lock(widget_t *widget)
{
    widget->dirty = 1;
    mutex_unlock(&widget->update);
}

int widget_face_gui_event(widget_t *widget, int event)
{
    switch (event) {
        case GUI_EVENT_GESTURE_UP:
            /* Next widget */
            controller_action_submit_input_action(widget,
                                                  CONTROLLER_ACTION_WIDGET_FACE_NEXT, NULL);
            break;
            /* Previous widget */
        case GUI_EVENT_GESTURE_DOWN:
            controller_action_submit_input_action(widget,
                                                  CONTROLLER_ACTION_WIDGET_FACE_PREVIOUS, NULL);
            break;
            /* Menu */
        case GUI_EVENT_GESTURE_LEFT:
            controller_action_submit_input_action(widget,
                                                  CONTROLLER_ACTION_WIDGET_MENU, NULL);
            break;
        case GUI_EVENT_GESTURE_RIGHT:
            break;
    }
    return 0;
}
