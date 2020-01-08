/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * This file overrides the NimBLE log definitions to prevent conflicts with the
 * RIOT-provided LOG defines
 */

#ifndef _BLEMAN_LOG_H
#define _BLEMAN_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

struct log {
};

#endif
