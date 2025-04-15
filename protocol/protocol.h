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
// #define ADDRESS_CONTROLLER_ESP32            (0x01)
// #define ADDRESS_CONTROLLER_STM32            (0x02)
// #define ADDRESS_MOBILE_OR_PC_APP            (0x03)
// #define ADDRESS_START_PERIPHERAL_ESP32      (0x04)

#define PACKET_DATA_LEN_MAX                 (500)

#define PACKET_SOMA                         (0x2A)  // Start of addressed message
#define PACKET_SOMB                         (0x2B)  // Start of broadcast message
#define PACKET_EOM                          (0x2C)  // End of message
#define PACKET_ESC                          (0x1B)  // Escape byte

#define MAC_ADDR_LEN                        (6)
#define DISCOVERY_TIMER_BASE_MS             (1000)  // Base timer value for discovery

/* Public enumerate/structure ----------------------------------------- */
typedef enum
{
    // Boot/Application commands
    PACKET_CMD_GET_DIMMER_STATUS = 0x01,
    PACKET_CMD_REBOOT            = 0x02,
    PACKET_CMD_GET_LAST_ERROR    = 0x03,

    // Boot commands
    PACKET_CMD_DISCOVERY_SET         = 0x11,  // Broadcast discovery request
    PACKET_CMD_DISCOVERY_RESP        = 0x12,  // Discovery response
    PACKET_CMD_DISCOVERY_RESULT_RESP = 0x13,  // Discovery result response to Mobile/PC
    PACKET_CMD_ADDRESS_SET           = 0x14,  // Broadcast address assignment request
    PACKET_CMD_ADDRESS_GET           = 0x15,  // Get address of the device
    PACKET_CMD_ADDRESS_RESP          = 0x16,  // Address response

    PACKET_CMD_GET_SERIAL               = 0x17,
    PACKET_CMD_GET_DIMMER_HARDWARE_INFO = 0x18,
    PACKET_CMD_GET_BOOTLOADER_VERSION   = 0x19,
    PACKET_CMD_GET_APPLICATION_VERSION  = 0x1A,
    PACKET_CMD_GET_REBOOT_CAUSE         = 0x1B,
    PACKET_CMD_GET_NV_PARAMS            = 0x1C,
    PACKET_CMD_SET_NV_PARAMS            = 0x1D,
    PACKET_CMD_GET_CALIBRATION_DATA     = 0x1E,
    PACKET_CMD_SET_CALIBRATION_DATA     = 0x1F,
    PACKET_CMD_START_APPLICATION        = 0x20,
    PACKET_CMD_FIRMWARE_UPDATE_START    = 0x21,
    PACKET_CMD_FIRMWARE_UPDATE_DATA     = 0x22,
    PACKET_CMD_FIRMWARE_UPDATE_FINISH   = 0x23,

    // Application commands
    PACKET_CMD_SET_OUTPUT_CHANNEL_1     = 0x41,
    PACKET_CMD_SET_OUTPUT_CHANNEL_2     = 0x42,
    PACKET_CMD_SET_OUTPUT_CHANNEL_3     = 0x43,
    PACKET_CMD_SET_OUTPUT_CHANNEL_4     = 0x44,
    PACKET_CMD_SET_INTENSITY_12         = 0x45,
    PACKET_CMD_SET_CCT_12               = 0x46,
    PACKET_CMD_SET_INTENSITY_34         = 0x47,
    PACKET_CMD_SET_CCT_34               = 0x48,
    PACKET_CMD_DELETE_GROUP_ASSIGNMENTS = 0x49,
    PACKET_CMD_ADD_TO_GROUPS            = 0x4A,
    PACKET_CMD_SET_FADE_TIME            = 0x7E,
    PACKET_CMD_SYNC                     = 0x7F,
    PACKET_CMD_MAX
} packet_cmd_t;

/*
 * Header for all packets.
 */
typedef struct __attribute__((packed))
{
    uint8_t som;       // Start of message
    uint8_t src_addr;  // Source address
    uint8_t dst_addr;  // Destination address
    uint8_t cmd;       // Command
    uint8_t len;       // Data length
} packet_header_t;

/*
 * Footer for all packets.
 */
typedef struct __attribute__((packed))
{
    uint8_t crc; // CRC-8
    uint8_t eom; // End of message
} packet_footer_t;

typedef struct __attribute__((packed))
{
    packet_header_t header;
    packet_footer_t footer;
} packet_application_version_get_t;

typedef struct __attribute__((packed))
{
    packet_header_t header;
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t version_patch;
    packet_footer_t footer;
} packet_application_version_resp_t;

typedef struct __attribute__((packed))
{
    packet_header_t header;
    packet_footer_t footer;
} packet_discovery_start_t;

typedef struct __attribute__((packed))
{
    packet_header_t header;
    uint8_t mac_addr[MAC_ADDR_LEN];
    packet_footer_t footer;
} packet_discovery_resp_t;

typedef struct __attribute__((packed))
{
    packet_header_t header;
    uint8_t mac_addr[MAC_ADDR_LEN];
    uint8_t new_address;
    packet_footer_t footer;
} packet_set_address_t;

typedef struct __attribute__((packed))
{
    packet_header_t header;
    uint8_t address;
    packet_footer_t footer;
} packet_address_resp_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/* -------------------------------------------------------------------- */
#ifdef __cplusplus
} // extern "C"
#endif

/* End of file -------------------------------------------------------- */