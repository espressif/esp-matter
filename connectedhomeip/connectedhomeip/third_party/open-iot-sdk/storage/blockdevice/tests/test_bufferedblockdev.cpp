/*
 * Copyright (c) 2019-2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "doubles/MockBlockDevice.h"
#include "iotsdk/BlockDevice.h"
#include "iotsdk/BufferedBlockDevice.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cstdint>
#include <string.h>
#include <utility>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

constexpr uint16_t BLOCK_SIZE = 512;
constexpr uint16_t DEVICE_SIZE = BLOCK_SIZE * 4;
constexpr uint16_t PROGRAM_SIZE = BLOCK_SIZE;
constexpr uint16_t READ_SIZE = BLOCK_SIZE;
constexpr uint16_t ERASE_SIZE = BLOCK_SIZE;

// basic, bare minimum test setup
class BufferedBlockSetup : public testing::Test {
public:
    explicit BufferedBlockSetup(uint32_t device_size = DEVICE_SIZE) : bd_mock{device_size}, bd{&bd_mock}
    {
        ON_CALL(bd_mock, size()).WillByDefault(Return(device_size));
        ON_CALL(bd_mock, get_program_size()).WillByDefault(Return(PROGRAM_SIZE));
        ON_CALL(bd_mock, get_read_size()).WillByDefault(Return(READ_SIZE));
        ON_CALL(bd_mock, get_erase_size()).WillByDefault(Return(ERASE_SIZE));
        ON_CALL(bd_mock, get_erase_size(_)).WillByDefault(Return(ERASE_SIZE));
        ON_CALL(bd_mock, get_erase_value()).WillByDefault(Return(-1));
        ON_CALL(bd_mock, get_type()).WillByDefault(Return("MOCK"));
    }

protected:
#ifdef GMOCK_WARNINGS
    MockBlockDevice bd_mock;
#else
    NiceMock<MockBlockDevice> bd_mock;
#endif
    BufferedBlockDevice bd;
};

// for more involved tests, create test buffers, init() + create/delegate a fake underlying block device
class BufferedBlockTest : public BufferedBlockSetup {
public:
    explicit BufferedBlockTest(uint32_t device_size = DEVICE_SIZE)
        : buffer_size{device_size}, magic{nullptr}, buf{nullptr}, BufferedBlockSetup(device_size)
    {
        bd_mock.DelegateFakeProgram(); // enable fake_program()
        bd_mock.DelegateFakeRead();    // enable fake_read()
        bd_mock.DelegateFakeErase();   // enable fake_erase()

        magic = new uint8_t[buffer_size];
        buf = new uint8_t[buffer_size];
        // Generate simple pattern to verify against
        for (int i = 0; i < buffer_size; i++) {
            magic[i] = 0xaa + i;
            buf[i] = 0;
        }
    }

    ~BufferedBlockTest()
    {
        delete[] magic;
        delete[] buf;
    }

private:
    const uint32_t buffer_size;

protected:
    uint8_t *magic;
    uint8_t *buf;

    void SetUp() override
    {
        EXPECT_CALL(bd_mock, init());
        ASSERT_EQ(bd.init(), bd_status::OK);
    }

    void TearDown() override
    {
        EXPECT_CALL(bd_mock, deinit());
        ASSERT_EQ(bd.deinit(), bd_status::OK);
    }
};

// gMock does not allow calling non-default constructors from test fixtures
// create a class that consists of only a single block to use when the test doesn't use much memory
class BufferedBlockTest_SingleBlock : public BufferedBlockTest {
public:
    explicit BufferedBlockTest_SingleBlock() : BufferedBlockTest(BLOCK_SIZE)
    {
    }
};

// test passing a nullptr in constructor
class BufferedBlockTest_NullPtr : public testing::Test {
protected:
    MockBlockDevice *bd_mock = nullptr;
    BufferedBlockDevice bd{bd_mock};
};

TEST_F(BufferedBlockTest_NullPtr, nullptr)
{
    ASSERT_EQ(bd.init(), bd_status::DEVICE_ERROR);
}

// test init()/deinit() in isolation
TEST_F(BufferedBlockSetup, init_deinit)
{
    EXPECT_CALL(bd_mock, init()).Times(1);
    ASSERT_EQ(bd.init(), bd_status::OK);
    EXPECT_CALL(bd_mock, deinit()).Times(1);
    ASSERT_EQ(bd.deinit(), bd_status::OK);
}

TEST_F(BufferedBlockSetup, double_init_deinit)
{
    EXPECT_CALL(bd_mock, init()).Times(1);
    ASSERT_EQ(bd.init(), bd_status::OK);
    ASSERT_EQ(bd.init(), bd_status::OK);
    EXPECT_CALL(bd_mock, deinit()).Times(1);
    ASSERT_EQ(bd.deinit(), bd_status::OK);
    ASSERT_EQ(bd.deinit(), bd_status::OK);
}

TEST_F(BufferedBlockSetup, init_fail)
{
    EXPECT_CALL(bd_mock, init()).Times(1);
    ON_CALL(bd_mock, init()).WillByDefault(Return(bd_status::DEVICE_ERROR));
    ASSERT_EQ(bd.init(), bd_status::DEVICE_ERROR);
}

TEST_F(BufferedBlockSetup, deinit_fail_sync)
{
    // on deinit() BufferedBlockDevice should sync() to write out cached items to storage
    // on failure, ensure error is return to avoid potential data loss
    EXPECT_CALL(bd_mock, init()).Times(1);
    ASSERT_EQ(bd.init(), bd_status::OK);

    // in cache as size < PROGRAM_SIZE
    EXPECT_EQ(bd.program("abcdef", 0, 6), bd_status::OK);
    // cause sync() to fail
    ON_CALL(bd_mock, program(_, _, _)).WillByDefault(Return(bd_status::DEVICE_ERROR));

    ASSERT_EQ(bd.deinit(), bd_status::DEVICE_ERROR);
}

TEST_F(BufferedBlockSetup, uninitialized_calls)
{
    uint8_t buf[10] = {0};
    EXPECT_EQ(bd.get_type(), "MOCK");
    EXPECT_EQ(bd.get_read_size(), 1);
    EXPECT_EQ(bd.get_program_size(), 1);
    EXPECT_EQ(bd.get_erase_size(), 0);
    EXPECT_EQ(bd.get_erase_size(0), 0);
    EXPECT_EQ(bd.get_erase_value(), static_cast<int>(bd_status::DEVICE_NOT_INITIALIZED));
    EXPECT_EQ(bd.size(), 0);
    EXPECT_EQ(bd.program(buf, 0, BLOCK_SIZE), bd_status::DEVICE_NOT_INITIALIZED);
    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE), bd_status::DEVICE_NOT_INITIALIZED);
    EXPECT_EQ(bd.deinit(), bd_status::OK);
    EXPECT_EQ(bd.sync(), bd_status::DEVICE_NOT_INITIALIZED);
    EXPECT_EQ(bd.erase(0, BLOCK_SIZE), bd_status::DEVICE_NOT_INITIALIZED);
}

TEST_F(BufferedBlockTest, program_read_block)
{
    EXPECT_EQ(bd.program(magic, 0, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ(0, memcmp(magic, buf, BLOCK_SIZE));
}

TEST_F(BufferedBlockTest, program_read_multiple_blocks)
{
    EXPECT_EQ(bd.program(magic, 0, DEVICE_SIZE), bd_status::OK);
    EXPECT_EQ(bd.read(buf, 0, DEVICE_SIZE), bd_status::OK);
    EXPECT_EQ(0, memcmp(magic, buf, DEVICE_SIZE));
}

TEST_F(BufferedBlockTest_SingleBlock, invalid_read)
{
    EXPECT_EQ(bd.program(magic, 0, BLOCK_SIZE), bd_status::OK);
    // out of bounds
    EXPECT_EQ(bd.read(buf, BLOCK_SIZE, BLOCK_SIZE), bd_status::INCORRECT_SIZE);
}

TEST_F(BufferedBlockTest_SingleBlock, unaligned_program)
{
    /*
        block 1    block 2
      ┌──────────┬─┬────────┐
      │on device │a│ ------ │
      └──────────┘▲└────────┘
                  │
                 in cache
     */

    // will be written to device as size >= PROGRAM_SIZE
    EXPECT_EQ(bd.program(magic, 0, BLOCK_SIZE), bd_status::OK);
    // will remain in cache as size < PROGRAM_SIZE
    EXPECT_EQ(bd.program("a", 0, 1), bd_status::OK);

    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ('a', buf[0]);
    EXPECT_EQ(0, memcmp(buf + 1, magic + 1, BLOCK_SIZE - 1));
}

