/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef RSA_HWDEFS_H
#define RSA_HWDEFS_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "secureboot_general_hwdefs.h"
#include "cc_pka_hw_plat_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* PKA big word size in bits */
#define RSA_PKA_BIG_WORD_SIZE_IN_BITS  CC_PKA_WORD_SIZE_IN_BITS
#define RSA_PKA_BIG_WORD_SIZE_IN_BYTES  (RSA_PKA_BIG_WORD_SIZE_IN_BITS/8)
#define RSA_PKA_BIG_WORD_SIZE_IN_32_BIT_WORDS  (RSA_PKA_BIG_WORD_SIZE_IN_BITS/32)

#define RSA_PKA_EXTRA_BITS  PKA_EXTRA_BITS

/* PKA max count of SRAM registers: */
#define RSA_HW_PKI_PKA_MAX_COUNT_OF_PHYS_MEM_REGS  PKA_MAX_COUNT_OF_PHYS_MEM_REGS

/* The maximal size of allowed PKA physical registers memory including tables */
#define RSA_PKA_MAX_REGS_MEM_SIZE_BYTES   (6 * 1024)

/* SRAM definitions */
#define RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR           g_SramPkaAddr

/* The maximal count of allowed sizes of PKA operands or register-variables */
#define RSA_PKA_MAX_COUNT_OF_REGS_SIZES                   8 /*PKA_MAX_COUNT_OF_REGS_SIZES */
#define RSA_PKA_MAX_COUNT_OF_ADDITIONAL_REGS          10
#define RSA_PKA_SRAM_REGS_MEM_OFFSET_WORDS                RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR


/* PKA OPCODE register fields positions (low bit position) */
#define RSA_HW_PKI_PKA_OPCODE_TAG_POS                               0
#define RSA_HW_PKI_PKA_OPCODE_RESULT_POS                            6
#define RSA_HW_PKI_PKA_OPCODE_R_DISCARD_POS                        11
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_2_POS                        12
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_2_IMMED_POS                  17
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_1_POS                        18
#define RSA_HW_PKI_PKA_OPCODE_OPERAND_1_IMMED_POS                  23
#define RSA_HW_PKI_PKA_OPCODE_LEN_POS                              24
#define RSA_HW_PKI_PKA_OPCODE_OPERATION_ID_POS                     27


/* PKA N_NP_T0_T1 register fields positions (low bit position) */
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_N_POS                         DX_N_NP_T0_T1_ADDR_N_VIRTUAL_ADDR_BIT_SHIFT
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_NP_POS                        DX_N_NP_T0_T1_ADDR_NP_VIRTUAL_ADDR_BIT_SHIFT
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T0_POS                        DX_N_NP_T0_T1_ADDR_T0_VIRTUAL_ADDR_BIT_SHIFT
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T1_POS                        DX_N_NP_T0_T1_ADDR_T1_VIRTUAL_ADDR_BIT_SHIFT

/* PKA N_NP_T0_T1 register default (reset) value: N=0, NP=1, T0=30, T1=31 */
#define PKA_N                                                    0UL
#define PKA_NP                                                   1UL
#define PKA_T0                                                  30UL
#define PKA_T1                                                  31UL
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL                  ( PKA_N  << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_N_POS  | \
                                                                  PKA_NP << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_NP_POS | \
                                                                  PKA_T0 << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T0_POS | \
                                                                  PKA_T1 << RSA_HW_PKI_PKA_N_NP_T0_T1_REG_T1_POS )

/* PKA control values  */
#define RSA_PKA_PIPE_READY                                   1
#define RSA_PKA_OP_DONE                                      1
#define RSA_PKA_SW_REST                                      1

