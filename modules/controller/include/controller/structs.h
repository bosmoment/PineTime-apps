/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_CONTROLLER_STRUCTS_H
#define APP_CONTROLLER_STRUCTS_H

#include "event.h"
#include "event/timeout.h"
#include "controller/time.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Controller actions as signalled by widgets
 * @{
 */
typedef enum {
    CONTROLLER_ACTION_WIDGET_LEAVE, /* Leave widget and return to menu */
    CONTROLLER_ACTION_WIDGET_HOME, /* Return to home screen */
} controller_action_widget_t;
/** @} */

/**
 * @name Controller events supplied to widgets
 * @{
 */
typedef enum {
    CONTROLLER_EVENT_TICK, /**< Time tick event (1 sec) */
    CONTROLLER_EVENT_BLUETOOTH, /**< Bluetooth state changed */
} controller_event_t;

typedef struct {
   event_t ev;
   event_timeout_t evt;
   mutex_t lock;
   uint16_t last_millivolts;
   uint16_t average_millivolts;
} controller_battery_t;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* APP_CONTROLLER_STRUCTS_H */
