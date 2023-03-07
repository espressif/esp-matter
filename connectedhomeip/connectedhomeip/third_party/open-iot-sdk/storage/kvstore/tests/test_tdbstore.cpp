/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "common/align.h"
#include "doubles/MockBlockDevice.h"
#include "iotsdk/BlockDevice.h"
#include "iotsdk/KVStore.h"
#include "iotsdk/TDBStore.h"

constexpr uint16_t BLOCK_SIZE = 256;
constexpr uint16_t DEVICE_SIZE = BLOCK_SIZE * 20;
constexpr uint16_t LARGE_DEVICE_SIZE = BLOCK_SIZE * 100;
constexpr uint16_t SMALL_DEVICE_SIZE = BLOCK_SIZE * 2;
constexpr uint8_t READ_SIZE = 1;
constexpr uint8_t ERASE_SIZE = 1;

using namespace iotsdk::storage;

using ::testing::_; // wildcard matcher
using ::testing::Gt;
using ::testing::NiceMock;
using ::testing::Return;

// basic, bare minimum test setup
class TDBStoreTestSetup : public testing::Test {
public:
    explicit TDBStoreTestSetup(uint32_t device_size = DEVICE_SIZE) : mock_bd{device_size}, tdb{&mock_bd}
    {
        ON_CALL(mock_bd, size()).WillByDefault(Return(device_size));
        ON_CALL(mock_bd, get_program_size()).WillByDefault(Return(BLOCK_SIZE));
        ON_CALL(mock_bd, get_read_size()).WillByDefault(Return(READ_SIZE));
        ON_CALL(mock_bd, get_erase_size()).WillByDefault(Return(ERASE_SIZE));
        ON_CALL(mock_bd, get_erase_size(_)).WillByDefault(Return(ERASE_SIZE));
        ON_CALL(mock_bd, get_erase_value()).WillByDefault(Return(-1));
    }

protected:
#ifdef GMOCK_WARNINGS
    MockBlockDevice mock_bd;
#else
    NiceMock<MockBlockDevice> mock_bd;
#endif
    TDBStore tdb;
};

class TDBStoreTestSetupSmall : public TDBStoreTestSetup {
public:
    TDBStoreTestSetupSmall() : TDBStoreTestSetup(SMALL_DEVICE_SIZE)
    {
    }
};

// for more involved tests, init()/deinit() + create/delegate a fake underlying block device
// that can be programmed/read/erased
class TDBStoreModuleTest : public TDBStoreTestSetup {
public:
    explicit TDBStoreModuleTest(uint32_t device_size = DEVICE_SIZE) : TDBStoreTestSetup{device_size}
    {
        mock_bd.DelegateFakeProgram(); // enable fake_program()
        mock_bd.DelegateFakeRead();    // enable fake_read()
        mock_bd.DelegateFakeErase();   // enable fake_erase()
    }

protected:
    void SetUp() override
    {
        EXPECT_CALL(mock_bd, init()).WillRepeatedly(Return(bd_status::OK));
        EXPECT_EQ(tdb.init(), kv_status::OK);
    }

    void TearDown() override
    {
        EXPECT_CALL(mock_bd, deinit()).WillRepeatedly(Return(bd_status::OK));
        EXPECT_EQ(tdb.deinit(), kv_status::OK);
    }

    // corrupt TDBStore header
    void corrupt_header(
        const char *key, int key_size, bd_size_t program_size, size_t member_offset, size_t member_size, uint32_t value)
    {
        // note: as this a linear search through memory for the given key, ensure
        // that this key does not exist in the data section of any items preceding
        // the entry we're trying to corrupt. otherwise we'll return an iterator
        // pointing at a data item, rather than the start of the key we want to
        // corrupt the header of
        auto it = std::search(mock_bd._fake_bd.begin(), mock_bd._fake_bd.end(), key, key + key_size);
        if (it != mock_bd._fake_bd.end()) {
            // go to start of header
            auto header_start = it - align_up(sizeof(record_header_t), program_size);
            for (size_t i = 0; i < member_size; i++) { // write 1 byte at a time
                *(header_start + member_offset + i) = value >> i * 8;
            }
        } else {
            std::cout << "corrupt_header: could not find key" << std::endl;
        }
    }
};

