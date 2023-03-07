/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IOTSDK_FLASHIAPBLOCKDEVICE_H
#define IOTSDK_FLASHIAPBLOCKDEVICE_H

#include "iotsdk/BlockDevice.h"
#include "iotsdk/CentauriBlockDevice.h"

#include "iotsdk/flashiap_block_device.h"

#include "hal/flash_api.h"

#include <stdint.h>

namespace iotsdk {
namespace storage {
/** BlockDevice using the FlashIAP API
 *
 */
class FlashIAPBlockDevice final : public CentauriBlockDevice {
public:
    /** Creates a FlashIAPBlockDevice
     *
     *  @param flash    Pointer to the underlying flash device
     *  @param address  Physical address where the block device start
     *  @param size     The block device size
     */
    FlashIAPBlockDevice(mdh_flash_t *flash, uint32_t address, uint32_t size);

    /** Get the BlockDevice class type.
     *
     *  @return         A string representing the BlockDevice class type.
     */
    const char *get_type() const override;

    /** Get the size of a eraseable block
     *
     *  @return         Size of a eraseable block in bytes
     *  @note Must be a multiple of the program size
     */
    bd_size_t get_erase_size() const override;

    /** Get the size of an erasable block given address
     *
     *  @param addr     Address within the erasable block
     *  @return         Size of an erasable block in bytes
     *  @note Must be a multiple of the program size
     */
    bd_size_t get_erase_size(bd_addr_t addr) const override;

private:
    iotsdk_flashiap_blockdevice_t _flashiap_bd;
};
} // namespace storage
} // namespace iotsdk

#endif /* IOTSDK_FLASHIAPBLOCKDEVICE_H */
