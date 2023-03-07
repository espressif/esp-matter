/**************************************************************************//**
 * @file
 * @brief EFR32FG25 RFFPLL register and bit field definitions
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
#ifndef EFR32FG25_RFFPLL_H
#define EFR32FG25_RFFPLL_H
#define RFFPLL_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32FG25_RFFPLL RFFPLL
 * @{
 * @brief EFR32FG25 RFFPLL Register Declaration.
 *****************************************************************************/

/** RFFPLL Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  uint32_t       RESERVED0[4U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  uint32_t       RESERVED1[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  uint32_t       RESERVED2[13U];                /**< Reserved for future use                            */
  __IOM uint32_t RFFPLLCTRL1;                   /**< PLL Control Register 1                             */
  uint32_t       RESERVED3[9U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED4[10U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  uint32_t       RESERVED5[979U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  uint32_t       RESERVED6[4U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  uint32_t       RESERVED7[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  uint32_t       RESERVED8[13U];                /**< Reserved for future use                            */
  __IOM uint32_t RFFPLLCTRL1_SET;               /**< PLL Control Register 1                             */
  uint32_t       RESERVED9[9U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED10[10U];               /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED11[979U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  uint32_t       RESERVED12[4U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  uint32_t       RESERVED13[2U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  uint32_t       RESERVED14[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RFFPLLCTRL1_CLR;               /**< PLL Control Register 1                             */
  uint32_t       RESERVED15[9U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED16[10U];               /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED17[979U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  uint32_t       RESERVED18[4U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  uint32_t       RESERVED19[2U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  uint32_t       RESERVED20[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RFFPLLCTRL1_TGL;               /**< PLL Control Register 1                             */
  uint32_t       RESERVED21[9U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED22[10U];               /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
} RFFPLL_TypeDef;
/** @} End of group EFR32FG25_RFFPLL */

/**************************************************************************//**
 * @addtogroup EFR32FG25_RFFPLL
 * @{
 * @defgroup EFR32FG25_RFFPLL_BitFields RFFPLL Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for RFFPLL IPVERSION */
#define _RFFPLL_IPVERSION_RESETVALUE             0x00000001UL                               /**< Default value for RFFPLL_IPVERSION          */
#define _RFFPLL_IPVERSION_MASK                   0xFFFFFFFFUL                               /**< Mask for RFFPLL_IPVERSION                   */
#define _RFFPLL_IPVERSION_IPVERSION_SHIFT        0                                          /**< Shift value for RFFPLL_IPVERSION            */
#define _RFFPLL_IPVERSION_IPVERSION_MASK         0xFFFFFFFFUL                               /**< Bit mask for RFFPLL_IPVERSION               */
#define _RFFPLL_IPVERSION_IPVERSION_DEFAULT      0x00000001UL                               /**< Mode DEFAULT for RFFPLL_IPVERSION           */
#define RFFPLL_IPVERSION_IPVERSION_DEFAULT       (_RFFPLL_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for RFFPLL_IPVERSION   */

/* Bit fields for RFFPLL CTRL */
#define _RFFPLL_CTRL_RESETVALUE                  0x00000000UL                            /**< Default value for RFFPLL_CTRL               */
#define _RFFPLL_CTRL_MASK                        0x80000103UL                            /**< Mask for RFFPLL_CTRL                        */
#define RFFPLL_CTRL_FORCEEN                      (0x1UL << 1)                            /**< Force PLL Enable                            */
#define _RFFPLL_CTRL_FORCEEN_SHIFT               1                                       /**< Shift value for RFFPLL_FORCEEN              */
#define _RFFPLL_CTRL_FORCEEN_MASK                0x2UL                                   /**< Bit mask for RFFPLL_FORCEEN                 */
#define _RFFPLL_CTRL_FORCEEN_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for RFFPLL_CTRL                */
#define RFFPLL_CTRL_FORCEEN_DEFAULT              (_RFFPLL_CTRL_FORCEEN_DEFAULT << 1)     /**< Shifted mode DEFAULT for RFFPLL_CTRL        */
#define RFFPLL_CTRL_DISONDEMAND                  (0x1UL << 8)                            /**< Disable On-demand request                   */
#define _RFFPLL_CTRL_DISONDEMAND_SHIFT           8                                       /**< Shift value for RFFPLL_DISONDEMAND          */
#define _RFFPLL_CTRL_DISONDEMAND_MASK            0x100UL                                 /**< Bit mask for RFFPLL_DISONDEMAND             */
#define _RFFPLL_CTRL_DISONDEMAND_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for RFFPLL_CTRL                */
#define RFFPLL_CTRL_DISONDEMAND_DEFAULT          (_RFFPLL_CTRL_DISONDEMAND_DEFAULT << 8) /**< Shifted mode DEFAULT for RFFPLL_CTRL        */

/* Bit fields for RFFPLL STATUS */
#define _RFFPLL_STATUS_RESETVALUE                0x00000000UL                                 /**< Default value for RFFPLL_STATUS             */
#define _RFFPLL_STATUS_MASK                      0x800F0113UL                                 /**< Mask for RFFPLL_STATUS                      */
#define RFFPLL_STATUS_RFFPLLRADIORDY             (0x1UL << 0)                                 /**< Radio Clock Output Ready                    */
#define _RFFPLL_STATUS_RFFPLLRADIORDY_SHIFT      0                                            /**< Shift value for RFFPLL_RFFPLLRADIORDY       */
#define _RFFPLL_STATUS_RFFPLLRADIORDY_MASK       0x1UL                                        /**< Bit mask for RFFPLL_RFFPLLRADIORDY          */
#define _RFFPLL_STATUS_RFFPLLRADIORDY_DEFAULT    0x00000000UL                                 /**< Mode DEFAULT for RFFPLL_STATUS              */
#define RFFPLL_STATUS_RFFPLLRADIORDY_DEFAULT     (_RFFPLL_STATUS_RFFPLLRADIORDY_DEFAULT << 0) /**< Shifted mode DEFAULT for RFFPLL_STATUS      */
#define RFFPLL_STATUS_RFFPLLSYSRDY               (0x1UL << 1)                                 /**< Digital System Clock Output Ready           */
#define _RFFPLL_STATUS_RFFPLLSYSRDY_SHIFT        1                                            /**< Shift value for RFFPLL_RFFPLLSYSRDY         */
#define _RFFPLL_STATUS_RFFPLLSYSRDY_MASK         0x2UL                                        /**< Bit mask for RFFPLL_RFFPLLSYSRDY            */
#define _RFFPLL_STATUS_RFFPLLSYSRDY_DEFAULT      0x00000000UL                                 /**< Mode DEFAULT for RFFPLL_STATUS              */
#define RFFPLL_STATUS_RFFPLLSYSRDY_DEFAULT       (_RFFPLL_STATUS_RFFPLLSYSRDY_DEFAULT << 1)   /**< Shifted mode DEFAULT for RFFPLL_STATUS      */
#define RFFPLL_STATUS_ENS                        (0x1UL << 8)                                 /**< Enabled Status                              */
#define _RFFPLL_STATUS_ENS_SHIFT                 8                                            /**< Shift value for RFFPLL_ENS                  */
#define _RFFPLL_STATUS_ENS_MASK                  0x100UL                                      /**< Bit mask for RFFPLL_ENS                     */
#define _RFFPLL_STATUS_ENS_DEFAULT               0x00000000UL                                 /**< Mode DEFAULT for RFFPLL_STATUS              */
#define RFFPLL_STATUS_ENS_DEFAULT                (_RFFPLL_STATUS_ENS_DEFAULT << 8)            /**< Shifted mode DEFAULT for RFFPLL_STATUS      */
#define RFFPLL_STATUS_HWREQRADIO                 (0x1UL << 16)                                /**< PLL Radio Output Requested                  */
#define _RFFPLL_STATUS_HWREQRADIO_SHIFT          16                                           /**< Shift value for RFFPLL_HWREQRADIO           */
#define _RFFPLL_STATUS_HWREQRADIO_MASK           0x10000UL                                    /**< Bit mask for RFFPLL_HWREQRADIO              */
#define _RFFPLL_STATUS_HWREQRADIO_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for RFFPLL_STATUS              */
#define RFFPLL_STATUS_HWREQRADIO_DEFAULT         (_RFFPLL_STATUS_HWREQRADIO_DEFAULT << 16)    /**< Shifted mode DEFAULT for RFFPLL_STATUS      */
#define RFFPLL_STATUS_HWREQSYS                   (0x1UL << 17)                                /**< PLL Digital System Output Requested         */
#define _RFFPLL_STATUS_HWREQSYS_SHIFT            17                                           /**< Shift value for RFFPLL_HWREQSYS             */
#define _RFFPLL_STATUS_HWREQSYS_MASK             0x20000UL                                    /**< Bit mask for RFFPLL_HWREQSYS                */
#define _RFFPLL_STATUS_HWREQSYS_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for RFFPLL_STATUS              */
#define RFFPLL_STATUS_HWREQSYS_DEFAULT           (_RFFPLL_STATUS_HWREQSYS_DEFAULT << 17)      /**< Shifted mode DEFAULT for RFFPLL_STATUS      */
#define RFFPLL_STATUS_LOCK                       (0x1UL << 31)                                /**< Configuration Lock Status                   */
#define _RFFPLL_STATUS_LOCK_SHIFT                31                                           /**< Shift value for RFFPLL_LOCK                 */
#define _RFFPLL_STATUS_LOCK_MASK                 0x80000000UL                                 /**< Bit mask for RFFPLL_LOCK                    */
#define _RFFPLL_STATUS_LOCK_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for RFFPLL_STATUS              */
#define _RFFPLL_STATUS_LOCK_UNLOCKED             0x00000000UL                                 /**< Mode UNLOCKED for RFFPLL_STATUS             */
#define _RFFPLL_STATUS_LOCK_LOCKED               0x00000001UL                                 /**< Mode LOCKED for RFFPLL_STATUS               */
#define RFFPLL_STATUS_LOCK_DEFAULT               (_RFFPLL_STATUS_LOCK_DEFAULT << 31)          /**< Shifted mode DEFAULT for RFFPLL_STATUS      */
#define RFFPLL_STATUS_LOCK_UNLOCKED              (_RFFPLL_STATUS_LOCK_UNLOCKED << 31)         /**< Shifted mode UNLOCKED for RFFPLL_STATUS     */
#define RFFPLL_STATUS_LOCK_LOCKED                (_RFFPLL_STATUS_LOCK_LOCKED << 31)           /**< Shifted mode LOCKED for RFFPLL_STATUS       */

/* Bit fields for RFFPLL RFFPLLCTRL1 */
#define _RFFPLL_RFFPLLCTRL1_RESETVALUE           0x17116057UL                             /**< Default value for RFFPLL_RFFPLLCTRL1        */
#define _RFFPLL_RFFPLLCTRL1_MASK                 0xFF1FF07FUL                             /**< Mask for RFFPLL_RFFPLLCTRL1                 */
#define _RFFPLL_RFFPLLCTRL1_DIVN_SHIFT           0                                        /**< Shift value for RFFPLL_DIVN                 */
#define _RFFPLL_RFFPLLCTRL1_DIVN_MASK            0x7FUL                                   /**< Bit mask for RFFPLL_DIVN                    */
#define _RFFPLL_RFFPLLCTRL1_DIVN_DEFAULT         0x00000057UL                             /**< Mode DEFAULT for RFFPLL_RFFPLLCTRL1         */
#define RFFPLL_RFFPLLCTRL1_DIVN_DEFAULT          (_RFFPLL_RFFPLLCTRL1_DIVN_DEFAULT << 0)  /**< Shifted mode DEFAULT for RFFPLL_RFFPLLCTRL1 */
#define _RFFPLL_RFFPLLCTRL1_DIVX_SHIFT           12                                       /**< Shift value for RFFPLL_DIVX                 */
#define _RFFPLL_RFFPLLCTRL1_DIVX_MASK            0xF000UL                                 /**< Bit mask for RFFPLL_DIVX                    */
#define _RFFPLL_RFFPLLCTRL1_DIVX_DEFAULT         0x00000006UL                             /**< Mode DEFAULT for RFFPLL_RFFPLLCTRL1         */
#define RFFPLL_RFFPLLCTRL1_DIVX_DEFAULT          (_RFFPLL_RFFPLLCTRL1_DIVX_DEFAULT << 12) /**< Shifted mode DEFAULT for RFFPLL_RFFPLLCTRL1 */
#define _RFFPLL_RFFPLLCTRL1_DIVY_SHIFT           16                                       /**< Shift value for RFFPLL_DIVY                 */
#define _RFFPLL_RFFPLLCTRL1_DIVY_MASK            0x1F0000UL                               /**< Bit mask for RFFPLL_DIVY                    */
#define _RFFPLL_RFFPLLCTRL1_DIVY_DEFAULT         0x00000011UL                             /**< Mode DEFAULT for RFFPLL_RFFPLLCTRL1         */
#define RFFPLL_RFFPLLCTRL1_DIVY_DEFAULT          (_RFFPLL_RFFPLLCTRL1_DIVY_DEFAULT << 16) /**< Shifted mode DEFAULT for RFFPLL_RFFPLLCTRL1 */

/* Bit fields for RFFPLL IF */
#define _RFFPLL_IF_RESETVALUE                    0x00000000UL                             /**< Default value for RFFPLL_IF                 */
#define _RFFPLL_IF_MASK                          0x00000003UL                             /**< Mask for RFFPLL_IF                          */
#define RFFPLL_IF_RFFPLLRADIORDY                 (0x1UL << 0)                             /**< Radio Output Ready Interrupt                */
#define _RFFPLL_IF_RFFPLLRADIORDY_SHIFT          0                                        /**< Shift value for RFFPLL_RFFPLLRADIORDY       */
#define _RFFPLL_IF_RFFPLLRADIORDY_MASK           0x1UL                                    /**< Bit mask for RFFPLL_RFFPLLRADIORDY          */
#define _RFFPLL_IF_RFFPLLRADIORDY_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for RFFPLL_IF                  */
#define RFFPLL_IF_RFFPLLRADIORDY_DEFAULT         (_RFFPLL_IF_RFFPLLRADIORDY_DEFAULT << 0) /**< Shifted mode DEFAULT for RFFPLL_IF          */
#define RFFPLL_IF_RFFPLLSYSRDY                   (0x1UL << 1)                             /**< Digital System Output Ready Interrupt       */
#define _RFFPLL_IF_RFFPLLSYSRDY_SHIFT            1                                        /**< Shift value for RFFPLL_RFFPLLSYSRDY         */
#define _RFFPLL_IF_RFFPLLSYSRDY_MASK             0x2UL                                    /**< Bit mask for RFFPLL_RFFPLLSYSRDY            */
#define _RFFPLL_IF_RFFPLLSYSRDY_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for RFFPLL_IF                  */
#define RFFPLL_IF_RFFPLLSYSRDY_DEFAULT           (_RFFPLL_IF_RFFPLLSYSRDY_DEFAULT << 1)   /**< Shifted mode DEFAULT for RFFPLL_IF          */

/* Bit fields for RFFPLL IEN */
#define _RFFPLL_IEN_RESETVALUE                   0x00000000UL                              /**< Default value for RFFPLL_IEN                */
#define _RFFPLL_IEN_MASK                         0x00000003UL                              /**< Mask for RFFPLL_IEN                         */
#define RFFPLL_IEN_RFFPLLRADIORDY                (0x1UL << 0)                              /**< Radio Output Ready Interrupt                */
#define _RFFPLL_IEN_RFFPLLRADIORDY_SHIFT         0                                         /**< Shift value for RFFPLL_RFFPLLRADIORDY       */
#define _RFFPLL_IEN_RFFPLLRADIORDY_MASK          0x1UL                                     /**< Bit mask for RFFPLL_RFFPLLRADIORDY          */
#define _RFFPLL_IEN_RFFPLLRADIORDY_DEFAULT       0x00000000UL                              /**< Mode DEFAULT for RFFPLL_IEN                 */
#define RFFPLL_IEN_RFFPLLRADIORDY_DEFAULT        (_RFFPLL_IEN_RFFPLLRADIORDY_DEFAULT << 0) /**< Shifted mode DEFAULT for RFFPLL_IEN         */
#define RFFPLL_IEN_RFFPLLSYSRDY                  (0x1UL << 1)                              /**< Digital System Output Ready Interrupt       */
#define _RFFPLL_IEN_RFFPLLSYSRDY_SHIFT           1                                         /**< Shift value for RFFPLL_RFFPLLSYSRDY         */
#define _RFFPLL_IEN_RFFPLLSYSRDY_MASK            0x2UL                                     /**< Bit mask for RFFPLL_RFFPLLSYSRDY            */
#define _RFFPLL_IEN_RFFPLLSYSRDY_DEFAULT         0x00000000UL                              /**< Mode DEFAULT for RFFPLL_IEN                 */
#define RFFPLL_IEN_RFFPLLSYSRDY_DEFAULT          (_RFFPLL_IEN_RFFPLLSYSRDY_DEFAULT << 1)   /**< Shifted mode DEFAULT for RFFPLL_IEN         */

/* Bit fields for RFFPLL LOCK */
#define _RFFPLL_LOCK_RESETVALUE                  0x000085A9UL                           /**< Default value for RFFPLL_LOCK               */
#define _RFFPLL_LOCK_MASK                        0x0000FFFFUL                           /**< Mask for RFFPLL_LOCK                        */
#define _RFFPLL_LOCK_LOCKKEY_SHIFT               0                                      /**< Shift value for RFFPLL_LOCKKEY              */
#define _RFFPLL_LOCK_LOCKKEY_MASK                0xFFFFUL                               /**< Bit mask for RFFPLL_LOCKKEY                 */
#define _RFFPLL_LOCK_LOCKKEY_DEFAULT             0x000085A9UL                           /**< Mode DEFAULT for RFFPLL_LOCK                */
#define _RFFPLL_LOCK_LOCKKEY_UNLOCK              0x000085A9UL                           /**< Mode UNLOCK for RFFPLL_LOCK                 */
#define RFFPLL_LOCK_LOCKKEY_DEFAULT              (_RFFPLL_LOCK_LOCKKEY_DEFAULT << 0)    /**< Shifted mode DEFAULT for RFFPLL_LOCK        */
#define RFFPLL_LOCK_LOCKKEY_UNLOCK               (_RFFPLL_LOCK_LOCKKEY_UNLOCK << 0)     /**< Shifted mode UNLOCK for RFFPLL_LOCK         */

/** @} End of group EFR32FG25_RFFPLL_BitFields */
/** @} End of group EFR32FG25_RFFPLL */
/** @} End of group Parts */

#endif /* EFR32FG25_RFFPLL_H */