/* Machine Opcodes definitions (according to HW CRS ) */
#define   PkaSepDone        0x00
#define   PkaAdd            0x04
#define   PkaAddIm          0x04
#define   PkaSub            0x05
#define   PkaSubIm          0x05
#define   PkaNeg            0x05
#define   PkaModAdd         0x06
#define   PkaModAddIm       0x06
#define   PkaModSub         0x07
#define   PkaModSubIm       0x07
#define   PkaModNeg         0x07
#define   PkaAND            0x08
#define   PkaTest0          0x08
#define   PkaClr0           0x08
#define   PkaClr            0x08
#define   PkaOR             0x09
#define   PkaCopy       0x09
#define   PkaSetBit0        0x09
#define   PkaXOR        0x0A
#define   PkaFlip0      0x0A
#define   PkaInvertBits     0x0A
#define   PkaCompare        0x0A
#define   PkaSHR0       0x0C
#define   PkaSHR1           0x0D
#define   PkaSHL0       0x0E
#define   PkaSHL1       0x0F
#define   PkaLMul       0x10
#define   PkaModMul     0x11
#define   PkaModMulNR       0x12
#define   PkaModExp     0x13
#define   PkaDiv        0x14
#define   PkaModInv         0x15
#define   PkaHMul       0x17
#define   PkaModMulAcc      0x18
#define   PkaModMulAccN     0x19
#define   PkaSepInt         0x1A
#define   PkaReduce         0x1B

#define   PKA_MAX_OPCODE    0x1B


/* **********************           MACROS          ******************************* */


/* Full register (operation) size including extra bits + one PKA-word (64 or  *
*  128 bits), rounded up to PKA words.                                        */
#define GET_FULL_OP_SIZE_PKA_WORDS(opSizeInBits)     (((opSizeInBits)/RSA_PKA_BIG_WORD_SIZE_IN_BITS + (((opSizeInBits) & (RSA_PKA_BIG_WORD_SIZE_IN_BITS-1)) > 0)) + 1)

/*************************************************************/
/* Macros for waiting PKA machine ready states               */
/*************************************************************/

/* defining a macro for waiting to the PKA_PIPE_READY */
#define RSA_PKA_WAIT_ON_PKA_PIPE_READY(VirtualHwBaseAddr) \
do \
{ \
   volatile uint32_t output_reg_val; \
   do \
   { \
    SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_PIPE_RDY), output_reg_val ); \
   }while( (output_reg_val & 0x01) != RSA_PKA_PIPE_READY ); \
}while(0)

/* defining a macro for waiting to the PKA_OP_DONE */
#define RSA_PKA_WAIT_ON_PKA_DONE(VirtualHwBaseAddr) \
do \
{ \
   volatile uint32_t output_reg_val; \
   do \
   { \
    SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_DONE), output_reg_val); \
   }while( (output_reg_val & 0x01) != RSA_PKA_OP_DONE ); \
}while(0)

/******************************************************************/
/* Macros for setting and reading sizes from PKA regsSizesTable   */
/******************************************************************/


/*  ReadRegSizeInTable:  Readss the size from regsSizesTable entry */
#define RSA_PKA_ReadRegSize(SizeBits, EntryNum, VirtualHwBaseAddr) \
    RSA_PKA_WAIT_ON_PKA_DONE( VirtualHwBaseAddr ); \
    SB_HAL_READ_REGISTER( (SB_REG_ADDR(VirtualHwBaseAddr,PKA_L0) + 4*(EntryNum)), \
                                (SizeBits) )

/******************************************************************/
/* Macros for setting and reading addresses of PKA data registers */
/******************************************************************/

/*The following macros are used for setting and reading the data registers addresses in mapping table.*/

/*  GetRegAddress:  Returns the physical address of register VirtReg from mapping table  */
#define RSA_PKA_GetRegAddress(VirtReg, VirtualHwBaseAddr) \
 ( *((volatile uint32_t*)(SB_REG_ADDR(VirtualHwBaseAddr,MEMORY_MAP0) + 4*(VirtReg))) )

/******************************************************************/
/*          Macros for setting Full PKI opcode                    */
/******************************************************************/

