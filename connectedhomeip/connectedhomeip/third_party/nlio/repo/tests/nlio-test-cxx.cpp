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
 *      binding of the Nest Labs memory-mapped I/O functions and
 *      macros.
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include <nlio.hpp>

#include <nlunit-test.h>

#include "nlio-test.h"

using namespace nl::IO;

static void CheckIntrospectionExplicitSize(nlTestSuite *inSuite, void *inContext)
{
    bool aligned;
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    _uint128_t v128;

    // Check for natural alignment of an 8-bit stack variable.

    aligned = IsAligned(&v8, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 16-bit stack variable, which
    // should also be aligned on 8-bit as well.

    aligned = IsAligned(&v16, sizeof(v16));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v16, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 32-bit stack variable, which
    // should also be aligned on 16- and 8-bit as well.

    aligned = IsAligned(&v32, sizeof(v32));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v32, sizeof(v16));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v32, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 64-bit stack variable, which
    // should also be aligned on 32-, 16-, and 8-bit as well.

    aligned = IsAligned(&v64, sizeof(v64));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v64, sizeof(v32));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v64, sizeof(v16));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v64, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 128-bit stack variable, which
    // should also be aligned on 64-, 32-, 16-, and 8-bit as well.

    aligned = IsAligned(&v128, sizeof(v128));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v128, sizeof(v64));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v128, sizeof(v32));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v128, sizeof(v16));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = IsAligned(&v128, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);
}

static void CheckIntrospectionTemplate(nlTestSuite *inSuite, void *inContext)
{
    bool aligned;
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    _uint128_t v128;

    // Check for natural alignment of an 8-bit stack variable.

    aligned = IsAligned(&v8);
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 16-bit stack variable.

    aligned = IsAligned(&v16);
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 32-bit stack variable.

    aligned = IsAligned(&v32);
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 64-bit stack variable.

    aligned = IsAligned(&v64);
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 128-bit stack variable.

    aligned = IsAligned(&v128);
    NL_TEST_ASSERT(inSuite, aligned == true);
}

static void CheckIntrospection(nlTestSuite *inSuite, void *inContext)
{
    CheckIntrospectionExplicitSize(inSuite, inContext);
    CheckIntrospectionTemplate(inSuite, inContext);
}
      
static void CheckAlignedGet(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t s8 = MAGIC_8;
    const uint16_t s16 = MAGIC_16;
    const uint32_t s32 = MAGIC_32;
    const uint64_t s64 = MAGIC_64;
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;

    v8 = GetAligned8(&s8);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);

    v16 = GetAligned16(&s16);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    v32 = GetAligned32(&s32);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    v64 = GetAligned64(&s64);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
}
         
static void CheckAlignedPut(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t v8 = MAGIC_8;
    const uint16_t v16 = MAGIC_16;
    const uint32_t v32 = MAGIC_32;
    const uint64_t v64 = MAGIC_64;
    uint8_t s8;
    uint16_t s16;
    uint32_t s32;
    uint64_t s64;

    PutAligned8(&s8, v8);
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);

    PutAligned16(&s16, v16);
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    PutAligned32(&s32, v32);
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    PutAligned64(&s64, v64);
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
}
         
