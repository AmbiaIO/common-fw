/*
 * File Name: ble_manager.c
 *
 * Author: Thuan Le (thuanle@hydratech-iot.com)
 *
 * Description: BLE Manager
 *
 * Copyright 2024, HydraTech. All rights reserved.
 * You may use this file only in accordance with the license, terms, conditions,
 * disclaimers, and limitations in the end user license agreement accompanying
 * the software package with which this file was provided.
 */

/* Includes ----------------------------------------------------------- */
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "ble_manager.h"
#include "ble_peripheral.h"
#include "network_manager.h"
#include "nvs_flash.h"
#include "base_board_defs.h"

/* Private defines ---------------------------------------------------------- */
static const char *TAG = "ble_manager";

/* Private enumerate/structure ---------------------------------------------- */
typedef struct
{
    uint16_t conn_handle;
    uint8_t ble_addr_type;
    bool is_connected;
} ble_manager_ctx_t;

/* Private Constants -------------------------------------------------------- */
/* Private variables -------------------------------------------------------- */
static ble_manager_ctx_t g_ctx;

/* Private macros ----------------------------------------------------------- */
/* Private Constants -------------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
static void ble_advertise(void);
static int ble_gap_event(struct ble_gap_event *event, void *arg);
static void ble_on_sync(void);
static void ble_on_reset(int reason);
static void ble_host_task(void *param);
static void print_addr(const void *addr);

static void ble_manager_received_handler(uint8_t *p_data, uint8_t data_len);

/* Function definitions ----------------------------------------------- */
void ble_manager_init(char *device_name)
{
    int rc;
    ble_manager_ctx_t *ctx = &g_ctx;

    // Initialize the context
    memset(ctx, 0, sizeof(*ctx));
    ctx->is_connected = false;

    nimble_port_init(); // Initialize the NimBLE host configuration

    ble_hs_cfg.sync_cb = ble_on_sync;
    ble_hs_cfg.reset_cb = ble_on_reset;

    ble_peripheral_init(ble_manager_received_handler);

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_svc_gap_device_name_set(device_name);
    assert(rc == 0);

    nimble_port_freertos_init(ble_host_task);
}

void ble_manager_peripheral_send_data(uint8_t *p_data, uint8_t len)
{
    ble_manager_ctx_t *ctx = &g_ctx;

    if (ctx->is_connected)
    {
        ble_peripheral_send_data(ctx->conn_handle, p_data, len);
    }
}

/* Private function definitions ---------------------------------------- */
static void ble_manager_received_handler(uint8_t *p_data, uint8_t data_len)
{
#if (CONFIG_WALL_DIMMER_BOARD)
    network_manager_process_protobuf_data(GATEWAY_PERIPHERAL, p_data, data_len);
#elif (CONFIG_CONTROLLER_ESP32_BOARD)
    network_manager_process_protobuf_data(GATEWAY_CONTROLLER, p_data, data_len);
#else
    ESP_LOGE(TAG, "Invalid board");
#endif
}

// Enables advertising with parameters:
// o General discoverable mode
// o Undirected connectable mode
static void ble_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    ble_manager_ctx_t *ctx = &g_ctx;
    int rc;

    // Set the advertisement data included in our advertisements:
    // o Flags (indicates advertisement type and other general info)
    // o Advertising tx power
    // o Device name
    // o 16-bit service UUIDs (alert notifications).
    memset(&fields, 0, sizeof(fields));

    // Advertise two flags:
    // o Discoverability in forthcoming advertisement (general)
    // o BLE-only (BR/EDR unsupported)
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    // Indicate that the TX power level field should be included; have the
    // stack fill this value automatically.  This is done by assigning the
    // special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)"BLE";
    fields.name_len = 10; // Limit the device name to 10 characters.
    fields.name_is_complete = 1;

    // 16-bit service UUIDs --- {
    fields.uuids16 = (ble_uuid16_t[]){
        BLE_UUID16_INIT(BLE_BOX_UUID)};
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;
    // }

    //  0xFF - Manufacturer specific data
    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(ctx->ble_addr_type, addr_val, NULL);
    fields.mfg_data = (uint8_t *)addr_val;
    fields.mfg_data_len = 6;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0)
    {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    // Begin advertising
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(ctx->ble_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_gap_event, NULL);
    if (rc != 0)
    {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}

static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    ble_manager_ctx_t *ctx = &g_ctx;

    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        // A new connection was established or a connection attempt failed
        MODLOG_DFLT(INFO, "connection %s; status=%d\n",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);

        if (event->connect.status != 0)
        {
            ble_advertise(); // Connection failed; resume advertising
            ctx->is_connected = false;
        }
        else
        {
            ctx->is_connected = true;
        }
        ctx->conn_handle = event->connect.conn_handle;
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ctx->is_connected = false;
        MODLOG_DFLT(INFO, "disconnect; reason=%d\n", event->disconnect.reason);

        ble_advertise(); // Connection terminated; resume advertising
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ctx->is_connected = false;
        MODLOG_DFLT(INFO, "adv complete\n");
        ble_advertise();
        break;

    case BLE_GAP_EVENT_MTU:
        MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.value);
        break;
    }

    return 0;
}

static void ble_on_sync(void)
{
    ble_manager_ctx_t *ctx = &g_ctx;
    int rc;

    rc = ble_hs_id_infer_auto(0, &ctx->ble_addr_type);
    assert(rc == 0);

    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(ctx->ble_addr_type, addr_val, NULL);

    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr_val);
    MODLOG_DFLT(INFO, "\n");

    ble_advertise();
}

static void ble_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void print_addr(const void *addr)
{
    const uint8_t *u8p;

    u8p = addr;
    MODLOG_DFLT(INFO, "%02x:%02x:%02x:%02x:%02x:%02x", u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}

static void ble_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");

    nimble_port_run(); // This function will return only when nimble_port_stop() is executed

    nimble_port_freertos_deinit();
}

/* End of file -------------------------------------------------------- */
