/**************************************************************************//**
 * @file
 * @brief EFR32FG25 USBAHB register and bit field definitions
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
#ifndef EFR32FG25_USBAHB_H
#define EFR32FG25_USBAHB_H

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32FG25_USBAHB_AHBS USBAHB_AHBS
 * @{
 * @brief EFR32FG25 USBAHB_AHBS Register Declaration.
 *****************************************************************************/

/** USBAHB_AHBS DEVINEP Register Group Declaration. */
typedef struct {
  __IOM uint32_t CTL;                                /**< Device Control IN EP 1 CTRL reg                    */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t INT;                                /**< Device IN EP 1 IRQ Register                        */
  uint32_t       RESERVED1[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t TSIZ;                               /**< Device IN EP 1 xfer Size reg                       */
  __IOM uint32_t DMAADDR;                            /**< Device IN EP 1 DMA Address reg                     */
  __IM uint32_t  DTXFSTS;                            /**< Device IN EP TX FIFO stat reg                      */
  uint32_t       RESERVED2[1U];                      /**< Reserved for future use                            */
} USBAHB_DEVINEP_TypeDef;

/** USBAHB_AHBS DEVOUTEP Register Group Declaration. */
typedef struct {
  __IOM uint32_t CTL;                                /**< Device CTRL OUT EP 1 CTRL reg                      */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t INT;                                /**< Device OUT EP 1 IRQ Register                       */
  uint32_t       RESERVED1[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t TSIZ;                               /**< Device OUT EP 1 xfer Size reg                      */
  __IOM uint32_t DMAADDR;                            /**< Device OUT EP 1 DMA addr reg                       */
  uint32_t       RESERVED2[2U];                      /**< Reserved for future use                            */
} USBAHB_DEVOUTEP_TypeDef;

/** USBAHB_AHBS FIFO_0_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO0D;                             /**< Device EP0 FIFO                                    */
} USBAHB_FIFO_0_Data_TypeDef;

/** USBAHB_AHBS FIFO_1_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO1D;                             /**< FIFO1                                              */
} USBAHB_FIFO_1_Data_TypeDef;

/** USBAHB_AHBS FIFO_2_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO2D;                             /**< EP2 data                                           */
} USBAHB_FIFO_2_Data_TypeDef;

/** USBAHB_AHBS FIFO_3_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO3D;                             /**< New Register                                       */
} USBAHB_FIFO_3_Data_TypeDef;

/** USBAHB_AHBS FIFO_4_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO4D;                             /**< New Register                                       */
} USBAHB_FIFO_4_Data_TypeDef;

/** USBAHB_AHBS FIFO_5_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO5D;                             /**< New Register                                       */
} USBAHB_FIFO_5_Data_TypeDef;

/** USBAHB_AHBS FIFO_6_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO6D;                             /**< New Register                                       */
} USBAHB_FIFO_6_Data_TypeDef;

/** USBAHB_AHBS FIFO_7_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO7D;                             /**< New Register                                       */
} USBAHB_FIFO_7_Data_TypeDef;

/** USBAHB_AHBS FIFO_8_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO8D;                             /**< New Register                                       */
} USBAHB_FIFO_8_Data_TypeDef;

/** USBAHB_AHBS FIFO_9_Data Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFO9D;                             /**< New Register                                       */
} USBAHB_FIFO_9_Data_TypeDef;

/** USBAHB_AHBS FIFORAM Register Group Declaration. */
typedef struct {
  __IOM uint32_t FIFORAM;                            /**< New Register                                       */
} USBAHB_FIFORAM_TypeDef;

/** USBAHB_AHBS Register Declaration. */
typedef struct {
  __IOM uint32_t             GOTGCTL;            /**< OTG_Ctrl_and_status_reg                            */
  uint32_t                   RESERVED0[1U];      /**< Reserved for future use                            */
  __IOM uint32_t             GAHBCFG;            /**< AHB Config Register                                */
  __IOM uint32_t             GUSBCFG;            /**< USB Configuration Reg                              */
  __IOM uint32_t             GRSTCTL;            /**< Reset Register                                     */
  __IOM uint32_t             GINTSTS;            /**< Interrupt Register                                 */
  __IOM uint32_t             GINTMSK;            /**< Interrupt Mask Register                            */
  __IM uint32_t              GRXSTSR;            /**< Receive Status Debug Read Reg                      */
  __IM uint32_t              GRXSTSP;            /**< Receive Status Read /Pop Reg                       */
  __IOM uint32_t             GRXFSIZ;            /**< Receive FIFO Size Register                         */
  __IOM uint32_t             GNPTXFSIZ;          /**< Non-per Transmit FIFO Size Reg                     */
  uint32_t                   RESERVED1[5U];      /**< Reserved for future use                            */
  __IM uint32_t              GSNPSID;            /**< Synopsys ID Register                               */
  __IM uint32_t              GHWCFG1;            /**< User HW Config1 Register                           */
  __IM uint32_t              GHWCFG2;            /**< User HW Config2 Register                           */
  __IM uint32_t              GHWCFG3;            /**< User HW Config3 Register                           */
  __IM uint32_t              GHWCFG4;            /**< User HW Config4 Register                           */
  uint32_t                   RESERVED2[2U];      /**< Reserved for future use                            */
  __IOM uint32_t             GDFIFOCFG;          /**< Global DFIFO Configuration Reg                     */
  uint32_t                   RESERVED3[41U];     /**< Reserved for future use                            */
  __IOM uint32_t             DIEPTXF1;           /**< Device IN EP xmit FIFO Siz Reg 1                   */
  __IOM uint32_t             DIEPTXF2;           /**< Device IN EP xmit FIFO Size Register 2             */
  __IOM uint32_t             DIEPTXF3;           /**< Device IN EP xmit FIFO Size Register 2             */
  __IOM uint32_t             DIEPTXF4;           /**< Device IN EP xmit FIFO Size Register 2             */
  __IOM uint32_t             DIEPTXF5;           /**< Device IN EP xmit FIFO Size Register 2             */
  __IOM uint32_t             DIEPTXF6;           /**< Device IN EP xmit FIFO Size Register 2             */
  __IOM uint32_t             DIEPTXF7;           /**< Device IN EP xmit FIFO Size Register 2             */
  __IOM uint32_t             DIEPTXF8;           /**< Device IN EP xmit FIFO Size Register 2             */
  __IOM uint32_t             DIEPTXF9;           /**< Device IN EP xmit FIFO Size Register 2             */
  uint32_t                   RESERVED4[438U];    /**< Reserved for future use                            */
  __IOM uint32_t             DCFG;               /**< Device Configuration Register                      */
  __IOM uint32_t             DCTL;               /**< Device Control Register                            */
  __IM uint32_t              DSTS;               /**< Device Status Register                             */
  uint32_t                   RESERVED5[1U];      /**< Reserved for future use                            */
  __IOM uint32_t             DIEPMSK;            /**< Device IN EP Common IRQ Mask Register              */
  __IOM uint32_t             DOEPMSK;            /**< New Register                                       */
  __IM uint32_t              DAINT;              /**< Device All EP Interrupt Register                   */
  __IOM uint32_t             DAINTMSK;           /**< Device All EP Interrupt Mask Register              */
  uint32_t                   RESERVED6[4U];      /**< Reserved for future use                            */
  __IOM uint32_t             DTHRCTL;            /**< Device Threshold Control Register                  */
  __IOM uint32_t             DIEPEMPMSK;         /**< Device IN EP FIFO Empty IRQ Mask reg               */
  uint32_t                   RESERVED7[50U];     /**< Reserved for future use                            */
  __IOM uint32_t             DIEP0CTL;           /**< Device Control IN EP 0 ctrl reg                    */
  uint32_t                   RESERVED8[1U];      /**< Reserved for future use                            */
  __IOM uint32_t             DIEP0INT;           /**< Device IN EP 0 IRQ reg                             */
  uint32_t                   RESERVED9[1U];      /**< Reserved for future use                            */
  __IOM uint32_t             DIEP0TSIZ;          /**< Device In EP 0 xfer size reg                       */
  __IOM uint32_t             DIEP0DMAADDR;       /**< Device IN EP 0 DMA addr reg                        */
  __IM uint32_t              DIEP0TXFSTS;        /**< DEvice In EP TX FIFO Status reg 0                  */
  uint32_t                   RESERVED10[1U];     /**< Reserved for future use                            */
  USBAHB_DEVINEP_TypeDef     DEVINEP[9U];        /**<                                                    */
  uint32_t                   RESERVED11[48U];    /**< Reserved for future use                            */
  __IOM uint32_t             DOEP0CTL;           /**< Device ctrl OUT EP 0 CTRL reg                      */
  uint32_t                   RESERVED12[1U];     /**< Reserved for future use                            */
  __IOM uint32_t             DOEP0INT;           /**< Device OUT EP 0 IRQ Register                       */
  uint32_t                   RESERVED13[1U];     /**< Reserved for future use                            */
  __IOM uint32_t             DOEP0TSIZ;          /**< Device OUT EP 0 xfer Size reg                      */
  __IOM uint32_t             DOEP0DMAADDR;       /**< Device OUT EP 0 DMA addr reg                       */
  uint32_t                   RESERVED14[2U];     /**< Reserved for future use                            */
  USBAHB_DEVOUTEP_TypeDef    DEVOUTEP[9U];       /**<                                                    */
  uint32_t                   RESERVED15[112U];   /**< Reserved for future use                            */
  __IOM uint32_t             PCGCCTL;            /**< PWR_CG_CTRL_reg                                    */
  uint32_t                   RESERVED16[127U];   /**< Reserved for future use                            */
  USBAHB_FIFO_0_Data_TypeDef FIFO_0_Data[1024U]; /**<                                                    */
  USBAHB_FIFO_1_Data_TypeDef FIFO_1_Data[1024U]; /**<                                                    */
  USBAHB_FIFO_2_Data_TypeDef FIFO_2_Data[1024U]; /**<                                                    */
  USBAHB_FIFO_3_Data_TypeDef FIFO_3_Data[1024U]; /**<                                                    */
  USBAHB_FIFO_4_Data_TypeDef FIFO_4_Data[1024U]; /**<                                                    */
  USBAHB_FIFO_5_Data_TypeDef FIFO_5_Data[1024U]; /**<                                                    */
  USBAHB_FIFO_6_Data_TypeDef FIFO_6_Data[1024U]; /**<                                                    */
  USBAHB_FIFO_7_Data_TypeDef FIFO_7_Data[1024U]; /**<                                                    */
  USBAHB_FIFO_8_Data_TypeDef FIFO_8_Data[1024U]; /**<                                                    */
  USBAHB_FIFO_9_Data_TypeDef FIFO_9_Data[1024U]; /**<                                                    */
  uint32_t                   RESERVED17[21504U]; /**< Reserved for future use                            */
  USBAHB_FIFORAM_TypeDef     FIFORAM[768U];      /**<                                                    */
} USBAHB_AHBS_TypeDef;
/** @} End of group EFR32FG25_USBAHB_AHBS */

/**************************************************************************//**
 * @addtogroup EFR32FG25_USBAHB_AHBS
 * @{
 * @defgroup EFR32FG25_USBAHB_AHBS_BitFields USBAHB_AHBS Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for USBAHB GOTGCTL */
#define _USBAHB_GOTGCTL_RESETVALUE                         0x00010000UL                           /**< Default value for USBAHB_GOTGCTL            */
#define _USBAHB_GOTGCTL_MASK                               0x003900C0UL                           /**< Mask for USBAHB_GOTGCTL                     */
#define USBAHB_GOTGCTL_CURMOD                              (0x1UL << 21)                          /**< Current Mode of Operation                   */
#define _USBAHB_GOTGCTL_CURMOD_SHIFT                       21                                     /**< Shift value for USBAHB_CURMOD               */
#define _USBAHB_GOTGCTL_CURMOD_MASK                        0x200000UL                             /**< Bit mask for USBAHB_CURMOD                  */
#define _USBAHB_GOTGCTL_CURMOD_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for USBAHB_GOTGCTL             */
#define USBAHB_GOTGCTL_CURMOD_DEFAULT                      (_USBAHB_GOTGCTL_CURMOD_DEFAULT << 21) /**< Shifted mode DEFAULT for USBAHB_GOTGCTL     */

/* Bit fields for USBAHB GAHBCFG */
#define _USBAHB_GAHBCFG_RESETVALUE                         0x00000000UL                                   /**< Default value for USBAHB_GAHBCFG            */
#define _USBAHB_GAHBCFG_MASK                               0x01E000BFUL                                   /**< Mask for USBAHB_GAHBCFG                     */
#define USBAHB_GAHBCFG_GLBLINTRMSK                         (0x1UL << 0)                                   /**< Global Interrupt Mask                       */
#define _USBAHB_GAHBCFG_GLBLINTRMSK_SHIFT                  0                                              /**< Shift value for USBAHB_GLBLINTRMSK          */
#define _USBAHB_GAHBCFG_GLBLINTRMSK_MASK                   0x1UL                                          /**< Bit mask for USBAHB_GLBLINTRMSK             */
#define _USBAHB_GAHBCFG_GLBLINTRMSK_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GAHBCFG             */
#define USBAHB_GAHBCFG_GLBLINTRMSK_DEFAULT                 (_USBAHB_GAHBCFG_GLBLINTRMSK_DEFAULT << 0)     /**< Shifted mode DEFAULT for USBAHB_GAHBCFG     */
#define _USBAHB_GAHBCFG_HBSTLEN_SHIFT                      1                                              /**< Shift value for USBAHB_HBSTLEN              */
#define _USBAHB_GAHBCFG_HBSTLEN_MASK                       0x1EUL                                         /**< Bit mask for USBAHB_HBSTLEN                 */
#define _USBAHB_GAHBCFG_HBSTLEN_DEFAULT                    0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GAHBCFG             */
#define USBAHB_GAHBCFG_HBSTLEN_DEFAULT                     (_USBAHB_GAHBCFG_HBSTLEN_DEFAULT << 1)         /**< Shifted mode DEFAULT for USBAHB_GAHBCFG     */
#define USBAHB_GAHBCFG_DMAEN                               (0x1UL << 5)                                   /**< DMA Enable                                  */
#define _USBAHB_GAHBCFG_DMAEN_SHIFT                        5                                              /**< Shift value for USBAHB_DMAEN                */
#define _USBAHB_GAHBCFG_DMAEN_MASK                         0x20UL                                         /**< Bit mask for USBAHB_DMAEN                   */
#define _USBAHB_GAHBCFG_DMAEN_DEFAULT                      0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GAHBCFG             */
#define USBAHB_GAHBCFG_DMAEN_DEFAULT                       (_USBAHB_GAHBCFG_DMAEN_DEFAULT << 5)           /**< Shifted mode DEFAULT for USBAHB_GAHBCFG     */
#define USBAHB_GAHBCFG_NPTXFEMPLVL                         (0x1UL << 7)                                   /**< Non-Periodic TxFIFO Empty Level             */
#define _USBAHB_GAHBCFG_NPTXFEMPLVL_SHIFT                  7                                              /**< Shift value for USBAHB_NPTXFEMPLVL          */
#define _USBAHB_GAHBCFG_NPTXFEMPLVL_MASK                   0x80UL                                         /**< Bit mask for USBAHB_NPTXFEMPLVL             */
#define _USBAHB_GAHBCFG_NPTXFEMPLVL_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GAHBCFG             */
#define USBAHB_GAHBCFG_NPTXFEMPLVL_DEFAULT                 (_USBAHB_GAHBCFG_NPTXFEMPLVL_DEFAULT << 7)     /**< Shifted mode DEFAULT for USBAHB_GAHBCFG     */
#define USBAHB_GAHBCFG_REMMEMSUPP                          (0x1UL << 21)                                  /**< Remote Memory Support                       */
#define _USBAHB_GAHBCFG_REMMEMSUPP_SHIFT                   21                                             /**< Shift value for USBAHB_REMMEMSUPP           */
#define _USBAHB_GAHBCFG_REMMEMSUPP_MASK                    0x200000UL                                     /**< Bit mask for USBAHB_REMMEMSUPP              */
#define _USBAHB_GAHBCFG_REMMEMSUPP_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GAHBCFG             */
#define USBAHB_GAHBCFG_REMMEMSUPP_DEFAULT                  (_USBAHB_GAHBCFG_REMMEMSUPP_DEFAULT << 21)     /**< Shifted mode DEFAULT for USBAHB_GAHBCFG     */
#define USBAHB_GAHBCFG_NOTIALLDMAWRIT                      (0x1UL << 22)                                  /**< Notify All Dma Write Trans                  */
#define _USBAHB_GAHBCFG_NOTIALLDMAWRIT_SHIFT               22                                             /**< Shift value for USBAHB_NOTIALLDMAWRIT       */
#define _USBAHB_GAHBCFG_NOTIALLDMAWRIT_MASK                0x400000UL                                     /**< Bit mask for USBAHB_NOTIALLDMAWRIT          */
#define _USBAHB_GAHBCFG_NOTIALLDMAWRIT_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GAHBCFG             */
#define USBAHB_GAHBCFG_NOTIALLDMAWRIT_DEFAULT              (_USBAHB_GAHBCFG_NOTIALLDMAWRIT_DEFAULT << 22) /**< Shifted mode DEFAULT for USBAHB_GAHBCFG     */
#define USBAHB_GAHBCFG_AHBSINGLE                           (0x1UL << 23)                                  /**< AHB Single Support                          */
#define _USBAHB_GAHBCFG_AHBSINGLE_SHIFT                    23                                             /**< Shift value for USBAHB_AHBSINGLE            */
#define _USBAHB_GAHBCFG_AHBSINGLE_MASK                     0x800000UL                                     /**< Bit mask for USBAHB_AHBSINGLE               */
#define _USBAHB_GAHBCFG_AHBSINGLE_DEFAULT                  0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GAHBCFG             */
#define USBAHB_GAHBCFG_AHBSINGLE_DEFAULT                   (_USBAHB_GAHBCFG_AHBSINGLE_DEFAULT << 23)      /**< Shifted mode DEFAULT for USBAHB_GAHBCFG     */
#define USBAHB_GAHBCFG_INVDESCEND                          (0x1UL << 24)                                  /**< Inverse Descriptor Endianness               */
#define _USBAHB_GAHBCFG_INVDESCEND_SHIFT                   24                                             /**< Shift value for USBAHB_INVDESCEND           */
#define _USBAHB_GAHBCFG_INVDESCEND_MASK                    0x1000000UL                                    /**< Bit mask for USBAHB_INVDESCEND              */
#define _USBAHB_GAHBCFG_INVDESCEND_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GAHBCFG             */
#define USBAHB_GAHBCFG_INVDESCEND_DEFAULT                  (_USBAHB_GAHBCFG_INVDESCEND_DEFAULT << 24)     /**< Shifted mode DEFAULT for USBAHB_GAHBCFG     */

/* Bit fields for USBAHB GUSBCFG */
#define _USBAHB_GUSBCFG_RESETVALUE                         0x00001440UL                                   /**< Default value for USBAHB_GUSBCFG            */
#define _USBAHB_GUSBCFG_MASK                               0x94403C6FUL                                   /**< Mask for USBAHB_GUSBCFG                     */
#define _USBAHB_GUSBCFG_TOUTCAL_SHIFT                      0                                              /**< Shift value for USBAHB_TOUTCAL              */
#define _USBAHB_GUSBCFG_TOUTCAL_MASK                       0x7UL                                          /**< Bit mask for USBAHB_TOUTCAL                 */
#define _USBAHB_GUSBCFG_TOUTCAL_DEFAULT                    0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GUSBCFG             */
#define USBAHB_GUSBCFG_TOUTCAL_DEFAULT                     (_USBAHB_GUSBCFG_TOUTCAL_DEFAULT << 0)         /**< Shifted mode DEFAULT for USBAHB_GUSBCFG     */
#define USBAHB_GUSBCFG_PHYIF                               (0x1UL << 3)                                   /**< PHY Interface                               */
#define _USBAHB_GUSBCFG_PHYIF_SHIFT                        3                                              /**< Shift value for USBAHB_PHYIF                */
#define _USBAHB_GUSBCFG_PHYIF_MASK                         0x8UL                                          /**< Bit mask for USBAHB_PHYIF                   */
#define _USBAHB_GUSBCFG_PHYIF_DEFAULT                      0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GUSBCFG             */
#define USBAHB_GUSBCFG_PHYIF_DEFAULT                       (_USBAHB_GUSBCFG_PHYIF_DEFAULT << 3)           /**< Shifted mode DEFAULT for USBAHB_GUSBCFG     */
#define USBAHB_GUSBCFG_FSINTF                              (0x1UL << 5)                                   /**< Full-Speed Serial Interface Sel             */
#define _USBAHB_GUSBCFG_FSINTF_SHIFT                       5                                              /**< Shift value for USBAHB_FSINTF               */
#define _USBAHB_GUSBCFG_FSINTF_MASK                        0x20UL                                         /**< Bit mask for USBAHB_FSINTF                  */
#define _USBAHB_GUSBCFG_FSINTF_DEFAULT                     0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GUSBCFG             */
#define USBAHB_GUSBCFG_FSINTF_DEFAULT                      (_USBAHB_GUSBCFG_FSINTF_DEFAULT << 5)          /**< Shifted mode DEFAULT for USBAHB_GUSBCFG     */
#define USBAHB_GUSBCFG_PHYSEL                              (0x1UL << 6)                                   /**< "2.0 HS sel or 1.1 FS sel                   */
#define _USBAHB_GUSBCFG_PHYSEL_SHIFT                       6                                              /**< Shift value for USBAHB_PHYSEL               */
#define _USBAHB_GUSBCFG_PHYSEL_MASK                        0x40UL                                         /**< Bit mask for USBAHB_PHYSEL                  */
#define _USBAHB_GUSBCFG_PHYSEL_DEFAULT                     0x00000001UL                                   /**< Mode DEFAULT for USBAHB_GUSBCFG             */
#define USBAHB_GUSBCFG_PHYSEL_DEFAULT                      (_USBAHB_GUSBCFG_PHYSEL_DEFAULT << 6)          /**< Shifted mode DEFAULT for USBAHB_GUSBCFG     */
#define _USBAHB_GUSBCFG_USBTRDTIM_SHIFT                    10                                             /**< Shift value for USBAHB_USBTRDTIM            */
#define _USBAHB_GUSBCFG_USBTRDTIM_MASK                     0x3C00UL                                       /**< Bit mask for USBAHB_USBTRDTIM               */
#define _USBAHB_GUSBCFG_USBTRDTIM_DEFAULT                  0x00000005UL                                   /**< Mode DEFAULT for USBAHB_GUSBCFG             */
#define USBAHB_GUSBCFG_USBTRDTIM_DEFAULT                   (_USBAHB_GUSBCFG_USBTRDTIM_DEFAULT << 10)      /**< Shifted mode DEFAULT for USBAHB_GUSBCFG     */
#define USBAHB_GUSBCFG_TERMSELDLPULSE                      (0x1UL << 22)                                  /**< TermSel DLine Pulsing Selection             */
#define _USBAHB_GUSBCFG_TERMSELDLPULSE_SHIFT               22                                             /**< Shift value for USBAHB_TERMSELDLPULSE       */
#define _USBAHB_GUSBCFG_TERMSELDLPULSE_MASK                0x400000UL                                     /**< Bit mask for USBAHB_TERMSELDLPULSE          */
#define _USBAHB_GUSBCFG_TERMSELDLPULSE_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GUSBCFG             */
#define USBAHB_GUSBCFG_TERMSELDLPULSE_DEFAULT              (_USBAHB_GUSBCFG_TERMSELDLPULSE_DEFAULT << 22) /**< Shifted mode DEFAULT for USBAHB_GUSBCFG     */
#define USBAHB_GUSBCFG_ICUSBCAP                            (0x1UL << 26)                                  /**< IC_USB-Capable                              */
#define _USBAHB_GUSBCFG_ICUSBCAP_SHIFT                     26                                             /**< Shift value for USBAHB_ICUSBCAP             */
#define _USBAHB_GUSBCFG_ICUSBCAP_MASK                      0x4000000UL                                    /**< Bit mask for USBAHB_ICUSBCAP                */
#define _USBAHB_GUSBCFG_ICUSBCAP_DEFAULT                   0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GUSBCFG             */
#define USBAHB_GUSBCFG_ICUSBCAP_DEFAULT                    (_USBAHB_GUSBCFG_ICUSBCAP_DEFAULT << 26)       /**< Shifted mode DEFAULT for USBAHB_GUSBCFG     */
#define USBAHB_GUSBCFG_TXENDDELAY                          (0x1UL << 28)                                  /**< Tx End Delay                                */
#define _USBAHB_GUSBCFG_TXENDDELAY_SHIFT                   28                                             /**< Shift value for USBAHB_TXENDDELAY           */
#define _USBAHB_GUSBCFG_TXENDDELAY_MASK                    0x10000000UL                                   /**< Bit mask for USBAHB_TXENDDELAY              */
#define _USBAHB_GUSBCFG_TXENDDELAY_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GUSBCFG             */
#define USBAHB_GUSBCFG_TXENDDELAY_DEFAULT                  (_USBAHB_GUSBCFG_TXENDDELAY_DEFAULT << 28)     /**< Shifted mode DEFAULT for USBAHB_GUSBCFG     */
#define USBAHB_GUSBCFG_CORRUPTTXPKT                        (0x1UL << 31)                                  /**< corrupt tx packet                           */
#define _USBAHB_GUSBCFG_CORRUPTTXPKT_SHIFT                 31                                             /**< Shift value for USBAHB_CORRUPTTXPKT         */
#define _USBAHB_GUSBCFG_CORRUPTTXPKT_MASK                  0x80000000UL                                   /**< Bit mask for USBAHB_CORRUPTTXPKT            */
#define _USBAHB_GUSBCFG_CORRUPTTXPKT_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for USBAHB_GUSBCFG             */
#define USBAHB_GUSBCFG_CORRUPTTXPKT_DEFAULT                (_USBAHB_GUSBCFG_CORRUPTTXPKT_DEFAULT << 31)   /**< Shifted mode DEFAULT for USBAHB_GUSBCFG     */

/* Bit fields for USBAHB GRSTCTL */
#define _USBAHB_GRSTCTL_RESETVALUE                         0x00000000UL                               /**< Default value for USBAHB_GRSTCTL            */
#define _USBAHB_GRSTCTL_MASK                               0xC00007F3UL                               /**< Mask for USBAHB_GRSTCTL                     */
#define USBAHB_GRSTCTL_CSFTRST                             (0x1UL << 0)                               /**< Core Soft Reset                             */
#define _USBAHB_GRSTCTL_CSFTRST_SHIFT                      0                                          /**< Shift value for USBAHB_CSFTRST              */
#define _USBAHB_GRSTCTL_CSFTRST_MASK                       0x1UL                                      /**< Bit mask for USBAHB_CSFTRST                 */
#define _USBAHB_GRSTCTL_CSFTRST_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for USBAHB_GRSTCTL             */
#define USBAHB_GRSTCTL_CSFTRST_DEFAULT                     (_USBAHB_GRSTCTL_CSFTRST_DEFAULT << 0)     /**< Shifted mode DEFAULT for USBAHB_GRSTCTL     */
#define USBAHB_GRSTCTL_PIUFSSFTRST                         (0x1UL << 1)                               /**< PIU FS Dedicated Controller Sft rst         */
#define _USBAHB_GRSTCTL_PIUFSSFTRST_SHIFT                  1                                          /**< Shift value for USBAHB_PIUFSSFTRST          */
#define _USBAHB_GRSTCTL_PIUFSSFTRST_MASK                   0x2UL                                      /**< Bit mask for USBAHB_PIUFSSFTRST             */
#define _USBAHB_GRSTCTL_PIUFSSFTRST_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for USBAHB_GRSTCTL             */
#define USBAHB_GRSTCTL_PIUFSSFTRST_DEFAULT                 (_USBAHB_GRSTCTL_PIUFSSFTRST_DEFAULT << 1) /**< Shifted mode DEFAULT for USBAHB_GRSTCTL     */
#define USBAHB_GRSTCTL_RXFFLSH                             (0x1UL << 4)                               /**< RxFIFO Flush                                */
#define _USBAHB_GRSTCTL_RXFFLSH_SHIFT                      4                                          /**< Shift value for USBAHB_RXFFLSH              */
#define _USBAHB_GRSTCTL_RXFFLSH_MASK                       0x10UL                                     /**< Bit mask for USBAHB_RXFFLSH                 */
#define _USBAHB_GRSTCTL_RXFFLSH_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for USBAHB_GRSTCTL             */
#define USBAHB_GRSTCTL_RXFFLSH_DEFAULT                     (_USBAHB_GRSTCTL_RXFFLSH_DEFAULT << 4)     /**< Shifted mode DEFAULT for USBAHB_GRSTCTL     */
#define USBAHB_GRSTCTL_TXFFLSH                             (0x1UL << 5)                               /**< TxFIFO Flush                                */
#define _USBAHB_GRSTCTL_TXFFLSH_SHIFT                      5                                          /**< Shift value for USBAHB_TXFFLSH              */
#define _USBAHB_GRSTCTL_TXFFLSH_MASK                       0x20UL                                     /**< Bit mask for USBAHB_TXFFLSH                 */
#define _USBAHB_GRSTCTL_TXFFLSH_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for USBAHB_GRSTCTL             */
#define USBAHB_GRSTCTL_TXFFLSH_DEFAULT                     (_USBAHB_GRSTCTL_TXFFLSH_DEFAULT << 5)     /**< Shifted mode DEFAULT for USBAHB_GRSTCTL     */
#define _USBAHB_GRSTCTL_TXFNUM_SHIFT                       6                                          /**< Shift value for USBAHB_TXFNUM               */
#define _USBAHB_GRSTCTL_TXFNUM_MASK                        0x7C0UL                                    /**< Bit mask for USBAHB_TXFNUM                  */
#define _USBAHB_GRSTCTL_TXFNUM_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for USBAHB_GRSTCTL             */
#define USBAHB_GRSTCTL_TXFNUM_DEFAULT                      (_USBAHB_GRSTCTL_TXFNUM_DEFAULT << 6)      /**< Shifted mode DEFAULT for USBAHB_GRSTCTL     */
#define USBAHB_GRSTCTL_DMAREQ                              (0x1UL << 30)                              /**< DMA Request Signal                          */
#define _USBAHB_GRSTCTL_DMAREQ_SHIFT                       30                                         /**< Shift value for USBAHB_DMAREQ               */
#define _USBAHB_GRSTCTL_DMAREQ_MASK                        0x40000000UL                               /**< Bit mask for USBAHB_DMAREQ                  */
#define _USBAHB_GRSTCTL_DMAREQ_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for USBAHB_GRSTCTL             */
#define USBAHB_GRSTCTL_DMAREQ_DEFAULT                      (_USBAHB_GRSTCTL_DMAREQ_DEFAULT << 30)     /**< Shifted mode DEFAULT for USBAHB_GRSTCTL     */
#define USBAHB_GRSTCTL_AHBIDLE                             (0x1UL << 31)                              /**< AHB Master Idle                             */
#define _USBAHB_GRSTCTL_AHBIDLE_SHIFT                      31                                         /**< Shift value for USBAHB_AHBIDLE              */
#define _USBAHB_GRSTCTL_AHBIDLE_MASK                       0x80000000UL                               /**< Bit mask for USBAHB_AHBIDLE                 */
#define _USBAHB_GRSTCTL_AHBIDLE_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for USBAHB_GRSTCTL             */
#define USBAHB_GRSTCTL_AHBIDLE_DEFAULT                     (_USBAHB_GRSTCTL_AHBIDLE_DEFAULT << 31)    /**< Shifted mode DEFAULT for USBAHB_GRSTCTL     */

