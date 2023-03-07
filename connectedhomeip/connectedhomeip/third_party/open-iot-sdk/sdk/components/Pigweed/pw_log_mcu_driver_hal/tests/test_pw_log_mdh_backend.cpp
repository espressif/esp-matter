/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fff.h"
#include "hal/serial_api.h"
#include "pw_log_mdh/util.h"
#include "gtest/gtest.h"
#include "pw_log_mdh/backend.h"
#include <iostream>

DEFINE_FFF_GLOBALS;

class TestPwLogMdhBackend : public ::testing::Test {
public:
    TestPwLogMdhBackend()
    {
        RESET_FAKE(_pw_log_init_lock);
        RESET_FAKE(_pw_log_lock);
        RESET_FAKE(_pw_log_unlock);
        RESET_FAKE(mdh_serial_put_data);
    }
};

TEST_F(TestPwLogMdhBackend, initializing_log_also_initializes_the_lock)
{
    mdh_serial_t serial = 0;
    pw_log_mdh_init(&serial);

    EXPECT_GT(_pw_log_init_lock_fake.call_count, 0);
}

TEST_F(TestPwLogMdhBackend, outputting_log_aquires_lock)
{
    mdh_serial_t serial = 0;
    pw_log_mdh_init(&serial);

    char message[] = "";
    _pw_log_mdh(message);
    EXPECT_GT(_pw_log_lock_fake.call_count, 0);
}

TEST_F(TestPwLogMdhBackend, outputting_log_releases_lock)
{
    mdh_serial_t serial = 0;
    pw_log_mdh_init(&serial);

    char message[] = "";
    _pw_log_mdh(message);
    EXPECT_GT(_pw_log_unlock_fake.call_count, 0);
}

TEST_F(TestPwLogMdhBackend, _pw_log_mdh_outputs_message_correctly)
{
    mdh_serial_t serial = 0;
    pw_log_mdh_init(&serial);

    char message[] = "Output Message";
    _pw_log_mdh(message);

    for (size_t index = 0; index < strlen(message); index++) {
        EXPECT_EQ(message[index], mdh_serial_put_data_fake.arg1_history[index]);
    }
}

TEST_F(TestPwLogMdhBackend, _pw_log_mdh_with_empty_string_does_not_output_message)
{
    mdh_serial_t serial = 0;
    pw_log_mdh_init(&serial);

    char message[] = "";
    _pw_log_mdh(message);
    EXPECT_EQ(mdh_serial_put_data_fake.call_count, 0);
}
