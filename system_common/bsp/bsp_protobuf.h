/*
 * File Name: bsp_protobuf.h
 *
 * Author: hello@hydratech-iot.com
 *
 * Description: Protobuf functions
 *
 * Copyright 2024, HydraTech. All rights reserved.
 * You may use this file only in accordance with the license, terms, conditions,
 * disclaimers, and limitations in the end user license agreement accompanying
 * the software package with which this file was provided.
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "base_include.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "ambiaio.pb.h"

/* Public defines ----------------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------------- */
/* Public Constants --------------------------------------------------------- */
/* Public variables --------------------------------------------------------- */
/* Public macros ------------------------------------------------------------ */
/* Public APIs -------------------------------------------------------------- */
uint32_t bsp_protobuf_encode_packet(packet_t *packet, uint8_t *p_buf, uint32_t len);
bool     bsp_protobuf_decode_packet(packet_t *packet, uint8_t *p_buf, uint32_t len);

/* -------------------------------------------------------------------------- */
#ifdef __cplusplus
} // extern "C" {
#endif

/* End of file ---------------------------------------------------------------- */
