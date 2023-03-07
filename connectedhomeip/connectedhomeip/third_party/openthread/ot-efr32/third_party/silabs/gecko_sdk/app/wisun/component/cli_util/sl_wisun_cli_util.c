/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#include "sl_wisun_cli_util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * @brief App util printable line
 * @details Static helper function
 * @param[in,out] line_buffer Line buffer
 * @param[in] data Data ptr
 * @param[in] data_length Data length
 * @param[in] line_length Line length
 * @return int Return with the calculated line length
 *****************************************************************************/
static int app_util_printable_line(char *const line_buffer,
                                   const uint8_t *const data,
                                   const uint16_t data_length,
                                   uint8_t line_length);

/**************************************************************************//**
 * @brief App util printable hex line
 * @details Static helper function
 * @param[in,out] line_buffer Line buffer
 * @param[in] data Data ptr
 * @param[in] data_length Data length
 * @param[in] line_length Line length
 * @return int Return with the calculated line length
 *****************************************************************************/
static int app_util_printable_hex_line(char *line_buffer,
                                       const uint8_t *data,
                                       const uint16_t data_length,
                                       uint8_t line_length);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/* App util get string */
sl_status_t app_util_get_string(char *const value_str,
                                uint32_t value,
                                const app_enum_t *const value_enum_list,
                                uint8_t is_value_signed,
                                uint8_t is_value_hex,
                                uint8_t value_length)
{
  const app_enum_t* value_enum;
  char value_format_str[10];
  char value_temp[10];

  // String is empty by default
  value_str[0] = '\0';

  // Attempt to find a matching enumeration
  value_enum = app_util_get_enum_by_integer(value_enum_list, value);

  if (value_length && is_value_hex) {
    // Fixed-length hex values are always zero-filled
    sprintf(value_format_str, "%%0%u", value_length);
  } else if (value_length) {
    // Fixed-length value
    sprintf(value_format_str, "%%%u", value_length);
  } else {
    sprintf(value_format_str, "%%");
  }

  if (is_value_hex) {
    // Hex value
    strcat(value_format_str, "lx");
    sprintf(value_temp, value_format_str, value);
  } else if (is_value_signed) {
    // Signed integer value
    strcat(value_format_str, "ld");
    sprintf(value_temp, value_format_str, (int32_t)value);
  } else {
    // Unsigned integer value
    strcat(value_format_str, "lu");
    sprintf(value_temp, value_format_str, value);
  }

  // String starts with an enumeration
  if (value_enum) {
    sprintf(value_str, "%s (", value_enum->value_str);
  }

  // Hex value is prefixed with "0x"
  if (is_value_hex) {
    strcat(value_str, "0x");
  }

  // Value itself
  strcat(value_str, value_temp);

  // Closing paranthesis in case an enumeration was used
  if (value_enum) {
    strcat(value_str, ")");
  }

  // Success
  return SL_STATUS_OK;
}

/* App util get integer */
sl_status_t app_util_get_integer(uint32_t *const value,
                                 const char *value_str,
                                 const app_enum_t *const value_enum_list,
                                 uint8_t is_value_signed)
{
  const app_enum_t* value_enum;
  int value_base = 10;
  char *value_end = NULL;

  // Attempt to find a matching enumeration
  value_enum = app_util_get_enum_by_string(value_enum_list, value_str);

  // Return a matching enumeration value directly
  if (value_enum) {
    *value = value_enum->value;
    return SL_STATUS_OK;
  }

  // Value string looks like a hex number
  if (strstr(value_str, "0x") == value_str) {
    value_str += 2;
    value_base = 16;
  }

  if (is_value_signed) {
    // Attempt to convert the value string to integer
    *value = strtol(value_str, &value_end, value_base);
  } else {
    // Attempt to convert the value string to integer
    *value = strtoul(value_str, &value_end, value_base);
  }

  // Integer conversion failed
  if (*value_end != '\0') {
    return SL_STATUS_INVALID_KEY;
  }

  // Success
  return SL_STATUS_OK;
}

/* App util get enum by string */
const app_enum_t* app_util_get_enum_by_string(const app_enum_t *value_enum_list,
                                              const char *const value)
{
  while (value_enum_list && value_enum_list->value_str) {
    if (!strcmp(value_enum_list->value_str, value)) {
      // Matching enumeration found
      return value_enum_list;
    }
    value_enum_list++;
  }

  return NULL;
}

