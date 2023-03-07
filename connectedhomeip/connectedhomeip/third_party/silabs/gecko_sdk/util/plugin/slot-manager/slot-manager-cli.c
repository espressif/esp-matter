/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include PLATFORM_HEADER
#include "slot-manager-cli.h"
#include "api/btl_interface.h"
#include "slot-manager.h"
#include "printf.h"
#include "sl_status.h"

void emAfPluginSlotManagerCliBootloadSlot(sl_cli_command_arg_t *arguments)
{
  uint32_t slotId = sl_cli_get_argument_uint32(arguments, 0);

  if (emberAfPluginSlotManagerVerifyAndBootloadSlot(slotId) != SL_STATUS_OK) {
    printf("Unable to boot image at slot %d\n", slotId);
  }
}

void emAfPluginSlotManagerCliEraseSlot(sl_cli_command_arg_t *arguments)
{
  uint32_t slotId = sl_cli_get_argument_uint32(arguments, 0);
  if (emberAfPluginSlotManagerEraseSlot(slotId) != SL_STATUS_OK) {
    printf("Slot Manager failed to erase slot\n");
  }
}

void emAfPluginSlotManagerCliPrintExternalFlashInfo(sl_cli_command_arg_t *arguments)
{
  emberAfPluginSlotManagerPrintExternalFlashInfo();
}

void emAfPluginSlotManagerCliReadExtFlash(sl_cli_command_arg_t *arguments)
{
  uint8_t offset, data[MAX_FLASH_READ_BYTES];
  uint32_t address = sl_cli_get_argument_uint32(arguments, 0);
  uint8_t len = sl_cli_get_argument_uint8(arguments, 1);

  if (len > MAX_FLASH_READ_BYTES) {
    printf("Max read limit set to %d bytes\n", MAX_FLASH_READ_BYTES);
    len = MAX_FLASH_READ_BYTES;
  }

  if (emberAfPluginSlotManagerReadExtFlash(address, data,
                                           len) == SL_STATUS_OK) {
    printf("Address          Data\n");
    for (offset = 0; offset < (len - 1); offset++) {
      printf("0x%4x       0x%x\n", address + offset, data[offset]);
    }
  } else {
    printf("Failed to read from flash\n");
  }
}

void emAfPluginSlotManagerCliPrintSlotsInfo(sl_cli_command_arg_t *arguments)
{
  uint32_t slotId = 0;
  SlotManagerSlotInfo_t slotInfo;
  bool imagePresentInSlot;
  uint8_t index;

  while (SL_STATUS_OK == emberAfPluginSlotManagerGetSlotInfo(slotId, &slotInfo)) {
    imagePresentInSlot = true;
    printf("Slot %d\n"
           "  Address     : 0x%4X\n"
           "  Length      : 0x%4X (%d bytes)\n",
           slotId,
           slotInfo.slotStorageInfo.address,
           slotInfo.slotStorageInfo.length,
           slotInfo.slotStorageInfo.length);
    printf("  Type        : ");
    switch (slotInfo.slotAppInfo.type) {
      case 0:
        // This means there's no GBL there
        printf("No GBL present\n");
        imagePresentInSlot = false;
        break;
      case APPLICATION_TYPE_ZIGBEE:
        printf("ZigBee\n");
        break;
      case APPLICATION_TYPE_THREAD:
        printf("Thread\n");
        break;
      case APPLICATION_TYPE_FLEX:
        printf("Connect\n");
        break;
      case APPLICATION_TYPE_BLUETOOTH:
        printf("Bluetooth\n");
        break;
      case APPLICATION_TYPE_BLUETOOTH_APP:
        printf("Bluetooth application\n");
        break;
      case APPLICATION_TYPE_MCU:
        printf("MCU\n");
        break;
      default:
        printf("Other (0x%X)\n", slotInfo.slotAppInfo.type);
        break;
    }

    if (imagePresentInSlot) {
      printf("  Version     : 0x%4X\n", slotInfo.slotAppInfo.version);
      printf("  Capabilities: 0x%4X\n", slotInfo.slotAppInfo.capabilities);
      printf("  Product ID  : ");
      uint8_t numProductIdDigits = COUNTOF(slotInfo.slotAppInfo.productId);
      for (index = 0; index < numProductIdDigits; index++) {
        printf("0x%X ", slotInfo.slotAppInfo.productId[index]);
      }
      printf("\n");
    }
    slotId++;
  }
}
