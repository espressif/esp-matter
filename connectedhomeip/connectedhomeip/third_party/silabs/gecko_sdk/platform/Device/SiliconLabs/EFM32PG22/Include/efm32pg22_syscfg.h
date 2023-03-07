/**************************************************************************//**
 * @file
 * @brief EFM32PG22 SYSCFG register and bit field definitions
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
#ifndef EFM32PG22_SYSCFG_H
#define EFM32PG22_SYSCFG_H
#define SYSCFG_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFM32PG22_SYSCFG SYSCFG
 * @{
 * @brief EFM32PG22 SYSCFG Register Declaration.
 *****************************************************************************/

/** SYSCFG Register Declaration. */
typedef struct {
  __IOM uint32_t IF;                            /**< Interrupt Flag                                     */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  uint32_t       RESERVED0[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW;                     /**< Chip Revision, Hard-wired                          */
  __IOM uint32_t CHIPREV;                       /**< Part Family and Revision Values                    */
  uint32_t       RESERVED1[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFGSYSTIC;                     /**< SysTick clock source                               */
  uint32_t       RESERVED2[55U];                /**< Reserved for future use                            */
  uint32_t       RESERVED3[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED4[63U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL;                          /**< Memory System Control                              */
  uint32_t       RESERVED5[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL;                 /**< DMEM0 Retention Control                            */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  DMEM0ECCADDR;                  /**< DMEM0 ECC Address                                  */
  __IOM uint32_t DMEM0ECCCTRL;                  /**< DMEM0 ECC Control                                  */
  uint32_t       RESERVED7[250U];               /**< Reserved for future use                            */
  __IOM uint32_t ROOTDATA0;                     /**< Root Data Register 0                               */
  __IOM uint32_t ROOTDATA1;                     /**< Root Data Register 1                               */
  __IM uint32_t  ROOTLOCKSTATUS;                /**< Lock Status                                        */
  uint32_t       RESERVED8[637U];               /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED9[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW_SET;                 /**< Chip Revision, Hard-wired                          */
  __IOM uint32_t CHIPREV_SET;                   /**< Part Family and Revision Values                    */
  uint32_t       RESERVED10[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CFGSYSTIC_SET;                 /**< SysTick clock source                               */
  uint32_t       RESERVED11[55U];               /**< Reserved for future use                            */
  uint32_t       RESERVED12[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED13[63U];               /**< Reserved for future use                            */
  __IOM uint32_t CTRL_SET;                      /**< Memory System Control                              */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL_SET;             /**< DMEM0 Retention Control                            */
  uint32_t       RESERVED15[1U];                /**< Reserved for future use                            */
  __IM uint32_t  DMEM0ECCADDR_SET;              /**< DMEM0 ECC Address                                  */
  __IOM uint32_t DMEM0ECCCTRL_SET;              /**< DMEM0 ECC Control                                  */
  uint32_t       RESERVED16[250U];              /**< Reserved for future use                            */
  __IOM uint32_t ROOTDATA0_SET;                 /**< Root Data Register 0                               */
  __IOM uint32_t ROOTDATA1_SET;                 /**< Root Data Register 1                               */
  __IM uint32_t  ROOTLOCKSTATUS_SET;            /**< Lock Status                                        */
  uint32_t       RESERVED17[637U];              /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED18[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW_CLR;                 /**< Chip Revision, Hard-wired                          */
  __IOM uint32_t CHIPREV_CLR;                   /**< Part Family and Revision Values                    */
  uint32_t       RESERVED19[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CFGSYSTIC_CLR;                 /**< SysTick clock source                               */
  uint32_t       RESERVED20[55U];               /**< Reserved for future use                            */
  uint32_t       RESERVED21[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED22[63U];               /**< Reserved for future use                            */
  __IOM uint32_t CTRL_CLR;                      /**< Memory System Control                              */
  uint32_t       RESERVED23[1U];                /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL_CLR;             /**< DMEM0 Retention Control                            */
  uint32_t       RESERVED24[1U];                /**< Reserved for future use                            */
  __IM uint32_t  DMEM0ECCADDR_CLR;              /**< DMEM0 ECC Address                                  */
  __IOM uint32_t DMEM0ECCCTRL_CLR;              /**< DMEM0 ECC Control                                  */
  uint32_t       RESERVED25[250U];              /**< Reserved for future use                            */
  __IOM uint32_t ROOTDATA0_CLR;                 /**< Root Data Register 0                               */
  __IOM uint32_t ROOTDATA1_CLR;                 /**< Root Data Register 1                               */
  __IM uint32_t  ROOTLOCKSTATUS_CLR;            /**< Lock Status                                        */
  uint32_t       RESERVED26[637U];              /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED27[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW_TGL;                 /**< Chip Revision, Hard-wired                          */
  __IOM uint32_t CHIPREV_TGL;                   /**< Part Family and Revision Values                    */
  uint32_t       RESERVED28[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CFGSYSTIC_TGL;                 /**< SysTick clock source                               */
  uint32_t       RESERVED29[55U];               /**< Reserved for future use                            */
  uint32_t       RESERVED30[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED31[63U];               /**< Reserved for future use                            */
  __IOM uint32_t CTRL_TGL;                      /**< Memory System Control                              */
  uint32_t       RESERVED32[1U];                /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL_TGL;             /**< DMEM0 Retention Control                            */
  uint32_t       RESERVED33[1U];                /**< Reserved for future use                            */
  __IM uint32_t  DMEM0ECCADDR_TGL;              /**< DMEM0 ECC Address                                  */
  __IOM uint32_t DMEM0ECCCTRL_TGL;              /**< DMEM0 ECC Control                                  */
  uint32_t       RESERVED34[250U];              /**< Reserved for future use                            */
  __IOM uint32_t ROOTDATA0_TGL;                 /**< Root Data Register 0                               */
  __IOM uint32_t ROOTDATA1_TGL;                 /**< Root Data Register 1                               */
  __IM uint32_t  ROOTLOCKSTATUS_TGL;            /**< Lock Status                                        */
} SYSCFG_TypeDef;
/** @} End of group EFM32PG22_SYSCFG */

/**************************************************************************//**
 * @addtogroup EFM32PG22_SYSCFG
 * @{
 * @defgroup EFM32PG22_SYSCFG_BitFields SYSCFG Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SYSCFG IF */
#define _SYSCFG_IF_RESETVALUE                           0x00000000UL                        /**< Default value for SYSCFG_IF                 */
#define _SYSCFG_IF_MASK                                 0x3303000FUL                        /**< Mask for SYSCFG_IF                          */
#define SYSCFG_IF_SW0                                   (0x1UL << 0)                        /**< Software Interrupt 0                        */
#define _SYSCFG_IF_SW0_SHIFT                            0                                   /**< Shift value for SYSCFG_SW0                  */
#define _SYSCFG_IF_SW0_MASK                             0x1UL                               /**< Bit mask for SYSCFG_SW0                     */
#define _SYSCFG_IF_SW0_DEFAULT                          0x00000000UL                        /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW0_DEFAULT                           (_SYSCFG_IF_SW0_DEFAULT << 0)       /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SW1                                   (0x1UL << 1)                        /**< Software Interrupt 1                        */
#define _SYSCFG_IF_SW1_SHIFT                            1                                   /**< Shift value for SYSCFG_SW1                  */
#define _SYSCFG_IF_SW1_MASK                             0x2UL                               /**< Bit mask for SYSCFG_SW1                     */
#define _SYSCFG_IF_SW1_DEFAULT                          0x00000000UL                        /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW1_DEFAULT                           (_SYSCFG_IF_SW1_DEFAULT << 1)       /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SW2                                   (0x1UL << 2)                        /**< Software Interrupt 2                        */
#define _SYSCFG_IF_SW2_SHIFT                            2                                   /**< Shift value for SYSCFG_SW2                  */
#define _SYSCFG_IF_SW2_MASK                             0x4UL                               /**< Bit mask for SYSCFG_SW2                     */
#define _SYSCFG_IF_SW2_DEFAULT                          0x00000000UL                        /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW2_DEFAULT                           (_SYSCFG_IF_SW2_DEFAULT << 2)       /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SW3                                   (0x1UL << 3)                        /**< Software Interrupt 3                        */
#define _SYSCFG_IF_SW3_SHIFT                            3                                   /**< Shift value for SYSCFG_SW3                  */
#define _SYSCFG_IF_SW3_MASK                             0x8UL                               /**< Bit mask for SYSCFG_SW3                     */
#define _SYSCFG_IF_SW3_DEFAULT                          0x00000000UL                        /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW3_DEFAULT                           (_SYSCFG_IF_SW3_DEFAULT << 3)       /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_RAMERR1B                              (0x1UL << 16)                       /**< RAM 1-Bit Error Interrupt Flag              */
#define _SYSCFG_IF_RAMERR1B_SHIFT                       16                                  /**< Shift value for SYSCFG_RAMERR1B             */
#define _SYSCFG_IF_RAMERR1B_MASK                        0x10000UL                           /**< Bit mask for SYSCFG_RAMERR1B                */
#define _SYSCFG_IF_RAMERR1B_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_RAMERR1B_DEFAULT                      (_SYSCFG_IF_RAMERR1B_DEFAULT << 16) /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_RAMERR2B                              (0x1UL << 17)                       /**< RAM 2-Bit Error Interrupt Flag              */
#define _SYSCFG_IF_RAMERR2B_SHIFT                       17                                  /**< Shift value for SYSCFG_RAMERR2B             */
#define _SYSCFG_IF_RAMERR2B_MASK                        0x20000UL                           /**< Bit mask for SYSCFG_RAMERR2B                */
#define _SYSCFG_IF_RAMERR2B_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_RAMERR2B_DEFAULT                      (_SYSCFG_IF_RAMERR2B_DEFAULT << 17) /**< Shifted mode DEFAULT for SYSCFG_IF          */

/* Bit fields for SYSCFG IEN */
#define _SYSCFG_IEN_RESETVALUE                          0x00000000UL                         /**< Default value for SYSCFG_IEN                */
#define _SYSCFG_IEN_MASK                                0x3303000FUL                         /**< Mask for SYSCFG_IEN                         */
#define SYSCFG_IEN_SW0                                  (0x1UL << 0)                         /**< Software interrupt 0                        */
#define _SYSCFG_IEN_SW0_SHIFT                           0                                    /**< Shift value for SYSCFG_SW0                  */
#define _SYSCFG_IEN_SW0_MASK                            0x1UL                                /**< Bit mask for SYSCFG_SW0                     */
#define _SYSCFG_IEN_SW0_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW0_DEFAULT                          (_SYSCFG_IEN_SW0_DEFAULT << 0)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SW1                                  (0x1UL << 1)                         /**< Software interrupt 1                        */
#define _SYSCFG_IEN_SW1_SHIFT                           1                                    /**< Shift value for SYSCFG_SW1                  */
#define _SYSCFG_IEN_SW1_MASK                            0x2UL                                /**< Bit mask for SYSCFG_SW1                     */
#define _SYSCFG_IEN_SW1_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW1_DEFAULT                          (_SYSCFG_IEN_SW1_DEFAULT << 1)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SW2                                  (0x1UL << 2)                         /**< Software interrupt 2                        */
#define _SYSCFG_IEN_SW2_SHIFT                           2                                    /**< Shift value for SYSCFG_SW2                  */
#define _SYSCFG_IEN_SW2_MASK                            0x4UL                                /**< Bit mask for SYSCFG_SW2                     */
#define _SYSCFG_IEN_SW2_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW2_DEFAULT                          (_SYSCFG_IEN_SW2_DEFAULT << 2)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SW3                                  (0x1UL << 3)                         /**< Software interrupt 3                        */
#define _SYSCFG_IEN_SW3_SHIFT                           3                                    /**< Shift value for SYSCFG_SW3                  */
#define _SYSCFG_IEN_SW3_MASK                            0x8UL                                /**< Bit mask for SYSCFG_SW3                     */
#define _SYSCFG_IEN_SW3_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW3_DEFAULT                          (_SYSCFG_IEN_SW3_DEFAULT << 3)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_RAMERR1B                             (0x1UL << 16)                        /**< RAM 1-bit Error Interrupt Enable            */
#define _SYSCFG_IEN_RAMERR1B_SHIFT                      16                                   /**< Shift value for SYSCFG_RAMERR1B             */
#define _SYSCFG_IEN_RAMERR1B_MASK                       0x10000UL                            /**< Bit mask for SYSCFG_RAMERR1B                */
#define _SYSCFG_IEN_RAMERR1B_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_RAMERR1B_DEFAULT                     (_SYSCFG_IEN_RAMERR1B_DEFAULT << 16) /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_RAMERR2B                             (0x1UL << 17)                        /**< RAM 2-bit Error Interrupt Enable            */
#define _SYSCFG_IEN_RAMERR2B_SHIFT                      17                                   /**< Shift value for SYSCFG_RAMERR2B             */
#define _SYSCFG_IEN_RAMERR2B_MASK                       0x20000UL                            /**< Bit mask for SYSCFG_RAMERR2B                */
#define _SYSCFG_IEN_RAMERR2B_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_RAMERR2B_DEFAULT                     (_SYSCFG_IEN_RAMERR2B_DEFAULT << 17) /**< Shifted mode DEFAULT for SYSCFG_IEN         */

/* Bit fields for SYSCFG CHIPREVHW */
#define _SYSCFG_CHIPREVHW_RESETVALUE                    0x00000C01UL                            /**< Default value for SYSCFG_CHIPREVHW          */
#define _SYSCFG_CHIPREVHW_MASK                          0xFF0FFFFFUL                            /**< Mask for SYSCFG_CHIPREVHW                   */
#define _SYSCFG_CHIPREVHW_MAJOR_SHIFT                   0                                       /**< Shift value for SYSCFG_MAJOR                */
#define _SYSCFG_CHIPREVHW_MAJOR_MASK                    0x3FUL                                  /**< Bit mask for SYSCFG_MAJOR                   */
#define _SYSCFG_CHIPREVHW_MAJOR_DEFAULT                 0x00000001UL                            /**< Mode DEFAULT for SYSCFG_CHIPREVHW           */
#define SYSCFG_CHIPREVHW_MAJOR_DEFAULT                  (_SYSCFG_CHIPREVHW_MAJOR_DEFAULT << 0)  /**< Shifted mode DEFAULT for SYSCFG_CHIPREVHW   */
#define _SYSCFG_CHIPREVHW_FAMILY_SHIFT                  6                                       /**< Shift value for SYSCFG_FAMILY               */
#define _SYSCFG_CHIPREVHW_FAMILY_MASK                   0xFC0UL                                 /**< Bit mask for SYSCFG_FAMILY                  */
#define _SYSCFG_CHIPREVHW_FAMILY_DEFAULT                0x00000030UL                            /**< Mode DEFAULT for SYSCFG_CHIPREVHW           */
#define SYSCFG_CHIPREVHW_FAMILY_DEFAULT                 (_SYSCFG_CHIPREVHW_FAMILY_DEFAULT << 6) /**< Shifted mode DEFAULT for SYSCFG_CHIPREVHW   */
#define _SYSCFG_CHIPREVHW_MINOR_SHIFT                   12                                      /**< Shift value for SYSCFG_MINOR                */
#define _SYSCFG_CHIPREVHW_MINOR_MASK                    0xFF000UL                               /**< Bit mask for SYSCFG_MINOR                   */
#define _SYSCFG_CHIPREVHW_MINOR_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for SYSCFG_CHIPREVHW           */
#define SYSCFG_CHIPREVHW_MINOR_DEFAULT                  (_SYSCFG_CHIPREVHW_MINOR_DEFAULT << 12) /**< Shifted mode DEFAULT for SYSCFG_CHIPREVHW   */

/* Bit fields for SYSCFG CHIPREV */
#define _SYSCFG_CHIPREV_RESETVALUE                      0x00000000UL                          /**< Default value for SYSCFG_CHIPREV            */
#define _SYSCFG_CHIPREV_MASK                            0x000FFFFFUL                          /**< Mask for SYSCFG_CHIPREV                     */
#define _SYSCFG_CHIPREV_MAJOR_SHIFT                     0                                     /**< Shift value for SYSCFG_MAJOR                */
#define _SYSCFG_CHIPREV_MAJOR_MASK                      0x3FUL                                /**< Bit mask for SYSCFG_MAJOR                   */
#define _SYSCFG_CHIPREV_MAJOR_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for SYSCFG_CHIPREV             */
#define SYSCFG_CHIPREV_MAJOR_DEFAULT                    (_SYSCFG_CHIPREV_MAJOR_DEFAULT << 0)  /**< Shifted mode DEFAULT for SYSCFG_CHIPREV     */
#define _SYSCFG_CHIPREV_FAMILY_SHIFT                    6                                     /**< Shift value for SYSCFG_FAMILY               */
#define _SYSCFG_CHIPREV_FAMILY_MASK                     0xFC0UL                               /**< Bit mask for SYSCFG_FAMILY                  */
#define _SYSCFG_CHIPREV_FAMILY_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for SYSCFG_CHIPREV             */
#define SYSCFG_CHIPREV_FAMILY_DEFAULT                   (_SYSCFG_CHIPREV_FAMILY_DEFAULT << 6) /**< Shifted mode DEFAULT for SYSCFG_CHIPREV     */
#define _SYSCFG_CHIPREV_MINOR_SHIFT                     12                                    /**< Shift value for SYSCFG_MINOR                */
#define _SYSCFG_CHIPREV_MINOR_MASK                      0xFF000UL                             /**< Bit mask for SYSCFG_MINOR                   */
#define _SYSCFG_CHIPREV_MINOR_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for SYSCFG_CHIPREV             */
#define SYSCFG_CHIPREV_MINOR_DEFAULT                    (_SYSCFG_CHIPREV_MINOR_DEFAULT << 12) /**< Shifted mode DEFAULT for SYSCFG_CHIPREV     */

/* Bit fields for SYSCFG CFGSYSTIC */
#define _SYSCFG_CFGSYSTIC_RESETVALUE                    0x00000000UL                                    /**< Default value for SYSCFG_CFGSYSTIC          */
#define _SYSCFG_CFGSYSTIC_MASK                          0x00000001UL                                    /**< Mask for SYSCFG_CFGSYSTIC                   */
#define SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN                 (0x1UL << 0)                                    /**< SysTick External Clock Enable               */
#define _SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_SHIFT          0                                               /**< Shift value for SYSCFG_SYSTICEXTCLKEN       */
#define _SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_MASK           0x1UL                                           /**< Bit mask for SYSCFG_SYSTICEXTCLKEN          */
#define _SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_DEFAULT        0x00000000UL                                    /**< Mode DEFAULT for SYSCFG_CFGSYSTIC           */
#define SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_DEFAULT         (_SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_CFGSYSTIC   */

/* Bit fields for SYSCFG CTRL */
#define _SYSCFG_CTRL_RESETVALUE                         0x00000021UL                                 /**< Default value for SYSCFG_CTRL               */
#define _SYSCFG_CTRL_MASK                               0x00000021UL                                 /**< Mask for SYSCFG_CTRL                        */
#define SYSCFG_CTRL_ADDRFAULTEN                         (0x1UL << 0)                                 /**< Invalid Address Bus Fault Response Enable   */
#define _SYSCFG_CTRL_ADDRFAULTEN_SHIFT                  0                                            /**< Shift value for SYSCFG_ADDRFAULTEN          */
#define _SYSCFG_CTRL_ADDRFAULTEN_MASK                   0x1UL                                        /**< Bit mask for SYSCFG_ADDRFAULTEN             */
#define _SYSCFG_CTRL_ADDRFAULTEN_DEFAULT                0x00000001UL                                 /**< Mode DEFAULT for SYSCFG_CTRL                */
#define SYSCFG_CTRL_ADDRFAULTEN_DEFAULT                 (_SYSCFG_CTRL_ADDRFAULTEN_DEFAULT << 0)      /**< Shifted mode DEFAULT for SYSCFG_CTRL        */
#define SYSCFG_CTRL_RAMECCERRFAULTEN                    (0x1UL << 5)                                 /**< Two bit ECC Error Bus Fault Response Enable */
#define _SYSCFG_CTRL_RAMECCERRFAULTEN_SHIFT             5                                            /**< Shift value for SYSCFG_RAMECCERRFAULTEN     */
#define _SYSCFG_CTRL_RAMECCERRFAULTEN_MASK              0x20UL                                       /**< Bit mask for SYSCFG_RAMECCERRFAULTEN        */
#define _SYSCFG_CTRL_RAMECCERRFAULTEN_DEFAULT           0x00000001UL                                 /**< Mode DEFAULT for SYSCFG_CTRL                */
#define SYSCFG_CTRL_RAMECCERRFAULTEN_DEFAULT            (_SYSCFG_CTRL_RAMECCERRFAULTEN_DEFAULT << 5) /**< Shifted mode DEFAULT for SYSCFG_CTRL        */

/* Bit fields for SYSCFG DMEM0RETNCTRL */
#define _SYSCFG_DMEM0RETNCTRL_RESETVALUE                0x00000000UL                                     /**< Default value for SYSCFG_DMEM0RETNCTRL      */
#define _SYSCFG_DMEM0RETNCTRL_MASK                      0x00000003UL                                     /**< Mask for SYSCFG_DMEM0RETNCTRL               */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_SHIFT         0                                                /**< Shift value for SYSCFG_RAMRETNCTRL          */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_MASK          0x3UL                                            /**< Bit mask for SYSCFG_RAMRETNCTRL             */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_DEFAULT       0x00000000UL                                     /**< Mode DEFAULT for SYSCFG_DMEM0RETNCTRL       */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_ALLON         0x00000000UL                                     /**< Mode ALLON for SYSCFG_DMEM0RETNCTRL         */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK0          0x00000001UL                                     /**< Mode BLK0 for SYSCFG_DMEM0RETNCTRL          */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK1          0x00000002UL                                     /**< Mode BLK1 for SYSCFG_DMEM0RETNCTRL          */
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_DEFAULT        (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_DMEM0RETNCTRL*/
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_ALLON          (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_ALLON << 0)   /**< Shifted mode ALLON for SYSCFG_DMEM0RETNCTRL */
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK0           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK0 << 0)    /**< Shifted mode BLK0 for SYSCFG_DMEM0RETNCTRL  */
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK1           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK1 << 0)    /**< Shifted mode BLK1 for SYSCFG_DMEM0RETNCTRL  */

/* Bit fields for SYSCFG DMEM0ECCADDR */
#define _SYSCFG_DMEM0ECCADDR_RESETVALUE                 0x00000000UL                                     /**< Default value for SYSCFG_DMEM0ECCADDR       */
#define _SYSCFG_DMEM0ECCADDR_MASK                       0xFFFFFFFFUL                                     /**< Mask for SYSCFG_DMEM0ECCADDR                */
#define _SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_SHIFT         0                                                /**< Shift value for SYSCFG_DMEM0ECCADDR         */
#define _SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_MASK          0xFFFFFFFFUL                                     /**< Bit mask for SYSCFG_DMEM0ECCADDR            */
#define _SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_DEFAULT       0x00000000UL                                     /**< Mode DEFAULT for SYSCFG_DMEM0ECCADDR        */
#define SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_DEFAULT        (_SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_DMEM0ECCADDR*/

/* Bit fields for SYSCFG DMEM0ECCCTRL */
#define _SYSCFG_DMEM0ECCCTRL_RESETVALUE                 0x00000000UL                                   /**< Default value for SYSCFG_DMEM0ECCCTRL       */
#define _SYSCFG_DMEM0ECCCTRL_MASK                       0x00000003UL                                   /**< Mask for SYSCFG_DMEM0ECCCTRL                */
#define SYSCFG_DMEM0ECCCTRL_RAMECCEN                    (0x1UL << 0)                                   /**< RAM ECC Enable                              */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCEN_SHIFT             0                                              /**< Shift value for SYSCFG_RAMECCEN             */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCEN_MASK              0x1UL                                          /**< Bit mask for SYSCFG_RAMECCEN                */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCEN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SYSCFG_DMEM0ECCCTRL        */
#define SYSCFG_DMEM0ECCCTRL_RAMECCEN_DEFAULT            (_SYSCFG_DMEM0ECCCTRL_RAMECCEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for SYSCFG_DMEM0ECCCTRL*/
#define SYSCFG_DMEM0ECCCTRL_RAMECCEWEN                  (0x1UL << 1)                                   /**< RAM ECC Error Writeback Enable              */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_SHIFT           1                                              /**< Shift value for SYSCFG_RAMECCEWEN           */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_MASK            0x2UL                                          /**< Bit mask for SYSCFG_RAMECCEWEN              */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_DEFAULT         0x00000000UL                                   /**< Mode DEFAULT for SYSCFG_DMEM0ECCCTRL        */
#define SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_DEFAULT          (_SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_DEFAULT << 1) /**< Shifted mode DEFAULT for SYSCFG_DMEM0ECCCTRL*/

/* Bit fields for SYSCFG ROOTDATA0 */
#define _SYSCFG_ROOTDATA0_RESETVALUE                    0x00000000UL                          /**< Default value for SYSCFG_ROOTDATA0          */
#define _SYSCFG_ROOTDATA0_MASK                          0xFFFFFFFFUL                          /**< Mask for SYSCFG_ROOTDATA0                   */
#define _SYSCFG_ROOTDATA0_DATA_SHIFT                    0                                     /**< Shift value for SYSCFG_DATA                 */
#define _SYSCFG_ROOTDATA0_DATA_MASK                     0xFFFFFFFFUL                          /**< Bit mask for SYSCFG_DATA                    */
#define _SYSCFG_ROOTDATA0_DATA_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for SYSCFG_ROOTDATA0           */
#define SYSCFG_ROOTDATA0_DATA_DEFAULT                   (_SYSCFG_ROOTDATA0_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_ROOTDATA0   */

/* Bit fields for SYSCFG ROOTDATA1 */
#define _SYSCFG_ROOTDATA1_RESETVALUE                    0x00000000UL                          /**< Default value for SYSCFG_ROOTDATA1          */
#define _SYSCFG_ROOTDATA1_MASK                          0xFFFFFFFFUL                          /**< Mask for SYSCFG_ROOTDATA1                   */
#define _SYSCFG_ROOTDATA1_DATA_SHIFT                    0                                     /**< Shift value for SYSCFG_DATA                 */
#define _SYSCFG_ROOTDATA1_DATA_MASK                     0xFFFFFFFFUL                          /**< Bit mask for SYSCFG_DATA                    */
#define _SYSCFG_ROOTDATA1_DATA_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for SYSCFG_ROOTDATA1           */
#define SYSCFG_ROOTDATA1_DATA_DEFAULT                   (_SYSCFG_ROOTDATA1_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_ROOTDATA1   */

/* Bit fields for SYSCFG ROOTLOCKSTATUS */
#define _SYSCFG_ROOTLOCKSTATUS_RESETVALUE               0x011F0107UL                                         /**< Default value for SYSCFG_ROOTLOCKSTATUS     */
#define _SYSCFG_ROOTLOCKSTATUS_MASK                     0x011F0117UL                                         /**< Mask for SYSCFG_ROOTLOCKSTATUS              */
#define SYSCFG_ROOTLOCKSTATUS_BUSLOCK                   (0x1UL << 0)                                         /**< Bus Lock                                    */
#define _SYSCFG_ROOTLOCKSTATUS_BUSLOCK_SHIFT            0                                                    /**< Shift value for SYSCFG_BUSLOCK              */
#define _SYSCFG_ROOTLOCKSTATUS_BUSLOCK_MASK             0x1UL                                                /**< Bit mask for SYSCFG_BUSLOCK                 */
#define _SYSCFG_ROOTLOCKSTATUS_BUSLOCK_DEFAULT          0x00000001UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_BUSLOCK_DEFAULT           (_SYSCFG_ROOTLOCKSTATUS_BUSLOCK_DEFAULT << 0)        /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_REGLOCK                   (0x1UL << 1)                                         /**< Register Lock                               */
#define _SYSCFG_ROOTLOCKSTATUS_REGLOCK_SHIFT            1                                                    /**< Shift value for SYSCFG_REGLOCK              */
#define _SYSCFG_ROOTLOCKSTATUS_REGLOCK_MASK             0x2UL                                                /**< Bit mask for SYSCFG_REGLOCK                 */
#define _SYSCFG_ROOTLOCKSTATUS_REGLOCK_DEFAULT          0x00000001UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_REGLOCK_DEFAULT           (_SYSCFG_ROOTLOCKSTATUS_REGLOCK_DEFAULT << 1)        /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_MFRLOCK                   (0x1UL << 2)                                         /**< Manufacture Lock                            */
#define _SYSCFG_ROOTLOCKSTATUS_MFRLOCK_SHIFT            2                                                    /**< Shift value for SYSCFG_MFRLOCK              */
#define _SYSCFG_ROOTLOCKSTATUS_MFRLOCK_MASK             0x4UL                                                /**< Bit mask for SYSCFG_MFRLOCK                 */
#define _SYSCFG_ROOTLOCKSTATUS_MFRLOCK_DEFAULT          0x00000001UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_MFRLOCK_DEFAULT           (_SYSCFG_ROOTLOCKSTATUS_MFRLOCK_DEFAULT << 2)        /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_ROOTMODELOCK              (0x1UL << 4)                                         /**< Root Mode Lock                              */
#define _SYSCFG_ROOTLOCKSTATUS_ROOTMODELOCK_SHIFT       4                                                    /**< Shift value for SYSCFG_ROOTMODELOCK         */
#define _SYSCFG_ROOTLOCKSTATUS_ROOTMODELOCK_MASK        0x10UL                                               /**< Bit mask for SYSCFG_ROOTMODELOCK            */
#define _SYSCFG_ROOTLOCKSTATUS_ROOTMODELOCK_DEFAULT     0x00000000UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_ROOTMODELOCK_DEFAULT      (_SYSCFG_ROOTLOCKSTATUS_ROOTMODELOCK_DEFAULT << 4)   /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK               (0x1UL << 8)                                         /**< Root Debug Lock                             */
#define _SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_SHIFT        8                                                    /**< Shift value for SYSCFG_ROOTDBGLOCK          */
#define _SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_MASK         0x100UL                                              /**< Bit mask for SYSCFG_ROOTDBGLOCK             */
#define _SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_DEFAULT      0x00000001UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_DEFAULT       (_SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_DEFAULT << 8)    /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK               (0x1UL << 16)                                        /**< User Invasive Debug Lock                    */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_SHIFT        16                                                   /**< Shift value for SYSCFG_USERDBGLOCK          */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_MASK         0x10000UL                                            /**< Bit mask for SYSCFG_USERDBGLOCK             */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_DEFAULT      0x00000001UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_DEFAULT       (_SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_DEFAULT << 16)   /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK               (0x1UL << 17)                                        /**< User Non-invasive Debug Lock                */
#define _SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_SHIFT        17                                                   /**< Shift value for SYSCFG_USERNIDLOCK          */
#define _SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_MASK         0x20000UL                                            /**< Bit mask for SYSCFG_USERNIDLOCK             */
#define _SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_DEFAULT      0x00000001UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_DEFAULT       (_SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_DEFAULT << 17)   /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK              (0x1UL << 18)                                        /**< User Secure Invasive Debug Lock             */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_SHIFT       18                                                   /**< Shift value for SYSCFG_USERSPIDLOCK         */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_MASK        0x40000UL                                            /**< Bit mask for SYSCFG_USERSPIDLOCK            */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_DEFAULT     0x00000001UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_DEFAULT      (_SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_DEFAULT << 18)  /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK             (0x1UL << 19)                                        /**< User Secure Non-invasive Debug Lock         */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_SHIFT      19                                                   /**< Shift value for SYSCFG_USERSPNIDLOCK        */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_MASK       0x80000UL                                            /**< Bit mask for SYSCFG_USERSPNIDLOCK           */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_DEFAULT    0x00000001UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_DEFAULT     (_SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_DEFAULT << 19) /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK             (0x1UL << 20)                                        /**< User Debug Access Port Lock                 */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_SHIFT      20                                                   /**< Shift value for SYSCFG_USERDBGAPLOCK        */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_MASK       0x100000UL                                           /**< Bit mask for SYSCFG_USERDBGAPLOCK           */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_DEFAULT    0x00000001UL                                         /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_DEFAULT     (_SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_DEFAULT << 20) /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/

/** @} End of group EFM32PG22_SYSCFG_BitFields */
/** @} End of group EFM32PG22_SYSCFG */
/**************************************************************************//**
 * @defgroup EFM32PG22_SYSCFG_CFGNS SYSCFG_CFGNS
 * @{
 * @brief EFM32PG22 SYSCFG_CFGNS Register Declaration.
 *****************************************************************************/

/** SYSCFG_CFGNS Register Declaration. */
typedef struct {
  uint32_t       RESERVED0[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFGNSTCALIB;                   /**< Configure Non-secure Sys-Tick Cal.                 */
  uint32_t       RESERVED1[376U];               /**< Reserved for future use                            */
  __IOM uint32_t ROOTNSDATA0;                   /**< Data Register 0                                    */
  __IOM uint32_t ROOTNSDATA1;                   /**< Data Register 1                                    */
  uint32_t       RESERVED2[638U];               /**< Reserved for future use                            */
  uint32_t       RESERVED3[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFGNSTCALIB_SET;               /**< Configure Non-secure Sys-Tick Cal.                 */
  uint32_t       RESERVED4[376U];               /**< Reserved for future use                            */
  __IOM uint32_t ROOTNSDATA0_SET;               /**< Data Register 0                                    */
  __IOM uint32_t ROOTNSDATA1_SET;               /**< Data Register 1                                    */
  uint32_t       RESERVED5[638U];               /**< Reserved for future use                            */
  uint32_t       RESERVED6[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFGNSTCALIB_CLR;               /**< Configure Non-secure Sys-Tick Cal.                 */
  uint32_t       RESERVED7[376U];               /**< Reserved for future use                            */
  __IOM uint32_t ROOTNSDATA0_CLR;               /**< Data Register 0                                    */
  __IOM uint32_t ROOTNSDATA1_CLR;               /**< Data Register 1                                    */
  uint32_t       RESERVED8[638U];               /**< Reserved for future use                            */
  uint32_t       RESERVED9[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFGNSTCALIB_TGL;               /**< Configure Non-secure Sys-Tick Cal.                 */
  uint32_t       RESERVED10[376U];              /**< Reserved for future use                            */
  __IOM uint32_t ROOTNSDATA0_TGL;               /**< Data Register 0                                    */
  __IOM uint32_t ROOTNSDATA1_TGL;               /**< Data Register 1                                    */
} SYSCFG_CFGNS_TypeDef;
/** @} End of group EFM32PG22_SYSCFG_CFGNS */

/**************************************************************************//**
 * @addtogroup EFM32PG22_SYSCFG_CFGNS
 * @{
 * @defgroup EFM32PG22_SYSCFG_CFGNS_BitFields SYSCFG_CFGNS Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SYSCFG CFGNSTCALIB */
#define _SYSCFG_CFGNSTCALIB_RESETVALUE       0x01004A37UL                               /**< Default value for SYSCFG_CFGNSTCALIB        */
#define _SYSCFG_CFGNSTCALIB_MASK             0x03FFFFFFUL                               /**< Mask for SYSCFG_CFGNSTCALIB                 */
#define _SYSCFG_CFGNSTCALIB_TENMS_SHIFT      0                                          /**< Shift value for SYSCFG_TENMS                */
#define _SYSCFG_CFGNSTCALIB_TENMS_MASK       0xFFFFFFUL                                 /**< Bit mask for SYSCFG_TENMS                   */
#define _SYSCFG_CFGNSTCALIB_TENMS_DEFAULT    0x00004A37UL                               /**< Mode DEFAULT for SYSCFG_CFGNSTCALIB         */
#define SYSCFG_CFGNSTCALIB_TENMS_DEFAULT     (_SYSCFG_CFGNSTCALIB_TENMS_DEFAULT << 0)   /**< Shifted mode DEFAULT for SYSCFG_CFGNSTCALIB */
#define SYSCFG_CFGNSTCALIB_SKEW              (0x1UL << 24)                              /**< Skew                                        */
#define _SYSCFG_CFGNSTCALIB_SKEW_SHIFT       24                                         /**< Shift value for SYSCFG_SKEW                 */
#define _SYSCFG_CFGNSTCALIB_SKEW_MASK        0x1000000UL                                /**< Bit mask for SYSCFG_SKEW                    */
#define _SYSCFG_CFGNSTCALIB_SKEW_DEFAULT     0x00000001UL                               /**< Mode DEFAULT for SYSCFG_CFGNSTCALIB         */
#define SYSCFG_CFGNSTCALIB_SKEW_DEFAULT      (_SYSCFG_CFGNSTCALIB_SKEW_DEFAULT << 24)   /**< Shifted mode DEFAULT for SYSCFG_CFGNSTCALIB */
#define SYSCFG_CFGNSTCALIB_NOREF             (0x1UL << 25)                              /**< No Reference                                */
#define _SYSCFG_CFGNSTCALIB_NOREF_SHIFT      25                                         /**< Shift value for SYSCFG_NOREF                */
#define _SYSCFG_CFGNSTCALIB_NOREF_MASK       0x2000000UL                                /**< Bit mask for SYSCFG_NOREF                   */
#define _SYSCFG_CFGNSTCALIB_NOREF_DEFAULT    0x00000000UL                               /**< Mode DEFAULT for SYSCFG_CFGNSTCALIB         */
#define _SYSCFG_CFGNSTCALIB_NOREF_REF        0x00000000UL                               /**< Mode REF for SYSCFG_CFGNSTCALIB             */
#define _SYSCFG_CFGNSTCALIB_NOREF_NOREF      0x00000001UL                               /**< Mode NOREF for SYSCFG_CFGNSTCALIB           */
#define SYSCFG_CFGNSTCALIB_NOREF_DEFAULT     (_SYSCFG_CFGNSTCALIB_NOREF_DEFAULT << 25)  /**< Shifted mode DEFAULT for SYSCFG_CFGNSTCALIB */
#define SYSCFG_CFGNSTCALIB_NOREF_REF         (_SYSCFG_CFGNSTCALIB_NOREF_REF << 25)      /**< Shifted mode REF for SYSCFG_CFGNSTCALIB     */
#define SYSCFG_CFGNSTCALIB_NOREF_NOREF       (_SYSCFG_CFGNSTCALIB_NOREF_NOREF << 25)    /**< Shifted mode NOREF for SYSCFG_CFGNSTCALIB   */

/* Bit fields for SYSCFG ROOTNSDATA0 */
#define _SYSCFG_ROOTNSDATA0_RESETVALUE       0x00000000UL                               /**< Default value for SYSCFG_ROOTNSDATA0        */
#define _SYSCFG_ROOTNSDATA0_MASK             0xFFFFFFFFUL                               /**< Mask for SYSCFG_ROOTNSDATA0                 */
#define _SYSCFG_ROOTNSDATA0_DATA_SHIFT       0                                          /**< Shift value for SYSCFG_DATA                 */
#define _SYSCFG_ROOTNSDATA0_DATA_MASK        0xFFFFFFFFUL                               /**< Bit mask for SYSCFG_DATA                    */
#define _SYSCFG_ROOTNSDATA0_DATA_DEFAULT     0x00000000UL                               /**< Mode DEFAULT for SYSCFG_ROOTNSDATA0         */
#define SYSCFG_ROOTNSDATA0_DATA_DEFAULT      (_SYSCFG_ROOTNSDATA0_DATA_DEFAULT << 0)    /**< Shifted mode DEFAULT for SYSCFG_ROOTNSDATA0 */

/* Bit fields for SYSCFG ROOTNSDATA1 */
#define _SYSCFG_ROOTNSDATA1_RESETVALUE       0x00000000UL                               /**< Default value for SYSCFG_ROOTNSDATA1        */
#define _SYSCFG_ROOTNSDATA1_MASK             0xFFFFFFFFUL                               /**< Mask for SYSCFG_ROOTNSDATA1                 */
#define _SYSCFG_ROOTNSDATA1_DATA_SHIFT       0                                          /**< Shift value for SYSCFG_DATA                 */
#define _SYSCFG_ROOTNSDATA1_DATA_MASK        0xFFFFFFFFUL                               /**< Bit mask for SYSCFG_DATA                    */
#define _SYSCFG_ROOTNSDATA1_DATA_DEFAULT     0x00000000UL                               /**< Mode DEFAULT for SYSCFG_ROOTNSDATA1         */
#define SYSCFG_ROOTNSDATA1_DATA_DEFAULT      (_SYSCFG_ROOTNSDATA1_DATA_DEFAULT << 0)    /**< Shifted mode DEFAULT for SYSCFG_ROOTNSDATA1 */

/** @} End of group EFM32PG22_SYSCFG_CFGNS_BitFields */
/** @} End of group EFM32PG22_SYSCFG_CFGNS */
/** @} End of group Parts */

#endif /* EFM32PG22_SYSCFG_H */
