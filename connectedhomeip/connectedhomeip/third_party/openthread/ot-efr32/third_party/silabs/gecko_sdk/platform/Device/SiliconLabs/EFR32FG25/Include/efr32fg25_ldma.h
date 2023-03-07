/**************************************************************************//**
 * @file
 * @brief EFR32FG25 LDMA register and bit field definitions
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
#ifndef EFR32FG25_LDMA_H
#define EFR32FG25_LDMA_H
#define LDMA_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32FG25_LDMA LDMA
 * @{
 * @brief EFR32FG25 LDMA Register Declaration.
 *****************************************************************************/

/** LDMA CH Register Group Declaration. */
typedef struct {
  __IOM uint32_t CFG;                                /**< Configuration                                      */
  __IOM uint32_t LOOP;                               /**< Loop Counter                                       */
  __IOM uint32_t CTRL;                               /**< Control                                            */
  __IOM uint32_t SRC;                                /**< Source Address                                     */
  __IOM uint32_t DST;                                /**< Destination Address                                */
  __IOM uint32_t LINK;                               /**< Link Address                                       */
  __IOM uint32_t XCTRL;                              /**< Extended Control                                   */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t ILSRC;                              /**< Interleaving Source Address                        */
  uint32_t       RESERVED1[3U];                      /**< Reserved for future use                            */
} LDMA_CH_TypeDef;

/** LDMA Register Declaration. */
typedef struct {
  __IM uint32_t   IPVERSION;                    /**< IP version                                         */
  __IOM uint32_t  EN;                           /**< Module enable disable Register                     */
  __IOM uint32_t  SWRST;                        /**< Software Reset Register                            */
  __IOM uint32_t  CTRL;                         /**< Control Register                                   */
  __IM uint32_t   STATUS;                       /**< Status Register                                    */
  __IOM uint32_t  SYNCSWSET;                    /**< Sync Trig Sw Set Register                          */
  __IOM uint32_t  SYNCSWCLR;                    /**< Sync Trig Sw Clear register                        */
  __IOM uint32_t  SYNCHWEN;                     /**< Sync HW trigger enable register                    */
  __IOM uint32_t  SYNCHWSEL;                    /**< Sync HW trigger selection register                 */
  __IM uint32_t   SYNCSTATUS;                   /**< Sync Trigger Status Register                       */
  __IOM uint32_t  CHEN;                         /**< Channel Enable Register                            */
  __IOM uint32_t  CHDIS;                        /**< Channel Disable Register                           */
  __IM uint32_t   CHSTATUS;                     /**< Channel Status Register                            */
  __IM uint32_t   CHBUSY;                       /**< Channel Busy Register                              */
  __IOM uint32_t  CHDONE;                       /**< Channel Linking Done Register                      */
  __IOM uint32_t  DBGHALT;                      /**< Channel Debug Halt Register                        */
  __IOM uint32_t  SWREQ;                        /**< Channel Software Transfer Request                  */
  __IOM uint32_t  REQDIS;                       /**< Channel Request Disable Register                   */
  __IM uint32_t   REQPEND;                      /**< Channel Requests Pending Register                  */
  __IOM uint32_t  LINKLOAD;                     /**< Channel Link Load Register                         */
  __IOM uint32_t  REQCLEAR;                     /**< Channel Request Clear Register                     */
  __IOM uint32_t  IF;                           /**< Interrupt Flag Register                            */
  __IOM uint32_t  IEN;                          /**< Done Interrupt Enable Register                     */
  uint32_t        RESERVED0[1U];                /**< Reserved for future use                            */
  LDMA_CH_TypeDef CH[16U];                      /**< DMA Channel Registers                              */
  uint32_t        RESERVED1[808U];              /**< Reserved for future use                            */
  __IM uint32_t   IPVERSION_SET;                /**< IP version                                         */
  __IOM uint32_t  EN_SET;                       /**< Module enable disable Register                     */
  __IOM uint32_t  SWRST_SET;                    /**< Software Reset Register                            */
  __IOM uint32_t  CTRL_SET;                     /**< Control Register                                   */
  __IM uint32_t   STATUS_SET;                   /**< Status Register                                    */
  __IOM uint32_t  SYNCSWSET_SET;                /**< Sync Trig Sw Set Register                          */
  __IOM uint32_t  SYNCSWCLR_SET;                /**< Sync Trig Sw Clear register                        */
  __IOM uint32_t  SYNCHWEN_SET;                 /**< Sync HW trigger enable register                    */
  __IOM uint32_t  SYNCHWSEL_SET;                /**< Sync HW trigger selection register                 */
  __IM uint32_t   SYNCSTATUS_SET;               /**< Sync Trigger Status Register                       */
  __IOM uint32_t  CHEN_SET;                     /**< Channel Enable Register                            */
  __IOM uint32_t  CHDIS_SET;                    /**< Channel Disable Register                           */
  __IM uint32_t   CHSTATUS_SET;                 /**< Channel Status Register                            */
  __IM uint32_t   CHBUSY_SET;                   /**< Channel Busy Register                              */
  __IOM uint32_t  CHDONE_SET;                   /**< Channel Linking Done Register                      */
  __IOM uint32_t  DBGHALT_SET;                  /**< Channel Debug Halt Register                        */
  __IOM uint32_t  SWREQ_SET;                    /**< Channel Software Transfer Request                  */
  __IOM uint32_t  REQDIS_SET;                   /**< Channel Request Disable Register                   */
  __IM uint32_t   REQPEND_SET;                  /**< Channel Requests Pending Register                  */
  __IOM uint32_t  LINKLOAD_SET;                 /**< Channel Link Load Register                         */
  __IOM uint32_t  REQCLEAR_SET;                 /**< Channel Request Clear Register                     */
  __IOM uint32_t  IF_SET;                       /**< Interrupt Flag Register                            */
  __IOM uint32_t  IEN_SET;                      /**< Done Interrupt Enable Register                     */
  uint32_t        RESERVED2[1U];                /**< Reserved for future use                            */
  LDMA_CH_TypeDef CH_SET[16U];                  /**< DMA Channel Registers                              */
  uint32_t        RESERVED3[808U];              /**< Reserved for future use                            */
  __IM uint32_t   IPVERSION_CLR;                /**< IP version                                         */
  __IOM uint32_t  EN_CLR;                       /**< Module enable disable Register                     */
  __IOM uint32_t  SWRST_CLR;                    /**< Software Reset Register                            */
  __IOM uint32_t  CTRL_CLR;                     /**< Control Register                                   */
  __IM uint32_t   STATUS_CLR;                   /**< Status Register                                    */
  __IOM uint32_t  SYNCSWSET_CLR;                /**< Sync Trig Sw Set Register                          */
  __IOM uint32_t  SYNCSWCLR_CLR;                /**< Sync Trig Sw Clear register                        */
  __IOM uint32_t  SYNCHWEN_CLR;                 /**< Sync HW trigger enable register                    */
  __IOM uint32_t  SYNCHWSEL_CLR;                /**< Sync HW trigger selection register                 */
  __IM uint32_t   SYNCSTATUS_CLR;               /**< Sync Trigger Status Register                       */
  __IOM uint32_t  CHEN_CLR;                     /**< Channel Enable Register                            */
  __IOM uint32_t  CHDIS_CLR;                    /**< Channel Disable Register                           */
  __IM uint32_t   CHSTATUS_CLR;                 /**< Channel Status Register                            */
  __IM uint32_t   CHBUSY_CLR;                   /**< Channel Busy Register                              */
  __IOM uint32_t  CHDONE_CLR;                   /**< Channel Linking Done Register                      */
  __IOM uint32_t  DBGHALT_CLR;                  /**< Channel Debug Halt Register                        */
  __IOM uint32_t  SWREQ_CLR;                    /**< Channel Software Transfer Request                  */
  __IOM uint32_t  REQDIS_CLR;                   /**< Channel Request Disable Register                   */
  __IM uint32_t   REQPEND_CLR;                  /**< Channel Requests Pending Register                  */
  __IOM uint32_t  LINKLOAD_CLR;                 /**< Channel Link Load Register                         */
  __IOM uint32_t  REQCLEAR_CLR;                 /**< Channel Request Clear Register                     */
  __IOM uint32_t  IF_CLR;                       /**< Interrupt Flag Register                            */
  __IOM uint32_t  IEN_CLR;                      /**< Done Interrupt Enable Register                     */
  uint32_t        RESERVED4[1U];                /**< Reserved for future use                            */
  LDMA_CH_TypeDef CH_CLR[16U];                  /**< DMA Channel Registers                              */
  uint32_t        RESERVED5[808U];              /**< Reserved for future use                            */
  __IM uint32_t   IPVERSION_TGL;                /**< IP version                                         */
  __IOM uint32_t  EN_TGL;                       /**< Module enable disable Register                     */
  __IOM uint32_t  SWRST_TGL;                    /**< Software Reset Register                            */
  __IOM uint32_t  CTRL_TGL;                     /**< Control Register                                   */
  __IM uint32_t   STATUS_TGL;                   /**< Status Register                                    */
  __IOM uint32_t  SYNCSWSET_TGL;                /**< Sync Trig Sw Set Register                          */
  __IOM uint32_t  SYNCSWCLR_TGL;                /**< Sync Trig Sw Clear register                        */
  __IOM uint32_t  SYNCHWEN_TGL;                 /**< Sync HW trigger enable register                    */
  __IOM uint32_t  SYNCHWSEL_TGL;                /**< Sync HW trigger selection register                 */
  __IM uint32_t   SYNCSTATUS_TGL;               /**< Sync Trigger Status Register                       */
  __IOM uint32_t  CHEN_TGL;                     /**< Channel Enable Register                            */
  __IOM uint32_t  CHDIS_TGL;                    /**< Channel Disable Register                           */
  __IM uint32_t   CHSTATUS_TGL;                 /**< Channel Status Register                            */
  __IM uint32_t   CHBUSY_TGL;                   /**< Channel Busy Register                              */
  __IOM uint32_t  CHDONE_TGL;                   /**< Channel Linking Done Register                      */
  __IOM uint32_t  DBGHALT_TGL;                  /**< Channel Debug Halt Register                        */
  __IOM uint32_t  SWREQ_TGL;                    /**< Channel Software Transfer Request                  */
  __IOM uint32_t  REQDIS_TGL;                   /**< Channel Request Disable Register                   */
  __IM uint32_t   REQPEND_TGL;                  /**< Channel Requests Pending Register                  */
  __IOM uint32_t  LINKLOAD_TGL;                 /**< Channel Link Load Register                         */
  __IOM uint32_t  REQCLEAR_TGL;                 /**< Channel Request Clear Register                     */
  __IOM uint32_t  IF_TGL;                       /**< Interrupt Flag Register                            */
  __IOM uint32_t  IEN_TGL;                      /**< Done Interrupt Enable Register                     */
  uint32_t        RESERVED6[1U];                /**< Reserved for future use                            */
  LDMA_CH_TypeDef CH_TGL[16U];                  /**< DMA Channel Registers                              */
} LDMA_TypeDef;
/** @} End of group EFR32FG25_LDMA */

/**************************************************************************//**
 * @addtogroup EFR32FG25_LDMA
 * @{
 * @defgroup EFR32FG25_LDMA_BitFields LDMA Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for LDMA IPVERSION */
#define _LDMA_IPVERSION_RESETVALUE              0x00000001UL                             /**< Default value for LDMA_IPVERSION            */
#define _LDMA_IPVERSION_MASK                    0xFFFFFFFFUL                             /**< Mask for LDMA_IPVERSION                     */
#define _LDMA_IPVERSION_IPVERSION_SHIFT         0                                        /**< Shift value for LDMA_IPVERSION              */
#define _LDMA_IPVERSION_IPVERSION_MASK          0xFFFFFFFFUL                             /**< Bit mask for LDMA_IPVERSION                 */
#define _LDMA_IPVERSION_IPVERSION_DEFAULT       0x00000001UL                             /**< Mode DEFAULT for LDMA_IPVERSION             */
#define LDMA_IPVERSION_IPVERSION_DEFAULT        (_LDMA_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for LDMA_IPVERSION     */