static void CheckUnalignedGet(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t s8 = MAGIC_8;
    const uint16_t s16 = MAGIC_16;
    const uint32_t s32 = MAGIC_32;
    const uint64_t s64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    memcpy(p, &s8, sizeof(s8));

    v8 = GetUnaligned8(p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(p, &s16, sizeof(s16));

    v16 = GetUnaligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    p = &buffer[1];
    memcpy(p, &s16, sizeof(s16));

    v16 = GetUnaligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(p, &s32, sizeof(s32));

    v32 = GetUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[1];
    memcpy(p, &s32, sizeof(s32));

    v32 = GetUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[2];
    memcpy(p, &s32, sizeof(s32));

    v32 = GetUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[3];
    memcpy(p, &s32, sizeof(s32));

    v32 = GetUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[1];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[2];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[3];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[4];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[5];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[6];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[7];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
}
       
static void CheckUnalignedPut(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t v8 = MAGIC_8;
    const uint16_t v16 = MAGIC_16;
    const uint32_t v32 = MAGIC_32;
    const uint64_t v64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t s8;
    uint16_t s16;
    uint32_t s32;
    uint64_t s64;
    void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    PutUnaligned8(p, v8);
    memcpy(&s8, p, sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    PutUnaligned16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    p = &buffer[1];
    PutUnaligned16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    PutUnaligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[1];
    PutUnaligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[2];
    PutUnaligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[3];
    PutUnaligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    PutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[1];
    PutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[2];
    PutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[3];
    PutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[4];
    PutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[5];
    PutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[6];
    PutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[7];
    PutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
}
       
static void CheckMaybeAlignedGet(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t s8 = MAGIC_8;
    const uint16_t s16 = MAGIC_16;
    const uint32_t s32 = MAGIC_32;
    const uint64_t s64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    v8 = GetMaybeAligned8(&s8);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(p, &s16, sizeof(s16));

    v16 = GetMaybeAligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    p = &buffer[1];
    memcpy(p, &s16, sizeof(s16));

    v16 = GetMaybeAligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(p, &s32, sizeof(s32));

    v32 = GetMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[1];
    memcpy(p, &s32, sizeof(s32));

    v32 = GetMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[2];
    memcpy(p, &s32, sizeof(s32));

    v32 = GetMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[3];
    memcpy(p, &s32, sizeof(s32));

    v32 = GetMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[1];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[2];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[3];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[4];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[5];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[6];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[7];
    memcpy(p, &s64, sizeof(s64));

    v64 = GetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
}
    
static void CheckMaybeAlignedPut(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t v8 = MAGIC_8;
    const uint16_t v16 = MAGIC_16;
    const uint32_t v32 = MAGIC_32;
    const uint64_t v64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t s8;
    uint16_t s16;
    uint32_t s32;
    uint64_t s64;
    void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    PutMaybeAligned8(p, v8);
    memcpy(&s8, p, sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    PutMaybeAligned16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    p = &buffer[1];
    PutMaybeAligned16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    PutMaybeAligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[1];
    PutMaybeAligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[2];
    PutMaybeAligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[3];
    PutMaybeAligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    PutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[1];
    PutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[2];
    PutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[3];
    PutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[4];
    PutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[5];
    PutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[6];
    PutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[7];
    PutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
}

static void CheckGet(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t s8 = MAGIC_8;
    const uint16_t s16 = MAGIC_16;
    const uint32_t s32 = MAGIC_32;
    const uint64_t s64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    v8 = Get8(&s8);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(p, &s16, sizeof(s16));

    v16 = Get16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    p = &buffer[1];
    memcpy(p, &s16, sizeof(s16));

    v16 = Get16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(p, &s32, sizeof(s32));

    v32 = Get32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[1];
    memcpy(p, &s32, sizeof(s32));

    v32 = Get32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[2];
    memcpy(p, &s32, sizeof(s32));

    v32 = Get32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[3];
    memcpy(p, &s32, sizeof(s32));

    v32 = Get32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(p, &s64, sizeof(s64));

    v64 = Get64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[1];
    memcpy(p, &s64, sizeof(s64));

    v64 = Get64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[2];
    memcpy(p, &s64, sizeof(s64));

    v64 = Get64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[3];
    memcpy(p, &s64, sizeof(s64));

    v64 = Get64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[4];
    memcpy(p, &s64, sizeof(s64));

    v64 = Get64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[5];
    memcpy(p, &s64, sizeof(s64));

    v64 = Get64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[6];
    memcpy(p, &s64, sizeof(s64));

    v64 = Get64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[7];
    memcpy(p, &s64, sizeof(s64));

    v64 = Get64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
}
                
static void CheckPut(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t v8 = MAGIC_8;
    const uint16_t v16 = MAGIC_16;
    const uint32_t v32 = MAGIC_32;
    const uint64_t v64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t s8;
    uint16_t s16;
    uint32_t s32;
    uint64_t s64;
    void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    Put8(p, v8);
    memcpy(&s8, p, sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    Put16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    p = &buffer[1];
    Put16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    Put32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[1];
    Put32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[2];
    Put32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[3];
    Put32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    Put64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[1];
    Put64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[2];
    Put64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[3];
    Put64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[4];
    Put64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[5];
    Put64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[6];
    Put64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[7];
    Put64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
}

static void CheckAlignedRead(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t s8 = MAGIC_8;
    const uint16_t s16 = MAGIC_16;
    const uint32_t s32 = MAGIC_32;
    const uint64_t s64 = MAGIC_64;
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    const void *p;

    p = &s8;
    v8 = ReadAligned8(p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s8 + sizeof(s8));

    p = &s16;
    v16 = ReadAligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s16 + sizeof(s16));

    p = &s32;
    v32 = ReadAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s32 + sizeof(s32));

    p = &s64;
    v64 = ReadAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s64 + sizeof(s64));
}
        
static void CheckAlignedWrite(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t v8 = MAGIC_8;
    const uint16_t v16 = MAGIC_16;
    const uint32_t v32 = MAGIC_32;
    const uint64_t v64 = MAGIC_64;
    uint8_t s8;
    uint16_t s16;
    uint32_t s32;
    uint64_t s64;
    void *p;

    p = &s8;
    WriteAligned8(p, v8);
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s8 + sizeof(s8));

    p = &s16;
    WriteAligned16(p, v16);
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s16 + sizeof(s16));

    p = &s32;
    WriteAligned32(p, v32);
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s32 + sizeof(s32));

    p = &s64;
    WriteAligned64(p, v64);
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s64 + sizeof(s64));
}
       
