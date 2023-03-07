/***************************************************************************//**
 * @file
 * @brief CPU Port File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @note     (1) This driver targets the following:
 *               Core      : ARMv6M Cortex-M
 *               Toolchain : GNU C Compiler
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               MODULE
 *
 *******************************************************************************************************/

#ifndef  _ARMV6M_CPU_PORT_H_
#define  _ARMV6M_CPU_PORT_H_

/********************************************************************************************************
 *                                          CPU INCLUDE FILES
 *
 * @note     (1) The following CPU files are located in the following directories :
 *
 *               (a) \<Your Product Application>\cpu_cfg.h
 *
 *               (b) (1) \<CPU-Compiler Directory>\cpu_def.h
 *                   (2) \<CPU-Compiler Directory>\<cpu>\<compiler>\cpu*.*
 *
 *                       where
 *                               <Your Product Application>      directory path for Your Product's Application
 *                               <CPU-Compiler Directory>        directory path for common   CPU-compiler software
 *                               <cpu>                           directory name for specific CPU
 *                               <compiler>                      directory name for specific compiler
 *
 * @note     (2) Compiler MUST be configured to include as additional include path directories :
 *
 *               (a) '\<Your Product Application>\' directory                            See Note #1a
 *
 *               (b) (1) '\<CPU-Compiler Directory>\'                  directory         See Note #1b1
 *                   (2) '\<CPU-Compiler Directory>\<cpu>\<compiler>\' directory         See Note #1b2
 *
 * @note     (3) Since NO custom library modules are included, 'cpu.h' may ONLY use configurations from
 *               CPU configuration file 'cpu_cfg.h' that do NOT reference any custom library definitions.
 *
 *               In other words, 'cpu.h' may use 'cpu_cfg.h' configurations that are #define'd to numeric
 *               constants or to NULL (i.e. NULL-valued #define's); but may NOT use configurations to
 *               custom library #define's (e.g. DEF_DISABLED or DEF_ENABLED).
 *******************************************************************************************************/

#include <cpu/include/cpu_def.h>
#include <cpu_cfg.h>                                           // See Note #3.
#include "em_core.h"

/********************************************************************************************************
 *                                    CONFIGURE STANDARD DATA TYPES
 *
 * @note     (1) Configure standard data types according to CPU-/compiler-specifications.
 *
 * @note     (2) (a) (1) 'CPU_FNCT_VOID' data type defined to replace the commonly-used function pointer
 *                       data type of a pointer to a function which returns void & has no arguments.
 *
 *                   (2) Example function pointer usage :
 *
 *                           CPU_FNCT_VOID  FnctName;
 *
 *                           FnctName();
 *
 *               (b) (1) 'CPU_FNCT_PTR'  data type defined to replace the commonly-used function pointer
 *                       data type of a pointer to a function which returns void & has a single void
 *                       pointer argument.
 *
 *                   (2) Example function pointer usage :
 *
 *                           CPU_FNCT_PTR   FnctName;
 *                           void          *p_obj
 *
 *                           FnctName(p_obj);
 *******************************************************************************************************/

typedef void CPU_VOID;
typedef char CPU_CHAR;                                          // 8-bit character
typedef unsigned char CPU_BOOLEAN;                              // 8-bit boolean or logical
typedef unsigned char CPU_INT08U;                               // 8-bit unsigned integer
typedef signed char CPU_INT08S;                                 // 8-bit   signed integer
typedef unsigned short CPU_INT16U;                              // 16-bit unsigned integer
typedef signed short CPU_INT16S;                                // 16-bit   signed integer
typedef unsigned int CPU_INT32U;                                // 32-bit unsigned integer
typedef signed int CPU_INT32S;                                  // 32-bit   signed integer
typedef unsigned long long CPU_INT64U;                          // 64-bit unsigned integer
typedef signed long long CPU_INT64S;                            // 64-bit   signed integer

typedef float CPU_FP32;                                         // 32-bit floating point
typedef double CPU_FP64;                                        // 64-bit floating point