class TDBStoreModuleTestLarge : public TDBStoreModuleTest {
public:
    TDBStoreModuleTestLarge() : TDBStoreModuleTest(LARGE_DEVICE_SIZE)
    {
    }
};

/*
 * GMock allows us to create custom matchers for use with EXPECT_CALL/ON_CALL
 * to match specific function arguments.
 *
 * In TDBStore, there are some (const void *) arguments e.g.
 *    BlockDevice::program(const void *buffer ..)
 * that we want to match by comparing to some string data aka (const char *)
 *
 * GMock can't dereference a void* to compare, so we need a custom compare function
 * to be able to match these.
 */
MATCHER_P2(CompareVoidPtrToStr, expected, len, "")
{
    return strncmp(static_cast<const char *>(arg), expected, len) == 0;
}

// test init/deinit methods
TEST_F(TDBStoreTestSetup, init_deinit)
{
    EXPECT_CALL(mock_bd, init()).Times(1).WillOnce(Return(bd_status::OK));
    EXPECT_CALL(mock_bd, deinit()).Times(1).WillOnce(Return(bd_status::OK));
    EXPECT_EQ(tdb.init(), kv_status::OK);
    EXPECT_EQ(tdb.deinit(), kv_status::OK);
}

TEST_F(TDBStoreTestSetup, init_fail_if_device_errors)
{
    ON_CALL(mock_bd, init()).WillByDefault(Return(bd_status::DEVICE_ERROR));
    EXPECT_EQ(tdb.init(), kv_status::INIT_FAIL);
    EXPECT_EQ(tdb.deinit(), kv_status::NOT_INITIALIZED);
}

TEST_F(TDBStoreTestSetupSmall, init_fails_if_media_full)
{
    // use a tiny block device size so there's no room for the master record
    EXPECT_EQ(tdb.init(), kv_status::MEDIA_FULL);
    EXPECT_EQ(tdb.deinit(), kv_status::NOT_INITIALIZED);
}

TEST_F(TDBStoreTestSetup, init_deinit_twice)
{
    EXPECT_CALL(mock_bd, init()).Times(1).WillOnce(Return(bd_status::OK));
    EXPECT_CALL(mock_bd, deinit()).Times(1).WillOnce(Return(bd_status::OK));
    EXPECT_EQ(tdb.init(), kv_status::OK);
    EXPECT_EQ(tdb.init(), kv_status::OK);
    EXPECT_EQ(tdb.deinit(), kv_status::OK);
    EXPECT_EQ(tdb.deinit(), kv_status::NOT_INITIALIZED);
}

TEST_F(TDBStoreTestSetup, uninitialized_calls)
{
    EXPECT_EQ(tdb.deinit(), kv_status::NOT_INITIALIZED);
    EXPECT_EQ(tdb.reset(), kv_status::NOT_INITIALIZED);
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::NOT_INITIALIZED);
    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_start(&handle, "key", 5, 0), kv_status::NOT_INITIALIZED);
    char buf[10];
    size_t size;
    EXPECT_EQ(tdb.get("key", buf, 10, &size), kv_status::NOT_INITIALIZED);
    KVStore::info_t info;
    EXPECT_EQ(tdb.get_info("key", &info), kv_status::NOT_INITIALIZED);
    EXPECT_EQ(tdb.remove("key"), kv_status::NOT_INITIALIZED);
}

// test everything else
TEST_F(TDBStoreModuleTest, set)
{
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
}

