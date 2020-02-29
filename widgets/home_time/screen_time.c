/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "home_time.h"
#include "hal.h"
#include "log.h"
#include "lvgl.h"
#include "gui.h"
#include "gui/theme.h"
#include "controller.h"
#include "kernel_defines.h"
#include "bleman.h"
#include "fonts/noto_sans_numeric_80.h"

static const widget_spec_t home_time_spec;
static lv_style_t style_time;

static int _screen_time_update_screen(widget_t *widget);

/* TODO: make configurable */
static const unsigned battery_low = 20; /* Battery low percentage */
static const char *battery_low_color = GUI_COLOR_LBL_BASIC_RED;
static const char *battery_mid_color = GUI_COLOR_LBL_BASIC_YELLOW;
static const char *battery_full_color = GUI_COLOR_LBL_BASIC_GREEN;
/* Widget context */
home_time_widget_t home_time_widget = {
    .widget = {.spec = &home_time_spec }
};

static const char *_state2color[] = {
    [BLEMAN_BLE_STATE_INACTIVE] = "#000000",
    [BLEMAN_BLE_STATE_DISCONNECTED] = GUI_COLOR_LBL_BASIC_RED,
    [BLEMAN_BLE_STATE_ADVERTISING] = GUI_COLOR_LBL_BASIC_BLUE,
    [BLEMAN_BLE_STATE_CONNECTED] = GUI_COLOR_LBL_DARK_GREEN,
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
        case LV_EVENT_CLICKED:
            LOG_INFO("Screen press event\n");
            controller_action_submit_input_action(&ht->widget,
                                                CONTROLLER_ACTION_WIDGET_MENU, NULL);
        default:
            break;
    }
}

