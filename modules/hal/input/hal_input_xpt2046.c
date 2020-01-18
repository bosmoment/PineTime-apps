/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "hal.h"
#include "log.h"

#include "xpt2046.h"

#include "lv_conf.h"

#if HAL_INPUT_SWITCH_XY
#define HAL_INPUT_X_RES LV_VER_RES_MAX
#define HAL_INPUT_Y_RES LV_HOR_RES_MAX
#else
#define HAL_INPUT_X_RES LV_HOR_RES_MAX
#define HAL_INPUT_Y_RES LV_VER_RES_MAX
#endif

/* xpt2046 device driver state */
static xpt2046_t _input_dev;

void *hal_input_get_context(void)
{
    return (input_t*)&_input_dev;
}

int hal_input_init(void)
{
    return xpt2046_init(&_input_dev, &_xpt2046_input_params);
}

int hal_input_get_measurement(input_t *input, hal_input_coord_t *coord)
{
    xpt2046_t *dev = (xpt2046_t*)input;
    xpt2046_xyz_t xyz;
    int res = xpt2046_get_xyz(dev, &xyz);
    if (res == XPT2046_NO_TOUCH) {
        return 0;
    }

    int16_t x = (((int32_t)xyz.x - HAL_INPUT_TOUCH_X_MIN) * HAL_INPUT_X_RES) /
                (HAL_INPUT_TOUCH_X_MAX - HAL_INPUT_TOUCH_X_MIN);
    int16_t y = (((int32_t)xyz.y - HAL_INPUT_TOUCH_Y_MIN) * HAL_INPUT_Y_RES) /
                (HAL_INPUT_TOUCH_Y_MAX - HAL_INPUT_TOUCH_Y_MIN);
    if (x > HAL_INPUT_X_RES) {
        x = HAL_INPUT_X_RES;
    }
    else if (x < 0) {
        x = 0;
    }
    if (y > HAL_INPUT_Y_RES) {
        y = HAL_INPUT_Y_RES;
    }
    else if (y < 0) {
        y = 0;
    }
    LOG_DEBUG("Z: %" PRIu16"\n", xyz.z);
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
