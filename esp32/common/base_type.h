/*
 * File Name: base_type.h
 *
 * Author: Thuan Le (thuanle@hydratech-iot.com)
 *
 * Description:
 *
 * Copyright 2024, HydraTech. All rights reserved.
 * You may use this file only in accordance with the license, terms, conditions,
 * disclaimers, and limitations in the end user license agreement accompanying
 * the software package with which this file was provided.
 */

/* Define to prevent recursive inclusion ------------------------------ */
#pragma once

/* Includes ----------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Public defines ----------------------------------------------------- */
/**
 * @brief Macro to check the status of an expression.
 *
 * This macro evaluates the given expression and checks.  
 * If the status is not `BS_OK`, just return the error code. Otherwise, return BS_OK
 *
 * @param expr The expression to be evaluated.
 * @return The status of the expression.
 */
#define CHECK_STATUS(expr)                                                                         \
    do                                                                                             \
    {                                                                                              \
        base_status_t ret = (expr);                                                                \
        if (BS_OK != ret)                                                                          \
        {                                                                                          \
            return (ret);                                                                          \
        }                                                                                          \
    } while (0)

/** @brief In case of error, print message, file and line and return with error code.
 *  @param[in] rc  Error code (rc).
 */
#define CHECK_RC(rc)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (rc != 0)                                                                               \
        {                                                                                          \
            return BS_ERROR;                                                                       \
        }                                                                                          \
    } while (0)

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

/* Public enumerate/structure ----------------------------------------- */
typedef enum
{
    BS_OK      = 0x00, ///< No Error - Okay
    BS_ERROR   = 0x01, ///< Generic Error
    BS_BUSY    = 0x02, ///< Unable to perform the function at this time (busy)
    BS_TIMEOUT = 0x03  ///< Blocking function timed out
} base_status_t;

/* Public macros ------------------------------------------------------ */
#define CONFIG_ENABLE_ESP_NOW        (0)

/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
