/***************************************************************************//**
 * @file
 * @brief Processes commands coming from the serial port.
 * See @ref commands for documentation.
 *
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

#ifndef SILABS_UC_COMMAND_INTERPRETER_H
#define SILABS_UC_COMMAND_INTERPRETER_H

#if defined(UC_BUILD) && defined(SL_CATALOG_CLI_PRESENT)
#include "sl_cli.h"
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "sl_zigbee_debug_print.h"
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

// TODO: doxygen
uint8_t *sl_zigbee_cli_get_argument_string_and_length(sl_cli_command_arg_t *arguments,
                                                      int8_t index,
                                                      uint8_t *length);

// TODO: doxygen
uint8_t sl_zigbee_copy_string_arg(sl_cli_command_arg_t *arguments,
                                  int8_t index,
                                  uint8_t *destination,
                                  uint8_t max_length,
                                  bool left_pad);

// TODO: doxygen
uint8_t sl_zigbee_copy_hex_arg(sl_cli_command_arg_t *arguments,
                               int8_t index,
                               uint8_t *destination,
                               uint8_t max_length,
                               bool left_pad);

// TODO: doxygen
uint8_t sl_zigbee_copy_eui64_arg(sl_cli_command_arg_t *arguments,
                                 uint8_t index,
                                 uint8_t* dest,
                                 bool big_endian);

// TODO: doxygen

#define sl_zigbee_copy_key_arg(arg, index, key_data_ptr)    \
  (sl_zigbee_copy_hex_arg((arg),                            \
                          (index),                          \
                          emberKeyContents((key_data_ptr)), \
                          EMBER_ENCRYPTION_KEY_SIZE,        \
                          true))

#elif !defined(UC_BUILD)
#include PLATFORM_HEADER
#include "app/util/serial/command-interpreter2.h"
#define SL_CLI_COMMAND_ARG void
#define SL_CLI_COMMAND_PARAM
#endif // !UC_BUILD

#endif // SILABS_UC_COMMAND_INTERPRETER_H
