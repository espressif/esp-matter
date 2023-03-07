/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef PKA_HW_DEFS_H
#define PKA_HW_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"
#include "dx_crys_kernel.h"
#include "cc_regs.h"
#include "cc_hal_plat.h"
#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "cc_bitops.h"
#include "pka_defs.h"
#include "cc_util_pm.h"


/* PKA OPCODE register fields positions (low bit position) */
#define PKA_OPCODE_RES_OPERAND_MSBIT_OFFSET   5
#define PKA_OPCODE_TAG_POS                        CC_REG_BIT_SHIFT(OPCODE, TAG)     // bit shift   0
#define PKA_OPCODE_RESULT_POS                     CC_REG_BIT_SHIFT(OPCODE, REG_R)   // bit shift   6
#define PKA_OPCODE_OPERAND_2_POS                  CC_REG_BIT_SHIFT(OPCODE, REG_B)   // bit shift  12
#define PKA_OPCODE_OPERAND_1_POS                  CC_REG_BIT_SHIFT(OPCODE, REG_A)   // bit shift  18
#define PKA_OPCODE_LEN_POS                        CC_REG_BIT_SHIFT(OPCODE, LEN)     // bit shift  24
#define PKA_OPCODE_OPERATION_ID_POS               CC_REG_BIT_SHIFT(OPCODE, OPCODE)      // bit shift  27
#define PKA_OPCODE_R_DISCARD_POS                  (PKA_OPCODE_RESULT_POS + PKA_OPCODE_RES_OPERAND_MSBIT_OFFSET)
#define PKA_OPCODE_OPERAND_2_IMMED_POS            (PKA_OPCODE_OPERAND_2_POS + PKA_OPCODE_RES_OPERAND_MSBIT_OFFSET)
#define PKA_OPCODE_OPERAND_1_IMMED_POS            (PKA_OPCODE_OPERAND_1_POS + PKA_OPCODE_RES_OPERAND_MSBIT_OFFSET)

/* PKA STATUS register fields positions (low bit position) */
#define PKA_STATUS_ALU_OUT_ZERO_POS               CC_REG_BIT_SHIFT(PKA_STATUS, ALU_OUT_ZERO)  // bit shift  12
#define PKA_STATUS_DIV_BY_ZERO_POS                CC_REG_BIT_SHIFT(PKA_STATUS, DIV_BY_ZERO)   // bit shift  14
#define PKA_STATUS_ALU_CARRY_POS                  CC_REG_BIT_SHIFT(PKA_STATUS, ALU_CARRY)     // bit shift  9


/* PKA N_NP_T0_T1 register fields positions (low bit position) */
#define PKA_N_NP_T0_T1_REG_N_POS                  CC_REG_BIT_SHIFT(N_NP_T0_T1_ADDR, N_VIRTUAL_ADDR)    // bit shift  0
#define PKA_N_NP_T0_T1_REG_NP_POS                 CC_REG_BIT_SHIFT(N_NP_T0_T1_ADDR, NP_VIRTUAL_ADDR)   // bit shift  5
#define PKA_N_NP_T0_T1_REG_T0_POS                 CC_REG_BIT_SHIFT(N_NP_T0_T1_ADDR, T0_VIRTUAL_ADDR)   // bit shift  10
#define PKA_N_NP_T0_T1_REG_T1_POS                 CC_REG_BIT_SHIFT(N_NP_T0_T1_ADDR, T1_VIRTUAL_ADDR)   // bit shift  15

/* PKA N_NP_T0_T1 register default (reset) value: N=0, NP=1, T0=30, T1=31 */
#define PKA_REG_N                                           0
#define PKA_REG_NP                                          1
#define PKA_REG_T0                                          30
#define PKA_REG_T1                                          31
#define PKA_N_NP_T0_T1_REG_DEFAULT_VAL                  (PKA_REG_N  << PKA_N_NP_T0_T1_REG_N_POS  | \
                                                         PKA_REG_NP << PKA_N_NP_T0_T1_REG_NP_POS | \
                                                         PKA_REG_T0 << PKA_N_NP_T0_T1_REG_T0_POS | \
                                                         PKA_REG_T1 << PKA_N_NP_T0_T1_REG_T1_POS)

/* PKA control values  */
#define PKA_PIPE_READY                                   1
#define PKA_OP_DONE                                      1


