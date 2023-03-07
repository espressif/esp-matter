/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "flash_cs300.h"
#include "iotsdk/block_device.h"
#include "iotsdk/flashiap_block_device.h"

#include <inttypes.h>

static mdh_flash_t *get_example_flash()
{
    return get_ram_drive_instance();
}

iotsdk_blockdevice_t *get_example_block_device(void)
{
    static iotsdk_flashiap_blockdevice_t flashiap_block_device;
    mdh_flash_t *dev;
    uint32_t address;
    uint32_t size;

    dev = get_example_flash();

    /* iotsdk_flashiap_blockdevice_new creates a block device given a MCU
     * Driver HAL flash device. The block device does not need to use the
     * entire flash, although we will use the whole device for this example.
     * Applications may wish to use only a portion of the flash device. This is
     * similar to hard disk partitioning on e.g. Linux, where /dev/sda1
     * (partiton 1) can be used as a block device, the same as /dev/sda2
     * (partition 2) or /dev/sda (whole device). */

    /* Passing in 0 for address will use the base address of the flash device
     * as known by the HAL. */
    address = 0;

    /* Passing in 0 for size will use the size of the flash device as known by
     * the HAL. */
    size = 0;

    flashiap_block_device = iotsdk_flashiap_blockdevice_new(dev, address, size);

    return (iotsdk_blockdevice_t *)&flashiap_block_device;
}
