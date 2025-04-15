/*
 * File Name: bsp_nvs.h
 *
 * Author: hello@hydratech-iot.com
 *
 * Description: NVS flash memory handle
 *
 * Copyright 2024, HydraTech. All rights reserved.
 * You may use this file only in accordance with the license, terms, conditions,
 * disclaimers, and limitations in the end user license agreement accompanying
 * the software package with which this file was provided.
 */

/* Define to prevent recursive inclusion ------------------------------ */
#pragma once

/* Includes ----------------------------------------------------------- */
#include "base_include.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
typedef struct
{
    char key[4];     // This is the key-pair of data stored in NVS, we limit it in 4 ASCII number, start from "0000" -> "9999"
    uint32_t offset; // The offset of variable in @ref nvs_data_struct
    uint32_t size;   // The size of variable in bytes
}
nvs_key_data_t;

typedef struct
{
  uint32_t expected_nvs_version;
  const nvs_key_data_t *nvs_data_list;
  uint32_t sizeof_nvs_data_list;
  uint32_t store_addr;
}
nvs_param_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Init NVS storage and automatically load data to RAM if the data version is valid.
 *         In case of data version is different, all data will be set to default value both in NVS and RAM.
 *  
 * @param[in]     p_nvs_param  Pointer to the nvs_param_t structure.
 *
 * @return  base_status_t
 */
base_status_t bsp_nvs_init(nvs_param_t *p_nvs_param);

/**
 * @brief  Deinit the NVS storage by closing it. 
 *
 * @return  base_status_t
 */
base_status_t bsp_nvs_deinit(void);

/**
 * @brief  Immediately store all data from @ref g_nvs_setting_data structure into NVS storage.
 *
 * @param[in]     p_nvs_param  Pointer to the nvs_param_t structure.
 *
 * @return  base_status_t
 */
base_status_t bsp_nvs_store_all(nvs_param_t *p_nvs_param);

/**
 * @brief  Immediately load all data from NVS storage to  @ref g_nvs_setting_data structure
 *
 * @param[in]     p_nvs_param  Pointer to the nvs_param_t structure.
 *
 * @return  base_status_t
 */
base_status_t bsp_nvs_load_all(nvs_param_t *p_nvs_param);

/**
 * @brief  Automatically look up the ID entry based in input parameter: offset and size of variable
 *
 * @param[in]     p_nvs_param  Pointer to the nvs_param_t structure.
 * @param[in]     offset  offset 
 * @param[in]     size    size of data in bytes.
 *
 * @attention the look up table is @ref VAR_DATA_ID_LIST[]
 * 
 * @return  pointer to key name
 */
char *bsp_nvs_lookup_key(nvs_param_t *p_nvs_param, uint32_t offset, uint32_t size);

/**
 * @brief  Erase all data in NVS storage.
 *
 * @return  base_status_t
 */
base_status_t bsp_nvs_factory_reset(void);

/**
 * @brief  Store one specific data into NVS storage
 *
 * @param[in]     p_key_name  pointer to the key name to pair with data, limit under 16 character.
 * @param[in]     p_src       pointer to buffer contains data.
 * @param[in]     len         length of data in bytes.
 *
 * @return  base_status_t
 */
base_status_t bsp_nvs_store(char *p_key_name, void *p_src, uint32_t len);

/**
 * @brief  Load one specific data from NVS storage to destination buffer.
 *
 * @param[in]     p_key_name  pointer to the key name to pair with data, limit under 16 character.
 * @param[out]    p_des       pointer to buffer will contain data.
 * @param[in]     len         length of the buffer in bytes.
 *
 * @return  base_status_t
 */
base_status_t bsp_nvs_load(char *p_key_name, void *p_des, uint32_t len);

/* End of file -------------------------------------------------------- */