/* PKA HW defined OPCODE values, according to HW documentation  */
#define PKA_OPCODE_ID_ADD           0x4U    // @0x4 - Add,Inc
#define PKA_OPCODE_ID_SUB           0x5U    // @0x5 - Sub,Dec,Neg
#define PKA_OPCODE_ID_MODADD        0x6U    // @0x6 - ModAdd,ModInc
#define PKA_OPCODE_ID_MODSUB        0x7U    // @0x7 - ModSub,ModDec,ModNeg
#define PKA_OPCODE_ID_AND           0x8U    // @0x8 - AND,TST0,CLR0
#define PKA_OPCODE_ID_OR            0x9U    // @0x9 - OR,COPY,SET0
#define PKA_OPCODE_ID_XOR           0xAU    // @0xa - XOR,FLIP0,INVERT,COMPARE
#define PKA_OPCODE_ID_SHR0          0xCU    // @0xc - SHR0
#define PKA_OPCODE_ID_SHR1          0xDU    // @0xd - SHR1
#define PKA_OPCODE_ID_SHL0          0xEU    // @0xe - SHL0
#define PKA_OPCODE_ID_SHL1          0xFU    // @0xf - SHL1
#define PKA_OPCODE_ID_MULLOW        0x10U   // @0x10 - MulLow
#define PKA_OPCODE_ID_MODMUL        0x11U   // @0x11 - ModMul
#define PKA_OPCODE_ID_MODMULN       0x12U   // @0x12 - ModMulN
#define PKA_OPCODE_ID_MODEXP        0x13U   // @0x13 - ModExp
#define PKA_OPCODE_ID_DIVISION      0x14U   // @0x14 - Division
#define PKA_OPCODE_ID_MODINV        0x15U   // @0x15 - ModInv
//#define PKA_OPCODE_ID_DIV         0x16U   // @0x16 - RL the code not used
#define PKA_OPCODE_ID_MULHIGH           0x17U   // 0x17 - MULHigh
#define PKA_OPCODE_ID_MODMLAC       0x18U   // 0x18 - ModMulAcc
#define PKA_OPCODE_ID_MODMLACNR         0x19U   // 0x19 - ModMulAccNR
#define PKA_OPCODE_ID_SEPINT        0x1AU   // 0x1A - sepInt
#define PKA_OPCODE_ID_REDUCTION         0x1BU   // 0x1B - Reduction
#define PKA_OPCODE_ID_TERMINATE     0x0U    // @0x00 - Terminate

#define   PKA_MAX_OPCODE    0x1BU


/*************************************************************/
/* Macros for waiting PKA machine ready states               */
/*************************************************************/


/* defining a macro for waiting to the PKA_PIPE_READY */
#define PKA_WAIT_ON_PKA_PIPE_READY() \
do { \
   volatile uint32_t output_reg_val; \
   do \
   { \
      output_reg_val = CC_HAL_READ_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_PIPE_RDY) ); \
   }while( (output_reg_val & 0x01) != PKA_PIPE_READY ); \
}while(0)

/* defining a macro for waiting to the PKA_OP_DONE */
#define PKA_WAIT_ON_PKA_DONE() \
do { \
   volatile uint32_t output_reg_val; \
   do \
   { \
      output_reg_val = CC_HAL_READ_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_DONE) ); \
   }while( (output_reg_val & 0x01) != PKA_OP_DONE ); \
}while(0)


/**************************************************************/
/*  Macros for controlling PKA machine and changing           */
/*  PKA sizes table and mapping table settings.               */
/**************************************************************/

/*  Set_N_NP_T0_T1_REG:  Sets addresses of registers N,NP,T0,T1 into special register */
#define   PKA_SET_N_NP_T0_T1_REG(N, NP, T0, T1) \
        PKA_WAIT_ON_PKA_DONE();  /*!! Needed to wait PKA not to disturb in the it's work !!!*/\
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, N_NP_T0_T1_ADDR), \
       (uint32_t)( (N) << PKA_N_NP_T0_T1_REG_N_POS   | \
                    (NP) << PKA_N_NP_T0_T1_REG_NP_POS  | \
                    (T0) << PKA_N_NP_T0_T1_REG_T0_POS  | \
                    (T1) << PKA_N_NP_T0_T1_REG_T1_POS  ) )

/*  Set default N_NP_T0_T1_REG:  Sets default values of registers addresses
    N=0, NP=1, T0=30, T1=31 into special register N_NP_T0_T1_REG */
#define   PKA_DEFAULT_N_NP_T0_T1_REG()  \
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, N_NP_T0_T1_ADDR), \
                              PKA_N_NP_T0_T1_REG_DEFAULT_VAL)

/* Returns the ALU Zero-bit from PKA_STATUS register */
#define PKA_GET_STATUS_ALU_OUT_ZERO(status)  { \
    PKA_WAIT_ON_PKA_DONE(); \
    status = CC_HAL_READ_REGISTER(CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> PKA_STATUS_ALU_OUT_ZERO_POS ) & 1UL; \
}

