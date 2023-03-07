/**************************************************************************//**
 * @file
 * @brief EFR32BG27 SEPUF register and bit field definitions
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
#ifndef EFR32BG27_SEPUF_H
#define EFR32BG27_SEPUF_H

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32BG27_SEPUF_APBCFG SEPUF_APBCFG
 * @{
 * @brief EFR32BG27 SEPUF_APBCFG Register Declaration.
 *****************************************************************************/

/** SEPUF_APBCFG Register Declaration. */
typedef struct {
  __IOM uint32_t QK_CR;                              /**< QK_CR                                              */
  uint32_t       RESERVED0[7U];                      /**< Reserved for future use                            */
  __IM uint32_t  QK_SR;                              /**< QK_SR                                              */
  uint32_t       RESERVED1[1U];                      /**< Reserved for future use                            */
  __IM uint32_t  QK_AR;                              /**< QK_AR                                              */
  uint32_t       RESERVED2[6U];                      /**< Reserved for future use                            */
  __IOM uint32_t QK_CI;                              /**< QK_CI                                              */
  __IM uint32_t  QK_CO;                              /**< QK_CO                                              */
  uint32_t       RESERVED3[36U];                     /**< Reserved for future use                            */
  __IOM uint32_t QK_IF_SR;                           /**< QK_IF_SR                                           */
  uint32_t       RESERVED4[7U];                      /**< Reserved for future use                            */
  __IM uint32_t  QK_VER;                             /**< QK_VER                                             */
} SEPUF_APBCFG_TypeDef;
/** @} End of group EFR32BG27_SEPUF_APBCFG */

/**************************************************************************//**
 * @addtogroup EFR32BG27_SEPUF_APBCFG
 * @{
 * @defgroup EFR32BG27_SEPUF_APBCFG_BitFields SEPUF_APBCFG Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SEPUF QK_CR */
#define _SEPUF_QK_CR_RESETVALUE                 0x00000000UL                            /**< Default value for SEPUF_QK_CR               */
#define _SEPUF_QK_CR_MASK                       0x00000007UL                            /**< Mask for SEPUF_QK_CR                        */
#define SEPUF_QK_CR_QK_ZEROIZE                  (0x1UL << 0)                            /**< QK_ZEROIZE                                  */
#define _SEPUF_QK_CR_QK_ZEROIZE_SHIFT           0                                       /**< Shift value for SEPUF_QK_ZEROIZE            */
#define _SEPUF_QK_CR_QK_ZEROIZE_MASK            0x1UL                                   /**< Bit mask for SEPUF_QK_ZEROIZE               */
#define _SEPUF_QK_CR_QK_ZEROIZE_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_CR                */
#define SEPUF_QK_CR_QK_ZEROIZE_DEFAULT          (_SEPUF_QK_CR_QK_ZEROIZE_DEFAULT << 0)  /**< Shifted mode DEFAULT for SEPUF_QK_CR        */
#define SEPUF_QK_CR_QK_ENROLL                   (0x1UL << 1)                            /**< QK_ENROLL                                   */
#define _SEPUF_QK_CR_QK_ENROLL_SHIFT            1                                       /**< Shift value for SEPUF_QK_ENROLL             */
#define _SEPUF_QK_CR_QK_ENROLL_MASK             0x2UL                                   /**< Bit mask for SEPUF_QK_ENROLL                */
#define _SEPUF_QK_CR_QK_ENROLL_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_CR                */
#define SEPUF_QK_CR_QK_ENROLL_DEFAULT           (_SEPUF_QK_CR_QK_ENROLL_DEFAULT << 1)   /**< Shifted mode DEFAULT for SEPUF_QK_CR        */
#define SEPUF_QK_CR_QK_START                    (0x1UL << 2)                            /**< QK_START                                    */
#define _SEPUF_QK_CR_QK_START_SHIFT             2                                       /**< Shift value for SEPUF_QK_START              */
#define _SEPUF_QK_CR_QK_START_MASK              0x4UL                                   /**< Bit mask for SEPUF_QK_START                 */
#define _SEPUF_QK_CR_QK_START_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_CR                */
#define SEPUF_QK_CR_QK_START_DEFAULT            (_SEPUF_QK_CR_QK_START_DEFAULT << 2)    /**< Shifted mode DEFAULT for SEPUF_QK_CR        */