/* Bit fields for USBAHB GINTSTS */
#define _USBAHB_GINTSTS_RESETVALUE                         0x10000020UL                                 /**< Default value for USBAHB_GINTSTS            */
#define _USBAHB_GINTSTS_MASK                               0x90FEFCFBUL                                 /**< Mask for USBAHB_GINTSTS                     */
#define USBAHB_GINTSTS_CURMOD                              (0x1UL << 0)                                 /**< Current Mode of Operation                   */
#define _USBAHB_GINTSTS_CURMOD_SHIFT                       0                                            /**< Shift value for USBAHB_CURMOD               */
#define _USBAHB_GINTSTS_CURMOD_MASK                        0x1UL                                        /**< Bit mask for USBAHB_CURMOD                  */
#define _USBAHB_GINTSTS_CURMOD_DEFAULT                     0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_CURMOD_DEFAULT                      (_USBAHB_GINTSTS_CURMOD_DEFAULT << 0)        /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_MODEMIS                             (0x1UL << 1)                                 /**< Mode Mismatch Interrupt                     */
#define _USBAHB_GINTSTS_MODEMIS_SHIFT                      1                                            /**< Shift value for USBAHB_MODEMIS              */
#define _USBAHB_GINTSTS_MODEMIS_MASK                       0x2UL                                        /**< Bit mask for USBAHB_MODEMIS                 */
#define _USBAHB_GINTSTS_MODEMIS_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_MODEMIS_DEFAULT                     (_USBAHB_GINTSTS_MODEMIS_DEFAULT << 1)       /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_SOF                                 (0x1UL << 3)                                 /**< tart of (micro)Frame                        */
#define _USBAHB_GINTSTS_SOF_SHIFT                          3                                            /**< Shift value for USBAHB_SOF                  */
#define _USBAHB_GINTSTS_SOF_MASK                           0x8UL                                        /**< Bit mask for USBAHB_SOF                     */
#define _USBAHB_GINTSTS_SOF_DEFAULT                        0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_SOF_DEFAULT                         (_USBAHB_GINTSTS_SOF_DEFAULT << 3)           /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_RXFLVL                              (0x1UL << 4)                                 /**< RxFIFO Non-Empty                            */
#define _USBAHB_GINTSTS_RXFLVL_SHIFT                       4                                            /**< Shift value for USBAHB_RXFLVL               */
#define _USBAHB_GINTSTS_RXFLVL_MASK                        0x10UL                                       /**< Bit mask for USBAHB_RXFLVL                  */
#define _USBAHB_GINTSTS_RXFLVL_DEFAULT                     0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_RXFLVL_DEFAULT                      (_USBAHB_GINTSTS_RXFLVL_DEFAULT << 4)        /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_NPTxFEmp                            (0x1UL << 5)                                 /**< Non periodic TXFIFO Empty                   */
#define _USBAHB_GINTSTS_NPTxFEmp_SHIFT                     5                                            /**< Shift value for USBAHB_NPTxFEmp             */
#define _USBAHB_GINTSTS_NPTxFEmp_MASK                      0x20UL                                       /**< Bit mask for USBAHB_NPTxFEmp                */
#define _USBAHB_GINTSTS_NPTxFEmp_DEFAULT                   0x00000001UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_NPTxFEmp_DEFAULT                    (_USBAHB_GINTSTS_NPTxFEmp_DEFAULT << 5)      /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_GINNAKEFF                           (0x1UL << 6)                                 /**< Global IN Non-periodic NAK Effective        */
#define _USBAHB_GINTSTS_GINNAKEFF_SHIFT                    6                                            /**< Shift value for USBAHB_GINNAKEFF            */
#define _USBAHB_GINTSTS_GINNAKEFF_MASK                     0x40UL                                       /**< Bit mask for USBAHB_GINNAKEFF               */
#define _USBAHB_GINTSTS_GINNAKEFF_DEFAULT                  0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_GINNAKEFF_DEFAULT                   (_USBAHB_GINTSTS_GINNAKEFF_DEFAULT << 6)     /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_GOUTNAKEFF                          (0x1UL << 7)                                 /**< Global OUT NAK Effective                    */
#define _USBAHB_GINTSTS_GOUTNAKEFF_SHIFT                   7                                            /**< Shift value for USBAHB_GOUTNAKEFF           */
#define _USBAHB_GINTSTS_GOUTNAKEFF_MASK                    0x80UL                                       /**< Bit mask for USBAHB_GOUTNAKEFF              */
#define _USBAHB_GINTSTS_GOUTNAKEFF_DEFAULT                 0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_GOUTNAKEFF_DEFAULT                  (_USBAHB_GINTSTS_GOUTNAKEFF_DEFAULT << 7)    /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_ERLYSUSP                            (0x1UL << 10)                                /**< Early Suspend                               */
#define _USBAHB_GINTSTS_ERLYSUSP_SHIFT                     10                                           /**< Shift value for USBAHB_ERLYSUSP             */
#define _USBAHB_GINTSTS_ERLYSUSP_MASK                      0x400UL                                      /**< Bit mask for USBAHB_ERLYSUSP                */
#define _USBAHB_GINTSTS_ERLYSUSP_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_ERLYSUSP_DEFAULT                    (_USBAHB_GINTSTS_ERLYSUSP_DEFAULT << 10)     /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_USBSUSP                             (0x1UL << 11)                                /**< USB Suspend                                 */
#define _USBAHB_GINTSTS_USBSUSP_SHIFT                      11                                           /**< Shift value for USBAHB_USBSUSP              */
#define _USBAHB_GINTSTS_USBSUSP_MASK                       0x800UL                                      /**< Bit mask for USBAHB_USBSUSP                 */
#define _USBAHB_GINTSTS_USBSUSP_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_USBSUSP_DEFAULT                     (_USBAHB_GINTSTS_USBSUSP_DEFAULT << 11)      /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_USBRST                              (0x1UL << 12)                                /**< USB Reset                                   */
#define _USBAHB_GINTSTS_USBRST_SHIFT                       12                                           /**< Shift value for USBAHB_USBRST               */
#define _USBAHB_GINTSTS_USBRST_MASK                        0x1000UL                                     /**< Bit mask for USBAHB_USBRST                  */
#define _USBAHB_GINTSTS_USBRST_DEFAULT                     0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_USBRST_DEFAULT                      (_USBAHB_GINTSTS_USBRST_DEFAULT << 12)       /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_ENUMDONE                            (0x1UL << 13)                                /**< Enumeration Done                            */
#define _USBAHB_GINTSTS_ENUMDONE_SHIFT                     13                                           /**< Shift value for USBAHB_ENUMDONE             */
#define _USBAHB_GINTSTS_ENUMDONE_MASK                      0x2000UL                                     /**< Bit mask for USBAHB_ENUMDONE                */
#define _USBAHB_GINTSTS_ENUMDONE_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_ENUMDONE_DEFAULT                    (_USBAHB_GINTSTS_ENUMDONE_DEFAULT << 13)     /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_ISOOUTDROP                          (0x1UL << 14)                                /**< Iso OUT Packet Dropped IRQ                  */
#define _USBAHB_GINTSTS_ISOOUTDROP_SHIFT                   14                                           /**< Shift value for USBAHB_ISOOUTDROP           */
#define _USBAHB_GINTSTS_ISOOUTDROP_MASK                    0x4000UL                                     /**< Bit mask for USBAHB_ISOOUTDROP              */
#define _USBAHB_GINTSTS_ISOOUTDROP_DEFAULT                 0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_ISOOUTDROP_DEFAULT                  (_USBAHB_GINTSTS_ISOOUTDROP_DEFAULT << 14)   /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_EOPF                                (0x1UL << 15)                                /**< End of Periodic Frame Interrupt             */
#define _USBAHB_GINTSTS_EOPF_SHIFT                         15                                           /**< Shift value for USBAHB_EOPF                 */
#define _USBAHB_GINTSTS_EOPF_MASK                          0x8000UL                                     /**< Bit mask for USBAHB_EOPF                    */
#define _USBAHB_GINTSTS_EOPF_DEFAULT                       0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_EOPF_DEFAULT                        (_USBAHB_GINTSTS_EOPF_DEFAULT << 15)         /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_EPMIS                               (0x1UL << 17)                                /**< Endpoint Mismatch Interrupt                 */
#define _USBAHB_GINTSTS_EPMIS_SHIFT                        17                                           /**< Shift value for USBAHB_EPMIS                */
#define _USBAHB_GINTSTS_EPMIS_MASK                         0x20000UL                                    /**< Bit mask for USBAHB_EPMIS                   */
#define _USBAHB_GINTSTS_EPMIS_DEFAULT                      0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_EPMIS_DEFAULT                       (_USBAHB_GINTSTS_EPMIS_DEFAULT << 17)        /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_IEPINT                              (0x1UL << 18)                                /**< IN Endpoints Interrupt                      */
#define _USBAHB_GINTSTS_IEPINT_SHIFT                       18                                           /**< Shift value for USBAHB_IEPINT               */
#define _USBAHB_GINTSTS_IEPINT_MASK                        0x40000UL                                    /**< Bit mask for USBAHB_IEPINT                  */
#define _USBAHB_GINTSTS_IEPINT_DEFAULT                     0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_IEPINT_DEFAULT                      (_USBAHB_GINTSTS_IEPINT_DEFAULT << 18)       /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_OEPINT                              (0x1UL << 19)                                /**< OUT Endpoints Interrupt                     */
#define _USBAHB_GINTSTS_OEPINT_SHIFT                       19                                           /**< Shift value for USBAHB_OEPINT               */
#define _USBAHB_GINTSTS_OEPINT_MASK                        0x80000UL                                    /**< Bit mask for USBAHB_OEPINT                  */
#define _USBAHB_GINTSTS_OEPINT_DEFAULT                     0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_OEPINT_DEFAULT                      (_USBAHB_GINTSTS_OEPINT_DEFAULT << 19)       /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_INCOMPISOIN                         (0x1UL << 20)                                /**< Incomplete Isochronous IN Transfer          */
#define _USBAHB_GINTSTS_INCOMPISOIN_SHIFT                  20                                           /**< Shift value for USBAHB_INCOMPISOIN          */
#define _USBAHB_GINTSTS_INCOMPISOIN_MASK                   0x100000UL                                   /**< Bit mask for USBAHB_INCOMPISOIN             */
#define _USBAHB_GINTSTS_INCOMPISOIN_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_INCOMPISOIN_DEFAULT                 (_USBAHB_GINTSTS_INCOMPISOIN_DEFAULT << 20)  /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_INCOMPLP                            (0x1UL << 21)                                /**< Incomplete Periodic Transfer                */
#define _USBAHB_GINTSTS_INCOMPLP_SHIFT                     21                                           /**< Shift value for USBAHB_INCOMPLP             */
#define _USBAHB_GINTSTS_INCOMPLP_MASK                      0x200000UL                                   /**< Bit mask for USBAHB_INCOMPLP                */
#define _USBAHB_GINTSTS_INCOMPLP_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_INCOMPLP_DEFAULT                    (_USBAHB_GINTSTS_INCOMPLP_DEFAULT << 21)     /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_FETSUSP                             (0x1UL << 22)                                /**< Data Fetch Suspended                        */
#define _USBAHB_GINTSTS_FETSUSP_SHIFT                      22                                           /**< Shift value for USBAHB_FETSUSP              */
#define _USBAHB_GINTSTS_FETSUSP_MASK                       0x400000UL                                   /**< Bit mask for USBAHB_FETSUSP                 */
#define _USBAHB_GINTSTS_FETSUSP_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_FETSUSP_DEFAULT                     (_USBAHB_GINTSTS_FETSUSP_DEFAULT << 22)      /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_RESETDET                            (0x1UL << 23)                                /**< Reset detected Interrupt                    */
#define _USBAHB_GINTSTS_RESETDET_SHIFT                     23                                           /**< Shift value for USBAHB_RESETDET             */
#define _USBAHB_GINTSTS_RESETDET_MASK                      0x800000UL                                   /**< Bit mask for USBAHB_RESETDET                */
#define _USBAHB_GINTSTS_RESETDET_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_RESETDET_DEFAULT                    (_USBAHB_GINTSTS_RESETDET_DEFAULT << 23)     /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_CONIDSTSCHNG                        (0x1UL << 28)                                /**< Connector ID Status Change                  */
#define _USBAHB_GINTSTS_CONIDSTSCHNG_SHIFT                 28                                           /**< Shift value for USBAHB_CONIDSTSCHNG         */
#define _USBAHB_GINTSTS_CONIDSTSCHNG_MASK                  0x10000000UL                                 /**< Bit mask for USBAHB_CONIDSTSCHNG            */
#define _USBAHB_GINTSTS_CONIDSTSCHNG_DEFAULT               0x00000001UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_CONIDSTSCHNG_DEFAULT                (_USBAHB_GINTSTS_CONIDSTSCHNG_DEFAULT << 28) /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */
#define USBAHB_GINTSTS_WKUPINT                             (0x1UL << 31)                                /**< Resume/Remote Wakeup Detected Irq           */
#define _USBAHB_GINTSTS_WKUPINT_SHIFT                      31                                           /**< Shift value for USBAHB_WKUPINT              */
#define _USBAHB_GINTSTS_WKUPINT_MASK                       0x80000000UL                                 /**< Bit mask for USBAHB_WKUPINT                 */
#define _USBAHB_GINTSTS_WKUPINT_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GINTSTS             */
#define USBAHB_GINTSTS_WKUPINT_DEFAULT                     (_USBAHB_GINTSTS_WKUPINT_DEFAULT << 31)      /**< Shifted mode DEFAULT for USBAHB_GINTSTS     */

/* Bit fields for USBAHB GINTMSK */
#define _USBAHB_GINTMSK_RESETVALUE                         0x00000000UL                                     /**< Default value for USBAHB_GINTMSK            */
#define _USBAHB_GINTMSK_MASK                               0x90FEFCFAUL                                     /**< Mask for USBAHB_GINTMSK                     */
#define USBAHB_GINTMSK_MODEMISMSK                          (0x1UL << 1)                                     /**< Mode Mismatch Interrupt Mask                */
#define _USBAHB_GINTMSK_MODEMISMSK_SHIFT                   1                                                /**< Shift value for USBAHB_MODEMISMSK           */
#define _USBAHB_GINTMSK_MODEMISMSK_MASK                    0x2UL                                            /**< Bit mask for USBAHB_MODEMISMSK              */
#define _USBAHB_GINTMSK_MODEMISMSK_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_MODEMISMSK_DEFAULT                  (_USBAHB_GINTMSK_MODEMISMSK_DEFAULT << 1)        /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_SOFMSK                              (0x1UL << 3)                                     /**< Start of (micro)Frame Mask                  */
#define _USBAHB_GINTMSK_SOFMSK_SHIFT                       3                                                /**< Shift value for USBAHB_SOFMSK               */
#define _USBAHB_GINTMSK_SOFMSK_MASK                        0x8UL                                            /**< Bit mask for USBAHB_SOFMSK                  */
#define _USBAHB_GINTMSK_SOFMSK_DEFAULT                     0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_SOFMSK_DEFAULT                      (_USBAHB_GINTMSK_SOFMSK_DEFAULT << 3)            /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_RXFLVLMSK                           (0x1UL << 4)                                     /**< Receive FIFO Non-Empty Mask                 */
#define _USBAHB_GINTMSK_RXFLVLMSK_SHIFT                    4                                                /**< Shift value for USBAHB_RXFLVLMSK            */
#define _USBAHB_GINTMSK_RXFLVLMSK_MASK                     0x10UL                                           /**< Bit mask for USBAHB_RXFLVLMSK               */
#define _USBAHB_GINTMSK_RXFLVLMSK_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_RXFLVLMSK_DEFAULT                   (_USBAHB_GINTMSK_RXFLVLMSK_DEFAULT << 4)         /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_NPTXFEMPMSK                         (0x1UL << 5)                                     /**< NP TXFIFO Empty Msk                         */
#define _USBAHB_GINTMSK_NPTXFEMPMSK_SHIFT                  5                                                /**< Shift value for USBAHB_NPTXFEMPMSK          */
#define _USBAHB_GINTMSK_NPTXFEMPMSK_MASK                   0x20UL                                           /**< Bit mask for USBAHB_NPTXFEMPMSK             */
#define _USBAHB_GINTMSK_NPTXFEMPMSK_DEFAULT                0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_NPTXFEMPMSK_DEFAULT                 (_USBAHB_GINTMSK_NPTXFEMPMSK_DEFAULT << 5)       /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_GINNAKEFFMSK                        (0x1UL << 6)                                     /**< Global Non-per IN NAK Eff Mask              */
#define _USBAHB_GINTMSK_GINNAKEFFMSK_SHIFT                 6                                                /**< Shift value for USBAHB_GINNAKEFFMSK         */
#define _USBAHB_GINTMSK_GINNAKEFFMSK_MASK                  0x40UL                                           /**< Bit mask for USBAHB_GINNAKEFFMSK            */
#define _USBAHB_GINTMSK_GINNAKEFFMSK_DEFAULT               0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_GINNAKEFFMSK_DEFAULT                (_USBAHB_GINTMSK_GINNAKEFFMSK_DEFAULT << 6)      /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_GOUTNAKEFFMSK                       (0x1UL << 7)                                     /**< Global OUT NAK Effective Mask               */
#define _USBAHB_GINTMSK_GOUTNAKEFFMSK_SHIFT                7                                                /**< Shift value for USBAHB_GOUTNAKEFFMSK        */
#define _USBAHB_GINTMSK_GOUTNAKEFFMSK_MASK                 0x80UL                                           /**< Bit mask for USBAHB_GOUTNAKEFFMSK           */
#define _USBAHB_GINTMSK_GOUTNAKEFFMSK_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_GOUTNAKEFFMSK_DEFAULT               (_USBAHB_GINTMSK_GOUTNAKEFFMSK_DEFAULT << 7)     /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_ERLYSUSPMSK                         (0x1UL << 10)                                    /**< Early Suspend Mask                          */
#define _USBAHB_GINTMSK_ERLYSUSPMSK_SHIFT                  10                                               /**< Shift value for USBAHB_ERLYSUSPMSK          */
#define _USBAHB_GINTMSK_ERLYSUSPMSK_MASK                   0x400UL                                          /**< Bit mask for USBAHB_ERLYSUSPMSK             */
#define _USBAHB_GINTMSK_ERLYSUSPMSK_DEFAULT                0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_ERLYSUSPMSK_DEFAULT                 (_USBAHB_GINTMSK_ERLYSUSPMSK_DEFAULT << 10)      /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_USBSUSPMSK                          (0x1UL << 11)                                    /**< USB Suspend Mask                            */
#define _USBAHB_GINTMSK_USBSUSPMSK_SHIFT                   11                                               /**< Shift value for USBAHB_USBSUSPMSK           */
#define _USBAHB_GINTMSK_USBSUSPMSK_MASK                    0x800UL                                          /**< Bit mask for USBAHB_USBSUSPMSK              */
#define _USBAHB_GINTMSK_USBSUSPMSK_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_USBSUSPMSK_DEFAULT                  (_USBAHB_GINTMSK_USBSUSPMSK_DEFAULT << 11)       /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_USBRSTMSK                           (0x1UL << 12)                                    /**< USB Reset Mask                              */
#define _USBAHB_GINTMSK_USBRSTMSK_SHIFT                    12                                               /**< Shift value for USBAHB_USBRSTMSK            */
#define _USBAHB_GINTMSK_USBRSTMSK_MASK                     0x1000UL                                         /**< Bit mask for USBAHB_USBRSTMSK               */
#define _USBAHB_GINTMSK_USBRSTMSK_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_USBRSTMSK_DEFAULT                   (_USBAHB_GINTMSK_USBRSTMSK_DEFAULT << 12)        /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_ENUMDONEMSK                         (0x1UL << 13)                                    /**< Enumeration Done Mask                       */
#define _USBAHB_GINTMSK_ENUMDONEMSK_SHIFT                  13                                               /**< Shift value for USBAHB_ENUMDONEMSK          */
#define _USBAHB_GINTMSK_ENUMDONEMSK_MASK                   0x2000UL                                         /**< Bit mask for USBAHB_ENUMDONEMSK             */
#define _USBAHB_GINTMSK_ENUMDONEMSK_DEFAULT                0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_ENUMDONEMSK_DEFAULT                 (_USBAHB_GINTMSK_ENUMDONEMSK_DEFAULT << 13)      /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_ISOOUTDROPMSK                       (0x1UL << 14)                                    /**< Iso OUT Pkt Dropped IRQ Mask                */
#define _USBAHB_GINTMSK_ISOOUTDROPMSK_SHIFT                14                                               /**< Shift value for USBAHB_ISOOUTDROPMSK        */
#define _USBAHB_GINTMSK_ISOOUTDROPMSK_MASK                 0x4000UL                                         /**< Bit mask for USBAHB_ISOOUTDROPMSK           */
#define _USBAHB_GINTMSK_ISOOUTDROPMSK_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_ISOOUTDROPMSK_DEFAULT               (_USBAHB_GINTMSK_ISOOUTDROPMSK_DEFAULT << 14)    /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_EOPFMSK                             (0x1UL << 15)                                    /**< End of Periodic Frame Irq Mask              */
#define _USBAHB_GINTMSK_EOPFMSK_SHIFT                      15                                               /**< Shift value for USBAHB_EOPFMSK              */
#define _USBAHB_GINTMSK_EOPFMSK_MASK                       0x8000UL                                         /**< Bit mask for USBAHB_EOPFMSK                 */
#define _USBAHB_GINTMSK_EOPFMSK_DEFAULT                    0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_EOPFMSK_DEFAULT                     (_USBAHB_GINTMSK_EOPFMSK_DEFAULT << 15)          /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_EPMISMSK                            (0x1UL << 17)                                    /**< Endpoint Mismatch Interrupt Mask            */
#define _USBAHB_GINTMSK_EPMISMSK_SHIFT                     17                                               /**< Shift value for USBAHB_EPMISMSK             */
#define _USBAHB_GINTMSK_EPMISMSK_MASK                      0x20000UL                                        /**< Bit mask for USBAHB_EPMISMSK                */
#define _USBAHB_GINTMSK_EPMISMSK_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_EPMISMSK_DEFAULT                    (_USBAHB_GINTMSK_EPMISMSK_DEFAULT << 17)         /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_IEPINTMSK                           (0x1UL << 18)                                    /**< IN Endpoints Interrupt Mask                 */
#define _USBAHB_GINTMSK_IEPINTMSK_SHIFT                    18                                               /**< Shift value for USBAHB_IEPINTMSK            */
#define _USBAHB_GINTMSK_IEPINTMSK_MASK                     0x40000UL                                        /**< Bit mask for USBAHB_IEPINTMSK               */
#define _USBAHB_GINTMSK_IEPINTMSK_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_IEPINTMSK_DEFAULT                   (_USBAHB_GINTMSK_IEPINTMSK_DEFAULT << 18)        /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_OEPINTMSK                           (0x1UL << 19)                                    /**< OUT Endpoints Interrupt Mask                */
#define _USBAHB_GINTMSK_OEPINTMSK_SHIFT                    19                                               /**< Shift value for USBAHB_OEPINTMSK            */
#define _USBAHB_GINTMSK_OEPINTMSK_MASK                     0x80000UL                                        /**< Bit mask for USBAHB_OEPINTMSK               */
#define _USBAHB_GINTMSK_OEPINTMSK_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_OEPINTMSK_DEFAULT                   (_USBAHB_GINTMSK_OEPINTMSK_DEFAULT << 19)        /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_INCOMPISOINMSK                      (0x1UL << 20)                                    /**< Incomp Iso IN Transfer Mask                 */
#define _USBAHB_GINTMSK_INCOMPISOINMSK_SHIFT               20                                               /**< Shift value for USBAHB_INCOMPISOINMSK       */
#define _USBAHB_GINTMSK_INCOMPISOINMSK_MASK                0x100000UL                                       /**< Bit mask for USBAHB_INCOMPISOINMSK          */
#define _USBAHB_GINTMSK_INCOMPISOINMSK_DEFAULT             0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_INCOMPISOINMSK_DEFAULT              (_USBAHB_GINTMSK_INCOMPISOINMSK_DEFAULT << 20)   /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_INCOMPLISOOUTMSK                    (0x1UL << 21)                                    /**< Incomp Iso OUT Transfer Mask                */
#define _USBAHB_GINTMSK_INCOMPLISOOUTMSK_SHIFT             21                                               /**< Shift value for USBAHB_INCOMPLISOOUTMSK     */
#define _USBAHB_GINTMSK_INCOMPLISOOUTMSK_MASK              0x200000UL                                       /**< Bit mask for USBAHB_INCOMPLISOOUTMSK        */
#define _USBAHB_GINTMSK_INCOMPLISOOUTMSK_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_INCOMPLISOOUTMSK_DEFAULT            (_USBAHB_GINTMSK_INCOMPLISOOUTMSK_DEFAULT << 21) /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_FETSUSPMSK                          (0x1UL << 22)                                    /**< Data Fetch Suspended Mask                   */
#define _USBAHB_GINTMSK_FETSUSPMSK_SHIFT                   22                                               /**< Shift value for USBAHB_FETSUSPMSK           */
#define _USBAHB_GINTMSK_FETSUSPMSK_MASK                    0x400000UL                                       /**< Bit mask for USBAHB_FETSUSPMSK              */
#define _USBAHB_GINTMSK_FETSUSPMSK_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_FETSUSPMSK_DEFAULT                  (_USBAHB_GINTMSK_FETSUSPMSK_DEFAULT << 22)       /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_RESETDETMSK                         (0x1UL << 23)                                    /**< Reset detected Interrupt Mask               */
#define _USBAHB_GINTMSK_RESETDETMSK_SHIFT                  23                                               /**< Shift value for USBAHB_RESETDETMSK          */
#define _USBAHB_GINTMSK_RESETDETMSK_MASK                   0x800000UL                                       /**< Bit mask for USBAHB_RESETDETMSK             */
#define _USBAHB_GINTMSK_RESETDETMSK_DEFAULT                0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_RESETDETMSK_DEFAULT                 (_USBAHB_GINTMSK_RESETDETMSK_DEFAULT << 23)      /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_CONIDSTSCHNGMSK                     (0x1UL << 28)                                    /**< Connector ID Status Change Mask             */
#define _USBAHB_GINTMSK_CONIDSTSCHNGMSK_SHIFT              28                                               /**< Shift value for USBAHB_CONIDSTSCHNGMSK      */
#define _USBAHB_GINTMSK_CONIDSTSCHNGMSK_MASK               0x10000000UL                                     /**< Bit mask for USBAHB_CONIDSTSCHNGMSK         */
#define _USBAHB_GINTMSK_CONIDSTSCHNGMSK_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_CONIDSTSCHNGMSK_DEFAULT             (_USBAHB_GINTMSK_CONIDSTSCHNGMSK_DEFAULT << 28)  /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */
#define USBAHB_GINTMSK_WKUPINTMSK                          (0x1UL << 31)                                    /**< Resume/Remote WkUp Det Irq Mask             */
#define _USBAHB_GINTMSK_WKUPINTMSK_SHIFT                   31                                               /**< Shift value for USBAHB_WKUPINTMSK           */
#define _USBAHB_GINTMSK_WKUPINTMSK_MASK                    0x80000000UL                                     /**< Bit mask for USBAHB_WKUPINTMSK              */
#define _USBAHB_GINTMSK_WKUPINTMSK_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for USBAHB_GINTMSK             */
#define USBAHB_GINTMSK_WKUPINTMSK_DEFAULT                  (_USBAHB_GINTMSK_WKUPINTMSK_DEFAULT << 31)       /**< Shifted mode DEFAULT for USBAHB_GINTMSK     */

/* Bit fields for USBAHB GRXSTSR */
#define _USBAHB_GRXSTSR_RESETVALUE                         0x00000000UL                           /**< Default value for USBAHB_GRXSTSR            */
#define _USBAHB_GRXSTSR_MASK                               0x01FFFFFFUL                           /**< Mask for USBAHB_GRXSTSR                     */
#define _USBAHB_GRXSTSR_EPNUM_SHIFT                        0                                      /**< Shift value for USBAHB_EPNUM                */
#define _USBAHB_GRXSTSR_EPNUM_MASK                         0xFUL                                  /**< Bit mask for USBAHB_EPNUM                   */
#define _USBAHB_GRXSTSR_EPNUM_DEFAULT                      0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSR             */
#define USBAHB_GRXSTSR_EPNUM_DEFAULT                       (_USBAHB_GRXSTSR_EPNUM_DEFAULT << 0)   /**< Shifted mode DEFAULT for USBAHB_GRXSTSR     */
#define _USBAHB_GRXSTSR_BCNT_SHIFT                         4                                      /**< Shift value for USBAHB_BCNT                 */
#define _USBAHB_GRXSTSR_BCNT_MASK                          0x7FF0UL                               /**< Bit mask for USBAHB_BCNT                    */
#define _USBAHB_GRXSTSR_BCNT_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSR             */
#define USBAHB_GRXSTSR_BCNT_DEFAULT                        (_USBAHB_GRXSTSR_BCNT_DEFAULT << 4)    /**< Shifted mode DEFAULT for USBAHB_GRXSTSR     */
#define _USBAHB_GRXSTSR_DPID_SHIFT                         15                                     /**< Shift value for USBAHB_DPID                 */
#define _USBAHB_GRXSTSR_DPID_MASK                          0x18000UL                              /**< Bit mask for USBAHB_DPID                    */
#define _USBAHB_GRXSTSR_DPID_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSR             */
#define USBAHB_GRXSTSR_DPID_DEFAULT                        (_USBAHB_GRXSTSR_DPID_DEFAULT << 15)   /**< Shifted mode DEFAULT for USBAHB_GRXSTSR     */
#define _USBAHB_GRXSTSR_PKTSTS_SHIFT                       17                                     /**< Shift value for USBAHB_PKTSTS               */
#define _USBAHB_GRXSTSR_PKTSTS_MASK                        0x1E0000UL                             /**< Bit mask for USBAHB_PKTSTS                  */
#define _USBAHB_GRXSTSR_PKTSTS_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSR             */
#define USBAHB_GRXSTSR_PKTSTS_DEFAULT                      (_USBAHB_GRXSTSR_PKTSTS_DEFAULT << 17) /**< Shifted mode DEFAULT for USBAHB_GRXSTSR     */
#define _USBAHB_GRXSTSR_FN_SHIFT                           21                                     /**< Shift value for USBAHB_FN                   */
#define _USBAHB_GRXSTSR_FN_MASK                            0x1E00000UL                            /**< Bit mask for USBAHB_FN                      */
#define _USBAHB_GRXSTSR_FN_DEFAULT                         0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSR             */
#define USBAHB_GRXSTSR_FN_DEFAULT                          (_USBAHB_GRXSTSR_FN_DEFAULT << 21)     /**< Shifted mode DEFAULT for USBAHB_GRXSTSR     */

/* Bit fields for USBAHB GRXSTSP */
#define _USBAHB_GRXSTSP_RESETVALUE                         0x00000000UL                           /**< Default value for USBAHB_GRXSTSP            */
#define _USBAHB_GRXSTSP_MASK                               0x01FFFFFFUL                           /**< Mask for USBAHB_GRXSTSP                     */
#define _USBAHB_GRXSTSP_EPNUM_SHIFT                        0                                      /**< Shift value for USBAHB_EPNUM                */
#define _USBAHB_GRXSTSP_EPNUM_MASK                         0xFUL                                  /**< Bit mask for USBAHB_EPNUM                   */
#define _USBAHB_GRXSTSP_EPNUM_DEFAULT                      0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSP             */
#define USBAHB_GRXSTSP_EPNUM_DEFAULT                       (_USBAHB_GRXSTSP_EPNUM_DEFAULT << 0)   /**< Shifted mode DEFAULT for USBAHB_GRXSTSP     */
#define _USBAHB_GRXSTSP_BCNT_SHIFT                         4                                      /**< Shift value for USBAHB_BCNT                 */
#define _USBAHB_GRXSTSP_BCNT_MASK                          0x7FF0UL                               /**< Bit mask for USBAHB_BCNT                    */
#define _USBAHB_GRXSTSP_BCNT_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSP             */
#define USBAHB_GRXSTSP_BCNT_DEFAULT                        (_USBAHB_GRXSTSP_BCNT_DEFAULT << 4)    /**< Shifted mode DEFAULT for USBAHB_GRXSTSP     */
#define _USBAHB_GRXSTSP_DPID_SHIFT                         15                                     /**< Shift value for USBAHB_DPID                 */
#define _USBAHB_GRXSTSP_DPID_MASK                          0x18000UL                              /**< Bit mask for USBAHB_DPID                    */
#define _USBAHB_GRXSTSP_DPID_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSP             */
#define USBAHB_GRXSTSP_DPID_DEFAULT                        (_USBAHB_GRXSTSP_DPID_DEFAULT << 15)   /**< Shifted mode DEFAULT for USBAHB_GRXSTSP     */
#define _USBAHB_GRXSTSP_PKTSTS_SHIFT                       17                                     /**< Shift value for USBAHB_PKTSTS               */
#define _USBAHB_GRXSTSP_PKTSTS_MASK                        0x1E0000UL                             /**< Bit mask for USBAHB_PKTSTS                  */
#define _USBAHB_GRXSTSP_PKTSTS_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSP             */
#define USBAHB_GRXSTSP_PKTSTS_DEFAULT                      (_USBAHB_GRXSTSP_PKTSTS_DEFAULT << 17) /**< Shifted mode DEFAULT for USBAHB_GRXSTSP     */
#define _USBAHB_GRXSTSP_FN_SHIFT                           21                                     /**< Shift value for USBAHB_FN                   */
#define _USBAHB_GRXSTSP_FN_MASK                            0x1E00000UL                            /**< Bit mask for USBAHB_FN                      */
#define _USBAHB_GRXSTSP_FN_DEFAULT                         0x00000000UL                           /**< Mode DEFAULT for USBAHB_GRXSTSP             */
#define USBAHB_GRXSTSP_FN_DEFAULT                          (_USBAHB_GRXSTSP_FN_DEFAULT << 21)     /**< Shifted mode DEFAULT for USBAHB_GRXSTSP     */

/* Bit fields for USBAHB GRXFSIZ */
#define _USBAHB_GRXFSIZ_RESETVALUE                         0x00000300UL                          /**< Default value for USBAHB_GRXFSIZ            */
#define _USBAHB_GRXFSIZ_MASK                               0x000003FFUL                          /**< Mask for USBAHB_GRXFSIZ                     */
#define _USBAHB_GRXFSIZ_RXFDEP_SHIFT                       0                                     /**< Shift value for USBAHB_RXFDEP               */
#define _USBAHB_GRXFSIZ_RXFDEP_MASK                        0x3FFUL                               /**< Bit mask for USBAHB_RXFDEP                  */
#define _USBAHB_GRXFSIZ_RXFDEP_DEFAULT                     0x00000300UL                          /**< Mode DEFAULT for USBAHB_GRXFSIZ             */
#define USBAHB_GRXFSIZ_RXFDEP_DEFAULT                      (_USBAHB_GRXFSIZ_RXFDEP_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_GRXFSIZ     */

