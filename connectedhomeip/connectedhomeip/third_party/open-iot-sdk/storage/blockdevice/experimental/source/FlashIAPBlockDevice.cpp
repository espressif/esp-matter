/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/FlashIAPBlockDevice.h"

#include "iotsdk/BlockDevice.h"

#include "iotsdk/block_device.h"

using namespace iotsdk::storage;

FlashIAPBlockDevice::FlashIAPBlockDevice(mdh_flash_t *flash, uint32_t address, uint32_t size)
    : CentauriBlockDevice(reinterpret_cast<iotsdk_blockdevice_t *>(&_flashiap_bd)),
      _flashiap_bd{iotsdk_flashiap_blockdevice_new(flash, address, size)}
{
}

bd_size_t FlashIAPBlockDevice::get_erase_size() const
{
    return 0;
}

bd_size_t FlashIAPBlockDevice::get_erase_size(bd_addr_t addr) const
{
    return CentauriBlockDevice::get_erase_size(addr);
}

const char *FlashIAPBlockDevice::get_type() const
{
    return "FLASHIAP";
}
