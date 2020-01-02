/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "gui.h"
#include "controller.h"
#include "log.h"
#include "lvgl.h"
#include "menu_tiles.h"
#include "widget_conf.h"

static const widget_spec_t menu_tiles_spec;

menu_tiles_widget_t menu_tiles_widget = {
    .widget = { .spec = &menu_tiles_spec }
};

static inline menu_tiles_widget_t *_from_widget(widget_t *widget)
{
    return container_of(widget, menu_tiles_widget_t, widget);
}

static menu_tiles_widget_t *active_widget(void)
{
    return &menu_tiles_widget;
}

static void _screen_menu_exit(lv_obj_t *obj, lv_event_t event)
{
    menu_tiles_widget_t *ht = active_widget();
    switch (event) {
        case LV_EVENT_CLICKED:
            LOG_INFO("Menu button press event\n");
            controller_action_submit_input_action(&ht->widget,
                                                CONTROLLER_ACTION_WIDGET_HOME);
        default:
            break;
    }
}

lv_obj_t *screen_menu_create(void)
{
    lv_obj_t *scr = lv_obj_create(NULL, NULL);

    lv_obj_t * list1 = lv_list_create(scr, NULL);
    lv_obj_set_size(list1, 240, 240);
    lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *list_btn;
    for (size_t i=2; i < ARRAY_SIZE(widgets_installed); i++) {
        const widget_t *widget = widgets_installed[i];
        LOG_INFO("[menu_tiles]: adding button for %s\n", widget->spec->name);
        list_btn = lv_list_add_btn(list1, NULL, widget_get_label(widget));
        assert(list_btn);
    }
    list_btn = lv_list_add_btn(list1, NULL, LV_SYMBOL_NEW_LINE " Close");
    assert(list_btn);
    lv_obj_set_event_cb(list_btn, _screen_menu_exit);
    (void)list_btn;

    return scr;
}

int menu_tiles_init(widget_t *widget)
{
    (void)widget;
    return 0;
}

int menu_tiles_launch(widget_t *widget)
{
    menu_tiles_widget_t *htwidget = _from_widget(widget);
    (void)htwidget;
    return 0;
}

int menu_tiles_draw(widget_t *widget, lv_obj_t *parent)
{
    menu_tiles_widget_t *mtwidget = _from_widget(widget);
    mtwidget->screen = screen_menu_create();
    return 0;
}

static lv_obj_t *menu_tiles_get_container(widget_t *widget)
{
    menu_tiles_widget_t *mtwidget = _from_widget(widget);
    return mtwidget->screen;
}

int menu_tiles_close(widget_t *widget)
{
    menu_tiles_widget_t *htwidget = _from_widget(widget);
    lv_obj_del(htwidget->screen);
    htwidget->screen = NULL;
    return 0;
}

static const widget_spec_t menu_tiles_spec = {
    .name = "time",
    .init = menu_tiles_init,
    .draw = menu_tiles_draw,
    .container = menu_tiles_get_container,
    .close = menu_tiles_close,
};
