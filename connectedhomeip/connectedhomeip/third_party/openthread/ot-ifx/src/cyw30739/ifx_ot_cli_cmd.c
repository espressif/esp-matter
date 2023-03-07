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
 *   This file implements the Infineon specific OpenThread CLI commands.
 */

//=================================================================================================
//  Includes
//=================================================================================================
#include <stdio.h>

#include <common/code_utils.hpp>
#include <openthread-core-config.h>
#include <openthread/cli.h>
#include <openthread/instance.h>

#if PACKET_STATISTICS
#include "packet_statistics.h"
#endif // PACKET_STATISTICS

#if SERIAL_FLASH
#include <wiced_hal_sflash.h>
#include <wiced_platform.h>
#endif

//=================================================================================================
// Type Definitions and Enums
//=================================================================================================
#if SERIAL_FLASH

/* Buffer length. */
#define IFX_OT_CLI_CMD_SERIAL_FLASH_BUFFER_LEN 256

/* Serial flash API source. */
typedef enum
{
    IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_BSP      = 0,
    IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_FIRMWARE = 1,
} IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_t;

#endif // SERIAL_FLASH

//=================================================================================================
//  Structure
//=================================================================================================

typedef struct
{
#if SERIAL_FLASH
    struct
    {
        IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_t api_source;
    } serial_flash;
#endif // SERIAL_FLASH
} IFX_OT_CLI_CMD_CB_t;

//=================================================================================================
//  Global Variables
//=================================================================================================

//=================================================================================================
//  Declaration of Static Functions
//=================================================================================================
#if PACKET_STATISTICS
#if (OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1)
static void ifx_ot_cli_cmd_handler_packet_statistics(uint8_t aArgsLength, char *aArgs[]);
#else  // OPENTHREAD_CONFIG_THREAD_VERSION != OT_THREAD_VERSION_1_1
static void ifx_ot_cli_cmd_handler_packet_statistics(void *aContext, uint8_t aArgsLength, char *aArgs[]);
#endif // OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1
#endif // PACKET_STATISTICS

#if SERIAL_FLASH
#if (OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1)
static void ifx_ot_cli_cmd_handler_serial_flash(uint8_t aArgsLength, char *aArgs[]);
#else  // OPENTHREAD_CONFIG_THREAD_VERSION != OT_THREAD_VERSION_1_1
static void ifx_ot_cli_cmd_handler_serial_flash(void *aContext, uint8_t aArgsLength, char *aArgs[]);
#endif // OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1
#endif // SERIAL_FLASH

//=================================================================================================
//  Static Variables
//=================================================================================================
static const otCliCommand ifx_ot_cli_commands[] = {
#if PACKET_STATISTICS
    {"ifx_packet_statistics", ifx_ot_cli_cmd_handler_packet_statistics},
#endif // PACKET_STATISTICS

#if SERIAL_FLASH
    {"ifx_serial_flash", ifx_ot_cli_cmd_handler_serial_flash},
#endif // SERIAL_FLASH
};

#if SERIAL_FLASH
static IFX_OT_CLI_CMD_CB_t ifx_ot_cli_cmd_cb = {0};
#endif // SERIAL_FLASH

//=================================================================================================
//	Global Functions
//=================================================================================================

/**************************************************************************************************
 * Function:     ifx_ot_cli_cmd_install
 *
 * Abstract:     Install the Infineon specific OpenThread CLI commands to stack.
 *
 * Input/Output:
 *   otInstance *ot_instance (I) - the instance
 *
 * Return:       None
 *
 * Notices:
 **************************************************************************************************/
void ifx_ot_cli_cmd_install(otInstance *ot_instance)
{
#if (OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1)
    OT_UNUSED_VARIABLE(ot_instance);
    otCliSetUserCommands(ifx_ot_cli_commands, OT_ARRAY_LENGTH(ifx_ot_cli_commands));
#else
    otCliSetUserCommands(ifx_ot_cli_commands, OT_ARRAY_LENGTH(ifx_ot_cli_commands), ot_instance);
#endif
}

//=================================================================================================
//	Local (Static) Functions
//=================================================================================================
#if SERIAL_FLASH

