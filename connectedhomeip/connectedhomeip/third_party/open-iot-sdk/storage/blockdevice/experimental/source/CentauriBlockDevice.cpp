/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/CentauriBlockDevice.h"

using namespace iotsdk::storage;

// Debug available
#ifndef CENTAURIBLOCKDEVICE_DEBUG
#define CENTAURIBLOCKDEVICE_DEBUG 0
#endif

#if CENTAURIBLOCKDEVICE_DEBUG
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

CentauriBlockDevice::CentauriBlockDevice(iotsdk_blockdevice_t *bd) : _bd{bd}
{
}

bd_status CentauriBlockDevice::init()
{
    DEBUG_PRINTF("init\r\n");
    return static_cast<bd_status>(iotsdk_blockdevice_initialize(_bd));
}

bd_status CentauriBlockDevice::deinit()
{
    DEBUG_PRINTF("deinit\r\n");
    return static_cast<bd_status>(iotsdk_blockdevice_uninitialize(_bd));
}

bd_status CentauriBlockDevice::read(void *buffer, bd_addr_t virtual_address, bd_size_t size)
{
    DEBUG_PRINTF("read: %" PRIX64 " %" PRIX64 "\r\n", virtual_address, size);
    return static_cast<bd_status>(iotsdk_blockdevice_read(_bd, virtual_address, buffer, size));
}

bd_status CentauriBlockDevice::program(const void *buffer, bd_addr_t virtual_address, bd_size_t size)
{
    DEBUG_PRINTF("program: %" PRIX64 " %" PRIX64 "\r\n", virtual_address, size);
    return static_cast<bd_status>(iotsdk_blockdevice_program(_bd, virtual_address, buffer, size));
}

bd_status CentauriBlockDevice::erase(bd_addr_t virtual_address, bd_size_t size)
{
    DEBUG_PRINTF("erase: %" PRIX64 " %" PRIX64 "\r\n", virtual_address, size);
    return static_cast<bd_status>(iotsdk_blockdevice_erase(_bd, virtual_address, size));
}

bd_size_t CentauriBlockDevice::get_read_size() const
{
    DEBUG_PRINTF("get_read_size: %d\r\n", FLASHIAP_READ_SIZE);
    return iotsdk_blockdevice_get_read_size(_bd);
}

bd_size_t CentauriBlockDevice::get_program_size() const
{
    const auto page_size = iotsdk_blockdevice_get_program_size(_bd);
    DEBUG_PRINTF("get_program_size: %" PRIX32 "\r\n", page_size);
    return page_size;
}

bd_size_t CentauriBlockDevice::get_erase_size() const
{
    const auto erase_size = iotsdk_blockdevice_get_erase_size(_bd, 0);
    DEBUG_PRINTF("get_erase_size: %" PRIX32 "\r\n", erase_size);
    return erase_size;
}

bd_size_t CentauriBlockDevice::get_erase_size(bd_addr_t addr) const
{
    const auto erase_size = iotsdk_blockdevice_get_erase_size(_bd, addr);
    DEBUG_PRINTF("get_erase_size: %" PRIX32 "\r\n", erase_size);
    return erase_size;
}

int CentauriBlockDevice::get_erase_value() const
{
    const auto erase_val = iotsdk_blockdevice_get_erase_value(_bd);

    DEBUG_PRINTF("get_erase_value: %" PRIX8 "\r\n", erase_val);

    return erase_val;
}

bd_size_t CentauriBlockDevice::size() const
{
    DEBUG_PRINTF("size: %" PRIX64 "\r\n", _size);

    return iotsdk_blockdevice_get_size(_bd);
}

bd_status CentauriBlockDevice::is_valid_read(bd_addr_t addr, bd_size_t size) const
{
    return static_cast<bd_status>(iotsdk_blockdevice_is_valid_read(_bd, addr, size));
}

bd_status CentauriBlockDevice::is_valid_program(bd_addr_t addr, bd_size_t size) const
{
    return static_cast<bd_status>(iotsdk_blockdevice_is_valid_program(_bd, addr, size));
}

bd_status CentauriBlockDevice::is_valid_erase(bd_addr_t addr, bd_size_t size) const
{
    return static_cast<bd_status>(iotsdk_blockdevice_is_valid_erase(_bd, addr, size));
}
