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

void *hal_display_get_context(void)
{
    return (display_t*)&_disp_dev;
}

/* Should be called somewhere during auto_init */
void hal_init(void)
{
    if (ili9341_init(&_disp_dev, &ili9341_params[0]) == 0) {
        LOG_INFO("[ILI9341]: OK!\n");
    }
    else {
        LOG_ERROR("[ILI9341]: Device initialization failed\n");
    }
}
