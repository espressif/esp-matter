/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for non-volatile storage of settings.
 *
 */

#include <openthread/platform/settings.h>

#include <platform_nvram.h>
#include <stdio.h>
#include <string.h>
#include <wiced_hal_nvram.h>
#include <wiced_platform_memory.h>
#include <wiced_result.h>

#if !OPENTHREAD_SETTINGS_RAM

#ifndef SETTINGS_DEBUG
#define SETTINGS_DEBUG 0
#endif // SETTINGS_DEBUG

#if (SETTINGS_DEBUG != 0)
#define SETTINGS_TRACE(format, ...) printf(format, ##__VA_ARGS__)
#else
#define SETTINGS_TRACE(...)
#endif

#define SETTINGS_NVRAM_ID_INFO PLATFORM_NVRAM_ID_ENTRY_INFO_UINT16
#define SETTINGS_NVRAM_ID_ENTRIES_START PLATFORM_NVRAM_ID_ENTRY_DATA_UINT16
#define SETTINGS_ENTRY_COUNT 20

typedef struct settings_entry
{
    bool        used; // true if the entry is used
    otInstance *aInstance;
    uint16_t    aKey;
    uint16_t    data_len; // written data length in the NVRAM for this entry
} settings_entry_t;

/*
 * Data block definition for each entry (aInstance + aKey)
 */
typedef struct settings_entry_data_block
{
    uint16_t data_len;
    uint8_t  data[1];
} settings_entry_data_block_t;

typedef struct settings_cb
{
    bool             initialized;
    settings_entry_t entries[SETTINGS_ENTRY_COUNT];
} settings_cb_t;

static settings_cb_t settings_cb = {0};

#if (SETTINGS_DEBUG)
static void settings_utils_key_value_display(uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    uint16_t i;

    printf("aKey: 0x%04X\n", aKey);
    printf("aValue: ");
    for (i = 0; i < aValueLength; i++)
    {
        printf("0x%02X, ", aValue[i]);
    }
    printf("\n");
}
#else  // SETTINGS_DEBUG
static void settings_utils_key_value_display(uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    OT_UNUSED_VARIABLE(aKey);
    OT_UNUSED_VARIABLE(aValue);
    OT_UNUSED_VARIABLE(aValueLength);
}
#endif // SETTINGS_DEBUG

/*
 * Load Entry Table from nvram.
 */
static bool settings_entry_load(void)
{
    uint16_t       nb_bytes;
    wiced_result_t status;

    nb_bytes = wiced_hal_read_nvram(SETTINGS_NVRAM_ID_INFO, sizeof(settings_entry_t) * SETTINGS_ENTRY_COUNT,
                                    (uint8_t *)&settings_cb.entries[0], &status);

    if ((nb_bytes == (sizeof(settings_entry_t) * SETTINGS_ENTRY_COUNT)) && (status == WICED_SUCCESS))
    {
        return true;
    }

    return false;
}

/*
 * Save Entry Table to NVRAM
 */
static void settings_entry_save(void)
{
    wiced_result_t status;

    wiced_hal_write_nvram(SETTINGS_NVRAM_ID_INFO, sizeof(settings_entry_t) * SETTINGS_ENTRY_COUNT,
                          (uint8_t *)&settings_cb.entries[0], &status);
}

/*
 * Find the entry for specific aInstance and aKey.
 *
 * @return: pointer to target entry or NULL for invalid
 */
static settings_entry_t *settings_entry_find(otInstance *aInstance, uint16_t aKey)
{
    uint8_t i;

    for (i = 0; i < SETTINGS_ENTRY_COUNT; i++)
    {
        if ((settings_cb.entries[i].used) && (settings_cb.entries[i].aInstance == aInstance) &&
            (settings_cb.entries[i].aKey == aKey))
        {
            return &settings_cb.entries[i];
        }
    }

    return NULL;
}

/*
 * Count the VS Id for specific entry.
 *
 *
 * @return  target VS Id or 0 for invalid.
 */