TEST_F(TDBStoreModuleTest, set_invalid_chars_in_key)
{
    EXPECT_EQ(tdb.set("k<e>y", "data", 5, 0), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_empty_key)
{
    EXPECT_EQ(tdb.set("", "data", 5, 0), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_oversized_key)
{
    std::string oversize_key(KVStore::MAX_KEY_SIZE + 1, 'x');
    EXPECT_EQ(tdb.set(oversize_key.c_str(), "data", 5, 0), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_key_with_invalid_buffer)
{
    EXPECT_EQ(tdb.set("key", NULL, 5, 0), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_key_with_invalid_flag)
{
    EXPECT_EQ(tdb.set("key", "data", 5, (1 << 24)), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_key_with_unsupported_flag)
{
    EXPECT_EQ(tdb.set("key", "data", 5, KVStore::create_flags::RESERVED_FLAG), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_fails_if_kv_pair_size_gt_device_size)
{
    EXPECT_EQ(tdb.set("key", "data", DEVICE_SIZE, 0), kv_status::MEDIA_FULL);
}

TEST_F(TDBStoreModuleTest, set_fails_if_device_errors)
{
    ON_CALL(mock_bd, program(_, _, _)).WillByDefault(Return(bd_status::DEVICE_ERROR));
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::WRITE_FAILED);
    ON_CALL(mock_bd, program(_, _, _)).WillByDefault(Return(bd_status::OK));
}

// TDBStore follows the KVStore API, which specifies an incremental set process where
// items can be written in 3 stages:
//  1. key (set_start)
//  2. data (set_add_data)
/// 3. header (set_finalize)

TEST_F(TDBStoreModuleTest, set_incremental)
{
    const char key[] = "abcdefhijkl";
    const char data[] = "mnopqrstuv";

    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_start(&handle, key, sizeof(data), 0), kv_status::OK);
    EXPECT_EQ(tdb.set_add_data(handle, data, sizeof(data)), kv_status::OK);
    EXPECT_EQ(tdb.set_finalize(handle), kv_status::OK);

    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.get(key, buf, 100, &size), kv_status::OK);
    EXPECT_EQ(size, sizeof(data));
    EXPECT_STREQ(data, buf);
}

TEST_F(TDBStoreModuleTest, set_incremental_multiple_add_data)
{
    const char data[] = "abcd";
    const char data2[] = "wxyz";

    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_start(&handle, "key", sizeof(data) + sizeof(data2), 0), kv_status::OK);
    EXPECT_EQ(tdb.set_add_data(handle, data, sizeof(data)), kv_status::OK);
    EXPECT_EQ(tdb.set_add_data(handle, data2, sizeof(data2)), kv_status::OK);
    EXPECT_EQ(tdb.set_finalize(handle), kv_status::OK);
}

TEST_F(TDBStoreModuleTest, set_incremental_multiple_large_data_small_buffer)
{
    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_start(&handle, "key", 1024, 0), kv_status::OK);
    char buf[64];
    for (size_t i = 0; i < (1024 / 64); i++) {
        memset(buf, i + 'a', 64);
        EXPECT_EQ(tdb.set_add_data(handle, buf, 64), kv_status::OK);
    }
    EXPECT_EQ(tdb.set_finalize(handle), kv_status::OK);
}

TEST_F(TDBStoreModuleTest, set_incremental_fails_if_not_enough_data_added)
{
    const char data[] = "abcd";
    const char data2[] = "wxyz";

    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_start(&handle, "key", sizeof(data) + sizeof(data2), 0), kv_status::OK);
    EXPECT_EQ(tdb.set_add_data(handle, data, sizeof(data)), kv_status::OK);
    EXPECT_EQ(tdb.set_finalize(handle), kv_status::INVALID_SIZE);
}

TEST_F(TDBStoreModuleTest, set_incremental_fails_if_too_much_data_added)
{
    const char data[] = "abcd";

    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_start(&handle, "key", 2, 0), kv_status::OK);
    EXPECT_EQ(tdb.set_add_data(handle, data, 4), kv_status::INVALID_SIZE);
}

TEST_F(TDBStoreModuleTest, set_incremental_fails_if_no_data_added)
{
    const char data[] = "abcd";

    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_start(&handle, "key", sizeof(data), 0), kv_status::OK);
    EXPECT_EQ(tdb.set_add_data(handle, nullptr, sizeof(data)), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_incremental_fails_with_no_finalize)
{
    const char data[] = "abcdefhijkl";

    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_start(&handle, "key", sizeof(data), 0), kv_status::OK);
    EXPECT_EQ(tdb.set_add_data(handle, data, sizeof(data)), kv_status::OK);

    // try add a new item without finalizing
    KVStore::set_handle_t handle2;
    EXPECT_EQ(tdb.set_start(&handle2, "key2", sizeof(data), 0), kv_status::NOT_FINALIZED);

    // finalize and check we can start adding another item
    EXPECT_EQ(tdb.set_finalize(handle), kv_status::OK);
    EXPECT_EQ(tdb.set_start(&handle2, "key2", sizeof(data), 0), kv_status::OK);
}

