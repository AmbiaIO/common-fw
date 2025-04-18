/*
 * File Name: ble_peripheral.h
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

/* Define to prevent recursive inclusion ------------------------------ */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------- */
#include "base_include.h"
#include "base_type.h"

/* NimBLE stack APIs */
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/* Public defines ----------------------------------------------------- */
#define BLE_UUID_UDS_SERVICE               0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e

#define BLE_BOX_UUID                       (0x0000)

// UUIDs for RX and TX characteristics
#define BLE_UUID_UDS_TX_CHARACTERISTIC      0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e
#define BLE_UUID_UDS_RX_CHARACTERISTIC      0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief User data charateristic enum
 */
typedef enum
{
   UDS_TX_CHARACTERISTIC
  ,UDS_RX_CHARACTERISTIC
}
ble_uds_char_t;

/* Public defines ----------------------------------------------------- */
typedef void (*ble_receive_cb_t)(uint8_t *p_data, uint8_t data_len);

/* Public function prototypes ----------------------------------------- */
void ble_peripheral_init(ble_receive_cb_t receive_cb);
void ble_peripheral_send_data(uint16_t conn_handle, uint8_t *p_data, uint8_t len);

/* End of file -------------------------------------------------------- */
