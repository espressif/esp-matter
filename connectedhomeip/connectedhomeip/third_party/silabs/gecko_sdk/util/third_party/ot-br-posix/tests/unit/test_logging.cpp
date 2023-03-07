/*
 *    Copyright (c) 2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

#define OTBR_LOG_TAG "TEST"

#include <CppUTest/TestHarness.h>

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "common/logging.hpp"

TEST_GROUP(Logging){};

TEST(Logging, TestLoggingHigherLevel)
{
    char ident[20];

    sprintf(ident, "otbr-test-%ld", clock());
    otbrLogInit(ident, OTBR_LOG_INFO, true);
    otbrLog(OTBR_LOG_DEBUG, OTBR_LOG_TAG, "cool-higher");
    otbrLogDeinit();
    sleep(0);

    char cmd[128];
    sprintf(cmd, "grep '%s.*cool-higher' /var/log/syslog", ident);
    CHECK(0 != system(cmd));
}

TEST(Logging, TestLoggingEqualLevel)
{
    char ident[20];

    sprintf(ident, "otbr-test-%ld", clock());
    otbrLogInit(ident, OTBR_LOG_INFO, true);
    otbrLog(OTBR_LOG_INFO, OTBR_LOG_TAG, "cool-equal");
    otbrLogDeinit();
    sleep(0);

    char cmd[128];
    sprintf(cmd, "grep '%s.*cool-equal' /var/log/syslog", ident);
    printf("CMD = %s\n", cmd);
    CHECK(0 == system(cmd));
}

TEST(Logging, TestLoggingLowerLevel)
{
    char ident[20];
    char cmd[128];

    sprintf(ident, "otbr-test-%ld", clock());
    otbrLogInit(ident, OTBR_LOG_INFO, true);
    otbrLog(OTBR_LOG_WARNING, OTBR_LOG_TAG, "cool-lower");
    otbrLogDeinit();
    sleep(0);

    sprintf(cmd, "grep '%s.*cool-lower' /var/log/syslog", ident);
    CHECK(0 == system(cmd));
}

TEST(Logging, TestLoggingDump)
{
    char ident[32];
    char cmd[128];

    sprintf(ident, "otbr-test-%ld", clock());
    otbrLogInit(ident, OTBR_LOG_DEBUG, true);
    const char s[] = "one super long string with lots of text";
    otbrDump(OTBR_LOG_INFO, "Test", "foobar", s, sizeof(s));
    otbrLogDeinit();
    sleep(0);

    /*
     * Above produces output like this
     * otbr-test-5976[47088]: foobar: 0000: 6f 6e 65 20 73 75 70 65 72 20 6c 6f 6e 67 20 73
     * otbr-test-5976[47088]: foobar: 0010: 74 72 69 6e 67 20 77 69 74 68 20 6c 6f 74 73 20
     * otbr-test-5976[47088]: foobar: 0020: 6f 66 20 74 65 78 74 00
     */

    sprintf(cmd, "grep '%s.*: foobar: 0000: 6f 6e 65 20 73 75 70 65 72 20 6c 6f 6e 67 20 73' /var/log/syslog", ident);
    CHECK(0 == system(cmd));

    sprintf(cmd, "grep '%s.*: foobar: 0010: 74 72 69 6e 67 20 77 69 74 68 20 6c 6f 74 73 20' /var/log/syslog", ident);
    CHECK(0 == system(cmd));

    sprintf(cmd, "grep '%s.*: foobar: 0020: 6f 66 20 74 65 78 74 00' /var/log/syslog", ident);
    CHECK(0 == system(cmd));
}