static void ifx_ot_cli_cmd_handler_serial_flash_usage_display(void)
{
    otCliOutputFormat("Usage:");
    otCliOutputFormat("  ifx_serial_flash api API_SOURCE\n");
    otCliOutputFormat("  ifx_serial_flash init\n");
    otCliOutputFormat("  ifx_serial_flash read ADDR LENGTH, ex: ifx_serial_flash 0x00010000 256\n");
    otCliOutputFormat("  ifx_serial_flash write ADDR LENGTH, ex: ifx_serial_flash 0x00010000 256\n");
    otCliOutputFormat("  ifx_serial_flash erase START_SECTOR SECTOR_NUM, ex: ifx_serial_flash 0 1\n");
    otCliOutputFormat("Note: The maximum value of LENGTH is %u\n", IFX_OT_CLI_CMD_SERIAL_FLASH_BUFFER_LEN);
}

static void ifx_ot_cli_cmd_handler_serial_flash_init(void)
{
    switch (ifx_ot_cli_cmd_cb.serial_flash.api_source)
    {
    case IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_BSP:
        wiced_platform_serial_flash_init();
        otCliOutputFormat("Serial Flash size: %lu bytes\n", wiced_platform_serial_flash_size_get());
        break;
    case IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_FIRMWARE:
        wiced_hal_sflash_init();
        wiced_hal_sflash_clk_rate_set(24000000);
        otCliOutputFormat("Serial Flash size: %lu bytes\n", wiced_hal_sflash_get_size());
        break;
    default:
        break;
    }
}

static void ifx_ot_cli_cmd_handler_serial_flash_read(uint32_t target_addr, uint8_t *p_data, uint32_t data_len)
{
    switch (ifx_ot_cli_cmd_cb.serial_flash.api_source)
    {
    case IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_BSP:
        wiced_platform_serial_flash_read(target_addr, p_data, data_len);
        break;
    case IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_FIRMWARE:
        wiced_hal_sflash_read(target_addr, data_len, p_data);
        break;
    default:
        break;
    }
}

static void ifx_ot_cli_cmd_handler_serial_flash_write(uint32_t target_addr, uint8_t *p_data, uint32_t data_len)
{
    switch (ifx_ot_cli_cmd_cb.serial_flash.api_source)
    {
    case IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_BSP:
        wiced_platform_serial_flash_write(target_addr, p_data, data_len);
        break;
    case IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_FIRMWARE:
        wiced_hal_sflash_write(target_addr, data_len, p_data);
        break;
    default:
        break;
    }
}