/* Bit fields for LDMA EN */
#define _LDMA_EN_RESETVALUE                     0x00000000UL                            /**< Default value for LDMA_EN                   */
#define _LDMA_EN_MASK                           0x00000003UL                            /**< Mask for LDMA_EN                            */
#define LDMA_EN_EN                              (0x1UL << 0)                            /**< Module Enable                               */
#define _LDMA_EN_EN_SHIFT                       0                                       /**< Shift value for LDMA_EN                     */
#define _LDMA_EN_EN_MASK                        0x1UL                                   /**< Bit mask for LDMA_EN                        */
#define _LDMA_EN_EN_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for LDMA_EN                    */
#define LDMA_EN_EN_DEFAULT                      (_LDMA_EN_EN_DEFAULT << 0)              /**< Shifted mode DEFAULT for LDMA_EN            */
#define LDMA_EN_DISABLING                       (0x1UL << 1)                            /**< Disablement busy status                     */
#define _LDMA_EN_DISABLING_SHIFT                1                                       /**< Shift value for LDMA_DISABLING              */
#define _LDMA_EN_DISABLING_MASK                 0x2UL                                   /**< Bit mask for LDMA_DISABLING                 */
#define _LDMA_EN_DISABLING_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for LDMA_EN                    */
#define LDMA_EN_DISABLING_DEFAULT               (_LDMA_EN_DISABLING_DEFAULT << 1)       /**< Shifted mode DEFAULT for LDMA_EN            */

/* Bit fields for LDMA SWRST */
#define _LDMA_SWRST_RESETVALUE                  0x00000000UL                            /**< Default value for LDMA_SWRST                */
#define _LDMA_SWRST_MASK                        0x00000003UL                            /**< Mask for LDMA_SWRST                         */
#define LDMA_SWRST_SWRST                        (0x1UL << 0)                            /**< Software Reset Command                      */
#define _LDMA_SWRST_SWRST_SHIFT                 0                                       /**< Shift value for LDMA_SWRST                  */
#define _LDMA_SWRST_SWRST_MASK                  0x1UL                                   /**< Bit mask for LDMA_SWRST                     */
#define _LDMA_SWRST_SWRST_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for LDMA_SWRST                 */
#define LDMA_SWRST_SWRST_DEFAULT                (_LDMA_SWRST_SWRST_DEFAULT << 0)        /**< Shifted mode DEFAULT for LDMA_SWRST         */
#define LDMA_SWRST_RESETTING                    (0x1UL << 1)                            /**< Software Reset Busy Status                  */
#define _LDMA_SWRST_RESETTING_SHIFT             1                                       /**< Shift value for LDMA_RESETTING              */
#define _LDMA_SWRST_RESETTING_MASK              0x2UL                                   /**< Bit mask for LDMA_RESETTING                 */
#define _LDMA_SWRST_RESETTING_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_SWRST                 */
#define LDMA_SWRST_RESETTING_DEFAULT            (_LDMA_SWRST_RESETTING_DEFAULT << 1)    /**< Shifted mode DEFAULT for LDMA_SWRST         */

/* Bit fields for LDMA CTRL */
#define _LDMA_CTRL_RESETVALUE                   0x1E000000UL                            /**< Default value for LDMA_CTRL                 */
#define _LDMA_CTRL_MASK                         0x3F000000UL                            /**< Mask for LDMA_CTRL                          */
#define _LDMA_CTRL_NUMFIXED_SHIFT               24                                      /**< Shift value for LDMA_NUMFIXED               */
#define _LDMA_CTRL_NUMFIXED_MASK                0x3F000000UL                            /**< Bit mask for LDMA_NUMFIXED                  */
#define _LDMA_CTRL_NUMFIXED_DEFAULT             0x0000001EUL                            /**< Mode DEFAULT for LDMA_CTRL                  */
#define LDMA_CTRL_NUMFIXED_DEFAULT              (_LDMA_CTRL_NUMFIXED_DEFAULT << 24)     /**< Shifted mode DEFAULT for LDMA_CTRL          */

/* Bit fields for LDMA STATUS */
#define _LDMA_STATUS_RESETVALUE                 0x10000078UL                            /**< Default value for LDMA_STATUS               */
#define _LDMA_STATUS_MASK                       0x1F000F7BUL                            /**< Mask for LDMA_STATUS                        */
#define LDMA_STATUS_ANYBUSY                     (0x1UL << 0)                            /**< Any DMA Channel Busy                        */
#define _LDMA_STATUS_ANYBUSY_SHIFT              0                                       /**< Shift value for LDMA_ANYBUSY                */
#define _LDMA_STATUS_ANYBUSY_MASK               0x1UL                                   /**< Bit mask for LDMA_ANYBUSY                   */
#define _LDMA_STATUS_ANYBUSY_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_STATUS                */
#define LDMA_STATUS_ANYBUSY_DEFAULT             (_LDMA_STATUS_ANYBUSY_DEFAULT << 0)     /**< Shifted mode DEFAULT for LDMA_STATUS        */
#define LDMA_STATUS_ANYREQ                      (0x1UL << 1)                            /**< Any DMA Channel Request Pending             */
#define _LDMA_STATUS_ANYREQ_SHIFT               1                                       /**< Shift value for LDMA_ANYREQ                 */
#define _LDMA_STATUS_ANYREQ_MASK                0x2UL                                   /**< Bit mask for LDMA_ANYREQ                    */
#define _LDMA_STATUS_ANYREQ_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for LDMA_STATUS                */
#define LDMA_STATUS_ANYREQ_DEFAULT              (_LDMA_STATUS_ANYREQ_DEFAULT << 1)      /**< Shifted mode DEFAULT for LDMA_STATUS        */
#define _LDMA_STATUS_CHGRANT_SHIFT              3                                       /**< Shift value for LDMA_CHGRANT                */
#define _LDMA_STATUS_CHGRANT_MASK               0x78UL                                  /**< Bit mask for LDMA_CHGRANT                   */
#define _LDMA_STATUS_CHGRANT_DEFAULT            0x0000000FUL                            /**< Mode DEFAULT for LDMA_STATUS                */
#define LDMA_STATUS_CHGRANT_DEFAULT             (_LDMA_STATUS_CHGRANT_DEFAULT << 3)     /**< Shifted mode DEFAULT for LDMA_STATUS        */
#define _LDMA_STATUS_CHERROR_SHIFT              8                                       /**< Shift value for LDMA_CHERROR                */
#define _LDMA_STATUS_CHERROR_MASK               0xF00UL                                 /**< Bit mask for LDMA_CHERROR                   */
#define _LDMA_STATUS_CHERROR_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_STATUS                */
#define LDMA_STATUS_CHERROR_DEFAULT             (_LDMA_STATUS_CHERROR_DEFAULT << 8)     /**< Shifted mode DEFAULT for LDMA_STATUS        */
#define _LDMA_STATUS_CHNUM_SHIFT                24                                      /**< Shift value for LDMA_CHNUM                  */
#define _LDMA_STATUS_CHNUM_MASK                 0x1F000000UL                            /**< Bit mask for LDMA_CHNUM                     */
#define _LDMA_STATUS_CHNUM_DEFAULT              0x00000010UL                            /**< Mode DEFAULT for LDMA_STATUS                */
#define LDMA_STATUS_CHNUM_DEFAULT               (_LDMA_STATUS_CHNUM_DEFAULT << 24)      /**< Shifted mode DEFAULT for LDMA_STATUS        */

/* Bit fields for LDMA SYNCSWSET */
#define _LDMA_SYNCSWSET_RESETVALUE              0x00000000UL                             /**< Default value for LDMA_SYNCSWSET            */
#define _LDMA_SYNCSWSET_MASK                    0x000000FFUL                             /**< Mask for LDMA_SYNCSWSET                     */
#define _LDMA_SYNCSWSET_SYNCSWSET_SHIFT         0                                        /**< Shift value for LDMA_SYNCSWSET              */
#define _LDMA_SYNCSWSET_SYNCSWSET_MASK          0xFFUL                                   /**< Bit mask for LDMA_SYNCSWSET                 */
#define _LDMA_SYNCSWSET_SYNCSWSET_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for LDMA_SYNCSWSET             */
#define LDMA_SYNCSWSET_SYNCSWSET_DEFAULT        (_LDMA_SYNCSWSET_SYNCSWSET_DEFAULT << 0) /**< Shifted mode DEFAULT for LDMA_SYNCSWSET     */

/* Bit fields for LDMA SYNCSWCLR */
#define _LDMA_SYNCSWCLR_RESETVALUE              0x00000000UL                             /**< Default value for LDMA_SYNCSWCLR            */
#define _LDMA_SYNCSWCLR_MASK                    0x000000FFUL                             /**< Mask for LDMA_SYNCSWCLR                     */
#define _LDMA_SYNCSWCLR_SYNCSWCLR_SHIFT         0                                        /**< Shift value for LDMA_SYNCSWCLR              */
#define _LDMA_SYNCSWCLR_SYNCSWCLR_MASK          0xFFUL                                   /**< Bit mask for LDMA_SYNCSWCLR                 */
#define _LDMA_SYNCSWCLR_SYNCSWCLR_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for LDMA_SYNCSWCLR             */
#define LDMA_SYNCSWCLR_SYNCSWCLR_DEFAULT        (_LDMA_SYNCSWCLR_SYNCSWCLR_DEFAULT << 0) /**< Shifted mode DEFAULT for LDMA_SYNCSWCLR     */

/* Bit fields for LDMA SYNCHWEN */
#define _LDMA_SYNCHWEN_RESETVALUE               0x00000000UL                             /**< Default value for LDMA_SYNCHWEN             */
#define _LDMA_SYNCHWEN_MASK                     0x00FF00FFUL                             /**< Mask for LDMA_SYNCHWEN                      */
#define _LDMA_SYNCHWEN_SYNCSETEN_SHIFT          0                                        /**< Shift value for LDMA_SYNCSETEN              */
#define _LDMA_SYNCHWEN_SYNCSETEN_MASK           0xFFUL                                   /**< Bit mask for LDMA_SYNCSETEN                 */
#define _LDMA_SYNCHWEN_SYNCSETEN_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for LDMA_SYNCHWEN              */
#define LDMA_SYNCHWEN_SYNCSETEN_DEFAULT         (_LDMA_SYNCHWEN_SYNCSETEN_DEFAULT << 0)  /**< Shifted mode DEFAULT for LDMA_SYNCHWEN      */
#define _LDMA_SYNCHWEN_SYNCCLREN_SHIFT          16                                       /**< Shift value for LDMA_SYNCCLREN              */
#define _LDMA_SYNCHWEN_SYNCCLREN_MASK           0xFF0000UL                               /**< Bit mask for LDMA_SYNCCLREN                 */
#define _LDMA_SYNCHWEN_SYNCCLREN_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for LDMA_SYNCHWEN              */
#define LDMA_SYNCHWEN_SYNCCLREN_DEFAULT         (_LDMA_SYNCHWEN_SYNCCLREN_DEFAULT << 16) /**< Shifted mode DEFAULT for LDMA_SYNCHWEN      */

/* Bit fields for LDMA SYNCHWSEL */
#define _LDMA_SYNCHWSEL_RESETVALUE              0x00000000UL                                /**< Default value for LDMA_SYNCHWSEL            */
#define _LDMA_SYNCHWSEL_MASK                    0x00FF00FFUL                                /**< Mask for LDMA_SYNCHWSEL                     */
#define _LDMA_SYNCHWSEL_SYNCSETEDGE_SHIFT       0                                           /**< Shift value for LDMA_SYNCSETEDGE            */
#define _LDMA_SYNCHWSEL_SYNCSETEDGE_MASK        0xFFUL                                      /**< Bit mask for LDMA_SYNCSETEDGE               */
#define _LDMA_SYNCHWSEL_SYNCSETEDGE_DEFAULT     0x00000000UL                                /**< Mode DEFAULT for LDMA_SYNCHWSEL             */
#define _LDMA_SYNCHWSEL_SYNCSETEDGE_RISE        0x00000000UL                                /**< Mode RISE for LDMA_SYNCHWSEL                */
#define _LDMA_SYNCHWSEL_SYNCSETEDGE_FALL        0x00000001UL                                /**< Mode FALL for LDMA_SYNCHWSEL                */
#define LDMA_SYNCHWSEL_SYNCSETEDGE_DEFAULT      (_LDMA_SYNCHWSEL_SYNCSETEDGE_DEFAULT << 0)  /**< Shifted mode DEFAULT for LDMA_SYNCHWSEL     */
#define LDMA_SYNCHWSEL_SYNCSETEDGE_RISE         (_LDMA_SYNCHWSEL_SYNCSETEDGE_RISE << 0)     /**< Shifted mode RISE for LDMA_SYNCHWSEL        */
#define LDMA_SYNCHWSEL_SYNCSETEDGE_FALL         (_LDMA_SYNCHWSEL_SYNCSETEDGE_FALL << 0)     /**< Shifted mode FALL for LDMA_SYNCHWSEL        */
#define _LDMA_SYNCHWSEL_SYNCCLREDGE_SHIFT       16                                          /**< Shift value for LDMA_SYNCCLREDGE            */
#define _LDMA_SYNCHWSEL_SYNCCLREDGE_MASK        0xFF0000UL                                  /**< Bit mask for LDMA_SYNCCLREDGE               */
#define _LDMA_SYNCHWSEL_SYNCCLREDGE_DEFAULT     0x00000000UL                                /**< Mode DEFAULT for LDMA_SYNCHWSEL             */
#define _LDMA_SYNCHWSEL_SYNCCLREDGE_RISE        0x00000000UL                                /**< Mode RISE for LDMA_SYNCHWSEL                */
#define _LDMA_SYNCHWSEL_SYNCCLREDGE_FALL        0x00000001UL                                /**< Mode FALL for LDMA_SYNCHWSEL                */
#define LDMA_SYNCHWSEL_SYNCCLREDGE_DEFAULT      (_LDMA_SYNCHWSEL_SYNCCLREDGE_DEFAULT << 16) /**< Shifted mode DEFAULT for LDMA_SYNCHWSEL     */
#define LDMA_SYNCHWSEL_SYNCCLREDGE_RISE         (_LDMA_SYNCHWSEL_SYNCCLREDGE_RISE << 16)    /**< Shifted mode RISE for LDMA_SYNCHWSEL        */
#define LDMA_SYNCHWSEL_SYNCCLREDGE_FALL         (_LDMA_SYNCHWSEL_SYNCCLREDGE_FALL << 16)    /**< Shifted mode FALL for LDMA_SYNCHWSEL        */

