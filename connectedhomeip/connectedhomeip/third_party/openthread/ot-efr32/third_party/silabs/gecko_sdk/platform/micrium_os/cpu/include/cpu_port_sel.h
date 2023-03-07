/***************************************************************************//**
 * @file
 * @brief CPU Port Selector
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

#ifndef  _CPU_PORT_SEL_H_
#define  _CPU_PORT_SEL_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>

#ifndef CPU_PORT_PATH
    #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M) \
  || (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V8_M))
        #if   (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_ARMCC)
            #include  <ports/source/armcc/armv7m_cpu_port.h>
        #elif (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_GNU)
            #include  <ports/source/gnu/armv7m_cpu_port.h>
        #elif (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_IAR)
            #include  <ports/source/iar/armv7m_cpu_port.h>
        #else
            #warning  Unknown toolchain
        #endif

    #elif (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        #if   (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_ARMCC)
            #include  <ports/source/armcc/armv6m_cpu_port.h>
        #elif (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_GNU)
            #include  <ports/source/gnu/armv6m_cpu_port.h>
        #elif (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_IAR)
            #include  <ports/source/iar/armv6m_cpu_port.h>
        #else
            #warning  Unknown toolchain
        #endif

    #elif (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMUL_POSIX)
        #if   (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_GNU)
            #include  <ports/source/gnu/posix_cpu_port.h>
        #else
            #warning  Unknown toolchain
        #endif

    #elif (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMUL_WIN32)
        #if   ((RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_WIN32) \
  || (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_GNU))
        #include  <ports/source/win32/win32_cpu_port.h>
        #include  "emul_nvic.h"
        #else
            #warning  Unknown toolchain
        #endif

    #else
        #warning  Unknown port
    #endif

#else
    #include  CPU_PORT_PATH
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of CPU port selector module include.
