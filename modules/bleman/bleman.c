/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "bleman.h"
#include "bleman/bas.h"
#include "log/log.h"
#include "log.h"
#include "assert.h"
#include "event/timeout.h"
#include "nimble_riot.h"
#include "luid.h"
#include "fmt.h"
#include "bleman/timesync.h"
#include "net/bluetil/ad.h"

/* NimBLE includes */
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"


#define BLEMAN_THREAD_NAME    "bleman"
#define BLEMAN_THREAD_PRIO    5
#define BLEMAN_STACKSIZE     (THREAD_STACKSIZE_LARGE)

static bleman_t _bleman;

static bleman_timesync_t _bleman_timesync;

static const char *_device_name = CONFIG_BLEMAN_DEVICE_NAME;
static const char *_manufacturer_name = CONFIG_BLEMAN_MANUF_NAME;
static const char *_model_number = CONFIG_BLEMAN_MODEL_NUM;
static const char *_fw_ver = CONFIG_BLEMAN_FW_VERSION;
static const char *_hw_ver = CONFIG_BLEMAN_HW_VERSION;

static char _stack[BLEMAN_STACKSIZE];

static void *_bleman_thread(void* arg);
static void _start_advertising(bleman_t *bleman);

extern int bleman_hrs_handler(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg);
extern int bleman_bas_handler(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg);
static int _devinfo_handler(uint16_t conn_handle, uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, void *arg);
static uint16_t _hrs_val_handle;

#include "bleman_conf.h"

extern int bleman_bas_init(bleman_t *bleman, bleman_bas_t *bas);

int bleman_thread_create(void)
{
    bleman_t *bleman = &_bleman;
    event_queue_init(&bleman->eq);
    int res = thread_create(_stack, BLEMAN_STACKSIZE, BLEMAN_THREAD_PRIO,
                            THREAD_CREATE_STACKTEST, _bleman_thread,
                            bleman, BLEMAN_THREAD_NAME);
    return res;
}

static int _devinfo_handler(uint16_t conn_handle, uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    const char *str;
    bleman_t *bleman = arg;

    switch (ble_uuid_u16(ctxt->chr->uuid)) {
        case BLE_GATT_CHAR_MANUFACTURER_NAME:
            printf("[READ] device information service: manufacturer name value\n");
            str = _manufacturer_name;
            break;
        case BLE_GATT_CHAR_MODEL_NUMBER_STR:
            printf("[READ] device information service: model number value\n");
            str = _model_number;
            break;
        case BLE_GATT_CHAR_SERIAL_NUMBER_STR:
            printf("[READ] device information service: serial number value\n");
            str = bleman->serial;
            break;
        case BLE_GATT_CHAR_FW_REV_STR:
            printf("[READ] device information service: firmware revision value\n");
            str = _fw_ver;
            break;
        case BLE_GATT_CHAR_HW_REV_STR:
            printf("[READ] device information service: hardware revision value\n");
            str = _hw_ver;
            break;
        default:
            return BLE_ATT_ERR_UNLIKELY;
    }

    printf("[READ] Result: \"%s\"\n", str);

    int res = os_mbuf_append(ctxt->om, str, strlen(str));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static void _bleman_notify_handlers(struct ble_gap_event *event, bleman_t *bleman)
{
    for (bleman_event_handler_t *handler = bleman->handlers; handler; handler = handler->next) {
        handler->handler(bleman, event, handler->arg);
    }
}

/* GAP events */
static int _gap_event_cb(struct ble_gap_event *event, void *arg)
{
    bleman_t *bleman = arg;

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status) {
                _start_advertising(bleman);
                bleman->conn_handle = 0;
                return 0;
            }
            else {
                bleman->conn_handle = event->connect.conn_handle;
                bleman->state = BLEMAN_BLE_STATE_CONNECTED;

                printf("[bleman] Connection established\n");
            }

            break;

        case BLE_GAP_EVENT_DISCONNECT:
            bleman->state = BLEMAN_BLE_STATE_DISCONNECTED;
            bleman->conn_handle = 0;
            _start_advertising(bleman);
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
            if (event->subscribe.attr_handle == _hrs_val_handle) {
                if (event->subscribe.cur_notify == 1) {
                    //_start_updating();
                }
                else {
                    //_stop_updating();
                }
            }
            else if (event->subscribe.attr_handle == bleman->bas.handle) {
                if (event->subscribe.cur_notify == 1) {
                   bleman_bas_notify(&bleman->bas, true);
                }
                else {
                   bleman_bas_notify(&bleman->bas, false);
                }
            }
            break;
	    case BLE_GAP_EVENT_PASSKEY_ACTION:
        {
            printf("passkey action event; action=%d",
                   event->passkey.params.action);
            if (event->passkey.params.action == BLE_SM_IOACT_NUMCMP) {
                printf(" numcmp=%06lu",
                           (unsigned long)event->passkey.params.numcmp);
            }
            printf("\n");
            if (event->passkey.params.action == BLE_SM_IOACT_NUMCMP) {
                struct ble_sm_io pk;
                pk.action = BLE_SM_IOACT_NUMCMP;
                pk.numcmp_accept = 1;
                int rc = ble_sm_inject_io(bleman->conn_handle, &pk);
                if (rc) {
                    printf("Confirming passcode failed\n");
                }
            }
            return 0;
        }
        case BLE_GAP_EVENT_ENC_CHANGE:
            printf("[ENC_CHANGE]: event status %d\n", event->enc_change.status);
            if (event->enc_change.status == 0) {
                /* Secure connection */
            }
            break;
        case BLE_GAP_EVENT_MTU:
            printf("mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                       event->mtu.conn_handle,
                       event->mtu.channel_id,
                       event->mtu.value);
            break;
        default:
            printf("[bleman]: unhandled event type: %u\n", event->type);
    }
    _bleman_notify_handlers(event, bleman);

    return 0;
}

