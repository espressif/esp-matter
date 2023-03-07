/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/BlockDevice.h"
#include "iotsdk/FlashIAPBlockDevice.h"

#include "hal/flash_api.h"

#include "fff.h"
#include "gtest/gtest.h"

#include <array>
#include <stdint.h>

DEFINE_FFF_GLOBALS

FAKE_VALUE_FUNC2(int32_t, flash_erase_sector, mdh_flash_t *, uint32_t);
FAKE_VALUE_FUNC4(int32_t, flash_read, mdh_flash_t *, uint32_t, uint8_t *, uint32_t);
FAKE_VALUE_FUNC4(int32_t, flash_program_page, mdh_flash_t *, uint32_t, const uint8_t *, uint32_t);
FAKE_VALUE_FUNC2(uint32_t, flash_get_sector_size, const mdh_flash_t *, uint32_t);
FAKE_VALUE_FUNC1(uint32_t, flash_get_page_size, const mdh_flash_t *);
FAKE_VALUE_FUNC1(uint32_t, flash_get_start_address, const mdh_flash_t *);
FAKE_VALUE_FUNC1(uint32_t, flash_get_size, const mdh_flash_t *);
FAKE_VALUE_FUNC1(uint8_t, flash_get_erase_value, const mdh_flash_t *);

static const mdh_flash_vtable_t gsc_vtable = {.erase_sector = flash_erase_sector,
                                              .read = flash_read,
                                              .program_page = flash_program_page,
                                              .get_sector_size = flash_get_sector_size,
                                              .get_page_size = flash_get_page_size,
                                              .get_start_address = flash_get_start_address,
                                              .get_size = flash_get_size,
                                              .get_erase_value = flash_get_erase_value};

static mdh_flash_t flash_dev = {.vfptr = &gsc_vtable};

class TestFlashIAPBlockDevice : public ::testing::Test {
public:
    TestFlashIAPBlockDevice()
    {
        RESET_FAKE(flash_erase_sector);
        RESET_FAKE(flash_read);
        RESET_FAKE(flash_program_page);
        RESET_FAKE(flash_get_sector_size);
        RESET_FAKE(flash_get_page_size);
        RESET_FAKE(flash_get_start_address);
        RESET_FAKE(flash_get_size);
        RESET_FAKE(flash_get_erase_value);
    }
};

TEST_F(TestFlashIAPBlockDevice, get_type_returns_bd_type_string)
{
    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, 0x10, 0x10};

    EXPECT_STREQ(block_dev.get_type(), "FLASHIAP");
}

TEST_F(TestFlashIAPBlockDevice, init_succeeds_with_valid_base_and_size)
{
    flash_get_start_address_fake.return_val = 0x10;
    flash_get_size_fake.return_val = 0x20;

    const uint32_t size{0x10};
    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, 0x10, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
}

TEST_F(TestFlashIAPBlockDevice, init_gets_size_from_flash_dev_if_zero)
{
    flash_get_start_address_fake.return_val = 0x10;
    flash_get_size_fake.return_val = 0x20;

    const uint32_t size{0x0};
    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, 0x10, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.size(), flash_get_size_fake.return_val);
}

TEST_F(TestFlashIAPBlockDevice, init_fails_when_device_pointer_null)
{
    const uint32_t size{100};
    iotsdk::storage::FlashIAPBlockDevice block_dev{nullptr, 0, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::DEVICE_ERROR);
}

TEST_F(TestFlashIAPBlockDevice, init_fails_when_size_exceeded)
{
    flash_get_start_address_fake.return_val = 0x0;
    flash_get_size_fake.return_val = 0x10;

    const uint32_t size{0x100};
    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, 0x0, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::INCORRECT_SIZE);
}

TEST_F(TestFlashIAPBlockDevice, init_fails_when_base_addr_less_than_start_addr)
{
    flash_get_start_address_fake.return_val = 0x10;
    flash_get_size_fake.return_val = 0x20;

    const uint32_t size{0x10};
    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, 0x01, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::INCORRECT_ADDRESS);
}

TEST_F(TestFlashIAPBlockDevice, deinit_deinits_flash_device)
{
    flash_get_start_address_fake.return_val = 0x10;
    flash_get_size_fake.return_val = 0x20;

    const uint32_t size{0x10};
    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, 0x10, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.deinit(), iotsdk::storage::bd_status::OK);
}

TEST_F(TestFlashIAPBlockDevice, deinit_returns_success_when_device_not_inited)
{
    const uint32_t size{0x10};
    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, 0x10, size};

    EXPECT_EQ(block_dev.deinit(), iotsdk::storage::bd_status::OK);
}