/* Bit fields for USBAHB GNPTXFSIZ */
#define _USBAHB_GNPTXFSIZ_RESETVALUE                       0x03000300UL                                       /**< Default value for USBAHB_GNPTXFSIZ          */
#define _USBAHB_GNPTXFSIZ_MASK                             0x07FF07FFUL                                       /**< Mask for USBAHB_GNPTXFSIZ                   */
#define _USBAHB_GNPTXFSIZ_NPTXFSTADDR_SHIFT                0                                                  /**< Shift value for USBAHB_NPTXFSTADDR          */
#define _USBAHB_GNPTXFSIZ_NPTXFSTADDR_MASK                 0x7FFUL                                            /**< Bit mask for USBAHB_NPTXFSTADDR             */
#define _USBAHB_GNPTXFSIZ_NPTXFSTADDR_DEFAULT              0x00000300UL                                       /**< Mode DEFAULT for USBAHB_GNPTXFSIZ           */
#define USBAHB_GNPTXFSIZ_NPTXFSTADDR_DEFAULT               (_USBAHB_GNPTXFSIZ_NPTXFSTADDR_DEFAULT << 0)       /**< Shifted mode DEFAULT for USBAHB_GNPTXFSIZ   */
#define _USBAHB_GNPTXFSIZ_NPTXFINEPTXF0DEP_SHIFT           16                                                 /**< Shift value for USBAHB_NPTXFINEPTXF0DEP     */
#define _USBAHB_GNPTXFSIZ_NPTXFINEPTXF0DEP_MASK            0x7FF0000UL                                        /**< Bit mask for USBAHB_NPTXFINEPTXF0DEP        */
#define _USBAHB_GNPTXFSIZ_NPTXFINEPTXF0DEP_DEFAULT         0x00000300UL                                       /**< Mode DEFAULT for USBAHB_GNPTXFSIZ           */
#define USBAHB_GNPTXFSIZ_NPTXFINEPTXF0DEP_DEFAULT          (_USBAHB_GNPTXFSIZ_NPTXFINEPTXF0DEP_DEFAULT << 16) /**< Shifted mode DEFAULT for USBAHB_GNPTXFSIZ   */

/* Bit fields for USBAHB GSNPSID */
#define _USBAHB_GSNPSID_RESETVALUE                         0x4F54330AUL                              /**< Default value for USBAHB_GSNPSID            */
#define _USBAHB_GSNPSID_MASK                               0xFFFFFFFFUL                              /**< Mask for USBAHB_GSNPSID                     */
#define _USBAHB_GSNPSID_SYNOPSYSID_SHIFT                   0                                         /**< Shift value for USBAHB_SYNOPSYSID           */
#define _USBAHB_GSNPSID_SYNOPSYSID_MASK                    0xFFFFFFFFUL                              /**< Bit mask for USBAHB_SYNOPSYSID              */
#define _USBAHB_GSNPSID_SYNOPSYSID_DEFAULT                 0x4F54330AUL                              /**< Mode DEFAULT for USBAHB_GSNPSID             */
#define USBAHB_GSNPSID_SYNOPSYSID_DEFAULT                  (_USBAHB_GSNPSID_SYNOPSYSID_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_GSNPSID     */

/* Bit fields for USBAHB GHWCFG1 */
#define _USBAHB_GHWCFG1_RESETVALUE                         0x00000000UL                         /**< Default value for USBAHB_GHWCFG1            */
#define _USBAHB_GHWCFG1_MASK                               0xFFFFFFFFUL                         /**< Mask for USBAHB_GHWCFG1                     */
#define _USBAHB_GHWCFG1_EPDIR_SHIFT                        0                                    /**< Shift value for USBAHB_EPDIR                */
#define _USBAHB_GHWCFG1_EPDIR_MASK                         0xFFFFFFFFUL                         /**< Bit mask for USBAHB_EPDIR                   */
#define _USBAHB_GHWCFG1_EPDIR_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_GHWCFG1             */
#define USBAHB_GHWCFG1_EPDIR_DEFAULT                       (_USBAHB_GHWCFG1_EPDIR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_GHWCFG1     */

/* Bit fields for USBAHB GHWCFG2 */
#define _USBAHB_GHWCFG2_RESETVALUE                         0x0000C014UL                                    /**< Default value for USBAHB_GHWCFG2            */
#define _USBAHB_GHWCFG2_MASK                               0x7FDFFFFFUL                                    /**< Mask for USBAHB_GHWCFG2                     */
#define _USBAHB_GHWCFG2_OTGMODE_SHIFT                      0                                               /**< Shift value for USBAHB_OTGMODE              */
#define _USBAHB_GHWCFG2_OTGMODE_MASK                       0x7UL                                           /**< Bit mask for USBAHB_OTGMODE                 */
#define _USBAHB_GHWCFG2_OTGMODE_DEFAULT                    0x00000004UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_OTGMODE_DEFAULT                     (_USBAHB_GHWCFG2_OTGMODE_DEFAULT << 0)          /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define _USBAHB_GHWCFG2_OTGARCH_SHIFT                      3                                               /**< Shift value for USBAHB_OTGARCH              */
#define _USBAHB_GHWCFG2_OTGARCH_MASK                       0x18UL                                          /**< Bit mask for USBAHB_OTGARCH                 */
#define _USBAHB_GHWCFG2_OTGARCH_DEFAULT                    0x00000002UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_OTGARCH_DEFAULT                     (_USBAHB_GHWCFG2_OTGARCH_DEFAULT << 3)          /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define USBAHB_GHWCFG2_SINGPNT                             (0x1UL << 5)                                    /**< Point-to-Point                              */
#define _USBAHB_GHWCFG2_SINGPNT_SHIFT                      5                                               /**< Shift value for USBAHB_SINGPNT              */
#define _USBAHB_GHWCFG2_SINGPNT_MASK                       0x20UL                                          /**< Bit mask for USBAHB_SINGPNT                 */
#define _USBAHB_GHWCFG2_SINGPNT_DEFAULT                    0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_SINGPNT_DEFAULT                     (_USBAHB_GHWCFG2_SINGPNT_DEFAULT << 5)          /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define _USBAHB_GHWCFG2_HSPHYTYPE_SHIFT                    6                                               /**< Shift value for USBAHB_HSPHYTYPE            */
#define _USBAHB_GHWCFG2_HSPHYTYPE_MASK                     0xC0UL                                          /**< Bit mask for USBAHB_HSPHYTYPE               */
#define _USBAHB_GHWCFG2_HSPHYTYPE_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_HSPHYTYPE_DEFAULT                   (_USBAHB_GHWCFG2_HSPHYTYPE_DEFAULT << 6)        /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define _USBAHB_GHWCFG2_FSPHYTYPE_SHIFT                    8                                               /**< Shift value for USBAHB_FSPHYTYPE            */
#define _USBAHB_GHWCFG2_FSPHYTYPE_MASK                     0x300UL                                         /**< Bit mask for USBAHB_FSPHYTYPE               */
#define _USBAHB_GHWCFG2_FSPHYTYPE_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_FSPHYTYPE_DEFAULT                   (_USBAHB_GHWCFG2_FSPHYTYPE_DEFAULT << 8)        /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define _USBAHB_GHWCFG2_NUMDEVEPS_SHIFT                    10                                              /**< Shift value for USBAHB_NUMDEVEPS            */
#define _USBAHB_GHWCFG2_NUMDEVEPS_MASK                     0x3C00UL                                        /**< Bit mask for USBAHB_NUMDEVEPS               */
#define _USBAHB_GHWCFG2_NUMDEVEPS_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_NUMDEVEPS_DEFAULT                   (_USBAHB_GHWCFG2_NUMDEVEPS_DEFAULT << 10)       /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define _USBAHB_GHWCFG2_NUMHSTCHNL_SHIFT                   14                                              /**< Shift value for USBAHB_NUMHSTCHNL           */
#define _USBAHB_GHWCFG2_NUMHSTCHNL_MASK                    0x3C000UL                                       /**< Bit mask for USBAHB_NUMHSTCHNL              */
#define _USBAHB_GHWCFG2_NUMHSTCHNL_DEFAULT                 0x00000003UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_NUMHSTCHNL_DEFAULT                  (_USBAHB_GHWCFG2_NUMHSTCHNL_DEFAULT << 14)      /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define USBAHB_GHWCFG2_PERIOSUPPORT                        (0x1UL << 18)                                   /**< Per OUT Chan Supported in Host Mode         */
#define _USBAHB_GHWCFG2_PERIOSUPPORT_SHIFT                 18                                              /**< Shift value for USBAHB_PERIOSUPPORT         */
#define _USBAHB_GHWCFG2_PERIOSUPPORT_MASK                  0x40000UL                                       /**< Bit mask for USBAHB_PERIOSUPPORT            */
#define _USBAHB_GHWCFG2_PERIOSUPPORT_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_PERIOSUPPORT_DEFAULT                (_USBAHB_GHWCFG2_PERIOSUPPORT_DEFAULT << 18)    /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define USBAHB_GHWCFG2_DYNFIFOSIZING                       (0x1UL << 19)                                   /**< Dynamic FIFO Sizing Enabled                 */
#define _USBAHB_GHWCFG2_DYNFIFOSIZING_SHIFT                19                                              /**< Shift value for USBAHB_DYNFIFOSIZING        */
#define _USBAHB_GHWCFG2_DYNFIFOSIZING_MASK                 0x80000UL                                       /**< Bit mask for USBAHB_DYNFIFOSIZING           */
#define _USBAHB_GHWCFG2_DYNFIFOSIZING_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_DYNFIFOSIZING_DEFAULT               (_USBAHB_GHWCFG2_DYNFIFOSIZING_DEFAULT << 19)   /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define USBAHB_GHWCFG2_MULTIPROCINTRPT                     (0x1UL << 20)                                   /**< Multi Processor Interrupt Enabled           */
#define _USBAHB_GHWCFG2_MULTIPROCINTRPT_SHIFT              20                                              /**< Shift value for USBAHB_MULTIPROCINTRPT      */
#define _USBAHB_GHWCFG2_MULTIPROCINTRPT_MASK               0x100000UL                                      /**< Bit mask for USBAHB_MULTIPROCINTRPT         */
#define _USBAHB_GHWCFG2_MULTIPROCINTRPT_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_MULTIPROCINTRPT_DEFAULT             (_USBAHB_GHWCFG2_MULTIPROCINTRPT_DEFAULT << 20) /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define _USBAHB_GHWCFG2_NPTXQDEPTH_SHIFT                   22                                              /**< Shift value for USBAHB_NPTXQDEPTH           */
#define _USBAHB_GHWCFG2_NPTXQDEPTH_MASK                    0xC00000UL                                      /**< Bit mask for USBAHB_NPTXQDEPTH              */
#define _USBAHB_GHWCFG2_NPTXQDEPTH_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_NPTXQDEPTH_DEFAULT                  (_USBAHB_GHWCFG2_NPTXQDEPTH_DEFAULT << 22)      /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define _USBAHB_GHWCFG2_PTXQDEPTH_SHIFT                    24                                              /**< Shift value for USBAHB_PTXQDEPTH            */
#define _USBAHB_GHWCFG2_PTXQDEPTH_MASK                     0x3000000UL                                     /**< Bit mask for USBAHB_PTXQDEPTH               */
#define _USBAHB_GHWCFG2_PTXQDEPTH_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_PTXQDEPTH_DEFAULT                   (_USBAHB_GHWCFG2_PTXQDEPTH_DEFAULT << 24)       /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */
#define _USBAHB_GHWCFG2_TKNQDEPTH_SHIFT                    26                                              /**< Shift value for USBAHB_TKNQDEPTH            */
#define _USBAHB_GHWCFG2_TKNQDEPTH_MASK                     0x7C000000UL                                    /**< Bit mask for USBAHB_TKNQDEPTH               */
#define _USBAHB_GHWCFG2_TKNQDEPTH_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for USBAHB_GHWCFG2             */
#define USBAHB_GHWCFG2_TKNQDEPTH_DEFAULT                   (_USBAHB_GHWCFG2_TKNQDEPTH_DEFAULT << 26)       /**< Shifted mode DEFAULT for USBAHB_GHWCFG2     */

/* Bit fields for USBAHB GHWCFG3 */
#define _USBAHB_GHWCFG3_RESETVALUE                         0x02EC0468UL                                 /**< Default value for USBAHB_GHWCFG3            */
#define _USBAHB_GHWCFG3_MASK                               0xFFFFFFFFUL                                 /**< Mask for USBAHB_GHWCFG3                     */
#define _USBAHB_GHWCFG3_XFERSIZEWIDTH_SHIFT                0                                            /**< Shift value for USBAHB_XFERSIZEWIDTH        */
#define _USBAHB_GHWCFG3_XFERSIZEWIDTH_MASK                 0xFUL                                        /**< Bit mask for USBAHB_XFERSIZEWIDTH           */
#define _USBAHB_GHWCFG3_XFERSIZEWIDTH_DEFAULT              0x00000008UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_XFERSIZEWIDTH_DEFAULT               (_USBAHB_GHWCFG3_XFERSIZEWIDTH_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define _USBAHB_GHWCFG3_PKTSIZEWIDTH_SHIFT                 4                                            /**< Shift value for USBAHB_PKTSIZEWIDTH         */
#define _USBAHB_GHWCFG3_PKTSIZEWIDTH_MASK                  0x70UL                                       /**< Bit mask for USBAHB_PKTSIZEWIDTH            */
#define _USBAHB_GHWCFG3_PKTSIZEWIDTH_DEFAULT               0x00000006UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_PKTSIZEWIDTH_DEFAULT                (_USBAHB_GHWCFG3_PKTSIZEWIDTH_DEFAULT << 4)  /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define USBAHB_GHWCFG3_OTGEN                               (0x1UL << 7)                                 /**< OTG Function Enabled                        */
#define _USBAHB_GHWCFG3_OTGEN_SHIFT                        7                                            /**< Shift value for USBAHB_OTGEN                */
#define _USBAHB_GHWCFG3_OTGEN_MASK                         0x80UL                                       /**< Bit mask for USBAHB_OTGEN                   */
#define _USBAHB_GHWCFG3_OTGEN_DEFAULT                      0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_OTGEN_DEFAULT                       (_USBAHB_GHWCFG3_OTGEN_DEFAULT << 7)         /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define USBAHB_GHWCFG3_I2CINTSEL                           (0x1UL << 8)                                 /**< I2C Selection                               */
#define _USBAHB_GHWCFG3_I2CINTSEL_SHIFT                    8                                            /**< Shift value for USBAHB_I2CINTSEL            */
#define _USBAHB_GHWCFG3_I2CINTSEL_MASK                     0x100UL                                      /**< Bit mask for USBAHB_I2CINTSEL               */
#define _USBAHB_GHWCFG3_I2CINTSEL_DEFAULT                  0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_I2CINTSEL_DEFAULT                   (_USBAHB_GHWCFG3_I2CINTSEL_DEFAULT << 8)     /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define USBAHB_GHWCFG3_VNDCTLSUPT                          (0x1UL << 9)                                 /**< Vendor Control Interface Support            */
#define _USBAHB_GHWCFG3_VNDCTLSUPT_SHIFT                   9                                            /**< Shift value for USBAHB_VNDCTLSUPT           */
#define _USBAHB_GHWCFG3_VNDCTLSUPT_MASK                    0x200UL                                      /**< Bit mask for USBAHB_VNDCTLSUPT              */
#define _USBAHB_GHWCFG3_VNDCTLSUPT_DEFAULT                 0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_VNDCTLSUPT_DEFAULT                  (_USBAHB_GHWCFG3_VNDCTLSUPT_DEFAULT << 9)    /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define USBAHB_GHWCFG3_OPTFEATURE                          (0x1UL << 10)                                /**< Optional Features Removed                   */
#define _USBAHB_GHWCFG3_OPTFEATURE_SHIFT                   10                                           /**< Shift value for USBAHB_OPTFEATURE           */
#define _USBAHB_GHWCFG3_OPTFEATURE_MASK                    0x400UL                                      /**< Bit mask for USBAHB_OPTFEATURE              */
#define _USBAHB_GHWCFG3_OPTFEATURE_DEFAULT                 0x00000001UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_OPTFEATURE_DEFAULT                  (_USBAHB_GHWCFG3_OPTFEATURE_DEFAULT << 10)   /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define USBAHB_GHWCFG3_RSTTYPE                             (0x1UL << 11)                                /**< Reset Style For Clocked always              */
#define _USBAHB_GHWCFG3_RSTTYPE_SHIFT                      11                                           /**< Shift value for USBAHB_RSTTYPE              */
#define _USBAHB_GHWCFG3_RSTTYPE_MASK                       0x800UL                                      /**< Bit mask for USBAHB_RSTTYPE                 */
#define _USBAHB_GHWCFG3_RSTTYPE_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_RSTTYPE_DEFAULT                     (_USBAHB_GHWCFG3_RSTTYPE_DEFAULT << 11)      /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define USBAHB_GHWCFG3_ADPSUPPORT                          (0x1UL << 12)                                /**< New BitField                                */
#define _USBAHB_GHWCFG3_ADPSUPPORT_SHIFT                   12                                           /**< Shift value for USBAHB_ADPSUPPORT           */
#define _USBAHB_GHWCFG3_ADPSUPPORT_MASK                    0x1000UL                                     /**< Bit mask for USBAHB_ADPSUPPORT              */
#define _USBAHB_GHWCFG3_ADPSUPPORT_DEFAULT                 0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_ADPSUPPORT_DEFAULT                  (_USBAHB_GHWCFG3_ADPSUPPORT_DEFAULT << 12)   /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define USBAHB_GHWCFG3_HSICMODE                            (0x1UL << 13)                                /**< New BitField                                */
#define _USBAHB_GHWCFG3_HSICMODE_SHIFT                     13                                           /**< Shift value for USBAHB_HSICMODE             */
#define _USBAHB_GHWCFG3_HSICMODE_MASK                      0x2000UL                                     /**< Bit mask for USBAHB_HSICMODE                */
#define _USBAHB_GHWCFG3_HSICMODE_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_HSICMODE_DEFAULT                    (_USBAHB_GHWCFG3_HSICMODE_DEFAULT << 13)     /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define USBAHB_GHWCFG3_BCSUPPORT                           (0x1UL << 14)                                /**< New BitField                                */
#define _USBAHB_GHWCFG3_BCSUPPORT_SHIFT                    14                                           /**< Shift value for USBAHB_BCSUPPORT            */
#define _USBAHB_GHWCFG3_BCSUPPORT_MASK                     0x4000UL                                     /**< Bit mask for USBAHB_BCSUPPORT               */
#define _USBAHB_GHWCFG3_BCSUPPORT_DEFAULT                  0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_BCSUPPORT_DEFAULT                   (_USBAHB_GHWCFG3_BCSUPPORT_DEFAULT << 14)    /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define USBAHB_GHWCFG3_LPMMODE                             (0x1UL << 15)                                /**< New BitField                                */
#define _USBAHB_GHWCFG3_LPMMODE_SHIFT                      15                                           /**< Shift value for USBAHB_LPMMODE              */
#define _USBAHB_GHWCFG3_LPMMODE_MASK                       0x8000UL                                     /**< Bit mask for USBAHB_LPMMODE                 */
#define _USBAHB_GHWCFG3_LPMMODE_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_LPMMODE_DEFAULT                     (_USBAHB_GHWCFG3_LPMMODE_DEFAULT << 15)      /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */
#define _USBAHB_GHWCFG3_DFIFODEPTH_SHIFT                   16                                           /**< Shift value for USBAHB_DFIFODEPTH           */
#define _USBAHB_GHWCFG3_DFIFODEPTH_MASK                    0xFFFF0000UL                                 /**< Bit mask for USBAHB_DFIFODEPTH              */
#define _USBAHB_GHWCFG3_DFIFODEPTH_DEFAULT                 0x000002ECUL                                 /**< Mode DEFAULT for USBAHB_GHWCFG3             */
#define USBAHB_GHWCFG3_DFIFODEPTH_DEFAULT                  (_USBAHB_GHWCFG3_DFIFODEPTH_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_GHWCFG3     */

/* Bit fields for USBAHB GHWCFG4 */
#define _USBAHB_GHWCFG4_RESETVALUE                         0x26008030UL                                       /**< Default value for USBAHB_GHWCFG4            */
#define _USBAHB_GHWCFG4_MASK                               0xFFFFC0FFUL                                       /**< Mask for USBAHB_GHWCFG4                     */
#define _USBAHB_GHWCFG4_NUMDEVPERIOEPS_SHIFT               0                                                  /**< Shift value for USBAHB_NUMDEVPERIOEPS       */
#define _USBAHB_GHWCFG4_NUMDEVPERIOEPS_MASK                0xFUL                                              /**< Bit mask for USBAHB_NUMDEVPERIOEPS          */
#define _USBAHB_GHWCFG4_NUMDEVPERIOEPS_DEFAULT             0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_NUMDEVPERIOEPS_DEFAULT              (_USBAHB_GHWCFG4_NUMDEVPERIOEPS_DEFAULT << 0)      /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_PARTIALPWRDN                        (0x1UL << 4)                                       /**< New BitField                                */
#define _USBAHB_GHWCFG4_PARTIALPWRDN_SHIFT                 4                                                  /**< Shift value for USBAHB_PARTIALPWRDN         */
#define _USBAHB_GHWCFG4_PARTIALPWRDN_MASK                  0x10UL                                             /**< Bit mask for USBAHB_PARTIALPWRDN            */
#define _USBAHB_GHWCFG4_PARTIALPWRDN_DEFAULT               0x00000001UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_PARTIALPWRDN_DEFAULT                (_USBAHB_GHWCFG4_PARTIALPWRDN_DEFAULT << 4)        /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_AHBFREQ                             (0x1UL << 5)                                       /**< AHB Frequency                               */
#define _USBAHB_GHWCFG4_AHBFREQ_SHIFT                      5                                                  /**< Shift value for USBAHB_AHBFREQ              */
#define _USBAHB_GHWCFG4_AHBFREQ_MASK                       0x20UL                                             /**< Bit mask for USBAHB_AHBFREQ                 */
#define _USBAHB_GHWCFG4_AHBFREQ_DEFAULT                    0x00000001UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_AHBFREQ_DEFAULT                     (_USBAHB_GHWCFG4_AHBFREQ_DEFAULT << 5)             /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_HIBERNATION                         (0x1UL << 6)                                       /**< New BitField                                */
#define _USBAHB_GHWCFG4_HIBERNATION_SHIFT                  6                                                  /**< Shift value for USBAHB_HIBERNATION          */
#define _USBAHB_GHWCFG4_HIBERNATION_MASK                   0x40UL                                             /**< Bit mask for USBAHB_HIBERNATION             */
#define _USBAHB_GHWCFG4_HIBERNATION_DEFAULT                0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_HIBERNATION_DEFAULT                 (_USBAHB_GHWCFG4_HIBERNATION_DEFAULT << 6)         /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_EXTENDEDHIBERNATION                 (0x1UL << 7)                                       /**< New BitField                                */
#define _USBAHB_GHWCFG4_EXTENDEDHIBERNATION_SHIFT          7                                                  /**< Shift value for USBAHB_EXTENDEDHIBERNATION  */
#define _USBAHB_GHWCFG4_EXTENDEDHIBERNATION_MASK           0x80UL                                             /**< Bit mask for USBAHB_EXTENDEDHIBERNATION     */
#define _USBAHB_GHWCFG4_EXTENDEDHIBERNATION_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_EXTENDEDHIBERNATION_DEFAULT         (_USBAHB_GHWCFG4_EXTENDEDHIBERNATION_DEFAULT << 7) /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define _USBAHB_GHWCFG4_PHYDATAWIDTH_SHIFT                 14                                                 /**< Shift value for USBAHB_PHYDATAWIDTH         */
#define _USBAHB_GHWCFG4_PHYDATAWIDTH_MASK                  0xC000UL                                           /**< Bit mask for USBAHB_PHYDATAWIDTH            */
#define _USBAHB_GHWCFG4_PHYDATAWIDTH_DEFAULT               0x00000002UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_PHYDATAWIDTH_DEFAULT                (_USBAHB_GHWCFG4_PHYDATAWIDTH_DEFAULT << 14)       /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define _USBAHB_GHWCFG4_NUMCTLEPS_SHIFT                    16                                                 /**< Shift value for USBAHB_NUMCTLEPS            */
#define _USBAHB_GHWCFG4_NUMCTLEPS_MASK                     0xF0000UL                                          /**< Bit mask for USBAHB_NUMCTLEPS               */
#define _USBAHB_GHWCFG4_NUMCTLEPS_DEFAULT                  0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_NUMCTLEPS_DEFAULT                   (_USBAHB_GHWCFG4_NUMCTLEPS_DEFAULT << 16)          /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_IDDGFLTR                            (0x1UL << 20)                                      /**< New BitField                                */
#define _USBAHB_GHWCFG4_IDDGFLTR_SHIFT                     20                                                 /**< Shift value for USBAHB_IDDGFLTR             */
#define _USBAHB_GHWCFG4_IDDGFLTR_MASK                      0x100000UL                                         /**< Bit mask for USBAHB_IDDGFLTR                */
#define _USBAHB_GHWCFG4_IDDGFLTR_DEFAULT                   0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_IDDGFLTR_DEFAULT                    (_USBAHB_GHWCFG4_IDDGFLTR_DEFAULT << 20)           /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_VBUSVALIDFLTR                       (0x1UL << 21)                                      /**< New BitField                                */
#define _USBAHB_GHWCFG4_VBUSVALIDFLTR_SHIFT                21                                                 /**< Shift value for USBAHB_VBUSVALIDFLTR        */
#define _USBAHB_GHWCFG4_VBUSVALIDFLTR_MASK                 0x200000UL                                         /**< Bit mask for USBAHB_VBUSVALIDFLTR           */
#define _USBAHB_GHWCFG4_VBUSVALIDFLTR_DEFAULT              0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_VBUSVALIDFLTR_DEFAULT               (_USBAHB_GHWCFG4_VBUSVALIDFLTR_DEFAULT << 21)      /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_AVALIDFLTR                          (0x1UL << 22)                                      /**< New BitField                                */
#define _USBAHB_GHWCFG4_AVALIDFLTR_SHIFT                   22                                                 /**< Shift value for USBAHB_AVALIDFLTR           */
#define _USBAHB_GHWCFG4_AVALIDFLTR_MASK                    0x400000UL                                         /**< Bit mask for USBAHB_AVALIDFLTR              */
#define _USBAHB_GHWCFG4_AVALIDFLTR_DEFAULT                 0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_AVALIDFLTR_DEFAULT                  (_USBAHB_GHWCFG4_AVALIDFLTR_DEFAULT << 22)         /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_BVALIDFLTR                          (0x1UL << 23)                                      /**< New BitField                                */
#define _USBAHB_GHWCFG4_BVALIDFLTR_SHIFT                   23                                                 /**< Shift value for USBAHB_BVALIDFLTR           */
#define _USBAHB_GHWCFG4_BVALIDFLTR_MASK                    0x800000UL                                         /**< Bit mask for USBAHB_BVALIDFLTR              */
#define _USBAHB_GHWCFG4_BVALIDFLTR_DEFAULT                 0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_BVALIDFLTR_DEFAULT                  (_USBAHB_GHWCFG4_BVALIDFLTR_DEFAULT << 23)         /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_SESSENDFLTR                         (0x1UL << 24)                                      /**< New BitField                                */
#define _USBAHB_GHWCFG4_SESSENDFLTR_SHIFT                  24                                                 /**< Shift value for USBAHB_SESSENDFLTR          */
#define _USBAHB_GHWCFG4_SESSENDFLTR_MASK                   0x1000000UL                                        /**< Bit mask for USBAHB_SESSENDFLTR             */
#define _USBAHB_GHWCFG4_SESSENDFLTR_DEFAULT                0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_SESSENDFLTR_DEFAULT                 (_USBAHB_GHWCFG4_SESSENDFLTR_DEFAULT << 24)        /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_DEDFIFOMODE                         (0x1UL << 25)                                      /**< Dedicated xmit FIFO For device IN EPs       */
#define _USBAHB_GHWCFG4_DEDFIFOMODE_SHIFT                  25                                                 /**< Shift value for USBAHB_DEDFIFOMODE          */
#define _USBAHB_GHWCFG4_DEDFIFOMODE_MASK                   0x2000000UL                                        /**< Bit mask for USBAHB_DEDFIFOMODE             */
#define _USBAHB_GHWCFG4_DEDFIFOMODE_DEFAULT                0x00000001UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_DEDFIFOMODE_DEFAULT                 (_USBAHB_GHWCFG4_DEDFIFOMODE_DEFAULT << 25)        /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define _USBAHB_GHWCFG4_INEPS_SHIFT                        26                                                 /**< Shift value for USBAHB_INEPS                */
#define _USBAHB_GHWCFG4_INEPS_MASK                         0x3C000000UL                                       /**< Bit mask for USBAHB_INEPS                   */
#define _USBAHB_GHWCFG4_INEPS_DEFAULT                      0x00000009UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_INEPS_DEFAULT                       (_USBAHB_GHWCFG4_INEPS_DEFAULT << 26)              /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_DESCDMAENABLED                      (0x1UL << 30)                                      /**< New BitField                                */
#define _USBAHB_GHWCFG4_DESCDMAENABLED_SHIFT               30                                                 /**< Shift value for USBAHB_DESCDMAENABLED       */
#define _USBAHB_GHWCFG4_DESCDMAENABLED_MASK                0x40000000UL                                       /**< Bit mask for USBAHB_DESCDMAENABLED          */
#define _USBAHB_GHWCFG4_DESCDMAENABLED_DEFAULT             0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_DESCDMAENABLED_DEFAULT              (_USBAHB_GHWCFG4_DESCDMAENABLED_DEFAULT << 30)     /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */
#define USBAHB_GHWCFG4_DESCDMA                             (0x1UL << 31)                                      /**< New BitField                                */
#define _USBAHB_GHWCFG4_DESCDMA_SHIFT                      31                                                 /**< Shift value for USBAHB_DESCDMA              */
#define _USBAHB_GHWCFG4_DESCDMA_MASK                       0x80000000UL                                       /**< Bit mask for USBAHB_DESCDMA                 */
#define _USBAHB_GHWCFG4_DESCDMA_DEFAULT                    0x00000000UL                                       /**< Mode DEFAULT for USBAHB_GHWCFG4             */
#define USBAHB_GHWCFG4_DESCDMA_DEFAULT                     (_USBAHB_GHWCFG4_DESCDMA_DEFAULT << 31)            /**< Shifted mode DEFAULT for USBAHB_GHWCFG4     */

/* Bit fields for USBAHB GDFIFOCFG */
#define _USBAHB_GDFIFOCFG_RESETVALUE                       0x02EC0300UL                                     /**< Default value for USBAHB_GDFIFOCFG          */
#define _USBAHB_GDFIFOCFG_MASK                             0xFFFFFFFFUL                                     /**< Mask for USBAHB_GDFIFOCFG                   */
#define _USBAHB_GDFIFOCFG_GDFIFOCFG_SHIFT                  0                                                /**< Shift value for USBAHB_GDFIFOCFG            */
#define _USBAHB_GDFIFOCFG_GDFIFOCFG_MASK                   0xFFFFUL                                         /**< Bit mask for USBAHB_GDFIFOCFG               */
#define _USBAHB_GDFIFOCFG_GDFIFOCFG_DEFAULT                0x00000300UL                                     /**< Mode DEFAULT for USBAHB_GDFIFOCFG           */
#define USBAHB_GDFIFOCFG_GDFIFOCFG_DEFAULT                 (_USBAHB_GDFIFOCFG_GDFIFOCFG_DEFAULT << 0)       /**< Shifted mode DEFAULT for USBAHB_GDFIFOCFG   */
#define _USBAHB_GDFIFOCFG_EPINFOBASEADDR_SHIFT             16                                               /**< Shift value for USBAHB_EPINFOBASEADDR       */
#define _USBAHB_GDFIFOCFG_EPINFOBASEADDR_MASK              0xFFFF0000UL                                     /**< Bit mask for USBAHB_EPINFOBASEADDR          */
#define _USBAHB_GDFIFOCFG_EPINFOBASEADDR_DEFAULT           0x000002ECUL                                     /**< Mode DEFAULT for USBAHB_GDFIFOCFG           */
#define USBAHB_GDFIFOCFG_EPINFOBASEADDR_DEFAULT            (_USBAHB_GDFIFOCFG_EPINFOBASEADDR_DEFAULT << 16) /**< Shifted mode DEFAULT for USBAHB_GDFIFOCFG   */

/* Bit fields for USBAHB DIEPTXF1 */
#define _USBAHB_DIEPTXF1_RESETVALUE                        0x03000600UL                                   /**< Default value for USBAHB_DIEPTXF1           */
#define _USBAHB_DIEPTXF1_MASK                              0x03FF0FFFUL                                   /**< Mask for USBAHB_DIEPTXF1                    */
#define _USBAHB_DIEPTXF1_INEPNTXFSTADDR_SHIFT              0                                              /**< Shift value for USBAHB_INEPNTXFSTADDR       */
#define _USBAHB_DIEPTXF1_INEPNTXFSTADDR_MASK               0xFFFUL                                        /**< Bit mask for USBAHB_INEPNTXFSTADDR          */
#define _USBAHB_DIEPTXF1_INEPNTXFSTADDR_DEFAULT            0x00000600UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF1            */
#define USBAHB_DIEPTXF1_INEPNTXFSTADDR_DEFAULT             (_USBAHB_DIEPTXF1_INEPNTXFSTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPTXF1    */
#define _USBAHB_DIEPTXF1_INEPNTXFDEP_SHIFT                 16                                             /**< Shift value for USBAHB_INEPNTXFDEP          */
#define _USBAHB_DIEPTXF1_INEPNTXFDEP_MASK                  0x3FF0000UL                                    /**< Bit mask for USBAHB_INEPNTXFDEP             */
#define _USBAHB_DIEPTXF1_INEPNTXFDEP_DEFAULT               0x00000300UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF1            */
#define USBAHB_DIEPTXF1_INEPNTXFDEP_DEFAULT                (_USBAHB_DIEPTXF1_INEPNTXFDEP_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_DIEPTXF1    */

