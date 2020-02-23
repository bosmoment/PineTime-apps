/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "home_terminal.h"
#include "hal.h"
#include "log.h"
#include "lvgl.h"
#include "gui.h"
#include "gui/theme.h"
#include "controller.h"
#include "kernel_defines.h"
#include "bleman.h"

static const widget_spec_t home_terminal_spec;

static int _screen_time_update_screen(widget_t *widget);

/* TODO: make configurable */
static const unsigned battery_low = 20;               /* Battery low percentage */
static const unsigned battery_high = 80;               /* Battery low percentage */
static const uint32_t battery_low_color = 0xDC143C;   /* Electric crimson */
static const uint32_t battery_mid_color = 0xFF7F00;   /* Orange */
static const uint32_t battery_full_color = 0x138808;  /* India green */
/* Widget context */
home_terminal_widget_t home_terminal_widget = {
    .widget = {.spec = &home_terminal_spec }
};

/* Quick hack to measure the temperature */
#include "board.h"
static uint32_t _get_internal_temp(void)
{
    NRF_TEMP->TASKS_START = 1;
    while (!NRF_TEMP->EVENTS_DATARDY) {
    }
    NRF_TEMP->EVENTS_DATARDY = 0;
    return NRF_TEMP->TEMP;
}

static const uint32_t _state2color[] = {
    [BLEMAN_BLE_STATE_INACTIVE] = 0x00,
    [BLEMAN_BLE_STATE_DISCONNECTED] = 0xDC143C,
    [BLEMAN_BLE_STATE_ADVERTISING] = 0x007BA7, /* Cerulean */
    [BLEMAN_BLE_STATE_CONNECTED] = 0x138808, /* India green */
};

static const char* _state2str[] = {
    [BLEMAN_BLE_STATE_INACTIVE] =     " INACTIVE",
    [BLEMAN_BLE_STATE_DISCONNECTED] = "  DISCONN",
    [BLEMAN_BLE_STATE_ADVERTISING] =  "   ADVERT", /* Cerulean */
    [BLEMAN_BLE_STATE_CONNECTED] =    "CONNECTED", /* India green */
};

static const char *full_string =
    "user@watch:~ $ now\n"
    "[TIME]    #007BA7 %02u:%02u:%02u#\n"
    "[DATE] #007BA7 %02u %s %04u#\n"
    "[BLE ]   #%06" PRIx32 " %s#\n"
    "[BATT] [#%06" PRIx32 " %s#] % 3u%%\n"
    "[VOLT]   %s % 4u mV\n"
    "[TEMP]    #DC143C % 6u °C\n"
    "[STEP]   #138808 % 6u steps#\n"
    "[HR  ]   #138808 % 6u bpm\n"
    "user@watch:~ $";


static void _home_terminal_set_label(home_terminal_widget_t *ht)
{
    char batt_string[] = "......";
    uint32_t battery_color = battery_mid_color;
    unsigned percentage = hal_battery_get_percentage(ht->millivolts);
    if (percentage <= battery_low) {
        battery_color = battery_low_color;
    }
    else if (percentage > battery_high) {
        battery_color = battery_full_color;
    }
    char *batt_symbol = ht->powered ?
        (ht->charging ? LV_SYMBOL_CHARGE : LV_SYMBOL_BATTERY_FULL) :
        " ";

    size_t num_plus = (percentage / 16);
    for (size_t i = 0; i < num_plus; i++) {
        batt_string[i] = '+';
    }

    lv_label_set_text_fmt(ht->lv_string,
                          full_string,
                          ht->time.hour,
                          ht->time.minute,
                          ht->time.second,
                          ht->time.dayofmonth,
                          controller_time_month_get_short_name(&ht->time),
                          ht->time.year,
                          _state2color[ht->ble_state],
                          _state2str[ht->ble_state],
                          battery_color,
                          batt_string,
                          percentage,
                          batt_symbol,
                          ht->millivolts,
                          ht->quartertemp/4,
                          55,
                          58
                          );
    lv_obj_align(ht->lv_string, ht->screen, LV_ALIGN_IN_TOP_LEFT, 0, 0);
}

static inline home_terminal_widget_t *_from_widget(widget_t *widget)
{
    return container_of(widget, home_terminal_widget_t, widget);
}

static inline home_terminal_widget_t *active_widget(void)
{
    return &home_terminal_widget;
}

