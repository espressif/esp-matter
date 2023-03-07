/**
 *    Copyright 2012-2016 Nest Labs Inc. All Rights Reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      This file implements a unit test suite for the C language
 *      binding of the Nest Labs byte ordering functions and macros.
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include <nlbyteorder.h>

#include <nlunit-test.h>

#include "nlbyteorder-test.h"

static void CheckPreprocessorDefinitions(nlTestSuite *inSuite, void *inContext)
{
    NL_TEST_ASSERT(inSuite, NLBYTEORDER_LITTLE_ENDIAN  == 0x1234);
    NL_TEST_ASSERT(inSuite, NLBYTEORDER_BIG_ENDIAN     == 0x4321);
    NL_TEST_ASSERT(inSuite, NLBYTEORDER_UNKNOWN_ENDIAN == 0xFFFF);
    NL_TEST_ASSERT(inSuite, NLBYTEORDER != NLBYTEORDER_UNKNOWN_ENDIAN);
}

static void CheckEnumerations(nlTestSuite *inSuite, void *inContext)
{
    NL_TEST_ASSERT(inSuite, nlByteOrderUnknown      == NLBYTEORDER_UNKNOWN_ENDIAN);
    NL_TEST_ASSERT(inSuite, nlByteOrderLittleEndian == NLBYTEORDER_LITTLE_ENDIAN);
    NL_TEST_ASSERT(inSuite, nlByteOrderBigEndian    == NLBYTEORDER_BIG_ENDIAN);
}

static void CheckByteOrder(nlTestSuite *inSuite, void *inContext)
{
    // Check that we get a meaningful value for the current byte order.

    NL_TEST_ASSERT(inSuite, nlByteOrderGetCurrent() != nlByteOrderUnknown);
}

static void CheckConstantMacros(nlTestSuite *inSuite, void *inContext)
{
    const uint16_t v16 = nlByteOrderConstantSwap16(MAGIC16);
    const uint32_t v32 = nlByteOrderConstantSwap32(MAGIC32);
    const uint64_t v64 = nlByteOrderConstantSwap64(MAGIC64);

    // Check that the constant swap macros work.

    NL_TEST_ASSERT(inSuite, v16 == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_SWAP64);
}

static void CheckValueInlines(nlTestSuite *inSuite, void *inContext)
{
    // Check that the constant swap inline free functions work.

    NL_TEST_ASSERT(inSuite, nlByteOrderValueSwap16(MAGIC16) == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, nlByteOrderValueSwap32(MAGIC32) == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, nlByteOrderValueSwap64(MAGIC64) == MAGIC_SWAP64);
}

static void CheckInPlaceInlines(nlTestSuite *inSuite, void *inContext)
{
    uint16_t v16 = MAGIC16;
    uint32_t v32 = MAGIC32;
    uint64_t v64 = MAGIC64;

    nlByteOrderPointerSwap16(&v16);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_SWAP16);

    nlByteOrderPointerSwap32(&v32);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_SWAP32);

    nlByteOrderPointerSwap64(&v64);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_SWAP64);
}

static void CheckSwapByValue(nlTestSuite *inSuite, void *inContext)
{
    uint16_t v16_in = MAGIC16;
    uint32_t v32_in = MAGIC32;
    uint64_t v64_in = MAGIC64;
    uint16_t v16_out;
    uint32_t v32_out;
    uint64_t v64_out;

    // Check little-to-host and host-to-little swap by value.

    v16_out = nlByteOrderSwap16LittleToHost(v16_in);
    v32_out = nlByteOrderSwap32LittleToHost(v32_in);
    v64_out = nlByteOrderSwap64LittleToHost(v64_in);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v16_out == v16_in);
    NL_TEST_ASSERT(inSuite, v32_out == v32_in);
    NL_TEST_ASSERT(inSuite, v64_out == v64_in);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#endif

    v16_in = nlByteOrderSwap16HostToLittle(v16_out);
    v32_in = nlByteOrderSwap32HostToLittle(v32_out);
    v64_in = nlByteOrderSwap64HostToLittle(v64_out);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v16_out == v16_in);
    NL_TEST_ASSERT(inSuite, v32_out == v32_in);
    NL_TEST_ASSERT(inSuite, v64_out == v64_in);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v16_in == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_in == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_in == MAGIC64);

#endif

    // Check big-to-host and host-to-big swap by value.

    v16_out = nlByteOrderSwap16BigToHost(v16_in);
    v32_out = nlByteOrderSwap32BigToHost(v32_in);
    v64_out = nlByteOrderSwap64BigToHost(v64_in);

#if NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v16_out == v16_in);
    NL_TEST_ASSERT(inSuite, v32_out == v32_in);
    NL_TEST_ASSERT(inSuite, v64_out == v64_in);

#elif NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#endif

    v16_in = nlByteOrderSwap16HostToBig(v16_out);
    v32_in = nlByteOrderSwap32HostToBig(v32_out);
    v64_in = nlByteOrderSwap64HostToBig(v64_out);

#if NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v16_out == v16_in);
    NL_TEST_ASSERT(inSuite, v32_out == v32_in);
    NL_TEST_ASSERT(inSuite, v64_out == v64_in);

#elif NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v16_in == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_in == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_in == MAGIC64);

#endif
}

static const nlTest sTests[] = {
    NL_TEST_DEF("preprocessor definitions", CheckPreprocessorDefinitions),
    NL_TEST_DEF("enumerations",             CheckEnumerations),
    NL_TEST_DEF("byte order",               CheckByteOrder),
    NL_TEST_DEF("constant macros",          CheckConstantMacros),
    NL_TEST_DEF("value inlines",            CheckValueInlines),
    NL_TEST_DEF("in-place inlines",         CheckInPlaceInlines),
    NL_TEST_DEF("swap-by-value",            CheckSwapByValue),
    NL_TEST_SENTINEL()
};

int main(void)
{
    nlTestSuite theSuite = {
        "nlbyteorder",
        &sTests[0]
    };

    nl_test_set_output_style(OUTPUT_CSV);

    nlTestRunner(&theSuite, NULL);

    return nlTestRunnerStats(&theSuite);
}