static void CheckUnalignedRead(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t s8 = MAGIC_8;
    const uint16_t s16 = MAGIC_16;
    const uint32_t s32 = MAGIC_32;
    const uint64_t s64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    const void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    memcpy(&buffer[0], &s8, sizeof(s8));

    v8 = ReadUnaligned8(p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s16, sizeof(s16));

    v16 = ReadUnaligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    memcpy(&buffer[1], &s16, sizeof(s16));

    v16 = ReadUnaligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s32, sizeof(s32));

    v32 = ReadUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    memcpy(&buffer[1], &s32, sizeof(s32));

    v32 = ReadUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    memcpy(&buffer[2], &s32, sizeof(s32));

    v32 = ReadUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    memcpy(&buffer[3], &s32, sizeof(s32));

    v32 = ReadUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s64, sizeof(s64));

    v64 = ReadUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    memcpy(&buffer[1], &s64, sizeof(s64));

    v64 = ReadUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    memcpy(&buffer[2], &s64, sizeof(s64));

    v64 = ReadUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    memcpy(&buffer[3], &s64, sizeof(s64));

    v64 = ReadUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    memcpy(&buffer[4], &s64, sizeof(s64));

    v64 = ReadUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    memcpy(&buffer[5], &s64, sizeof(s64));

    v64 = ReadUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    memcpy(&buffer[6], &s64, sizeof(s64));

    v64 = ReadUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    memcpy(&buffer[7], &s64, sizeof(s64));

    v64 = ReadUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(s64));
}
      
