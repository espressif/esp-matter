/***************************************************************************//**
 * @file
 * @brief Common Configuration - Configuration Template File
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

// <<< Use Configuration Wizard in Context Menu >>>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _COMMON_CFG_H_
#define  _COMMON_CFG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                     MEMORY LIBRARY CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/
// <h>Memory Library Configuration

// <q LIB_MEM_CFG_STD_C_LIB_EN> Replace common lib memory functions with standard C lib functions
// <i> Cause the following functions replacement: Mem_Set() -> memset(), Mem_Copy() -> memcpy(),
// <i> Mem_Move() -> memmove(), Mem_Clr() -> memset() and Mem_Cmp() -> memcmp() functions.
// <i> Default: 0
#define  LIB_MEM_CFG_STD_C_LIB_EN                           0

// <q LIB_MEM_CFG_DBG_INFO_EN> Enable Memory allocation usage tracking
// <i> Associates a name with each segment or dynamic pool allocated.
// <i> A table of the different allocations can be displayed. For debugging purposes.
// <i> Default: 0
#define  LIB_MEM_CFG_DBG_INFO_EN                            0

// <o LIB_MEM_CFG_HEAP_SIZE> Size of heap memory (in octets).
// <i> Size, in octets, of the general-purpose heap memory used as default memory segment.
// <i> Default: 9216
#define  LIB_MEM_CFG_HEAP_SIZE                              9216

// <o LIB_MEM_CFG_HEAP_PADDING_ALIGN> Padding alignment for hardware allocations on heap (in octets)
// <i> Padding alignment that will be added at the end of each memory allocation on heap when "HW" functions are used.
// <i> Only useful when data cache is used. Should be set to the size of a cache line.
// <i> 1 means no alignment.
// <i> Default: 1
#define  LIB_MEM_CFG_HEAP_PADDING_ALIGN                     1

// <q LIB_MEM_CFG_HEAP_BASE_ADDR_EN> Enable Custom heap location
// <i> If enabled, the location must be specified by defining LIB_MEM_CFG_HEAP_BASE_ADDR to the desired address
// <i> either in "common_cfg.h" or from the compiler defines list.
// <i> Default: 0
#define  LIB_MEM_CFG_HEAP_BASE_ADDR_EN                      0
// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                     STRING LIBRARY CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>String Library Configuration

// <q LIB_STR_CFG_FP_EN> Support conversion to string of floating point numbers
// <i> Default: 0
#define  LIB_STR_CFG_FP_EN                                  0

// <o LIB_STR_CFG_FP_MAX_NBR_DIG_SIG> Maximum number of significant digits to display in string conversion of floating point number functions <1-9>
// <i> Default: 7
#define  LIB_STR_CFG_FP_MAX_NBR_DIG_SIG                     7
// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of common_cfg.h module include.

// <<< end of configuration section >>>