/* Bit fields for LDMA SYNCSTATUS */
#define _LDMA_SYNCSTATUS_RESETVALUE             0x00000000UL                             /**< Default value for LDMA_SYNCSTATUS           */
#define _LDMA_SYNCSTATUS_MASK                   0x000000FFUL                             /**< Mask for LDMA_SYNCSTATUS                    */
#define _LDMA_SYNCSTATUS_SYNCTRIG_SHIFT         0                                        /**< Shift value for LDMA_SYNCTRIG               */
#define _LDMA_SYNCSTATUS_SYNCTRIG_MASK          0xFFUL                                   /**< Bit mask for LDMA_SYNCTRIG                  */
#define _LDMA_SYNCSTATUS_SYNCTRIG_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for LDMA_SYNCSTATUS            */
#define LDMA_SYNCSTATUS_SYNCTRIG_DEFAULT        (_LDMA_SYNCSTATUS_SYNCTRIG_DEFAULT << 0) /**< Shifted mode DEFAULT for LDMA_SYNCSTATUS    */

/* Bit fields for LDMA CHEN */
#define _LDMA_CHEN_RESETVALUE                   0x00000000UL                            /**< Default value for LDMA_CHEN                 */
#define _LDMA_CHEN_MASK                         0x0000FFFFUL                            /**< Mask for LDMA_CHEN                          */
#define _LDMA_CHEN_CHEN_SHIFT                   0                                       /**< Shift value for LDMA_CHEN                   */
#define _LDMA_CHEN_CHEN_MASK                    0xFFFFUL                                /**< Bit mask for LDMA_CHEN                      */
#define _LDMA_CHEN_CHEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_CHEN                  */
#define LDMA_CHEN_CHEN_DEFAULT                  (_LDMA_CHEN_CHEN_DEFAULT << 0)          /**< Shifted mode DEFAULT for LDMA_CHEN          */

/* Bit fields for LDMA CHDIS */
#define _LDMA_CHDIS_RESETVALUE                  0x00000000UL                            /**< Default value for LDMA_CHDIS                */
#define _LDMA_CHDIS_MASK                        0x0000FFFFUL                            /**< Mask for LDMA_CHDIS                         */
#define _LDMA_CHDIS_CHDIS_SHIFT                 0                                       /**< Shift value for LDMA_CHDIS                  */
#define _LDMA_CHDIS_CHDIS_MASK                  0xFFFFUL                                /**< Bit mask for LDMA_CHDIS                     */
#define _LDMA_CHDIS_CHDIS_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDIS                 */
#define LDMA_CHDIS_CHDIS_DEFAULT                (_LDMA_CHDIS_CHDIS_DEFAULT << 0)        /**< Shifted mode DEFAULT for LDMA_CHDIS         */

/* Bit fields for LDMA CHSTATUS */
#define _LDMA_CHSTATUS_RESETVALUE               0x00000000UL                            /**< Default value for LDMA_CHSTATUS             */
#define _LDMA_CHSTATUS_MASK                     0x0000FFFFUL                            /**< Mask for LDMA_CHSTATUS                      */
#define _LDMA_CHSTATUS_CHSTATUS_SHIFT           0                                       /**< Shift value for LDMA_CHSTATUS               */
#define _LDMA_CHSTATUS_CHSTATUS_MASK            0xFFFFUL                                /**< Bit mask for LDMA_CHSTATUS                  */
#define _LDMA_CHSTATUS_CHSTATUS_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for LDMA_CHSTATUS              */
#define LDMA_CHSTATUS_CHSTATUS_DEFAULT          (_LDMA_CHSTATUS_CHSTATUS_DEFAULT << 0)  /**< Shifted mode DEFAULT for LDMA_CHSTATUS      */

/* Bit fields for LDMA CHBUSY */
#define _LDMA_CHBUSY_RESETVALUE                 0x00000000UL                            /**< Default value for LDMA_CHBUSY               */
#define _LDMA_CHBUSY_MASK                       0x0000FFFFUL                            /**< Mask for LDMA_CHBUSY                        */
#define _LDMA_CHBUSY_BUSY_SHIFT                 0                                       /**< Shift value for LDMA_BUSY                   */
#define _LDMA_CHBUSY_BUSY_MASK                  0xFFFFUL                                /**< Bit mask for LDMA_BUSY                      */
#define _LDMA_CHBUSY_BUSY_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for LDMA_CHBUSY                */
#define LDMA_CHBUSY_BUSY_DEFAULT                (_LDMA_CHBUSY_BUSY_DEFAULT << 0)        /**< Shifted mode DEFAULT for LDMA_CHBUSY        */

/* Bit fields for LDMA CHDONE */
#define _LDMA_CHDONE_RESETVALUE                 0x00000000UL                            /**< Default value for LDMA_CHDONE               */
#define _LDMA_CHDONE_MASK                       0x0000FFFFUL                            /**< Mask for LDMA_CHDONE                        */
#define LDMA_CHDONE_CHDONE0                     (0x1UL << 0)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE0_SHIFT              0                                       /**< Shift value for LDMA_CHDONE0                */
#define _LDMA_CHDONE_CHDONE0_MASK               0x1UL                                   /**< Bit mask for LDMA_CHDONE0                   */
#define _LDMA_CHDONE_CHDONE0_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE0_DEFAULT             (_LDMA_CHDONE_CHDONE0_DEFAULT << 0)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE1                     (0x1UL << 1)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE1_SHIFT              1                                       /**< Shift value for LDMA_CHDONE1                */
#define _LDMA_CHDONE_CHDONE1_MASK               0x2UL                                   /**< Bit mask for LDMA_CHDONE1                   */
#define _LDMA_CHDONE_CHDONE1_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE1_DEFAULT             (_LDMA_CHDONE_CHDONE1_DEFAULT << 1)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE2                     (0x1UL << 2)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE2_SHIFT              2                                       /**< Shift value for LDMA_CHDONE2                */
#define _LDMA_CHDONE_CHDONE2_MASK               0x4UL                                   /**< Bit mask for LDMA_CHDONE2                   */
#define _LDMA_CHDONE_CHDONE2_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE2_DEFAULT             (_LDMA_CHDONE_CHDONE2_DEFAULT << 2)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE3                     (0x1UL << 3)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE3_SHIFT              3                                       /**< Shift value for LDMA_CHDONE3                */
#define _LDMA_CHDONE_CHDONE3_MASK               0x8UL                                   /**< Bit mask for LDMA_CHDONE3                   */
#define _LDMA_CHDONE_CHDONE3_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE3_DEFAULT             (_LDMA_CHDONE_CHDONE3_DEFAULT << 3)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE4                     (0x1UL << 4)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE4_SHIFT              4                                       /**< Shift value for LDMA_CHDONE4                */
#define _LDMA_CHDONE_CHDONE4_MASK               0x10UL                                  /**< Bit mask for LDMA_CHDONE4                   */
#define _LDMA_CHDONE_CHDONE4_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE4_DEFAULT             (_LDMA_CHDONE_CHDONE4_DEFAULT << 4)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE5                     (0x1UL << 5)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE5_SHIFT              5                                       /**< Shift value for LDMA_CHDONE5                */
#define _LDMA_CHDONE_CHDONE5_MASK               0x20UL                                  /**< Bit mask for LDMA_CHDONE5                   */
#define _LDMA_CHDONE_CHDONE5_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE5_DEFAULT             (_LDMA_CHDONE_CHDONE5_DEFAULT << 5)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE6                     (0x1UL << 6)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE6_SHIFT              6                                       /**< Shift value for LDMA_CHDONE6                */
#define _LDMA_CHDONE_CHDONE6_MASK               0x40UL                                  /**< Bit mask for LDMA_CHDONE6                   */
#define _LDMA_CHDONE_CHDONE6_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE6_DEFAULT             (_LDMA_CHDONE_CHDONE6_DEFAULT << 6)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE7                     (0x1UL << 7)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE7_SHIFT              7                                       /**< Shift value for LDMA_CHDONE7                */
#define _LDMA_CHDONE_CHDONE7_MASK               0x80UL                                  /**< Bit mask for LDMA_CHDONE7                   */
#define _LDMA_CHDONE_CHDONE7_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE7_DEFAULT             (_LDMA_CHDONE_CHDONE7_DEFAULT << 7)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE8                     (0x1UL << 8)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE8_SHIFT              8                                       /**< Shift value for LDMA_CHDONE8                */
#define _LDMA_CHDONE_CHDONE8_MASK               0x100UL                                 /**< Bit mask for LDMA_CHDONE8                   */
#define _LDMA_CHDONE_CHDONE8_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE8_DEFAULT             (_LDMA_CHDONE_CHDONE8_DEFAULT << 8)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE9                     (0x1UL << 9)                            /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE9_SHIFT              9                                       /**< Shift value for LDMA_CHDONE9                */
#define _LDMA_CHDONE_CHDONE9_MASK               0x200UL                                 /**< Bit mask for LDMA_CHDONE9                   */
#define _LDMA_CHDONE_CHDONE9_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE9_DEFAULT             (_LDMA_CHDONE_CHDONE9_DEFAULT << 9)     /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE10                    (0x1UL << 10)                           /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE10_SHIFT             10                                      /**< Shift value for LDMA_CHDONE10               */
#define _LDMA_CHDONE_CHDONE10_MASK              0x400UL                                 /**< Bit mask for LDMA_CHDONE10                  */
#define _LDMA_CHDONE_CHDONE10_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE10_DEFAULT            (_LDMA_CHDONE_CHDONE10_DEFAULT << 10)   /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE11                    (0x1UL << 11)                           /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE11_SHIFT             11                                      /**< Shift value for LDMA_CHDONE11               */
#define _LDMA_CHDONE_CHDONE11_MASK              0x800UL                                 /**< Bit mask for LDMA_CHDONE11                  */
#define _LDMA_CHDONE_CHDONE11_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE11_DEFAULT            (_LDMA_CHDONE_CHDONE11_DEFAULT << 11)   /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE12                    (0x1UL << 12)                           /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE12_SHIFT             12                                      /**< Shift value for LDMA_CHDONE12               */
#define _LDMA_CHDONE_CHDONE12_MASK              0x1000UL                                /**< Bit mask for LDMA_CHDONE12                  */
#define _LDMA_CHDONE_CHDONE12_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE12_DEFAULT            (_LDMA_CHDONE_CHDONE12_DEFAULT << 12)   /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE13                    (0x1UL << 13)                           /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE13_SHIFT             13                                      /**< Shift value for LDMA_CHDONE13               */
#define _LDMA_CHDONE_CHDONE13_MASK              0x2000UL                                /**< Bit mask for LDMA_CHDONE13                  */
#define _LDMA_CHDONE_CHDONE13_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE13_DEFAULT            (_LDMA_CHDONE_CHDONE13_DEFAULT << 13)   /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE14                    (0x1UL << 14)                           /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE14_SHIFT             14                                      /**< Shift value for LDMA_CHDONE14               */
#define _LDMA_CHDONE_CHDONE14_MASK              0x4000UL                                /**< Bit mask for LDMA_CHDONE14                  */
#define _LDMA_CHDONE_CHDONE14_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE14_DEFAULT            (_LDMA_CHDONE_CHDONE14_DEFAULT << 14)   /**< Shifted mode DEFAULT for LDMA_CHDONE        */
#define LDMA_CHDONE_CHDONE15                    (0x1UL << 15)                           /**< Channel Linking Done Status flag            */
#define _LDMA_CHDONE_CHDONE15_SHIFT             15                                      /**< Shift value for LDMA_CHDONE15               */
#define _LDMA_CHDONE_CHDONE15_MASK              0x8000UL                                /**< Bit mask for LDMA_CHDONE15                  */
#define _LDMA_CHDONE_CHDONE15_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_CHDONE                */
#define LDMA_CHDONE_CHDONE15_DEFAULT            (_LDMA_CHDONE_CHDONE15_DEFAULT << 15)   /**< Shifted mode DEFAULT for LDMA_CHDONE        */