/* App util get enum by integerApp util get enum by integer */
const app_enum_t* app_util_get_enum_by_integer(const app_enum_t *value_enum_list,
                                               uint32_t value)
{
  while (value_enum_list && value_enum_list->value_str) {
    if (value_enum_list->value == value) {
      // Matching enumeration found
      return value_enum_list;
    }
    value_enum_list++;
  }

  return NULL;
}

/* App util get MAC address */
sl_status_t app_util_get_mac_address(sl_wisun_mac_address_t *const value,
                                     const char *value_str)
{
  int hex_str_max_length = SL_WISUN_MAC_ADDRESS_SIZE * 2;
  int hex_str_length = 0;
  char hex_str[3] = { 0, 0, 0 };
  char *hex_str_end = NULL;

  while (*value_str != '\0') {
    if ((*value_str != ' ') && (*value_str != ':')) {
      if (hex_str_length >= hex_str_max_length) {
        // Cannot fit any more characters in to the address
        return SL_STATUS_INVALID_KEY;
      }

      if (hex_str_length % 2 == 0) {
        hex_str[0] = *value_str;
      } else {
        hex_str[1] = *value_str;

        // Attempt to convert the hex value
        value->address[hex_str_length / 2] = (uint8_t)strtol(hex_str, &hex_str_end, 16);
        if (*hex_str_end != '\0') {
          // The full string wasn't converted, the hex value is invalid
          return SL_STATUS_INVALID_KEY;
        }
      }

      hex_str_length++;
    }

    // Move to the next character
    value_str++;
  }

  if (hex_str_length != hex_str_max_length) {
    // Incomplete MAC address
    return SL_STATUS_INVALID_KEY;
  }

  return SL_STATUS_OK;
}

/* App util get MAC address string */
sl_status_t app_util_get_mac_address_string(char *value_str,
                                            const sl_wisun_mac_address_t *value)
{
  int i;

  for (i = 0; i < SL_WISUN_MAC_ADDRESS_SIZE; ++i) {
    sprintf(value_str, "%02x:", value->address[i]);
    value_str += 3;
  }

  // Remove the last colon
  *(value_str - 1) = '\0';

  return SL_STATUS_OK;
}

/* App util printable data init */
char *app_util_printable_data_init(app_printable_data_ctx_t *const ctx,
                                   const uint8_t *const data,
                                   const uint16_t data_length,
                                   uint8_t is_hex,
                                   uint8_t line_length)
{
  if (!line_length) {
    return NULL;
  }

  if (line_length > APP_UTIL_PRINTABLE_DATA_MAX_LENGTH) {
    return NULL;
  }

  // Initialize context
  ctx->data = data;
  ctx->data_left = data_length;
  ctx->is_hex = is_hex;
  ctx->line_length = line_length;

  // Get the first line
  return app_util_printable_data_next(ctx);
}

/* App util get next printable data */
char *app_util_printable_data_next(app_printable_data_ctx_t *const ctx)
{
  int ret;

  if (!ctx->data || !ctx->data_left) {
    // All done
    return NULL;
  }

  // Get the next line
  if (ctx->is_hex) {
    ret = app_util_printable_hex_line(ctx->line_buffer,
                                      ctx->data,
                                      ctx->data_left,
                                      ctx->line_length);
  } else {
    ret = app_util_printable_line(ctx->line_buffer,
                                  ctx->data,
                                  ctx->data_left,
                                  ctx->line_length);
  }

  // Prepare for the next line
  ctx->data += ret;
  ctx->data_left -= ret;

  return ctx->line_buffer;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/* App util printable line */
static int app_util_printable_line(char *const line_buffer,
                                   const uint8_t *const data,
                                   const uint16_t data_length,
                                   uint8_t line_length)
{
  if (data_length < line_length) {
    line_length = (uint8_t)data_length;
  }

  memcpy(line_buffer, data, line_length);
  line_buffer[line_length] = 0;

  return line_length;
}

/* App util printable hex line */
static int app_util_printable_hex_line(char *line_buffer,
                                       const uint8_t *data,
                                       const uint16_t data_length,
                                       uint8_t line_length)
{
  int data_left;

  // Hex value takes twice the space
  line_length /= 2;

  if (data_length < line_length) {
    line_length = (uint8_t)data_length;
  }

  data_left = line_length;
  while (data_left--) {
    sprintf(line_buffer, "%02x", *data++);
    line_buffer += 2;
  }

  return line_length;
}