TEST_F(TDBStoreModuleTest, set_incremental_add_data_fails_if_called_out_of_order)
{
    const char data[] = "abcd";
    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_add_data(handle, data, sizeof(data)), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_incremental_finalize_fails_if_called_out_of_order)
{
    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_finalize(handle), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_incremental_fails_if_data_size_is_too_large)
{
    KVStore::set_handle_t handle = nullptr;
    // maximum data size is UINT32_MAX -  0xFFFFFFFF
    EXPECT_EQ(tdb.set_start(&handle, "key", 0xFFFFFFFFF, 0), kv_status::INVALID_SIZE);
}

TEST_F(TDBStoreModuleTest, set_incremental_fails_with_write_once_flag_set)
{
    EXPECT_EQ(tdb.set("key", "data", 5, KVStore::create_flags::WRITE_ONCE_FLAG), kv_status::OK);
    KVStore::set_handle_t handle = nullptr;
    EXPECT_EQ(tdb.set_start(&handle, "key", 8, 0), kv_status::WRITE_PROTECTED);
    EXPECT_EQ(tdb.set_add_data(handle, "newdata", 8), kv_status::INVALID_ARGUMENT);
    EXPECT_EQ(tdb.set_finalize(handle), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, set_fails_if_device_errors_at_set_start)
{
    // Due to BufferedBlockDevice, we won't actually call program() until we've filled
    // up a buffer equal to get_program_size()
    // set program size to 1 so we always write to device + don't buffer
    ON_CALL(mock_bd, get_program_size()).WillByDefault(Return(1));
    // re-init using new program size
    EXPECT_EQ(tdb.deinit(), kv_status::OK);
    EXPECT_EQ(tdb.init(), kv_status::OK);

    // fail at key stage (set_start)
    const char key[] = "abcdefhijkl";
    // hook a program() call where the buffer to write equals the key + fail here
    ON_CALL(mock_bd, program(CompareVoidPtrToStr(key, strlen(key)), _, _))
        .WillByDefault(Return(bd_status::DEVICE_ERROR));
    EXPECT_EQ(tdb.set(key, "data", 5, 0), kv_status::WRITE_FAILED);
}

TEST_F(TDBStoreModuleTest, set_fails_if_device_errors_at_set_add_data)
{
    // Don't buffer writes
    ON_CALL(mock_bd, get_program_size()).WillByDefault(Return(1));
    EXPECT_EQ(tdb.deinit(), kv_status::OK);
    EXPECT_EQ(tdb.init(), kv_status::OK);

    // fail at data stage (set_add_data)
    const char data[] = "mnopqrstuv";
    ON_CALL(mock_bd, program(CompareVoidPtrToStr(data, sizeof(data)), _, _))
        .WillByDefault(Return(bd_status::DEVICE_ERROR));
    EXPECT_EQ(tdb.set("key", data, sizeof(data), 0), kv_status::WRITE_FAILED);

    // check we handle an aborted incremental set process correctly
    // we abandoned half way through at set_add_data()
    ON_CALL(mock_bd, program(CompareVoidPtrToStr(data, sizeof(data)), _, _)).WillByDefault(Return(bd_status::OK));
    EXPECT_EQ(tdb.set("key", data, sizeof(data), 0), kv_status::NOT_FINALIZED);
}

TEST_F(TDBStoreModuleTest, get)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::OK);
    EXPECT_EQ(size, 5);
    EXPECT_STREQ("data", buf);
}

TEST_F(TDBStoreModuleTest, get_multiple_blocks)
{
    const size_t size = 512;
    std::string set_buf(size, 'x');
    EXPECT_EQ(tdb.set("key", set_buf.c_str(), size, 0), kv_status::OK);
    char get_buf[size];
    size_t get_size;
    EXPECT_EQ(tdb.get("key", get_buf, size, &get_size), kv_status::OK);
    EXPECT_EQ(get_size, size);
}

TEST_F(TDBStoreModuleTest, get_fails_if_device_errors)
{
    char buf[5] = "xxxx";
    size_t size = 0;
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    ON_CALL(mock_bd, read(_, _, _)).WillByDefault(Return(bd_status::DEVICE_ERROR));
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::READ_FAILED);
    EXPECT_EQ(size, 0);
    EXPECT_STREQ("xxxx", buf);
}