/*Returns the DividerIsZero-bit from PKA_STATUS register */
#define PKA_GET_STATUS_DIV_BY_ZERO( status)  { \
    PKA_WAIT_ON_PKA_DONE(); \
    status = CC_HAL_READ_REGISTER( CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> PKA_STATUS_DIV_BY_ZERO_POS ) & 1; \
}

/* Returns the ALU Carry-bit from PKA_STATUS register */
#define PKA_GET_STATUS_CARRY(status)  { \
    PKA_WAIT_ON_PKA_DONE(); \
    status = CC_HAL_READ_REGISTER(CC_REG_OFFSET (CRY_KERNEL,PKA_STATUS)); \
    status = ((status) >> PKA_STATUS_ALU_CARRY_POS ) & 1; \
}


/******************************************************************/
/* Macros for setting and reading sizes from PKA regsSizesTable   */
/******************************************************************/

/*  Sets the size into regsSizesTable entry */
#define PKA_SET_REG_SIZE(SizeBits, EntryNum) \
    PKA_WAIT_ON_PKA_DONE(); /*!! Needed to wait PKA not to disturb in the it's work !!!*/\
    CC_HAL_WRITE_REGISTER((CC_REG_OFFSET(CRY_KERNEL ,PKA_L0) + 4*(EntryNum)), (SizeBits) )

/*  Gets the size from regsSizesTable entry */
#define PKA_GET_REG_SIZE(SizeBits, EntryNum) \
    PKA_WAIT_ON_PKA_DONE(); /*!! Needed to wait PKA to get right results !!!*/\
    SizeBits = CC_HAL_READ_REGISTER(CC_REG_OFFSET(CRY_KERNEL ,PKA_L0) + 4*(EntryNum))

/******************************************************************/
/* Macros for setting and reading addresses of PKA data registers */
/******************************************************************/

/*The following macros are used for setting and reading the data registers addresses in mapping table.*/

/*  Sets the physical address PhysAddr of register VirtReg in mapping table */
#define PKA_SET_REG_ADDRESS(VirtReg, PhysAddr) \
    PKA_WAIT_ON_PKA_DONE(); /*!! Needed to wait PKA to complete before changing mapping !!!*/\
    CC_HAL_WRITE_REGISTER((CC_REG_OFFSET(CRY_KERNEL, MEMORY_MAP0) + 4*(VirtReg)), (PhysAddr))

/*  Returns the physical address of register VirtReg from mapping table  */
#define PKA_GET_REG_ADDRESS(VirtReg, PhysAddr)\
    PKA_WAIT_ON_PKA_DONE(); /*!! Needed to wait PKA to complete before changing mapping !!!*/\
    (PhysAddr) = CC_HAL_READ_REGISTER(CC_REG_OFFSET(CRY_KERNEL, MEMORY_MAP0) + 4*(VirtReg))


/******************************************************************/
/*          Macros for setting Full PKI opcode                    */
/******************************************************************/
#define PKA_SET_FULL_OPCODE( Opcode,LenID,IsAImmed,OpA,IsBImmed,OpB,ResDiscard,Res,Tag ) \
    ( ((Opcode))      << PKA_OPCODE_OPERATION_ID_POS     | \
      ((LenID))       << PKA_OPCODE_LEN_POS              | \
      ((IsAImmed))    << PKA_OPCODE_OPERAND_1_IMMED_POS  | \
      ((OpA))         << PKA_OPCODE_OPERAND_1_POS        | \
      ((IsBImmed))    << PKA_OPCODE_OPERAND_2_IMMED_POS  | \
      ((OpB))         << PKA_OPCODE_OPERAND_2_POS        | \
      ((ResDiscard))  << PKA_OPCODE_R_DISCARD_POS        | \
      ((Res))         << PKA_OPCODE_RESULT_POS           | \
      ((Tag))         << PKA_OPCODE_TAG_POS  )


/******************************************************************/
/*          Macros for reading and loading PKA memory data        */
/******************************************************************/
/* macros to lock/unlock the Mutex on each SRAM read/write    *
*  used only when symmetric driver uses the one or more of the following regs:
*  PKA_SRAM_ADDR, PKA_SRAM_RDATA, PKA_SRAM_WDATA, SRAM_DATA_READY     */
#ifdef CC_IOT
    #define PKA_MUTEX_LOCK
    #define PKA_MUTEX_UNLOCK
#else
    extern CC_PalMutex CCSymCryptoMutex;

    #define PKA_MUTEX_LOCK  \
        if(CC_PalMutexLock(&CCSymCryptoMutex, CC_INFINITE) != CC_SUCCESS) { \
           CC_PalAbort("Fail to acquire mutex\n");}

    #define PKA_MUTEX_UNLOCK \
        if(CC_PalMutexUnlock(&CCSymCryptoMutex) != CC_SUCCESS) { \
           CC_PalAbort("Fail to release mutex\n");}
#endif

