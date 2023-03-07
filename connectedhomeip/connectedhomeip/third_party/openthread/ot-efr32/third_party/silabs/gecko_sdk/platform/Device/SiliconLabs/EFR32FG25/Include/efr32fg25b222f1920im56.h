/**************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M Peripheral Access Layer Header File
 *        for EFR32FG25B222F1920IM56
 ******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *****************************************************************************/
#ifndef EFR32FG25B222F1920IM56_H
#define EFR32FG25B222F1920IM56_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * @addtogroup Parts
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup EFR32FG25B222F1920IM56 EFR32FG25B222F1920IM56
 * @{
 *****************************************************************************/

/** Interrupt Number Definition */
typedef enum IRQn{
  /******  Cortex-M Processor Exceptions Numbers ******************************************/
  NonMaskableInt_IRQn    = -14,             /*!< -14 Cortex-M Non Maskable Interrupt      */
  HardFault_IRQn         = -13,             /*!< -13 Cortex-M Hard Fault Interrupt        */
  MemoryManagement_IRQn  = -12,             /*!< -12 Cortex-M Memory Management Interrupt */
  BusFault_IRQn          = -11,             /*!< -11 Cortex-M Bus Fault Interrupt         */
  UsageFault_IRQn        = -10,             /*!< -10 Cortex-M Usage Fault Interrupt       */
  SVCall_IRQn            = -5,              /*!< -5  Cortex-M SV Call Interrupt           */
  DebugMonitor_IRQn      = -4,              /*!< -4  Cortex-M Debug Monitor Interrupt     */
  PendSV_IRQn            = -2,              /*!< -2  Cortex-M Pend SV Interrupt           */
  SysTick_IRQn           = -1,              /*!< -1  Cortex-M System Tick Interrupt       */

  /******  EFR32FG25 Peripheral Interrupt Numbers ******************************************/

  SMU_SECURE_IRQn        = 0,  /*!<  0 EFR32 SMU_SECURE Interrupt */
  SMU_S_PRIVILEGED_IRQn  = 1,  /*!<  1 EFR32 SMU_S_PRIVILEGED Interrupt */
  SMU_NS_PRIVILEGED_IRQn = 2,  /*!<  2 EFR32 SMU_NS_PRIVILEGED Interrupt */
  EMU_IRQn               = 3,  /*!<  3 EFR32 EMU Interrupt */
  TIMER0_IRQn            = 4,  /*!<  4 EFR32 TIMER0 Interrupt */
  TIMER1_IRQn            = 5,  /*!<  5 EFR32 TIMER1 Interrupt */
  TIMER2_IRQn            = 6,  /*!<  6 EFR32 TIMER2 Interrupt */
  TIMER3_IRQn            = 7,  /*!<  7 EFR32 TIMER3 Interrupt */
  ETAMPDET_IRQn          = 8,  /*!<  8 EFR32 ETAMPDET Interrupt */
  TIMER4_IRQn            = 9,  /*!<  9 EFR32 TIMER4 Interrupt */
  TIMER5_IRQn            = 10, /*!< 10 EFR32 TIMER5 Interrupt */
  TIMER6_IRQn            = 11, /*!< 11 EFR32 TIMER6 Interrupt */
  TIMER7_IRQn            = 12, /*!< 12 EFR32 TIMER7 Interrupt */
  EUSART0_RX_IRQn        = 13, /*!< 13 EFR32 EUSART0_RX Interrupt */
  EUSART0_TX_IRQn        = 14, /*!< 14 EFR32 EUSART0_TX Interrupt */
  EUSART1_RX_IRQn        = 15, /*!< 15 EFR32 EUSART1_RX Interrupt */
  EUSART1_TX_IRQn        = 16, /*!< 16 EFR32 EUSART1_TX Interrupt */
  EUSART2_RX_IRQn        = 17, /*!< 17 EFR32 EUSART2_RX Interrupt */
  EUSART2_TX_IRQn        = 18, /*!< 18 EFR32 EUSART2_TX Interrupt */
  EUSART3_RX_IRQn        = 19, /*!< 19 EFR32 EUSART3_RX Interrupt */
  EUSART3_TX_IRQn        = 20, /*!< 20 EFR32 EUSART3_TX Interrupt */
  EUSART4_RX_IRQn        = 21, /*!< 21 EFR32 EUSART4_RX Interrupt */
  EUSART4_TX_IRQn        = 22, /*!< 22 EFR32 EUSART4_TX Interrupt */
  ICACHE0_IRQn           = 23, /*!< 23 EFR32 ICACHE0 Interrupt */
  BURTC_IRQn             = 24, /*!< 24 EFR32 BURTC Interrupt */
  LETIMER0_IRQn          = 25, /*!< 25 EFR32 LETIMER0 Interrupt */
  SYSCFG_IRQn            = 26, /*!< 26 EFR32 SYSCFG Interrupt */
  MPAHBRAM_IRQn          = 27, /*!< 27 EFR32 MPAHBRAM Interrupt */
  LDMA_IRQn              = 28, /*!< 28 EFR32 LDMA Interrupt */
  LFXO_IRQn              = 29, /*!< 29 EFR32 LFXO Interrupt */
  LFRCO_IRQn             = 30, /*!< 30 EFR32 LFRCO Interrupt */
  ULFRCO_IRQn            = 31, /*!< 31 EFR32 ULFRCO Interrupt */
  GPIO_ODD_IRQn          = 32, /*!< 32 EFR32 GPIO_ODD Interrupt */
  GPIO_EVEN_IRQn         = 33, /*!< 33 EFR32 GPIO_EVEN Interrupt */
  I2C0_IRQn              = 34, /*!< 34 EFR32 I2C0 Interrupt */
  I2C1_IRQn              = 35, /*!< 35 EFR32 I2C1 Interrupt */
  EMUDG_IRQn             = 36, /*!< 36 EFR32 EMUDG Interrupt */
  AGC_IRQn               = 37, /*!< 37 EFR32 AGC Interrupt */
  BUFC_IRQn              = 38, /*!< 38 EFR32 BUFC Interrupt */
  FRC_PRI_IRQn           = 39, /*!< 39 EFR32 FRC_PRI Interrupt */
  FRC_IRQn               = 40, /*!< 40 EFR32 FRC Interrupt */
  MODEM_IRQn             = 41, /*!< 41 EFR32 MODEM Interrupt */
  PROTIMER_IRQn          = 42, /*!< 42 EFR32 PROTIMER Interrupt */
  RAC_RSM_IRQn           = 43, /*!< 43 EFR32 RAC_RSM Interrupt */
  RAC_SEQ_IRQn           = 44, /*!< 44 EFR32 RAC_SEQ Interrupt */
  HOSTMAILBOX_IRQn       = 45, /*!< 45 EFR32 HOSTMAILBOX Interrupt */
  SYNTH_IRQn             = 46, /*!< 46 EFR32 SYNTH Interrupt */
  ACMP0_IRQn             = 47, /*!< 47 EFR32 ACMP0 Interrupt */
  ACMP1_IRQn             = 48, /*!< 48 EFR32 ACMP1 Interrupt */
  WDOG0_IRQn             = 49, /*!< 49 EFR32 WDOG0 Interrupt */
  WDOG1_IRQn             = 50, /*!< 50 EFR32 WDOG1 Interrupt */
  HFXO0_IRQn             = 51, /*!< 51 EFR32 HFXO0 Interrupt */
  HFRCO0_IRQn            = 52, /*!< 52 EFR32 HFRCO0 Interrupt */
  HFRCOEM23_IRQn         = 53, /*!< 53 EFR32 HFRCOEM23 Interrupt */
  CMU_IRQn               = 54, /*!< 54 EFR32 CMU Interrupt */
  AES_IRQn               = 55, /*!< 55 EFR32 AES Interrupt */
  IADC_IRQn              = 56, /*!< 56 EFR32 IADC Interrupt */
  MSC_IRQn               = 57, /*!< 57 EFR32 MSC Interrupt */
  DPLL0_IRQn             = 58, /*!< 58 EFR32 DPLL0 Interrupt */
  DCDC_IRQn              = 60, /*!< 60 EFR32 DCDC Interrupt */
  VDAC_IRQn              = 61, /*!< 61 EFR32 VDAC Interrupt */
  PCNT0_IRQn             = 62, /*!< 62 EFR32 PCNT0 Interrupt */
  USB_IRQn               = 63, /*!< 63 EFR32 USB Interrupt */
  SW0_IRQn               = 64, /*!< 64 EFR32 SW0 Interrupt */
  SW1_IRQn               = 65, /*!< 65 EFR32 SW1 Interrupt */
  SW2_IRQn               = 66, /*!< 66 EFR32 SW2 Interrupt */
  SW3_IRQn               = 67, /*!< 67 EFR32 SW3 Interrupt */
  KERNEL0_IRQn           = 68, /*!< 68 EFR32 KERNEL0 Interrupt */
  KERNEL1_IRQn           = 69, /*!< 69 EFR32 KERNEL1 Interrupt */
  M33CTI0_IRQn           = 70, /*!< 70 EFR32 M33CTI0 Interrupt */
  M33CTI1_IRQn           = 71, /*!< 71 EFR32 M33CTI1 Interrupt */
  FPUEXH_IRQn            = 72, /*!< 72 EFR32 FPUEXH Interrupt */
  SETAMPERHOST_IRQn      = 73, /*!< 73 EFR32 SETAMPERHOST Interrupt */
  SEMBRX_IRQn            = 74, /*!< 74 EFR32 SEMBRX Interrupt */
  SEMBTX_IRQn            = 75, /*!< 75 EFR32 SEMBTX Interrupt */
  LESENSE_IRQn           = 76, /*!< 76 EFR32 LESENSE Interrupt */
  SYSRTC_APP_IRQn        = 77, /*!< 77 EFR32 SYSRTC_APP Interrupt */
  SYSRTC_SEQ_IRQn        = 78, /*!< 78 EFR32 SYSRTC_SEQ Interrupt */
  RFECA0_IRQn            = 79, /*!< 79 EFR32 RFECA0 Interrupt */
  RFECA1_IRQn            = 80, /*!< 80 EFR32 RFECA1 Interrupt */
  RFFPLL_IRQn            = 81, /*!< 81 EFR32 RFFPLL Interrupt */
  USBPLL0_IRQn           = 82, /*!< 82 EFR32 USBPLL0 Interrupt */
  AHB2AHB0_IRQn          = 83, /*!< 83 EFR32 AHB2AHB0 Interrupt */
  AHB2AHB1_IRQn          = 84, /*!< 84 EFR32 AHB2AHB1 Interrupt */
  AHB2AHB2_IRQn          = 85, /*!< 85 EFR32 AHB2AHB2 Interrupt */
  AHB2AHB3_IRQn          = 86, /*!< 86 EFR32 AHB2AHB3 Interrupt */
  RFTIMER_IRQn           = 87, /*!< 87 EFR32 RFTIMER Interrupt */
  SOFTM_IRQn             = 88, /*!< 88 EFR32 SOFTM Interrupt */
  RFLDMA_IRQn            = 89, /*!< 89 EFR32 RFLDMA Interrupt */
} IRQn_Type;

/**************************************************************************//**
 * @defgroup EFR32FG25B222F1920IM56_Core EFR32FG25B222F1920IM56 Core
 * @{
 * @brief Processor and Core Peripheral Section
 *****************************************************************************/

#define __CM33_REV                0x0004U /**< Cortex-M33 Core revision */
#define __DSP_PRESENT             1U      /**< Presence of DSP  */
#define __FPU_PRESENT             1U      /**< Presence of FPU  */
#define __MPU_PRESENT             1U      /**< Presence of MPU  */
#define __SAUREGION_PRESENT       1U      /**< Presence of FPU  */
#define __TZ_PRESENT              1U      /**< Presence of TrustZone */
#define __VTOR_PRESENT            1U      /**< Presence of VTOR register in SCB  */
#define __NVIC_PRIO_BITS          4U      /**< NVIC interrupt priority bits */
#define __Vendor_SysTickConfig    0U      /**< Is 1 if different SysTick counter is used */

/** @} End of group EFR32FG25B222F1920IM56_Core */

/**************************************************************************//**
* @defgroup EFR32FG25B222F1920IM56_Part EFR32FG25B222F1920IM56 Part
* @{
******************************************************************************/

/** Part number */

/* If part number is not defined as compiler option, define it */
#if !defined(EFR32FG25B222F1920IM56)
#define EFR32FG25B222F1920IM56    1 /**< FULL Part */
#endif

/** Configure part number */
#define PART_NUMBER                                        "EFR32FG25B222F1920IM56" /**< Part Number */

/** Family / Line / Series / Config */
#define _EFR32_FLEX_FAMILY                                 1                                    /** Device Family Name Identifier */
#define _EFR32_FG_FAMILY                                   1                                    /** Device Family Identifier */
#define _EFR_DEVICE                                        1                                    /** Product Line Identifier */
#define _SILICON_LABS_32B_SERIES_2                                                              /** Product Series Identifier */
#define _SILICON_LABS_32B_SERIES                           2                                    /** Product Series Identifier */
#define _SILICON_LABS_32B_SERIES_2_CONFIG_5                                                     /** Product Config Identifier */
#define _SILICON_LABS_32B_SERIES_2_CONFIG                  5                                    /** Product Config Identifier */
#define _SILICON_LABS_GECKO_INTERNAL_SDID                  220                                  /** Silicon Labs internal use only */
#define _SILICON_LABS_GECKO_INTERNAL_SDID_220                                                   /** Silicon Labs internal use only */
#define _SILICON_LABS_SECURITY_FEATURE_SE                  0                                    /** Mid */
#define _SILICON_LABS_SECURITY_FEATURE_VAULT               1                                    /** High */
#define _SILICON_LABS_SECURITY_FEATURE_ROT                 2                                    /** Root Of Trust */
#define _SILICON_LABS_SECURITY_FEATURE                     _SILICON_LABS_SECURITY_FEATURE_VAULT /** Security feature set */
#define _SILICON_LABS_DCDC_FEATURE_NOTUSED                 0                                    /** Not Used */
#define _SILICON_LABS_DCDC_FEATURE_DCDC_BUCK               1                                    /** Includes Buck DCDC */
#define _SILICON_LABS_DCDC_FEATURE_DCDC_BOOST              2                                    /** Includes Boost DCDC */
#define _SILICON_LABS_DCDC_FEATURE_DCDC_BOB                3                                    /** Includes Buck or Boost DCDC */
#define _SILICON_LABS_DCDC_FEATURE                         _SILICON_LABS_DCDC_FEATURE_DCDC_BUCK /** DCDC feature set */
#define _SILICON_LABS_EFR32_RADIO_NONE                     0                                    /** No radio present */
#define _SILICON_LABS_EFR32_RADIO_SUBGHZ                   1                                    /** Radio supports Sub-GHz */
#define _SILICON_LABS_EFR32_RADIO_2G4HZ                    2                                    /** Radio supports 2.4 GHz */
#define _SILICON_LABS_EFR32_RADIO_DUALBAND                 3                                    /** Radio supports dual band */
#define _SILICON_LABS_EFR32_RADIO_TYPE                     _SILICON_LABS_EFR32_RADIO_SUBGHZ     /** Radio type */
#define _SILICON_LABS_EFR32_SUBGHZ_HP_PA_MAX_OUTPUT_DBM    16                                   /** Radio SUBGHZ HP PA output power */
#define _SILICON_LABS_EFR32_SUBGHZ_HP_PA_PRESENT                                                /** Radio SUBGHZ HP PA is present */

/** Memory Base addresses and limits */
#define FLASH_MEM_BASE                                     (0x08000000UL) /** FLASH_MEM base address */
#define FLASH_MEM_SIZE                                     (0x001E0000UL) /** FLASH_MEM available address space */
#define FLASH_MEM_END                                      (0x081DFFFFUL) /** FLASH_MEM end address */
#define FLASH_MEM_BITS                                     (0x15UL)       /** FLASH_MEM used bits */
#define MSC_FLASH_MEM_BASE                                 (0x08000000UL) /** MSC_FLASH_MEM base address */
#define MSC_FLASH_MEM_SIZE                                 (0x001E0000UL) /** MSC_FLASH_MEM available address space */
#define MSC_FLASH_MEM_END                                  (0x081DFFFFUL) /** MSC_FLASH_MEM end address */
#define MSC_FLASH_MEM_BITS                                 (0x15UL)       /** MSC_FLASH_MEM used bits */
#define MSC_FLASH_USERDATA_MEM_BASE                        (0x0FE00000UL) /** MSC_FLASH_USERDATA_MEM base address */
#define MSC_FLASH_USERDATA_MEM_SIZE                        (0x00000400UL) /** MSC_FLASH_USERDATA_MEM available address space */
#define MSC_FLASH_USERDATA_MEM_END                         (0x0FE003FFUL) /** MSC_FLASH_USERDATA_MEM end address */
#define MSC_FLASH_USERDATA_MEM_BITS                        (0xBUL)        /** MSC_FLASH_USERDATA_MEM used bits */
#define USERDATA_BASE                                      (0x0FE00000UL) /** USERDATA base address */
#define USERDATA_SIZE                                      (0x00000400UL) /** USERDATA available address space */
#define USERDATA_END                                       (0x0FE003FFUL) /** USERDATA end address */
#define USERDATA_BITS                                      (0xBUL)        /** USERDATA used bits */
#define MSC_FLASH_DEVINFO_MEM_BASE                         (0x0FE08000UL) /** MSC_FLASH_DEVINFO_MEM base address */
#define MSC_FLASH_DEVINFO_MEM_SIZE                         (0x00000400UL) /** MSC_FLASH_DEVINFO_MEM available address space */
#define MSC_FLASH_DEVINFO_MEM_END                          (0x0FE083FFUL) /** MSC_FLASH_DEVINFO_MEM end address */
#define MSC_FLASH_DEVINFO_MEM_BITS                         (0xBUL)        /** MSC_FLASH_DEVINFO_MEM used bits */
#define MSC_FLASH_CHIPCONFIG_MEM_BASE                      (0x0FE08400UL) /** MSC_FLASH_CHIPCONFIG_MEM base address */
#define MSC_FLASH_CHIPCONFIG_MEM_SIZE                      (0x00000600UL) /** MSC_FLASH_CHIPCONFIG_MEM available address space */
#define MSC_FLASH_CHIPCONFIG_MEM_END                       (0x0FE089FFUL) /** MSC_FLASH_CHIPCONFIG_MEM end address */
#define MSC_FLASH_CHIPCONFIG_MEM_BITS                      (0xBUL)        /** MSC_FLASH_CHIPCONFIG_MEM used bits */
#define DMEM_RAM0_RAM_MEM_BASE                             (0x20000000UL) /** DMEM_RAM0_RAM_MEM base address */
#define DMEM_RAM0_RAM_MEM_SIZE                             (0x00080000UL) /** DMEM_RAM0_RAM_MEM available address space */
#define DMEM_RAM0_RAM_MEM_END                              (0x2007FFFFUL) /** DMEM_RAM0_RAM_MEM end address */
#define DMEM_RAM0_RAM_MEM_BITS                             (0x14UL)       /** DMEM_RAM0_RAM_MEM used bits */
#define RAM_MEM_BASE                                       (0x20000000UL) /** RAM_MEM base address */
#define RAM_MEM_SIZE                                       (0x00080000UL) /** RAM_MEM available address space */
#define RAM_MEM_END                                        (0x2007FFFFUL) /** RAM_MEM end address */
#define RAM_MEM_BITS                                       (0x14UL)       /** RAM_MEM used bits */
#define RDMEM_SEQRAM_S_MEM_BASE                            (0xA0000000UL) /** RDMEM_SEQRAM_S_MEM base address */
#define RDMEM_SEQRAM_S_MEM_SIZE                            (0x00006000UL) /** RDMEM_SEQRAM_S_MEM available address space */
#define RDMEM_SEQRAM_S_MEM_END                             (0xA0005FFFUL) /** RDMEM_SEQRAM_S_MEM end address */
#define RDMEM_SEQRAM_S_MEM_BITS                            (0xFUL)        /** RDMEM_SEQRAM_S_MEM used bits */
#define RDMEM_FRCRAM_S_MEM_BASE                            (0xA0006000UL) /** RDMEM_FRCRAM_S_MEM base address */
#define RDMEM_FRCRAM_S_MEM_SIZE                            (0x00001000UL) /** RDMEM_FRCRAM_S_MEM available address space */
#define RDMEM_FRCRAM_S_MEM_END                             (0xA0006FFFUL) /** RDMEM_FRCRAM_S_MEM end address */
#define RDMEM_FRCRAM_S_MEM_BITS                            (0xDUL)        /** RDMEM_FRCRAM_S_MEM used bits */
#define AHBSM_TCM_NEW_MEM_S_MEM_BASE                       (0xA2000000UL) /** AHBSM_TCM_NEW_MEM_S_MEM base address */
#define AHBSM_TCM_NEW_MEM_S_MEM_SIZE                       (0x02000000UL) /** AHBSM_TCM_NEW_MEM_S_MEM available address space */
#define AHBSM_TCM_NEW_MEM_S_MEM_END                        (0xA3FFFFFFUL) /** AHBSM_TCM_NEW_MEM_S_MEM end address */
#define AHBSM_TCM_NEW_MEM_S_MEM_BITS                       (0x1AUL)       /** AHBSM_TCM_NEW_MEM_S_MEM used bits */
#define AHBSM_AHBRAM0_MEM_S_MEM_BASE                       (0xA5000000UL) /** AHBSM_AHBRAM0_MEM_S_MEM base address */
#define AHBSM_AHBRAM0_MEM_S_MEM_SIZE                       (0x00010000UL) /** AHBSM_AHBRAM0_MEM_S_MEM available address space */
#define AHBSM_AHBRAM0_MEM_S_MEM_END                        (0xA500FFFFUL) /** AHBSM_AHBRAM0_MEM_S_MEM end address */
#define AHBSM_AHBRAM0_MEM_S_MEM_BITS                       (0x11UL)       /** AHBSM_AHBRAM0_MEM_S_MEM used bits */
#define AHBROM_RAM0_RAM_S_MEM_BASE                         (0xA6800000UL) /** AHBROM_RAM0_RAM_S_MEM base address */
#define AHBROM_RAM0_RAM_S_MEM_SIZE                         (0x00002000UL) /** AHBROM_RAM0_RAM_S_MEM available address space */
#define AHBROM_RAM0_RAM_S_MEM_END                          (0xA6801FFFUL) /** AHBROM_RAM0_RAM_S_MEM end address */
#define AHBROM_RAM0_RAM_S_MEM_BITS                         (0xEUL)        /** AHBROM_RAM0_RAM_S_MEM used bits */
#define RDMEM_SEQRAM_NS_MEM_BASE                           (0xB0000000UL) /** RDMEM_SEQRAM_NS_MEM base address */
#define RDMEM_SEQRAM_NS_MEM_SIZE                           (0x00006000UL) /** RDMEM_SEQRAM_NS_MEM available address space */
#define RDMEM_SEQRAM_NS_MEM_END                            (0xB0005FFFUL) /** RDMEM_SEQRAM_NS_MEM end address */
#define RDMEM_SEQRAM_NS_MEM_BITS                           (0xFUL)        /** RDMEM_SEQRAM_NS_MEM used bits */
#define RDMEM_SEQRAM_SEQRAM_MEM_BASE                       (0xB0000000UL) /** RDMEM_SEQRAM_SEQRAM_MEM base address */
#define RDMEM_SEQRAM_SEQRAM_MEM_SIZE                       (0x00006000UL) /** RDMEM_SEQRAM_SEQRAM_MEM available address space */
#define RDMEM_SEQRAM_SEQRAM_MEM_END                        (0xB0005FFFUL) /** RDMEM_SEQRAM_SEQRAM_MEM end address */
#define RDMEM_SEQRAM_SEQRAM_MEM_BITS                       (0xFUL)        /** RDMEM_SEQRAM_SEQRAM_MEM used bits */
#define RDMEM_FRCRAM_FRCRAM_MEM_BASE                       (0xB0006000UL) /** RDMEM_FRCRAM_FRCRAM_MEM base address */
#define RDMEM_FRCRAM_FRCRAM_MEM_SIZE                       (0x00001000UL) /** RDMEM_FRCRAM_FRCRAM_MEM available address space */
#define RDMEM_FRCRAM_FRCRAM_MEM_END                        (0xB0006FFFUL) /** RDMEM_FRCRAM_FRCRAM_MEM end address */
#define RDMEM_FRCRAM_FRCRAM_MEM_BITS                       (0xDUL)        /** RDMEM_FRCRAM_FRCRAM_MEM used bits */
#define RDMEM_FRCRAM_NS_MEM_BASE                           (0xB0006000UL) /** RDMEM_FRCRAM_NS_MEM base address */
#define RDMEM_FRCRAM_NS_MEM_SIZE                           (0x00001000UL) /** RDMEM_FRCRAM_NS_MEM available address space */
#define RDMEM_FRCRAM_NS_MEM_END                            (0xB0006FFFUL) /** RDMEM_FRCRAM_NS_MEM end address */
#define RDMEM_FRCRAM_NS_MEM_BITS                           (0xDUL)        /** RDMEM_FRCRAM_NS_MEM used bits */
#define AHBSM_TCM_NEW_MEM_MEM_BASE                         (0xB2000000UL) /** AHBSM_TCM_NEW_MEM_MEM base address */
#define AHBSM_TCM_NEW_MEM_MEM_SIZE                         (0x02000000UL) /** AHBSM_TCM_NEW_MEM_MEM available address space */
#define AHBSM_TCM_NEW_MEM_MEM_END                          (0xB3FFFFFFUL) /** AHBSM_TCM_NEW_MEM_MEM end address */
#define AHBSM_TCM_NEW_MEM_MEM_BITS                         (0x1AUL)       /** AHBSM_TCM_NEW_MEM_MEM used bits */
#define AHBSM_TCM_NEW_MEM_NS_MEM_BASE                      (0xB2000000UL) /** AHBSM_TCM_NEW_MEM_NS_MEM base address */
#define AHBSM_TCM_NEW_MEM_NS_MEM_SIZE                      (0x02000000UL) /** AHBSM_TCM_NEW_MEM_NS_MEM available address space */
#define AHBSM_TCM_NEW_MEM_NS_MEM_END                       (0xB3FFFFFFUL) /** AHBSM_TCM_NEW_MEM_NS_MEM end address */
#define AHBSM_TCM_NEW_MEM_NS_MEM_BITS                      (0x1AUL)       /** AHBSM_TCM_NEW_MEM_NS_MEM used bits */
#define AHBSM_AHBRAM0_MEM_MEM_BASE                         (0xB5000000UL) /** AHBSM_AHBRAM0_MEM_MEM base address */
#define AHBSM_AHBRAM0_MEM_MEM_SIZE                         (0x00010000UL) /** AHBSM_AHBRAM0_MEM_MEM available address space */
#define AHBSM_AHBRAM0_MEM_MEM_END                          (0xB500FFFFUL) /** AHBSM_AHBRAM0_MEM_MEM end address */
#define AHBSM_AHBRAM0_MEM_MEM_BITS                         (0x11UL)       /** AHBSM_AHBRAM0_MEM_MEM used bits */
#define AHBSM_AHBRAM0_MEM_NS_MEM_BASE                      (0xB5000000UL) /** AHBSM_AHBRAM0_MEM_NS_MEM base address */
#define AHBSM_AHBRAM0_MEM_NS_MEM_SIZE                      (0x00010000UL) /** AHBSM_AHBRAM0_MEM_NS_MEM available address space */
#define AHBSM_AHBRAM0_MEM_NS_MEM_END                       (0xB500FFFFUL) /** AHBSM_AHBRAM0_MEM_NS_MEM end address */
#define AHBSM_AHBRAM0_MEM_NS_MEM_BITS                      (0x11UL)       /** AHBSM_AHBRAM0_MEM_NS_MEM used bits */
#define AHBROM_RAM0_RAM_MEM_BASE                           (0xB6800000UL) /** AHBROM_RAM0_RAM_MEM base address */
#define AHBROM_RAM0_RAM_MEM_SIZE                           (0x00002000UL) /** AHBROM_RAM0_RAM_MEM available address space */
#define AHBROM_RAM0_RAM_MEM_END                            (0xB6801FFFUL) /** AHBROM_RAM0_RAM_MEM end address */
#define AHBROM_RAM0_RAM_MEM_BITS                           (0xEUL)        /** AHBROM_RAM0_RAM_MEM used bits */
#define AHBROM_RAM0_RAM_NS_MEM_BASE                        (0xB6800000UL) /** AHBROM_RAM0_RAM_NS_MEM base address */
#define AHBROM_RAM0_RAM_NS_MEM_SIZE                        (0x00002000UL) /** AHBROM_RAM0_RAM_NS_MEM available address space */
#define AHBROM_RAM0_RAM_NS_MEM_END                         (0xB6801FFFUL) /** AHBROM_RAM0_RAM_NS_MEM end address */
#define AHBROM_RAM0_RAM_NS_MEM_BITS                        (0xEUL)        /** AHBROM_RAM0_RAM_NS_MEM used bits */

/** Flash and SRAM limits for EFR32FG25B222F1920IM56 */
#define FLASH_BASE                                         (0x08000000UL) /**< Flash Base Address */
#define FLASH_SIZE                                         (0x001E0000UL) /**< Available Flash Memory */
#define FLASH_PAGE_SIZE                                    (0x00002000UL) /**< Flash Memory page size */
#define SRAM_BASE                                          (0x20000000UL) /**< SRAM Base Address */
#define SRAM_SIZE                                          (0x00080000UL) /**< Available SRAM Memory */
#define DMA_CHAN_COUNT                                     LDMA_CH_NUM    /**< Number of DMA channels */
#define EXT_IRQ_COUNT                                      90             /**< Number of External (NVIC) interrupts */

/* GPIO Avalibility Info */
#define GPIO_PA_INDEX                                      0U         /**< Index of port PA */
#define GPIO_PA_COUNT                                      12U        /**< Number of pins on port PA */
#define GPIO_PA_MASK                                       (0x0FFFUL) /**< Port PA pin mask */
#define GPIO_PA_PIN0                                       1U         /**< GPIO pin PA0 is present. */
#define GPIO_PA_PIN1                                       1U         /**< GPIO pin PA1 is present. */
#define GPIO_PA_PIN2                                       1U         /**< GPIO pin PA2 is present. */
#define GPIO_PA_PIN3                                       1U         /**< GPIO pin PA3 is present. */
#define GPIO_PA_PIN4                                       1U         /**< GPIO pin PA4 is present. */
#define GPIO_PA_PIN5                                       1U         /**< GPIO pin PA5 is present. */
#define GPIO_PA_PIN6                                       1U         /**< GPIO pin PA6 is present. */
#define GPIO_PA_PIN7                                       1U         /**< GPIO pin PA7 is present. */
#define GPIO_PA_PIN8                                       1U         /**< GPIO pin PA8 is present. */
#define GPIO_PA_PIN9                                       1U         /**< GPIO pin PA9 is present. */
#define GPIO_PA_PIN10                                      1U         /**< GPIO pin PA10 is present. */
#define GPIO_PA_PIN11                                      1U         /**< GPIO pin PA11 is present. */
#define GPIO_PB_INDEX                                      1U         /**< Index of port PB */
#define GPIO_PB_COUNT                                      6U         /**< Number of pins on port PB */
#define GPIO_PB_MASK                                       (0x003FUL) /**< Port PB pin mask */
#define GPIO_PB_PIN0                                       1U         /**< GPIO pin PB0 is present. */
#define GPIO_PB_PIN1                                       1U         /**< GPIO pin PB1 is present. */
#define GPIO_PB_PIN2                                       1U         /**< GPIO pin PB2 is present. */
#define GPIO_PB_PIN3                                       1U         /**< GPIO pin PB3 is present. */
#define GPIO_PB_PIN4                                       1U         /**< GPIO pin PB4 is present. */
#define GPIO_PB_PIN5                                       1U         /**< GPIO pin PB5 is present. */
#define GPIO_PC_INDEX                                      2U         /**< Index of port PC */
#define GPIO_PC_COUNT                                      10U        /**< Number of pins on port PC */
#define GPIO_PC_MASK                                       (0x03FFUL) /**< Port PC pin mask */
#define GPIO_PC_PIN0                                       1U         /**< GPIO pin PC0 is present. */
#define GPIO_PC_PIN1                                       1U         /**< GPIO pin PC1 is present. */
#define GPIO_PC_PIN2                                       1U         /**< GPIO pin PC2 is present. */
#define GPIO_PC_PIN3                                       1U         /**< GPIO pin PC3 is present. */
#define GPIO_PC_PIN4                                       1U         /**< GPIO pin PC4 is present. */
#define GPIO_PC_PIN5                                       1U         /**< GPIO pin PC5 is present. */
#define GPIO_PC_PIN6                                       1U         /**< GPIO pin PC6 is present. */
#define GPIO_PC_PIN7                                       1U         /**< GPIO pin PC7 is present. */
#define GPIO_PC_PIN8                                       1U         /**< GPIO pin PC8 is present. */
#define GPIO_PC_PIN9                                       1U         /**< GPIO pin PC9 is present. */
#define GPIO_PD_INDEX                                      3U         /**< Index of port PD */
#define GPIO_PD_COUNT                                      8U         /**< Number of pins on port PD */
#define GPIO_PD_MASK                                       (0x00FFUL) /**< Port PD pin mask */
#define GPIO_PD_PIN0                                       1U         /**< GPIO pin PD0 is present. */
#define GPIO_PD_PIN1                                       1U         /**< GPIO pin PD1 is present. */
#define GPIO_PD_PIN2                                       1U         /**< GPIO pin PD2 is present. */
#define GPIO_PD_PIN3                                       1U         /**< GPIO pin PD3 is present. */
#define GPIO_PD_PIN4                                       1U         /**< GPIO pin PD4 is present. */
#define GPIO_PD_PIN5                                       1U         /**< GPIO pin PD5 is present. */
#define GPIO_PD_PIN6                                       1U         /**< GPIO pin PD6 is present. */
#define GPIO_PD_PIN7                                       1U         /**< GPIO pin PD7 is present. */

/* Fixed Resource Locations */
#define ETAMPDET_ETAMPIN0_PORT                             GPIO_PA_INDEX /**< Port of ETAMPIN0.*/
#define ETAMPDET_ETAMPIN0_PIN                              5U            /**< Pin of ETAMPIN0.*/
#define ETAMPDET_ETAMPIN1_PORT                             GPIO_PD_INDEX /**< Port of ETAMPIN1.*/
#define ETAMPDET_ETAMPIN1_PIN                              5U            /**< Pin of ETAMPIN1.*/
#define ETAMPDET_ETAMPOUT0_PORT                            GPIO_PA_INDEX /**< Port of ETAMPOUT0.*/
#define ETAMPDET_ETAMPOUT0_PIN                             6U            /**< Pin of ETAMPOUT0.*/
#define ETAMPDET_ETAMPOUT1_PORT                            GPIO_PD_INDEX /**< Port of ETAMPOUT1.*/
#define ETAMPDET_ETAMPOUT1_PIN                             4U            /**< Pin of ETAMPOUT1.*/
#define GPIO_SWCLK_PORT                                    GPIO_PA_INDEX /**< Port of SWCLK.*/
#define GPIO_SWCLK_PIN                                     1U            /**< Pin of SWCLK.*/
#define GPIO_SWDIO_PORT                                    GPIO_PA_INDEX /**< Port of SWDIO.*/
#define GPIO_SWDIO_PIN                                     2U            /**< Pin of SWDIO.*/
#define GPIO_SWV_PORT                                      GPIO_PA_INDEX /**< Port of SWV.*/
#define GPIO_SWV_PIN                                       3U            /**< Pin of SWV.*/
#define GPIO_TDI_PORT                                      GPIO_PA_INDEX /**< Port of TDI.*/
#define GPIO_TDI_PIN                                       4U            /**< Pin of TDI.*/
#define GPIO_TDO_PORT                                      GPIO_PA_INDEX /**< Port of TDO.*/
#define GPIO_TDO_PIN                                       3U            /**< Pin of TDO.*/
#define GPIO_TRACECLK_PORT                                 GPIO_PA_INDEX /**< Port of TRACECLK.*/
#define GPIO_TRACECLK_PIN                                  4U            /**< Pin of TRACECLK.*/
#define GPIO_TRACEDATA0_PORT                               GPIO_PA_INDEX /**< Port of TRACEDATA0.*/
#define GPIO_TRACEDATA0_PIN                                3U            /**< Pin of TRACEDATA0.*/
#define GPIO_DBG_TRACEDATA1_PORT                           GPIO_PA_INDEX /**< Port of DBG_TRACEDATA1.*/
#define GPIO_DBG_TRACEDATA1_PIN                            5U            /**< Pin of DBG_TRACEDATA1.*/
#define GPIO_DBG_TRACEDATA2_PORT                           GPIO_PA_INDEX /**< Port of DBG_TRACEDATA2.*/
#define GPIO_DBG_TRACEDATA2_PIN                            6U            /**< Pin of DBG_TRACEDATA2.*/
#define GPIO_DBG_TRACEDATA3_PORT                           GPIO_PA_INDEX /**< Port of DBG_TRACEDATA3.*/
#define GPIO_DBG_TRACEDATA3_PIN                            7U            /**< Pin of DBG_TRACEDATA3.*/
#define GPIO_EM4WU0_PORT                                   GPIO_PA_INDEX /**< Port of EM4WU0.*/
#define GPIO_EM4WU0_PIN                                    5U            /**< Pin of EM4WU0.*/
#define GPIO_EM4WU1_PORT                                   GPIO_PA_INDEX /**< Port of EM4WU1.*/
#define GPIO_EM4WU1_PIN                                    11U           /**< Pin of EM4WU1.*/
#define GPIO_EM4WU10_PORT                                  GPIO_PD_INDEX /**< Port of EM4WU10.*/
#define GPIO_EM4WU10_PIN                                   5U            /**< Pin of EM4WU10.*/
#define GPIO_EM4WU3_PORT                                   GPIO_PB_INDEX /**< Port of EM4WU3.*/
#define GPIO_EM4WU3_PIN                                    1U            /**< Pin of EM4WU3.*/
#define GPIO_EM4WU4_PORT                                   GPIO_PB_INDEX /**< Port of EM4WU4.*/
#define GPIO_EM4WU4_PIN                                    3U            /**< Pin of EM4WU4.*/
#define GPIO_EM4WU6_PORT                                   GPIO_PC_INDEX /**< Port of EM4WU6.*/
#define GPIO_EM4WU6_PIN                                    0U            /**< Pin of EM4WU6.*/
#define GPIO_EM4WU7_PORT                                   GPIO_PC_INDEX /**< Port of EM4WU7.*/
#define GPIO_EM4WU7_PIN                                    5U            /**< Pin of EM4WU7.*/
#define GPIO_EM4WU8_PORT                                   GPIO_PC_INDEX /**< Port of EM4WU8.*/
#define GPIO_EM4WU8_PIN                                    7U            /**< Pin of EM4WU8.*/
#define GPIO_EM4WU9_PORT                                   GPIO_PD_INDEX /**< Port of EM4WU9.*/
#define GPIO_EM4WU9_PIN                                    2U            /**< Pin of EM4WU9.*/
#define GPIO_THMSW_EN_PORT                                 GPIO_PC_INDEX /**< Port of THMSW_EN.*/
#define GPIO_THMSW_EN_PIN                                  9U            /**< Pin of THMSW_EN.*/
#define GPIO_THMSW_EN_PRIMARY_PORT                         GPIO_PC_INDEX /**< Port of THMSW_EN_PRIMARY.*/
#define GPIO_THMSW_EN_PRIMARY_PIN                          10U           /**< Pin of THMSW_EN_PRIMARY.*/
#define GPIO_THMSW_HALFSWITCH_PORT                         GPIO_PC_INDEX /**< Port of THMSW_HALFSWITCH.*/
#define GPIO_THMSW_HALFSWITCH_PIN                          9U            /**< Pin of THMSW_HALFSWITCH.*/
#define GPIO_THMSW_HALFSWITCH_PRIMARY_PORT                 GPIO_PC_INDEX /**< Port of THMSW_HALFSWITCH_PRIMARY.*/
#define GPIO_THMSW_HALFSWITCH_PRIMARY_PIN                  10U           /**< Pin of THMSW_HALFSWITCH_PRIMARY.*/
#define LESENSE_EN_0_PORT                                  GPIO_PA_INDEX /**< Port of EN_0.*/
#define LESENSE_EN_0_PIN                                   3U            /**< Pin of EN_0.*/
#define LESENSE_EN_1_PORT                                  GPIO_PA_INDEX /**< Port of EN_1.*/
#define LESENSE_EN_1_PIN                                   4U            /**< Pin of EN_1.*/
#define LESENSE_EN_2_PORT                                  GPIO_PA_INDEX /**< Port of EN_2.*/
#define LESENSE_EN_2_PIN                                   5U            /**< Pin of EN_2.*/
#define LFXO_LFXTAL_I_PORT                                 GPIO_PD_INDEX /**< Port of LFXTAL_I.*/
#define LFXO_LFXTAL_I_PIN                                  1U            /**< Pin of LFXTAL_I.*/
#define LFXO_LFXTAL_O_PORT                                 GPIO_PD_INDEX /**< Port of LFXTAL_O.*/
#define LFXO_LFXTAL_O_PIN                                  0U            /**< Pin of LFXTAL_O.*/
#define LFXO_LF_EXTCLK_PORT                                GPIO_PD_INDEX /**< Port of LF_EXTCLK.*/
#define LFXO_LF_EXTCLK_PIN                                 1U            /**< Pin of LF_EXTCLK.*/
#define USB_USB_DMINUS_PORT                                GPIO_PD_INDEX /**< Port of USB_DMINUS.*/
#define USB_USB_DMINUS_PIN                                 7U            /**< Pin of USB_DMINUS.*/
#define USB_USB_DPLUS_PORT                                 GPIO_PD_INDEX /**< Port of USB_DPLUS.*/
#define USB_USB_DPLUS_PIN                                  6U            /**< Pin of USB_DPLUS.*/
#define VDAC0_VDAC_CH0_MAIN_OUTPUT_PORT                    GPIO_PB_INDEX /**< Port of VDAC_CH0_MAIN_OUTPUT.*/
#define VDAC0_VDAC_CH0_MAIN_OUTPUT_PIN                     0U            /**< Pin of VDAC_CH0_MAIN_OUTPUT.*/
#define VDAC0_VDAC_CH1_MAIN_OUTPUT_PORT                    GPIO_PB_INDEX /**< Port of VDAC_CH1_MAIN_OUTPUT.*/
#define VDAC0_VDAC_CH1_MAIN_OUTPUT_PIN                     1U            /**< Pin of VDAC_CH1_MAIN_OUTPUT.*/

/* Part number capabilities */
#define ACMP_PRESENT                                         /** ACMP is available in this part */
#define ACMP_COUNT                                         2 /** 2 ACMPs available  */
#define AMUXCP_PRESENT                                       /** AMUXCP is available in this part */
#define AMUXCP_COUNT                                       1 /** 1 AMUXCPs available  */
#define BUFC_PRESENT                                         /** BUFC is available in this part */
#define BUFC_COUNT                                         1 /** 1 BUFCs available  */
#define BURAM_PRESENT                                        /** BURAM is available in this part */
#define BURAM_COUNT                                        1 /** 1 BURAMs available  */
#define BURTC_PRESENT                                        /** BURTC is available in this part */
#define BURTC_COUNT                                        1 /** 1 BURTCs available  */
#define CMU_PRESENT                                          /** CMU is available in this part */
#define CMU_COUNT                                          1 /** 1 CMUs available  */
#define DCDC_PRESENT                                         /** DCDC is available in this part */
#define DCDC_COUNT                                         1 /** 1 DCDCs available  */
#define DMEM_PRESENT                                         /** DMEM is available in this part */
#define DMEM_COUNT                                         1 /** 1 DMEMs available  */
#define DPLL_PRESENT                                         /** DPLL is available in this part */
#define DPLL_COUNT                                         1 /** 1 DPLLs available  */
#define EMU_PRESENT                                          /** EMU is available in this part */
#define EMU_COUNT                                          1 /** 1 EMUs available  */
#define ETAMPDET_PRESENT                                     /** ETAMPDET is available in this part */
#define ETAMPDET_COUNT                                     1 /** 1 ETAMPDETs available  */
#define EUSART_PRESENT                                       /** EUSART is available in this part */
#define EUSART_COUNT                                       5 /** 5 EUSARTs available  */
#define FSRCO_PRESENT                                        /** FSRCO is available in this part */
#define FSRCO_COUNT                                        1 /** 1 FSRCOs available  */
#define GPCRC_PRESENT                                        /** GPCRC is available in this part */
#define GPCRC_COUNT                                        1 /** 1 GPCRCs available  */
#define GPIO_PRESENT                                         /** GPIO is available in this part */
#define GPIO_COUNT                                         1 /** 1 GPIOs available  */
#define HFRCO_PRESENT                                        /** HFRCO is available in this part */
#define HFRCO_COUNT                                        1 /** 1 HFRCOs available  */
#define HFRCOEM23_PRESENT                                    /** HFRCOEM23 is available in this part */
#define HFRCOEM23_COUNT                                    1 /** 1 HFRCOEM23s available  */
#define HFXO_PRESENT                                         /** HFXO is available in this part */
#define HFXO_COUNT                                         1 /** 1 HFXOs available  */
#define HOSTMAILBOX_PRESENT                                  /** HOSTMAILBOX is available in this part */
#define HOSTMAILBOX_COUNT                                  1 /** 1 HOSTMAILBOXs available  */
#define I2C_PRESENT                                          /** I2C is available in this part */
#define I2C_COUNT                                          2 /** 2 I2Cs available  */
#define IADC_PRESENT                                         /** IADC is available in this part */
#define IADC_COUNT                                         1 /** 1 IADCs available  */
#define ICACHE_PRESENT                                       /** ICACHE is available in this part */
#define ICACHE_COUNT                                       1 /** 1 ICACHEs available  */
#define LDMA_PRESENT                                         /** LDMA is available in this part */
#define LDMA_COUNT                                         1 /** 1 LDMAs available  */
#define LDMAXBAR_PRESENT                                     /** LDMAXBAR is available in this part */
#define LDMAXBAR_COUNT                                     1 /** 1 LDMAXBARs available  */
#define LESENSE_PRESENT                                      /** LESENSE is available in this part */
#define LESENSE_COUNT                                      1 /** 1 LESENSEs available  */
#define LETIMER_PRESENT                                      /** LETIMER is available in this part */
#define LETIMER_COUNT                                      1 /** 1 LETIMERs available  */
#define LFRCO_PRESENT                                        /** LFRCO is available in this part */
#define LFRCO_COUNT                                        1 /** 1 LFRCOs available  */
#define LFXO_PRESENT                                         /** LFXO is available in this part */
#define LFXO_COUNT                                         1 /** 1 LFXOs available  */
#define MPAHBRAM_PRESENT                                     /** MPAHBRAM is available in this part */
#define MPAHBRAM_COUNT                                     1 /** 1 MPAHBRAMs available  */
#define MSC_PRESENT                                          /** MSC is available in this part */
#define MSC_COUNT                                          1 /** 1 MSCs available  */
#define PCNT_PRESENT                                         /** PCNT is available in this part */
#define PCNT_COUNT                                         1 /** 1 PCNTs available  */
#define PFMXPPRF_PRESENT                                     /** PFMXPPRF is available in this part */
#define PFMXPPRF_COUNT                                     1 /** 1 PFMXPPRFs available  */
#define PRS_PRESENT                                          /** PRS is available in this part */
#define PRS_COUNT                                          1 /** 1 PRSs available  */
#define RADIOAES_PRESENT                                     /** RADIOAES is available in this part */
#define RADIOAES_COUNT                                     1 /** 1 RADIOAESs available  */
#define RFFPLL_PRESENT                                       /** RFFPLL is available in this part */
#define RFFPLL_COUNT                                       1 /** 1 RFFPLLs available  */
#define SCRATCHPAD_PRESENT                                   /** SCRATCHPAD is available in this part */
#define SCRATCHPAD_COUNT                                   1 /** 1 SCRATCHPADs available  */
#define SEMAILBOX_PRESENT                                    /** SEMAILBOX is available in this part */
#define SEMAILBOX_COUNT                                    1 /** 1 SEMAILBOXs available  */
#define SMU_PRESENT                                          /** SMU is available in this part */
#define SMU_COUNT                                          1 /** 1 SMUs available  */
#define SYSCFG_PRESENT                                       /** SYSCFG is available in this part */
#define SYSCFG_COUNT                                       1 /** 1 SYSCFGs available  */
#define SYSRTC_PRESENT                                       /** SYSRTC is available in this part */
#define SYSRTC_COUNT                                       1 /** 1 SYSRTCs available  */
#define TIMER_PRESENT                                        /** TIMER is available in this part */
#define TIMER_COUNT                                        8 /** 8 TIMERs available  */
#define ULFRCO_PRESENT                                       /** ULFRCO is available in this part */
#define ULFRCO_COUNT                                       1 /** 1 ULFRCOs available  */
#define USB_PRESENT                                          /** USB is available in this part */
#define USB_COUNT                                          1 /** 1 USBs available  */
#define USBAHB_PRESENT                                       /** USBAHB is available in this part */
#define USBAHB_COUNT                                       1 /** 1 USBAHBs available  */
#define USBPLL_PRESENT                                       /** USBPLL is available in this part */
#define USBPLL_COUNT                                       1 /** 1 USBPLLs available  */
#define VDAC_PRESENT                                         /** VDAC is available in this part */
#define VDAC_COUNT                                         1 /** 1 VDACs available  */
#define WDOG_PRESENT                                         /** WDOG is available in this part */
#define WDOG_COUNT                                         2 /** 2 WDOGs available  */
#define DEVINFO_PRESENT                                      /** DEVINFO is available in this part */
#define DEVINFO_COUNT                                      1 /** 1 DEVINFOs available  */

/* Include standard ARM headers for the core */
#include "core_cm33.h"        /* Core Header File */
#include "system_efr32fg25.h" /* System Header File */

/** @} End of group EFR32FG25B222F1920IM56_Part */

/**************************************************************************//**
 * @defgroup EFR32FG25B222F1920IM56_Peripheral_TypeDefs EFR32FG25B222F1920IM56 Peripheral TypeDefs
 * @{
 * @brief Device Specific Peripheral Register Structures
 *****************************************************************************/
#include "efr32fg25_scratchpad.h"
#include "efr32fg25_emu.h"
#include "efr32fg25_cmu.h"
#include "efr32fg25_hfrco.h"
#include "efr32fg25_fsrco.h"
#include "efr32fg25_dpll.h"
#include "efr32fg25_lfxo.h"
#include "efr32fg25_lfrco.h"
#include "efr32fg25_ulfrco.h"
#include "efr32fg25_msc.h"
#include "efr32fg25_icache.h"
#include "efr32fg25_prs.h"
#include "efr32fg25_gpio.h"
#include "efr32fg25_ldma.h"
#include "efr32fg25_ldmaxbar.h"
#include "efr32fg25_timer.h"
#include "efr32fg25_burtc.h"
#include "efr32fg25_i2c.h"
#include "efr32fg25_syscfg.h"
#include "efr32fg25_buram.h"
#include "efr32fg25_gpcrc.h"
#include "efr32fg25_dcdc.h"
#include "efr32fg25_mailbox.h"
#include "efr32fg25_eusart.h"
#include "efr32fg25_sysrtc.h"
#include "efr32fg25_mpahbram.h"
#include "efr32fg25_pfmxpprf.h"
#include "efr32fg25_rffpll.h"
#include "efr32fg25_etampdet.h"
#include "efr32fg25_bufc.h"
#include "efr32fg25_aes.h"
#include "efr32fg25_smu.h"
#include "efr32fg25_usbahb.h"
#include "efr32fg25_usb.h"
#include "efr32fg25_pll.h"
#include "efr32fg25_letimer.h"
#include "efr32fg25_iadc.h"
#include "efr32fg25_acmp.h"
#include "efr32fg25_amuxcp.h"
#include "efr32fg25_vdac.h"
#include "efr32fg25_pcnt.h"
#include "efr32fg25_lesense.h"
#include "efr32fg25_hfxo.h"
#include "efr32fg25_wdog.h"
#include "efr32fg25_semailbox.h"
#include "efr32fg25_devinfo.h"

/* Custom headers for LDMAXBAR and PRS mappings */
#include "efr32fg25_prs_signals.h"
#include "efr32fg25_dma_descriptor.h"
#include "efr32fg25_ldmaxbar_defines.h"

/** @} End of group EFR32FG25B222F1920IM56_Peripheral_TypeDefs  */

/**************************************************************************//**
 * @defgroup EFR32FG25B222F1920IM56_Peripheral_Base EFR32FG25B222F1920IM56 Peripheral Memory Map
 * @{
 *****************************************************************************/

#define SCRATCHPAD_S_BASE         (0x40000000UL) /* SCRATCHPAD_S base address */
#define EMU_S_BASE                (0x40004000UL) /* EMU_S base address */
#define CMU_S_BASE                (0x40008000UL) /* CMU_S base address */
#define HFRCO0_S_BASE             (0x40010000UL) /* HFRCO0_S base address */
#define FSRCO_S_BASE              (0x40018000UL) /* FSRCO_S base address */
#define DPLL0_S_BASE              (0x4001C000UL) /* DPLL0_S base address */
#define LFXO_S_BASE               (0x40020000UL) /* LFXO_S base address */
#define LFRCO_S_BASE              (0x40024000UL) /* LFRCO_S base address */
#define ULFRCO_S_BASE             (0x40028000UL) /* ULFRCO_S base address */
#define MSC_S_BASE                (0x40030000UL) /* MSC_S base address */
#define ICACHE0_S_BASE            (0x40034000UL) /* ICACHE0_S base address */
#define PRS_S_BASE                (0x40038000UL) /* PRS_S base address */
#define GPIO_S_BASE               (0x4003C000UL) /* GPIO_S base address */
#define LDMA_S_BASE               (0x40040000UL) /* LDMA_S base address */
#define LDMAXBAR_S_BASE           (0x40044000UL) /* LDMAXBAR_S base address */
#define TIMER0_S_BASE             (0x40048000UL) /* TIMER0_S base address */
#define TIMER1_S_BASE             (0x4004C000UL) /* TIMER1_S base address */
#define TIMER2_S_BASE             (0x40050000UL) /* TIMER2_S base address */
#define TIMER3_S_BASE             (0x40054000UL) /* TIMER3_S base address */
#define TIMER4_S_BASE             (0x40058000UL) /* TIMER4_S base address */
#define TIMER5_S_BASE             (0x4005C000UL) /* TIMER5_S base address */
#define TIMER6_S_BASE             (0x40060000UL) /* TIMER6_S base address */
#define TIMER7_S_BASE             (0x40064000UL) /* TIMER7_S base address */
#define BURTC_S_BASE              (0x4006C000UL) /* BURTC_S base address */
#define I2C1_S_BASE               (0x40070000UL) /* I2C1_S base address */
#define SYSCFG_S_CFGNS_BASE       (0x40078000UL) /* SYSCFG_S_CFGNS base address */
#define SYSCFG_S_BASE             (0x4007C000UL) /* SYSCFG_S base address */
#define BURAM_S_BASE              (0x40080000UL) /* BURAM_S base address */
#define GPCRC_S_BASE              (0x40088000UL) /* GPCRC_S base address */
#define DCDC_S_BASE               (0x40094000UL) /* DCDC_S base address */
#define HOSTMAILBOX_S_BASE        (0x40098000UL) /* HOSTMAILBOX_S base address */
#define EUSART1_S_BASE            (0x400A0000UL) /* EUSART1_S base address */
#define EUSART2_S_BASE            (0x400A4000UL) /* EUSART2_S base address */
#define EUSART3_S_BASE            (0x400A8000UL) /* EUSART3_S base address */
#define EUSART4_S_BASE            (0x400AC000UL) /* EUSART4_S base address */
#define SYSRTC0_S_BASE            (0x400B0000UL) /* SYSRTC0_S base address */
#define DMEM_S_BASE               (0x400C0000UL) /* DMEM_S base address */
#define PFMXPPRF_S_BASE           (0x400C4000UL) /* PFMXPPRF_S base address */
#define RFFPLL0_S_BASE            (0x400C8000UL) /* RFFPLL0_S base address */
#define ETAMPDET_S_BASE           (0x400D0000UL) /* ETAMPDET_S base address */
#define BUFC_S_BASE               (0x42000000UL) /* BUFC_S base address */
#define RADIOAES_S_BASE           (0x44000000UL) /* RADIOAES_S base address */
#define SMU_S_BASE                (0x44008000UL) /* SMU_S base address */
#define SMU_S_CFGNS_BASE          (0x4400C000UL) /* SMU_S_CFGNS base address */
#define USBAHB_S_AHBS_BASE        (0x46000000UL) /* USBAHB_S_AHBS base address */
#define USB_S_APBS_BASE           (0x47000000UL) /* USB_S_APBS base address */
#define USBPLL0_S_BASE            (0x47004000UL) /* USBPLL0_S base address */
#define LETIMER0_S_BASE           (0x49000000UL) /* LETIMER0_S base address */
#define IADC0_S_BASE              (0x49004000UL) /* IADC0_S base address */
#define ACMP0_S_BASE              (0x49008000UL) /* ACMP0_S base address */
#define ACMP1_S_BASE              (0x4900C000UL) /* ACMP1_S base address */
#define AMUXCP0_S_BASE            (0x49020000UL) /* AMUXCP0_S base address */
#define VDAC0_S_BASE              (0x49024000UL) /* VDAC0_S base address */
#define PCNT0_S_BASE              (0x49030000UL) /* PCNT0_S base address */
#define LESENSE_S_BASE            (0x49038000UL) /* LESENSE_S base address */
#define HFRCOEM23_S_BASE          (0x4A000000UL) /* HFRCOEM23_S base address */
#define HFXO0_S_BASE              (0x4A004000UL) /* HFXO0_S base address */
#define I2C0_S_BASE               (0x4B000000UL) /* I2C0_S base address */
#define WDOG0_S_BASE              (0x4B004000UL) /* WDOG0_S base address */
#define WDOG1_S_BASE              (0x4B008000UL) /* WDOG1_S base address */
#define EUSART0_S_BASE            (0x4B010000UL) /* EUSART0_S base address */
#define SEMAILBOX_S_HOST_BASE     (0x4C000000UL) /* SEMAILBOX_S_HOST base address */
#define MPAHBRAM_S_BASE           (0xA6020000UL) /* MPAHBRAM_S base address */
#define SCRATCHPAD_NS_BASE        (0x50000000UL) /* SCRATCHPAD_NS base address */
#define EMU_NS_BASE               (0x50004000UL) /* EMU_NS base address */
#define CMU_NS_BASE               (0x50008000UL) /* CMU_NS base address */
#define HFRCO0_NS_BASE            (0x50010000UL) /* HFRCO0_NS base address */
#define FSRCO_NS_BASE             (0x50018000UL) /* FSRCO_NS base address */
#define DPLL0_NS_BASE             (0x5001C000UL) /* DPLL0_NS base address */
#define LFXO_NS_BASE              (0x50020000UL) /* LFXO_NS base address */
#define LFRCO_NS_BASE             (0x50024000UL) /* LFRCO_NS base address */
#define ULFRCO_NS_BASE            (0x50028000UL) /* ULFRCO_NS base address */
#define MSC_NS_BASE               (0x50030000UL) /* MSC_NS base address */
#define ICACHE0_NS_BASE           (0x50034000UL) /* ICACHE0_NS base address */
#define PRS_NS_BASE               (0x50038000UL) /* PRS_NS base address */
#define GPIO_NS_BASE              (0x5003C000UL) /* GPIO_NS base address */
#define LDMA_NS_BASE              (0x50040000UL) /* LDMA_NS base address */
#define LDMAXBAR_NS_BASE          (0x50044000UL) /* LDMAXBAR_NS base address */
#define TIMER0_NS_BASE            (0x50048000UL) /* TIMER0_NS base address */
#define TIMER1_NS_BASE            (0x5004C000UL) /* TIMER1_NS base address */
#define TIMER2_NS_BASE            (0x50050000UL) /* TIMER2_NS base address */
#define TIMER3_NS_BASE            (0x50054000UL) /* TIMER3_NS base address */
#define TIMER4_NS_BASE            (0x50058000UL) /* TIMER4_NS base address */
#define TIMER5_NS_BASE            (0x5005C000UL) /* TIMER5_NS base address */
#define TIMER6_NS_BASE            (0x50060000UL) /* TIMER6_NS base address */
#define TIMER7_NS_BASE            (0x50064000UL) /* TIMER7_NS base address */
#define BURTC_NS_BASE             (0x5006C000UL) /* BURTC_NS base address */
#define I2C1_NS_BASE              (0x50070000UL) /* I2C1_NS base address */
#define SYSCFG_NS_CFGNS_BASE      (0x50078000UL) /* SYSCFG_NS_CFGNS base address */
#define SYSCFG_NS_BASE            (0x5007C000UL) /* SYSCFG_NS base address */
#define BURAM_NS_BASE             (0x50080000UL) /* BURAM_NS base address */
#define GPCRC_NS_BASE             (0x50088000UL) /* GPCRC_NS base address */
#define DCDC_NS_BASE              (0x50094000UL) /* DCDC_NS base address */
#define HOSTMAILBOX_NS_BASE       (0x50098000UL) /* HOSTMAILBOX_NS base address */
#define EUSART1_NS_BASE           (0x500A0000UL) /* EUSART1_NS base address */
#define EUSART2_NS_BASE           (0x500A4000UL) /* EUSART2_NS base address */
#define EUSART3_NS_BASE           (0x500A8000UL) /* EUSART3_NS base address */
#define EUSART4_NS_BASE           (0x500AC000UL) /* EUSART4_NS base address */
#define SYSRTC0_NS_BASE           (0x500B0000UL) /* SYSRTC0_NS base address */
#define DMEM_NS_BASE              (0x500C0000UL) /* DMEM_NS base address */
#define PFMXPPRF_NS_BASE          (0x500C4000UL) /* PFMXPPRF_NS base address */
#define RFFPLL0_NS_BASE           (0x500C8000UL) /* RFFPLL0_NS base address */
#define ETAMPDET_NS_BASE          (0x500D0000UL) /* ETAMPDET_NS base address */
#define BUFC_NS_BASE              (0x52000000UL) /* BUFC_NS base address */
#define RADIOAES_NS_BASE          (0x54000000UL) /* RADIOAES_NS base address */
#define SMU_NS_BASE               (0x54008000UL) /* SMU_NS base address */
#define SMU_NS_CFGNS_BASE         (0x5400C000UL) /* SMU_NS_CFGNS base address */
#define USBAHB_NS_AHBS_BASE       (0x56000000UL) /* USBAHB_NS_AHBS base address */
#define USB_NS_APBS_BASE          (0x57000000UL) /* USB_NS_APBS base address */
#define USBPLL0_NS_BASE           (0x57004000UL) /* USBPLL0_NS base address */
#define LETIMER0_NS_BASE          (0x59000000UL) /* LETIMER0_NS base address */
#define IADC0_NS_BASE             (0x59004000UL) /* IADC0_NS base address */
#define ACMP0_NS_BASE             (0x59008000UL) /* ACMP0_NS base address */
#define ACMP1_NS_BASE             (0x5900C000UL) /* ACMP1_NS base address */
#define AMUXCP0_NS_BASE           (0x59020000UL) /* AMUXCP0_NS base address */
#define VDAC0_NS_BASE             (0x59024000UL) /* VDAC0_NS base address */
#define PCNT0_NS_BASE             (0x59030000UL) /* PCNT0_NS base address */
#define LESENSE_NS_BASE           (0x59038000UL) /* LESENSE_NS base address */
#define HFRCOEM23_NS_BASE         (0x5A000000UL) /* HFRCOEM23_NS base address */
#define HFXO0_NS_BASE             (0x5A004000UL) /* HFXO0_NS base address */
#define I2C0_NS_BASE              (0x5B000000UL) /* I2C0_NS base address */
#define WDOG0_NS_BASE             (0x5B004000UL) /* WDOG0_NS base address */
#define WDOG1_NS_BASE             (0x5B008000UL) /* WDOG1_NS base address */
#define EUSART0_NS_BASE           (0x5B010000UL) /* EUSART0_NS base address */
#define SEMAILBOX_NS_HOST_BASE    (0x5C000000UL) /* SEMAILBOX_NS_HOST base address */
#define MPAHBRAM_NS_BASE          (0xB6020000UL) /* MPAHBRAM_NS base address */

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"

#endif
#if defined(SL_CATALOG_TRUSTZONE_SECURE_CONFIG_PRESENT)
#include "sl_trustzone_secure_config.h"

#endif

#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_SCRATCHPAD_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_SCRATCHPAD_S) && (SL_TRUSTZONE_PERIPHERAL_SCRATCHPAD_S != 0)))
#define SCRATCHPAD_BASE        (SCRATCHPAD_S_BASE)           /* SCRATCHPAD base address */
#else
#define SCRATCHPAD_BASE        (SCRATCHPAD_NS_BASE)          /* SCRATCHPAD base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_SCRATCHPAD_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_EMU_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_EMU_S) && (SL_TRUSTZONE_PERIPHERAL_EMU_S != 0)))
#define EMU_BASE               (EMU_S_BASE)                  /* EMU base address */
#else
#define EMU_BASE               (EMU_NS_BASE)                 /* EMU base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_EMU_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_CMU_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_CMU_S) && (SL_TRUSTZONE_PERIPHERAL_CMU_S != 0)))
#define CMU_BASE               (CMU_S_BASE)                  /* CMU base address */
#else
#define CMU_BASE               (CMU_NS_BASE)                 /* CMU base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_CMU_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_HFRCO0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_HFRCO0_S) && (SL_TRUSTZONE_PERIPHERAL_HFRCO0_S != 0)))
#define HFRCO0_BASE            (HFRCO0_S_BASE)               /* HFRCO0 base address */
#else
#define HFRCO0_BASE            (HFRCO0_NS_BASE)              /* HFRCO0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_HFRCO0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_FSRCO_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_FSRCO_S) && (SL_TRUSTZONE_PERIPHERAL_FSRCO_S != 0)))
#define FSRCO_BASE             (FSRCO_S_BASE)                /* FSRCO base address */
#else
#define FSRCO_BASE             (FSRCO_NS_BASE)               /* FSRCO base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_FSRCO_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_DPLL0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_DPLL0_S) && (SL_TRUSTZONE_PERIPHERAL_DPLL0_S != 0)))
#define DPLL0_BASE             (DPLL0_S_BASE)                /* DPLL0 base address */
#else
#define DPLL0_BASE             (DPLL0_NS_BASE)               /* DPLL0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_DPLL0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_LFXO_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_LFXO_S) && (SL_TRUSTZONE_PERIPHERAL_LFXO_S != 0)))
#define LFXO_BASE              (LFXO_S_BASE)                 /* LFXO base address */
#else
#define LFXO_BASE              (LFXO_NS_BASE)                /* LFXO base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_LFXO_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_LFRCO_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_LFRCO_S) && (SL_TRUSTZONE_PERIPHERAL_LFRCO_S != 0)))
#define LFRCO_BASE             (LFRCO_S_BASE)                /* LFRCO base address */
#else
#define LFRCO_BASE             (LFRCO_NS_BASE)               /* LFRCO base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_LFRCO_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_ULFRCO_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_ULFRCO_S) && (SL_TRUSTZONE_PERIPHERAL_ULFRCO_S != 0)))
#define ULFRCO_BASE            (ULFRCO_S_BASE)               /* ULFRCO base address */
#else
#define ULFRCO_BASE            (ULFRCO_NS_BASE)              /* ULFRCO base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_ULFRCO_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_MSC_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_MSC_S) && (SL_TRUSTZONE_PERIPHERAL_MSC_S != 0)))
#define MSC_BASE               (MSC_S_BASE)                  /* MSC base address */
#else
#define MSC_BASE               (MSC_NS_BASE)                 /* MSC base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_MSC_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_ICACHE0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_ICACHE0_S) && (SL_TRUSTZONE_PERIPHERAL_ICACHE0_S != 0)))
#define ICACHE0_BASE           (ICACHE0_S_BASE)              /* ICACHE0 base address */
#else
#define ICACHE0_BASE           (ICACHE0_NS_BASE)             /* ICACHE0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_ICACHE0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_PRS_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_PRS_S) && (SL_TRUSTZONE_PERIPHERAL_PRS_S != 0)))
#define PRS_BASE               (PRS_S_BASE)                  /* PRS base address */
#else
#define PRS_BASE               (PRS_NS_BASE)                 /* PRS base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_PRS_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_GPIO_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_GPIO_S) && (SL_TRUSTZONE_PERIPHERAL_GPIO_S != 0)))
#define GPIO_BASE              (GPIO_S_BASE)                 /* GPIO base address */
#else
#define GPIO_BASE              (GPIO_NS_BASE)                /* GPIO base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_GPIO_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_LDMA_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_LDMA_S) && (SL_TRUSTZONE_PERIPHERAL_LDMA_S != 0)))
#define LDMA_BASE              (LDMA_S_BASE)                 /* LDMA base address */
#else
#define LDMA_BASE              (LDMA_NS_BASE)                /* LDMA base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_LDMA_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_LDMAXBAR_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_LDMAXBAR_S) && (SL_TRUSTZONE_PERIPHERAL_LDMAXBAR_S != 0)))
#define LDMAXBAR_BASE          (LDMAXBAR_S_BASE)             /* LDMAXBAR base address */
#else
#define LDMAXBAR_BASE          (LDMAXBAR_NS_BASE)            /* LDMAXBAR base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_LDMAXBAR_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_TIMER0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_TIMER0_S) && (SL_TRUSTZONE_PERIPHERAL_TIMER0_S != 0)))
#define TIMER0_BASE            (TIMER0_S_BASE)               /* TIMER0 base address */
#else
#define TIMER0_BASE            (TIMER0_NS_BASE)              /* TIMER0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_TIMER0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_TIMER1_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_TIMER1_S) && (SL_TRUSTZONE_PERIPHERAL_TIMER1_S != 0)))
#define TIMER1_BASE            (TIMER1_S_BASE)               /* TIMER1 base address */
#else
#define TIMER1_BASE            (TIMER1_NS_BASE)              /* TIMER1 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_TIMER1_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_TIMER2_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_TIMER2_S) && (SL_TRUSTZONE_PERIPHERAL_TIMER2_S != 0)))
#define TIMER2_BASE            (TIMER2_S_BASE)               /* TIMER2 base address */
#else
#define TIMER2_BASE            (TIMER2_NS_BASE)              /* TIMER2 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_TIMER2_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_TIMER3_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_TIMER3_S) && (SL_TRUSTZONE_PERIPHERAL_TIMER3_S != 0)))
#define TIMER3_BASE            (TIMER3_S_BASE)               /* TIMER3 base address */
#else
#define TIMER3_BASE            (TIMER3_NS_BASE)              /* TIMER3 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_TIMER3_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_TIMER4_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_TIMER4_S) && (SL_TRUSTZONE_PERIPHERAL_TIMER4_S != 0)))
#define TIMER4_BASE            (TIMER4_S_BASE)               /* TIMER4 base address */
#else
#define TIMER4_BASE            (TIMER4_NS_BASE)              /* TIMER4 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_TIMER4_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_TIMER5_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_TIMER5_S) && (SL_TRUSTZONE_PERIPHERAL_TIMER5_S != 0)))
#define TIMER5_BASE            (TIMER5_S_BASE)               /* TIMER5 base address */
#else
#define TIMER5_BASE            (TIMER5_NS_BASE)              /* TIMER5 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_TIMER5_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_TIMER6_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_TIMER6_S) && (SL_TRUSTZONE_PERIPHERAL_TIMER6_S != 0)))
#define TIMER6_BASE            (TIMER6_S_BASE)               /* TIMER6 base address */
#else
#define TIMER6_BASE            (TIMER6_NS_BASE)              /* TIMER6 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_TIMER6_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_TIMER7_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_TIMER7_S) && (SL_TRUSTZONE_PERIPHERAL_TIMER7_S != 0)))
#define TIMER7_BASE            (TIMER7_S_BASE)               /* TIMER7 base address */
#else
#define TIMER7_BASE            (TIMER7_NS_BASE)              /* TIMER7 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_TIMER7_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_BURTC_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_BURTC_S) && (SL_TRUSTZONE_PERIPHERAL_BURTC_S != 0)))
#define BURTC_BASE             (BURTC_S_BASE)                /* BURTC base address */
#else
#define BURTC_BASE             (BURTC_NS_BASE)               /* BURTC base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_BURTC_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_I2C1_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_I2C1_S) && (SL_TRUSTZONE_PERIPHERAL_I2C1_S != 0)))
#define I2C1_BASE              (I2C1_S_BASE)                 /* I2C1 base address */
#else
#define I2C1_BASE              (I2C1_NS_BASE)                /* I2C1 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_I2C1_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_SYSCFG_CFGNS_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_SYSCFG_CFGNS_S) && (SL_TRUSTZONE_PERIPHERAL_SYSCFG_CFGNS_S != 0)))
#define SYSCFG_CFGNS_BASE      (SYSCFG_S_CFGNS_BASE)         /* SYSCFG_CFGNS base address */
#else
#define SYSCFG_CFGNS_BASE      (SYSCFG_NS_CFGNS_BASE)        /* SYSCFG_CFGNS base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_SYSCFG_CFGNS_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_SYSCFG_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_SYSCFG_S) && (SL_TRUSTZONE_PERIPHERAL_SYSCFG_S != 0)))
#define SYSCFG_BASE            (SYSCFG_S_BASE)               /* SYSCFG base address */
#else
#define SYSCFG_BASE            (SYSCFG_NS_BASE)              /* SYSCFG base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_SYSCFG_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_BURAM_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_BURAM_S) && (SL_TRUSTZONE_PERIPHERAL_BURAM_S != 0)))
#define BURAM_BASE             (BURAM_S_BASE)                /* BURAM base address */
#else
#define BURAM_BASE             (BURAM_NS_BASE)               /* BURAM base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_BURAM_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_GPCRC_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_GPCRC_S) && (SL_TRUSTZONE_PERIPHERAL_GPCRC_S != 0)))
#define GPCRC_BASE             (GPCRC_S_BASE)                /* GPCRC base address */
#else
#define GPCRC_BASE             (GPCRC_NS_BASE)               /* GPCRC base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_GPCRC_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_DCDC_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_DCDC_S) && (SL_TRUSTZONE_PERIPHERAL_DCDC_S != 0)))
#define DCDC_BASE              (DCDC_S_BASE)                 /* DCDC base address */
#else
#define DCDC_BASE              (DCDC_NS_BASE)                /* DCDC base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_DCDC_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_HOSTMAILBOX_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_HOSTMAILBOX_S) && (SL_TRUSTZONE_PERIPHERAL_HOSTMAILBOX_S != 0)))
#define HOSTMAILBOX_BASE       (HOSTMAILBOX_S_BASE)          /* HOSTMAILBOX base address */
#else
#define HOSTMAILBOX_BASE       (HOSTMAILBOX_NS_BASE)         /* HOSTMAILBOX base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_HOSTMAILBOX_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_EUSART1_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_EUSART1_S) && (SL_TRUSTZONE_PERIPHERAL_EUSART1_S != 0)))
#define EUSART1_BASE           (EUSART1_S_BASE)              /* EUSART1 base address */
#else
#define EUSART1_BASE           (EUSART1_NS_BASE)             /* EUSART1 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_EUSART1_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_EUSART2_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_EUSART2_S) && (SL_TRUSTZONE_PERIPHERAL_EUSART2_S != 0)))
#define EUSART2_BASE           (EUSART2_S_BASE)              /* EUSART2 base address */
#else
#define EUSART2_BASE           (EUSART2_NS_BASE)             /* EUSART2 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_EUSART2_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_EUSART3_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_EUSART3_S) && (SL_TRUSTZONE_PERIPHERAL_EUSART3_S != 0)))
#define EUSART3_BASE           (EUSART3_S_BASE)              /* EUSART3 base address */
#else
#define EUSART3_BASE           (EUSART3_NS_BASE)             /* EUSART3 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_EUSART3_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_EUSART4_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_EUSART4_S) && (SL_TRUSTZONE_PERIPHERAL_EUSART4_S != 0)))
#define EUSART4_BASE           (EUSART4_S_BASE)              /* EUSART4 base address */
#else
#define EUSART4_BASE           (EUSART4_NS_BASE)             /* EUSART4 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_EUSART4_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_SYSRTC0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_SYSRTC0_S) && (SL_TRUSTZONE_PERIPHERAL_SYSRTC0_S != 0)))
#define SYSRTC0_BASE           (SYSRTC0_S_BASE)              /* SYSRTC0 base address */
#else
#define SYSRTC0_BASE           (SYSRTC0_NS_BASE)             /* SYSRTC0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_SYSRTC0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_DMEM_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_DMEM_S) && (SL_TRUSTZONE_PERIPHERAL_DMEM_S != 0)))
#define DMEM_BASE              (DMEM_S_BASE)                 /* DMEM base address */
#else
#define DMEM_BASE              (DMEM_NS_BASE)                /* DMEM base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_DMEM_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_PFMXPPRF_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_PFMXPPRF_S) && (SL_TRUSTZONE_PERIPHERAL_PFMXPPRF_S != 0)))
#define PFMXPPRF_BASE          (PFMXPPRF_S_BASE)             /* PFMXPPRF base address */
#else
#define PFMXPPRF_BASE          (PFMXPPRF_NS_BASE)            /* PFMXPPRF base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_PFMXPPRF_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_RFFPLL0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_RFFPLL0_S) && (SL_TRUSTZONE_PERIPHERAL_RFFPLL0_S != 0)))
#define RFFPLL0_BASE           (RFFPLL0_S_BASE)              /* RFFPLL0 base address */
#else
#define RFFPLL0_BASE           (RFFPLL0_NS_BASE)             /* RFFPLL0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_RFFPLL0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_ETAMPDET_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_ETAMPDET_S) && (SL_TRUSTZONE_PERIPHERAL_ETAMPDET_S != 0)))
#define ETAMPDET_BASE          (ETAMPDET_S_BASE)             /* ETAMPDET base address */
#else
#define ETAMPDET_BASE          (ETAMPDET_NS_BASE)            /* ETAMPDET base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_ETAMPDET_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_BUFC_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_BUFC_S) && (SL_TRUSTZONE_PERIPHERAL_BUFC_S != 0)))
#define BUFC_BASE              (BUFC_S_BASE)                 /* BUFC base address */
#else
#define BUFC_BASE              (BUFC_NS_BASE)                /* BUFC base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_BUFC_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_RADIOAES_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_RADIOAES_S) && (SL_TRUSTZONE_PERIPHERAL_RADIOAES_S != 0)))
#define RADIOAES_BASE          (RADIOAES_S_BASE)             /* RADIOAES base address */
#else
#define RADIOAES_BASE          (RADIOAES_NS_BASE)            /* RADIOAES base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_RADIOAES_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_SMU_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_SMU_S) && (SL_TRUSTZONE_PERIPHERAL_SMU_S != 0)))
#define SMU_BASE               (SMU_S_BASE)                  /* SMU base address */
#else
#define SMU_BASE               (SMU_S_BASE)                  /* SMU base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_SMU_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_SMU_CFGNS_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_SMU_CFGNS_S) && (SL_TRUSTZONE_PERIPHERAL_SMU_CFGNS_S != 0)))
#define SMU_CFGNS_BASE         (SMU_S_CFGNS_BASE)            /* SMU_CFGNS base address */
#else
#define SMU_CFGNS_BASE         (SMU_NS_CFGNS_BASE)           /* SMU_CFGNS base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_SMU_CFGNS_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_USBAHB_AHBS_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_USBAHB_AHBS_S) && (SL_TRUSTZONE_PERIPHERAL_USBAHB_AHBS_S != 0)))
#define USBAHB_AHBS_BASE       (USBAHB_S_AHBS_BASE)          /* USBAHB_AHBS base address */
#else
#define USBAHB_AHBS_BASE       (USBAHB_NS_AHBS_BASE)         /* USBAHB_AHBS base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_USBAHB_AHBS_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_USB_APBS_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_USB_APBS_S) && (SL_TRUSTZONE_PERIPHERAL_USB_APBS_S != 0)))
#define USB_APBS_BASE          (USB_S_APBS_BASE)             /* USB_APBS base address */
#else
#define USB_APBS_BASE          (USB_NS_APBS_BASE)            /* USB_APBS base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_USB_APBS_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_USBPLL0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_USBPLL0_S) && (SL_TRUSTZONE_PERIPHERAL_USBPLL0_S != 0)))
#define USBPLL0_BASE           (USBPLL0_S_BASE)              /* USBPLL0 base address */
#else
#define USBPLL0_BASE           (USBPLL0_NS_BASE)             /* USBPLL0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_USBPLL0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_LETIMER0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_LETIMER0_S) && (SL_TRUSTZONE_PERIPHERAL_LETIMER0_S != 0)))
#define LETIMER0_BASE          (LETIMER0_S_BASE)             /* LETIMER0 base address */
#else
#define LETIMER0_BASE          (LETIMER0_NS_BASE)            /* LETIMER0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_LETIMER0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_IADC0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_IADC0_S) && (SL_TRUSTZONE_PERIPHERAL_IADC0_S != 0)))
#define IADC0_BASE             (IADC0_S_BASE)                /* IADC0 base address */
#else
#define IADC0_BASE             (IADC0_NS_BASE)               /* IADC0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_IADC0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_ACMP0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_ACMP0_S) && (SL_TRUSTZONE_PERIPHERAL_ACMP0_S != 0)))
#define ACMP0_BASE             (ACMP0_S_BASE)                /* ACMP0 base address */
#else
#define ACMP0_BASE             (ACMP0_NS_BASE)               /* ACMP0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_ACMP0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_ACMP1_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_ACMP1_S) && (SL_TRUSTZONE_PERIPHERAL_ACMP1_S != 0)))
#define ACMP1_BASE             (ACMP1_S_BASE)                /* ACMP1 base address */
#else
#define ACMP1_BASE             (ACMP1_NS_BASE)               /* ACMP1 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_ACMP1_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_AMUXCP0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_AMUXCP0_S) && (SL_TRUSTZONE_PERIPHERAL_AMUXCP0_S != 0)))
#define AMUXCP0_BASE           (AMUXCP0_S_BASE)              /* AMUXCP0 base address */
#else
#define AMUXCP0_BASE           (AMUXCP0_NS_BASE)             /* AMUXCP0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_AMUXCP0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_VDAC0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_VDAC0_S) && (SL_TRUSTZONE_PERIPHERAL_VDAC0_S != 0)))
#define VDAC0_BASE             (VDAC0_S_BASE)                /* VDAC0 base address */
#else
#define VDAC0_BASE             (VDAC0_NS_BASE)               /* VDAC0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_VDAC0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_PCNT0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_PCNT0_S) && (SL_TRUSTZONE_PERIPHERAL_PCNT0_S != 0)))
#define PCNT0_BASE             (PCNT0_S_BASE)                /* PCNT0 base address */
#else
#define PCNT0_BASE             (PCNT0_NS_BASE)               /* PCNT0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_PCNT0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_LESENSE_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_LESENSE_S) && (SL_TRUSTZONE_PERIPHERAL_LESENSE_S != 0)))
#define LESENSE_BASE           (LESENSE_S_BASE)              /* LESENSE base address */
#else
#define LESENSE_BASE           (LESENSE_NS_BASE)             /* LESENSE base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_LESENSE_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_HFRCOEM23_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_HFRCOEM23_S) && (SL_TRUSTZONE_PERIPHERAL_HFRCOEM23_S != 0)))
#define HFRCOEM23_BASE         (HFRCOEM23_S_BASE)            /* HFRCOEM23 base address */
#else
#define HFRCOEM23_BASE         (HFRCOEM23_NS_BASE)           /* HFRCOEM23 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_HFRCOEM23_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_HFXO0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_HFXO0_S) && (SL_TRUSTZONE_PERIPHERAL_HFXO0_S != 0)))
#define HFXO0_BASE             (HFXO0_S_BASE)                /* HFXO0 base address */
#else
#define HFXO0_BASE             (HFXO0_NS_BASE)               /* HFXO0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_HFXO0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_I2C0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_I2C0_S) && (SL_TRUSTZONE_PERIPHERAL_I2C0_S != 0)))
#define I2C0_BASE              (I2C0_S_BASE)                 /* I2C0 base address */
#else
#define I2C0_BASE              (I2C0_NS_BASE)                /* I2C0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_I2C0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_WDOG0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_WDOG0_S) && (SL_TRUSTZONE_PERIPHERAL_WDOG0_S != 0)))
#define WDOG0_BASE             (WDOG0_S_BASE)                /* WDOG0 base address */
#else
#define WDOG0_BASE             (WDOG0_NS_BASE)               /* WDOG0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_WDOG0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_WDOG1_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_WDOG1_S) && (SL_TRUSTZONE_PERIPHERAL_WDOG1_S != 0)))
#define WDOG1_BASE             (WDOG1_S_BASE)                /* WDOG1 base address */
#else
#define WDOG1_BASE             (WDOG1_NS_BASE)               /* WDOG1 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_WDOG1_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_EUSART0_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_EUSART0_S) && (SL_TRUSTZONE_PERIPHERAL_EUSART0_S != 0)))
#define EUSART0_BASE           (EUSART0_S_BASE)              /* EUSART0 base address */
#else
#define EUSART0_BASE           (EUSART0_NS_BASE)             /* EUSART0 base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_EUSART0_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_SEMAILBOX_HOST_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_SEMAILBOX_HOST_S) && (SL_TRUSTZONE_PERIPHERAL_SEMAILBOX_HOST_S != 0)))
#define SEMAILBOX_HOST_BASE    (SEMAILBOX_S_HOST_BASE)       /* SEMAILBOX_HOST base address */
#else
#define SEMAILBOX_HOST_BASE    (SEMAILBOX_S_HOST_BASE)       /* SEMAILBOX_HOST base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_SEMAILBOX_HOST_S */
#if ((defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_PERIPHERAL_MPAHBRAM_S)) || (defined(SL_TRUSTZONE_PERIPHERAL_MPAHBRAM_S) && (SL_TRUSTZONE_PERIPHERAL_MPAHBRAM_S != 0)))
#define MPAHBRAM_BASE          (MPAHBRAM_S_BASE)             /* MPAHBRAM base address */
#else
#define MPAHBRAM_BASE          (MPAHBRAM_NS_BASE)            /* MPAHBRAM base address */
#endif /* SL_TRUSTZONE_PERIPHERAL_MPAHBRAM_S */

#define DEVINFO_BASE           (0x0FE08000UL) /* DEVINFO base address */
/** @} End of group EFR32FG25B222F1920IM56_Peripheral_Base */

/**************************************************************************//**
 * @defgroup EFR32FG25B222F1920IM56_Peripheral_Declaration EFR32FG25B222F1920IM56 Peripheral Declarations Map
 * @{
 *****************************************************************************/

#define SCRATCHPAD_S         ((SCRATCHPAD_TypeDef *) SCRATCHPAD_S_BASE)          /**< SCRATCHPAD_S base pointer */
#define EMU_S                ((EMU_TypeDef *) EMU_S_BASE)                        /**< EMU_S base pointer */
#define CMU_S                ((CMU_TypeDef *) CMU_S_BASE)                        /**< CMU_S base pointer */
#define HFRCO0_S             ((HFRCO_TypeDef *) HFRCO0_S_BASE)                   /**< HFRCO0_S base pointer */
#define FSRCO_S              ((FSRCO_TypeDef *) FSRCO_S_BASE)                    /**< FSRCO_S base pointer */
#define DPLL0_S              ((DPLL_TypeDef *) DPLL0_S_BASE)                     /**< DPLL0_S base pointer */
#define LFXO_S               ((LFXO_TypeDef *) LFXO_S_BASE)                      /**< LFXO_S base pointer */
#define LFRCO_S              ((LFRCO_TypeDef *) LFRCO_S_BASE)                    /**< LFRCO_S base pointer */
#define ULFRCO_S             ((ULFRCO_TypeDef *) ULFRCO_S_BASE)                  /**< ULFRCO_S base pointer */
#define MSC_S                ((MSC_TypeDef *) MSC_S_BASE)                        /**< MSC_S base pointer */
#define ICACHE0_S            ((ICACHE_TypeDef *) ICACHE0_S_BASE)                 /**< ICACHE0_S base pointer */
#define PRS_S                ((PRS_TypeDef *) PRS_S_BASE)                        /**< PRS_S base pointer */
#define GPIO_S               ((GPIO_TypeDef *) GPIO_S_BASE)                      /**< GPIO_S base pointer */
#define LDMA_S               ((LDMA_TypeDef *) LDMA_S_BASE)                      /**< LDMA_S base pointer */
#define LDMAXBAR_S           ((LDMAXBAR_TypeDef *) LDMAXBAR_S_BASE)              /**< LDMAXBAR_S base pointer */
#define TIMER0_S             ((TIMER_TypeDef *) TIMER0_S_BASE)                   /**< TIMER0_S base pointer */
#define TIMER1_S             ((TIMER_TypeDef *) TIMER1_S_BASE)                   /**< TIMER1_S base pointer */
#define TIMER2_S             ((TIMER_TypeDef *) TIMER2_S_BASE)                   /**< TIMER2_S base pointer */
#define TIMER3_S             ((TIMER_TypeDef *) TIMER3_S_BASE)                   /**< TIMER3_S base pointer */
#define TIMER4_S             ((TIMER_TypeDef *) TIMER4_S_BASE)                   /**< TIMER4_S base pointer */
#define TIMER5_S             ((TIMER_TypeDef *) TIMER5_S_BASE)                   /**< TIMER5_S base pointer */
#define TIMER6_S             ((TIMER_TypeDef *) TIMER6_S_BASE)                   /**< TIMER6_S base pointer */
#define TIMER7_S             ((TIMER_TypeDef *) TIMER7_S_BASE)                   /**< TIMER7_S base pointer */
#define BURTC_S              ((BURTC_TypeDef *) BURTC_S_BASE)                    /**< BURTC_S base pointer */
#define I2C1_S               ((I2C_TypeDef *) I2C1_S_BASE)                       /**< I2C1_S base pointer */
#define SYSCFG_S_CFGNS       ((SYSCFG_CFGNS_TypeDef *) SYSCFG_S_CFGNS_BASE)      /**< SYSCFG_S_CFGNS base pointer */
#define SYSCFG_S             ((SYSCFG_TypeDef *) SYSCFG_S_BASE)                  /**< SYSCFG_S base pointer */
#define BURAM_S              ((BURAM_TypeDef *) BURAM_S_BASE)                    /**< BURAM_S base pointer */
#define GPCRC_S              ((GPCRC_TypeDef *) GPCRC_S_BASE)                    /**< GPCRC_S base pointer */
#define DCDC_S               ((DCDC_TypeDef *) DCDC_S_BASE)                      /**< DCDC_S base pointer */
#define HOSTMAILBOX_S        ((MAILBOX_TypeDef *) HOSTMAILBOX_S_BASE)            /**< HOSTMAILBOX_S base pointer */
#define EUSART1_S            ((EUSART_TypeDef *) EUSART1_S_BASE)                 /**< EUSART1_S base pointer */
#define EUSART2_S            ((EUSART_TypeDef *) EUSART2_S_BASE)                 /**< EUSART2_S base pointer */
#define EUSART3_S            ((EUSART_TypeDef *) EUSART3_S_BASE)                 /**< EUSART3_S base pointer */
#define EUSART4_S            ((EUSART_TypeDef *) EUSART4_S_BASE)                 /**< EUSART4_S base pointer */
#define SYSRTC0_S            ((SYSRTC_TypeDef *) SYSRTC0_S_BASE)                 /**< SYSRTC0_S base pointer */
#define DMEM_S               ((MPAHBRAM_TypeDef *) DMEM_S_BASE)                  /**< DMEM_S base pointer */
#define PFMXPPRF_S           ((PFMXPPRF_TypeDef *) PFMXPPRF_S_BASE)              /**< PFMXPPRF_S base pointer */
#define RFFPLL0_S            ((RFFPLL_TypeDef *) RFFPLL0_S_BASE)                 /**< RFFPLL0_S base pointer */
#define ETAMPDET_S           ((ETAMPDET_TypeDef *) ETAMPDET_S_BASE)              /**< ETAMPDET_S base pointer */
#define BUFC_S               ((BUFC_TypeDef *) BUFC_S_BASE)                      /**< BUFC_S base pointer */
#define RADIOAES_S           ((AES_TypeDef *) RADIOAES_S_BASE)                   /**< RADIOAES_S base pointer */
#define SMU_S                ((SMU_TypeDef *) SMU_S_BASE)                        /**< SMU_S base pointer */
#define SMU_S_CFGNS          ((SMU_CFGNS_TypeDef *) SMU_S_CFGNS_BASE)            /**< SMU_S_CFGNS base pointer */
#define USBAHB_S_AHBS        ((USBAHB_AHBS_TypeDef *) USBAHB_S_AHBS_BASE)        /**< USBAHB_S_AHBS base pointer */
#define USB_S_APBS           ((USB_APBS_TypeDef *) USB_S_APBS_BASE)              /**< USB_S_APBS base pointer */
#define USBPLL0_S            ((PLL_TypeDef *) USBPLL0_S_BASE)                    /**< USBPLL0_S base pointer */
#define LETIMER0_S           ((LETIMER_TypeDef *) LETIMER0_S_BASE)               /**< LETIMER0_S base pointer */
#define IADC0_S              ((IADC_TypeDef *) IADC0_S_BASE)                     /**< IADC0_S base pointer */
#define ACMP0_S              ((ACMP_TypeDef *) ACMP0_S_BASE)                     /**< ACMP0_S base pointer */
#define ACMP1_S              ((ACMP_TypeDef *) ACMP1_S_BASE)                     /**< ACMP1_S base pointer */
#define AMUXCP0_S            ((AMUXCP_TypeDef *) AMUXCP0_S_BASE)                 /**< AMUXCP0_S base pointer */
#define VDAC0_S              ((VDAC_TypeDef *) VDAC0_S_BASE)                     /**< VDAC0_S base pointer */
#define PCNT0_S              ((PCNT_TypeDef *) PCNT0_S_BASE)                     /**< PCNT0_S base pointer */
#define LESENSE_S            ((LESENSE_TypeDef *) LESENSE_S_BASE)                /**< LESENSE_S base pointer */
#define HFRCOEM23_S          ((HFRCO_TypeDef *) HFRCOEM23_S_BASE)                /**< HFRCOEM23_S base pointer */
#define HFXO0_S              ((HFXO_TypeDef *) HFXO0_S_BASE)                     /**< HFXO0_S base pointer */
#define I2C0_S               ((I2C_TypeDef *) I2C0_S_BASE)                       /**< I2C0_S base pointer */
#define WDOG0_S              ((WDOG_TypeDef *) WDOG0_S_BASE)                     /**< WDOG0_S base pointer */
#define WDOG1_S              ((WDOG_TypeDef *) WDOG1_S_BASE)                     /**< WDOG1_S base pointer */
#define EUSART0_S            ((EUSART_TypeDef *) EUSART0_S_BASE)                 /**< EUSART0_S base pointer */
#define SEMAILBOX_S_HOST     ((SEMAILBOX_HOST_TypeDef *) SEMAILBOX_S_HOST_BASE)  /**< SEMAILBOX_S_HOST base pointer */
#define MPAHBRAM_S           ((MPAHBRAM_TypeDef *) MPAHBRAM_S_BASE)              /**< MPAHBRAM_S base pointer */
#define SCRATCHPAD_NS        ((SCRATCHPAD_TypeDef *) SCRATCHPAD_NS_BASE)         /**< SCRATCHPAD_NS base pointer */
#define EMU_NS               ((EMU_TypeDef *) EMU_NS_BASE)                       /**< EMU_NS base pointer */
#define CMU_NS               ((CMU_TypeDef *) CMU_NS_BASE)                       /**< CMU_NS base pointer */
#define HFRCO0_NS            ((HFRCO_TypeDef *) HFRCO0_NS_BASE)                  /**< HFRCO0_NS base pointer */
#define FSRCO_NS             ((FSRCO_TypeDef *) FSRCO_NS_BASE)                   /**< FSRCO_NS base pointer */
#define DPLL0_NS             ((DPLL_TypeDef *) DPLL0_NS_BASE)                    /**< DPLL0_NS base pointer */
#define LFXO_NS              ((LFXO_TypeDef *) LFXO_NS_BASE)                     /**< LFXO_NS base pointer */
#define LFRCO_NS             ((LFRCO_TypeDef *) LFRCO_NS_BASE)                   /**< LFRCO_NS base pointer */
#define ULFRCO_NS            ((ULFRCO_TypeDef *) ULFRCO_NS_BASE)                 /**< ULFRCO_NS base pointer */
#define MSC_NS               ((MSC_TypeDef *) MSC_NS_BASE)                       /**< MSC_NS base pointer */
#define ICACHE0_NS           ((ICACHE_TypeDef *) ICACHE0_NS_BASE)                /**< ICACHE0_NS base pointer */
#define PRS_NS               ((PRS_TypeDef *) PRS_NS_BASE)                       /**< PRS_NS base pointer */
#define GPIO_NS              ((GPIO_TypeDef *) GPIO_NS_BASE)                     /**< GPIO_NS base pointer */
#define LDMA_NS              ((LDMA_TypeDef *) LDMA_NS_BASE)                     /**< LDMA_NS base pointer */
#define LDMAXBAR_NS          ((LDMAXBAR_TypeDef *) LDMAXBAR_NS_BASE)             /**< LDMAXBAR_NS base pointer */
#define TIMER0_NS            ((TIMER_TypeDef *) TIMER0_NS_BASE)                  /**< TIMER0_NS base pointer */
#define TIMER1_NS            ((TIMER_TypeDef *) TIMER1_NS_BASE)                  /**< TIMER1_NS base pointer */
#define TIMER2_NS            ((TIMER_TypeDef *) TIMER2_NS_BASE)                  /**< TIMER2_NS base pointer */
#define TIMER3_NS            ((TIMER_TypeDef *) TIMER3_NS_BASE)                  /**< TIMER3_NS base pointer */
#define TIMER4_NS            ((TIMER_TypeDef *) TIMER4_NS_BASE)                  /**< TIMER4_NS base pointer */
#define TIMER5_NS            ((TIMER_TypeDef *) TIMER5_NS_BASE)                  /**< TIMER5_NS base pointer */
#define TIMER6_NS            ((TIMER_TypeDef *) TIMER6_NS_BASE)                  /**< TIMER6_NS base pointer */
#define TIMER7_NS            ((TIMER_TypeDef *) TIMER7_NS_BASE)                  /**< TIMER7_NS base pointer */
#define BURTC_NS             ((BURTC_TypeDef *) BURTC_NS_BASE)                   /**< BURTC_NS base pointer */
#define I2C1_NS              ((I2C_TypeDef *) I2C1_NS_BASE)                      /**< I2C1_NS base pointer */
#define SYSCFG_NS_CFGNS      ((SYSCFG_CFGNS_TypeDef *) SYSCFG_NS_CFGNS_BASE)     /**< SYSCFG_NS_CFGNS base pointer */
#define SYSCFG_NS            ((SYSCFG_TypeDef *) SYSCFG_NS_BASE)                 /**< SYSCFG_NS base pointer */
#define BURAM_NS             ((BURAM_TypeDef *) BURAM_NS_BASE)                   /**< BURAM_NS base pointer */
#define GPCRC_NS             ((GPCRC_TypeDef *) GPCRC_NS_BASE)                   /**< GPCRC_NS base pointer */
#define DCDC_NS              ((DCDC_TypeDef *) DCDC_NS_BASE)                     /**< DCDC_NS base pointer */
#define HOSTMAILBOX_NS       ((MAILBOX_TypeDef *) HOSTMAILBOX_NS_BASE)           /**< HOSTMAILBOX_NS base pointer */
#define EUSART1_NS           ((EUSART_TypeDef *) EUSART1_NS_BASE)                /**< EUSART1_NS base pointer */
#define EUSART2_NS           ((EUSART_TypeDef *) EUSART2_NS_BASE)                /**< EUSART2_NS base pointer */
#define EUSART3_NS           ((EUSART_TypeDef *) EUSART3_NS_BASE)                /**< EUSART3_NS base pointer */
#define EUSART4_NS           ((EUSART_TypeDef *) EUSART4_NS_BASE)                /**< EUSART4_NS base pointer */
#define SYSRTC0_NS           ((SYSRTC_TypeDef *) SYSRTC0_NS_BASE)                /**< SYSRTC0_NS base pointer */
#define DMEM_NS              ((MPAHBRAM_TypeDef *) DMEM_NS_BASE)                 /**< DMEM_NS base pointer */
#define PFMXPPRF_NS          ((PFMXPPRF_TypeDef *) PFMXPPRF_NS_BASE)             /**< PFMXPPRF_NS base pointer */
#define RFFPLL0_NS           ((RFFPLL_TypeDef *) RFFPLL0_NS_BASE)                /**< RFFPLL0_NS base pointer */
#define ETAMPDET_NS          ((ETAMPDET_TypeDef *) ETAMPDET_NS_BASE)             /**< ETAMPDET_NS base pointer */
#define BUFC_NS              ((BUFC_TypeDef *) BUFC_NS_BASE)                     /**< BUFC_NS base pointer */
#define RADIOAES_NS          ((AES_TypeDef *) RADIOAES_NS_BASE)                  /**< RADIOAES_NS base pointer */
#define SMU_NS               ((SMU_TypeDef *) SMU_NS_BASE)                       /**< SMU_NS base pointer */
#define SMU_NS_CFGNS         ((SMU_CFGNS_TypeDef *) SMU_NS_CFGNS_BASE)           /**< SMU_NS_CFGNS base pointer */
#define USBAHB_NS_AHBS       ((USBAHB_AHBS_TypeDef *) USBAHB_NS_AHBS_BASE)       /**< USBAHB_NS_AHBS base pointer */
#define USB_NS_APBS          ((USB_APBS_TypeDef *) USB_NS_APBS_BASE)             /**< USB_NS_APBS base pointer */
#define USBPLL0_NS           ((PLL_TypeDef *) USBPLL0_NS_BASE)                   /**< USBPLL0_NS base pointer */
#define LETIMER0_NS          ((LETIMER_TypeDef *) LETIMER0_NS_BASE)              /**< LETIMER0_NS base pointer */
#define IADC0_NS             ((IADC_TypeDef *) IADC0_NS_BASE)                    /**< IADC0_NS base pointer */
#define ACMP0_NS             ((ACMP_TypeDef *) ACMP0_NS_BASE)                    /**< ACMP0_NS base pointer */
#define ACMP1_NS             ((ACMP_TypeDef *) ACMP1_NS_BASE)                    /**< ACMP1_NS base pointer */
#define AMUXCP0_NS           ((AMUXCP_TypeDef *) AMUXCP0_NS_BASE)                /**< AMUXCP0_NS base pointer */
#define VDAC0_NS             ((VDAC_TypeDef *) VDAC0_NS_BASE)                    /**< VDAC0_NS base pointer */
#define PCNT0_NS             ((PCNT_TypeDef *) PCNT0_NS_BASE)                    /**< PCNT0_NS base pointer */
#define LESENSE_NS           ((LESENSE_TypeDef *) LESENSE_NS_BASE)               /**< LESENSE_NS base pointer */
#define HFRCOEM23_NS         ((HFRCO_TypeDef *) HFRCOEM23_NS_BASE)               /**< HFRCOEM23_NS base pointer */
#define HFXO0_NS             ((HFXO_TypeDef *) HFXO0_NS_BASE)                    /**< HFXO0_NS base pointer */
#define I2C0_NS              ((I2C_TypeDef *) I2C0_NS_BASE)                      /**< I2C0_NS base pointer */
#define WDOG0_NS             ((WDOG_TypeDef *) WDOG0_NS_BASE)                    /**< WDOG0_NS base pointer */
#define WDOG1_NS             ((WDOG_TypeDef *) WDOG1_NS_BASE)                    /**< WDOG1_NS base pointer */
#define EUSART0_NS           ((EUSART_TypeDef *) EUSART0_NS_BASE)                /**< EUSART0_NS base pointer */
#define SEMAILBOX_NS_HOST    ((SEMAILBOX_HOST_TypeDef *) SEMAILBOX_NS_HOST_BASE) /**< SEMAILBOX_NS_HOST base pointer */
#define MPAHBRAM_NS          ((MPAHBRAM_TypeDef *) MPAHBRAM_NS_BASE)             /**< MPAHBRAM_NS base pointer */
#define SCRATCHPAD           ((SCRATCHPAD_TypeDef *) SCRATCHPAD_BASE)            /**< SCRATCHPAD base pointer */
#define EMU                  ((EMU_TypeDef *) EMU_BASE)                          /**< EMU base pointer */
#define CMU                  ((CMU_TypeDef *) CMU_BASE)                          /**< CMU base pointer */
#define HFRCO0               ((HFRCO_TypeDef *) HFRCO0_BASE)                     /**< HFRCO0 base pointer */
#define FSRCO                ((FSRCO_TypeDef *) FSRCO_BASE)                      /**< FSRCO base pointer */
#define DPLL0                ((DPLL_TypeDef *) DPLL0_BASE)                       /**< DPLL0 base pointer */
#define LFXO                 ((LFXO_TypeDef *) LFXO_BASE)                        /**< LFXO base pointer */
#define LFRCO                ((LFRCO_TypeDef *) LFRCO_BASE)                      /**< LFRCO base pointer */
#define ULFRCO               ((ULFRCO_TypeDef *) ULFRCO_BASE)                    /**< ULFRCO base pointer */
#define MSC                  ((MSC_TypeDef *) MSC_BASE)                          /**< MSC base pointer */
#define ICACHE0              ((ICACHE_TypeDef *) ICACHE0_BASE)                   /**< ICACHE0 base pointer */
#define PRS                  ((PRS_TypeDef *) PRS_BASE)                          /**< PRS base pointer */
#define GPIO                 ((GPIO_TypeDef *) GPIO_BASE)                        /**< GPIO base pointer */
#define LDMA                 ((LDMA_TypeDef *) LDMA_BASE)                        /**< LDMA base pointer */
#define LDMAXBAR             ((LDMAXBAR_TypeDef *) LDMAXBAR_BASE)                /**< LDMAXBAR base pointer */
#define TIMER0               ((TIMER_TypeDef *) TIMER0_BASE)                     /**< TIMER0 base pointer */
#define TIMER1               ((TIMER_TypeDef *) TIMER1_BASE)                     /**< TIMER1 base pointer */
#define TIMER2               ((TIMER_TypeDef *) TIMER2_BASE)                     /**< TIMER2 base pointer */
#define TIMER3               ((TIMER_TypeDef *) TIMER3_BASE)                     /**< TIMER3 base pointer */
#define TIMER4               ((TIMER_TypeDef *) TIMER4_BASE)                     /**< TIMER4 base pointer */
#define TIMER5               ((TIMER_TypeDef *) TIMER5_BASE)                     /**< TIMER5 base pointer */
#define TIMER6               ((TIMER_TypeDef *) TIMER6_BASE)                     /**< TIMER6 base pointer */
#define TIMER7               ((TIMER_TypeDef *) TIMER7_BASE)                     /**< TIMER7 base pointer */
#define BURTC                ((BURTC_TypeDef *) BURTC_BASE)                      /**< BURTC base pointer */
#define I2C1                 ((I2C_TypeDef *) I2C1_BASE)                         /**< I2C1 base pointer */
#define SYSCFG_CFGNS         ((SYSCFG_CFGNS_TypeDef *) SYSCFG_CFGNS_BASE)        /**< SYSCFG_CFGNS base pointer */
#define SYSCFG               ((SYSCFG_TypeDef *) SYSCFG_BASE)                    /**< SYSCFG base pointer */
#define BURAM                ((BURAM_TypeDef *) BURAM_BASE)                      /**< BURAM base pointer */
#define GPCRC                ((GPCRC_TypeDef *) GPCRC_BASE)                      /**< GPCRC base pointer */
#define DCDC                 ((DCDC_TypeDef *) DCDC_BASE)                        /**< DCDC base pointer */
#define HOSTMAILBOX          ((MAILBOX_TypeDef *) HOSTMAILBOX_BASE)              /**< HOSTMAILBOX base pointer */
#define EUSART1              ((EUSART_TypeDef *) EUSART1_BASE)                   /**< EUSART1 base pointer */
#define EUSART2              ((EUSART_TypeDef *) EUSART2_BASE)                   /**< EUSART2 base pointer */
#define EUSART3              ((EUSART_TypeDef *) EUSART3_BASE)                   /**< EUSART3 base pointer */
#define EUSART4              ((EUSART_TypeDef *) EUSART4_BASE)                   /**< EUSART4 base pointer */
#define SYSRTC0              ((SYSRTC_TypeDef *) SYSRTC0_BASE)                   /**< SYSRTC0 base pointer */
#define DMEM                 ((MPAHBRAM_TypeDef *) DMEM_BASE)                    /**< DMEM base pointer */
#define PFMXPPRF             ((PFMXPPRF_TypeDef *) PFMXPPRF_BASE)                /**< PFMXPPRF base pointer */
#define RFFPLL0              ((RFFPLL_TypeDef *) RFFPLL0_BASE)                   /**< RFFPLL0 base pointer */
#define ETAMPDET             ((ETAMPDET_TypeDef *) ETAMPDET_BASE)                /**< ETAMPDET base pointer */
#define BUFC                 ((BUFC_TypeDef *) BUFC_BASE)                        /**< BUFC base pointer */
#define RADIOAES             ((AES_TypeDef *) RADIOAES_BASE)                     /**< RADIOAES base pointer */
#define SMU                  ((SMU_TypeDef *) SMU_BASE)                          /**< SMU base pointer */
#define SMU_CFGNS            ((SMU_CFGNS_TypeDef *) SMU_CFGNS_BASE)              /**< SMU_CFGNS base pointer */
#define USBAHB_AHBS          ((USBAHB_AHBS_TypeDef *) USBAHB_AHBS_BASE)          /**< USBAHB_AHBS base pointer */
#define USB_APBS             ((USB_APBS_TypeDef *) USB_APBS_BASE)                /**< USB_APBS base pointer */
#define USBPLL0              ((PLL_TypeDef *) USBPLL0_BASE)                      /**< USBPLL0 base pointer */
#define LETIMER0             ((LETIMER_TypeDef *) LETIMER0_BASE)                 /**< LETIMER0 base pointer */
#define IADC0                ((IADC_TypeDef *) IADC0_BASE)                       /**< IADC0 base pointer */
#define ACMP0                ((ACMP_TypeDef *) ACMP0_BASE)                       /**< ACMP0 base pointer */
#define ACMP1                ((ACMP_TypeDef *) ACMP1_BASE)                       /**< ACMP1 base pointer */
#define AMUXCP0              ((AMUXCP_TypeDef *) AMUXCP0_BASE)                   /**< AMUXCP0 base pointer */
#define VDAC0                ((VDAC_TypeDef *) VDAC0_BASE)                       /**< VDAC0 base pointer */
#define PCNT0                ((PCNT_TypeDef *) PCNT0_BASE)                       /**< PCNT0 base pointer */
#define LESENSE              ((LESENSE_TypeDef *) LESENSE_BASE)                  /**< LESENSE base pointer */
#define HFRCOEM23            ((HFRCO_TypeDef *) HFRCOEM23_BASE)                  /**< HFRCOEM23 base pointer */
#define HFXO0                ((HFXO_TypeDef *) HFXO0_BASE)                       /**< HFXO0 base pointer */
#define I2C0                 ((I2C_TypeDef *) I2C0_BASE)                         /**< I2C0 base pointer */
#define WDOG0                ((WDOG_TypeDef *) WDOG0_BASE)                       /**< WDOG0 base pointer */
#define WDOG1                ((WDOG_TypeDef *) WDOG1_BASE)                       /**< WDOG1 base pointer */
#define EUSART0              ((EUSART_TypeDef *) EUSART0_BASE)                   /**< EUSART0 base pointer */
#define SEMAILBOX_HOST       ((SEMAILBOX_HOST_TypeDef *) SEMAILBOX_HOST_BASE)    /**< SEMAILBOX_HOST base pointer */
#define MPAHBRAM             ((MPAHBRAM_TypeDef *) MPAHBRAM_BASE)                /**< MPAHBRAM base pointer */
#define DEVINFO              ((DEVINFO_TypeDef *) DEVINFO_BASE)                  /**< DEVINFO base pointer */
/** @} End of group EFR32FG25B222F1920IM56_Peripheral_Declaration */

/**************************************************************************//**
 * @defgroup EFR32FG25B222F1920IM56_Peripheral_Parameters EFR32FG25B222F1920IM56 Peripheral Parameters
 * @{
 * @brief Device peripheral parameter values
 *****************************************************************************/

/* Common peripheral register block offsets. */
#define PER_REG_BLOCK_SET_OFFSET                0x1000UL    /**< Offset to SET register block */
#define PER_REG_BLOCK_CLR_OFFSET                0x2000UL    /**< Offset to CLEAR register block */
#define PER_REG_BLOCK_TGL_OFFSET                0x3000UL    /**< Offset to TOGGLE register block */
#define DMEM_AHB_DATA_WIDTH                     0x20UL      /**> Data width of the AHB interface */
#define DMEM_BANK0_SIZE                         0x8000UL    /**> Bank0 size */
#define DMEM_BANK10_SIZE                        0x8000UL    /**> Bank10 size */
#define DMEM_BANK11_SIZE                        0x8000UL    /**> Bank11 size */
#define DMEM_BANK12_SIZE                        0x8000UL    /**> Bank12 size */
#define DMEM_BANK13_SIZE                        0x8000UL    /**> Bank13 size */
#define DMEM_BANK14_SIZE                        0x8000UL    /**> Bank14 size */
#define DMEM_BANK15_SIZE                        0x8000UL    /**> Bank15 size */
#define DMEM_BANK16_SIZE                        0x0UL       /**> Bank16 size */
#define DMEM_BANK17_SIZE                        0x0UL       /**> Bank17 size */
#define DMEM_BANK18_SIZE                        0x0UL       /**> Bank18 size */
#define DMEM_BANK19_SIZE                        0x0UL       /**> Bank19 size */
#define DMEM_BANK1_SIZE                         0x8000UL    /**> Bank1 size */
#define DMEM_BANK20_SIZE                        0x0UL       /**> Bank20 size */
#define DMEM_BANK21_SIZE                        0x0UL       /**> Bank21 size */
#define DMEM_BANK22_SIZE                        0x0UL       /**> Bank22 size */
#define DMEM_BANK23_SIZE                        0x0UL       /**> Bank23 size */
#define DMEM_BANK24_SIZE                        0x0UL       /**> Bank24 size */
#define DMEM_BANK25_SIZE                        0x0UL       /**> Bank25 size */
#define DMEM_BANK26_SIZE                        0x0UL       /**> Bank26 size */
#define DMEM_BANK27_SIZE                        0x0UL       /**> Bank27 size */
#define DMEM_BANK28_SIZE                        0x0UL       /**> Bank28 size */
#define DMEM_BANK29_SIZE                        0x0UL       /**> Bank29 size */
#define DMEM_BANK2_SIZE                         0x8000UL    /**> Bank2 size */
#define DMEM_BANK30_SIZE                        0x0UL       /**> Bank30 size */
#define DMEM_BANK31_SIZE                        0x0UL       /**> Bank31 size */
#define DMEM_BANK3_SIZE                         0x8000UL    /**> Bank3 size */
#define DMEM_BANK4_SIZE                         0x8000UL    /**> Bank4 size */
#define DMEM_BANK5_SIZE                         0x8000UL    /**> Bank5 size */
#define DMEM_BANK6_SIZE                         0x8000UL    /**> Bank6 size */
#define DMEM_BANK7_SIZE                         0x8000UL    /**> Bank7 size */
#define DMEM_BANK8_SIZE                         0x8000UL    /**> Bank8 size */
#define DMEM_BANK9_SIZE                         0x8000UL    /**> Bank9 size */
#define DMEM_ECC_EXCLUDE                        0x0UL       /**> ECC exclude */
#define DMEM_MEM_SIZE                           0x80000UL   /**> Total memory size */
#define DMEM_NUM_BANKS                          0x10UL      /**> Number of physical SRAM banks */
#define DMEM_NUM_PORTS                          0x2UL       /**> Number of AHB slave ports */
#define DMEM_NUM_PORTS_IS_2                     0x1UL       /**> Boolean indicating if NUM_PORTS=2 */
#define DMEM_WAITSTATE_EXCLUDE                  0x1UL       /**> Waitstate exclude */
#define CMU_EXCLUDELCD                          0x1UL       /**> Exclude LCD */
#define HFRCO0_EM23ONDEMAND                     0x0UL       /**> EM23 On Demand */
#define HFRCO0_EXCLUDEEM23ONDEMAND              0x1UL       /**> Exclude EM23 On Demand */
#define LFXO_NO_CTUNE                           0x0UL       /**> CTUNE Not Present */
#define LFXO_CTUNE                              0x1UL       /**> CTUNE Present */
#define MSC_CDA_PRESENT                         0x0UL       /**>  */
#define MSC_FDIO_WIDTH                          0x80UL      /**> None */
#define MSC_FLASHADDRBITS                       0x15UL      /**> None */
#define MSC_FLASHBLOCKADDRBITS                  0x15UL      /**> None */
#define MSC_FLASH_BLOCK_INFO_PCOUNT             0x2UL       /**> None */
#define MSC_FLASH_BLOCK_MAIN_PCOUNT             0x100UL     /**>  */
#define MSC_INFOADDRBITS                        0xEUL       /**> None */
#define MSC_INFOBLOCKADDRBITS                   0xEUL       /**> None */
#define MSC_INFO_PSIZE_BITS                     0xDUL       /**> None */
#define MSC_MAIN_PSIZE_BITS                     0xDUL       /**> None */
#define MSC_REDUNDANCY                          0x2UL       /**> None */
#define MSC_ROOTMAIN_PRESENT                    0x1UL       /**>  */
#define MSC_UD_PRESENT                          0x1UL       /**>  */
#define MSC_YADDRBITS                           0x5UL       /**>  */
#define ICACHE0_AHB_LITE                        0x0UL       /**> AHB Lite */
#define ICACHE0_CACHEABLE_SIZE                  0x1E0000UL  /**> Cache Size */
#define ICACHE0_CACHEABLE_START                 0x8000000UL /**> Cache Start */
#define ICACHE0_DEFAULT_OFF                     0x0UL       /**> Default off */
#define ICACHE0_FLASH_SIZE                      0x1E0000UL  /**> Flash size */
#define ICACHE0_FLASH_START                     0x8000000UL /**> Flash start */
#define ICACHE0_LOOPCACHE_MEM_ADDR_BITS         0x3UL       /**> Loopcache Memory Address bits */
#define ICACHE0_LOOPCACHE_STICKINESS_BITS       0x4UL       /**> Loopcache Stickiness bits */
#define ICACHE0_PARITY_BITS                     0x1UL       /**> Use Parity */
#define ICACHE0_PC_BITS                         0x20UL      /**> Performance Counter bits */
#define ICACHE0_PIPE_STAGE                      0x1UL       /**> Pipeline Stage */
#define ICACHE0_RAM_ADDR_BITS                   0x0UL       /**> RAM Address bits */
#define ICACHE0_RAM_DATA_BITS                   0x0UL       /**> RAM Data bits */
#define ICACHE0_SET_BITS                        0x7UL       /**> Set bits */
#define ICACHE0_USE_HREADY_GATING               0x1UL       /**> Use HREADY gating */
#define ICACHE0_USE_IDLE_GATING                 0x1UL       /**> Use IDLE gating */
#define ICACHE0_USE_LOOPCACHE                   0x1UL       /**> Use Loopcache */
#define ICACHE0_WAY_BITS                        0x1UL       /**> Way bits */
#define ICACHE0_WORDS_PER_BLOCK                 0x0UL       /**> Words Per Block */
#define ICACHE0_WPB_BITS                        0x2UL       /**> Words Per Block bits */
#define ICACHE0_WPL_BITS                        0x3UL       /**> Words Per Line bits */
#define PRS_ASYNC_CH_NUM                        0xCUL       /**> None */
#define PRS_PRSSEL_WIDTH                        0x4UL       /**> New Param */
#define PRS_SPRSSEL_WIDTH                       0x2UL       /**> New Param */
#define PRS_SYNC_CH_NUM                         0x4UL       /**> None */
#define GPIO_MODE_WIDTH                         0x4UL       /**> Mode Width */
#define GPIO_NUM_EM4_WU                         0xCUL       /**> New Param */
#define GPIO_NUM_EVEN_PA                        0x6UL       /**> Num of even pins port A */
#define GPIO_NUM_EVEN_PB                        0x5UL       /**> Num of even pins port B */
#define GPIO_NUM_EVEN_PC                        0x6UL       /**> Num of even pins port C */
#define GPIO_NUM_EVEN_PD                        0x4UL       /**> Num of even pins port D */
#define GPIO_NUM_EXT_INT                        0xCUL       /**> New Param */
#define GPIO_NUM_EXT_INT_L                      0x8UL       /**> New Param */
#define GPIO_NUM_EXT_INT_U                      0x4UL       /**> New Param */
#define GPIO_NUM_EXT_INT_U_ZERO                 0x0UL       /**> New Param */
#define GPIO_NUM_ODD_PA                         0x6UL       /**> Num of odd pins port A */
#define GPIO_NUM_ODD_PB                         0x4UL       /**> Num of odd pins port B */
#define GPIO_NUM_ODD_PC                         0x5UL       /**> Num of odd pins port C */
#define GPIO_NUM_ODD_PD                         0x4UL       /**> Num of odd pins port D */
#define GPIO_PINSEL_WIDTH                       0x4UL       /**> Route config pin select width */
#define GPIO_PORTSEL_WIDTH                      0x2UL       /**> Route config port select width */
#define GPIO_PORT_A_WIDTH                       0xCUL       /**> Port A Width */
#define GPIO_PORT_A_WIDTH_ZERO                  0x0UL       /**> Port A Width is Zero */
#define GPIO_PORT_A_WL                          0x8UL       /**> New Param */
#define GPIO_PORT_A_WU                          0x4UL       /**> New Param */
#define GPIO_PORT_A_WU_ZERO                     0x0UL       /**> New Param */
#define GPIO_PORT_B_WIDTH                       0x9UL       /**> Port B Width */
#define GPIO_PORT_B_WIDTH_ZERO                  0x0UL       /**> Port B Width is Zero */
#define GPIO_PORT_B_WL                          0x8UL       /**> New Param */
#define GPIO_PORT_B_WU                          0x1UL       /**> New Param */
#define GPIO_PORT_B_WU_ZERO                     0x0UL       /**> New Param */
#define GPIO_PORT_C_WIDTH                       0xBUL       /**> Port C Width */
#define GPIO_PORT_C_WIDTH_ZERO                  0x0UL       /**> Port C Width is Zero */
#define GPIO_PORT_C_WL                          0x8UL       /**> New Param */
#define GPIO_PORT_C_WU                          0x3UL       /**> New Param */
#define GPIO_PORT_C_WU_ZERO                     0x0UL       /**> New Param */
#define GPIO_PORT_D_WIDTH                       0x8UL       /**> Port D Width */
#define GPIO_PORT_D_WIDTH_ZERO                  0x0UL       /**> Port D Width is Zero */
#define GPIO_PORT_D_WL                          0x8UL       /**> New Param */
#define GPIO_PORT_D_WU                          0x0UL       /**> New Param */
#define GPIO_PORT_D_WU_ZERO                     0x1UL       /**> New Param */
#define GPIO_SLEWRATE_WIDTH                     0x3UL       /**> Slew Rate Width Param */
#define LDMA_AHB1_ASYNC                         0x0UL       /**> AHB master 1 asynchronous support */
#define LDMA_CH_BITS                            0x4UL       /**> Channel Index Bit-width */
#define LDMA_CH_NUM                             0x10UL      /**> Number Of Chnnels */
#define LDMA_DUALDSTCHNL                        0x0UL       /**> Dual Destination Channels */
#define LDMA_DUALDSTCHNL_DIS                    0x1UL       /**> New Param */
#define LDMA_FIFO_BITS                          0x5UL       /**> FIFO Bit Width */
#define LDMA_FIFO_DEPTH                         0x10UL      /**> FIFO Depth */
#define LDMA_ILCHNL                             0x3UL       /**> Interleaving Channels */
#define LDMA_ILCHNL_DIS                         0x0UL       /**> New Param */
#define LDMA_SYNC_NUM                           0x8UL       /**> Sync Channels */
#define LDMA_SYNC_DIS                           0x0UL       /**> Sync structs supported */
#define LDMAXBAR_CH_BITS                        0x5UL       /**> None */
#define LDMAXBAR_CH_NUM                         0x10UL      /**> None */
#define LDMAXBAR_SIGSEL_W                       0x4UL       /**> New Param */
#define LDMAXBAR_SOURCESEL_W                    0x6UL       /**> New Param */
#define TIMER0_CC_NUM                           0x3UL       /**> Number of Compare/Capture Channels */
#define TIMER0_CNTWIDTH                         0x20UL      /**> Counter Width */
#define TIMER0_DTI                              0x1UL       /**> Dead-time insertion enabled */
#define TIMER0_DTI_CC_NUM                       0x3UL       /**> Number of DTI Channels */
#define TIMER0_NO_DTI                           0x0UL       /**>  */
#define TIMER1_CC_NUM                           0x3UL       /**> Number of Compare/Capture Channels */
#define TIMER1_CNTWIDTH                         0x20UL      /**> Counter Width */
#define TIMER1_DTI                              0x1UL       /**> Dead-time insertion enabled */
#define TIMER1_DTI_CC_NUM                       0x3UL       /**> Number of DTI Channels */
#define TIMER1_NO_DTI                           0x0UL       /**>  */
#define TIMER2_CC_NUM                           0x3UL       /**> Number of Compare/Capture Channels */
#define TIMER2_CNTWIDTH                         0x10UL      /**> Counter Width */
#define TIMER2_DTI                              0x1UL       /**> Dead-time insertion enabled */
#define TIMER2_DTI_CC_NUM                       0x3UL       /**> Number of DTI Channels */
#define TIMER2_NO_DTI                           0x0UL       /**>  */
#define TIMER3_CC_NUM                           0x3UL       /**> Number of Compare/Capture Channels */
#define TIMER3_CNTWIDTH                         0x10UL      /**> Counter Width */
#define TIMER3_DTI                              0x1UL       /**> Dead-time insertion enabled */
#define TIMER3_DTI_CC_NUM                       0x3UL       /**> Number of DTI Channels */
#define TIMER3_NO_DTI                           0x0UL       /**>  */
#define TIMER4_CC_NUM                           0x3UL       /**> Number of Compare/Capture Channels */
#define TIMER4_CNTWIDTH                         0x10UL      /**> Counter Width */
#define TIMER4_DTI                              0x1UL       /**> Dead-time insertion enabled */
#define TIMER4_DTI_CC_NUM                       0x3UL       /**> Number of DTI Channels */
#define TIMER4_NO_DTI                           0x0UL       /**>  */
#define TIMER5_CC_NUM                           0x3UL       /**> Number of Compare/Capture Channels */
#define TIMER5_CNTWIDTH                         0x10UL      /**> Counter Width */
#define TIMER5_DTI                              0x1UL       /**> Dead-time insertion enabled */
#define TIMER5_DTI_CC_NUM                       0x3UL       /**> Number of DTI Channels */
#define TIMER5_NO_DTI                           0x0UL       /**>  */
#define TIMER6_CC_NUM                           0x3UL       /**> Number of Compare/Capture Channels */
#define TIMER6_CNTWIDTH                         0x10UL      /**> Counter Width */
#define TIMER6_DTI                              0x1UL       /**> Dead-time insertion enabled */
#define TIMER6_DTI_CC_NUM                       0x3UL       /**> Number of DTI Channels */
#define TIMER6_NO_DTI                           0x0UL       /**>  */
#define TIMER7_CC_NUM                           0x3UL       /**> Number of Compare/Capture Channels */
#define TIMER7_CNTWIDTH                         0x10UL      /**> Counter Width */
#define TIMER7_DTI                              0x1UL       /**> Dead-time insertion enabled */
#define TIMER7_DTI_CC_NUM                       0x3UL       /**> Number of DTI Channels */
#define TIMER7_NO_DTI                           0x0UL       /**>  */
#define BURTC_CNTWIDTH                          0x20UL      /**> None */
#define BURTC_PRECNT_WIDTH                      0xFUL       /**>  */
#define I2C1_DELAY                              0x7D0UL     /**> Delay cell selection */
#define I2C1_DELAY_CHAIN_NUM                    0x2UL       /**> Number of delay chain */
#define SYSCFG_CHIP_PARTNUMBER                  0x0UL       /**> Chip Part Number */
#define SYSCFG_DEMODRAM_INST_COUNT              0x1UL       /**>  */
#define SYSCFG_FRCRAM_INST_COUNT                0x1UL       /**>  */
#define SYSCFG_SEQRAM_INST_COUNT                0x3UL       /**> None */
#define SYSCFG_SWINT_NUM                        0x4UL       /**> Software interupts */
#define DCDC_DRVSPEED_WIDTH                     0x2UL       /**> Drive Speed bitfield width */
#define DCDC_IPKVAL_WIDTH                       0x4UL       /**> Peak Current Setting bitfield Width */
#define DCDC_VCMPIBIAS_WIDTH                    0x2UL       /**> VCMP ibias bitfield width */
#define HOSTMAILBOX_NUM_MSGPTRS                 0x4UL       /**>  */
#define EUSART1_EM2_CAPABLE                     0x0UL       /**> EM2 Capable instance */
#define EUSART1_NOT_EM2_CAPABLE                 0x1UL       /**> Not EM2 Capable instance */
#define EUSART2_EM2_CAPABLE                     0x0UL       /**> EM2 Capable instance */
#define EUSART2_NOT_EM2_CAPABLE                 0x1UL       /**> Not EM2 Capable instance */
#define EUSART3_EM2_CAPABLE                     0x0UL       /**> EM2 Capable instance */
#define EUSART3_NOT_EM2_CAPABLE                 0x1UL       /**> Not EM2 Capable instance */
#define EUSART4_EM2_CAPABLE                     0x0UL       /**> EM2 Capable instance */
#define EUSART4_NOT_EM2_CAPABLE                 0x1UL       /**> Not EM2 Capable instance */
#define SYSRTC0_GROUP0_ALTIRQDIS                0x1UL       /**> Group 0 Alternate IRQ disable */
#define SYSRTC0_GROUP0_CAPDIS                   0x0UL       /**> Group 0 Capture disable */
#define SYSRTC0_GROUP0_CMP1DIS                  0x0UL       /**> Group 0 Compare1 disable */
#define SYSRTC0_GROUP0_DIS                      0x0UL       /**> Group 0 Disable */
#define SYSRTC0_GROUP0_ROOTDIS                  0x1UL       /**> Group 0 ROOT disable */
#define SYSRTC0_GROUP1_ALTIRQDIS                0x0UL       /**> Group 1 Alternate IRQ disable */
#define SYSRTC0_GROUP1_CAPDIS                   0x0UL       /**> Group 1 Capture disable */
#define SYSRTC0_GROUP1_CMP1DIS                  0x0UL       /**> Group 1 Compare1 disable */
#define SYSRTC0_GROUP1_DIS                      0x0UL       /**> Group 1 Disable */
#define SYSRTC0_GROUP1_ROOTDIS                  0x1UL       /**> Group 1 ROOT disable */
#define SYSRTC0_GROUP2_ALTIRQDIS                0x1UL       /**> Group 2 Alternate IRQ disable */
#define SYSRTC0_GROUP2_CAPDIS                   0x1UL       /**> Group 2 Capture disable */
#define SYSRTC0_GROUP2_CMP1DIS                  0x1UL       /**> Group 2 Compare1 disable */
#define SYSRTC0_GROUP2_DIS                      0x0UL       /**> Group 2 Disable */
#define SYSRTC0_GROUP2_ROOTDIS                  0x0UL       /**> Group 2 ROOT disable */
#define SYSRTC0_GROUP3_ALTIRQDIS                0x1UL       /**> Group 3 Alternate IRQ disable */
#define SYSRTC0_GROUP3_CAPDIS                   0x1UL       /**> Group 3 Capture disable */
#define SYSRTC0_GROUP3_CMP1DIS                  0x1UL       /**> Group 3 Compare1 disable */
#define SYSRTC0_GROUP3_DIS                      0x1UL       /**> Group 3 Disable */
#define SYSRTC0_GROUP3_ROOTDIS                  0x1UL       /**> Group 3 ROOT disable */
#define SYSRTC0_GROUP4_ALTIRQDIS                0x1UL       /**> Group 4 Alternate IRQ disable */
#define SYSRTC0_GROUP4_CAPDIS                   0x1UL       /**> Group 4 Capture disable */
#define SYSRTC0_GROUP4_CMP1DIS                  0x1UL       /**> Group 4 Compare1 disable */
#define SYSRTC0_GROUP4_DIS                      0x1UL       /**> Group 4 Disable */
#define SYSRTC0_GROUP4_ROOTDIS                  0x1UL       /**> Group 4 ROOT disable */
#define SYSRTC0_GROUP5_ALTIRQDIS                0x1UL       /**> Group 5 Alternate IRQ disable */
#define SYSRTC0_GROUP5_CAPDIS                   0x1UL       /**> Group 5 Capture disable */
#define SYSRTC0_GROUP5_CMP1DIS                  0x1UL       /**> Group 5 Compare1 disable */
#define SYSRTC0_GROUP5_DIS                      0x1UL       /**> Group 5 Disable */
#define SYSRTC0_GROUP5_ROOTDIS                  0x1UL       /**> Group 5 ROOT disable */
#define SYSRTC0_GROUP6_ALTIRQDIS                0x1UL       /**> Group 6 Alternate IRQ disable */
#define SYSRTC0_GROUP6_CAPDIS                   0x1UL       /**> Group 6 Capture disable */
#define SYSRTC0_GROUP6_CMP1DIS                  0x1UL       /**> Group 6 Compare1 disable */
#define SYSRTC0_GROUP6_DIS                      0x1UL       /**> Group 6 Disable */
#define SYSRTC0_GROUP6_ROOTDIS                  0x1UL       /**> Group 6 ROOT disable */
#define SYSRTC0_GROUP7_ALTIRQDIS                0x1UL       /**> Group 7 Alternate IRQ disable */
#define SYSRTC0_GROUP7_CAPDIS                   0x1UL       /**> Group 7 Capture disable */
#define SYSRTC0_GROUP7_CMP1DIS                  0x1UL       /**> Group 7 Compare1 disable */
#define SYSRTC0_GROUP7_DIS                      0x1UL       /**> Group 7 Disable */
#define SYSRTC0_GROUP7_ROOTDIS                  0x1UL       /**> Group 7 ROOT disable */
#define SYSRTC0_ROOTDIS                         0x0UL       /**> ROOT disable */
#define PFMXPPRF_COUNT_WIDTH                    0x9UL       /**> Width of counters for pulse-pairing */
#define ETAMPDET_NUM_CHNLS                      0x2UL       /**>  */
#define BUFC_BUFC_FRC_ASYNC                     0x1UL       /**> New Param */
#define BUFC_LOG2NUMOFBUFS                      0x2UL       /**> New Param */
#define BUFC_LOG2NUMOFINPUTS                    0x1UL       /**> New Param */
#define BUFC_NUMOFBUFS                          0x4UL       /**> New Param */
#define BUFC_NUMOFINPUTS                        0x1UL       /**> New Param */
#define RADIOAES_SIDECHANNEL_COUNTERMEASURES    0x1UL       /**> Enable sidechannel counter measures */
#define SMU_NUM_BMPUS                           0xAUL       /**> Number of BMPUs */
#define SMU_NUM_PPU_PERIPHS                     0x3EUL      /**> Number of PPU Peripherals */
#define SMU_NUM_PPU_PERIPHS_MOD_32              0x1EUL      /**> Number of PPU Peripherals (mod 32) */
#define SMU_NUM_PPU_PERIPHS_SUB_32              0x1EUL      /**> Number of PPU peripherals minus 32 */
#define SMU_PERIPHID_BITS                       0x8UL       /**> Bits used for Peripheral ID */
#define USBAHB_EP_NUM                           0x9UL       /**>  */
#define LETIMER0_CNT_WIDTH                      0x18UL      /**> Count Width */
#define IADC0_CONFIGNUM                         0x2UL       /**> CONFIG */
#define IADC0_FULLRANGEUNIPOLAR                 0x0UL       /**> FULLRANGEUNIPOLAR */
#define IADC0_SCANBYTES                         0x1UL       /**> SCANBYTES */
#define IADC0_ENTRIES                           0x10UL      /**> ENTRIES */
#define ACMP0_DAC_INPUT                         0x1UL       /**> None */
#define ACMP0_EXT_OVR_IF                        0x1UL       /**> None */
#define ACMP1_DAC_INPUT                         0x1UL       /**> None */
#define ACMP1_EXT_OVR_IF                        0x1UL       /**> None */
#define AMUXCP0_AMUXCPNUM                       0x0UL       /**> AMUXCPNUM */
#define VDAC0_ALT_WIDTH                         0x6UL       /**> VOUT_AUX Out Width */
#define VDAC0_CH0_TRIG_LESENSE                  0x1UL       /**> CH0 Trig Source = LESENSE */
#define VDAC0_CH1_TRIG_LESENSE                  0x0UL       /**> CH1 Trig Source = LESENSE */
#define VDAC0_FIFO_DEPTH                        0x4UL       /**> WFIFO Depth */
#define VDAC0_INT_PRESC_WIDTH                   0x7UL       /**> Internal Prescaler Width */
#define VDAC0_RESOLUTION                        0xCUL       /**> DAC Resolution */
#define PCNT0_PCNT_WIDTH                        0x10UL      /**> None */
#define LESENSE_CHANNEL_NUM                     0x10UL      /**> None */
#define LESENSE_RIPCNT_WIDTH                    0x10UL      /**> None */
#define LESENSE_STATE_NUM                       0x20UL      /**> None */
#define HFRCOEM23_EM23ONDEMAND                  0x1UL       /**> EM23 On Demand */
#define HFRCOEM23_EXCLUDEEM23ONDEMAND           0x0UL       /**> Exclude EM23 On Demand */
#define HFXO0_BUFOUT                            0x1UL       /**> BUFOUT */
#define HFXO0_EXCLUDEBUFOUT                     0x0UL       /**> Exclude BUFOUT */
#define I2C0_DELAY                              0x3E8UL     /**> Delay cell selection */
#define I2C0_DELAY_CHAIN_NUM                    0x2UL       /**> Number of delay chain */
#define WDOG0_PCNUM                             0x2UL       /**> None */
#define WDOG1_PCNUM                             0x2UL       /**> None */
#define EUSART0_EM2_CAPABLE                     0x1UL       /**> EM2 Capable instance */
#define EUSART0_NOT_EM2_CAPABLE                 0x0UL       /**> Not EM2 Capable instance */
#define RDMEM_FRC_BANK0_SIZE                    0x1000UL    /**> FRC_RAM_BANK0_SIZE */
#define RDMEM_FRC_BANK1_SIZE                    0x0UL       /**> FRC_RAM_BANK1_SIZE */
#define RDMEM_FRC_BANK2_SIZE                    0x0UL       /**> FRC_RAM_BANK2_SIZE */
#define RDMEM_FRC_BANK3_SIZE                    0x0UL       /**> FRC_RAM_BANK3_SIZE */
#define RDMEM_FRC_BANK4_SIZE                    0x0UL       /**> FRC_RAM_BANK4_SIZE */
#define RDMEM_FRC_BANK5_SIZE                    0x0UL       /**> FRC_RAM_BANK5_SIZE */
#define RDMEM_FRC_BANK6_SIZE                    0x0UL       /**> FRC_RAM_BANK6_SIZE */
#define RDMEM_FRC_BANK7_SIZE                    0x0UL       /**> FRC_RAM_BANK7_SIZE */
#define RDMEM_FRC_NUM_BANK                      0x1UL       /**> FRC_NUM_BANK */
#define RDMEM_FRC_RAMADDRBITS                   0xCUL       /**> FRC RAM ADDRBITS */
#define RDMEM_FRC_RAMADDRMINBITS                0xCUL       /**> FRC RAM address bits for one bank */
#define RDMEM_FRC_RAMECCADDR_WIDTH              0x20UL      /**> FRC RAM ECC Address width */
#define RDMEM_FRC_RAM_BWE_WIDTH                 0x27UL      /**> FRCRAM BWE width */
#define RDMEM_FRC_RAM_DATA_WIDTH                0x27UL      /**> FRC_RAM_DATA_WIDTH */
#define RDMEM_FRC_RAM_ECC_EN                    0x1UL       /**> FRC RAM ECCEN */
#define RDMEM_FRC_RAM_TOTAL_SIZE                0x1000UL    /**> FRC_RAM_TOTAL_SIZE */
#define RDMEM_SEQ_BANK0_SIZE                    0x2000UL    /**> SEQ_RAM_BANK0_SIZE */
#define RDMEM_SEQ_BANK1_SIZE                    0x2000UL    /**> SEQ_RAM_BANK1_SIZE */
#define RDMEM_SEQ_BANK2_SIZE                    0x2000UL    /**> SEQ_RAM_BANK2_SIZE */
#define RDMEM_SEQ_BANK3_SIZE                    0x0UL       /**> SEQ_RAM_BANK3_SIZE */
#define RDMEM_SEQ_BANK4_SIZE                    0x0UL       /**> SEQ_RAM_BANK4_SIZE */
#define RDMEM_SEQ_BANK5_SIZE                    0x0UL       /**> SEQ_RAM_BANK5_SIZE */
#define RDMEM_SEQ_BANK6_SIZE                    0x0UL       /**> SEQ_RAM_BANK6_SIZE */
#define RDMEM_SEQ_BANK7_SIZE                    0x0UL       /**> SEQ_RAM_BANK7_SIZE */
#define RDMEM_SEQ_NUM_BANK                      0x3UL       /**> SEQ_NUM_BANK */
#define RDMEM_SEQ_RAMADDRBITS                   0xFUL       /**> SEQ RAM ADDRBITS */
#define RDMEM_SEQ_RAMADDRMINBITS                0xDUL       /**> SEQ RAM address bits for one bank */
#define RDMEM_SEQ_RAMECCADDR_WIDTH              0x20UL      /**> SEQ RAM ECC Address width */
#define RDMEM_SEQ_RAM_BWE_WIDTH                 0x27UL      /**> SEQRAM BWE width */
#define RDMEM_SEQ_RAM_DATA_WIDTH                0x27UL      /**> SEQ_RAM_DATA_WIDTH */
#define RDMEM_SEQ_RAM_ECC_EN                    0x1UL       /**> SEQ RAM ECCEN */
#define RDMEM_SEQ_RAM_TOTAL_SIZE                0x6000UL    /**> SEQ_RAM_TOTAL_SIZE */
#define MPAHBRAM_AHB_DATA_WIDTH                 0x20UL      /**> Data width of the AHB interface */
#define MPAHBRAM_BANK0_SIZE                     0x4000UL    /**> Bank0 size */
#define MPAHBRAM_BANK10_SIZE                    0x0UL       /**> Bank10 size */
#define MPAHBRAM_BANK11_SIZE                    0x0UL       /**> Bank11 size */
#define MPAHBRAM_BANK12_SIZE                    0x0UL       /**> Bank12 size */
#define MPAHBRAM_BANK13_SIZE                    0x0UL       /**> Bank13 size */
#define MPAHBRAM_BANK14_SIZE                    0x0UL       /**> Bank14 size */
#define MPAHBRAM_BANK15_SIZE                    0x0UL       /**> Bank15 size */
#define MPAHBRAM_BANK16_SIZE                    0x0UL       /**> Bank16 size */
#define MPAHBRAM_BANK17_SIZE                    0x0UL       /**> Bank17 size */
#define MPAHBRAM_BANK18_SIZE                    0x0UL       /**> Bank18 size */
#define MPAHBRAM_BANK19_SIZE                    0x0UL       /**> Bank19 size */
#define MPAHBRAM_BANK1_SIZE                     0x4000UL    /**> Bank1 size */
#define MPAHBRAM_BANK20_SIZE                    0x0UL       /**> Bank20 size */
#define MPAHBRAM_BANK21_SIZE                    0x0UL       /**> Bank21 size */
#define MPAHBRAM_BANK22_SIZE                    0x0UL       /**> Bank22 size */
#define MPAHBRAM_BANK23_SIZE                    0x0UL       /**> Bank23 size */
#define MPAHBRAM_BANK24_SIZE                    0x0UL       /**> Bank24 size */
#define MPAHBRAM_BANK25_SIZE                    0x0UL       /**> Bank25 size */
#define MPAHBRAM_BANK26_SIZE                    0x0UL       /**> Bank26 size */
#define MPAHBRAM_BANK27_SIZE                    0x0UL       /**> Bank27 size */
#define MPAHBRAM_BANK28_SIZE                    0x0UL       /**> Bank28 size */
#define MPAHBRAM_BANK29_SIZE                    0x0UL       /**> Bank29 size */
#define MPAHBRAM_BANK2_SIZE                     0x4000UL    /**> Bank2 size */
#define MPAHBRAM_BANK30_SIZE                    0x0UL       /**> Bank30 size */
#define MPAHBRAM_BANK31_SIZE                    0x0UL       /**> Bank31 size */
#define MPAHBRAM_BANK3_SIZE                     0x4000UL    /**> Bank3 size */
#define MPAHBRAM_BANK4_SIZE                     0x0UL       /**> Bank4 size */
#define MPAHBRAM_BANK5_SIZE                     0x0UL       /**> Bank5 size */
#define MPAHBRAM_BANK6_SIZE                     0x0UL       /**> Bank6 size */
#define MPAHBRAM_BANK7_SIZE                     0x0UL       /**> Bank7 size */
#define MPAHBRAM_BANK8_SIZE                     0x0UL       /**> Bank8 size */
#define MPAHBRAM_BANK9_SIZE                     0x0UL       /**> Bank9 size */
#define MPAHBRAM_ECC_EXCLUDE                    0x0UL       /**> ECC exclude */
#define MPAHBRAM_MEM_SIZE                       0x10000UL   /**> Total memory size */
#define MPAHBRAM_NUM_BANKS                      0x4UL       /**> Number of physical SRAM banks */
#define MPAHBRAM_NUM_PORTS                      0x2UL       /**> Number of AHB slave ports */
#define MPAHBRAM_NUM_PORTS_IS_2                 0x1UL       /**> Boolean indicating if NUM_PORTS=2 */
#define MPAHBRAM_WAITSTATE_EXCLUDE              0x1UL       /**> Waitstate exclude */
#define AHBROM_BANK0_SIZE                       0x2000UL    /**> Bank0 Size */
#define AHBROM_BANK1_SIZE                       0x0UL       /**> Bank1 Size */
#define AHBROM_BANK2_SIZE                       0x0UL       /**> Bank2 Size */
#define AHBROM_BANK3_SIZE                       0x0UL       /**> Bank3 Size */
#define AHBROM_BANK4_SIZE                       0x0UL       /**> Bank4 Size */
#define AHBROM_BANK5_SIZE                       0x0UL       /**> Bank5 Size */
#define AHBROM_BANK6_SIZE                       0x0UL       /**> Bank6 Size */
#define AHBROM_BANK7_SIZE                       0x0UL       /**> Bank7 Size */
#define AHBROM_NUM_BANKS                        0x1UL       /**> Number of Banks */
#define AHBROM_SIZE                             0x2000UL    /**> Total size */

/* Instance macros for ACMP */
#define ACMP(n)                         (((n) == 0) ? ACMP0   \
                                         : ((n) == 1) ? ACMP1 \
                                         : 0x0UL)
