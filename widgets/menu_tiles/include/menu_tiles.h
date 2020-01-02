/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef WIDGET_MENU_TILES_H
#define WIDGET_MENU_TILES_H

#include "lvgl.h"
#include "widget.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _menu_tiles_widget {
    widget_t widget;
    lv_obj_t *screen;
} menu_tiles_widget_t;

extern menu_tiles_widget_t menu_tiles_widget;

#ifdef __cplusplus
}
#endif


#endif /* WIDGET_MENU_TILES_H */


