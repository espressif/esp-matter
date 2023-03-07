/**************************************************************************//**
 * @file
 * @brief EFR32MG27 ETAMPDET register and bit field definitions
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
#ifndef EFR32MG27_ETAMPDET_H
#define EFR32MG27_ETAMPDET_H
#define ETAMPDET_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG27_ETAMPDET ETAMPDET
 * @{
 * @brief EFR32MG27 ETAMPDET Register Declaration.
 *****************************************************************************/

/** ETAMPDET Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  __IOM uint32_t EN;                            /**< Module Enable Register                             */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFG;                           /**< Configuration Register                             */
  __IOM uint32_t CNTMISMATCHMAX;                /**< Filter Threshold Register                          */
  __IOM uint32_t CHNLFILTWINSIZE;               /**< Filter moving window size Register                 */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IM uint32_t  SYNCBUSY;                      /**< Syncbusy Status Register                           */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t EM4WUEN;                       /**< EM4 wakeup request Enable Register                 */
  __IOM uint32_t CHNLSEEDVAL0;                  /**< CHNL0 LFSR Seed Ctrl Register                      */
  __IOM uint32_t CHNLSEEDVAL1;                  /**< CHNL1 LFSR Seed Ctrl Register                      */
  __IOM uint32_t CLKPRESCVAL;                   /**< Prescaler Ctrl Register                            */
  uint32_t       RESERVED1[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  uint32_t       RESERVED2[1005U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  __IOM uint32_t EN_SET;                        /**< Module Enable Register                             */
  uint32_t       RESERVED3[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFG_SET;                       /**< Configuration Register                             */
  __IOM uint32_t CNTMISMATCHMAX_SET;            /**< Filter Threshold Register                          */
  __IOM uint32_t CHNLFILTWINSIZE_SET;           /**< Filter moving window size Register                 */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IM uint32_t  SYNCBUSY_SET;                  /**< Syncbusy Status Register                           */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t EM4WUEN_SET;                   /**< EM4 wakeup request Enable Register                 */
  __IOM uint32_t CHNLSEEDVAL0_SET;              /**< CHNL0 LFSR Seed Ctrl Register                      */
  __IOM uint32_t CHNLSEEDVAL1_SET;              /**< CHNL1 LFSR Seed Ctrl Register                      */
  __IOM uint32_t CLKPRESCVAL_SET;               /**< Prescaler Ctrl Register                            */
  uint32_t       RESERVED4[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED5[1005U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  __IOM uint32_t EN_CLR;                        /**< Module Enable Register                             */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFG_CLR;                       /**< Configuration Register                             */
  __IOM uint32_t CNTMISMATCHMAX_CLR;            /**< Filter Threshold Register                          */
  __IOM uint32_t CHNLFILTWINSIZE_CLR;           /**< Filter moving window size Register                 */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< Syncbusy Status Register                           */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t EM4WUEN_CLR;                   /**< EM4 wakeup request Enable Register                 */
  __IOM uint32_t CHNLSEEDVAL0_CLR;              /**< CHNL0 LFSR Seed Ctrl Register                      */
  __IOM uint32_t CHNLSEEDVAL1_CLR;              /**< CHNL1 LFSR Seed Ctrl Register                      */
  __IOM uint32_t CLKPRESCVAL_CLR;               /**< Prescaler Ctrl Register                            */
  uint32_t       RESERVED7[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED8[1005U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  __IOM uint32_t EN_TGL;                        /**< Module Enable Register                             */
  uint32_t       RESERVED9[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFG_TGL;                       /**< Configuration Register                             */
  __IOM uint32_t CNTMISMATCHMAX_TGL;            /**< Filter Threshold Register                          */
  __IOM uint32_t CHNLFILTWINSIZE_TGL;           /**< Filter moving window size Register                 */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< Syncbusy Status Register                           */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t EM4WUEN_TGL;                   /**< EM4 wakeup request Enable Register                 */
  __IOM uint32_t CHNLSEEDVAL0_TGL;              /**< CHNL0 LFSR Seed Ctrl Register                      */
  __IOM uint32_t CHNLSEEDVAL1_TGL;              /**< CHNL1 LFSR Seed Ctrl Register                      */
  __IOM uint32_t CLKPRESCVAL_TGL;               /**< Prescaler Ctrl Register                            */
  uint32_t       RESERVED10[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
} ETAMPDET_TypeDef;
/** @} End of group EFR32MG27_ETAMPDET */

/**************************************************************************//**
 * @addtogroup EFR32MG27_ETAMPDET
 * @{
 * @defgroup EFR32MG27_ETAMPDET_BitFields ETAMPDET Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for ETAMPDET IPVERSION */
#define _ETAMPDET_IPVERSION_RESETVALUE                                            0x00000002UL                                 /**< Default value for ETAMPDET_IPVERSION        */
#define _ETAMPDET_IPVERSION_MASK                                                  0xFFFFFFFFUL                                 /**< Mask for ETAMPDET_IPVERSION                 */
#define _ETAMPDET_IPVERSION_IPVERSION_SHIFT                                       0                                            /**< Shift value for ETAMPDET_IPVERSION          */
#define _ETAMPDET_IPVERSION_IPVERSION_MASK                                        0xFFFFFFFFUL                                 /**< Bit mask for ETAMPDET_IPVERSION             */
#define _ETAMPDET_IPVERSION_IPVERSION_DEFAULT                                     0x00000002UL                                 /**< Mode DEFAULT for ETAMPDET_IPVERSION         */
#define ETAMPDET_IPVERSION_IPVERSION_DEFAULT                                      (_ETAMPDET_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_IPVERSION */

/* Bit fields for ETAMPDET EN */
#define _ETAMPDET_EN_RESETVALUE                                                   0x00000000UL                   /**< Default value for ETAMPDET_EN               */
#define _ETAMPDET_EN_MASK                                                         0x00000001UL                   /**< Mask for ETAMPDET_EN                        */
#define ETAMPDET_EN_EN                                                            (0x1UL << 0)                   /**< ETAMPDET Enable                             */
#define _ETAMPDET_EN_EN_SHIFT                                                     0                              /**< Shift value for ETAMPDET_EN                 */
#define _ETAMPDET_EN_EN_MASK                                                      0x1UL                          /**< Bit mask for ETAMPDET_EN                    */
#define _ETAMPDET_EN_EN_DEFAULT                                                   0x00000000UL                   /**< Mode DEFAULT for ETAMPDET_EN                */
#define ETAMPDET_EN_EN_DEFAULT                                                    (_ETAMPDET_EN_EN_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_EN        */

/* Bit fields for ETAMPDET CFG */
#define _ETAMPDET_CFG_RESETVALUE                                                  0x00000000UL                                    /**< Default value for ETAMPDET_CFG              */
#define _ETAMPDET_CFG_MASK                                                        0x0000003FUL                                    /**< Mask for ETAMPDET_CFG                       */
#define ETAMPDET_CFG_CHNLCMPDLYEN0                                                (0x1UL << 0)                                    /**< enable delay for comparison                 */
#define _ETAMPDET_CFG_CHNLCMPDLYEN0_SHIFT                                         0                                               /**< Shift value for ETAMPDET_CHNLCMPDLYEN0      */
#define _ETAMPDET_CFG_CHNLCMPDLYEN0_MASK                                          0x1UL                                           /**< Bit mask for ETAMPDET_CHNLCMPDLYEN0         */
#define _ETAMPDET_CFG_CHNLCMPDLYEN0_DEFAULT                                       0x00000000UL                                    /**< Mode DEFAULT for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLCMPDLYEN0_X0                                            0x00000000UL                                    /**< Mode X0 for ETAMPDET_CFG                    */
#define _ETAMPDET_CFG_CHNLCMPDLYEN0_X1                                            0x00000001UL                                    /**< Mode X1 for ETAMPDET_CFG                    */
#define ETAMPDET_CFG_CHNLCMPDLYEN0_DEFAULT                                        (_ETAMPDET_CFG_CHNLCMPDLYEN0_DEFAULT << 0)      /**< Shifted mode DEFAULT for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLCMPDLYEN0_X0                                             (_ETAMPDET_CFG_CHNLCMPDLYEN0_X0 << 0)           /**< Shifted mode X0 for ETAMPDET_CFG            */
#define ETAMPDET_CFG_CHNLCMPDLYEN0_X1                                             (_ETAMPDET_CFG_CHNLCMPDLYEN0_X1 << 0)           /**< Shifted mode X1 for ETAMPDET_CFG            */
#define ETAMPDET_CFG_CHNLTAMPDETFILTEN0                                           (0x1UL << 1)                                    /**< enable detect filtering                     */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN0_SHIFT                                    1                                               /**< Shift value for ETAMPDET_CHNLTAMPDETFILTEN0 */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN0_MASK                                     0x2UL                                           /**< Bit mask for ETAMPDET_CHNLTAMPDETFILTEN0    */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN0_DEFAULT                                  0x00000000UL                                    /**< Mode DEFAULT for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN0_DISABLE                                  0x00000000UL                                    /**< Mode DISABLE for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN0_ENABLE                                   0x00000001UL                                    /**< Mode ENABLE for ETAMPDET_CFG                */
#define ETAMPDET_CFG_CHNLTAMPDETFILTEN0_DEFAULT                                   (_ETAMPDET_CFG_CHNLTAMPDETFILTEN0_DEFAULT << 1) /**< Shifted mode DEFAULT for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLTAMPDETFILTEN0_DISABLE                                   (_ETAMPDET_CFG_CHNLTAMPDETFILTEN0_DISABLE << 1) /**< Shifted mode DISABLE for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLTAMPDETFILTEN0_ENABLE                                    (_ETAMPDET_CFG_CHNLTAMPDETFILTEN0_ENABLE << 1)  /**< Shifted mode ENABLE for ETAMPDET_CFG        */
#define ETAMPDET_CFG_CHNLPADEN0                                                   (0x1UL << 2)                                    /**< enable driving pad                          */
#define _ETAMPDET_CFG_CHNLPADEN0_SHIFT                                            2                                               /**< Shift value for ETAMPDET_CHNLPADEN0         */
#define _ETAMPDET_CFG_CHNLPADEN0_MASK                                             0x4UL                                           /**< Bit mask for ETAMPDET_CHNLPADEN0            */
#define _ETAMPDET_CFG_CHNLPADEN0_DEFAULT                                          0x00000000UL                                    /**< Mode DEFAULT for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLPADEN0_DISABLE                                          0x00000000UL                                    /**< Mode DISABLE for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLPADEN0_ENABLE                                           0x00000001UL                                    /**< Mode ENABLE for ETAMPDET_CFG                */
#define ETAMPDET_CFG_CHNLPADEN0_DEFAULT                                           (_ETAMPDET_CFG_CHNLPADEN0_DEFAULT << 2)         /**< Shifted mode DEFAULT for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLPADEN0_DISABLE                                           (_ETAMPDET_CFG_CHNLPADEN0_DISABLE << 2)         /**< Shifted mode DISABLE for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLPADEN0_ENABLE                                            (_ETAMPDET_CFG_CHNLPADEN0_ENABLE << 2)          /**< Shifted mode ENABLE for ETAMPDET_CFG        */
#define ETAMPDET_CFG_CHNLCMPDLYEN1                                                (0x1UL << 3)                                    /**< enable delay for comparison                 */
#define _ETAMPDET_CFG_CHNLCMPDLYEN1_SHIFT                                         3                                               /**< Shift value for ETAMPDET_CHNLCMPDLYEN1      */
#define _ETAMPDET_CFG_CHNLCMPDLYEN1_MASK                                          0x8UL                                           /**< Bit mask for ETAMPDET_CHNLCMPDLYEN1         */
#define _ETAMPDET_CFG_CHNLCMPDLYEN1_DEFAULT                                       0x00000000UL                                    /**< Mode DEFAULT for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLCMPDLYEN1_DISABLE                                       0x00000000UL                                    /**< Mode DISABLE for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLCMPDLYEN1_ENABLE                                        0x00000001UL                                    /**< Mode ENABLE for ETAMPDET_CFG                */
#define ETAMPDET_CFG_CHNLCMPDLYEN1_DEFAULT                                        (_ETAMPDET_CFG_CHNLCMPDLYEN1_DEFAULT << 3)      /**< Shifted mode DEFAULT for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLCMPDLYEN1_DISABLE                                        (_ETAMPDET_CFG_CHNLCMPDLYEN1_DISABLE << 3)      /**< Shifted mode DISABLE for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLCMPDLYEN1_ENABLE                                         (_ETAMPDET_CFG_CHNLCMPDLYEN1_ENABLE << 3)       /**< Shifted mode ENABLE for ETAMPDET_CFG        */
#define ETAMPDET_CFG_CHNLTAMPDETFILTEN1                                           (0x1UL << 4)                                    /**< enable detect filtering                     */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN1_SHIFT                                    4                                               /**< Shift value for ETAMPDET_CHNLTAMPDETFILTEN1 */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN1_MASK                                     0x10UL                                          /**< Bit mask for ETAMPDET_CHNLTAMPDETFILTEN1    */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN1_DEFAULT                                  0x00000000UL                                    /**< Mode DEFAULT for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN1_DISABLE                                  0x00000000UL                                    /**< Mode DISABLE for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLTAMPDETFILTEN1_ENABLE                                   0x00000001UL                                    /**< Mode ENABLE for ETAMPDET_CFG                */
#define ETAMPDET_CFG_CHNLTAMPDETFILTEN1_DEFAULT                                   (_ETAMPDET_CFG_CHNLTAMPDETFILTEN1_DEFAULT << 4) /**< Shifted mode DEFAULT for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLTAMPDETFILTEN1_DISABLE                                   (_ETAMPDET_CFG_CHNLTAMPDETFILTEN1_DISABLE << 4) /**< Shifted mode DISABLE for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLTAMPDETFILTEN1_ENABLE                                    (_ETAMPDET_CFG_CHNLTAMPDETFILTEN1_ENABLE << 4)  /**< Shifted mode ENABLE for ETAMPDET_CFG        */
#define ETAMPDET_CFG_CHNLPADEN1                                                   (0x1UL << 5)                                    /**< enable driving pad                          */
#define _ETAMPDET_CFG_CHNLPADEN1_SHIFT                                            5                                               /**< Shift value for ETAMPDET_CHNLPADEN1         */
#define _ETAMPDET_CFG_CHNLPADEN1_MASK                                             0x20UL                                          /**< Bit mask for ETAMPDET_CHNLPADEN1            */
#define _ETAMPDET_CFG_CHNLPADEN1_DEFAULT                                          0x00000000UL                                    /**< Mode DEFAULT for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLPADEN1_DISABLE                                          0x00000000UL                                    /**< Mode DISABLE for ETAMPDET_CFG               */
#define _ETAMPDET_CFG_CHNLPADEN1_ENABLE                                           0x00000001UL                                    /**< Mode ENABLE for ETAMPDET_CFG                */
#define ETAMPDET_CFG_CHNLPADEN1_DEFAULT                                           (_ETAMPDET_CFG_CHNLPADEN1_DEFAULT << 5)         /**< Shifted mode DEFAULT for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLPADEN1_DISABLE                                           (_ETAMPDET_CFG_CHNLPADEN1_DISABLE << 5)         /**< Shifted mode DISABLE for ETAMPDET_CFG       */
#define ETAMPDET_CFG_CHNLPADEN1_ENABLE                                            (_ETAMPDET_CFG_CHNLPADEN1_ENABLE << 5)          /**< Shifted mode ENABLE for ETAMPDET_CFG        */

/* Bit fields for ETAMPDET CNTMISMATCHMAX */
#define _ETAMPDET_CNTMISMATCHMAX_RESETVALUE                                       0x00000000UL                                                               /**< Default value for ETAMPDET_CNTMISMATCHMAX   */
#define _ETAMPDET_CNTMISMATCHMAX_MASK                                             0x0000003FUL                                                               /**< Mask for ETAMPDET_CNTMISMATCHMAX            */
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_SHIFT                        0                                                                          /**< Shift value for ETAMPDET_CHNLCNTMISMATCHMAX0*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_MASK                         0x7UL                                                                      /**< Bit mask for ETAMPDET_CHNLCNTMISMATCHMAX0   */
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DEFAULT                      0x00000000UL                                                               /**< Mode DEFAULT for ETAMPDET_CNTMISMATCHMAX    */
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold1       0x00000000UL                                                               /**< Mode DetectFilterThreshold1 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold2       0x00000001UL                                                               /**< Mode DetectFilterThreshold2 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold3       0x00000002UL                                                               /**< Mode DetectFilterThreshold3 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold4       0x00000003UL                                                               /**< Mode DetectFilterThreshold4 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold5       0x00000004UL                                                               /**< Mode DetectFilterThreshold5 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold6       0x00000005UL                                                               /**< Mode DetectFilterThreshold6 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold7       0x00000006UL                                                               /**< Mode DetectFilterThreshold7 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold8       0x00000007UL                                                               /**< Mode DetectFilterThreshold8 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DEFAULT                       (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DEFAULT << 0)                /**< Shifted mode DEFAULT for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold1        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold1 << 0) /**< Shifted mode DetectFilterThreshold1 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold2        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold2 << 0) /**< Shifted mode DetectFilterThreshold2 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold3        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold3 << 0) /**< Shifted mode DetectFilterThreshold3 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold4        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold4 << 0) /**< Shifted mode DetectFilterThreshold4 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold5        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold5 << 0) /**< Shifted mode DetectFilterThreshold5 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold6        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold6 << 0) /**< Shifted mode DetectFilterThreshold6 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold7        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold7 << 0) /**< Shifted mode DetectFilterThreshold7 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold8        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold8 << 0) /**< Shifted mode DetectFilterThreshold8 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_SHIFT                        3                                                                          /**< Shift value for ETAMPDET_CHNLCNTMISMATCHMAX1*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_MASK                         0x38UL                                                                     /**< Bit mask for ETAMPDET_CHNLCNTMISMATCHMAX1   */
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DEFAULT                      0x00000000UL                                                               /**< Mode DEFAULT for ETAMPDET_CNTMISMATCHMAX    */
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold1       0x00000000UL                                                               /**< Mode DetectFilterThreshold1 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold2       0x00000001UL                                                               /**< Mode DetectFilterThreshold2 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold3       0x00000002UL                                                               /**< Mode DetectFilterThreshold3 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold4       0x00000003UL                                                               /**< Mode DetectFilterThreshold4 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold5       0x00000004UL                                                               /**< Mode DetectFilterThreshold5 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold6       0x00000005UL                                                               /**< Mode DetectFilterThreshold6 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold7       0x00000006UL                                                               /**< Mode DetectFilterThreshold7 for ETAMPDET_CNTMISMATCHMAX*/
#define _ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold8       0x00000007UL                                                               /**< Mode DetectFilterThreshold8 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DEFAULT                       (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DEFAULT << 3)                /**< Shifted mode DEFAULT for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold1        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold1 << 3) /**< Shifted mode DetectFilterThreshold1 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold2        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold2 << 3) /**< Shifted mode DetectFilterThreshold2 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold3        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold3 << 3) /**< Shifted mode DetectFilterThreshold3 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold4        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold4 << 3) /**< Shifted mode DetectFilterThreshold4 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold5        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold5 << 3) /**< Shifted mode DetectFilterThreshold5 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold6        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold6 << 3) /**< Shifted mode DetectFilterThreshold6 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold7        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold7 << 3) /**< Shifted mode DetectFilterThreshold7 for ETAMPDET_CNTMISMATCHMAX*/
#define ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold8        (_ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold8 << 3) /**< Shifted mode DetectFilterThreshold8 for ETAMPDET_CNTMISMATCHMAX*/

/* Bit fields for ETAMPDET CHNLFILTWINSIZE */
#define _ETAMPDET_CHNLFILTWINSIZE_RESETVALUE                                      0x00000000UL                                                                  /**< Default value for ETAMPDET_CHNLFILTWINSIZE  */
#define _ETAMPDET_CHNLFILTWINSIZE_MASK                                            0x000000FFUL                                                                  /**< Mask for ETAMPDET_CHNLFILTWINSIZE           */
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_SHIFT                          0                                                                             /**< Shift value for ETAMPDET_CHNLFILTWINSIZE0   */
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_MASK                           0xFUL                                                                         /**< Bit mask for ETAMPDET_CHNLFILTWINSIZE0      */
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DEFAULT                        0x00000000UL                                                                  /**< Mode DEFAULT for ETAMPDET_CHNLFILTWINSIZE   */
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_Reserved                       0x00000000UL                                                                  /**< Mode Reserved for ETAMPDET_CHNLFILTWINSIZE  */
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize2     0x00000001UL                                                                  /**< Mode DetectFilterMovingWinSize2 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize3     0x00000002UL                                                                  /**< Mode DetectFilterMovingWinSize3 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize4     0x00000003UL                                                                  /**< Mode DetectFilterMovingWinSize4 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize5     0x00000004UL                                                                  /**< Mode DetectFilterMovingWinSize5 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize6     0x00000005UL                                                                  /**< Mode DetectFilterMovingWinSize6 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize7     0x00000006UL                                                                  /**< Mode DetectFilterMovingWinSize7 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize8     0x00000007UL                                                                  /**< Mode DetectFilterMovingWinSize8 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize9     0x00000008UL                                                                  /**< Mode DetectFilterMovingWinSize9 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize10    0x00000009UL                                                                  /**< Mode DetectFilterMovingWinSize10 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize11    0x0000000AUL                                                                  /**< Mode DetectFilterMovingWinSize11 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize12    0x0000000BUL                                                                  /**< Mode DetectFilterMovingWinSize12 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize13    0x0000000CUL                                                                  /**< Mode DetectFilterMovingWinSize13 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize14    0x0000000DUL                                                                  /**< Mode DetectFilterMovingWinSize14 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize15    0x0000000EUL                                                                  /**< Mode DetectFilterMovingWinSize15 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize16    0x0000000FUL                                                                  /**< Mode DetectFilterMovingWinSize16 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DEFAULT                         (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DEFAULT << 0)                     /**< Shifted mode DEFAULT for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_Reserved                        (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_Reserved << 0)                    /**< Shifted mode Reserved for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize2      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize2 << 0)  /**< Shifted mode DetectFilterMovingWinSize2 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize3      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize3 << 0)  /**< Shifted mode DetectFilterMovingWinSize3 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize4      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize4 << 0)  /**< Shifted mode DetectFilterMovingWinSize4 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize5      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize5 << 0)  /**< Shifted mode DetectFilterMovingWinSize5 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize6      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize6 << 0)  /**< Shifted mode DetectFilterMovingWinSize6 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize7      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize7 << 0)  /**< Shifted mode DetectFilterMovingWinSize7 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize8      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize8 << 0)  /**< Shifted mode DetectFilterMovingWinSize8 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize9      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize9 << 0)  /**< Shifted mode DetectFilterMovingWinSize9 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize10     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize10 << 0) /**< Shifted mode DetectFilterMovingWinSize10 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize11     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize11 << 0) /**< Shifted mode DetectFilterMovingWinSize11 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize12     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize12 << 0) /**< Shifted mode DetectFilterMovingWinSize12 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize13     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize13 << 0) /**< Shifted mode DetectFilterMovingWinSize13 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize14     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize14 << 0) /**< Shifted mode DetectFilterMovingWinSize14 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize15     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize15 << 0) /**< Shifted mode DetectFilterMovingWinSize15 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize16     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize16 << 0) /**< Shifted mode DetectFilterMovingWinSize16 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_SHIFT                          4                                                                             /**< Shift value for ETAMPDET_CHNLFILTWINSIZE1   */
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_MASK                           0xF0UL                                                                        /**< Bit mask for ETAMPDET_CHNLFILTWINSIZE1      */
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DEFAULT                        0x00000000UL                                                                  /**< Mode DEFAULT for ETAMPDET_CHNLFILTWINSIZE   */
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_Reserved                       0x00000000UL                                                                  /**< Mode Reserved for ETAMPDET_CHNLFILTWINSIZE  */
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize2     0x00000001UL                                                                  /**< Mode DetectFilterMovingWinSize2 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize3     0x00000002UL                                                                  /**< Mode DetectFilterMovingWinSize3 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize4     0x00000003UL                                                                  /**< Mode DetectFilterMovingWinSize4 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize5     0x00000004UL                                                                  /**< Mode DetectFilterMovingWinSize5 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize6     0x00000005UL                                                                  /**< Mode DetectFilterMovingWinSize6 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize7     0x00000006UL                                                                  /**< Mode DetectFilterMovingWinSize7 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize8     0x00000007UL                                                                  /**< Mode DetectFilterMovingWinSize8 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize9     0x00000008UL                                                                  /**< Mode DetectFilterMovingWinSize9 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize10    0x00000009UL                                                                  /**< Mode DetectFilterMovingWinSize10 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize11    0x0000000AUL                                                                  /**< Mode DetectFilterMovingWinSize11 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize12    0x0000000BUL                                                                  /**< Mode DetectFilterMovingWinSize12 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize13    0x0000000CUL                                                                  /**< Mode DetectFilterMovingWinSize13 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize14    0x0000000DUL                                                                  /**< Mode DetectFilterMovingWinSize14 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize15    0x0000000EUL                                                                  /**< Mode DetectFilterMovingWinSize15 for ETAMPDET_CHNLFILTWINSIZE*/
#define _ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize16    0x0000000FUL                                                                  /**< Mode DetectFilterMovingWinSize16 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DEFAULT                         (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DEFAULT << 4)                     /**< Shifted mode DEFAULT for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_Reserved                        (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_Reserved << 4)                    /**< Shifted mode Reserved for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize2      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize2 << 4)  /**< Shifted mode DetectFilterMovingWinSize2 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize3      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize3 << 4)  /**< Shifted mode DetectFilterMovingWinSize3 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize4      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize4 << 4)  /**< Shifted mode DetectFilterMovingWinSize4 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize5      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize5 << 4)  /**< Shifted mode DetectFilterMovingWinSize5 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize6      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize6 << 4)  /**< Shifted mode DetectFilterMovingWinSize6 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize7      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize7 << 4)  /**< Shifted mode DetectFilterMovingWinSize7 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize8      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize8 << 4)  /**< Shifted mode DetectFilterMovingWinSize8 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize9      (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize9 << 4)  /**< Shifted mode DetectFilterMovingWinSize9 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize10     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize10 << 4) /**< Shifted mode DetectFilterMovingWinSize10 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize11     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize11 << 4) /**< Shifted mode DetectFilterMovingWinSize11 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize12     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize12 << 4) /**< Shifted mode DetectFilterMovingWinSize12 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize13     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize13 << 4) /**< Shifted mode DetectFilterMovingWinSize13 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize14     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize14 << 4) /**< Shifted mode DetectFilterMovingWinSize14 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize15     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize15 << 4) /**< Shifted mode DetectFilterMovingWinSize15 for ETAMPDET_CHNLFILTWINSIZE*/
#define ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize16     (_ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize16 << 4) /**< Shifted mode DetectFilterMovingWinSize16 for ETAMPDET_CHNLFILTWINSIZE*/

/* Bit fields for ETAMPDET CMD */
#define _ETAMPDET_CMD_RESETVALUE                                                  0x00000000UL                            /**< Default value for ETAMPDET_CMD              */
#define _ETAMPDET_CMD_MASK                                                        0x0000003FUL                            /**< Mask for ETAMPDET_CMD                       */
#define ETAMPDET_CMD_CHNLSTART0                                                   (0x1UL << 0)                            /**< Start channel 0 tamper detection            */
#define _ETAMPDET_CMD_CHNLSTART0_SHIFT                                            0                                       /**< Shift value for ETAMPDET_CHNLSTART0         */
#define _ETAMPDET_CMD_CHNLSTART0_MASK                                             0x1UL                                   /**< Bit mask for ETAMPDET_CHNLSTART0            */
#define _ETAMPDET_CMD_CHNLSTART0_DEFAULT                                          0x00000000UL                            /**< Mode DEFAULT for ETAMPDET_CMD               */
#define ETAMPDET_CMD_CHNLSTART0_DEFAULT                                           (_ETAMPDET_CMD_CHNLSTART0_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_CMD       */
#define ETAMPDET_CMD_CHNLSTOP0                                                    (0x1UL << 1)                            /**< Stop channel 0 tamper detection             */
#define _ETAMPDET_CMD_CHNLSTOP0_SHIFT                                             1                                       /**< Shift value for ETAMPDET_CHNLSTOP0          */
#define _ETAMPDET_CMD_CHNLSTOP0_MASK                                              0x2UL                                   /**< Bit mask for ETAMPDET_CHNLSTOP0             */
#define _ETAMPDET_CMD_CHNLSTOP0_DEFAULT                                           0x00000000UL                            /**< Mode DEFAULT for ETAMPDET_CMD               */
#define ETAMPDET_CMD_CHNLSTOP0_DEFAULT                                            (_ETAMPDET_CMD_CHNLSTOP0_DEFAULT << 1)  /**< Shifted mode DEFAULT for ETAMPDET_CMD       */
#define ETAMPDET_CMD_CHNLLOAD0                                                    (0x1UL << 2)                            /**< Start channel 0 tamper detection            */
#define _ETAMPDET_CMD_CHNLLOAD0_SHIFT                                             2                                       /**< Shift value for ETAMPDET_CHNLLOAD0          */
#define _ETAMPDET_CMD_CHNLLOAD0_MASK                                              0x4UL                                   /**< Bit mask for ETAMPDET_CHNLLOAD0             */
#define _ETAMPDET_CMD_CHNLLOAD0_DEFAULT                                           0x00000000UL                            /**< Mode DEFAULT for ETAMPDET_CMD               */
#define ETAMPDET_CMD_CHNLLOAD0_DEFAULT                                            (_ETAMPDET_CMD_CHNLLOAD0_DEFAULT << 2)  /**< Shifted mode DEFAULT for ETAMPDET_CMD       */
#define ETAMPDET_CMD_CHNLSTART1                                                   (0x1UL << 3)                            /**< Start channel 1 tamper detection            */
#define _ETAMPDET_CMD_CHNLSTART1_SHIFT                                            3                                       /**< Shift value for ETAMPDET_CHNLSTART1         */
#define _ETAMPDET_CMD_CHNLSTART1_MASK                                             0x8UL                                   /**< Bit mask for ETAMPDET_CHNLSTART1            */
#define _ETAMPDET_CMD_CHNLSTART1_DEFAULT                                          0x00000000UL                            /**< Mode DEFAULT for ETAMPDET_CMD               */
#define ETAMPDET_CMD_CHNLSTART1_DEFAULT                                           (_ETAMPDET_CMD_CHNLSTART1_DEFAULT << 3) /**< Shifted mode DEFAULT for ETAMPDET_CMD       */
#define ETAMPDET_CMD_CHNLSTOP1                                                    (0x1UL << 4)                            /**< Stop channel 1 tamper detection             */
#define _ETAMPDET_CMD_CHNLSTOP1_SHIFT                                             4                                       /**< Shift value for ETAMPDET_CHNLSTOP1          */
#define _ETAMPDET_CMD_CHNLSTOP1_MASK                                              0x10UL                                  /**< Bit mask for ETAMPDET_CHNLSTOP1             */
#define _ETAMPDET_CMD_CHNLSTOP1_DEFAULT                                           0x00000000UL                            /**< Mode DEFAULT for ETAMPDET_CMD               */
#define ETAMPDET_CMD_CHNLSTOP1_DEFAULT                                            (_ETAMPDET_CMD_CHNLSTOP1_DEFAULT << 4)  /**< Shifted mode DEFAULT for ETAMPDET_CMD       */
#define ETAMPDET_CMD_CHNLLOAD1                                                    (0x1UL << 5)                            /**< Start channel 1 tamper detection            */
#define _ETAMPDET_CMD_CHNLLOAD1_SHIFT                                             5                                       /**< Shift value for ETAMPDET_CHNLLOAD1          */
#define _ETAMPDET_CMD_CHNLLOAD1_MASK                                              0x20UL                                  /**< Bit mask for ETAMPDET_CHNLLOAD1             */
#define _ETAMPDET_CMD_CHNLLOAD1_DEFAULT                                           0x00000000UL                            /**< Mode DEFAULT for ETAMPDET_CMD               */
#define ETAMPDET_CMD_CHNLLOAD1_DEFAULT                                            (_ETAMPDET_CMD_CHNLLOAD1_DEFAULT << 5)  /**< Shifted mode DEFAULT for ETAMPDET_CMD       */

/* Bit fields for ETAMPDET SYNCBUSY */
#define _ETAMPDET_SYNCBUSY_RESETVALUE                                             0x00000000UL                                 /**< Default value for ETAMPDET_SYNCBUSY         */
#define _ETAMPDET_SYNCBUSY_MASK                                                   0x0000007FUL                                 /**< Mask for ETAMPDET_SYNCBUSY                  */
#define ETAMPDET_SYNCBUSY_CHNLSTART0                                              (0x1UL << 0)                                 /**< Synchronizer busy status                    */
#define _ETAMPDET_SYNCBUSY_CHNLSTART0_SHIFT                                       0                                            /**< Shift value for ETAMPDET_CHNLSTART0         */
#define _ETAMPDET_SYNCBUSY_CHNLSTART0_MASK                                        0x1UL                                        /**< Bit mask for ETAMPDET_CHNLSTART0            */
#define _ETAMPDET_SYNCBUSY_CHNLSTART0_DEFAULT                                     0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_SYNCBUSY          */
#define ETAMPDET_SYNCBUSY_CHNLSTART0_DEFAULT                                      (_ETAMPDET_SYNCBUSY_CHNLSTART0_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_SYNCBUSY  */
#define ETAMPDET_SYNCBUSY_CHNLSTOP0                                               (0x1UL << 1)                                 /**< Synchronizer busy status                    */
#define _ETAMPDET_SYNCBUSY_CHNLSTOP0_SHIFT                                        1                                            /**< Shift value for ETAMPDET_CHNLSTOP0          */
#define _ETAMPDET_SYNCBUSY_CHNLSTOP0_MASK                                         0x2UL                                        /**< Bit mask for ETAMPDET_CHNLSTOP0             */
#define _ETAMPDET_SYNCBUSY_CHNLSTOP0_DEFAULT                                      0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_SYNCBUSY          */
#define ETAMPDET_SYNCBUSY_CHNLSTOP0_DEFAULT                                       (_ETAMPDET_SYNCBUSY_CHNLSTOP0_DEFAULT << 1)  /**< Shifted mode DEFAULT for ETAMPDET_SYNCBUSY  */
#define ETAMPDET_SYNCBUSY_CHNLLOAD0                                               (0x1UL << 2)                                 /**< Synchronizer busy status                    */
#define _ETAMPDET_SYNCBUSY_CHNLLOAD0_SHIFT                                        2                                            /**< Shift value for ETAMPDET_CHNLLOAD0          */
#define _ETAMPDET_SYNCBUSY_CHNLLOAD0_MASK                                         0x4UL                                        /**< Bit mask for ETAMPDET_CHNLLOAD0             */
#define _ETAMPDET_SYNCBUSY_CHNLLOAD0_DEFAULT                                      0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_SYNCBUSY          */
#define ETAMPDET_SYNCBUSY_CHNLLOAD0_DEFAULT                                       (_ETAMPDET_SYNCBUSY_CHNLLOAD0_DEFAULT << 2)  /**< Shifted mode DEFAULT for ETAMPDET_SYNCBUSY  */
#define ETAMPDET_SYNCBUSY_CHNLSTART1                                              (0x1UL << 3)                                 /**< Synchronizer busy status                    */
#define _ETAMPDET_SYNCBUSY_CHNLSTART1_SHIFT                                       3                                            /**< Shift value for ETAMPDET_CHNLSTART1         */
#define _ETAMPDET_SYNCBUSY_CHNLSTART1_MASK                                        0x8UL                                        /**< Bit mask for ETAMPDET_CHNLSTART1            */
#define _ETAMPDET_SYNCBUSY_CHNLSTART1_DEFAULT                                     0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_SYNCBUSY          */
#define ETAMPDET_SYNCBUSY_CHNLSTART1_DEFAULT                                      (_ETAMPDET_SYNCBUSY_CHNLSTART1_DEFAULT << 3) /**< Shifted mode DEFAULT for ETAMPDET_SYNCBUSY  */
#define ETAMPDET_SYNCBUSY_CHNLSTOP1                                               (0x1UL << 4)                                 /**< Synchronizer busy status                    */
#define _ETAMPDET_SYNCBUSY_CHNLSTOP1_SHIFT                                        4                                            /**< Shift value for ETAMPDET_CHNLSTOP1          */
#define _ETAMPDET_SYNCBUSY_CHNLSTOP1_MASK                                         0x10UL                                       /**< Bit mask for ETAMPDET_CHNLSTOP1             */
#define _ETAMPDET_SYNCBUSY_CHNLSTOP1_DEFAULT                                      0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_SYNCBUSY          */
#define ETAMPDET_SYNCBUSY_CHNLSTOP1_DEFAULT                                       (_ETAMPDET_SYNCBUSY_CHNLSTOP1_DEFAULT << 4)  /**< Shifted mode DEFAULT for ETAMPDET_SYNCBUSY  */
#define ETAMPDET_SYNCBUSY_CHNLLOAD1                                               (0x1UL << 5)                                 /**< Synchronizer busy status                    */
#define _ETAMPDET_SYNCBUSY_CHNLLOAD1_SHIFT                                        5                                            /**< Shift value for ETAMPDET_CHNLLOAD1          */
#define _ETAMPDET_SYNCBUSY_CHNLLOAD1_MASK                                         0x20UL                                       /**< Bit mask for ETAMPDET_CHNLLOAD1             */
#define _ETAMPDET_SYNCBUSY_CHNLLOAD1_DEFAULT                                      0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_SYNCBUSY          */
#define ETAMPDET_SYNCBUSY_CHNLLOAD1_DEFAULT                                       (_ETAMPDET_SYNCBUSY_CHNLLOAD1_DEFAULT << 5)  /**< Shifted mode DEFAULT for ETAMPDET_SYNCBUSY  */
#define ETAMPDET_SYNCBUSY_EN                                                      (0x1UL << 6)                                 /**< Synchronizer busy status                    */
#define _ETAMPDET_SYNCBUSY_EN_SHIFT                                               6                                            /**< Shift value for ETAMPDET_EN                 */
#define _ETAMPDET_SYNCBUSY_EN_MASK                                                0x40UL                                       /**< Bit mask for ETAMPDET_EN                    */
#define _ETAMPDET_SYNCBUSY_EN_DEFAULT                                             0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_SYNCBUSY          */
#define ETAMPDET_SYNCBUSY_EN_DEFAULT                                              (_ETAMPDET_SYNCBUSY_EN_DEFAULT << 6)         /**< Shifted mode DEFAULT for ETAMPDET_SYNCBUSY  */

/* Bit fields for ETAMPDET IEN */
#define _ETAMPDET_IEN_RESETVALUE                                                  0x00000000UL                             /**< Default value for ETAMPDET_IEN              */
#define _ETAMPDET_IEN_MASK                                                        0x00000003UL                             /**< Mask for ETAMPDET_IEN                       */
#define ETAMPDET_IEN_TAMPDET0IEN                                                  (0x1UL << 0)                             /**< TAMPDET0 interrupt enable                   */
#define _ETAMPDET_IEN_TAMPDET0IEN_SHIFT                                           0                                        /**< Shift value for ETAMPDET_TAMPDET0IEN        */
#define _ETAMPDET_IEN_TAMPDET0IEN_MASK                                            0x1UL                                    /**< Bit mask for ETAMPDET_TAMPDET0IEN           */
#define _ETAMPDET_IEN_TAMPDET0IEN_DEFAULT                                         0x00000000UL                             /**< Mode DEFAULT for ETAMPDET_IEN               */
#define ETAMPDET_IEN_TAMPDET0IEN_DEFAULT                                          (_ETAMPDET_IEN_TAMPDET0IEN_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_IEN       */
#define ETAMPDET_IEN_TAMPDET1IEN                                                  (0x1UL << 1)                             /**< TAMPDET1 interrupt enable                   */
#define _ETAMPDET_IEN_TAMPDET1IEN_SHIFT                                           1                                        /**< Shift value for ETAMPDET_TAMPDET1IEN        */
#define _ETAMPDET_IEN_TAMPDET1IEN_MASK                                            0x2UL                                    /**< Bit mask for ETAMPDET_TAMPDET1IEN           */
#define _ETAMPDET_IEN_TAMPDET1IEN_DEFAULT                                         0x00000000UL                             /**< Mode DEFAULT for ETAMPDET_IEN               */
#define ETAMPDET_IEN_TAMPDET1IEN_DEFAULT                                          (_ETAMPDET_IEN_TAMPDET1IEN_DEFAULT << 1) /**< Shifted mode DEFAULT for ETAMPDET_IEN       */

/* Bit fields for ETAMPDET IF */
#define _ETAMPDET_IF_RESETVALUE                                                   0x00000000UL                           /**< Default value for ETAMPDET_IF               */
#define _ETAMPDET_IF_MASK                                                         0x00000003UL                           /**< Mask for ETAMPDET_IF                        */
#define ETAMPDET_IF_TAMPDET0IF                                                    (0x1UL << 0)                           /**< Tamper0 Detect Flag                         */
#define _ETAMPDET_IF_TAMPDET0IF_SHIFT                                             0                                      /**< Shift value for ETAMPDET_TAMPDET0IF         */
#define _ETAMPDET_IF_TAMPDET0IF_MASK                                              0x1UL                                  /**< Bit mask for ETAMPDET_TAMPDET0IF            */
#define _ETAMPDET_IF_TAMPDET0IF_DEFAULT                                           0x00000000UL                           /**< Mode DEFAULT for ETAMPDET_IF                */
#define ETAMPDET_IF_TAMPDET0IF_DEFAULT                                            (_ETAMPDET_IF_TAMPDET0IF_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_IF        */
#define ETAMPDET_IF_TAMPDET1IF                                                    (0x1UL << 1)                           /**< Tamper1 Detect Flag                         */
#define _ETAMPDET_IF_TAMPDET1IF_SHIFT                                             1                                      /**< Shift value for ETAMPDET_TAMPDET1IF         */
#define _ETAMPDET_IF_TAMPDET1IF_MASK                                              0x2UL                                  /**< Bit mask for ETAMPDET_TAMPDET1IF            */
#define _ETAMPDET_IF_TAMPDET1IF_DEFAULT                                           0x00000000UL                           /**< Mode DEFAULT for ETAMPDET_IF                */
#define ETAMPDET_IF_TAMPDET1IF_DEFAULT                                            (_ETAMPDET_IF_TAMPDET1IF_DEFAULT << 1) /**< Shifted mode DEFAULT for ETAMPDET_IF        */

/* Bit fields for ETAMPDET STATUS */
#define _ETAMPDET_STATUS_RESETVALUE                                               0x00000000UL                                 /**< Default value for ETAMPDET_STATUS           */
#define _ETAMPDET_STATUS_MASK                                                     0x80000003UL                                 /**< Mask for ETAMPDET_STATUS                    */
#define ETAMPDET_STATUS_CHNLRUNNING0                                              (0x1UL << 0)                                 /**< Channel0 Running Status                     */
#define _ETAMPDET_STATUS_CHNLRUNNING0_SHIFT                                       0                                            /**< Shift value for ETAMPDET_CHNLRUNNING0       */
#define _ETAMPDET_STATUS_CHNLRUNNING0_MASK                                        0x1UL                                        /**< Bit mask for ETAMPDET_CHNLRUNNING0          */
#define _ETAMPDET_STATUS_CHNLRUNNING0_DEFAULT                                     0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_STATUS            */
#define ETAMPDET_STATUS_CHNLRUNNING0_DEFAULT                                      (_ETAMPDET_STATUS_CHNLRUNNING0_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_STATUS    */
#define ETAMPDET_STATUS_CHNLRUNNING1                                              (0x1UL << 1)                                 /**< Channel1 Running Status                     */
#define _ETAMPDET_STATUS_CHNLRUNNING1_SHIFT                                       1                                            /**< Shift value for ETAMPDET_CHNLRUNNING1       */
#define _ETAMPDET_STATUS_CHNLRUNNING1_MASK                                        0x2UL                                        /**< Bit mask for ETAMPDET_CHNLRUNNING1          */
#define _ETAMPDET_STATUS_CHNLRUNNING1_DEFAULT                                     0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_STATUS            */
#define ETAMPDET_STATUS_CHNLRUNNING1_DEFAULT                                      (_ETAMPDET_STATUS_CHNLRUNNING1_DEFAULT << 1) /**< Shifted mode DEFAULT for ETAMPDET_STATUS    */
#define ETAMPDET_STATUS_LOCKSTATUS                                                (0x1UL << 31)                                /**< Lock Status                                 */
#define _ETAMPDET_STATUS_LOCKSTATUS_SHIFT                                         31                                           /**< Shift value for ETAMPDET_LOCKSTATUS         */
#define _ETAMPDET_STATUS_LOCKSTATUS_MASK                                          0x80000000UL                                 /**< Bit mask for ETAMPDET_LOCKSTATUS            */
#define _ETAMPDET_STATUS_LOCKSTATUS_DEFAULT                                       0x00000000UL                                 /**< Mode DEFAULT for ETAMPDET_STATUS            */
#define _ETAMPDET_STATUS_LOCKSTATUS_UNLOCKED                                      0x00000000UL                                 /**< Mode UNLOCKED for ETAMPDET_STATUS           */
#define _ETAMPDET_STATUS_LOCKSTATUS_LOCKED                                        0x00000001UL                                 /**< Mode LOCKED for ETAMPDET_STATUS             */
#define ETAMPDET_STATUS_LOCKSTATUS_DEFAULT                                        (_ETAMPDET_STATUS_LOCKSTATUS_DEFAULT << 31)  /**< Shifted mode DEFAULT for ETAMPDET_STATUS    */
#define ETAMPDET_STATUS_LOCKSTATUS_UNLOCKED                                       (_ETAMPDET_STATUS_LOCKSTATUS_UNLOCKED << 31) /**< Shifted mode UNLOCKED for ETAMPDET_STATUS   */
#define ETAMPDET_STATUS_LOCKSTATUS_LOCKED                                         (_ETAMPDET_STATUS_LOCKSTATUS_LOCKED << 31)   /**< Shifted mode LOCKED for ETAMPDET_STATUS     */

/* Bit fields for ETAMPDET EM4WUEN */
#define _ETAMPDET_EM4WUEN_RESETVALUE                                              0x00000000UL                                  /**< Default value for ETAMPDET_EM4WUEN          */
#define _ETAMPDET_EM4WUEN_MASK                                                    0x00000003UL                                  /**< Mask for ETAMPDET_EM4WUEN                   */
#define ETAMPDET_EM4WUEN_CHNLEM4WUEN0                                             (0x1UL << 0)                                  /**< Channel0 Tampdet EM4 Wakeup Enable          */
#define _ETAMPDET_EM4WUEN_CHNLEM4WUEN0_SHIFT                                      0                                             /**< Shift value for ETAMPDET_CHNLEM4WUEN0       */
#define _ETAMPDET_EM4WUEN_CHNLEM4WUEN0_MASK                                       0x1UL                                         /**< Bit mask for ETAMPDET_CHNLEM4WUEN0          */
#define _ETAMPDET_EM4WUEN_CHNLEM4WUEN0_DEFAULT                                    0x00000000UL                                  /**< Mode DEFAULT for ETAMPDET_EM4WUEN           */
#define ETAMPDET_EM4WUEN_CHNLEM4WUEN0_DEFAULT                                     (_ETAMPDET_EM4WUEN_CHNLEM4WUEN0_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_EM4WUEN   */
#define ETAMPDET_EM4WUEN_CHNLEM4WUEN1                                             (0x1UL << 1)                                  /**< Channel1 Tampdet EM4 Wakeup Enable          */
#define _ETAMPDET_EM4WUEN_CHNLEM4WUEN1_SHIFT                                      1                                             /**< Shift value for ETAMPDET_CHNLEM4WUEN1       */
#define _ETAMPDET_EM4WUEN_CHNLEM4WUEN1_MASK                                       0x2UL                                         /**< Bit mask for ETAMPDET_CHNLEM4WUEN1          */
#define _ETAMPDET_EM4WUEN_CHNLEM4WUEN1_DEFAULT                                    0x00000000UL                                  /**< Mode DEFAULT for ETAMPDET_EM4WUEN           */
#define ETAMPDET_EM4WUEN_CHNLEM4WUEN1_DEFAULT                                     (_ETAMPDET_EM4WUEN_CHNLEM4WUEN1_DEFAULT << 1) /**< Shifted mode DEFAULT for ETAMPDET_EM4WUEN   */

/* Bit fields for ETAMPDET CHNLSEEDVAL0 */
#define _ETAMPDET_CHNLSEEDVAL0_RESETVALUE                                         0x00000000UL                                       /**< Default value for ETAMPDET_CHNLSEEDVAL0     */
#define _ETAMPDET_CHNLSEEDVAL0_MASK                                               0xFFFFFFFFUL                                       /**< Mask for ETAMPDET_CHNLSEEDVAL0              */
#define _ETAMPDET_CHNLSEEDVAL0_CHNLSEEDVAL0_SHIFT                                 0                                                  /**< Shift value for ETAMPDET_CHNLSEEDVAL0       */
#define _ETAMPDET_CHNLSEEDVAL0_CHNLSEEDVAL0_MASK                                  0xFFFFFFFFUL                                       /**< Bit mask for ETAMPDET_CHNLSEEDVAL0          */
#define _ETAMPDET_CHNLSEEDVAL0_CHNLSEEDVAL0_DEFAULT                               0x00000000UL                                       /**< Mode DEFAULT for ETAMPDET_CHNLSEEDVAL0      */
#define ETAMPDET_CHNLSEEDVAL0_CHNLSEEDVAL0_DEFAULT                                (_ETAMPDET_CHNLSEEDVAL0_CHNLSEEDVAL0_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_CHNLSEEDVAL0*/

/* Bit fields for ETAMPDET CHNLSEEDVAL1 */
#define _ETAMPDET_CHNLSEEDVAL1_RESETVALUE                                         0x00000000UL                                       /**< Default value for ETAMPDET_CHNLSEEDVAL1     */
#define _ETAMPDET_CHNLSEEDVAL1_MASK                                               0xFFFFFFFFUL                                       /**< Mask for ETAMPDET_CHNLSEEDVAL1              */
#define _ETAMPDET_CHNLSEEDVAL1_CHNLSEEDVAL1_SHIFT                                 0                                                  /**< Shift value for ETAMPDET_CHNLSEEDVAL1       */
#define _ETAMPDET_CHNLSEEDVAL1_CHNLSEEDVAL1_MASK                                  0xFFFFFFFFUL                                       /**< Bit mask for ETAMPDET_CHNLSEEDVAL1          */
#define _ETAMPDET_CHNLSEEDVAL1_CHNLSEEDVAL1_DEFAULT                               0x00000000UL                                       /**< Mode DEFAULT for ETAMPDET_CHNLSEEDVAL1      */
#define ETAMPDET_CHNLSEEDVAL1_CHNLSEEDVAL1_DEFAULT                                (_ETAMPDET_CHNLSEEDVAL1_CHNLSEEDVAL1_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_CHNLSEEDVAL1*/

/* Bit fields for ETAMPDET CLKPRESCVAL */
#define _ETAMPDET_CLKPRESCVAL_RESETVALUE                                          0x00000000UL                                       /**< Default value for ETAMPDET_CLKPRESCVAL      */
#define _ETAMPDET_CLKPRESCVAL_MASK                                                0x0000073FUL                                       /**< Mask for ETAMPDET_CLKPRESCVAL               */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_SHIFT                                    0                                                  /**< Shift value for ETAMPDET_LOWERPRESC         */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_MASK                                     0x3FUL                                             /**< Bit mask for ETAMPDET_LOWERPRESC            */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DEFAULT                                  0x00000000UL                                       /**< Mode DEFAULT for ETAMPDET_CLKPRESCVAL       */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_Bypass                                   0x00000000UL                                       /**< Mode Bypass for ETAMPDET_CLKPRESCVAL        */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy2                                0x00000001UL                                       /**< Mode DivideBy2 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy3                                0x00000002UL                                       /**< Mode DivideBy3 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy4                                0x00000003UL                                       /**< Mode DivideBy4 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy5                                0x00000004UL                                       /**< Mode DivideBy5 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy6                                0x00000005UL                                       /**< Mode DivideBy6 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy7                                0x00000006UL                                       /**< Mode DivideBy7 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy8                                0x00000007UL                                       /**< Mode DivideBy8 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy9                                0x00000008UL                                       /**< Mode DivideBy9 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy10                               0x00000009UL                                       /**< Mode DivideBy10 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy11                               0x0000000AUL                                       /**< Mode DivideBy11 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy12                               0x0000000BUL                                       /**< Mode DivideBy12 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy13                               0x0000000CUL                                       /**< Mode DivideBy13 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy14                               0x0000000DUL                                       /**< Mode DivideBy14 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy15                               0x0000000EUL                                       /**< Mode DivideBy15 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy16                               0x0000000FUL                                       /**< Mode DivideBy16 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy17                               0x00000010UL                                       /**< Mode DivideBy17 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy18                               0x00000011UL                                       /**< Mode DivideBy18 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy19                               0x00000012UL                                       /**< Mode DivideBy19 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy20                               0x00000013UL                                       /**< Mode DivideBy20 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy21                               0x00000014UL                                       /**< Mode DivideBy21 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy22                               0x00000015UL                                       /**< Mode DivideBy22 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy23                               0x00000016UL                                       /**< Mode DivideBy23 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy24                               0x00000017UL                                       /**< Mode DivideBy24 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy25                               0x00000018UL                                       /**< Mode DivideBy25 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy26                               0x00000019UL                                       /**< Mode DivideBy26 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy27                               0x0000001AUL                                       /**< Mode DivideBy27 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy28                               0x0000001BUL                                       /**< Mode DivideBy28 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy29                               0x0000001CUL                                       /**< Mode DivideBy29 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy30                               0x0000001DUL                                       /**< Mode DivideBy30 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy31                               0x0000001EUL                                       /**< Mode DivideBy31 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy32                               0x0000001FUL                                       /**< Mode DivideBy32 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy33                               0x00000020UL                                       /**< Mode DivideBy33 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy34                               0x00000021UL                                       /**< Mode DivideBy34 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy35                               0x00000022UL                                       /**< Mode DivideBy35 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy36                               0x00000023UL                                       /**< Mode DivideBy36 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy37                               0x00000024UL                                       /**< Mode DivideBy37 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy38                               0x00000025UL                                       /**< Mode DivideBy38 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy39                               0x00000026UL                                       /**< Mode DivideBy39 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy40                               0x00000027UL                                       /**< Mode DivideBy40 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy41                               0x00000028UL                                       /**< Mode DivideBy41 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy42                               0x00000029UL                                       /**< Mode DivideBy42 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy43                               0x0000002AUL                                       /**< Mode DivideBy43 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy44                               0x0000002BUL                                       /**< Mode DivideBy44 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy45                               0x0000002CUL                                       /**< Mode DivideBy45 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy46                               0x0000002DUL                                       /**< Mode DivideBy46 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy47                               0x0000002EUL                                       /**< Mode DivideBy47 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy48                               0x0000002FUL                                       /**< Mode DivideBy48 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy49                               0x00000030UL                                       /**< Mode DivideBy49 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy50                               0x00000031UL                                       /**< Mode DivideBy50 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy51                               0x00000032UL                                       /**< Mode DivideBy51 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy52                               0x00000033UL                                       /**< Mode DivideBy52 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy53                               0x00000034UL                                       /**< Mode DivideBy53 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy54                               0x00000035UL                                       /**< Mode DivideBy54 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy55                               0x00000036UL                                       /**< Mode DivideBy55 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy56                               0x00000037UL                                       /**< Mode DivideBy56 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy57                               0x00000038UL                                       /**< Mode DivideBy57 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy58                               0x00000039UL                                       /**< Mode DivideBy58 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy59                               0x0000003AUL                                       /**< Mode DivideBy59 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy60                               0x0000003BUL                                       /**< Mode DivideBy60 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy61                               0x0000003CUL                                       /**< Mode DivideBy61 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy62                               0x0000003DUL                                       /**< Mode DivideBy62 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy63                               0x0000003EUL                                       /**< Mode DivideBy63 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy64                               0x0000003FUL                                       /**< Mode DivideBy64 for ETAMPDET_CLKPRESCVAL    */
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DEFAULT                                   (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DEFAULT << 0)    /**< Shifted mode DEFAULT for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_Bypass                                    (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_Bypass << 0)     /**< Shifted mode Bypass for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy2                                 (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy2 << 0)  /**< Shifted mode DivideBy2 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy3                                 (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy3 << 0)  /**< Shifted mode DivideBy3 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy4                                 (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy4 << 0)  /**< Shifted mode DivideBy4 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy5                                 (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy5 << 0)  /**< Shifted mode DivideBy5 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy6                                 (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy6 << 0)  /**< Shifted mode DivideBy6 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy7                                 (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy7 << 0)  /**< Shifted mode DivideBy7 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy8                                 (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy8 << 0)  /**< Shifted mode DivideBy8 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy9                                 (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy9 << 0)  /**< Shifted mode DivideBy9 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy10                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy10 << 0) /**< Shifted mode DivideBy10 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy11                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy11 << 0) /**< Shifted mode DivideBy11 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy12                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy12 << 0) /**< Shifted mode DivideBy12 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy13                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy13 << 0) /**< Shifted mode DivideBy13 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy14                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy14 << 0) /**< Shifted mode DivideBy14 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy15                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy15 << 0) /**< Shifted mode DivideBy15 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy16                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy16 << 0) /**< Shifted mode DivideBy16 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy17                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy17 << 0) /**< Shifted mode DivideBy17 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy18                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy18 << 0) /**< Shifted mode DivideBy18 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy19                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy19 << 0) /**< Shifted mode DivideBy19 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy20                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy20 << 0) /**< Shifted mode DivideBy20 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy21                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy21 << 0) /**< Shifted mode DivideBy21 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy22                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy22 << 0) /**< Shifted mode DivideBy22 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy23                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy23 << 0) /**< Shifted mode DivideBy23 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy24                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy24 << 0) /**< Shifted mode DivideBy24 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy25                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy25 << 0) /**< Shifted mode DivideBy25 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy26                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy26 << 0) /**< Shifted mode DivideBy26 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy27                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy27 << 0) /**< Shifted mode DivideBy27 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy28                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy28 << 0) /**< Shifted mode DivideBy28 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy29                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy29 << 0) /**< Shifted mode DivideBy29 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy30                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy30 << 0) /**< Shifted mode DivideBy30 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy31                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy31 << 0) /**< Shifted mode DivideBy31 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy32                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy32 << 0) /**< Shifted mode DivideBy32 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy33                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy33 << 0) /**< Shifted mode DivideBy33 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy34                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy34 << 0) /**< Shifted mode DivideBy34 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy35                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy35 << 0) /**< Shifted mode DivideBy35 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy36                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy36 << 0) /**< Shifted mode DivideBy36 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy37                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy37 << 0) /**< Shifted mode DivideBy37 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy38                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy38 << 0) /**< Shifted mode DivideBy38 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy39                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy39 << 0) /**< Shifted mode DivideBy39 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy40                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy40 << 0) /**< Shifted mode DivideBy40 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy41                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy41 << 0) /**< Shifted mode DivideBy41 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy42                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy42 << 0) /**< Shifted mode DivideBy42 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy43                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy43 << 0) /**< Shifted mode DivideBy43 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy44                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy44 << 0) /**< Shifted mode DivideBy44 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy45                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy45 << 0) /**< Shifted mode DivideBy45 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy46                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy46 << 0) /**< Shifted mode DivideBy46 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy47                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy47 << 0) /**< Shifted mode DivideBy47 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy48                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy48 << 0) /**< Shifted mode DivideBy48 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy49                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy49 << 0) /**< Shifted mode DivideBy49 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy50                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy50 << 0) /**< Shifted mode DivideBy50 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy51                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy51 << 0) /**< Shifted mode DivideBy51 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy52                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy52 << 0) /**< Shifted mode DivideBy52 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy53                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy53 << 0) /**< Shifted mode DivideBy53 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy54                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy54 << 0) /**< Shifted mode DivideBy54 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy55                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy55 << 0) /**< Shifted mode DivideBy55 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy56                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy56 << 0) /**< Shifted mode DivideBy56 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy57                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy57 << 0) /**< Shifted mode DivideBy57 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy58                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy58 << 0) /**< Shifted mode DivideBy58 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy59                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy59 << 0) /**< Shifted mode DivideBy59 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy60                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy60 << 0) /**< Shifted mode DivideBy60 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy61                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy61 << 0) /**< Shifted mode DivideBy61 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy62                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy62 << 0) /**< Shifted mode DivideBy62 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy63                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy63 << 0) /**< Shifted mode DivideBy63 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy64                                (_ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy64 << 0) /**< Shifted mode DivideBy64 for ETAMPDET_CLKPRESCVAL*/
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_SHIFT                                    8                                                  /**< Shift value for ETAMPDET_UPPERPRESC         */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_MASK                                     0x700UL                                            /**< Bit mask for ETAMPDET_UPPERPRESC            */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_DEFAULT                                  0x00000000UL                                       /**< Mode DEFAULT for ETAMPDET_CLKPRESCVAL       */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_Bypass                                   0x00000000UL                                       /**< Mode Bypass for ETAMPDET_CLKPRESCVAL        */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy2                                0x00000001UL                                       /**< Mode DivideBy2 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy4                                0x00000002UL                                       /**< Mode DivideBy4 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy8                                0x00000003UL                                       /**< Mode DivideBy8 for ETAMPDET_CLKPRESCVAL     */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy16                               0x00000004UL                                       /**< Mode DivideBy16 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy32                               0x00000005UL                                       /**< Mode DivideBy32 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy64                               0x00000006UL                                       /**< Mode DivideBy64 for ETAMPDET_CLKPRESCVAL    */
#define _ETAMPDET_CLKPRESCVAL_UPPERPRESC_Reserved                                 0x00000007UL                                       /**< Mode Reserved for ETAMPDET_CLKPRESCVAL      */
#define ETAMPDET_CLKPRESCVAL_UPPERPRESC_DEFAULT                                   (_ETAMPDET_CLKPRESCVAL_UPPERPRESC_DEFAULT << 8)    /**< Shifted mode DEFAULT for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_UPPERPRESC_Bypass                                    (_ETAMPDET_CLKPRESCVAL_UPPERPRESC_Bypass << 8)     /**< Shifted mode Bypass for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy2                                 (_ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy2 << 8)  /**< Shifted mode DivideBy2 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy4                                 (_ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy4 << 8)  /**< Shifted mode DivideBy4 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy8                                 (_ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy8 << 8)  /**< Shifted mode DivideBy8 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy16                                (_ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy16 << 8) /**< Shifted mode DivideBy16 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy32                                (_ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy32 << 8) /**< Shifted mode DivideBy32 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy64                                (_ETAMPDET_CLKPRESCVAL_UPPERPRESC_DivideBy64 << 8) /**< Shifted mode DivideBy64 for ETAMPDET_CLKPRESCVAL*/
#define ETAMPDET_CLKPRESCVAL_UPPERPRESC_Reserved                                  (_ETAMPDET_CLKPRESCVAL_UPPERPRESC_Reserved << 8)   /**< Shifted mode Reserved for ETAMPDET_CLKPRESCVAL*/

/* Bit fields for ETAMPDET LOCK */
#define _ETAMPDET_LOCK_RESETVALUE                                                 0x0000AEE8UL                          /**< Default value for ETAMPDET_LOCK             */
#define _ETAMPDET_LOCK_MASK                                                       0x0000FFFFUL                          /**< Mask for ETAMPDET_LOCK                      */
#define _ETAMPDET_LOCK_LOCKKEY_SHIFT                                              0                                     /**< Shift value for ETAMPDET_LOCKKEY            */
#define _ETAMPDET_LOCK_LOCKKEY_MASK                                               0xFFFFUL                              /**< Bit mask for ETAMPDET_LOCKKEY               */
#define _ETAMPDET_LOCK_LOCKKEY_DEFAULT                                            0x0000AEE8UL                          /**< Mode DEFAULT for ETAMPDET_LOCK              */
#define _ETAMPDET_LOCK_LOCKKEY_UNLOCK                                             0x0000AEE8UL                          /**< Mode UNLOCK for ETAMPDET_LOCK               */
#define ETAMPDET_LOCK_LOCKKEY_DEFAULT                                             (_ETAMPDET_LOCK_LOCKKEY_DEFAULT << 0) /**< Shifted mode DEFAULT for ETAMPDET_LOCK      */
#define ETAMPDET_LOCK_LOCKKEY_UNLOCK                                              (_ETAMPDET_LOCK_LOCKKEY_UNLOCK << 0)  /**< Shifted mode UNLOCK for ETAMPDET_LOCK       */

/** @} End of group EFR32MG27_ETAMPDET_BitFields */
/** @} End of group EFR32MG27_ETAMPDET */
/** @} End of group Parts */

#endif /* EFR32MG27_ETAMPDET_H */