#define ACMP_NUM(ref)                   (((ref) == ACMP0) ? 0   \
                                         : ((ref) == ACMP1) ? 1 \
                                         : -1)
#define ACMP_DAC_INPUT(n)               (((n) == 0) ? ACMP0_DAC_INPUT   \
                                         : ((n) == 1) ? ACMP1_DAC_INPUT \
                                         : 0x0UL)
#define ACMP_EXT_OVR_IF(n)              (((n) == 0) ? ACMP0_EXT_OVR_IF   \
                                         : ((n) == 1) ? ACMP1_EXT_OVR_IF \
                                         : 0x0UL)

/* Instance macros for EUSART */
#define EUSART(n)                       (((n) == 0) ? EUSART0   \
                                         : ((n) == 1) ? EUSART1 \
                                         : ((n) == 2) ? EUSART2 \
                                         : ((n) == 3) ? EUSART3 \
                                         : ((n) == 4) ? EUSART4 \
                                         : 0x0UL)
#define EUSART_NUM(ref)                 (((ref) == EUSART0) ? 0   \
                                         : ((ref) == EUSART1) ? 1 \
                                         : ((ref) == EUSART2) ? 2 \
                                         : ((ref) == EUSART3) ? 3 \
                                         : ((ref) == EUSART4) ? 4 \
                                         : -1)
