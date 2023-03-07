/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/FlashIAPBlockDevice.h"

using namespace iotsdk::storage;

#define FLASHIAP_READ_SIZE 1

// Debug available
#ifndef FLASHIAP_DEBUG
#define FLASHIAP_DEBUG 0
#endif

#if FLASHIAP_DEBUG
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

FlashIAPBlockDevice::FlashIAPBlockDevice(mdh_flash_t *flash, uint32_t address, uint32_t size)
    : _flash{flash}, _base(address), _size(size), _is_initialized(false)
{
}

FlashIAPBlockDevice::~FlashIAPBlockDevice()
{
}

bd_status FlashIAPBlockDevice::init()
{
    DEBUG_PRINTF("init\r\n");

    if (!_flash) {
        return bd_status::DEVICE_ERROR;
    }

    if (!_base) {
        _base = mdh_flash_get_start_address(_flash);
    }

    if (!_size) {
        _size = mdh_flash_get_size(_flash) - (_base - mdh_flash_get_start_address(_flash));
    }

    if (_size + _base > mdh_flash_get_size(_flash) + mdh_flash_get_start_address(_flash)) {
        return bd_status::INCORRECT_SIZE;
    }

    if (_base < mdh_flash_get_start_address(_flash)) {
        return bd_status::INCORRECT_ADDRESS;
    }

    _is_initialized = true;
    return bd_status::OK;
}

bd_status FlashIAPBlockDevice::deinit()
{
    DEBUG_PRINTF("deinit\r\n");

    if (!_is_initialized) {
        return bd_status::OK;
    }

    _is_initialized = false;

    _flash = nullptr;

    return bd_status::OK;
}

bd_status FlashIAPBlockDevice::read(void *buffer, bd_addr_t virtual_address, bd_size_t size)
{
    DEBUG_PRINTF("read: %" PRIX64 " %" PRIX64 "\r\n", virtual_address, size);

    if (!_is_initialized) {
        return bd_status::DEVICE_NOT_INITIALIZED;
    }

    /* Check that the address and size are properly aligned and fit. */
    bd_status status = is_valid_read(virtual_address, size);
    if (status != bd_status::OK) {
        return status;
    }

    /* Convert virtual address to the physical address for the device. */
    const auto physical_address = static_cast<uint32_t>(_base + virtual_address);

    /* Read data using the internal flash driver. */
    if (mdh_flash_read(_flash, physical_address, static_cast<uint8_t *>(buffer), static_cast<uint32_t>(size))) {
        return bd_status::DEVICE_ERROR;
    }

    DEBUG_PRINTF("physical: %" PRIX64 "\r\n", physical_address);

    return bd_status::OK;
}

bd_status FlashIAPBlockDevice::program(const void *buffer, bd_addr_t virtual_address, bd_size_t size)
{
    DEBUG_PRINTF("program: %" PRIX64 " %" PRIX64 "\r\n", virtual_address, size);

    if (!_is_initialized) {
        return bd_status::DEVICE_NOT_INITIALIZED;
    }

    /* Check that the address and size are properly aligned and fit. */
    bd_status status = is_valid_program(virtual_address, size);
    if (status != bd_status::OK) {
        return status;
    }

    /* Convert virtual address to the physical address for the device. */
    auto physical_address = static_cast<uint32_t>(_base + virtual_address);
    auto remaining_size = static_cast<uint32_t>(size);
    const auto *buf = static_cast<const uint8_t *>(buffer);

    while (remaining_size > 0) {
        /* Multiple pages can be programmed at once but cannot cross sector boundaries */
        const auto sector_size = mdh_flash_get_sector_size(_flash, physical_address);
        const auto sector_end = (physical_address / sector_size + 1) * sector_size;
        const auto chunk =
            (physical_address + remaining_size > sector_end) ? (sector_end - physical_address) : remaining_size;
        /* Write data using the internal flash driver. */
        if (mdh_flash_program_page(_flash, physical_address, buf, chunk) != 0) {
            return bd_status::DEVICE_ERROR;
        }
        physical_address += chunk;
        remaining_size -= chunk;
        buf += chunk;
    }

    DEBUG_PRINTF("physical: %" PRIX64 " %" PRIX64 "\r\n", physical_address, size);

    return bd_status::OK;
}

