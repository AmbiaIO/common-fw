/*
 * File Name: esp_now_manager.h
 *
 * Author: hello@hydratech-iot.com
 *
 * Description: ESP-NOW Manager
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
#include "esp_now.h"

/* Public defines ----------------------------------------------------- */
typedef enum
{
    ESP_NOW_MANAGER_DATA_BROADCAST,
    ESP_NOW_MANAGER_DATA_UNICAST,
    ESP_NOW_MANAGER_DATA_MAX,
} esp_now_manager_data_type_t;

typedef enum
{
    ESP_NOW_MANAGER_SEND_CB,
    ESP_NOW_MANAGER_RECV_CB,
} esp_now_manager_event_id_t;

typedef struct
{
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    esp_now_send_status_t status;
} esp_now_manager_event_send_cb_t;

typedef struct
{
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t data[ESP_NOW_MAX_DATA_LEN];
    int data_len;
} esp_now_manager_event_recv_cb_t;

typedef union
{
    esp_now_manager_event_send_cb_t send_cb;
    esp_now_manager_event_recv_cb_t recv_cb;
} esp_now_manager_event_info_t;

/* When ESP-NOW sending or receiving callback function is called, post event to ESP-NOW task. */
typedef struct
{
    esp_now_manager_event_id_t id;
    esp_now_manager_event_info_t info;
} esp_now_manager_event_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/* Public defines ----------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
void esp_now_manager_init(void);
void esp_now_manager_deinit(void);
void esp_now_manager_add_peer(uint8_t *peer_mac);
void esp_now_manager_send_data(uint8_t *p_data, uint8_t len);

/* End of file -------------------------------------------------------- */