#define EUSART_EM2_CAPABLE(n)           (((n) == 0) ? EUSART0_EM2_CAPABLE   \
                                         : ((n) == 1) ? EUSART1_EM2_CAPABLE \
                                         : ((n) == 2) ? EUSART2_EM2_CAPABLE \
                                         : ((n) == 3) ? EUSART3_EM2_CAPABLE \
                                         : ((n) == 4) ? EUSART4_EM2_CAPABLE \
                                         : 0x0UL)
#define EUSART_NOT_EM2_CAPABLE(n)       (((n) == 0) ? EUSART0_NOT_EM2_CAPABLE   \
                                         : ((n) == 1) ? EUSART1_NOT_EM2_CAPABLE \
                                         : ((n) == 2) ? EUSART2_NOT_EM2_CAPABLE \
                                         : ((n) == 3) ? EUSART3_NOT_EM2_CAPABLE \
                                         : ((n) == 4) ? EUSART4_NOT_EM2_CAPABLE \
                                         : 0x0UL)

/* Instance macros for HFRCO */
#define HFRCO(n)                        (((n) == 0) ? HFRCO0      \
                                         : ((n) == 1) ? HFRCOEM23 \
                                         : 0x0UL)
#define HFRCO_NUM(ref)                  (((ref) == HFRCO0) ? 0      \
                                         : ((ref) == HFRCOEM23) ? 1 \
                                         : -1)
