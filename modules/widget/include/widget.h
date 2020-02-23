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
#include "controller/structs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _widget widget_t;
typedef widget_t* widget_map_t;

/**
 * @brief widget specification map
 *
 * @note should be declared const to ensure it resides in the MCU flash
 */
typedef struct widget_spec {
   /**
    * @brief Simple name of the widget, must be printable on the console
    */
    const char *name;

/**
 * @brief Complex LVGL GUI label, used in LVGL elements such as the menu.
 *
 * It is allowed to use unicode and special features described here:
 * https://docs.littlevgl.com/en/html/overview/font.html
 */
    const char *label;

/**
 * @brief Initialization function, called once on boot by the controller
 *
 * @param   widget  The widget context
 */
    int (*init)(widget_t *widget);

/**
 * @brief Launch the widget
 *
 * This function is called by
 */
    int (*launch)(widget_t *widget);

/**
 * @brief Draw the full widget
 *
 * This function is called by the GUI when the widget is expected to draw the
 * initial set of elements
 *
 * @param   parent  The parent object the widget must draw into
 */
    int (*draw)(widget_t *widget, lv_obj_t *parent);

/**
 * @brief update the drawings of the widget
 *
 * This function is called by the GUI when the widget signals that an update of
 * the view is required to reflect the current state.
 *
 * It is possible and allowed to modify the full view, but usually only the data
 * of the LVGL elements has to be modified.
 *
 * @param   widget  The widget context
 */
    int (*update_draw)(widget_t *widget);

/**
 * @brief Retrieve the container drawn in
 *
 * @param   widget  The widget context
 */
    lv_obj_t *(*container)(widget_t *widget);

/**
 * @brief The controller forcefully closed the widget due to external event,
 *        e.g. incomming phone call or alarm ring
 *
 * @param   widget  The widget context
 */
    int (*close)(widget_t *widget);

/**
 * @brief Event signals from the controller to the application, can be used to
 * receive events relevant for the widget rendering.
 *
 * @note Events are only submitted to the widget when the widget is active.
 *
 * @param   widget  The widget context
 * @param   event   The event
 */
    int (*event)(widget_t *widget, controller_event_t event);

/**
 * @brief Event signals from the gui to the application, events such as gestures
 * for the application are transmitted through this callback
 *
 * @note Events are only submitted to the widget when the widget is active.
 *
 * @param   widget  The widget context
 * @param   event   The event
 */
    int (*gui_event)(widget_t *widget, int event);

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