TEST_F(BufferedBlockTest, unaligned_reads)
{
    /*

         block 1   block 2     block 3
      ┌──────────┬─────┬─────┬──────────┐
      │on device │cache│ --- │ -------- │
      └──────────▲─────┴─────┴──────────┘
                 │
              _write_cache_addr
    */
    EXPECT_EQ(bd.program(magic, 0, BLOCK_SIZE + 256), bd_status::OK);

    // 1 aligned read (full block), 1 unaligned read (half block)
    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE + 256), bd_status::OK);
    EXPECT_EQ(0, memcmp(magic, buf, BLOCK_SIZE));
    EXPECT_EQ(0, memcmp(magic + BLOCK_SIZE, buf + BLOCK_SIZE, BLOCK_SIZE - 256));

    memset(buf, 0, DEVICE_SIZE);
    // 2 unaligned reads, spanning across 2 blocks
    EXPECT_EQ(bd.read(buf, 256, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ(0, memcmp(magic + 256, buf, BLOCK_SIZE));

    memset(buf, 0, DEVICE_SIZE);
    // read from the third block
    EXPECT_EQ(bd.read(buf, BLOCK_SIZE * 2, 256), bd_status::OK);
}

TEST_F(BufferedBlockTest, failed_program)
{
    ON_CALL(bd_mock, program(_, _, _)).WillByDefault(Return(bd_status::DEVICE_ERROR));
    EXPECT_EQ(bd.program(magic, 0, BLOCK_SIZE), bd_status::DEVICE_ERROR);
}

TEST_F(BufferedBlockTest, failed_read)
{
    EXPECT_EQ(bd.program(magic, 0, BLOCK_SIZE), bd_status::OK);
    ON_CALL(bd_mock, read(_, _, _)).WillByDefault(Return(bd_status::DEVICE_ERROR));
    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE), bd_status::DEVICE_ERROR);
}

