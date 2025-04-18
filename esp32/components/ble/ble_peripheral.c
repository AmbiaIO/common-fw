/*
 * File Name: ble_peripheral.c
 *
 * Author: hello@hydratech-iot.com
 *
 * Description: BLE Peripheral
 *
 * Copyright 2024, HydraTech. All rights reserved.
 * You may use this file only in accordance with the license, terms, conditions,
 * disclaimers, and limitations in the end user license agreement accompanying
 * the software package with which this file was provided.
 */

/* Includes ----------------------------------------------------------- */
#include "ble_peripheral.h"
#include "base_include.h"

/* Private defines ---------------------------------------------------- */
static char *TAG = "ble_peripheral";

/* Private enumerate/structure ---------------------------------------- */
typedef struct
{
    ble_receive_cb_t receive_callback;
    uint16_t uds_tx_handle;
} ble_peripheral_ctx_t;

/* Private variables -------------------------------------------------- */
const uint8_t UDS_CHAR_UUID[][16] =
    {
        {BLE_UUID_UDS_TX_CHARACTERISTIC},
        {BLE_UUID_UDS_RX_CHARACTERISTIC},
};

static ble_peripheral_ctx_t g_ctx;

/* Private function prototypes ---------------------------------------- */
static int m_ble_peripheral_access(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt, void *arg);
static int m_ble_peripheral_chr_write(struct os_mbuf *om, uint16_t min_len,
                                      uint16_t max_len, void *dst,
                                      uint16_t *len);

/* Private enumerate/structure ---------------------------------------- */
static const struct ble_gatt_svc_def ble_peripheral_defs[] =
{
    {// Service: User data Service (UDS)
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID128_DECLARE(BLE_UUID_UDS_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = BLE_UUID128_DECLARE(BLE_UUID_UDS_TX_CHARACTERISTIC),
                .access_cb = m_ble_peripheral_access,
                .val_handle = &g_ctx.uds_tx_handle,
                .flags = BLE_GATT_CHR_F_NOTIFY,
            },
            {
                .uuid = BLE_UUID128_DECLARE(BLE_UUID_UDS_RX_CHARACTERISTIC),
                .access_cb = m_ble_peripheral_access,
                .flags = BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_WRITE,
            },
            {
                0, // No more characteristics in this service
            },
        }},
    {
        0, // No more services
    },
};

/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Function definitions ----------------------------------------------- */
void ble_peripheral_init(ble_receive_cb_t receive_cb)
{
    ble_peripheral_ctx_t *ctx = &g_ctx;

    // Initialize the context
    memset(ctx, 0, sizeof(*ctx));

    ble_gatts_count_cfg(ble_peripheral_defs);

    ble_gatts_add_svcs(ble_peripheral_defs);

    // LOG_INF("Bluetooth initialized");
    ctx->receive_callback = receive_cb;
}

void ble_peripheral_send_data(uint16_t conn_handle, uint8_t *p_data, uint8_t len)
{
    struct os_mbuf *om;
    ble_peripheral_ctx_t *ctx = &g_ctx;

    om = ble_hs_mbuf_from_flat(p_data, len);
    ble_gattc_notify_custom(conn_handle, ctx->uds_tx_handle, om);
}

/* Private function definitions---------------------------------------- */
static int m_ble_peripheral_access(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ble_peripheral_ctx_t *ctx = &g_ctx;
    int rc = 0;
    uint8_t uds_rx_buf[256] = {0};
    uint16_t uds_rx_buf_len;

    if (memcmp(BLE_UUID128(ctxt->chr->uuid)->value, UDS_CHAR_UUID[UDS_RX_CHARACTERISTIC], 16) == 0)
    {
        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR)
        {
            rc = m_ble_peripheral_chr_write(ctxt->om, 0, sizeof(uds_rx_buf), uds_rx_buf, &uds_rx_buf_len);

            ctx->receive_callback((uint8_t *)uds_rx_buf, uds_rx_buf_len);
        }
    }

    return rc;
}

/**
 * Writes the received value from a characteristic write to
 * the given destination.
 */
static int m_ble_peripheral_chr_write(struct os_mbuf *om, uint16_t min_len,
                                      uint16_t max_len, void *dst,
                                      uint16_t *len)
{
    uint16_t om_len;
    int rc;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len)
    {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0)
    {
        return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

/* End of file -------------------------------------------------------- */
