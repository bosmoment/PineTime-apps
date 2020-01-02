/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_WIDGET_CONF
#define APP_WIDGET_CONF

#include "widget.h"

#include "time_conf.h"
#include "menu_tiles_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Home widget, shown when display is turned on.
 *        Fullfills the task of home screen
 */
#ifndef CONFIG_WIDGET_HOME
#define CONFIG_WIDGET_HOME WIDGET_HOME_TIME
#endif /* WIDGET_HOME */

#ifndef CONFIG_WIDGET_MENU
#define CONFIG_WIDGET_MENU WIDGET_MENU_TILES
#endif /* CONFIG_WIDGET_MENU */

const widget_map_t widgets_installed[] = {
    WIDGET_HOME_TIME,
    WIDGET_MENU_TILES,
};

#ifdef __cplusplus
}
#endif

#endif /* APP_WIDGET_CONF */
