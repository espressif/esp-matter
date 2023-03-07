/***************************************************************************//**
 * @file
 * @brief This file implements the address filtering commands in RAILtest apps.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdio.h>
#include <string.h>

#include "response_print.h"

#include "rail.h"
#include "app_common.h"

// Define for the maximum number of filter fields
#define ADDRFILT_FIELD_COUNT (2)
#define ADDRFILT_ENTRY_COUNT (4)
#define ADDRFILT_ENTRY_SIZE (8)

/// This holds the addresses used by this application. We get 4 addresses per
/// field of up to 8 bytes in length.
static uint8_t addresses[ADDRFILT_FIELD_COUNT * ADDRFILT_ENTRY_COUNT][ADDRFILT_ENTRY_SIZE];

/// Track which addresses we have enabled
static bool addressEnabled[ADDRFILT_FIELD_COUNT][ADDRFILT_ENTRY_COUNT];

/// The current address field configuration
static RAIL_AddrConfig_t config = { { 0, 0 }, { 0, 0 }, 0 };

/// Buffer to help print address values to the screen
static char addressPrintBuffer[(ADDRFILT_ENTRY_SIZE * 5) + 8];

#if RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK
/// This holds the address masks used by this application. We get 1 address mask per
/// field of up to 8 bytes in length.
static uint8_t addressMasks[ADDRFILT_FIELD_COUNT][ADDRFILT_ENTRY_SIZE] = { { 0xFF }, { 0xFF } };

/// Buffer to help print address mask values to the screen
static char addressMaskPrintBuffer[(ADDRFILT_ENTRY_SIZE * 5) + 8];
#endif

void setAddressFilterConfig(sl_cli_command_arg_t *args)
{
  int i = 0, count = ADDRFILT_FIELD_COUNT * 2;

  // Reset the config struct to zero as the default for everything
  memset(addresses, 0, sizeof(addresses));
  memset(addressEnabled, 0, sizeof(addressEnabled));
#if RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK
  memset(addressMasks, 0xFF, sizeof(addressMasks));
#endif

  // Set the match table in the config struct
  config.matchTable = sl_cli_get_argument_uint32(args, 0);

  // Set count to the maximum allowed size or the maximum provided parameters
  if ((sl_cli_get_argument_count(args) - 1) < count) {
    count = sl_cli_get_argument_count(args) - 1;
  }

  // Set as many offset and sizes as we can
  for (i = 0; i < count; i++) {
    if ((i % 2) == 0) {
      config.offsets[i / 2] = sl_cli_get_argument_uint8(args, i + 1);
    } else {
      config.sizes[i / 2] = sl_cli_get_argument_uint8(args, i + 1);
    }
  }

  if (RAIL_ConfigAddressFilter(railHandle, &config)
      == RAIL_STATUS_NO_ERROR) {
    args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
    printAddresses(args);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x30, "Invalid address filtering configuration.");
  }
}

void setAddressFilter(sl_cli_command_arg_t *args)
{
  uint32_t enable = sl_cli_get_argument_uint32(args, 0);

  RAIL_EnableAddressFilter(railHandle, !!enable);

  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getAddressFilter(args);
}

void getAddressFilter(sl_cli_command_arg_t *args)
{
  bool filteringEnabled = RAIL_IsAddressFilterEnabled(railHandle);

  responsePrint(sl_cli_get_command_string(args, 0),
                "AddressFiltering:%s",
                filteringEnabled ? "Enabled" : "Disabled");
}

static void printByteBuffer(char *printBuf, uint8_t *dataBuf, uint8_t size)
{
  if (size > 0) {
    for (int i = 0; i < size; i++) {
      printBuf += sprintf(printBuf, "0x%.2x ", dataBuf[i]);
    }
    *(printBuf - 1) = '\0'; // Get rid of the last space
  } else {
    *printBuf = '\0';
  }
}

void printAddresses(sl_cli_command_arg_t *args)
{
  int i, j;

  responsePrintHeader(sl_cli_get_command_string(args, 0),
                      "Field:%u,Index:%u,Offset:%u,"
                      "Size:%u,Address:%s,"
#if RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK
                      "Mask:%s,"
#endif
                      "Status:%s");

  for (i = 0; i < ADDRFILT_FIELD_COUNT; i++) {
#if RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK
    printByteBuffer(addressMaskPrintBuffer, addressMasks[i], config.sizes[i]);
#endif

    for (j = 0; j < ADDRFILT_ENTRY_COUNT; j++) {
      printByteBuffer(addressPrintBuffer,
                      addresses[(i * ADDRFILT_ENTRY_COUNT) + j],
                      config.sizes[i]);

      responsePrintMulti("Field:%u,Index:%u,Offset:%u,"
                         "Size:%u,Address:%s,"
#if RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK
                         "Mask:%s,"
#endif
                         "Status:%s",
                         i,
                         j,
                         config.offsets[i],
                         config.sizes[i],
                         addressPrintBuffer,
#if RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK
                         addressMaskPrintBuffer,
#endif
                         addressEnabled[i][j] ? "Enabled" : "Disabled");
    }
  }
}

void setAddressMask(sl_cli_command_arg_t *args)
{
#if RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK
  int i;
  RAIL_Status_t result;
  uint8_t field = sl_cli_get_argument_uint8(args, 0);
  uint8_t addressMask[ADDRFILT_ENTRY_SIZE];

  // Make sure the field and index parameters are in range
  if (field >= ADDRFILT_FIELD_COUNT) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x31, "Address field out of range!");
    return;
  }

  // Make sure this isn't too large of an entry
  if (sl_cli_get_argument_count(args) >= (ADDRFILT_ENTRY_SIZE + 2)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x32, "Too many address bytes given!");
    return;
  }

  // Read out all the bytes given into the address mask cache
  memset(addressMask, 0xFF, sizeof(addressMask));
  for (i = 0; i < sl_cli_get_argument_count(args) - 1; i++) {
    addressMask[i] = sl_cli_get_argument_uint8(args, i + 1);
  }

  result = RAIL_SetAddressFilterAddressMask(railHandle,
                                            field,
                                            addressMask);
  if (result != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x33, "Could not configure address!");
    return;
  }
  // Copy this into our local structure for tracking
  memcpy(addressMasks[field], addressMask, sizeof(addressMask));

  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  printAddresses(args);
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0x34, "Masked address filtering not supported!");
#endif
}

void setAddress(sl_cli_command_arg_t *args)
{
  int i;
  RAIL_Status_t result;
  uint8_t field = sl_cli_get_argument_uint8(args, 0);
  uint8_t index = sl_cli_get_argument_uint8(args, 1);
  uint8_t address[ADDRFILT_ENTRY_SIZE];
  int location = field * ADDRFILT_ENTRY_COUNT + index;

  // Make sure the field and index parameters are in range
  if (field >= ADDRFILT_FIELD_COUNT || index >= ADDRFILT_ENTRY_COUNT) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x31, "Address field or index out of range!");
    return;
  }

  // Make sure this isn't too large of an entry
  if (sl_cli_get_argument_count(args) >= (ADDRFILT_ENTRY_SIZE + 3)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x32, "Too many address bytes given!");
    return;
  }

  // Read out all the bytes given into the address cache
  memset(address, 0, sizeof(address));
  for (i = 0; i < sl_cli_get_argument_count(args) - 2; i++) {
    address[i] = sl_cli_get_argument_uint8(args, i + 2);
  }

  result = RAIL_SetAddressFilterAddress(railHandle,
                                        field,
                                        index,
                                        address,
                                        addressEnabled[field][index]);
  if (result != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x33, "Could not configure address!");
    return;
  }
  // Copy this into our local structure for tracking
  memcpy(addresses[location], address, sizeof(address));

  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  printAddresses(args);
}

void enableAddress(sl_cli_command_arg_t *args)
{
  uint8_t field = sl_cli_get_argument_uint8(args, 0);
  uint8_t index = sl_cli_get_argument_uint8(args, 1);
  uint8_t enable = sl_cli_get_argument_uint8(args, 2);
  RAIL_Status_t result;

  result = RAIL_EnableAddressFilterAddress(railHandle, !!enable, field, index);

  if (result != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x34, "Could not enable/disable address!");
    return;
  } else {
    addressEnabled[field][index] = (enable != 0);
    responsePrint(sl_cli_get_command_string(args, 0),
                  "Field:%u,Index:%u,Offset:%u,"
                  "Size:%u,Address:%s,Status:%s",
                  field,
                  index,
                  config.offsets[field],
                  config.sizes[field],
                  addressPrintBuffer,
                  addressEnabled[field][index] ? "Enabled" : "Disabled");
  }
}