bd_status FlashIAPBlockDevice::erase(bd_addr_t virtual_address, bd_size_t size)
{
    DEBUG_PRINTF("erase: %" PRIX64 " %" PRIX64 "\r\n", virtual_address, size);

    if (!_is_initialized) {
        return bd_status::DEVICE_NOT_INITIALIZED;
    }

    /* Check that the address and size are properly aligned and fit. */
    const bd_status status = is_valid_erase(virtual_address, size);
    if (status != bd_status::OK) {
        return status;
    }

    /* Convert virtual address to the physical address for the device. */
    auto physical_address = static_cast<uint32_t>(_base + virtual_address);
    const auto sector_size = mdh_flash_get_sector_size(_flash, _base);
    const auto erase_region = (physical_address + size) - sector_size;

    while (physical_address <= erase_region) {
        if (mdh_flash_erase_sector(_flash, physical_address)) {
            return bd_status::DEVICE_ERROR;
        }
        physical_address += sector_size;
    }

    return bd_status::OK;
}

bd_size_t FlashIAPBlockDevice::get_read_size() const
{
    DEBUG_PRINTF("get_read_size: %d\r\n", FLASHIAP_READ_SIZE);

    return FLASHIAP_READ_SIZE;
}

bd_size_t FlashIAPBlockDevice::get_program_size() const
{
    if (!_is_initialized) {
        return 0;
    }

    uint32_t page_size = mdh_flash_get_page_size(_flash);

    DEBUG_PRINTF("get_program_size: %" PRIX32 "\r\n", page_size);

    return page_size;
}

bd_size_t FlashIAPBlockDevice::get_erase_size() const
{
    DEBUG_PRINTF("get_erase_size: FlashIAPBlockDevice requires an address to query erase size\r\n");

    return 0;
}

bd_size_t FlashIAPBlockDevice::get_erase_size(bd_addr_t addr) const
{
    if (!_is_initialized) {
        return 0;
    }

    if (static_cast<uint64_t>(_base) + addr > UINT32_MAX) {
        return 0;
    }

    uint32_t erase_size = mdh_flash_get_sector_size(_flash, static_cast<uint32_t>(_base + addr));

    DEBUG_PRINTF("get_erase_size: %" PRIX32 "\r\n", erase_size);

    return erase_size;
}

int FlashIAPBlockDevice::get_erase_value() const
{
    if (!_is_initialized) {
        return -1;
    }

    uint8_t erase_val = mdh_flash_get_erase_value(_flash);

    DEBUG_PRINTF("get_erase_value: %" PRIX8 "\r\n", erase_val);

    return erase_val;
}

bd_size_t FlashIAPBlockDevice::size() const
{
    DEBUG_PRINTF("size: %" PRIX64 "\r\n", _size);

    return _size;
}

const char *FlashIAPBlockDevice::get_type() const
{
    return "FLASHIAP";
}

bd_status FlashIAPBlockDevice::is_valid_read(bd_addr_t addr, bd_size_t size) const
{
    if (static_cast<uint64_t>(_base) + addr > UINT32_MAX) {
        return bd_status::INCORRECT_ADDRESS;
    }

    if (size > UINT32_MAX) {
        return bd_status::INCORRECT_SIZE;
    }

    return BlockDevice::is_valid_read(addr, size);
}

bd_status FlashIAPBlockDevice::is_valid_program(bd_addr_t addr, bd_size_t size) const
{
    if (static_cast<uint64_t>(_base) + addr > UINT32_MAX) {
        return bd_status::INCORRECT_ADDRESS;
    }

    if (size > UINT32_MAX) {
        return bd_status::INCORRECT_SIZE;
    }

    return BlockDevice::is_valid_program(addr, size);
}

bd_status FlashIAPBlockDevice::is_valid_erase(bd_addr_t addr, bd_size_t size) const
{
    if (static_cast<uint64_t>(_base) + addr > UINT32_MAX) {
        return bd_status::INCORRECT_ADDRESS;
    }

    if (size > UINT32_MAX) {
        return bd_status::INCORRECT_SIZE;
    }

    return BlockDevice::is_valid_erase(addr, size);
}