static void CheckUnalignedWrite(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t v8 = MAGIC_8;
    const uint16_t v16 = MAGIC_16;
    const uint32_t v32 = MAGIC_32;
    const uint64_t v64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t s8;
    uint16_t s16;
    uint32_t s32;
    uint64_t s64;
    void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    WriteUnaligned8(p, v8);
    memcpy(&s8, &buffer[0], sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    WriteUnaligned16(p, v16);
    memcpy(&s16, &buffer[0], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    WriteUnaligned16(p, v16);
    memcpy(&s16, &buffer[1], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    WriteUnaligned32(p, v32);
    memcpy(&s32, &buffer[0], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    WriteUnaligned32(p, v32);
    memcpy(&s32, &buffer[1], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    WriteUnaligned32(p, v32);
    memcpy(&s32, &buffer[2], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    WriteUnaligned32(p, v32);
    memcpy(&s32, &buffer[3], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    WriteUnaligned64(p, v64);
    memcpy(&s64, &buffer[0], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    WriteUnaligned64(p, v64);
    memcpy(&s64, &buffer[1], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    WriteUnaligned64(p, v64);
    memcpy(&s64, &buffer[2], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    WriteUnaligned64(p, v64);
    memcpy(&s64, &buffer[3], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    WriteUnaligned64(p, v64);
    memcpy(&s64, &buffer[4], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    WriteUnaligned64(p, v64);
    memcpy(&s64, &buffer[5], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    WriteUnaligned64(p, v64);
    memcpy(&s64, &buffer[6], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    WriteUnaligned64(p, v64);
    memcpy(&s64, &buffer[7], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(s64));
}
     
static void CheckMaybeAlignedRead(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t s8 = MAGIC_8;
    const uint16_t s16 = MAGIC_16;
    const uint32_t s32 = MAGIC_32;
    const uint64_t s64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    const void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    memcpy(&buffer[0], &s8, sizeof(s8));

    v8 = ReadMaybeAligned8(p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s16, sizeof(s16));

    v16 = ReadMaybeAligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    memcpy(&buffer[1], &s16, sizeof(s16));

    v16 = ReadMaybeAligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s32, sizeof(s32));

    v32 = ReadMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    memcpy(&buffer[1], &s32, sizeof(s32));

    v32 = ReadMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    memcpy(&buffer[2], &s32, sizeof(s32));

    v32 = ReadMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    memcpy(&buffer[3], &s32, sizeof(s32));

    v32 = ReadMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s64, sizeof(s64));

    v64 = ReadMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    memcpy(&buffer[1], &s64, sizeof(s64));

    v64 = ReadMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    memcpy(&buffer[2], &s64, sizeof(s64));

    v64 = ReadMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    memcpy(&buffer[3], &s64, sizeof(s64));

    v64 = ReadMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    memcpy(&buffer[4], &s64, sizeof(s64));

    v64 = ReadMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    memcpy(&buffer[5], &s64, sizeof(s64));

    v64 = ReadMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    memcpy(&buffer[6], &s64, sizeof(s64));

    v64 = ReadMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    memcpy(&buffer[7], &s64, sizeof(s64));

    v64 = ReadMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(s64));
}
   
static void CheckMaybeAlignedWrite(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t v8 = MAGIC_8;
    const uint16_t v16 = MAGIC_16;
    const uint32_t v32 = MAGIC_32;
    const uint64_t v64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t s8;
    uint16_t s16;
    uint32_t s32;
    uint64_t s64;
    void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    WriteMaybeAligned8(p, v8);
    memcpy(&s8, &buffer[0], sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    WriteMaybeAligned16(p, v16);
    memcpy(&s16, &buffer[0], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    WriteMaybeAligned16(p, v16);
    memcpy(&s16, &buffer[1], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    WriteMaybeAligned32(p, v32);
    memcpy(&s32, &buffer[0], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    WriteMaybeAligned32(p, v32);
    memcpy(&s32, &buffer[1], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    WriteMaybeAligned32(p, v32);
    memcpy(&s32, &buffer[2], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    WriteMaybeAligned32(p, v32);
    memcpy(&s32, &buffer[3], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    WriteMaybeAligned64(p, v64);
    memcpy(&s64, &buffer[0], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    WriteMaybeAligned64(p, v64);
    memcpy(&s64, &buffer[1], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    WriteMaybeAligned64(p, v64);
    memcpy(&s64, &buffer[2], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    WriteMaybeAligned64(p, v64);
    memcpy(&s64, &buffer[3], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    WriteMaybeAligned64(p, v64);
    memcpy(&s64, &buffer[4], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    WriteMaybeAligned64(p, v64);
    memcpy(&s64, &buffer[5], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    WriteMaybeAligned64(p, v64);
    memcpy(&s64, &buffer[6], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    WriteMaybeAligned64(p, v64);
    memcpy(&s64, &buffer[7], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(s64));
}

static void CheckRead(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t s8 = MAGIC_8;
    const uint16_t s16 = MAGIC_16;
    const uint32_t s32 = MAGIC_32;
    const uint64_t s64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    const void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    memcpy(&buffer[0], &s8, sizeof(s8));

    v8 = Read8(p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s16, sizeof(s16));

    v16 = Read16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    memcpy(&buffer[1], &s16, sizeof(s16));

    v16 = Read16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s32, sizeof(s32));

    v32 = Read32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    memcpy(&buffer[1], &s32, sizeof(s32));

    v32 = Read32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    memcpy(&buffer[2], &s32, sizeof(s32));

    v32 = Read32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    memcpy(&buffer[3], &s32, sizeof(s32));

    v32 = Read32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s64, sizeof(s64));

    v64 = Read64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    memcpy(&buffer[1], &s64, sizeof(s64));

    v64 = Read64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    memcpy(&buffer[2], &s64, sizeof(s64));

    v64 = Read64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    memcpy(&buffer[3], &s64, sizeof(s64));

    v64 = Read64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    memcpy(&buffer[4], &s64, sizeof(s64));

    v64 = Read64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    memcpy(&buffer[5], &s64, sizeof(s64));

    v64 = Read64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    memcpy(&buffer[6], &s64, sizeof(s64));

    v64 = Read64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    memcpy(&buffer[7], &s64, sizeof(s64));

    v64 = Read64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(s64));
}

static void CheckWrite(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t v8 = MAGIC_8;
    const uint16_t v16 = MAGIC_16;
    const uint32_t v32 = MAGIC_32;
    const uint64_t v64 = MAGIC_64;
    uint8_t buffer[sizeof(uint64_t) * 2];
    uint8_t s8;
    uint16_t s16;
    uint32_t s32;
    uint64_t s64;
    void *p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    Write8(p, v8);
    memcpy(&s8, &buffer[0], sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    Write16(p, v16);
    memcpy(&s16, &buffer[0], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    Write16(p, v16);
    memcpy(&s16, &buffer[1], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    Write32(p, v32);
    memcpy(&s32, &buffer[0], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    Write32(p, v32);
    memcpy(&s32, &buffer[1], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    Write32(p, v32);
    memcpy(&s32, &buffer[2], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    Write32(p, v32);
    memcpy(&s32, &buffer[3], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    Write64(p, v64);
    memcpy(&s64, &buffer[0], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    Write64(p, v64);
    memcpy(&s64, &buffer[1], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    Write64(p, v64);
    memcpy(&s64, &buffer[2], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    Write64(p, v64);
    memcpy(&s64, &buffer[3], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    Write64(p, v64);
    memcpy(&s64, &buffer[4], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    Write64(p, v64);
    memcpy(&s64, &buffer[5], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    Write64(p, v64);
    memcpy(&s64, &buffer[6], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    Write64(p, v64);
    memcpy(&s64, &buffer[7], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(s64));
}

static const nlTest sTests[] = {
    NL_TEST_DEF("introspection",       CheckIntrospection),
    NL_TEST_DEF("aligned get",         CheckAlignedGet),
    NL_TEST_DEF("aligned put",         CheckAlignedPut),
    NL_TEST_DEF("unaligned get",       CheckUnalignedGet),
    NL_TEST_DEF("unaligned put",       CheckUnalignedPut),
    NL_TEST_DEF("maybe aligned get",   CheckMaybeAlignedGet),
    NL_TEST_DEF("maybe aligned put",   CheckMaybeAlignedPut),
    NL_TEST_DEF("get",                 CheckGet),
    NL_TEST_DEF("put",                 CheckPut),
    NL_TEST_DEF("aligned read",        CheckAlignedRead),
    NL_TEST_DEF("aligned write",       CheckAlignedWrite),
    NL_TEST_DEF("unaligned read",      CheckUnalignedRead),
    NL_TEST_DEF("unaligned write",     CheckUnalignedWrite),
    NL_TEST_DEF("maybe aligned read",  CheckMaybeAlignedRead),
    NL_TEST_DEF("maybe aligned write", CheckMaybeAlignedWrite),
    NL_TEST_DEF("read",                CheckRead),
    NL_TEST_DEF("write",               CheckWrite),
    NL_TEST_SENTINEL()
};

int main(void)
{
    nlTestSuite theSuite = {
        "nlio-cxx",
        &sTests[0]
    };

    nl_test_set_output_style(OUTPUT_CSV);

    nlTestRunner(&theSuite, NULL);

    return nlTestRunnerStats(&theSuite);
}