/* Bit fields for SEPUF QK_SR */
#define _SEPUF_QK_SR_RESETVALUE                 0x00000001UL                            /**< Default value for SEPUF_QK_SR               */
#define _SEPUF_QK_SR_MASK                       0x000000E7UL                            /**< Mask for SEPUF_QK_SR                        */
#define SEPUF_QK_SR_QK_BUSY                     (0x1UL << 0)                            /**< QK_BUSY                                     */
#define _SEPUF_QK_SR_QK_BUSY_SHIFT              0                                       /**< Shift value for SEPUF_QK_BUSY               */
#define _SEPUF_QK_SR_QK_BUSY_MASK               0x1UL                                   /**< Bit mask for SEPUF_QK_BUSY                  */
#define _SEPUF_QK_SR_QK_BUSY_DEFAULT            0x00000001UL                            /**< Mode DEFAULT for SEPUF_QK_SR                */
#define SEPUF_QK_SR_QK_BUSY_DEFAULT             (_SEPUF_QK_SR_QK_BUSY_DEFAULT << 0)     /**< Shifted mode DEFAULT for SEPUF_QK_SR        */
#define SEPUF_QK_SR_QK_OK                       (0x1UL << 1)                            /**< QK_OK                                       */
#define _SEPUF_QK_SR_QK_OK_SHIFT                1                                       /**< Shift value for SEPUF_QK_OK                 */
#define _SEPUF_QK_SR_QK_OK_MASK                 0x2UL                                   /**< Bit mask for SEPUF_QK_OK                    */
#define _SEPUF_QK_SR_QK_OK_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_SR                */
#define SEPUF_QK_SR_QK_OK_DEFAULT               (_SEPUF_QK_SR_QK_OK_DEFAULT << 1)       /**< Shifted mode DEFAULT for SEPUF_QK_SR        */
#define SEPUF_QK_SR_QK_ERROR                    (0x1UL << 2)                            /**< QK_ERROR                                    */
#define _SEPUF_QK_SR_QK_ERROR_SHIFT             2                                       /**< Shift value for SEPUF_QK_ERROR              */
#define _SEPUF_QK_SR_QK_ERROR_MASK              0x4UL                                   /**< Bit mask for SEPUF_QK_ERROR                 */
#define _SEPUF_QK_SR_QK_ERROR_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_SR                */
#define SEPUF_QK_SR_QK_ERROR_DEFAULT            (_SEPUF_QK_SR_QK_ERROR_DEFAULT << 2)    /**< Shifted mode DEFAULT for SEPUF_QK_SR        */
#define SEPUF_QK_SR_QK_KO_OR                    (0x1UL << 5)                            /**< QK_KO_OR                                    */
#define _SEPUF_QK_SR_QK_KO_OR_SHIFT             5                                       /**< Shift value for SEPUF_QK_KO_OR              */
#define _SEPUF_QK_SR_QK_KO_OR_MASK              0x20UL                                  /**< Bit mask for SEPUF_QK_KO_OR                 */
#define _SEPUF_QK_SR_QK_KO_OR_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_SR                */
#define SEPUF_QK_SR_QK_KO_OR_DEFAULT            (_SEPUF_QK_SR_QK_KO_OR_DEFAULT << 5)    /**< Shifted mode DEFAULT for SEPUF_QK_SR        */
#define SEPUF_QK_SR_QK_CI_IR                    (0x1UL << 6)                            /**< QK_CI_IR                                    */
#define _SEPUF_QK_SR_QK_CI_IR_SHIFT             6                                       /**< Shift value for SEPUF_QK_CI_IR              */
#define _SEPUF_QK_SR_QK_CI_IR_MASK              0x40UL                                  /**< Bit mask for SEPUF_QK_CI_IR                 */
#define _SEPUF_QK_SR_QK_CI_IR_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_SR                */
#define SEPUF_QK_SR_QK_CI_IR_DEFAULT            (_SEPUF_QK_SR_QK_CI_IR_DEFAULT << 6)    /**< Shifted mode DEFAULT for SEPUF_QK_SR        */
#define SEPUF_QK_SR_QK_CO_OR                    (0x1UL << 7)                            /**< QK_CO_OR                                    */
#define _SEPUF_QK_SR_QK_CO_OR_SHIFT             7                                       /**< Shift value for SEPUF_QK_CO_OR              */
#define _SEPUF_QK_SR_QK_CO_OR_MASK              0x80UL                                  /**< Bit mask for SEPUF_QK_CO_OR                 */
#define _SEPUF_QK_SR_QK_CO_OR_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_SR                */
#define SEPUF_QK_SR_QK_CO_OR_DEFAULT            (_SEPUF_QK_SR_QK_CO_OR_DEFAULT << 7)    /**< Shifted mode DEFAULT for SEPUF_QK_SR        */

