/*
 * File Name: bsp_nvs.c
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

/* Includes ----------------------------------------------------------- */
#include "bsp_nvs.h"

/* Public variables --------------------------------------------------- */
/* Private defines ---------------------------------------------------- */
#define NVS_STORAGE_SPACENAME "Storage_1"
#define NVS_VERSION_KEY_NAME  "VERS"

/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Private Constants -------------------------------------------------------- */
static char *TAG = "bsp_nvs";

/* Private variables -------------------------------------------------- */
nvs_handle m_nvs_handle;

/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
base_status_t bsp_nvs_init(nvs_param_t *p_nvs_param)
{
    uint32_t nvs_ver = 0;
    esp_err_t err;

    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased, retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    if (err != ESP_OK)
        goto _LBL_END_;

    // Open NVS
    err = nvs_open(NVS_STORAGE_SPACENAME, NVS_READWRITE, &m_nvs_handle);
    if (err != ESP_OK)
        goto _LBL_END_;

    ESP_LOGI(TAG, "Data version in NVS storage: %lu ", nvs_ver);

    // Get NVS data version
    nvs_get_u32(m_nvs_handle, NVS_VERSION_KEY_NAME, &nvs_ver);

    ESP_LOGI(TAG, "Data version in NVS storage: %lu ", nvs_ver);
    ESP_LOGI(TAG, "Data version in NVS Macro  : %lu ", p_nvs_param->expected_nvs_version);

    // Check NVS data version
    if (nvs_ver != p_nvs_param->expected_nvs_version)
    {
        ESP_LOGI(TAG, "NVS data version is different, all current data in NVS will be erased");

        // Erase NVS storage
        bsp_nvs_factory_reset();

        // Store new data into NVS
        bsp_nvs_store_all(p_nvs_param);

        // Update new NVS data version
        err = nvs_set_u32(m_nvs_handle, NVS_VERSION_KEY_NAME, p_nvs_param->expected_nvs_version);
        if (err != ESP_OK)
            goto _LBL_END_;

        err = nvs_commit(m_nvs_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "NVS commit error: %s", esp_err_to_name(err));
            goto _LBL_END_;
        }
    }
    else
    {
        // Load all data from NVS to RAM structure data
        bsp_nvs_load_all(p_nvs_param);
    }

    return BS_OK;

_LBL_END_:
    ESP_LOGE(TAG, "NVS storage init failed");
    return BS_ERROR;
}

base_status_t bsp_nvs_deinit(void)
{
    nvs_close(m_nvs_handle);

    return BS_OK;
}

base_status_t bsp_nvs_store_all(nvs_param_t *p_nvs_param)
{
    esp_err_t err;
    void *p_data;
    size_t var_len;

    // Automatically looking into the nvs data list in order to get data information and store to NVS
    for (uint_fast16_t i = 0; i < p_nvs_param->sizeof_nvs_data_list; i++)
    {
        p_data = (void *)(p_nvs_param->store_addr + p_nvs_param->nvs_data_list[i].offset);
        var_len = (size_t)p_nvs_param->nvs_data_list[i].size;

        err = nvs_set_blob(m_nvs_handle, p_nvs_param->nvs_data_list[i].key, p_data, var_len);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "NVS set blob error: %s", esp_err_to_name(err));
            goto _LBL_END_;
        }

        // Commit written value
        err = nvs_commit(m_nvs_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "NVS commit error: %s", esp_err_to_name(err));
            goto _LBL_END_;
        }
    }

    return BS_OK;

_LBL_END_:
    ESP_LOGE(TAG, "NVS store all data error");
    return BS_ERROR;
}

base_status_t bsp_nvs_load_all(nvs_param_t *p_nvs_param)
{
    esp_err_t err;
    void *p_data;
    size_t var_len;

    // Load variable data from ID List Table
    for (uint_fast16_t i = 0; i < p_nvs_param->sizeof_nvs_data_list; i++)
    {
        p_data = (void *)(p_nvs_param->store_addr + p_nvs_param->nvs_data_list[i].offset);
        var_len = (size_t)p_nvs_param->nvs_data_list[i].size;

        err = nvs_get_blob(m_nvs_handle, p_nvs_param->nvs_data_list[i].key, p_data, &var_len);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "NVS get blob error: %s", esp_err_to_name(err));
            ESP_LOGE(TAG, "NVS load all data error");
            return BS_ERROR;
        }
    }

    return BS_OK;
}

char *bsp_nvs_lookup_key(nvs_param_t *p_nvs_param, uint32_t offset, uint32_t size)
{
    for (uint_fast16_t i = 0; i < p_nvs_param->sizeof_nvs_data_list; i++)
    {
        if ((p_nvs_param->nvs_data_list[i].offset == offset) && (p_nvs_param->nvs_data_list[i].size == size))
        {
            return (char *)p_nvs_param->nvs_data_list[i].key;
        }
    }

    // In case there are no key in table, return NULL. Please refer @nvs_data_list
    return NULL;
}


base_status_t bsp_nvs_store(char *p_key_name, void *p_src, uint32_t len)
{
    assert(p_key_name != NULL);
    assert(p_src != NULL);
    esp_err_t err;

    err = nvs_set_blob(m_nvs_handle, p_key_name, p_src, len);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS set blob error: %s", esp_err_to_name(err));
        goto _LBL_END_;
    }

    err = nvs_commit(m_nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS commit error: %s", esp_err_to_name(err));
        goto _LBL_END_;
    }

    return BS_OK;

_LBL_END_:
    ESP_LOGE(TAG, "NVS store data error");
    return BS_ERROR;
}

base_status_t bsp_nvs_load(char *p_key_name, void *p_des, uint32_t len)
{
    assert(p_key_name != NULL);
    assert(p_des != NULL);
    esp_err_t err;

    err = nvs_get_blob(m_nvs_handle, p_key_name, p_des, (size_t *)&len);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS get blob error: %s", esp_err_to_name(err));
        ESP_LOGE(TAG, "NVS load data error");
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t bsp_nvs_factory_reset(void)
{
    esp_err_t err;

    err = nvs_erase_all(m_nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS erase all error: %s", esp_err_to_name(err));
        goto _LBL_END_;
    }

    err = nvs_commit(m_nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS commit error: %s", esp_err_to_name(err));
        goto _LBL_END_;
    }

    return BS_OK;

_LBL_END_:
    ESP_LOGE(TAG, "NVS factory reset failed");
    return BS_ERROR;
}

/* End of file -------------------------------------------------------- */
