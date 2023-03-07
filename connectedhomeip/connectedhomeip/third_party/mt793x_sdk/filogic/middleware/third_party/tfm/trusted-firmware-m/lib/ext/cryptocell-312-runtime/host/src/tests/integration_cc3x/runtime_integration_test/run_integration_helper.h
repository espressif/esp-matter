/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RUN_INTEGRATION_HELPER_H_
#define _RUN_INTEGRATION_HELPER_H_

#include MBEDTLS_CONFIG_FILE

#include <inttypes.h>

/* cc lib */
#include "cc_rnd_common.h"
#include "cc_pal_types.h"
#include "cc_pal_perf.h"

#include "mbedtls/platform.h"
#include "mbedtls/ctr_drbg.h"
/* pal */
#include "test_pal_mem.h"
#include "test_pal_mem_s.h"

#include MBEDTLS_CONFIG_FILE
#include "run_integration_profiler.h"

#define RUNIT_ALIGN32_SLACK                     (sizeof(uint32_t) - 1)

#define RUNIT_DEBUG_ALLOC(_pStruct)              do {} while(0); // RUNIT_PRINT_DBG("%s[%p]\n", #_pStruct, _pStruct)

/************************************************************
 *
 * Alloc macros
 *
 ************************************************************/
#define FREE_IF_NOT_NULL(_buff)    \
                if (_buff.buf != NULL) {     \
                    mbedtls_free(_buff.buf);    \
                }