/* Bit fields for LDMA DBGHALT */
#define _LDMA_DBGHALT_RESETVALUE                0x00000000UL                            /**< Default value for LDMA_DBGHALT              */
#define _LDMA_DBGHALT_MASK                      0x0000FFFFUL                            /**< Mask for LDMA_DBGHALT                       */
#define _LDMA_DBGHALT_DBGHALT_SHIFT             0                                       /**< Shift value for LDMA_DBGHALT                */
#define _LDMA_DBGHALT_DBGHALT_MASK              0xFFFFUL                                /**< Bit mask for LDMA_DBGHALT                   */
#define _LDMA_DBGHALT_DBGHALT_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_DBGHALT               */
#define LDMA_DBGHALT_DBGHALT_DEFAULT            (_LDMA_DBGHALT_DBGHALT_DEFAULT << 0)    /**< Shifted mode DEFAULT for LDMA_DBGHALT       */

/* Bit fields for LDMA SWREQ */
#define _LDMA_SWREQ_RESETVALUE                  0x00000000UL                            /**< Default value for LDMA_SWREQ                */
#define _LDMA_SWREQ_MASK                        0x0000FFFFUL                            /**< Mask for LDMA_SWREQ                         */
#define _LDMA_SWREQ_SWREQ_SHIFT                 0                                       /**< Shift value for LDMA_SWREQ                  */
#define _LDMA_SWREQ_SWREQ_MASK                  0xFFFFUL                                /**< Bit mask for LDMA_SWREQ                     */
#define _LDMA_SWREQ_SWREQ_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for LDMA_SWREQ                 */
#define LDMA_SWREQ_SWREQ_DEFAULT                (_LDMA_SWREQ_SWREQ_DEFAULT << 0)        /**< Shifted mode DEFAULT for LDMA_SWREQ         */

/* Bit fields for LDMA REQDIS */
#define _LDMA_REQDIS_RESETVALUE                 0x00000000UL                            /**< Default value for LDMA_REQDIS               */
#define _LDMA_REQDIS_MASK                       0x0000FFFFUL                            /**< Mask for LDMA_REQDIS                        */
#define _LDMA_REQDIS_REQDIS_SHIFT               0                                       /**< Shift value for LDMA_REQDIS                 */
#define _LDMA_REQDIS_REQDIS_MASK                0xFFFFUL                                /**< Bit mask for LDMA_REQDIS                    */
#define _LDMA_REQDIS_REQDIS_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for LDMA_REQDIS                */
#define LDMA_REQDIS_REQDIS_DEFAULT              (_LDMA_REQDIS_REQDIS_DEFAULT << 0)      /**< Shifted mode DEFAULT for LDMA_REQDIS        */

/* Bit fields for LDMA REQPEND */
#define _LDMA_REQPEND_RESETVALUE                0x00000000UL                            /**< Default value for LDMA_REQPEND              */
#define _LDMA_REQPEND_MASK                      0x0000FFFFUL                            /**< Mask for LDMA_REQPEND                       */
#define _LDMA_REQPEND_REQPEND_SHIFT             0                                       /**< Shift value for LDMA_REQPEND                */
#define _LDMA_REQPEND_REQPEND_MASK              0xFFFFUL                                /**< Bit mask for LDMA_REQPEND                   */
#define _LDMA_REQPEND_REQPEND_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_REQPEND               */
#define LDMA_REQPEND_REQPEND_DEFAULT            (_LDMA_REQPEND_REQPEND_DEFAULT << 0)    /**< Shifted mode DEFAULT for LDMA_REQPEND       */

/* Bit fields for LDMA LINKLOAD */
#define _LDMA_LINKLOAD_RESETVALUE               0x00000000UL                            /**< Default value for LDMA_LINKLOAD             */
#define _LDMA_LINKLOAD_MASK                     0x0000FFFFUL                            /**< Mask for LDMA_LINKLOAD                      */
#define _LDMA_LINKLOAD_LINKLOAD_SHIFT           0                                       /**< Shift value for LDMA_LINKLOAD               */
#define _LDMA_LINKLOAD_LINKLOAD_MASK            0xFFFFUL                                /**< Bit mask for LDMA_LINKLOAD                  */
#define _LDMA_LINKLOAD_LINKLOAD_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for LDMA_LINKLOAD              */
#define LDMA_LINKLOAD_LINKLOAD_DEFAULT          (_LDMA_LINKLOAD_LINKLOAD_DEFAULT << 0)  /**< Shifted mode DEFAULT for LDMA_LINKLOAD      */

/* Bit fields for LDMA REQCLEAR */
#define _LDMA_REQCLEAR_RESETVALUE               0x00000000UL                            /**< Default value for LDMA_REQCLEAR             */
#define _LDMA_REQCLEAR_MASK                     0x0000FFFFUL                            /**< Mask for LDMA_REQCLEAR                      */
#define _LDMA_REQCLEAR_REQCLEAR_SHIFT           0                                       /**< Shift value for LDMA_REQCLEAR               */
#define _LDMA_REQCLEAR_REQCLEAR_MASK            0xFFFFUL                                /**< Bit mask for LDMA_REQCLEAR                  */
#define _LDMA_REQCLEAR_REQCLEAR_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for LDMA_REQCLEAR              */
#define LDMA_REQCLEAR_REQCLEAR_DEFAULT          (_LDMA_REQCLEAR_REQCLEAR_DEFAULT << 0)  /**< Shifted mode DEFAULT for LDMA_REQCLEAR      */

/* Bit fields for LDMA IF */
#define _LDMA_IF_RESETVALUE                     0x00000000UL                            /**< Default value for LDMA_IF                   */
#define _LDMA_IF_MASK                           0x8000FFFFUL                            /**< Mask for LDMA_IF                            */
#define LDMA_IF_DONE0                           (0x1UL << 0)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE0_SHIFT                    0                                       /**< Shift value for LDMA_DONE0                  */
#define _LDMA_IF_DONE0_MASK                     0x1UL                                   /**< Bit mask for LDMA_DONE0                     */
#define _LDMA_IF_DONE0_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE0_DEFAULT                   (_LDMA_IF_DONE0_DEFAULT << 0)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE1                           (0x1UL << 1)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE1_SHIFT                    1                                       /**< Shift value for LDMA_DONE1                  */
#define _LDMA_IF_DONE1_MASK                     0x2UL                                   /**< Bit mask for LDMA_DONE1                     */
#define _LDMA_IF_DONE1_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE1_DEFAULT                   (_LDMA_IF_DONE1_DEFAULT << 1)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE2                           (0x1UL << 2)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE2_SHIFT                    2                                       /**< Shift value for LDMA_DONE2                  */
#define _LDMA_IF_DONE2_MASK                     0x4UL                                   /**< Bit mask for LDMA_DONE2                     */
#define _LDMA_IF_DONE2_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE2_DEFAULT                   (_LDMA_IF_DONE2_DEFAULT << 2)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE3                           (0x1UL << 3)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE3_SHIFT                    3                                       /**< Shift value for LDMA_DONE3                  */
#define _LDMA_IF_DONE3_MASK                     0x8UL                                   /**< Bit mask for LDMA_DONE3                     */
#define _LDMA_IF_DONE3_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE3_DEFAULT                   (_LDMA_IF_DONE3_DEFAULT << 3)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE4                           (0x1UL << 4)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE4_SHIFT                    4                                       /**< Shift value for LDMA_DONE4                  */
#define _LDMA_IF_DONE4_MASK                     0x10UL                                  /**< Bit mask for LDMA_DONE4                     */
#define _LDMA_IF_DONE4_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE4_DEFAULT                   (_LDMA_IF_DONE4_DEFAULT << 4)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE5                           (0x1UL << 5)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE5_SHIFT                    5                                       /**< Shift value for LDMA_DONE5                  */
#define _LDMA_IF_DONE5_MASK                     0x20UL                                  /**< Bit mask for LDMA_DONE5                     */
#define _LDMA_IF_DONE5_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE5_DEFAULT                   (_LDMA_IF_DONE5_DEFAULT << 5)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE6                           (0x1UL << 6)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE6_SHIFT                    6                                       /**< Shift value for LDMA_DONE6                  */
#define _LDMA_IF_DONE6_MASK                     0x40UL                                  /**< Bit mask for LDMA_DONE6                     */
#define _LDMA_IF_DONE6_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE6_DEFAULT                   (_LDMA_IF_DONE6_DEFAULT << 6)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE7                           (0x1UL << 7)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE7_SHIFT                    7                                       /**< Shift value for LDMA_DONE7                  */
#define _LDMA_IF_DONE7_MASK                     0x80UL                                  /**< Bit mask for LDMA_DONE7                     */
#define _LDMA_IF_DONE7_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE7_DEFAULT                   (_LDMA_IF_DONE7_DEFAULT << 7)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE8                           (0x1UL << 8)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE8_SHIFT                    8                                       /**< Shift value for LDMA_DONE8                  */
#define _LDMA_IF_DONE8_MASK                     0x100UL                                 /**< Bit mask for LDMA_DONE8                     */
#define _LDMA_IF_DONE8_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE8_DEFAULT                   (_LDMA_IF_DONE8_DEFAULT << 8)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE9                           (0x1UL << 9)                            /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE9_SHIFT                    9                                       /**< Shift value for LDMA_DONE9                  */
#define _LDMA_IF_DONE9_MASK                     0x200UL                                 /**< Bit mask for LDMA_DONE9                     */
#define _LDMA_IF_DONE9_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE9_DEFAULT                   (_LDMA_IF_DONE9_DEFAULT << 9)           /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE10                          (0x1UL << 10)                           /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE10_SHIFT                   10                                      /**< Shift value for LDMA_DONE10                 */
#define _LDMA_IF_DONE10_MASK                    0x400UL                                 /**< Bit mask for LDMA_DONE10                    */
#define _LDMA_IF_DONE10_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE10_DEFAULT                  (_LDMA_IF_DONE10_DEFAULT << 10)         /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE11                          (0x1UL << 11)                           /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE11_SHIFT                   11                                      /**< Shift value for LDMA_DONE11                 */
#define _LDMA_IF_DONE11_MASK                    0x800UL                                 /**< Bit mask for LDMA_DONE11                    */
#define _LDMA_IF_DONE11_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE11_DEFAULT                  (_LDMA_IF_DONE11_DEFAULT << 11)         /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE12                          (0x1UL << 12)                           /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE12_SHIFT                   12                                      /**< Shift value for LDMA_DONE12                 */
#define _LDMA_IF_DONE12_MASK                    0x1000UL                                /**< Bit mask for LDMA_DONE12                    */
#define _LDMA_IF_DONE12_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE12_DEFAULT                  (_LDMA_IF_DONE12_DEFAULT << 12)         /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE13                          (0x1UL << 13)                           /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE13_SHIFT                   13                                      /**< Shift value for LDMA_DONE13                 */
#define _LDMA_IF_DONE13_MASK                    0x2000UL                                /**< Bit mask for LDMA_DONE13                    */
#define _LDMA_IF_DONE13_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE13_DEFAULT                  (_LDMA_IF_DONE13_DEFAULT << 13)         /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE14                          (0x1UL << 14)                           /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE14_SHIFT                   14                                      /**< Shift value for LDMA_DONE14                 */
#define _LDMA_IF_DONE14_MASK                    0x4000UL                                /**< Bit mask for LDMA_DONE14                    */
#define _LDMA_IF_DONE14_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE14_DEFAULT                  (_LDMA_IF_DONE14_DEFAULT << 14)         /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_DONE15                          (0x1UL << 15)                           /**< Done Interrupt Flag                         */
#define _LDMA_IF_DONE15_SHIFT                   15                                      /**< Shift value for LDMA_DONE15                 */
#define _LDMA_IF_DONE15_MASK                    0x8000UL                                /**< Bit mask for LDMA_DONE15                    */
#define _LDMA_IF_DONE15_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_DONE15_DEFAULT                  (_LDMA_IF_DONE15_DEFAULT << 15)         /**< Shifted mode DEFAULT for LDMA_IF            */
#define LDMA_IF_ERROR                           (0x1UL << 31)                           /**< Error Flag                                  */
#define _LDMA_IF_ERROR_SHIFT                    31                                      /**< Shift value for LDMA_ERROR                  */
#define _LDMA_IF_ERROR_MASK                     0x80000000UL                            /**< Bit mask for LDMA_ERROR                     */
#define _LDMA_IF_ERROR_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LDMA_IF                    */
#define LDMA_IF_ERROR_DEFAULT                   (_LDMA_IF_ERROR_DEFAULT << 31)          /**< Shifted mode DEFAULT for LDMA_IF            */

