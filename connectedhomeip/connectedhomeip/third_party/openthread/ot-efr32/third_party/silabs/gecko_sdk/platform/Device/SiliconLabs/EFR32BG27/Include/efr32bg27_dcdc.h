/**************************************************************************//**
 * @file
 * @brief EFR32BG27 DCDC register and bit field definitions
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
#ifndef EFR32BG27_DCDC_H
#define EFR32BG27_DCDC_H
#define DCDC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32BG27_DCDC DCDC
 * @{
 * @brief EFR32BG27 DCDC Register Declaration.
 *****************************************************************************/

/** DCDC Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IPVERSION                                          */
  __IOM uint32_t CTRL;                          /**< Control                                            */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t EM01CTRL0;                     /**< EM01 Control                                       */
  __IOM uint32_t EM23CTRL0;                     /**< EM23 Control                                       */
  uint32_t       RESERVED1[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t BSTCTRL;                       /**< Boost Control Register                             */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t BSTEM01CTRL;                   /**< EM01 Boost Control                                 */
  __IOM uint32_t BSTEM23CTRL;                   /**< EM23 Boost Control                                 */
  uint32_t       RESERVED3[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF;                            /**< Interrupt Flags                                    */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IM uint32_t  SYNCBUSY;                      /**< Syncbusy Status Register                           */
  uint32_t       RESERVED4[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t CCCTRL;                        /**< Coulomb Counter Control                            */
  __IOM uint32_t CCCALCTRL;                     /**< Coulomb Counter Calibration Control                */
  __IOM uint32_t CCCMD;                         /**< Coulomb Counter Command                            */
  __IM uint32_t  CCEM0CNT;                      /**< Coulomb Counter EM0 Count Value                    */
  __IM uint32_t  CCEM2CNT;                      /**< Coulomb Counter EM2 Count Value                    */
  __IOM uint32_t CCTHR;                         /**< Coulomb Counter Threshold                          */
  __IOM uint32_t CCIF;                          /**< Coulomb Counter Interrupt Flag                     */
  __IOM uint32_t CCIEN;                         /**< Coulomb Counter Interrupt Enable                   */
  __IM uint32_t  CCSTATUS;                      /**< Coulomb Counter Status                             */
  uint32_t       RESERVED5[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Lock Register                                      */
  __IM uint32_t  LOCKSTATUS;                    /**< Lock Status Register                               */
  uint32_t       RESERVED6[2U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED7[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED8[7U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED9[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED10[975U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IPVERSION                                          */
  __IOM uint32_t CTRL_SET;                      /**< Control                                            */
  uint32_t       RESERVED11[1U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01CTRL0_SET;                 /**< EM01 Control                                       */
  __IOM uint32_t EM23CTRL0_SET;                 /**< EM23 Control                                       */
  uint32_t       RESERVED12[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BSTCTRL_SET;                   /**< Boost Control Register                             */
  uint32_t       RESERVED13[1U];                /**< Reserved for future use                            */
  __IOM uint32_t BSTEM01CTRL_SET;               /**< EM01 Boost Control                                 */
  __IOM uint32_t BSTEM23CTRL_SET;               /**< EM23 Boost Control                                 */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IM uint32_t  SYNCBUSY_SET;                  /**< Syncbusy Status Register                           */
  uint32_t       RESERVED15[7U];                /**< Reserved for future use                            */
  __IOM uint32_t CCCTRL_SET;                    /**< Coulomb Counter Control                            */
  __IOM uint32_t CCCALCTRL_SET;                 /**< Coulomb Counter Calibration Control                */
  __IOM uint32_t CCCMD_SET;                     /**< Coulomb Counter Command                            */
  __IM uint32_t  CCEM0CNT_SET;                  /**< Coulomb Counter EM0 Count Value                    */
  __IM uint32_t  CCEM2CNT_SET;                  /**< Coulomb Counter EM2 Count Value                    */
  __IOM uint32_t CCTHR_SET;                     /**< Coulomb Counter Threshold                          */
  __IOM uint32_t CCIF_SET;                      /**< Coulomb Counter Interrupt Flag                     */
  __IOM uint32_t CCIEN_SET;                     /**< Coulomb Counter Interrupt Enable                   */
  __IM uint32_t  CCSTATUS_SET;                  /**< Coulomb Counter Status                             */
  uint32_t       RESERVED16[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Lock Register                                      */
  __IM uint32_t  LOCKSTATUS_SET;                /**< Lock Status Register                               */
  uint32_t       RESERVED17[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED18[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED19[7U];                /**< Reserved for future use                            */
  uint32_t       RESERVED20[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED21[975U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IPVERSION                                          */
  __IOM uint32_t CTRL_CLR;                      /**< Control                                            */
  uint32_t       RESERVED22[1U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01CTRL0_CLR;                 /**< EM01 Control                                       */
  __IOM uint32_t EM23CTRL0_CLR;                 /**< EM23 Control                                       */
  uint32_t       RESERVED23[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BSTCTRL_CLR;                   /**< Boost Control Register                             */
  uint32_t       RESERVED24[1U];                /**< Reserved for future use                            */
  __IOM uint32_t BSTEM01CTRL_CLR;               /**< EM01 Boost Control                                 */
  __IOM uint32_t BSTEM23CTRL_CLR;               /**< EM23 Boost Control                                 */
  uint32_t       RESERVED25[1U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< Syncbusy Status Register                           */
  uint32_t       RESERVED26[7U];                /**< Reserved for future use                            */
  __IOM uint32_t CCCTRL_CLR;                    /**< Coulomb Counter Control                            */
  __IOM uint32_t CCCALCTRL_CLR;                 /**< Coulomb Counter Calibration Control                */
  __IOM uint32_t CCCMD_CLR;                     /**< Coulomb Counter Command                            */
  __IM uint32_t  CCEM0CNT_CLR;                  /**< Coulomb Counter EM0 Count Value                    */
  __IM uint32_t  CCEM2CNT_CLR;                  /**< Coulomb Counter EM2 Count Value                    */
  __IOM uint32_t CCTHR_CLR;                     /**< Coulomb Counter Threshold                          */
  __IOM uint32_t CCIF_CLR;                      /**< Coulomb Counter Interrupt Flag                     */
  __IOM uint32_t CCIEN_CLR;                     /**< Coulomb Counter Interrupt Enable                   */
  __IM uint32_t  CCSTATUS_CLR;                  /**< Coulomb Counter Status                             */
  uint32_t       RESERVED27[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Lock Register                                      */
  __IM uint32_t  LOCKSTATUS_CLR;                /**< Lock Status Register                               */
  uint32_t       RESERVED28[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED29[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED30[7U];                /**< Reserved for future use                            */
  uint32_t       RESERVED31[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED32[975U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IPVERSION                                          */
  __IOM uint32_t CTRL_TGL;                      /**< Control                                            */
  uint32_t       RESERVED33[1U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01CTRL0_TGL;                 /**< EM01 Control                                       */
  __IOM uint32_t EM23CTRL0_TGL;                 /**< EM23 Control                                       */
  uint32_t       RESERVED34[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BSTCTRL_TGL;                   /**< Boost Control Register                             */
  uint32_t       RESERVED35[1U];                /**< Reserved for future use                            */
  __IOM uint32_t BSTEM01CTRL_TGL;               /**< EM01 Boost Control                                 */
  __IOM uint32_t BSTEM23CTRL_TGL;               /**< EM23 Boost Control                                 */
  uint32_t       RESERVED36[1U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< Syncbusy Status Register                           */
  uint32_t       RESERVED37[7U];                /**< Reserved for future use                            */
  __IOM uint32_t CCCTRL_TGL;                    /**< Coulomb Counter Control                            */
  __IOM uint32_t CCCALCTRL_TGL;                 /**< Coulomb Counter Calibration Control                */
  __IOM uint32_t CCCMD_TGL;                     /**< Coulomb Counter Command                            */
  __IM uint32_t  CCEM0CNT_TGL;                  /**< Coulomb Counter EM0 Count Value                    */
  __IM uint32_t  CCEM2CNT_TGL;                  /**< Coulomb Counter EM2 Count Value                    */
  __IOM uint32_t CCTHR_TGL;                     /**< Coulomb Counter Threshold                          */
  __IOM uint32_t CCIF_TGL;                      /**< Coulomb Counter Interrupt Flag                     */
  __IOM uint32_t CCIEN_TGL;                     /**< Coulomb Counter Interrupt Enable                   */
  __IM uint32_t  CCSTATUS_TGL;                  /**< Coulomb Counter Status                             */
  uint32_t       RESERVED38[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Lock Register                                      */
  __IM uint32_t  LOCKSTATUS_TGL;                /**< Lock Status Register                               */
  uint32_t       RESERVED39[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED40[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED41[7U];                /**< Reserved for future use                            */
  uint32_t       RESERVED42[1U];                /**< Reserved for future use                            */
} DCDC_TypeDef;
/** @} End of group EFR32BG27_DCDC */

/**************************************************************************//**
 * @addtogroup EFR32BG27_DCDC
 * @{
 * @defgroup EFR32BG27_DCDC_BitFields DCDC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for DCDC IPVERSION */
#define _DCDC_IPVERSION_RESETVALUE                    0x00000003UL                             /**< Default value for DCDC_IPVERSION            */
#define _DCDC_IPVERSION_MASK                          0xFFFFFFFFUL                             /**< Mask for DCDC_IPVERSION                     */
#define _DCDC_IPVERSION_IPVERSION_SHIFT               0                                        /**< Shift value for DCDC_IPVERSION              */
#define _DCDC_IPVERSION_IPVERSION_MASK                0xFFFFFFFFUL                             /**< Bit mask for DCDC_IPVERSION                 */
#define _DCDC_IPVERSION_IPVERSION_DEFAULT             0x00000003UL                             /**< Mode DEFAULT for DCDC_IPVERSION             */
#define DCDC_IPVERSION_IPVERSION_DEFAULT              (_DCDC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for DCDC_IPVERSION     */

/* Bit fields for DCDC CTRL */
#define _DCDC_CTRL_RESETVALUE                         0x00000040UL                              /**< Default value for DCDC_CTRL                 */
#define _DCDC_CTRL_MASK                               0x00000071UL                              /**< Mask for DCDC_CTRL                          */
#define DCDC_CTRL_MODE                                (0x1UL << 0)                              /**< DCDC/Bypass Mode Control                    */
#define _DCDC_CTRL_MODE_SHIFT                         0                                         /**< Shift value for DCDC_MODE                   */
#define _DCDC_CTRL_MODE_MASK                          0x1UL                                     /**< Bit mask for DCDC_MODE                      */
#define _DCDC_CTRL_MODE_DEFAULT                       0x00000000UL                              /**< Mode DEFAULT for DCDC_CTRL                  */
#define _DCDC_CTRL_MODE_BYPASS                        0x00000000UL                              /**< Mode BYPASS for DCDC_CTRL                   */
#define _DCDC_CTRL_MODE_DCDCREGULATION                0x00000001UL                              /**< Mode DCDCREGULATION for DCDC_CTRL           */
#define DCDC_CTRL_MODE_DEFAULT                        (_DCDC_CTRL_MODE_DEFAULT << 0)            /**< Shifted mode DEFAULT for DCDC_CTRL          */
#define DCDC_CTRL_MODE_BYPASS                         (_DCDC_CTRL_MODE_BYPASS << 0)             /**< Shifted mode BYPASS for DCDC_CTRL           */
#define DCDC_CTRL_MODE_DCDCREGULATION                 (_DCDC_CTRL_MODE_DCDCREGULATION << 0)     /**< Shifted mode DCDCREGULATION for DCDC_CTRL   */
#define _DCDC_CTRL_IPKTMAXCTRL_SHIFT                  4                                         /**< Shift value for DCDC_IPKTMAXCTRL            */
#define _DCDC_CTRL_IPKTMAXCTRL_MASK                   0x70UL                                    /**< Bit mask for DCDC_IPKTMAXCTRL               */
#define _DCDC_CTRL_IPKTMAXCTRL_DEFAULT                0x00000004UL                              /**< Mode DEFAULT for DCDC_CTRL                  */
#define _DCDC_CTRL_IPKTMAXCTRL_OFF                    0x00000000UL                              /**< Mode OFF for DCDC_CTRL                      */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_0P35us            0x00000001UL                              /**< Mode TMAX_0P35us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_0P63us            0x00000002UL                              /**< Mode TMAX_0P63us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_0P91us            0x00000003UL                              /**< Mode TMAX_0P91us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_1P19us            0x00000004UL                              /**< Mode TMAX_1P19us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_1P47us            0x00000005UL                              /**< Mode TMAX_1P47us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_1P75us            0x00000006UL                              /**< Mode TMAX_1P75us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_2P03us            0x00000007UL                              /**< Mode TMAX_2P03us for DCDC_CTRL              */
#define DCDC_CTRL_IPKTMAXCTRL_DEFAULT                 (_DCDC_CTRL_IPKTMAXCTRL_DEFAULT << 4)     /**< Shifted mode DEFAULT for DCDC_CTRL          */
#define DCDC_CTRL_IPKTMAXCTRL_OFF                     (_DCDC_CTRL_IPKTMAXCTRL_OFF << 4)         /**< Shifted mode OFF for DCDC_CTRL              */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_0P35us             (_DCDC_CTRL_IPKTMAXCTRL_TMAX_0P35us << 4) /**< Shifted mode TMAX_0P35us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_0P63us             (_DCDC_CTRL_IPKTMAXCTRL_TMAX_0P63us << 4) /**< Shifted mode TMAX_0P63us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_0P91us             (_DCDC_CTRL_IPKTMAXCTRL_TMAX_0P91us << 4) /**< Shifted mode TMAX_0P91us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_1P19us             (_DCDC_CTRL_IPKTMAXCTRL_TMAX_1P19us << 4) /**< Shifted mode TMAX_1P19us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_1P47us             (_DCDC_CTRL_IPKTMAXCTRL_TMAX_1P47us << 4) /**< Shifted mode TMAX_1P47us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_1P75us             (_DCDC_CTRL_IPKTMAXCTRL_TMAX_1P75us << 4) /**< Shifted mode TMAX_1P75us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_2P03us             (_DCDC_CTRL_IPKTMAXCTRL_TMAX_2P03us << 4) /**< Shifted mode TMAX_2P03us for DCDC_CTRL      */

/* Bit fields for DCDC EM01CTRL0 */
#define _DCDC_EM01CTRL0_RESETVALUE                    0x00000109UL                                    /**< Default value for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_MASK                          0x0000030FUL                                    /**< Mask for DCDC_EM01CTRL0                     */
#define _DCDC_EM01CTRL0_IPKVAL_SHIFT                  0                                               /**< Shift value for DCDC_IPKVAL                 */
#define _DCDC_EM01CTRL0_IPKVAL_MASK                   0xFUL                                           /**< Bit mask for DCDC_IPKVAL                    */
#define _DCDC_EM01CTRL0_IPKVAL_DEFAULT                0x00000009UL                                    /**< Mode DEFAULT for DCDC_EM01CTRL0             */
#define _DCDC_EM01CTRL0_IPKVAL_Load36mA               0x00000003UL                                    /**< Mode Load36mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load40mA               0x00000004UL                                    /**< Mode Load40mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load44mA               0x00000005UL                                    /**< Mode Load44mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load48mA               0x00000006UL                                    /**< Mode Load48mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load52mA               0x00000007UL                                    /**< Mode Load52mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load56mA               0x00000008UL                                    /**< Mode Load56mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load60mA               0x00000009UL                                    /**< Mode Load60mA for DCDC_EM01CTRL0            */
#define DCDC_EM01CTRL0_IPKVAL_DEFAULT                 (_DCDC_EM01CTRL0_IPKVAL_DEFAULT << 0)           /**< Shifted mode DEFAULT for DCDC_EM01CTRL0     */
#define DCDC_EM01CTRL0_IPKVAL_Load36mA                (_DCDC_EM01CTRL0_IPKVAL_Load36mA << 0)          /**< Shifted mode Load36mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load40mA                (_DCDC_EM01CTRL0_IPKVAL_Load40mA << 0)          /**< Shifted mode Load40mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load44mA                (_DCDC_EM01CTRL0_IPKVAL_Load44mA << 0)          /**< Shifted mode Load44mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load48mA                (_DCDC_EM01CTRL0_IPKVAL_Load48mA << 0)          /**< Shifted mode Load48mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load52mA                (_DCDC_EM01CTRL0_IPKVAL_Load52mA << 0)          /**< Shifted mode Load52mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load56mA                (_DCDC_EM01CTRL0_IPKVAL_Load56mA << 0)          /**< Shifted mode Load56mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load60mA                (_DCDC_EM01CTRL0_IPKVAL_Load60mA << 0)          /**< Shifted mode Load60mA for DCDC_EM01CTRL0    */
#define _DCDC_EM01CTRL0_DRVSPEED_SHIFT                8                                               /**< Shift value for DCDC_DRVSPEED               */
#define _DCDC_EM01CTRL0_DRVSPEED_MASK                 0x300UL                                         /**< Bit mask for DCDC_DRVSPEED                  */
#define _DCDC_EM01CTRL0_DRVSPEED_DEFAULT              0x00000001UL                                    /**< Mode DEFAULT for DCDC_EM01CTRL0             */
#define _DCDC_EM01CTRL0_DRVSPEED_BEST_EMI             0x00000000UL                                    /**< Mode BEST_EMI for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING      0x00000001UL                                    /**< Mode DEFAULT_SETTING for DCDC_EM01CTRL0     */
#define _DCDC_EM01CTRL0_DRVSPEED_INTERMEDIATE         0x00000002UL                                    /**< Mode INTERMEDIATE for DCDC_EM01CTRL0        */
#define _DCDC_EM01CTRL0_DRVSPEED_BEST_EFFICIENCY      0x00000003UL                                    /**< Mode BEST_EFFICIENCY for DCDC_EM01CTRL0     */
#define DCDC_EM01CTRL0_DRVSPEED_DEFAULT               (_DCDC_EM01CTRL0_DRVSPEED_DEFAULT << 8)         /**< Shifted mode DEFAULT for DCDC_EM01CTRL0     */
#define DCDC_EM01CTRL0_DRVSPEED_BEST_EMI              (_DCDC_EM01CTRL0_DRVSPEED_BEST_EMI << 8)        /**< Shifted mode BEST_EMI for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING       (_DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING << 8) /**< Shifted mode DEFAULT_SETTING for DCDC_EM01CTRL0*/
#define DCDC_EM01CTRL0_DRVSPEED_INTERMEDIATE          (_DCDC_EM01CTRL0_DRVSPEED_INTERMEDIATE << 8)    /**< Shifted mode INTERMEDIATE for DCDC_EM01CTRL0*/
#define DCDC_EM01CTRL0_DRVSPEED_BEST_EFFICIENCY       (_DCDC_EM01CTRL0_DRVSPEED_BEST_EFFICIENCY << 8) /**< Shifted mode BEST_EFFICIENCY for DCDC_EM01CTRL0*/

/* Bit fields for DCDC EM23CTRL0 */
#define _DCDC_EM23CTRL0_RESETVALUE                    0x00000103UL                                    /**< Default value for DCDC_EM23CTRL0            */
#define _DCDC_EM23CTRL0_MASK                          0x0000030FUL                                    /**< Mask for DCDC_EM23CTRL0                     */
#define _DCDC_EM23CTRL0_IPKVAL_SHIFT                  0                                               /**< Shift value for DCDC_IPKVAL                 */
#define _DCDC_EM23CTRL0_IPKVAL_MASK                   0xFUL                                           /**< Bit mask for DCDC_IPKVAL                    */
#define _DCDC_EM23CTRL0_IPKVAL_DEFAULT                0x00000003UL                                    /**< Mode DEFAULT for DCDC_EM23CTRL0             */
#define _DCDC_EM23CTRL0_IPKVAL_Load5mA                0x00000003UL                                    /**< Mode Load5mA for DCDC_EM23CTRL0             */
#define _DCDC_EM23CTRL0_IPKVAL_Load10mA               0x00000009UL                                    /**< Mode Load10mA for DCDC_EM23CTRL0            */
#define DCDC_EM23CTRL0_IPKVAL_DEFAULT                 (_DCDC_EM23CTRL0_IPKVAL_DEFAULT << 0)           /**< Shifted mode DEFAULT for DCDC_EM23CTRL0     */
#define DCDC_EM23CTRL0_IPKVAL_Load5mA                 (_DCDC_EM23CTRL0_IPKVAL_Load5mA << 0)           /**< Shifted mode Load5mA for DCDC_EM23CTRL0     */
#define DCDC_EM23CTRL0_IPKVAL_Load10mA                (_DCDC_EM23CTRL0_IPKVAL_Load10mA << 0)          /**< Shifted mode Load10mA for DCDC_EM23CTRL0    */
#define _DCDC_EM23CTRL0_DRVSPEED_SHIFT                8                                               /**< Shift value for DCDC_DRVSPEED               */
#define _DCDC_EM23CTRL0_DRVSPEED_MASK                 0x300UL                                         /**< Bit mask for DCDC_DRVSPEED                  */
#define _DCDC_EM23CTRL0_DRVSPEED_DEFAULT              0x00000001UL                                    /**< Mode DEFAULT for DCDC_EM23CTRL0             */
#define _DCDC_EM23CTRL0_DRVSPEED_BEST_EMI             0x00000000UL                                    /**< Mode BEST_EMI for DCDC_EM23CTRL0            */
#define _DCDC_EM23CTRL0_DRVSPEED_DEFAULT_SETTING      0x00000001UL                                    /**< Mode DEFAULT_SETTING for DCDC_EM23CTRL0     */
#define _DCDC_EM23CTRL0_DRVSPEED_INTERMEDIATE         0x00000002UL                                    /**< Mode INTERMEDIATE for DCDC_EM23CTRL0        */
#define _DCDC_EM23CTRL0_DRVSPEED_BEST_EFFICIENCY      0x00000003UL                                    /**< Mode BEST_EFFICIENCY for DCDC_EM23CTRL0     */
#define DCDC_EM23CTRL0_DRVSPEED_DEFAULT               (_DCDC_EM23CTRL0_DRVSPEED_DEFAULT << 8)         /**< Shifted mode DEFAULT for DCDC_EM23CTRL0     */
#define DCDC_EM23CTRL0_DRVSPEED_BEST_EMI              (_DCDC_EM23CTRL0_DRVSPEED_BEST_EMI << 8)        /**< Shifted mode BEST_EMI for DCDC_EM23CTRL0    */
#define DCDC_EM23CTRL0_DRVSPEED_DEFAULT_SETTING       (_DCDC_EM23CTRL0_DRVSPEED_DEFAULT_SETTING << 8) /**< Shifted mode DEFAULT_SETTING for DCDC_EM23CTRL0*/
#define DCDC_EM23CTRL0_DRVSPEED_INTERMEDIATE          (_DCDC_EM23CTRL0_DRVSPEED_INTERMEDIATE << 8)    /**< Shifted mode INTERMEDIATE for DCDC_EM23CTRL0*/
#define DCDC_EM23CTRL0_DRVSPEED_BEST_EFFICIENCY       (_DCDC_EM23CTRL0_DRVSPEED_BEST_EFFICIENCY << 8) /**< Shifted mode BEST_EFFICIENCY for DCDC_EM23CTRL0*/

/* Bit fields for DCDC BSTCTRL */
#define _DCDC_BSTCTRL_RESETVALUE                      0x00000047UL                                 /**< Default value for DCDC_BSTCTRL              */
#define _DCDC_BSTCTRL_MASK                            0x00000077UL                                 /**< Mask for DCDC_BSTCTRL                       */
#define _DCDC_BSTCTRL_BSTTOFFMAX_SHIFT                0                                            /**< Shift value for DCDC_BSTTOFFMAX             */
#define _DCDC_BSTCTRL_BSTTOFFMAX_MASK                 0x7UL                                        /**< Bit mask for DCDC_BSTTOFFMAX                */
#define _DCDC_BSTCTRL_BSTTOFFMAX_DEFAULT              0x00000007UL                                 /**< Mode DEFAULT for DCDC_BSTCTRL               */
#define _DCDC_BSTCTRL_BSTTOFFMAX_OFF                  0x00000000UL                                 /**< Mode OFF for DCDC_BSTCTRL                   */
#define _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P35us          0x00000001UL                                 /**< Mode TMAX_0P35us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P63us          0x00000002UL                                 /**< Mode TMAX_0P63us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P91us          0x00000003UL                                 /**< Mode TMAX_0P91us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P19us          0x00000004UL                                 /**< Mode TMAX_1P19us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P47us          0x00000005UL                                 /**< Mode TMAX_1P47us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P75us          0x00000006UL                                 /**< Mode TMAX_1P75us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_BSTTOFFMAX_TMAX_2P03us          0x00000007UL                                 /**< Mode TMAX_2P03us for DCDC_BSTCTRL           */
#define DCDC_BSTCTRL_BSTTOFFMAX_DEFAULT               (_DCDC_BSTCTRL_BSTTOFFMAX_DEFAULT << 0)      /**< Shifted mode DEFAULT for DCDC_BSTCTRL       */
#define DCDC_BSTCTRL_BSTTOFFMAX_OFF                   (_DCDC_BSTCTRL_BSTTOFFMAX_OFF << 0)          /**< Shifted mode OFF for DCDC_BSTCTRL           */
#define DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P35us           (_DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P35us << 0)  /**< Shifted mode TMAX_0P35us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P63us           (_DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P63us << 0)  /**< Shifted mode TMAX_0P63us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P91us           (_DCDC_BSTCTRL_BSTTOFFMAX_TMAX_0P91us << 0)  /**< Shifted mode TMAX_0P91us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P19us           (_DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P19us << 0)  /**< Shifted mode TMAX_1P19us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P47us           (_DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P47us << 0)  /**< Shifted mode TMAX_1P47us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P75us           (_DCDC_BSTCTRL_BSTTOFFMAX_TMAX_1P75us << 0)  /**< Shifted mode TMAX_1P75us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_BSTTOFFMAX_TMAX_2P03us           (_DCDC_BSTCTRL_BSTTOFFMAX_TMAX_2P03us << 0)  /**< Shifted mode TMAX_2P03us for DCDC_BSTCTRL   */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_SHIFT               4                                            /**< Shift value for DCDC_IPKTMAXCTRL            */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_MASK                0x70UL                                       /**< Bit mask for DCDC_IPKTMAXCTRL               */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_DEFAULT             0x00000004UL                                 /**< Mode DEFAULT for DCDC_BSTCTRL               */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_OFF                 0x00000000UL                                 /**< Mode OFF for DCDC_BSTCTRL                   */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P35us         0x00000001UL                                 /**< Mode TMAX_0P35us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P63us         0x00000002UL                                 /**< Mode TMAX_0P63us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P91us         0x00000003UL                                 /**< Mode TMAX_0P91us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P19us         0x00000004UL                                 /**< Mode TMAX_1P19us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P47us         0x00000005UL                                 /**< Mode TMAX_1P47us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P75us         0x00000006UL                                 /**< Mode TMAX_1P75us for DCDC_BSTCTRL           */
#define _DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_2P03us         0x00000007UL                                 /**< Mode TMAX_2P03us for DCDC_BSTCTRL           */
#define DCDC_BSTCTRL_IPKTMAXCTRL_DEFAULT              (_DCDC_BSTCTRL_IPKTMAXCTRL_DEFAULT << 4)     /**< Shifted mode DEFAULT for DCDC_BSTCTRL       */
#define DCDC_BSTCTRL_IPKTMAXCTRL_OFF                  (_DCDC_BSTCTRL_IPKTMAXCTRL_OFF << 4)         /**< Shifted mode OFF for DCDC_BSTCTRL           */
#define DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P35us          (_DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P35us << 4) /**< Shifted mode TMAX_0P35us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P63us          (_DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P63us << 4) /**< Shifted mode TMAX_0P63us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P91us          (_DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_0P91us << 4) /**< Shifted mode TMAX_0P91us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P19us          (_DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P19us << 4) /**< Shifted mode TMAX_1P19us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P47us          (_DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P47us << 4) /**< Shifted mode TMAX_1P47us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P75us          (_DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_1P75us << 4) /**< Shifted mode TMAX_1P75us for DCDC_BSTCTRL   */
#define DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_2P03us          (_DCDC_BSTCTRL_IPKTMAXCTRL_TMAX_2P03us << 4) /**< Shifted mode TMAX_2P03us for DCDC_BSTCTRL   */

/* Bit fields for DCDC BSTEM01CTRL */
#define _DCDC_BSTEM01CTRL_RESETVALUE                  0x0000010CUL                                      /**< Default value for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_MASK                        0x0000030FUL                                      /**< Mask for DCDC_BSTEM01CTRL                   */
#define _DCDC_BSTEM01CTRL_IPKVAL_SHIFT                0                                                 /**< Shift value for DCDC_IPKVAL                 */
#define _DCDC_BSTEM01CTRL_IPKVAL_MASK                 0xFUL                                             /**< Bit mask for DCDC_IPKVAL                    */
#define _DCDC_BSTEM01CTRL_IPKVAL_DEFAULT              0x0000000CUL                                      /**< Mode DEFAULT for DCDC_BSTEM01CTRL           */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load10mA             0x00000003UL                                      /**< Mode Load10mA for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load11mA             0x00000004UL                                      /**< Mode Load11mA for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load13mA             0x00000005UL                                      /**< Mode Load13mA for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load15mA             0x00000006UL                                      /**< Mode Load15mA for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load16mA             0x00000007UL                                      /**< Mode Load16mA for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load18mA             0x00000008UL                                      /**< Mode Load18mA for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load20mA             0x00000009UL                                      /**< Mode Load20mA for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load21mA             0x0000000AUL                                      /**< Mode Load21mA for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load23mA             0x0000000BUL                                      /**< Mode Load23mA for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_IPKVAL_Load25mA             0x0000000CUL                                      /**< Mode Load25mA for DCDC_BSTEM01CTRL          */
#define DCDC_BSTEM01CTRL_IPKVAL_DEFAULT               (_DCDC_BSTEM01CTRL_IPKVAL_DEFAULT << 0)           /**< Shifted mode DEFAULT for DCDC_BSTEM01CTRL   */
#define DCDC_BSTEM01CTRL_IPKVAL_Load10mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load10mA << 0)          /**< Shifted mode Load10mA for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_IPKVAL_Load11mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load11mA << 0)          /**< Shifted mode Load11mA for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_IPKVAL_Load13mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load13mA << 0)          /**< Shifted mode Load13mA for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_IPKVAL_Load15mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load15mA << 0)          /**< Shifted mode Load15mA for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_IPKVAL_Load16mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load16mA << 0)          /**< Shifted mode Load16mA for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_IPKVAL_Load18mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load18mA << 0)          /**< Shifted mode Load18mA for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_IPKVAL_Load20mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load20mA << 0)          /**< Shifted mode Load20mA for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_IPKVAL_Load21mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load21mA << 0)          /**< Shifted mode Load21mA for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_IPKVAL_Load23mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load23mA << 0)          /**< Shifted mode Load23mA for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_IPKVAL_Load25mA              (_DCDC_BSTEM01CTRL_IPKVAL_Load25mA << 0)          /**< Shifted mode Load25mA for DCDC_BSTEM01CTRL  */
#define _DCDC_BSTEM01CTRL_DRVSPEED_SHIFT              8                                                 /**< Shift value for DCDC_DRVSPEED               */
#define _DCDC_BSTEM01CTRL_DRVSPEED_MASK               0x300UL                                           /**< Bit mask for DCDC_DRVSPEED                  */
#define _DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT            0x00000001UL                                      /**< Mode DEFAULT for DCDC_BSTEM01CTRL           */
#define _DCDC_BSTEM01CTRL_DRVSPEED_BEST_EMI           0x00000000UL                                      /**< Mode BEST_EMI for DCDC_BSTEM01CTRL          */
#define _DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT_SETTING    0x00000001UL                                      /**< Mode DEFAULT_SETTING for DCDC_BSTEM01CTRL   */
#define _DCDC_BSTEM01CTRL_DRVSPEED_INTERMEDIATE       0x00000002UL                                      /**< Mode INTERMEDIATE for DCDC_BSTEM01CTRL      */
#define _DCDC_BSTEM01CTRL_DRVSPEED_BEST_EFFICIENCY    0x00000003UL                                      /**< Mode BEST_EFFICIENCY for DCDC_BSTEM01CTRL   */
#define DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT             (_DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT << 8)         /**< Shifted mode DEFAULT for DCDC_BSTEM01CTRL   */
#define DCDC_BSTEM01CTRL_DRVSPEED_BEST_EMI            (_DCDC_BSTEM01CTRL_DRVSPEED_BEST_EMI << 8)        /**< Shifted mode BEST_EMI for DCDC_BSTEM01CTRL  */
#define DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT_SETTING     (_DCDC_BSTEM01CTRL_DRVSPEED_DEFAULT_SETTING << 8) /**< Shifted mode DEFAULT_SETTING for DCDC_BSTEM01CTRL*/
#define DCDC_BSTEM01CTRL_DRVSPEED_INTERMEDIATE        (_DCDC_BSTEM01CTRL_DRVSPEED_INTERMEDIATE << 8)    /**< Shifted mode INTERMEDIATE for DCDC_BSTEM01CTRL*/
#define DCDC_BSTEM01CTRL_DRVSPEED_BEST_EFFICIENCY     (_DCDC_BSTEM01CTRL_DRVSPEED_BEST_EFFICIENCY << 8) /**< Shifted mode BEST_EFFICIENCY for DCDC_BSTEM01CTRL*/

/* Bit fields for DCDC BSTEM23CTRL */
#define _DCDC_BSTEM23CTRL_RESETVALUE                  0x00000109UL                                      /**< Default value for DCDC_BSTEM23CTRL          */
#define _DCDC_BSTEM23CTRL_MASK                        0x0000030FUL                                      /**< Mask for DCDC_BSTEM23CTRL                   */
#define _DCDC_BSTEM23CTRL_IPKVAL_SHIFT                0                                                 /**< Shift value for DCDC_IPKVAL                 */
#define _DCDC_BSTEM23CTRL_IPKVAL_MASK                 0xFUL                                             /**< Bit mask for DCDC_IPKVAL                    */
#define _DCDC_BSTEM23CTRL_IPKVAL_DEFAULT              0x00000009UL                                      /**< Mode DEFAULT for DCDC_BSTEM23CTRL           */
#define _DCDC_BSTEM23CTRL_IPKVAL_Load10mA             0x00000009UL                                      /**< Mode Load10mA for DCDC_BSTEM23CTRL          */
#define DCDC_BSTEM23CTRL_IPKVAL_DEFAULT               (_DCDC_BSTEM23CTRL_IPKVAL_DEFAULT << 0)           /**< Shifted mode DEFAULT for DCDC_BSTEM23CTRL   */
#define DCDC_BSTEM23CTRL_IPKVAL_Load10mA              (_DCDC_BSTEM23CTRL_IPKVAL_Load10mA << 0)          /**< Shifted mode Load10mA for DCDC_BSTEM23CTRL  */
#define _DCDC_BSTEM23CTRL_DRVSPEED_SHIFT              8                                                 /**< Shift value for DCDC_DRVSPEED               */
#define _DCDC_BSTEM23CTRL_DRVSPEED_MASK               0x300UL                                           /**< Bit mask for DCDC_DRVSPEED                  */
#define _DCDC_BSTEM23CTRL_DRVSPEED_DEFAULT            0x00000001UL                                      /**< Mode DEFAULT for DCDC_BSTEM23CTRL           */
#define _DCDC_BSTEM23CTRL_DRVSPEED_BEST_EMI           0x00000000UL                                      /**< Mode BEST_EMI for DCDC_BSTEM23CTRL          */
#define _DCDC_BSTEM23CTRL_DRVSPEED_DEFAULT_SETTING    0x00000001UL                                      /**< Mode DEFAULT_SETTING for DCDC_BSTEM23CTRL   */
#define _DCDC_BSTEM23CTRL_DRVSPEED_INTERMEDIATE       0x00000002UL                                      /**< Mode INTERMEDIATE for DCDC_BSTEM23CTRL      */
#define _DCDC_BSTEM23CTRL_DRVSPEED_BEST_EFFICIENCY    0x00000003UL                                      /**< Mode BEST_EFFICIENCY for DCDC_BSTEM23CTRL   */
#define DCDC_BSTEM23CTRL_DRVSPEED_DEFAULT             (_DCDC_BSTEM23CTRL_DRVSPEED_DEFAULT << 8)         /**< Shifted mode DEFAULT for DCDC_BSTEM23CTRL   */
#define DCDC_BSTEM23CTRL_DRVSPEED_BEST_EMI            (_DCDC_BSTEM23CTRL_DRVSPEED_BEST_EMI << 8)        /**< Shifted mode BEST_EMI for DCDC_BSTEM23CTRL  */
#define DCDC_BSTEM23CTRL_DRVSPEED_DEFAULT_SETTING     (_DCDC_BSTEM23CTRL_DRVSPEED_DEFAULT_SETTING << 8) /**< Shifted mode DEFAULT_SETTING for DCDC_BSTEM23CTRL*/
#define DCDC_BSTEM23CTRL_DRVSPEED_INTERMEDIATE        (_DCDC_BSTEM23CTRL_DRVSPEED_INTERMEDIATE << 8)    /**< Shifted mode INTERMEDIATE for DCDC_BSTEM23CTRL*/
#define DCDC_BSTEM23CTRL_DRVSPEED_BEST_EFFICIENCY     (_DCDC_BSTEM23CTRL_DRVSPEED_BEST_EFFICIENCY << 8) /**< Shifted mode BEST_EFFICIENCY for DCDC_BSTEM23CTRL*/

/* Bit fields for DCDC IF */
#define _DCDC_IF_RESETVALUE                           0x00000000UL                       /**< Default value for DCDC_IF                   */
#define _DCDC_IF_MASK                                 0x000000FFUL                       /**< Mask for DCDC_IF                            */
#define DCDC_IF_BYPSW                                 (0x1UL << 0)                       /**< Bypass Switch Enabled                       */
#define _DCDC_IF_BYPSW_SHIFT                          0                                  /**< Shift value for DCDC_BYPSW                  */
#define _DCDC_IF_BYPSW_MASK                           0x1UL                              /**< Bit mask for DCDC_BYPSW                     */
#define _DCDC_IF_BYPSW_DEFAULT                        0x00000000UL                       /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_BYPSW_DEFAULT                         (_DCDC_IF_BYPSW_DEFAULT << 0)      /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_WARM                                  (0x1UL << 1)                       /**< DCDC Warmup Time Done                       */
#define _DCDC_IF_WARM_SHIFT                           1                                  /**< Shift value for DCDC_WARM                   */
#define _DCDC_IF_WARM_MASK                            0x2UL                              /**< Bit mask for DCDC_WARM                      */
#define _DCDC_IF_WARM_DEFAULT                         0x00000000UL                       /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_WARM_DEFAULT                          (_DCDC_IF_WARM_DEFAULT << 1)       /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_RUNNING                               (0x1UL << 2)                       /**< DCDC Running                                */
#define _DCDC_IF_RUNNING_SHIFT                        2                                  /**< Shift value for DCDC_RUNNING                */
#define _DCDC_IF_RUNNING_MASK                         0x4UL                              /**< Bit mask for DCDC_RUNNING                   */
#define _DCDC_IF_RUNNING_DEFAULT                      0x00000000UL                       /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_RUNNING_DEFAULT                       (_DCDC_IF_RUNNING_DEFAULT << 2)    /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_VREGINLOW                             (0x1UL << 3)                       /**< VREGVDD below threshold                     */
#define _DCDC_IF_VREGINLOW_SHIFT                      3                                  /**< Shift value for DCDC_VREGINLOW              */
#define _DCDC_IF_VREGINLOW_MASK                       0x8UL                              /**< Bit mask for DCDC_VREGINLOW                 */
#define _DCDC_IF_VREGINLOW_DEFAULT                    0x00000000UL                       /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_VREGINLOW_DEFAULT                     (_DCDC_IF_VREGINLOW_DEFAULT << 3)  /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_VREGINHIGH                            (0x1UL << 4)                       /**< VREGVDD above threshold                     */
#define _DCDC_IF_VREGINHIGH_SHIFT                     4                                  /**< Shift value for DCDC_VREGINHIGH             */
#define _DCDC_IF_VREGINHIGH_MASK                      0x10UL                             /**< Bit mask for DCDC_VREGINHIGH                */
#define _DCDC_IF_VREGINHIGH_DEFAULT                   0x00000000UL                       /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_VREGINHIGH_DEFAULT                    (_DCDC_IF_VREGINHIGH_DEFAULT << 4) /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_REGULATION                            (0x1UL << 5)                       /**< DCDC in regulation                          */
#define _DCDC_IF_REGULATION_SHIFT                     5                                  /**< Shift value for DCDC_REGULATION             */
#define _DCDC_IF_REGULATION_MASK                      0x20UL                             /**< Bit mask for DCDC_REGULATION                */
#define _DCDC_IF_REGULATION_DEFAULT                   0x00000000UL                       /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_REGULATION_DEFAULT                    (_DCDC_IF_REGULATION_DEFAULT << 5) /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_TMAX                                  (0x1UL << 6)                       /**< Buck Max Ton/Boost Max Toff reached         */
#define _DCDC_IF_TMAX_SHIFT                           6                                  /**< Shift value for DCDC_TMAX                   */
#define _DCDC_IF_TMAX_MASK                            0x40UL                             /**< Bit mask for DCDC_TMAX                      */
#define _DCDC_IF_TMAX_DEFAULT                         0x00000000UL                       /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_TMAX_DEFAULT                          (_DCDC_IF_TMAX_DEFAULT << 6)       /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_EM4ERR                                (0x1UL << 7)                       /**< EM4 Entry Request Error                     */
#define _DCDC_IF_EM4ERR_SHIFT                         7                                  /**< Shift value for DCDC_EM4ERR                 */
#define _DCDC_IF_EM4ERR_MASK                          0x80UL                             /**< Bit mask for DCDC_EM4ERR                    */
#define _DCDC_IF_EM4ERR_DEFAULT                       0x00000000UL                       /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_EM4ERR_DEFAULT                        (_DCDC_IF_EM4ERR_DEFAULT << 7)     /**< Shifted mode DEFAULT for DCDC_IF            */

/* Bit fields for DCDC IEN */
#define _DCDC_IEN_RESETVALUE                          0x00000000UL                        /**< Default value for DCDC_IEN                  */
#define _DCDC_IEN_MASK                                0x000000FFUL                        /**< Mask for DCDC_IEN                           */
#define DCDC_IEN_BYPSW                                (0x1UL << 0)                        /**< Bypass Switch Enabled Interrupt Enable      */
#define _DCDC_IEN_BYPSW_SHIFT                         0                                   /**< Shift value for DCDC_BYPSW                  */
#define _DCDC_IEN_BYPSW_MASK                          0x1UL                               /**< Bit mask for DCDC_BYPSW                     */
#define _DCDC_IEN_BYPSW_DEFAULT                       0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_BYPSW_DEFAULT                        (_DCDC_IEN_BYPSW_DEFAULT << 0)      /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_WARM                                 (0x1UL << 1)                        /**< DCDC Warmup Time Done Interrupt Enable      */
#define _DCDC_IEN_WARM_SHIFT                          1                                   /**< Shift value for DCDC_WARM                   */
#define _DCDC_IEN_WARM_MASK                           0x2UL                               /**< Bit mask for DCDC_WARM                      */
#define _DCDC_IEN_WARM_DEFAULT                        0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_WARM_DEFAULT                         (_DCDC_IEN_WARM_DEFAULT << 1)       /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_RUNNING                              (0x1UL << 2)                        /**< DCDC Running Interrupt Enable               */
#define _DCDC_IEN_RUNNING_SHIFT                       2                                   /**< Shift value for DCDC_RUNNING                */
#define _DCDC_IEN_RUNNING_MASK                        0x4UL                               /**< Bit mask for DCDC_RUNNING                   */
#define _DCDC_IEN_RUNNING_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_RUNNING_DEFAULT                      (_DCDC_IEN_RUNNING_DEFAULT << 2)    /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_VREGINLOW                            (0x1UL << 3)                        /**< VREGVDD below threshold Interrupt Enable    */
#define _DCDC_IEN_VREGINLOW_SHIFT                     3                                   /**< Shift value for DCDC_VREGINLOW              */
#define _DCDC_IEN_VREGINLOW_MASK                      0x8UL                               /**< Bit mask for DCDC_VREGINLOW                 */
#define _DCDC_IEN_VREGINLOW_DEFAULT                   0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_VREGINLOW_DEFAULT                    (_DCDC_IEN_VREGINLOW_DEFAULT << 3)  /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_VREGINHIGH                           (0x1UL << 4)                        /**< VREGVDD above threshold Interrupt Enable    */
#define _DCDC_IEN_VREGINHIGH_SHIFT                    4                                   /**< Shift value for DCDC_VREGINHIGH             */
#define _DCDC_IEN_VREGINHIGH_MASK                     0x10UL                              /**< Bit mask for DCDC_VREGINHIGH                */
#define _DCDC_IEN_VREGINHIGH_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_VREGINHIGH_DEFAULT                   (_DCDC_IEN_VREGINHIGH_DEFAULT << 4) /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_REGULATION                           (0x1UL << 5)                        /**< DCDC in Regulation Interrupt Enable         */
#define _DCDC_IEN_REGULATION_SHIFT                    5                                   /**< Shift value for DCDC_REGULATION             */
#define _DCDC_IEN_REGULATION_MASK                     0x20UL                              /**< Bit mask for DCDC_REGULATION                */
#define _DCDC_IEN_REGULATION_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_REGULATION_DEFAULT                   (_DCDC_IEN_REGULATION_DEFAULT << 5) /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_TMAX                                 (0x1UL << 6)                        /**< Ton_max Timeout Interrupt Enable            */
#define _DCDC_IEN_TMAX_SHIFT                          6                                   /**< Shift value for DCDC_TMAX                   */
#define _DCDC_IEN_TMAX_MASK                           0x40UL                              /**< Bit mask for DCDC_TMAX                      */
#define _DCDC_IEN_TMAX_DEFAULT                        0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_TMAX_DEFAULT                         (_DCDC_IEN_TMAX_DEFAULT << 6)       /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_EM4ERR                               (0x1UL << 7)                        /**< EM4 Entry Req Interrupt Enable              */
#define _DCDC_IEN_EM4ERR_SHIFT                        7                                   /**< Shift value for DCDC_EM4ERR                 */
#define _DCDC_IEN_EM4ERR_MASK                         0x80UL                              /**< Bit mask for DCDC_EM4ERR                    */
#define _DCDC_IEN_EM4ERR_DEFAULT                      0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_EM4ERR_DEFAULT                       (_DCDC_IEN_EM4ERR_DEFAULT << 7)     /**< Shifted mode DEFAULT for DCDC_IEN           */

/* Bit fields for DCDC STATUS */
#define _DCDC_STATUS_RESETVALUE                       0x00000000UL                          /**< Default value for DCDC_STATUS               */
#define _DCDC_STATUS_MASK                             0x0000001FUL                          /**< Mask for DCDC_STATUS                        */
#define DCDC_STATUS_BYPSW                             (0x1UL << 0)                          /**< Bypass Switch is currently enabled          */
#define _DCDC_STATUS_BYPSW_SHIFT                      0                                     /**< Shift value for DCDC_BYPSW                  */
#define _DCDC_STATUS_BYPSW_MASK                       0x1UL                                 /**< Bit mask for DCDC_BYPSW                     */
#define _DCDC_STATUS_BYPSW_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_BYPSW_DEFAULT                     (_DCDC_STATUS_BYPSW_DEFAULT << 0)     /**< Shifted mode DEFAULT for DCDC_STATUS        */
#define DCDC_STATUS_WARM                              (0x1UL << 1)                          /**< DCDC Warmup Done                            */
#define _DCDC_STATUS_WARM_SHIFT                       1                                     /**< Shift value for DCDC_WARM                   */
#define _DCDC_STATUS_WARM_MASK                        0x2UL                                 /**< Bit mask for DCDC_WARM                      */
#define _DCDC_STATUS_WARM_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_WARM_DEFAULT                      (_DCDC_STATUS_WARM_DEFAULT << 1)      /**< Shifted mode DEFAULT for DCDC_STATUS        */
#define DCDC_STATUS_RUNNING                           (0x1UL << 2)                          /**< DCDC is running                             */
#define _DCDC_STATUS_RUNNING_SHIFT                    2                                     /**< Shift value for DCDC_RUNNING                */
#define _DCDC_STATUS_RUNNING_MASK                     0x4UL                                 /**< Bit mask for DCDC_RUNNING                   */
#define _DCDC_STATUS_RUNNING_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_RUNNING_DEFAULT                   (_DCDC_STATUS_RUNNING_DEFAULT << 2)   /**< Shifted mode DEFAULT for DCDC_STATUS        */
#define DCDC_STATUS_VREGIN                            (0x1UL << 3)                          /**< VREGVDD comparator status                   */
#define _DCDC_STATUS_VREGIN_SHIFT                     3                                     /**< Shift value for DCDC_VREGIN                 */
#define _DCDC_STATUS_VREGIN_MASK                      0x8UL                                 /**< Bit mask for DCDC_VREGIN                    */
#define _DCDC_STATUS_VREGIN_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_VREGIN_DEFAULT                    (_DCDC_STATUS_VREGIN_DEFAULT << 3)    /**< Shifted mode DEFAULT for DCDC_STATUS        */
#define DCDC_STATUS_BYPCMPOUT                         (0x1UL << 4)                          /**< Bypass Comparator Output                    */
#define _DCDC_STATUS_BYPCMPOUT_SHIFT                  4                                     /**< Shift value for DCDC_BYPCMPOUT              */
#define _DCDC_STATUS_BYPCMPOUT_MASK                   0x10UL                                /**< Bit mask for DCDC_BYPCMPOUT                 */
#define _DCDC_STATUS_BYPCMPOUT_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_BYPCMPOUT_DEFAULT                 (_DCDC_STATUS_BYPCMPOUT_DEFAULT << 4) /**< Shifted mode DEFAULT for DCDC_STATUS        */

/* Bit fields for DCDC SYNCBUSY */
#define _DCDC_SYNCBUSY_RESETVALUE                     0x00000000UL                           /**< Default value for DCDC_SYNCBUSY             */
#define _DCDC_SYNCBUSY_MASK                           0x00000001UL                           /**< Mask for DCDC_SYNCBUSY                      */
#define DCDC_SYNCBUSY_SYNCBUSY                        (0x1UL << 0)                           /**< Combined Sync Busy Status                   */
#define _DCDC_SYNCBUSY_SYNCBUSY_SHIFT                 0                                      /**< Shift value for DCDC_SYNCBUSY               */
#define _DCDC_SYNCBUSY_SYNCBUSY_MASK                  0x1UL                                  /**< Bit mask for DCDC_SYNCBUSY                  */
#define _DCDC_SYNCBUSY_SYNCBUSY_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for DCDC_SYNCBUSY              */
#define DCDC_SYNCBUSY_SYNCBUSY_DEFAULT                (_DCDC_SYNCBUSY_SYNCBUSY_DEFAULT << 0) /**< Shifted mode DEFAULT for DCDC_SYNCBUSY      */

/* Bit fields for DCDC CCCTRL */
#define _DCDC_CCCTRL_RESETVALUE                       0x00000000UL                      /**< Default value for DCDC_CCCTRL               */
#define _DCDC_CCCTRL_MASK                             0x00000001UL                      /**< Mask for DCDC_CCCTRL                        */
#define DCDC_CCCTRL_CCEN                              (0x1UL << 0)                      /**< Coulomb Counter Enable                      */
#define _DCDC_CCCTRL_CCEN_SHIFT                       0                                 /**< Shift value for DCDC_CCEN                   */
#define _DCDC_CCCTRL_CCEN_MASK                        0x1UL                             /**< Bit mask for DCDC_CCEN                      */
#define _DCDC_CCCTRL_CCEN_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for DCDC_CCCTRL                */
#define DCDC_CCCTRL_CCEN_DEFAULT                      (_DCDC_CCCTRL_CCEN_DEFAULT << 0)  /**< Shifted mode DEFAULT for DCDC_CCCTRL        */

/* Bit fields for DCDC CCCALCTRL */
#define _DCDC_CCCALCTRL_RESETVALUE                    0x00000000UL                             /**< Default value for DCDC_CCCALCTRL            */
#define _DCDC_CCCALCTRL_MASK                          0x0000030FUL                             /**< Mask for DCDC_CCCALCTRL                     */
#define DCDC_CCCALCTRL_CCLOADEN                       (0x1UL << 0)                             /**< CC Load Circuit Enable                      */
#define _DCDC_CCCALCTRL_CCLOADEN_SHIFT                0                                        /**< Shift value for DCDC_CCLOADEN               */
#define _DCDC_CCCALCTRL_CCLOADEN_MASK                 0x1UL                                    /**< Bit mask for DCDC_CCLOADEN                  */
#define _DCDC_CCCALCTRL_CCLOADEN_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for DCDC_CCCALCTRL             */
#define DCDC_CCCALCTRL_CCLOADEN_DEFAULT               (_DCDC_CCCALCTRL_CCLOADEN_DEFAULT << 0)  /**< Shifted mode DEFAULT for DCDC_CCCALCTRL     */
#define _DCDC_CCCALCTRL_CCLVL_SHIFT                   1                                        /**< Shift value for DCDC_CCLVL                  */
#define _DCDC_CCCALCTRL_CCLVL_MASK                    0xEUL                                    /**< Bit mask for DCDC_CCLVL                     */
#define _DCDC_CCCALCTRL_CCLVL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for DCDC_CCCALCTRL             */
#define _DCDC_CCCALCTRL_CCLVL_LOAD0                   0x00000000UL                             /**< Mode LOAD0 for DCDC_CCCALCTRL               */
#define _DCDC_CCCALCTRL_CCLVL_LOAD1                   0x00000001UL                             /**< Mode LOAD1 for DCDC_CCCALCTRL               */
#define _DCDC_CCCALCTRL_CCLVL_LOAD2                   0x00000002UL                             /**< Mode LOAD2 for DCDC_CCCALCTRL               */
#define _DCDC_CCCALCTRL_CCLVL_LOAD3                   0x00000003UL                             /**< Mode LOAD3 for DCDC_CCCALCTRL               */
#define _DCDC_CCCALCTRL_CCLVL_LOAD4                   0x00000004UL                             /**< Mode LOAD4 for DCDC_CCCALCTRL               */
#define _DCDC_CCCALCTRL_CCLVL_LOAD5                   0x00000005UL                             /**< Mode LOAD5 for DCDC_CCCALCTRL               */
#define _DCDC_CCCALCTRL_CCLVL_LOAD6                   0x00000006UL                             /**< Mode LOAD6 for DCDC_CCCALCTRL               */
#define _DCDC_CCCALCTRL_CCLVL_LOAD7                   0x00000007UL                             /**< Mode LOAD7 for DCDC_CCCALCTRL               */
#define DCDC_CCCALCTRL_CCLVL_DEFAULT                  (_DCDC_CCCALCTRL_CCLVL_DEFAULT << 1)     /**< Shifted mode DEFAULT for DCDC_CCCALCTRL     */
#define DCDC_CCCALCTRL_CCLVL_LOAD0                    (_DCDC_CCCALCTRL_CCLVL_LOAD0 << 1)       /**< Shifted mode LOAD0 for DCDC_CCCALCTRL       */
#define DCDC_CCCALCTRL_CCLVL_LOAD1                    (_DCDC_CCCALCTRL_CCLVL_LOAD1 << 1)       /**< Shifted mode LOAD1 for DCDC_CCCALCTRL       */
#define DCDC_CCCALCTRL_CCLVL_LOAD2                    (_DCDC_CCCALCTRL_CCLVL_LOAD2 << 1)       /**< Shifted mode LOAD2 for DCDC_CCCALCTRL       */
#define DCDC_CCCALCTRL_CCLVL_LOAD3                    (_DCDC_CCCALCTRL_CCLVL_LOAD3 << 1)       /**< Shifted mode LOAD3 for DCDC_CCCALCTRL       */
#define DCDC_CCCALCTRL_CCLVL_LOAD4                    (_DCDC_CCCALCTRL_CCLVL_LOAD4 << 1)       /**< Shifted mode LOAD4 for DCDC_CCCALCTRL       */
#define DCDC_CCCALCTRL_CCLVL_LOAD5                    (_DCDC_CCCALCTRL_CCLVL_LOAD5 << 1)       /**< Shifted mode LOAD5 for DCDC_CCCALCTRL       */
#define DCDC_CCCALCTRL_CCLVL_LOAD6                    (_DCDC_CCCALCTRL_CCLVL_LOAD6 << 1)       /**< Shifted mode LOAD6 for DCDC_CCCALCTRL       */
#define DCDC_CCCALCTRL_CCLVL_LOAD7                    (_DCDC_CCCALCTRL_CCLVL_LOAD7 << 1)       /**< Shifted mode LOAD7 for DCDC_CCCALCTRL       */
#define DCDC_CCCALCTRL_CCCALEM2                       (0x1UL << 8)                             /**< CC Calibrate EM2                            */
#define _DCDC_CCCALCTRL_CCCALEM2_SHIFT                8                                        /**< Shift value for DCDC_CCCALEM2               */
#define _DCDC_CCCALCTRL_CCCALEM2_MASK                 0x100UL                                  /**< Bit mask for DCDC_CCCALEM2                  */
#define _DCDC_CCCALCTRL_CCCALEM2_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for DCDC_CCCALCTRL             */
#define DCDC_CCCALCTRL_CCCALEM2_DEFAULT               (_DCDC_CCCALCTRL_CCCALEM2_DEFAULT << 8)  /**< Shifted mode DEFAULT for DCDC_CCCALCTRL     */
#define DCDC_CCCALCTRL_CCCALHALT                      (0x1UL << 9)                             /**< CC Calibration Halt Req                     */
#define _DCDC_CCCALCTRL_CCCALHALT_SHIFT               9                                        /**< Shift value for DCDC_CCCALHALT              */
#define _DCDC_CCCALCTRL_CCCALHALT_MASK                0x200UL                                  /**< Bit mask for DCDC_CCCALHALT                 */
#define _DCDC_CCCALCTRL_CCCALHALT_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for DCDC_CCCALCTRL             */
#define DCDC_CCCALCTRL_CCCALHALT_DEFAULT              (_DCDC_CCCALCTRL_CCCALHALT_DEFAULT << 9) /**< Shifted mode DEFAULT for DCDC_CCCALCTRL     */

/* Bit fields for DCDC CCCMD */
#define _DCDC_CCCMD_RESETVALUE                        0x00000000UL                      /**< Default value for DCDC_CCCMD                */
#define _DCDC_CCCMD_MASK                              0x00000007UL                      /**< Mask for DCDC_CCCMD                         */
#define DCDC_CCCMD_START                              (0x1UL << 0)                      /**< Start CC                                    */
#define _DCDC_CCCMD_START_SHIFT                       0                                 /**< Shift value for DCDC_START                  */
#define _DCDC_CCCMD_START_MASK                        0x1UL                             /**< Bit mask for DCDC_START                     */
#define _DCDC_CCCMD_START_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for DCDC_CCCMD                 */
#define DCDC_CCCMD_START_DEFAULT                      (_DCDC_CCCMD_START_DEFAULT << 0)  /**< Shifted mode DEFAULT for DCDC_CCCMD         */
#define DCDC_CCCMD_STOP                               (0x1UL << 1)                      /**< Stop CC                                     */
#define _DCDC_CCCMD_STOP_SHIFT                        1                                 /**< Shift value for DCDC_STOP                   */
#define _DCDC_CCCMD_STOP_MASK                         0x2UL                             /**< Bit mask for DCDC_STOP                      */
#define _DCDC_CCCMD_STOP_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for DCDC_CCCMD                 */
#define DCDC_CCCMD_STOP_DEFAULT                       (_DCDC_CCCMD_STOP_DEFAULT << 1)   /**< Shifted mode DEFAULT for DCDC_CCCMD         */
#define DCDC_CCCMD_CLR                                (0x1UL << 2)                      /**< Clear CC                                    */
#define _DCDC_CCCMD_CLR_SHIFT                         2                                 /**< Shift value for DCDC_CLR                    */
#define _DCDC_CCCMD_CLR_MASK                          0x4UL                             /**< Bit mask for DCDC_CLR                       */
#define _DCDC_CCCMD_CLR_DEFAULT                       0x00000000UL                      /**< Mode DEFAULT for DCDC_CCCMD                 */
#define DCDC_CCCMD_CLR_DEFAULT                        (_DCDC_CCCMD_CLR_DEFAULT << 2)    /**< Shifted mode DEFAULT for DCDC_CCCMD         */

/* Bit fields for DCDC CCEM0CNT */
#define _DCDC_CCEM0CNT_RESETVALUE                     0x00000000UL                        /**< Default value for DCDC_CCEM0CNT             */
#define _DCDC_CCEM0CNT_MASK                           0xFFFFFFFFUL                        /**< Mask for DCDC_CCEM0CNT                      */
#define _DCDC_CCEM0CNT_CCCNT_SHIFT                    0                                   /**< Shift value for DCDC_CCCNT                  */
#define _DCDC_CCEM0CNT_CCCNT_MASK                     0xFFFFFFFFUL                        /**< Bit mask for DCDC_CCCNT                     */
#define _DCDC_CCEM0CNT_CCCNT_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for DCDC_CCEM0CNT              */
#define DCDC_CCEM0CNT_CCCNT_DEFAULT                   (_DCDC_CCEM0CNT_CCCNT_DEFAULT << 0) /**< Shifted mode DEFAULT for DCDC_CCEM0CNT      */

/* Bit fields for DCDC CCEM2CNT */
#define _DCDC_CCEM2CNT_RESETVALUE                     0x00000000UL                        /**< Default value for DCDC_CCEM2CNT             */
#define _DCDC_CCEM2CNT_MASK                           0xFFFFFFFFUL                        /**< Mask for DCDC_CCEM2CNT                      */
#define _DCDC_CCEM2CNT_CCCNT_SHIFT                    0                                   /**< Shift value for DCDC_CCCNT                  */
#define _DCDC_CCEM2CNT_CCCNT_MASK                     0xFFFFFFFFUL                        /**< Bit mask for DCDC_CCCNT                     */
#define _DCDC_CCEM2CNT_CCCNT_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for DCDC_CCEM2CNT              */
#define DCDC_CCEM2CNT_CCCNT_DEFAULT                   (_DCDC_CCEM2CNT_CCCNT_DEFAULT << 0) /**< Shifted mode DEFAULT for DCDC_CCEM2CNT      */

/* Bit fields for DCDC CCTHR */
#define _DCDC_CCTHR_RESETVALUE                        0x00010001UL                       /**< Default value for DCDC_CCTHR                */
#define _DCDC_CCTHR_MASK                              0xFFFFFFFFUL                       /**< Mask for DCDC_CCTHR                         */
#define _DCDC_CCTHR_EM0CNT_SHIFT                      0                                  /**< Shift value for DCDC_EM0CNT                 */
#define _DCDC_CCTHR_EM0CNT_MASK                       0xFFFFUL                           /**< Bit mask for DCDC_EM0CNT                    */
#define _DCDC_CCTHR_EM0CNT_DEFAULT                    0x00000001UL                       /**< Mode DEFAULT for DCDC_CCTHR                 */
#define DCDC_CCTHR_EM0CNT_DEFAULT                     (_DCDC_CCTHR_EM0CNT_DEFAULT << 0)  /**< Shifted mode DEFAULT for DCDC_CCTHR         */
#define _DCDC_CCTHR_EM2CNT_SHIFT                      16                                 /**< Shift value for DCDC_EM2CNT                 */
#define _DCDC_CCTHR_EM2CNT_MASK                       0xFFFF0000UL                       /**< Bit mask for DCDC_EM2CNT                    */
#define _DCDC_CCTHR_EM2CNT_DEFAULT                    0x00000001UL                       /**< Mode DEFAULT for DCDC_CCTHR                 */
#define DCDC_CCTHR_EM2CNT_DEFAULT                     (_DCDC_CCTHR_EM2CNT_DEFAULT << 16) /**< Shifted mode DEFAULT for DCDC_CCTHR         */

/* Bit fields for DCDC CCIF */
#define _DCDC_CCIF_RESETVALUE                         0x00000000UL                       /**< Default value for DCDC_CCIF                 */
#define _DCDC_CCIF_MASK                               0x0000000FUL                       /**< Mask for DCDC_CCIF                          */
#define DCDC_CCIF_EM0OFIF                             (0x1UL << 0)                       /**< EM0 Counter Overflow                        */
#define _DCDC_CCIF_EM0OFIF_SHIFT                      0                                  /**< Shift value for DCDC_EM0OFIF                */
#define _DCDC_CCIF_EM0OFIF_MASK                       0x1UL                              /**< Bit mask for DCDC_EM0OFIF                   */
#define _DCDC_CCIF_EM0OFIF_DEFAULT                    0x00000000UL                       /**< Mode DEFAULT for DCDC_CCIF                  */
#define DCDC_CCIF_EM0OFIF_DEFAULT                     (_DCDC_CCIF_EM0OFIF_DEFAULT << 0)  /**< Shifted mode DEFAULT for DCDC_CCIF          */
#define DCDC_CCIF_EM2OFIF                             (0x1UL << 1)                       /**< EM2 Counter Overflow                        */
#define _DCDC_CCIF_EM2OFIF_SHIFT                      1                                  /**< Shift value for DCDC_EM2OFIF                */
#define _DCDC_CCIF_EM2OFIF_MASK                       0x2UL                              /**< Bit mask for DCDC_EM2OFIF                   */
#define _DCDC_CCIF_EM2OFIF_DEFAULT                    0x00000000UL                       /**< Mode DEFAULT for DCDC_CCIF                  */
#define DCDC_CCIF_EM2OFIF_DEFAULT                     (_DCDC_CCIF_EM2OFIF_DEFAULT << 1)  /**< Shifted mode DEFAULT for DCDC_CCIF          */
#define DCDC_CCIF_EM0CMPIF                            (0x1UL << 2)                       /**< EM0 Counter Compare Match                   */
#define _DCDC_CCIF_EM0CMPIF_SHIFT                     2                                  /**< Shift value for DCDC_EM0CMPIF               */
#define _DCDC_CCIF_EM0CMPIF_MASK                      0x4UL                              /**< Bit mask for DCDC_EM0CMPIF                  */
#define _DCDC_CCIF_EM0CMPIF_DEFAULT                   0x00000000UL                       /**< Mode DEFAULT for DCDC_CCIF                  */
#define DCDC_CCIF_EM0CMPIF_DEFAULT                    (_DCDC_CCIF_EM0CMPIF_DEFAULT << 2) /**< Shifted mode DEFAULT for DCDC_CCIF          */
#define DCDC_CCIF_EM2CMPIF                            (0x1UL << 3)                       /**< EM2 Counter Compare Match                   */
#define _DCDC_CCIF_EM2CMPIF_SHIFT                     3                                  /**< Shift value for DCDC_EM2CMPIF               */
#define _DCDC_CCIF_EM2CMPIF_MASK                      0x8UL                              /**< Bit mask for DCDC_EM2CMPIF                  */
#define _DCDC_CCIF_EM2CMPIF_DEFAULT                   0x00000000UL                       /**< Mode DEFAULT for DCDC_CCIF                  */
#define DCDC_CCIF_EM2CMPIF_DEFAULT                    (_DCDC_CCIF_EM2CMPIF_DEFAULT << 3) /**< Shifted mode DEFAULT for DCDC_CCIF          */

/* Bit fields for DCDC CCIEN */
#define _DCDC_CCIEN_RESETVALUE                        0x00000000UL                         /**< Default value for DCDC_CCIEN                */
#define _DCDC_CCIEN_MASK                              0x0000000FUL                         /**< Mask for DCDC_CCIEN                         */
#define DCDC_CCIEN_EM0OFIEN                           (0x1UL << 0)                         /**< Clmb Cntr EM0 Overflow Interrupt Enable     */
#define _DCDC_CCIEN_EM0OFIEN_SHIFT                    0                                    /**< Shift value for DCDC_EM0OFIEN               */
#define _DCDC_CCIEN_EM0OFIEN_MASK                     0x1UL                                /**< Bit mask for DCDC_EM0OFIEN                  */
#define _DCDC_CCIEN_EM0OFIEN_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for DCDC_CCIEN                 */
#define DCDC_CCIEN_EM0OFIEN_DEFAULT                   (_DCDC_CCIEN_EM0OFIEN_DEFAULT << 0)  /**< Shifted mode DEFAULT for DCDC_CCIEN         */
#define DCDC_CCIEN_EM2OFIEN                           (0x1UL << 1)                         /**< Clmb Cntr EM2 Overflow Interrupt Enable     */
#define _DCDC_CCIEN_EM2OFIEN_SHIFT                    1                                    /**< Shift value for DCDC_EM2OFIEN               */
#define _DCDC_CCIEN_EM2OFIEN_MASK                     0x2UL                                /**< Bit mask for DCDC_EM2OFIEN                  */
#define _DCDC_CCIEN_EM2OFIEN_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for DCDC_CCIEN                 */
#define DCDC_CCIEN_EM2OFIEN_DEFAULT                   (_DCDC_CCIEN_EM2OFIEN_DEFAULT << 1)  /**< Shifted mode DEFAULT for DCDC_CCIEN         */
#define DCDC_CCIEN_EM0CMPIEN                          (0x1UL << 2)                         /**< Clmb Cntr EM0 Cmp Match Interrupt Enable    */
#define _DCDC_CCIEN_EM0CMPIEN_SHIFT                   2                                    /**< Shift value for DCDC_EM0CMPIEN              */
#define _DCDC_CCIEN_EM0CMPIEN_MASK                    0x4UL                                /**< Bit mask for DCDC_EM0CMPIEN                 */
#define _DCDC_CCIEN_EM0CMPIEN_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for DCDC_CCIEN                 */
#define DCDC_CCIEN_EM0CMPIEN_DEFAULT                  (_DCDC_CCIEN_EM0CMPIEN_DEFAULT << 2) /**< Shifted mode DEFAULT for DCDC_CCIEN         */
#define DCDC_CCIEN_EM2CMPIEN                          (0x1UL << 3)                         /**< Clmb Cntr EM2 Cmp Match Interrupt Enable    */
#define _DCDC_CCIEN_EM2CMPIEN_SHIFT                   3                                    /**< Shift value for DCDC_EM2CMPIEN              */
#define _DCDC_CCIEN_EM2CMPIEN_MASK                    0x8UL                                /**< Bit mask for DCDC_EM2CMPIEN                 */
#define _DCDC_CCIEN_EM2CMPIEN_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for DCDC_CCIEN                 */
#define DCDC_CCIEN_EM2CMPIEN_DEFAULT                  (_DCDC_CCIEN_EM2CMPIEN_DEFAULT << 3) /**< Shifted mode DEFAULT for DCDC_CCIEN         */

/* Bit fields for DCDC CCSTATUS */
#define _DCDC_CCSTATUS_RESETVALUE                     0x00000000UL                            /**< Default value for DCDC_CCSTATUS             */
#define _DCDC_CCSTATUS_MASK                           0x00000003UL                            /**< Mask for DCDC_CCSTATUS                      */
#define DCDC_CCSTATUS_CLRBSY                          (0x1UL << 0)                            /**< Coulomb Counter Clear Busy                  */
#define _DCDC_CCSTATUS_CLRBSY_SHIFT                   0                                       /**< Shift value for DCDC_CLRBSY                 */
#define _DCDC_CCSTATUS_CLRBSY_MASK                    0x1UL                                   /**< Bit mask for DCDC_CLRBSY                    */
#define _DCDC_CCSTATUS_CLRBSY_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for DCDC_CCSTATUS              */
#define DCDC_CCSTATUS_CLRBSY_DEFAULT                  (_DCDC_CCSTATUS_CLRBSY_DEFAULT << 0)    /**< Shifted mode DEFAULT for DCDC_CCSTATUS      */
#define DCDC_CCSTATUS_CCRUNNING                       (0x1UL << 1)                            /**< Coulomb Counter Running                     */
#define _DCDC_CCSTATUS_CCRUNNING_SHIFT                1                                       /**< Shift value for DCDC_CCRUNNING              */
#define _DCDC_CCSTATUS_CCRUNNING_MASK                 0x2UL                                   /**< Bit mask for DCDC_CCRUNNING                 */
#define _DCDC_CCSTATUS_CCRUNNING_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for DCDC_CCSTATUS              */
#define DCDC_CCSTATUS_CCRUNNING_DEFAULT               (_DCDC_CCSTATUS_CCRUNNING_DEFAULT << 1) /**< Shifted mode DEFAULT for DCDC_CCSTATUS      */

/* Bit fields for DCDC LOCK */
#define _DCDC_LOCK_RESETVALUE                         0x00000000UL                        /**< Default value for DCDC_LOCK                 */
#define _DCDC_LOCK_MASK                               0x0000FFFFUL                        /**< Mask for DCDC_LOCK                          */
#define _DCDC_LOCK_LOCKKEY_SHIFT                      0                                   /**< Shift value for DCDC_LOCKKEY                */
#define _DCDC_LOCK_LOCKKEY_MASK                       0xFFFFUL                            /**< Bit mask for DCDC_LOCKKEY                   */
#define _DCDC_LOCK_LOCKKEY_DEFAULT                    0x00000000UL                        /**< Mode DEFAULT for DCDC_LOCK                  */
#define _DCDC_LOCK_LOCKKEY_UNLOCKKEY                  0x0000ABCDUL                        /**< Mode UNLOCKKEY for DCDC_LOCK                */
#define DCDC_LOCK_LOCKKEY_DEFAULT                     (_DCDC_LOCK_LOCKKEY_DEFAULT << 0)   /**< Shifted mode DEFAULT for DCDC_LOCK          */
#define DCDC_LOCK_LOCKKEY_UNLOCKKEY                   (_DCDC_LOCK_LOCKKEY_UNLOCKKEY << 0) /**< Shifted mode UNLOCKKEY for DCDC_LOCK        */

/* Bit fields for DCDC LOCKSTATUS */
#define _DCDC_LOCKSTATUS_RESETVALUE                   0x00000000UL                          /**< Default value for DCDC_LOCKSTATUS           */
#define _DCDC_LOCKSTATUS_MASK                         0x00000001UL                          /**< Mask for DCDC_LOCKSTATUS                    */
#define DCDC_LOCKSTATUS_LOCK                          (0x1UL << 0)                          /**< Lock Status                                 */
#define _DCDC_LOCKSTATUS_LOCK_SHIFT                   0                                     /**< Shift value for DCDC_LOCK                   */
#define _DCDC_LOCKSTATUS_LOCK_MASK                    0x1UL                                 /**< Bit mask for DCDC_LOCK                      */
#define _DCDC_LOCKSTATUS_LOCK_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for DCDC_LOCKSTATUS            */
#define _DCDC_LOCKSTATUS_LOCK_UNLOCKED                0x00000000UL                          /**< Mode UNLOCKED for DCDC_LOCKSTATUS           */
#define _DCDC_LOCKSTATUS_LOCK_LOCKED                  0x00000001UL                          /**< Mode LOCKED for DCDC_LOCKSTATUS             */
#define DCDC_LOCKSTATUS_LOCK_DEFAULT                  (_DCDC_LOCKSTATUS_LOCK_DEFAULT << 0)  /**< Shifted mode DEFAULT for DCDC_LOCKSTATUS    */
#define DCDC_LOCKSTATUS_LOCK_UNLOCKED                 (_DCDC_LOCKSTATUS_LOCK_UNLOCKED << 0) /**< Shifted mode UNLOCKED for DCDC_LOCKSTATUS   */
#define DCDC_LOCKSTATUS_LOCK_LOCKED                   (_DCDC_LOCKSTATUS_LOCK_LOCKED << 0)   /**< Shifted mode LOCKED for DCDC_LOCKSTATUS     */

/** @} End of group EFR32BG27_DCDC_BitFields */
/** @} End of group EFR32BG27_DCDC */
/** @} End of group Parts */

#endif /* EFR32BG27_DCDC_H */
