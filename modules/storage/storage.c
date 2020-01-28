/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <errno.h>
#include "log.h"
#include "fs/littlefs_fs.h"
#include "vfs.h"
#include "board.h"
#include "storage.h"

static mtd_mapper_parent_t parent = MTD_PARENT_INIT(NULL);

mtd_mapper_region_t storage_nor_fw_a = {
    .mtd = {
        .driver = &mtd_mapper_driver,
        .sector_count = 128,              /* 512KiB */
        .pages_per_sector = PINETIME_NOR_PAGES_PER_SECTOR,
        .page_size = PINETIME_NOR_PAGE_SIZE,
    },
    .parent = &parent,
    .offset = 0
};

mtd_mapper_region_t storage_nor_fw_b = {
    .mtd = {
        .driver = &mtd_mapper_driver,
        .sector_count = 128,              /* 512KiB */
        .pages_per_sector = PINETIME_NOR_PAGES_PER_SECTOR,
        .page_size = PINETIME_NOR_PAGE_SIZE,
    },
    .parent = &parent,
    .offset = 0x80000 /* 512KiB */
};

mtd_mapper_region_t storage_nor_fs_sys = {
    .mtd = {
        .driver = &mtd_mapper_driver,
        .sector_count = 512,              /* 2MiB */
        .pages_per_sector = PINETIME_NOR_PAGES_PER_SECTOR,
        .page_size = PINETIME_NOR_PAGE_SIZE,
    },
    .parent = &parent,
    .offset = 0x100000 /* offset at 1MiB */
};

mtd_mapper_region_t storage_nor_fs_user = {
    .mtd = {
        .driver = &mtd_mapper_driver,
        .sector_count = 1280,             /* 5MiB */
        .pages_per_sector = PINETIME_NOR_PAGES_PER_SECTOR,
        .page_size = PINETIME_NOR_PAGE_SIZE,
    },
    .parent = &parent,
    .offset = 0x300000 /* 3MiB */
};

static littlefs_desc_t _storage_fs_desc_sys = {
    .lock = MUTEX_INIT,
    .dev = &storage_nor_fs_sys.mtd,
};

static littlefs_desc_t _storage_fs_desc_user = {
    .lock = MUTEX_INIT,
    .dev = &storage_nor_fs_user.mtd,
};

static vfs_mount_t _storage_fs_mount_sys = {
    .fs = &littlefs_file_system,
    .mount_point = STORAGE_FS_SYS_MOUNT_POINT,
    .private_data = &_storage_fs_desc_sys,
};

static vfs_mount_t _storage_fs_mount_user = {
    .fs = &littlefs_file_system,
    .mount_point = STORAGE_FS_USER_MOUNT_POINT,
    .private_data = &_storage_fs_desc_user,
};

static int _storage_prepare(vfs_mount_t *fs)
{
    LOG_INFO("[fs]: Initializing file system mount %s\n", fs->mount_point);
    if (STORAGE_FS_FORCE_FORMAT) {
        assert(vfs_format(fs) == 0);
    }
    int res = vfs_mount(fs);
    if (res < 0) {
        LOG_ERROR("[fs]: Unable to mount filesystem %s: %d\n", fs->mount_point,
                  res);

        res = vfs_format(fs);
        if (res < 0) {
            LOG_ERROR("[fs]: Unable to format filesystem %s: %d\n",
                      fs->mount_point, res);
        }
        return vfs_mount(fs);
    }
    return res;
}

int storage_init(void)
{
    parent.mtd = MTD_0;
    assert(_storage_prepare(&_storage_fs_mount_sys) == 0);
    assert(_storage_prepare(&_storage_fs_mount_user) == 0);
    return 0;
}
