/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "log.h"
#include "bleman.h"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "services/gatt/ble_svc_gatt.h"
#include "controller.h"

typedef struct __attribute__((packed)) {
    uint8_t flags;
    uint16_t level;
} battery_bas_data_t;

static void _bas_update(event_t *event)
{
    struct os_mbuf *om;
    bleman_bas_t *bas = container_of(event, bleman_bas_t, ev);

    uint16_t battery_voltage = controller_get_battery_voltage(controller_get());
    uint8_t percentage = hal_battery_get_percentage(battery_voltage);

    LOG_DEBUG("[bleman_bas] battery measurement %u\n", percentage);

    if (percentage != bas->last_percentage) {
        om = ble_hs_mbuf_from_flat(&percentage, sizeof(percentage));
        if (om) {
            ble_gattc_notify_custom(bleman_get()->conn_handle, bas->handle, om);
        }
    }
    /* schedule next update event */
    event_timeout_set(&bas->evt, BLEMAN_BATTERY_UPDATE_INTERVAL * US_PER_MS);
}

void bleman_bas_notify(bleman_bas_t *bas, bool enable)
{
    if (enable) {
        _bas_update(&bas->ev);
    }
    else {
        event_timeout_clear(&bas->evt);
    }
}

int bleman_bas_handler(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    uint16_t level = controller_get_battery_voltage(controller_get());
    uint8_t percentage = hal_battery_get_percentage(level);
    LOG_DEBUG("[bleman_bas] battery level service: battery level value %u\n", percentage);
    int res = os_mbuf_append(ctxt->om, &percentage, sizeof(percentage));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

int bleman_bas_init(bleman_t *bleman, bleman_bas_t *bas)
{
    bas->ev.handler = _bas_update;
    event_timeout_init(&bas->evt, &bleman->eq, &bas->ev);
    return 0;
}