/* Bit fields for LDMA IEN */
#define _LDMA_IEN_RESETVALUE                    0x00000000UL                            /**< Default value for LDMA_IEN                  */
#define _LDMA_IEN_MASK                          0x8000FFFFUL                            /**< Mask for LDMA_IEN                           */
#define LDMA_IEN_DONE0                          (0x1UL << 0)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE0_SHIFT                   0                                       /**< Shift value for LDMA_DONE0                  */
#define _LDMA_IEN_DONE0_MASK                    0x1UL                                   /**< Bit mask for LDMA_DONE0                     */
#define _LDMA_IEN_DONE0_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE0_DEFAULT                  (_LDMA_IEN_DONE0_DEFAULT << 0)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE1                          (0x1UL << 1)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE1_SHIFT                   1                                       /**< Shift value for LDMA_DONE1                  */
#define _LDMA_IEN_DONE1_MASK                    0x2UL                                   /**< Bit mask for LDMA_DONE1                     */
#define _LDMA_IEN_DONE1_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE1_DEFAULT                  (_LDMA_IEN_DONE1_DEFAULT << 1)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE2                          (0x1UL << 2)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE2_SHIFT                   2                                       /**< Shift value for LDMA_DONE2                  */
#define _LDMA_IEN_DONE2_MASK                    0x4UL                                   /**< Bit mask for LDMA_DONE2                     */
#define _LDMA_IEN_DONE2_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE2_DEFAULT                  (_LDMA_IEN_DONE2_DEFAULT << 2)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE3                          (0x1UL << 3)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE3_SHIFT                   3                                       /**< Shift value for LDMA_DONE3                  */
#define _LDMA_IEN_DONE3_MASK                    0x8UL                                   /**< Bit mask for LDMA_DONE3                     */
#define _LDMA_IEN_DONE3_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE3_DEFAULT                  (_LDMA_IEN_DONE3_DEFAULT << 3)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE4                          (0x1UL << 4)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE4_SHIFT                   4                                       /**< Shift value for LDMA_DONE4                  */
#define _LDMA_IEN_DONE4_MASK                    0x10UL                                  /**< Bit mask for LDMA_DONE4                     */
#define _LDMA_IEN_DONE4_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE4_DEFAULT                  (_LDMA_IEN_DONE4_DEFAULT << 4)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE5                          (0x1UL << 5)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE5_SHIFT                   5                                       /**< Shift value for LDMA_DONE5                  */
#define _LDMA_IEN_DONE5_MASK                    0x20UL                                  /**< Bit mask for LDMA_DONE5                     */
#define _LDMA_IEN_DONE5_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE5_DEFAULT                  (_LDMA_IEN_DONE5_DEFAULT << 5)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE6                          (0x1UL << 6)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE6_SHIFT                   6                                       /**< Shift value for LDMA_DONE6                  */
#define _LDMA_IEN_DONE6_MASK                    0x40UL                                  /**< Bit mask for LDMA_DONE6                     */
#define _LDMA_IEN_DONE6_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE6_DEFAULT                  (_LDMA_IEN_DONE6_DEFAULT << 6)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE7                          (0x1UL << 7)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE7_SHIFT                   7                                       /**< Shift value for LDMA_DONE7                  */
#define _LDMA_IEN_DONE7_MASK                    0x80UL                                  /**< Bit mask for LDMA_DONE7                     */
#define _LDMA_IEN_DONE7_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE7_DEFAULT                  (_LDMA_IEN_DONE7_DEFAULT << 7)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE8                          (0x1UL << 8)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE8_SHIFT                   8                                       /**< Shift value for LDMA_DONE8                  */
#define _LDMA_IEN_DONE8_MASK                    0x100UL                                 /**< Bit mask for LDMA_DONE8                     */
#define _LDMA_IEN_DONE8_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE8_DEFAULT                  (_LDMA_IEN_DONE8_DEFAULT << 8)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE9                          (0x1UL << 9)                            /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE9_SHIFT                   9                                       /**< Shift value for LDMA_DONE9                  */
#define _LDMA_IEN_DONE9_MASK                    0x200UL                                 /**< Bit mask for LDMA_DONE9                     */
#define _LDMA_IEN_DONE9_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE9_DEFAULT                  (_LDMA_IEN_DONE9_DEFAULT << 9)          /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE10                         (0x1UL << 10)                           /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE10_SHIFT                  10                                      /**< Shift value for LDMA_DONE10                 */
#define _LDMA_IEN_DONE10_MASK                   0x400UL                                 /**< Bit mask for LDMA_DONE10                    */
#define _LDMA_IEN_DONE10_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE10_DEFAULT                 (_LDMA_IEN_DONE10_DEFAULT << 10)        /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE11                         (0x1UL << 11)                           /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE11_SHIFT                  11                                      /**< Shift value for LDMA_DONE11                 */
#define _LDMA_IEN_DONE11_MASK                   0x800UL                                 /**< Bit mask for LDMA_DONE11                    */
#define _LDMA_IEN_DONE11_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE11_DEFAULT                 (_LDMA_IEN_DONE11_DEFAULT << 11)        /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE12                         (0x1UL << 12)                           /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE12_SHIFT                  12                                      /**< Shift value for LDMA_DONE12                 */
#define _LDMA_IEN_DONE12_MASK                   0x1000UL                                /**< Bit mask for LDMA_DONE12                    */
#define _LDMA_IEN_DONE12_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE12_DEFAULT                 (_LDMA_IEN_DONE12_DEFAULT << 12)        /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE13                         (0x1UL << 13)                           /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE13_SHIFT                  13                                      /**< Shift value for LDMA_DONE13                 */
#define _LDMA_IEN_DONE13_MASK                   0x2000UL                                /**< Bit mask for LDMA_DONE13                    */
#define _LDMA_IEN_DONE13_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE13_DEFAULT                 (_LDMA_IEN_DONE13_DEFAULT << 13)        /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE14                         (0x1UL << 14)                           /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE14_SHIFT                  14                                      /**< Shift value for LDMA_DONE14                 */
#define _LDMA_IEN_DONE14_MASK                   0x4000UL                                /**< Bit mask for LDMA_DONE14                    */
#define _LDMA_IEN_DONE14_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE14_DEFAULT                 (_LDMA_IEN_DONE14_DEFAULT << 14)        /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_DONE15                         (0x1UL << 15)                           /**< Done Interrupt Enable                       */
#define _LDMA_IEN_DONE15_SHIFT                  15                                      /**< Shift value for LDMA_DONE15                 */
#define _LDMA_IEN_DONE15_MASK                   0x8000UL                                /**< Bit mask for LDMA_DONE15                    */
#define _LDMA_IEN_DONE15_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_DONE15_DEFAULT                 (_LDMA_IEN_DONE15_DEFAULT << 15)        /**< Shifted mode DEFAULT for LDMA_IEN           */
#define LDMA_IEN_ERROR                          (0x1UL << 31)                           /**< Error Interrupt Enable                      */
#define _LDMA_IEN_ERROR_SHIFT                   31                                      /**< Shift value for LDMA_ERROR                  */
#define _LDMA_IEN_ERROR_MASK                    0x80000000UL                            /**< Bit mask for LDMA_ERROR                     */
#define _LDMA_IEN_ERROR_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LDMA_IEN                   */
#define LDMA_IEN_ERROR_DEFAULT                  (_LDMA_IEN_ERROR_DEFAULT << 31)         /**< Shifted mode DEFAULT for LDMA_IEN           */

