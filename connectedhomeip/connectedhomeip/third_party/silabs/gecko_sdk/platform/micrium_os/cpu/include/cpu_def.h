/***************************************************************************//**
 * @file
 * @brief CPU Configuration Defines
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _CPU_DEF_H_
#define  _CPU_DEF_H_

/********************************************************************************************************
 *                                           CPU WORD CONFIGURATION
 *
 * Note(s) : (1) Configure CPU_CFG_ADDR_SIZE & CPU_CFG_DATA_SIZE in '[arch]_cpu_port.h' with CPU's word sizes :
 *
 *                   CPU_WORD_SIZE_08             8-bit word size
 *                   CPU_WORD_SIZE_16            16-bit word size
 *                   CPU_WORD_SIZE_32            32-bit word size
 *                   CPU_WORD_SIZE_64            64-bit word size
 *
 *           (2) Configure CPU_CFG_ENDIAN_TYPE in '[arch]_cpu_port.h' with CPU's data-word-memory order :
 *
 *               (a) CPU_ENDIAN_TYPE_BIG         Big-   endian word order (CPU words' most  significant
 *                                                                           octet @ lowest memory address)
 *               (b) CPU_ENDIAN_TYPE_LITTLE      Little-endian word order (CPU words' least significant
 *                                                                           octet @ lowest memory address)
 *******************************************************************************************************/

//                                                                 ------------------ CPU WORD SIZE -------------------
#define  CPU_WORD_SIZE_08                          1u           // 8-bit word size (in octets).
#define  CPU_WORD_SIZE_16                          2u           // 16-bit word size (in octets).
#define  CPU_WORD_SIZE_32                          4u           // 32-bit word size (in octets).
#define  CPU_WORD_SIZE_64                          8u           // 64-bit word size (in octets).

//                                                                 -------------- CPU WORD-ENDIAN ORDER ---------------
#define  CPU_ENDIAN_TYPE_NONE                      0u
#define  CPU_ENDIAN_TYPE_BIG                       1u           // Big-   endian word order (see Note #1a).
#define  CPU_ENDIAN_TYPE_LITTLE                    2u           // Little-endian word order (see Note #1b).

/********************************************************************************************************
 *                                           CPU STACK CONFIGURATION
 *
 * Note(s) : (1) Configure CPU_CFG_STK_GROWTH in '[arch]_cpu_port.h' with CPU's stack growth order :
 *
 *               (a) CPU_STK_GROWTH_LO_TO_HI     CPU stack pointer increments to the next higher  stack
 *                                                   memory address after data is pushed onto the stack
 *               (b) CPU_STK_GROWTH_HI_TO_LO     CPU stack pointer decrements to the next lower   stack
 *                                                   memory address after data is pushed onto the stack
 *******************************************************************************************************/

//                                                                 -------------- CPU STACK GROWTH ORDER --------------
#define  CPU_STK_GROWTH_NONE                       0u
#define  CPU_STK_GROWTH_LO_TO_HI                   1u           // CPU stk incs towards higher mem addrs (see Note #1a).
#define  CPU_STK_GROWTH_HI_TO_LO                   2u           // CPU stk decs towards lower  mem addrs (see Note #1b).

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of CPU def module include.