/* Bit fields for SEPUF QK_AR */
#define _SEPUF_QK_AR_RESETVALUE                 0x00000000UL                                /**< Default value for SEPUF_QK_AR               */
#define _SEPUF_QK_AR_MASK                       0x00000003UL                                /**< Mask for SEPUF_QK_AR                        */
#define SEPUF_QK_AR_QK_ALLOW_ENROLL             (0x1UL << 0)                                /**< QK_ALLOW_ENROLL                             */
#define _SEPUF_QK_AR_QK_ALLOW_ENROLL_SHIFT      0                                           /**< Shift value for SEPUF_QK_ALLOW_ENROLL       */
#define _SEPUF_QK_AR_QK_ALLOW_ENROLL_MASK       0x1UL                                       /**< Bit mask for SEPUF_QK_ALLOW_ENROLL          */
#define _SEPUF_QK_AR_QK_ALLOW_ENROLL_DEFAULT    0x00000000UL                                /**< Mode DEFAULT for SEPUF_QK_AR                */
#define SEPUF_QK_AR_QK_ALLOW_ENROLL_DEFAULT     (_SEPUF_QK_AR_QK_ALLOW_ENROLL_DEFAULT << 0) /**< Shifted mode DEFAULT for SEPUF_QK_AR        */
#define SEPUF_QK_AR_QK_ALLOW_START              (0x1UL << 1)                                /**< QK_ALLOW_START                              */
#define _SEPUF_QK_AR_QK_ALLOW_START_SHIFT       1                                           /**< Shift value for SEPUF_QK_ALLOW_START        */
#define _SEPUF_QK_AR_QK_ALLOW_START_MASK        0x2UL                                       /**< Bit mask for SEPUF_QK_ALLOW_START           */
#define _SEPUF_QK_AR_QK_ALLOW_START_DEFAULT     0x00000000UL                                /**< Mode DEFAULT for SEPUF_QK_AR                */
#define SEPUF_QK_AR_QK_ALLOW_START_DEFAULT      (_SEPUF_QK_AR_QK_ALLOW_START_DEFAULT << 1)  /**< Shifted mode DEFAULT for SEPUF_QK_AR        */

/* Bit fields for SEPUF QK_CI */
#define _SEPUF_QK_CI_RESETVALUE                 0x00000000UL                            /**< Default value for SEPUF_QK_CI               */
#define _SEPUF_QK_CI_MASK                       0x00000001UL                            /**< Mask for SEPUF_QK_CI                        */
#define SEPUF_QK_CI_QK_CI                       (0x1UL << 0)                            /**< QK_CI                                       */
#define _SEPUF_QK_CI_QK_CI_SHIFT                0                                       /**< Shift value for SEPUF_QK_CI                 */
#define _SEPUF_QK_CI_QK_CI_MASK                 0x1UL                                   /**< Bit mask for SEPUF_QK_CI                    */
#define _SEPUF_QK_CI_QK_CI_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_CI                */
#define SEPUF_QK_CI_QK_CI_DEFAULT               (_SEPUF_QK_CI_QK_CI_DEFAULT << 0)       /**< Shifted mode DEFAULT for SEPUF_QK_CI        */