#define HFRCO_EM23ONDEMAND(n)           (((n) == 0) ? HFRCO0_EM23ONDEMAND      \
                                         : ((n) == 1) ? HFRCOEM23_EM23ONDEMAND \
                                         : 0x0UL)
#define HFRCO_EXCLUDEEM23ONDEMAND(n)    (((n) == 0) ? HFRCO0_EXCLUDEEM23ONDEMAND      \
                                         : ((n) == 1) ? HFRCOEM23_EXCLUDEEM23ONDEMAND \
                                         : 0x0UL)

/* Instance macros for I2C */
#define I2C(n)                          (((n) == 0) ? I2C0   \
                                         : ((n) == 1) ? I2C1 \
                                         : 0x0UL)
#define I2C_NUM(ref)                    (((ref) == I2C0) ? 0   \
                                         : ((ref) == I2C1) ? 1 \
                                         : -1)
#define I2C_DELAY(n)                    (((n) == 0) ? I2C0_DELAY   \
                                         : ((n) == 1) ? I2C1_DELAY \
                                         : 0x0UL)
#define I2C_DELAY_CHAIN_NUM(n)          (((n) == 0) ? I2C0_DELAY_CHAIN_NUM   \
                                         : ((n) == 1) ? I2C1_DELAY_CHAIN_NUM \
                                         : 0x0UL)