TEST_F(TestFlashIAPBlockDevice, read_fills_io_buffer_with_data)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_read_fake.custom_fake = [](mdh_flash_t *obj, uint32_t addr, uint8_t *buf, uint32_t size) -> int32_t {
        buf[0] = 'A';
        buf[1] = 'B';
        return flash_read_fake.return_val;
    };
    std::array<char, 2> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.read(buffer.data(), 0, sizeof(buffer.data())), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(buffer[0], 'A');
    EXPECT_EQ(buffer[1], 'B');
}

TEST_F(TestFlashIAPBlockDevice, read_fails_if_device_not_inited)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};

    std::array<char, 2> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.read(buffer.data(), 0, sizeof(buffer.data())),
              iotsdk::storage::bd_status::DEVICE_NOT_INITIALIZED);
    EXPECT_EQ(flash_read_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, read_fails_if_address_overflows)
{
    const uint32_t start_addr{0xFFFF};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;

    std::array<char, 2> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.read(buffer.data(), 0xFFFFFFFF, sizeof(buffer.data())),
              iotsdk::storage::bd_status::INCORRECT_ADDRESS);
    EXPECT_EQ(flash_read_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, read_fails_if_size_overflows)
{
    const uint32_t start_addr{0x20};
    const uint32_t size{0xFFFF};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;

    std::array<char, 2> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.read(buffer.data(), 0x0, 0xFFFFFFFFF), iotsdk::storage::bd_status::INCORRECT_SIZE);
    EXPECT_EQ(flash_read_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, read_fails_if_size_out_of_bounds)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;

    std::array<char, 2> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.read(buffer.data(), start_addr, sizeof(buffer.data()) * 100),
              iotsdk::storage::bd_status::INCORRECT_SIZE);
    EXPECT_EQ(flash_read_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, read_fails_if_device_errors)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_read_fake.return_val = -1;

    std::array<char, 2> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.read(buffer.data(), start_addr, sizeof(buffer.data())),
              iotsdk::storage::bd_status::DEVICE_ERROR);
}

TEST_F(TestFlashIAPBlockDevice, get_read_size_returns_read_size_in_bytes)
{
    const uint32_t size{0x10};
    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, 0x10, size};

    EXPECT_EQ(block_dev.get_read_size(), 1);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_read_succeeds_if_size_valid)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_read(start_addr, size), iotsdk::storage::bd_status::OK);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_read_fails_if_size_out_of_bounds)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_read(start_addr, 100), iotsdk::storage::bd_status::INCORRECT_SIZE);
}

TEST_F(TestFlashIAPBlockDevice, erase_single_sector)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x1FFF};
    const uint32_t sector_size = 0xFFF;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = sector_size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(start_addr, sector_size), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(flash_erase_sector_fake.call_count, 1);
    EXPECT_EQ(flash_erase_sector_fake.arg1_val, start_addr);
}

TEST_F(TestFlashIAPBlockDevice, erase_multiple_sectors)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{8192};
    const uint32_t sector_size = 4096;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = sector_size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(start_addr, size), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(flash_erase_sector_fake.call_count, 2);
    EXPECT_EQ(flash_erase_sector_fake.arg1_history[0], start_addr);
    EXPECT_EQ(flash_erase_sector_fake.arg1_history[1], start_addr + sector_size);
}

TEST_F(TestFlashIAPBlockDevice, erase_single_sector_from_offset)
{
    const uint32_t start_addr{0x10};
    const uint32_t size{8192};
    const uint32_t sector_size = 4096;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = sector_size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(sector_size, sector_size), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(flash_erase_sector_fake.call_count, 1);
    EXPECT_EQ(flash_erase_sector_fake.arg1_val, start_addr + sector_size);
}

