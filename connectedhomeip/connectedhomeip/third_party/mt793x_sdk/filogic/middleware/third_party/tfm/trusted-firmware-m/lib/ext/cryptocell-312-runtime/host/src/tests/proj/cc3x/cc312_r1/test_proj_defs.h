/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TEST_PROJ_DEFS_H_
#define _TEST_PROJ_DEFS_H_

#include <stdint.h>

#include "dx_reg_base_host.h"
#include "dx_host.h"
#include "dx_crys_kernel.h"
#include "dx_env.h"
#include "dx_nvm.h"
#include "cc_regs.h"

#define BITS_IN_32BIT_WORD 32

/* Peripheral ID registers values */
#define TEST_PID_0_VAL      0x000000C0UL
#define TEST_PID_1_VAL      0x000000B0UL
#define TEST_PID_2_VAL      0x0000001BUL
#define TEST_PID_3_VAL      0x00000000UL
#define TEST_PID_4_VAL      0x00000004UL
#define TEST_PID_SIZE_WORDS     5

/* Component ID registers values */
#define TEST_CID_0_VAL      0x0DUL
#define TEST_CID_1_VAL      0xF0UL
#define TEST_CID_2_VAL      0x05UL
#define TEST_CID_3_VAL      0xB1UL
#define TEST_CID_SIZE_WORDS     4

/* HW KEYS */
#define TEST_SB_HUK_KEY            0
#define TEST_SB_KCP_KEY            1
#define TEST_SB_KCE_KEY            2
#define TEST_SB_KPICV_KEY          3
#define TEST_SB_KCEICV_KEY         4


#define TEST_PROJ_LCS_CM 0
#define TEST_PROJ_LCS_DM 1
#define TEST_PROJ_LCS_SECURE 5
#define TEST_PROJ_LCS_RMA 7
#define INVALID_LCS     (-1)

#define TST_SET_ENV_TO_SECURE()                     \
    do {                                \
        TEST_WRITE_TEE_ENV_REG( DX_ENV_APB_PPROT_OVERRIDE_REG_OFFSET, 0x9);  \
    }while(0)

/* poll NVM register to be assure that the NVM boot is finished (and LCS and the keys are valid) */
#define WAIT_NVM_IDLE() \
    do {                                            \
        uint32_t regVal;                                \
        do {                                        \
            regVal = TEST_READ_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, NVM_IS_IDLE));            \
            regVal = CC_REG_FLD_GET(0, NVM_IS_IDLE, VALUE, regVal);         \
        }while( !regVal );                              \
    }while(0)

/* turn off DFA  */
#define TURN_DFA_OFF() {\
    uint32_t regVal;                            \
    regVal = TEST_READ_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS));          \
    CC_REG_FLD_SET(0, HOST_AO_LOCK_BITS, HOST_FORCE_DFA_ENABLE, regVal, 0); \
    CC_REG_FLD_SET(0, HOST_AO_LOCK_BITS, HOST_DFA_ENABLE_LOCK, regVal, 1);  \
    TEST_WRITE_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS)  ,regVal );    \
    TEST_WRITE_TEE_CC_REG(CC_REG_OFFSET(HOST_RGF, AES_DFA_IS_ON)  ,0 );         \
}

/****************************************************************************/
/*                              External API                                */
/*
 * @brief This function Maps the proj HW.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_ProjMap(void);

/****************************************************************************/
/*
 * @brief This function Maps the proj HW.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return none.
 */
void Test_ProjUnmap(void);

/****************************************************************************/
/*
 * @brief This function reads LCS register and verifies that LCS value is as expected.
 *
 * @param[in] LCS correct value.
 *
 * @param[out]
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_ProjCheckLcs(uint32_t nextLcs);


/****************************************************************************/
/*
 * @brief This function reads LCS register, verifies that LCS value is as
 *          expected and no HW errors exist in HUK, Kcp* and Kce*.
 *
 * @param[in] LCS correct value.
 *
 * @param[out]
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_ProjCheckLcsAndError(uint32_t  nextLcs);


/****************************************************************************/
/*
 * @brief This function returns the current LCS value.
 *
 * @param[in]
 *
 * @param[out] LCS value.
 *
 * @return 0.
 */
uint32_t Test_ProjGetLcs(uint32_t *lcs);


/****************************************************************************/
/*
 * @brief This function performs global reset of CC, AO and environment registers.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return
 */
void Test_ProjPerformPowerOnReset(void);

/****************************************************************************/
/*
 * @brief This function resets CC and AO registers.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return
 */
void Test_ProjPerformColdReset(void);

/****************************************************************************/
/*
 * @brief This function resets CC registers.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return
 */
void Test_ProjPerformWarmReset(void);

/****************************************************************************/
/*
 * @brief This function resets CC registers.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return
 */
uint32_t Test_ProjVerifyPIDReg(void);

/****************************************************************************/
/*
 * @brief This function resets CC registers.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return
 */
uint32_t Test_ProjVerifyCIDReg(void);


#endif /* _TEST_PROJ_DEFS_H_ */
