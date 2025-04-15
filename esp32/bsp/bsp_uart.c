/*
 * File Name: bsp_uart.c
 *
 * Author: hello@hydratech-iot.com
 *
 * Description:
 *
 * Copyright 2024, HydraTech. All rights reserved.
 * You may use this file only in accordance with the license, terms, conditions,
 * disclaimers, and limitations in the end user license agreement accompanying
 * the software package with which this file was provided.
 */

/* Includes ----------------------------------------------------------------- */
#include "bsp_uart.h"
#include "base_board_defs.h"
#include "driver/uart.h"
#include "driver/gpio.h"

/* Public defines ----------------------------------------------------- */
static const char *TAG = "bsp_uart";

/* Private defines ---------------------------------------------------------- */
static const int RX_BUF_SIZE = 1024;

/* Private enumerate/structure ---------------------------------------------- */
/* Private Constants -------------------------------------------------------- */
/* Private variables -------------------------------------------------------- */
/* Private macros ----------------------------------------------------------- */
/* Private Constants -------------------------------------------------------- */
/* Private prototypes ------------------------------------------------------- */
/* Public APIs -------------------------------------------------------------- */
void bsp_uart_init(int tx_io_num, int rx_io_num)
{
    esp_err_t err;

    const uart_config_t uart_config =
        {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
        };

    // Install driver
    err = uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    if (err != ESP_OK)
    {
        printf("Failed to install UART driver: %s\n", esp_err_to_name(err));
        return;
    }

    // Configure UART parameters
    err = uart_param_config(UART_NUM_1, &uart_config);
    if (err != ESP_OK)
    {
        printf("Failed to configure UART parameters: %s\n", esp_err_to_name(err));
        return;
    }

    // Set UART pins
    err = uart_set_pin(UART_NUM_1, tx_io_num, rx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK)
    {
        printf("Failed to set UART pins: %s\n", esp_err_to_name(err));
        return;
    }
}

void bsp_uart_send_data(uint8_t *data, uint16_t len)
{
    ESP_LOGW(TAG, "bsp_uart_send_data len: %d", len);
    uart_write_bytes(UART_NUM_1, data, len);
}

void bsp_uart_read_data(uint8_t *data, uint16_t *len, uint32_t ticks_to_wait)
{
    *len = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, ticks_to_wait);
}

/* Private function --------------------------------------------------------- */
/* End of file -------------------------------------------------------------- */