/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* system includes */
#include <stdlib.h>
#include <string.h>


/* cc_productionLib */
#include "cc_cmpu.h"
#include "cc_pal_types.h"

/* shared */
#include "dx_reg_base_host.h"

/* pal/hal */
#include "board_configs.h"
#include "test_pal_time.h"
#include "test_pal_mem.h"
#include "test_pal_thread.h"
#include "test_proj.h"
#include "test_proj_defs.h"
#include "test_proj_otp.h"

/* internal */
#include "cmpu_integration_test.h"
#include "cmpu_integration_helper.h"
#include "cmpu_integration_pal_log.h"

/************************************************************
 *
 * defines
 *
 ************************************************************/
#define CMPUIT_PTHREAD_STACK_MIN                 16384

/************************************************************
 *
 * macros
 *
 ************************************************************/

/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static CmpuItError_t cmpuIt_cmpuTest(void);
static void* cmpuIt_executer(void *params);

/************************************************************
 *
 * variables
 *
 ************************************************************/
static const CCCmpuUniqueDataType_t  UNIQUE_DATA_TYPE = CMPU_UNIQUE_IS_USER_DATA;
static const uint8_t USER_DATA[PROD_UNIQUE_BUFF_SIZE] = { 0x06, 0xb5, 0x3e, 0xe9, 0x2b, 0x75, 0xde, 0x4a, 0x59, 0xb3, 0x46, 0x9b, 0xf7, 0x0a, 0x72, 0x1b, };
static const CCPlainAsset_t KCP_BUFF = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x10, };
static const CCAssetPkg_t KCE_BUFF = { 0x50726f64, 0x00010000, 0x00000010, 0x52657631, 0x52657632, 0xab6b3e7f, 0x7f4544b2, 0xe53d0952, 0xb66e49b3, 0x5e639bd6, 0x731bd98a, 0xb1ed2ae4, 0x36371f61, 0x8f4e9404, 0x36262e12, 0x267232c8, };
static const uint32_t ICV_DCU_DEFAULT_LOCK[PROD_DCU_LOCK_WORD_SIZE] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };

/* pass result between executer and main thread */
CmpuItError_t g_cmpuIt_executerRc = CMPUIT_ERROR__FAIL;

static uint32_t CMPUIT_OTP_CM_VALUES[] = {

    /*  [0x00-0x07]: 256bit Device root key (HUK) */
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x08-0x0B]: 128bit ICV Provosioning secret (Kpicv) */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x0C-0x0F]: 128bit ICV Provosioning secret (Kceicv) */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x10]: manufacturer programmed flag */
    0x00000000,
    /*  [0x11-0x18]: a single 256b SHA256 (Hbk), or, two 128b truncated SHA256 HBK0 and HBK1 */
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x19-0x1C]: 128bit OEM provisioning secret (Kcp) */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x1D-0x20]: 128bit OEM code encryption secret (Kce) */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x21]: OEM programmed flag */
    0x00000000,
    /*  [0x22-0x26]: Hbk trusted FW min version */
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x27]: general purpose configuration flag */
    0x00000000,
    /*  [0x28-0x2B] - 128b DCU lock mask */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};


/************************************************************
 *
 * static functions
 *
 ************************************************************/