TEST_F(TDBStoreModuleTest, get_fails_with_zero_size_key)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.get("", buf, 100, &size), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, get_fails_with_oversized_key)
{
    char buf[100];
    size_t size;
    std::string oversize_key(KVStore::MAX_KEY_SIZE + 1, 'x');
    EXPECT_EQ(tdb.get(oversize_key.c_str(), buf, 100, &size), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, get_offset)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "abcdefghijklmnopqrstuvxyz", 26, 0), kv_status::OK);
    EXPECT_EQ(tdb.get("key", buf, 100, &size, 13UL), kv_status::OK); // get from offset +13
    EXPECT_EQ(size, 13);
    EXPECT_STREQ("nopqrstuvxyz", buf);
}

TEST_F(TDBStoreModuleTest, get_offset_zero_partial_read)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "abcdefghijklmnopqrstuvxyz", 26, 0), kv_status::OK);
    EXPECT_EQ(tdb.get("key", buf, 13, &size, 0), kv_status::OK); // get from offset 0
    EXPECT_EQ(size, 13);
    buf[13] = '\0'; // NULL terminate partial string
    EXPECT_STREQ("abcdefghijklm", buf);
}

TEST_F(TDBStoreModuleTest, get_offset_fails_if_offset_is_gt_data_size)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "abcdefghijklmnopqrstuvxyz", 26, 0), kv_status::OK);
    EXPECT_EQ(tdb.get("key", buf, 100, &size, 30UL), kv_status::INVALID_SIZE); // get from offset +30 aka off end
}

TEST_F(TDBStoreModuleTest, get_fails_if_header_crc_corrupted)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);

    // corrupt data_size
    corrupt_header("key",
                   strlen("key"),
                   mock_bd.get_program_size(),
                   offsetof(record_header_t, crc),
                   sizeof(record_header_t::crc),
                   0x12341234);
    // mock_bd.print_memory(true);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::INVALID_DATA_DETECTED);
}

TEST_F(TDBStoreModuleTest, get_fails_if_header_key_size_corrupted)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);

    // corrupt key_size in header
    corrupt_header("key",
                   strlen("key"),
                   mock_bd.get_program_size(),
                   offsetof(record_header_t, key_size),
                   sizeof(record_header_t::key_size),
                   0x1234);
    // mock_bd.print_memory(true);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::INVALID_DATA_DETECTED);
}

TEST_F(TDBStoreModuleTest, get_fails_if_header_data_size_corrupted)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);

    // corrupt data_size
    corrupt_header("key",
                   strlen("key"),
                   mock_bd.get_program_size(),
                   offsetof(record_header_t, data_size),
                   sizeof(record_header_t::data_size),
                   0xFFFFFFFF);
    // mock_bd.print_memory(true);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::INVALID_DATA_DETECTED);
}

TEST_F(TDBStoreModuleTest, set_get_multi)
{
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.set("two", "here", 5, 0), kv_status::OK);

    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::OK);
    EXPECT_EQ(size, 5);
    EXPECT_STREQ("data", buf);

    char buf2[100];
    size_t size2;
    EXPECT_EQ(tdb.get("two", buf2, 100, &size2), kv_status::OK);
    EXPECT_EQ(size, 5);
    EXPECT_STREQ("here", buf2);
}

TEST_F(TDBStoreModuleTest, get_info)
{
    KVStore::info_t info;
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.get_info("key", &info), kv_status::OK);
    EXPECT_EQ(info.size, 5);
    EXPECT_FALSE(info.flags); // expect no flags to be set
}

TEST_F(TDBStoreModuleTest, get_info_fails_with_invalid_key)
{
    KVStore::info_t info;
    EXPECT_EQ(tdb.get_info("k<e>y", &info), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreModuleTest, write_once)
{
    KVStore::info_t info;
    EXPECT_EQ(tdb.set("key", "data", 5, KVStore::create_flags::WRITE_ONCE_FLAG), kv_status::OK);
    EXPECT_EQ(tdb.get_info("key", &info), kv_status::OK);
    // std::bitset<32> b(info.flags); std::cout << "info.flags=" << b << std::endl; // DEBUG
    EXPECT_TRUE(info.flags & KVStore::create_flags::WRITE_ONCE_FLAG);
    EXPECT_EQ(tdb.set("key", "twice", 5, 0), kv_status::WRITE_PROTECTED);
}

TEST_F(TDBStoreModuleTest, reset)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.reset(), kv_status::OK);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::ITEM_NOT_FOUND);
}