#define RSA_PKA_FullOpCode( Opcode,LenID,IsAImmed,OpA,IsBImmed,OpB,ResDiscard,Res,Tag ) \
   ( ((Opcode))      << RSA_HW_PKI_PKA_OPCODE_OPERATION_ID_POS     | \
     ((LenID))       << RSA_HW_PKI_PKA_OPCODE_LEN_POS              | \
     ((IsAImmed))    << RSA_HW_PKI_PKA_OPCODE_OPERAND_1_IMMED_POS  | \
     ((OpA))         << RSA_HW_PKI_PKA_OPCODE_OPERAND_1_POS        | \
     ((IsBImmed))    << RSA_HW_PKI_PKA_OPCODE_OPERAND_2_IMMED_POS  | \
     ((OpB))         << RSA_HW_PKI_PKA_OPCODE_OPERAND_2_POS        | \
     ((ResDiscard))  << RSA_HW_PKI_PKA_OPCODE_R_DISCARD_POS        | \
     ((Res))         << RSA_HW_PKI_PKA_OPCODE_RESULT_POS           | \
     ((Tag))         << RSA_HW_PKI_PKA_OPCODE_TAG_POS  )

/******************************************************************/
/*          Macros for reading and loading PKA memory data        */
/******************************************************************/

/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

    /*These registers  not included in the HW_CC definitions because they are HOST registers */
#ifdef CC_SB_INDIRECT_SRAM_ACCESS
/* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

    #define RSA_PKA_SRAM_WRITE_CLR(VirtualHwBaseAddr, SizeWords) \
    for(; ii < 4*(((SizeWords)+3)/4) ; ii++) { \
        SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), 0); \
    }

    #ifndef BIG__ENDIAN
    /* macro to load a value to SRAM with 32-bit access */
    #define RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM(VirtualHwBaseAddr, Addr, Val) \
    do \
    { \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), ((Addr)+ RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), (Val)); \
    }while(0)
    #else
    #define RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM(VirtualHwBaseAddr, Addr, Val) \
    do \
    { \
       uint32_t tempVal; \
       UTIL_ReverseMemCopy((uint8_t*)&tempVal , (uint8_t*)&Val , sizeof(uint32_t) ); \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), ((Addr)+ RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), (tempVal)); \
    }while(0)
    #endif

    #ifndef BIG__ENDIAN
    /* macro to load block to SRAM memory */
    #define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM(VirtualHwBaseAddr, Addr, ptr, SizeWords) \
    do \
    { \
       uint32_t ii; \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), ((Addr)+RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
       for(ii = 0; ii < (SizeWords); ii++) \
       { \
           SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), ((uint32_t*)(ptr))[ii]); \
       } \
       RSA_PKA_SRAM_WRITE_CLR(VirtualHwBaseAddr, SizeWords) \
    }while(0)
    /* defining a macro to load a block to the PKA data memory */

    #else
    #define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM(VirtualHwBaseAddr, Addr, ptr, SizeWords) \
    do \
    { \
       uint32_t ii; \
       uint32_t tempWord; \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), ((Addr)+RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
       for(ii = 0; ii < (SizeWords); ii++) \
       { \
           UTIL_ReverseMemCopy((uint8_t*)&tempWord , (uint8_t*)&(ptr)[ii] , sizeof(uint32_t) ); \
           SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), tempWord); \
       } \
       RSA_PKA_SRAM_WRITE_CLR(VirtualHwBaseAddr, SizeWords) \
    }while(0)
    #endif

        /* macro to clear PKA memory: Addr must be alighned to PKA_WORD size */
    #define RSA_HW_PKI_HW_CLEAR_PKA_MEM(VirtualHwBaseAddr, Addr, SizeWords) \
    do \
    { \
       uint32_t ii; \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_ADDR), (Addr)); \
       for( ii = 0; ii < (uint32_t)(SizeWords); ii++ ) \
       { \
        SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_WDATA), 0x0UL ); \
       }\
       RSA_PKA_SRAM_WRITE_CLR(VirtualHwBaseAddr, (SizeWords)); \
    }while(0)

    #ifndef BIG__ENDIAN
    /* macro to read a value from the PKA data memory */
    #define RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM(VirtualHwBaseAddr, Addr, Val) \
    do \
    { \
       volatile uint32_t dummy; \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RADDR), ((Addr)+ RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
       SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RDATA), (dummy)); \
       (Val) = dummy; \
    }while(0)
    #else
    #define RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM(VirtualHwBaseAddr, Addr, Val) \
    do \
    { \
       volatile uint32_t dummy; \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RADDR), ((Addr)+ RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)); \
       SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RDATA), (dummy)); \
       (Val) = dummy; \
       UTIL_ReverseBuff((uint8_t*)&Val , sizeof(uint32_t) ); \
    }while(0)
    #endif

    #ifndef BIG__ENDIAN
    /* defining a macro to read a block from the PKA data memory */
    #define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM(VirtualHwBaseAddr, Addr, ptr, SizeWords) \
    do \
    { \
       uint32_t ii; \
       volatile uint32_t dummy; \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RADDR), (Addr)); \
       for(ii = 0; ii < (SizeWords); ii++) \
       { \
          SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RDATA), (dummy));\
          ((uint32_t*)(ptr))[ii] = (dummy); \
       } \
    }while(0)
    #else
    #define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM(VirtualHwBaseAddr, Addr, ptr, SizeWords) \
    do \
    { \
       uint32_t ii; \
       volatile uint32_t dummy; \
       SB_HAL_WRITE_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RADDR), (Addr)); \
       for(ii = 0; ii < (SizeWords); ii++) \
       { \
          SB_HAL_READ_REGISTER( SB_REG_ADDR(VirtualHwBaseAddr, PKA_SRAM_RDATA), (dummy));\
          ((uint32_t*)(ptr))[ii] = (dummy); \
          UTIL_ReverseBuff((uint8_t*)&(ptr)[ii] , sizeof(uint32_t) ); \
       } \
    }while(0)
    #endif

