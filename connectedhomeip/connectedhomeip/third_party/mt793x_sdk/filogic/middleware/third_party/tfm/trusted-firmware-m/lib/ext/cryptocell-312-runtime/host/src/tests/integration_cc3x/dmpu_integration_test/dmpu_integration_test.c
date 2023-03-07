/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* system includes */
#include "dmpu_integration_test.h"

#include <stdlib.h>
#include <string.h>


/* cc_productionLib */
#include "cc_dmpu.h"
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
#include "dmpu_integration_helper.h"
#include "dmpu_integration_pal_log.h"

/************************************************************
 *
 * defines
 *
 ************************************************************/
#define DMPUIT_PTHREAD_STACK_MIN                 16384

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
static DmpuItError_t dmpuIt_dmpuTest(void);
static void* dmpuIt_executer(void *params);

static const uint8_t HBK[] = {0xe4, 0xf3, 0x28, 0x86, 0xdd, 0xf9, 0x7a, 0xcc, 0x8d, 0x27, 0xc6, 0x16, 0x85, 0x4c, 0xc9, 0xa1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};
static const CCAssetPkg_t KCP_BUFF_ASSET = { 0x50726f64, 0x00010000, 0x00000010, 0x52657631, 0x52657632, 0x7f37da03, 0x2ed0e4cd, 0x36ec18c0, 0x40b8680d, 0xa0178f19, 0x907ede6b, 0x53c71ac2, 0xd36d22f5, 0x3ba4d98b, 0x457644de, 0x7a609102 };
static const CCAssetPkg_t KCE_BUFF_ASSET = { 0x50726f64, 0x00010000, 0x00000010, 0x52657631, 0x52657632, 0x5dfa0a1e, 0x958aa4be, 0xe9b43a65, 0x67d057c5, 0x286066f0, 0x45a84078, 0xedb95c0b, 0xf6f194c5, 0x2d37d205, 0x8e4e6b52, 0x96d44168 };
static const uint32_t ICV_DCU_DEFAULT_LOCK[PROD_DCU_LOCK_WORD_SIZE] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };

/************************************************************
 *
 * variables
 *
 ************************************************************/
/* pass result between executer and main thread */
DmpuItError_t g_dmpuIt_executerRc = DMPUIT_ERROR__FAIL;

static uint32_t DMPUIT_OTP_DM_VALUES[] = {
    /*  [0x00-0x07]: 256bit Device root key (HUK) */
    0xb1467c29, 0xf2ad05eb, 0x385e8808, 0x423833d1, 0xa9698203, 0x7c9f71d6, 0x63a605e8, 0x460a5ab1,
    /*  [0x08-0x0B]: 128bit ICV Provosioning secret (Kpicv) */
    0x14131211, 0x18171615, 0x1c1b1a19, 0x101f1e1d,
    /*  [0x0C-0x0F]: 128bit ICV Provosioning secret (Kceicv) */
    0x222222ff, 0xffffffff, 0xff1111ff, 0xffffffff,
    /*  [0x10]: manufacturer programmed flag */
    0x801e508c,
    /*  [0x11-0x14]:  HBK0 */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x15-0x18]:  HBK1 */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x19-0x1C]: 128bit OEM provisioning secret (Kcp) */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x1D-0x20]: 128bit OEM code encryption secret (Kce) */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /*  [0x21]: OEM programmed flag */
    0x00000000,
    /*  [0x22-0x23]: Hbk0 trusted FW min version */
    0x00000000, 0x00000000,
    /*  [0x24-0x26]: Hbk1 trusted FW min version */
    0x00000000, 0x00000000, 0x00000000,
    /*  [0x27]: general purpose configuration flag */
    0x12345678,
    /*  [0x28-0x2B] - 128b DCU lock mask */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static DmpuItError_t dmpuIt_dmpuTest(void)
{
    DmpuItError_t rc = DMPUIT_ERROR__OK;

    unsigned long ccHwRegBaseAddr = processMap.processTeeHwRegBaseAddr;
    uint8_t *pWorkspaceBuf = NULL;
    uint32_t *pWorkspace = NULL;
    CCDmpuData_t dmpuData;

    const char* TEST_NAME = "DMPU";
    DMPUIT_TEST_START(TEST_NAME);

    /* workspace should be 32bit aligned */
    ALLOC32(pWorkspaceBuf, pWorkspace, DMPU_WORKSPACE_MINIMUM_SIZE);

    /* populate dmpuData with data */
    dmpuData.hbkType = DMPU_HBK_TYPE_HBK;
    memcpy(dmpuData.hbkBuff.hbk, HBK, sizeof(HBK));
    dmpuData.kcpDataType = ASSET_PKG_KEY;
    memcpy(dmpuData.kcp.pkgAsset, KCP_BUFF_ASSET, sizeof(dmpuData.kcp.pkgAsset));
    dmpuData.kceDataType = ASSET_PKG_KEY;
    memcpy(dmpuData.kce.pkgAsset, KCE_BUFF_ASSET, sizeof(dmpuData.kce.pkgAsset));
    dmpuData.oemMinVersion = 5;
    memcpy(dmpuData.oemDcuDefaultLock, ICV_DCU_DEFAULT_LOCK, sizeof(ICV_DCU_DEFAULT_LOCK));

    /* debug pointers */
    DMPUIT_PRINT_DBG("pWorkspace[%p]\n", pWorkspace);
    DMPUIT_PRINT_DBG("ccHwRegBaseAddr[%p]\n", (void*)ccHwRegBaseAddr);
    DMPUIT_PRINT_DBG("DMPU_WORKSPACE_MINIMUM_SIZE[%d]\n", DMPU_WORKSPACE_MINIMUM_SIZE);

    /* call API */
    DMPUIT_ASSERT_WITH_RESULT(CCProd_Dmpu(ccHwRegBaseAddr,
                              &dmpuData,
                              (unsigned long)pWorkspace,
                              DMPU_WORKSPACE_MINIMUM_SIZE), CC_OK);

    /* Perform SW reset to reach DM LCS */
    Test_ProjPerformPowerOnReset();
    Test_PalDelay(1000);

    /* verify LCS */
    DMPUIT_ASSERT(Test_ProjCheckLcs(TEST_PROJ_LCS_SECURE) == 0);

bail:
    FREE_IF_NOT_NULL(pWorkspaceBuf);

    DMPUIT_TEST_RESULT(TEST_NAME);
    return rc;
}

