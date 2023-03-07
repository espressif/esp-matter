/***************************************************************************//**
 * @file
 * @brief processes commands incoming over the serial port.
 *
 * @deprecated command-interpreter and command-interpreter2
 * are deprecated and will be removed in a future release.
 * Use sl_zigbee_command_interpreter instead.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#if defined(UC_BUILD)
#include "sl_cli.h"
#include "sl_zigbee_command_interpreter.h"

#define EUI64_SIZE 8
typedef uint8_t EmberEUI64[EUI64_SIZE];

// We want to use this component without being dependent on platform headers/UP,
// this could be a valid usecase e.g. in case of GPD
#include "stdlib.h"
#include "string.h"

uint8_t *sl_zigbee_cli_get_argument_string_and_length(sl_cli_command_arg_t *arguments,
                                                      int8_t index,
                                                      uint8_t *length)
{
  uint8_t count = 0;
  uint8_t leading_quote = 0;
  uint8_t *start = (uint8_t *) sl_cli_get_argument_string(arguments, index);
  uint8_t *end = start;

  while (*end != '\0') {
    count++;
    end++;
  }
  if (length != NULL) {
    *length = (count == 0 ? 0 : count - leading_quote);
  }
  return start + leading_quote;
}

uint8_t sl_zigbee_copy_string_arg(sl_cli_command_arg_t *arguments,
                                  int8_t index,
                                  uint8_t *destination,
                                  uint8_t max_length,
                                  bool left_pad)
{
  uint8_t pad_length;
  uint8_t arg_length;
  uint8_t *contents = sl_zigbee_cli_get_argument_string_and_length(arguments, index, &arg_length);
  if (arg_length > max_length) {
    arg_length = max_length;
  }
  pad_length = left_pad ? max_length - arg_length : 0;
  (void) memset(destination, 0, pad_length);
  (void) memmove(destination + pad_length, contents, arg_length);
  return arg_length;
}

uint8_t sl_zigbee_copy_hex_arg(sl_cli_command_arg_t *arguments,
                               int8_t index,
                               uint8_t *destination,
                               uint8_t max_length,
                               bool left_pad)
{
  size_t pad_length;
  size_t arg_length;
  uint8_t *contents = sl_cli_get_argument_hex(arguments, index, &arg_length);
  if (arg_length > max_length) {
    arg_length = max_length;
  }
  pad_length = left_pad ? max_length - arg_length : 0;
  (void) memset(destination, 0, pad_length);
  (void) memmove(destination + pad_length, contents, arg_length);
  return arg_length;
}

uint8_t sl_zigbee_copy_eui64_arg(sl_cli_command_arg_t *arguments,
                                 uint8_t index,
                                 EmberEUI64 dest,
                                 bool big_endian)
{
  size_t len = 0;
  uint8_t *hex_array_ptr = sl_cli_get_argument_hex(arguments, index, &len);
  if (len >= EUI64_SIZE) {
    for (uint8_t i = 0; i < EUI64_SIZE; i++) {
      uint8_t x = big_endian ? (EUI64_SIZE - i - 1) : i;
      dest[x] = hex_array_ptr[i];
    }
  }
  return (uint16_t) len;
}
#endif