static void _screen_terminal_pressed(lv_obj_t *obj, lv_event_t event)
{
    home_terminal_widget_t *ht = active_widget();
    switch (event) {
        case LV_EVENT_CLICKED:
            LOG_INFO("Screen press event\n");
            controller_action_submit_input_action(&ht->widget,
                                                CONTROLLER_ACTION_WIDGET_MENU, NULL);
        default:
            break;
    }
}

lv_obj_t *screen_term_create(home_terminal_widget_t *ht)
{
    lv_obj_t *scr = lv_obj_create(NULL, NULL);

    /* time (00:00)*/
    lv_obj_t * label1 = lv_label_create(scr, NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label1, 240);
    lv_obj_set_height(label1, 240);
    lv_label_set_align(label1, LV_LABEL_ALIGN_LEFT);
    lv_obj_align(label1, scr, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_recolor(label1, true);
    ht->lv_string = label1;

    lv_obj_set_click(scr, true);

    lv_obj_set_event_cb(scr, _screen_terminal_pressed);
    lv_obj_set_event_cb(label1, _screen_terminal_pressed);

    _screen_time_update_screen(&ht->widget);
    return scr;
}

static int _screen_time_update_screen(widget_t *widget)
{
    home_terminal_widget_t *ht = _from_widget(widget);

    _home_terminal_set_label(ht);
    return 0;
}

static int home_terminal_update_screen(widget_t *widget)
{
    if (widget_get_gui_lock(widget) == 0) {
        return 0;
    }
    LOG_DEBUG("[home_screen]: updating drawing\n");
    _screen_time_update_screen(widget);
    widget_release_gui_lock(widget);
    return 1;
}

int home_terminal_init(widget_t *widget)
{
    home_terminal_widget_t *htwidget = _from_widget(widget);
    widget_init_local(widget);
    htwidget->handler.events = CONTROLLER_EVENT_FLAG(CONTROLLER_EVENT_TICK) |
                               CONTROLLER_EVENT_FLAG(CONTROLLER_EVENT_BLUETOOTH);
    htwidget->handler.widget = widget;

    controller_add_control_handler(controller_get(), &htwidget->handler);
    htwidget->ble_state = bleman_get_conn_state(bleman_get(), NULL);
    return 0;
}

int home_terminal_launch(widget_t *widget)
{
    home_terminal_widget_t *htwidget = _from_widget(widget);
    (void)htwidget;
    return 0;
}

int home_terminal_draw(widget_t *widget, lv_obj_t *parent)
{
    LOG_INFO("drawing time widget\n");
    home_terminal_widget_t *htwidget = _from_widget(widget);
    htwidget->screen = screen_term_create(htwidget);
    return 0;
}

lv_obj_t *home_terminal_get_container(widget_t *widget)
{
    home_terminal_widget_t *htwidget = _from_widget(widget);
    return htwidget->screen;
}

int home_terminal_close(widget_t *widget)
{
    home_terminal_widget_t *htwidget = _from_widget(widget);
    lv_obj_del(htwidget->screen);
    htwidget->screen = NULL;
    return 0;
}

static void _update_power_stats(home_terminal_widget_t *htwidget)
{
    htwidget->powered = hal_battery_is_powered();
    htwidget->charging = hal_battery_is_charging();
    htwidget->millivolts = controller_get_battery_voltage(controller_get());
}

int home_terminal_event(widget_t *widget, controller_event_t event)
{
    home_terminal_widget_t *htwidget = _from_widget(widget);
    widget_get_control_lock(widget);
    if (event == CONTROLLER_EVENT_TICK) {
        memcpy(&htwidget->time, controller_time_get_time(controller_get()), sizeof(controller_time_spec_t));
        _update_power_stats(htwidget);
        htwidget->quartertemp = _get_internal_temp();
    }
#ifdef MODULE_BLEMAN
    if (event == CONTROLLER_EVENT_BLUETOOTH) {
        htwidget->ble_state = bleman_get_conn_state(bleman_get(), NULL);
    }
#endif
    widget_release_control_lock(widget);
    return 0;
}

static const widget_spec_t home_terminal_spec = {
    .name = "time",
    .init = home_terminal_init,
    .draw = home_terminal_draw,
    .container = home_terminal_get_container,
    .close = home_terminal_close,
    .event = home_terminal_event,
    .update_draw = home_terminal_update_screen,
};