TEST_F(TDBStoreModuleTestLarge, remove_kv_pair)
{
    char buf[100];
    size_t size;
    // add items around "key" to test the RAM table is appropriately restructured
    // when an item in the middle is removed
    EXPECT_EQ(tdb.set("key1", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.set("key3", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.remove("key"), kv_status::OK);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::ITEM_NOT_FOUND);
}

TEST_F(TDBStoreModuleTest, erase)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    // erase underlying block device
    EXPECT_EQ(mock_bd.erase(0, mock_bd.size()), bd_status::OK);
    // since tdb.remove() wasn't called, RAM table will still contain entry,
    // & data will appear corrupted when we read
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::INVALID_DATA_DETECTED);
}

TEST_F(TDBStoreModuleTest, reset_fails_if_bd_erase_errors)
{
    char buf[100];
    size_t size;
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    // fail to erase underlying block device for some reason
    ON_CALL(mock_bd, erase(_, _)).WillByDefault(Return(bd_status::DEVICE_ERROR));
    // we call bd->erase on tdbstore->reset
    EXPECT_EQ(tdb.reset(), kv_status::WRITE_FAILED);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::OK);
}

TEST_F(TDBStoreModuleTest, rebuild_tdbstore_where_tdbstore_already_exists_on_storage)
{
    // if existing TDBStore data exists on underlying storage, init() can detect this and
    // recover/rebuild the RAM table with build_ram_table()
    size_t size;
    char buf[100] = {0};
    // write some value to storage then shut down the TDBStore
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.deinit(), kv_status::OK);
    // when we init() again, build_ram_table() should re-populate with the data on storage
    EXPECT_EQ(tdb.init(), kv_status::OK);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::OK);
    EXPECT_EQ(size, 5);
    EXPECT_STREQ("data", buf);
}

TEST_F(TDBStoreModuleTest, rebuild_tdbstore_respects_kvpair_removal)
{
    size_t size;
    char buf[100] = {0};
    // write some value to storage then shut down the TDBStore
    EXPECT_EQ(tdb.set("key", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.deinit(), kv_status::OK);
    // when we init() again, build_ram_table() should re-populate with the data on storage
    EXPECT_EQ(tdb.init(), kv_status::OK);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::OK);
    EXPECT_EQ(size, 5);
    EXPECT_STREQ("data", buf);
    // implementation detail:
    // set the delete flag - remove doesn't actually delete from storage, just marks with a flag
    EXPECT_EQ(tdb.remove("key"), kv_status::OK);
    // check that item wasn't recovered
    EXPECT_EQ(tdb.deinit(), kv_status::OK);
    EXPECT_EQ(tdb.init(), kv_status::OK);
    EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::ITEM_NOT_FOUND);
}

TEST_F(TDBStoreModuleTest, rebuild_ram_table_multiple_areas)
{
    size_t size;
    // fill up the active area so the standby area becomes active
    // this means there's two areas to choose from when build_ram_table()is called
    for (int i = 0; i < 5; ++i) {
        char str[11] = {0};
        char buf[100] = {0};
        int len = snprintf(str, 11, "data%d", i) + 1;
        // write some value to storage then shut down the TDBStore
        EXPECT_EQ(tdb.set("key", str, len, 0), kv_status::OK);
        EXPECT_EQ(tdb.deinit(), kv_status::OK);
        // when we init() again, build_ram_table() should re-populate with the data on storage
        EXPECT_EQ(tdb.init(), kv_status::OK);
        EXPECT_EQ(tdb.get("key", buf, 100, &size), kv_status::OK);
        EXPECT_EQ(size, len);
        EXPECT_STREQ(str, buf);
        // set delete flag on "key" so it's not added next iteration
        EXPECT_EQ(tdb.remove("key"), kv_status::OK);
    }
}

