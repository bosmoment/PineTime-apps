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
#include "bitarithm.h"
#include "periph/adc.h"

#include "ili9341.h"
#include "ili9341_params.h"
#include "ili9341_internal.h"

#include "lvgl.h"

/* ili9341 device driver state */
static ili9341_t _disp_dev;
static bool display_on;

typedef struct {
    uint32_t bound; /**< Upper bound of this region */
    int32_t A;
    int32_t B;
} hal_battery_piece_t;

static const hal_battery_piece_t hal_battery_piecewise_func[] = {
    //(7396.0, -457.0, 461.0, -7803.0, 192.0, -19026.0, 508.0, -6830.0, 888.0,
    //-3527.0, 8475.0, 493.0)
    {.bound = 3530, .A = 7396, .B = -457},
    {.bound = 3620, .A = 461, .B = -7803},
    {.bound = 3680, .A = 192, .B = -19026},
    {.bound = 3830, .A = 508, .B = -6830},
    {.bound = 3900, .A = 888, .B = -3527},
    {.bound = UINT32_MAX, .A = 8475, .B = 493},
};

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

void hal_display_scroll(uint16_t lines)
{
    ili9341_set_scroll_start(hal_display_get_context(), lines);
}

uint32_t hal_battery_read_voltage(void)
{
    int sample = adc_sample(BATTERY_ADC, ADC_RES_12BIT);
    return ((uint32_t)sample * 2000) / 1241;
}

int hal_battery_get_percentage(uint32_t voltage)
{
    /* 4200mV is full, 3500mV is empty */
    int percentage = 100;
    for (size_t i = 0; i < ARRAY_SIZE(hal_battery_piecewise_func); i++) {
        const hal_battery_piece_t *piece = &hal_battery_piecewise_func[i];
        if (voltage <= piece->bound) {
            percentage = ((1024 * (int32_t)voltage) / piece->A + piece->B) / 10;
            break;
        }
    }

    if (percentage < 0) {
        return 0;
    }
    else if (percentage > 100) {
        return 100;
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

uint32_t hal_get_internal_temp(void)
{
    NRF_TEMP->TASKS_START = 1;
    while (!NRF_TEMP->EVENTS_DATARDY) {
    }
    NRF_TEMP->EVENTS_DATARDY = 0;
    NRF_TEMP->TASKS_STOP = 1;
    return NRF_TEMP->TEMP;
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
        LOG_INFO("[ILI9341]: OK!\n");
        display_on = false;

        ili9341_set_fixed_scroll_area(&_disp_dev, 0, 0);
        hal_display_scroll(0);
    }
    else {
        LOG_ERROR("[ILI9341]: Device initialization failed\n");
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