static void ifx_ot_cli_cmd_handler_serial_flash_erase(uint32_t start_sector, uint32_t sector_num)
{
    switch (ifx_ot_cli_cmd_cb.serial_flash.api_source)
    {
    case IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_BSP:
        wiced_platform_serial_flash_erase(start_sector, sector_num);
        break;
    case IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_FIRMWARE:
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * Function:     ifx_ot_cli_cmd_handler_serial_flash
 *
 * Abstract:     Infineon specific OpenThread CLI command handler for serial flash control.
 *
 * Input/Output:
 *   void *aContext (I)      - instance
 *   uint8_t aArgsLength (I) - number of input arguments
 *   char *aArgs[]           - arguments
 *
 * Return:       None
 *
 * Notices:
 **************************************************************************************************/
#if (OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1)
static void ifx_ot_cli_cmd_handler_serial_flash(uint8_t aArgsLength, char *aArgs[])
#else
static void ifx_ot_cli_cmd_handler_serial_flash(void *aContext, uint8_t aArgsLength, char *aArgs[])
#endif
{
    uint8_t  data[IFX_OT_CLI_CMD_SERIAL_FLASH_BUFFER_LEN] = {0};
    uint32_t i;
    uint32_t target_addr;
    uint32_t data_len;

#if !(OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1)
    OT_UNUSED_VARIABLE(aContext);
#endif

    /* Check parameter. */
    if (!aArgsLength)
    {
        ifx_ot_cli_cmd_handler_serial_flash_usage_display();
        return;
    }

    if ((strncmp("api", aArgs[0], 3) == 0) && (strlen(aArgs[0]) == 3))
    {
        if (aArgsLength != 2)
        {
            ifx_ot_cli_cmd_handler_serial_flash_usage_display();
            return;
        }

        /* Set API source. */
        ifx_ot_cli_cmd_cb.serial_flash.api_source =
            (IFX_OT_CLI_CMD_SERIAL_FLASH_API_SOURCE_t)strtol(aArgs[1], NULL, 10);
    }
    else if ((strncmp("init", aArgs[0], 4) == 0) && (strlen(aArgs[0]) == 4))
    {
        ifx_ot_cli_cmd_handler_serial_flash_init();
    }
    else if ((strncmp("read", aArgs[0], 4) == 0) && (strlen(aArgs[0]) == 4))
    {
        if (aArgsLength != 3)
        {
            ifx_ot_cli_cmd_handler_serial_flash_usage_display();
            return;
        }

        /* Parse address. */
        target_addr = (uint32_t)strtol(aArgs[1], NULL, 0);

        /* Parse length. */
        data_len = (uint32_t)strtol(aArgs[2], NULL, 10);

        /* Check data length. */
        if (data_len > IFX_OT_CLI_CMD_SERIAL_FLASH_BUFFER_LEN)
        {
            ifx_ot_cli_cmd_handler_serial_flash_usage_display();
            return;
        }

        /* Read data from serial flash. */
        ifx_ot_cli_cmd_handler_serial_flash_read(target_addr, data, data_len);

        otCliOutputFormat("data in flash (0x%08X, data_len: %lu)\n", target_addr, data_len);
        for (i = 0; i < data_len; i++)
        {
            if (i % 10 == 0)
            {
                otCliOutputFormat("\n");
            }
            otCliOutputFormat("%u ", data[i]);
        }
        otCliOutputFormat("\n");
    }
    else if ((strncmp("write", aArgs[0], 5) == 0) && (strlen(aArgs[0]) == 5))
    {
        if (aArgsLength < 3)
        {
            ifx_ot_cli_cmd_handler_serial_flash_usage_display();
            return;
        }

        /* Parse address. */
        target_addr = (uint32_t)strtol(aArgs[1], NULL, 0);

        /* Parse length. */
        data_len = (uint32_t)strtol(aArgs[2], NULL, 10);

        /* Check data length. */
        if (data_len > IFX_OT_CLI_CMD_SERIAL_FLASH_BUFFER_LEN)
        {
            ifx_ot_cli_cmd_handler_serial_flash_usage_display();
            return;
        }

        /* Check argument length. */
        if (aArgsLength != (data_len + 3))
        {
            ifx_ot_cli_cmd_handler_serial_flash_usage_display();
            return;
        }

        /* Parse data. */
        for (i = 0; i < data_len; i++)
        {
            data[i] = (uint8_t)strtol(aArgs[3 + i], NULL, 0);
        }

        /* Write data to serial flash. */
        otCliOutputFormat("Write data to 0x%08X (%lu)\n", target_addr, data_len);
        ifx_ot_cli_cmd_handler_serial_flash_write(target_addr, data, data_len);
    }
    else if ((strncmp("erase", aArgs[0], 5) == 0) && (strlen(aArgs[0]) == 5))
    {
        if (aArgsLength != 3)
        {
            ifx_ot_cli_cmd_handler_serial_flash_usage_display();
            return;
        }

        ifx_ot_cli_cmd_handler_serial_flash_erase((uint32_t)strtol(aArgs[1], NULL, 10),
                                                  (uint32_t)strtol(aArgs[2], NULL, 10));
    }
    else
    {
        ifx_ot_cli_cmd_handler_serial_flash_usage_display();
        return;
    }
}
#endif // SERIAL_FLASH

#if PACKET_STATISTICS
/**************************************************************************************************
 * Function:     ifx_ot_cli_cmd_handler_packet_statistics
 *
 * Abstract:     Infineon specific OpenThread CLI command handler for ifx_packet_statistics.
 *
 * Input/Output:
 *   void *aContext (I)      - instance
 *   uint8_t aArgsLength (I) - number of input arguments
 *   char *aArgs[]           - arguments
 *
 * Return:       None
 *
 * Notices:
 **************************************************************************************************/
#if (OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1)
static void ifx_ot_cli_cmd_handler_packet_statistics(uint8_t aArgsLength, char *aArgs[])
#else
static void ifx_ot_cli_cmd_handler_packet_statistics(void *aContext, uint8_t aArgsLength, char *aArgs[])
#endif
{
    PACKET_STATISTICS_t statistics = {0};

#if !(OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1)
    OT_UNUSED_VARIABLE(aContext);
#endif
    OT_UNUSED_VARIABLE(aArgsLength);
    OT_UNUSED_VARIABLE(aArgs);

    /* Collect information. */
    otPlatRadioPacketStatisticsGet(&statistics);

    /* Send result to host device. */
    otCliOutputFormat("Tx. - total: %ld, success: %ld, no_ack: %ld, channel_access_failure: %ld\n", statistics.tx.num,
                      statistics.tx.status.success, statistics.tx.status.no_ack,
                      statistics.tx.status.channel_access_failure);
    otCliOutputFormat("Rx. - total: %ld\n", statistics.rx.num);
}
#endif // PACKET_STATISTICS

//=================================================================================================
//	End of File (ifx_ot_cli_cmd.c)
//=================================================================================================