typedef volatile CPU_INT08U CPU_REG08;                          // 8-bit register
typedef volatile CPU_INT16U CPU_REG16;                          // 16-bit register
typedef volatile CPU_INT32U CPU_REG32;                          // 32-bit register
typedef volatile CPU_INT64U CPU_REG64;                          // 64-bit register

typedef void (*CPU_FNCT_VOID)(void);                            // See Note #2a.
typedef void (*CPU_FNCT_PTR )(void *p_obj);                     // See Note #2b.

/********************************************************************************************************
 *                                       CPU WORD CONFIGURATION
 *
 * @note     (1) Configure CPU_CFG_ADDR_SIZE, CPU_CFG_DATA_SIZE, & CPU_CFG_DATA_SIZE_MAX with CPU's &/or
 *               compiler's word sizes :
 *
 *                   CPU_WORD_SIZE_08             8-bit word size
 *                   CPU_WORD_SIZE_16            16-bit word size
 *                   CPU_WORD_SIZE_32            32-bit word size
 *                   CPU_WORD_SIZE_64            64-bit word size
 *
 * @note     (2) Configure CPU_CFG_ENDIAN_TYPE with CPU's data-word-memory order :
 *
 *               (a) CPU_ENDIAN_TYPE_BIG         Big-   endian word order (CPU words' most  significant
 *                                                                         octet @ lowest memory address)
 *               (b) CPU_ENDIAN_TYPE_LITTLE      Little-endian word order (CPU words' least significant
 *                                                                         octet @ lowest memory address)
 *******************************************************************************************************/

//                                                                 Define  CPU         word sizes (see Note #1) :
#define  CPU_CFG_ADDR_SIZE              CPU_WORD_SIZE_32        // Defines CPU address word size  (in octets).
#define  CPU_CFG_DATA_SIZE              CPU_WORD_SIZE_32        // Defines CPU data    word size  (in octets).
#define  CPU_CFG_DATA_SIZE_MAX          CPU_WORD_SIZE_64        // Defines CPU maximum word size  (in octets).

#define  CPU_CFG_ENDIAN_TYPE            CPU_ENDIAN_TYPE_LITTLE  // Defines CPU data    word-memory order (see Note #2).

/********************************************************************************************************
 *                                 CONFIGURE CPU ADDRESS & DATA TYPES
 *******************************************************************************************************/

//                                                                 CPU address type based on address bus size.
#if     (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_32)
typedef CPU_INT32U CPU_ADDR;
#elif   (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_16)
typedef CPU_INT16U CPU_ADDR;
#else
typedef CPU_INT08U CPU_ADDR;
#endif

//                                                                 CPU data    type based on data    bus size.
#if     (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_32)
typedef CPU_INT32U CPU_DATA;
#elif   (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_16)
typedef CPU_INT16U CPU_DATA;
#else
typedef CPU_INT08U CPU_DATA;
#endif

typedef CPU_DATA CPU_ALIGN;                                     // Defines CPU data-word-alignment size.
typedef CPU_ADDR CPU_SIZE_T;                                    // Defines CPU standard 'size_t'   size.

/********************************************************************************************************
 *                                       CPU STACK CONFIGURATION
 *
 * @note     (1) Configure CPU_CFG_STK_GROWTH in 'cpu.h' with CPU's stack growth order :
 *
 *               (a) CPU_STK_GROWTH_LO_TO_HI     CPU stack pointer increments to the next higher  stack
 *                                                   memory address after data is pushed onto the stack
 *               (b) CPU_STK_GROWTH_HI_TO_LO     CPU stack pointer decrements to the next lower   stack
 *                                                   memory address after data is pushed onto the stack
 *
 * @note     (2) Configure CPU_CFG_STK_ALIGN_BYTES with the highest minimum alignement required for
 *               cpu stacks.
 *
 *               (a) ARM Procedure Calls Standard requires an 8 bytes stack alignment.
 *******************************************************************************************************/

#define  CPU_CFG_STK_GROWTH       CPU_STK_GROWTH_HI_TO_LO       // Defines CPU stack growth order (see Note #1).

#define  CPU_CFG_STK_ALIGN_BYTES  (8u)                          // Defines CPU stack alignment in bytes. (see Note #2).

