/*
 * File Name: protocol.h
 *
 * Author: hello@hydratech-iot.com
 *
 * Description: Protocol
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
#include <stdint.h>
#include <stdbool.h>

/* Public defines ----------------------------------------------------- */
#define PACKET_DATA_LEN_MAX                 (500)

#define PACKET_SOMA                         (0x2A)  // Start of addressed message
#define PACKET_EOM                          (0x2C)  // End of message
#define SIZE_OF_ADDITIONAL_UART_FRAME       (4) // SOM (1 byte) + CRC (2 bytes) + EOM (1 byte)
#define POSITION_OF_PROTOBUF_DATA           (1) // Position of the protobuf data in the UART frame

#define MAC_ADDR_LEN                        (6)

/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief Creates a framed packet (SOM + Protobuf Data + CRC + EOM) for UART transmission.
 *
 * @param protobuf_data Pointer to the protobuf data.
 * @param protobuf_len Length of the protobuf data.
 * @param output_buffer Buffer to store the framed packet. Should be large enough (protobuf_len + 3).
 * @return Total length of the framed packet.
 */
uint16_t protocol_create_uart_frame(uint8_t *protobuf_data, uint16_t protobuf_len, uint8_t *output_buffer);

/* -------------------------------------------------------------------- */
#ifdef __cplusplus
} // extern "C"
#endif

/* End of file -------------------------------------------------------- */