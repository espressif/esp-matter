/***************************************************************************//**
 * @file
 * @brief Kernel Types
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
 * @defgroup KERNEL_TYPES Kernel Types API
 * @ingroup  KERNEL
 * @brief   Kernel Types API
 * @addtogroup KERNEL_TYPES
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _OS_TYPE_H_
#define  _OS_TYPE_H_

#ifdef    VSC_INCLUDE_H_FILE_NAMES
const CPU_CHAR *os_type__h = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Description                             # Bits
//                                                                 <recommended>
//                                                                 ----------------------------------------------------

typedef CPU_INT16U OS_CPU_USAGE;                                ///< CPU Usage 0..10000                           <16>/32

typedef CPU_INT32U OS_CTR;                                      ///< Counter                                           32

typedef CPU_INT32U OS_CTX_SW_CTR;                               ///< Counter of context switches                       32

typedef CPU_INT32U OS_CYCLES;                                   ///< CPU clock cycles                             <32>/64

typedef CPU_INT32U OS_FLAGS;                                    ///< Event flags                                8/16/<32>

typedef CPU_INT32U OS_IDLE_CTR;                                 ///< Holds number of times the idle task runs     <32>/64

typedef CPU_INT16U OS_MSG_QTY;                                  ///< Number of OS_MSGs in the msg pool            <16>/32
typedef CPU_INT16U OS_MSG_SIZE;                                 ///< Size of messages in number of bytes          <16>/32

typedef CPU_INT08U OS_NESTING_CTR;                              ///< Interrupt and scheduler nesting            <8>/16/32

typedef CPU_INT16U OS_OBJ_QTY;                                  ///< Number of kernel objects counter             <16>/32
typedef CPU_INT32U OS_OBJ_TYPE;                                 ///< Special flag to determine object type             32

typedef CPU_INT16U OS_OPT;                                      ///< Holds function options                       <16>/32

typedef CPU_INT32U OS_MON_RES;                                  ///< Monitor result flags

typedef CPU_INT16U OS_QTY;                                      ///< Quantity                                     <16>/32

typedef CPU_INT32U OS_RATE_HZ;                                  ///< Rate in Hertz                                     32

#if (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_64)                     ///< Task register                           8/16/<32/64>
typedef CPU_INT64U OS_REG;
#else
typedef CPU_INT32U OS_REG;
#endif
typedef CPU_INT08U OS_REG_ID;                                   ///< Index to task register                     <8>/16/32

typedef CPU_INT32U OS_SEM_CTR;                                  ///< Semaphore value                              16/<32>

typedef CPU_INT08U OS_STATE;                                    ///< State variable                             <8>/16/32

typedef CPU_INT08U OS_STATUS;                                   ///< Status                                     <8>/16/32

typedef uint32_t OS_TICK;                                       ///< Clock tick counter                           <32>/64

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of Kernel types module include.