#else //CC_SB_INDIRECT_SRAM_ACCESS=0 => direct access
        /* MACRO DEFINITIONS FOR WORKING WITH INDIRECT ACCESS TO SRAM PKA DATA REGISTERS */

    /*These registers  not included in the HW_CC definitions because they are HOST registers */

    /* defining a macro to load a value to the PKA data memory */
    #define RSA_HW_PKI_HW_LOAD_VALUE_TO_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
    do \
    { \
       *(uint32_t*)(((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR)) = (uint32_t)Val; \
    }while(0)


    /* defining a macro to load a block to the PKA data memory */
    #define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
    do \
    { \
        UTIL_MemCopy((uint8_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(uint8_t*)(ptr),(SizeWords)*sizeof(uint32_t)); \
    }while(0)


    /* defining a macro to clear PKA data memory */
    #define RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , Addr , SizeWords ) \
    do \
    { \
        UTIL_MemSet((uint8_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),0x0,(SizeWords)*sizeof(uint32_t)); \
    }while(0)


    /* defining a macro to read a value from the PKA data memory */
    #define RSA_HW_PKI_HW_READ_VALUE_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , Val ) \
    do \
    { \
        Val = *(uint32_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR); \
    }while(0)


    /* defining a macro to read a block from the PKA data memory */
    #define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
    do \
    { \
        UTIL_MemCopy((uint8_t*)(ptr),(uint8_t*)((Addr) + RSA_HW_PKI_PKA_DATA_REGS_MEMORY_OFFSET_ADDR),(SizeWords)*sizeof(uint32_t)); \
    }while(0)

#endif //CC_SB_INDIRECT_SRAM_ACCESS


/************************ Defines ******************************/

/* ********************** Macros ******************************* */

/************************ Enums ********************************/

#ifdef __cplusplus
}
#endif

#endif


