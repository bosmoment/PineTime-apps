/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "hal.h"
#include "log.h"

#include "ili9341.h"
#include "ili9341_params.h"

#include "lvgl.h"

/* ili9341 device driver state */
static ili9341_t _disp_dev;
static bool display_on;

void *hal_display_get_context(void)
{
    return (display_t*)&_disp_dev;
}

void hal_display_off(void)
{
    ili9341_set_brightness(hal_display_get_context(), 0x00);
    ili9341_sleep_mode(hal_display_get_context(), true);
    display_on = false;
}

void hal_display_on(void)
{
    ili9341_sleep_mode(hal_display_get_context(), false);
    ili9341_set_brightness(hal_display_get_context(), 0xff);
    display_on = true;
}

/* Should be called somewhere during auto_init */
void hal_init(void)
{
    if (ili9341_init(&_disp_dev, &ili9341_params[0]) == 0) {
        LOG_INFO("[ILI9341]: OK!\n");
        display_on = true;
    }
    else {
        LOG_ERROR("[ILI9341]: Device initialization failed\n");
    }
    if (hal_input_init() == 0) {
        LOG_INFO("[XPT2046]: OK!\n");
    }
    else {
        LOG_ERROR("[XPT2046]: Device initialization failed\n");
    }
}
