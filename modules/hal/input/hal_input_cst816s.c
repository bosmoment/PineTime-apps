/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "hal.h"
#include "log.h"

#include "cst816s.h"

#include "lv_conf.h"

#if HAL_INPUT_SWITCH_XY
#define HAL_INPUT_X_RES LV_VER_RES_MAX
#define HAL_INPUT_Y_RES LV_HOR_RES_MAX
#else
#define HAL_INPUT_X_RES LV_HOR_RES_MAX
#define HAL_INPUT_Y_RES LV_VER_RES_MAX
#endif

/* xpt2046 device driver state */
static cst816s_t _input_dev;

void *hal_input_get_context(void)
{
    return (input_t*)&_input_dev;
}

int hal_input_init(void)
{
    return cst816s_init(&_input_dev, &_cst816s_input_params, NULL, NULL);
}

int hal_input_get_measurement(input_t *input, hal_input_coord_t *coord)
{
    cst816s_t *dev = (cst816s_t*)input;
    cst816s_touch_data_t touch;
    int res = cst816s_read(dev, &touch, 1);
    if (res <= 0) {
        /* No touch */
        return 0;
    }
    uint16_t x = touch.x;
    uint16_t y = touch.y;

#if HAL_INPUT_REVERSE_Y
    y = LV_VER_RES_MAX - y;
#endif
#if HAL_INPUT_REVERSE_X
    x = LV_VER_RES_MAX - x;
#endif
#if HAL_INPUT_SWITCH_XY
    coord->y = x;
    coord->x = y;
#else
    coord->x = x;
    coord->y = y;
#endif
    return 1;
}

