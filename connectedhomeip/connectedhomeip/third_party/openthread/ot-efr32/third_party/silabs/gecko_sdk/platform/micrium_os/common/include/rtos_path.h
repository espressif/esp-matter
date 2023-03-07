/***************************************************************************//**
 * @file
 * @brief Common - RTOS Path
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _RTOS_PATH_H_
#define  _RTOS_PATH_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>
#if (RTOS_CPU_SEL == RTOS_CPU_SEL_SILABS_GECKO_AUTO)
#include "em_device.h"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if ((!defined(RTOS_CPU_SEL)))
#error  "RTOS_CPU_SEL not defined, please #define RTOS_CPU_SEL to a known MCU. Supported MCUs can be found in common/include/rtos_opt_def.h."
#endif

#if (RTOS_CPU_SEL == RTOS_CPU_SEL_NONE)
#error  "RTOS_CPU_SEL must be #define'd to a known MCU. Supported MCUs can be found in common/include/rtos_opt_def.h."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 Auto selection of toolchain.
#if ((!defined(RTOS_TOOLCHAIN_SEL)) \
  || (RTOS_TOOLCHAIN_SEL == RTOS_TOOLCHAIN_AUTO))

    #if (defined(__IAR_SYSTEMS_ICC__))
        #define RTOS_TOOLCHAIN      RTOS_TOOLCHAIN_IAR

    #elif defined(_MSC_VER)
        #define RTOS_TOOLCHAIN      RTOS_TOOLCHAIN_WIN32

    #elif (defined(__ARMCC_VERSION))
        #define RTOS_TOOLCHAIN      RTOS_TOOLCHAIN_ARMCC

    #elif (defined(__GNUC__) || defined(__GNUG__))
        #if (defined(__arm__))
//                                                                 GNU for ARM arch.
            #define RTOS_TOOLCHAIN  RTOS_TOOLCHAIN_GNU
        #else
//                                                                 Generic GNU toolchain.
            #define RTOS_TOOLCHAIN  RTOS_TOOLCHAIN_GNU
        #endif
    #else
        #error Unable to detect toolchain. Set RTOS_CFG_TOOLCHAIN.
    #endif

#else
    #define RTOS_TOOLCHAIN          RTOS_TOOLCHAIN_SEL
#endif

#define CORTEX_M0        0x00
#define CORTEX_M0P       0x00
#define CORTEX_M3        0x03
#define CORTEX_M4        0x04
#define CORTEX_M7        0x07
#define CORTEX_M33        33u

/********************************************************************************************************
 *                                   CPU, INTERRUPT & TOOLCHAIN NAMES
 *******************************************************************************************************/

//                                                                 CPU Selector.
#if (RTOS_CPU_SEL == RTOS_CPU_SEL_SILABS_GECKO_AUTO)

    #if ((__CORTEX_M == CORTEX_M0) || (__CORTEX_M == CORTEX_M0P))
        #define  RTOS_CPU_PORT_NAME         RTOS_CPU_SEL_ARM_V6_M
    #elif ((__CORTEX_M == CORTEX_M3) \
  || (__CORTEX_M == CORTEX_M4)       \
  || (__CORTEX_M == CORTEX_M7))
        #define  RTOS_CPU_PORT_NAME         RTOS_CPU_SEL_ARM_V7_M
    #elif (__CORTEX_M == CORTEX_M33)
        #define  RTOS_CPU_PORT_NAME         RTOS_CPU_SEL_ARM_V8_M
    #else
        #define  RTOS_CPU_PORT_NAME         RTOS_CPU_SEL_NONE
    #endif

#elif ((RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M3) \
  || (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M4)   \
  || (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M7)   \
  || (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_V7_M))
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_ARM_V7_M

#elif ((RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M0) \
  || (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M0P)  \
  || (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M1)   \
  || (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_V6_M))
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_ARM_V6_M

#elif (RTOS_CPU_SEL == RTOS_CPU_SEL_EMUL_POSIX)
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_EMUL_POSIX

#elif (RTOS_CPU_SEL == RTOS_CPU_SEL_EMUL_WIN32)
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_EMUL_WIN32

#elif (RTOS_CPU_SEL == RTOS_CPU_SEL_EMPTY)
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_EMPTY
#else
#error  Unsupported RTOS_CPU_SEL specified.
#endif

//                                                                 Toolchain Selector.
#if ((RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_ARMCC) \
  || (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_GNU)   \
  || (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_IAR)   \
  || (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_WIN32))
#define  RTOS_TOOLCHAIN_NAME            RTOS_TOOLCHAIN
#else
#error  Unsupported RTOS_TOOLCHAIN specified.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of rtos path module include.
