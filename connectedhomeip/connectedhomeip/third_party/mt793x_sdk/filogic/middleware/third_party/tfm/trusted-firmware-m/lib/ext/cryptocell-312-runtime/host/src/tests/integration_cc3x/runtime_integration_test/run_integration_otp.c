/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stdint.h"
#include "stdio.h"
#include <string.h>

#include "run_integration_pal_otp.h"
#include "run_integration_pal_log.h"
#include "run_integration_pal_reg.h"

#include "board_configs.h"
#include "test_pal_time.h"
#include "test_proj_defs.h"

#include "dx_env.h"
#include "dx_host.h"
#include "dx_nvm.h"
#include "dx_crys_kernel.h"
#include "dx_reg_base_host.h"

#include "cc_otp_defs.h"
#include "cc_regs.h"
#include "run_integration_otp.h"

#ifdef RUNIT_PIE_ENABLED
/* include sbrom data file to determine whether we are running system flows */
#include "bsv_integration_data_def.h"
#endif /* RUNIT_PIE_ENABLED */

#ifndef RUNIT_SECURE_BOOT_SKIP_BURN_OTP
#define RUNIT_SECURE_BOOT_SKIP_BURN_OTP 0
#endif
/************************************************************
 *
 * macros
 *
 ************************************************************/
/* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
#define RUNIT_WAIT_NVM_IDLE(regVal) \
                do {                                            \
                    uint32_t regVal;                                \
                    do {                                        \
                        regVal = RUNIT_READ_REG(CC_REG_OFFSET(HOST_RGF, NVM_IS_IDLE));            \
                        regVal = CC_REG_FLD_GET(0, NVM_IS_IDLE, VALUE, regVal);         \
                    }while( !regVal );                              \
                }while(0)


#define RUNIT_TEST_CALC_BUFF_ZEROS(wordBuf, buffWordSize, zeros) \
                do {\
                    int i = 0;\
                    int j = 0;\
                    int mask = 0;\
                    zeros = 0;\
                    for (i = 0; i< buffWordSize; i++) {\
                        for (j = 0; j<32; j++) {\
                            mask = 0x1;\
                            if (!(*(wordBuf+i) & (mask << j))) {\
                                zeros++;\
                            }\
                        }\
                    }\
                }

/************************************************************
 *
 * public functions
 *
 ************************************************************/


RunItError_t runIt_getLcs(unsigned int *lcs)
{
    unsigned int regVal = 0;

    /* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
    RUNIT_WAIT_NVM_IDLE(regVal);

    /* Read the LCS register */
    regVal = RUNIT_READ_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);

    *lcs = regVal;

    return RUNIT_ERROR__OK;
}

RunItError_t runIt_checkLcs(unsigned int lcs)
{
    unsigned int regVal = 0;

    /* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
    RUNIT_WAIT_NVM_IDLE(regVal);

    /* Read the LCS register */
    regVal = RUNIT_READ_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);

    /* Verify lcs */
    if(regVal != lcs)
    {
        RUNIT_PRINT_ERROR("actual LCS %d != expected LCS %d\n", regVal, lcs);
        return RUNIT_ERROR__FAIL;
    }

    return RUNIT_ERROR__OK;
}

RunItError_t runIt_burnOtp(unsigned int *otpBuf, unsigned int nextLcs)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if !RUNIT_SECURE_BOOT_SKIP_BURN_OTP
    unsigned int i = 0;

    /*  Perform SW reset to reach CM LCS */
    Test_ProjPerformPowerOnReset();

    RUNIT_PRINT_DBG("Restarted\n");
    /* Copy new OTP buffer */
    for (i = 0; i < TEST_OTP_SIZE_IN_WORDS; i++)
    {
        RUNIT_WRITE_OTP(i, otpBuf[i]);
        RUNIT_PRINT_DBG("0x%02x: 0x%08X\n", i, otpBuf[i]);

        Test_PalDelay(10);
    }

    /*  Perform SW reset after writing to OTP new values */
    Test_ProjPerformPowerOnReset();
    RUNIT_PRINT_DBG("Restarted\n");

    /* verify LCS */
    rc = runIt_checkLcs(nextLcs);
    if (rc == RUNIT_ERROR__OK)
    {
        RUNIT_PRINT_DBG("OTP burn succeeded with new LCS = 0x%02x \n", nextLcs);
    }
    else
    {
        RUNIT_PRINT_ERROR("Error: Failed to burn OTP!!\n");
    }
#endif /* !RUNIT_SECURE_BOOT_SKIP_BURN_OTP */
    return rc;
}

