/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_PAL_PERF_H_
#define _CC_PAL_PERF_H_

#include <string.h>

#ifdef LIB_PERF
#include "cc_pal_perf_plat.h"
#endif

typedef enum
{
    PERF_TEST_TYPE_AES_INIT,
    PERF_TEST_TYPE_AES_SET_KEY,
    PERF_TEST_TYPE_AES_BLOCK,
    PERF_TEST_TYPE_AES_FIN,
    PERF_TEST_TYPE_CC_AES_INIT,
    PERF_TEST_TYPE_CC_AES_BLOCK,
    PERF_TEST_TYPE_CC_AES_FIN,
    PERF_TEST_TYPE_HW_CMPLT ,
    PERF_TEST_TYPE_PAL_MAP,
    PERF_TEST_TYPE_PAL_UNMAP,
    PERF_TEST_TYPE_MLLI_BUILD,
    PERF_TEST_TYPE_SYM_DRV_INIT,
    PERF_TEST_TYPE_SYM_DRV_PROC,
    PERF_TEST_TYPE_SYM_DRV_FIN,
    PERF_TEST_TYPE_CC_HASH_INIT,
    PERF_TEST_TYPE_CC_HASH_UPDATE,
    PERF_TEST_TYPE_CC_HASH_FIN,
    PERF_TEST_TYPE_CC_HMAC_INIT,
    PERF_TEST_TYPE_CC_HMAC_UPDATE,
    PERF_TEST_TYPE_CC_HMAC_FIN,
    PERF_TEST_TYPE_CMPLT_SLEEP,
    PERF_TEST_TYPE_CC_ECDSA_SIGN_INIT,
    PERF_TEST_TYPE_CC_ECDSA_SIGN_UPDATE,
    PERF_TEST_TYPE_CC_ECDSA_SIGN_FINISH,
    PERF_TEST_TYPE_CC_ECDSA_VERIFY_INIT,
    PERF_TEST_TYPE_CC_ECDSA_VERIFY_UPDATE,
    PERF_TEST_TYPE_CC_ECDSA_VERIFY_FINISH,
    PERF_TEST_TYPE_PKA_EC_WRST_SCALAR_MULT,
    PERF_TEST_TYPE_CALC_SIGNATURE,
    PERF_TEST_TYPE_PKA_SCALAR_MULT_AFF,
    PERF_TEST_TYPE_PKA_SCALAR_MULT_SCA,
    PERF_TEST_TYPE_PKA_ECDSA_VERIFY,
    PERF_TEST_TYPE_PKA_ModExp = 0x30,
    PERF_TEST_TYPE_TEST_BASE = 0x100,
    PERF_TEST_TYPE_MAX,
    PERF_TEST_TYPE_RESERVE32 = 0x7FFFFFFF
} CCPalPerfType_t;

#ifdef LIB_PERF

static char* CC_PalPerfTypeStr(CCPalPerfType_t type, char* pStr, uint32_t buffLen)
{
    #define CCPalPerfTypeStr_str(a) case a: strncpy(pStr, #a + strlen("PERF_TEST_TYPE_"), buffLen); break;

    switch (type)
    {
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_AES_INIT);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_AES_SET_KEY);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_AES_BLOCK);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_AES_FIN);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_AES_INIT);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_AES_BLOCK);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_AES_FIN);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_HW_CMPLT );
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_PAL_MAP);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_PAL_UNMAP);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_MLLI_BUILD);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_SYM_DRV_INIT);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_SYM_DRV_PROC);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_SYM_DRV_FIN);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_HASH_INIT);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_HASH_UPDATE);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_HASH_FIN);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_HMAC_INIT);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_HMAC_UPDATE);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_HMAC_FIN);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CMPLT_SLEEP);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_ECDSA_SIGN_INIT);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_ECDSA_SIGN_UPDATE);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_ECDSA_SIGN_FINISH);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_ECDSA_VERIFY_INIT);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_ECDSA_VERIFY_UPDATE);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CC_ECDSA_VERIFY_FINISH);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_PKA_EC_WRST_SCALAR_MULT);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_CALC_SIGNATURE);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_PKA_SCALAR_MULT_AFF);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_PKA_SCALAR_MULT_SCA);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_PKA_ECDSA_VERIFY);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_PKA_ModExp);
        CCPalPerfTypeStr_str(PERF_TEST_TYPE_TEST_BASE);
        default: strncpy(pStr, "PERF_TEST_TYPE_UNKNOWN", buffLen);
    }

    return pStr;
}

#define CC_PAL_PERF_INIT                            CC_PalPerfInit
#define CC_PAL_PERF_OPEN_NEW_ENTRY(num, type)       num = CC_PalPerfOpenNewEntry(type)
#define CC_PAL_PERF_CLOSE_ENTRY(num, type)          CC_PalPerfCloseEntry(num, type)
#define CC_PAL_PERF_DUMP                            CC_PalPerfDump
#define CC_PAL_PERF_FIN                             CC_PalPerfFin

/**
 * @brief   initialize performance test mechanism
 *
 * @param[in]
 * *
 * @return None
 */
void CC_PalPerfInit(void);


/**
 * @brief   opens new entry in perf buffer to record new entry
 *
 * @param[in] entryType -  entry type (defined in cc_pal_perf.h) to be recorded in buffer
 *
 * @return A non-zero value in case of failure.
 */
CCPalPerfData_t CC_PalPerfOpenNewEntry(CCPalPerfType_t entryType);


/**
 * @brief   closes entry in perf buffer previously opened by CC_PalPerfOpenNewEntry
 *
 * @param[in] idx -  index of the entry to be closed, the return value of CC_PalPerfOpenNewEntry
 * @param[in] entryType -  entry type (defined in cc_pal_perf.h) to be recorded in buffer
 *
 * @return A non-zero value in case of failure.
 */
void CC_PalPerfCloseEntry(CCPalPerfData_t idx, CCPalPerfType_t entryType);


/**
 * @brief   dumps the performance buffer
 *
 * @param[in] None
 *
 * @return None
 */
void CC_PalPerfDump(void);


/**
 * @brief   terminates resources used for performance tests
 *
 * @param[in]
 * *
 * @return None
 */
void CC_PalPerfFin(void);

#else  //LIB_PERF
#define CC_PAL_PERF_INIT()
#define CC_PAL_PERF_OPEN_NEW_ENTRY(num, type)  (num=num)
#define CC_PAL_PERF_CLOSE_ENTRY(num, type)
#define CC_PAL_PERF_DUMP()
#define CC_PAL_PERF_FIN()


typedef unsigned int CCPalPerfData_t;

#endif  //LIB_PERF


#endif /*_CC_PAL_PERF_H__*/