static void _start_advertising(bleman_t *bleman)
{
    struct ble_gap_adv_params advp;

    memset(&advp, 0, sizeof advp);
    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;
    advp.itvl_min  = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    advp.itvl_max  = BLE_GAP_ADV_FAST_INTERVAL1_MAX;
    bleman->state = BLEMAN_BLE_STATE_ADVERTISING;
    int res = ble_gap_adv_start(nimble_riot_own_addr_type, NULL, BLE_HS_FOREVER,
                                &advp, _gap_event_cb, bleman);
    if (res != 0) {
        printf("[bleman] Error enabling advertisement mode: %d\n", res);
    }
}

static void _fill_serial(bleman_t *bleman)
{
    uint8_t serial_bytes[CONFIG_BLEMAN_SERIAL_LEN/2];
    luid_get(serial_bytes, sizeof(serial_bytes));
    fmt_bytes_hex(bleman->serial, serial_bytes, sizeof(serial_bytes));
}

void bleman_add_event_handler(bleman_t *bleman, bleman_event_handler_t *event,
                              bleman_event_cb_t cb, void *arg)
{
    event->arg = arg;
    event->handler = cb;
    event->next = bleman->handlers;
    bleman->handlers = event;
}

bleman_t *bleman_get(void)
{
    return &_bleman;
}

bleman_ble_state_t bleman_get_conn_state(bleman_t *bleman, struct ble_gap_conn_desc *state)
{
    ble_gap_conn_find(bleman->conn_handle, state);
    return bleman->state;
}

static void *_bleman_thread(void *arg)
{
    int res = 0;
    bleman_t *bleman = arg;

    LOG_INFO("[bleman] Initializing as thread no %u\n", (unsigned)sched_active_pid);

    _fill_serial(bleman);
    assert(bleman->serial);

    /* setup local event queue (for handling heart rate updates) */
    event_queue_init(&bleman->eq);

    //_update_evt.handler = _hr_update;
    //_disc_evt.handler = _disc_update;

    //_chrs_evt.handler = _chrs_handler;
    //_read_evt.handler = _read_handler;

    bleman_bas_init(bleman, &bleman->bas);

    /* verify and add our custom services */
    res = ble_gatts_count_cfg(gatt_svr_svcs);
    assert(res == 0);
    res = ble_gatts_add_svcs(gatt_svr_svcs);
    assert(res == 0);

    /* set the device name */
    ble_svc_gap_device_name_set(_device_name);
    /* reload the GATT server to link our added services */
    ble_gatts_start();

    /* configure and set the advertising data */
    uint8_t buf[BLE_HS_ADV_MAX_SZ];
    bluetil_ad_t ad;
    bluetil_ad_init_with_flags(&ad, buf, sizeof(buf), BLUETIL_AD_FLAGS_DEFAULT);
    const ble_uuid128_t uuid = BLE_UUID128_INIT(CONFIG_BLEMAN_PINETIME_UUID);
    bluetil_ad_add(&ad, BLE_GAP_AD_UUID128_INCOMP, &uuid.value, sizeof(uuid.value));
    bluetil_ad_add_name(&ad, _device_name);
    ble_gap_adv_set_data(ad.buf, ad.pos);

    bleman_timesync_init(bleman, &_bleman_timesync);

    /* start to advertise this node */
    _start_advertising(bleman);

    /* run an event loop for handling the heart rate update events */
    event_loop(&bleman->eq);

    return NULL;
}
