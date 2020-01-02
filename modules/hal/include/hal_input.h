/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_HAL_INPUT
#define APP_HAL_INPUT

#include <stdint.h>
#include "xpt2046.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAL_INPUT_TOUCH_X_MIN
#define HAL_INPUT_TOUCH_X_MIN   260
#endif

#ifndef HAL_INPUT_TOUCH_Y_MIN
#define HAL_INPUT_TOUCH_Y_MIN   400
#endif

#ifndef HAL_INPUT_TOUCH_X_MAX
#define HAL_INPUT_TOUCH_X_MAX   3850
#endif

#ifndef HAL_INPUT_TOUCH_Y_MAX
#define HAL_INPUT_TOUCH_Y_MAX   2970
#endif

/* TODO: move to params include */
static const xpt2046_params_t _input_params = {
    .spi = SPI_DEV(0),
    .spi_clk = SPI_CLK_1MHZ,
    .cs_pin  = GPIO_PIN(0, 28),
};

typedef void input_t;

typedef struct {
    uint16_t x;
    uint16_t y;
} hal_input_coord_t;

void *hal_input_get_context(void);

/**
 * @brief returns a measurement of the display touch screen
 *
 * @returns     0 if no touch detected
 * @returns     1 if touched
 * @returns     negative on error
 */
int hal_input_get_measurement(input_t *input, hal_input_coord_t *coord);

int hal_input_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_HAL_INPUT */
