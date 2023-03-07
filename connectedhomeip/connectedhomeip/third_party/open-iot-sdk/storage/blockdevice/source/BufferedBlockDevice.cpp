/*
 * Copyright (c) 2018-2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <algorithm>
#include <cstdint>
#include <memory>
#include <new>
#include <string.h>

#include "common/align.h"
#include "iotsdk/BlockDevice.h"
#include "iotsdk/BufferedBlockDevice.h"

using namespace iotsdk::storage;

BufferedBlockDevice::BufferedBlockDevice(BlockDevice *bd)
    : _bd(bd), _bd_program_size(0), _bd_read_size(0), _bd_size(0), _write_cache_addr(0), _write_cache_valid(false),
      _init_ref_count(0), _is_initialized(false)
{
}

bd_status BufferedBlockDevice::init()
{
    if (!_bd) {
        return bd_status::DEVICE_ERROR;
    }

    if (_is_initialized) {
        return bd_status::OK;
    }

    const bd_status status = _bd->init();
    if (status != bd_status::OK) {
        return status;
    }

    _bd_read_size = _bd->get_read_size();
    _bd_program_size = _bd->get_program_size();
    _bd_size = _bd->size();

    if (!_write_cache) {
        _write_cache = std::unique_ptr<uint8_t[]>(new (std::nothrow) uint8_t[_bd_program_size]);
        if (_write_cache == nullptr) {
            return bd_status::DEVICE_NOT_INITIALIZED;
        }
    }

    if (!_read_buf) {
        _read_buf = std::unique_ptr<uint8_t[]>(new (std::nothrow) uint8_t[_bd_read_size]);
        if (_read_buf == nullptr) {
            return bd_status::DEVICE_NOT_INITIALIZED;
        }
    }

    invalidate_write_cache();

    _is_initialized = true;
    return bd_status::OK;
}

bd_status BufferedBlockDevice::deinit()
{
    if (!_is_initialized) {
        return bd_status::OK;
    }

    // Flush out all data from buffers
    const bd_status status = sync();
    if (status != bd_status::OK) {
        return status;
    }

    _is_initialized = false;
    return _bd->deinit();
}

bd_status BufferedBlockDevice::sync()
{
    if (!_is_initialized) {
        return bd_status::DEVICE_NOT_INITIALIZED;
    }

    if (_write_cache_valid) {
        const bd_status ret = _bd->program(_write_cache.get(), _write_cache_addr, _bd_program_size);
        if (ret != bd_status::OK) {
            return ret;
        }
        invalidate_write_cache();
    }
    return bd_status::OK;
}

void BufferedBlockDevice::invalidate_write_cache()
{
    _write_cache_addr = _bd_size;
    _write_cache_valid = false;
}

bd_status BufferedBlockDevice::read(void *b, bd_addr_t addr, bd_size_t size)
{
    if (!_is_initialized) {
        return bd_status::DEVICE_NOT_INITIALIZED;
    }

    bd_status status = is_valid_read(addr, size);
    if (status != bd_status::OK) {
        return status;
    }

    // Common case - no need to involve write cache or read buffer
    if (_bd->is_valid_read(addr, size) == bd_status::OK && // check if valid on underlying storage, e.g. is aligned etc
        ((addr + size <= _write_cache_addr) || (addr > _write_cache_addr + _bd_program_size))) {
        return _bd->read(b, addr, size);
    }

    uint8_t *buf = static_cast<uint8_t *>(b);

    // Read logic: Split read to chunks, according to whether we cross the write cache
    while (size) {
        bd_size_t chunk;
        bool read_from_bd = true;
        if (_write_cache_valid && addr < _write_cache_addr) {
            chunk = std::min(size, _write_cache_addr - addr);
        } else if (_write_cache_valid && (addr >= _write_cache_addr) && (addr < _write_cache_addr + _bd_program_size)) {
            // One case we need to take our data from cache
            chunk = std::min(size, _bd_program_size - addr % _bd_program_size);
            memcpy(buf, _write_cache.get() + addr % _bd_program_size, chunk);
            read_from_bd = false;
        } else {
            chunk = size;
        }

        // Now, in case we read from the BD, make sure we are aligned with its read size.
        // If not, use read buffer as a helper.
        if (read_from_bd) {
            bd_size_t offs_in_read_buf = addr % _bd_read_size;
            if (offs_in_read_buf || (chunk < _bd_read_size)) {
                chunk = std::min(chunk, _bd_read_size - offs_in_read_buf);
                status = _bd->read(_read_buf.get(), addr - offs_in_read_buf, _bd_read_size);
                memcpy(buf, _read_buf.get() + offs_in_read_buf, chunk);
            } else {
                chunk = align_down(chunk, _bd_read_size);
                status = _bd->read(buf, addr, chunk);
            }
            if (status != bd_status::OK) {
                return status;
            }
        }

        buf += chunk;
        addr += chunk;
        size -= chunk;
    }

    return bd_status::OK;
}

bd_status BufferedBlockDevice::program(const void *b, bd_addr_t addr, bd_size_t size)
{
    if (!_is_initialized) {
        return bd_status::DEVICE_NOT_INITIALIZED;
    }

    bd_status status;

    bd_addr_t aligned_addr = align_down(addr, _bd_program_size);

    const uint8_t *buf = static_cast<const uint8_t *>(b);

    // Need to flush cache/sync if moved to another program unit
    if (aligned_addr != _write_cache_addr) {
        status = sync();
        if (status != bd_status::OK) {
            return status;
        }
    }

    // Write logic: Keep data in cache as long as we don't reach the end of the program unit.
    // Otherwise, program to the underlying BD.
    while (size) {
        _write_cache_addr = align_down(addr, _bd_program_size);
        bd_addr_t offs_in_buf = addr - _write_cache_addr;
        bd_size_t chunk;
        if (offs_in_buf) {
            chunk = std::min(_bd_program_size - offs_in_buf, size);
        } else if (size >= _bd_program_size) {
            chunk = align_down(size, _bd_program_size);
        } else {
            chunk = size;
        }

        const uint8_t *prog_buf;
        if (chunk < _bd_program_size) {
            // If cache not valid, and program doesn't cover an entire unit, it means we need to
            // read it from the underlying BD
            if (!_write_cache_valid) {
                status = _bd->read(_write_cache.get(), _write_cache_addr, _bd_program_size);
                if (status != bd_status::OK) {
                    return status;
                }
            }
            memcpy(_write_cache.get() + offs_in_buf, buf, chunk);
            prog_buf = _write_cache.get();
        } else {
            prog_buf = buf;
        }

        // Only program if we reached the end of a program unit
        if (!((offs_in_buf + chunk) % _bd_program_size)) {
            status = _bd->program(prog_buf, _write_cache_addr, std::max(chunk, _bd_program_size));
            if (status != bd_status::OK) {
                return status;
            }
            invalidate_write_cache();
        } else {
            _write_cache_valid = true;
        }

        buf += chunk;
        addr += chunk;
        size -= chunk;
    }

    return bd_status::OK;
}

bd_status BufferedBlockDevice::erase(bd_addr_t addr, bd_size_t size)
{
    if (!_is_initialized) {
        return bd_status::DEVICE_NOT_INITIALIZED;
    }

    const bd_status status = is_valid_erase(addr, size);
    if (status != bd_status::OK) {
        return status;
    }

    if ((_write_cache_addr >= addr) && (_write_cache_addr <= addr + size)) {
        invalidate_write_cache();
    }
    return _bd->erase(addr, size);
}

bd_size_t BufferedBlockDevice::get_read_size() const
{
    return 1;
}

bd_size_t BufferedBlockDevice::get_program_size() const
{
    return 1;
}

bd_size_t BufferedBlockDevice::get_erase_size() const
{
    if (!_is_initialized) {
        return 0;
    }

    return _bd->get_erase_size();
}

bd_size_t BufferedBlockDevice::get_erase_size(bd_addr_t addr) const
{
    if (!_is_initialized) {
        return 0;
    }

    return _bd->get_erase_size(addr);
}

int BufferedBlockDevice::get_erase_value() const
{
    if (!_is_initialized) {
        return static_cast<int>(bd_status::DEVICE_NOT_INITIALIZED);
    }

    return _bd->get_erase_value();
}

bd_size_t BufferedBlockDevice::size() const
{
    if (!_is_initialized) {
        return 0;
    }

    return _bd_size;
}

const char *BufferedBlockDevice::get_type() const
{
    return _bd->get_type();
}