#define ALLOC_VERIFY(_buff, _size) \
                do {                                                                                    \
                    if (_buff == NULL)                                                                  \
                    {                                                                                   \
                        RUNIT_PRINT_ERROR("Failed to allocate %u bytes for %s\n", (unsigned int)_size, #_buff);       \
                        rc = RUNIT_ERROR__FAIL;                                                         \
                        goto bail;                                                                      \
                    }                                                                                   \
                } while(0)

/**
 * Allocate 8 bit aligned buf using mbedtls
 * the buf will be unaligned on purpose
 */
#define MALLOC(_buff, _pStruct, _size) \
                do { \
                    runIt_malloc(&_buff, _size); \
                    ALLOC_VERIFY(_buff.buf, _size); \
                } while(0)

#define ALLOC(_buff, _pStruct, _size) \
                do { \
                    MALLOC(_buff, _pStruct, _size); \
                    _pStruct = (void*)((void*)GET_UNALIGNED_PTR(_buff)); \
                    RUNIT_DEBUG_ALLOC(_pStruct); \
                } while(0)

/** Allocate 32 bit aligned buf using mbedtls */
#define ALLOC_STRUCT(_type, _buff, _pStruct) \
                do { \
                    MALLOC(_buff, _pStruct, sizeof(_type)); \
                    _pStruct = (_type*)((void*)GET_PTR(_buff)); \
                    RUNIT_DEBUG_ALLOC(_pStruct); \
                } while(0)

/**
 * Allocat 8 bit aligned buf using mbedtls and copy contents from other buf
 * the buf will be unaligned on purpose
 */
#define ALLOC_AND_COPY(_buff, _pStruct, _src_buf, _size) \
                do { \
                    ALLOC(_buff, _pStruct, _size); \
                    RUNIT_DEBUG_ALLOC(_pStruct); \
                    memcpy((uint8_t*)_pStruct, (uint8_t*)_src_buf, _size); \
                    RUNIT_DEBUG_ALLOC(_pStruct); \
                } while(0)

/** Allocate 32 bit aligned buf using mbedtls and copy contents from other buf */
#define ALLOC32_AND_COPY(_buff, _pStruct, _src_buf, _size) \
                do { \
                    ALLOC(_buff, _pStruct, _size); \
                    _pStruct = (void*)((void*)GET_PTR(_buff)); \
                    memcpy((uint8_t*)_pStruct, (uint8_t*)_src_buf, _size); \
                    RUNIT_DEBUG_ALLOC(_pStruct); \
                } while(0)

/** Allocate 32 bit aligned buf using mbedtls */
#define ALLOC32(_buff, _pStruct, _size) \
                do { \
                    ALLOC(_buff, _pStruct, _size); \
                    _pStruct = (void*)((void*)GET_PTR(_buff)); \
                    RUNIT_DEBUG_ALLOC(_pStruct); \
                } while(0)

#ifdef ARCH_V8M
#define RUNIT_TEST_PAL_ALLOC    Test_PalDMAContigBufferAlloc_s
#define RUNIT_TEST_PAL_FREE     Test_PalDMAContigBufferFree_s
#else
#define RUNIT_TEST_PAL_ALLOC    Test_PalDMAContigBufferAlloc
#define RUNIT_TEST_PAL_FREE     Test_PalDMAContigBufferFree
#endif

/** Allocate 32 bit aligned buf using pal */
#define ALLOC_BUFF_ALIGN32(_bufWithSlack, _alignedBuff, _wantedBuffSize) \
                do {                                                                                    \
                    _bufWithSlack = (uint8_t*)RUNIT_TEST_PAL_ALLOC(_wantedBuffSize + RUNIT_ALIGN32_SLACK);\
                    ALLOC_VERIFY(_bufWithSlack, _wantedBuffSize + RUNIT_ALIGN32_SLACK);                 \
                    runIt_buffAlign32(_bufWithSlack, &_alignedBuff, _wantedBuffSize, #_bufWithSlack);                                    \
                }while(0)

/**
 *
 * Assert Macros
 *
 */
#define RUNIT_ASSERT_WITH_RESULT(_c, _exp)                                            \
                do {                                                                  \
                    int _ret = 0;                                                     \
                    runItPerfType_t _type = PERF_TYPE_TEST_NOT_SET;                   \
                    runItPerfData_t _data = 0;                                        \
                    _type = runIt_perfTypeFromStr(#_c, "");                           \
                    RUNIT_PRINT_DBG("running %-30.30s\n", #_c);                       \
                    _data = runIt_perfOpenNewEntry(_type);                            \
                    if( ( _ret = (int)_c) != _exp )                                   \
                    {                                                                 \
                        RUNIT_PRINT_ERROR( "failed with rc 0x%08x\n", _ret );         \
                        rc = RUNIT_ERROR__FAIL;                                       \
                        runIt_perfCloseEntry(_data, _type);                           \
                        goto bail;                                                    \
                    }                                                                 \
                    runIt_perfCloseEntry(_data, _type);                               \
                } while(0)

#define RUNIT_ASSERT(_c)                                                              \
                do {                                                                  \
                    if( !(_c))                                                        \
                    {                                                                 \
                        RUNIT_PRINT_ERROR( "failed\n" );                              \
                        rc = RUNIT_ERROR__FAIL;                                       \
                        goto bail;                                                    \
                    }                                                                 \
                }                                                                     \
                while(0)

#define RUNIT_API(_c)                                                                 \
                do {                                                                  \
                    runItPerfType_t _type = PERF_TYPE_TEST_NOT_SET;                   \
                    runItPerfData_t _data = 0;                                        \
                    _type = runIt_perfTypeFromStr(#_c, "");                           \
                    RUNIT_PRINT_DBG("running %-30.30s\n", #_c);                       \
                    _data = runIt_perfOpenNewEntry(_type);                            \
                    _c;                                                               \
                    runIt_perfCloseEntry(_data, _type);                               \
                } while(0)

#define RUNIT_API_ASSIGNMENT(_res, _c)                                                \
                do {                                                                  \
                    runItPerfType_t _type = PERF_TYPE_TEST_NOT_SET;                   \
                    runItPerfData_t _data = 0;                                        \
                    _type = runIt_perfTypeFromStr(#_c, "");                           \
                    RUNIT_PRINT_DBG("running %-30.30s\n", #_c);                       \
                    _data = runIt_perfOpenNewEntry(_type);                            \
                    _res = _c;                                                        \
                    runIt_perfCloseEntry(_data, _type);                               \
                } while(0)

#define RUNIT_ASSERT_API(_c)                                                          \
                do {                                                                  \
                    runItPerfType_t _type = PERF_TYPE_TEST_NOT_SET;                   \
                    runItPerfData_t _data = 0;                                        \
                    _type = runIt_perfTypeFromStr(#_c, "");                           \
                    RUNIT_PRINT_DBG("running %-30.30s\n", #_c);                       \
                    _data = runIt_perfOpenNewEntry(_type);                            \
                    RUNIT_ASSERT(_c);                                                 \
                    runIt_perfCloseEntry(_data, _type);                               \
                } while(0)

#define RUNIT_ASSERT_W_PARAM(_s, _c)                                                  \
                do {                                                                  \
                    runItPerfType_t _type = PERF_TYPE_TEST_NOT_SET;                   \
                    runItPerfData_t _data = 0;                                        \
                    _type = runIt_perfTypeFromStr(#_c, _s);                           \
                    RUNIT_PRINT_DBG("running %-30.30s\n", #_c);                       \
                    _data = runIt_perfOpenNewEntry(_type);                            \
                    RUNIT_ASSERT(_c);                                                 \
                    runIt_perfCloseEntry(_data, _type);                               \
                } while(0)

#define GET_PTR(_c) (_c.ba)
#define GET_UNALIGNED_PTR(_c) ((uint8_t*)(_c.ba) + 1)

#define RUNIT_PERF_REG_API_W_PARAM(_c, _s, _is_hw)                                    \
    runIt_perfEntryInit(#_c, _s, _is_hw)

#define RUNIT_PERF_REG_API(_c, _is_hw)                                                \
    runIt_perfEntryInit(#_c, "", _is_hw)
/************************************************************
 *
 * type decelerations
 *
 ************************************************************/
typedef struct rnd_buf_info
{
    unsigned char *buf;
    size_t length;
} rnd_buf_info;

typedef struct RunItPtr
{
    uint8_t*    buf;
    uint32_t*   ba;
    size_t      length;
} RunItPtr;

/************************************************************
 *
 * externs
 *
 *******************    *****************************************/
extern CCRndContext_t* gpRndContext;
extern mbedtls_ctr_drbg_context* gpRndState;
extern uint32_t gRunItStackSize;

/************************************************************
 *
 * function prototypes
 *
 ************************************************************/
void runIt_buffAlign32(uint8_t* pBuff, uint32_t **ppBuffAligned, uint32_t wantedBuffSize, const char * name);
int runIt_unhexify(unsigned char *obuf, const char *ibuf);
void runIt_hexify(unsigned char *obuf, const unsigned char *ibuf, int len);
int runIt_rndBufferRand(void *rng_state, unsigned char *output, size_t len);
int runIt_rand(void *rng_state, unsigned char *output, size_t len);
int runIt_free(RunItPtr *ptrElement);
int runIt_malloc(RunItPtr *ptrElement, size_t length);
uint32_t runIt_flashReadWrap(void* flashAddress, uint8_t *memDst, uint32_t sizeToRead, void* context);
int runIt_buildRandomBuffer(uint8_t *pBuf, size_t bufSize);
#endif //_RUN_INTEGRATION_HELPER_H_