typedef CPU_INT32U CPU_STK;                                     // Defines CPU stack data type.
typedef CPU_ADDR CPU_STK_SIZE;                                  // Defines CPU stack size data type.

/********************************************************************************************************
 *                                    MEMORY BARRIERS CONFIGURATION
 *
 * @note     (1) (a) Configure memory barriers if required by the architecture.
 *
 *                   CPU_MB      Full memory barrier.
 *                   CPU_RMB     Read (Loads) memory barrier.
 *                   CPU_WMB     Write (Stores) memory barrier.
 *
 *******************************************************************************************************/

#define  CPU_MB()       __asm__ __volatile__ ("dsb" : : : "memory")
#define  CPU_RMB()      __asm__ __volatile__ ("dsb" : : : "memory")
#define  CPU_WMB()      __asm__ __volatile__ ("dsb" : : : "memory")

/********************************************************************************************************
 *                                          INTERRUPT SOURCES
 *******************************************************************************************************/

#define  CPU_INT_STK_PTR                                   0u
#define  CPU_INT_RESET                                     1u
#define  CPU_INT_NMI                                       2u
#define  CPU_INT_HFAULT                                    3u
#define  CPU_INT_RSVD_04                                   4u   // RSVD interrupts may be designated on newer models
#define  CPU_INT_RSVD_05                                   5u
#define  CPU_INT_RSVD_06                                   6u
#define  CPU_INT_RSVD_07                                   7u
#define  CPU_INT_RSVD_08                                   8u
#define  CPU_INT_RSVD_09                                   9u
#define  CPU_INT_RSVD_10                                  10u
#define  CPU_INT_SVCALL                                   11u
#define  CPU_INT_RSVD_12                                  12u
#define  CPU_INT_RSVD_13                                  13u
#define  CPU_INT_PENDSV                                   14u
#define  CPU_INT_SYSTICK                                  15u
#define  CPU_INT_EXT0                                     16u

/********************************************************************************************************
 *                                      EXTERNAL INTERRUPT OFFSET
 *******************************************************************************************************/

#define  CPU_INT_EXT_OFFSET                               CPU_INT_EXT0

/********************************************************************************************************
 *                                            CPU REGISTERS
 *******************************************************************************************************/

#define  CPU_REG_NVIC_NVIC           (*((CPU_REG32 *)(0xE000E004)))            // Int Ctrl'er Type Reg.
#define  CPU_REG_NVIC_ST_CTRL        (*((CPU_REG32 *)(0xE000E010)))            // SysTick Ctrl & Status Reg.
#define  CPU_REG_NVIC_ST_RELOAD      (*((CPU_REG32 *)(0xE000E014)))            // SysTick Reload      Value Reg.
#define  CPU_REG_NVIC_ST_CURRENT     (*((CPU_REG32 *)(0xE000E018)))            // SysTick Current     Value Reg.
#define  CPU_REG_NVIC_ST_CAL         (*((CPU_REG32 *)(0xE000E01C)))            // SysTick Calibration Value Reg.

#define  CPU_REG_NVIC_SETEN(n)       (*((CPU_REG32 *)(0xE000E100 + (n) * 4u))) // IRQ Set En Reg.
#define  CPU_REG_NVIC_CLREN(n)       (*((CPU_REG32 *)(0xE000E180 + (n) * 4u))) // IRQ Clr En Reg.
#define  CPU_REG_NVIC_SETPEND(n)     (*((CPU_REG32 *)(0xE000E200 + (n) * 4u))) // IRQ Set Pending Reg.
#define  CPU_REG_NVIC_CLRPEND(n)     (*((CPU_REG32 *)(0xE000E280 + (n) * 4u))) // IRQ Clr Pending Reg.
#define  CPU_REG_NVIC_PRIO(n)        (*((CPU_REG32 *)(0xE000E400 + (n) * 4u))) // IRQ Prio Reg.