static CmpuItError_t cmpuIt_cmpuTest(void)
{
    CmpuItError_t rc = CMPUIT_ERROR__OK;

    unsigned long ccHwRegBaseAddr = processMap.processTeeHwRegBaseAddr;
    uint8_t *pWorkspaceBuf = NULL;
    uint32_t *pWorkspace = NULL;
    CCCmpuData_t cmpuData;

    const char* TEST_NAME = "CMPU";
    CMPUIT_TEST_START(TEST_NAME);

    /* workspace should be 32bit aligned */
    ALLOC32(pWorkspaceBuf, pWorkspace, CMPU_WORKSPACE_MINIMUM_SIZE);

    /* populate cmpuData with data */
    cmpuData.uniqueDataType = UNIQUE_DATA_TYPE;
    memcpy(cmpuData.uniqueBuff.userData, USER_DATA, sizeof(USER_DATA));
    cmpuData.kpicvDataType = ASSET_PLAIN_KEY;
    memcpy(cmpuData.kpicv.plainAsset, KCP_BUFF, sizeof(cmpuData.kpicv.plainAsset));
    cmpuData.kceicvDataType = ASSET_PKG_KEY;
    memcpy(cmpuData.kceicv.pkgAsset, KCE_BUFF, sizeof(cmpuData.kceicv.pkgAsset));
    cmpuData.icvMinVersion = 5;
    cmpuData.icvConfigWord = 0x12345678;
    memcpy(cmpuData.icvDcuDefaultLock, ICV_DCU_DEFAULT_LOCK, sizeof(ICV_DCU_DEFAULT_LOCK));

    /* debug pointers */
    CMPUIT_PRINT_DBG("pWorkspace[%p]\n", pWorkspace);
    CMPUIT_PRINT_DBG("ccHwRegBaseAddr[%p]\n", (void*)ccHwRegBaseAddr);
    CMPUIT_PRINT_DBG("CMPU_WORKSPACE_MINIMUM_SIZE[%d]\n", CMPU_WORKSPACE_MINIMUM_SIZE);

    /* call API */
    CMPUIT_ASSERT_WITH_RESULT(CCProd_Cmpu(ccHwRegBaseAddr,
                                          &cmpuData,
                                          (unsigned long)pWorkspace,
                                          CMPU_WORKSPACE_MINIMUM_SIZE), CC_OK);

    /* Perform SW reset to reach DM LCS */
    Test_ProjPerformPowerOnReset();
    Test_PalDelay(1000);

    /* verify LCS */
    CMPUIT_ASSERT(Test_ProjCheckLcs(TEST_PROJ_LCS_DM) == 0);

bail:
    FREE_IF_NOT_NULL(pWorkspaceBuf);

    CMPUIT_TEST_RESULT(TEST_NAME);
    return rc;
}

/**
 * @brief               Executor function. Called from a side thread to perform the sequence of tests.
 * @note                This is a workaround the issue that CC API requires DMA-able stack.
 *                      When using Test_PalThreadCreate we are able to ensure that the stack is DMA-able.
 * @param params        Not used
 */
static void* cmpuIt_executer(void *params)
{
    CmpuItError_t rc = CMPUIT_ERROR__OK;
    const char* TEST_NAME = "All Tests";

    CC_UNUSED_PARAM(params);

    CMPUIT_PRINT("cc312 cmpu integration test\n");
    CMPUIT_PRINT("---------------------------------------------------------------\n");


    CMPUIT_TEST_START(TEST_NAME);

    rc += cmpuIt_cmpuTest();

    g_cmpuIt_executerRc = rc;

    CMPUIT_TEST_RESULT(TEST_NAME);

    /* continue to next test only if all passed */
    CMPUIT_ASSERT(rc == CMPUIT_ERROR__OK);

bail:
    return NULL;
}

CmpuItError_t cmpuIt_all(void)
{
    CmpuItError_t rc = CMPUIT_ERROR__OK;
    ThreadHandle threadHandle = NULL;
    const char* TASK_NAME = "cmpuIt_executer";
    uint32_t priority = Test_PalGetDefaultPriority();

    /* init platform and map memory */
    CMPUIT_ASSERT(Test_ProjInit() == 0);
    CMPUIT_ASSERT(Test_ProjBurnOtp(CMPUIT_OTP_CM_VALUES, TEST_PROJ_LCS_CM, TEST_CHIP_STATE_NOT_INITIALIZED) == 0);

    /* Create a task that will allocate a DMA -able stack */
    threadHandle = Test_PalThreadCreate(CMPUIT_PTHREAD_STACK_MIN,
                                        cmpuIt_executer,
                                        priority,
                                        NULL,
                                        (char*)TASK_NAME,
                                        sizeof(TASK_NAME),
                                        true);

    /* Verify task was created successfully */
    CMPUIT_ASSERT(threadHandle != NULL);

    /* Wait for task to complete */
    Test_PalThreadJoin(threadHandle, NULL);

    /* Finalize task's resources */
    Test_PalThreadDestroy(threadHandle);

    /* Read result code */
    rc = g_cmpuIt_executerRc;

bail:
    /* Free platform */
    Test_ProjFree();

    return rc;
}

#if defined(DX_PLAT_ZYNQ7000)
int main(int argc, char** argv)
{
    ((void)argc);
    ((void)argv);

    cmpuIt_all();

    return 0;
}
#endif