/* Bit fields for USBAHB DIEPTXF2 */
#define _USBAHB_DIEPTXF2_RESETVALUE                        0x03000900UL                                   /**< Default value for USBAHB_DIEPTXF2           */
#define _USBAHB_DIEPTXF2_MASK                              0x03FF0FFFUL                                   /**< Mask for USBAHB_DIEPTXF2                    */
#define _USBAHB_DIEPTXF2_INEPNTXFSTADDR_SHIFT              0                                              /**< Shift value for USBAHB_INEPNTXFSTADDR       */
#define _USBAHB_DIEPTXF2_INEPNTXFSTADDR_MASK               0xFFFUL                                        /**< Bit mask for USBAHB_INEPNTXFSTADDR          */
#define _USBAHB_DIEPTXF2_INEPNTXFSTADDR_DEFAULT            0x00000900UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF2            */
#define USBAHB_DIEPTXF2_INEPNTXFSTADDR_DEFAULT             (_USBAHB_DIEPTXF2_INEPNTXFSTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPTXF2    */
#define _USBAHB_DIEPTXF2_INEPNTXFDEP_SHIFT                 16                                             /**< Shift value for USBAHB_INEPNTXFDEP          */
#define _USBAHB_DIEPTXF2_INEPNTXFDEP_MASK                  0x3FF0000UL                                    /**< Bit mask for USBAHB_INEPNTXFDEP             */
#define _USBAHB_DIEPTXF2_INEPNTXFDEP_DEFAULT               0x00000300UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF2            */
#define USBAHB_DIEPTXF2_INEPNTXFDEP_DEFAULT                (_USBAHB_DIEPTXF2_INEPNTXFDEP_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_DIEPTXF2    */

/* Bit fields for USBAHB DIEPTXF3 */
#define _USBAHB_DIEPTXF3_RESETVALUE                        0x03000C00UL                                   /**< Default value for USBAHB_DIEPTXF3           */
#define _USBAHB_DIEPTXF3_MASK                              0x03FF0FFFUL                                   /**< Mask for USBAHB_DIEPTXF3                    */
#define _USBAHB_DIEPTXF3_INEPNTXFSTADDR_SHIFT              0                                              /**< Shift value for USBAHB_INEPNTXFSTADDR       */
#define _USBAHB_DIEPTXF3_INEPNTXFSTADDR_MASK               0xFFFUL                                        /**< Bit mask for USBAHB_INEPNTXFSTADDR          */
#define _USBAHB_DIEPTXF3_INEPNTXFSTADDR_DEFAULT            0x00000C00UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF3            */
#define USBAHB_DIEPTXF3_INEPNTXFSTADDR_DEFAULT             (_USBAHB_DIEPTXF3_INEPNTXFSTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPTXF3    */
#define _USBAHB_DIEPTXF3_INEPNTXFDEP_SHIFT                 16                                             /**< Shift value for USBAHB_INEPNTXFDEP          */
#define _USBAHB_DIEPTXF3_INEPNTXFDEP_MASK                  0x3FF0000UL                                    /**< Bit mask for USBAHB_INEPNTXFDEP             */
#define _USBAHB_DIEPTXF3_INEPNTXFDEP_DEFAULT               0x00000300UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF3            */
#define USBAHB_DIEPTXF3_INEPNTXFDEP_DEFAULT                (_USBAHB_DIEPTXF3_INEPNTXFDEP_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_DIEPTXF3    */

/* Bit fields for USBAHB DIEPTXF4 */
#define _USBAHB_DIEPTXF4_RESETVALUE                        0x03000F00UL                                   /**< Default value for USBAHB_DIEPTXF4           */
#define _USBAHB_DIEPTXF4_MASK                              0x03FF0FFFUL                                   /**< Mask for USBAHB_DIEPTXF4                    */
#define _USBAHB_DIEPTXF4_INEPNTXFSTADDR_SHIFT              0                                              /**< Shift value for USBAHB_INEPNTXFSTADDR       */
#define _USBAHB_DIEPTXF4_INEPNTXFSTADDR_MASK               0xFFFUL                                        /**< Bit mask for USBAHB_INEPNTXFSTADDR          */
#define _USBAHB_DIEPTXF4_INEPNTXFSTADDR_DEFAULT            0x00000F00UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF4            */
#define USBAHB_DIEPTXF4_INEPNTXFSTADDR_DEFAULT             (_USBAHB_DIEPTXF4_INEPNTXFSTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPTXF4    */
#define _USBAHB_DIEPTXF4_INEPNTXFDEP_SHIFT                 16                                             /**< Shift value for USBAHB_INEPNTXFDEP          */
#define _USBAHB_DIEPTXF4_INEPNTXFDEP_MASK                  0x3FF0000UL                                    /**< Bit mask for USBAHB_INEPNTXFDEP             */
#define _USBAHB_DIEPTXF4_INEPNTXFDEP_DEFAULT               0x00000300UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF4            */
#define USBAHB_DIEPTXF4_INEPNTXFDEP_DEFAULT                (_USBAHB_DIEPTXF4_INEPNTXFDEP_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_DIEPTXF4    */

/* Bit fields for USBAHB DIEPTXF5 */
#define _USBAHB_DIEPTXF5_RESETVALUE                        0x03001200UL                                   /**< Default value for USBAHB_DIEPTXF5           */
#define _USBAHB_DIEPTXF5_MASK                              0x03FF1FFFUL                                   /**< Mask for USBAHB_DIEPTXF5                    */
#define _USBAHB_DIEPTXF5_INEPNTXFSTADDR_SHIFT              0                                              /**< Shift value for USBAHB_INEPNTXFSTADDR       */
#define _USBAHB_DIEPTXF5_INEPNTXFSTADDR_MASK               0x1FFFUL                                       /**< Bit mask for USBAHB_INEPNTXFSTADDR          */
#define _USBAHB_DIEPTXF5_INEPNTXFSTADDR_DEFAULT            0x00001200UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF5            */
#define USBAHB_DIEPTXF5_INEPNTXFSTADDR_DEFAULT             (_USBAHB_DIEPTXF5_INEPNTXFSTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPTXF5    */
#define _USBAHB_DIEPTXF5_INEPNTXFDEP_SHIFT                 16                                             /**< Shift value for USBAHB_INEPNTXFDEP          */
#define _USBAHB_DIEPTXF5_INEPNTXFDEP_MASK                  0x3FF0000UL                                    /**< Bit mask for USBAHB_INEPNTXFDEP             */
#define _USBAHB_DIEPTXF5_INEPNTXFDEP_DEFAULT               0x00000300UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF5            */
#define USBAHB_DIEPTXF5_INEPNTXFDEP_DEFAULT                (_USBAHB_DIEPTXF5_INEPNTXFDEP_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_DIEPTXF5    */

/* Bit fields for USBAHB DIEPTXF6 */
#define _USBAHB_DIEPTXF6_RESETVALUE                        0x03001500UL                                   /**< Default value for USBAHB_DIEPTXF6           */
#define _USBAHB_DIEPTXF6_MASK                              0x03FF1FFFUL                                   /**< Mask for USBAHB_DIEPTXF6                    */
#define _USBAHB_DIEPTXF6_INEPNTXFSTADDR_SHIFT              0                                              /**< Shift value for USBAHB_INEPNTXFSTADDR       */
#define _USBAHB_DIEPTXF6_INEPNTXFSTADDR_MASK               0x1FFFUL                                       /**< Bit mask for USBAHB_INEPNTXFSTADDR          */
#define _USBAHB_DIEPTXF6_INEPNTXFSTADDR_DEFAULT            0x00001500UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF6            */
#define USBAHB_DIEPTXF6_INEPNTXFSTADDR_DEFAULT             (_USBAHB_DIEPTXF6_INEPNTXFSTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPTXF6    */
#define _USBAHB_DIEPTXF6_INEPNTXFDEP_SHIFT                 16                                             /**< Shift value for USBAHB_INEPNTXFDEP          */
#define _USBAHB_DIEPTXF6_INEPNTXFDEP_MASK                  0x3FF0000UL                                    /**< Bit mask for USBAHB_INEPNTXFDEP             */
#define _USBAHB_DIEPTXF6_INEPNTXFDEP_DEFAULT               0x00000300UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF6            */
#define USBAHB_DIEPTXF6_INEPNTXFDEP_DEFAULT                (_USBAHB_DIEPTXF6_INEPNTXFDEP_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_DIEPTXF6    */

/* Bit fields for USBAHB DIEPTXF7 */
#define _USBAHB_DIEPTXF7_RESETVALUE                        0x03001800UL                                   /**< Default value for USBAHB_DIEPTXF7           */
#define _USBAHB_DIEPTXF7_MASK                              0x03FF1FFFUL                                   /**< Mask for USBAHB_DIEPTXF7                    */
#define _USBAHB_DIEPTXF7_INEPNTXFSTADDR_SHIFT              0                                              /**< Shift value for USBAHB_INEPNTXFSTADDR       */
#define _USBAHB_DIEPTXF7_INEPNTXFSTADDR_MASK               0x1FFFUL                                       /**< Bit mask for USBAHB_INEPNTXFSTADDR          */
#define _USBAHB_DIEPTXF7_INEPNTXFSTADDR_DEFAULT            0x00001800UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF7            */
#define USBAHB_DIEPTXF7_INEPNTXFSTADDR_DEFAULT             (_USBAHB_DIEPTXF7_INEPNTXFSTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPTXF7    */
#define _USBAHB_DIEPTXF7_INEPNTXFDEP_SHIFT                 16                                             /**< Shift value for USBAHB_INEPNTXFDEP          */
#define _USBAHB_DIEPTXF7_INEPNTXFDEP_MASK                  0x3FF0000UL                                    /**< Bit mask for USBAHB_INEPNTXFDEP             */
#define _USBAHB_DIEPTXF7_INEPNTXFDEP_DEFAULT               0x00000300UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF7            */
#define USBAHB_DIEPTXF7_INEPNTXFDEP_DEFAULT                (_USBAHB_DIEPTXF7_INEPNTXFDEP_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_DIEPTXF7    */

/* Bit fields for USBAHB DIEPTXF8 */
#define _USBAHB_DIEPTXF8_RESETVALUE                        0x03001B00UL                                   /**< Default value for USBAHB_DIEPTXF8           */
#define _USBAHB_DIEPTXF8_MASK                              0x03FF1FFFUL                                   /**< Mask for USBAHB_DIEPTXF8                    */
#define _USBAHB_DIEPTXF8_INEPNTXFSTADDR_SHIFT              0                                              /**< Shift value for USBAHB_INEPNTXFSTADDR       */
#define _USBAHB_DIEPTXF8_INEPNTXFSTADDR_MASK               0x1FFFUL                                       /**< Bit mask for USBAHB_INEPNTXFSTADDR          */
#define _USBAHB_DIEPTXF8_INEPNTXFSTADDR_DEFAULT            0x00001B00UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF8            */
#define USBAHB_DIEPTXF8_INEPNTXFSTADDR_DEFAULT             (_USBAHB_DIEPTXF8_INEPNTXFSTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPTXF8    */
#define _USBAHB_DIEPTXF8_INEPNTXFDEP_SHIFT                 16                                             /**< Shift value for USBAHB_INEPNTXFDEP          */
#define _USBAHB_DIEPTXF8_INEPNTXFDEP_MASK                  0x3FF0000UL                                    /**< Bit mask for USBAHB_INEPNTXFDEP             */
#define _USBAHB_DIEPTXF8_INEPNTXFDEP_DEFAULT               0x00000300UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF8            */
#define USBAHB_DIEPTXF8_INEPNTXFDEP_DEFAULT                (_USBAHB_DIEPTXF8_INEPNTXFDEP_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_DIEPTXF8    */

/* Bit fields for USBAHB DIEPTXF9 */
#define _USBAHB_DIEPTXF9_RESETVALUE                        0x03001E00UL                                   /**< Default value for USBAHB_DIEPTXF9           */
#define _USBAHB_DIEPTXF9_MASK                              0x03FF1FFFUL                                   /**< Mask for USBAHB_DIEPTXF9                    */
#define _USBAHB_DIEPTXF9_INEPNTXFSTADDR_SHIFT              0                                              /**< Shift value for USBAHB_INEPNTXFSTADDR       */
#define _USBAHB_DIEPTXF9_INEPNTXFSTADDR_MASK               0x1FFFUL                                       /**< Bit mask for USBAHB_INEPNTXFSTADDR          */
#define _USBAHB_DIEPTXF9_INEPNTXFSTADDR_DEFAULT            0x00001E00UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF9            */
#define USBAHB_DIEPTXF9_INEPNTXFSTADDR_DEFAULT             (_USBAHB_DIEPTXF9_INEPNTXFSTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPTXF9    */
#define _USBAHB_DIEPTXF9_INEPNTXFDEP_SHIFT                 16                                             /**< Shift value for USBAHB_INEPNTXFDEP          */
#define _USBAHB_DIEPTXF9_INEPNTXFDEP_MASK                  0x3FF0000UL                                    /**< Bit mask for USBAHB_INEPNTXFDEP             */
#define _USBAHB_DIEPTXF9_INEPNTXFDEP_DEFAULT               0x00000300UL                                   /**< Mode DEFAULT for USBAHB_DIEPTXF9            */
#define USBAHB_DIEPTXF9_INEPNTXFDEP_DEFAULT                (_USBAHB_DIEPTXF9_INEPNTXFDEP_DEFAULT << 16)   /**< Shifted mode DEFAULT for USBAHB_DIEPTXF9    */

/* Bit fields for USBAHB DCFG */
#define _USBAHB_DCFG_RESETVALUE                            0x08000000UL                               /**< Default value for USBAHB_DCFG               */
#define _USBAHB_DCFG_MASK                                  0xFF00FFFFUL                               /**< Mask for USBAHB_DCFG                        */
#define _USBAHB_DCFG_DEVSPD_SHIFT                          0                                          /**< Shift value for USBAHB_DEVSPD               */
#define _USBAHB_DCFG_DEVSPD_MASK                           0x3UL                                      /**< Bit mask for USBAHB_DEVSPD                  */
#define _USBAHB_DCFG_DEVSPD_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_DEVSPD_DEFAULT                         (_USBAHB_DCFG_DEVSPD_DEFAULT << 0)         /**< Shifted mode DEFAULT for USBAHB_DCFG        */
#define USBAHB_DCFG_NZSTSOUTHSHK                           (0x1UL << 2)                               /**< NZ-Length Status OUT Handshake              */
#define _USBAHB_DCFG_NZSTSOUTHSHK_SHIFT                    2                                          /**< Shift value for USBAHB_NZSTSOUTHSHK         */
#define _USBAHB_DCFG_NZSTSOUTHSHK_MASK                     0x4UL                                      /**< Bit mask for USBAHB_NZSTSOUTHSHK            */
#define _USBAHB_DCFG_NZSTSOUTHSHK_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_NZSTSOUTHSHK_DEFAULT                   (_USBAHB_DCFG_NZSTSOUTHSHK_DEFAULT << 2)   /**< Shifted mode DEFAULT for USBAHB_DCFG        */
#define USBAHB_DCFG_ENA32KHZSUSP                           (0x1UL << 3)                               /**< Enable 32 KHz Suspend mode                  */
#define _USBAHB_DCFG_ENA32KHZSUSP_SHIFT                    3                                          /**< Shift value for USBAHB_ENA32KHZSUSP         */
#define _USBAHB_DCFG_ENA32KHZSUSP_MASK                     0x8UL                                      /**< Bit mask for USBAHB_ENA32KHZSUSP            */
#define _USBAHB_DCFG_ENA32KHZSUSP_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_ENA32KHZSUSP_DEFAULT                   (_USBAHB_DCFG_ENA32KHZSUSP_DEFAULT << 3)   /**< Shifted mode DEFAULT for USBAHB_DCFG        */
#define _USBAHB_DCFG_DEVADDR_SHIFT                         4                                          /**< Shift value for USBAHB_DEVADDR              */
#define _USBAHB_DCFG_DEVADDR_MASK                          0x7F0UL                                    /**< Bit mask for USBAHB_DEVADDR                 */
#define _USBAHB_DCFG_DEVADDR_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_DEVADDR_DEFAULT                        (_USBAHB_DCFG_DEVADDR_DEFAULT << 4)        /**< Shifted mode DEFAULT for USBAHB_DCFG        */
#define _USBAHB_DCFG_PERFRINT_SHIFT                        11                                         /**< Shift value for USBAHB_PERFRINT             */
#define _USBAHB_DCFG_PERFRINT_MASK                         0x1800UL                                   /**< Bit mask for USBAHB_PERFRINT                */
#define _USBAHB_DCFG_PERFRINT_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_PERFRINT_DEFAULT                       (_USBAHB_DCFG_PERFRINT_DEFAULT << 11)      /**< Shifted mode DEFAULT for USBAHB_DCFG        */
#define USBAHB_DCFG_ENDEVOUTNAK                            (0x1UL << 13)                              /**< Enable Device out of NAK                    */
#define _USBAHB_DCFG_ENDEVOUTNAK_SHIFT                     13                                         /**< Shift value for USBAHB_ENDEVOUTNAK          */
#define _USBAHB_DCFG_ENDEVOUTNAK_MASK                      0x2000UL                                   /**< Bit mask for USBAHB_ENDEVOUTNAK             */
#define _USBAHB_DCFG_ENDEVOUTNAK_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_ENDEVOUTNAK_DEFAULT                    (_USBAHB_DCFG_ENDEVOUTNAK_DEFAULT << 13)   /**< Shifted mode DEFAULT for USBAHB_DCFG        */
#define USBAHB_DCFG_XCVRDLY                                (0x1UL << 14)                              /**<                                             */
#define _USBAHB_DCFG_XCVRDLY_SHIFT                         14                                         /**< Shift value for USBAHB_XCVRDLY              */
#define _USBAHB_DCFG_XCVRDLY_MASK                          0x4000UL                                   /**< Bit mask for USBAHB_XCVRDLY                 */
#define _USBAHB_DCFG_XCVRDLY_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_XCVRDLY_DEFAULT                        (_USBAHB_DCFG_XCVRDLY_DEFAULT << 14)       /**< Shifted mode DEFAULT for USBAHB_DCFG        */
#define USBAHB_DCFG_ERRATICINTMSK                          (0x1UL << 15)                              /**<                                             */
#define _USBAHB_DCFG_ERRATICINTMSK_SHIFT                   15                                         /**< Shift value for USBAHB_ERRATICINTMSK        */
#define _USBAHB_DCFG_ERRATICINTMSK_MASK                    0x8000UL                                   /**< Bit mask for USBAHB_ERRATICINTMSK           */
#define _USBAHB_DCFG_ERRATICINTMSK_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_ERRATICINTMSK_DEFAULT                  (_USBAHB_DCFG_ERRATICINTMSK_DEFAULT << 15) /**< Shifted mode DEFAULT for USBAHB_DCFG        */
#define _USBAHB_DCFG_PERSCHINTVL_SHIFT                     24                                         /**< Shift value for USBAHB_PERSCHINTVL          */
#define _USBAHB_DCFG_PERSCHINTVL_MASK                      0x3000000UL                                /**< Bit mask for USBAHB_PERSCHINTVL             */
#define _USBAHB_DCFG_PERSCHINTVL_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_PERSCHINTVL_DEFAULT                    (_USBAHB_DCFG_PERSCHINTVL_DEFAULT << 24)   /**< Shifted mode DEFAULT for USBAHB_DCFG        */
#define _USBAHB_DCFG_RESVALID_SHIFT                        26                                         /**< Shift value for USBAHB_RESVALID             */
#define _USBAHB_DCFG_RESVALID_MASK                         0xFC000000UL                               /**< Bit mask for USBAHB_RESVALID                */
#define _USBAHB_DCFG_RESVALID_DEFAULT                      0x00000002UL                               /**< Mode DEFAULT for USBAHB_DCFG                */
#define USBAHB_DCFG_RESVALID_DEFAULT                       (_USBAHB_DCFG_RESVALID_DEFAULT << 26)      /**< Shifted mode DEFAULT for USBAHB_DCFG        */

/* Bit fields for USBAHB DCTL */
#define _USBAHB_DCTL_RESETVALUE                            0x00000002UL                              /**< Default value for USBAHB_DCTL               */
#define _USBAHB_DCTL_MASK                                  0x00018FFFUL                              /**< Mask for USBAHB_DCTL                        */
#define USBAHB_DCTL_RMTWKUPSIG                             (0x1UL << 0)                              /**< Remote Wakeup Signalling                    */
#define _USBAHB_DCTL_RMTWKUPSIG_SHIFT                      0                                         /**< Shift value for USBAHB_RMTWKUPSIG           */
#define _USBAHB_DCTL_RMTWKUPSIG_MASK                       0x1UL                                     /**< Bit mask for USBAHB_RMTWKUPSIG              */
#define _USBAHB_DCTL_RMTWKUPSIG_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_RMTWKUPSIG_DEFAULT                     (_USBAHB_DCTL_RMTWKUPSIG_DEFAULT << 0)    /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_SFTDISCON                              (0x1UL << 1)                              /**< Soft Disconnect                             */
#define _USBAHB_DCTL_SFTDISCON_SHIFT                       1                                         /**< Shift value for USBAHB_SFTDISCON            */
#define _USBAHB_DCTL_SFTDISCON_MASK                        0x2UL                                     /**< Bit mask for USBAHB_SFTDISCON               */
#define _USBAHB_DCTL_SFTDISCON_DEFAULT                     0x00000001UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_SFTDISCON_DEFAULT                      (_USBAHB_DCTL_SFTDISCON_DEFAULT << 1)     /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_GNPINNAKSTS                            (0x1UL << 2)                              /**< Global Non-periodic IN NAK Status           */
#define _USBAHB_DCTL_GNPINNAKSTS_SHIFT                     2                                         /**< Shift value for USBAHB_GNPINNAKSTS          */
#define _USBAHB_DCTL_GNPINNAKSTS_MASK                      0x4UL                                     /**< Bit mask for USBAHB_GNPINNAKSTS             */
#define _USBAHB_DCTL_GNPINNAKSTS_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_GNPINNAKSTS_DEFAULT                    (_USBAHB_DCTL_GNPINNAKSTS_DEFAULT << 2)   /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_GOUTNAKSTS                             (0x1UL << 3)                              /**< Global OUT NAK Status                       */
#define _USBAHB_DCTL_GOUTNAKSTS_SHIFT                      3                                         /**< Shift value for USBAHB_GOUTNAKSTS           */
#define _USBAHB_DCTL_GOUTNAKSTS_MASK                       0x8UL                                     /**< Bit mask for USBAHB_GOUTNAKSTS              */
#define _USBAHB_DCTL_GOUTNAKSTS_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_GOUTNAKSTS_DEFAULT                     (_USBAHB_DCTL_GOUTNAKSTS_DEFAULT << 3)    /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define _USBAHB_DCTL_TSTCTL_SHIFT                          4                                         /**< Shift value for USBAHB_TSTCTL               */
#define _USBAHB_DCTL_TSTCTL_MASK                           0x70UL                                    /**< Bit mask for USBAHB_TSTCTL                  */
#define _USBAHB_DCTL_TSTCTL_DEFAULT                        0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_TSTCTL_DEFAULT                         (_USBAHB_DCTL_TSTCTL_DEFAULT << 4)        /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_SGNPINNAK                              (0x1UL << 7)                              /**< Set Global Non-periodic in NAK              */
#define _USBAHB_DCTL_SGNPINNAK_SHIFT                       7                                         /**< Shift value for USBAHB_SGNPINNAK            */
#define _USBAHB_DCTL_SGNPINNAK_MASK                        0x80UL                                    /**< Bit mask for USBAHB_SGNPINNAK               */
#define _USBAHB_DCTL_SGNPINNAK_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_SGNPINNAK_DEFAULT                      (_USBAHB_DCTL_SGNPINNAK_DEFAULT << 7)     /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_CGNPINNAK                              (0x1UL << 8)                              /**< Clear Global Non-periodic IN NAK            */
#define _USBAHB_DCTL_CGNPINNAK_SHIFT                       8                                         /**< Shift value for USBAHB_CGNPINNAK            */
#define _USBAHB_DCTL_CGNPINNAK_MASK                        0x100UL                                   /**< Bit mask for USBAHB_CGNPINNAK               */
#define _USBAHB_DCTL_CGNPINNAK_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_CGNPINNAK_DEFAULT                      (_USBAHB_DCTL_CGNPINNAK_DEFAULT << 8)     /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_SGOUTNAK                               (0x1UL << 9)                              /**< Set Global Out NAK                          */
#define _USBAHB_DCTL_SGOUTNAK_SHIFT                        9                                         /**< Shift value for USBAHB_SGOUTNAK             */
#define _USBAHB_DCTL_SGOUTNAK_MASK                         0x200UL                                   /**< Bit mask for USBAHB_SGOUTNAK                */
#define _USBAHB_DCTL_SGOUTNAK_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_SGOUTNAK_DEFAULT                       (_USBAHB_DCTL_SGOUTNAK_DEFAULT << 9)      /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_CGOUTNAK                               (0x1UL << 10)                             /**< Clear Global Out NAK                        */
#define _USBAHB_DCTL_CGOUTNAK_SHIFT                        10                                        /**< Shift value for USBAHB_CGOUTNAK             */
#define _USBAHB_DCTL_CGOUTNAK_MASK                         0x400UL                                   /**< Bit mask for USBAHB_CGOUTNAK                */
#define _USBAHB_DCTL_CGOUTNAK_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_CGOUTNAK_DEFAULT                       (_USBAHB_DCTL_CGOUTNAK_DEFAULT << 10)     /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_PWRONPRGDONE                           (0x1UL << 11)                             /**< Power-On Programming Done                   */
#define _USBAHB_DCTL_PWRONPRGDONE_SHIFT                    11                                        /**< Shift value for USBAHB_PWRONPRGDONE         */
#define _USBAHB_DCTL_PWRONPRGDONE_MASK                     0x800UL                                   /**< Bit mask for USBAHB_PWRONPRGDONE            */
#define _USBAHB_DCTL_PWRONPRGDONE_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_PWRONPRGDONE_DEFAULT                   (_USBAHB_DCTL_PWRONPRGDONE_DEFAULT << 11) /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_IGNRFRMNUM                             (0x1UL << 15)                             /**< Ignore frame num for ISO end points         */
#define _USBAHB_DCTL_IGNRFRMNUM_SHIFT                      15                                        /**< Shift value for USBAHB_IGNRFRMNUM           */
#define _USBAHB_DCTL_IGNRFRMNUM_MASK                       0x8000UL                                  /**< Bit mask for USBAHB_IGNRFRMNUM              */
#define _USBAHB_DCTL_IGNRFRMNUM_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_IGNRFRMNUM_DEFAULT                     (_USBAHB_DCTL_IGNRFRMNUM_DEFAULT << 15)   /**< Shifted mode DEFAULT for USBAHB_DCTL        */
#define USBAHB_DCTL_NAKONBBLE                              (0x1UL << 16)                             /**< NAK on Babble Error                         */
#define _USBAHB_DCTL_NAKONBBLE_SHIFT                       16                                        /**< Shift value for USBAHB_NAKONBBLE            */
#define _USBAHB_DCTL_NAKONBBLE_MASK                        0x10000UL                                 /**< Bit mask for USBAHB_NAKONBBLE               */
#define _USBAHB_DCTL_NAKONBBLE_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for USBAHB_DCTL                */
#define USBAHB_DCTL_NAKONBBLE_DEFAULT                      (_USBAHB_DCTL_NAKONBBLE_DEFAULT << 16)    /**< Shifted mode DEFAULT for USBAHB_DCTL        */

/* Bit fields for USBAHB DSTS */
#define _USBAHB_DSTS_RESETVALUE                            0x00000000UL                          /**< Default value for USBAHB_DSTS               */
#define _USBAHB_DSTS_MASK                                  0x00FFFF0FUL                          /**< Mask for USBAHB_DSTS                        */
#define USBAHB_DSTS_SUSPSTS                                (0x1UL << 0)                          /**< Suspend Status                              */
#define _USBAHB_DSTS_SUSPSTS_SHIFT                         0                                     /**< Shift value for USBAHB_SUSPSTS              */
#define _USBAHB_DSTS_SUSPSTS_MASK                          0x1UL                                 /**< Bit mask for USBAHB_SUSPSTS                 */
#define _USBAHB_DSTS_SUSPSTS_DEFAULT                       0x00000000UL                          /**< Mode DEFAULT for USBAHB_DSTS                */
#define USBAHB_DSTS_SUSPSTS_DEFAULT                        (_USBAHB_DSTS_SUSPSTS_DEFAULT << 0)   /**< Shifted mode DEFAULT for USBAHB_DSTS        */
#define _USBAHB_DSTS_ENUMSPD_SHIFT                         1                                     /**< Shift value for USBAHB_ENUMSPD              */
#define _USBAHB_DSTS_ENUMSPD_MASK                          0x6UL                                 /**< Bit mask for USBAHB_ENUMSPD                 */
#define _USBAHB_DSTS_ENUMSPD_DEFAULT                       0x00000000UL                          /**< Mode DEFAULT for USBAHB_DSTS                */
#define USBAHB_DSTS_ENUMSPD_DEFAULT                        (_USBAHB_DSTS_ENUMSPD_DEFAULT << 1)   /**< Shifted mode DEFAULT for USBAHB_DSTS        */
#define USBAHB_DSTS_ERRTICERR                              (0x1UL << 3)                          /**< Erratic Error                               */
#define _USBAHB_DSTS_ERRTICERR_SHIFT                       3                                     /**< Shift value for USBAHB_ERRTICERR            */
#define _USBAHB_DSTS_ERRTICERR_MASK                        0x8UL                                 /**< Bit mask for USBAHB_ERRTICERR               */
#define _USBAHB_DSTS_ERRTICERR_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for USBAHB_DSTS                */
#define USBAHB_DSTS_ERRTICERR_DEFAULT                      (_USBAHB_DSTS_ERRTICERR_DEFAULT << 3) /**< Shifted mode DEFAULT for USBAHB_DSTS        */
#define _USBAHB_DSTS_SOFFN_SHIFT                           8                                     /**< Shift value for USBAHB_SOFFN                */
#define _USBAHB_DSTS_SOFFN_MASK                            0x3FFF00UL                            /**< Bit mask for USBAHB_SOFFN                   */
#define _USBAHB_DSTS_SOFFN_DEFAULT                         0x00000000UL                          /**< Mode DEFAULT for USBAHB_DSTS                */
#define USBAHB_DSTS_SOFFN_DEFAULT                          (_USBAHB_DSTS_SOFFN_DEFAULT << 8)     /**< Shifted mode DEFAULT for USBAHB_DSTS        */
#define _USBAHB_DSTS_DEVLNSTS_SHIFT                        22                                    /**< Shift value for USBAHB_DEVLNSTS             */
#define _USBAHB_DSTS_DEVLNSTS_MASK                         0xC00000UL                            /**< Bit mask for USBAHB_DEVLNSTS                */
#define _USBAHB_DSTS_DEVLNSTS_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for USBAHB_DSTS                */
#define USBAHB_DSTS_DEVLNSTS_DEFAULT                       (_USBAHB_DSTS_DEVLNSTS_DEFAULT << 22) /**< Shifted mode DEFAULT for USBAHB_DSTS        */