/* Bit fields for SEPUF QK_CO */
#define _SEPUF_QK_CO_RESETVALUE                 0x00000000UL                            /**< Default value for SEPUF_QK_CO               */
#define _SEPUF_QK_CO_MASK                       0x00000001UL                            /**< Mask for SEPUF_QK_CO                        */
#define SEPUF_QK_CO_QK_CO                       (0x1UL << 0)                            /**< QK_CO                                       */
#define _SEPUF_QK_CO_QK_CO_SHIFT                0                                       /**< Shift value for SEPUF_QK_CO                 */
#define _SEPUF_QK_CO_QK_CO_MASK                 0x1UL                                   /**< Bit mask for SEPUF_QK_CO                    */
#define _SEPUF_QK_CO_QK_CO_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for SEPUF_QK_CO                */
#define SEPUF_QK_CO_QK_CO_DEFAULT               (_SEPUF_QK_CO_QK_CO_DEFAULT << 0)       /**< Shifted mode DEFAULT for SEPUF_QK_CO        */

/* Bit fields for SEPUF QK_IF_SR */
#define _SEPUF_QK_IF_SR_RESETVALUE              0x00000000UL                                /**< Default value for SEPUF_QK_IF_SR            */
#define _SEPUF_QK_IF_SR_MASK                    0x00000081UL                                /**< Mask for SEPUF_QK_IF_SR                     */
#define SEPUF_QK_IF_SR_IF_ERROR                 (0x1UL << 0)                                /**< IF_ERROR                                    */
#define _SEPUF_QK_IF_SR_IF_ERROR_SHIFT          0                                           /**< Shift value for SEPUF_IF_ERROR              */
#define _SEPUF_QK_IF_SR_IF_ERROR_MASK           0x1UL                                       /**< Bit mask for SEPUF_IF_ERROR                 */
#define _SEPUF_QK_IF_SR_IF_ERROR_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for SEPUF_QK_IF_SR             */
#define SEPUF_QK_IF_SR_IF_ERROR_DEFAULT         (_SEPUF_QK_IF_SR_IF_ERROR_DEFAULT << 0)     /**< Shifted mode DEFAULT for SEPUF_QK_IF_SR     */
#define SEPUF_QK_IF_SR_MEM_IF_ERROR             (0x1UL << 7)                                /**< MEM_IF_ERROR                                */
#define _SEPUF_QK_IF_SR_MEM_IF_ERROR_SHIFT      7                                           /**< Shift value for SEPUF_MEM_IF_ERROR          */
#define _SEPUF_QK_IF_SR_MEM_IF_ERROR_MASK       0x80UL                                      /**< Bit mask for SEPUF_MEM_IF_ERROR             */
#define _SEPUF_QK_IF_SR_MEM_IF_ERROR_DEFAULT    0x00000000UL                                /**< Mode DEFAULT for SEPUF_QK_IF_SR             */
#define SEPUF_QK_IF_SR_MEM_IF_ERROR_DEFAULT     (_SEPUF_QK_IF_SR_MEM_IF_ERROR_DEFAULT << 7) /**< Shifted mode DEFAULT for SEPUF_QK_IF_SR     */

/* Bit fields for SEPUF QK_VER */
#define _SEPUF_QK_VER_RESETVALUE                0x18910201UL                            /**< Default value for SEPUF_QK_VER              */
#define _SEPUF_QK_VER_MASK                      0xFFFFFFFFUL                            /**< Mask for SEPUF_QK_VER                       */
#define _SEPUF_QK_VER_QK_VERSION_SHIFT          0                                       /**< Shift value for SEPUF_QK_VERSION            */
#define _SEPUF_QK_VER_QK_VERSION_MASK           0xFFFFFFFFUL                            /**< Bit mask for SEPUF_QK_VERSION               */
#define _SEPUF_QK_VER_QK_VERSION_DEFAULT        0x18910201UL                            /**< Mode DEFAULT for SEPUF_QK_VER               */
#define SEPUF_QK_VER_QK_VERSION_DEFAULT         (_SEPUF_QK_VER_QK_VERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for SEPUF_QK_VER       */

/** @} End of group EFR32BG27_SEPUF_APBCFG_BitFields */
/** @} End of group EFR32BG27_SEPUF_APBCFG */
/** @} End of group Parts */

#endif /* EFR32BG27_SEPUF_H */
