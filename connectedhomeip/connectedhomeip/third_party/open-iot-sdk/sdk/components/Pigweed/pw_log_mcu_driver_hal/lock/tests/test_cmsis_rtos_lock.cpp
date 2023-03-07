/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fff.h"
#include "gtest/gtest.h"
#include "util.h"
#include "cmsis_os2.h"

DEFINE_FFF_GLOBALS;

class TestCmsisRtosLock : public ::testing::Test {
public:
    TestCmsisRtosLock()
    {
        RESET_FAKE(osMutexNew);
        RESET_FAKE(osMutexAcquire);
        RESET_FAKE(osMutexRelease);
    }
};

class TestCmsisRtosLockInitializedLock : public TestCmsisRtosLock {
public:
    TestCmsisRtosLockInitializedLock() : TestCmsisRtosLock()
    {
        initialize_lock();
    }
    const uint32_t my_lock = 1;
    const osMutexId_t lock = (osMutexId_t)&my_lock;

private:
    void initialize_lock(void)
    {

        osMutexNew_fake.return_val = lock;
        _pw_log_init_lock();
    }
};

TEST_F(TestCmsisRtosLock, initializing_lock_creates_os_mutex)
{
    _pw_log_init_lock();
    EXPECT_GT(osMutexNew_fake.call_count, 0);
}

TEST_F(TestCmsisRtosLock, acquiring_log_lock_acquires_os_mutex)
{
    _pw_log_lock();
    EXPECT_GT(osMutexAcquire_fake.call_count, 0);
}

TEST_F(TestCmsisRtosLock, unlocking_log_lock_releases_os_mutex)
{
    _pw_log_unlock();
    EXPECT_GT(osMutexRelease_fake.call_count, 0);
}

TEST_F(TestCmsisRtosLockInitializedLock, acquiring_log_lock_uses_os_mutex)
{
    _pw_log_lock();
    EXPECT_EQ(osMutexAcquire_fake.arg0_val, TestCmsisRtosLockInitializedLock::lock);
}

TEST_F(TestCmsisRtosLockInitializedLock, unlocking_log_lock_uses_os_mutex)
{
    _pw_log_unlock();
    EXPECT_EQ(osMutexRelease_fake.arg0_val, TestCmsisRtosLockInitializedLock::lock);
}