/* Bit fields for LDMA CH_CFG */
#define _LDMA_CH_CFG_RESETVALUE                 0x00000000UL                               /**< Default value for LDMA_CH_CFG               */
#define _LDMA_CH_CFG_MASK                       0x01F30000UL                               /**< Mask for LDMA_CH_CFG                        */
#define _LDMA_CH_CFG_ARBSLOTS_SHIFT             16                                         /**< Shift value for LDMA_ARBSLOTS               */
#define _LDMA_CH_CFG_ARBSLOTS_MASK              0x30000UL                                  /**< Bit mask for LDMA_ARBSLOTS                  */
#define _LDMA_CH_CFG_ARBSLOTS_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LDMA_CH_CFG                */
#define _LDMA_CH_CFG_ARBSLOTS_ONE               0x00000000UL                               /**< Mode ONE for LDMA_CH_CFG                    */
#define _LDMA_CH_CFG_ARBSLOTS_TWO               0x00000001UL                               /**< Mode TWO for LDMA_CH_CFG                    */
#define _LDMA_CH_CFG_ARBSLOTS_FOUR              0x00000002UL                               /**< Mode FOUR for LDMA_CH_CFG                   */
#define _LDMA_CH_CFG_ARBSLOTS_EIGHT             0x00000003UL                               /**< Mode EIGHT for LDMA_CH_CFG                  */
#define LDMA_CH_CFG_ARBSLOTS_DEFAULT            (_LDMA_CH_CFG_ARBSLOTS_DEFAULT << 16)      /**< Shifted mode DEFAULT for LDMA_CH_CFG        */
#define LDMA_CH_CFG_ARBSLOTS_ONE                (_LDMA_CH_CFG_ARBSLOTS_ONE << 16)          /**< Shifted mode ONE for LDMA_CH_CFG            */
#define LDMA_CH_CFG_ARBSLOTS_TWO                (_LDMA_CH_CFG_ARBSLOTS_TWO << 16)          /**< Shifted mode TWO for LDMA_CH_CFG            */
#define LDMA_CH_CFG_ARBSLOTS_FOUR               (_LDMA_CH_CFG_ARBSLOTS_FOUR << 16)         /**< Shifted mode FOUR for LDMA_CH_CFG           */
#define LDMA_CH_CFG_ARBSLOTS_EIGHT              (_LDMA_CH_CFG_ARBSLOTS_EIGHT << 16)        /**< Shifted mode EIGHT for LDMA_CH_CFG          */
#define LDMA_CH_CFG_SRCINCSIGN                  (0x1UL << 20)                              /**< Source Address Increment Sign               */
#define _LDMA_CH_CFG_SRCINCSIGN_SHIFT           20                                         /**< Shift value for LDMA_SRCINCSIGN             */
#define _LDMA_CH_CFG_SRCINCSIGN_MASK            0x100000UL                                 /**< Bit mask for LDMA_SRCINCSIGN                */
#define _LDMA_CH_CFG_SRCINCSIGN_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LDMA_CH_CFG                */
#define _LDMA_CH_CFG_SRCINCSIGN_POSITIVE        0x00000000UL                               /**< Mode POSITIVE for LDMA_CH_CFG               */
#define _LDMA_CH_CFG_SRCINCSIGN_NEGATIVE        0x00000001UL                               /**< Mode NEGATIVE for LDMA_CH_CFG               */
#define LDMA_CH_CFG_SRCINCSIGN_DEFAULT          (_LDMA_CH_CFG_SRCINCSIGN_DEFAULT << 20)    /**< Shifted mode DEFAULT for LDMA_CH_CFG        */
#define LDMA_CH_CFG_SRCINCSIGN_POSITIVE         (_LDMA_CH_CFG_SRCINCSIGN_POSITIVE << 20)   /**< Shifted mode POSITIVE for LDMA_CH_CFG       */
#define LDMA_CH_CFG_SRCINCSIGN_NEGATIVE         (_LDMA_CH_CFG_SRCINCSIGN_NEGATIVE << 20)   /**< Shifted mode NEGATIVE for LDMA_CH_CFG       */
#define LDMA_CH_CFG_DSTINCSIGN                  (0x1UL << 21)                              /**< Destination Address Increment Sign          */
#define _LDMA_CH_CFG_DSTINCSIGN_SHIFT           21                                         /**< Shift value for LDMA_DSTINCSIGN             */
#define _LDMA_CH_CFG_DSTINCSIGN_MASK            0x200000UL                                 /**< Bit mask for LDMA_DSTINCSIGN                */
#define _LDMA_CH_CFG_DSTINCSIGN_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LDMA_CH_CFG                */
#define _LDMA_CH_CFG_DSTINCSIGN_POSITIVE        0x00000000UL                               /**< Mode POSITIVE for LDMA_CH_CFG               */
#define _LDMA_CH_CFG_DSTINCSIGN_NEGATIVE        0x00000001UL                               /**< Mode NEGATIVE for LDMA_CH_CFG               */
#define LDMA_CH_CFG_DSTINCSIGN_DEFAULT          (_LDMA_CH_CFG_DSTINCSIGN_DEFAULT << 21)    /**< Shifted mode DEFAULT for LDMA_CH_CFG        */
#define LDMA_CH_CFG_DSTINCSIGN_POSITIVE         (_LDMA_CH_CFG_DSTINCSIGN_POSITIVE << 21)   /**< Shifted mode POSITIVE for LDMA_CH_CFG       */
#define LDMA_CH_CFG_DSTINCSIGN_NEGATIVE         (_LDMA_CH_CFG_DSTINCSIGN_NEGATIVE << 21)   /**< Shifted mode NEGATIVE for LDMA_CH_CFG       */
#define LDMA_CH_CFG_STRUCTBUSPORT               (0x1UL << 22)                              /**< Structure Fetch Bus Port                    */
#define _LDMA_CH_CFG_STRUCTBUSPORT_SHIFT        22                                         /**< Shift value for LDMA_STRUCTBUSPORT          */
#define _LDMA_CH_CFG_STRUCTBUSPORT_MASK         0x400000UL                                 /**< Bit mask for LDMA_STRUCTBUSPORT             */
#define _LDMA_CH_CFG_STRUCTBUSPORT_DEFAULT      0x00000000UL                               /**< Mode DEFAULT for LDMA_CH_CFG                */
#define _LDMA_CH_CFG_STRUCTBUSPORT_AHBM0        0x00000000UL                               /**< Mode AHBM0 for LDMA_CH_CFG                  */
#define _LDMA_CH_CFG_STRUCTBUSPORT_AHBM1        0x00000001UL                               /**< Mode AHBM1 for LDMA_CH_CFG                  */
#define LDMA_CH_CFG_STRUCTBUSPORT_DEFAULT       (_LDMA_CH_CFG_STRUCTBUSPORT_DEFAULT << 22) /**< Shifted mode DEFAULT for LDMA_CH_CFG        */
#define LDMA_CH_CFG_STRUCTBUSPORT_AHBM0         (_LDMA_CH_CFG_STRUCTBUSPORT_AHBM0 << 22)   /**< Shifted mode AHBM0 for LDMA_CH_CFG          */
#define LDMA_CH_CFG_STRUCTBUSPORT_AHBM1         (_LDMA_CH_CFG_STRUCTBUSPORT_AHBM1 << 22)   /**< Shifted mode AHBM1 for LDMA_CH_CFG          */
#define LDMA_CH_CFG_SRCBUSPORT                  (0x1UL << 23)                              /**< Source Bus Port                             */
#define _LDMA_CH_CFG_SRCBUSPORT_SHIFT           23                                         /**< Shift value for LDMA_SRCBUSPORT             */
#define _LDMA_CH_CFG_SRCBUSPORT_MASK            0x800000UL                                 /**< Bit mask for LDMA_SRCBUSPORT                */
#define _LDMA_CH_CFG_SRCBUSPORT_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LDMA_CH_CFG                */
#define _LDMA_CH_CFG_SRCBUSPORT_AHBM0           0x00000000UL                               /**< Mode AHBM0 for LDMA_CH_CFG                  */
#define _LDMA_CH_CFG_SRCBUSPORT_AHBM1           0x00000001UL                               /**< Mode AHBM1 for LDMA_CH_CFG                  */
#define LDMA_CH_CFG_SRCBUSPORT_DEFAULT          (_LDMA_CH_CFG_SRCBUSPORT_DEFAULT << 23)    /**< Shifted mode DEFAULT for LDMA_CH_CFG        */
#define LDMA_CH_CFG_SRCBUSPORT_AHBM0            (_LDMA_CH_CFG_SRCBUSPORT_AHBM0 << 23)      /**< Shifted mode AHBM0 for LDMA_CH_CFG          */
#define LDMA_CH_CFG_SRCBUSPORT_AHBM1            (_LDMA_CH_CFG_SRCBUSPORT_AHBM1 << 23)      /**< Shifted mode AHBM1 for LDMA_CH_CFG          */
#define LDMA_CH_CFG_DSTBUSPORT                  (0x1UL << 24)                              /**< Destination Bus Port                        */
#define _LDMA_CH_CFG_DSTBUSPORT_SHIFT           24                                         /**< Shift value for LDMA_DSTBUSPORT             */
#define _LDMA_CH_CFG_DSTBUSPORT_MASK            0x1000000UL                                /**< Bit mask for LDMA_DSTBUSPORT                */
#define _LDMA_CH_CFG_DSTBUSPORT_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LDMA_CH_CFG                */
#define _LDMA_CH_CFG_DSTBUSPORT_AHBM0           0x00000000UL                               /**< Mode AHBM0 for LDMA_CH_CFG                  */
#define _LDMA_CH_CFG_DSTBUSPORT_AHBM1           0x00000001UL                               /**< Mode AHBM1 for LDMA_CH_CFG                  */
#define LDMA_CH_CFG_DSTBUSPORT_DEFAULT          (_LDMA_CH_CFG_DSTBUSPORT_DEFAULT << 24)    /**< Shifted mode DEFAULT for LDMA_CH_CFG        */
#define LDMA_CH_CFG_DSTBUSPORT_AHBM0            (_LDMA_CH_CFG_DSTBUSPORT_AHBM0 << 24)      /**< Shifted mode AHBM0 for LDMA_CH_CFG          */
#define LDMA_CH_CFG_DSTBUSPORT_AHBM1            (_LDMA_CH_CFG_DSTBUSPORT_AHBM1 << 24)      /**< Shifted mode AHBM1 for LDMA_CH_CFG          */

/* Bit fields for LDMA CH_LOOP */
#define _LDMA_CH_LOOP_RESETVALUE                0x00000000UL                            /**< Default value for LDMA_CH_LOOP              */
#define _LDMA_CH_LOOP_MASK                      0x000000FFUL                            /**< Mask for LDMA_CH_LOOP                       */
#define _LDMA_CH_LOOP_LOOPCNT_SHIFT             0                                       /**< Shift value for LDMA_LOOPCNT                */
#define _LDMA_CH_LOOP_LOOPCNT_MASK              0xFFUL                                  /**< Bit mask for LDMA_LOOPCNT                   */
#define _LDMA_CH_LOOP_LOOPCNT_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for LDMA_CH_LOOP               */
#define LDMA_CH_LOOP_LOOPCNT_DEFAULT            (_LDMA_CH_LOOP_LOOPCNT_DEFAULT << 0)    /**< Shifted mode DEFAULT for LDMA_CH_LOOP       */

