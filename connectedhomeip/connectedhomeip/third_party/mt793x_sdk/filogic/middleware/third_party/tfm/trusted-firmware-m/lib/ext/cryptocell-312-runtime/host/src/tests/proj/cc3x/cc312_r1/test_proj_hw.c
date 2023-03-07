/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "test_proj.h"
#include "test_proj_defs.h"
#include "test_pal_time.h"
#include "test_pal_log.h"

#include "dx_id_registers.h"

uint32_t Test_ProjCheckLcs(uint32_t  nextLcs)
{
    uint32_t regVal = 0;

    /* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
    WAIT_NVM_IDLE();

    /* Read the LCS register */
    regVal = TEST_READ_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);

    /* Verify lcs */
    if(regVal != nextLcs) {
        TEST_PRINTF_ERROR("actual LCS %d != expected LCS %d", regVal, nextLcs);
        return TEST_COMPARE_ERR;
    }

    return TEST_OK;
}


uint32_t Test_ProjCheckLcsAndError(uint32_t  nextLcs)
{
    uint32_t regVal = 0;

    /* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
    WAIT_NVM_IDLE();

    /* Read the LCS register */
    regVal = TEST_READ_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);

    /* Verify lcs */
    if(regVal != nextLcs) {
                TEST_PRINTF_ERROR("actual LCS %d != expected LCS %d", regVal, nextLcs);
        return TEST_COMPARE_ERR;
    }

    if ((CC_REG_FLD_GET(0, LCS_REG, ERROR_KDR_ZERO_CNT, regVal) != 0) ||
            (CC_REG_FLD_GET(0, LCS_REG, ERROR_KPICV_ZERO_CNT, regVal) != 0) ||
            (CC_REG_FLD_GET(0, LCS_REG, ERROR_KCEICV_ZERO_CNT, regVal) != 0) ||
        (CC_REG_FLD_GET(0, LCS_REG, ERROR_PROV_ZERO_CNT, regVal) != 0) ||
            (CC_REG_FLD_GET(0, LCS_REG, ERROR_KCE_ZERO_CNT, regVal) != 0)) {
                TEST_PRINTF_ERROR("regVal 0x%x indicates error for LCS %d", regVal, nextLcs);
        return TEST_HW_FAIL_ERR;
    }

    return TEST_OK;
}


uint32_t Test_ProjGetLcs(uint32_t  *lcs)
{
    uint32_t regVal = 0;

    /* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
    WAIT_NVM_IDLE();

    /* Read the LCS register */
    regVal = TEST_READ_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, LCS_REG));
    regVal = CC_REG_FLD_GET(0, LCS_REG, LCS_REG, regVal);

    *lcs = regVal;

    return TEST_OK;
}
/* Global Reset of CC and AO and env regs */
void Test_ProjPerformPowerOnReset(void)
{
    TEST_WRITE_TEE_ENV_REG(DX_ENV_CC_POR_N_ADDR_REG_OFFSET , 0x1UL);
    Test_PalDelay(1000);

    /* poll NVM register to assure that the NVM boot is finished (and LCS and the keys are valid) */
    WAIT_NVM_IDLE();

#ifdef BIG__ENDIAN
    /* Set DMA endianess to big */
    TEST_WRITE_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    TEST_WRITE_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif

    /* turn off the DFA since Cerberus doesn't support it */
    TURN_DFA_OFF();
    return;

}


/* Reset both CC and AO regs */
void Test_ProjPerformColdReset(void)
{
    TEST_WRITE_TEE_ENV_REG(DX_ENV_CC_COLD_RST_REG_OFFSET , 0x1UL);
    Test_PalDelay(1000);

#ifdef BIG__ENDIAN
    /* Set DMA endianess to big */
    TEST_WRITE_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    TEST_WRITE_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif
    /* turn off the DFA since Cerberus doesn't support it */
    TURN_DFA_OFF();
    return;
}


/* Reset only CC regs */
void Test_ProjPerformWarmReset(void)
{
    TEST_WRITE_TEE_ENV_REG(DX_ENV_CC_RST_N_REG_OFFSET , 0x1UL);
    Test_PalDelay(1000);

#ifdef BIG__ENDIAN
    /* Set DMA endianess to big */
    TEST_WRITE_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    TEST_WRITE_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif
    return;

}

uint32_t Test_ProjVerifyPIDReg(void)
{
    uint32_t i = 0;
    uint32_t pidReg = 0;
    uint32_t pidValTable[TEST_PID_SIZE_WORDS][2] = {
            {DX_PERIPHERAL_ID_0_REG_OFFSET, TEST_PID_0_VAL},
            {DX_PERIPHERAL_ID_1_REG_OFFSET, TEST_PID_1_VAL},
            {DX_PERIPHERAL_ID_2_REG_OFFSET, TEST_PID_2_VAL},
            {DX_PERIPHERAL_ID_3_REG_OFFSET, TEST_PID_3_VAL},
            {DX_PERIPHERAL_ID_4_REG_OFFSET, TEST_PID_4_VAL},
            };


    /* verify peripheral ID (PIDR) */
    for (i=0; i<TEST_PID_SIZE_WORDS; i++) {
        pidReg = TEST_READ_TEE_CC_REG(pidValTable[i][0]);
        if (pidReg != pidValTable[i][1]) {
            TEST_PRINTF_ERROR("ERROR: verify peripheral ID (PIDR) "
                    "%d value=0x%08x expected 0x%08x\n", i,
                pidReg, pidValTable[i][1]);
            return 1;
        }
    }
    return 0;
}
/******************************************************************************/
uint32_t Test_ProjVerifyCIDReg(void)
{
    uint32_t i =0;
    uint32_t cidReg = 0;

    uint32_t cidValTable[TEST_CID_SIZE_WORDS][2] = {
            {DX_COMPONENT_ID_0_REG_OFFSET, TEST_CID_0_VAL},
            {DX_COMPONENT_ID_1_REG_OFFSET, TEST_CID_1_VAL},
            {DX_COMPONENT_ID_2_REG_OFFSET, TEST_CID_2_VAL},
            {DX_COMPONENT_ID_3_REG_OFFSET, TEST_CID_3_VAL},
            };

    /* verify component ID (CIDR) */
    for (i=0; i<TEST_CID_SIZE_WORDS; i++) {
        cidReg = TEST_READ_TEE_CC_REG(cidValTable[i][0]);
        if (cidReg != cidValTable[i][1]) {
            TEST_PRINTF_ERROR("ERROR: verify component ID (CIDR) "
                    "%d value=0x%08x expected 0x%08x", i,
                cidReg, cidValTable[i][1]);
            return 1;
        }
    }
    return 0;
}

