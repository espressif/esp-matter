/**
 *    Copyright 2015-2016 Nest Labs Inc. All Rights Reserved.
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
 *      binding of the Nest Labs memory-mapped I/O functions with
 *      byte reordering.
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include <nlio-byteorder.h>

#include <nlunit-test.h>

#include "nlio-byteorder-test.h"

static void CheckGetBig(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    v8_out  = nlIOBigEndianGet8(&v8_in);
    v16_out = nlIOBigEndianGet16(&v16_in);
    v32_out = nlIOBigEndianGet32(&v32_in);
    v64_out = nlIOBigEndianGet64(&v64_in);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckGetLittle(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    v8_out  = nlIOLittleEndianGet8(&v8_in);
    v16_out = nlIOLittleEndianGet16(&v16_in);
    v32_out = nlIOLittleEndianGet32(&v32_in);
    v64_out = nlIOLittleEndianGet64(&v64_in);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckPutBig(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    nlIOBigEndianPut8(&v8_out,   v8_in);
    nlIOBigEndianPut16(&v16_out, v16_in);
    nlIOBigEndianPut32(&v32_out, v32_in);
    nlIOBigEndianPut64(&v64_out, v64_in);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckPutLittle(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    nlIOLittleEndianPut8(&v8_out,   v8_in);
    nlIOLittleEndianPut16(&v16_out, v16_in);
    nlIOLittleEndianPut32(&v32_out, v32_in);
    nlIOLittleEndianPut64(&v64_out, v64_in);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckReadBig(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    const void     *p8_in  = &v8_in;
    const void     *p16_in = &v16_in;
    const void     *p32_in = &v32_in;
    const void     *p64_in = &v64_in;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    v8_out  = nlIOBigEndianRead8(&p8_in);
    v16_out = nlIOBigEndianRead16(&p16_in);
    v32_out = nlIOBigEndianRead32(&p32_in);
    v64_out = nlIOBigEndianRead64(&p64_in);

    NL_TEST_ASSERT(inSuite, p8_in == (&v8_in + 1));
    NL_TEST_ASSERT(inSuite, p16_in == (&v16_in + 1));
    NL_TEST_ASSERT(inSuite, p32_in == (&v32_in + 1));
    NL_TEST_ASSERT(inSuite, p64_in == (&v64_in + 1));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckReadLittle(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    const void     *p8_in  = &v8_in;
    const void     *p16_in = &v16_in;
    const void     *p32_in = &v32_in;
    const void     *p64_in = &v64_in;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    v8_out  = nlIOLittleEndianRead8(&p8_in);
    v16_out = nlIOLittleEndianRead16(&p16_in);
    v32_out = nlIOLittleEndianRead32(&p32_in);
    v64_out = nlIOLittleEndianRead64(&p64_in);

    NL_TEST_ASSERT(inSuite, p8_in == (&v8_in + 1));
    NL_TEST_ASSERT(inSuite, p16_in == (&v16_in + 1));
    NL_TEST_ASSERT(inSuite, p32_in == (&v32_in + 1));
    NL_TEST_ASSERT(inSuite, p64_in == (&v64_in + 1));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckWriteBig(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;
    void           *p8_out  = &v8_out;
    void           *p16_out = &v16_out;
    void           *p32_out = &v32_out;
    void           *p64_out = &v64_out;

    nlIOBigEndianWrite8(&p8_out, v8_in);
    nlIOBigEndianWrite16(&p16_out, v16_in);
    nlIOBigEndianWrite32(&p32_out, v32_in);
    nlIOBigEndianWrite64(&p64_out, v64_in);

    NL_TEST_ASSERT(inSuite, p8_out == (&v8_out + 1));
    NL_TEST_ASSERT(inSuite, p16_out == (&v16_out + 1));
    NL_TEST_ASSERT(inSuite, p32_out == (&v32_out + 1));
    NL_TEST_ASSERT(inSuite, p64_out == (&v64_out + 1));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckWriteLittle(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;
    void           *p8_out  = &v8_out;
    void           *p16_out = &v16_out;
    void           *p32_out = &v32_out;
    void           *p64_out = &v64_out;

    nlIOLittleEndianWrite8(&p8_out, v8_in);
    nlIOLittleEndianWrite16(&p16_out, v16_in);
    nlIOLittleEndianWrite32(&p32_out, v32_in);
    nlIOLittleEndianWrite64(&p64_out, v64_in);

    NL_TEST_ASSERT(inSuite, p8_out == (&v8_out + 1));
    NL_TEST_ASSERT(inSuite, p16_out == (&v16_out + 1));
    NL_TEST_ASSERT(inSuite, p32_out == (&v32_out + 1));
    NL_TEST_ASSERT(inSuite, p64_out == (&v64_out + 1));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckGetBigAligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    v8_out  = nlIOBigEndianGetAligned8(&v8_in);
    v16_out = nlIOBigEndianGetAligned16(&v16_in);
    v32_out = nlIOBigEndianGetAligned32(&v32_in);
    v64_out = nlIOBigEndianGetAligned64(&v64_in);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckGetLittleAligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    v8_out  = nlIOLittleEndianGetAligned8(&v8_in);
    v16_out = nlIOLittleEndianGetAligned16(&v16_in);
    v32_out = nlIOLittleEndianGetAligned32(&v32_in);
    v64_out = nlIOLittleEndianGetAligned64(&v64_in);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckPutBigAligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    nlIOBigEndianPutAligned8(&v8_out,   v8_in);
    nlIOBigEndianPutAligned16(&v16_out, v16_in);
    nlIOBigEndianPutAligned32(&v32_out, v32_in);
    nlIOBigEndianPutAligned64(&v64_out, v64_in);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckPutLittleAligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    nlIOLittleEndianPutAligned8(&v8_out,   v8_in);
    nlIOLittleEndianPutAligned16(&v16_out, v16_in);
    nlIOLittleEndianPutAligned32(&v32_out, v32_in);
    nlIOLittleEndianPutAligned64(&v64_out, v64_in);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckReadBigAligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    const void     *p8_in  = &v8_in;
    const void     *p16_in = &v16_in;
    const void     *p32_in = &v32_in;
    const void     *p64_in = &v64_in;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    v8_out  = nlIOBigEndianReadAligned8(&p8_in);
    v16_out = nlIOBigEndianReadAligned16(&p16_in);
    v32_out = nlIOBigEndianReadAligned32(&p32_in);
    v64_out = nlIOBigEndianReadAligned64(&p64_in);

    NL_TEST_ASSERT(inSuite, p8_in == (&v8_in + 1));
    NL_TEST_ASSERT(inSuite, p16_in == (&v16_in + 1));
    NL_TEST_ASSERT(inSuite, p32_in == (&v32_in + 1));
    NL_TEST_ASSERT(inSuite, p64_in == (&v64_in + 1));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckReadLittleAligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    const void     *p8_in  = &v8_in;
    const void     *p16_in = &v16_in;
    const void     *p32_in = &v32_in;
    const void     *p64_in = &v64_in;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;

    v8_out  = nlIOLittleEndianReadAligned8(&p8_in);
    v16_out = nlIOLittleEndianReadAligned16(&p16_in);
    v32_out = nlIOLittleEndianReadAligned32(&p32_in);
    v64_out = nlIOLittleEndianReadAligned64(&p64_in);

    NL_TEST_ASSERT(inSuite, p8_in == (&v8_in + 1));
    NL_TEST_ASSERT(inSuite, p16_in == (&v16_in + 1));
    NL_TEST_ASSERT(inSuite, p32_in == (&v32_in + 1));
    NL_TEST_ASSERT(inSuite, p64_in == (&v64_in + 1));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckWriteBigAligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;
    void           *p8_out  = &v8_out;
    void           *p16_out = &v16_out;
    void           *p32_out = &v32_out;
    void           *p64_out = &v64_out;

    nlIOBigEndianWriteAligned8(&p8_out, v8_in);
    nlIOBigEndianWriteAligned16(&p16_out, v16_in);
    nlIOBigEndianWriteAligned32(&p32_out, v32_in);
    nlIOBigEndianWriteAligned64(&p64_out, v64_in);

    NL_TEST_ASSERT(inSuite, p8_out == (&v8_out + 1));
    NL_TEST_ASSERT(inSuite, p16_out == (&v16_out + 1));
    NL_TEST_ASSERT(inSuite, p32_out == (&v32_out + 1));
    NL_TEST_ASSERT(inSuite, p64_out == (&v64_out + 1));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckWriteLittleAligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_out;
    uint32_t       v32_out;
    uint64_t       v64_out;
    void           *p8_out  = &v8_out;
    void           *p16_out = &v16_out;
    void           *p32_out = &v32_out;
    void           *p64_out = &v64_out;

    nlIOLittleEndianWriteAligned8(&p8_out, v8_in);
    nlIOLittleEndianWriteAligned16(&p16_out, v16_in);
    nlIOLittleEndianWriteAligned32(&p32_out, v32_in);
    nlIOLittleEndianWriteAligned64(&p64_out, v64_in);

    NL_TEST_ASSERT(inSuite, p8_out == (&v8_out + 1));
    NL_TEST_ASSERT(inSuite, p16_out == (&v16_out + 1));
    NL_TEST_ASSERT(inSuite, p32_out == (&v32_out + 1));
    NL_TEST_ASSERT(inSuite, p64_out == (&v64_out + 1));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out  == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

// XXX - Unaligned

static void CheckGetBigUnaligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_0_out;
    uint16_t       v16_1_out;
    uint32_t       v32_0_out;
    uint32_t       v32_1_out;
    uint32_t       v32_2_out;
    uint32_t       v32_3_out;
    uint64_t       v64_0_out;
    uint64_t       v64_1_out;
    uint64_t       v64_2_out;
    uint64_t       v64_3_out;
    uint64_t       v64_4_out;
    uint64_t       v64_5_out;
    uint64_t       v64_6_out;
    uint64_t       v64_7_out;
    uint8_t        buffer[sizeof(uint64_t) * 2];
    void *         p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    memcpy(p, &v8_in, sizeof(v8_in));

    v8_out = nlIOBigEndianGetUnaligned8(p);

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(p, &v16_in, sizeof(v16_in));

    v16_0_out = nlIOBigEndianGetUnaligned16(p);

    p = &buffer[1];
    memcpy(p, &v16_in, sizeof(v16_in));

    v16_1_out = nlIOBigEndianGetUnaligned16(p);

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(p, &v32_in, sizeof(v32_in));

    v32_0_out = nlIOBigEndianGetUnaligned32(p);

    p = &buffer[1];
    memcpy(p, &v32_in, sizeof(v32_in));

    v32_1_out = nlIOBigEndianGetUnaligned32(p);

    p = &buffer[2];
    memcpy(p, &v32_in, sizeof(v32_in));

    v32_2_out = nlIOBigEndianGetUnaligned32(p);

    p = &buffer[3];
    memcpy(p, &v32_in, sizeof(v32_in));

    v32_3_out = nlIOBigEndianGetUnaligned32(p);

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_0_out = nlIOBigEndianGetUnaligned64(p);

    p = &buffer[1];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_1_out = nlIOBigEndianGetUnaligned64(p);

    p = &buffer[2];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_2_out = nlIOBigEndianGetUnaligned64(p);

    p = &buffer[3];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_3_out = nlIOBigEndianGetUnaligned64(p);

    p = &buffer[4];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_4_out = nlIOBigEndianGetUnaligned64(p);

    p = &buffer[5];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_5_out = nlIOBigEndianGetUnaligned64(p);

    p = &buffer[6];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_6_out = nlIOBigEndianGetUnaligned64(p);

    p = &buffer[7];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_7_out = nlIOBigEndianGetUnaligned64(p);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckGetLittleUnaligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_0_out;
    uint16_t       v16_1_out;
    uint32_t       v32_0_out;
    uint32_t       v32_1_out;
    uint32_t       v32_2_out;
    uint32_t       v32_3_out;
    uint64_t       v64_0_out;
    uint64_t       v64_1_out;
    uint64_t       v64_2_out;
    uint64_t       v64_3_out;
    uint64_t       v64_4_out;
    uint64_t       v64_5_out;
    uint64_t       v64_6_out;
    uint64_t       v64_7_out;
    uint8_t        buffer[sizeof(uint64_t) * 2];
    void *         p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    memcpy(p, &v8_in, sizeof(v8_in));

    v8_out = nlIOLittleEndianGetUnaligned8(p);

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(p, &v16_in, sizeof(v16_in));

    v16_0_out = nlIOLittleEndianGetUnaligned16(p);

    p = &buffer[1];
    memcpy(p, &v16_in, sizeof(v16_in));

    v16_1_out = nlIOLittleEndianGetUnaligned16(p);

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(p, &v32_in, sizeof(v32_in));

    v32_0_out = nlIOLittleEndianGetUnaligned32(p);

    p = &buffer[1];
    memcpy(p, &v32_in, sizeof(v32_in));

    v32_1_out = nlIOLittleEndianGetUnaligned32(p);

    p = &buffer[2];
    memcpy(p, &v32_in, sizeof(v32_in));

    v32_2_out = nlIOLittleEndianGetUnaligned32(p);

    p = &buffer[3];
    memcpy(p, &v32_in, sizeof(v32_in));

    v32_3_out = nlIOLittleEndianGetUnaligned32(p);

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_0_out = nlIOLittleEndianGetUnaligned64(p);

    p = &buffer[1];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_1_out = nlIOLittleEndianGetUnaligned64(p);

    p = &buffer[2];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_2_out = nlIOLittleEndianGetUnaligned64(p);

    p = &buffer[3];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_3_out = nlIOLittleEndianGetUnaligned64(p);

    p = &buffer[4];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_4_out = nlIOLittleEndianGetUnaligned64(p);

    p = &buffer[5];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_5_out = nlIOLittleEndianGetUnaligned64(p);

    p = &buffer[6];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_6_out = nlIOLittleEndianGetUnaligned64(p);

    p = &buffer[7];
    memcpy(p, &v64_in, sizeof(v64_in));

    v64_7_out = nlIOLittleEndianGetUnaligned64(p);

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckPutBigUnaligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_0_out;
    uint16_t       v16_1_out;
    uint32_t       v32_0_out;
    uint32_t       v32_1_out;
    uint32_t       v32_2_out;
    uint32_t       v32_3_out;
    uint64_t       v64_0_out;
    uint64_t       v64_1_out;
    uint64_t       v64_2_out;
    uint64_t       v64_3_out;
    uint64_t       v64_4_out;
    uint64_t       v64_5_out;
    uint64_t       v64_6_out;
    uint64_t       v64_7_out;
    uint8_t        buffer[sizeof(uint64_t) * 2];
    void *         p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    nlIOBigEndianPutUnaligned8(p, v8_in);
    memcpy(&v8_out, p, sizeof(v8_in));

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOBigEndianPutUnaligned16(p, v16_in);
    memcpy(&v16_0_out, p, sizeof(v16_in));

    p = &buffer[1];
    nlIOBigEndianPutUnaligned16(p, v16_in);
    memcpy(&v16_1_out, p, sizeof(v16_in));

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOBigEndianPutUnaligned32(p, v32_in);
    memcpy(&v32_0_out, p, sizeof(v32_in));

    p = &buffer[1];
    nlIOBigEndianPutUnaligned32(p, v32_in);
    memcpy(&v32_1_out, p, sizeof(v32_in));

    p = &buffer[2];
    nlIOBigEndianPutUnaligned32(p, v32_in);
    memcpy(&v32_2_out, p, sizeof(v32_in));

    p = &buffer[3];
    nlIOBigEndianPutUnaligned32(p, v32_in);
    memcpy(&v32_3_out, p, sizeof(v32_in));

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOBigEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_0_out, p, sizeof(v64_in));

    p = &buffer[1];
    nlIOBigEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_1_out, p, sizeof(v64_in));

    p = &buffer[2];
    nlIOBigEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_2_out, p, sizeof(v64_in));

    p = &buffer[3];
    nlIOBigEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_3_out, p, sizeof(v64_in));

    p = &buffer[4];
    nlIOBigEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_4_out, p, sizeof(v64_in));

    p = &buffer[5];
    nlIOBigEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_5_out, p, sizeof(v64_in));

    p = &buffer[6];
    nlIOBigEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_6_out, p, sizeof(v64_in));

    p = &buffer[7];
    nlIOBigEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_7_out, p, sizeof(v64_in));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckPutLittleUnaligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_0_out;
    uint16_t       v16_1_out;
    uint32_t       v32_0_out;
    uint32_t       v32_1_out;
    uint32_t       v32_2_out;
    uint32_t       v32_3_out;
    uint64_t       v64_0_out;
    uint64_t       v64_1_out;
    uint64_t       v64_2_out;
    uint64_t       v64_3_out;
    uint64_t       v64_4_out;
    uint64_t       v64_5_out;
    uint64_t       v64_6_out;
    uint64_t       v64_7_out;
    uint8_t        buffer[sizeof(uint64_t) * 2];
    void *         p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    nlIOLittleEndianPutUnaligned8(p, v8_in);
    memcpy(&v8_out, p, sizeof(v8_in));

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOLittleEndianPutUnaligned16(p, v16_in);
    memcpy(&v16_0_out, p, sizeof(v16_in));

    p = &buffer[1];
    nlIOLittleEndianPutUnaligned16(p, v16_in);
    memcpy(&v16_1_out, p, sizeof(v16_in));

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOLittleEndianPutUnaligned32(p, v32_in);
    memcpy(&v32_0_out, p, sizeof(v32_in));

    p = &buffer[1];
    nlIOLittleEndianPutUnaligned32(p, v32_in);
    memcpy(&v32_1_out, p, sizeof(v32_in));

    p = &buffer[2];
    nlIOLittleEndianPutUnaligned32(p, v32_in);
    memcpy(&v32_2_out, p, sizeof(v32_in));

    p = &buffer[3];
    nlIOLittleEndianPutUnaligned32(p, v32_in);
    memcpy(&v32_3_out, p, sizeof(v32_in));

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOLittleEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_0_out, p, sizeof(v64_in));

    p = &buffer[1];
    nlIOLittleEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_1_out, p, sizeof(v64_in));

    p = &buffer[2];
    nlIOLittleEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_2_out, p, sizeof(v64_in));

    p = &buffer[3];
    nlIOLittleEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_3_out, p, sizeof(v64_in));

    p = &buffer[4];
    nlIOLittleEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_4_out, p, sizeof(v64_in));

    p = &buffer[5];
    nlIOLittleEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_5_out, p, sizeof(v64_in));

    p = &buffer[6];
    nlIOLittleEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_6_out, p, sizeof(v64_in));

    p = &buffer[7];
    nlIOLittleEndianPutUnaligned64(p, v64_in);
    memcpy(&v64_7_out, p, sizeof(v64_in));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckReadBigUnaligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_0_out;
    uint16_t       v16_1_out;
    uint32_t       v32_0_out;
    uint32_t       v32_1_out;
    uint32_t       v32_2_out;
    uint32_t       v32_3_out;
    uint64_t       v64_0_out;
    uint64_t       v64_1_out;
    uint64_t       v64_2_out;
    uint64_t       v64_3_out;
    uint64_t       v64_4_out;
    uint64_t       v64_5_out;
    uint64_t       v64_6_out;
    uint64_t       v64_7_out;
    uint8_t        buffer[sizeof(uint64_t) * 2];
    const void *   p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    memcpy(&buffer[0], &v8_in, sizeof(v8_in));

    v8_out = nlIOBigEndianReadUnaligned8((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v8_in));

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &v16_in, sizeof(v16_in));

    v16_0_out = nlIOBigEndianReadUnaligned16((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v16_in));

    p = &buffer[1];
    memcpy(&buffer[1], &v16_in, sizeof(v16_in));

    v16_1_out = nlIOBigEndianReadUnaligned16((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v16_in));

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &v32_in, sizeof(v32_in));

    v32_0_out = nlIOBigEndianReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v32_in));

    p = &buffer[1];
    memcpy(&buffer[1], &v32_in, sizeof(v32_in));

    v32_1_out = nlIOBigEndianReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v32_in));

    p = &buffer[2];
    memcpy(&buffer[2], &v32_in, sizeof(v32_in));

    v32_2_out = nlIOBigEndianReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(v32_in));

    p = &buffer[3];
    memcpy(&buffer[3], &v32_in, sizeof(v32_in));

    v32_3_out = nlIOBigEndianReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(v32_in));

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &v64_in, sizeof(v64_in));

    v64_0_out = nlIOBigEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v64_in));

    p = &buffer[1];
    memcpy(&buffer[1], &v64_in, sizeof(v64_in));

    v64_1_out = nlIOBigEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v64_in));

    p = &buffer[2];
    memcpy(&buffer[2], &v64_in, sizeof(v64_in));

    v64_2_out = nlIOBigEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(v64_in));

    p = &buffer[3];
    memcpy(&buffer[3], &v64_in, sizeof(v64_in));

    v64_3_out = nlIOBigEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(v64_in));

    p = &buffer[4];
    memcpy(&buffer[4], &v64_in, sizeof(v64_in));

    v64_4_out = nlIOBigEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(v64_in));

    p = &buffer[5];
    memcpy(&buffer[5], &v64_in, sizeof(v64_in));

    v64_5_out = nlIOBigEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(v64_in));

    p = &buffer[6];
    memcpy(&buffer[6], &v64_in, sizeof(v64_in));

    v64_6_out = nlIOBigEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(v64_in));

    p = &buffer[7];
    memcpy(&buffer[7], &v64_in, sizeof(v64_in));

    v64_7_out = nlIOBigEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(v64_in));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckReadLittleUnaligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in  = MAGIC8;
    const uint16_t v16_in = MAGIC16;
    const uint32_t v32_in = MAGIC32;
    const uint64_t v64_in = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_0_out;
    uint16_t       v16_1_out;
    uint32_t       v32_0_out;
    uint32_t       v32_1_out;
    uint32_t       v32_2_out;
    uint32_t       v32_3_out;
    uint64_t       v64_0_out;
    uint64_t       v64_1_out;
    uint64_t       v64_2_out;
    uint64_t       v64_3_out;
    uint64_t       v64_4_out;
    uint64_t       v64_5_out;
    uint64_t       v64_6_out;
    uint64_t       v64_7_out;
    uint8_t        buffer[sizeof(uint64_t) * 2];
    const void *   p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    memcpy(&buffer[0], &v8_in, sizeof(v8_in));

    v8_out = nlIOLittleEndianReadUnaligned8((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v8_in));

    // Try 16-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &v16_in, sizeof(v16_in));

    v16_0_out = nlIOLittleEndianReadUnaligned16((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v16_in));

    p = &buffer[1];
    memcpy(&buffer[1], &v16_in, sizeof(v16_in));

    v16_1_out = nlIOLittleEndianReadUnaligned16((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v16_in));

    // Try 32-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &v32_in, sizeof(v32_in));

    v32_0_out = nlIOLittleEndianReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v32_in));

    p = &buffer[1];
    memcpy(&buffer[1], &v32_in, sizeof(v32_in));

    v32_1_out = nlIOLittleEndianReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v32_in));

    p = &buffer[2];
    memcpy(&buffer[2], &v32_in, sizeof(v32_in));

    v32_2_out = nlIOLittleEndianReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(v32_in));

    p = &buffer[3];
    memcpy(&buffer[3], &v32_in, sizeof(v32_in));

    v32_3_out = nlIOLittleEndianReadUnaligned32((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(v32_in));

    // Try 64-bit quantities.

    p = &buffer[0];
    memcpy(&buffer[0], &v64_in, sizeof(v64_in));

    v64_0_out = nlIOLittleEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v64_in));

    p = &buffer[1];
    memcpy(&buffer[1], &v64_in, sizeof(v64_in));

    v64_1_out = nlIOLittleEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v64_in));

    p = &buffer[2];
    memcpy(&buffer[2], &v64_in, sizeof(v64_in));

    v64_2_out = nlIOLittleEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(v64_in));

    p = &buffer[3];
    memcpy(&buffer[3], &v64_in, sizeof(v64_in));

    v64_3_out = nlIOLittleEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(v64_in));

    p = &buffer[4];
    memcpy(&buffer[4], &v64_in, sizeof(v64_in));

    v64_4_out = nlIOLittleEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(v64_in));

    p = &buffer[5];
    memcpy(&buffer[5], &v64_in, sizeof(v64_in));

    v64_5_out = nlIOLittleEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(v64_in));

    p = &buffer[6];
    memcpy(&buffer[6], &v64_in, sizeof(v64_in));

    v64_6_out = nlIOLittleEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(v64_in));

    p = &buffer[7];
    memcpy(&buffer[7], &v64_in, sizeof(v64_in));

    v64_7_out = nlIOLittleEndianReadUnaligned64((const void **)&p);
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(v64_in));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckWriteBigUnaligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_0_out;
    uint16_t       v16_1_out;
    uint32_t       v32_0_out;
    uint32_t       v32_1_out;
    uint32_t       v32_2_out;
    uint32_t       v32_3_out;
    uint64_t       v64_0_out;
    uint64_t       v64_1_out;
    uint64_t       v64_2_out;
    uint64_t       v64_3_out;
    uint64_t       v64_4_out;
    uint64_t       v64_5_out;
    uint64_t       v64_6_out;
    uint64_t       v64_7_out;
    uint8_t        buffer[sizeof(uint64_t) * 2];
    void *         p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    nlIOBigEndianWriteUnaligned8(&p, v8_in);
    memcpy(&v8_out, &buffer[0], sizeof(v8_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v8_out));

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOBigEndianWriteUnaligned16(&p, v16_in);
    memcpy(&v16_0_out, &buffer[0], sizeof(v16_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v16_0_out));

    p = &buffer[1];
    nlIOBigEndianWriteUnaligned16(&p, v16_in);
    memcpy(&v16_1_out, &buffer[1], sizeof(v16_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v16_1_out));

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOBigEndianWriteUnaligned32(&p, v32_in);
    memcpy(&v32_0_out, &buffer[0], sizeof(v32_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v32_0_out));

    p = &buffer[1];
    nlIOBigEndianWriteUnaligned32(&p, v32_in);
    memcpy(&v32_1_out, &buffer[1], sizeof(v32_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v32_1_out));

    p = &buffer[2];
    nlIOBigEndianWriteUnaligned32(&p, v32_in);
    memcpy(&v32_2_out, &buffer[2], sizeof(v32_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(v32_2_out));

    p = &buffer[3];
    nlIOBigEndianWriteUnaligned32(&p, v32_in);
    memcpy(&v32_3_out, &buffer[3], sizeof(v32_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(v32_3_out));

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOBigEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_0_out, &buffer[0], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v64_0_out));

    p = &buffer[1];
    nlIOBigEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_1_out, &buffer[1], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v64_1_out));

    p = &buffer[2];
    nlIOBigEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_2_out, &buffer[2], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(v64_2_out));

    p = &buffer[3];
    nlIOBigEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_3_out, &buffer[3], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(v64_3_out));

    p = &buffer[4];
    nlIOBigEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_4_out, &buffer[4], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(v64_4_out));

    p = &buffer[5];
    nlIOBigEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_5_out, &buffer[5], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(v64_5_out));

    p = &buffer[6];
    nlIOBigEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_6_out, &buffer[6], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(v64_6_out));

    p = &buffer[7];
    nlIOBigEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_7_out, &buffer[7], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(v64_7_out));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC_SWAP64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static void CheckWriteLittleUnaligned(nlTestSuite *inSuite, void *inContext)
{
    const uint8_t  v8_in   = MAGIC8;
    const uint16_t v16_in  = MAGIC16;
    const uint32_t v32_in  = MAGIC32;
    const uint64_t v64_in  = MAGIC64;
    uint8_t        v8_out;
    uint16_t       v16_0_out;
    uint16_t       v16_1_out;
    uint32_t       v32_0_out;
    uint32_t       v32_1_out;
    uint32_t       v32_2_out;
    uint32_t       v32_3_out;
    uint64_t       v64_0_out;
    uint64_t       v64_1_out;
    uint64_t       v64_2_out;
    uint64_t       v64_3_out;
    uint64_t       v64_4_out;
    uint64_t       v64_5_out;
    uint64_t       v64_6_out;
    uint64_t       v64_7_out;
    uint8_t        buffer[sizeof(uint64_t) * 2];
    void *         p;

    // Try an 8-bit quantity. They are aligned anywhere and unaligned
    // nowhere.

    p = &buffer[0];
    nlIOLittleEndianWriteUnaligned8(&p, v8_in);
    memcpy(&v8_out, &buffer[0], sizeof(v8_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v8_out));

    // Try 16-bit quantities.

    p = &buffer[0];
    nlIOLittleEndianWriteUnaligned16(&p, v16_in);
    memcpy(&v16_0_out, &buffer[0], sizeof(v16_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v16_0_out));

    p = &buffer[1];
    nlIOLittleEndianWriteUnaligned16(&p, v16_in);
    memcpy(&v16_1_out, &buffer[1], sizeof(v16_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v16_1_out));

    // Try 32-bit quantities.

    p = &buffer[0];
    nlIOLittleEndianWriteUnaligned32(&p, v32_in);
    memcpy(&v32_0_out, &buffer[0], sizeof(v32_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v32_0_out));

    p = &buffer[1];
    nlIOLittleEndianWriteUnaligned32(&p, v32_in);
    memcpy(&v32_1_out, &buffer[1], sizeof(v32_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v32_1_out));

    p = &buffer[2];
    nlIOLittleEndianWriteUnaligned32(&p, v32_in);
    memcpy(&v32_2_out, &buffer[2], sizeof(v32_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(v32_2_out));

    p = &buffer[3];
    nlIOLittleEndianWriteUnaligned32(&p, v32_in);
    memcpy(&v32_3_out, &buffer[3], sizeof(v32_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(v32_3_out));

    // Try 64-bit quantities.

    p = &buffer[0];
    nlIOLittleEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_0_out, &buffer[0], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[0] + sizeof(v64_0_out));

    p = &buffer[1];
    nlIOLittleEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_1_out, &buffer[1], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[1] + sizeof(v64_1_out));

    p = &buffer[2];
    nlIOLittleEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_2_out, &buffer[2], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[2] + sizeof(v64_2_out));

    p = &buffer[3];
    nlIOLittleEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_3_out, &buffer[3], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[3] + sizeof(v64_3_out));

    p = &buffer[4];
    nlIOLittleEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_4_out, &buffer[4], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[4] + sizeof(v64_4_out));

    p = &buffer[5];
    nlIOLittleEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_5_out, &buffer[5], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[5] + sizeof(v64_5_out));

    p = &buffer[6];
    nlIOLittleEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_6_out, &buffer[6], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[6] + sizeof(v64_6_out));

    p = &buffer[7];
    nlIOLittleEndianWriteUnaligned64(&p, v64_in);
    memcpy(&v64_7_out, &buffer[7], sizeof(v64_in));
    NL_TEST_ASSERT(inSuite, p == (uint8_t *)&buffer[7] + sizeof(v64_7_out));

#if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC64);

#elif NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
    NL_TEST_ASSERT(inSuite, v8_out    == MAGIC_SWAP8);
    NL_TEST_ASSERT(inSuite, v16_0_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v16_1_out == MAGIC_SWAP16);
    NL_TEST_ASSERT(inSuite, v32_0_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_1_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_2_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v32_3_out == MAGIC_SWAP32);
    NL_TEST_ASSERT(inSuite, v64_0_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_1_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_2_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_3_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_4_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_5_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_6_out == MAGIC_SWAP64);
    NL_TEST_ASSERT(inSuite, v64_7_out == MAGIC_SWAP64);

#else
    NL_TEST_ASSERT(inSuite, false);

#endif
}

static const nlTest sTests[] = {
    NL_TEST_DEF("get big",                CheckGetBig),
    NL_TEST_DEF("get little",             CheckGetLittle),
    NL_TEST_DEF("put big",                CheckPutBig),
    NL_TEST_DEF("put little",             CheckPutLittle),
    NL_TEST_DEF("read big",               CheckReadBig),
    NL_TEST_DEF("read little",            CheckReadLittle),
    NL_TEST_DEF("write big",              CheckWriteBig),
    NL_TEST_DEF("write little",           CheckWriteLittle),
    NL_TEST_DEF("get big aligned",        CheckGetBigAligned),
    NL_TEST_DEF("get little aligned",     CheckGetLittleAligned),
    NL_TEST_DEF("put big aligned",        CheckPutBigAligned),
    NL_TEST_DEF("put little aligned",     CheckPutLittleAligned),
    NL_TEST_DEF("read big aligned",       CheckReadBigAligned),
    NL_TEST_DEF("read little aligned",    CheckReadLittleAligned),
    NL_TEST_DEF("write big aligned",      CheckWriteBigAligned),
    NL_TEST_DEF("write little aligned",   CheckWriteLittleAligned),
    NL_TEST_DEF("get big unaligned",      CheckGetBigUnaligned),
    NL_TEST_DEF("get little unaligned",   CheckGetLittleUnaligned),
    NL_TEST_DEF("put big unaligned",      CheckPutBigUnaligned),
    NL_TEST_DEF("put little unaligned",   CheckPutLittleUnaligned),
    NL_TEST_DEF("read big unaligned",     CheckReadBigUnaligned),
    NL_TEST_DEF("read little unaligned",  CheckReadLittleUnaligned),
    NL_TEST_DEF("write big unaligned",    CheckWriteBigUnaligned),
    NL_TEST_DEF("write little unaligned", CheckWriteLittleUnaligned),

    NL_TEST_SENTINEL()
};

int main(void)
{
    nlTestSuite theSuite = {
        "nlio-byteorder",
        &sTests[0]
    };

    nl_test_set_output_style(OUTPUT_CSV);

    nlTestRunner(&theSuite, NULL);

    return nlTestRunnerStats(&theSuite);
}
