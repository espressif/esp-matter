/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DOUBLES_MOCKBLOCKDEVICE_H
#define DOUBLES_MOCKBLOCKDEVICE_H

#include <algorithm>
#include <iostream>
#include <vector>

#include "iotsdk/BlockDevice.h"

#include "gmock/gmock.h"

using namespace iotsdk::storage;

typedef struct {
    uint32_t magic;
    uint16_t header_size;
    uint16_t revision;
    uint32_t flags;
    uint16_t key_size;
    uint16_t reserved;
    uint32_t data_size;
    uint32_t crc;
} record_header_t;

class MockBlockDevice : public BlockDevice {
public:
    // give public access to underlying fake block device vector _fake_bd
    // this allows tests to corrupt the data as required
    std::vector<uint8_t> _fake_bd;

    MOCK_METHOD(bd_status, init, (), (override));
    MOCK_METHOD(bd_status, deinit, (), (override));
    MOCK_METHOD(bd_status, read, (void *buffer, bd_addr_t addr, bd_size_t size), (override));
    MOCK_METHOD(bd_status, program, (const void *buffer, bd_addr_t addr, bd_size_t size), (override));
    MOCK_METHOD(bd_status, erase, (bd_addr_t addr, bd_size_t size), (override));
    MOCK_METHOD(bd_size_t, get_read_size, (), (const, override));
    MOCK_METHOD(bd_size_t, get_program_size, (), (const, override));
    MOCK_METHOD(bd_size_t, get_erase_size, (), (const, override));
    MOCK_METHOD(bd_size_t, get_erase_size, (bd_addr_t addr), (const, override));
    MOCK_METHOD(int, get_erase_value, (), (const, override));
    MOCK_METHOD(bd_size_t, size, (), (const, override));
    MOCK_METHOD(const char *, get_type, (), (const, override));

    // initialise size of vector _fake_bd in constructor
    explicit MockBlockDevice(uint32_t device_size) : _fake_bd(device_size)
    {
    }

    // hook function calls and forward to our fake version
    void DelegateFakeProgram()
    {
        ON_CALL(*this, program).WillByDefault([this](const void *buffer, bd_addr_t addr, bd_size_t size) {
            return fake_program(buffer, addr, size);
        });
    }

    void DelegateFakeRead()
    {
        ON_CALL(*this, read).WillByDefault([this](void *buffer, bd_addr_t addr, bd_size_t size) {
            return fake_read(buffer, addr, size);
        });
    }

    void DelegateFakeErase()
    {
        ON_CALL(*this, erase).WillByDefault([this](bd_addr_t addr, bd_size_t size) { return fake_erase(addr, size); });
    }

    void print_memory(bool hex = false) const // useful when developing tests to see what's in _fake_bd
    {
        std::cout << "[";
        for (const auto &elem : _fake_bd) {
            if (hex) {
                std::cout << std::uppercase << std::hex << static_cast<int>(elem) << std::dec;
            } else {
                std::cout << elem;
            }
            std::cout << ","; // element separator char
        }
        std::cout << "]" << std::endl;
    }

private:
    bd_status fake_program(const void *buffer, bd_addr_t addr, bd_size_t size)
    {
        const bd_status err = is_valid_program(addr, size);
        if (err != bd_status::OK) {
            return err;
        }

        const uint8_t *u8_buf = static_cast<const uint8_t *>(buffer);
        for (bd_size_t i = 0; i < size; i++) {
            _fake_bd[addr + i] = u8_buf[i]; // manually copy each byte
        }

        return bd_status::OK;
    }

    bd_status fake_read(void *buffer, bd_addr_t addr, bd_size_t size) const
    {
        const bd_status err = is_valid_read(addr, size);
        if (err != bd_status::OK) {
            return err;
        }

        uint8_t *u8_buf = static_cast<uint8_t *>(buffer);
        for (bd_size_t i = 0; i < size; i++) {
            u8_buf[i] = _fake_bd[addr + i];
        }

        return bd_status::OK;
    }

    bd_status fake_erase(bd_addr_t addr, bd_size_t size)
    {
        const bd_status err = is_valid_erase(addr, size);
        if (err != bd_status::OK) {
            return err;
        }

        typename std::vector<uint8_t>::iterator start_erase = _fake_bd.begin() + addr;
        typename std::vector<uint8_t>::iterator end_erase = start_erase + size;
        std::fill(start_erase, end_erase, 0);

        return bd_status::OK;
    }
};

#endif // DOUBLES_MOCKBLOCKDEVICE_H
