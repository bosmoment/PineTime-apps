/*
 * Copyright (C) 2019 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef APP_BLEMAN_CONF_H
#define APP_BLEMAN_CONF_H

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "net/bluetil/ad.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_BLEMAN_CUSTOM_SERVICE
/* GATT service definitions */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID128_DECLARE(CONFIG_BLEMAN_PINETIME_UUID),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            0, /* no more characteristics in this service */
        }, }

    },
    {
        /* Heart Rate Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_HRS),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_HEART_RATE_MEASURE),
            .access_cb = bleman_hrs_handler,
            .val_handle = &_hrs_val_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_BODY_SENSE_LOC),
            .access_cb = bleman_hrs_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        /* Device Information Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_DEVINFO),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_MANUFACTURER_NAME),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
            .arg = &_bleman,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_MODEL_NUMBER_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
            .arg = &_bleman,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_SERIAL_NUMBER_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
            .arg = &_bleman,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_FW_REV_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
            .arg = &_bleman,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_HW_REV_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
            .arg = &_bleman,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        /* Battery Level Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_BAS),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_BATTERY_LEVEL),
            .access_cb = bleman_bas_handler,
            .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_AUTHOR,
            .val_handle = &_bleman.bas.handle,
            .arg = &_bleman,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        0, /* no more services */
    },
};
#endif

#ifdef __cplusplus
}
#endif

#endif /* APP_BLEMAN_CONF_H */
