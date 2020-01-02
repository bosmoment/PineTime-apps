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
    for (size_t i = 0; i < ARRAY_SIZE(widgets_installed); i++) {
        widget_init(widgets_installed[i]);
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