TEST_F(TestFlashIAPBlockDevice, erase_fails_if_device_not_inited)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.erase(start_addr, size), iotsdk::storage::bd_status::DEVICE_NOT_INITIALIZED);
    EXPECT_EQ(flash_erase_sector_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, erase_fails_if_address_overflows)
{
    const uint32_t start_addr{0x10};
    const uint32_t size{8192};
    const uint32_t sector_size = 4096;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = sector_size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(0xFFFFFFFF, sector_size), iotsdk::storage::bd_status::INCORRECT_ADDRESS);
    EXPECT_EQ(flash_erase_sector_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, erase_fails_if_size_overflows)
{
    const uint32_t start_addr{0x10};
    const uint32_t size{8192};
    const uint32_t sector_size = 4096;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = sector_size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(start_addr, 0xFFFFFFFFF), iotsdk::storage::bd_status::INCORRECT_SIZE);
    EXPECT_EQ(flash_erase_sector_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, erase_fails_if_address_not_aligned)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(start_addr + 1, size - 1), iotsdk::storage::bd_status::ADDRESS_NOT_ALIGNED);
    EXPECT_EQ(flash_erase_sector_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, erase_fails_if_size_not_aligned)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(start_addr, size - 1), iotsdk::storage::bd_status::SIZE_NOT_ALIGNED);
    EXPECT_EQ(flash_erase_sector_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, erase_fails_if_block_out_of_bounds)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(start_addr + 1, size), iotsdk::storage::bd_status::INCORRECT_SIZE);
    EXPECT_EQ(flash_erase_sector_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, erase_fails_if_device_erase_errors)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = size;
    flash_get_size_fake.return_val = size;
    flash_erase_sector_fake.return_val = -1;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(start_addr, size), iotsdk::storage::bd_status::DEVICE_ERROR);
}

TEST_F(TestFlashIAPBlockDevice, erase_fails_if_sector_size_is_0)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_sector_size_fake.return_val = 0;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.erase(start_addr, size), iotsdk::storage::bd_status::DEVICE_ERROR);
}

TEST_F(TestFlashIAPBlockDevice, get_erase_value_returns_value_from_hal)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_erase_value_fake.return_val = 0xFF;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.get_erase_value(), flash_get_erase_value_fake.return_val);
}

TEST_F(TestFlashIAPBlockDevice, get_erase_value_returns_negative_1_if_device_not_inited)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.get_erase_value(), -1);
}

TEST_F(TestFlashIAPBlockDevice, get_erase_size_returns_value_from_hal)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x2000};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_sector_size_fake.return_val = 0x1000;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.get_erase_size(start_addr), flash_get_sector_size_fake.return_val);
}

TEST_F(TestFlashIAPBlockDevice, get_erase_size_returns_0_if_no_address_given)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x2000};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_sector_size_fake.return_val = 0x1000;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.get_erase_size(), 0);
}

TEST_F(TestFlashIAPBlockDevice, get_erase_size_returns_0_if_device_not_inited)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.get_erase_size(20), 0);
}

TEST_F(TestFlashIAPBlockDevice, get_erase_size_returns_0_if_address_overflows)
{
    const uint32_t start_addr{0x1};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.get_erase_size(0xFFFFFFFF), 0);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_erase_succeeds_if_address_and_size_valid)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x1FFF};
    const uint32_t sector_size = 0xFFF;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = sector_size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_erase(start_addr, sector_size), iotsdk::storage::bd_status::OK);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_erase_fails_if_address_not_aligned)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_erase(start_addr + 1, size - 1), iotsdk::storage::bd_status::ADDRESS_NOT_ALIGNED);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_erase_fails_if_size_not_aligned)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_erase(start_addr, size - 1), iotsdk::storage::bd_status::SIZE_NOT_ALIGNED);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_erase_fails_if_block_out_of_bounds)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_sector_size_fake.return_val = size;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_erase(start_addr + 1, size), iotsdk::storage::bd_status::INCORRECT_SIZE);
}

TEST_F(TestFlashIAPBlockDevice, program_single_page)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{8192};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 256;
    flash_get_sector_size_fake.return_val = 4096;
    std::array<char, 256> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), start_addr, buffer.max_size()), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(flash_program_page_fake.call_count, 1);
    EXPECT_EQ(flash_program_page_fake.arg1_val, start_addr);
    EXPECT_EQ(flash_program_page_fake.arg3_val, buffer.max_size());
}

TEST_F(TestFlashIAPBlockDevice, program_multiple_pages_same_sector)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{8192};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 256;
    flash_get_sector_size_fake.return_val = 4096;
    std::array<char, 512> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), start_addr, buffer.max_size()), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(flash_program_page_fake.call_count, 1);
    EXPECT_EQ(flash_program_page_fake.arg1_val, start_addr);
    EXPECT_EQ(flash_program_page_fake.arg3_val, buffer.max_size());
}

TEST_F(TestFlashIAPBlockDevice, program_multiple_pages_crossing_sector_boundary)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{8192};
    const uint32_t page_size{256};
    const uint32_t sector_size{4096};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = page_size;
    flash_get_sector_size_fake.return_val = sector_size;
    std::array<char, 3 * page_size> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), start_addr + sector_size - page_size, buffer.max_size()),
              iotsdk::storage::bd_status::OK);
    EXPECT_EQ(flash_program_page_fake.call_count, 2);
    EXPECT_EQ(flash_program_page_fake.arg1_history[0], start_addr + sector_size - page_size);
    EXPECT_EQ(flash_program_page_fake.arg3_history[0], page_size);
    EXPECT_EQ(flash_program_page_fake.arg1_history[1], start_addr + sector_size);
    EXPECT_EQ(flash_program_page_fake.arg3_history[1], 2 * page_size);
}

