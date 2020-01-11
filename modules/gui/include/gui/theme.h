/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _APP_GUI_THEME_H
#define _APP_GUI_THEME_H

#include "gui.h"
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

lv_theme_t * gui_theme_init(uint16_t hue, lv_font_t * font);
lv_theme_t * gui_theme_get(void);

#ifdef __cplusplus
}
#endif

#endif /* _APP_GUI_THEME_H */
