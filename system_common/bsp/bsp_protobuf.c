/*
 * File Name: bsp_protobuf.c
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

/* Includes ----------------------------------------------------------------- */
#include "bsp_protobuf.h"

/* Public defines ----------------------------------------------------- */
static const char *TAG = "bsp_protobuf";

/* Private defines ---------------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------------- */
/* Private Constants -------------------------------------------------------- */
/* Private variables -------------------------------------------------------- */
/* Private macros ----------------------------------------------------------- */
/* Private Constants -------------------------------------------------------- */
/* Private prototypes ------------------------------------------------------- */
/* Public APIs -------------------------------------------------------------- */
uint32_t bsp_protobuf_encode_packet(packet_t *packet, uint8_t *p_buf, uint32_t len)
{
    pb_ostream_t ostream = pb_ostream_from_buffer(p_buf, len);
    bool         status  = pb_encode(&ostream, packet_t_fields, packet);

    if (!status)
    {
        return 0;
    }

    return ostream.bytes_written;
}

bool bsp_protobuf_decode_packet(packet_t *packet, uint8_t *p_buf, uint32_t len)
{
    pb_istream_t istream = pb_istream_from_buffer(p_buf, len);
    bool         status  = pb_decode(&istream, packet_t_fields, packet);

    if (!status)
    {
        return false;
    }

    return true;
}

/* Private function --------------------------------------------------------- */
/* End of file -------------------------------------------------------------- */