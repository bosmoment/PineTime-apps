/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_HAL
#define APP_HAL

#include <stdint.h>
#include "ili9341.h"
#include "event.h"

#include "hal_input.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void display_t;

static inline void hal_display_flush(display_t *display, uint16_t x1,
                                     uint16_t x2, uint16_t y1, uint16_t y2,
                                     uint16_t *color)
{
    ili9341_t *disp = (ili9341_t*)display;
    ili9341_pixmap(disp, x1, x2, y1, y2, color);
}

display_t *hal_display_get_context(void);

void hal_init(void);
void hal_display_on(void);
void hal_display_off(void);
void hal_set_button_cb(gpio_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* APP_HAL */
