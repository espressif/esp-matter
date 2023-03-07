/**************************************************************************//**
 * @file
 * @brief EFR32FG25 PLL register and bit field definitions
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
#ifndef EFR32FG25_PLL_H
#define EFR32FG25_PLL_H
#define PLL_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32FG25_PLL PLL
 * @{
 * @brief EFR32FG25 PLL Register Declaration.
 *****************************************************************************/

/** PLL Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  uint32_t       RESERVED0[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL;                          /**< Control                                            */
  __IM uint32_t  STATUS;                        /**< Status                                             */
  uint32_t       RESERVED1[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t DCOCTRL;                       /**< DAC oscillator Control                             */
  uint32_t       RESERVED2[4U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Lock PLL                                           */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED3[1009U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  uint32_t       RESERVED4[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL_SET;                      /**< Control                                            */
  __IM uint32_t  STATUS_SET;                    /**< Status                                             */
  uint32_t       RESERVED5[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t DCOCTRL_SET;                   /**< DAC oscillator Control                             */
  uint32_t       RESERVED6[4U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Lock PLL                                           */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED7[1009U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  uint32_t       RESERVED8[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL_CLR;                      /**< Control                                            */
  __IM uint32_t  STATUS_CLR;                    /**< Status                                             */
  uint32_t       RESERVED9[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t DCOCTRL_CLR;                   /**< DAC oscillator Control                             */
  uint32_t       RESERVED10[4U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Lock PLL                                           */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED11[1009U];             /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  uint32_t       RESERVED12[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL_TGL;                      /**< Control                                            */
  __IM uint32_t  STATUS_TGL;                    /**< Status                                             */
  uint32_t       RESERVED13[2U];                /**< Reserved for future use                            */
  __IOM uint32_t DCOCTRL_TGL;                   /**< DAC oscillator Control                             */
  uint32_t       RESERVED14[4U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Lock PLL                                           */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
} PLL_TypeDef;
/** @} End of group EFR32FG25_PLL */

/**************************************************************************//**
 * @addtogroup EFR32FG25_PLL
 * @{
 * @defgroup EFR32FG25_PLL_BitFields PLL Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for PLL IPVERSION */
#define _PLL_IPVERSION_RESETVALUE           0x00000001UL                                /**< Default value for PLL_IPVERSION             */
#define _PLL_IPVERSION_MASK                 0xFFFFFFFFUL                                /**< Mask for PLL_IPVERSION                      */
#define _PLL_IPVERSION_IPVERSION_SHIFT      0                                           /**< Shift value for PLL_IPVERSION               */
#define _PLL_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                                /**< Bit mask for PLL_IPVERSION                  */
#define _PLL_IPVERSION_IPVERSION_DEFAULT    0x00000001UL                                /**< Mode DEFAULT for PLL_IPVERSION              */
#define PLL_IPVERSION_IPVERSION_DEFAULT     (_PLL_IPVERSION_IPVERSION_DEFAULT << 0)     /**< Shifted mode DEFAULT for PLL_IPVERSION      */

/* Bit fields for PLL CTRL */
#define _PLL_CTRL_RESETVALUE                0x00360100UL                                /**< Default value for PLL_CTRL                  */
#define _PLL_CTRL_MASK                      0x01FFFF13UL                                /**< Mask for PLL_CTRL                           */
#define PLL_CTRL_FORCEEN                    (0x1UL << 0)                                /**< Force Enable                                */
#define _PLL_CTRL_FORCEEN_SHIFT             0                                           /**< Shift value for PLL_FORCEEN                 */
#define _PLL_CTRL_FORCEEN_MASK              0x1UL                                       /**< Bit mask for PLL_FORCEEN                    */
#define _PLL_CTRL_FORCEEN_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for PLL_CTRL                   */
#define PLL_CTRL_FORCEEN_DEFAULT            (_PLL_CTRL_FORCEEN_DEFAULT << 0)            /**< Shifted mode DEFAULT for PLL_CTRL           */
#define PLL_CTRL_DISONDEMAND                (0x1UL << 1)                                /**< Disable on Demand                           */
#define _PLL_CTRL_DISONDEMAND_SHIFT         1                                           /**< Shift value for PLL_DISONDEMAND             */
#define _PLL_CTRL_DISONDEMAND_MASK          0x2UL                                       /**< Bit mask for PLL_DISONDEMAND                */
#define _PLL_CTRL_DISONDEMAND_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for PLL_CTRL                   */
#define PLL_CTRL_DISONDEMAND_DEFAULT        (_PLL_CTRL_DISONDEMAND_DEFAULT << 1)        /**< Shifted mode DEFAULT for PLL_CTRL           */
#define PLL_CTRL_SHUNTREGLPEN               (0x1UL << 4)                                /**< Shunt Regulator LP Enable                   */
#define _PLL_CTRL_SHUNTREGLPEN_SHIFT        4                                           /**< Shift value for PLL_SHUNTREGLPEN            */
#define _PLL_CTRL_SHUNTREGLPEN_MASK         0x10UL                                      /**< Bit mask for PLL_SHUNTREGLPEN               */
#define _PLL_CTRL_SHUNTREGLPEN_DEFAULT      0x00000000UL                                /**< Mode DEFAULT for PLL_CTRL                   */
#define PLL_CTRL_SHUNTREGLPEN_DEFAULT       (_PLL_CTRL_SHUNTREGLPEN_DEFAULT << 4)       /**< Shifted mode DEFAULT for PLL_CTRL           */
#define _PLL_CTRL_DIVR_SHIFT                8                                           /**< Shift value for PLL_DIVR                    */
#define _PLL_CTRL_DIVR_MASK                 0x1F00UL                                    /**< Bit mask for PLL_DIVR                       */
#define _PLL_CTRL_DIVR_DEFAULT              0x00000001UL                                /**< Mode DEFAULT for PLL_CTRL                   */
#define PLL_CTRL_DIVR_DEFAULT               (_PLL_CTRL_DIVR_DEFAULT << 8)               /**< Shifted mode DEFAULT for PLL_CTRL           */
#define _PLL_CTRL_DIVX_SHIFT                13                                          /**< Shift value for PLL_DIVX                    */
#define _PLL_CTRL_DIVX_MASK                 0x3E000UL                                   /**< Bit mask for PLL_DIVX                       */
#define _PLL_CTRL_DIVX_DEFAULT              0x00000010UL                                /**< Mode DEFAULT for PLL_CTRL                   */
#define PLL_CTRL_DIVX_DEFAULT               (_PLL_CTRL_DIVX_DEFAULT << 13)              /**< Shifted mode DEFAULT for PLL_CTRL           */
#define _PLL_CTRL_DIVN_SHIFT                18                                          /**< Shift value for PLL_DIVN                    */
#define _PLL_CTRL_DIVN_MASK                 0x1FC0000UL                                 /**< Bit mask for PLL_DIVN                       */
#define _PLL_CTRL_DIVN_DEFAULT              0x0000000DUL                                /**< Mode DEFAULT for PLL_CTRL                   */
#define PLL_CTRL_DIVN_DEFAULT               (_PLL_CTRL_DIVN_DEFAULT << 18)              /**< Shifted mode DEFAULT for PLL_CTRL           */

/* Bit fields for PLL STATUS */
#define _PLL_STATUS_RESETVALUE              0x00000000UL                                /**< Default value for PLL_STATUS                */
#define _PLL_STATUS_MASK                    0xC0030000UL                                /**< Mask for PLL_STATUS                         */
#define PLL_STATUS_PLLRDY                   (0x1UL << 16)                               /**< PLL Ready                                   */
#define _PLL_STATUS_PLLRDY_SHIFT            16                                          /**< Shift value for PLL_PLLRDY                  */
#define _PLL_STATUS_PLLRDY_MASK             0x10000UL                                   /**< Bit mask for PLL_PLLRDY                     */
#define _PLL_STATUS_PLLRDY_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for PLL_STATUS                 */
#define PLL_STATUS_PLLRDY_DEFAULT           (_PLL_STATUS_PLLRDY_DEFAULT << 16)          /**< Shifted mode DEFAULT for PLL_STATUS         */
#define PLL_STATUS_PLLLOCK                  (0x1UL << 17)                               /**< PLL is locked                               */
#define _PLL_STATUS_PLLLOCK_SHIFT           17                                          /**< Shift value for PLL_PLLLOCK                 */
#define _PLL_STATUS_PLLLOCK_MASK            0x20000UL                                   /**< Bit mask for PLL_PLLLOCK                    */
#define _PLL_STATUS_PLLLOCK_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for PLL_STATUS                 */
#define PLL_STATUS_PLLLOCK_DEFAULT          (_PLL_STATUS_PLLLOCK_DEFAULT << 17)         /**< Shifted mode DEFAULT for PLL_STATUS         */
#define PLL_STATUS_SYNCBUSY                 (0x1UL << 30)                               /**< Sync Busy                                   */
#define _PLL_STATUS_SYNCBUSY_SHIFT          30                                          /**< Shift value for PLL_SYNCBUSY                */
#define _PLL_STATUS_SYNCBUSY_MASK           0x40000000UL                                /**< Bit mask for PLL_SYNCBUSY                   */
#define _PLL_STATUS_SYNCBUSY_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for PLL_STATUS                 */
#define PLL_STATUS_SYNCBUSY_DEFAULT         (_PLL_STATUS_SYNCBUSY_DEFAULT << 30)        /**< Shifted mode DEFAULT for PLL_STATUS         */
#define PLL_STATUS_LOCK                     (0x1UL << 31)                               /**< Locks out registers                         */
#define _PLL_STATUS_LOCK_SHIFT              31                                          /**< Shift value for PLL_LOCK                    */
#define _PLL_STATUS_LOCK_MASK               0x80000000UL                                /**< Bit mask for PLL_LOCK                       */
#define _PLL_STATUS_LOCK_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for PLL_STATUS                 */
#define _PLL_STATUS_LOCK_UNLOCKED           0x00000000UL                                /**< Mode UNLOCKED for PLL_STATUS                */
#define _PLL_STATUS_LOCK_LOCKED             0x00000001UL                                /**< Mode LOCKED for PLL_STATUS                  */
#define PLL_STATUS_LOCK_DEFAULT             (_PLL_STATUS_LOCK_DEFAULT << 31)            /**< Shifted mode DEFAULT for PLL_STATUS         */
#define PLL_STATUS_LOCK_UNLOCKED            (_PLL_STATUS_LOCK_UNLOCKED << 31)           /**< Shifted mode UNLOCKED for PLL_STATUS        */
#define PLL_STATUS_LOCK_LOCKED              (_PLL_STATUS_LOCK_LOCKED << 31)             /**< Shifted mode LOCKED for PLL_STATUS          */

/* Bit fields for PLL DCOCTRL */
#define _PLL_DCOCTRL_RESETVALUE             0x00060E00UL                                /**< Default value for PLL_DCOCTRL               */
#define _PLL_DCOCTRL_MASK                   0x003E0E00UL                                /**< Mask for PLL_DCOCTRL                        */
#define PLL_DCOCTRL_DCOBIASHALF             (0x1UL << 9)                                /**< DCO Half bias                               */
#define _PLL_DCOCTRL_DCOBIASHALF_SHIFT      9                                           /**< Shift value for PLL_DCOBIASHALF             */
#define _PLL_DCOCTRL_DCOBIASHALF_MASK       0x200UL                                     /**< Bit mask for PLL_DCOBIASHALF                */
#define _PLL_DCOCTRL_DCOBIASHALF_DEFAULT    0x00000001UL                                /**< Mode DEFAULT for PLL_DCOCTRL                */
#define _PLL_DCOCTRL_DCOBIASHALF_DISABLE    0x00000000UL                                /**< Mode DISABLE for PLL_DCOCTRL                */
#define _PLL_DCOCTRL_DCOBIASHALF_ENABLE     0x00000001UL                                /**< Mode ENABLE for PLL_DCOCTRL                 */
#define PLL_DCOCTRL_DCOBIASHALF_DEFAULT     (_PLL_DCOCTRL_DCOBIASHALF_DEFAULT << 9)     /**< Shifted mode DEFAULT for PLL_DCOCTRL        */
#define PLL_DCOCTRL_DCOBIASHALF_DISABLE     (_PLL_DCOCTRL_DCOBIASHALF_DISABLE << 9)     /**< Shifted mode DISABLE for PLL_DCOCTRL        */
#define PLL_DCOCTRL_DCOBIASHALF_ENABLE      (_PLL_DCOCTRL_DCOBIASHALF_ENABLE << 9)      /**< Shifted mode ENABLE for PLL_DCOCTRL         */

/* Bit fields for PLL LOCK */
#define _PLL_LOCK_RESETVALUE                0x0000580EUL                                /**< Default value for PLL_LOCK                  */
#define _PLL_LOCK_MASK                      0x0000FFFFUL                                /**< Mask for PLL_LOCK                           */
#define _PLL_LOCK_LOCKKEY_SHIFT             0                                           /**< Shift value for PLL_LOCKKEY                 */
#define _PLL_LOCK_LOCKKEY_MASK              0xFFFFUL                                    /**< Bit mask for PLL_LOCKKEY                    */
#define _PLL_LOCK_LOCKKEY_DEFAULT           0x0000580EUL                                /**< Mode DEFAULT for PLL_LOCK                   */
#define _PLL_LOCK_LOCKKEY_UNLOCK            0x0000580EUL                                /**< Mode UNLOCK for PLL_LOCK                    */
#define PLL_LOCK_LOCKKEY_DEFAULT            (_PLL_LOCK_LOCKKEY_DEFAULT << 0)            /**< Shifted mode DEFAULT for PLL_LOCK           */
#define PLL_LOCK_LOCKKEY_UNLOCK             (_PLL_LOCK_LOCKKEY_UNLOCK << 0)             /**< Shifted mode UNLOCK for PLL_LOCK            */

/* Bit fields for PLL IF */
#define _PLL_IF_RESETVALUE                  0x00000000UL                                /**< Default value for PLL_IF                    */
#define _PLL_IF_MASK                        0x00000007UL                                /**< Mask for PLL_IF                             */
#define PLL_IF_PLLRDY                       (0x1UL << 0)                                /**< PLL Ready interrupt                         */
#define _PLL_IF_PLLRDY_SHIFT                0                                           /**< Shift value for PLL_PLLRDY                  */
#define _PLL_IF_PLLRDY_MASK                 0x1UL                                       /**< Bit mask for PLL_PLLRDY                     */
#define _PLL_IF_PLLRDY_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for PLL_IF                     */
#define PLL_IF_PLLRDY_DEFAULT               (_PLL_IF_PLLRDY_DEFAULT << 0)               /**< Shifted mode DEFAULT for PLL_IF             */
#define PLL_IF_PLLLOCKIF                    (0x1UL << 1)                                /**< PLL lock IRQ                                */
#define _PLL_IF_PLLLOCKIF_SHIFT             1                                           /**< Shift value for PLL_PLLLOCKIF               */
#define _PLL_IF_PLLLOCKIF_MASK              0x2UL                                       /**< Bit mask for PLL_PLLLOCKIF                  */
#define _PLL_IF_PLLLOCKIF_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for PLL_IF                     */
#define PLL_IF_PLLLOCKIF_DEFAULT            (_PLL_IF_PLLLOCKIF_DEFAULT << 1)            /**< Shifted mode DEFAULT for PLL_IF             */
#define PLL_IF_PLLNOLOCKIF                  (0x1UL << 2)                                /**< PLL loss of lock IRQ                        */
#define _PLL_IF_PLLNOLOCKIF_SHIFT           2                                           /**< Shift value for PLL_PLLNOLOCKIF             */
#define _PLL_IF_PLLNOLOCKIF_MASK            0x4UL                                       /**< Bit mask for PLL_PLLNOLOCKIF                */
#define _PLL_IF_PLLNOLOCKIF_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for PLL_IF                     */
#define PLL_IF_PLLNOLOCKIF_DEFAULT          (_PLL_IF_PLLNOLOCKIF_DEFAULT << 2)          /**< Shifted mode DEFAULT for PLL_IF             */

/* Bit fields for PLL IEN */
#define _PLL_IEN_RESETVALUE                 0x00000000UL                                /**< Default value for PLL_IEN                   */
#define _PLL_IEN_MASK                       0x00000007UL                                /**< Mask for PLL_IEN                            */
#define PLL_IEN_PLLRDY                      (0x1UL << 0)                                /**< PLL is ready                                */
#define _PLL_IEN_PLLRDY_SHIFT               0                                           /**< Shift value for PLL_PLLRDY                  */
#define _PLL_IEN_PLLRDY_MASK                0x1UL                                       /**< Bit mask for PLL_PLLRDY                     */
#define _PLL_IEN_PLLRDY_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for PLL_IEN                    */
#define PLL_IEN_PLLRDY_DEFAULT              (_PLL_IEN_PLLRDY_DEFAULT << 0)              /**< Shifted mode DEFAULT for PLL_IEN            */
#define PLL_IEN_PLLLOCKIEN                  (0x1UL << 1)                                /**< PLL lock                                    */
#define _PLL_IEN_PLLLOCKIEN_SHIFT           1                                           /**< Shift value for PLL_PLLLOCKIEN              */
#define _PLL_IEN_PLLLOCKIEN_MASK            0x2UL                                       /**< Bit mask for PLL_PLLLOCKIEN                 */
#define _PLL_IEN_PLLLOCKIEN_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for PLL_IEN                    */
#define PLL_IEN_PLLLOCKIEN_DEFAULT          (_PLL_IEN_PLLLOCKIEN_DEFAULT << 1)          /**< Shifted mode DEFAULT for PLL_IEN            */
#define PLL_IEN_PLLNOLOCKIEN                (0x1UL << 2)                                /**< PLL loss of lock                            */
#define _PLL_IEN_PLLNOLOCKIEN_SHIFT         2                                           /**< Shift value for PLL_PLLNOLOCKIEN            */
#define _PLL_IEN_PLLNOLOCKIEN_MASK          0x4UL                                       /**< Bit mask for PLL_PLLNOLOCKIEN               */
#define _PLL_IEN_PLLNOLOCKIEN_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for PLL_IEN                    */
#define PLL_IEN_PLLNOLOCKIEN_DEFAULT        (_PLL_IEN_PLLNOLOCKIEN_DEFAULT << 2)        /**< Shifted mode DEFAULT for PLL_IEN            */

/** @} End of group EFR32FG25_PLL_BitFields */
/** @} End of group EFR32FG25_PLL */
/** @} End of group Parts */

#endif /* EFR32FG25_PLL_H */
