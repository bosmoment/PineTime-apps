/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "log.h"
#include "lvgl.h"

static void _screen_time_pressed(lv_obj_t *obj, lv_event_t event)
{
    switch (event) {
        case LV_EVENT_PRESSED:
            printf("screen clicked!\n");
            break;
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