/**
 * @brief               Executor function. Called from a side thread to perform the sequence of tests.
 * @note                This is a workaround the issue that CC API requires DMA-able stack.
 *                      When using Test_PalThreadCreate we are able to ensure that the stack is DMA-able.
 * @param params        Not used
 */
static void* dmpuIt_executer(void *params)
{
    DmpuItError_t rc = DMPUIT_ERROR__OK;
    const char* TEST_NAME = "All Tests";

    CC_UNUSED_PARAM(params);

    DMPUIT_PRINT("cc312 dmpu integration test\n");
    DMPUIT_PRINT("---------------------------------------------------------------\n");


    DMPUIT_TEST_START(TEST_NAME);

    rc += dmpuIt_dmpuTest();

    g_dmpuIt_executerRc = rc;

    DMPUIT_TEST_RESULT(TEST_NAME);

    /* continue to next test only if all passed */
    DMPUIT_ASSERT(rc == DMPUIT_ERROR__OK);

bail:
    return NULL;
}

DmpuItError_t dmpuIt_all(void)
{
    DmpuItError_t rc = DMPUIT_ERROR__OK;
    ThreadHandle threadHandle = NULL;
    const char* TASK_NAME = "dmpuIt_executer";
    uint32_t priority = Test_PalGetDefaultPriority();

    /* init platform and map memory */
    DMPUIT_ASSERT(Test_ProjInit() == 0);
    DMPUIT_ASSERT(Test_ProjBurnOtp(DMPUIT_OTP_DM_VALUES, TEST_PROJ_LCS_DM, TEST_CHIP_STATE_TEST) == 0);

    /* Create a task that will allocate a DMA -able stack */
    threadHandle = Test_PalThreadCreate(DMPUIT_PTHREAD_STACK_MIN,
                                        dmpuIt_executer,
                                        priority,
                                        NULL,
                                        (char*)TASK_NAME,
                                        sizeof(TASK_NAME),
                                        true);

    /* Verify task was created successfully */
    DMPUIT_ASSERT(threadHandle != NULL);

    /* Wait for task to complete */
    Test_PalThreadJoin(threadHandle, NULL);

    /* Finalize task's resources */
    Test_PalThreadDestroy(threadHandle);

    /* Read result code */
    rc = g_dmpuIt_executerRc;

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

    dmpuIt_all();

    return 0;
}
#endif