/* Instance macros for TIMER */
#define TIMER(n)                        (((n) == 0) ? TIMER0   \
                                         : ((n) == 1) ? TIMER1 \
                                         : ((n) == 2) ? TIMER2 \
                                         : ((n) == 3) ? TIMER3 \
                                         : ((n) == 4) ? TIMER4 \
                                         : ((n) == 5) ? TIMER5 \
                                         : ((n) == 6) ? TIMER6 \
                                         : ((n) == 7) ? TIMER7 \
                                         : 0x0UL)
#define TIMER_NUM(ref)                  (((ref) == TIMER0) ? 0   \
                                         : ((ref) == TIMER1) ? 1 \
                                         : ((ref) == TIMER2) ? 2 \
                                         : ((ref) == TIMER3) ? 3 \
                                         : ((ref) == TIMER4) ? 4 \
                                         : ((ref) == TIMER5) ? 5 \
                                         : ((ref) == TIMER6) ? 6 \
                                         : ((ref) == TIMER7) ? 7 \
                                         : -1)
#define TIMER_CC_NUM(n)                 (((n) == 0) ? TIMER0_CC_NUM   \
                                         : ((n) == 1) ? TIMER1_CC_NUM \
                                         : ((n) == 2) ? TIMER2_CC_NUM \
                                         : ((n) == 3) ? TIMER3_CC_NUM \
                                         : ((n) == 4) ? TIMER4_CC_NUM \
                                         : ((n) == 5) ? TIMER5_CC_NUM \
                                         : ((n) == 6) ? TIMER6_CC_NUM \
                                         : ((n) == 7) ? TIMER7_CC_NUM \
                                         : 0x0UL)
