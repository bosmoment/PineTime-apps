/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef WIDGET_HOME_TIME_CONF_H
#define WIDGET_HOME_TIME_CONF_H

#include "home_time.h"

#ifdef __cplusplus
extern "C" {
#endif

extern home_time_widget_t home_time_widget;
#define WIDGET_HOME_TIME (&home_time_widget.widget)

#ifdef __cplusplus
}
#endif


#endif /* WIDGET_HOME_TIME_CONF_H */
