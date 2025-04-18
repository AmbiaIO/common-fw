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
#define SIZE_OF_ADDITIONAL_UART_FRAME       (7)     // SOM (1 byte) + Gateway (1 byte) + Payload Length (2 bytes) + CRC (2 bytes) + EOM (1 byte)
#define POSITION_OF_PROTOBUF_DATA           (3)     // Position of the protobuf data in the UART frame

#define UART_TX_BUFFER_SIZE                 (PACKET_DATA_LEN_MAX + SIZE_OF_ADDITIONAL_UART_FRAME)
#define MAC_ADDR_LEN                        (6)

#define POSITION_OF_SOM_IN_UART_FRAME       (0)
#define POSITION_OF_GATEWAY_IN_UART_FRAME   (1)

/* Public enumerate/structure ----------------------------------------- */
typedef enum {
    GATEWAY_NONE,
    GATEWAY_CONTROLLER,
    GATEWAY_PERIPHERAL,
} gateway_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief Creates a framed packet (SOM + Protobuf Data + CRC + EOM) for UART transmission.
 *
 * @param gateway Gateway of the packet.
 * @param protobuf_data Pointer to the protobuf data.
 * @param protobuf_len Length of the protobuf data.
 * @param output_buffer Buffer to store the framed packet. Should be large enough (protobuf_len + 3).
 * @return Total length of the framed packet.
 */
uint16_t protocol_create_uart_frame(gateway_t gateway, uint8_t *protobuf_data, uint16_t protobuf_len, uint8_t *output_buffer);

/**
 * @brief Get the CRC from the UART frame.
 * 
 * @param uart_frame Pointer to the UART frame.
 * @param uart_frame_len Length of the UART frame.
 * @return CRC of the UART frame.
 */
uint16_t protocol_get_crc_from_uart_frame(uint8_t *uart_frame, uint16_t uart_frame_len);

/**
 * @brief Get the gateway from the UART frame.
 * 
 * @param uart_frame Pointer to the UART frame.
 * @param uart_frame_len Length of the UART frame.
 * @return Gateway of the UART frame.
 */
gateway_t protocol_get_gateway_from_uart_frame(uint8_t *uart_frame, uint16_t uart_frame_len);

/* -------------------------------------------------------------------- */
#ifdef __cplusplus
} // extern "C"
#endif

/* End of file -------------------------------------------------------- */