#define TIMER_CNTWIDTH(n)               (((n) == 0) ? TIMER0_CNTWIDTH   \
                                         : ((n) == 1) ? TIMER1_CNTWIDTH \
                                         : ((n) == 2) ? TIMER2_CNTWIDTH \
                                         : ((n) == 3) ? TIMER3_CNTWIDTH \
                                         : ((n) == 4) ? TIMER4_CNTWIDTH \
                                         : ((n) == 5) ? TIMER5_CNTWIDTH \
                                         : ((n) == 6) ? TIMER6_CNTWIDTH \
                                         : ((n) == 7) ? TIMER7_CNTWIDTH \
                                         : 0x0UL)
#define TIMER_DTI(n)                    (((n) == 0) ? TIMER0_DTI   \
                                         : ((n) == 1) ? TIMER1_DTI \
                                         : ((n) == 2) ? TIMER2_DTI \
                                         : ((n) == 3) ? TIMER3_DTI \
                                         : ((n) == 4) ? TIMER4_DTI \
                                         : ((n) == 5) ? TIMER5_DTI \
                                         : ((n) == 6) ? TIMER6_DTI \
                                         : ((n) == 7) ? TIMER7_DTI \
                                         : 0x0UL)
#define TIMER_DTI_CC_NUM(n)             (((n) == 0) ? TIMER0_DTI_CC_NUM   \
                                         : ((n) == 1) ? TIMER1_DTI_CC_NUM \
                                         : ((n) == 2) ? TIMER2_DTI_CC_NUM \
                                         : ((n) == 3) ? TIMER3_DTI_CC_NUM \
                                         : ((n) == 4) ? TIMER4_DTI_CC_NUM \
                                         : ((n) == 5) ? TIMER5_DTI_CC_NUM \
                                         : ((n) == 6) ? TIMER6_DTI_CC_NUM \
                                         : ((n) == 7) ? TIMER7_DTI_CC_NUM \
                                         : 0x0UL)