/* Bit fields for USBAHB DIEPMSK */
#define _USBAHB_DIEPMSK_RESETVALUE                         0x00000000UL                                  /**< Default value for USBAHB_DIEPMSK            */
#define _USBAHB_DIEPMSK_MASK                               0x0000217FUL                                  /**< Mask for USBAHB_DIEPMSK                     */
#define USBAHB_DIEPMSK_XFERCOMPLMSK                        (0x1UL << 0)                                  /**< Transfer Completed Interrupt Mask           */
#define _USBAHB_DIEPMSK_XFERCOMPLMSK_SHIFT                 0                                             /**< Shift value for USBAHB_XFERCOMPLMSK         */
#define _USBAHB_DIEPMSK_XFERCOMPLMSK_MASK                  0x1UL                                         /**< Bit mask for USBAHB_XFERCOMPLMSK            */
#define _USBAHB_DIEPMSK_XFERCOMPLMSK_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DIEPMSK             */
#define USBAHB_DIEPMSK_XFERCOMPLMSK_DEFAULT                (_USBAHB_DIEPMSK_XFERCOMPLMSK_DEFAULT << 0)   /**< Shifted mode DEFAULT for USBAHB_DIEPMSK     */
#define USBAHB_DIEPMSK_EPDISBLDMSK                         (0x1UL << 1)                                  /**< Endpoint Disabled Interrupt Mask            */
#define _USBAHB_DIEPMSK_EPDISBLDMSK_SHIFT                  1                                             /**< Shift value for USBAHB_EPDISBLDMSK          */
#define _USBAHB_DIEPMSK_EPDISBLDMSK_MASK                   0x2UL                                         /**< Bit mask for USBAHB_EPDISBLDMSK             */
#define _USBAHB_DIEPMSK_EPDISBLDMSK_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DIEPMSK             */
#define USBAHB_DIEPMSK_EPDISBLDMSK_DEFAULT                 (_USBAHB_DIEPMSK_EPDISBLDMSK_DEFAULT << 1)    /**< Shifted mode DEFAULT for USBAHB_DIEPMSK     */
#define USBAHB_DIEPMSK_AHBERRMSK                           (0x1UL << 2)                                  /**< AHB Error Mask                              */
#define _USBAHB_DIEPMSK_AHBERRMSK_SHIFT                    2                                             /**< Shift value for USBAHB_AHBERRMSK            */
#define _USBAHB_DIEPMSK_AHBERRMSK_MASK                     0x4UL                                         /**< Bit mask for USBAHB_AHBERRMSK               */
#define _USBAHB_DIEPMSK_AHBERRMSK_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DIEPMSK             */
#define USBAHB_DIEPMSK_AHBERRMSK_DEFAULT                   (_USBAHB_DIEPMSK_AHBERRMSK_DEFAULT << 2)      /**< Shifted mode DEFAULT for USBAHB_DIEPMSK     */
#define USBAHB_DIEPMSK_TIMEOUTMSK                          (0x1UL << 3)                                  /**< Timout Condition Mask                       */
#define _USBAHB_DIEPMSK_TIMEOUTMSK_SHIFT                   3                                             /**< Shift value for USBAHB_TIMEOUTMSK           */
#define _USBAHB_DIEPMSK_TIMEOUTMSK_MASK                    0x8UL                                         /**< Bit mask for USBAHB_TIMEOUTMSK              */
#define _USBAHB_DIEPMSK_TIMEOUTMSK_DEFAULT                 0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DIEPMSK             */
#define USBAHB_DIEPMSK_TIMEOUTMSK_DEFAULT                  (_USBAHB_DIEPMSK_TIMEOUTMSK_DEFAULT << 3)     /**< Shifted mode DEFAULT for USBAHB_DIEPMSK     */
#define USBAHB_DIEPMSK_INTKNTXFEMPMSK                      (0x1UL << 4)                                  /**< IN token recvd when FIFO mt irq mask        */
#define _USBAHB_DIEPMSK_INTKNTXFEMPMSK_SHIFT               4                                             /**< Shift value for USBAHB_INTKNTXFEMPMSK       */
#define _USBAHB_DIEPMSK_INTKNTXFEMPMSK_MASK                0x10UL                                        /**< Bit mask for USBAHB_INTKNTXFEMPMSK          */
#define _USBAHB_DIEPMSK_INTKNTXFEMPMSK_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DIEPMSK             */
#define USBAHB_DIEPMSK_INTKNTXFEMPMSK_DEFAULT              (_USBAHB_DIEPMSK_INTKNTXFEMPMSK_DEFAULT << 4) /**< Shifted mode DEFAULT for USBAHB_DIEPMSK     */
#define USBAHB_DIEPMSK_INTKNEPMISMSK                       (0x1UL << 5)                                  /**< IN Token recvd with EP Mismatch Mask        */
#define _USBAHB_DIEPMSK_INTKNEPMISMSK_SHIFT                5                                             /**< Shift value for USBAHB_INTKNEPMISMSK        */
#define _USBAHB_DIEPMSK_INTKNEPMISMSK_MASK                 0x20UL                                        /**< Bit mask for USBAHB_INTKNEPMISMSK           */
#define _USBAHB_DIEPMSK_INTKNEPMISMSK_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DIEPMSK             */
#define USBAHB_DIEPMSK_INTKNEPMISMSK_DEFAULT               (_USBAHB_DIEPMSK_INTKNEPMISMSK_DEFAULT << 5)  /**< Shifted mode DEFAULT for USBAHB_DIEPMSK     */
#define USBAHB_DIEPMSK_INEPNAKEFFMSK                       (0x1UL << 6)                                  /**< IN Endpoint NAK Effective Mask              */
#define _USBAHB_DIEPMSK_INEPNAKEFFMSK_SHIFT                6                                             /**< Shift value for USBAHB_INEPNAKEFFMSK        */
#define _USBAHB_DIEPMSK_INEPNAKEFFMSK_MASK                 0x40UL                                        /**< Bit mask for USBAHB_INEPNAKEFFMSK           */
#define _USBAHB_DIEPMSK_INEPNAKEFFMSK_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DIEPMSK             */
#define USBAHB_DIEPMSK_INEPNAKEFFMSK_DEFAULT               (_USBAHB_DIEPMSK_INEPNAKEFFMSK_DEFAULT << 6)  /**< Shifted mode DEFAULT for USBAHB_DIEPMSK     */
#define USBAHB_DIEPMSK_TXFIFOUNDRNMSK                      (0x1UL << 8)                                  /**< FIFO Underrun Mask                          */
#define _USBAHB_DIEPMSK_TXFIFOUNDRNMSK_SHIFT               8                                             /**< Shift value for USBAHB_TXFIFOUNDRNMSK       */
#define _USBAHB_DIEPMSK_TXFIFOUNDRNMSK_MASK                0x100UL                                       /**< Bit mask for USBAHB_TXFIFOUNDRNMSK          */
#define _USBAHB_DIEPMSK_TXFIFOUNDRNMSK_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DIEPMSK             */
#define USBAHB_DIEPMSK_TXFIFOUNDRNMSK_DEFAULT              (_USBAHB_DIEPMSK_TXFIFOUNDRNMSK_DEFAULT << 8) /**< Shifted mode DEFAULT for USBAHB_DIEPMSK     */
#define USBAHB_DIEPMSK_NAKMSK                              (0x1UL << 13)                                 /**< NAK interrupt Mask                          */
#define _USBAHB_DIEPMSK_NAKMSK_SHIFT                       13                                            /**< Shift value for USBAHB_NAKMSK               */
#define _USBAHB_DIEPMSK_NAKMSK_MASK                        0x2000UL                                      /**< Bit mask for USBAHB_NAKMSK                  */
#define _USBAHB_DIEPMSK_NAKMSK_DEFAULT                     0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DIEPMSK             */
#define USBAHB_DIEPMSK_NAKMSK_DEFAULT                      (_USBAHB_DIEPMSK_NAKMSK_DEFAULT << 13)        /**< Shifted mode DEFAULT for USBAHB_DIEPMSK     */

/* Bit fields for USBAHB DOEPMSK */
#define _USBAHB_DOEPMSK_RESETVALUE                         0x00000000UL                                  /**< Default value for USBAHB_DOEPMSK            */
#define _USBAHB_DOEPMSK_MASK                               0x0000717FUL                                  /**< Mask for USBAHB_DOEPMSK                     */
#define USBAHB_DOEPMSK_XFERCOMPPLMSK                       (0x1UL << 0)                                  /**< Device OUT EP Common IRQ Mask Register      */
#define _USBAHB_DOEPMSK_XFERCOMPPLMSK_SHIFT                0                                             /**< Shift value for USBAHB_XFERCOMPPLMSK        */
#define _USBAHB_DOEPMSK_XFERCOMPPLMSK_MASK                 0x1UL                                         /**< Bit mask for USBAHB_XFERCOMPPLMSK           */
#define _USBAHB_DOEPMSK_XFERCOMPPLMSK_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_XFERCOMPPLMSK_DEFAULT               (_USBAHB_DOEPMSK_XFERCOMPPLMSK_DEFAULT << 0)  /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_EPDISBLDMSK                         (0x1UL << 1)                                  /**< Endpoint Disabled Interrupt Mask            */
#define _USBAHB_DOEPMSK_EPDISBLDMSK_SHIFT                  1                                             /**< Shift value for USBAHB_EPDISBLDMSK          */
#define _USBAHB_DOEPMSK_EPDISBLDMSK_MASK                   0x2UL                                         /**< Bit mask for USBAHB_EPDISBLDMSK             */
#define _USBAHB_DOEPMSK_EPDISBLDMSK_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_EPDISBLDMSK_DEFAULT                 (_USBAHB_DOEPMSK_EPDISBLDMSK_DEFAULT << 1)    /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_AHBERRMSK                           (0x1UL << 2)                                  /**< Ahb Error Mask                              */
#define _USBAHB_DOEPMSK_AHBERRMSK_SHIFT                    2                                             /**< Shift value for USBAHB_AHBERRMSK            */
#define _USBAHB_DOEPMSK_AHBERRMSK_MASK                     0x4UL                                         /**< Bit mask for USBAHB_AHBERRMSK               */
#define _USBAHB_DOEPMSK_AHBERRMSK_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_AHBERRMSK_DEFAULT                   (_USBAHB_DOEPMSK_AHBERRMSK_DEFAULT << 2)      /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_SETUPMSK                            (0x1UL << 3)                                  /**< Setup Phase Done Mask                       */
#define _USBAHB_DOEPMSK_SETUPMSK_SHIFT                     3                                             /**< Shift value for USBAHB_SETUPMSK             */
#define _USBAHB_DOEPMSK_SETUPMSK_MASK                      0x8UL                                         /**< Bit mask for USBAHB_SETUPMSK                */
#define _USBAHB_DOEPMSK_SETUPMSK_DEFAULT                   0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_SETUPMSK_DEFAULT                    (_USBAHB_DOEPMSK_SETUPMSK_DEFAULT << 3)       /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_OUTTKNEPDISMSK                      (0x1UL << 4)                                  /**< OUT Token recvd when EP Disabled Mask       */
#define _USBAHB_DOEPMSK_OUTTKNEPDISMSK_SHIFT               4                                             /**< Shift value for USBAHB_OUTTKNEPDISMSK       */
#define _USBAHB_DOEPMSK_OUTTKNEPDISMSK_MASK                0x10UL                                        /**< Bit mask for USBAHB_OUTTKNEPDISMSK          */
#define _USBAHB_DOEPMSK_OUTTKNEPDISMSK_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_OUTTKNEPDISMSK_DEFAULT              (_USBAHB_DOEPMSK_OUTTKNEPDISMSK_DEFAULT << 4) /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_STSPHSERCVDMSK                      (0x1UL << 5)                                  /**< Status Phase Received Mask                  */
#define _USBAHB_DOEPMSK_STSPHSERCVDMSK_SHIFT               5                                             /**< Shift value for USBAHB_STSPHSERCVDMSK       */
#define _USBAHB_DOEPMSK_STSPHSERCVDMSK_MASK                0x20UL                                        /**< Bit mask for USBAHB_STSPHSERCVDMSK          */
#define _USBAHB_DOEPMSK_STSPHSERCVDMSK_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_STSPHSERCVDMSK_DEFAULT              (_USBAHB_DOEPMSK_STSPHSERCVDMSK_DEFAULT << 5) /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_BACK2BACKSETUP                      (0x1UL << 6)                                  /**< Back-to-Back SETUP Packets recvd Mask       */
#define _USBAHB_DOEPMSK_BACK2BACKSETUP_SHIFT               6                                             /**< Shift value for USBAHB_BACK2BACKSETUP       */
#define _USBAHB_DOEPMSK_BACK2BACKSETUP_MASK                0x40UL                                        /**< Bit mask for USBAHB_BACK2BACKSETUP          */
#define _USBAHB_DOEPMSK_BACK2BACKSETUP_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_BACK2BACKSETUP_DEFAULT              (_USBAHB_DOEPMSK_BACK2BACKSETUP_DEFAULT << 6) /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_OUTPKTERRMSK                        (0x1UL << 8)                                  /**< OUT Packet Error Mask                       */
#define _USBAHB_DOEPMSK_OUTPKTERRMSK_SHIFT                 8                                             /**< Shift value for USBAHB_OUTPKTERRMSK         */
#define _USBAHB_DOEPMSK_OUTPKTERRMSK_MASK                  0x100UL                                       /**< Bit mask for USBAHB_OUTPKTERRMSK            */
#define _USBAHB_DOEPMSK_OUTPKTERRMSK_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_OUTPKTERRMSK_DEFAULT                (_USBAHB_DOEPMSK_OUTPKTERRMSK_DEFAULT << 8)   /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_BBLEERRMSK                          (0x1UL << 12)                                 /**< Babble Error Interrupt Mask                 */
#define _USBAHB_DOEPMSK_BBLEERRMSK_SHIFT                   12                                            /**< Shift value for USBAHB_BBLEERRMSK           */
#define _USBAHB_DOEPMSK_BBLEERRMSK_MASK                    0x1000UL                                      /**< Bit mask for USBAHB_BBLEERRMSK              */
#define _USBAHB_DOEPMSK_BBLEERRMSK_DEFAULT                 0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_BBLEERRMSK_DEFAULT                  (_USBAHB_DOEPMSK_BBLEERRMSK_DEFAULT << 12)    /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_NAKMSK                              (0x1UL << 13)                                 /**< NAK Interrupt Mask                          */
#define _USBAHB_DOEPMSK_NAKMSK_SHIFT                       13                                            /**< Shift value for USBAHB_NAKMSK               */
#define _USBAHB_DOEPMSK_NAKMSK_MASK                        0x2000UL                                      /**< Bit mask for USBAHB_NAKMSK                  */
#define _USBAHB_DOEPMSK_NAKMSK_DEFAULT                     0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_NAKMSK_DEFAULT                      (_USBAHB_DOEPMSK_NAKMSK_DEFAULT << 13)        /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */
#define USBAHB_DOEPMSK_NYETMSK                             (0x1UL << 14)                                 /**< NYET Interrupt Mask                         */
#define _USBAHB_DOEPMSK_NYETMSK_SHIFT                      14                                            /**< Shift value for USBAHB_NYETMSK              */
#define _USBAHB_DOEPMSK_NYETMSK_MASK                       0x4000UL                                      /**< Bit mask for USBAHB_NYETMSK                 */
#define _USBAHB_DOEPMSK_NYETMSK_DEFAULT                    0x00000000UL                                  /**< Mode DEFAULT for USBAHB_DOEPMSK             */
#define USBAHB_DOEPMSK_NYETMSK_DEFAULT                     (_USBAHB_DOEPMSK_NYETMSK_DEFAULT << 14)       /**< Shifted mode DEFAULT for USBAHB_DOEPMSK     */

/* Bit fields for USBAHB DAINT */
#define _USBAHB_DAINT_RESETVALUE                           0x00000000UL                            /**< Default value for USBAHB_DAINT              */
#define _USBAHB_DAINT_MASK                                 0x03FF03FFUL                            /**< Mask for USBAHB_DAINT                       */
#define USBAHB_DAINT_INEPINT0                              (0x1UL << 0)                            /**< EP0 IRQ                                     */
#define _USBAHB_DAINT_INEPINT0_SHIFT                       0                                       /**< Shift value for USBAHB_INEPINT0             */
#define _USBAHB_DAINT_INEPINT0_MASK                        0x1UL                                   /**< Bit mask for USBAHB_INEPINT0                */
#define _USBAHB_DAINT_INEPINT0_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT0_DEFAULT                      (_USBAHB_DAINT_INEPINT0_DEFAULT << 0)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_INEPINT1                              (0x1UL << 1)                            /**< EP1 IRQ                                     */
#define _USBAHB_DAINT_INEPINT1_SHIFT                       1                                       /**< Shift value for USBAHB_INEPINT1             */
#define _USBAHB_DAINT_INEPINT1_MASK                        0x2UL                                   /**< Bit mask for USBAHB_INEPINT1                */
#define _USBAHB_DAINT_INEPINT1_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT1_DEFAULT                      (_USBAHB_DAINT_INEPINT1_DEFAULT << 1)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_INEPINT2                              (0x1UL << 2)                            /**< EP2 IRQ                                     */
#define _USBAHB_DAINT_INEPINT2_SHIFT                       2                                       /**< Shift value for USBAHB_INEPINT2             */
#define _USBAHB_DAINT_INEPINT2_MASK                        0x4UL                                   /**< Bit mask for USBAHB_INEPINT2                */
#define _USBAHB_DAINT_INEPINT2_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT2_DEFAULT                      (_USBAHB_DAINT_INEPINT2_DEFAULT << 2)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_INEPINT3                              (0x1UL << 3)                            /**< EP3 IRQ                                     */
#define _USBAHB_DAINT_INEPINT3_SHIFT                       3                                       /**< Shift value for USBAHB_INEPINT3             */
#define _USBAHB_DAINT_INEPINT3_MASK                        0x8UL                                   /**< Bit mask for USBAHB_INEPINT3                */
#define _USBAHB_DAINT_INEPINT3_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT3_DEFAULT                      (_USBAHB_DAINT_INEPINT3_DEFAULT << 3)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_INEPINT4                              (0x1UL << 4)                            /**< EP4 IRQ                                     */
#define _USBAHB_DAINT_INEPINT4_SHIFT                       4                                       /**< Shift value for USBAHB_INEPINT4             */
#define _USBAHB_DAINT_INEPINT4_MASK                        0x10UL                                  /**< Bit mask for USBAHB_INEPINT4                */
#define _USBAHB_DAINT_INEPINT4_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT4_DEFAULT                      (_USBAHB_DAINT_INEPINT4_DEFAULT << 4)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_INEPINT5                              (0x1UL << 5)                            /**< EP5 IRQ                                     */
#define _USBAHB_DAINT_INEPINT5_SHIFT                       5                                       /**< Shift value for USBAHB_INEPINT5             */
#define _USBAHB_DAINT_INEPINT5_MASK                        0x20UL                                  /**< Bit mask for USBAHB_INEPINT5                */
#define _USBAHB_DAINT_INEPINT5_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT5_DEFAULT                      (_USBAHB_DAINT_INEPINT5_DEFAULT << 5)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_INEPINT6                              (0x1UL << 6)                            /**< EP6 IRQ                                     */
#define _USBAHB_DAINT_INEPINT6_SHIFT                       6                                       /**< Shift value for USBAHB_INEPINT6             */
#define _USBAHB_DAINT_INEPINT6_MASK                        0x40UL                                  /**< Bit mask for USBAHB_INEPINT6                */
#define _USBAHB_DAINT_INEPINT6_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT6_DEFAULT                      (_USBAHB_DAINT_INEPINT6_DEFAULT << 6)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_INEPINT7                              (0x1UL << 7)                            /**< EP7 IRQ                                     */
#define _USBAHB_DAINT_INEPINT7_SHIFT                       7                                       /**< Shift value for USBAHB_INEPINT7             */
#define _USBAHB_DAINT_INEPINT7_MASK                        0x80UL                                  /**< Bit mask for USBAHB_INEPINT7                */
#define _USBAHB_DAINT_INEPINT7_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT7_DEFAULT                      (_USBAHB_DAINT_INEPINT7_DEFAULT << 7)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_INEPINT8                              (0x1UL << 8)                            /**< EP8 IRQ                                     */
#define _USBAHB_DAINT_INEPINT8_SHIFT                       8                                       /**< Shift value for USBAHB_INEPINT8             */
#define _USBAHB_DAINT_INEPINT8_MASK                        0x100UL                                 /**< Bit mask for USBAHB_INEPINT8                */
#define _USBAHB_DAINT_INEPINT8_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT8_DEFAULT                      (_USBAHB_DAINT_INEPINT8_DEFAULT << 8)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_INEPINT9                              (0x1UL << 9)                            /**< EP9 IRQ                                     */
#define _USBAHB_DAINT_INEPINT9_SHIFT                       9                                       /**< Shift value for USBAHB_INEPINT9             */
#define _USBAHB_DAINT_INEPINT9_MASK                        0x200UL                                 /**< Bit mask for USBAHB_INEPINT9                */
#define _USBAHB_DAINT_INEPINT9_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_INEPINT9_DEFAULT                      (_USBAHB_DAINT_INEPINT9_DEFAULT << 9)   /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT0                             (0x1UL << 16)                           /**< EP0 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT0_SHIFT                      16                                      /**< Shift value for USBAHB_OUTEPINT0            */
#define _USBAHB_DAINT_OUTEPINT0_MASK                       0x10000UL                               /**< Bit mask for USBAHB_OUTEPINT0               */
#define _USBAHB_DAINT_OUTEPINT0_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT0_DEFAULT                     (_USBAHB_DAINT_OUTEPINT0_DEFAULT << 16) /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT1                             (0x1UL << 17)                           /**< EP1 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT1_SHIFT                      17                                      /**< Shift value for USBAHB_OUTEPINT1            */
#define _USBAHB_DAINT_OUTEPINT1_MASK                       0x20000UL                               /**< Bit mask for USBAHB_OUTEPINT1               */
#define _USBAHB_DAINT_OUTEPINT1_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT1_DEFAULT                     (_USBAHB_DAINT_OUTEPINT1_DEFAULT << 17) /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT2                             (0x1UL << 18)                           /**< EP2 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT2_SHIFT                      18                                      /**< Shift value for USBAHB_OUTEPINT2            */
#define _USBAHB_DAINT_OUTEPINT2_MASK                       0x40000UL                               /**< Bit mask for USBAHB_OUTEPINT2               */
#define _USBAHB_DAINT_OUTEPINT2_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT2_DEFAULT                     (_USBAHB_DAINT_OUTEPINT2_DEFAULT << 18) /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT3                             (0x1UL << 19)                           /**< EP3 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT3_SHIFT                      19                                      /**< Shift value for USBAHB_OUTEPINT3            */
#define _USBAHB_DAINT_OUTEPINT3_MASK                       0x80000UL                               /**< Bit mask for USBAHB_OUTEPINT3               */
#define _USBAHB_DAINT_OUTEPINT3_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT3_DEFAULT                     (_USBAHB_DAINT_OUTEPINT3_DEFAULT << 19) /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT4                             (0x1UL << 20)                           /**< EP4 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT4_SHIFT                      20                                      /**< Shift value for USBAHB_OUTEPINT4            */
#define _USBAHB_DAINT_OUTEPINT4_MASK                       0x100000UL                              /**< Bit mask for USBAHB_OUTEPINT4               */
#define _USBAHB_DAINT_OUTEPINT4_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT4_DEFAULT                     (_USBAHB_DAINT_OUTEPINT4_DEFAULT << 20) /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT5                             (0x1UL << 21)                           /**< EP5 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT5_SHIFT                      21                                      /**< Shift value for USBAHB_OUTEPINT5            */
#define _USBAHB_DAINT_OUTEPINT5_MASK                       0x200000UL                              /**< Bit mask for USBAHB_OUTEPINT5               */
#define _USBAHB_DAINT_OUTEPINT5_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT5_DEFAULT                     (_USBAHB_DAINT_OUTEPINT5_DEFAULT << 21) /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT6                             (0x1UL << 22)                           /**< EP6 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT6_SHIFT                      22                                      /**< Shift value for USBAHB_OUTEPINT6            */
#define _USBAHB_DAINT_OUTEPINT6_MASK                       0x400000UL                              /**< Bit mask for USBAHB_OUTEPINT6               */
#define _USBAHB_DAINT_OUTEPINT6_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT6_DEFAULT                     (_USBAHB_DAINT_OUTEPINT6_DEFAULT << 22) /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT7                             (0x1UL << 23)                           /**< EP7 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT7_SHIFT                      23                                      /**< Shift value for USBAHB_OUTEPINT7            */
#define _USBAHB_DAINT_OUTEPINT7_MASK                       0x800000UL                              /**< Bit mask for USBAHB_OUTEPINT7               */
#define _USBAHB_DAINT_OUTEPINT7_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT7_DEFAULT                     (_USBAHB_DAINT_OUTEPINT7_DEFAULT << 23) /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT8                             (0x1UL << 24)                           /**< EP8 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT8_SHIFT                      24                                      /**< Shift value for USBAHB_OUTEPINT8            */
#define _USBAHB_DAINT_OUTEPINT8_MASK                       0x1000000UL                             /**< Bit mask for USBAHB_OUTEPINT8               */
#define _USBAHB_DAINT_OUTEPINT8_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT8_DEFAULT                     (_USBAHB_DAINT_OUTEPINT8_DEFAULT << 24) /**< Shifted mode DEFAULT for USBAHB_DAINT       */
#define USBAHB_DAINT_OUTEPINT9                             (0x1UL << 25)                           /**< EP9 OUT IRQ                                 */
#define _USBAHB_DAINT_OUTEPINT9_SHIFT                      25                                      /**< Shift value for USBAHB_OUTEPINT9            */
#define _USBAHB_DAINT_OUTEPINT9_MASK                       0x2000000UL                             /**< Bit mask for USBAHB_OUTEPINT9               */
#define _USBAHB_DAINT_OUTEPINT9_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for USBAHB_DAINT               */
#define USBAHB_DAINT_OUTEPINT9_DEFAULT                     (_USBAHB_DAINT_OUTEPINT9_DEFAULT << 25) /**< Shifted mode DEFAULT for USBAHB_DAINT       */

/* Bit fields for USBAHB DAINTMSK */
#define _USBAHB_DAINTMSK_RESETVALUE                        0x00000000UL                               /**< Default value for USBAHB_DAINTMSK           */
#define _USBAHB_DAINTMSK_MASK                              0x03FF03FFUL                               /**< Mask for USBAHB_DAINTMSK                    */
#define USBAHB_DAINTMSK_INEPMSK0                           (0x1UL << 0)                               /**< IN EP MASK0                                 */
#define _USBAHB_DAINTMSK_INEPMSK0_SHIFT                    0                                          /**< Shift value for USBAHB_INEPMSK0             */
#define _USBAHB_DAINTMSK_INEPMSK0_MASK                     0x1UL                                      /**< Bit mask for USBAHB_INEPMSK0                */
#define _USBAHB_DAINTMSK_INEPMSK0_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK0_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK0_DEFAULT << 0)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_INEPMSK1                           (0x1UL << 1)                               /**< IN EP MASK1                                 */
#define _USBAHB_DAINTMSK_INEPMSK1_SHIFT                    1                                          /**< Shift value for USBAHB_INEPMSK1             */
#define _USBAHB_DAINTMSK_INEPMSK1_MASK                     0x2UL                                      /**< Bit mask for USBAHB_INEPMSK1                */
#define _USBAHB_DAINTMSK_INEPMSK1_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK1_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK1_DEFAULT << 1)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_INEPMSK2                           (0x1UL << 2)                               /**< IN EP MASK2                                 */
#define _USBAHB_DAINTMSK_INEPMSK2_SHIFT                    2                                          /**< Shift value for USBAHB_INEPMSK2             */
#define _USBAHB_DAINTMSK_INEPMSK2_MASK                     0x4UL                                      /**< Bit mask for USBAHB_INEPMSK2                */
#define _USBAHB_DAINTMSK_INEPMSK2_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK2_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK2_DEFAULT << 2)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_INEPMSK3                           (0x1UL << 3)                               /**< IN EP MASK3                                 */
#define _USBAHB_DAINTMSK_INEPMSK3_SHIFT                    3                                          /**< Shift value for USBAHB_INEPMSK3             */
#define _USBAHB_DAINTMSK_INEPMSK3_MASK                     0x8UL                                      /**< Bit mask for USBAHB_INEPMSK3                */
#define _USBAHB_DAINTMSK_INEPMSK3_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK3_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK3_DEFAULT << 3)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_INEPMSK4                           (0x1UL << 4)                               /**< IN EP MASK4                                 */
#define _USBAHB_DAINTMSK_INEPMSK4_SHIFT                    4                                          /**< Shift value for USBAHB_INEPMSK4             */
#define _USBAHB_DAINTMSK_INEPMSK4_MASK                     0x10UL                                     /**< Bit mask for USBAHB_INEPMSK4                */
#define _USBAHB_DAINTMSK_INEPMSK4_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK4_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK4_DEFAULT << 4)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_INEPMSK5                           (0x1UL << 5)                               /**< IN EP MASK5                                 */
#define _USBAHB_DAINTMSK_INEPMSK5_SHIFT                    5                                          /**< Shift value for USBAHB_INEPMSK5             */
#define _USBAHB_DAINTMSK_INEPMSK5_MASK                     0x20UL                                     /**< Bit mask for USBAHB_INEPMSK5                */
#define _USBAHB_DAINTMSK_INEPMSK5_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK5_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK5_DEFAULT << 5)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_INEPMSK6                           (0x1UL << 6)                               /**< IN EP MASK6                                 */
#define _USBAHB_DAINTMSK_INEPMSK6_SHIFT                    6                                          /**< Shift value for USBAHB_INEPMSK6             */
#define _USBAHB_DAINTMSK_INEPMSK6_MASK                     0x40UL                                     /**< Bit mask for USBAHB_INEPMSK6                */
#define _USBAHB_DAINTMSK_INEPMSK6_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK6_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK6_DEFAULT << 6)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_INEPMSK7                           (0x1UL << 7)                               /**< IN EP MASK7                                 */
#define _USBAHB_DAINTMSK_INEPMSK7_SHIFT                    7                                          /**< Shift value for USBAHB_INEPMSK7             */
#define _USBAHB_DAINTMSK_INEPMSK7_MASK                     0x80UL                                     /**< Bit mask for USBAHB_INEPMSK7                */
#define _USBAHB_DAINTMSK_INEPMSK7_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK7_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK7_DEFAULT << 7)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_INEPMSK8                           (0x1UL << 8)                               /**< IN EP MASK8                                 */
#define _USBAHB_DAINTMSK_INEPMSK8_SHIFT                    8                                          /**< Shift value for USBAHB_INEPMSK8             */
#define _USBAHB_DAINTMSK_INEPMSK8_MASK                     0x100UL                                    /**< Bit mask for USBAHB_INEPMSK8                */
#define _USBAHB_DAINTMSK_INEPMSK8_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK8_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK8_DEFAULT << 8)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_INEPMSK9                           (0x1UL << 9)                               /**< IN EP MASK9                                 */
#define _USBAHB_DAINTMSK_INEPMSK9_SHIFT                    9                                          /**< Shift value for USBAHB_INEPMSK9             */
#define _USBAHB_DAINTMSK_INEPMSK9_MASK                     0x200UL                                    /**< Bit mask for USBAHB_INEPMSK9                */
#define _USBAHB_DAINTMSK_INEPMSK9_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_INEPMSK9_DEFAULT                   (_USBAHB_DAINTMSK_INEPMSK9_DEFAULT << 9)   /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK0                          (0x1UL << 16)                              /**< OUT EP MASK0                                */
#define _USBAHB_DAINTMSK_OUTEPMSK0_SHIFT                   16                                         /**< Shift value for USBAHB_OUTEPMSK0            */
#define _USBAHB_DAINTMSK_OUTEPMSK0_MASK                    0x10000UL                                  /**< Bit mask for USBAHB_OUTEPMSK0               */
#define _USBAHB_DAINTMSK_OUTEPMSK0_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK0_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK0_DEFAULT << 16) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK1                          (0x1UL << 17)                              /**< OUT EP MASK1                                */
#define _USBAHB_DAINTMSK_OUTEPMSK1_SHIFT                   17                                         /**< Shift value for USBAHB_OUTEPMSK1            */
#define _USBAHB_DAINTMSK_OUTEPMSK1_MASK                    0x20000UL                                  /**< Bit mask for USBAHB_OUTEPMSK1               */
#define _USBAHB_DAINTMSK_OUTEPMSK1_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK1_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK1_DEFAULT << 17) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK2                          (0x1UL << 18)                              /**< OUT EP MASK2                                */
#define _USBAHB_DAINTMSK_OUTEPMSK2_SHIFT                   18                                         /**< Shift value for USBAHB_OUTEPMSK2            */
#define _USBAHB_DAINTMSK_OUTEPMSK2_MASK                    0x40000UL                                  /**< Bit mask for USBAHB_OUTEPMSK2               */
#define _USBAHB_DAINTMSK_OUTEPMSK2_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK2_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK2_DEFAULT << 18) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK3                          (0x1UL << 19)                              /**< OUT EP MASK3                                */
#define _USBAHB_DAINTMSK_OUTEPMSK3_SHIFT                   19                                         /**< Shift value for USBAHB_OUTEPMSK3            */
#define _USBAHB_DAINTMSK_OUTEPMSK3_MASK                    0x80000UL                                  /**< Bit mask for USBAHB_OUTEPMSK3               */
#define _USBAHB_DAINTMSK_OUTEPMSK3_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK3_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK3_DEFAULT << 19) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK4                          (0x1UL << 20)                              /**< OUT EP MASK4                                */
#define _USBAHB_DAINTMSK_OUTEPMSK4_SHIFT                   20                                         /**< Shift value for USBAHB_OUTEPMSK4            */
#define _USBAHB_DAINTMSK_OUTEPMSK4_MASK                    0x100000UL                                 /**< Bit mask for USBAHB_OUTEPMSK4               */
#define _USBAHB_DAINTMSK_OUTEPMSK4_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK4_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK4_DEFAULT << 20) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK5                          (0x1UL << 21)                              /**< OUT EP MASK5                                */
#define _USBAHB_DAINTMSK_OUTEPMSK5_SHIFT                   21                                         /**< Shift value for USBAHB_OUTEPMSK5            */
#define _USBAHB_DAINTMSK_OUTEPMSK5_MASK                    0x200000UL                                 /**< Bit mask for USBAHB_OUTEPMSK5               */
#define _USBAHB_DAINTMSK_OUTEPMSK5_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK5_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK5_DEFAULT << 21) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK6                          (0x1UL << 22)                              /**< OUT EP MASK6                                */
#define _USBAHB_DAINTMSK_OUTEPMSK6_SHIFT                   22                                         /**< Shift value for USBAHB_OUTEPMSK6            */
#define _USBAHB_DAINTMSK_OUTEPMSK6_MASK                    0x400000UL                                 /**< Bit mask for USBAHB_OUTEPMSK6               */
#define _USBAHB_DAINTMSK_OUTEPMSK6_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK6_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK6_DEFAULT << 22) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK7                          (0x1UL << 23)                              /**< OUT EP MASK7                                */
#define _USBAHB_DAINTMSK_OUTEPMSK7_SHIFT                   23                                         /**< Shift value for USBAHB_OUTEPMSK7            */
#define _USBAHB_DAINTMSK_OUTEPMSK7_MASK                    0x800000UL                                 /**< Bit mask for USBAHB_OUTEPMSK7               */
#define _USBAHB_DAINTMSK_OUTEPMSK7_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK7_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK7_DEFAULT << 23) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK8                          (0x1UL << 24)                              /**< OUT EP MASK8                                */
#define _USBAHB_DAINTMSK_OUTEPMSK8_SHIFT                   24                                         /**< Shift value for USBAHB_OUTEPMSK8            */
#define _USBAHB_DAINTMSK_OUTEPMSK8_MASK                    0x1000000UL                                /**< Bit mask for USBAHB_OUTEPMSK8               */
#define _USBAHB_DAINTMSK_OUTEPMSK8_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK8_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK8_DEFAULT << 24) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */
#define USBAHB_DAINTMSK_OUTEPMSK9                          (0x1UL << 25)                              /**< OUT EP MASK9                                */
#define _USBAHB_DAINTMSK_OUTEPMSK9_SHIFT                   25                                         /**< Shift value for USBAHB_OUTEPMSK9            */
#define _USBAHB_DAINTMSK_OUTEPMSK9_MASK                    0x2000000UL                                /**< Bit mask for USBAHB_OUTEPMSK9               */
#define _USBAHB_DAINTMSK_OUTEPMSK9_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for USBAHB_DAINTMSK            */
#define USBAHB_DAINTMSK_OUTEPMSK9_DEFAULT                  (_USBAHB_DAINTMSK_OUTEPMSK9_DEFAULT << 25) /**< Shifted mode DEFAULT for USBAHB_DAINTMSK    */

