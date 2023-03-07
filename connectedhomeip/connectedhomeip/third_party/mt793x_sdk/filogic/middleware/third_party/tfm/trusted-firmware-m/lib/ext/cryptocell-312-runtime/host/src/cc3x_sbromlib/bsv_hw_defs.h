/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef _BSV_HW_DEFS_H
#define _BSV_HW_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif


/* ********************** Macros ******************************* */

/* poll NVM register to assure that the NVM boot is finished (and LCS and the keys are valid) */
#define CC_BSV_WAIT_ON_NVM_IDLE_BIT(hwBaseAddress)                      \
    do {                                            \
        uint32_t regVal;                                \
        do {                                        \
            SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, NVM_IS_IDLE), regVal); \
            regVal = CC_REG_FLD_GET(0, NVM_IS_IDLE, VALUE, regVal);         \
        }while( !regVal );                              \
    }while(0)

/* poll on the AIB acknowledge bit */
#define CC_BSV_WAIT_ON_AIB_ACK_BIT(hwBaseAddress)       \
    do {                                                \
        uint32_t regVal;                                \
        do {                                            \
            SB_HAL_READ_REGISTER( SB_REG_ADDR(hwBaseAddress, AIB_FUSE_PROG_COMPLETED), regVal); \
        }while( !(regVal & 0x1 ));                      \
    }while(0)

/* check HUK error bit in LCS register */
#define CC_BSV_IS_OTP_HUK_ERROR(hwBaseAddress, regVal)                      \
    do {                                            \
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);      \
        regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_KDR_ZERO_CNT, regVal);        \
    }while(0)

/* check KPICV error bit in LCS register */
#define CC_BSV_IS_OTP_KPICV_ERROR(hwBaseAddress, regVal)                    \
    do {                                            \
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);      \
        regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_KPICV_ZERO_CNT, regVal);      \
    }while(0)

/* check KCEICV error bit in LCS register */
#define CC_BSV_IS_OTP_KCEICV_ERROR(hwBaseAddress, regVal)                   \
    do {                                            \
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);      \
        regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_KCEICV_ZERO_CNT, regVal);     \
    }while(0)

/* check KCP error bit in LCS register */
#define CC_BSV_IS_OTP_KCP_ERROR(hwBaseAddress, regVal)                      \
    do {                                            \
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);      \
        regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_PROV_ZERO_CNT, regVal);       \
    }while(0)

/* check KCE error bit in LCS register */
#define CC_BSV_IS_OTP_KCE_ERROR(hwBaseAddress, regVal)                      \
    do {                                            \
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, LCS_REG), regVal);      \
        regVal = CC_REG_FLD_GET(0, LCS_REG, ERROR_KCE_ZERO_CNT, regVal);        \
    }while(0)

/* read a word directly from OTP memory */
#define CC_BSV_READ_OTP_WORD(hwBaseAddress, otpAddr, otpData)                           \
    do {                                                                                \
        SB_HAL_READ_REGISTER((hwBaseAddress + CC_OTP_BASE_ADDR + otpAddr), otpData);    \
    }while(0)

/* write a word directly from OTP memory */
#define CC_BSV_WRITE_OTP_WORD(hwBaseAddress, otpAddr, otpData)                          \
    do {                                                                                \
        SB_HAL_WRITE_REGISTER((hwBaseAddress + CC_OTP_BASE_ADDR + otpAddr), otpData);   \
        CC_BSV_WAIT_ON_AIB_ACK_BIT(hwBaseAddress);                                      \
    }while(0)

/* calc OTP memory length:
   read RTL OTP address width. The supported sizes are 6 (for 2 Kbits),7,8,9,10,11 (for 64 Kbits).
   convert value parameter to addresses of 32b words */
#define CC_BSV_GET_OTP_LENGTH(hwBaseAddress, otpLength)                         \
    do {                                                \
        SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, OTP_ADDR_WIDTH_DEF), otpLength);    \
        otpLength = CC_REG_FLD_GET(0, OTP_ADDR_WIDTH_DEF, VALUE, otpLength);            \
        otpLength = (1 << otpLength);                               \
    }while(0)

/* check Hbk configuration in OTP memory */
#define CC_BSV_IS_HBK_FULL(hwBaseAddress, isHbkFull, error)                             \
    do {                                                        \
        error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_MANUFACTURE_FLAG_OFFSET, &isHbkFull);           \
        isHbkFull = CC_REG_FLD_GET2(0, OTP_MANUFACTURE_FLAG, HBK0_NOT_IN_USE, isHbkFull);           \
    }while(0)

