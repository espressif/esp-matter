/**************************************************************************//**
 * @file
 * @brief EFR32FG25 USB register and bit field definitions
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
#ifndef EFR32FG25_USB_H
#define EFR32FG25_USB_H
#define USB_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32FG25_USB_APBS USB_APBS
 * @{
 * @brief EFR32FG25 USB_APBS Register Declaration.
 *****************************************************************************/

/** USB_APBS Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version                                         */
  __IOM uint32_t EN;                            /**< USB Module Enable                                  */
  __IOM uint32_t CTRL;                          /**< System ctrl reg                                    */
  __IOM uint32_t IF;                            /**< IRQ flag reg                                       */
  uint32_t       RESERVED0[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t IEN;                           /**< IRQ enable reg                                     */
  uint32_t       RESERVED1[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CAL;                           /**< Cal values - PHY                                   */
  __IM uint32_t  STATUS;                        /**< STATUS reg                                         */
  uint32_t       RESERVED2[1013U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version                                         */
  __IOM uint32_t EN_SET;                        /**< USB Module Enable                                  */
  __IOM uint32_t CTRL_SET;                      /**< System ctrl reg                                    */
  __IOM uint32_t IF_SET;                        /**< IRQ flag reg                                       */
  uint32_t       RESERVED3[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t IEN_SET;                       /**< IRQ enable reg                                     */
  uint32_t       RESERVED4[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CAL_SET;                       /**< Cal values - PHY                                   */
  __IM uint32_t  STATUS_SET;                    /**< STATUS reg                                         */
  uint32_t       RESERVED5[1013U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version                                         */
  __IOM uint32_t EN_CLR;                        /**< USB Module Enable                                  */
  __IOM uint32_t CTRL_CLR;                      /**< System ctrl reg                                    */
  __IOM uint32_t IF_CLR;                        /**< IRQ flag reg                                       */
  uint32_t       RESERVED6[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t IEN_CLR;                       /**< IRQ enable reg                                     */
  uint32_t       RESERVED7[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CAL_CLR;                       /**< Cal values - PHY                                   */
  __IM uint32_t  STATUS_CLR;                    /**< STATUS reg                                         */
  uint32_t       RESERVED8[1013U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version                                         */
  __IOM uint32_t EN_TGL;                        /**< USB Module Enable                                  */
  __IOM uint32_t CTRL_TGL;                      /**< System ctrl reg                                    */
  __IOM uint32_t IF_TGL;                        /**< IRQ flag reg                                       */
  uint32_t       RESERVED9[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t IEN_TGL;                       /**< IRQ enable reg                                     */
  uint32_t       RESERVED10[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CAL_TGL;                       /**< Cal values - PHY                                   */
  __IM uint32_t  STATUS_TGL;                    /**< STATUS reg                                         */
} USB_APBS_TypeDef;
/** @} End of group EFR32FG25_USB_APBS */

/**************************************************************************//**
 * @addtogroup EFR32FG25_USB_APBS
 * @{
 * @defgroup EFR32FG25_USB_APBS_BitFields USB_APBS Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for USB IPVERSION */
#define _USB_IPVERSION_RESETVALUE           0x00000001UL                                /**< Default value for USB_IPVERSION             */
#define _USB_IPVERSION_MASK                 0x00000001UL                                /**< Mask for USB_IPVERSION                      */
#define USB_IPVERSION_IPVERSION             (0x1UL << 0)                                /**< IP Version ID                               */
#define _USB_IPVERSION_IPVERSION_SHIFT      0                                           /**< Shift value for USB_IPVERSION               */
#define _USB_IPVERSION_IPVERSION_MASK       0x1UL                                       /**< Bit mask for USB_IPVERSION                  */
#define _USB_IPVERSION_IPVERSION_DEFAULT    0x00000001UL                                /**< Mode DEFAULT for USB_IPVERSION              */
#define USB_IPVERSION_IPVERSION_DEFAULT     (_USB_IPVERSION_IPVERSION_DEFAULT << 0)     /**< Shifted mode DEFAULT for USB_IPVERSION      */

/* Bit fields for USB EN */
#define _USB_EN_RESETVALUE                  0x00000000UL                                /**< Default value for USB_EN                    */
#define _USB_EN_MASK                        0x00000003UL                                /**< Mask for USB_EN                             */
#define USB_EN_EN                           (0x1UL << 0)                                /**< Module enable                               */
#define _USB_EN_EN_SHIFT                    0                                           /**< Shift value for USB_EN                      */
#define _USB_EN_EN_MASK                     0x1UL                                       /**< Bit mask for USB_EN                         */
#define _USB_EN_EN_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for USB_EN                     */
#define USB_EN_EN_DEFAULT                   (_USB_EN_EN_DEFAULT << 0)                   /**< Shifted mode DEFAULT for USB_EN             */
#define USB_EN_DISABLING                    (0x1UL << 1)                                /**< Disablement busy status                     */
#define _USB_EN_DISABLING_SHIFT             1                                           /**< Shift value for USB_DISABLING               */
#define _USB_EN_DISABLING_MASK              0x2UL                                       /**< Bit mask for USB_DISABLING                  */
#define _USB_EN_DISABLING_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for USB_EN                     */
#define USB_EN_DISABLING_DEFAULT            (_USB_EN_DISABLING_DEFAULT << 1)            /**< Shifted mode DEFAULT for USB_EN             */

/* Bit fields for USB CTRL */
#define _USB_CTRL_RESETVALUE                0x03E40000UL                                /**< Default value for USB_CTRL                  */
#define _USB_CTRL_MASK                      0x07E78001UL                                /**< Mask for USB_CTRL                           */
#define USB_CTRL_TXDSE0MXSEL                (0x1UL << 0)                                /**< TXD SE0 chicken bit                         */
#define _USB_CTRL_TXDSE0MXSEL_SHIFT         0                                           /**< Shift value for USB_TXDSE0MXSEL             */
#define _USB_CTRL_TXDSE0MXSEL_MASK          0x1UL                                       /**< Bit mask for USB_TXDSE0MXSEL                */
#define _USB_CTRL_TXDSE0MXSEL_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for USB_CTRL                   */
#define USB_CTRL_TXDSE0MXSEL_DEFAULT        (_USB_CTRL_TXDSE0MXSEL_DEFAULT << 0)        /**< Shifted mode DEFAULT for USB_CTRL           */
#define USB_CTRL_VBSSNSHEN                  (0x1UL << 16)                               /**< VBUSSENSE going high - enable               */
#define _USB_CTRL_VBSSNSHEN_SHIFT           16                                          /**< Shift value for USB_VBSSNSHEN               */
#define _USB_CTRL_VBSSNSHEN_MASK            0x10000UL                                   /**< Bit mask for USB_VBSSNSHEN                  */
#define _USB_CTRL_VBSSNSHEN_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for USB_CTRL                   */
#define USB_CTRL_VBSSNSHEN_DEFAULT          (_USB_CTRL_VBSSNSHEN_DEFAULT << 16)         /**< Shifted mode DEFAULT for USB_CTRL           */
#define USB_CTRL_VBSSNSLEN                  (0x1UL << 17)                               /**< VBUSSENSE going low - enable                */
#define _USB_CTRL_VBSSNSLEN_SHIFT           17                                          /**< Shift value for USB_VBSSNSLEN               */
#define _USB_CTRL_VBSSNSLEN_MASK            0x20000UL                                   /**< Bit mask for USB_VBSSNSLEN                  */
#define _USB_CTRL_VBSSNSLEN_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for USB_CTRL                   */
#define USB_CTRL_VBSSNSLEN_DEFAULT          (_USB_CTRL_VBSSNSLEN_DEFAULT << 17)         /**< Shifted mode DEFAULT for USB_CTRL           */
#define USB_CTRL_VBUSOVRIDE                 (0x1UL << 18)                               /**< vbus over ride to GPPLL                     */
#define _USB_CTRL_VBUSOVRIDE_SHIFT          18                                          /**< Shift value for USB_VBUSOVRIDE              */
#define _USB_CTRL_VBUSOVRIDE_MASK           0x40000UL                                   /**< Bit mask for USB_VBUSOVRIDE                 */
#define _USB_CTRL_VBUSOVRIDE_DEFAULT        0x00000001UL                                /**< Mode DEFAULT for USB_CTRL                   */
#define USB_CTRL_VBUSOVRIDE_DEFAULT         (_USB_CTRL_VBUSOVRIDE_DEFAULT << 18)        /**< Shifted mode DEFAULT for USB_CTRL           */
#define _USB_CTRL_FSSLEW_SHIFT              21                                          /**< Shift value for USB_FSSLEW                  */
#define _USB_CTRL_FSSLEW_MASK               0xE00000UL                                  /**< Bit mask for USB_FSSLEW                     */
#define _USB_CTRL_FSSLEW_DEFAULT            0x00000007UL                                /**< Mode DEFAULT for USB_CTRL                   */
#define USB_CTRL_FSSLEW_DEFAULT             (_USB_CTRL_FSSLEW_DEFAULT << 21)            /**< Shifted mode DEFAULT for USB_CTRL           */
#define USB_CTRL_DISRUPTX                   (0x1UL << 24)                               /**< disable tx pull up res                      */
#define _USB_CTRL_DISRUPTX_SHIFT            24                                          /**< Shift value for USB_DISRUPTX                */
#define _USB_CTRL_DISRUPTX_MASK             0x1000000UL                                 /**< Bit mask for USB_DISRUPTX                   */
#define _USB_CTRL_DISRUPTX_DEFAULT          0x00000001UL                                /**< Mode DEFAULT for USB_CTRL                   */
#define USB_CTRL_DISRUPTX_DEFAULT           (_USB_CTRL_DISRUPTX_DEFAULT << 24)          /**< Shifted mode DEFAULT for USB_CTRL           */
#define USB_CTRL_BYPSVREG                   (0x1UL << 25)                               /**< bypass_vreg signal to phy                   */
#define _USB_CTRL_BYPSVREG_SHIFT            25                                          /**< Shift value for USB_BYPSVREG                */
#define _USB_CTRL_BYPSVREG_MASK             0x2000000UL                                 /**< Bit mask for USB_BYPSVREG                   */
#define _USB_CTRL_BYPSVREG_DEFAULT          0x00000001UL                                /**< Mode DEFAULT for USB_CTRL                   */
#define USB_CTRL_BYPSVREG_DEFAULT           (_USB_CTRL_BYPSVREG_DEFAULT << 25)          /**< Shifted mode DEFAULT for USB_CTRL           */
#define USB_CTRL_ENDATACAP                  (0x1UL << 26)                               /**< adds 40pf cap to dm/dp in tx mode           */
#define _USB_CTRL_ENDATACAP_SHIFT           26                                          /**< Shift value for USB_ENDATACAP               */
#define _USB_CTRL_ENDATACAP_MASK            0x4000000UL                                 /**< Bit mask for USB_ENDATACAP                  */
#define _USB_CTRL_ENDATACAP_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for USB_CTRL                   */
#define USB_CTRL_ENDATACAP_DEFAULT          (_USB_CTRL_ENDATACAP_DEFAULT << 26)         /**< Shifted mode DEFAULT for USB_CTRL           */

/* Bit fields for USB IF */
#define _USB_IF_RESETVALUE                  0x00000000UL                                /**< Default value for USB_IF                    */
#define _USB_IF_MASK                        0x00000003UL                                /**< Mask for USB_IF                             */
#define USB_IF_VBUS                         (0x1UL << 0)                                /**< VBUS_SENSE_IRQ                              */
#define _USB_IF_VBUS_SHIFT                  0                                           /**< Shift value for USB_VBUS                    */
#define _USB_IF_VBUS_MASK                   0x1UL                                       /**< Bit mask for USB_VBUS                       */
#define _USB_IF_VBUS_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for USB_IF                     */
#define USB_IF_VBUS_DEFAULT                 (_USB_IF_VBUS_DEFAULT << 0)                 /**< Shifted mode DEFAULT for USB_IF             */
#define USB_IF_DWCOTG                       (0x1UL << 1)                                /**< DWC OTG IRQ flg                             */
#define _USB_IF_DWCOTG_SHIFT                1                                           /**< Shift value for USB_DWCOTG                  */
#define _USB_IF_DWCOTG_MASK                 0x2UL                                       /**< Bit mask for USB_DWCOTG                     */
#define _USB_IF_DWCOTG_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for USB_IF                     */
#define USB_IF_DWCOTG_DEFAULT               (_USB_IF_DWCOTG_DEFAULT << 1)               /**< Shifted mode DEFAULT for USB_IF             */

/* Bit fields for USB IEN */
#define _USB_IEN_RESETVALUE                 0x00000000UL                                /**< Default value for USB_IEN                   */
#define _USB_IEN_MASK                       0x00000003UL                                /**< Mask for USB_IEN                            */
#define USB_IEN_VBUS                        (0x1UL << 0)                                /**< VBUS_SENSE Enable                           */
#define _USB_IEN_VBUS_SHIFT                 0                                           /**< Shift value for USB_VBUS                    */
#define _USB_IEN_VBUS_MASK                  0x1UL                                       /**< Bit mask for USB_VBUS                       */
#define _USB_IEN_VBUS_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for USB_IEN                    */
#define USB_IEN_VBUS_DEFAULT                (_USB_IEN_VBUS_DEFAULT << 0)                /**< Shifted mode DEFAULT for USB_IEN            */
#define USB_IEN_DWCOTG                      (0x1UL << 1)                                /**< DWC_otg IRQ Enable                          */
#define _USB_IEN_DWCOTG_SHIFT               1                                           /**< Shift value for USB_DWCOTG                  */
#define _USB_IEN_DWCOTG_MASK                0x2UL                                       /**< Bit mask for USB_DWCOTG                     */
#define _USB_IEN_DWCOTG_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for USB_IEN                    */
#define USB_IEN_DWCOTG_DEFAULT              (_USB_IEN_DWCOTG_DEFAULT << 1)              /**< Shifted mode DEFAULT for USB_IEN            */

/* Bit fields for USB CAL */
#define _USB_CAL_RESETVALUE                 0x07078000UL                                /**< Default value for USB_CAL                   */
#define _USB_CAL_MASK                       0x0707830FUL                                /**< Mask for USB_CAL                            */
#define _USB_CAL_FSSLEWT_SHIFT              24                                          /**< Shift value for USB_FSSLEWT                 */
#define _USB_CAL_FSSLEWT_MASK               0x7000000UL                                 /**< Bit mask for USB_FSSLEWT                    */
#define _USB_CAL_FSSLEWT_DEFAULT            0x00000007UL                                /**< Mode DEFAULT for USB_CAL                    */
#define USB_CAL_FSSLEWT_DEFAULT             (_USB_CAL_FSSLEWT_DEFAULT << 24)            /**< Shifted mode DEFAULT for USB_CAL            */

/* Bit fields for USB STATUS */
#define _USB_STATUS_RESETVALUE              0x00000000UL                                /**< Default value for USB_STATUS                */
#define _USB_STATUS_MASK                    0x00000001UL                                /**< Mask for USB_STATUS                         */
#define USB_STATUS_VBUSVALID                (0x1UL << 0)                                /**< VBus valid                                  */
#define _USB_STATUS_VBUSVALID_SHIFT         0                                           /**< Shift value for USB_VBUSVALID               */
#define _USB_STATUS_VBUSVALID_MASK          0x1UL                                       /**< Bit mask for USB_VBUSVALID                  */
#define _USB_STATUS_VBUSVALID_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for USB_STATUS                 */
#define USB_STATUS_VBUSVALID_DEFAULT        (_USB_STATUS_VBUSVALID_DEFAULT << 0)        /**< Shifted mode DEFAULT for USB_STATUS         */

/** @} End of group EFR32FG25_USB_APBS_BitFields */
/** @} End of group EFR32FG25_USB_APBS */
/** @} End of group Parts */

#endif /* EFR32FG25_USB_H */
