/***************************************************************************//**
 * @file
 * @brief Common - RTOS Options Defines
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

#ifndef  _RTOS_OPT_DEF_H_
#define  _RTOS_OPT_DEF_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CPU ARCHITECTURES
 *******************************************************************************************************/

#define  RTOS_CPU_SEL_NONE                          0u

#define  RTOS_CPU_SEL_SILABS_GECKO_AUTO             1u

//                                                                 Arm
#define  RTOS_CPU_SEL_ARM_CORTEX_M0                 11u
#define  RTOS_CPU_SEL_ARM_CORTEX_M0P                12u
#define  RTOS_CPU_SEL_ARM_CORTEX_M1                 13u
#define  RTOS_CPU_SEL_ARM_CORTEX_M3                 14u
#define  RTOS_CPU_SEL_ARM_CORTEX_M4                 15u
#define  RTOS_CPU_SEL_ARM_CORTEX_M7                 16u
#define  RTOS_CPU_SEL_ARM_CORTEX_M33                17u
#define  RTOS_CPU_SEL_ARM_V7_M                      18u
#define  RTOS_CPU_SEL_ARM_V6_M                      19u
#define  RTOS_CPU_SEL_ARM_V8_M                      20u

//                                                                 Emulation
#define  RTOS_CPU_SEL_EMUL_WIN32                    21u
#define  RTOS_CPU_SEL_EMUL_POSIX                    22u

//                                                                 Empty
#define  RTOS_CPU_SEL_EMPTY                         30u

/********************************************************************************************************
 *                                               TOOLCHAINS
 *******************************************************************************************************/

#define  RTOS_TOOLCHAIN_NONE                    0u

#define  RTOS_TOOLCHAIN_ARMCC                   1u
#define  RTOS_TOOLCHAIN_GNU                     2u
#define  RTOS_TOOLCHAIN_IAR                     3u
#define  RTOS_TOOLCHAIN_WIN32                   4u

#define  RTOS_TOOLCHAIN_AUTO                    0xFFFFFFFFu     // Automatic detection of toolchain.

/********************************************************************************************************
 *                                           INTERRUPT CONTROLLERS
 *******************************************************************************************************/

#define  RTOS_INT_CONTROLLER_NONE               0u
#define  RTOS_INT_CONTROLLER_ARMV7_M            1u
#define  RTOS_INT_CONTROLLER_ARMV6_M            2u
#define  RTOS_INT_CONTROLLER_ARMV8_M            3u

#define  RTOS_INT_CONTROLLER_AUTO               0xFFFFFFFFu     // Automatic detection of interrupt controller.

/********************************************************************************************************
 *                                               MODULES DEFINES
 *******************************************************************************************************/

#define  RTOS_CFG_MODULE_NONE               0x0000u
#define  RTOS_CFG_MODULE_ALL                0xFFFFu

#define  RTOS_CFG_MODULE_APP                0x0001u
#define  RTOS_CFG_MODULE_BSP                0x0002u
#define  RTOS_CFG_MODULE_ALL_APP            0x0003u

#define  RTOS_CFG_MODULE_CAN                0x0010u
#define  RTOS_CFG_MODULE_COMMON             0x0020u
#define  RTOS_CFG_MODULE_CPU                0x0040u
#define  RTOS_CFG_MODULE_FS                 0x0080u
#define  RTOS_CFG_MODULE_KERNEL             0x0100u
#define  RTOS_CFG_MODULE_NET                0x0200u
#define  RTOS_CFG_MODULE_NET_APP            0x0400u
#define  RTOS_CFG_MODULE_USBD               0x0800u
#define  RTOS_CFG_MODULE_USBH               0x1000u
#define  RTOS_CFG_MODULE_IO                 0x2000u
#define  RTOS_CFG_MODULE_CANOPEN            0x4000u
#define  RTOS_CFG_MODULE_PROBE              0x8000u
#define  RTOS_CFG_MODULE_ALL_PRODUCTS       0xFFF0u

/********************************************************************************************************
 ********************************************************************************************************
 *                                ASSERTION END CALL SELECTION DEFINES
 ********************************************************************************************************
 */

#define  RTOS_ASSERT_END_CALL_SEL_TRAP                      2
#define  RTOS_ASSERT_END_CALL_SEL_RETURN                    3
#define  RTOS_ASSERT_END_CALL_SEL_CUSTOM                    4

/*
 ********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of rtos opt def module include.
