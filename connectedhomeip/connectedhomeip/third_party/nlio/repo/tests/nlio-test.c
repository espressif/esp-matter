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

#include <nlio.h>

#include <nlunit-test.h>

#include "nlio-test.h"

static void CheckIntrospection(nlTestSuite *inSuite, void *inContext)
{
    bool aligned;
    uint8_t v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
    _uint128_t v128;

    // Check for natural alignment of an 8-bit stack variable.

    aligned = nlIOIsAligned(&v8, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 16-bit stack variable, which
    // should also be aligned on 8-bit as well.

    aligned = nlIOIsAligned(&v16, sizeof(v16));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v16, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 32-bit stack variable, which
    // should also be aligned on 16- and 8-bit as well.

    aligned = nlIOIsAligned(&v32, sizeof(v32));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v32, sizeof(v16));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v32, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 64-bit stack variable, which
    // should also be aligned on 32-, 16-, and 8-bit as well.

    aligned = nlIOIsAligned(&v64, sizeof(v64));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v64, sizeof(v32));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v64, sizeof(v16));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v64, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);

    // Check for natural alignment of an 128-bit stack variable, which
    // should also be aligned on 64-, 32-, 16-, and 8-bit as well.

    aligned = nlIOIsAligned(&v128, sizeof(v128));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v128, sizeof(v64));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v128, sizeof(v32));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v128, sizeof(v16));
    NL_TEST_ASSERT(inSuite, aligned == true);

    aligned = nlIOIsAligned(&v128, sizeof(v8));
    NL_TEST_ASSERT(inSuite, aligned == true);
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

    v8 = nlIOGetAligned8(&s8);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);

    v16 = nlIOGetAligned16(&s16);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    v32 = nlIOGetAligned32(&s32);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    v64 = nlIOGetAligned64(&s64);
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

    nlIOPutAligned8(&s8, v8);
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);

    nlIOPutAligned16(&s16, v16);
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    nlIOPutAligned32(&s32, v32);
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    nlIOPutAligned64(&s64, v64);
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

    v8 = nlIOGetUnaligned8(p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(p, &s16, sizeof(s16));

    v16 = nlIOGetUnaligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    p = &buffer[1];
    memcpy(p, &s16, sizeof(s16));

    v16 = nlIOGetUnaligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGetUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[1];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGetUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[2];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGetUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[3];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGetUnaligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[1];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[2];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[3];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[4];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[5];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[6];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetUnaligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[7];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetUnaligned64(p);
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
    nlIOPutUnaligned8(p, v8);
    memcpy(&s8, p, sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOPutUnaligned16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    p = &buffer[1];
    nlIOPutUnaligned16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOPutUnaligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[1];
    nlIOPutUnaligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[2];
    nlIOPutUnaligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[3];
    nlIOPutUnaligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOPutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[1];
    nlIOPutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[2];
    nlIOPutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[3];
    nlIOPutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[4];
    nlIOPutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[5];
    nlIOPutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[6];
    nlIOPutUnaligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[7];
    nlIOPutUnaligned64(p, v64);
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

    v8 = nlIOGetMaybeAligned8(&s8);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(p, &s16, sizeof(s16));

    v16 = nlIOGetMaybeAligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    p = &buffer[1];
    memcpy(p, &s16, sizeof(s16));

    v16 = nlIOGetMaybeAligned16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGetMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[1];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGetMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[2];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGetMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[3];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGetMaybeAligned32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[1];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[2];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[3];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[4];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[5];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[6];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetMaybeAligned64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[7];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGetMaybeAligned64(p);
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
    nlIOPutMaybeAligned8(p, v8);
    memcpy(&s8, p, sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOPutMaybeAligned16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    p = &buffer[1];
    nlIOPutMaybeAligned16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOPutMaybeAligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[1];
    nlIOPutMaybeAligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[2];
    nlIOPutMaybeAligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[3];
    nlIOPutMaybeAligned32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOPutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[1];
    nlIOPutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[2];
    nlIOPutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[3];
    nlIOPutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[4];
    nlIOPutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[5];
    nlIOPutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[6];
    nlIOPutMaybeAligned64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[7];
    nlIOPutMaybeAligned64(p, v64);
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

    v8 = nlIOGet8(&s8);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(p, &s16, sizeof(s16));

    v16 = nlIOGet16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    p = &buffer[1];
    memcpy(p, &s16, sizeof(s16));

    v16 = nlIOGet16(p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGet32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[1];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGet32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[2];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGet32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    p = &buffer[3];
    memcpy(p, &s32, sizeof(s32));

    v32 = nlIOGet32(p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGet64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[1];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGet64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[2];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGet64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[3];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGet64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[4];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGet64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[5];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGet64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[6];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGet64(p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);

    p = &buffer[7];
    memcpy(p, &s64, sizeof(s64));

    v64 = nlIOGet64(p);
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
    nlIOPut8(p, v8);
    memcpy(&s8, p, sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOPut16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    p = &buffer[1];
    nlIOPut16(p, v16);
    memcpy(&s16, p, sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOPut32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[1];
    nlIOPut32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[2];
    nlIOPut32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    p = &buffer[3];
    nlIOPut32(p, v32);
    memcpy(&s32, p, sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOPut64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[1];
    nlIOPut64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[2];
    nlIOPut64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[3];
    nlIOPut64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[4];
    nlIOPut64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[5];
    nlIOPut64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[6];
    nlIOPut64(p, v64);
    memcpy(&s64, p, sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);

    p = &buffer[7];
    nlIOPut64(p, v64);
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
    v8 = nlIOReadAligned8(&p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s8 + sizeof(s8));

    p = &s16;
    v16 = nlIOReadAligned16(&p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s16 + sizeof(s16));

    p = &s32;
    v32 = nlIOReadAligned32(&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s32 + sizeof(s32));

    p = &s64;
    v64 = nlIOReadAligned64(&p);
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
    nlIOWriteAligned8(&p, v8);
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s8 + sizeof(s8));

    p = &s16;
    nlIOWriteAligned16(&p, v16);
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s16 + sizeof(s16));

    p = &s32;
    nlIOWriteAligned32(&p, v32);
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&s32 + sizeof(s32));

    p = &s64;
    nlIOWriteAligned64(&p, v64);
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

    v8 = nlIOReadUnaligned8((const void **)&p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s16, sizeof(s16));

    v16 = nlIOReadUnaligned16((const void **)&p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    memcpy(&buffer[1], &s16, sizeof(s16));

    v16 = nlIOReadUnaligned16((const void **)&p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s32, sizeof(s32));

    v32 = nlIOReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    memcpy(&buffer[1], &s32, sizeof(s32));

    v32 = nlIOReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    memcpy(&buffer[2], &s32, sizeof(s32));

    v32 = nlIOReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    memcpy(&buffer[3], &s32, sizeof(s32));

    v32 = nlIOReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s64, sizeof(s64));

    v64 = nlIOReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    memcpy(&buffer[1], &s64, sizeof(s64));

    v64 = nlIOReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    memcpy(&buffer[2], &s64, sizeof(s64));

    v64 = nlIOReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    memcpy(&buffer[3], &s64, sizeof(s64));

    v64 = nlIOReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    memcpy(&buffer[4], &s64, sizeof(s64));

    v64 = nlIOReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    memcpy(&buffer[5], &s64, sizeof(s64));

    v64 = nlIOReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    memcpy(&buffer[6], &s64, sizeof(s64));

    v64 = nlIOReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    memcpy(&buffer[7], &s64, sizeof(s64));

    v64 = nlIOReadUnaligned64((const void **)&p);
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
    nlIOWriteUnaligned8(&p, v8);
    memcpy(&s8, &buffer[0], sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOWriteUnaligned16(&p, v16);
    memcpy(&s16, &buffer[0], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    nlIOWriteUnaligned16(&p, v16);
    memcpy(&s16, &buffer[1], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOWriteUnaligned32(&p, v32);
    memcpy(&s32, &buffer[0], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    nlIOWriteUnaligned32(&p, v32);
    memcpy(&s32, &buffer[1], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    nlIOWriteUnaligned32(&p, v32);
    memcpy(&s32, &buffer[2], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    nlIOWriteUnaligned32(&p, v32);
    memcpy(&s32, &buffer[3], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOWriteUnaligned64(&p, v64);
    memcpy(&s64, &buffer[0], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    nlIOWriteUnaligned64(&p, v64);
    memcpy(&s64, &buffer[1], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    nlIOWriteUnaligned64(&p, v64);
    memcpy(&s64, &buffer[2], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    nlIOWriteUnaligned64(&p, v64);
    memcpy(&s64, &buffer[3], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    nlIOWriteUnaligned64(&p, v64);
    memcpy(&s64, &buffer[4], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    nlIOWriteUnaligned64(&p, v64);
    memcpy(&s64, &buffer[5], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    nlIOWriteUnaligned64(&p, v64);
    memcpy(&s64, &buffer[6], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    nlIOWriteUnaligned64(&p, v64);
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

    v8 = nlIOReadMaybeAligned8((const void **)&p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s16, sizeof(s16));

    v16 = nlIOReadMaybeAligned16((const void **)&p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    memcpy(&buffer[1], &s16, sizeof(s16));

    v16 = nlIOReadMaybeAligned16((const void **)&p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s32, sizeof(s32));

    v32 = nlIOReadMaybeAligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    memcpy(&buffer[1], &s32, sizeof(s32));

    v32 = nlIOReadMaybeAligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    memcpy(&buffer[2], &s32, sizeof(s32));

    v32 = nlIOReadMaybeAligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    memcpy(&buffer[3], &s32, sizeof(s32));

    v32 = nlIOReadMaybeAligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s64, sizeof(s64));

    v64 = nlIOReadMaybeAligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    memcpy(&buffer[1], &s64, sizeof(s64));

    v64 = nlIOReadMaybeAligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    memcpy(&buffer[2], &s64, sizeof(s64));

    v64 = nlIOReadMaybeAligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    memcpy(&buffer[3], &s64, sizeof(s64));

    v64 = nlIOReadMaybeAligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    memcpy(&buffer[4], &s64, sizeof(s64));

    v64 = nlIOReadMaybeAligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    memcpy(&buffer[5], &s64, sizeof(s64));

    v64 = nlIOReadMaybeAligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    memcpy(&buffer[6], &s64, sizeof(s64));

    v64 = nlIOReadMaybeAligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    memcpy(&buffer[7], &s64, sizeof(s64));

    v64 = nlIOReadMaybeAligned64((const void **)&p);
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
    nlIOWriteMaybeAligned8(&p, v8);
    memcpy(&s8, &buffer[0], sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOWriteMaybeAligned16(&p, v16);
    memcpy(&s16, &buffer[0], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    nlIOWriteMaybeAligned16(&p, v16);
    memcpy(&s16, &buffer[1], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOWriteMaybeAligned32(&p, v32);
    memcpy(&s32, &buffer[0], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    nlIOWriteMaybeAligned32(&p, v32);
    memcpy(&s32, &buffer[1], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    nlIOWriteMaybeAligned32(&p, v32);
    memcpy(&s32, &buffer[2], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    nlIOWriteMaybeAligned32(&p, v32);
    memcpy(&s32, &buffer[3], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOWriteMaybeAligned64(&p, v64);
    memcpy(&s64, &buffer[0], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    nlIOWriteMaybeAligned64(&p, v64);
    memcpy(&s64, &buffer[1], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    nlIOWriteMaybeAligned64(&p, v64);
    memcpy(&s64, &buffer[2], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    nlIOWriteMaybeAligned64(&p, v64);
    memcpy(&s64, &buffer[3], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    nlIOWriteMaybeAligned64(&p, v64);
    memcpy(&s64, &buffer[4], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    nlIOWriteMaybeAligned64(&p, v64);
    memcpy(&s64, &buffer[5], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    nlIOWriteMaybeAligned64(&p, v64);
    memcpy(&s64, &buffer[6], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    nlIOWriteMaybeAligned64(&p, v64);
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

    v8 = nlIORead8((const void **)&p);
    NL_TEST_ASSERT(inSuite, v8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s16, sizeof(s16));

    v16 = nlIORead16((const void **)&p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    memcpy(&buffer[1], &s16, sizeof(s16));

    v16 = nlIORead16((const void **)&p);
    NL_TEST_ASSERT(inSuite, v16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s32, sizeof(s32));

    v32 = nlIORead32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    memcpy(&buffer[1], &s32, sizeof(s32));

    v32 = nlIORead32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    memcpy(&buffer[2], &s32, sizeof(s32));

    v32 = nlIORead32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    memcpy(&buffer[3], &s32, sizeof(s32));

    v32 = nlIORead32((const void **)&p);
    NL_TEST_ASSERT(inSuite, v32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &s64, sizeof(s64));

    v64 = nlIORead64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    memcpy(&buffer[1], &s64, sizeof(s64));

    v64 = nlIORead64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    memcpy(&buffer[2], &s64, sizeof(s64));

    v64 = nlIORead64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    memcpy(&buffer[3], &s64, sizeof(s64));

    v64 = nlIORead64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    memcpy(&buffer[4], &s64, sizeof(s64));

    v64 = nlIORead64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    memcpy(&buffer[5], &s64, sizeof(s64));

    v64 = nlIORead64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    memcpy(&buffer[6], &s64, sizeof(s64));

    v64 = nlIORead64((const void **)&p);
    NL_TEST_ASSERT(inSuite, v64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    memcpy(&buffer[7], &s64, sizeof(s64));

    v64 = nlIORead64((const void **)&p);
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
    nlIOWrite8(&p, v8);
    memcpy(&s8, &buffer[0], sizeof(v8));
    NL_TEST_ASSERT(inSuite, s8 == MAGIC_8);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s8));

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOWrite16(&p, v16);
    memcpy(&s16, &buffer[0], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s16));

    p = &buffer[1];
    nlIOWrite16(&p, v16);
    memcpy(&s16, &buffer[1], sizeof(v16));
    NL_TEST_ASSERT(inSuite, s16 == MAGIC_16);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s16));

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOWrite32(&p, v32);
    memcpy(&s32, &buffer[0], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s32));

    p = &buffer[1];
    nlIOWrite32(&p, v32);
    memcpy(&s32, &buffer[1], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s32));

    p = &buffer[2];
    nlIOWrite32(&p, v32);
    memcpy(&s32, &buffer[2], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s32));

    p = &buffer[3];
    nlIOWrite32(&p, v32);
    memcpy(&s32, &buffer[3], sizeof(v32));
    NL_TEST_ASSERT(inSuite, s32 == MAGIC_32);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s32));

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOWrite64(&p, v64);
    memcpy(&s64, &buffer[0], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(s64));

    p = &buffer[1];
    nlIOWrite64(&p, v64);
    memcpy(&s64, &buffer[1], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(s64));

    p = &buffer[2];
    nlIOWrite64(&p, v64);
    memcpy(&s64, &buffer[2], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(s64));

    p = &buffer[3];
    nlIOWrite64(&p, v64);
    memcpy(&s64, &buffer[3], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(s64));

    p = &buffer[4];
    nlIOWrite64(&p, v64);
    memcpy(&s64, &buffer[4], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(s64));

    p = &buffer[5];
    nlIOWrite64(&p, v64);
    memcpy(&s64, &buffer[5], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(s64));

    p = &buffer[6];
    nlIOWrite64(&p, v64);
    memcpy(&s64, &buffer[6], sizeof(v64));
    NL_TEST_ASSERT(inSuite, s64 == MAGIC_64);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(s64));

    p = &buffer[7];
    nlIOWrite64(&p, v64);
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
        "nlio",
        &sTests[0]
    };

    nl_test_set_output_style(OUTPUT_CSV);

    nlTestRunner(&theSuite, NULL);

    return nlTestRunnerStats(&theSuite);
}