lv_obj_t *screen_time_create(home_time_widget_t *ht)
{
    lv_obj_t *scr = lv_obj_create(NULL, NULL);

    /* time (00:00)*/
    lv_obj_t * label1 = lv_label_create(scr, NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);
    lv_label_set_text(label1, "00:00");
    lv_obj_set_width(label1, 240);
    lv_obj_set_height(label1, 200);
    lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(label1, scr, LV_ALIGN_CENTER, 0, -30);
    lv_label_set_style(label1, LV_LABEL_STYLE_MAIN, &style_time);
    ht->lv_time = label1;

    lv_obj_t * l_state = lv_label_create(scr, NULL);
    lv_obj_set_width(l_state, 50);
    lv_obj_set_height(l_state, 80);
    lv_label_set_text(l_state, "");
    lv_label_set_recolor(l_state, true);
    lv_label_set_align(l_state, LV_LABEL_ALIGN_LEFT);
    lv_obj_align(l_state, scr, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    ht->lv_ble = l_state;

    /* Power indicator */
    lv_obj_t * l_power = lv_label_create(scr, NULL);
    lv_obj_set_width(l_power, 80);
    lv_obj_set_height(l_power, 20);
    lv_label_set_text(l_power, "");
    lv_label_set_recolor(l_power, true);
    lv_label_set_align(l_power, LV_LABEL_ALIGN_RIGHT);
    lv_obj_align(l_power, scr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    ht->lv_power = l_power;

    /* Date */
    lv_obj_t * label_date = lv_label_create(scr, NULL);
    lv_label_set_long_mode(label_date, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label_date, 200);
    lv_obj_set_height(label_date, 200);
    lv_label_set_align(label_date, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(label_date, scr, LV_ALIGN_CENTER, 0, 40);
    ht->lv_date = label_date;

    lv_obj_set_click(scr, true);

    lv_obj_set_event_cb(scr, _screen_time_pressed);
    lv_obj_set_event_cb(label1, _screen_time_pressed);

    _screen_time_update_screen(&ht->widget);
    return scr;
}

static void _home_time_set_bt_label(home_time_widget_t *htwidget)
{

    if (htwidget->ble_state == BLEMAN_BLE_STATE_DISCONNECTED ) {
        lv_label_set_text(htwidget->lv_ble, "");
    }
    else {
        const char *color = _state2color[htwidget->ble_state];
        lv_label_set_text_fmt(htwidget->lv_ble,
                              "%s "LV_SYMBOL_BLUETOOTH"#",
                              color);
    }
}

static void _home_time_set_power_label(home_time_widget_t *htwidget)
{
    const char *color = battery_mid_color;
    unsigned percentage = hal_battery_get_percentage(htwidget->millivolts);
    if (percentage <= battery_low) {
        color = battery_low_color;
    }
    if (htwidget->powered && !(htwidget->charging) ) {
        /* Battery charge cycle finished */
        color = battery_full_color;
    }
    lv_label_set_text_fmt(htwidget->lv_power,
                          "%s %u%%%s#\n(%"PRIu32"mV)",
                          color, percentage,
                          htwidget->powered ? LV_SYMBOL_CHARGE : " ",
                          htwidget->millivolts
                          );
    lv_obj_align(htwidget->lv_power, htwidget->screen, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
}

static int _home_time_set_time_label(home_time_widget_t *ht)
{
    char time[6];
    char date[15];
    int res = snprintf(time, sizeof(time), "%02u:%02u", ht->time.hour,
                       ht->time.minute);
    if (res != sizeof(time) - 1) {
        LOG_ERROR("[home_time]: error formatting time string %*s\n", res, time);
        return -1;
    }
    lv_label_set_text(ht->lv_time, time);

    res = snprintf(date, sizeof(date), "%u %s %u\n", ht->time.dayofmonth,
                   controller_time_month_get_short_name(&ht->time),
                   ht->time.year);
    if (res == sizeof(date)) {
        LOG_ERROR("[home_time]: error formatting date string %*s\n", res, date);
        return -1;
    }
    lv_label_set_text(ht->lv_date, date);
    return 0;
}

static int _screen_time_update_screen(widget_t *widget)
{
    home_time_widget_t *ht = _from_widget(widget);

    _home_time_set_time_label(ht);
    _home_time_set_bt_label(ht);
    _home_time_set_power_label(ht);
    return 0;
}

static int home_time_update_screen(widget_t *widget)
{
    if (widget_get_gui_lock(widget) == 0) {
        return 0;
    }
    LOG_DEBUG("[home_screen]: updating drawing\n");
    _screen_time_update_screen(widget);
    widget_release_gui_lock(widget);
    return 1;
}

int home_time_init(widget_t *widget)
{
    home_time_widget_t *htwidget = _from_widget(widget);
    widget_init_local(widget);
    htwidget->handler.events = CONTROLLER_EVENT_FLAG(CONTROLLER_EVENT_TICK) |
                               CONTROLLER_EVENT_FLAG(CONTROLLER_EVENT_BLUETOOTH);
    htwidget->handler.widget = widget;

    lv_style_copy(&style_time, gui_theme_get()->style.label.prim);
    style_time.text.font = &noto_sans_numeric_80;

    controller_add_control_handler(controller_get(), &htwidget->handler);
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
    htwidget->screen = screen_time_create(htwidget);
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

static void _update_power_stats(home_time_widget_t *htwidget)
{
    htwidget->powered = hal_battery_is_powered();
    htwidget->charging = hal_battery_is_charging();
    htwidget->millivolts = controller_get_battery_voltage(controller_get());
}

int home_time_event(widget_t *widget, controller_event_t event)
{
    home_time_widget_t *htwidget = _from_widget(widget);
    widget_get_control_lock(widget);
    if (event == CONTROLLER_EVENT_TICK) {
        memcpy(&htwidget->time, controller_time_get_time(controller_get()), sizeof(controller_time_spec_t));
        _update_power_stats(htwidget);
    }
#ifdef MODULE_BLEMAN
    if (event == CONTROLLER_EVENT_BLUETOOTH) {
        htwidget->ble_state = bleman_get_conn_state(bleman_get(), NULL);
    }
#endif
    widget_release_control_lock(widget);
    return 0;
}

static const widget_spec_t home_time_spec = {
    .name = "time",
    .init = home_time_init,
    .launch = home_time_launch,
    .draw = home_time_draw,
    .container = home_time_get_container,
    .close = home_time_close,
    .event = home_time_event,
    .update_draw = home_time_update_screen,
    .gui_event = widget_face_gui_event,
};