/* Bit fields for USBAHB DTHRCTL */
#define _USBAHB_DTHRCTL_RESETVALUE                         0x08100020UL                                /**< Default value for USBAHB_DTHRCTL            */
#define _USBAHB_DTHRCTL_MASK                               0x0BFF1FFFUL                                /**< Mask for USBAHB_DTHRCTL                     */
#define USBAHB_DTHRCTL_NONISOTHREN                         (0x1UL << 0)                                /**< Non-ISO IN EP Threshold Enable              */
#define _USBAHB_DTHRCTL_NONISOTHREN_SHIFT                  0                                           /**< Shift value for USBAHB_NONISOTHREN          */
#define _USBAHB_DTHRCTL_NONISOTHREN_MASK                   0x1UL                                       /**< Bit mask for USBAHB_NONISOTHREN             */
#define _USBAHB_DTHRCTL_NONISOTHREN_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for USBAHB_DTHRCTL             */
#define USBAHB_DTHRCTL_NONISOTHREN_DEFAULT                 (_USBAHB_DTHRCTL_NONISOTHREN_DEFAULT << 0)  /**< Shifted mode DEFAULT for USBAHB_DTHRCTL     */
#define USBAHB_DTHRCTL_ISOTHREN                            (0x1UL << 1)                                /**< ISO IN EP Threshold Enable                  */
#define _USBAHB_DTHRCTL_ISOTHREN_SHIFT                     1                                           /**< Shift value for USBAHB_ISOTHREN             */
#define _USBAHB_DTHRCTL_ISOTHREN_MASK                      0x2UL                                       /**< Bit mask for USBAHB_ISOTHREN                */
#define _USBAHB_DTHRCTL_ISOTHREN_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for USBAHB_DTHRCTL             */
#define USBAHB_DTHRCTL_ISOTHREN_DEFAULT                    (_USBAHB_DTHRCTL_ISOTHREN_DEFAULT << 1)     /**< Shifted mode DEFAULT for USBAHB_DTHRCTL     */
#define _USBAHB_DTHRCTL_TXTHRLEN_SHIFT                     2                                           /**< Shift value for USBAHB_TXTHRLEN             */
#define _USBAHB_DTHRCTL_TXTHRLEN_MASK                      0x7FCUL                                     /**< Bit mask for USBAHB_TXTHRLEN                */
#define _USBAHB_DTHRCTL_TXTHRLEN_DEFAULT                   0x00000008UL                                /**< Mode DEFAULT for USBAHB_DTHRCTL             */
#define USBAHB_DTHRCTL_TXTHRLEN_DEFAULT                    (_USBAHB_DTHRCTL_TXTHRLEN_DEFAULT << 2)     /**< Shifted mode DEFAULT for USBAHB_DTHRCTL     */
#define _USBAHB_DTHRCTL_AHBTHRRATIO_SHIFT                  11                                          /**< Shift value for USBAHB_AHBTHRRATIO          */
#define _USBAHB_DTHRCTL_AHBTHRRATIO_MASK                   0x1800UL                                    /**< Bit mask for USBAHB_AHBTHRRATIO             */
#define _USBAHB_DTHRCTL_AHBTHRRATIO_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for USBAHB_DTHRCTL             */
#define USBAHB_DTHRCTL_AHBTHRRATIO_DEFAULT                 (_USBAHB_DTHRCTL_AHBTHRRATIO_DEFAULT << 11) /**< Shifted mode DEFAULT for USBAHB_DTHRCTL     */
#define USBAHB_DTHRCTL_RXTHREN                             (0x1UL << 16)                               /**< Receive Threshold Enable                    */
#define _USBAHB_DTHRCTL_RXTHREN_SHIFT                      16                                          /**< Shift value for USBAHB_RXTHREN              */
#define _USBAHB_DTHRCTL_RXTHREN_MASK                       0x10000UL                                   /**< Bit mask for USBAHB_RXTHREN                 */
#define _USBAHB_DTHRCTL_RXTHREN_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for USBAHB_DTHRCTL             */
#define USBAHB_DTHRCTL_RXTHREN_DEFAULT                     (_USBAHB_DTHRCTL_RXTHREN_DEFAULT << 16)     /**< Shifted mode DEFAULT for USBAHB_DTHRCTL     */
#define _USBAHB_DTHRCTL_RXTHRLEN_SHIFT                     17                                          /**< Shift value for USBAHB_RXTHRLEN             */
#define _USBAHB_DTHRCTL_RXTHRLEN_MASK                      0x3FE0000UL                                 /**< Bit mask for USBAHB_RXTHRLEN                */
#define _USBAHB_DTHRCTL_RXTHRLEN_DEFAULT                   0x00000008UL                                /**< Mode DEFAULT for USBAHB_DTHRCTL             */
#define USBAHB_DTHRCTL_RXTHRLEN_DEFAULT                    (_USBAHB_DTHRCTL_RXTHRLEN_DEFAULT << 17)    /**< Shifted mode DEFAULT for USBAHB_DTHRCTL     */
#define USBAHB_DTHRCTL_ARBPRKEN                            (0x1UL << 27)                               /**< Arbiter Parking Enable                      */
#define _USBAHB_DTHRCTL_ARBPRKEN_SHIFT                     27                                          /**< Shift value for USBAHB_ARBPRKEN             */
#define _USBAHB_DTHRCTL_ARBPRKEN_MASK                      0x8000000UL                                 /**< Bit mask for USBAHB_ARBPRKEN                */
#define _USBAHB_DTHRCTL_ARBPRKEN_DEFAULT                   0x00000001UL                                /**< Mode DEFAULT for USBAHB_DTHRCTL             */
#define USBAHB_DTHRCTL_ARBPRKEN_DEFAULT                    (_USBAHB_DTHRCTL_ARBPRKEN_DEFAULT << 27)    /**< Shifted mode DEFAULT for USBAHB_DTHRCTL     */

/* Bit fields for USBAHB DIEPEMPMSK */
#define _USBAHB_DIEPEMPMSK_RESETVALUE                      0x00000000UL                                /**< Default value for USBAHB_DIEPEMPMSK         */
#define _USBAHB_DIEPEMPMSK_MASK                            0x0000FFFFUL                                /**< Mask for USBAHB_DIEPEMPMSK                  */
#define _USBAHB_DIEPEMPMSK_DIEPMPMSK_SHIFT                 0                                           /**< Shift value for USBAHB_DIEPMPMSK            */
#define _USBAHB_DIEPEMPMSK_DIEPMPMSK_MASK                  0xFFFFUL                                    /**< Bit mask for USBAHB_DIEPMPMSK               */
#define _USBAHB_DIEPEMPMSK_DIEPMPMSK_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEPEMPMSK          */
#define USBAHB_DIEPEMPMSK_DIEPMPMSK_DEFAULT                (_USBAHB_DIEPEMPMSK_DIEPMPMSK_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEPEMPMSK  */

/* Bit fields for USBAHB DIEP0CTL */
#define _USBAHB_DIEP0CTL_RESETVALUE                        0x00008000UL                              /**< Default value for USBAHB_DIEP0CTL           */
#define _USBAHB_DIEP0CTL_MASK                              0xCFEE8003UL                              /**< Mask for USBAHB_DIEP0CTL                    */
#define _USBAHB_DIEP0CTL_MPS_SHIFT                         0                                         /**< Shift value for USBAHB_MPS                  */
#define _USBAHB_DIEP0CTL_MPS_MASK                          0x3UL                                     /**< Bit mask for USBAHB_MPS                     */
#define _USBAHB_DIEP0CTL_MPS_DEFAULT                       0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_MPS_DEFAULT                        (_USBAHB_DIEP0CTL_MPS_DEFAULT << 0)       /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */
#define USBAHB_DIEP0CTL_USBACTEP                           (0x1UL << 15)                             /**< USB Active Endpoint                         */
#define _USBAHB_DIEP0CTL_USBACTEP_SHIFT                    15                                        /**< Shift value for USBAHB_USBACTEP             */
#define _USBAHB_DIEP0CTL_USBACTEP_MASK                     0x8000UL                                  /**< Bit mask for USBAHB_USBACTEP                */
#define _USBAHB_DIEP0CTL_USBACTEP_DEFAULT                  0x00000001UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_USBACTEP_DEFAULT                   (_USBAHB_DIEP0CTL_USBACTEP_DEFAULT << 15) /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */
#define USBAHB_DIEP0CTL_NAKSTS                             (0x1UL << 17)                             /**< NAK Status                                  */
#define _USBAHB_DIEP0CTL_NAKSTS_SHIFT                      17                                        /**< Shift value for USBAHB_NAKSTS               */
#define _USBAHB_DIEP0CTL_NAKSTS_MASK                       0x20000UL                                 /**< Bit mask for USBAHB_NAKSTS                  */
#define _USBAHB_DIEP0CTL_NAKSTS_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_NAKSTS_DEFAULT                     (_USBAHB_DIEP0CTL_NAKSTS_DEFAULT << 17)   /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */
#define _USBAHB_DIEP0CTL_EPTYPE_SHIFT                      18                                        /**< Shift value for USBAHB_EPTYPE               */
#define _USBAHB_DIEP0CTL_EPTYPE_MASK                       0xC0000UL                                 /**< Bit mask for USBAHB_EPTYPE                  */
#define _USBAHB_DIEP0CTL_EPTYPE_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_EPTYPE_DEFAULT                     (_USBAHB_DIEP0CTL_EPTYPE_DEFAULT << 18)   /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */
#define USBAHB_DIEP0CTL_STALL                              (0x1UL << 21)                             /**< Handshake                                   */
#define _USBAHB_DIEP0CTL_STALL_SHIFT                       21                                        /**< Shift value for USBAHB_STALL                */
#define _USBAHB_DIEP0CTL_STALL_MASK                        0x200000UL                                /**< Bit mask for USBAHB_STALL                   */
#define _USBAHB_DIEP0CTL_STALL_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_STALL_DEFAULT                      (_USBAHB_DIEP0CTL_STALL_DEFAULT << 21)    /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */
#define _USBAHB_DIEP0CTL_TXFNUM_SHIFT                      22                                        /**< Shift value for USBAHB_TXFNUM               */
#define _USBAHB_DIEP0CTL_TXFNUM_MASK                       0x3C00000UL                               /**< Bit mask for USBAHB_TXFNUM                  */
#define _USBAHB_DIEP0CTL_TXFNUM_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_TXFNUM_DEFAULT                     (_USBAHB_DIEP0CTL_TXFNUM_DEFAULT << 22)   /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */
#define USBAHB_DIEP0CTL_CNAK                               (0x1UL << 26)                             /**< Clear NAK                                   */
#define _USBAHB_DIEP0CTL_CNAK_SHIFT                        26                                        /**< Shift value for USBAHB_CNAK                 */
#define _USBAHB_DIEP0CTL_CNAK_MASK                         0x4000000UL                               /**< Bit mask for USBAHB_CNAK                    */
#define _USBAHB_DIEP0CTL_CNAK_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_CNAK_DEFAULT                       (_USBAHB_DIEP0CTL_CNAK_DEFAULT << 26)     /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */
#define USBAHB_DIEP0CTL_SNAK                               (0x1UL << 27)                             /**< Set NAK                                     */
#define _USBAHB_DIEP0CTL_SNAK_SHIFT                        27                                        /**< Shift value for USBAHB_SNAK                 */
#define _USBAHB_DIEP0CTL_SNAK_MASK                         0x8000000UL                               /**< Bit mask for USBAHB_SNAK                    */
#define _USBAHB_DIEP0CTL_SNAK_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_SNAK_DEFAULT                       (_USBAHB_DIEP0CTL_SNAK_DEFAULT << 27)     /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */
#define USBAHB_DIEP0CTL_EPDIS                              (0x1UL << 30)                             /**< Endpoint Disable                            */
#define _USBAHB_DIEP0CTL_EPDIS_SHIFT                       30                                        /**< Shift value for USBAHB_EPDIS                */
#define _USBAHB_DIEP0CTL_EPDIS_MASK                        0x40000000UL                              /**< Bit mask for USBAHB_EPDIS                   */
#define _USBAHB_DIEP0CTL_EPDIS_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_EPDIS_DEFAULT                      (_USBAHB_DIEP0CTL_EPDIS_DEFAULT << 30)    /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */
#define USBAHB_DIEP0CTL_EPENA                              (0x1UL << 31)                             /**< End point enable                            */
#define _USBAHB_DIEP0CTL_EPENA_SHIFT                       31                                        /**< Shift value for USBAHB_EPENA                */
#define _USBAHB_DIEP0CTL_EPENA_MASK                        0x80000000UL                              /**< Bit mask for USBAHB_EPENA                   */
#define _USBAHB_DIEP0CTL_EPENA_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0CTL            */
#define USBAHB_DIEP0CTL_EPENA_DEFAULT                      (_USBAHB_DIEP0CTL_EPENA_DEFAULT << 31)    /**< Shifted mode DEFAULT for USBAHB_DIEP0CTL    */

/* Bit fields for USBAHB DIEP0INT */
#define _USBAHB_DIEP0INT_RESETVALUE                        0x00000080UL                                /**< Default value for USBAHB_DIEP0INT           */
#define _USBAHB_DIEP0INT_MASK                              0x00007BFFUL                                /**< Mask for USBAHB_DIEP0INT                    */
#define USBAHB_DIEP0INT_XFERCOMPL                          (0x1UL << 0)                                /**< Transfer Completed Interrupt                */
#define _USBAHB_DIEP0INT_XFERCOMPL_SHIFT                   0                                           /**< Shift value for USBAHB_XFERCOMPL            */
#define _USBAHB_DIEP0INT_XFERCOMPL_MASK                    0x1UL                                       /**< Bit mask for USBAHB_XFERCOMPL               */
#define _USBAHB_DIEP0INT_XFERCOMPL_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_XFERCOMPL_DEFAULT                  (_USBAHB_DIEP0INT_XFERCOMPL_DEFAULT << 0)   /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_EPDISBLD                           (0x1UL << 1)                                /**< Endpoint Disabled Interrupt                 */
#define _USBAHB_DIEP0INT_EPDISBLD_SHIFT                    1                                           /**< Shift value for USBAHB_EPDISBLD             */
#define _USBAHB_DIEP0INT_EPDISBLD_MASK                     0x2UL                                       /**< Bit mask for USBAHB_EPDISBLD                */
#define _USBAHB_DIEP0INT_EPDISBLD_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_EPDISBLD_DEFAULT                   (_USBAHB_DIEP0INT_EPDISBLD_DEFAULT << 1)    /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_AHBERR                             (0x1UL << 2)                                /**< AHB Error                                   */
#define _USBAHB_DIEP0INT_AHBERR_SHIFT                      2                                           /**< Shift value for USBAHB_AHBERR               */
#define _USBAHB_DIEP0INT_AHBERR_MASK                       0x4UL                                       /**< Bit mask for USBAHB_AHBERR                  */
#define _USBAHB_DIEP0INT_AHBERR_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_AHBERR_DEFAULT                     (_USBAHB_DIEP0INT_AHBERR_DEFAULT << 2)      /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_TIMEOUT                            (0x1UL << 3)                                /**< Condition                                   */
#define _USBAHB_DIEP0INT_TIMEOUT_SHIFT                     3                                           /**< Shift value for USBAHB_TIMEOUT              */
#define _USBAHB_DIEP0INT_TIMEOUT_MASK                      0x8UL                                       /**< Bit mask for USBAHB_TIMEOUT                 */
#define _USBAHB_DIEP0INT_TIMEOUT_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_TIMEOUT_DEFAULT                    (_USBAHB_DIEP0INT_TIMEOUT_DEFAULT << 3)     /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_INTKNTXFEMP                        (0x1UL << 4)                                /**< In Token recvd when TXFIFO Empty            */
#define _USBAHB_DIEP0INT_INTKNTXFEMP_SHIFT                 4                                           /**< Shift value for USBAHB_INTKNTXFEMP          */
#define _USBAHB_DIEP0INT_INTKNTXFEMP_MASK                  0x10UL                                      /**< Bit mask for USBAHB_INTKNTXFEMP             */
#define _USBAHB_DIEP0INT_INTKNTXFEMP_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_INTKNTXFEMP_DEFAULT                (_USBAHB_DIEP0INT_INTKNTXFEMP_DEFAULT << 4) /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_INTKNEPMIS                         (0x1UL << 5)                                /**< IN Token recvd with EP Mismatch             */
#define _USBAHB_DIEP0INT_INTKNEPMIS_SHIFT                  5                                           /**< Shift value for USBAHB_INTKNEPMIS           */
#define _USBAHB_DIEP0INT_INTKNEPMIS_MASK                   0x20UL                                      /**< Bit mask for USBAHB_INTKNEPMIS              */
#define _USBAHB_DIEP0INT_INTKNEPMIS_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_INTKNEPMIS_DEFAULT                 (_USBAHB_DIEP0INT_INTKNEPMIS_DEFAULT << 5)  /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_INEPNAKEFF                         (0x1UL << 6)                                /**< In Endpoint NAK Effective                   */
#define _USBAHB_DIEP0INT_INEPNAKEFF_SHIFT                  6                                           /**< Shift value for USBAHB_INEPNAKEFF           */
#define _USBAHB_DIEP0INT_INEPNAKEFF_MASK                   0x40UL                                      /**< Bit mask for USBAHB_INEPNAKEFF              */
#define _USBAHB_DIEP0INT_INEPNAKEFF_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_INEPNAKEFF_DEFAULT                 (_USBAHB_DIEP0INT_INEPNAKEFF_DEFAULT << 6)  /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_TXFEMP                             (0x1UL << 7)                                /**< TX FIFO Empty                               */
#define _USBAHB_DIEP0INT_TXFEMP_SHIFT                      7                                           /**< Shift value for USBAHB_TXFEMP               */
#define _USBAHB_DIEP0INT_TXFEMP_MASK                       0x80UL                                      /**< Bit mask for USBAHB_TXFEMP                  */
#define _USBAHB_DIEP0INT_TXFEMP_DEFAULT                    0x00000001UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_TXFEMP_DEFAULT                     (_USBAHB_DIEP0INT_TXFEMP_DEFAULT << 7)      /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_TXFIFOUNDRN                        (0x1UL << 8)                                /**< FIFO Underrun                               */
#define _USBAHB_DIEP0INT_TXFIFOUNDRN_SHIFT                 8                                           /**< Shift value for USBAHB_TXFIFOUNDRN          */
#define _USBAHB_DIEP0INT_TXFIFOUNDRN_MASK                  0x100UL                                     /**< Bit mask for USBAHB_TXFIFOUNDRN             */
#define _USBAHB_DIEP0INT_TXFIFOUNDRN_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_TXFIFOUNDRN_DEFAULT                (_USBAHB_DIEP0INT_TXFIFOUNDRN_DEFAULT << 8) /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_BNAINTR                            (0x1UL << 9)                                /**< BNA(Buffer Not Available) IRQ               */
#define _USBAHB_DIEP0INT_BNAINTR_SHIFT                     9                                           /**< Shift value for USBAHB_BNAINTR              */
#define _USBAHB_DIEP0INT_BNAINTR_MASK                      0x200UL                                     /**< Bit mask for USBAHB_BNAINTR                 */
#define _USBAHB_DIEP0INT_BNAINTR_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_BNAINTR_DEFAULT                    (_USBAHB_DIEP0INT_BNAINTR_DEFAULT << 9)     /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_PKTDRPSTS                          (0x1UL << 11)                               /**< Packet Drop Status                          */
#define _USBAHB_DIEP0INT_PKTDRPSTS_SHIFT                   11                                          /**< Shift value for USBAHB_PKTDRPSTS            */
#define _USBAHB_DIEP0INT_PKTDRPSTS_MASK                    0x800UL                                     /**< Bit mask for USBAHB_PKTDRPSTS               */
#define _USBAHB_DIEP0INT_PKTDRPSTS_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_PKTDRPSTS_DEFAULT                  (_USBAHB_DIEP0INT_PKTDRPSTS_DEFAULT << 11)  /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_BBLEERR                            (0x1UL << 12)                               /**< Babble error                                */
#define _USBAHB_DIEP0INT_BBLEERR_SHIFT                     12                                          /**< Shift value for USBAHB_BBLEERR              */
#define _USBAHB_DIEP0INT_BBLEERR_MASK                      0x1000UL                                    /**< Bit mask for USBAHB_BBLEERR                 */
#define _USBAHB_DIEP0INT_BBLEERR_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_BBLEERR_DEFAULT                    (_USBAHB_DIEP0INT_BBLEERR_DEFAULT << 12)    /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_NAKINTRPT                          (0x1UL << 13)                               /**< NAK Interrupt                               */
#define _USBAHB_DIEP0INT_NAKINTRPT_SHIFT                   13                                          /**< Shift value for USBAHB_NAKINTRPT            */
#define _USBAHB_DIEP0INT_NAKINTRPT_MASK                    0x2000UL                                    /**< Bit mask for USBAHB_NAKINTRPT               */
#define _USBAHB_DIEP0INT_NAKINTRPT_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_NAKINTRPT_DEFAULT                  (_USBAHB_DIEP0INT_NAKINTRPT_DEFAULT << 13)  /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */
#define USBAHB_DIEP0INT_NYETINTRPT                         (0x1UL << 14)                               /**< NYET Interrupt                              */
#define _USBAHB_DIEP0INT_NYETINTRPT_SHIFT                  14                                          /**< Shift value for USBAHB_NYETINTRPT           */
#define _USBAHB_DIEP0INT_NYETINTRPT_MASK                   0x4000UL                                    /**< Bit mask for USBAHB_NYETINTRPT              */
#define _USBAHB_DIEP0INT_NYETINTRPT_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0INT            */
#define USBAHB_DIEP0INT_NYETINTRPT_DEFAULT                 (_USBAHB_DIEP0INT_NYETINTRPT_DEFAULT << 14) /**< Shifted mode DEFAULT for USBAHB_DIEP0INT    */

/* Bit fields for USBAHB DIEP0TSIZ */
#define _USBAHB_DIEP0TSIZ_RESETVALUE                       0x00000000UL                              /**< Default value for USBAHB_DIEP0TSIZ          */
#define _USBAHB_DIEP0TSIZ_MASK                             0x0018007FUL                              /**< Mask for USBAHB_DIEP0TSIZ                   */
#define _USBAHB_DIEP0TSIZ_XFERSIZE_SHIFT                   0                                         /**< Shift value for USBAHB_XFERSIZE             */
#define _USBAHB_DIEP0TSIZ_XFERSIZE_MASK                    0x7FUL                                    /**< Bit mask for USBAHB_XFERSIZE                */
#define _USBAHB_DIEP0TSIZ_XFERSIZE_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0TSIZ           */
#define USBAHB_DIEP0TSIZ_XFERSIZE_DEFAULT                  (_USBAHB_DIEP0TSIZ_XFERSIZE_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEP0TSIZ   */
#define _USBAHB_DIEP0TSIZ_PKTCNT_SHIFT                     19                                        /**< Shift value for USBAHB_PKTCNT               */
#define _USBAHB_DIEP0TSIZ_PKTCNT_MASK                      0x180000UL                                /**< Bit mask for USBAHB_PKTCNT                  */
#define _USBAHB_DIEP0TSIZ_PKTCNT_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for USBAHB_DIEP0TSIZ           */
#define USBAHB_DIEP0TSIZ_PKTCNT_DEFAULT                    (_USBAHB_DIEP0TSIZ_PKTCNT_DEFAULT << 19)  /**< Shifted mode DEFAULT for USBAHB_DIEP0TSIZ   */

/* Bit fields for USBAHB DIEP0DMAADDR */
#define _USBAHB_DIEP0DMAADDR_RESETVALUE                    0x00000000UL                                /**< Default value for USBAHB_DIEP0DMAADDR       */
#define _USBAHB_DIEP0DMAADDR_MASK                          0xFFFFFFFFUL                                /**< Mask for USBAHB_DIEP0DMAADDR                */
#define _USBAHB_DIEP0DMAADDR_DMAADDR_SHIFT                 0                                           /**< Shift value for USBAHB_DMAADDR              */
#define _USBAHB_DIEP0DMAADDR_DMAADDR_MASK                  0xFFFFFFFFUL                                /**< Bit mask for USBAHB_DMAADDR                 */
#define _USBAHB_DIEP0DMAADDR_DMAADDR_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for USBAHB_DIEP0DMAADDR        */
#define USBAHB_DIEP0DMAADDR_DMAADDR_DEFAULT                (_USBAHB_DIEP0DMAADDR_DMAADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEP0DMAADDR*/

/* Bit fields for USBAHB DIEP0TXFSTS */
#define _USBAHB_DIEP0TXFSTS_RESETVALUE                     0x00000300UL                                /**< Default value for USBAHB_DIEP0TXFSTS        */
#define _USBAHB_DIEP0TXFSTS_MASK                           0x0000FFFFUL                                /**< Mask for USBAHB_DIEP0TXFSTS                 */
#define _USBAHB_DIEP0TXFSTS_SPCAVAIL_SHIFT                 0                                           /**< Shift value for USBAHB_SPCAVAIL             */
#define _USBAHB_DIEP0TXFSTS_SPCAVAIL_MASK                  0xFFFFUL                                    /**< Bit mask for USBAHB_SPCAVAIL                */
#define _USBAHB_DIEP0TXFSTS_SPCAVAIL_DEFAULT               0x00000300UL                                /**< Mode DEFAULT for USBAHB_DIEP0TXFSTS         */
#define USBAHB_DIEP0TXFSTS_SPCAVAIL_DEFAULT                (_USBAHB_DIEP0TXFSTS_SPCAVAIL_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DIEP0TXFSTS */

/* Bit fields for USBAHB DEVINEP_CTL */
#define _USBAHB_DEVINEP_CTL_RESETVALUE                     0x00000000UL                                   /**< Default value for USBAHB_DEVINEP_CTL        */
#define _USBAHB_DEVINEP_CTL_MASK                           0xFFEF87FFUL                                   /**< Mask for USBAHB_DEVINEP_CTL                 */
#define _USBAHB_DEVINEP_CTL_MPS_SHIFT                      0                                              /**< Shift value for USBAHB_MPS                  */
#define _USBAHB_DEVINEP_CTL_MPS_MASK                       0x7FFUL                                        /**< Bit mask for USBAHB_MPS                     */
#define _USBAHB_DEVINEP_CTL_MPS_DEFAULT                    0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_MPS_DEFAULT                     (_USBAHB_DEVINEP_CTL_MPS_DEFAULT << 0)         /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_USBACTEP                        (0x1UL << 15)                                  /**< USB ACtive Endpoint                         */
#define _USBAHB_DEVINEP_CTL_USBACTEP_SHIFT                 15                                             /**< Shift value for USBAHB_USBACTEP             */
#define _USBAHB_DEVINEP_CTL_USBACTEP_MASK                  0x8000UL                                       /**< Bit mask for USBAHB_USBACTEP                */
#define _USBAHB_DEVINEP_CTL_USBACTEP_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_USBACTEP_DEFAULT                (_USBAHB_DEVINEP_CTL_USBACTEP_DEFAULT << 15)   /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_DPID                            (0x1UL << 16)                                  /**< Endpoint Data PID, EO_FrNum                 */
#define _USBAHB_DEVINEP_CTL_DPID_SHIFT                     16                                             /**< Shift value for USBAHB_DPID                 */
#define _USBAHB_DEVINEP_CTL_DPID_MASK                      0x10000UL                                      /**< Bit mask for USBAHB_DPID                    */
#define _USBAHB_DEVINEP_CTL_DPID_DEFAULT                   0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_DPID_DEFAULT                    (_USBAHB_DEVINEP_CTL_DPID_DEFAULT << 16)       /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_NAKSTS                          (0x1UL << 17)                                  /**< NAK STatus                                  */
#define _USBAHB_DEVINEP_CTL_NAKSTS_SHIFT                   17                                             /**< Shift value for USBAHB_NAKSTS               */
#define _USBAHB_DEVINEP_CTL_NAKSTS_MASK                    0x20000UL                                      /**< Bit mask for USBAHB_NAKSTS                  */
#define _USBAHB_DEVINEP_CTL_NAKSTS_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_NAKSTS_DEFAULT                  (_USBAHB_DEVINEP_CTL_NAKSTS_DEFAULT << 17)     /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define _USBAHB_DEVINEP_CTL_EPTYPE_SHIFT                   18                                             /**< Shift value for USBAHB_EPTYPE               */
#define _USBAHB_DEVINEP_CTL_EPTYPE_MASK                    0xC0000UL                                      /**< Bit mask for USBAHB_EPTYPE                  */
#define _USBAHB_DEVINEP_CTL_EPTYPE_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_EPTYPE_DEFAULT                  (_USBAHB_DEVINEP_CTL_EPTYPE_DEFAULT << 18)     /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_STALL                           (0x1UL << 21)                                  /**< Handshake                                   */
#define _USBAHB_DEVINEP_CTL_STALL_SHIFT                    21                                             /**< Shift value for USBAHB_STALL                */
#define _USBAHB_DEVINEP_CTL_STALL_MASK                     0x200000UL                                     /**< Bit mask for USBAHB_STALL                   */
#define _USBAHB_DEVINEP_CTL_STALL_DEFAULT                  0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_STALL_DEFAULT                   (_USBAHB_DEVINEP_CTL_STALL_DEFAULT << 21)      /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define _USBAHB_DEVINEP_CTL_TXFNUM_SHIFT                   22                                             /**< Shift value for USBAHB_TXFNUM               */
#define _USBAHB_DEVINEP_CTL_TXFNUM_MASK                    0x3C00000UL                                    /**< Bit mask for USBAHB_TXFNUM                  */
#define _USBAHB_DEVINEP_CTL_TXFNUM_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_TXFNUM_DEFAULT                  (_USBAHB_DEVINEP_CTL_TXFNUM_DEFAULT << 22)     /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_CNAK                            (0x1UL << 26)                                  /**< Clear NAK                                   */
#define _USBAHB_DEVINEP_CTL_CNAK_SHIFT                     26                                             /**< Shift value for USBAHB_CNAK                 */
#define _USBAHB_DEVINEP_CTL_CNAK_MASK                      0x4000000UL                                    /**< Bit mask for USBAHB_CNAK                    */
#define _USBAHB_DEVINEP_CTL_CNAK_DEFAULT                   0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_CNAK_DEFAULT                    (_USBAHB_DEVINEP_CTL_CNAK_DEFAULT << 26)       /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_SNAK                            (0x1UL << 27)                                  /**< Set NAK                                     */
#define _USBAHB_DEVINEP_CTL_SNAK_SHIFT                     27                                             /**< Shift value for USBAHB_SNAK                 */
#define _USBAHB_DEVINEP_CTL_SNAK_MASK                      0x8000000UL                                    /**< Bit mask for USBAHB_SNAK                    */
#define _USBAHB_DEVINEP_CTL_SNAK_DEFAULT                   0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_SNAK_DEFAULT                    (_USBAHB_DEVINEP_CTL_SNAK_DEFAULT << 27)       /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_SETD0PIDEF                      (0x1UL << 28)                                  /**< Set DATA0 PID / Even Frame                  */
#define _USBAHB_DEVINEP_CTL_SETD0PIDEF_SHIFT               28                                             /**< Shift value for USBAHB_SETD0PIDEF           */
#define _USBAHB_DEVINEP_CTL_SETD0PIDEF_MASK                0x10000000UL                                   /**< Bit mask for USBAHB_SETD0PIDEF              */
#define _USBAHB_DEVINEP_CTL_SETD0PIDEF_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_SETD0PIDEF_DEFAULT              (_USBAHB_DEVINEP_CTL_SETD0PIDEF_DEFAULT << 28) /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_SETD1PIDOF                      (0x1UL << 29)                                  /**< Set DATA1 PID / odd Frame                   */
#define _USBAHB_DEVINEP_CTL_SETD1PIDOF_SHIFT               29                                             /**< Shift value for USBAHB_SETD1PIDOF           */
#define _USBAHB_DEVINEP_CTL_SETD1PIDOF_MASK                0x20000000UL                                   /**< Bit mask for USBAHB_SETD1PIDOF              */
#define _USBAHB_DEVINEP_CTL_SETD1PIDOF_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_SETD1PIDOF_DEFAULT              (_USBAHB_DEVINEP_CTL_SETD1PIDOF_DEFAULT << 29) /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_EPDIS                           (0x1UL << 30)                                  /**< Endpoint Disable                            */
#define _USBAHB_DEVINEP_CTL_EPDIS_SHIFT                    30                                             /**< Shift value for USBAHB_EPDIS                */
#define _USBAHB_DEVINEP_CTL_EPDIS_MASK                     0x40000000UL                                   /**< Bit mask for USBAHB_EPDIS                   */
#define _USBAHB_DEVINEP_CTL_EPDIS_DEFAULT                  0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_EPDIS_DEFAULT                   (_USBAHB_DEVINEP_CTL_EPDIS_DEFAULT << 30)      /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */
#define USBAHB_DEVINEP_CTL_EPENA                           (0x1UL << 31)                                  /**< Endpoint enable                             */
#define _USBAHB_DEVINEP_CTL_EPENA_SHIFT                    31                                             /**< Shift value for USBAHB_EPENA                */
#define _USBAHB_DEVINEP_CTL_EPENA_MASK                     0x80000000UL                                   /**< Bit mask for USBAHB_EPENA                   */
#define _USBAHB_DEVINEP_CTL_EPENA_DEFAULT                  0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_CTL         */
#define USBAHB_DEVINEP_CTL_EPENA_DEFAULT                   (_USBAHB_DEVINEP_CTL_EPENA_DEFAULT << 31)      /**< Shifted mode DEFAULT for USBAHB_DEVINEP_CTL */