TEST_F(BufferedBlockTest, failed_sync_before_program)
{
    // since we can only buffer one block, we have to sync/flush to device if we're
    // partially writing to another block - any failed sync's need to be correctly
    // caught to avoid data loss

    EXPECT_EQ(bd.program(magic, 0, 256), bd_status::OK);                               // partial write block 1
    ON_CALL(bd_mock, program(_, 0, _)).WillByDefault(Return(bd_status::DEVICE_ERROR)); // fail on sync()
    EXPECT_EQ(bd.program(magic, BLOCK_SIZE, 256), bd_status::DEVICE_ERROR); // partial write block 2 triggers sync()
    bd_mock.DelegateFakeProgram();                                          // sync() will now work
    EXPECT_EQ(bd.program(magic, BLOCK_SIZE, 256), bd_status::OK);           // retry

    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE * 2), bd_status::OK);
    EXPECT_EQ(0, memcmp(buf, magic, 256));
    EXPECT_EQ(0, memcmp(buf + BLOCK_SIZE, magic + BLOCK_SIZE, 256));
}

TEST_F(BufferedBlockTest_SingleBlock, erase)
{
    uint8_t zero_buf[BLOCK_SIZE] = {0};

    EXPECT_EQ(bd.get_erase_size(), ERASE_SIZE);
    EXPECT_EQ(bd.get_erase_size(0), ERASE_SIZE);

    EXPECT_EQ(bd.program(magic, 0, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ(0, memcmp(magic, buf, BLOCK_SIZE));

    EXPECT_EQ(bd.get_erase_value(), -1);

    EXPECT_EQ(bd.erase(0, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ(0, memcmp(zero_buf, buf, BLOCK_SIZE));
}

TEST_F(BufferedBlockTest, erase_unaligned)
{
    // unaligned/partial erase is not supported
    EXPECT_EQ(bd.erase(BLOCK_SIZE / 2, BLOCK_SIZE), bd_status::ADDRESS_NOT_ALIGNED);
    EXPECT_EQ(bd.erase(0, BLOCK_SIZE / 2), bd_status::SIZE_NOT_ALIGNED);
}

TEST_F(BufferedBlockTest, erase_incorrect_size)
{
    // attempt to erase past the device limits
    EXPECT_EQ(bd.erase(DEVICE_SIZE - BLOCK_SIZE, BLOCK_SIZE * 2), bd_status::INCORRECT_SIZE);
    EXPECT_EQ(bd.erase(0, DEVICE_SIZE + BLOCK_SIZE), bd_status::INCORRECT_SIZE);
}

TEST_F(BufferedBlockTest_SingleBlock, program_small_chunks)
{
    for (int i = 0; i < BLOCK_SIZE - 1; ++i) {
        EXPECT_EQ(bd.program(magic + i, i, 1), bd_status::OK);
    }
    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ(0, memcmp(buf, magic, BLOCK_SIZE - 1));

    // write cache will be flushed on deinit.
    EXPECT_CALL(bd_mock, program(_, 0, BLOCK_SIZE)).Times(1).WillOnce(Return(bd_status::OK));
}

TEST_F(BufferedBlockTest_SingleBlock, sync)
{
    // will be written to device
    EXPECT_EQ(bd.program(magic, 0, BLOCK_SIZE), bd_status::OK);
    // will remain in cache as size < PROGRAM_SIZE
    EXPECT_EQ(bd.program("a", 0, 1), bd_status::OK);

    // sync failure
    ON_CALL(bd_mock, program(_, _, _)).WillByDefault(Return(bd_status::DEVICE_ERROR));
    EXPECT_EQ(bd.sync(), bd_status::DEVICE_ERROR);

    // retry
    bd_mock.DelegateFakeProgram();
    EXPECT_EQ(bd.sync(), bd_status::OK);

    EXPECT_EQ(bd.read(buf, 0, BLOCK_SIZE), bd_status::OK);
    EXPECT_EQ('a', buf[0]);
    EXPECT_EQ(0, memcmp(buf + 1, magic + 1, BLOCK_SIZE - 1));
}
