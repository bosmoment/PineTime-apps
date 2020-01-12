/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "hal.h"
#include "log.h"
#include "board.h"

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
    gpio_set(LCD_BACKLIGHT_LOW);
    gpio_set(LCD_BACKLIGHT_MID);
    gpio_set(LCD_BACKLIGHT_HIGH);
    ili9341_sleep_mode(hal_display_get_context(), true);
    display_on = false;
}

void hal_display_on(void)
{
    ili9341_sleep_mode(hal_display_get_context(), false);
    display_on = true;
    /* Low brightness for now */
    gpio_clear(LCD_BACKLIGHT_LOW);
    gpio_set(LCD_BACKLIGHT_MID);
    gpio_set(LCD_BACKLIGHT_HIGH);
}

/* Should be called somewhere during auto_init */
void hal_init(void)
{
    gpio_set(VIBRATOR);
    gpio_set(LCD_BACKLIGHT_MID);
    gpio_set(LCD_BACKLIGHT_HIGH);
    if (ili9341_init(&_disp_dev, &ili9341_params[0]) == 0) {
        //ili9341_set_brightness(&_disp_dev, 0xff);
        hal_display_on();
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

void hal_set_button_cb(gpio_cb_t cb, void *arg)
{
    gpio_init(BUTTON0, GPIO_OUT);
    gpio_clear(BUTTON0);
    gpio_init_int(BUTTON0_ENABLE, GPIO_IN_PU, GPIO_FALLING, cb, arg);
}
