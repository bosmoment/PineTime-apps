/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_UTIL_TS_EVENT
#define APP_UTIL_TS_EVENT

#ifdef __cplusplus
extern "C" {
#endif

#include "event.h"

typedef struct {
    event_t super;
    bool busy;
} ts_event_t;

static inline bool ts_event_is_busy(ts_event_t *event)
{
    return (event->busy == 1);
}

static inline void ts_event_set_busy(ts_event_t *event)
{
    event->busy = 1;
}

static inline void ts_event_clear(ts_event_t *event)
{
    event->busy = 0;
}

int ts_event_claim(ts_event_t *event);

#ifdef __cplusplus
}
#endif

#endif /* APP_UTIL_TS_EVENT */