/* Bit fields for USBAHB DEVINEP_INT */
#define _USBAHB_DEVINEP_INT_RESETVALUE                     0x00000080UL                                   /**< Default value for USBAHB_DEVINEP_INT        */
#define _USBAHB_DEVINEP_INT_MASK                           0x00007BFFUL                                   /**< Mask for USBAHB_DEVINEP_INT                 */
#define USBAHB_DEVINEP_INT_XFERCOMPL                       (0x1UL << 0)                                   /**< Transfer Completed IRQ                      */
#define _USBAHB_DEVINEP_INT_XFERCOMPL_SHIFT                0                                              /**< Shift value for USBAHB_XFERCOMPL            */
#define _USBAHB_DEVINEP_INT_XFERCOMPL_MASK                 0x1UL                                          /**< Bit mask for USBAHB_XFERCOMPL               */
#define _USBAHB_DEVINEP_INT_XFERCOMPL_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_XFERCOMPL_DEFAULT               (_USBAHB_DEVINEP_INT_XFERCOMPL_DEFAULT << 0)   /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_EPDISBLD                        (0x1UL << 1)                                   /**< Endpoint Disabled IRQ                       */
#define _USBAHB_DEVINEP_INT_EPDISBLD_SHIFT                 1                                              /**< Shift value for USBAHB_EPDISBLD             */
#define _USBAHB_DEVINEP_INT_EPDISBLD_MASK                  0x2UL                                          /**< Bit mask for USBAHB_EPDISBLD                */
#define _USBAHB_DEVINEP_INT_EPDISBLD_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_EPDISBLD_DEFAULT                (_USBAHB_DEVINEP_INT_EPDISBLD_DEFAULT << 1)    /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_AHBERR                          (0x1UL << 2)                                   /**< AHB Error                                   */
#define _USBAHB_DEVINEP_INT_AHBERR_SHIFT                   2                                              /**< Shift value for USBAHB_AHBERR               */
#define _USBAHB_DEVINEP_INT_AHBERR_MASK                    0x4UL                                          /**< Bit mask for USBAHB_AHBERR                  */
#define _USBAHB_DEVINEP_INT_AHBERR_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_AHBERR_DEFAULT                  (_USBAHB_DEVINEP_INT_AHBERR_DEFAULT << 2)      /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_TIMEOUT                         (0x1UL << 3)                                   /**< COndition                                   */
#define _USBAHB_DEVINEP_INT_TIMEOUT_SHIFT                  3                                              /**< Shift value for USBAHB_TIMEOUT              */
#define _USBAHB_DEVINEP_INT_TIMEOUT_MASK                   0x8UL                                          /**< Bit mask for USBAHB_TIMEOUT                 */
#define _USBAHB_DEVINEP_INT_TIMEOUT_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_TIMEOUT_DEFAULT                 (_USBAHB_DEVINEP_INT_TIMEOUT_DEFAULT << 3)     /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_INTKNTXFEMP                     (0x1UL << 4)                                   /**< IN Token recvd When TxFIFO is Empty         */
#define _USBAHB_DEVINEP_INT_INTKNTXFEMP_SHIFT              4                                              /**< Shift value for USBAHB_INTKNTXFEMP          */
#define _USBAHB_DEVINEP_INT_INTKNTXFEMP_MASK               0x10UL                                         /**< Bit mask for USBAHB_INTKNTXFEMP             */
#define _USBAHB_DEVINEP_INT_INTKNTXFEMP_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_INTKNTXFEMP_DEFAULT             (_USBAHB_DEVINEP_INT_INTKNTXFEMP_DEFAULT << 4) /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_INTKNEPMIS                      (0x1UL << 5)                                   /**< IN Token recvd with EP Mismatch             */
#define _USBAHB_DEVINEP_INT_INTKNEPMIS_SHIFT               5                                              /**< Shift value for USBAHB_INTKNEPMIS           */
#define _USBAHB_DEVINEP_INT_INTKNEPMIS_MASK                0x20UL                                         /**< Bit mask for USBAHB_INTKNEPMIS              */
#define _USBAHB_DEVINEP_INT_INTKNEPMIS_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_INTKNEPMIS_DEFAULT              (_USBAHB_DEVINEP_INT_INTKNEPMIS_DEFAULT << 5)  /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_INEPNAKEFF                      (0x1UL << 6)                                   /**< IN Endpoint NAK Effective                   */
#define _USBAHB_DEVINEP_INT_INEPNAKEFF_SHIFT               6                                              /**< Shift value for USBAHB_INEPNAKEFF           */
#define _USBAHB_DEVINEP_INT_INEPNAKEFF_MASK                0x40UL                                         /**< Bit mask for USBAHB_INEPNAKEFF              */
#define _USBAHB_DEVINEP_INT_INEPNAKEFF_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_INEPNAKEFF_DEFAULT              (_USBAHB_DEVINEP_INT_INEPNAKEFF_DEFAULT << 6)  /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_TXFEMP                          (0x1UL << 7)                                   /**< TX FIFO Empty                               */
#define _USBAHB_DEVINEP_INT_TXFEMP_SHIFT                   7                                              /**< Shift value for USBAHB_TXFEMP               */
#define _USBAHB_DEVINEP_INT_TXFEMP_MASK                    0x80UL                                         /**< Bit mask for USBAHB_TXFEMP                  */
#define _USBAHB_DEVINEP_INT_TXFEMP_DEFAULT                 0x00000001UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_TXFEMP_DEFAULT                  (_USBAHB_DEVINEP_INT_TXFEMP_DEFAULT << 7)      /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_TXFIFOUNDRN                     (0x1UL << 8)                                   /**< FIFO Underrun                               */
#define _USBAHB_DEVINEP_INT_TXFIFOUNDRN_SHIFT              8                                              /**< Shift value for USBAHB_TXFIFOUNDRN          */
#define _USBAHB_DEVINEP_INT_TXFIFOUNDRN_MASK               0x100UL                                        /**< Bit mask for USBAHB_TXFIFOUNDRN             */
#define _USBAHB_DEVINEP_INT_TXFIFOUNDRN_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_TXFIFOUNDRN_DEFAULT             (_USBAHB_DEVINEP_INT_TXFIFOUNDRN_DEFAULT << 8) /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_BNAINTR                         (0x1UL << 9)                                   /**< BNA (Buffer Not Available) IRQ              */
#define _USBAHB_DEVINEP_INT_BNAINTR_SHIFT                  9                                              /**< Shift value for USBAHB_BNAINTR              */
#define _USBAHB_DEVINEP_INT_BNAINTR_MASK                   0x200UL                                        /**< Bit mask for USBAHB_BNAINTR                 */
#define _USBAHB_DEVINEP_INT_BNAINTR_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_BNAINTR_DEFAULT                 (_USBAHB_DEVINEP_INT_BNAINTR_DEFAULT << 9)     /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_PKTDRPSTS                       (0x1UL << 11)                                  /**< Packet Drop Status                          */
#define _USBAHB_DEVINEP_INT_PKTDRPSTS_SHIFT                11                                             /**< Shift value for USBAHB_PKTDRPSTS            */
#define _USBAHB_DEVINEP_INT_PKTDRPSTS_MASK                 0x800UL                                        /**< Bit mask for USBAHB_PKTDRPSTS               */
#define _USBAHB_DEVINEP_INT_PKTDRPSTS_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_PKTDRPSTS_DEFAULT               (_USBAHB_DEVINEP_INT_PKTDRPSTS_DEFAULT << 11)  /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_BBLEERR                         (0x1UL << 12)                                  /**< Babble Interrupt                            */
#define _USBAHB_DEVINEP_INT_BBLEERR_SHIFT                  12                                             /**< Shift value for USBAHB_BBLEERR              */
#define _USBAHB_DEVINEP_INT_BBLEERR_MASK                   0x1000UL                                       /**< Bit mask for USBAHB_BBLEERR                 */
#define _USBAHB_DEVINEP_INT_BBLEERR_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_BBLEERR_DEFAULT                 (_USBAHB_DEVINEP_INT_BBLEERR_DEFAULT << 12)    /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_NAKINTRPT                       (0x1UL << 13)                                  /**< NAK Interrupt                               */
#define _USBAHB_DEVINEP_INT_NAKINTRPT_SHIFT                13                                             /**< Shift value for USBAHB_NAKINTRPT            */
#define _USBAHB_DEVINEP_INT_NAKINTRPT_MASK                 0x2000UL                                       /**< Bit mask for USBAHB_NAKINTRPT               */
#define _USBAHB_DEVINEP_INT_NAKINTRPT_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_NAKINTRPT_DEFAULT               (_USBAHB_DEVINEP_INT_NAKINTRPT_DEFAULT << 13)  /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */
#define USBAHB_DEVINEP_INT_NYETINTRPT                      (0x1UL << 14)                                  /**< NYET Interrupt                              */
#define _USBAHB_DEVINEP_INT_NYETINTRPT_SHIFT               14                                             /**< Shift value for USBAHB_NYETINTRPT           */
#define _USBAHB_DEVINEP_INT_NYETINTRPT_MASK                0x4000UL                                       /**< Bit mask for USBAHB_NYETINTRPT              */
#define _USBAHB_DEVINEP_INT_NYETINTRPT_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_INT         */
#define USBAHB_DEVINEP_INT_NYETINTRPT_DEFAULT              (_USBAHB_DEVINEP_INT_NYETINTRPT_DEFAULT << 14) /**< Shifted mode DEFAULT for USBAHB_DEVINEP_INT */

/* Bit fields for USBAHB DEVINEP_TSIZ */
#define _USBAHB_DEVINEP_TSIZ_RESETVALUE                    0x00000000UL                                 /**< Default value for USBAHB_DEVINEP_TSIZ       */
#define _USBAHB_DEVINEP_TSIZ_MASK                          0x7FFFFFFFUL                                 /**< Mask for USBAHB_DEVINEP_TSIZ                */
#define _USBAHB_DEVINEP_TSIZ_XFERSIZE_SHIFT                0                                            /**< Shift value for USBAHB_XFERSIZE             */
#define _USBAHB_DEVINEP_TSIZ_XFERSIZE_MASK                 0x7FFFFUL                                    /**< Bit mask for USBAHB_XFERSIZE                */
#define _USBAHB_DEVINEP_TSIZ_XFERSIZE_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for USBAHB_DEVINEP_TSIZ        */
#define USBAHB_DEVINEP_TSIZ_XFERSIZE_DEFAULT               (_USBAHB_DEVINEP_TSIZ_XFERSIZE_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DEVINEP_TSIZ*/
#define _USBAHB_DEVINEP_TSIZ_PKTCNT_SHIFT                  19                                           /**< Shift value for USBAHB_PKTCNT               */
#define _USBAHB_DEVINEP_TSIZ_PKTCNT_MASK                   0x1FF80000UL                                 /**< Bit mask for USBAHB_PKTCNT                  */
#define _USBAHB_DEVINEP_TSIZ_PKTCNT_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for USBAHB_DEVINEP_TSIZ        */
#define USBAHB_DEVINEP_TSIZ_PKTCNT_DEFAULT                 (_USBAHB_DEVINEP_TSIZ_PKTCNT_DEFAULT << 19)  /**< Shifted mode DEFAULT for USBAHB_DEVINEP_TSIZ*/
#define _USBAHB_DEVINEP_TSIZ_MC_SHIFT                      29                                           /**< Shift value for USBAHB_MC                   */
#define _USBAHB_DEVINEP_TSIZ_MC_MASK                       0x60000000UL                                 /**< Bit mask for USBAHB_MC                      */
#define _USBAHB_DEVINEP_TSIZ_MC_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for USBAHB_DEVINEP_TSIZ        */
#define USBAHB_DEVINEP_TSIZ_MC_DEFAULT                     (_USBAHB_DEVINEP_TSIZ_MC_DEFAULT << 29)      /**< Shifted mode DEFAULT for USBAHB_DEVINEP_TSIZ*/

/* Bit fields for USBAHB DEVINEP_DMAADDR */
#define _USBAHB_DEVINEP_DMAADDR_RESETVALUE                 0x00000000UL                                   /**< Default value for USBAHB_DEVINEP_DMAADDR    */
#define _USBAHB_DEVINEP_DMAADDR_MASK                       0xFFFFFFFFUL                                   /**< Mask for USBAHB_DEVINEP_DMAADDR             */
#define _USBAHB_DEVINEP_DMAADDR_DMAADDR_SHIFT              0                                              /**< Shift value for USBAHB_DMAADDR              */
#define _USBAHB_DEVINEP_DMAADDR_DMAADDR_MASK               0xFFFFFFFFUL                                   /**< Bit mask for USBAHB_DMAADDR                 */
#define _USBAHB_DEVINEP_DMAADDR_DMAADDR_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DEVINEP_DMAADDR     */
#define USBAHB_DEVINEP_DMAADDR_DMAADDR_DEFAULT             (_USBAHB_DEVINEP_DMAADDR_DMAADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DEVINEP_DMAADDR*/

/* Bit fields for USBAHB DEVINEP_DTXFSTS */
#define _USBAHB_DEVINEP_DTXFSTS_RESETVALUE                 0x00000300UL                                           /**< Default value for USBAHB_DEVINEP_DTXFSTS    */
#define _USBAHB_DEVINEP_DTXFSTS_MASK                       0x0000FFFFUL                                           /**< Mask for USBAHB_DEVINEP_DTXFSTS             */
#define _USBAHB_DEVINEP_DTXFSTS_INEPTXFSPCAVAIL_SHIFT      0                                                      /**< Shift value for USBAHB_INEPTXFSPCAVAIL      */
#define _USBAHB_DEVINEP_DTXFSTS_INEPTXFSPCAVAIL_MASK       0xFFFFUL                                               /**< Bit mask for USBAHB_INEPTXFSPCAVAIL         */
#define _USBAHB_DEVINEP_DTXFSTS_INEPTXFSPCAVAIL_DEFAULT    0x00000300UL                                           /**< Mode DEFAULT for USBAHB_DEVINEP_DTXFSTS     */
#define USBAHB_DEVINEP_DTXFSTS_INEPTXFSPCAVAIL_DEFAULT     (_USBAHB_DEVINEP_DTXFSTS_INEPTXFSPCAVAIL_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DEVINEP_DTXFSTS*/

/* Bit fields for USBAHB DOEP0CTL */
#define _USBAHB_DOEP0CTL_RESETVALUE                        0x00000000UL                              /**< Default value for USBAHB_DOEP0CTL           */
#define _USBAHB_DOEP0CTL_MASK                              0xCC3E8003UL                              /**< Mask for USBAHB_DOEP0CTL                    */
#define _USBAHB_DOEP0CTL_MPS_SHIFT                         0                                         /**< Shift value for USBAHB_MPS                  */
#define _USBAHB_DOEP0CTL_MPS_MASK                          0x3UL                                     /**< Bit mask for USBAHB_MPS                     */
#define _USBAHB_DOEP0CTL_MPS_DEFAULT                       0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_MPS_DEFAULT                        (_USBAHB_DOEP0CTL_MPS_DEFAULT << 0)       /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */
#define USBAHB_DOEP0CTL_USBACTEP                           (0x1UL << 15)                             /**< US Active Endpoint                          */
#define _USBAHB_DOEP0CTL_USBACTEP_SHIFT                    15                                        /**< Shift value for USBAHB_USBACTEP             */
#define _USBAHB_DOEP0CTL_USBACTEP_MASK                     0x8000UL                                  /**< Bit mask for USBAHB_USBACTEP                */
#define _USBAHB_DOEP0CTL_USBACTEP_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_USBACTEP_DEFAULT                   (_USBAHB_DOEP0CTL_USBACTEP_DEFAULT << 15) /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */
#define USBAHB_DOEP0CTL_NAKSTS                             (0x1UL << 17)                             /**< NAK Status                                  */
#define _USBAHB_DOEP0CTL_NAKSTS_SHIFT                      17                                        /**< Shift value for USBAHB_NAKSTS               */
#define _USBAHB_DOEP0CTL_NAKSTS_MASK                       0x20000UL                                 /**< Bit mask for USBAHB_NAKSTS                  */
#define _USBAHB_DOEP0CTL_NAKSTS_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_NAKSTS_DEFAULT                     (_USBAHB_DOEP0CTL_NAKSTS_DEFAULT << 17)   /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */
#define _USBAHB_DOEP0CTL_EPTYPE_SHIFT                      18                                        /**< Shift value for USBAHB_EPTYPE               */
#define _USBAHB_DOEP0CTL_EPTYPE_MASK                       0xC0000UL                                 /**< Bit mask for USBAHB_EPTYPE                  */
#define _USBAHB_DOEP0CTL_EPTYPE_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_EPTYPE_DEFAULT                     (_USBAHB_DOEP0CTL_EPTYPE_DEFAULT << 18)   /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */
#define USBAHB_DOEP0CTL_SNP                                (0x1UL << 20)                             /**< Snoop mode                                  */
#define _USBAHB_DOEP0CTL_SNP_SHIFT                         20                                        /**< Shift value for USBAHB_SNP                  */
#define _USBAHB_DOEP0CTL_SNP_MASK                          0x100000UL                                /**< Bit mask for USBAHB_SNP                     */
#define _USBAHB_DOEP0CTL_SNP_DEFAULT                       0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_SNP_DEFAULT                        (_USBAHB_DOEP0CTL_SNP_DEFAULT << 20)      /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */
#define USBAHB_DOEP0CTL_STALL                              (0x1UL << 21)                             /**< Handshake                                   */
#define _USBAHB_DOEP0CTL_STALL_SHIFT                       21                                        /**< Shift value for USBAHB_STALL                */
#define _USBAHB_DOEP0CTL_STALL_MASK                        0x200000UL                                /**< Bit mask for USBAHB_STALL                   */
#define _USBAHB_DOEP0CTL_STALL_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_STALL_DEFAULT                      (_USBAHB_DOEP0CTL_STALL_DEFAULT << 21)    /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */
#define USBAHB_DOEP0CTL_CNAK                               (0x1UL << 26)                             /**< Clear NAK                                   */
#define _USBAHB_DOEP0CTL_CNAK_SHIFT                        26                                        /**< Shift value for USBAHB_CNAK                 */
#define _USBAHB_DOEP0CTL_CNAK_MASK                         0x4000000UL                               /**< Bit mask for USBAHB_CNAK                    */
#define _USBAHB_DOEP0CTL_CNAK_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_CNAK_DEFAULT                       (_USBAHB_DOEP0CTL_CNAK_DEFAULT << 26)     /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */
#define USBAHB_DOEP0CTL_SNAK                               (0x1UL << 27)                             /**< Set NAK                                     */
#define _USBAHB_DOEP0CTL_SNAK_SHIFT                        27                                        /**< Shift value for USBAHB_SNAK                 */
#define _USBAHB_DOEP0CTL_SNAK_MASK                         0x8000000UL                               /**< Bit mask for USBAHB_SNAK                    */
#define _USBAHB_DOEP0CTL_SNAK_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_SNAK_DEFAULT                       (_USBAHB_DOEP0CTL_SNAK_DEFAULT << 27)     /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */
#define USBAHB_DOEP0CTL_EPDIS                              (0x1UL << 30)                             /**< Endpoint Disable                            */
#define _USBAHB_DOEP0CTL_EPDIS_SHIFT                       30                                        /**< Shift value for USBAHB_EPDIS                */
#define _USBAHB_DOEP0CTL_EPDIS_MASK                        0x40000000UL                              /**< Bit mask for USBAHB_EPDIS                   */
#define _USBAHB_DOEP0CTL_EPDIS_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_EPDIS_DEFAULT                      (_USBAHB_DOEP0CTL_EPDIS_DEFAULT << 30)    /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */
#define USBAHB_DOEP0CTL_EPENA                              (0x1UL << 31)                             /**< Endpoint Enable                             */
#define _USBAHB_DOEP0CTL_EPENA_SHIFT                       31                                        /**< Shift value for USBAHB_EPENA                */
#define _USBAHB_DOEP0CTL_EPENA_MASK                        0x80000000UL                              /**< Bit mask for USBAHB_EPENA                   */
#define _USBAHB_DOEP0CTL_EPENA_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0CTL            */
#define USBAHB_DOEP0CTL_EPENA_DEFAULT                      (_USBAHB_DOEP0CTL_EPENA_DEFAULT << 31)    /**< Shifted mode DEFAULT for USBAHB_DOEP0CTL    */

/* Bit fields for USBAHB DOEP0INT */
#define _USBAHB_DOEP0INT_RESETVALUE                        0x00000000UL                                   /**< Default value for USBAHB_DOEP0INT           */
#define _USBAHB_DOEP0INT_MASK                              0x0000FB7FUL                                   /**< Mask for USBAHB_DOEP0INT                    */
#define USBAHB_DOEP0INT_XFERCOMPL                          (0x1UL << 0)                                   /**< Transfer Completed Interrupt                */
#define _USBAHB_DOEP0INT_XFERCOMPL_SHIFT                   0                                              /**< Shift value for USBAHB_XFERCOMPL            */
#define _USBAHB_DOEP0INT_XFERCOMPL_MASK                    0x1UL                                          /**< Bit mask for USBAHB_XFERCOMPL               */
#define _USBAHB_DOEP0INT_XFERCOMPL_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_XFERCOMPL_DEFAULT                  (_USBAHB_DOEP0INT_XFERCOMPL_DEFAULT << 0)      /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_EPDISBLD                           (0x1UL << 1)                                   /**< Endpoint Disabled Interrupt                 */
#define _USBAHB_DOEP0INT_EPDISBLD_SHIFT                    1                                              /**< Shift value for USBAHB_EPDISBLD             */
#define _USBAHB_DOEP0INT_EPDISBLD_MASK                     0x2UL                                          /**< Bit mask for USBAHB_EPDISBLD                */
#define _USBAHB_DOEP0INT_EPDISBLD_DEFAULT                  0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_EPDISBLD_DEFAULT                   (_USBAHB_DOEP0INT_EPDISBLD_DEFAULT << 1)       /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_AHBERR                             (0x1UL << 2)                                   /**< AHB Error                                   */
#define _USBAHB_DOEP0INT_AHBERR_SHIFT                      2                                              /**< Shift value for USBAHB_AHBERR               */
#define _USBAHB_DOEP0INT_AHBERR_MASK                       0x4UL                                          /**< Bit mask for USBAHB_AHBERR                  */
#define _USBAHB_DOEP0INT_AHBERR_DEFAULT                    0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_AHBERR_DEFAULT                     (_USBAHB_DOEP0INT_AHBERR_DEFAULT << 2)         /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_SETUP                              (0x1UL << 3)                                   /**< Phase Done                                  */
#define _USBAHB_DOEP0INT_SETUP_SHIFT                       3                                              /**< Shift value for USBAHB_SETUP                */
#define _USBAHB_DOEP0INT_SETUP_MASK                        0x8UL                                          /**< Bit mask for USBAHB_SETUP                   */
#define _USBAHB_DOEP0INT_SETUP_DEFAULT                     0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_SETUP_DEFAULT                      (_USBAHB_DOEP0INT_SETUP_DEFAULT << 3)          /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_OUTTKNEPDIS                        (0x1UL << 4)                                   /**< OUT Token recvd When EP Disabled            */
#define _USBAHB_DOEP0INT_OUTTKNEPDIS_SHIFT                 4                                              /**< Shift value for USBAHB_OUTTKNEPDIS          */
#define _USBAHB_DOEP0INT_OUTTKNEPDIS_MASK                  0x10UL                                         /**< Bit mask for USBAHB_OUTTKNEPDIS             */
#define _USBAHB_DOEP0INT_OUTTKNEPDIS_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_OUTTKNEPDIS_DEFAULT                (_USBAHB_DOEP0INT_OUTTKNEPDIS_DEFAULT << 4)    /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_STSPHSERCVD                        (0x1UL << 5)                                   /**< Status Phase recvd for ctrl Write           */
#define _USBAHB_DOEP0INT_STSPHSERCVD_SHIFT                 5                                              /**< Shift value for USBAHB_STSPHSERCVD          */
#define _USBAHB_DOEP0INT_STSPHSERCVD_MASK                  0x20UL                                         /**< Bit mask for USBAHB_STSPHSERCVD             */
#define _USBAHB_DOEP0INT_STSPHSERCVD_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_STSPHSERCVD_DEFAULT                (_USBAHB_DOEP0INT_STSPHSERCVD_DEFAULT << 5)    /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_BACK2BACKSETUP                     (0x1UL << 6)                                   /**< Back-to-Back SETUP Packets recvd            */
#define _USBAHB_DOEP0INT_BACK2BACKSETUP_SHIFT              6                                              /**< Shift value for USBAHB_BACK2BACKSETUP       */
#define _USBAHB_DOEP0INT_BACK2BACKSETUP_MASK               0x40UL                                         /**< Bit mask for USBAHB_BACK2BACKSETUP          */
#define _USBAHB_DOEP0INT_BACK2BACKSETUP_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_BACK2BACKSETUP_DEFAULT             (_USBAHB_DOEP0INT_BACK2BACKSETUP_DEFAULT << 6) /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_OUTPKTERR                          (0x1UL << 8)                                   /**< OUT Packet Error                            */
#define _USBAHB_DOEP0INT_OUTPKTERR_SHIFT                   8                                              /**< Shift value for USBAHB_OUTPKTERR            */
#define _USBAHB_DOEP0INT_OUTPKTERR_MASK                    0x100UL                                        /**< Bit mask for USBAHB_OUTPKTERR               */
#define _USBAHB_DOEP0INT_OUTPKTERR_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_OUTPKTERR_DEFAULT                  (_USBAHB_DOEP0INT_OUTPKTERR_DEFAULT << 8)      /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_BNAINTR                            (0x1UL << 9)                                   /**< BNA (Buffer Not Available) IRQ              */
#define _USBAHB_DOEP0INT_BNAINTR_SHIFT                     9                                              /**< Shift value for USBAHB_BNAINTR              */
#define _USBAHB_DOEP0INT_BNAINTR_MASK                      0x200UL                                        /**< Bit mask for USBAHB_BNAINTR                 */
#define _USBAHB_DOEP0INT_BNAINTR_DEFAULT                   0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_BNAINTR_DEFAULT                    (_USBAHB_DOEP0INT_BNAINTR_DEFAULT << 9)        /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_PKTDRPSTS                          (0x1UL << 11)                                  /**< Packet Drop Status                          */
#define _USBAHB_DOEP0INT_PKTDRPSTS_SHIFT                   11                                             /**< Shift value for USBAHB_PKTDRPSTS            */
#define _USBAHB_DOEP0INT_PKTDRPSTS_MASK                    0x800UL                                        /**< Bit mask for USBAHB_PKTDRPSTS               */
#define _USBAHB_DOEP0INT_PKTDRPSTS_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_PKTDRPSTS_DEFAULT                  (_USBAHB_DOEP0INT_PKTDRPSTS_DEFAULT << 11)     /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_BBLEERR                            (0x1UL << 12)                                  /**< Babble Interrupt                            */
#define _USBAHB_DOEP0INT_BBLEERR_SHIFT                     12                                             /**< Shift value for USBAHB_BBLEERR              */
#define _USBAHB_DOEP0INT_BBLEERR_MASK                      0x1000UL                                       /**< Bit mask for USBAHB_BBLEERR                 */
#define _USBAHB_DOEP0INT_BBLEERR_DEFAULT                   0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_BBLEERR_DEFAULT                    (_USBAHB_DOEP0INT_BBLEERR_DEFAULT << 12)       /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_NAKINTRPT                          (0x1UL << 13)                                  /**< NAK Interrupt                               */
#define _USBAHB_DOEP0INT_NAKINTRPT_SHIFT                   13                                             /**< Shift value for USBAHB_NAKINTRPT            */
#define _USBAHB_DOEP0INT_NAKINTRPT_MASK                    0x2000UL                                       /**< Bit mask for USBAHB_NAKINTRPT               */
#define _USBAHB_DOEP0INT_NAKINTRPT_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_NAKINTRPT_DEFAULT                  (_USBAHB_DOEP0INT_NAKINTRPT_DEFAULT << 13)     /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_NYETINTRPT                         (0x1UL << 14)                                  /**< NYET Interrupt                              */
#define _USBAHB_DOEP0INT_NYETINTRPT_SHIFT                  14                                             /**< Shift value for USBAHB_NYETINTRPT           */
#define _USBAHB_DOEP0INT_NYETINTRPT_MASK                   0x4000UL                                       /**< Bit mask for USBAHB_NYETINTRPT              */
#define _USBAHB_DOEP0INT_NYETINTRPT_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_NYETINTRPT_DEFAULT                 (_USBAHB_DOEP0INT_NYETINTRPT_DEFAULT << 14)    /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */
#define USBAHB_DOEP0INT_STUPPKTRCVD                        (0x1UL << 15)                                  /**< Setup Packet Received                       */
#define _USBAHB_DOEP0INT_STUPPKTRCVD_SHIFT                 15                                             /**< Shift value for USBAHB_STUPPKTRCVD          */
#define _USBAHB_DOEP0INT_STUPPKTRCVD_MASK                  0x8000UL                                       /**< Bit mask for USBAHB_STUPPKTRCVD             */
#define _USBAHB_DOEP0INT_STUPPKTRCVD_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for USBAHB_DOEP0INT            */
#define USBAHB_DOEP0INT_STUPPKTRCVD_DEFAULT                (_USBAHB_DOEP0INT_STUPPKTRCVD_DEFAULT << 15)   /**< Shifted mode DEFAULT for USBAHB_DOEP0INT    */

/* Bit fields for USBAHB DOEP0TSIZ */
#define _USBAHB_DOEP0TSIZ_RESETVALUE                       0x00000000UL                              /**< Default value for USBAHB_DOEP0TSIZ          */
#define _USBAHB_DOEP0TSIZ_MASK                             0x6008007FUL                              /**< Mask for USBAHB_DOEP0TSIZ                   */
#define _USBAHB_DOEP0TSIZ_XFERSIZE_SHIFT                   0                                         /**< Shift value for USBAHB_XFERSIZE             */
#define _USBAHB_DOEP0TSIZ_XFERSIZE_MASK                    0x7FUL                                    /**< Bit mask for USBAHB_XFERSIZE                */
#define _USBAHB_DOEP0TSIZ_XFERSIZE_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0TSIZ           */
#define USBAHB_DOEP0TSIZ_XFERSIZE_DEFAULT                  (_USBAHB_DOEP0TSIZ_XFERSIZE_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DOEP0TSIZ   */
#define USBAHB_DOEP0TSIZ_PKTCNT                            (0x1UL << 19)                             /**< Packet Count                                */
#define _USBAHB_DOEP0TSIZ_PKTCNT_SHIFT                     19                                        /**< Shift value for USBAHB_PKTCNT               */
#define _USBAHB_DOEP0TSIZ_PKTCNT_MASK                      0x80000UL                                 /**< Bit mask for USBAHB_PKTCNT                  */
#define _USBAHB_DOEP0TSIZ_PKTCNT_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0TSIZ           */
#define USBAHB_DOEP0TSIZ_PKTCNT_DEFAULT                    (_USBAHB_DOEP0TSIZ_PKTCNT_DEFAULT << 19)  /**< Shifted mode DEFAULT for USBAHB_DOEP0TSIZ   */
#define _USBAHB_DOEP0TSIZ_SUPCNT_SHIFT                     29                                        /**< Shift value for USBAHB_SUPCNT               */
#define _USBAHB_DOEP0TSIZ_SUPCNT_MASK                      0x60000000UL                              /**< Bit mask for USBAHB_SUPCNT                  */
#define _USBAHB_DOEP0TSIZ_SUPCNT_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for USBAHB_DOEP0TSIZ           */
#define USBAHB_DOEP0TSIZ_SUPCNT_DEFAULT                    (_USBAHB_DOEP0TSIZ_SUPCNT_DEFAULT << 29)  /**< Shifted mode DEFAULT for USBAHB_DOEP0TSIZ   */

/* Bit fields for USBAHB DOEP0DMAADDR */
#define _USBAHB_DOEP0DMAADDR_RESETVALUE                    0x00000000UL                                /**< Default value for USBAHB_DOEP0DMAADDR       */
#define _USBAHB_DOEP0DMAADDR_MASK                          0xFFFFFFFFUL                                /**< Mask for USBAHB_DOEP0DMAADDR                */
#define _USBAHB_DOEP0DMAADDR_DMAADDR_SHIFT                 0                                           /**< Shift value for USBAHB_DMAADDR              */
#define _USBAHB_DOEP0DMAADDR_DMAADDR_MASK                  0xFFFFFFFFUL                                /**< Bit mask for USBAHB_DMAADDR                 */
#define _USBAHB_DOEP0DMAADDR_DMAADDR_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for USBAHB_DOEP0DMAADDR        */
#define USBAHB_DOEP0DMAADDR_DMAADDR_DEFAULT                (_USBAHB_DOEP0DMAADDR_DMAADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DOEP0DMAADDR*/

