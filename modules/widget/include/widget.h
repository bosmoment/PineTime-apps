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
#include "mutex.h"

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

    const char *label;  /**< Complex LVGL GUI label */

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

    int (*event)(widget_t *widget, unsigned event);

    int (*update_draw)(widget_t *widget);

/**
 * @brief Flags as defined above
 */
    uint16_t flags;
} widget_spec_t;

struct _widget {
    const widget_spec_t *spec;
    mutex_t update;
    bool dirty;
};

static inline const char *widget_get_label(const widget_t *widget)
{
    return widget->spec->label;
}

void widget_init_installed(void);

int widget_get_gui_lock(widget_t *widget);
void widget_release_gui_lock(widget_t *widget);

void widget_get_control_lock(widget_t *widget);

void widget_release_control_lock(widget_t *widget);

widget_t *widget_get_home(void);
widget_t *widget_get_menu(void);
void widget_init(widget_t *widget);
int widget_launch(widget_t *widget);
int widget_draw(widget_t *widget);
int widget_update_draw(widget_t *widget);
int widget_close(widget_t *widget);
void widget_init_local(widget_t *widget);

static inline bool widget_is_dirty(widget_t *widget)
{
    return widget->dirty == 1;
}

#ifdef __cplusplus
}
#endif

#endif /* APP_WIDGET */
