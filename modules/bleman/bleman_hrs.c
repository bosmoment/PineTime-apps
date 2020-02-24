/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <stdint.h>
#include "log.h"
#include "bleman.h"
#include "net/ble.h"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "services/gatt/ble_svc_gatt.h"

#define SENSOR_LOCATION         (0x02)      /* wrist sensor */

#define BLEMAN_HRS_FAKE_STEP_SIZE       (2U)
#define BLEMAN_HRS_FAKE_MAX             (160U)
#define BLEMAN_HRS_FAKE_MIN             (80U)

typedef struct __attribute__((packed)) {
    uint8_t flags;
    uint16_t bpm;
} _hrs_data_t;

static void _hrs_update(event_t *event)
{
    struct os_mbuf *om;
    bleman_hrs_t *hrs = container_of(event, bleman_hrs_t, ev);

    /* our mock-up heart rate is going up and down */
    if ((hrs->bpm == BLEMAN_HRS_FAKE_MAX) || (hrs->bpm == BLEMAN_HRS_FAKE_MIN)) {
        hrs->step *= -1;
    }
    hrs->bpm += hrs->step;

    LOG_DEBUG("[bleman_hrs] heart rate %" PRIu16"\n", hrs->bpm);
    _hrs_data_t hrs_data = {
        .flags = 0x01,
        .bpm = hrs->bpm,
    };

    /* send heart rate data notification to GATT client */
    om = ble_hs_mbuf_from_flat(&hrs_data, sizeof(hrs_data));
    if (om) {
        ble_gattc_notify_custom(bleman_get()->conn_handle, hrs->handle, om);
    }

    /* schedule next update event */
    event_timeout_set(&hrs->evt, BLEMAN_HEART_RATE_UPDATE_INTERVAL * US_PER_MS);
}

void bleman_hrs_notify(bleman_hrs_t *hrs, bool enable)
{
    if (enable) {
        _hrs_update(&hrs->ev);
    }
    else {
        event_timeout_clear(&hrs->evt);
    }
}

int bleman_hrs_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    if (ble_uuid_u16(ctxt->chr->uuid) != BLE_GATT_CHAR_BODY_SENSE_LOC) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    puts("[READ] heart rate service: body sensor location value");

    uint8_t loc = SENSOR_LOCATION;
    int res = os_mbuf_append(ctxt->om, &loc, sizeof(loc));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

int bleman_hrs_init(bleman_t *bleman, bleman_hrs_t *hrs)
{
    hrs->step = -BLEMAN_HRS_FAKE_STEP_SIZE;
    hrs->bpm = BLEMAN_HRS_FAKE_MIN;
    hrs->ev.handler = _hrs_update;
    event_timeout_init(&hrs->evt, &bleman->eq, &hrs->ev);
    return 0;
}