#define  CPU_REG_NVIC_CPUID          (*((CPU_REG32 *)(0xE000ED00)))            // CPUID Base Reg.
#define  CPU_REG_NVIC_ICSR           (*((CPU_REG32 *)(0xE000ED04)))            // Int Ctrl State  Reg.
#define  CPU_REG_NVIC_VTOR           (*((CPU_REG32 *)(0xE000ED08)))            // Vect Tbl Offset Reg.
#define  CPU_REG_NVIC_AIRCR          (*((CPU_REG32 *)(0xE000ED0C)))            // App Int/Reset Ctrl Reg.
#define  CPU_REG_NVIC_SCR            (*((CPU_REG32 *)(0xE000ED10)))            // System Ctrl Reg.
#define  CPU_REG_NVIC_CCR            (*((CPU_REG32 *)(0xE000ED14)))            // Cfg    Ctrl Reg.
#define  CPU_REG_NVIC_SHPRI2         (*((CPU_REG32 *)(0xE000ED1C)))            // System Handlers  8 to 11 Prio.
#define  CPU_REG_NVIC_SHPRI3         (*((CPU_REG32 *)(0xE000ED20)))            // System Handlers 12 to 15 Prio.
#define  CPU_REG_NVIC_SHCSR          (*((CPU_REG32 *)(0xE000ED24)))            // System Handler Ctrl & State Reg.
#define  CPU_REG_NVIC_DFSR           (*((CPU_REG32 *)(0xE000ED30)))            // Debug Fault Status Reg.

#define  CPU_REG_NVIC_MMFR0          (*((CPU_REG32 *)(0xE000ED50)))            // Memory Model Feature Reg 0.
#define  CPU_REG_NVIC_MMFR1          (*((CPU_REG32 *)(0xE000ED54)))            // Memory Model Feature Reg 1.
#define  CPU_REG_NVIC_MMFR2          (*((CPU_REG32 *)(0xE000ED58)))            // Memory Model Feature Reg 2.
#define  CPU_REG_NVIC_MMFR3          (*((CPU_REG32 *)(0xE000ED5C)))            // Memory Model Feature Reg 3.
#define  CPU_REG_NVIC_ISAFR0         (*((CPU_REG32 *)(0xE000ED60)))            // ISA Feature Reg 0.
#define  CPU_REG_NVIC_ISAFR1         (*((CPU_REG32 *)(0xE000ED64)))            // ISA Feature Reg 1.
#define  CPU_REG_NVIC_ISAFR2         (*((CPU_REG32 *)(0xE000ED68)))            // ISA Feature Reg 2.
#define  CPU_REG_NVIC_ISAFR3         (*((CPU_REG32 *)(0xE000ED6C)))            // ISA Feature Reg 3.
#define  CPU_REG_NVIC_ISAFR4         (*((CPU_REG32 *)(0xE000ED70)))            // ISA Feature Reg 4.
#define  CPU_REG_NVIC_SW_TRIG        (*((CPU_REG32 *)(0xE000EF00)))            // Software Trigger Int Reg.

#define  CPU_REG_MPU_TYPE            (*((CPU_REG32 *)(0xE000ED90)))            // MPU Type Reg.
#define  CPU_REG_MPU_CTRL            (*((CPU_REG32 *)(0xE000ED94)))            // MPU Ctrl Reg.
#define  CPU_REG_MPU_REG_NBR         (*((CPU_REG32 *)(0xE000ED98)))            // MPU Region Nbr Reg.
#define  CPU_REG_MPU_REG_BASE        (*((CPU_REG32 *)(0xE000ED9C)))            // MPU Region Base Addr Reg.
#define  CPU_REG_MPU_REG_ATTR        (*((CPU_REG32 *)(0xE000EDA0)))            // MPU Region Attrib & Size Reg.

#define  CPU_REG_DBG_SELECT          (*((CPU_REG32 *)(0xE000EDF4)))            // Debug Core Reg Selector Reg.
#define  CPU_REG_DBG_DATA            (*((CPU_REG32 *)(0xE000EDF8)))            // Debug Core Reg Data     Reg.
#define  CPU_REG_DBG_INT             (*((CPU_REG32 *)(0xE000EDFC)))            // Debug Except & Monitor Ctrl Reg.

