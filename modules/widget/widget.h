/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_WIDGET
#define APP_WIDGET

#include <stdint.h>
#include "lvgl.h"
#include "event.h"
#include "ts_event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _widget widget_t;
typedef widget_t* widget_map_t;

/**
 * @brief widget specification map
 *
 * @note should be declared const
 */
typedef struct widget_spec {
    const char *name;

/**
 * @brief Initialization function, called once on boot
 */
    int (*init)(widget_t *widget);

/**
 * @brief Launch the widget
 */
    int (*launch)(widget_t *widget);

/**
 * @brief Draw the widget
 */
    int (*draw)(widget_t *widget, lv_obj_t *parent);

/**
 * @brief Retrieve the container drawn in
 */
    lv_obj_t *(*container)(widget_t *widget);

/**
 * @brief forcefully close the widget due to external event,
 *        e.g. incomming phone call or alarm ring
 */
    int (*close)(widget_t *widget);

/**
 * @brief Flags as defined above
 */
    uint16_t flags;
} widget_spec_t;

struct _widget {
    const widget_spec_t *spec;
};

widget_t *widget_get_home(void);
widget_t *widget_get_menu(void);
int widget_launch(widget_t *widget);
int widget_draw(widget_t *widget);
int widget_close(widget_t *widget);

#ifdef __cplusplus
}
#endif

#endif /* APP_WIDGET */