/* Bit fields for LDMA CH_CTRL */
#define _LDMA_CH_CTRL_RESETVALUE                0x00000000UL                                /**< Default value for LDMA_CH_CTRL              */
#define _LDMA_CH_CTRL_MASK                      0xFFFFFFFFUL                                /**< Mask for LDMA_CH_CTRL                       */
#define _LDMA_CH_CTRL_STRUCTTYPE_SHIFT          0                                           /**< Shift value for LDMA_STRUCTTYPE             */
#define _LDMA_CH_CTRL_STRUCTTYPE_MASK           0x3UL                                       /**< Bit mask for LDMA_STRUCTTYPE                */
#define _LDMA_CH_CTRL_STRUCTTYPE_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_STRUCTTYPE_TRANSFER       0x00000000UL                                /**< Mode TRANSFER for LDMA_CH_CTRL              */
#define _LDMA_CH_CTRL_STRUCTTYPE_SYNCHRONIZE    0x00000001UL                                /**< Mode SYNCHRONIZE for LDMA_CH_CTRL           */
#define _LDMA_CH_CTRL_STRUCTTYPE_WRITE          0x00000002UL                                /**< Mode WRITE for LDMA_CH_CTRL                 */
#define LDMA_CH_CTRL_STRUCTTYPE_DEFAULT         (_LDMA_CH_CTRL_STRUCTTYPE_DEFAULT << 0)     /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_STRUCTTYPE_TRANSFER        (_LDMA_CH_CTRL_STRUCTTYPE_TRANSFER << 0)    /**< Shifted mode TRANSFER for LDMA_CH_CTRL      */
#define LDMA_CH_CTRL_STRUCTTYPE_SYNCHRONIZE     (_LDMA_CH_CTRL_STRUCTTYPE_SYNCHRONIZE << 0) /**< Shifted mode SYNCHRONIZE for LDMA_CH_CTRL   */
#define LDMA_CH_CTRL_STRUCTTYPE_WRITE           (_LDMA_CH_CTRL_STRUCTTYPE_WRITE << 0)       /**< Shifted mode WRITE for LDMA_CH_CTRL         */
#define LDMA_CH_CTRL_EXTEND                     (0x1UL << 2)                                /**< Extend                                      */
#define _LDMA_CH_CTRL_EXTEND_SHIFT              2                                           /**< Shift value for LDMA_EXTEND                 */
#define _LDMA_CH_CTRL_EXTEND_MASK               0x4UL                                       /**< Bit mask for LDMA_EXTEND                    */
#define _LDMA_CH_CTRL_EXTEND_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define LDMA_CH_CTRL_EXTEND_DEFAULT             (_LDMA_CH_CTRL_EXTEND_DEFAULT << 2)         /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_STRUCTREQ                  (0x1UL << 3)                                /**< Structure DMA Transfer Request              */
#define _LDMA_CH_CTRL_STRUCTREQ_SHIFT           3                                           /**< Shift value for LDMA_STRUCTREQ              */
#define _LDMA_CH_CTRL_STRUCTREQ_MASK            0x8UL                                       /**< Bit mask for LDMA_STRUCTREQ                 */
#define _LDMA_CH_CTRL_STRUCTREQ_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define LDMA_CH_CTRL_STRUCTREQ_DEFAULT          (_LDMA_CH_CTRL_STRUCTREQ_DEFAULT << 3)      /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define _LDMA_CH_CTRL_XFERCNT_SHIFT             4                                           /**< Shift value for LDMA_XFERCNT                */
#define _LDMA_CH_CTRL_XFERCNT_MASK              0x7FF0UL                                    /**< Bit mask for LDMA_XFERCNT                   */
#define _LDMA_CH_CTRL_XFERCNT_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define LDMA_CH_CTRL_XFERCNT_DEFAULT            (_LDMA_CH_CTRL_XFERCNT_DEFAULT << 4)        /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_BYTESWAP                   (0x1UL << 15)                               /**< Endian Byte Swap                            */
#define _LDMA_CH_CTRL_BYTESWAP_SHIFT            15                                          /**< Shift value for LDMA_BYTESWAP               */
#define _LDMA_CH_CTRL_BYTESWAP_MASK             0x8000UL                                    /**< Bit mask for LDMA_BYTESWAP                  */
#define _LDMA_CH_CTRL_BYTESWAP_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define LDMA_CH_CTRL_BYTESWAP_DEFAULT           (_LDMA_CH_CTRL_BYTESWAP_DEFAULT << 15)      /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define _LDMA_CH_CTRL_BLOCKSIZE_SHIFT           16                                          /**< Shift value for LDMA_BLOCKSIZE              */
#define _LDMA_CH_CTRL_BLOCKSIZE_MASK            0xF0000UL                                   /**< Bit mask for LDMA_BLOCKSIZE                 */
#define _LDMA_CH_CTRL_BLOCKSIZE_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT1           0x00000000UL                                /**< Mode UNIT1 for LDMA_CH_CTRL                 */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT2           0x00000001UL                                /**< Mode UNIT2 for LDMA_CH_CTRL                 */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT3           0x00000002UL                                /**< Mode UNIT3 for LDMA_CH_CTRL                 */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT4           0x00000003UL                                /**< Mode UNIT4 for LDMA_CH_CTRL                 */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT6           0x00000004UL                                /**< Mode UNIT6 for LDMA_CH_CTRL                 */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT8           0x00000005UL                                /**< Mode UNIT8 for LDMA_CH_CTRL                 */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT12          0x00000006UL                                /**< Mode UNIT12 for LDMA_CH_CTRL                */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT16          0x00000007UL                                /**< Mode UNIT16 for LDMA_CH_CTRL                */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT24          0x00000008UL                                /**< Mode UNIT24 for LDMA_CH_CTRL                */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT32          0x00000009UL                                /**< Mode UNIT32 for LDMA_CH_CTRL                */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT64          0x0000000AUL                                /**< Mode UNIT64 for LDMA_CH_CTRL                */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT128         0x0000000BUL                                /**< Mode UNIT128 for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT256         0x0000000CUL                                /**< Mode UNIT256 for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT512         0x0000000DUL                                /**< Mode UNIT512 for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_BLOCKSIZE_UNIT1024        0x0000000EUL                                /**< Mode UNIT1024 for LDMA_CH_CTRL              */
#define _LDMA_CH_CTRL_BLOCKSIZE_ALL             0x0000000FUL                                /**< Mode ALL for LDMA_CH_CTRL                   */
#define LDMA_CH_CTRL_BLOCKSIZE_DEFAULT          (_LDMA_CH_CTRL_BLOCKSIZE_DEFAULT << 16)     /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT1            (_LDMA_CH_CTRL_BLOCKSIZE_UNIT1 << 16)       /**< Shifted mode UNIT1 for LDMA_CH_CTRL         */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT2            (_LDMA_CH_CTRL_BLOCKSIZE_UNIT2 << 16)       /**< Shifted mode UNIT2 for LDMA_CH_CTRL         */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT3            (_LDMA_CH_CTRL_BLOCKSIZE_UNIT3 << 16)       /**< Shifted mode UNIT3 for LDMA_CH_CTRL         */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT4            (_LDMA_CH_CTRL_BLOCKSIZE_UNIT4 << 16)       /**< Shifted mode UNIT4 for LDMA_CH_CTRL         */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT6            (_LDMA_CH_CTRL_BLOCKSIZE_UNIT6 << 16)       /**< Shifted mode UNIT6 for LDMA_CH_CTRL         */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT8            (_LDMA_CH_CTRL_BLOCKSIZE_UNIT8 << 16)       /**< Shifted mode UNIT8 for LDMA_CH_CTRL         */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT12           (_LDMA_CH_CTRL_BLOCKSIZE_UNIT12 << 16)      /**< Shifted mode UNIT12 for LDMA_CH_CTRL        */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT16           (_LDMA_CH_CTRL_BLOCKSIZE_UNIT16 << 16)      /**< Shifted mode UNIT16 for LDMA_CH_CTRL        */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT24           (_LDMA_CH_CTRL_BLOCKSIZE_UNIT24 << 16)      /**< Shifted mode UNIT24 for LDMA_CH_CTRL        */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT32           (_LDMA_CH_CTRL_BLOCKSIZE_UNIT32 << 16)      /**< Shifted mode UNIT32 for LDMA_CH_CTRL        */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT64           (_LDMA_CH_CTRL_BLOCKSIZE_UNIT64 << 16)      /**< Shifted mode UNIT64 for LDMA_CH_CTRL        */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT128          (_LDMA_CH_CTRL_BLOCKSIZE_UNIT128 << 16)     /**< Shifted mode UNIT128 for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT256          (_LDMA_CH_CTRL_BLOCKSIZE_UNIT256 << 16)     /**< Shifted mode UNIT256 for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT512          (_LDMA_CH_CTRL_BLOCKSIZE_UNIT512 << 16)     /**< Shifted mode UNIT512 for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_BLOCKSIZE_UNIT1024         (_LDMA_CH_CTRL_BLOCKSIZE_UNIT1024 << 16)    /**< Shifted mode UNIT1024 for LDMA_CH_CTRL      */
#define LDMA_CH_CTRL_BLOCKSIZE_ALL              (_LDMA_CH_CTRL_BLOCKSIZE_ALL << 16)         /**< Shifted mode ALL for LDMA_CH_CTRL           */
#define LDMA_CH_CTRL_DONESET                    (0x1UL << 20)                               /**< DMA Operation Done Interrupt Flag Set       */
#define _LDMA_CH_CTRL_DONESET_SHIFT             20                                          /**< Shift value for LDMA_DONESET                */
#define _LDMA_CH_CTRL_DONESET_MASK              0x100000UL                                  /**< Bit mask for LDMA_DONESET                   */
#define _LDMA_CH_CTRL_DONESET_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define LDMA_CH_CTRL_DONESET_DEFAULT            (_LDMA_CH_CTRL_DONESET_DEFAULT << 20)       /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_REQMODE                    (0x1UL << 21)                               /**< DMA Request Transfer Mode Select            */
#define _LDMA_CH_CTRL_REQMODE_SHIFT             21                                          /**< Shift value for LDMA_REQMODE                */
#define _LDMA_CH_CTRL_REQMODE_MASK              0x200000UL                                  /**< Bit mask for LDMA_REQMODE                   */
#define _LDMA_CH_CTRL_REQMODE_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_REQMODE_BLOCK             0x00000000UL                                /**< Mode BLOCK for LDMA_CH_CTRL                 */
#define _LDMA_CH_CTRL_REQMODE_ALL               0x00000001UL                                /**< Mode ALL for LDMA_CH_CTRL                   */
#define LDMA_CH_CTRL_REQMODE_DEFAULT            (_LDMA_CH_CTRL_REQMODE_DEFAULT << 21)       /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_REQMODE_BLOCK              (_LDMA_CH_CTRL_REQMODE_BLOCK << 21)         /**< Shifted mode BLOCK for LDMA_CH_CTRL         */
#define LDMA_CH_CTRL_REQMODE_ALL                (_LDMA_CH_CTRL_REQMODE_ALL << 21)           /**< Shifted mode ALL for LDMA_CH_CTRL           */
#define LDMA_CH_CTRL_DECLOOPCNT                 (0x1UL << 22)                               /**< Decrement Loop Count                        */
#define _LDMA_CH_CTRL_DECLOOPCNT_SHIFT          22                                          /**< Shift value for LDMA_DECLOOPCNT             */
#define _LDMA_CH_CTRL_DECLOOPCNT_MASK           0x400000UL                                  /**< Bit mask for LDMA_DECLOOPCNT                */
#define _LDMA_CH_CTRL_DECLOOPCNT_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define LDMA_CH_CTRL_DECLOOPCNT_DEFAULT         (_LDMA_CH_CTRL_DECLOOPCNT_DEFAULT << 22)    /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_IGNORESREQ                 (0x1UL << 23)                               /**< Ignore Sreq                                 */
#define _LDMA_CH_CTRL_IGNORESREQ_SHIFT          23                                          /**< Shift value for LDMA_IGNORESREQ             */
#define _LDMA_CH_CTRL_IGNORESREQ_MASK           0x800000UL                                  /**< Bit mask for LDMA_IGNORESREQ                */
#define _LDMA_CH_CTRL_IGNORESREQ_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define LDMA_CH_CTRL_IGNORESREQ_DEFAULT         (_LDMA_CH_CTRL_IGNORESREQ_DEFAULT << 23)    /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define _LDMA_CH_CTRL_SRCINC_SHIFT              24                                          /**< Shift value for LDMA_SRCINC                 */
#define _LDMA_CH_CTRL_SRCINC_MASK               0x3000000UL                                 /**< Bit mask for LDMA_SRCINC                    */
#define _LDMA_CH_CTRL_SRCINC_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_SRCINC_ONE                0x00000000UL                                /**< Mode ONE for LDMA_CH_CTRL                   */
#define _LDMA_CH_CTRL_SRCINC_TWO                0x00000001UL                                /**< Mode TWO for LDMA_CH_CTRL                   */
#define _LDMA_CH_CTRL_SRCINC_FOUR               0x00000002UL                                /**< Mode FOUR for LDMA_CH_CTRL                  */
#define _LDMA_CH_CTRL_SRCINC_NONE               0x00000003UL                                /**< Mode NONE for LDMA_CH_CTRL                  */
#define LDMA_CH_CTRL_SRCINC_DEFAULT             (_LDMA_CH_CTRL_SRCINC_DEFAULT << 24)        /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_SRCINC_ONE                 (_LDMA_CH_CTRL_SRCINC_ONE << 24)            /**< Shifted mode ONE for LDMA_CH_CTRL           */
#define LDMA_CH_CTRL_SRCINC_TWO                 (_LDMA_CH_CTRL_SRCINC_TWO << 24)            /**< Shifted mode TWO for LDMA_CH_CTRL           */
#define LDMA_CH_CTRL_SRCINC_FOUR                (_LDMA_CH_CTRL_SRCINC_FOUR << 24)           /**< Shifted mode FOUR for LDMA_CH_CTRL          */
#define LDMA_CH_CTRL_SRCINC_NONE                (_LDMA_CH_CTRL_SRCINC_NONE << 24)           /**< Shifted mode NONE for LDMA_CH_CTRL          */
#define _LDMA_CH_CTRL_SIZE_SHIFT                26                                          /**< Shift value for LDMA_SIZE                   */
#define _LDMA_CH_CTRL_SIZE_MASK                 0xC000000UL                                 /**< Bit mask for LDMA_SIZE                      */
#define _LDMA_CH_CTRL_SIZE_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_SIZE_BYTE                 0x00000000UL                                /**< Mode BYTE for LDMA_CH_CTRL                  */
#define _LDMA_CH_CTRL_SIZE_HALFWORD             0x00000001UL                                /**< Mode HALFWORD for LDMA_CH_CTRL              */
#define _LDMA_CH_CTRL_SIZE_WORD                 0x00000002UL                                /**< Mode WORD for LDMA_CH_CTRL                  */
#define LDMA_CH_CTRL_SIZE_DEFAULT               (_LDMA_CH_CTRL_SIZE_DEFAULT << 26)          /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_SIZE_BYTE                  (_LDMA_CH_CTRL_SIZE_BYTE << 26)             /**< Shifted mode BYTE for LDMA_CH_CTRL          */
#define LDMA_CH_CTRL_SIZE_HALFWORD              (_LDMA_CH_CTRL_SIZE_HALFWORD << 26)         /**< Shifted mode HALFWORD for LDMA_CH_CTRL      */
#define LDMA_CH_CTRL_SIZE_WORD                  (_LDMA_CH_CTRL_SIZE_WORD << 26)             /**< Shifted mode WORD for LDMA_CH_CTRL          */
#define _LDMA_CH_CTRL_DSTINC_SHIFT              28                                          /**< Shift value for LDMA_DSTINC                 */
#define _LDMA_CH_CTRL_DSTINC_MASK               0x30000000UL                                /**< Bit mask for LDMA_DSTINC                    */
#define _LDMA_CH_CTRL_DSTINC_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_DSTINC_ONE                0x00000000UL                                /**< Mode ONE for LDMA_CH_CTRL                   */
#define _LDMA_CH_CTRL_DSTINC_TWO                0x00000001UL                                /**< Mode TWO for LDMA_CH_CTRL                   */
#define _LDMA_CH_CTRL_DSTINC_FOUR               0x00000002UL                                /**< Mode FOUR for LDMA_CH_CTRL                  */
#define _LDMA_CH_CTRL_DSTINC_NONE               0x00000003UL                                /**< Mode NONE for LDMA_CH_CTRL                  */
#define LDMA_CH_CTRL_DSTINC_DEFAULT             (_LDMA_CH_CTRL_DSTINC_DEFAULT << 28)        /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_DSTINC_ONE                 (_LDMA_CH_CTRL_DSTINC_ONE << 28)            /**< Shifted mode ONE for LDMA_CH_CTRL           */
#define LDMA_CH_CTRL_DSTINC_TWO                 (_LDMA_CH_CTRL_DSTINC_TWO << 28)            /**< Shifted mode TWO for LDMA_CH_CTRL           */
#define LDMA_CH_CTRL_DSTINC_FOUR                (_LDMA_CH_CTRL_DSTINC_FOUR << 28)           /**< Shifted mode FOUR for LDMA_CH_CTRL          */
#define LDMA_CH_CTRL_DSTINC_NONE                (_LDMA_CH_CTRL_DSTINC_NONE << 28)           /**< Shifted mode NONE for LDMA_CH_CTRL          */
#define LDMA_CH_CTRL_SRCMODE                    (0x1UL << 30)                               /**< Source Addressing Mode                      */
#define _LDMA_CH_CTRL_SRCMODE_SHIFT             30                                          /**< Shift value for LDMA_SRCMODE                */
#define _LDMA_CH_CTRL_SRCMODE_MASK              0x40000000UL                                /**< Bit mask for LDMA_SRCMODE                   */
#define _LDMA_CH_CTRL_SRCMODE_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_SRCMODE_ABSOLUTE          0x00000000UL                                /**< Mode ABSOLUTE for LDMA_CH_CTRL              */
#define _LDMA_CH_CTRL_SRCMODE_RELATIVE          0x00000001UL                                /**< Mode RELATIVE for LDMA_CH_CTRL              */
#define LDMA_CH_CTRL_SRCMODE_DEFAULT            (_LDMA_CH_CTRL_SRCMODE_DEFAULT << 30)       /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_SRCMODE_ABSOLUTE           (_LDMA_CH_CTRL_SRCMODE_ABSOLUTE << 30)      /**< Shifted mode ABSOLUTE for LDMA_CH_CTRL      */
#define LDMA_CH_CTRL_SRCMODE_RELATIVE           (_LDMA_CH_CTRL_SRCMODE_RELATIVE << 30)      /**< Shifted mode RELATIVE for LDMA_CH_CTRL      */
#define LDMA_CH_CTRL_DSTMODE                    (0x1UL << 31)                               /**< Destination Addressing Mode                 */
#define _LDMA_CH_CTRL_DSTMODE_SHIFT             31                                          /**< Shift value for LDMA_DSTMODE                */
#define _LDMA_CH_CTRL_DSTMODE_MASK              0x80000000UL                                /**< Bit mask for LDMA_DSTMODE                   */
#define _LDMA_CH_CTRL_DSTMODE_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for LDMA_CH_CTRL               */
#define _LDMA_CH_CTRL_DSTMODE_ABSOLUTE          0x00000000UL                                /**< Mode ABSOLUTE for LDMA_CH_CTRL              */
#define _LDMA_CH_CTRL_DSTMODE_RELATIVE          0x00000001UL                                /**< Mode RELATIVE for LDMA_CH_CTRL              */
#define LDMA_CH_CTRL_DSTMODE_DEFAULT            (_LDMA_CH_CTRL_DSTMODE_DEFAULT << 31)       /**< Shifted mode DEFAULT for LDMA_CH_CTRL       */
#define LDMA_CH_CTRL_DSTMODE_ABSOLUTE           (_LDMA_CH_CTRL_DSTMODE_ABSOLUTE << 31)      /**< Shifted mode ABSOLUTE for LDMA_CH_CTRL      */
#define LDMA_CH_CTRL_DSTMODE_RELATIVE           (_LDMA_CH_CTRL_DSTMODE_RELATIVE << 31)      /**< Shifted mode RELATIVE for LDMA_CH_CTRL      */

