/*
 * File Name: esp_now_manager.c
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

/* Includes ----------------------------------------------------------- */
#include "esp_now_manager.h"
#include "network_manager.h"
#include "base_include.h"

#include "nvs_flash.h"
#include "esp_random.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_crc.h"

/* Private defines ---------------------------------------------------- */
static char *TAG = "esp_now_manager";

#define ESP_NOW_QUEUE_SIZE              (6)
#define ESP_NOW_MAX_DELAY               (200)
#define ESP_NOW_CHANNEL                 (1)

#define ESP_NOW_WIFI_MODE               WIFI_MODE_STA
#define ESP_NOW_WIFI_IF                 ESP_IF_WIFI_STA

#define ESP_NOW_PMK                     "pmk1234567890123"

/* Private enumerate/structure ---------------------------------------- */
typedef struct
{
    esp_now_manager_event_t evt;
    QueueHandle_t queue;
    esp_now_peer_info_t peer;
} esp_now_manager_ctx_t;

/* Private variables -------------------------------------------------- */
static esp_now_manager_ctx_t g_ctx;

static uint8_t broadcast_mac[ESP_NOW_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
#define IS_BROADCAST_ADDR(addr) (memcmp(addr, broadcast_mac, ESP_NOW_ETH_ALEN) == 0)

/* Public variables --------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
static void esp_now_manager_send_callback(const uint8_t *mac_addr, esp_now_send_status_t status);
static void esp_now_manager_receive_callback(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
static void esp_now_manager_task(void *parameter);

/* Function definitions ----------------------------------------------- */
/* WiFi should start before using ESP-NOW */
static void esp_now_manager_wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(ESP_NOW_WIFI_MODE) );
    ESP_ERROR_CHECK( esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK( esp_wifi_set_protocol(ESP_NOW_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
}

void esp_now_manager_init(void)
{
    esp_now_manager_ctx_t *ctx = &g_ctx;

    // Initialize the context
    memset(ctx, 0, sizeof(*ctx));

    esp_now_manager_wifi_init();

    // Initialize ESP-NOW and register sending and receiving callback function.
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(esp_now_manager_send_callback));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(esp_now_manager_receive_callback));

    // Set primary master key
    ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)ESP_NOW_PMK) );

    // Add broadcast peer information to peer list.
    esp_now_manager_add_peer(broadcast_mac);

    // Create queue for ESP-NOW task
    ctx->queue = xQueueCreate(ESP_NOW_QUEUE_SIZE, sizeof(esp_now_manager_event_t));
    if (ctx->queue == NULL)
    {
        ESP_LOGE(TAG, "Create queue fail");
        return;
    }

    // Create ESP-NOW task
    xTaskCreate(esp_now_manager_task, "esp_now_manager", 2048 * 2, NULL, 5, NULL);
}

void esp_now_manager_deinit(void)
{
    esp_now_manager_ctx_t *ctx = &g_ctx;

    ESP_ERROR_CHECK(esp_now_deinit());

    // Delete queue for ESP-NOW task
    vQueueDelete(ctx->queue);
    ctx->queue = NULL;

    // Delete ESP-NOW task
    vTaskDelete(NULL);
}

void esp_now_manager_add_peer(uint8_t *peer_mac)
{
    esp_now_manager_ctx_t *ctx = &g_ctx;

    memset(&ctx->peer, 0, sizeof(esp_now_peer_info_t));
    
    ctx->peer.channel = ESP_NOW_CHANNEL;
    ctx->peer.ifidx   = ESP_NOW_WIFI_IF;
    ctx->peer.encrypt = false;

    memcpy(ctx->peer.peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(&ctx->peer));
}

void esp_now_manager_send_data(uint8_t *p_data, uint8_t len)
{
    ESP_ERROR_CHECK(esp_now_send(broadcast_mac, p_data, len));
}

/* Private function definitions---------------------------------------------- */
static void esp_now_manager_task(void *parameter)
{
    esp_now_manager_ctx_t *ctx = &g_ctx;

    while (xQueueReceive(ctx->queue, &ctx->evt, portMAX_DELAY) == pdTRUE)
    {
        switch (ctx->evt.id)
        {
        case ESP_NOW_MANAGER_SEND_CB:
        {
            esp_now_manager_event_send_cb_t *send_cb = &ctx->evt.info.send_cb;

            ESP_LOGI(TAG, "Send data to " MACSTR ", status: %d", MAC2STR(send_cb->mac_addr), send_cb->status);
            break;
        }
        case ESP_NOW_MANAGER_RECV_CB:
        {
            esp_now_manager_event_recv_cb_t *recv_cb = &ctx->evt.info.recv_cb;

            ESP_LOGI(TAG, "Receive data from " MACSTR ", len: %d", MAC2STR(recv_cb->mac_addr), recv_cb->data_len);

            network_manager_process_uart_data(recv_cb->data, recv_cb->data_len);
            break;
        }
        default:
            ESP_LOGE(TAG, "Callback type error: %d", ctx->evt.id);
            break;
        }
    }
}

static void esp_now_manager_send_callback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    esp_now_manager_ctx_t *ctx = &g_ctx;

    if (mac_addr == NULL)
    {
        ESP_LOGE(TAG, "Send callback argument error");
        return;
    }

    // Fill event info
    ctx->evt.id = ESP_NOW_MANAGER_SEND_CB;
    memcpy(ctx->evt.info.send_cb.mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    ctx->evt.info.send_cb.status = status;

    // Post event to ESP-NOW task
    if (xQueueSend(ctx->queue, &ctx->evt, ESP_NOW_MAX_DELAY) != pdTRUE)
    {
        ESP_LOGW(TAG, "Send send queue fail");
    }
}

static void esp_now_manager_receive_callback(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    esp_now_manager_ctx_t *ctx = &g_ctx;
    uint8_t *mac_addr = recv_info->src_addr;
    uint8_t *des_addr = recv_info->des_addr;

    if (mac_addr == NULL || data == NULL || len <= 0)
    {
        ESP_LOGE(TAG, "Receive cb arg error");
        return;
    }

    if (IS_BROADCAST_ADDR(des_addr))
    {
        /* If added a peer with encryption before, the receive packets may be
         * encrypted as peer-to-peer message or unencrypted over the broadcast channel.
         * Users can check the destination address to distinguish it.
         */
        ESP_LOGI(TAG, "Receive broadcast ESP-NOW data");
    }
    else
    {
        ESP_LOGI(TAG, "Receive unicast ESP-NOW data");
    }

    // Fill event info
    ctx->evt.id = ESP_NOW_MANAGER_RECV_CB;
    memcpy(ctx->evt.info.recv_cb.mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    memcpy(ctx->evt.info.recv_cb.data, data, len);
    ctx->evt.info.recv_cb.data_len = len;

    // Post event to ESP-NOW task
    if (xQueueSend(ctx->queue, &ctx->evt, ESP_NOW_MAX_DELAY) != pdTRUE)
    {
        ESP_LOGW(TAG, "Send receive queue fail");
    }
}

/* End of file -------------------------------------------------------- */