#define TIMER_NO_DTI(n)                 (((n) == 0) ? TIMER0_NO_DTI   \
                                         : ((n) == 1) ? TIMER1_NO_DTI \
                                         : ((n) == 2) ? TIMER2_NO_DTI \
                                         : ((n) == 3) ? TIMER3_NO_DTI \
                                         : ((n) == 4) ? TIMER4_NO_DTI \
                                         : ((n) == 5) ? TIMER5_NO_DTI \
                                         : ((n) == 6) ? TIMER6_NO_DTI \
                                         : ((n) == 7) ? TIMER7_NO_DTI \
                                         : 0x0UL)

/* Instance macros for WDOG */
#define WDOG(n)                         (((n) == 0) ? WDOG0   \
                                         : ((n) == 1) ? WDOG1 \
                                         : 0x0UL)
#define WDOG_NUM(ref)                   (((ref) == WDOG0) ? 0   \
                                         : ((ref) == WDOG1) ? 1 \
                                         : -1)
#define WDOG_PCNUM(n)                   (((n) == 0) ? WDOG0_PCNUM   \
                                         : ((n) == 1) ? WDOG1_PCNUM \
                                         : 0x0UL)

/** @} End of group EFR32FG25B222F1920IM56_Peripheral_Parameters  */

/** @} End of group EFR32FG25B222F1920IM56 */
/** @}} End of group Parts */

#ifdef __cplusplus
}
#endif
#endif
