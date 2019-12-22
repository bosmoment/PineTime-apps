/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <errno.h>
#include "ts_event.h"
#include "irq.h"

int ts_event_claim(ts_event_t *event)
{
    unsigned state =irq_disable();
    int res = -EBUSY;
    if (!ts_event_is_busy(event)) {
        ts_event_set_busy(event);
        res = 0;
    }
    irq_restore(state);
    return res;
}

