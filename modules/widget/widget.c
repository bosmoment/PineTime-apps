/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "widget.h"
#include "widget_conf.h"

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

int widget_close(widget_t *widget)
{
    return widget->spec->close(widget);
}
