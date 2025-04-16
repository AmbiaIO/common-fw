/*
 * File Name: bsp_crc.h
 *
 * Author: hello@hydratech-iot.com
 *
 * Description: CRC functions
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

/* Public defines ----------------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------------- */
/* Public Constants --------------------------------------------------------- */
/* Public variables --------------------------------------------------------- */
/* Public macros ------------------------------------------------------------ */
/* Public APIs -------------------------------------------------------------- */
uint16_t bsp_crc_16_calculate(const uint8_t *data, uint16_t len);

/* -------------------------------------------------------------------------- */
#ifdef __cplusplus
} // extern "C" {
#endif

/* End of file ---------------------------------------------------------------- */
