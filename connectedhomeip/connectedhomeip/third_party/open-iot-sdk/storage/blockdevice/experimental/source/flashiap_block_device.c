/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/flashiap_block_device.h"

#ifdef __cplusplus
extern "C" {
#endif

static iotsdk_blockdevice_status_t flashiap_initialize(iotsdk_blockdevice_t *self)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;

    if (!derived->dev) {
        return IOTSDK_BD_STATUS_DEVICE_ERROR;
    }

    if (!derived->base_addr) {
        derived->base_addr = mdh_flash_get_start_address(derived->dev);
    }

    if (!derived->size) {
        derived->size =
            mdh_flash_get_size(derived->dev) - (derived->base_addr - mdh_flash_get_start_address(derived->dev));
    }

    if (derived->size + derived->base_addr
        > mdh_flash_get_size(derived->dev) + mdh_flash_get_start_address(derived->dev)) {
        return IOTSDK_BD_STATUS_INCORRECT_SIZE;
    }

    if (derived->base_addr < mdh_flash_get_start_address(derived->dev)) {
        return IOTSDK_BD_STATUS_INCORRECT_ADDRESS;
    }

    derived->is_initialized = true;
    return IOTSDK_BD_STATUS_OK;
}

static iotsdk_blockdevice_status_t flashiap_uninitialize(iotsdk_blockdevice_t *self)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;
    derived->is_initialized = false;
    return IOTSDK_BD_STATUS_OK;
}

static iotsdk_blockdevice_status_t flashiap_read(iotsdk_blockdevice_t *self, size_t addr, void *data, size_t size)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;

    if (!derived->is_initialized) {
        return IOTSDK_BD_STATUS_DEVICE_NOT_INITIALIZED;
    }

    const iotsdk_blockdevice_status_t status = iotsdk_blockdevice_is_valid_read(self, addr, size);
    if (status != IOTSDK_BD_STATUS_OK) {
        return status;
    }

    /* Convert virtual address to the physical address for the device. */
    const uint32_t physical_address = (uint32_t)(derived->base_addr + addr);

    /* Read data using the internal flash driver. */
    if (mdh_flash_read(derived->dev, physical_address, (uint8_t *)data, (uint32_t)size)) {
        return IOTSDK_BD_STATUS_DEVICE_ERROR;
    }

    return IOTSDK_BD_STATUS_OK;
}

static iotsdk_blockdevice_status_t flashiap_is_valid_read(iotsdk_blockdevice_t *self, size_t addr, size_t size)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;

    /* Check that the address and size are properly aligned and fit. */
    if ((uint64_t)derived->base_addr + addr > UINT32_MAX) {
        return IOTSDK_BD_STATUS_INCORRECT_ADDRESS;
    }

    if (size > UINT32_MAX || addr + size > derived->size) {
        return IOTSDK_BD_STATUS_INCORRECT_SIZE;
    }

    return IOTSDK_BD_STATUS_OK;
}

static size_t flashiap_get_read_size(iotsdk_blockdevice_t *self)
{
    (void)self;
    return FLASHIAP_READ_SIZE;
}

static iotsdk_blockdevice_status_t
flashiap_program(iotsdk_blockdevice_t *self, size_t addr, const void *data, size_t size)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;

    if (!derived->is_initialized) {
        return IOTSDK_BD_STATUS_DEVICE_NOT_INITIALIZED;
    }

    const iotsdk_blockdevice_status_t status = iotsdk_blockdevice_is_valid_program(self, addr, size);
    if (status != IOTSDK_BD_STATUS_OK) {
        return status;
    }

    /* Convert virtual address to the physical address for the device. */
    uint32_t physical_address = (uint32_t)(derived->base_addr + addr);
    uint32_t remaining_size = (uint32_t)size;
    const uint8_t *buf = (const uint8_t *)data;

    while (remaining_size > 0) {
        /* Multiple pages can be programmed at once but cannot cross sector boundaries */
        const uint32_t sector_size = mdh_flash_get_sector_size(derived->dev, physical_address);
        const uint32_t sector_end = (physical_address / sector_size + 1) * sector_size;
        const uint32_t chunk =
            (physical_address + remaining_size > sector_end) ? (sector_end - physical_address) : remaining_size;
        /* Write data using the internal flash driver. */
        if (mdh_flash_program_page(derived->dev, physical_address, buf, chunk) != 0) {
            return IOTSDK_BD_STATUS_DEVICE_ERROR;
        }
        physical_address += chunk;
        remaining_size -= chunk;
        buf += chunk;
    }

    return IOTSDK_BD_STATUS_OK;
}

static iotsdk_blockdevice_status_t flashiap_is_valid_program(iotsdk_blockdevice_t *self, size_t addr, size_t size)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;
    const uint32_t chunk = mdh_flash_get_page_size(derived->dev);

    if (chunk == 0) {
        return IOTSDK_BD_STATUS_DEVICE_ERROR;
    }

    const uint64_t wide_base_addr = (uint64_t)derived->base_addr;

    /* Check that the address and size are properly aligned and fit. */
    if (wide_base_addr + addr > UINT32_MAX) {
        return IOTSDK_BD_STATUS_INCORRECT_ADDRESS;
    }

    if (size > UINT32_MAX || ((wide_base_addr + addr + size)) > UINT32_MAX) {
        return IOTSDK_BD_STATUS_INCORRECT_SIZE;
    }

    if (addr + size > derived->size) {
        return IOTSDK_BD_STATUS_INCORRECT_SIZE;
    }

    if (size % chunk != 0) {
        return IOTSDK_BD_STATUS_SIZE_NOT_ALIGNED;
    }

    if (addr % chunk != 0) {
        return IOTSDK_BD_STATUS_ADDRESS_NOT_ALIGNED;
    }

    return IOTSDK_BD_STATUS_OK;
}

