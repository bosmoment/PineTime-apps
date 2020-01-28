/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _APP_STORAGE_H
#define _APP_STORAGE_H

#include <stdint.h>
#include "board.h"
#include "mtd.h"
#include "mtd_mapper.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STORAGE_FS_SYS_MOUNT_POINT      "/sys"
#define STORAGE_FS_USER_MOUNT_POINT     "/user"

#ifndef STORAGE_FS_FORCE_FORMAT
#define STORAGE_FS_FORCE_FORMAT         (0)
#endif

/**
 * @name storage regions
 *
 * A number of storage regions are defined for different types of persistent
 * data.
 *  - 512KB firmware A
 *  - 512KB firmware B
 *  - 2MB system storage
 *  - 5MB user storage
 *
 * Two regions of 512KB are reserved for firmware images. These are treated as
 * raw block storage, no filesystem is present on these regions.
 *
 * The other two regions are two separate filesystem partitions.
 *
 * The first region of 2MB in size is used for system data which is considered
 * essential for system operation.
 *
 * The last region of 5MB in size is for the end user of the phone to store data
 * in.
 *
 * @{
 */
extern mtd_mapper_region_t storage_nor_fw_a;
extern mtd_mapper_region_t storage_nor_fw_b;
extern mtd_mapper_region_t storage_nor_fs_sys;
extern mtd_mapper_region_t storage_nor_fs_user;
/** @} */

int storage_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _APP_STORAGE_H */