static uint16_t settings_entry_vs_id_count(settings_entry_t *p_target_entry)
{
    uint8_t shift;

    /* Check parameter. */
    if (!p_target_entry)
    {
        return 0;
    }

    shift = (uint8_t)(p_target_entry - &settings_cb.entries[0]);

    return (SETTINGS_NVRAM_ID_ENTRIES_START + shift);
}

/*
 * Allocate a free entry.
 *
 * @return  pointer to the allocated entry or NULL for invalid
 */
static settings_entry_t *settings_entry_allocate(void)
{
    uint8_t i;

    for (i = 0; i < SETTINGS_ENTRY_COUNT; i++)
    {
        if (settings_cb.entries[i].used == false)
        {
            settings_cb.entries[i].used = true;

            return &settings_cb.entries[i];
        }
    }

    return NULL;
}

/*
 * Release the entry.
 */
static void settings_entry_release(settings_entry_t *p_target_entry)
{
    if (!p_target_entry)
    {
        return;
    }

    memset((void *)p_target_entry, 0, sizeof(settings_entry_t));
}

void otPlatSettingsInit(otInstance *aInstance, const uint16_t *aSensitiveKeys, uint16_t aSensitiveKeysLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aSensitiveKeys);
    OT_UNUSED_VARIABLE(aSensitiveKeysLength);

    SETTINGS_TRACE("%s\n", __FUNCTION__);

    /* Check module initialization state.*/
    if (!settings_cb.initialized)
    {
        /* Load mapping table from NVRAM. */
        settings_cb.initialized = settings_entry_load();
    }
}