TEST_F(TDBStoreModuleTestLarge, rebuild_ram_table_gt_max_keys)
{
    // test rebuilding RAM table when there's more than _max_keys on storage
    size_t size;
    char set_key[10];
    char buf[10] = {0};
    int len;
    for (int i = 0; i < 17; i++) {               // initial_max_keys currently set to 16
        len = snprintf(set_key, 10, "key%d", i); // create some unique keys
        EXPECT_EQ(tdb.set(set_key, "data", 5, 0), kv_status::OK);
    }
    EXPECT_EQ(tdb.deinit(), kv_status::OK);
    EXPECT_EQ(tdb.init(), kv_status::OK);

    char get_key[10];
    for (int i = 0; i < 17; i++) {
        snprintf(get_key, 10, "key%d", i);
        EXPECT_EQ(tdb.get(get_key, buf, 10, &size), kv_status::OK);
        EXPECT_EQ(size, len);
        EXPECT_STREQ("data", buf);
        EXPECT_EQ(tdb.remove(get_key), kv_status::OK);
    }
}

TEST_F(TDBStoreModuleTest, iterate_keys)
{
    char buf[100];
    KVStore::iterator_t iterator;
    EXPECT_EQ(tdb.set("prefix_key", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.set("prefix_second_key", "value", 6, 0), kv_status::OK);
    EXPECT_EQ(tdb.iterator_open(&iterator, "prefix"), kv_status::OK);
    EXPECT_EQ(tdb.iterator_next(iterator, buf, 100), kv_status::OK);
    EXPECT_STREQ("prefix_key", buf);
    EXPECT_EQ(tdb.iterator_next(iterator, buf, 100), kv_status::OK);
    EXPECT_STREQ("prefix_second_key", buf);
    EXPECT_EQ(tdb.iterator_next(iterator, buf, 100), kv_status::ITEM_NOT_FOUND);
    EXPECT_EQ(tdb.iterator_close(iterator), kv_status::OK);
}

TEST_F(TDBStoreModuleTestLarge, update_iterators)
{
    // items are stored in the _ram_table in descending hash order.
    // therefore when we add a new item, it may be added anywhere in the
    // table depending on the calculated hash. as iterations are done
    // linearaly/sequentially through the _ram_table, it's an implementation
    // detail as to what will be returned when calling iterator_next() after
    // adding an item
    // therefore we expect a return of either kv_status::OK || kv_status::ITEM_NOT_FOUND
    char buf[25];
    kv_status result;
    KVStore::iterator_t iterator;
    EXPECT_EQ(tdb.set("prefix_key", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.set("prefix_second_key", "value", 6, 0), kv_status::OK);

    EXPECT_EQ(tdb.iterator_open(&iterator, "prefix"), kv_status::OK);

    // iterate over the first two keys starting with "prefix"
    EXPECT_EQ(tdb.iterator_next(iterator, buf, 25), kv_status::OK);
    EXPECT_STREQ("prefix_key", buf);

    EXPECT_EQ(tdb.iterator_next(iterator, buf, 25), kv_status::OK);
    EXPECT_STREQ("prefix_second_key", buf);

    // non-existent entry
    EXPECT_EQ(tdb.iterator_next(iterator, buf, 25), kv_status::ITEM_NOT_FOUND);

    // add one more key with the same prefix
    EXPECT_EQ(tdb.set("prefix_third_key", "value", 6, 0), kv_status::OK);
    result = tdb.iterator_next(iterator, buf, 25);
    // we can't guarantee where it's placed in the _ram_table so unknown what
    // iterator_next will return
    EXPECT_TRUE(result == kv_status::OK || result == kv_status::ITEM_NOT_FOUND);

    // add then remove a key
    EXPECT_EQ(tdb.set("prefix_fourth_key", "value", 6, 0), kv_status::OK);
    EXPECT_EQ(tdb.remove("prefix_third_key"), kv_status::OK);
    result = tdb.iterator_next(iterator, buf, 25);
    EXPECT_TRUE(result == kv_status::OK || result == kv_status::ITEM_NOT_FOUND);

    // add two keys at once
    EXPECT_EQ(tdb.set("prefix_fifth_key", "value", 6, 0), kv_status::OK);
    EXPECT_EQ(tdb.set("prefix_sixth_key", "value", 6, 0), kv_status::OK);
    result = tdb.iterator_next(iterator, buf, 25);
    EXPECT_TRUE(result == kv_status::OK || result == kv_status::ITEM_NOT_FOUND);

    EXPECT_EQ(tdb.iterator_close(iterator), kv_status::OK);
}

TEST_F(TDBStoreModuleTest, max_iterators)
{
    KVStore::iterator_t iterator[17]; // _max_open_iterators = 16, TDBStore.h
    for (int i = 0; i < 17; i++) {
        if (i >= 16) {
            EXPECT_EQ(tdb.iterator_open(&iterator[i], "prefix"), kv_status::OUT_OF_RESOURCES);
        } else {
            EXPECT_EQ(tdb.iterator_open(&iterator[i], "prefix"), kv_status::OK);
        }
    }
    for (int i = 0; i < 16; i++) { // free memory
        EXPECT_EQ(tdb.iterator_close(iterator[i]), kv_status::OK);
    }
}

TEST_F(TDBStoreModuleTest, no_prefix_iterator)
{
    char buf[100];
    KVStore::iterator_t iterator;
    EXPECT_EQ(tdb.set("abcd", "data", 5, 0), kv_status::OK);
    EXPECT_EQ(tdb.set("efgh", "value", 6, 0), kv_status::OK);
    EXPECT_EQ(tdb.iterator_open(&iterator, ""), kv_status::OK);
    EXPECT_EQ(tdb.iterator_next(iterator, buf, 100), kv_status::OK);
    EXPECT_STREQ("efgh", buf);
    EXPECT_EQ(tdb.iterator_next(iterator, buf, 100), kv_status::OK);
    EXPECT_STREQ("abcd", buf);
    EXPECT_EQ(tdb.iterator_next(iterator, buf, 100), kv_status::ITEM_NOT_FOUND);
    EXPECT_EQ(tdb.iterator_close(iterator), kv_status::OK);
}

TEST_F(TDBStoreModuleTest, nullptr_iterator_fails)
{
    EXPECT_EQ(tdb.iterator_open(nullptr, "prefix"), kv_status::INVALID_ARGUMENT);
}

TEST_F(TDBStoreTestSetup, uninitialized_iterator_calls)
{
    KVStore::iterator_t iterator;
    EXPECT_EQ(tdb.iterator_open(&iterator, "prefix"), kv_status::NOT_INITIALIZED);
    EXPECT_EQ(tdb.iterator_close(iterator), kv_status::NOT_INITIALIZED);
    EXPECT_EQ(tdb.iterator_next(iterator, nullptr, 1), kv_status::NOT_INITIALIZED);
    EXPECT_EQ(tdb.iterator_open(nullptr, "prefix"), kv_status::NOT_INITIALIZED);
}

TEST_F(TDBStoreModuleTest, reserved_data_set_get)
{
    char reserved_key[] = "value";
    char buf[64];
    size_t size;
    EXPECT_EQ(tdb.reserved_data_set(reserved_key, 6), kv_status::OK);
    EXPECT_EQ(tdb.reserved_data_get(buf, 64, &size), kv_status::OK);
    EXPECT_STREQ("value", buf);
    EXPECT_EQ(size, 6);
    EXPECT_EQ(tdb.reserved_data_set(reserved_key, 6), kv_status::WRITE_FAILED);
}

TEST_F(TDBStoreModuleTest, reserved_data_set_fails_if_buffer_gt_u16)
{
    char reserved_key[] = "value";
    EXPECT_EQ(tdb.reserved_data_set(reserved_key, 128), kv_status::INVALID_SIZE);
    EXPECT_EQ(tdb.reserved_data_set(reserved_key, UINT16_MAX + 16), kv_status::INVALID_SIZE);
}

TEST_F(TDBStoreTestSetup, reserved_data_set_get_fails_when_uninitialized)
{
    char buf[6];
    size_t size;
    EXPECT_EQ(tdb.reserved_data_set("value", 6), kv_status::NOT_INITIALIZED);
    EXPECT_EQ(tdb.reserved_data_get(buf, 6, &size), kv_status::NOT_INITIALIZED);
}

TEST_F(TDBStoreModuleTestLarge, increment_max_keys)
{
    char key[10];
    for (int i = 0; i < 17; i++) {     // initial_max_keys currently set to 16
        snprintf(key, 10, "key%d", i); // create some unique keys
        EXPECT_EQ(tdb.set(key, "data", 5, 0), kv_status::OK);
    }
}
