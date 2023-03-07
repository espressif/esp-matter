/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/** \addtogroup storage */
/** @{*/

#ifndef IOTSDK_FLASHIAP_BLOCK_DEVICE_H
#define IOTSDK_FLASHIAP_BLOCK_DEVICE_H

#include "iotsdk/block_device.h"

#include "hal/flash_api.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Read size of the underlying flash device in bytes. */
#ifndef FLASHIAP_READ_SIZE
#define FLASHIAP_READ_SIZE 1
#endif

/* FlashIAP block device instance struct, derived from iotsdk_blockdevice_t */
typedef struct flashiap_s {
    iotsdk_blockdevice_t parent;
    mdh_flash_t *dev;
    uint32_t base_addr;
    uint32_t size;
    bool is_initialized;
} iotsdk_flashiap_blockdevice_t;

/** Create a FlashIAP block device
 *
 * @param dev      Pointer to the underlying flash device HAL implementation
 * @param address  Base address of the block device, if 0 the address is taken from the underlying HAL
 * @param size     Size of the block device, if 0 size is taken from the underlying HAL
 */
iotsdk_flashiap_blockdevice_t iotsdk_flashiap_blockdevice_new(mdh_flash_t *dev, uint32_t address, uint32_t size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IOTSDK_FLASHIAP_BLOCK_DEVICE_H

/** @}*/
