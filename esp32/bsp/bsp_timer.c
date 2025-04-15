/*
 * File Name: bsp_timer.c
 *
 * Author: Thuan Le (thuanle@hydratech-iot.com)
 *
 * Description: System Manager
 *
 * Copyright 2024, HydraTech. All rights reserved.
 * You may use this file only in accordance with the license, terms, conditions,
 * disclaimers, and limitations in the end user license agreement accompanying
 * the software package with which this file was provided.
 */

/* Includes ----------------------------------------------------------------- */
#include "bsp_timer.h"
#include "base_include.h"

/* Private defines ---------------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------------- */
/* Private Constants -------------------------------------------------------- */
/* Private variables -------------------------------------------------------- */
/* Private macros ----------------------------------------------------------- */
/* Private Constants -------------------------------------------------------- */

/* Private prototypes ------------------------------------------------------- */
static void tmr_start_ex(tmr_t *tm, tick_t start, tick_t interval);
static base_status_t tmr_auto_start(timer_id_t *handle, tick_t interval, timer_timeout_handler_t callback);

/* Public APIs -------------------------------------------------------------- */
/**
 * @brief Start the simple timer
 */
void bsp_tmr_start(tmr_t *tm, tick_t interval)
{
    tmr_start_ex(tm, bsp_tmr_get_tick_ms(), interval);
}

/**
 * @brief Stop the simple timer
 */
void bsp_tmr_stop(tmr_t *tm)
{
    tm->start = 0;
    tm->interval = 0;
}

/**
 * @brief Restart the simple timer
 */
void bsp_tmr_restart(tmr_t *tm, tick_t interval)
{
    bsp_tmr_stop(tm);
    bsp_tmr_start(tm, interval);
}

bool bsp_tmr_is_expired(tmr_t *tm)
{
    if (tm->interval && (bsp_tmr_get_tick_ms() - tm->start >= tm->interval))
    {
        bsp_tmr_stop(tm);
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Initialize auto timer
 */
void bsp_tmr_auto_init(auto_timer_t *atm, timer_timeout_handler_t callback)
{
    atm->timer.interval = 0;
    atm->timer.start = 0;
    atm->callback = callback;
}

/**
 * @brief Start auto timer
 */
void bsp_tmr_auto_start(auto_timer_t *atm, tick_t interval)
{
    atm->timer.interval = interval;
    bsp_tmr_auto_restart(atm);
}

/**
 * @brief Restart auto timer
 */
void bsp_tmr_auto_restart(auto_timer_t *atm)
{
    if (atm->timer.interval == 0)
        return;

    tmr_auto_start(&atm->handle, atm->timer.interval, atm->callback);
}

/**
 * @brief Stop auto timer. Only called in non-IRQ routine/timer callback
 */
void bsp_tmr_auto_stop(auto_timer_t *atm)
{
    xTimerStop(atm->handle, 0);
}

/* Private function --------------------------------------------------------- */
/**
 * @brief Start the simple timer at exact start time
 */
static void tmr_start_ex(tmr_t *tm, tick_t start, tick_t interval)
{
    tm->start = start;
    tm->interval = interval;
}

/**
 * @brief         Time start
 */
static base_status_t tmr_auto_start(timer_id_t *handle, tick_t interval, timer_timeout_handler_t callback)
{
    *handle = xTimerCreate("Auto Timer", pdMS_TO_TICKS(interval), pdFALSE, (void*)0, callback);
    xTimerStart(*handle, 0);

    return BS_OK;
}

/* End of file -------------------------------------------------------------- */