static size_t flashiap_get_program_size(iotsdk_blockdevice_t *self)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;

    if (!derived->is_initialized) {
        return 0;
    }

    return mdh_flash_get_page_size(derived->dev);
}

static iotsdk_blockdevice_status_t flashiap_erase(iotsdk_blockdevice_t *self, size_t addr, size_t size)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;

    if (!derived->is_initialized) {
        return IOTSDK_BD_STATUS_DEVICE_NOT_INITIALIZED;
    }

    const iotsdk_blockdevice_status_t status = iotsdk_blockdevice_is_valid_erase(self, addr, size);
    if (status != IOTSDK_BD_STATUS_OK) {
        return status;
    }

    /* Convert virtual address to the physical address for the device. */
    uint32_t physical_address = (uint32_t)(derived->base_addr + addr);
    const uint32_t erase_region = (uint32_t)(physical_address + size)
                                  - mdh_flash_get_sector_size(derived->dev, (uint32_t)(physical_address + size - 1));

    while (physical_address <= erase_region) {
        if (mdh_flash_erase_sector(derived->dev, physical_address)) {
            return IOTSDK_BD_STATUS_DEVICE_ERROR;
        }
        uint32_t sector_size = mdh_flash_get_sector_size(derived->dev, physical_address);
        if (sector_size == 0) {
            return IOTSDK_BD_STATUS_DEVICE_ERROR;
        }
        physical_address += sector_size;
    }

    return IOTSDK_BD_STATUS_OK;
}

static iotsdk_blockdevice_status_t flashiap_is_valid_erase(iotsdk_blockdevice_t *self, size_t addr, size_t size)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;

    const uint64_t wide_base_addr = (uint64_t)derived->base_addr;

    /* Check that the address and size are properly aligned and fit. */
    if (wide_base_addr + addr > UINT32_MAX) {
        return IOTSDK_BD_STATUS_INCORRECT_ADDRESS;
    }

    /* Convert virtual address to the physical address for the device. */
    uint32_t physical_address = (uint32_t)(wide_base_addr + addr);

    const uint32_t first_sector_size = mdh_flash_get_sector_size(derived->dev, physical_address);
    const uint32_t last_sector_size = mdh_flash_get_sector_size(derived->dev, physical_address + (uint32_t)size - 1);

    if (first_sector_size == 0 || last_sector_size == 0) {
        return IOTSDK_BD_STATUS_DEVICE_ERROR;
    }

    if (size > UINT32_MAX || ((uint64_t)physical_address + size) > UINT32_MAX) {
        return IOTSDK_BD_STATUS_INCORRECT_SIZE;
    }

    /* Check that the address and size are properly aligned and fit. */
    if ((addr + size) > derived->size) {
        return IOTSDK_BD_STATUS_INCORRECT_SIZE;
    }

    if (addr % mdh_flash_get_sector_size(derived->dev, physical_address) != 0) {
        return IOTSDK_BD_STATUS_ADDRESS_NOT_ALIGNED;
    }

    if ((addr + size) % last_sector_size != 0) {
        return IOTSDK_BD_STATUS_SIZE_NOT_ALIGNED;
    }

    return IOTSDK_BD_STATUS_OK;
}

static size_t flashiap_get_erase_size(iotsdk_blockdevice_t *self, size_t addr)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;

    if (!derived->is_initialized) {
        return 0;
    }

    if ((uint64_t)derived->base_addr + addr > UINT32_MAX) {
        return 0;
    }

    return mdh_flash_get_sector_size(derived->dev, derived->base_addr + (uint32_t)addr);
}

static int32_t flashiap_get_erase_value(iotsdk_blockdevice_t *self)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;

    if (!derived->is_initialized) {
        return -1;
    }

    return mdh_flash_get_erase_value(derived->dev);
}

static size_t flashiap_get_size(iotsdk_blockdevice_t *self)
{
    iotsdk_flashiap_blockdevice_t *derived = (iotsdk_flashiap_blockdevice_t *)self;
    return (size_t)derived->size;
}

static const iotsdk_blockdevice_vtable_t gsc_vtbl = {
    .initialize = flashiap_initialize,
    .uninitialize = flashiap_uninitialize,
    .read = flashiap_read,
    .is_valid_read = flashiap_is_valid_read,
    .get_read_size = flashiap_get_read_size,
    .program = flashiap_program,
    .is_valid_program = flashiap_is_valid_program,
    .get_program_size = flashiap_get_program_size,
    .erase = flashiap_erase,
    .is_valid_erase = flashiap_is_valid_erase,
    .get_erase_size = flashiap_get_erase_size,
    .get_erase_value = flashiap_get_erase_value,
    .get_size = flashiap_get_size,
};
static const iotsdk_blockdevice_t gsc_parent = {.vtbl = &gsc_vtbl};

iotsdk_flashiap_blockdevice_t iotsdk_flashiap_blockdevice_new(mdh_flash_t *dev, uint32_t address, uint32_t size)
{
    iotsdk_flashiap_blockdevice_t inst = {
        .parent = gsc_parent, .dev = dev, .base_addr = address, .size = size, .is_initialized = false};
    return inst;
}

#ifdef __cplusplus
} // extern "C"
#endif