/* Bit fields for USBAHB DEVOUTEP_CTL */
#define _USBAHB_DEVOUTEP_CTL_RESETVALUE                    0x00000000UL                                    /**< Default value for USBAHB_DEVOUTEP_CTL       */
#define _USBAHB_DEVOUTEP_CTL_MASK                          0xFC3F87FFUL                                    /**< Mask for USBAHB_DEVOUTEP_CTL                */
#define _USBAHB_DEVOUTEP_CTL_MPS_SHIFT                     0                                               /**< Shift value for USBAHB_MPS                  */
#define _USBAHB_DEVOUTEP_CTL_MPS_MASK                      0x7FFUL                                         /**< Bit mask for USBAHB_MPS                     */
#define _USBAHB_DEVOUTEP_CTL_MPS_DEFAULT                   0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_MPS_DEFAULT                    (_USBAHB_DEVOUTEP_CTL_MPS_DEFAULT << 0)         /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_USBACTEP                       (0x1UL << 15)                                   /**< USB Active Endpoint                         */
#define _USBAHB_DEVOUTEP_CTL_USBACTEP_SHIFT                15                                              /**< Shift value for USBAHB_USBACTEP             */
#define _USBAHB_DEVOUTEP_CTL_USBACTEP_MASK                 0x8000UL                                        /**< Bit mask for USBAHB_USBACTEP                */
#define _USBAHB_DEVOUTEP_CTL_USBACTEP_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_USBACTEP_DEFAULT               (_USBAHB_DEVOUTEP_CTL_USBACTEP_DEFAULT << 15)   /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_DPID                           (0x1UL << 16)                                   /**< Endpoint Data PID                           */
#define _USBAHB_DEVOUTEP_CTL_DPID_SHIFT                    16                                              /**< Shift value for USBAHB_DPID                 */
#define _USBAHB_DEVOUTEP_CTL_DPID_MASK                     0x10000UL                                       /**< Bit mask for USBAHB_DPID                    */
#define _USBAHB_DEVOUTEP_CTL_DPID_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_DPID_DEFAULT                   (_USBAHB_DEVOUTEP_CTL_DPID_DEFAULT << 16)       /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_NAKSTS                         (0x1UL << 17)                                   /**< NAK Status                                  */
#define _USBAHB_DEVOUTEP_CTL_NAKSTS_SHIFT                  17                                              /**< Shift value for USBAHB_NAKSTS               */
#define _USBAHB_DEVOUTEP_CTL_NAKSTS_MASK                   0x20000UL                                       /**< Bit mask for USBAHB_NAKSTS                  */
#define _USBAHB_DEVOUTEP_CTL_NAKSTS_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_NAKSTS_DEFAULT                 (_USBAHB_DEVOUTEP_CTL_NAKSTS_DEFAULT << 17)     /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define _USBAHB_DEVOUTEP_CTL_EPTYPE_SHIFT                  18                                              /**< Shift value for USBAHB_EPTYPE               */
#define _USBAHB_DEVOUTEP_CTL_EPTYPE_MASK                   0xC0000UL                                       /**< Bit mask for USBAHB_EPTYPE                  */
#define _USBAHB_DEVOUTEP_CTL_EPTYPE_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_EPTYPE_DEFAULT                 (_USBAHB_DEVOUTEP_CTL_EPTYPE_DEFAULT << 18)     /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_SNP                            (0x1UL << 20)                                   /**< Snoop Mode                                  */
#define _USBAHB_DEVOUTEP_CTL_SNP_SHIFT                     20                                              /**< Shift value for USBAHB_SNP                  */
#define _USBAHB_DEVOUTEP_CTL_SNP_MASK                      0x100000UL                                      /**< Bit mask for USBAHB_SNP                     */
#define _USBAHB_DEVOUTEP_CTL_SNP_DEFAULT                   0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_SNP_DEFAULT                    (_USBAHB_DEVOUTEP_CTL_SNP_DEFAULT << 20)        /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_STALL                          (0x1UL << 21)                                   /**< Handshake                                   */
#define _USBAHB_DEVOUTEP_CTL_STALL_SHIFT                   21                                              /**< Shift value for USBAHB_STALL                */
#define _USBAHB_DEVOUTEP_CTL_STALL_MASK                    0x200000UL                                      /**< Bit mask for USBAHB_STALL                   */
#define _USBAHB_DEVOUTEP_CTL_STALL_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_STALL_DEFAULT                  (_USBAHB_DEVOUTEP_CTL_STALL_DEFAULT << 21)      /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_CNAK                           (0x1UL << 26)                                   /**< Clear NAK                                   */
#define _USBAHB_DEVOUTEP_CTL_CNAK_SHIFT                    26                                              /**< Shift value for USBAHB_CNAK                 */
#define _USBAHB_DEVOUTEP_CTL_CNAK_MASK                     0x4000000UL                                     /**< Bit mask for USBAHB_CNAK                    */
#define _USBAHB_DEVOUTEP_CTL_CNAK_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_CNAK_DEFAULT                   (_USBAHB_DEVOUTEP_CTL_CNAK_DEFAULT << 26)       /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_SNAK                           (0x1UL << 27)                                   /**< Set NAK                                     */
#define _USBAHB_DEVOUTEP_CTL_SNAK_SHIFT                    27                                              /**< Shift value for USBAHB_SNAK                 */
#define _USBAHB_DEVOUTEP_CTL_SNAK_MASK                     0x8000000UL                                     /**< Bit mask for USBAHB_SNAK                    */
#define _USBAHB_DEVOUTEP_CTL_SNAK_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_SNAK_DEFAULT                   (_USBAHB_DEVOUTEP_CTL_SNAK_DEFAULT << 27)       /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_SETD0PIDEF                     (0x1UL << 28)                                   /**< Set DATA0 PID, even fr                      */
#define _USBAHB_DEVOUTEP_CTL_SETD0PIDEF_SHIFT              28                                              /**< Shift value for USBAHB_SETD0PIDEF           */
#define _USBAHB_DEVOUTEP_CTL_SETD0PIDEF_MASK               0x10000000UL                                    /**< Bit mask for USBAHB_SETD0PIDEF              */
#define _USBAHB_DEVOUTEP_CTL_SETD0PIDEF_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_SETD0PIDEF_DEFAULT             (_USBAHB_DEVOUTEP_CTL_SETD0PIDEF_DEFAULT << 28) /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_SETD1PIDOF                     (0x1UL << 29)                                   /**< Set DATA1 PID, odd fr                       */
#define _USBAHB_DEVOUTEP_CTL_SETD1PIDOF_SHIFT              29                                              /**< Shift value for USBAHB_SETD1PIDOF           */
#define _USBAHB_DEVOUTEP_CTL_SETD1PIDOF_MASK               0x20000000UL                                    /**< Bit mask for USBAHB_SETD1PIDOF              */
#define _USBAHB_DEVOUTEP_CTL_SETD1PIDOF_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_SETD1PIDOF_DEFAULT             (_USBAHB_DEVOUTEP_CTL_SETD1PIDOF_DEFAULT << 29) /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_EPDIS                          (0x1UL << 30)                                   /**< Endpoint Disable                            */
#define _USBAHB_DEVOUTEP_CTL_EPDIS_SHIFT                   30                                              /**< Shift value for USBAHB_EPDIS                */
#define _USBAHB_DEVOUTEP_CTL_EPDIS_MASK                    0x40000000UL                                    /**< Bit mask for USBAHB_EPDIS                   */
#define _USBAHB_DEVOUTEP_CTL_EPDIS_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_EPDIS_DEFAULT                  (_USBAHB_DEVOUTEP_CTL_EPDIS_DEFAULT << 30)      /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/
#define USBAHB_DEVOUTEP_CTL_EPENA                          (0x1UL << 31)                                   /**< Endpoint Enable                             */
#define _USBAHB_DEVOUTEP_CTL_EPENA_SHIFT                   31                                              /**< Shift value for USBAHB_EPENA                */
#define _USBAHB_DEVOUTEP_CTL_EPENA_MASK                    0x80000000UL                                    /**< Bit mask for USBAHB_EPENA                   */
#define _USBAHB_DEVOUTEP_CTL_EPENA_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_CTL        */
#define USBAHB_DEVOUTEP_CTL_EPENA_DEFAULT                  (_USBAHB_DEVOUTEP_CTL_EPENA_DEFAULT << 31)      /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_CTL*/

/* Bit fields for USBAHB DEVOUTEP_INT */
#define _USBAHB_DEVOUTEP_INT_RESETVALUE                    0x00000000UL                                       /**< Default value for USBAHB_DEVOUTEP_INT       */
#define _USBAHB_DEVOUTEP_INT_MASK                          0x0000FB7FUL                                       /**< Mask for USBAHB_DEVOUTEP_INT                */
#define USBAHB_DEVOUTEP_INT_XFERCOMPL                      (0x1UL << 0)                                       /**< Transfer Completed Interrupt                */
#define _USBAHB_DEVOUTEP_INT_XFERCOMPL_SHIFT               0                                                  /**< Shift value for USBAHB_XFERCOMPL            */
#define _USBAHB_DEVOUTEP_INT_XFERCOMPL_MASK                0x1UL                                              /**< Bit mask for USBAHB_XFERCOMPL               */
#define _USBAHB_DEVOUTEP_INT_XFERCOMPL_DEFAULT             0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_XFERCOMPL_DEFAULT              (_USBAHB_DEVOUTEP_INT_XFERCOMPL_DEFAULT << 0)      /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_EPDISBLD                       (0x1UL << 1)                                       /**< Endpoint Disabled Interrupt                 */
#define _USBAHB_DEVOUTEP_INT_EPDISBLD_SHIFT                1                                                  /**< Shift value for USBAHB_EPDISBLD             */
#define _USBAHB_DEVOUTEP_INT_EPDISBLD_MASK                 0x2UL                                              /**< Bit mask for USBAHB_EPDISBLD                */
#define _USBAHB_DEVOUTEP_INT_EPDISBLD_DEFAULT              0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_EPDISBLD_DEFAULT               (_USBAHB_DEVOUTEP_INT_EPDISBLD_DEFAULT << 1)       /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_AHBERR                         (0x1UL << 2)                                       /**< AHB Error                                   */
#define _USBAHB_DEVOUTEP_INT_AHBERR_SHIFT                  2                                                  /**< Shift value for USBAHB_AHBERR               */
#define _USBAHB_DEVOUTEP_INT_AHBERR_MASK                   0x4UL                                              /**< Bit mask for USBAHB_AHBERR                  */
#define _USBAHB_DEVOUTEP_INT_AHBERR_DEFAULT                0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_AHBERR_DEFAULT                 (_USBAHB_DEVOUTEP_INT_AHBERR_DEFAULT << 2)         /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_SETUP                          (0x1UL << 3)                                       /**< Phase Done                                  */
#define _USBAHB_DEVOUTEP_INT_SETUP_SHIFT                   3                                                  /**< Shift value for USBAHB_SETUP                */
#define _USBAHB_DEVOUTEP_INT_SETUP_MASK                    0x8UL                                              /**< Bit mask for USBAHB_SETUP                   */
#define _USBAHB_DEVOUTEP_INT_SETUP_DEFAULT                 0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_SETUP_DEFAULT                  (_USBAHB_DEVOUTEP_INT_SETUP_DEFAULT << 3)          /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_OUTTKNEPDIS                    (0x1UL << 4)                                       /**< OUT Token recvd when EP Disabled            */
#define _USBAHB_DEVOUTEP_INT_OUTTKNEPDIS_SHIFT             4                                                  /**< Shift value for USBAHB_OUTTKNEPDIS          */
#define _USBAHB_DEVOUTEP_INT_OUTTKNEPDIS_MASK              0x10UL                                             /**< Bit mask for USBAHB_OUTTKNEPDIS             */
#define _USBAHB_DEVOUTEP_INT_OUTTKNEPDIS_DEFAULT           0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_OUTTKNEPDIS_DEFAULT            (_USBAHB_DEVOUTEP_INT_OUTTKNEPDIS_DEFAULT << 4)    /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_STSPHSERCVD                    (0x1UL << 5)                                       /**< Status Phase recvd For ctrl Write           */
#define _USBAHB_DEVOUTEP_INT_STSPHSERCVD_SHIFT             5                                                  /**< Shift value for USBAHB_STSPHSERCVD          */
#define _USBAHB_DEVOUTEP_INT_STSPHSERCVD_MASK              0x20UL                                             /**< Bit mask for USBAHB_STSPHSERCVD             */
#define _USBAHB_DEVOUTEP_INT_STSPHSERCVD_DEFAULT           0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_STSPHSERCVD_DEFAULT            (_USBAHB_DEVOUTEP_INT_STSPHSERCVD_DEFAULT << 5)    /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_BACK2BACKSETUP                 (0x1UL << 6)                                       /**< Back-to-Back SETUP Packets recvd            */
#define _USBAHB_DEVOUTEP_INT_BACK2BACKSETUP_SHIFT          6                                                  /**< Shift value for USBAHB_BACK2BACKSETUP       */
#define _USBAHB_DEVOUTEP_INT_BACK2BACKSETUP_MASK           0x40UL                                             /**< Bit mask for USBAHB_BACK2BACKSETUP          */
#define _USBAHB_DEVOUTEP_INT_BACK2BACKSETUP_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_BACK2BACKSETUP_DEFAULT         (_USBAHB_DEVOUTEP_INT_BACK2BACKSETUP_DEFAULT << 6) /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_OUTPKTERR                      (0x1UL << 8)                                       /**< OUT Packet Error                            */
#define _USBAHB_DEVOUTEP_INT_OUTPKTERR_SHIFT               8                                                  /**< Shift value for USBAHB_OUTPKTERR            */
#define _USBAHB_DEVOUTEP_INT_OUTPKTERR_MASK                0x100UL                                            /**< Bit mask for USBAHB_OUTPKTERR               */
#define _USBAHB_DEVOUTEP_INT_OUTPKTERR_DEFAULT             0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_OUTPKTERR_DEFAULT              (_USBAHB_DEVOUTEP_INT_OUTPKTERR_DEFAULT << 8)      /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_BNAINTR                        (0x1UL << 9)                                       /**< BNA (Buffer Not Available) IRQ              */
#define _USBAHB_DEVOUTEP_INT_BNAINTR_SHIFT                 9                                                  /**< Shift value for USBAHB_BNAINTR              */
#define _USBAHB_DEVOUTEP_INT_BNAINTR_MASK                  0x200UL                                            /**< Bit mask for USBAHB_BNAINTR                 */
#define _USBAHB_DEVOUTEP_INT_BNAINTR_DEFAULT               0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_BNAINTR_DEFAULT                (_USBAHB_DEVOUTEP_INT_BNAINTR_DEFAULT << 9)        /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_PKTDRPSTS                      (0x1UL << 11)                                      /**< Packet Drop Status                          */
#define _USBAHB_DEVOUTEP_INT_PKTDRPSTS_SHIFT               11                                                 /**< Shift value for USBAHB_PKTDRPSTS            */
#define _USBAHB_DEVOUTEP_INT_PKTDRPSTS_MASK                0x800UL                                            /**< Bit mask for USBAHB_PKTDRPSTS               */
#define _USBAHB_DEVOUTEP_INT_PKTDRPSTS_DEFAULT             0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_PKTDRPSTS_DEFAULT              (_USBAHB_DEVOUTEP_INT_PKTDRPSTS_DEFAULT << 11)     /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_BBLEERR                        (0x1UL << 12)                                      /**< Babble Error Interupt                       */
#define _USBAHB_DEVOUTEP_INT_BBLEERR_SHIFT                 12                                                 /**< Shift value for USBAHB_BBLEERR              */
#define _USBAHB_DEVOUTEP_INT_BBLEERR_MASK                  0x1000UL                                           /**< Bit mask for USBAHB_BBLEERR                 */
#define _USBAHB_DEVOUTEP_INT_BBLEERR_DEFAULT               0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_BBLEERR_DEFAULT                (_USBAHB_DEVOUTEP_INT_BBLEERR_DEFAULT << 12)       /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_NAKINTRPT                      (0x1UL << 13)                                      /**< NAK Interrupt                               */
#define _USBAHB_DEVOUTEP_INT_NAKINTRPT_SHIFT               13                                                 /**< Shift value for USBAHB_NAKINTRPT            */
#define _USBAHB_DEVOUTEP_INT_NAKINTRPT_MASK                0x2000UL                                           /**< Bit mask for USBAHB_NAKINTRPT               */
#define _USBAHB_DEVOUTEP_INT_NAKINTRPT_DEFAULT             0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_NAKINTRPT_DEFAULT              (_USBAHB_DEVOUTEP_INT_NAKINTRPT_DEFAULT << 13)     /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_NYETINTRPT                     (0x1UL << 14)                                      /**< NYET Interrupt                              */
#define _USBAHB_DEVOUTEP_INT_NYETINTRPT_SHIFT              14                                                 /**< Shift value for USBAHB_NYETINTRPT           */
#define _USBAHB_DEVOUTEP_INT_NYETINTRPT_MASK               0x4000UL                                           /**< Bit mask for USBAHB_NYETINTRPT              */
#define _USBAHB_DEVOUTEP_INT_NYETINTRPT_DEFAULT            0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_NYETINTRPT_DEFAULT             (_USBAHB_DEVOUTEP_INT_NYETINTRPT_DEFAULT << 14)    /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/
#define USBAHB_DEVOUTEP_INT_STUPPKTRCVD                    (0x1UL << 15)                                      /**< Setup PKT received Interrupt                */
#define _USBAHB_DEVOUTEP_INT_STUPPKTRCVD_SHIFT             15                                                 /**< Shift value for USBAHB_STUPPKTRCVD          */
#define _USBAHB_DEVOUTEP_INT_STUPPKTRCVD_MASK              0x8000UL                                           /**< Bit mask for USBAHB_STUPPKTRCVD             */
#define _USBAHB_DEVOUTEP_INT_STUPPKTRCVD_DEFAULT           0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_INT        */
#define USBAHB_DEVOUTEP_INT_STUPPKTRCVD_DEFAULT            (_USBAHB_DEVOUTEP_INT_STUPPKTRCVD_DEFAULT << 15)   /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_INT*/

/* Bit fields for USBAHB DEVOUTEP_TSIZ */
#define _USBAHB_DEVOUTEP_TSIZ_RESETVALUE                   0x00000000UL                                       /**< Default value for USBAHB_DEVOUTEP_TSIZ      */
#define _USBAHB_DEVOUTEP_TSIZ_MASK                         0x7FFFFFFFUL                                       /**< Mask for USBAHB_DEVOUTEP_TSIZ               */
#define _USBAHB_DEVOUTEP_TSIZ_XFERSIZE_SHIFT               0                                                  /**< Shift value for USBAHB_XFERSIZE             */
#define _USBAHB_DEVOUTEP_TSIZ_XFERSIZE_MASK                0x7FFFFUL                                          /**< Bit mask for USBAHB_XFERSIZE                */
#define _USBAHB_DEVOUTEP_TSIZ_XFERSIZE_DEFAULT             0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_TSIZ       */
#define USBAHB_DEVOUTEP_TSIZ_XFERSIZE_DEFAULT              (_USBAHB_DEVOUTEP_TSIZ_XFERSIZE_DEFAULT << 0)      /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_TSIZ*/
#define _USBAHB_DEVOUTEP_TSIZ_PKTCNT_SHIFT                 19                                                 /**< Shift value for USBAHB_PKTCNT               */
#define _USBAHB_DEVOUTEP_TSIZ_PKTCNT_MASK                  0x1FF80000UL                                       /**< Bit mask for USBAHB_PKTCNT                  */
#define _USBAHB_DEVOUTEP_TSIZ_PKTCNT_DEFAULT               0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_TSIZ       */
#define USBAHB_DEVOUTEP_TSIZ_PKTCNT_DEFAULT                (_USBAHB_DEVOUTEP_TSIZ_PKTCNT_DEFAULT << 19)       /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_TSIZ*/
#define _USBAHB_DEVOUTEP_TSIZ_RXDPIDSUPCNT_SHIFT           29                                                 /**< Shift value for USBAHB_RXDPIDSUPCNT         */
#define _USBAHB_DEVOUTEP_TSIZ_RXDPIDSUPCNT_MASK            0x60000000UL                                       /**< Bit mask for USBAHB_RXDPIDSUPCNT            */
#define _USBAHB_DEVOUTEP_TSIZ_RXDPIDSUPCNT_DEFAULT         0x00000000UL                                       /**< Mode DEFAULT for USBAHB_DEVOUTEP_TSIZ       */
#define USBAHB_DEVOUTEP_TSIZ_RXDPIDSUPCNT_DEFAULT          (_USBAHB_DEVOUTEP_TSIZ_RXDPIDSUPCNT_DEFAULT << 29) /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_TSIZ*/

/* Bit fields for USBAHB DEVOUTEP_DMAADDR */
#define _USBAHB_DEVOUTEP_DMAADDR_RESETVALUE                0x00000000UL                                    /**< Default value for USBAHB_DEVOUTEP_DMAADDR   */
#define _USBAHB_DEVOUTEP_DMAADDR_MASK                      0xFFFFFFFFUL                                    /**< Mask for USBAHB_DEVOUTEP_DMAADDR            */
#define _USBAHB_DEVOUTEP_DMAADDR_DMAADDR_SHIFT             0                                               /**< Shift value for USBAHB_DMAADDR              */
#define _USBAHB_DEVOUTEP_DMAADDR_DMAADDR_MASK              0xFFFFFFFFUL                                    /**< Bit mask for USBAHB_DMAADDR                 */
#define _USBAHB_DEVOUTEP_DMAADDR_DMAADDR_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for USBAHB_DEVOUTEP_DMAADDR    */
#define USBAHB_DEVOUTEP_DMAADDR_DMAADDR_DEFAULT            (_USBAHB_DEVOUTEP_DMAADDR_DMAADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_DEVOUTEP_DMAADDR*/

/* Bit fields for USBAHB PCGCCTL */
#define _USBAHB_PCGCCTL_RESETVALUE                         0x00000000UL                                 /**< Default value for USBAHB_PCGCCTL            */
#define _USBAHB_PCGCCTL_MASK                               0x000000CFUL                                 /**< Mask for USBAHB_PCGCCTL                     */
#define USBAHB_PCGCCTL_STOPPCLK                            (0x1UL << 0)                                 /**< Stop Pclk                                   */
#define _USBAHB_PCGCCTL_STOPPCLK_SHIFT                     0                                            /**< Shift value for USBAHB_STOPPCLK             */
#define _USBAHB_PCGCCTL_STOPPCLK_MASK                      0x1UL                                        /**< Bit mask for USBAHB_STOPPCLK                */
#define _USBAHB_PCGCCTL_STOPPCLK_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for USBAHB_PCGCCTL             */
#define USBAHB_PCGCCTL_STOPPCLK_DEFAULT                    (_USBAHB_PCGCCTL_STOPPCLK_DEFAULT << 0)      /**< Shifted mode DEFAULT for USBAHB_PCGCCTL     */
#define USBAHB_PCGCCTL_GATEHCLK                            (0x1UL << 1)                                 /**< Gate HCLK                                   */
#define _USBAHB_PCGCCTL_GATEHCLK_SHIFT                     1                                            /**< Shift value for USBAHB_GATEHCLK             */
#define _USBAHB_PCGCCTL_GATEHCLK_MASK                      0x2UL                                        /**< Bit mask for USBAHB_GATEHCLK                */
#define _USBAHB_PCGCCTL_GATEHCLK_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for USBAHB_PCGCCTL             */
#define USBAHB_PCGCCTL_GATEHCLK_DEFAULT                    (_USBAHB_PCGCCTL_GATEHCLK_DEFAULT << 1)      /**< Shifted mode DEFAULT for USBAHB_PCGCCTL     */
#define USBAHB_PCGCCTL_PWRCLMP                             (0x1UL << 2)                                 /**< Power Clamp                                 */
#define _USBAHB_PCGCCTL_PWRCLMP_SHIFT                      2                                            /**< Shift value for USBAHB_PWRCLMP              */
#define _USBAHB_PCGCCTL_PWRCLMP_MASK                       0x4UL                                        /**< Bit mask for USBAHB_PWRCLMP                 */
#define _USBAHB_PCGCCTL_PWRCLMP_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for USBAHB_PCGCCTL             */
#define USBAHB_PCGCCTL_PWRCLMP_DEFAULT                     (_USBAHB_PCGCCTL_PWRCLMP_DEFAULT << 2)       /**< Shifted mode DEFAULT for USBAHB_PCGCCTL     */
#define USBAHB_PCGCCTL_RSTPDWNMODULE                       (0x1UL << 3)                                 /**< RST Power-Down Modules                      */
#define _USBAHB_PCGCCTL_RSTPDWNMODULE_SHIFT                3                                            /**< Shift value for USBAHB_RSTPDWNMODULE        */
#define _USBAHB_PCGCCTL_RSTPDWNMODULE_MASK                 0x8UL                                        /**< Bit mask for USBAHB_RSTPDWNMODULE           */
#define _USBAHB_PCGCCTL_RSTPDWNMODULE_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for USBAHB_PCGCCTL             */
#define USBAHB_PCGCCTL_RSTPDWNMODULE_DEFAULT               (_USBAHB_PCGCCTL_RSTPDWNMODULE_DEFAULT << 3) /**< Shifted mode DEFAULT for USBAHB_PCGCCTL     */
#define USBAHB_PCGCCTL_PHYSLEEP                            (0x1UL << 6)                                 /**< New BitField                                */
#define _USBAHB_PCGCCTL_PHYSLEEP_SHIFT                     6                                            /**< Shift value for USBAHB_PHYSLEEP             */
#define _USBAHB_PCGCCTL_PHYSLEEP_MASK                      0x40UL                                       /**< Bit mask for USBAHB_PHYSLEEP                */
#define _USBAHB_PCGCCTL_PHYSLEEP_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for USBAHB_PCGCCTL             */
#define USBAHB_PCGCCTL_PHYSLEEP_DEFAULT                    (_USBAHB_PCGCCTL_PHYSLEEP_DEFAULT << 6)      /**< Shifted mode DEFAULT for USBAHB_PCGCCTL     */
#define USBAHB_PCGCCTL_L1SUSPENDED                         (0x1UL << 7)                                 /**< New BitField                                */
#define _USBAHB_PCGCCTL_L1SUSPENDED_SHIFT                  7                                            /**< Shift value for USBAHB_L1SUSPENDED          */
#define _USBAHB_PCGCCTL_L1SUSPENDED_MASK                   0x80UL                                       /**< Bit mask for USBAHB_L1SUSPENDED             */
#define _USBAHB_PCGCCTL_L1SUSPENDED_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for USBAHB_PCGCCTL             */
#define USBAHB_PCGCCTL_L1SUSPENDED_DEFAULT                 (_USBAHB_PCGCCTL_L1SUSPENDED_DEFAULT << 7)   /**< Shifted mode DEFAULT for USBAHB_PCGCCTL     */

/* Bit fields for USBAHB FIFO0D */
#define _USBAHB_FIFO0D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO0D             */
#define _USBAHB_FIFO0D_MASK                                0xFFFFFFFFUL                         /**< Mask for USBAHB_FIFO0D                      */
#define _USBAHB_FIFO0D_FIFO0D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO0D               */
#define _USBAHB_FIFO0D_FIFO0D_MASK                         0xFFFFFFFFUL                         /**< Bit mask for USBAHB_FIFO0D                  */
#define _USBAHB_FIFO0D_FIFO0D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO0D              */
#define USBAHB_FIFO0D_FIFO0D_DEFAULT                       (_USBAHB_FIFO0D_FIFO0D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO0D      */

/* Bit fields for USBAHB FIFO1D */
#define _USBAHB_FIFO1D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO1D             */
#define _USBAHB_FIFO1D_MASK                                0xFFFFFFFFUL                         /**< Mask for USBAHB_FIFO1D                      */
#define _USBAHB_FIFO1D_FIFO1D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO1D               */
#define _USBAHB_FIFO1D_FIFO1D_MASK                         0xFFFFFFFFUL                         /**< Bit mask for USBAHB_FIFO1D                  */
#define _USBAHB_FIFO1D_FIFO1D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO1D              */
#define USBAHB_FIFO1D_FIFO1D_DEFAULT                       (_USBAHB_FIFO1D_FIFO1D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO1D      */

/* Bit fields for USBAHB FIFO2D */
#define _USBAHB_FIFO2D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO2D             */
#define _USBAHB_FIFO2D_MASK                                0xFFFFFFFFUL                         /**< Mask for USBAHB_FIFO2D                      */
#define _USBAHB_FIFO2D_FIFO2D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO2D               */
#define _USBAHB_FIFO2D_FIFO2D_MASK                         0xFFFFFFFFUL                         /**< Bit mask for USBAHB_FIFO2D                  */
#define _USBAHB_FIFO2D_FIFO2D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO2D              */
#define USBAHB_FIFO2D_FIFO2D_DEFAULT                       (_USBAHB_FIFO2D_FIFO2D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO2D      */

/* Bit fields for USBAHB FIFO3D */
#define _USBAHB_FIFO3D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO3D             */
#define _USBAHB_FIFO3D_MASK                                0x00000001UL                         /**< Mask for USBAHB_FIFO3D                      */
#define USBAHB_FIFO3D_FIFO3D                               (0x1UL << 0)                         /**< EP 3 Data                                   */
#define _USBAHB_FIFO3D_FIFO3D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO3D               */
#define _USBAHB_FIFO3D_FIFO3D_MASK                         0x1UL                                /**< Bit mask for USBAHB_FIFO3D                  */
#define _USBAHB_FIFO3D_FIFO3D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO3D              */
#define USBAHB_FIFO3D_FIFO3D_DEFAULT                       (_USBAHB_FIFO3D_FIFO3D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO3D      */

/* Bit fields for USBAHB FIFO4D */
#define _USBAHB_FIFO4D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO4D             */
#define _USBAHB_FIFO4D_MASK                                0xFFFFFFFFUL                         /**< Mask for USBAHB_FIFO4D                      */
#define _USBAHB_FIFO4D_FIFO4D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO4D               */
#define _USBAHB_FIFO4D_FIFO4D_MASK                         0xFFFFFFFFUL                         /**< Bit mask for USBAHB_FIFO4D                  */
#define _USBAHB_FIFO4D_FIFO4D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO4D              */
#define USBAHB_FIFO4D_FIFO4D_DEFAULT                       (_USBAHB_FIFO4D_FIFO4D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO4D      */

/* Bit fields for USBAHB FIFO5D */
#define _USBAHB_FIFO5D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO5D             */
#define _USBAHB_FIFO5D_MASK                                0x00000001UL                         /**< Mask for USBAHB_FIFO5D                      */
#define USBAHB_FIFO5D_FIFO5D                               (0x1UL << 0)                         /**< EP 5 Data                                   */
#define _USBAHB_FIFO5D_FIFO5D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO5D               */
#define _USBAHB_FIFO5D_FIFO5D_MASK                         0x1UL                                /**< Bit mask for USBAHB_FIFO5D                  */
#define _USBAHB_FIFO5D_FIFO5D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO5D              */
#define USBAHB_FIFO5D_FIFO5D_DEFAULT                       (_USBAHB_FIFO5D_FIFO5D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO5D      */

/* Bit fields for USBAHB FIFO6D */
#define _USBAHB_FIFO6D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO6D             */
#define _USBAHB_FIFO6D_MASK                                0x00000001UL                         /**< Mask for USBAHB_FIFO6D                      */
#define USBAHB_FIFO6D_FIFO6D                               (0x1UL << 0)                         /**< EP 6 Data                                   */
#define _USBAHB_FIFO6D_FIFO6D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO6D               */
#define _USBAHB_FIFO6D_FIFO6D_MASK                         0x1UL                                /**< Bit mask for USBAHB_FIFO6D                  */
#define _USBAHB_FIFO6D_FIFO6D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO6D              */
#define USBAHB_FIFO6D_FIFO6D_DEFAULT                       (_USBAHB_FIFO6D_FIFO6D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO6D      */

/* Bit fields for USBAHB FIFO7D */
#define _USBAHB_FIFO7D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO7D             */
#define _USBAHB_FIFO7D_MASK                                0xFFFFFFFFUL                         /**< Mask for USBAHB_FIFO7D                      */
#define _USBAHB_FIFO7D_FIFO7D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO7D               */
#define _USBAHB_FIFO7D_FIFO7D_MASK                         0xFFFFFFFFUL                         /**< Bit mask for USBAHB_FIFO7D                  */
#define _USBAHB_FIFO7D_FIFO7D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO7D              */
#define USBAHB_FIFO7D_FIFO7D_DEFAULT                       (_USBAHB_FIFO7D_FIFO7D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO7D      */

/* Bit fields for USBAHB FIFO8D */
#define _USBAHB_FIFO8D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO8D             */
#define _USBAHB_FIFO8D_MASK                                0xFFFFFFFFUL                         /**< Mask for USBAHB_FIFO8D                      */
#define _USBAHB_FIFO8D_FIFO8D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO8D               */
#define _USBAHB_FIFO8D_FIFO8D_MASK                         0xFFFFFFFFUL                         /**< Bit mask for USBAHB_FIFO8D                  */
#define _USBAHB_FIFO8D_FIFO8D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO8D              */
#define USBAHB_FIFO8D_FIFO8D_DEFAULT                       (_USBAHB_FIFO8D_FIFO8D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO8D      */

/* Bit fields for USBAHB FIFO9D */
#define _USBAHB_FIFO9D_RESETVALUE                          0x00000000UL                         /**< Default value for USBAHB_FIFO9D             */
#define _USBAHB_FIFO9D_MASK                                0xFFFFFFFFUL                         /**< Mask for USBAHB_FIFO9D                      */
#define _USBAHB_FIFO9D_FIFO9D_SHIFT                        0                                    /**< Shift value for USBAHB_FIFO9D               */
#define _USBAHB_FIFO9D_FIFO9D_MASK                         0xFFFFFFFFUL                         /**< Bit mask for USBAHB_FIFO9D                  */
#define _USBAHB_FIFO9D_FIFO9D_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for USBAHB_FIFO9D              */
#define USBAHB_FIFO9D_FIFO9D_DEFAULT                       (_USBAHB_FIFO9D_FIFO9D_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFO9D      */

/* Bit fields for USBAHB FIFORAM */
#define _USBAHB_FIFORAM_RESETVALUE                         0x00000000UL                           /**< Default value for USBAHB_FIFORAM            */
#define _USBAHB_FIFORAM_MASK                               0xFFFFFFFFUL                           /**< Mask for USBAHB_FIFORAM                     */
#define _USBAHB_FIFORAM_FIFORAM_SHIFT                      0                                      /**< Shift value for USBAHB_FIFORAM              */
#define _USBAHB_FIFORAM_FIFORAM_MASK                       0xFFFFFFFFUL                           /**< Bit mask for USBAHB_FIFORAM                 */
#define _USBAHB_FIFORAM_FIFORAM_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for USBAHB_FIFORAM             */
#define USBAHB_FIFORAM_FIFORAM_DEFAULT                     (_USBAHB_FIFORAM_FIFORAM_DEFAULT << 0) /**< Shifted mode DEFAULT for USBAHB_FIFORAM     */

/** @} End of group EFR32FG25_USBAHB_AHBS_BitFields */
/** @} End of group EFR32FG25_USBAHB_AHBS */
/** @} End of group Parts */

#endif /* EFR32FG25_USBAHB_H */
