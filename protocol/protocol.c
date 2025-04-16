/*
 * File Name: protocol.c
 *
 * Author: hello@hydratech-iot.com
 *
 * Description: Protocol functions
 *
 * Copyright 2024, HydraTech. All rights reserved.
 * You may use this file only in accordance with the license, terms, conditions,
 * disclaimers, and limitations in the end user license agreement accompanying
 * the software package with which this file was provided.
 */

/* Includes ----------------------------------------------------------------- */
#include "protocol.h"
#include "bsp_crc.h"

/* Public defines ----------------------------------------------------- */
static const char *TAG = "protocol";

/* Private defines ---------------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------------- */
/* Private Constants -------------------------------------------------------- */
/* Private variables -------------------------------------------------------- */
/* Private macros ----------------------------------------------------------- */
/* Private Constants -------------------------------------------------------- */
/* Private prototypes ------------------------------------------------------- */
/* Public APIs -------------------------------------------------------------- */

/**
 * @brief Creates a framed packet (SOM + Protobuf Data + CRC + EOM) for UART transmission.
 *
 * @param protobuf_data Pointer to the protobuf data.
 * @param protobuf_len Length of the protobuf data.
 * @param output_buffer Buffer to store the framed packet. Should be large enough (protobuf_len + 3).
 * @param output_buffer_len Length of the output buffer.
 * @return Total length of the framed packet.
 */
uint16_t protocol_create_uart_frame(uint8_t *protobuf_data, uint16_t protobuf_len, uint8_t *output_buffer)
{
    if (protobuf_len > PACKET_DATA_LEN_MAX) 
    {
        return 0; // Indicate error
    }

    uint16_t total_len = 0;

    // Add SOM
    output_buffer[total_len++] = PACKET_SOMA;

    // Add Protobuf Data
    memcpy(&output_buffer[total_len], protobuf_data, protobuf_len);
    total_len += protobuf_len;

    // Add CRC
    uint16_t crc = bsp_crc_16_calculate(output_buffer, total_len);
    output_buffer[total_len++] = (crc >> 8) & 0xFF;
    output_buffer[total_len++] = (crc >> 0) & 0xFF;

    // Add EOM
    output_buffer[total_len++] = PACKET_EOM;

    return total_len;
}

/* Private function --------------------------------------------------------- */
/* End of file -------------------------------------------------------------- */