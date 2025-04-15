/*
 * File Name: bsp_timer.h
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

/* Define to prevent recursive inclusion ------------------------------------ */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "base_type.h"
#include "base_include.h"

/* Public defines ----------------------------------------------------------- */
#define ONE_SECOND_IN_MS (1000)
#define ONE_MINUTE_IN_MS (60 * ONE_SECOND_IN_MS)

#define bsp_tmr_get_tick_ms() (xTaskGetTickCount() * portTICK_PERIOD_MS)

/* Public enumerate/structure ----------------------------------------------- */
typedef uint32_t tick_t;    //!< Count of system tick

typedef void (*timer_timeout_handler_t)(TimerHandle_t timer);
typedef TimerHandle_t timer_id_t;

\
/**
 * @brief Simple timer
 */
typedef struct
{
  tick_t start;
  tick_t interval;
}
tmr_t;

/**
 * @brief Auto timer structure
 */
typedef struct
{
  timer_id_t handle;
  tmr_t timer;
  timer_timeout_handler_t callback;
}
auto_timer_t;

/* Public Constants --------------------------------------------------------- */
/* Public variables --------------------------------------------------------- */
/* Public macros ------------------------------------------------------------ */
/* Public APIs -------------------------------------------------------------- */
void bsp_tmr_start     (tmr_t *tm, tick_t interval);
void bsp_tmr_restart   (tmr_t *tm, tick_t interval);
void bsp_tmr_stop      (tmr_t *tm);
bool bsp_tmr_is_expired(tmr_t *tm);

void bsp_tmr_auto_init   (auto_timer_t *atm, timer_timeout_handler_t callback);
void bsp_tmr_auto_start  (auto_timer_t *atm, tick_t interval);
void bsp_tmr_auto_restart(auto_timer_t *atm);
void bsp_tmr_auto_stop   (auto_timer_t *atm);

/* -------------------------------------------------------------------------- */
#ifdef __cplusplus
} // extern "C" {
#endif

/* End of file ---------------------------------------------------------------- */