/* check OEM RMA flag bit in OTP memory */
#define CC_BSV_IS_OEM_RMA_FLAG_SET(hwBaseAddress, isOemRmaFlag, error)                          \
    do {                                                        \
        error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_OEM_FLAG_OFFSET, &isOemRmaFlag);            \
        isOemRmaFlag = CC_REG_FLD_GET2(0, OTP_OEM_FLAG, OEM_RMA_MODE, isOemRmaFlag);                \
    }while(0)

/* check ICV RMA flag bit in OTP memory */
#define CC_BSV_IS_ICV_RMA_FLAG_SET(hwBaseAddress, isIcvRmaFlag, error)                          \
    do {                                                        \
        error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_OEM_FLAG_OFFSET, &isIcvRmaFlag);            \
        isIcvRmaFlag = CC_REG_FLD_GET2(0, OTP_OEM_FLAG, ICV_RMA_MODE, isIcvRmaFlag);                \
    }while(0)

/* poll on the crypto busy till it is = 0 */
#define CC_BSV_WAIT_ON_CRYPTO_BUSY()                                \
    do {                                            \
        uint32_t regVal=1;                              \
        do {                                        \
            SB_HAL_READ_REGISTER(SB_REG_ADDR(hwBaseAddress, CRYPTO_BUSY), regVal);  \
                }while( regVal );                               \
        }while(0)

/* check KPICV "Not In Use" bit in OTP memory */
#define CC_BSV_IS_KPICV_IN_USE(hwBaseAddress, isKpicvInUse, error)                          \
    do {                                                        \
        error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_MANUFACTURE_FLAG_OFFSET, &isKpicvInUse);        \
        isKpicvInUse = 0x1UL ^ (CC_REG_FLD_GET2(0, OTP_MANUFACTURE_FLAG, KPICV_NOT_IN_USE, isKpicvInUse));  \
    }while(0)

/* check KCEICV "Not In Use" bit in OTP memory */
#define CC_BSV_IS_KCEICV_IN_USE(hwBaseAddress, isKceicvInUse, error)                            \
    do {                                                        \
        error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_MANUFACTURE_FLAG_OFFSET, &isKceicvInUse);       \
        isKceicvInUse = 0x1UL ^ (CC_REG_FLD_GET2(0, OTP_MANUFACTURE_FLAG, KCEICV_NOT_IN_USE, isKceicvInUse));   \
    }while(0)

/* check KCP "Not In Use" bit in OTP memory */
#define CC_BSV_IS_KCP_IN_USE(hwBaseAddress, isKcpInUse, error)                      \
    do {                                                \
        error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_OEM_FLAG_OFFSET, &isKcpInUse);      \
        isKcpInUse = 0x1UL ^ (CC_REG_FLD_GET2(0, OTP_OEM_FLAG, KCP_NOT_IN_USE, isKcpInUse));    \
    }while(0)

/* check KCE "Not In Use" bit in OTP memory */
#define CC_BSV_IS_KCE_IN_USE(hwBaseAddress, isKceInUse, error)                      \
    do {                                                \
        error = CC_BsvOTPWordRead(hwBaseAddress, CC_OTP_OEM_FLAG_OFFSET, &isKceInUse);      \
        isKceInUse = 0x1UL ^ (CC_REG_FLD_GET2(0, OTP_OEM_FLAG, KCE_NOT_IN_USE, isKceInUse));    \
    }while(0)



/* ********************** Definitions ******************************* */

/* HW clocks */
#define CC_BSV_CLOCK_ENABLE     0x1UL
#define CC_BSV_CLOCK_DISABLE        0x0UL

/* Peripheral ID registers values */
#define CC_BSV_PID_0_VAL        0x000000C0UL
#define CC_BSV_PID_1_VAL        0x000000B0UL
#define CC_BSV_PID_2_VAL        0x0000000BUL
#define CC_BSV_PID_3_VAL        0x00000000UL
#define CC_BSV_PID_4_VAL        0x00000004UL
#define CC_BSV_PID_SIZE_WORDS       5

/* Component ID registers values */
#define CC_BSV_CID_0_VAL        0x0DUL
#define CC_BSV_CID_1_VAL        0xF0UL
#define CC_BSV_CID_2_VAL        0x05UL
#define CC_BSV_CID_3_VAL        0xB1UL
#define CC_BSV_CID_SIZE_WORDS       4

/* Secret Key */
#define CC_BSV_SK_ERASER_VAL            0xFFFFFFFFUL
#define CC_BSV_256B_SK_INTEGRITY_ERASER_VAL 0xFFUL
#define CC_BSV_128B_SK_INTEGRITY_ERASER_VAL 0x7FUL

#ifdef __cplusplus
}
#endif

#endif



