/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <errno.h>

#include "controller.h"
#include "controller/time.h"
#include "controller/battery.h"
#include "log.h"
#include "xtimer.h"
#include "gui.h"
#include "ts_event.h"

#include "periph/wdt.h"

#ifdef MODULE_BLEMAN
#include "bleman.h"
#include "host/ble_gap.h"
#endif

#define CONTROLLER_THREAD_NAME    "controller"
#define CONTROLLER_THREAD_PRIO    6
#define CONTROLLER_STACKSIZE      (THREAD_STACKSIZE_DEFAULT)

static void *_control_thread(void* arg);
static char _stack[CONTROLLER_STACKSIZE];

static controller_t _control;

static void _handle_input_event(event_t *event);

typedef struct {
    ts_event_t super;
    widget_t *widget;
    controller_action_widget_t action;
} controller_widget_event_t;

static controller_widget_event_t ev_widget = {
    .super = { .super = { .handler = _handle_input_event } }
};

static void _handle_input_event(event_t *event)
{
    controller_widget_event_t *ev = (controller_widget_event_t*)event;
    switch(ev->action) {
        case CONTROLLER_ACTION_WIDGET_LEAVE:
            gui_event_submit_switch_widget(widget_get_menu());
            break;
        case CONTROLLER_ACTION_WIDGET_HOME:
            gui_event_submit_switch_widget(widget_get_home());
            break;
        default:
            break;
    }
    ts_event_clear(&ev->super);
}

inline uint16_t controller_get_battery_voltage(controller_t *controller)
{
    return controller_battery_get_voltage(&controller->batt);
}

int controller_action_submit_input_action(widget_t *widget, controller_action_widget_t action)
{
    if (ts_event_claim(&ev_widget.super) == -EBUSY) {
        return -EBUSY;
    }
    LOG_DEBUG("[controller] Submitting widget action\n");
    ev_widget.widget = widget;
    ev_widget.action = action;
    event_post(&_control.queue, &ev_widget.super.super);
    return 0;
}

void controller_add_control_handler(controller_t *controller, control_event_handler_t *handler)
{
    /* See note above for reasons against clist.h */
    control_event_handler_t **last = &controller->handlers;
    handler->next = NULL;
    while (*last) {
        last = &(*last)->next;
    }
    *last = handler;
}

static void _submit_events(controller_t *controller, controller_event_t event)
{
    for (control_event_handler_t *handler = controller->handlers;
         handler; handler = handler->next) {
        if (handler->events & CONTROLLER_EVENT_FLAG(event)) {
            LOG_DEBUG("[controller]: Submitting event %u to %s\n", (unsigned)event,
                     handler->widget->spec->name);
            handler->widget->spec->event(handler->widget, event);
        }
    }
}

static void _controller_wdt_setup(controller_t *controller)
{
    /* Timeout + half a second for good measure :) */
    wdt_setup_reboot(0, CONTROLLER_WDT_TIMEOUT_SEC * MS_PER_SEC + 500);
    wdt_start();
}

static void _controller_wdt_kick(controller_t *controller)
{
    if (CONTROLLER_WDT_RESET_ON_BUTTON_PRESS) {
        if (!(gpio_read(BUTTON0_ENABLE))) {
            LOG_WARNING("[controller]: Skipping WDT kick\n");
            return;
        }
    }

    wdt_kick();
}

#ifdef MODULE_BLEMAN
static void _bleman_control_event_cb(bleman_t *bleman, struct ble_gap_event *event,
                                     void *arg)
{
    controller_t *controller = arg;
    thread_flags_set((thread_t*)sched_threads[controller->pid], CONTROLLER_THREAD_FLAG_BLUETOOTH);
}
#endif

controller_t *controller_get(void)
{
	return &_control;
}

int controller_thread_create(void)
{
    int res = thread_create(_stack, CONTROLLER_STACKSIZE, CONTROLLER_THREAD_PRIO,
                            THREAD_CREATE_STACKTEST, _control_thread,
                            &_control, CONTROLLER_THREAD_NAME);
    return res;
}

static void *_control_thread(void* arg)
{
    controller_t *controller = arg;
    controller->pid = thread_getpid();
    _controller_wdt_setup(controller);
    event_queue_init(&_control.queue);
	controller_time_init();
	controller_battery_init(controller, &controller->batt);
#ifdef MODULE_BLEMAN
    bleman_add_event_handler(bleman_get(), &controller->handler,
                             _bleman_control_event_cb, controller);
#endif

    controller->reset_reason = hal_get_reset_reason();

    LOG_INFO("[controller]: MCU reset reason %02x\n", controller->reset_reason);

    widget_init_installed();

    gui_event_submit_switch_widget(widget_get_home());
    while(1)
    {
        thread_flags_t flags = thread_flags_wait_any(
            THREAD_FLAG_EVENT | CONTROLLER_THREAD_FLAG_TICK | CONTROLLER_THREAD_FLAG_BLUETOOTH
            );
        /* Tick event from the RTC */
        if (flags & CONTROLLER_THREAD_FLAG_TICK) {
            _controller_wdt_kick(controller);
            controller_update_time(controller);
            _submit_events(controller, CONTROLLER_EVENT_TICK);
        }
        if (flags & CONTROLLER_THREAD_FLAG_BLUETOOTH) {
            _submit_events(controller, CONTROLLER_EVENT_BLUETOOTH);
        }
        if (flags & THREAD_FLAG_EVENT) {
            event_t *ev = NULL;
            while ((ev = event_get(&controller->queue))) {
                ev->handler(ev);
            }
        }
    }
    assert(false);
    /* should be never reached */
    return NULL;
}
