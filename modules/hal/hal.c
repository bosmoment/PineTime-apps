/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "board.h"
#include "hal.h"
#include "log.h"
#include "board.h"
#include "periph/adc.h"

#include "ili9341.h"
#include "ili9341_params.h"
#include "ili9341_internal.h"

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

uint32_t hal_battery_read_voltage(void)
{
    int sample = adc_sample(BATTERY_ADC, ADC_RES_12BIT);
    return ((uint32_t)sample * 2000) / 1241;
}

int hal_battery_get_percentage(uint32_t voltage)
{
    /* 4200mV is full, 3500mV is empty */
    int percentage = (voltage - 3500) / 7;
    if (percentage > 100) {
        return 100;
    }
    else if (voltage < 3500) {
        return 0;
    }
    return percentage;
}

bool hal_battery_is_powered(void)
{
    return gpio_read(POWER_PRESENCE) ? false : true;
}

bool hal_battery_is_charging(void)
{
    return gpio_read(CHARGING_ACTIVE) ? false : true;
}

hal_reset_reason_t hal_get_reset_reason(void)
{
    uint32_t reset_reason = NRF_POWER->RESETREAS;
    NRF_POWER->RESETREAS = 0x000f000f; /* Writing 1 to the reason clears */
    if (reset_reason == 0) {
        return HAL_RESET_REASON_ON_CHIP;
    }
    else {
        return bitarithm_lsb(reset_reason);
    }
}

/* Should be called somewhere during auto_init */
void hal_init(void)
{
    gpio_set(VIBRATOR);
    gpio_set(LCD_BACKLIGHT_MID);
    gpio_set(LCD_BACKLIGHT_HIGH);
    if (ili9341_init(&_disp_dev, &ili9341_params[0]) == 0) {
        hal_display_off();
        if (HAL_DISPLAY_DISABLE_INVERT_COLORS) {
            ili9341_invert_off(&_disp_dev);
        }
        if (HAL_DISPLAY_COLORS_BGR) {
            static const uint8_t command_params = ILI9341_MADCTL_HORZ_FLIP |
                                                  ILI9341_MADCTL_BGR;
            ili9341_write_cmd(&_disp_dev, ILI9341_CMD_MADCTL, &command_params,
                       sizeof(command_params));
        }
        //ili9341_set_brightness(&_disp_dev, 0xff);
        hal_display_on();
        LOG_INFO("[ILI9341]: OK!\n");
        display_on = true;
    }
    else {
        LOG_ERROR("[ILI9341]: Device initialization failed\n");
    }
    if (hal_input_init() == 0) {
        LOG_INFO("[cst816s]: OK!\n");
    }
    else {
        LOG_ERROR("[cst816s]: Device initialization failed\n");
    }
    adc_init(BATTERY_ADC);
    gpio_init(POWER_PRESENCE, GPIO_IN);
    gpio_init(CHARGING_ACTIVE, GPIO_IN);
}

void hal_set_button_cb(gpio_cb_t cb, void *arg)
{
    gpio_init(BUTTON0, GPIO_OUT);
    gpio_clear(BUTTON0);
    gpio_init_int(BUTTON0_ENABLE, GPIO_IN_PU, GPIO_FALLING, cb, arg);
}
