/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "log.h"
#include "bleman.h"
#include "bleman/timesync.h"
#include "event/timeout.h"
#include "controller.h"

#define BLE_GATT_SVC_CTS        (0x1805)
#define BLE_GATT_CHR_CUR_TIME   (0x2A2B)

#define TIMESYNC_INTERVAL       (60 * US_PER_MS * MS_PER_SEC)
#define TIMESYNC_INITIAL_WAIT   (5 * US_PER_MS * MS_PER_SEC)

#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static int _time_result(uint16_t conn_handle, const struct ble_gatt_error *error,
                        struct ble_gatt_attr *attr, void *arg)
{
    (void)conn_handle;
    (void)arg;
    if (error->status) {
        LOG_ERROR("[bleman_timesync] error retrieving current time: %d\n", error->status);
        return 0;
    }
    bleman_timesync_ble_cts_t result;
    os_mbuf_copydata(attr->om, 0, sizeof(bleman_timesync_ble_cts_t), &result);
    LOG_INFO("[bleman_timesync] Received data: "
             "%" PRIu16 "-%u-%u %02u:%02u:%02u\n", htons(result.year),
             result.month, result.dayofmonth,
             result.hour, result.minute, result.second);
    controller_time_spec_t time;
    time.year = result.year;
    time.month = result.month - 1;
    time.dayofmonth = result.dayofmonth; /* Bluetooth spec defines Jan as 1 */
    time.hour = result.hour;
    time.minute = result.minute;
    time.second = result.second;
    time.fracs = result.millis;
    controller_time_set_time(controller_get(), &time);
    return 0;
}

static void _read_handler(event_t *ev)
{
    bleman_timesync_t *sync = container_of(ev, bleman_timesync_t, start_read_ev);
    puts("[bleman_timesync] Reading characteristic!");
    int rc = ble_gattc_read(sync->bleman->conn_handle, sync->time_chr.val_handle,
                            _time_result, sync);
    if (rc != 0) {
        LOG_ERROR("read result: %d\n", rc);
    }
}

static int _chr_disced(uint16_t conn_handle, const struct ble_gatt_error *error,
                       const struct ble_gatt_chr *chr, void *arg)
{
    bleman_timesync_t *sync = arg;
    int rc = 0;
    if (error->status) {
        LOG_ERROR("[bleman_timesync] Error: %d\n", error->status);
    }
    if (chr) {
        LOG_DEBUG("[bleman_timesync] post Read event!");
        memcpy(&sync->time_chr, chr, sizeof(chr));
        event_post(&sync->bleman->eq, &sync->start_read_ev);
    }
    return rc;
}

void _time_chrs_handler(event_t *ev)
{
    bleman_timesync_t *sync = container_of(ev, bleman_timesync_t, start_chrs_ev);
    int res = ble_gattc_disc_chrs_by_uuid(sync->bleman->conn_handle,
                                          sync->time_svc.start_handle, sync->time_svc.end_handle,
                                          BLE_UUID16_DECLARE(BLE_GATT_CHR_CUR_TIME),
                                          _chr_disced, sync);
    if (res != 0) {
        LOG_DEBUG("[bleman_timesync] characteristic result : %d\n", res);
    }
}

static int _gatt_disc_svc_fn(uint16_t conn_handle,
                             const struct ble_gatt_error *error,
                             const struct ble_gatt_svc *service,
                             void *arg)
{
    (void)error;
    bleman_timesync_t *sync = arg;
    LOG_DEBUG("[bleman_timesync] Discovery complete: %" PRIu16 ", %"PRIx16"-%"PRIx16"\n", conn_handle, service->start_handle, service->end_handle);
    if ((service) &&
        (ble_uuid_cmp(&service->uuid.u,
                       BLE_UUID16_DECLARE(BLE_GATT_SVC_CTS))== 0)) {
        LOG_DEBUG("[bleman_timesync] Discovered CTS at 0x%"PRIx16"\n", service->start_handle);
        memcpy(&sync->time_svc, service, sizeof(service));
        event_post(&sync->bleman->eq, &sync->start_chrs_ev);
    }
    return 0;
}

static void _sync_handler(event_t *event)
{
    bleman_timesync_t *sync = container_of(event, bleman_timesync_t, ev);
    ble_gattc_disc_svc_by_uuid(sync->bleman->conn_handle, BLE_UUID16_DECLARE(BLE_GATT_SVC_CTS),
                               _gatt_disc_svc_fn, sync);
    event_timeout_set(&sync->timeout_ev, TIMESYNC_INTERVAL);
}

static void _bleman_timesync_event_cb(bleman_t *bleman, struct ble_gap_event *event,
                                      void *arg)
{
    bleman_timesync_t *sync = arg;

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                LOG_INFO("[bleman_timesync]: starting timesync events\n");
                bleman_timesync_start_events(sync);
            }
            return;
        case BLE_GAP_EVENT_DISCONNECT:
            bleman_timesync_stop_events(sync);
            return;
    }
}

void bleman_timesync_start_events(bleman_timesync_t *sync)
{
    event_timeout_set(&sync->timeout_ev, TIMESYNC_INITIAL_WAIT);
}

void bleman_timesync_stop_events(bleman_timesync_t *sync)
{
    event_timeout_clear(&sync->timeout_ev);
}

void bleman_timesync_init(bleman_t *bleman, bleman_timesync_t *sync)
{
    sync->bleman = bleman;
    sync->ev.handler = _sync_handler;
    sync->start_chrs_ev.handler = _time_chrs_handler;
    sync->start_read_ev.handler = _read_handler;
    event_timeout_init(&sync->timeout_ev, &bleman->eq, &sync->ev);
    bleman_add_event_handler(bleman, &sync->handler, _bleman_timesync_event_cb, sync);
}