/********************************************************************************************************
 *                                          CPU REGISTER BITS
 *******************************************************************************************************/

//                                                                 ---------- SYSTICK CTRL & STATUS REG BITS ----------
#define  CPU_REG_NVIC_ST_CTRL_COUNTFLAG           0x00010000
#define  CPU_REG_NVIC_ST_CTRL_CLKSOURCE           0x00000004
#define  CPU_REG_NVIC_ST_CTRL_TICKINT             0x00000002
#define  CPU_REG_NVIC_ST_CTRL_ENABLE              0x00000001

//                                                                 -------- SYSTICK CALIBRATION VALUE REG BITS --------
#define  CPU_REG_NVIC_ST_CAL_NOREF                0x80000000
#define  CPU_REG_NVIC_ST_CAL_SKEW                 0x40000000

//                                                                 -------------- INT CTRL STATE REG BITS -------------
#define  CPU_REG_NVIC_ICSR_NMIPENDSET             0x80000000
#define  CPU_REG_NVIC_ICSR_PENDSVSET              0x10000000
#define  CPU_REG_NVIC_ICSR_PENDSVCLR              0x08000000
#define  CPU_REG_NVIC_ICSR_PENDSTSET              0x04000000
#define  CPU_REG_NVIC_ICSR_PENDSTCLR              0x02000000
#define  CPU_REG_NVIC_ICSR_ISRPREEMPT             0x00800000
#define  CPU_REG_NVIC_ICSR_ISRPENDING             0x00400000

//                                                                 ------------- VECT TBL OFFSET REG BITS -------------
#define  CPU_REG_NVIC_VTOR_TBLBASE                0x20000000

//                                                                 ------------ APP INT/RESET CTRL REG BITS -----------
#define  CPU_REG_NVIC_AIRCR_ENDIANNESS            0x00008000
#define  CPU_REG_NVIC_AIRCR_SYSRESETREQ           0x00000004
#define  CPU_REG_NVIC_AIRCR_VECTCLRACTIVE         0x00000002

//                                                                 --------------- SYSTEM CTRL REG BITS ---------------
#define  CPU_REG_NVIC_SCR_SEVONPEND               0x00000010
#define  CPU_REG_NVIC_SCR_SLEEPDEEP               0x00000004
#define  CPU_REG_NVIC_SCR_SLEEPONEXIT             0x00000002

//                                                                 ----------------- CFG CTRL REG BITS ----------------
#define  CPU_REG_NVIC_CCR_STKALIGN                0x00000200
#define  CPU_REG_NVIC_CCR_UNALIGN_TRP             0x00000008

//                                                                 ------- SYSTEM HANDLER CTRL & STATE REG BITS -------
#define  CPU_REG_NVIC_SHCSR_SVCALLPENDED          0x00008000

//                                                                 ------------ DEBUG FAULT STATUS REG BITS -----------
#define  CPU_REG_NVIC_DFSR_EXTERNAL               0x00000010
#define  CPU_REG_NVIC_DFSR_VCATCH                 0x00000008
#define  CPU_REG_NVIC_DFSR_DWTTRAP                0x00000004
#define  CPU_REG_NVIC_DFSR_BKPT                   0x00000002
#define  CPU_REG_NVIC_DFSR_HALTED                 0x00000001

/********************************************************************************************************
 *                                          CPU REGISTER MASK
 *******************************************************************************************************/

#define  CPU_MSK_NVIC_ICSR_VECT_ACTIVE            0x000001FF

/********************************************************************************************************
 *                                        CONFIGURATION ERRORS
 *******************************************************************************************************/

#ifndef  CPU_CFG_ADDR_SIZE
#error  "CPU_CFG_ADDR_SIZE              not #define'd in 'cpu.h'               "
#error  "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_64  64-bit alignment]"

#elif  ((CPU_CFG_ADDR_SIZE != CPU_WORD_SIZE_08) \
  && (CPU_CFG_ADDR_SIZE != CPU_WORD_SIZE_16)    \
  && (CPU_CFG_ADDR_SIZE != CPU_WORD_SIZE_32)    \
  && (CPU_CFG_ADDR_SIZE != CPU_WORD_SIZE_64))
