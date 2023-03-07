/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "fff.h"
#include "iot_ntp_client.h"

#include "gtest/gtest.h"

DEFINE_FFF_GLOBALS

class TestNtptimeOpeniot : public ::testing::Test {
public:
    TestNtptimeOpeniot()
    {
        RESET_FAKE(iotNtpClientGetTime);
    }
};

TEST_F(TestNtptimeOpeniot, request_time_when_ntp_client_not_initialized)
{
    time_t fake_time = 0xDEADBEEF;
    iotNtpClientGetTime_fake.return_val = fake_time;
    iotNtpClientGetState_fake.return_val = IOT_NTP_CLIENT_UNINITIALISED;
    time_t result = time(nullptr);
    EXPECT_EQ(result, 0);
}

TEST_F(TestNtptimeOpeniot, request_time_when_ntp_client_stopped)
{
    time_t fake_time = 0xDEADBEEF;
    iotNtpClientGetTime_fake.return_val = fake_time;
    iotNtpClientGetState_fake.return_val = IOT_NTP_CLIENT_STOPPED;
    time_t result = time(nullptr);
    EXPECT_EQ(result, 0);
}

TEST_F(TestNtptimeOpeniot, request_time_with_null_dst_returns_time)
{
    time_t fake_time = 0xDEADBEEF;
    iotNtpClientGetTime_fake.return_val = fake_time;
    iotNtpClientGetState_fake.return_val = IOT_NTP_CLIENT_RUNNING;
    time_t result = time(nullptr);
    EXPECT_EQ(result, fake_time);
}

TEST_F(TestNtptimeOpeniot, request_time_with_valid_dst_returns_time_in_dst)
{
    time_t fake_time = 0xDEADBEEF;
    iotNtpClientGetTime_fake.return_val = fake_time;
    iotNtpClientGetState_fake.return_val = IOT_NTP_CLIENT_RUNNING;
    time_t dst = 0xABCDEF;
    (void)time(&dst);
    EXPECT_EQ(dst, fake_time);
}

TEST_F(TestNtptimeOpeniot, request_time_with_valid_dst_returns_time)
{
    time_t fake_time = 0xDEADBEEF;
    iotNtpClientGetTime_fake.return_val = fake_time;
    iotNtpClientGetState_fake.return_val = IOT_NTP_CLIENT_RUNNING;
    time_t dst = 0xABCDEF;
    time_t result = time(&dst);
    EXPECT_EQ(result, fake_time);
}
