/******************************************************************************/
/**
 * @file
 * @brief This file implements the binary backchannel debug functionality
 * that allows printing to the PTI in Simplicity Studio
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#if defined(SL_CATALOG_OT_DEBUG_CHANNEL_PRESENT)

#include "byte_util.h"
#include "debug_channel.h"
#include <sl_iostream.h>
#include <sl_iostream_debug.h>
#include <sl_iostream_swo_itm_8.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

//------------------------------------------------------------------------------
// Forward declarations.

static void        sl_debug_internal_binary_printf(sl_iostream_swo_itm_8_msg_type_t debugType,
                                                   const char *                     formatString,
                                                   va_list                          args);
static void        startMessage(sl_iostream_swo_itm_8_msg_type_t debugType, uint8_t *buffer, size_t bufferSize);
static sl_status_t putBytes(uint8_t *bytes, size_t count);
static sl_status_t endMessage(void);

//------------------------------------------------------------------------------
// Static functions

/**
 * @brief Builds and prints a message on the debug channel using a SL-style format string
 *
 * @param[in]   debugType       the debug message type
 * @param[in]   formatString    SL-style format string
 * @param[in]   args            arguments for \ref formatString
 */
static void sl_debug_internal_binary_printf(sl_iostream_swo_itm_8_msg_type_t debugType,
                                            const char *                     formatString,
                                            va_list                          args)
{
    // Output longer than 128 bytes (with debug framing added) is split into
    // multiple messages.
    uint8_t buffer[128];
    uint8_t pointerLength = 0;

    startMessage(debugType, buffer, sizeof(buffer));
    if (formatString != NULL)
    {
        for (; *formatString; formatString++)
        {
            if (*formatString >= '0' && *formatString <= '9')
            {
                pointerLength = 10 * pointerLength + (*formatString - '0');
                continue;
            }
            switch (*formatString)
            {
            case 'B':
            case 'W':
            case 'D':
            {
                uint8_t data[4];
                uint8_t count;
                if (*formatString == 'B')
                {
                    slStoreLowHighInt32u(data, va_arg(args, unsigned int));
                    count = 1;
                }
                else if (*formatString == 'W')
                {
                    slStoreLowHighInt32u(data, va_arg(args, unsigned int));
                    count = 2;
                }
                else
                {
                    slStoreLowHighInt32u(data, va_arg(args, uint32_t));
                    count = 4;
                }
                putBytes(data, count);
            }
            break;
            case 'l':
                pointerLength = BYTE_0(va_arg(args, unsigned int));
                break;
            case 'p':
            case 'f':
            case 'F':
            {
                // The AVR required special handling for const char * but all current
                // platforms can treat const char * as (uint8_t *).
                uint8_t *data = va_arg(args, uint8_t *);
                if (*formatString == 'F')
                {
                    pointerLength = 0;
                    while (pointerLength < 0xFF && data[pointerLength] != '\0')
                    {
                        pointerLength++;
                    }
                }
                if (putBytes(data, pointerLength) != SL_STATUS_OK)
                {
                    goto fail;
                }
                pointerLength = 0;
            }
            break;
            case 'b':
                // TODO: UNSUPPORTED until I find a replacement for the legacy buffer-management code
                {
                    unsigned int data = va_arg(args, unsigned int);
                    (void)(data);
                }
                break;
            default:
            {
                uint8_t data = *formatString;
                putBytes(&data, 1);
            }
            break;
            }
        }
    }
fail:
    endMessage();
}

static uint8_t *txBuffer;
static size_t   txBufferSize;
static size_t   txBufferLength;

static void startMessage(sl_iostream_swo_itm_8_msg_type_t debugType, uint8_t *buffer, size_t bufferSize)
{
    sl_iostream_set_debug_type(debugType);
    txBuffer       = buffer;
    txBufferSize   = bufferSize;
    txBufferLength = 0;
}

static sl_status_t putBytes(uint8_t *bytes, size_t count)
{
    sl_status_t status    = SL_STATUS_OK;
    size_t      remaining = count;
    while (remaining > 0)
    {
        // Copy largest possible chunk of data that will fit into buffer
        size_t chunk = txBufferSize - txBufferLength;
        if (remaining < chunk)
        {
            chunk = remaining;
        }

        memmove(txBuffer + txBufferLength, bytes, chunk);
        txBufferLength += chunk;
        remaining -= chunk;
        bytes += chunk;

        // Flush txBuffer if there are still bytes that need to be added to the message
        if (remaining > 0)
        {
            status = endMessage();
        }
    }
    return status;
}

static sl_status_t endMessage(void)
{
    sl_status_t status = SL_STATUS_OK;
    status             = sl_iostream_write(sl_iostream_debug_handle, txBuffer, txBufferLength);
    if (status != SL_STATUS_OK)
    {
        goto exit;
    }

    txBufferLength = 0;

exit:
    return status;
}


//------------------------------------------------------------------------------
// Public functions

void sl_debug_binary_format(sl_iostream_swo_itm_8_msg_type_t debugType, const char *formatString, ...)
{
    va_list args;
    va_start(args, formatString);
    sl_debug_internal_binary_printf(debugType, formatString, args);
    va_end(args);
}

void sl_debug_printf(const char *formatString, ...)
{
    va_list args;
    va_start(args, formatString);
    sl_iostream_printf(sl_iostream_debug_handle, formatString, args);
    va_end(args);
}

#endif // SL_CATALOG_OT_DEBUG_CHANNEL_PRESENT