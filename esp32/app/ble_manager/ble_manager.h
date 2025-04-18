/*
 * File Name: ble_manager.h
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

/* Define to prevent recursive inclusion ------------------------------ */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ----------------------------------------------------------- */
#include "ble_peripheral.h"

/* Public defines ----------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
void ble_manager_init(char *device_name);
void ble_manager_peripheral_send_data(uint8_t *p_data, uint8_t len);

/* -------------------------------------------------------------------------- */
#ifdef __cplusplus
} // extern "C"
#endif

/* End of file -------------------------------------------------------- */