/* ******************************************************************* */
/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO PKA SRAM DATA */
/* ******************************************************************* */
/* defining a macro to clear the not-completed part of PKA big word*
*  ii value was set in macro calling  PKA_SRAM_WRITE_CLR()         */
#define PKA_SRAM_WRITE_CLR(SizeWords) \
   for(; ii < (((SizeWords+(PKA_WORD_SIZE_IN_32BIT_WORDS-1))/PKA_WORD_SIZE_IN_32BIT_WORDS)*PKA_WORD_SIZE_IN_32BIT_WORDS) ; ii++) { \
       CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), 0); \
       WAIT_SRAM_DATA_READY; \
   }


/* defining a macro to wait on SRAM ready;
       currently waiting is not needed   */
// #define DO_WAIT_SRAM_DATA_READY
// #define PKA_DELEY 0
#ifndef DO_WAIT_SRAM_DATA_READY
    #define WAIT_SRAM_DATA_READY
#elif defined PKA_DELEY
    #define WAIT_SRAM_DATA_READY  { \
        volatile uint32_t ii; \
        for(ii=0; ii<PKA_DELEY; ) ii++; \
    }
#else
    #define WAIT_SRAM_DATA_READY { \
        volatile uint32_t dummy; \
        do { \
            dummy = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, SRAM_DATA_READY)); \
           }while(!(dummy & 0x1));\
    }
#endif

/* macro to load block to SRAM memory */
#define PKA_HW_LOAD_BLOCK_TO_PKA_MEM(Addr, ptr, SizeWords) \
do { \
   uint32_t ii; \
   PKA_MUTEX_LOCK; \
   PKA_WAIT_ON_PKA_DONE();\
   CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_SRAM_ADDR), (Addr)); \
   for(ii = 0; ii < (SizeWords); ii++) \
   { \
       CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), ((uint32_t*)(ptr))[ii]); \
       WAIT_SRAM_DATA_READY; \
   } \
   PKA_SRAM_WRITE_CLR(SizeWords) \
   PKA_MUTEX_UNLOCK; \
}while(0)

/* macro to load a value to SRAM with 32-bit access */
#define PKA_HW_LOAD_VALUE_TO_PKA_MEM(Addr, Val) \
do { \
   PKA_MUTEX_LOCK; \
   PKA_WAIT_ON_PKA_DONE();\
   CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_ADDR), (Addr)); \
   CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), (Val)); \
   WAIT_SRAM_DATA_READY; \
   PKA_MUTEX_UNLOCK; \
}while(0)


/* macro to clear PKA memory: Addr must be alighned to PKA_WORD_SIZE */
#define PKA_HW_CLEAR_PKA_MEM(Addr, SizeWords) \
do { \
   uint32_t ii; \
   PKA_MUTEX_LOCK; \
   PKA_WAIT_ON_PKA_DONE();\
   CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_ADDR), (Addr)); \
   for( ii = 0; ii < (uint32_t)(SizeWords); ii++ ) \
   { \
      CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_WDATA), 0x0UL ); \
      WAIT_SRAM_DATA_READY; \
   }\
   PKA_SRAM_WRITE_CLR((SizeWords)); \
   PKA_MUTEX_UNLOCK; \
}while(0)

/* macro to read a value from the PKA data memory */
#define PKA_HW_READ_VALUE_FROM_PKA_MEM(Addr, Val) \
do { \
   volatile uint32_t dummy; \
   PKA_MUTEX_LOCK; \
   PKA_WAIT_ON_PKA_DONE();\
   CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_RADDR), (Addr)); \
   WAIT_SRAM_DATA_READY; \
   dummy = CC_HAL_READ_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_RDATA)); \
   (Val) = dummy; \
   WAIT_SRAM_DATA_READY; \
   PKA_MUTEX_UNLOCK; \
}while(0)

/* macro to read a block from the PKA data memory */
#define PKA_HW_READ_BLOCK_FROM_PKA_MEM(Addr, ptr, SizeWords) \
do { \
   uint32_t ii; \
   volatile uint32_t dummy; \
   PKA_MUTEX_LOCK; \
   PKA_WAIT_ON_PKA_DONE();\
   CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_RADDR), (Addr)); \
   for(ii = 0; ii < (SizeWords); ii++) \
   { \
      WAIT_SRAM_DATA_READY; \
      dummy = CC_HAL_READ_REGISTER(CC_REG_OFFSET(CRY_KERNEL, PKA_SRAM_RDATA));\
      ((uint32_t*)(ptr))[ii] = (dummy); \
   } \
   WAIT_SRAM_DATA_READY; \
   PKA_MUTEX_UNLOCK; \
}while(0)

/************************ Enums ********************************/

#ifdef __cplusplus
}
#endif

#endif // PKA_HW_DEFS_H