#error  "CPU_CFG_ADDR_SIZE        illegally #define'd in 'cpu.h'               "
#error  "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_64  64-bit alignment]"
#endif

#ifndef  CPU_CFG_DATA_SIZE
#error  "CPU_CFG_DATA_SIZE              not #define'd in 'cpu.h'               "
#error  "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_64  64-bit alignment]"

#elif  ((CPU_CFG_DATA_SIZE != CPU_WORD_SIZE_08) \
  && (CPU_CFG_DATA_SIZE != CPU_WORD_SIZE_16)    \
  && (CPU_CFG_DATA_SIZE != CPU_WORD_SIZE_32)    \
  && (CPU_CFG_DATA_SIZE != CPU_WORD_SIZE_64))
#error  "CPU_CFG_DATA_SIZE        illegally #define'd in 'cpu.h'               "
#error  "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_64  64-bit alignment]"
#endif

#ifndef  CPU_CFG_DATA_SIZE_MAX
#error  "CPU_CFG_DATA_SIZE_MAX          not #define'd in 'cpu.h'               "
#error  "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_64  64-bit alignment]"

#elif  ((CPU_CFG_DATA_SIZE_MAX != CPU_WORD_SIZE_08) \
  && (CPU_CFG_DATA_SIZE_MAX != CPU_WORD_SIZE_16)    \
  && (CPU_CFG_DATA_SIZE_MAX != CPU_WORD_SIZE_32)    \
  && (CPU_CFG_DATA_SIZE_MAX != CPU_WORD_SIZE_64))
#error  "CPU_CFG_DATA_SIZE_MAX    illegally #define'd in 'cpu.h'               "
#error  "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"
#error  "                         [     ||  CPU_WORD_SIZE_64  64-bit alignment]"
#endif

#if     (CPU_CFG_DATA_SIZE_MAX < CPU_CFG_DATA_SIZE)
#error  "CPU_CFG_DATA_SIZE_MAX    illegally #define'd in 'cpu.h' "
#error  "                         [MUST be  >= CPU_CFG_DATA_SIZE]"
#endif

#ifndef  CPU_CFG_ENDIAN_TYPE
#error  "CPU_CFG_ENDIAN_TYPE            not #define'd in 'cpu.h'   "
#error  "                         [MUST be  CPU_ENDIAN_TYPE_BIG   ]"
#error  "                         [     ||  CPU_ENDIAN_TYPE_LITTLE]"

#elif  ((CPU_CFG_ENDIAN_TYPE != CPU_ENDIAN_TYPE_BIG) \
  && (CPU_CFG_ENDIAN_TYPE != CPU_ENDIAN_TYPE_LITTLE))
#error  "CPU_CFG_ENDIAN_TYPE      illegally #define'd in 'cpu.h'   "
#error  "                         [MUST be  CPU_ENDIAN_TYPE_BIG   ]"
#error  "                         [     ||  CPU_ENDIAN_TYPE_LITTLE]"
#endif

#ifndef  CPU_CFG_STK_GROWTH
#error  "CPU_CFG_STK_GROWTH             not #define'd in 'cpu.h'    "
#error  "                         [MUST be  CPU_STK_GROWTH_LO_TO_HI]"
#error  "                         [     ||  CPU_STK_GROWTH_HI_TO_LO]"

#elif  ((CPU_CFG_STK_GROWTH != CPU_STK_GROWTH_LO_TO_HI) \
  && (CPU_CFG_STK_GROWTH != CPU_STK_GROWTH_HI_TO_LO))
#error  "CPU_CFG_STK_GROWTH       illegally #define'd in 'cpu.h'    "
#error  "                         [MUST be  CPU_STK_GROWTH_LO_TO_HI]"
#error  "                         [     ||  CPU_STK_GROWTH_HI_TO_LO]"
#endif

/********************************************************************************************************
 *                                             MODULE END
 *
 * @note     (1) See 'cpu.h  MODULE'.
 *******************************************************************************************************/

#endif // End of CPU module include.