/* Bit fields for LDMA CH_SRC */
#define _LDMA_CH_SRC_RESETVALUE                 0x00000000UL                            /**< Default value for LDMA_CH_SRC               */
#define _LDMA_CH_SRC_MASK                       0xFFFFFFFFUL                            /**< Mask for LDMA_CH_SRC                        */
#define _LDMA_CH_SRC_ADDR_SHIFT                 0                                       /**< Shift value for LDMA_ADDR                   */
#define _LDMA_CH_SRC_ADDR_MASK                  0xFFFFFFFFUL                            /**< Bit mask for LDMA_ADDR                      */
#define _LDMA_CH_SRC_ADDR_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for LDMA_CH_SRC                */
#define LDMA_CH_SRC_ADDR_DEFAULT                (_LDMA_CH_SRC_ADDR_DEFAULT << 0)        /**< Shifted mode DEFAULT for LDMA_CH_SRC        */

/* Bit fields for LDMA CH_DST */
#define _LDMA_CH_DST_RESETVALUE                 0x00000000UL                            /**< Default value for LDMA_CH_DST               */
#define _LDMA_CH_DST_MASK                       0xFFFFFFFFUL                            /**< Mask for LDMA_CH_DST                        */
#define _LDMA_CH_DST_ADDR_SHIFT                 0                                       /**< Shift value for LDMA_ADDR                   */
#define _LDMA_CH_DST_ADDR_MASK                  0xFFFFFFFFUL                            /**< Bit mask for LDMA_ADDR                      */
#define _LDMA_CH_DST_ADDR_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for LDMA_CH_DST                */
#define LDMA_CH_DST_ADDR_DEFAULT                (_LDMA_CH_DST_ADDR_DEFAULT << 0)        /**< Shifted mode DEFAULT for LDMA_CH_DST        */

/* Bit fields for LDMA CH_LINK */
#define _LDMA_CH_LINK_RESETVALUE                0x00000000UL                            /**< Default value for LDMA_CH_LINK              */
#define _LDMA_CH_LINK_MASK                      0xFFFFFFFFUL                            /**< Mask for LDMA_CH_LINK                       */
#define LDMA_CH_LINK_LINKMODE                   (0x1UL << 0)                            /**< Link Structure Addressing Mode              */
#define _LDMA_CH_LINK_LINKMODE_SHIFT            0                                       /**< Shift value for LDMA_LINKMODE               */
#define _LDMA_CH_LINK_LINKMODE_MASK             0x1UL                                   /**< Bit mask for LDMA_LINKMODE                  */
#define _LDMA_CH_LINK_LINKMODE_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for LDMA_CH_LINK               */
#define _LDMA_CH_LINK_LINKMODE_ABSOLUTE         0x00000000UL                            /**< Mode ABSOLUTE for LDMA_CH_LINK              */
#define _LDMA_CH_LINK_LINKMODE_RELATIVE         0x00000001UL                            /**< Mode RELATIVE for LDMA_CH_LINK              */
#define LDMA_CH_LINK_LINKMODE_DEFAULT           (_LDMA_CH_LINK_LINKMODE_DEFAULT << 0)   /**< Shifted mode DEFAULT for LDMA_CH_LINK       */
#define LDMA_CH_LINK_LINKMODE_ABSOLUTE          (_LDMA_CH_LINK_LINKMODE_ABSOLUTE << 0)  /**< Shifted mode ABSOLUTE for LDMA_CH_LINK      */
#define LDMA_CH_LINK_LINKMODE_RELATIVE          (_LDMA_CH_LINK_LINKMODE_RELATIVE << 0)  /**< Shifted mode RELATIVE for LDMA_CH_LINK      */
#define LDMA_CH_LINK_LINK                       (0x1UL << 1)                            /**< Link Next Structure                         */
#define _LDMA_CH_LINK_LINK_SHIFT                1                                       /**< Shift value for LDMA_LINK                   */
#define _LDMA_CH_LINK_LINK_MASK                 0x2UL                                   /**< Bit mask for LDMA_LINK                      */
#define _LDMA_CH_LINK_LINK_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for LDMA_CH_LINK               */
#define LDMA_CH_LINK_LINK_DEFAULT               (_LDMA_CH_LINK_LINK_DEFAULT << 1)       /**< Shifted mode DEFAULT for LDMA_CH_LINK       */
#define _LDMA_CH_LINK_LINKADDR_SHIFT            2                                       /**< Shift value for LDMA_LINKADDR               */
#define _LDMA_CH_LINK_LINKADDR_MASK             0xFFFFFFFCUL                            /**< Bit mask for LDMA_LINKADDR                  */
#define _LDMA_CH_LINK_LINKADDR_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for LDMA_CH_LINK               */
#define LDMA_CH_LINK_LINKADDR_DEFAULT           (_LDMA_CH_LINK_LINKADDR_DEFAULT << 2)   /**< Shifted mode DEFAULT for LDMA_CH_LINK       */

/* Bit fields for LDMA CH_XCTRL */
#define _LDMA_CH_XCTRL_RESETVALUE               0x00000000UL                             /**< Default value for LDMA_CH_XCTRL             */
#define _LDMA_CH_XCTRL_MASK                     0x000000F0UL                             /**< Mask for LDMA_CH_XCTRL                      */
#define LDMA_CH_XCTRL_DSTILEN                   (0x1UL << 4)                             /**< Destination Interleave                      */
#define _LDMA_CH_XCTRL_DSTILEN_SHIFT            4                                        /**< Shift value for LDMA_DSTILEN                */
#define _LDMA_CH_XCTRL_DSTILEN_MASK             0x10UL                                   /**< Bit mask for LDMA_DSTILEN                   */
#define _LDMA_CH_XCTRL_DSTILEN_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for LDMA_CH_XCTRL              */
#define LDMA_CH_XCTRL_DSTILEN_DEFAULT           (_LDMA_CH_XCTRL_DSTILEN_DEFAULT << 4)    /**< Shifted mode DEFAULT for LDMA_CH_XCTRL      */
#define _LDMA_CH_XCTRL_ILMODE_SHIFT             5                                        /**< Shift value for LDMA_ILMODE                 */
#define _LDMA_CH_XCTRL_ILMODE_MASK              0x60UL                                   /**< Bit mask for LDMA_ILMODE                    */
#define _LDMA_CH_XCTRL_ILMODE_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for LDMA_CH_XCTRL              */
#define _LDMA_CH_XCTRL_ILMODE_ABSOLUTE          0x00000000UL                             /**< Mode ABSOLUTE for LDMA_CH_XCTRL             */
#define _LDMA_CH_XCTRL_ILMODE_RELATIVE16        0x00000001UL                             /**< Mode RELATIVE16 for LDMA_CH_XCTRL           */
#define _LDMA_CH_XCTRL_ILMODE_RELATIVE8         0x00000002UL                             /**< Mode RELATIVE8 for LDMA_CH_XCTRL            */
#define LDMA_CH_XCTRL_ILMODE_DEFAULT            (_LDMA_CH_XCTRL_ILMODE_DEFAULT << 5)     /**< Shifted mode DEFAULT for LDMA_CH_XCTRL      */
#define LDMA_CH_XCTRL_ILMODE_ABSOLUTE           (_LDMA_CH_XCTRL_ILMODE_ABSOLUTE << 5)    /**< Shifted mode ABSOLUTE for LDMA_CH_XCTRL     */
#define LDMA_CH_XCTRL_ILMODE_RELATIVE16         (_LDMA_CH_XCTRL_ILMODE_RELATIVE16 << 5)  /**< Shifted mode RELATIVE16 for LDMA_CH_XCTRL   */
#define LDMA_CH_XCTRL_ILMODE_RELATIVE8          (_LDMA_CH_XCTRL_ILMODE_RELATIVE8 << 5)   /**< Shifted mode RELATIVE8 for LDMA_CH_XCTRL    */
#define LDMA_CH_XCTRL_BUFFERABLE                (0x1UL << 7)                             /**< Allow AHB buffering                         */
#define _LDMA_CH_XCTRL_BUFFERABLE_SHIFT         7                                        /**< Shift value for LDMA_BUFFERABLE             */
#define _LDMA_CH_XCTRL_BUFFERABLE_MASK          0x80UL                                   /**< Bit mask for LDMA_BUFFERABLE                */
#define _LDMA_CH_XCTRL_BUFFERABLE_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for LDMA_CH_XCTRL              */
#define LDMA_CH_XCTRL_BUFFERABLE_DEFAULT        (_LDMA_CH_XCTRL_BUFFERABLE_DEFAULT << 7) /**< Shifted mode DEFAULT for LDMA_CH_XCTRL      */

/* Bit fields for LDMA CH_ILSRC */
#define _LDMA_CH_ILSRC_RESETVALUE               0x00000000UL                            /**< Default value for LDMA_CH_ILSRC             */
#define _LDMA_CH_ILSRC_MASK                     0xFFFFFFFFUL                            /**< Mask for LDMA_CH_ILSRC                      */
#define _LDMA_CH_ILSRC_ADDR_SHIFT               0                                       /**< Shift value for LDMA_ADDR                   */
#define _LDMA_CH_ILSRC_ADDR_MASK                0xFFFFFFFFUL                            /**< Bit mask for LDMA_ADDR                      */
#define _LDMA_CH_ILSRC_ADDR_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for LDMA_CH_ILSRC              */
#define LDMA_CH_ILSRC_ADDR_DEFAULT              (_LDMA_CH_ILSRC_ADDR_DEFAULT << 0)      /**< Shifted mode DEFAULT for LDMA_CH_ILSRC      */

/** @} End of group EFR32FG25_LDMA_BitFields */
/** @} End of group EFR32FG25_LDMA */
/** @} End of group Parts */

#endif /* EFR32FG25_LDMA_H */