TEST_F(TestFlashIAPBlockDevice, program_fails_if_device_not_inited)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    std::array<char, 3> buffer{'h', 'e', 'y'};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.program(buffer.data(), start_addr, size), iotsdk::storage::bd_status::DEVICE_NOT_INITIALIZED);
    EXPECT_EQ(flash_program_page_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, program_fails_if_address_overflows)
{
    const uint32_t start_addr{0x10};
    const uint32_t size{8192};
    flash_get_page_size_fake.return_val = 256;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    std::array<char, 512> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), 0xFFFFFFFF, buffer.max_size()),
              iotsdk::storage::bd_status::INCORRECT_ADDRESS);
    EXPECT_EQ(flash_program_page_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, program_fails_if_size_overflows)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{8192};
    flash_get_page_size_fake.return_val = 256;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    std::array<char, 512> buffer;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), start_addr, 0xFFFFFFFFF), iotsdk::storage::bd_status::INCORRECT_SIZE);
    EXPECT_EQ(flash_program_page_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, program_fails_if_size_out_of_bounds)
{
    const uint32_t start_addr{0x10};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 256;

    std::array<char, 3> buffer{'h', 'e', 'y'};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), start_addr, size * 2), iotsdk::storage::bd_status::INCORRECT_SIZE);
    EXPECT_EQ(flash_program_page_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, program_fails_if_size_not_aligned)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 0x10;
    std::array<char, 3> buffer{'h', 'e', 'y'};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), start_addr, size - 3), iotsdk::storage::bd_status::SIZE_NOT_ALIGNED);
    EXPECT_EQ(flash_program_page_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, program_fails_if_address_not_aligned)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 0x10;
    std::array<char, 3> buffer{'h', 'e', 'y'};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), start_addr + 3, 0x10), iotsdk::storage::bd_status::ADDRESS_NOT_ALIGNED);
    EXPECT_EQ(flash_program_page_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, program_fails_if_device_write_errors)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 0x10;
    flash_get_sector_size_fake.return_val = 0x20;
    flash_program_page_fake.return_val = -1;
    std::array<char, 4> buffer{'h', 'e', 'y', 'a'};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), start_addr, 0x10), iotsdk::storage::bd_status::DEVICE_ERROR);
    EXPECT_EQ(flash_program_page_fake.call_count, 1);
}

TEST_F(TestFlashIAPBlockDevice, program_fails_if_device_program_size_returns_0)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 0;
    std::array<char, 4> buffer{'h', 'e', 'y', 'a'};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.program(buffer.data(), start_addr, 0x10), iotsdk::storage::bd_status::DEVICE_ERROR);
    EXPECT_EQ(flash_program_page_fake.call_count, 0);
}

TEST_F(TestFlashIAPBlockDevice, get_program_size_returns_page_size_from_hal)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 0x10;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.get_program_size(), flash_get_page_size_fake.return_val);
}

TEST_F(TestFlashIAPBlockDevice, get_program_size_returns_0_if_device_not_inited)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.get_program_size(), 0);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_program_succeeds_if_address_and_size_valid)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{8192};
    flash_get_page_size_fake.return_val = 256;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_program(start_addr, size), iotsdk::storage::bd_status::OK);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_program_fails_if_out_of_bounds)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_page_size_fake.return_val = 256;
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_program(start_addr, size * 2), iotsdk::storage::bd_status::INCORRECT_SIZE);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_program_fails_if_size_not_aligned)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 0x10;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_program(start_addr, size - 3), iotsdk::storage::bd_status::SIZE_NOT_ALIGNED);
}

TEST_F(TestFlashIAPBlockDevice, is_valid_program_fails_if_address_not_aligned)
{
    const uint32_t start_addr{0x0};
    const uint32_t size{0x20};
    flash_get_start_address_fake.return_val = start_addr;
    flash_get_size_fake.return_val = size;
    flash_get_page_size_fake.return_val = 0x10;

    iotsdk::storage::FlashIAPBlockDevice block_dev{&flash_dev, start_addr, size};

    EXPECT_EQ(block_dev.init(), iotsdk::storage::bd_status::OK);
    EXPECT_EQ(block_dev.is_valid_program(start_addr + 3, 0x10), iotsdk::storage::bd_status::ADDRESS_NOT_ALIGNED);
}
