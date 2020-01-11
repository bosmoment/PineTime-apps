/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _APP_GUI_DISPATCHER_H
#define _APP_GUI_DISPATCHER_H

#include "gui.h"
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

int gui_dispatcher_thread_create(gui_t *gui);
void gui_dispatcher_display_flush(event_t *event);

#ifdef __cplusplus
}
#endif

#endif /* _APP_GUI_DISPATCHER_H */