void otPlatSettingsDeinit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    SETTINGS_TRACE("%s\n", __FUNCTION__);

    memset((void *)&settings_cb.entries[0], 0, sizeof(settings_entry_t) * SETTINGS_ENTRY_COUNT);

    settings_cb.initialized = false;
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    settings_entry_t *           p_target_entry = NULL;
    uint16_t                     nb_bytes;
    wiced_result_t               status;
    settings_entry_data_block_t *p_data_block = NULL;
    uint32_t                     data_block_size;
    settings_entry_data_block_t *p_data_block_new = NULL;
    uint8_t *                    p_index          = NULL;

    SETTINGS_TRACE("%s\n", __FUNCTION__);
    settings_utils_key_value_display(aKey, aValue, aValueLength);

    /* Check parameter. */
    if (aValueLength > 0)
    {
        if (aValue == NULL)
        {
            return OT_ERROR_INVALID_ARGS;
        }
    }

    /* Check if this entry already exists. */
    p_target_entry = settings_entry_find(aInstance, aKey);

    if (p_target_entry)
    { // entry exists
        /* Allocate memory. */
        data_block_size = aValueLength > 0 ? sizeof(settings_entry_data_block_t) - sizeof(uint8_t) + aValueLength
                                           : sizeof(settings_entry_data_block_t);

        data_block_size += p_target_entry->data_len;

        p_data_block = (settings_entry_data_block_t *)wiced_platform_memory_allocate(data_block_size);

        if (p_data_block == NULL)
        {
            return OT_ERROR_NO_BUFS;
        }

        /* Prepare data to be written to the NVRAM. */
        // Read original data from NVRAM.
        nb_bytes = wiced_hal_read_nvram(settings_entry_vs_id_count(p_target_entry), p_target_entry->data_len,
                                        (uint8_t *)p_data_block, &status);

        if ((nb_bytes != p_target_entry->data_len) || (status != WICED_SUCCESS))
        { // fail to read original data
            /* Free memory. */
            wiced_platform_memory_free((void *)p_data_block);

            /* Release this entry. */
            settings_entry_release(p_target_entry);

            /* Call this utility again to add this data. */
            return otPlatSettingsAdd(aInstance, aKey, aValue, aValueLength);
        }

        // Append data to the last data block.
        p_index = (uint8_t *)p_data_block;
        p_index += p_target_entry->data_len;

        p_data_block_new           = (settings_entry_data_block_t *)p_index;
        p_data_block_new->data_len = aValueLength;
        memcpy((void *)&p_data_block_new->data[0], (void *)aValue, aValueLength);

        /* Write data to NVRAM. */
        nb_bytes = wiced_hal_write_nvram(settings_entry_vs_id_count(p_target_entry), data_block_size,
                                         (uint8_t *)p_data_block, &status);

        if ((nb_bytes != data_block_size) || (status != WICED_SUCCESS))
        { // fail to write data to NVRAM
            /* Free memory. */
            wiced_platform_memory_free((void *)p_data_block);

            return OT_ERROR_FAILED;
        }

        /* Update information. */
        p_target_entry->data_len = data_block_size;

        /* Free memory. */
        wiced_platform_memory_free((void *)p_data_block);
    }
    else
    { // entry doesn't exist
        /* Allocate an entry. */
        p_target_entry = settings_entry_allocate();

        if (p_target_entry == NULL)
        { // no sufficient memory
            return OT_ERROR_NO_BUFS;
        }

        /* Allocate memory. */
        data_block_size = aValueLength > 0 ? sizeof(settings_entry_data_block_t) - sizeof(uint8_t) + aValueLength
                                           : sizeof(settings_entry_data_block_t);

        p_data_block = (settings_entry_data_block_t *)wiced_platform_memory_allocate(data_block_size);

        if (p_data_block == NULL)
        {
            settings_entry_release(p_target_entry);

            return OT_ERROR_NO_BUFS;
        }

        /* Prepare data to be written to the NVRAM. */
        p_data_block->data_len = aValueLength;
        memcpy((void *)&p_data_block->data[0], (void *)aValue, aValueLength);

        /* Write data to NVRAM. */
        nb_bytes = wiced_hal_write_nvram(settings_entry_vs_id_count(p_target_entry), data_block_size,
                                         (uint8_t *)p_data_block, &status);

        if ((nb_bytes != data_block_size) || (status != WICED_SUCCESS))
        {
            /* Free memory. */
            wiced_platform_memory_free((void *)p_data_block);

            /* Release this entry. */
            settings_entry_release(p_target_entry);

            return OT_ERROR_FAILED;
        }

        /* Update information. */
        p_target_entry->aInstance = aInstance;
        p_target_entry->aKey      = aKey;
        p_target_entry->data_len  = data_block_size;

        /* Free memory. */
        wiced_platform_memory_free((void *)p_data_block);
    }

    /* Update entries information. */
    settings_entry_save();

    return OT_ERROR_NONE;
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    settings_entry_t *           p_target_entry = NULL;
    settings_entry_data_block_t *p_data_block   = NULL;
    uint16_t                     nb_bytes;
    wiced_result_t               status;
    int32_t                      residual_data_len;
    uint16_t                     block_index   = 0;
    uint8_t *                    p_index       = NULL;
    settings_entry_data_block_t *p_block_index = NULL;
    uint16_t                     block_len;
    uint8_t *                    p_tmp              = NULL;
    uint8_t *                    p_index_next_block = NULL;

    SETTINGS_TRACE("%s (0x%04X)\n", __FUNCTION__, aKey);

    /* Check if the target entry exists. */
    p_target_entry = settings_entry_find(aInstance, aKey);

    if (p_target_entry == NULL)
    {
        return OT_ERROR_NOT_FOUND;
    }

    /* Check parameter. */
    if (aIndex < -1)
    {
        return OT_ERROR_INVALID_ARGS;
    }

    if (aIndex == -1)
    {
        /* Release this entry. */
        settings_entry_release(p_target_entry);

        /* Update entries information. */
        settings_entry_save();

        return OT_ERROR_NONE;
    }

    /* Allocate memory. */
    p_data_block = (settings_entry_data_block_t *)wiced_platform_memory_allocate(p_target_entry->data_len);

    if (p_data_block == NULL)
    {
        return OT_ERROR_NO_BUFS;
    }

    /* Read data from NVRAM. */
    nb_bytes = wiced_hal_read_nvram(settings_entry_vs_id_count(p_target_entry), p_target_entry->data_len,
                                    (uint8_t *)p_data_block, &status);

    if ((nb_bytes != p_target_entry->data_len) || (status != WICED_SUCCESS))
    { // fail to read original data
        /* Free memory. */
        wiced_platform_memory_free((void *)p_data_block);

        /* Release this entry. */
        settings_entry_release(p_target_entry);

        /* Update entries information. */
        settings_entry_save();

        return OT_ERROR_NOT_FOUND;
    }

    /* Check if the target data block exists. */
    residual_data_len = p_target_entry->data_len;

    p_index       = (uint8_t *)p_data_block;
    p_block_index = p_data_block;

    while (residual_data_len > 0)
    {
        /* Check data length. */
        block_len = p_block_index->data_len
                        ? sizeof(settings_entry_data_block_t) - sizeof(uint8_t) + p_block_index->data_len
                        : sizeof(settings_entry_data_block_t);

        if (residual_data_len < block_len)
        { /* The content for this entry is incorrect. */
            /* Free memory. */
            wiced_platform_memory_free((void *)p_data_block);

            /* Release this entry. */
            settings_entry_release(p_target_entry);

            /* Update entries information. */
            settings_entry_save();

            return OT_ERROR_NOT_FOUND;
        }

        if (block_index == (uint16_t)aIndex)
        {
            /* Remove this data block. */
            residual_data_len -= block_len;

            if (residual_data_len > 0)
            {
                p_tmp = (uint8_t *)wiced_platform_memory_allocate(residual_data_len);
                if (p_tmp == NULL)
                {
                    /* Free memory. */
                    wiced_platform_memory_free((void *)p_data_block);

                    return OT_ERROR_NO_BUFS;
                }

                p_index_next_block = p_index + block_len;

                memcpy((void *)p_tmp, (void *)p_index_next_block, residual_data_len);
                memcpy((void *)p_index, (void *)p_tmp, residual_data_len);

                wiced_platform_memory_free((void *)p_tmp);
            }

            /* Write data to NVRAM. */
            if ((p_target_entry->data_len - block_len) > 0)
            {
                nb_bytes =
                    wiced_hal_write_nvram(settings_entry_vs_id_count(p_target_entry),
                                          p_target_entry->data_len - block_len, (uint8_t *)p_data_block, &status);

                if ((nb_bytes != (p_target_entry->data_len - block_len)) || (status != WICED_SUCCESS))
                { // fail to write data to NVRAM
                    /* Free memory. */
                    wiced_platform_memory_free((void *)p_data_block);

                    return OT_ERROR_FAILED;
                }
            }

            /* Update information. */
            p_target_entry->data_len = p_target_entry->data_len - block_len;

            if (p_target_entry->data_len == 0)
            {
                settings_entry_release(p_target_entry);
            }

            /* Free memory. */
            wiced_platform_memory_free((void *)p_data_block);

            /* Update entries information. */
            settings_entry_save();

            return OT_ERROR_NONE;
        }

        /* Calculate the residual data length. */
        residual_data_len -= block_len;

        /* Update index. */
        p_index += block_len;
        p_block_index = (settings_entry_data_block_t *)p_index;
        block_index++;
    }

    /* Free memory. */
    wiced_platform_memory_free((void *)p_data_block);

    return OT_ERROR_NOT_FOUND;
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
    settings_entry_t *           p_target_entry;
    settings_entry_data_block_t *p_data;
    uint16_t                     nb_bytes;
    wiced_result_t               status;
    uint16_t                     residual_data_len;
    uint16_t                     block_index   = 0;
    uint8_t *                    p_index       = NULL;
    settings_entry_data_block_t *p_block_index = NULL;
    uint16_t                     block_len;

    SETTINGS_TRACE("%s (0x%04X, %d)\n", __FUNCTION__, aKey, aIndex);

    /* Check parameter. */
    if (aIndex < 0)
    {
        return OT_ERROR_INVALID_ARGS;
    }

    if ((aValueLength == NULL) && (aValue != NULL))
    {
        return OT_ERROR_INVALID_ARGS;
    }

    /* Check if target entry exist. */
    p_target_entry = settings_entry_find(aInstance, aKey);

    if (p_target_entry == NULL)
    {
        return OT_ERROR_NOT_FOUND;
    }

    /* Allocate memory. */
    p_data = (settings_entry_data_block_t *)wiced_platform_memory_allocate(p_target_entry->data_len);

    if (p_data == NULL)
    {
        return OT_ERROR_NO_BUFS;
    }

    /* Read data from NVRAM. */
    nb_bytes = wiced_hal_read_nvram(settings_entry_vs_id_count(p_target_entry), p_target_entry->data_len,
                                    (uint8_t *)p_data, &status);

    if ((nb_bytes != p_target_entry->data_len) || (status != WICED_SUCCESS))
    { // fail to read original data
        /* Free memory. */
        wiced_platform_memory_free((void *)p_data);

        /* Release this entry. */
        settings_entry_release(p_target_entry);

        /* Update entries information. */
        settings_entry_save();

        return OT_ERROR_NOT_FOUND;
    }

    /* Check if the target data block exists. */
    residual_data_len = p_target_entry->data_len;

    p_index       = (uint8_t *)p_data;
    p_block_index = p_data;

    while (residual_data_len > 0)
    {
        /* Check data length. */
        block_len = p_block_index->data_len
                        ? sizeof(settings_entry_data_block_t) - sizeof(uint8_t) + p_block_index->data_len
                        : sizeof(settings_entry_data_block_t);

        if (residual_data_len < block_len)
        { /* The content for this entry is incorrect. */
            /* Free memory. */
            wiced_platform_memory_free((void *)p_data);

            /* Release this entry. */
            settings_entry_release(p_target_entry);

            /* Update entries information. */
            settings_entry_save();

            return OT_ERROR_NOT_FOUND;
        }

        if (block_index == (uint16_t)aIndex)
        { // target block found
            if (aValueLength)
            {
                *aValueLength = p_block_index->data_len;
            }

            if (aValue)
            {
                memcpy((void *)aValue, (void *)&p_block_index->data[0], p_block_index->data_len);
            }

            wiced_platform_memory_free((void *)p_data);

            return OT_ERROR_NONE;
        }

        /* Calculate the residual data length. */
        residual_data_len -= block_len;

        /* Update index. */
        p_index += block_len;
        p_block_index = (settings_entry_data_block_t *)p_index;
        block_index++;
    }

    /* Free memory. */
    wiced_platform_memory_free((void *)p_data);

    return OT_ERROR_NOT_FOUND;
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    settings_entry_t *p_target_entry;

    SETTINGS_TRACE("%s\n", __FUNCTION__);
    settings_utils_key_value_display(aKey, aValue, aValueLength);

    /* Check parameter. */
    if (aValueLength > 0)
    {
        if (aValue == NULL)
        {
            return OT_ERROR_INVALID_ARGS;
        }
    }

    /* Check if this entry already exists. */
    p_target_entry = settings_entry_find(aInstance, aKey);

    if (p_target_entry)
    { // target entry already exists - Replace the existent content
        /* Release current entry. */
        settings_entry_release(p_target_entry);
    }

    /* Add a new entry. */
    return otPlatSettingsAdd(aInstance, aKey, aValue, aValueLength);
}

void otPlatSettingsWipe(otInstance *aInstance)
{
    uint8_t i;
    bool    update = false;

    SETTINGS_TRACE("%s\n", __FUNCTION__);

    for (i = 0; i < SETTINGS_ENTRY_COUNT; i++)
    {
        if ((settings_cb.entries[i].used) && (settings_cb.entries[i].aInstance == aInstance))
        {
            settings_entry_release(&settings_cb.entries[i]);

            update = true;
        }
    }

    if (update == true)
    {
        settings_entry_save();
    }
}

#endif // !OPENTHREAD_SETTINGS_RAM
