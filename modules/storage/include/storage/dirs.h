/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _APP_STORAGE_DIRS_H
#define _APP_STORAGE_DIRS_H

#include <stdint.h>
#include "board.h"
#include "mtd.h"
#include "mtd_mapper.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _storage_dir storage_dir_t;

struct _storage_dir {
    const storage_dir_t *subdirs;
    const char *name;
};

#ifdef __cplusplus
}
#endif

#endif /* _APP_STORAGE_H */
