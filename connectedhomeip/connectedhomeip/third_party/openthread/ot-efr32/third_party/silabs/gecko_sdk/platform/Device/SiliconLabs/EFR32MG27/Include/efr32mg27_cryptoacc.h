/**************************************************************************//**
 * @file
 * @brief EFR32MG27 CRYPTOACC register and bit field definitions
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
#ifndef EFR32MG27_CRYPTOACC_H
#define EFR32MG27_CRYPTOACC_H

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG27_CRYPTOACC CRYPTOACC
 * @{
 * @brief EFR32MG27 CRYPTOACC Register Declaration.
 *****************************************************************************/

/** CRYPTOACC Register Declaration. */
typedef struct {
  __IOM uint32_t DMACTRL_FETCHADDR;                  /**< CRYPTOMASTER_DMACTRL_FETCH_ADDR_LSB                */
  __IOM uint32_t DMACTRL_FETCHADDRMSB;               /**< CRYPTOMASTER_DMACTRL_FETCH_ADDR_MSB                */
  __IOM uint32_t DMACTRL_FETCHLEN;                   /**< CRYPTOMASTER_DMACTRL_FETCH_LEN                     */
  __IOM uint32_t DMACTRL_FETCHTAG;                   /**< CRYPTOMASTER_DMACTRL_FETCH_TAG                     */
  __IOM uint32_t DMACTRL_PUSHADDR;                   /**< CRYPTOMASTER_DMACTRL_PUSH_ADDR_LSB                 */
  __IOM uint32_t DMACTRL_PUSHADDR_MSB;               /**< CRYPTOMASTER_DMACTRL_PUSH_ADDR_MSB                 */
  __IOM uint32_t DMACTRL_PUSHLEN;                    /**< CRYPTOMASTER_DMACTRL_PUSH_LEN                      */
  __IOM uint32_t DMACTRL_IEN;                        /**< CRYPTOMASTER_DMACTRL_INT_EN                        */
  __IOM uint32_t DMACTRL_IEN_SET;                    /**< CRYPTOMASTER_DMACTRL_INT_ENSET                     */
  __IOM uint32_t DMACTRL_IEN_CLR;                    /**< CRYPTOMASTER_DMACTRL_INT_ENCLR                     */
  __IM uint32_t  DMACTRL_IF;                         /**< CRYPTOMASTER_DMACTRL_INT_STATRAW                   */
  __IM uint32_t  DMACTRL_IFMASKED;                   /**< CRYPTOMASTER_DMACTRL_INT_STAT                      */
  __IOM uint32_t DMACTRL_IF_CLR;                     /**< CRYPTOMASTER_DMACTRL_INT_STATCLR                   */
  __IOM uint32_t DMACTRL_CONFIG;                     /**< CRYPTOMASTER_DMACTRL_CONFIG                        */
  __IOM uint32_t DMACTRL_START;                      /**< CRYPTOMASTER_DMACTRL_START                         */
  __IM uint32_t  DMACTRL_STATUS;                     /**< CRYPTOMASTER_DMACTRL_STATUS                        */
  uint32_t       RESERVED0[240U];                    /**< Reserved for future use                            */
  __IM uint32_t  INCLIPS_HWCFG;                      /**< CRYPTOMASTER_CRYPTOMASTER_REGISTERS_INCLIPS_HWCFG  */
  __IM uint32_t  BA411E_HWCFG1;                      /**< CRYPTOMASTER_CRYPTOMASTER_REGISTERS_BA411E_HWCFG_1 */
  __IM uint32_t  BA411E_HWCFG2;                      /**< CRYPTOMASTER_CRYPTOMASTER_REGISTERS_BA411E_HWCFG_2 */
  __IM uint32_t  BA413_HWCFG;                        /**< CRYPTOMASTER_CRYPTOMASTER_REGISTERS_BA413_HWCFG    */
  __IM uint32_t  BA418_HWCFG;                        /**< CRYPTOMASTER_CRYPTOMASTER_REGISTERS_BA418_HWCFG    */
  __IM uint32_t  BA419_HWCFG;                        /**< CRYPTOMASTER_CRYPTOMASTER_REGISTERS_BA419_HWCFG    */
  uint32_t       RESERVED1[762U];                    /**< Reserved for future use                            */
  __IOM uint32_t NDRNG_CONTROL;                      /**< BA431_NDRNG_REGISTERS_CONTROL                      */
  __IOM uint32_t NDRNG_FIFOLEVEL;                    /**< BA431_NDRNG_REGISTERS_FIFOLEVEL                    */
  __IOM uint32_t NDRNG_FIFOTHRESH;                   /**< BA431_NDRNG_REGISTERS_FIFOTHRESHOLD                */
  __IM uint32_t  NDRNG_FIFODEPTH;                    /**< BA431_NDRNG_REGISTERS_FIFODEPTH                    */
  __IOM uint32_t NDRNG_KEY0;                         /**< BA431_NDRNG_REGISTERS_KEY0                         */
  __IOM uint32_t NDRNG_KEY1;                         /**< BA431_NDRNG_REGISTERS_KEY1                         */
  __IOM uint32_t NDRNG_KEY2;                         /**< BA431_NDRNG_REGISTERS_KEY2                         */
  __IOM uint32_t NDRNG_KEY3;                         /**< BA431_NDRNG_REGISTERS_KEY3                         */
  __IOM uint32_t NDRNG_TESTDATA;                     /**< BA431_NDRNG_REGISTERS_TESTDATA                     */
  __IOM uint32_t NDRNG_REPTHRES;                     /**< BA431_NDRNG_REGISTERS_REPEATTHRESHOLD              */
  __IOM uint32_t NDRNG_PROPTHRES;                    /**< BA431_NDRNG_REGISTERS_PROPTHRESHOLD                */
  uint32_t       RESERVED2[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t NDRNG_STATUS;                       /**< BA431_NDRNG_REGISTERS_STATUS                       */
  __IOM uint32_t NDRNG_INITWAITVAL;                  /**< BA431_NDRNG_REGISTERS_INITWAITVAL                  */
  __IOM uint32_t NDRNG_DISABLEOSC0;                  /**< BA431_NDRNG_REGISTERS_DISABLEOSC0                  */
  __IOM uint32_t NDRNG_DISABLEOSC1;                  /**< BA431_NDRNG_REGISTERS_DISABLEOSC1                  */
  __IOM uint32_t NDRNG_SWOFFTMRVAL;                  /**< BA431_NDRNG_REGISTERS_SWOFFTMRVAL                  */
  __IOM uint32_t NDRNG_CLKDIV;                       /**< BA431_NDRNG_REGISTERS_CLKDIV                       */
  __IOM uint32_t NDRNG_AIS31CONF0;                   /**< BA431_NDRNG_REGISTERS_AIS31CONF0                   */
  __IOM uint32_t NDRNG_AIS31CONF1;                   /**< BA431_NDRNG_REGISTERS_AIS31CONF1                   */
  __IOM uint32_t NDRNG_AIS31CONF2;                   /**< BA431_NDRNG_REGISTERS_AIS31CONF2                   */
  __IOM uint32_t NDRNG_AIS31STATUS;                  /**< BA431_NDRNG_REGISTERS_AIS31STATUS                  */
  __IM uint32_t  NDRNG_HWCONFIG;                     /**< BA431_NDRNG_REGISTERS_HWCONFIG                     */
  uint32_t       RESERVED3[9U];                      /**< Reserved for future use                            */
  __IM uint32_t  NDRNG_FIFOOUTPUTDATA;               /**< BA431_NDRNG_DATA_FIFOOUTPUTDATA                    */
  uint32_t       RESERVED4[991U];                    /**< Reserved for future use                            */
  __IOM uint32_t PK_POINTERS;                        /**< BA414EP_PK_REGISTERS_POINTERS                      */
  __IOM uint32_t PK_COMMAND;                         /**< BA414EP_PK_REGISTERS_COMMAND                       */
  __IOM uint32_t PK_CONTROL;                         /**< BA414EP_PK_REGISTERS_CONTROL                       */
  __IM uint32_t  PK_STATUS;                          /**< BA414EP_PK_REGISTERS_STATUS                        */
  uint32_t       RESERVED5[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t PK_TIMER;                           /**< BA414EP_PK_REGISTERS_TIMER                         */
  __IM uint32_t  PK_HWCONFIG;                        /**< BA414EP_PK_REGISTERS_HWCONFIG                      */
} CRYPTOACC_TypeDef;
/** @} End of group EFR32MG27_CRYPTOACC */

/**************************************************************************//**
 * @addtogroup EFR32MG27_CRYPTOACC
 * @{
 * @defgroup EFR32MG27_CRYPTOACC_BitFields CRYPTOACC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for CRYPTOACC DMACTRL_FETCHADDR */
#define _CRYPTOACC_DMACTRL_FETCHADDR_RESETVALUE                     0x00000000UL                                               /**< Default value for CRYPTOACC_DMACTRL_FETCHADDR*/
#define _CRYPTOACC_DMACTRL_FETCHADDR_MASK                           0xFFFFFFFFUL                                               /**< Mask for CRYPTOACC_DMACTRL_FETCHADDR        */
#define _CRYPTOACC_DMACTRL_FETCHADDR_FETCH_ADDR_LSB_SHIFT           0                                                          /**< Shift value for CRYPTOACC_FETCH_ADDR_LSB    */
#define _CRYPTOACC_DMACTRL_FETCHADDR_FETCH_ADDR_LSB_MASK            0xFFFFFFFFUL                                               /**< Bit mask for CRYPTOACC_FETCH_ADDR_LSB       */
#define _CRYPTOACC_DMACTRL_FETCHADDR_FETCH_ADDR_LSB_DEFAULT         0x00000000UL                                               /**< Mode DEFAULT for CRYPTOACC_DMACTRL_FETCHADDR*/
#define CRYPTOACC_DMACTRL_FETCHADDR_FETCH_ADDR_LSB_DEFAULT          (_CRYPTOACC_DMACTRL_FETCHADDR_FETCH_ADDR_LSB_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_FETCHADDR*/

/* Bit fields for CRYPTOACC DMACTRL_FETCHADDRMSB */
#define _CRYPTOACC_DMACTRL_FETCHADDRMSB_RESETVALUE                  0x00000000UL                                                  /**< Default value for CRYPTOACC_DMACTRL_FETCHADDRMSB*/
#define _CRYPTOACC_DMACTRL_FETCHADDRMSB_MASK                        0xFFFFFFFFUL                                                  /**< Mask for CRYPTOACC_DMACTRL_FETCHADDRMSB     */
#define _CRYPTOACC_DMACTRL_FETCHADDRMSB_FETCH_ADDR_MSB_SHIFT        0                                                             /**< Shift value for CRYPTOACC_FETCH_ADDR_MSB    */
#define _CRYPTOACC_DMACTRL_FETCHADDRMSB_FETCH_ADDR_MSB_MASK         0xFFFFFFFFUL                                                  /**< Bit mask for CRYPTOACC_FETCH_ADDR_MSB       */
#define _CRYPTOACC_DMACTRL_FETCHADDRMSB_FETCH_ADDR_MSB_DEFAULT      0x00000000UL                                                  /**< Mode DEFAULT for CRYPTOACC_DMACTRL_FETCHADDRMSB*/
#define CRYPTOACC_DMACTRL_FETCHADDRMSB_FETCH_ADDR_MSB_DEFAULT       (_CRYPTOACC_DMACTRL_FETCHADDRMSB_FETCH_ADDR_MSB_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_FETCHADDRMSB*/

/* Bit fields for CRYPTOACC DMACTRL_FETCHLEN */
#define _CRYPTOACC_DMACTRL_FETCHLEN_RESETVALUE                      0x00000000UL                                              /**< Default value for CRYPTOACC_DMACTRL_FETCHLEN*/
#define _CRYPTOACC_DMACTRL_FETCHLEN_MASK                            0x3FFFFFFFUL                                              /**< Mask for CRYPTOACC_DMACTRL_FETCHLEN         */
#define _CRYPTOACC_DMACTRL_FETCHLEN_FETCH_LEN_SHIFT                 0                                                         /**< Shift value for CRYPTOACC_FETCH_LEN         */
#define _CRYPTOACC_DMACTRL_FETCHLEN_FETCH_LEN_MASK                  0xFFFFFFFUL                                               /**< Bit mask for CRYPTOACC_FETCH_LEN            */
#define _CRYPTOACC_DMACTRL_FETCHLEN_FETCH_LEN_DEFAULT               0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_DMACTRL_FETCHLEN */
#define CRYPTOACC_DMACTRL_FETCHLEN_FETCH_LEN_DEFAULT                (_CRYPTOACC_DMACTRL_FETCHLEN_FETCH_LEN_DEFAULT << 0)      /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_FETCHLEN*/
#define CRYPTOACC_DMACTRL_FETCHLEN_FETCH_CSTADDR                    (0x1UL << 28)                                             /**< FETCH_CSTADDR                               */
#define _CRYPTOACC_DMACTRL_FETCHLEN_FETCH_CSTADDR_SHIFT             28                                                        /**< Shift value for CRYPTOACC_FETCH_CSTADDR     */
#define _CRYPTOACC_DMACTRL_FETCHLEN_FETCH_CSTADDR_MASK              0x10000000UL                                              /**< Bit mask for CRYPTOACC_FETCH_CSTADDR        */
#define _CRYPTOACC_DMACTRL_FETCHLEN_FETCH_CSTADDR_DEFAULT           0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_DMACTRL_FETCHLEN */
#define CRYPTOACC_DMACTRL_FETCHLEN_FETCH_CSTADDR_DEFAULT            (_CRYPTOACC_DMACTRL_FETCHLEN_FETCH_CSTADDR_DEFAULT << 28) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_FETCHLEN*/
#define CRYPTOACC_DMACTRL_FETCHLEN_FETCH_REALIGN                    (0x1UL << 29)                                             /**< FETCH_REALIGN                               */
#define _CRYPTOACC_DMACTRL_FETCHLEN_FETCH_REALIGN_SHIFT             29                                                        /**< Shift value for CRYPTOACC_FETCH_REALIGN     */
#define _CRYPTOACC_DMACTRL_FETCHLEN_FETCH_REALIGN_MASK              0x20000000UL                                              /**< Bit mask for CRYPTOACC_FETCH_REALIGN        */
#define _CRYPTOACC_DMACTRL_FETCHLEN_FETCH_REALIGN_DEFAULT           0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_DMACTRL_FETCHLEN */
#define CRYPTOACC_DMACTRL_FETCHLEN_FETCH_REALIGN_DEFAULT            (_CRYPTOACC_DMACTRL_FETCHLEN_FETCH_REALIGN_DEFAULT << 29) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_FETCHLEN*/

/* Bit fields for CRYPTOACC DMACTRL_FETCHTAG */
#define _CRYPTOACC_DMACTRL_FETCHTAG_RESETVALUE                      0x00000000UL                                         /**< Default value for CRYPTOACC_DMACTRL_FETCHTAG*/
#define _CRYPTOACC_DMACTRL_FETCHTAG_MASK                            0xFFFFFFFFUL                                         /**< Mask for CRYPTOACC_DMACTRL_FETCHTAG         */
#define _CRYPTOACC_DMACTRL_FETCHTAG_FETCH_TAG_SHIFT                 0                                                    /**< Shift value for CRYPTOACC_FETCH_TAG         */
#define _CRYPTOACC_DMACTRL_FETCHTAG_FETCH_TAG_MASK                  0xFFFFFFFFUL                                         /**< Bit mask for CRYPTOACC_FETCH_TAG            */
#define _CRYPTOACC_DMACTRL_FETCHTAG_FETCH_TAG_DEFAULT               0x00000000UL                                         /**< Mode DEFAULT for CRYPTOACC_DMACTRL_FETCHTAG */
#define CRYPTOACC_DMACTRL_FETCHTAG_FETCH_TAG_DEFAULT                (_CRYPTOACC_DMACTRL_FETCHTAG_FETCH_TAG_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_FETCHTAG*/

/* Bit fields for CRYPTOACC DMACTRL_PUSHADDR */
#define _CRYPTOACC_DMACTRL_PUSHADDR_RESETVALUE                      0x00000000UL                                             /**< Default value for CRYPTOACC_DMACTRL_PUSHADDR*/
#define _CRYPTOACC_DMACTRL_PUSHADDR_MASK                            0xFFFFFFFFUL                                             /**< Mask for CRYPTOACC_DMACTRL_PUSHADDR         */
#define _CRYPTOACC_DMACTRL_PUSHADDR_PUSH_ADDR_LSB_SHIFT             0                                                        /**< Shift value for CRYPTOACC_PUSH_ADDR_LSB     */
#define _CRYPTOACC_DMACTRL_PUSHADDR_PUSH_ADDR_LSB_MASK              0xFFFFFFFFUL                                             /**< Bit mask for CRYPTOACC_PUSH_ADDR_LSB        */
#define _CRYPTOACC_DMACTRL_PUSHADDR_PUSH_ADDR_LSB_DEFAULT           0x00000000UL                                             /**< Mode DEFAULT for CRYPTOACC_DMACTRL_PUSHADDR */
#define CRYPTOACC_DMACTRL_PUSHADDR_PUSH_ADDR_LSB_DEFAULT            (_CRYPTOACC_DMACTRL_PUSHADDR_PUSH_ADDR_LSB_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_PUSHADDR*/

/* Bit fields for CRYPTOACC DMACTRL_PUSHADDR_MSB */
#define _CRYPTOACC_DMACTRL_PUSHADDR_MSB_RESETVALUE                  0x00000000UL                                                 /**< Default value for CRYPTOACC_DMACTRL_PUSHADDR_MSB*/
#define _CRYPTOACC_DMACTRL_PUSHADDR_MSB_MASK                        0xFFFFFFFFUL                                                 /**< Mask for CRYPTOACC_DMACTRL_PUSHADDR_MSB     */
#define _CRYPTOACC_DMACTRL_PUSHADDR_MSB_PUSH_ADDR_MSB_SHIFT         0                                                            /**< Shift value for CRYPTOACC_PUSH_ADDR_MSB     */
#define _CRYPTOACC_DMACTRL_PUSHADDR_MSB_PUSH_ADDR_MSB_MASK          0xFFFFFFFFUL                                                 /**< Bit mask for CRYPTOACC_PUSH_ADDR_MSB        */
#define _CRYPTOACC_DMACTRL_PUSHADDR_MSB_PUSH_ADDR_MSB_DEFAULT       0x00000000UL                                                 /**< Mode DEFAULT for CRYPTOACC_DMACTRL_PUSHADDR_MSB*/
#define CRYPTOACC_DMACTRL_PUSHADDR_MSB_PUSH_ADDR_MSB_DEFAULT        (_CRYPTOACC_DMACTRL_PUSHADDR_MSB_PUSH_ADDR_MSB_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_PUSHADDR_MSB*/

/* Bit fields for CRYPTOACC DMACTRL_PUSHLEN */
#define _CRYPTOACC_DMACTRL_PUSHLEN_RESETVALUE                       0x00000000UL                                            /**< Default value for CRYPTOACC_DMACTRL_PUSHLEN */
#define _CRYPTOACC_DMACTRL_PUSHLEN_MASK                             0x7FFFFFFFUL                                            /**< Mask for CRYPTOACC_DMACTRL_PUSHLEN          */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_LEN_SHIFT                   0                                                       /**< Shift value for CRYPTOACC_PUSH_LEN          */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_LEN_MASK                    0xFFFFFFFUL                                             /**< Bit mask for CRYPTOACC_PUSH_LEN             */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_LEN_DEFAULT                 0x00000000UL                                            /**< Mode DEFAULT for CRYPTOACC_DMACTRL_PUSHLEN  */
#define CRYPTOACC_DMACTRL_PUSHLEN_PUSH_LEN_DEFAULT                  (_CRYPTOACC_DMACTRL_PUSHLEN_PUSH_LEN_DEFAULT << 0)      /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_PUSHLEN*/
#define CRYPTOACC_DMACTRL_PUSHLEN_PUSH_CSTADDR                      (0x1UL << 28)                                           /**< PUSH_CSTADDR                                */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_CSTADDR_SHIFT               28                                                      /**< Shift value for CRYPTOACC_PUSH_CSTADDR      */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_CSTADDR_MASK                0x10000000UL                                            /**< Bit mask for CRYPTOACC_PUSH_CSTADDR         */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_CSTADDR_DEFAULT             0x00000000UL                                            /**< Mode DEFAULT for CRYPTOACC_DMACTRL_PUSHLEN  */
#define CRYPTOACC_DMACTRL_PUSHLEN_PUSH_CSTADDR_DEFAULT              (_CRYPTOACC_DMACTRL_PUSHLEN_PUSH_CSTADDR_DEFAULT << 28) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_PUSHLEN*/
#define CRYPTOACC_DMACTRL_PUSHLEN_PUSH_REALIGN                      (0x1UL << 29)                                           /**< PUSH_REALIGN                                */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_REALIGN_SHIFT               29                                                      /**< Shift value for CRYPTOACC_PUSH_REALIGN      */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_REALIGN_MASK                0x20000000UL                                            /**< Bit mask for CRYPTOACC_PUSH_REALIGN         */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_REALIGN_DEFAULT             0x00000000UL                                            /**< Mode DEFAULT for CRYPTOACC_DMACTRL_PUSHLEN  */
#define CRYPTOACC_DMACTRL_PUSHLEN_PUSH_REALIGN_DEFAULT              (_CRYPTOACC_DMACTRL_PUSHLEN_PUSH_REALIGN_DEFAULT << 29) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_PUSHLEN*/
#define CRYPTOACC_DMACTRL_PUSHLEN_PUSH_DISCARD                      (0x1UL << 30)                                           /**< PUSH_DISCARD                                */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_DISCARD_SHIFT               30                                                      /**< Shift value for CRYPTOACC_PUSH_DISCARD      */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_DISCARD_MASK                0x40000000UL                                            /**< Bit mask for CRYPTOACC_PUSH_DISCARD         */
#define _CRYPTOACC_DMACTRL_PUSHLEN_PUSH_DISCARD_DEFAULT             0x00000000UL                                            /**< Mode DEFAULT for CRYPTOACC_DMACTRL_PUSHLEN  */
#define CRYPTOACC_DMACTRL_PUSHLEN_PUSH_DISCARD_DEFAULT              (_CRYPTOACC_DMACTRL_PUSHLEN_PUSH_DISCARD_DEFAULT << 30) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_PUSHLEN*/

/* Bit fields for CRYPTOACC DMACTRL_IEN */
#define _CRYPTOACC_DMACTRL_IEN_RESETVALUE                           0x00000000UL                                 /**< Default value for CRYPTOACC_DMACTRL_IEN     */
#define _CRYPTOACC_DMACTRL_IEN_MASK                                 0x0000003FUL                                 /**< Mask for CRYPTOACC_DMACTRL_IEN              */
#define _CRYPTOACC_DMACTRL_IEN_INT_EN_SHIFT                         0                                            /**< Shift value for CRYPTOACC_INT_EN            */
#define _CRYPTOACC_DMACTRL_IEN_INT_EN_MASK                          0x3FUL                                       /**< Bit mask for CRYPTOACC_INT_EN               */
#define _CRYPTOACC_DMACTRL_IEN_INT_EN_DEFAULT                       0x00000000UL                                 /**< Mode DEFAULT for CRYPTOACC_DMACTRL_IEN      */
#define CRYPTOACC_DMACTRL_IEN_INT_EN_DEFAULT                        (_CRYPTOACC_DMACTRL_IEN_INT_EN_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_IEN*/

/* Bit fields for CRYPTOACC DMACTRL_IEN_SET */
#define _CRYPTOACC_DMACTRL_IEN_SET_RESETVALUE                       0x00000000UL                                        /**< Default value for CRYPTOACC_DMACTRL_IEN_SET */
#define _CRYPTOACC_DMACTRL_IEN_SET_MASK                             0x0000003FUL                                        /**< Mask for CRYPTOACC_DMACTRL_IEN_SET          */
#define _CRYPTOACC_DMACTRL_IEN_SET_INT_ENSET_SHIFT                  0                                                   /**< Shift value for CRYPTOACC_INT_ENSET         */
#define _CRYPTOACC_DMACTRL_IEN_SET_INT_ENSET_MASK                   0x3FUL                                              /**< Bit mask for CRYPTOACC_INT_ENSET            */
#define _CRYPTOACC_DMACTRL_IEN_SET_INT_ENSET_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_DMACTRL_IEN_SET  */
#define CRYPTOACC_DMACTRL_IEN_SET_INT_ENSET_DEFAULT                 (_CRYPTOACC_DMACTRL_IEN_SET_INT_ENSET_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_IEN_SET*/

/* Bit fields for CRYPTOACC DMACTRL_IEN_CLR */
#define _CRYPTOACC_DMACTRL_IEN_CLR_RESETVALUE                       0x00000000UL                                        /**< Default value for CRYPTOACC_DMACTRL_IEN_CLR */
#define _CRYPTOACC_DMACTRL_IEN_CLR_MASK                             0x0000003FUL                                        /**< Mask for CRYPTOACC_DMACTRL_IEN_CLR          */
#define _CRYPTOACC_DMACTRL_IEN_CLR_INT_ENCLR_SHIFT                  0                                                   /**< Shift value for CRYPTOACC_INT_ENCLR         */
#define _CRYPTOACC_DMACTRL_IEN_CLR_INT_ENCLR_MASK                   0x3FUL                                              /**< Bit mask for CRYPTOACC_INT_ENCLR            */
#define _CRYPTOACC_DMACTRL_IEN_CLR_INT_ENCLR_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_DMACTRL_IEN_CLR  */
#define CRYPTOACC_DMACTRL_IEN_CLR_INT_ENCLR_DEFAULT                 (_CRYPTOACC_DMACTRL_IEN_CLR_INT_ENCLR_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_IEN_CLR*/

/* Bit fields for CRYPTOACC DMACTRL_IF */
#define _CRYPTOACC_DMACTRL_IF_RESETVALUE                            0x00000000UL                                     /**< Default value for CRYPTOACC_DMACTRL_IF      */
#define _CRYPTOACC_DMACTRL_IF_MASK                                  0x0000003FUL                                     /**< Mask for CRYPTOACC_DMACTRL_IF               */
#define _CRYPTOACC_DMACTRL_IF_INT_STATRAW_SHIFT                     0                                                /**< Shift value for CRYPTOACC_INT_STATRAW       */
#define _CRYPTOACC_DMACTRL_IF_INT_STATRAW_MASK                      0x3FUL                                           /**< Bit mask for CRYPTOACC_INT_STATRAW          */
#define _CRYPTOACC_DMACTRL_IF_INT_STATRAW_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for CRYPTOACC_DMACTRL_IF       */
#define CRYPTOACC_DMACTRL_IF_INT_STATRAW_DEFAULT                    (_CRYPTOACC_DMACTRL_IF_INT_STATRAW_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_IF*/

/* Bit fields for CRYPTOACC DMACTRL_IFMASKED */
#define _CRYPTOACC_DMACTRL_IFMASKED_RESETVALUE                      0x00000000UL                                        /**< Default value for CRYPTOACC_DMACTRL_IFMASKED*/
#define _CRYPTOACC_DMACTRL_IFMASKED_MASK                            0x0000003FUL                                        /**< Mask for CRYPTOACC_DMACTRL_IFMASKED         */
#define _CRYPTOACC_DMACTRL_IFMASKED_INT_STAT_SHIFT                  0                                                   /**< Shift value for CRYPTOACC_INT_STAT          */
#define _CRYPTOACC_DMACTRL_IFMASKED_INT_STAT_MASK                   0x3FUL                                              /**< Bit mask for CRYPTOACC_INT_STAT             */
#define _CRYPTOACC_DMACTRL_IFMASKED_INT_STAT_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_DMACTRL_IFMASKED */
#define CRYPTOACC_DMACTRL_IFMASKED_INT_STAT_DEFAULT                 (_CRYPTOACC_DMACTRL_IFMASKED_INT_STAT_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_IFMASKED*/

/* Bit fields for CRYPTOACC DMACTRL_IF_CLR */
#define _CRYPTOACC_DMACTRL_IF_CLR_RESETVALUE                        0x00000000UL                                         /**< Default value for CRYPTOACC_DMACTRL_IF_CLR  */
#define _CRYPTOACC_DMACTRL_IF_CLR_MASK                              0x0000003FUL                                         /**< Mask for CRYPTOACC_DMACTRL_IF_CLR           */
#define _CRYPTOACC_DMACTRL_IF_CLR_INT_STATCLR_SHIFT                 0                                                    /**< Shift value for CRYPTOACC_INT_STATCLR       */
#define _CRYPTOACC_DMACTRL_IF_CLR_INT_STATCLR_MASK                  0x3FUL                                               /**< Bit mask for CRYPTOACC_INT_STATCLR          */
#define _CRYPTOACC_DMACTRL_IF_CLR_INT_STATCLR_DEFAULT               0x00000000UL                                         /**< Mode DEFAULT for CRYPTOACC_DMACTRL_IF_CLR   */
#define CRYPTOACC_DMACTRL_IF_CLR_INT_STATCLR_DEFAULT                (_CRYPTOACC_DMACTRL_IF_CLR_INT_STATCLR_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_IF_CLR*/

/* Bit fields for CRYPTOACC DMACTRL_CONFIG */
#define _CRYPTOACC_DMACTRL_CONFIG_RESETVALUE                        0x00000000UL                                                 /**< Default value for CRYPTOACC_DMACTRL_CONFIG  */
#define _CRYPTOACC_DMACTRL_CONFIG_MASK                              0x0000001FUL                                                 /**< Mask for CRYPTOACC_DMACTRL_CONFIG           */
#define CRYPTOACC_DMACTRL_CONFIG_FETCH_CTRL_INDIRECT                (0x1UL << 0)                                                 /**< FETCH_CTRL_INDIRECT                         */
#define _CRYPTOACC_DMACTRL_CONFIG_FETCH_CTRL_INDIRECT_SHIFT         0                                                            /**< Shift value for CRYPTOACC_FETCH_CTRL_INDIRECT*/
#define _CRYPTOACC_DMACTRL_CONFIG_FETCH_CTRL_INDIRECT_MASK          0x1UL                                                        /**< Bit mask for CRYPTOACC_FETCH_CTRL_INDIRECT  */
#define _CRYPTOACC_DMACTRL_CONFIG_FETCH_CTRL_INDIRECT_DEFAULT       0x00000000UL                                                 /**< Mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG   */
#define CRYPTOACC_DMACTRL_CONFIG_FETCH_CTRL_INDIRECT_DEFAULT        (_CRYPTOACC_DMACTRL_CONFIG_FETCH_CTRL_INDIRECT_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG*/
#define CRYPTOACC_DMACTRL_CONFIG_PUSH_CTRL_INDIRECT                 (0x1UL << 1)                                                 /**< PUSH_CTRL_INDIRECT                          */
#define _CRYPTOACC_DMACTRL_CONFIG_PUSH_CTRL_INDIRECT_SHIFT          1                                                            /**< Shift value for CRYPTOACC_PUSH_CTRL_INDIRECT*/
#define _CRYPTOACC_DMACTRL_CONFIG_PUSH_CTRL_INDIRECT_MASK           0x2UL                                                        /**< Bit mask for CRYPTOACC_PUSH_CTRL_INDIRECT   */
#define _CRYPTOACC_DMACTRL_CONFIG_PUSH_CTRL_INDIRECT_DEFAULT        0x00000000UL                                                 /**< Mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG   */
#define CRYPTOACC_DMACTRL_CONFIG_PUSH_CTRL_INDIRECT_DEFAULT         (_CRYPTOACC_DMACTRL_CONFIG_PUSH_CTRL_INDIRECT_DEFAULT << 1)  /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG*/
#define CRYPTOACC_DMACTRL_CONFIG_FETCH_STOP                         (0x1UL << 2)                                                 /**< FETCH_STOP                                  */
#define _CRYPTOACC_DMACTRL_CONFIG_FETCH_STOP_SHIFT                  2                                                            /**< Shift value for CRYPTOACC_FETCH_STOP        */
#define _CRYPTOACC_DMACTRL_CONFIG_FETCH_STOP_MASK                   0x4UL                                                        /**< Bit mask for CRYPTOACC_FETCH_STOP           */
#define _CRYPTOACC_DMACTRL_CONFIG_FETCH_STOP_DEFAULT                0x00000000UL                                                 /**< Mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG   */
#define CRYPTOACC_DMACTRL_CONFIG_FETCH_STOP_DEFAULT                 (_CRYPTOACC_DMACTRL_CONFIG_FETCH_STOP_DEFAULT << 2)          /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG*/
#define CRYPTOACC_DMACTRL_CONFIG_PUSH_STOP                          (0x1UL << 3)                                                 /**< PUSH_STOP                                   */
#define _CRYPTOACC_DMACTRL_CONFIG_PUSH_STOP_SHIFT                   3                                                            /**< Shift value for CRYPTOACC_PUSH_STOP         */
#define _CRYPTOACC_DMACTRL_CONFIG_PUSH_STOP_MASK                    0x8UL                                                        /**< Bit mask for CRYPTOACC_PUSH_STOP            */
#define _CRYPTOACC_DMACTRL_CONFIG_PUSH_STOP_DEFAULT                 0x00000000UL                                                 /**< Mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG   */
#define CRYPTOACC_DMACTRL_CONFIG_PUSH_STOP_DEFAULT                  (_CRYPTOACC_DMACTRL_CONFIG_PUSH_STOP_DEFAULT << 3)           /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG*/
#define CRYPTOACC_DMACTRL_CONFIG_SOFT_RST                           (0x1UL << 4)                                                 /**< SOFT_RST                                    */
#define _CRYPTOACC_DMACTRL_CONFIG_SOFT_RST_SHIFT                    4                                                            /**< Shift value for CRYPTOACC_SOFT_RST          */
#define _CRYPTOACC_DMACTRL_CONFIG_SOFT_RST_MASK                     0x10UL                                                       /**< Bit mask for CRYPTOACC_SOFT_RST             */
#define _CRYPTOACC_DMACTRL_CONFIG_SOFT_RST_DEFAULT                  0x00000000UL                                                 /**< Mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG   */
#define CRYPTOACC_DMACTRL_CONFIG_SOFT_RST_DEFAULT                   (_CRYPTOACC_DMACTRL_CONFIG_SOFT_RST_DEFAULT << 4)            /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_CONFIG*/

/* Bit fields for CRYPTOACC DMACTRL_START */
#define _CRYPTOACC_DMACTRL_START_RESETVALUE                         0x00000000UL                                        /**< Default value for CRYPTOACC_DMACTRL_START   */
#define _CRYPTOACC_DMACTRL_START_MASK                               0x00000003UL                                        /**< Mask for CRYPTOACC_DMACTRL_START            */
#define CRYPTOACC_DMACTRL_START_START_FETCH                         (0x1UL << 0)                                        /**< START_FETCH                                 */
#define _CRYPTOACC_DMACTRL_START_START_FETCH_SHIFT                  0                                                   /**< Shift value for CRYPTOACC_START_FETCH       */
#define _CRYPTOACC_DMACTRL_START_START_FETCH_MASK                   0x1UL                                               /**< Bit mask for CRYPTOACC_START_FETCH          */
#define _CRYPTOACC_DMACTRL_START_START_FETCH_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_DMACTRL_START    */
#define CRYPTOACC_DMACTRL_START_START_FETCH_DEFAULT                 (_CRYPTOACC_DMACTRL_START_START_FETCH_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_START*/
#define CRYPTOACC_DMACTRL_START_START_PUSH                          (0x1UL << 1)                                        /**< START_PUSH                                  */
#define _CRYPTOACC_DMACTRL_START_START_PUSH_SHIFT                   1                                                   /**< Shift value for CRYPTOACC_START_PUSH        */
#define _CRYPTOACC_DMACTRL_START_START_PUSH_MASK                    0x2UL                                               /**< Bit mask for CRYPTOACC_START_PUSH           */
#define _CRYPTOACC_DMACTRL_START_START_PUSH_DEFAULT                 0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_DMACTRL_START    */
#define CRYPTOACC_DMACTRL_START_START_PUSH_DEFAULT                  (_CRYPTOACC_DMACTRL_START_START_PUSH_DEFAULT << 1)  /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_START*/

/* Bit fields for CRYPTOACC DMACTRL_STATUS */
#define _CRYPTOACC_DMACTRL_STATUS_RESETVALUE                        0x00000000UL                                              /**< Default value for CRYPTOACC_DMACTRL_STATUS  */
#define _CRYPTOACC_DMACTRL_STATUS_MASK                              0xFFFF0073UL                                              /**< Mask for CRYPTOACC_DMACTRL_STATUS           */
#define CRYPTOACC_DMACTRL_STATUS_FETCH_BUSY                         (0x1UL << 0)                                              /**< FETCH_BUSY                                  */
#define _CRYPTOACC_DMACTRL_STATUS_FETCH_BUSY_SHIFT                  0                                                         /**< Shift value for CRYPTOACC_FETCH_BUSY        */
#define _CRYPTOACC_DMACTRL_STATUS_FETCH_BUSY_MASK                   0x1UL                                                     /**< Bit mask for CRYPTOACC_FETCH_BUSY           */
#define _CRYPTOACC_DMACTRL_STATUS_FETCH_BUSY_DEFAULT                0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_DMACTRL_STATUS   */
#define CRYPTOACC_DMACTRL_STATUS_FETCH_BUSY_DEFAULT                 (_CRYPTOACC_DMACTRL_STATUS_FETCH_BUSY_DEFAULT << 0)       /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_STATUS*/
#define CRYPTOACC_DMACTRL_STATUS_PUSH_BUSY                          (0x1UL << 1)                                              /**< PUSH_BUSY                                   */
#define _CRYPTOACC_DMACTRL_STATUS_PUSH_BUSY_SHIFT                   1                                                         /**< Shift value for CRYPTOACC_PUSH_BUSY         */
#define _CRYPTOACC_DMACTRL_STATUS_PUSH_BUSY_MASK                    0x2UL                                                     /**< Bit mask for CRYPTOACC_PUSH_BUSY            */
#define _CRYPTOACC_DMACTRL_STATUS_PUSH_BUSY_DEFAULT                 0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_DMACTRL_STATUS   */
#define CRYPTOACC_DMACTRL_STATUS_PUSH_BUSY_DEFAULT                  (_CRYPTOACC_DMACTRL_STATUS_PUSH_BUSY_DEFAULT << 1)        /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_STATUS*/
#define CRYPTOACC_DMACTRL_STATUS_FETCH_NOT_EMPTY                    (0x1UL << 4)                                              /**< FETCH_NOT_EMPTY                             */
#define _CRYPTOACC_DMACTRL_STATUS_FETCH_NOT_EMPTY_SHIFT             4                                                         /**< Shift value for CRYPTOACC_FETCH_NOT_EMPTY   */
#define _CRYPTOACC_DMACTRL_STATUS_FETCH_NOT_EMPTY_MASK              0x10UL                                                    /**< Bit mask for CRYPTOACC_FETCH_NOT_EMPTY      */
#define _CRYPTOACC_DMACTRL_STATUS_FETCH_NOT_EMPTY_DEFAULT           0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_DMACTRL_STATUS   */
#define CRYPTOACC_DMACTRL_STATUS_FETCH_NOT_EMPTY_DEFAULT            (_CRYPTOACC_DMACTRL_STATUS_FETCH_NOT_EMPTY_DEFAULT << 4)  /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_STATUS*/
#define CRYPTOACC_DMACTRL_STATUS_PUSH_WAITINGFIFO                   (0x1UL << 5)                                              /**< PUSH_WAITINGFIFO                            */
#define _CRYPTOACC_DMACTRL_STATUS_PUSH_WAITINGFIFO_SHIFT            5                                                         /**< Shift value for CRYPTOACC_PUSH_WAITINGFIFO  */
#define _CRYPTOACC_DMACTRL_STATUS_PUSH_WAITINGFIFO_MASK             0x20UL                                                    /**< Bit mask for CRYPTOACC_PUSH_WAITINGFIFO     */
#define _CRYPTOACC_DMACTRL_STATUS_PUSH_WAITINGFIFO_DEFAULT          0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_DMACTRL_STATUS   */
#define CRYPTOACC_DMACTRL_STATUS_PUSH_WAITINGFIFO_DEFAULT           (_CRYPTOACC_DMACTRL_STATUS_PUSH_WAITINGFIFO_DEFAULT << 5) /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_STATUS*/
#define CRYPTOACC_DMACTRL_STATUS_SOFT_RST_BUSY                      (0x1UL << 6)                                              /**< SOFT_RST_BUSY                               */
#define _CRYPTOACC_DMACTRL_STATUS_SOFT_RST_BUSY_SHIFT               6                                                         /**< Shift value for CRYPTOACC_SOFT_RST_BUSY     */
#define _CRYPTOACC_DMACTRL_STATUS_SOFT_RST_BUSY_MASK                0x40UL                                                    /**< Bit mask for CRYPTOACC_SOFT_RST_BUSY        */
#define _CRYPTOACC_DMACTRL_STATUS_SOFT_RST_BUSY_DEFAULT             0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_DMACTRL_STATUS   */
#define CRYPTOACC_DMACTRL_STATUS_SOFT_RST_BUSY_DEFAULT              (_CRYPTOACC_DMACTRL_STATUS_SOFT_RST_BUSY_DEFAULT << 6)    /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_STATUS*/
#define _CRYPTOACC_DMACTRL_STATUS_PUSH_NBDATA_SHIFT                 16                                                        /**< Shift value for CRYPTOACC_PUSH_NBDATA       */
#define _CRYPTOACC_DMACTRL_STATUS_PUSH_NBDATA_MASK                  0xFFFF0000UL                                              /**< Bit mask for CRYPTOACC_PUSH_NBDATA          */
#define _CRYPTOACC_DMACTRL_STATUS_PUSH_NBDATA_DEFAULT               0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_DMACTRL_STATUS   */
#define CRYPTOACC_DMACTRL_STATUS_PUSH_NBDATA_DEFAULT                (_CRYPTOACC_DMACTRL_STATUS_PUSH_NBDATA_DEFAULT << 16)     /**< Shifted mode DEFAULT for CRYPTOACC_DMACTRL_STATUS*/

/* Bit fields for CRYPTOACC INCLIPS_HWCFG */
#define _CRYPTOACC_INCLIPS_HWCFG_RESETVALUE                         0x00000611UL                                                   /**< Default value for CRYPTOACC_INCLIPS_HWCFG   */
#define _CRYPTOACC_INCLIPS_HWCFG_MASK                               0x00003FFFUL                                                   /**< Mask for CRYPTOACC_INCLIPS_HWCFG            */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAES                        (0x1UL << 0)                                                   /**< G_INCLUDEAES                                */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAES_SHIFT                 0                                                              /**< Shift value for CRYPTOACC_G_INCLUDEAES      */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAES_MASK                  0x1UL                                                          /**< Bit mask for CRYPTOACC_G_INCLUDEAES         */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAES_DEFAULT               0x00000001UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAES_DEFAULT                (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAES_DEFAULT << 0)           /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESGCM                     (0x1UL << 1)                                                   /**< G_INCLUDEAESGCM                             */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESGCM_SHIFT              1                                                              /**< Shift value for CRYPTOACC_G_INCLUDEAESGCM   */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESGCM_MASK               0x2UL                                                          /**< Bit mask for CRYPTOACC_G_INCLUDEAESGCM      */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESGCM_DEFAULT            0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESGCM_DEFAULT             (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESGCM_DEFAULT << 1)        /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESXTS                     (0x1UL << 2)                                                   /**< G_INCLUDEAESXTS                             */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESXTS_SHIFT              2                                                              /**< Shift value for CRYPTOACC_G_INCLUDEAESXTS   */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESXTS_MASK               0x4UL                                                          /**< Bit mask for CRYPTOACC_G_INCLUDEAESXTS      */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESXTS_DEFAULT            0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESXTS_DEFAULT             (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEAESXTS_DEFAULT << 2)        /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEDES                        (0x1UL << 3)                                                   /**< G_INCLUDEDES                                */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEDES_SHIFT                 3                                                              /**< Shift value for CRYPTOACC_G_INCLUDEDES      */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEDES_MASK                  0x8UL                                                          /**< Bit mask for CRYPTOACC_G_INCLUDEDES         */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEDES_DEFAULT               0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEDES_DEFAULT                (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEDES_DEFAULT << 3)           /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHASH                       (0x1UL << 4)                                                   /**< G_INCLUDEHASH                               */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHASH_SHIFT                4                                                              /**< Shift value for CRYPTOACC_G_INCLUDEHASH     */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHASH_MASK                 0x10UL                                                         /**< Bit mask for CRYPTOACC_G_INCLUDEHASH        */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHASH_DEFAULT              0x00000001UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHASH_DEFAULT               (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHASH_DEFAULT << 4)          /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDECHACHAPOLY                 (0x1UL << 5)                                                   /**< G_INCLUDECHACHAPOLY                         */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDECHACHAPOLY_SHIFT          5                                                              /**< Shift value for CRYPTOACC_G_INCLUDECHACHAPOLY*/
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDECHACHAPOLY_MASK           0x20UL                                                         /**< Bit mask for CRYPTOACC_G_INCLUDECHACHAPOLY  */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDECHACHAPOLY_DEFAULT        0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDECHACHAPOLY_DEFAULT         (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDECHACHAPOLY_DEFAULT << 5)    /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESHA3                       (0x1UL << 6)                                                   /**< G_INCLUDESHA3                               */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESHA3_SHIFT                6                                                              /**< Shift value for CRYPTOACC_G_INCLUDESHA3     */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESHA3_MASK                 0x40UL                                                         /**< Bit mask for CRYPTOACC_G_INCLUDESHA3        */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESHA3_DEFAULT              0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESHA3_DEFAULT               (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESHA3_DEFAULT << 6)          /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEZUC                        (0x1UL << 7)                                                   /**< G_INCLUDEZUC                                */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEZUC_SHIFT                 7                                                              /**< Shift value for CRYPTOACC_G_INCLUDEZUC      */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEZUC_MASK                  0x80UL                                                         /**< Bit mask for CRYPTOACC_G_INCLUDEZUC         */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEZUC_DEFAULT               0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEZUC_DEFAULT                (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEZUC_DEFAULT << 7)           /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESM4                        (0x1UL << 8)                                                   /**< G_INCLUDESM4                                */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESM4_SHIFT                 8                                                              /**< Shift value for CRYPTOACC_G_INCLUDESM4      */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESM4_MASK                  0x100UL                                                        /**< Bit mask for CRYPTOACC_G_INCLUDESM4         */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESM4_DEFAULT               0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESM4_DEFAULT                (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESM4_DEFAULT << 8)           /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEPKE                        (0x1UL << 9)                                                   /**< G_INCLUDEPKE                                */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEPKE_SHIFT                 9                                                              /**< Shift value for CRYPTOACC_G_INCLUDEPKE      */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEPKE_MASK                  0x200UL                                                        /**< Bit mask for CRYPTOACC_G_INCLUDEPKE         */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEPKE_DEFAULT               0x00000001UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEPKE_DEFAULT                (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEPKE_DEFAULT << 9)           /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDENDRNG                      (0x1UL << 10)                                                  /**< G_INCLUDENDRNG                              */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDENDRNG_SHIFT               10                                                             /**< Shift value for CRYPTOACC_G_INCLUDENDRNG    */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDENDRNG_MASK                0x400UL                                                        /**< Bit mask for CRYPTOACC_G_INCLUDENDRNG       */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDENDRNG_DEFAULT             0x00000001UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDENDRNG_DEFAULT              (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDENDRNG_DEFAULT << 10)        /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHPCHACHAPOLY               (0x1UL << 11)                                                  /**< G_INCLUDEHPCHACHAPOLY                       */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHPCHACHAPOLY_SHIFT        11                                                             /**< Shift value for CRYPTOACC_G_INCLUDEHPCHACHAPOLY*/
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHPCHACHAPOLY_MASK         0x800UL                                                        /**< Bit mask for CRYPTOACC_G_INCLUDEHPCHACHAPOLY*/
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHPCHACHAPOLY_DEFAULT      0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHPCHACHAPOLY_DEFAULT       (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEHPCHACHAPOLY_DEFAULT << 11) /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESNOW3G                     (0x1UL << 12)                                                  /**< G_INCLUDESNOW3G                             */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESNOW3G_SHIFT              12                                                             /**< Shift value for CRYPTOACC_G_INCLUDESNOW3G   */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESNOW3G_MASK               0x1000UL                                                       /**< Bit mask for CRYPTOACC_G_INCLUDESNOW3G      */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESNOW3G_DEFAULT            0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESNOW3G_DEFAULT             (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDESNOW3G_DEFAULT << 12)       /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEKASUMI                     (0x1UL << 13)                                                  /**< G_INCLUDEKASUMI                             */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEKASUMI_SHIFT              13                                                             /**< Shift value for CRYPTOACC_G_INCLUDEKASUMI   */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEKASUMI_MASK               0x2000UL                                                       /**< Bit mask for CRYPTOACC_G_INCLUDEKASUMI      */
#define _CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEKASUMI_DEFAULT            0x00000000UL                                                   /**< Mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG    */
#define CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEKASUMI_DEFAULT             (_CRYPTOACC_INCLIPS_HWCFG_G_INCLUDEKASUMI_DEFAULT << 13)       /**< Shifted mode DEFAULT for CRYPTOACC_INCLIPS_HWCFG*/

/* Bit fields for CRYPTOACC BA411E_HWCFG1 */
#define _CRYPTOACC_BA411E_HWCFG1_RESETVALUE                         0x0703017FUL                                           /**< Default value for CRYPTOACC_BA411E_HWCFG1   */
#define _CRYPTOACC_BA411E_HWCFG1_MASK                               0x070301FFUL                                           /**< Mask for CRYPTOACC_BA411E_HWCFG1            */
#define _CRYPTOACC_BA411E_HWCFG1_G_AESMODESPOSS_SHIFT               0                                                      /**< Shift value for CRYPTOACC_G_AESMODESPOSS    */
#define _CRYPTOACC_BA411E_HWCFG1_G_AESMODESPOSS_MASK                0x1FFUL                                                /**< Bit mask for CRYPTOACC_G_AESMODESPOSS       */
#define _CRYPTOACC_BA411E_HWCFG1_G_AESMODESPOSS_DEFAULT             0x0000017FUL                                           /**< Mode DEFAULT for CRYPTOACC_BA411E_HWCFG1    */
#define CRYPTOACC_BA411E_HWCFG1_G_AESMODESPOSS_DEFAULT              (_CRYPTOACC_BA411E_HWCFG1_G_AESMODESPOSS_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_BA411E_HWCFG1*/
#define CRYPTOACC_BA411E_HWCFG1_G_CS                                (0x1UL << 16)                                          /**< G_CS                                        */
#define _CRYPTOACC_BA411E_HWCFG1_G_CS_SHIFT                         16                                                     /**< Shift value for CRYPTOACC_G_CS              */
#define _CRYPTOACC_BA411E_HWCFG1_G_CS_MASK                          0x10000UL                                              /**< Bit mask for CRYPTOACC_G_CS                 */
#define _CRYPTOACC_BA411E_HWCFG1_G_CS_DEFAULT                       0x00000001UL                                           /**< Mode DEFAULT for CRYPTOACC_BA411E_HWCFG1    */
#define CRYPTOACC_BA411E_HWCFG1_G_CS_DEFAULT                        (_CRYPTOACC_BA411E_HWCFG1_G_CS_DEFAULT << 16)          /**< Shifted mode DEFAULT for CRYPTOACC_BA411E_HWCFG1*/
#define CRYPTOACC_BA411E_HWCFG1_G_USEMASKING                        (0x1UL << 17)                                          /**< G_USEMASKING                                */
#define _CRYPTOACC_BA411E_HWCFG1_G_USEMASKING_SHIFT                 17                                                     /**< Shift value for CRYPTOACC_G_USEMASKING      */
#define _CRYPTOACC_BA411E_HWCFG1_G_USEMASKING_MASK                  0x20000UL                                              /**< Bit mask for CRYPTOACC_G_USEMASKING         */
#define _CRYPTOACC_BA411E_HWCFG1_G_USEMASKING_DEFAULT               0x00000001UL                                           /**< Mode DEFAULT for CRYPTOACC_BA411E_HWCFG1    */
#define CRYPTOACC_BA411E_HWCFG1_G_USEMASKING_DEFAULT                (_CRYPTOACC_BA411E_HWCFG1_G_USEMASKING_DEFAULT << 17)  /**< Shifted mode DEFAULT for CRYPTOACC_BA411E_HWCFG1*/
#define _CRYPTOACC_BA411E_HWCFG1_G_KEYSIZE_SHIFT                    24                                                     /**< Shift value for CRYPTOACC_G_KEYSIZE         */
#define _CRYPTOACC_BA411E_HWCFG1_G_KEYSIZE_MASK                     0x7000000UL                                            /**< Bit mask for CRYPTOACC_G_KEYSIZE            */
#define _CRYPTOACC_BA411E_HWCFG1_G_KEYSIZE_DEFAULT                  0x00000007UL                                           /**< Mode DEFAULT for CRYPTOACC_BA411E_HWCFG1    */
#define CRYPTOACC_BA411E_HWCFG1_G_KEYSIZE_DEFAULT                   (_CRYPTOACC_BA411E_HWCFG1_G_KEYSIZE_DEFAULT << 24)     /**< Shifted mode DEFAULT for CRYPTOACC_BA411E_HWCFG1*/

/* Bit fields for CRYPTOACC BA411E_HWCFG2 */
#define _CRYPTOACC_BA411E_HWCFG2_RESETVALUE                         0x00000080UL                                      /**< Default value for CRYPTOACC_BA411E_HWCFG2   */
#define _CRYPTOACC_BA411E_HWCFG2_MASK                               0x0000FFFFUL                                      /**< Mask for CRYPTOACC_BA411E_HWCFG2            */
#define _CRYPTOACC_BA411E_HWCFG2_G_CTRSIZE_SHIFT                    0                                                 /**< Shift value for CRYPTOACC_G_CTRSIZE         */
#define _CRYPTOACC_BA411E_HWCFG2_G_CTRSIZE_MASK                     0xFFFFUL                                          /**< Bit mask for CRYPTOACC_G_CTRSIZE            */
#define _CRYPTOACC_BA411E_HWCFG2_G_CTRSIZE_DEFAULT                  0x00000080UL                                      /**< Mode DEFAULT for CRYPTOACC_BA411E_HWCFG2    */
#define CRYPTOACC_BA411E_HWCFG2_G_CTRSIZE_DEFAULT                   (_CRYPTOACC_BA411E_HWCFG2_G_CTRSIZE_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_BA411E_HWCFG2*/

/* Bit fields for CRYPTOACC BA413_HWCFG */
#define _CRYPTOACC_BA413_HWCFG_RESETVALUE                           0x0003007FUL                                              /**< Default value for CRYPTOACC_BA413_HWCFG     */
#define _CRYPTOACC_BA413_HWCFG_MASK                                 0x0007007FUL                                              /**< Mask for CRYPTOACC_BA413_HWCFG              */
#define _CRYPTOACC_BA413_HWCFG_G_HASHMASKFUNC_SHIFT                 0                                                         /**< Shift value for CRYPTOACC_G_HASHMASKFUNC    */
#define _CRYPTOACC_BA413_HWCFG_G_HASHMASKFUNC_MASK                  0x7FUL                                                    /**< Bit mask for CRYPTOACC_G_HASHMASKFUNC       */
#define _CRYPTOACC_BA413_HWCFG_G_HASHMASKFUNC_DEFAULT               0x0000007FUL                                              /**< Mode DEFAULT for CRYPTOACC_BA413_HWCFG      */
#define CRYPTOACC_BA413_HWCFG_G_HASHMASKFUNC_DEFAULT                (_CRYPTOACC_BA413_HWCFG_G_HASHMASKFUNC_DEFAULT << 0)      /**< Shifted mode DEFAULT for CRYPTOACC_BA413_HWCFG*/
#define CRYPTOACC_BA413_HWCFG_G_HASHPADDING                         (0x1UL << 16)                                             /**< G_HASHPADDING                               */
#define _CRYPTOACC_BA413_HWCFG_G_HASHPADDING_SHIFT                  16                                                        /**< Shift value for CRYPTOACC_G_HASHPADDING     */
#define _CRYPTOACC_BA413_HWCFG_G_HASHPADDING_MASK                   0x10000UL                                                 /**< Bit mask for CRYPTOACC_G_HASHPADDING        */
#define _CRYPTOACC_BA413_HWCFG_G_HASHPADDING_DEFAULT                0x00000001UL                                              /**< Mode DEFAULT for CRYPTOACC_BA413_HWCFG      */
#define CRYPTOACC_BA413_HWCFG_G_HASHPADDING_DEFAULT                 (_CRYPTOACC_BA413_HWCFG_G_HASHPADDING_DEFAULT << 16)      /**< Shifted mode DEFAULT for CRYPTOACC_BA413_HWCFG*/
#define CRYPTOACC_BA413_HWCFG_G_HMAC_ENABLED                        (0x1UL << 17)                                             /**< G_HMAC_ENABLED                              */
#define _CRYPTOACC_BA413_HWCFG_G_HMAC_ENABLED_SHIFT                 17                                                        /**< Shift value for CRYPTOACC_G_HMAC_ENABLED    */
#define _CRYPTOACC_BA413_HWCFG_G_HMAC_ENABLED_MASK                  0x20000UL                                                 /**< Bit mask for CRYPTOACC_G_HMAC_ENABLED       */
#define _CRYPTOACC_BA413_HWCFG_G_HMAC_ENABLED_DEFAULT               0x00000001UL                                              /**< Mode DEFAULT for CRYPTOACC_BA413_HWCFG      */
#define CRYPTOACC_BA413_HWCFG_G_HMAC_ENABLED_DEFAULT                (_CRYPTOACC_BA413_HWCFG_G_HMAC_ENABLED_DEFAULT << 17)     /**< Shifted mode DEFAULT for CRYPTOACC_BA413_HWCFG*/
#define CRYPTOACC_BA413_HWCFG_G_HASHVERIFYDIGEST                    (0x1UL << 18)                                             /**< G_HASHVERIFYDIGEST                          */
#define _CRYPTOACC_BA413_HWCFG_G_HASHVERIFYDIGEST_SHIFT             18                                                        /**< Shift value for CRYPTOACC_G_HASHVERIFYDIGEST*/
#define _CRYPTOACC_BA413_HWCFG_G_HASHVERIFYDIGEST_MASK              0x40000UL                                                 /**< Bit mask for CRYPTOACC_G_HASHVERIFYDIGEST   */
#define _CRYPTOACC_BA413_HWCFG_G_HASHVERIFYDIGEST_DEFAULT           0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_BA413_HWCFG      */
#define CRYPTOACC_BA413_HWCFG_G_HASHVERIFYDIGEST_DEFAULT            (_CRYPTOACC_BA413_HWCFG_G_HASHVERIFYDIGEST_DEFAULT << 18) /**< Shifted mode DEFAULT for CRYPTOACC_BA413_HWCFG*/

/* Bit fields for CRYPTOACC BA418_HWCFG */
#define _CRYPTOACC_BA418_HWCFG_RESETVALUE                           0x00000001UL                                       /**< Default value for CRYPTOACC_BA418_HWCFG     */
#define _CRYPTOACC_BA418_HWCFG_MASK                                 0x00000001UL                                       /**< Mask for CRYPTOACC_BA418_HWCFG              */
#define CRYPTOACC_BA418_HWCFG_G_SHA3CTXTEN                          (0x1UL << 0)                                       /**< G_SHA3CTXTEN                                */
#define _CRYPTOACC_BA418_HWCFG_G_SHA3CTXTEN_SHIFT                   0                                                  /**< Shift value for CRYPTOACC_G_SHA3CTXTEN      */
#define _CRYPTOACC_BA418_HWCFG_G_SHA3CTXTEN_MASK                    0x1UL                                              /**< Bit mask for CRYPTOACC_G_SHA3CTXTEN         */
#define _CRYPTOACC_BA418_HWCFG_G_SHA3CTXTEN_DEFAULT                 0x00000001UL                                       /**< Mode DEFAULT for CRYPTOACC_BA418_HWCFG      */
#define CRYPTOACC_BA418_HWCFG_G_SHA3CTXTEN_DEFAULT                  (_CRYPTOACC_BA418_HWCFG_G_SHA3CTXTEN_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_BA418_HWCFG*/

/* Bit fields for CRYPTOACC BA419_HWCFG */
#define _CRYPTOACC_BA419_HWCFG_RESETVALUE                           0x00000000UL                                         /**< Default value for CRYPTOACC_BA419_HWCFG     */
#define _CRYPTOACC_BA419_HWCFG_MASK                                 0x0000007FUL                                         /**< Mask for CRYPTOACC_BA419_HWCFG              */
#define _CRYPTOACC_BA419_HWCFG_G_SM4MODESPOSS_SHIFT                 0                                                    /**< Shift value for CRYPTOACC_G_SM4MODESPOSS    */
#define _CRYPTOACC_BA419_HWCFG_G_SM4MODESPOSS_MASK                  0x7FUL                                               /**< Bit mask for CRYPTOACC_G_SM4MODESPOSS       */
#define _CRYPTOACC_BA419_HWCFG_G_SM4MODESPOSS_DEFAULT               0x00000000UL                                         /**< Mode DEFAULT for CRYPTOACC_BA419_HWCFG      */
#define CRYPTOACC_BA419_HWCFG_G_SM4MODESPOSS_DEFAULT                (_CRYPTOACC_BA419_HWCFG_G_SM4MODESPOSS_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_BA419_HWCFG*/

/* Bit fields for CRYPTOACC NDRNG_CONTROL */
#define _CRYPTOACC_NDRNG_CONTROL_RESETVALUE                         0x00000000UL                                              /**< Default value for CRYPTOACC_NDRNG_CONTROL   */
#define _CRYPTOACC_NDRNG_CONTROL_MASK                               0x001FFFFFUL                                              /**< Mask for CRYPTOACC_NDRNG_CONTROL            */
#define CRYPTOACC_NDRNG_CONTROL_ENABLE                              (0x1UL << 0)                                              /**< ENABLE                                      */
#define _CRYPTOACC_NDRNG_CONTROL_ENABLE_SHIFT                       0                                                         /**< Shift value for CRYPTOACC_ENABLE            */
#define _CRYPTOACC_NDRNG_CONTROL_ENABLE_MASK                        0x1UL                                                     /**< Bit mask for CRYPTOACC_ENABLE               */
#define _CRYPTOACC_NDRNG_CONTROL_ENABLE_DEFAULT                     0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_ENABLE_DEFAULT                      (_CRYPTOACC_NDRNG_CONTROL_ENABLE_DEFAULT << 0)            /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_LFSREN                              (0x1UL << 1)                                              /**< LFSREN                                      */
#define _CRYPTOACC_NDRNG_CONTROL_LFSREN_SHIFT                       1                                                         /**< Shift value for CRYPTOACC_LFSREN            */
#define _CRYPTOACC_NDRNG_CONTROL_LFSREN_MASK                        0x2UL                                                     /**< Bit mask for CRYPTOACC_LFSREN               */
#define _CRYPTOACC_NDRNG_CONTROL_LFSREN_DEFAULT                     0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_LFSREN_DEFAULT                      (_CRYPTOACC_NDRNG_CONTROL_LFSREN_DEFAULT << 1)            /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_TESTEN                              (0x1UL << 2)                                              /**< TESTEN                                      */
#define _CRYPTOACC_NDRNG_CONTROL_TESTEN_SHIFT                       2                                                         /**< Shift value for CRYPTOACC_TESTEN            */
#define _CRYPTOACC_NDRNG_CONTROL_TESTEN_MASK                        0x4UL                                                     /**< Bit mask for CRYPTOACC_TESTEN               */
#define _CRYPTOACC_NDRNG_CONTROL_TESTEN_DEFAULT                     0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_TESTEN_DEFAULT                      (_CRYPTOACC_NDRNG_CONTROL_TESTEN_DEFAULT << 2)            /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_CONDBYPASS                          (0x1UL << 3)                                              /**< CONDBYPASS                                  */
#define _CRYPTOACC_NDRNG_CONTROL_CONDBYPASS_SHIFT                   3                                                         /**< Shift value for CRYPTOACC_CONDBYPASS        */
#define _CRYPTOACC_NDRNG_CONTROL_CONDBYPASS_MASK                    0x8UL                                                     /**< Bit mask for CRYPTOACC_CONDBYPASS           */
#define _CRYPTOACC_NDRNG_CONTROL_CONDBYPASS_DEFAULT                 0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_CONDBYPASS_DEFAULT                  (_CRYPTOACC_NDRNG_CONTROL_CONDBYPASS_DEFAULT << 3)        /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_INTENREP                            (0x1UL << 4)                                              /**< INTENREP                                    */
#define _CRYPTOACC_NDRNG_CONTROL_INTENREP_SHIFT                     4                                                         /**< Shift value for CRYPTOACC_INTENREP          */
#define _CRYPTOACC_NDRNG_CONTROL_INTENREP_MASK                      0x10UL                                                    /**< Bit mask for CRYPTOACC_INTENREP             */
#define _CRYPTOACC_NDRNG_CONTROL_INTENREP_DEFAULT                   0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_INTENREP_DEFAULT                    (_CRYPTOACC_NDRNG_CONTROL_INTENREP_DEFAULT << 4)          /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_INTENPROP                           (0x1UL << 5)                                              /**< INTENPROP                                   */
#define _CRYPTOACC_NDRNG_CONTROL_INTENPROP_SHIFT                    5                                                         /**< Shift value for CRYPTOACC_INTENPROP         */
#define _CRYPTOACC_NDRNG_CONTROL_INTENPROP_MASK                     0x20UL                                                    /**< Bit mask for CRYPTOACC_INTENPROP            */
#define _CRYPTOACC_NDRNG_CONTROL_INTENPROP_DEFAULT                  0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_INTENPROP_DEFAULT                   (_CRYPTOACC_NDRNG_CONTROL_INTENPROP_DEFAULT << 5)         /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_RESERVED                            (0x1UL << 6)                                              /**< RESERVED                                    */
#define _CRYPTOACC_NDRNG_CONTROL_RESERVED_SHIFT                     6                                                         /**< Shift value for CRYPTOACC_RESERVED          */
#define _CRYPTOACC_NDRNG_CONTROL_RESERVED_MASK                      0x40UL                                                    /**< Bit mask for CRYPTOACC_RESERVED             */
#define _CRYPTOACC_NDRNG_CONTROL_RESERVED_DEFAULT                   0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_RESERVED_DEFAULT                    (_CRYPTOACC_NDRNG_CONTROL_RESERVED_DEFAULT << 6)          /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_INTENFULL                           (0x1UL << 7)                                              /**< INTENFULL                                   */
#define _CRYPTOACC_NDRNG_CONTROL_INTENFULL_SHIFT                    7                                                         /**< Shift value for CRYPTOACC_INTENFULL         */
#define _CRYPTOACC_NDRNG_CONTROL_INTENFULL_MASK                     0x80UL                                                    /**< Bit mask for CRYPTOACC_INTENFULL            */
#define _CRYPTOACC_NDRNG_CONTROL_INTENFULL_DEFAULT                  0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_INTENFULL_DEFAULT                   (_CRYPTOACC_NDRNG_CONTROL_INTENFULL_DEFAULT << 7)         /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_SOFTRST                             (0x1UL << 8)                                              /**< SOFTRST                                     */
#define _CRYPTOACC_NDRNG_CONTROL_SOFTRST_SHIFT                      8                                                         /**< Shift value for CRYPTOACC_SOFTRST           */
#define _CRYPTOACC_NDRNG_CONTROL_SOFTRST_MASK                       0x100UL                                                   /**< Bit mask for CRYPTOACC_SOFTRST              */
#define _CRYPTOACC_NDRNG_CONTROL_SOFTRST_DEFAULT                    0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_SOFTRST_DEFAULT                     (_CRYPTOACC_NDRNG_CONTROL_SOFTRST_DEFAULT << 8)           /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_INTENPRE                            (0x1UL << 9)                                              /**< INTENPRE                                    */
#define _CRYPTOACC_NDRNG_CONTROL_INTENPRE_SHIFT                     9                                                         /**< Shift value for CRYPTOACC_INTENPRE          */
#define _CRYPTOACC_NDRNG_CONTROL_INTENPRE_MASK                      0x200UL                                                   /**< Bit mask for CRYPTOACC_INTENPRE             */
#define _CRYPTOACC_NDRNG_CONTROL_INTENPRE_DEFAULT                   0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_INTENPRE_DEFAULT                    (_CRYPTOACC_NDRNG_CONTROL_INTENPRE_DEFAULT << 9)          /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_INTENALM                            (0x1UL << 10)                                             /**< INTENALM                                    */
#define _CRYPTOACC_NDRNG_CONTROL_INTENALM_SHIFT                     10                                                        /**< Shift value for CRYPTOACC_INTENALM          */
#define _CRYPTOACC_NDRNG_CONTROL_INTENALM_MASK                      0x400UL                                                   /**< Bit mask for CRYPTOACC_INTENALM             */
#define _CRYPTOACC_NDRNG_CONTROL_INTENALM_DEFAULT                   0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_INTENALM_DEFAULT                    (_CRYPTOACC_NDRNG_CONTROL_INTENALM_DEFAULT << 10)         /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_FORCEACTIVEROS                      (0x1UL << 11)                                             /**< FORCEACTIVEROS                              */
#define _CRYPTOACC_NDRNG_CONTROL_FORCEACTIVEROS_SHIFT               11                                                        /**< Shift value for CRYPTOACC_FORCEACTIVEROS    */
#define _CRYPTOACC_NDRNG_CONTROL_FORCEACTIVEROS_MASK                0x800UL                                                   /**< Bit mask for CRYPTOACC_FORCEACTIVEROS       */
#define _CRYPTOACC_NDRNG_CONTROL_FORCEACTIVEROS_DEFAULT             0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_FORCEACTIVEROS_DEFAULT              (_CRYPTOACC_NDRNG_CONTROL_FORCEACTIVEROS_DEFAULT << 11)   /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_HEALTHTESTBYPASS                    (0x1UL << 12)                                             /**< HEALTHTESTBYPASS                            */
#define _CRYPTOACC_NDRNG_CONTROL_HEALTHTESTBYPASS_SHIFT             12                                                        /**< Shift value for CRYPTOACC_HEALTHTESTBYPASS  */
#define _CRYPTOACC_NDRNG_CONTROL_HEALTHTESTBYPASS_MASK              0x1000UL                                                  /**< Bit mask for CRYPTOACC_HEALTHTESTBYPASS     */
#define _CRYPTOACC_NDRNG_CONTROL_HEALTHTESTBYPASS_DEFAULT           0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_HEALTHTESTBYPASS_DEFAULT            (_CRYPTOACC_NDRNG_CONTROL_HEALTHTESTBYPASS_DEFAULT << 12) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_AIS31BYPASS                         (0x1UL << 13)                                             /**< AIS31BYPASS                                 */
#define _CRYPTOACC_NDRNG_CONTROL_AIS31BYPASS_SHIFT                  13                                                        /**< Shift value for CRYPTOACC_AIS31BYPASS       */
#define _CRYPTOACC_NDRNG_CONTROL_AIS31BYPASS_MASK                   0x2000UL                                                  /**< Bit mask for CRYPTOACC_AIS31BYPASS          */
#define _CRYPTOACC_NDRNG_CONTROL_AIS31BYPASS_DEFAULT                0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_AIS31BYPASS_DEFAULT                 (_CRYPTOACC_NDRNG_CONTROL_AIS31BYPASS_DEFAULT << 13)      /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_HEALTHTESTSEL                       (0x1UL << 14)                                             /**< HEALTHTESTSEL                               */
#define _CRYPTOACC_NDRNG_CONTROL_HEALTHTESTSEL_SHIFT                14                                                        /**< Shift value for CRYPTOACC_HEALTHTESTSEL     */
#define _CRYPTOACC_NDRNG_CONTROL_HEALTHTESTSEL_MASK                 0x4000UL                                                  /**< Bit mask for CRYPTOACC_HEALTHTESTSEL        */
#define _CRYPTOACC_NDRNG_CONTROL_HEALTHTESTSEL_DEFAULT              0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_HEALTHTESTSEL_DEFAULT               (_CRYPTOACC_NDRNG_CONTROL_HEALTHTESTSEL_DEFAULT << 14)    /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_AIS31TESTSEL                        (0x1UL << 15)                                             /**< AIS31TESTSEL                                */
#define _CRYPTOACC_NDRNG_CONTROL_AIS31TESTSEL_SHIFT                 15                                                        /**< Shift value for CRYPTOACC_AIS31TESTSEL      */
#define _CRYPTOACC_NDRNG_CONTROL_AIS31TESTSEL_MASK                  0x8000UL                                                  /**< Bit mask for CRYPTOACC_AIS31TESTSEL         */
#define _CRYPTOACC_NDRNG_CONTROL_AIS31TESTSEL_DEFAULT               0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_AIS31TESTSEL_DEFAULT                (_CRYPTOACC_NDRNG_CONTROL_AIS31TESTSEL_DEFAULT << 15)     /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define _CRYPTOACC_NDRNG_CONTROL_NB128BITBLOCKS_SHIFT               16                                                        /**< Shift value for CRYPTOACC_NB128BITBLOCKS    */
#define _CRYPTOACC_NDRNG_CONTROL_NB128BITBLOCKS_MASK                0xF0000UL                                                 /**< Bit mask for CRYPTOACC_NB128BITBLOCKS       */
#define _CRYPTOACC_NDRNG_CONTROL_NB128BITBLOCKS_DEFAULT             0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_NB128BITBLOCKS_DEFAULT              (_CRYPTOACC_NDRNG_CONTROL_NB128BITBLOCKS_DEFAULT << 16)   /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/
#define CRYPTOACC_NDRNG_CONTROL_FIFOWRITESTARTUP                    (0x1UL << 20)                                             /**< FIFOWRITESTARTUP                            */
#define _CRYPTOACC_NDRNG_CONTROL_FIFOWRITESTARTUP_SHIFT             20                                                        /**< Shift value for CRYPTOACC_FIFOWRITESTARTUP  */
#define _CRYPTOACC_NDRNG_CONTROL_FIFOWRITESTARTUP_MASK              0x100000UL                                                /**< Bit mask for CRYPTOACC_FIFOWRITESTARTUP     */
#define _CRYPTOACC_NDRNG_CONTROL_FIFOWRITESTARTUP_DEFAULT           0x00000000UL                                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_CONTROL    */
#define CRYPTOACC_NDRNG_CONTROL_FIFOWRITESTARTUP_DEFAULT            (_CRYPTOACC_NDRNG_CONTROL_FIFOWRITESTARTUP_DEFAULT << 20) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CONTROL*/

/* Bit fields for CRYPTOACC NDRNG_FIFOLEVEL */
#define _CRYPTOACC_NDRNG_FIFOLEVEL_RESETVALUE                       0x00000000UL                                        /**< Default value for CRYPTOACC_NDRNG_FIFOLEVEL */
#define _CRYPTOACC_NDRNG_FIFOLEVEL_MASK                             0xFFFFFFFFUL                                        /**< Mask for CRYPTOACC_NDRNG_FIFOLEVEL          */
#define _CRYPTOACC_NDRNG_FIFOLEVEL_FIFOLEVEL_SHIFT                  0                                                   /**< Shift value for CRYPTOACC_FIFOLEVEL         */
#define _CRYPTOACC_NDRNG_FIFOLEVEL_FIFOLEVEL_MASK                   0xFFFFFFFFUL                                        /**< Bit mask for CRYPTOACC_FIFOLEVEL            */
#define _CRYPTOACC_NDRNG_FIFOLEVEL_FIFOLEVEL_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_FIFOLEVEL  */
#define CRYPTOACC_NDRNG_FIFOLEVEL_FIFOLEVEL_DEFAULT                 (_CRYPTOACC_NDRNG_FIFOLEVEL_FIFOLEVEL_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_FIFOLEVEL*/

/* Bit fields for CRYPTOACC NDRNG_FIFOTHRESH */
#define _CRYPTOACC_NDRNG_FIFOTHRESH_RESETVALUE                      0x00000000UL                                             /**< Default value for CRYPTOACC_NDRNG_FIFOTHRESH*/
#define _CRYPTOACC_NDRNG_FIFOTHRESH_MASK                            0xFFFFFFFFUL                                             /**< Mask for CRYPTOACC_NDRNG_FIFOTHRESH         */
#define _CRYPTOACC_NDRNG_FIFOTHRESH_FIFOTHRESHOLD_SHIFT             0                                                        /**< Shift value for CRYPTOACC_FIFOTHRESHOLD     */
#define _CRYPTOACC_NDRNG_FIFOTHRESH_FIFOTHRESHOLD_MASK              0xFFFFFFFFUL                                             /**< Bit mask for CRYPTOACC_FIFOTHRESHOLD        */
#define _CRYPTOACC_NDRNG_FIFOTHRESH_FIFOTHRESHOLD_DEFAULT           0x00000000UL                                             /**< Mode DEFAULT for CRYPTOACC_NDRNG_FIFOTHRESH */
#define CRYPTOACC_NDRNG_FIFOTHRESH_FIFOTHRESHOLD_DEFAULT            (_CRYPTOACC_NDRNG_FIFOTHRESH_FIFOTHRESHOLD_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_FIFOTHRESH*/

/* Bit fields for CRYPTOACC NDRNG_FIFODEPTH */
#define _CRYPTOACC_NDRNG_FIFODEPTH_RESETVALUE                       0x00000000UL                                        /**< Default value for CRYPTOACC_NDRNG_FIFODEPTH */
#define _CRYPTOACC_NDRNG_FIFODEPTH_MASK                             0xFFFFFFFFUL                                        /**< Mask for CRYPTOACC_NDRNG_FIFODEPTH          */
#define _CRYPTOACC_NDRNG_FIFODEPTH_FIFODEPTH_SHIFT                  0                                                   /**< Shift value for CRYPTOACC_FIFODEPTH         */
#define _CRYPTOACC_NDRNG_FIFODEPTH_FIFODEPTH_MASK                   0xFFFFFFFFUL                                        /**< Bit mask for CRYPTOACC_FIFODEPTH            */
#define _CRYPTOACC_NDRNG_FIFODEPTH_FIFODEPTH_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_FIFODEPTH  */
#define CRYPTOACC_NDRNG_FIFODEPTH_FIFODEPTH_DEFAULT                 (_CRYPTOACC_NDRNG_FIFODEPTH_FIFODEPTH_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_FIFODEPTH*/

/* Bit fields for CRYPTOACC NDRNG_KEY0 */
#define _CRYPTOACC_NDRNG_KEY0_RESETVALUE                            0x00000000UL                              /**< Default value for CRYPTOACC_NDRNG_KEY0      */
#define _CRYPTOACC_NDRNG_KEY0_MASK                                  0xFFFFFFFFUL                              /**< Mask for CRYPTOACC_NDRNG_KEY0               */
#define _CRYPTOACC_NDRNG_KEY0_KEY0_SHIFT                            0                                         /**< Shift value for CRYPTOACC_KEY0              */
#define _CRYPTOACC_NDRNG_KEY0_KEY0_MASK                             0xFFFFFFFFUL                              /**< Bit mask for CRYPTOACC_KEY0                 */
#define _CRYPTOACC_NDRNG_KEY0_KEY0_DEFAULT                          0x00000000UL                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_KEY0       */
#define CRYPTOACC_NDRNG_KEY0_KEY0_DEFAULT                           (_CRYPTOACC_NDRNG_KEY0_KEY0_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_KEY0*/

/* Bit fields for CRYPTOACC NDRNG_KEY1 */
#define _CRYPTOACC_NDRNG_KEY1_RESETVALUE                            0x00000000UL                              /**< Default value for CRYPTOACC_NDRNG_KEY1      */
#define _CRYPTOACC_NDRNG_KEY1_MASK                                  0xFFFFFFFFUL                              /**< Mask for CRYPTOACC_NDRNG_KEY1               */
#define _CRYPTOACC_NDRNG_KEY1_KEY1_SHIFT                            0                                         /**< Shift value for CRYPTOACC_KEY1              */
#define _CRYPTOACC_NDRNG_KEY1_KEY1_MASK                             0xFFFFFFFFUL                              /**< Bit mask for CRYPTOACC_KEY1                 */
#define _CRYPTOACC_NDRNG_KEY1_KEY1_DEFAULT                          0x00000000UL                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_KEY1       */
#define CRYPTOACC_NDRNG_KEY1_KEY1_DEFAULT                           (_CRYPTOACC_NDRNG_KEY1_KEY1_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_KEY1*/

/* Bit fields for CRYPTOACC NDRNG_KEY2 */
#define _CRYPTOACC_NDRNG_KEY2_RESETVALUE                            0x00000000UL                              /**< Default value for CRYPTOACC_NDRNG_KEY2      */
#define _CRYPTOACC_NDRNG_KEY2_MASK                                  0xFFFFFFFFUL                              /**< Mask for CRYPTOACC_NDRNG_KEY2               */
#define _CRYPTOACC_NDRNG_KEY2_KEY2_SHIFT                            0                                         /**< Shift value for CRYPTOACC_KEY2              */
#define _CRYPTOACC_NDRNG_KEY2_KEY2_MASK                             0xFFFFFFFFUL                              /**< Bit mask for CRYPTOACC_KEY2                 */
#define _CRYPTOACC_NDRNG_KEY2_KEY2_DEFAULT                          0x00000000UL                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_KEY2       */
#define CRYPTOACC_NDRNG_KEY2_KEY2_DEFAULT                           (_CRYPTOACC_NDRNG_KEY2_KEY2_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_KEY2*/

/* Bit fields for CRYPTOACC NDRNG_KEY3 */
#define _CRYPTOACC_NDRNG_KEY3_RESETVALUE                            0x00000000UL                              /**< Default value for CRYPTOACC_NDRNG_KEY3      */
#define _CRYPTOACC_NDRNG_KEY3_MASK                                  0xFFFFFFFFUL                              /**< Mask for CRYPTOACC_NDRNG_KEY3               */
#define _CRYPTOACC_NDRNG_KEY3_KEY3_SHIFT                            0                                         /**< Shift value for CRYPTOACC_KEY3              */
#define _CRYPTOACC_NDRNG_KEY3_KEY3_MASK                             0xFFFFFFFFUL                              /**< Bit mask for CRYPTOACC_KEY3                 */
#define _CRYPTOACC_NDRNG_KEY3_KEY3_DEFAULT                          0x00000000UL                              /**< Mode DEFAULT for CRYPTOACC_NDRNG_KEY3       */
#define CRYPTOACC_NDRNG_KEY3_KEY3_DEFAULT                           (_CRYPTOACC_NDRNG_KEY3_KEY3_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_KEY3*/

/* Bit fields for CRYPTOACC NDRNG_TESTDATA */
#define _CRYPTOACC_NDRNG_TESTDATA_RESETVALUE                        0x00000000UL                                      /**< Default value for CRYPTOACC_NDRNG_TESTDATA  */
#define _CRYPTOACC_NDRNG_TESTDATA_MASK                              0xFFFFFFFFUL                                      /**< Mask for CRYPTOACC_NDRNG_TESTDATA           */
#define _CRYPTOACC_NDRNG_TESTDATA_TESTDATA_SHIFT                    0                                                 /**< Shift value for CRYPTOACC_TESTDATA          */
#define _CRYPTOACC_NDRNG_TESTDATA_TESTDATA_MASK                     0xFFFFFFFFUL                                      /**< Bit mask for CRYPTOACC_TESTDATA             */
#define _CRYPTOACC_NDRNG_TESTDATA_TESTDATA_DEFAULT                  0x00000000UL                                      /**< Mode DEFAULT for CRYPTOACC_NDRNG_TESTDATA   */
#define CRYPTOACC_NDRNG_TESTDATA_TESTDATA_DEFAULT                   (_CRYPTOACC_NDRNG_TESTDATA_TESTDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_TESTDATA*/

/* Bit fields for CRYPTOACC NDRNG_REPTHRES */
#define _CRYPTOACC_NDRNG_REPTHRES_RESETVALUE                        0x00000029UL                                             /**< Default value for CRYPTOACC_NDRNG_REPTHRES  */
#define _CRYPTOACC_NDRNG_REPTHRES_MASK                              0x0000003FUL                                             /**< Mask for CRYPTOACC_NDRNG_REPTHRES           */
#define _CRYPTOACC_NDRNG_REPTHRES_REPEATTHRESHOLD_SHIFT             0                                                        /**< Shift value for CRYPTOACC_REPEATTHRESHOLD   */
#define _CRYPTOACC_NDRNG_REPTHRES_REPEATTHRESHOLD_MASK              0x3FUL                                                   /**< Bit mask for CRYPTOACC_REPEATTHRESHOLD      */
#define _CRYPTOACC_NDRNG_REPTHRES_REPEATTHRESHOLD_DEFAULT           0x00000029UL                                             /**< Mode DEFAULT for CRYPTOACC_NDRNG_REPTHRES   */
#define CRYPTOACC_NDRNG_REPTHRES_REPEATTHRESHOLD_DEFAULT            (_CRYPTOACC_NDRNG_REPTHRES_REPEATTHRESHOLD_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_REPTHRES*/

/* Bit fields for CRYPTOACC NDRNG_PROPTHRES */
#define _CRYPTOACC_NDRNG_PROPTHRES_RESETVALUE                       0x00000319UL                                            /**< Default value for CRYPTOACC_NDRNG_PROPTHRES */
#define _CRYPTOACC_NDRNG_PROPTHRES_MASK                             0x000003FFUL                                            /**< Mask for CRYPTOACC_NDRNG_PROPTHRES          */
#define _CRYPTOACC_NDRNG_PROPTHRES_PROPTHRESHOLD_SHIFT              0                                                       /**< Shift value for CRYPTOACC_PROPTHRESHOLD     */
#define _CRYPTOACC_NDRNG_PROPTHRES_PROPTHRESHOLD_MASK               0x3FFUL                                                 /**< Bit mask for CRYPTOACC_PROPTHRESHOLD        */
#define _CRYPTOACC_NDRNG_PROPTHRES_PROPTHRESHOLD_DEFAULT            0x00000319UL                                            /**< Mode DEFAULT for CRYPTOACC_NDRNG_PROPTHRES  */
#define CRYPTOACC_NDRNG_PROPTHRES_PROPTHRESHOLD_DEFAULT             (_CRYPTOACC_NDRNG_PROPTHRES_PROPTHRESHOLD_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_PROPTHRES*/

/* Bit fields for CRYPTOACC NDRNG_STATUS */
#define _CRYPTOACC_NDRNG_STATUS_RESETVALUE                          0x00000000UL                                        /**< Default value for CRYPTOACC_NDRNG_STATUS    */
#define _CRYPTOACC_NDRNG_STATUS_MASK                                0x00000FBFUL                                        /**< Mask for CRYPTOACC_NDRNG_STATUS             */
#define CRYPTOACC_NDRNG_STATUS_TESTDATABUSY                         (0x1UL << 0)                                        /**< TESTDATABUSY                                */
#define _CRYPTOACC_NDRNG_STATUS_TESTDATABUSY_SHIFT                  0                                                   /**< Shift value for CRYPTOACC_TESTDATABUSY      */
#define _CRYPTOACC_NDRNG_STATUS_TESTDATABUSY_MASK                   0x1UL                                               /**< Bit mask for CRYPTOACC_TESTDATABUSY         */
#define _CRYPTOACC_NDRNG_STATUS_TESTDATABUSY_DEFAULT                0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_STATUS     */
#define CRYPTOACC_NDRNG_STATUS_TESTDATABUSY_DEFAULT                 (_CRYPTOACC_NDRNG_STATUS_TESTDATABUSY_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_STATUS*/
#define _CRYPTOACC_NDRNG_STATUS_STATE_SHIFT                         1                                                   /**< Shift value for CRYPTOACC_STATE             */
#define _CRYPTOACC_NDRNG_STATUS_STATE_MASK                          0xEUL                                               /**< Bit mask for CRYPTOACC_STATE                */
#define _CRYPTOACC_NDRNG_STATUS_STATE_DEFAULT                       0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_STATUS     */
#define CRYPTOACC_NDRNG_STATUS_STATE_DEFAULT                        (_CRYPTOACC_NDRNG_STATUS_STATE_DEFAULT << 1)        /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_STATUS*/
#define CRYPTOACC_NDRNG_STATUS_REPFAIL                              (0x1UL << 4)                                        /**< REPFAIL                                     */
#define _CRYPTOACC_NDRNG_STATUS_REPFAIL_SHIFT                       4                                                   /**< Shift value for CRYPTOACC_REPFAIL           */
#define _CRYPTOACC_NDRNG_STATUS_REPFAIL_MASK                        0x10UL                                              /**< Bit mask for CRYPTOACC_REPFAIL              */
#define _CRYPTOACC_NDRNG_STATUS_REPFAIL_DEFAULT                     0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_STATUS     */
#define CRYPTOACC_NDRNG_STATUS_REPFAIL_DEFAULT                      (_CRYPTOACC_NDRNG_STATUS_REPFAIL_DEFAULT << 4)      /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_STATUS*/
#define CRYPTOACC_NDRNG_STATUS_PROPFAIL                             (0x1UL << 5)                                        /**< PROPFAIL                                    */
#define _CRYPTOACC_NDRNG_STATUS_PROPFAIL_SHIFT                      5                                                   /**< Shift value for CRYPTOACC_PROPFAIL          */
#define _CRYPTOACC_NDRNG_STATUS_PROPFAIL_MASK                       0x20UL                                              /**< Bit mask for CRYPTOACC_PROPFAIL             */
#define _CRYPTOACC_NDRNG_STATUS_PROPFAIL_DEFAULT                    0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_STATUS     */
#define CRYPTOACC_NDRNG_STATUS_PROPFAIL_DEFAULT                     (_CRYPTOACC_NDRNG_STATUS_PROPFAIL_DEFAULT << 5)     /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_STATUS*/
#define CRYPTOACC_NDRNG_STATUS_FULLINT                              (0x1UL << 7)                                        /**< FULLINT                                     */
#define _CRYPTOACC_NDRNG_STATUS_FULLINT_SHIFT                       7                                                   /**< Shift value for CRYPTOACC_FULLINT           */
#define _CRYPTOACC_NDRNG_STATUS_FULLINT_MASK                        0x80UL                                              /**< Bit mask for CRYPTOACC_FULLINT              */
#define _CRYPTOACC_NDRNG_STATUS_FULLINT_DEFAULT                     0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_STATUS     */
#define CRYPTOACC_NDRNG_STATUS_FULLINT_DEFAULT                      (_CRYPTOACC_NDRNG_STATUS_FULLINT_DEFAULT << 7)      /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_STATUS*/
#define CRYPTOACC_NDRNG_STATUS_PREINT                               (0x1UL << 8)                                        /**< PREINT                                      */
#define _CRYPTOACC_NDRNG_STATUS_PREINT_SHIFT                        8                                                   /**< Shift value for CRYPTOACC_PREINT            */
#define _CRYPTOACC_NDRNG_STATUS_PREINT_MASK                         0x100UL                                             /**< Bit mask for CRYPTOACC_PREINT               */
#define _CRYPTOACC_NDRNG_STATUS_PREINT_DEFAULT                      0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_STATUS     */
#define CRYPTOACC_NDRNG_STATUS_PREINT_DEFAULT                       (_CRYPTOACC_NDRNG_STATUS_PREINT_DEFAULT << 8)       /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_STATUS*/
#define CRYPTOACC_NDRNG_STATUS_ALMINT                               (0x1UL << 9)                                        /**< ALMINT                                      */
#define _CRYPTOACC_NDRNG_STATUS_ALMINT_SHIFT                        9                                                   /**< Shift value for CRYPTOACC_ALMINT            */
#define _CRYPTOACC_NDRNG_STATUS_ALMINT_MASK                         0x200UL                                             /**< Bit mask for CRYPTOACC_ALMINT               */
#define _CRYPTOACC_NDRNG_STATUS_ALMINT_DEFAULT                      0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_STATUS     */
#define CRYPTOACC_NDRNG_STATUS_ALMINT_DEFAULT                       (_CRYPTOACC_NDRNG_STATUS_ALMINT_DEFAULT << 9)       /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_STATUS*/
#define CRYPTOACC_NDRNG_STATUS_STARTUPFAIL                          (0x1UL << 10)                                       /**< STARTUPFAIL                                 */
#define _CRYPTOACC_NDRNG_STATUS_STARTUPFAIL_SHIFT                   10                                                  /**< Shift value for CRYPTOACC_STARTUPFAIL       */
#define _CRYPTOACC_NDRNG_STATUS_STARTUPFAIL_MASK                    0x400UL                                             /**< Bit mask for CRYPTOACC_STARTUPFAIL          */
#define _CRYPTOACC_NDRNG_STATUS_STARTUPFAIL_DEFAULT                 0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_STATUS     */
#define CRYPTOACC_NDRNG_STATUS_STARTUPFAIL_DEFAULT                  (_CRYPTOACC_NDRNG_STATUS_STARTUPFAIL_DEFAULT << 10) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_STATUS*/
#define CRYPTOACC_NDRNG_STATUS_FIFOACCFAIL                          (0x1UL << 11)                                       /**< FIFOACCFAIL                                 */
#define _CRYPTOACC_NDRNG_STATUS_FIFOACCFAIL_SHIFT                   11                                                  /**< Shift value for CRYPTOACC_FIFOACCFAIL       */
#define _CRYPTOACC_NDRNG_STATUS_FIFOACCFAIL_MASK                    0x800UL                                             /**< Bit mask for CRYPTOACC_FIFOACCFAIL          */
#define _CRYPTOACC_NDRNG_STATUS_FIFOACCFAIL_DEFAULT                 0x00000000UL                                        /**< Mode DEFAULT for CRYPTOACC_NDRNG_STATUS     */
#define CRYPTOACC_NDRNG_STATUS_FIFOACCFAIL_DEFAULT                  (_CRYPTOACC_NDRNG_STATUS_FIFOACCFAIL_DEFAULT << 11) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_STATUS*/

/* Bit fields for CRYPTOACC NDRNG_INITWAITVAL */
#define _CRYPTOACC_NDRNG_INITWAITVAL_RESETVALUE                     0x0000FFFFUL                                            /**< Default value for CRYPTOACC_NDRNG_INITWAITVAL*/
#define _CRYPTOACC_NDRNG_INITWAITVAL_MASK                           0x0000FFFFUL                                            /**< Mask for CRYPTOACC_NDRNG_INITWAITVAL        */
#define _CRYPTOACC_NDRNG_INITWAITVAL_INITWAITVAL_SHIFT              0                                                       /**< Shift value for CRYPTOACC_INITWAITVAL       */
#define _CRYPTOACC_NDRNG_INITWAITVAL_INITWAITVAL_MASK               0xFFFFUL                                                /**< Bit mask for CRYPTOACC_INITWAITVAL          */
#define _CRYPTOACC_NDRNG_INITWAITVAL_INITWAITVAL_DEFAULT            0x0000FFFFUL                                            /**< Mode DEFAULT for CRYPTOACC_NDRNG_INITWAITVAL*/
#define CRYPTOACC_NDRNG_INITWAITVAL_INITWAITVAL_DEFAULT             (_CRYPTOACC_NDRNG_INITWAITVAL_INITWAITVAL_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_INITWAITVAL*/

/* Bit fields for CRYPTOACC NDRNG_DISABLEOSC0 */
#define _CRYPTOACC_NDRNG_DISABLEOSC0_RESETVALUE                     0x00000000UL                                            /**< Default value for CRYPTOACC_NDRNG_DISABLEOSC0*/
#define _CRYPTOACC_NDRNG_DISABLEOSC0_MASK                           0xFFFFFFFFUL                                            /**< Mask for CRYPTOACC_NDRNG_DISABLEOSC0        */
#define _CRYPTOACC_NDRNG_DISABLEOSC0_DISABLEOSC0_SHIFT              0                                                       /**< Shift value for CRYPTOACC_DISABLEOSC0       */
#define _CRYPTOACC_NDRNG_DISABLEOSC0_DISABLEOSC0_MASK               0xFFFFFFFFUL                                            /**< Bit mask for CRYPTOACC_DISABLEOSC0          */
#define _CRYPTOACC_NDRNG_DISABLEOSC0_DISABLEOSC0_DEFAULT            0x00000000UL                                            /**< Mode DEFAULT for CRYPTOACC_NDRNG_DISABLEOSC0*/
#define CRYPTOACC_NDRNG_DISABLEOSC0_DISABLEOSC0_DEFAULT             (_CRYPTOACC_NDRNG_DISABLEOSC0_DISABLEOSC0_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_DISABLEOSC0*/

/* Bit fields for CRYPTOACC NDRNG_DISABLEOSC1 */
#define _CRYPTOACC_NDRNG_DISABLEOSC1_RESETVALUE                     0x00000000UL                                            /**< Default value for CRYPTOACC_NDRNG_DISABLEOSC1*/
#define _CRYPTOACC_NDRNG_DISABLEOSC1_MASK                           0xFFFFFFFFUL                                            /**< Mask for CRYPTOACC_NDRNG_DISABLEOSC1        */
#define _CRYPTOACC_NDRNG_DISABLEOSC1_DISABLEOSC1_SHIFT              0                                                       /**< Shift value for CRYPTOACC_DISABLEOSC1       */
#define _CRYPTOACC_NDRNG_DISABLEOSC1_DISABLEOSC1_MASK               0xFFFFFFFFUL                                            /**< Bit mask for CRYPTOACC_DISABLEOSC1          */
#define _CRYPTOACC_NDRNG_DISABLEOSC1_DISABLEOSC1_DEFAULT            0x00000000UL                                            /**< Mode DEFAULT for CRYPTOACC_NDRNG_DISABLEOSC1*/
#define CRYPTOACC_NDRNG_DISABLEOSC1_DISABLEOSC1_DEFAULT             (_CRYPTOACC_NDRNG_DISABLEOSC1_DISABLEOSC1_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_DISABLEOSC1*/

/* Bit fields for CRYPTOACC NDRNG_SWOFFTMRVAL */
#define _CRYPTOACC_NDRNG_SWOFFTMRVAL_RESETVALUE                     0x0000FFFFUL                                            /**< Default value for CRYPTOACC_NDRNG_SWOFFTMRVAL*/
#define _CRYPTOACC_NDRNG_SWOFFTMRVAL_MASK                           0x0000FFFFUL                                            /**< Mask for CRYPTOACC_NDRNG_SWOFFTMRVAL        */
#define _CRYPTOACC_NDRNG_SWOFFTMRVAL_SWOFFTMRVAL_SHIFT              0                                                       /**< Shift value for CRYPTOACC_SWOFFTMRVAL       */
#define _CRYPTOACC_NDRNG_SWOFFTMRVAL_SWOFFTMRVAL_MASK               0xFFFFUL                                                /**< Bit mask for CRYPTOACC_SWOFFTMRVAL          */
#define _CRYPTOACC_NDRNG_SWOFFTMRVAL_SWOFFTMRVAL_DEFAULT            0x0000FFFFUL                                            /**< Mode DEFAULT for CRYPTOACC_NDRNG_SWOFFTMRVAL*/
#define CRYPTOACC_NDRNG_SWOFFTMRVAL_SWOFFTMRVAL_DEFAULT             (_CRYPTOACC_NDRNG_SWOFFTMRVAL_SWOFFTMRVAL_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_SWOFFTMRVAL*/

/* Bit fields for CRYPTOACC NDRNG_CLKDIV */
#define _CRYPTOACC_NDRNG_CLKDIV_RESETVALUE                          0x00000000UL                                  /**< Default value for CRYPTOACC_NDRNG_CLKDIV    */
#define _CRYPTOACC_NDRNG_CLKDIV_MASK                                0x000000FFUL                                  /**< Mask for CRYPTOACC_NDRNG_CLKDIV             */
#define _CRYPTOACC_NDRNG_CLKDIV_CLKDIV_SHIFT                        0                                             /**< Shift value for CRYPTOACC_CLKDIV            */
#define _CRYPTOACC_NDRNG_CLKDIV_CLKDIV_MASK                         0xFFUL                                        /**< Bit mask for CRYPTOACC_CLKDIV               */
#define _CRYPTOACC_NDRNG_CLKDIV_CLKDIV_DEFAULT                      0x00000000UL                                  /**< Mode DEFAULT for CRYPTOACC_NDRNG_CLKDIV     */
#define CRYPTOACC_NDRNG_CLKDIV_CLKDIV_DEFAULT                       (_CRYPTOACC_NDRNG_CLKDIV_CLKDIV_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_CLKDIV*/

/* Bit fields for CRYPTOACC NDRNG_AIS31CONF0 */
#define _CRYPTOACC_NDRNG_AIS31CONF0_RESETVALUE                      0x00000000UL                                                /**< Default value for CRYPTOACC_NDRNG_AIS31CONF0*/
#define _CRYPTOACC_NDRNG_AIS31CONF0_MASK                            0x7FFF7FFFUL                                                /**< Mask for CRYPTOACC_NDRNG_AIS31CONF0         */
#define _CRYPTOACC_NDRNG_AIS31CONF0_STARTUPTHRESHOLD_SHIFT          0                                                           /**< Shift value for CRYPTOACC_STARTUPTHRESHOLD  */
#define _CRYPTOACC_NDRNG_AIS31CONF0_STARTUPTHRESHOLD_MASK           0x7FFFUL                                                    /**< Bit mask for CRYPTOACC_STARTUPTHRESHOLD     */
#define _CRYPTOACC_NDRNG_AIS31CONF0_STARTUPTHRESHOLD_DEFAULT        0x00000000UL                                                /**< Mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF0 */
#define CRYPTOACC_NDRNG_AIS31CONF0_STARTUPTHRESHOLD_DEFAULT         (_CRYPTOACC_NDRNG_AIS31CONF0_STARTUPTHRESHOLD_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF0*/
#define _CRYPTOACC_NDRNG_AIS31CONF0_ONLINETHRESHOLD_SHIFT           16                                                          /**< Shift value for CRYPTOACC_ONLINETHRESHOLD   */
#define _CRYPTOACC_NDRNG_AIS31CONF0_ONLINETHRESHOLD_MASK            0x7FFF0000UL                                                /**< Bit mask for CRYPTOACC_ONLINETHRESHOLD      */
#define _CRYPTOACC_NDRNG_AIS31CONF0_ONLINETHRESHOLD_DEFAULT         0x00000000UL                                                /**< Mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF0 */
#define CRYPTOACC_NDRNG_AIS31CONF0_ONLINETHRESHOLD_DEFAULT          (_CRYPTOACC_NDRNG_AIS31CONF0_ONLINETHRESHOLD_DEFAULT << 16) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF0*/

/* Bit fields for CRYPTOACC NDRNG_AIS31CONF1 */
#define _CRYPTOACC_NDRNG_AIS31CONF1_RESETVALUE                      0x00000000UL                                                  /**< Default value for CRYPTOACC_NDRNG_AIS31CONF1*/
#define _CRYPTOACC_NDRNG_AIS31CONF1_MASK                            0x7FFF7FFFUL                                                  /**< Mask for CRYPTOACC_NDRNG_AIS31CONF1         */
#define _CRYPTOACC_NDRNG_AIS31CONF1_ONLINEREPTHRESHOLD_SHIFT        0                                                             /**< Shift value for CRYPTOACC_ONLINEREPTHRESHOLD*/
#define _CRYPTOACC_NDRNG_AIS31CONF1_ONLINEREPTHRESHOLD_MASK         0x7FFFUL                                                      /**< Bit mask for CRYPTOACC_ONLINEREPTHRESHOLD   */
#define _CRYPTOACC_NDRNG_AIS31CONF1_ONLINEREPTHRESHOLD_DEFAULT      0x00000000UL                                                  /**< Mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF1 */
#define CRYPTOACC_NDRNG_AIS31CONF1_ONLINEREPTHRESHOLD_DEFAULT       (_CRYPTOACC_NDRNG_AIS31CONF1_ONLINEREPTHRESHOLD_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF1*/
#define _CRYPTOACC_NDRNG_AIS31CONF1_HEXPECTEDVALUE_SHIFT            16                                                            /**< Shift value for CRYPTOACC_HEXPECTEDVALUE    */
#define _CRYPTOACC_NDRNG_AIS31CONF1_HEXPECTEDVALUE_MASK             0x7FFF0000UL                                                  /**< Bit mask for CRYPTOACC_HEXPECTEDVALUE       */
#define _CRYPTOACC_NDRNG_AIS31CONF1_HEXPECTEDVALUE_DEFAULT          0x00000000UL                                                  /**< Mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF1 */
#define CRYPTOACC_NDRNG_AIS31CONF1_HEXPECTEDVALUE_DEFAULT           (_CRYPTOACC_NDRNG_AIS31CONF1_HEXPECTEDVALUE_DEFAULT << 16)    /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF1*/

/* Bit fields for CRYPTOACC NDRNG_AIS31CONF2 */
#define _CRYPTOACC_NDRNG_AIS31CONF2_RESETVALUE                      0x00000000UL                                     /**< Default value for CRYPTOACC_NDRNG_AIS31CONF2*/
#define _CRYPTOACC_NDRNG_AIS31CONF2_MASK                            0x7FFF7FFFUL                                     /**< Mask for CRYPTOACC_NDRNG_AIS31CONF2         */
#define _CRYPTOACC_NDRNG_AIS31CONF2_HMIN_SHIFT                      0                                                /**< Shift value for CRYPTOACC_HMIN              */
#define _CRYPTOACC_NDRNG_AIS31CONF2_HMIN_MASK                       0x7FFFUL                                         /**< Bit mask for CRYPTOACC_HMIN                 */
#define _CRYPTOACC_NDRNG_AIS31CONF2_HMIN_DEFAULT                    0x00000000UL                                     /**< Mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF2 */
#define CRYPTOACC_NDRNG_AIS31CONF2_HMIN_DEFAULT                     (_CRYPTOACC_NDRNG_AIS31CONF2_HMIN_DEFAULT << 0)  /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF2*/
#define _CRYPTOACC_NDRNG_AIS31CONF2_HMAX_SHIFT                      16                                               /**< Shift value for CRYPTOACC_HMAX              */
#define _CRYPTOACC_NDRNG_AIS31CONF2_HMAX_MASK                       0x7FFF0000UL                                     /**< Bit mask for CRYPTOACC_HMAX                 */
#define _CRYPTOACC_NDRNG_AIS31CONF2_HMAX_DEFAULT                    0x00000000UL                                     /**< Mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF2 */
#define CRYPTOACC_NDRNG_AIS31CONF2_HMAX_DEFAULT                     (_CRYPTOACC_NDRNG_AIS31CONF2_HMAX_DEFAULT << 16) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_AIS31CONF2*/

/* Bit fields for CRYPTOACC NDRNG_AIS31STATUS */
#define _CRYPTOACC_NDRNG_AIS31STATUS_RESETVALUE                     0x00000000UL                                                     /**< Default value for CRYPTOACC_NDRNG_AIS31STATUS*/
#define _CRYPTOACC_NDRNG_AIS31STATUS_MASK                           0x0003FFFFUL                                                     /**< Mask for CRYPTOACC_NDRNG_AIS31STATUS        */
#define _CRYPTOACC_NDRNG_AIS31STATUS_NUMPRELIMALARMS_SHIFT          0                                                                /**< Shift value for CRYPTOACC_NUMPRELIMALARMS   */
#define _CRYPTOACC_NDRNG_AIS31STATUS_NUMPRELIMALARMS_MASK           0xFFFFUL                                                         /**< Bit mask for CRYPTOACC_NUMPRELIMALARMS      */
#define _CRYPTOACC_NDRNG_AIS31STATUS_NUMPRELIMALARMS_DEFAULT        0x00000000UL                                                     /**< Mode DEFAULT for CRYPTOACC_NDRNG_AIS31STATUS*/
#define CRYPTOACC_NDRNG_AIS31STATUS_NUMPRELIMALARMS_DEFAULT         (_CRYPTOACC_NDRNG_AIS31STATUS_NUMPRELIMALARMS_DEFAULT << 0)      /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_AIS31STATUS*/
#define CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMRNG             (0x1UL << 16)                                                    /**< PRELIMNOISEALARMRNG                         */
#define _CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMRNG_SHIFT      16                                                               /**< Shift value for CRYPTOACC_PRELIMNOISEALARMRNG*/
#define _CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMRNG_MASK       0x10000UL                                                        /**< Bit mask for CRYPTOACC_PRELIMNOISEALARMRNG  */
#define _CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMRNG_DEFAULT    0x00000000UL                                                     /**< Mode DEFAULT for CRYPTOACC_NDRNG_AIS31STATUS*/
#define CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMRNG_DEFAULT     (_CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMRNG_DEFAULT << 16) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_AIS31STATUS*/
#define CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMREP             (0x1UL << 17)                                                    /**< PRELIMNOISEALARMREP                         */
#define _CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMREP_SHIFT      17                                                               /**< Shift value for CRYPTOACC_PRELIMNOISEALARMREP*/
#define _CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMREP_MASK       0x20000UL                                                        /**< Bit mask for CRYPTOACC_PRELIMNOISEALARMREP  */
#define _CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMREP_DEFAULT    0x00000000UL                                                     /**< Mode DEFAULT for CRYPTOACC_NDRNG_AIS31STATUS*/
#define CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMREP_DEFAULT     (_CRYPTOACC_NDRNG_AIS31STATUS_PRELIMNOISEALARMREP_DEFAULT << 17) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_AIS31STATUS*/

/* Bit fields for CRYPTOACC NDRNG_HWCONFIG */
#define _CRYPTOACC_NDRNG_HWCONFIG_RESETVALUE                        0x00000317UL                                         /**< Default value for CRYPTOACC_NDRNG_HWCONFIG  */
#define _CRYPTOACC_NDRNG_HWCONFIG_MASK                              0x000003FFUL                                         /**< Mask for CRYPTOACC_NDRNG_HWCONFIG           */
#define _CRYPTOACC_NDRNG_HWCONFIG_NUMBOFRINGS_SHIFT                 0                                                    /**< Shift value for CRYPTOACC_NUMBOFRINGS       */
#define _CRYPTOACC_NDRNG_HWCONFIG_NUMBOFRINGS_MASK                  0xFFUL                                               /**< Bit mask for CRYPTOACC_NUMBOFRINGS          */
#define _CRYPTOACC_NDRNG_HWCONFIG_NUMBOFRINGS_DEFAULT               0x00000017UL                                         /**< Mode DEFAULT for CRYPTOACC_NDRNG_HWCONFIG   */
#define CRYPTOACC_NDRNG_HWCONFIG_NUMBOFRINGS_DEFAULT                (_CRYPTOACC_NDRNG_HWCONFIG_NUMBOFRINGS_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_HWCONFIG*/
#define CRYPTOACC_NDRNG_HWCONFIG_AIS31                              (0x1UL << 8)                                         /**< AIS31                                       */
#define _CRYPTOACC_NDRNG_HWCONFIG_AIS31_SHIFT                       8                                                    /**< Shift value for CRYPTOACC_AIS31             */
#define _CRYPTOACC_NDRNG_HWCONFIG_AIS31_MASK                        0x100UL                                              /**< Bit mask for CRYPTOACC_AIS31                */
#define _CRYPTOACC_NDRNG_HWCONFIG_AIS31_DEFAULT                     0x00000001UL                                         /**< Mode DEFAULT for CRYPTOACC_NDRNG_HWCONFIG   */
#define CRYPTOACC_NDRNG_HWCONFIG_AIS31_DEFAULT                      (_CRYPTOACC_NDRNG_HWCONFIG_AIS31_DEFAULT << 8)       /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_HWCONFIG*/
#define CRYPTOACC_NDRNG_HWCONFIG_AIS31FULL                          (0x1UL << 9)                                         /**< AIS31FULL                                   */
#define _CRYPTOACC_NDRNG_HWCONFIG_AIS31FULL_SHIFT                   9                                                    /**< Shift value for CRYPTOACC_AIS31FULL         */
#define _CRYPTOACC_NDRNG_HWCONFIG_AIS31FULL_MASK                    0x200UL                                              /**< Bit mask for CRYPTOACC_AIS31FULL            */
#define _CRYPTOACC_NDRNG_HWCONFIG_AIS31FULL_DEFAULT                 0x00000001UL                                         /**< Mode DEFAULT for CRYPTOACC_NDRNG_HWCONFIG   */
#define CRYPTOACC_NDRNG_HWCONFIG_AIS31FULL_DEFAULT                  (_CRYPTOACC_NDRNG_HWCONFIG_AIS31FULL_DEFAULT << 9)   /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_HWCONFIG*/

/* Bit fields for CRYPTOACC NDRNG_FIFOOUTPUTDATA */
#define _CRYPTOACC_NDRNG_FIFOOUTPUTDATA_RESETVALUE                  0x00000000UL                                                  /**< Default value for CRYPTOACC_NDRNG_FIFOOUTPUTDATA*/
#define _CRYPTOACC_NDRNG_FIFOOUTPUTDATA_MASK                        0xFFFFFFFFUL                                                  /**< Mask for CRYPTOACC_NDRNG_FIFOOUTPUTDATA     */
#define _CRYPTOACC_NDRNG_FIFOOUTPUTDATA_FIFOOUTPUTDATA_SHIFT        0                                                             /**< Shift value for CRYPTOACC_FIFOOUTPUTDATA    */
#define _CRYPTOACC_NDRNG_FIFOOUTPUTDATA_FIFOOUTPUTDATA_MASK         0xFFFFFFFFUL                                                  /**< Bit mask for CRYPTOACC_FIFOOUTPUTDATA       */
#define _CRYPTOACC_NDRNG_FIFOOUTPUTDATA_FIFOOUTPUTDATA_DEFAULT      0x00000000UL                                                  /**< Mode DEFAULT for CRYPTOACC_NDRNG_FIFOOUTPUTDATA*/
#define CRYPTOACC_NDRNG_FIFOOUTPUTDATA_FIFOOUTPUTDATA_DEFAULT       (_CRYPTOACC_NDRNG_FIFOOUTPUTDATA_FIFOOUTPUTDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_NDRNG_FIFOOUTPUTDATA*/

/* Bit fields for CRYPTOACC PK_POINTERS */
#define _CRYPTOACC_PK_POINTERS_RESETVALUE                           0x00000000UL                                  /**< Default value for CRYPTOACC_PK_POINTERS     */
#define _CRYPTOACC_PK_POINTERS_MASK                                 0x0F0F0F0FUL                                  /**< Mask for CRYPTOACC_PK_POINTERS              */
#define _CRYPTOACC_PK_POINTERS_OPPTRA_SHIFT                         0                                             /**< Shift value for CRYPTOACC_OPPTRA            */
#define _CRYPTOACC_PK_POINTERS_OPPTRA_MASK                          0xFUL                                         /**< Bit mask for CRYPTOACC_OPPTRA               */
#define _CRYPTOACC_PK_POINTERS_OPPTRA_DEFAULT                       0x00000000UL                                  /**< Mode DEFAULT for CRYPTOACC_PK_POINTERS      */
#define CRYPTOACC_PK_POINTERS_OPPTRA_DEFAULT                        (_CRYPTOACC_PK_POINTERS_OPPTRA_DEFAULT << 0)  /**< Shifted mode DEFAULT for CRYPTOACC_PK_POINTERS*/
#define _CRYPTOACC_PK_POINTERS_OPPTRB_SHIFT                         8                                             /**< Shift value for CRYPTOACC_OPPTRB            */
#define _CRYPTOACC_PK_POINTERS_OPPTRB_MASK                          0xF00UL                                       /**< Bit mask for CRYPTOACC_OPPTRB               */
#define _CRYPTOACC_PK_POINTERS_OPPTRB_DEFAULT                       0x00000000UL                                  /**< Mode DEFAULT for CRYPTOACC_PK_POINTERS      */
#define CRYPTOACC_PK_POINTERS_OPPTRB_DEFAULT                        (_CRYPTOACC_PK_POINTERS_OPPTRB_DEFAULT << 8)  /**< Shifted mode DEFAULT for CRYPTOACC_PK_POINTERS*/
#define _CRYPTOACC_PK_POINTERS_OPPTRC_SHIFT                         16                                            /**< Shift value for CRYPTOACC_OPPTRC            */
#define _CRYPTOACC_PK_POINTERS_OPPTRC_MASK                          0xF0000UL                                     /**< Bit mask for CRYPTOACC_OPPTRC               */
#define _CRYPTOACC_PK_POINTERS_OPPTRC_DEFAULT                       0x00000000UL                                  /**< Mode DEFAULT for CRYPTOACC_PK_POINTERS      */
#define CRYPTOACC_PK_POINTERS_OPPTRC_DEFAULT                        (_CRYPTOACC_PK_POINTERS_OPPTRC_DEFAULT << 16) /**< Shifted mode DEFAULT for CRYPTOACC_PK_POINTERS*/
#define _CRYPTOACC_PK_POINTERS_OPPTRN_SHIFT                         24                                            /**< Shift value for CRYPTOACC_OPPTRN            */
#define _CRYPTOACC_PK_POINTERS_OPPTRN_MASK                          0xF000000UL                                   /**< Bit mask for CRYPTOACC_OPPTRN               */
#define _CRYPTOACC_PK_POINTERS_OPPTRN_DEFAULT                       0x00000000UL                                  /**< Mode DEFAULT for CRYPTOACC_PK_POINTERS      */
#define CRYPTOACC_PK_POINTERS_OPPTRN_DEFAULT                        (_CRYPTOACC_PK_POINTERS_OPPTRN_DEFAULT << 24) /**< Shifted mode DEFAULT for CRYPTOACC_PK_POINTERS*/

/* Bit fields for CRYPTOACC PK_COMMAND */
#define _CRYPTOACC_PK_COMMAND_RESETVALUE                            0x00000000UL                                    /**< Default value for CRYPTOACC_PK_COMMAND      */
#define _CRYPTOACC_PK_COMMAND_MASK                                  0xFF77FFFFUL                                    /**< Mask for CRYPTOACC_PK_COMMAND               */
#define _CRYPTOACC_PK_COMMAND_OPEADDR_SHIFT                         0                                               /**< Shift value for CRYPTOACC_OPEADDR           */
#define _CRYPTOACC_PK_COMMAND_OPEADDR_MASK                          0x7FUL                                          /**< Bit mask for CRYPTOACC_OPEADDR              */
#define _CRYPTOACC_PK_COMMAND_OPEADDR_DEFAULT                       0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_OPEADDR_DEFAULT                        (_CRYPTOACC_PK_COMMAND_OPEADDR_DEFAULT << 0)    /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define CRYPTOACC_PK_COMMAND_FIELDF                                 (0x1UL << 7)                                    /**< FIELDF                                      */
#define _CRYPTOACC_PK_COMMAND_FIELDF_SHIFT                          7                                               /**< Shift value for CRYPTOACC_FIELDF            */
#define _CRYPTOACC_PK_COMMAND_FIELDF_MASK                           0x80UL                                          /**< Bit mask for CRYPTOACC_FIELDF               */
#define _CRYPTOACC_PK_COMMAND_FIELDF_DEFAULT                        0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_FIELDF_DEFAULT                         (_CRYPTOACC_PK_COMMAND_FIELDF_DEFAULT << 7)     /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define _CRYPTOACC_PK_COMMAND_OPBYTESM1_SHIFT                       8                                               /**< Shift value for CRYPTOACC_OPBYTESM1         */
#define _CRYPTOACC_PK_COMMAND_OPBYTESM1_MASK                        0x7FF00UL                                       /**< Bit mask for CRYPTOACC_OPBYTESM1            */
#define _CRYPTOACC_PK_COMMAND_OPBYTESM1_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_OPBYTESM1_DEFAULT                      (_CRYPTOACC_PK_COMMAND_OPBYTESM1_DEFAULT << 8)  /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define _CRYPTOACC_PK_COMMAND_SELCURVE_SHIFT                        20                                              /**< Shift value for CRYPTOACC_SELCURVE          */
#define _CRYPTOACC_PK_COMMAND_SELCURVE_MASK                         0x700000UL                                      /**< Bit mask for CRYPTOACC_SELCURVE             */
#define _CRYPTOACC_PK_COMMAND_SELCURVE_DEFAULT                      0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_SELCURVE_DEFAULT                       (_CRYPTOACC_PK_COMMAND_SELCURVE_DEFAULT << 20)  /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define CRYPTOACC_PK_COMMAND_RANDKE                                 (0x1UL << 24)                                   /**< RANDKE                                      */
#define _CRYPTOACC_PK_COMMAND_RANDKE_SHIFT                          24                                              /**< Shift value for CRYPTOACC_RANDKE            */
#define _CRYPTOACC_PK_COMMAND_RANDKE_MASK                           0x1000000UL                                     /**< Bit mask for CRYPTOACC_RANDKE               */
#define _CRYPTOACC_PK_COMMAND_RANDKE_DEFAULT                        0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_RANDKE_DEFAULT                         (_CRYPTOACC_PK_COMMAND_RANDKE_DEFAULT << 24)    /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define CRYPTOACC_PK_COMMAND_RANDPROJ                               (0x1UL << 25)                                   /**< RANDPROJ                                    */
#define _CRYPTOACC_PK_COMMAND_RANDPROJ_SHIFT                        25                                              /**< Shift value for CRYPTOACC_RANDPROJ          */
#define _CRYPTOACC_PK_COMMAND_RANDPROJ_MASK                         0x2000000UL                                     /**< Bit mask for CRYPTOACC_RANDPROJ             */
#define _CRYPTOACC_PK_COMMAND_RANDPROJ_DEFAULT                      0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_RANDPROJ_DEFAULT                       (_CRYPTOACC_PK_COMMAND_RANDPROJ_DEFAULT << 25)  /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define CRYPTOACC_PK_COMMAND_EDWARDS                                (0x1UL << 26)                                   /**< EDWARDS                                     */
#define _CRYPTOACC_PK_COMMAND_EDWARDS_SHIFT                         26                                              /**< Shift value for CRYPTOACC_EDWARDS           */
#define _CRYPTOACC_PK_COMMAND_EDWARDS_MASK                          0x4000000UL                                     /**< Bit mask for CRYPTOACC_EDWARDS              */
#define _CRYPTOACC_PK_COMMAND_EDWARDS_DEFAULT                       0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_EDWARDS_DEFAULT                        (_CRYPTOACC_PK_COMMAND_EDWARDS_DEFAULT << 26)   /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define CRYPTOACC_PK_COMMAND_BUFSEL                                 (0x1UL << 27)                                   /**< BUFSEL                                      */
#define _CRYPTOACC_PK_COMMAND_BUFSEL_SHIFT                          27                                              /**< Shift value for CRYPTOACC_BUFSEL            */
#define _CRYPTOACC_PK_COMMAND_BUFSEL_MASK                           0x8000000UL                                     /**< Bit mask for CRYPTOACC_BUFSEL               */
#define _CRYPTOACC_PK_COMMAND_BUFSEL_DEFAULT                        0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_BUFSEL_DEFAULT                         (_CRYPTOACC_PK_COMMAND_BUFSEL_DEFAULT << 27)    /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define CRYPTOACC_PK_COMMAND_SWAPBYTES                              (0x1UL << 28)                                   /**< SWAPBYTES                                   */
#define _CRYPTOACC_PK_COMMAND_SWAPBYTES_SHIFT                       28                                              /**< Shift value for CRYPTOACC_SWAPBYTES         */
#define _CRYPTOACC_PK_COMMAND_SWAPBYTES_MASK                        0x10000000UL                                    /**< Bit mask for CRYPTOACC_SWAPBYTES            */
#define _CRYPTOACC_PK_COMMAND_SWAPBYTES_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_SWAPBYTES_DEFAULT                      (_CRYPTOACC_PK_COMMAND_SWAPBYTES_DEFAULT << 28) /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define CRYPTOACC_PK_COMMAND_FLAGA                                  (0x1UL << 29)                                   /**< FLAGA                                       */
#define _CRYPTOACC_PK_COMMAND_FLAGA_SHIFT                           29                                              /**< Shift value for CRYPTOACC_FLAGA             */
#define _CRYPTOACC_PK_COMMAND_FLAGA_MASK                            0x20000000UL                                    /**< Bit mask for CRYPTOACC_FLAGA                */
#define _CRYPTOACC_PK_COMMAND_FLAGA_DEFAULT                         0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_FLAGA_DEFAULT                          (_CRYPTOACC_PK_COMMAND_FLAGA_DEFAULT << 29)     /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define CRYPTOACC_PK_COMMAND_FLAGB                                  (0x1UL << 30)                                   /**< FLAGB                                       */
#define _CRYPTOACC_PK_COMMAND_FLAGB_SHIFT                           30                                              /**< Shift value for CRYPTOACC_FLAGB             */
#define _CRYPTOACC_PK_COMMAND_FLAGB_MASK                            0x40000000UL                                    /**< Bit mask for CRYPTOACC_FLAGB                */
#define _CRYPTOACC_PK_COMMAND_FLAGB_DEFAULT                         0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_FLAGB_DEFAULT                          (_CRYPTOACC_PK_COMMAND_FLAGB_DEFAULT << 30)     /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/
#define CRYPTOACC_PK_COMMAND_CALCR2                                 (0x1UL << 31)                                   /**< CALCR2                                      */
#define _CRYPTOACC_PK_COMMAND_CALCR2_SHIFT                          31                                              /**< Shift value for CRYPTOACC_CALCR2            */
#define _CRYPTOACC_PK_COMMAND_CALCR2_MASK                           0x80000000UL                                    /**< Bit mask for CRYPTOACC_CALCR2               */
#define _CRYPTOACC_PK_COMMAND_CALCR2_DEFAULT                        0x00000000UL                                    /**< Mode DEFAULT for CRYPTOACC_PK_COMMAND       */
#define CRYPTOACC_PK_COMMAND_CALCR2_DEFAULT                         (_CRYPTOACC_PK_COMMAND_CALCR2_DEFAULT << 31)    /**< Shifted mode DEFAULT for CRYPTOACC_PK_COMMAND*/

/* Bit fields for CRYPTOACC PK_CONTROL */
#define _CRYPTOACC_PK_CONTROL_RESETVALUE                            0x00000000UL                                  /**< Default value for CRYPTOACC_PK_CONTROL      */
#define _CRYPTOACC_PK_CONTROL_MASK                                  0x00000003UL                                  /**< Mask for CRYPTOACC_PK_CONTROL               */
#define CRYPTOACC_PK_CONTROL_START                                  (0x1UL << 0)                                  /**< START                                       */
#define _CRYPTOACC_PK_CONTROL_START_SHIFT                           0                                             /**< Shift value for CRYPTOACC_START             */
#define _CRYPTOACC_PK_CONTROL_START_MASK                            0x1UL                                         /**< Bit mask for CRYPTOACC_START                */
#define _CRYPTOACC_PK_CONTROL_START_DEFAULT                         0x00000000UL                                  /**< Mode DEFAULT for CRYPTOACC_PK_CONTROL       */
#define CRYPTOACC_PK_CONTROL_START_DEFAULT                          (_CRYPTOACC_PK_CONTROL_START_DEFAULT << 0)    /**< Shifted mode DEFAULT for CRYPTOACC_PK_CONTROL*/
#define CRYPTOACC_PK_CONTROL_CLEARIRQ                               (0x1UL << 1)                                  /**< CLEARIRQ                                    */
#define _CRYPTOACC_PK_CONTROL_CLEARIRQ_SHIFT                        1                                             /**< Shift value for CRYPTOACC_CLEARIRQ          */
#define _CRYPTOACC_PK_CONTROL_CLEARIRQ_MASK                         0x2UL                                         /**< Bit mask for CRYPTOACC_CLEARIRQ             */
#define _CRYPTOACC_PK_CONTROL_CLEARIRQ_DEFAULT                      0x00000000UL                                  /**< Mode DEFAULT for CRYPTOACC_PK_CONTROL       */
#define CRYPTOACC_PK_CONTROL_CLEARIRQ_DEFAULT                       (_CRYPTOACC_PK_CONTROL_CLEARIRQ_DEFAULT << 1) /**< Shifted mode DEFAULT for CRYPTOACC_PK_CONTROL*/

/* Bit fields for CRYPTOACC PK_STATUS */
#define _CRYPTOACC_PK_STATUS_RESETVALUE                             0x00000000UL                                      /**< Default value for CRYPTOACC_PK_STATUS       */
#define _CRYPTOACC_PK_STATUS_MASK                                   0x0003FFFFUL                                      /**< Mask for CRYPTOACC_PK_STATUS                */
#define _CRYPTOACC_PK_STATUS_FAILPTR_SHIFT                          0                                                 /**< Shift value for CRYPTOACC_FAILPTR           */
#define _CRYPTOACC_PK_STATUS_FAILPTR_MASK                           0xFUL                                             /**< Bit mask for CRYPTOACC_FAILPTR              */
#define _CRYPTOACC_PK_STATUS_FAILPTR_DEFAULT                        0x00000000UL                                      /**< Mode DEFAULT for CRYPTOACC_PK_STATUS        */
#define CRYPTOACC_PK_STATUS_FAILPTR_DEFAULT                         (_CRYPTOACC_PK_STATUS_FAILPTR_DEFAULT << 0)       /**< Shifted mode DEFAULT for CRYPTOACC_PK_STATUS*/
#define _CRYPTOACC_PK_STATUS_ERRORFLAGS_SHIFT                       4                                                 /**< Shift value for CRYPTOACC_ERRORFLAGS        */
#define _CRYPTOACC_PK_STATUS_ERRORFLAGS_MASK                        0xFFF0UL                                          /**< Bit mask for CRYPTOACC_ERRORFLAGS           */
#define _CRYPTOACC_PK_STATUS_ERRORFLAGS_DEFAULT                     0x00000000UL                                      /**< Mode DEFAULT for CRYPTOACC_PK_STATUS        */
#define CRYPTOACC_PK_STATUS_ERRORFLAGS_DEFAULT                      (_CRYPTOACC_PK_STATUS_ERRORFLAGS_DEFAULT << 4)    /**< Shifted mode DEFAULT for CRYPTOACC_PK_STATUS*/
#define CRYPTOACC_PK_STATUS_PK_BUSY                                 (0x1UL << 16)                                     /**< PK_BUSY                                     */
#define _CRYPTOACC_PK_STATUS_PK_BUSY_SHIFT                          16                                                /**< Shift value for CRYPTOACC_PK_BUSY           */
#define _CRYPTOACC_PK_STATUS_PK_BUSY_MASK                           0x10000UL                                         /**< Bit mask for CRYPTOACC_PK_BUSY              */
#define _CRYPTOACC_PK_STATUS_PK_BUSY_DEFAULT                        0x00000000UL                                      /**< Mode DEFAULT for CRYPTOACC_PK_STATUS        */
#define CRYPTOACC_PK_STATUS_PK_BUSY_DEFAULT                         (_CRYPTOACC_PK_STATUS_PK_BUSY_DEFAULT << 16)      /**< Shifted mode DEFAULT for CRYPTOACC_PK_STATUS*/
#define CRYPTOACC_PK_STATUS_INTRPTSTATUS                            (0x1UL << 17)                                     /**< INTRPTSTATUS                                */
#define _CRYPTOACC_PK_STATUS_INTRPTSTATUS_SHIFT                     17                                                /**< Shift value for CRYPTOACC_INTRPTSTATUS      */
#define _CRYPTOACC_PK_STATUS_INTRPTSTATUS_MASK                      0x20000UL                                         /**< Bit mask for CRYPTOACC_INTRPTSTATUS         */
#define _CRYPTOACC_PK_STATUS_INTRPTSTATUS_DEFAULT                   0x00000000UL                                      /**< Mode DEFAULT for CRYPTOACC_PK_STATUS        */
#define CRYPTOACC_PK_STATUS_INTRPTSTATUS_DEFAULT                    (_CRYPTOACC_PK_STATUS_INTRPTSTATUS_DEFAULT << 17) /**< Shifted mode DEFAULT for CRYPTOACC_PK_STATUS*/

/* Bit fields for CRYPTOACC PK_TIMER */
#define _CRYPTOACC_PK_TIMER_RESETVALUE                              0x00000000UL                             /**< Default value for CRYPTOACC_PK_TIMER        */
#define _CRYPTOACC_PK_TIMER_MASK                                    0xFFFFFFFFUL                             /**< Mask for CRYPTOACC_PK_TIMER                 */
#define _CRYPTOACC_PK_TIMER_TIMER_SHIFT                             0                                        /**< Shift value for CRYPTOACC_TIMER             */
#define _CRYPTOACC_PK_TIMER_TIMER_MASK                              0xFFFFFFFFUL                             /**< Bit mask for CRYPTOACC_TIMER                */
#define _CRYPTOACC_PK_TIMER_TIMER_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for CRYPTOACC_PK_TIMER         */
#define CRYPTOACC_PK_TIMER_TIMER_DEFAULT                            (_CRYPTOACC_PK_TIMER_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for CRYPTOACC_PK_TIMER */

/* Bit fields for CRYPTOACC PK_HWCONFIG */
#define _CRYPTOACC_PK_HWCONFIG_RESETVALUE                           0x01F11021UL                                       /**< Default value for CRYPTOACC_PK_HWCONFIG     */
#define _CRYPTOACC_PK_HWCONFIG_MASK                                 0x81F3FFFFUL                                       /**< Mask for CRYPTOACC_PK_HWCONFIG              */
#define _CRYPTOACC_PK_HWCONFIG_MAXOPSIZE_SHIFT                      0                                                  /**< Shift value for CRYPTOACC_MAXOPSIZE         */
#define _CRYPTOACC_PK_HWCONFIG_MAXOPSIZE_MASK                       0xFFFUL                                            /**< Bit mask for CRYPTOACC_MAXOPSIZE            */
#define _CRYPTOACC_PK_HWCONFIG_MAXOPSIZE_DEFAULT                    0x00000021UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_MAXOPSIZE_DEFAULT                     (_CRYPTOACC_PK_HWCONFIG_MAXOPSIZE_DEFAULT << 0)    /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/
#define _CRYPTOACC_PK_HWCONFIG_NBMULT_SHIFT                         12                                                 /**< Shift value for CRYPTOACC_NBMULT            */
#define _CRYPTOACC_PK_HWCONFIG_NBMULT_MASK                          0xF000UL                                           /**< Bit mask for CRYPTOACC_NBMULT               */
#define _CRYPTOACC_PK_HWCONFIG_NBMULT_DEFAULT                       0x00000001UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_NBMULT_DEFAULT                        (_CRYPTOACC_PK_HWCONFIG_NBMULT_DEFAULT << 12)      /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/
#define CRYPTOACC_PK_HWCONFIG_PRIMEFIELD                            (0x1UL << 16)                                      /**< PRIMEFIELD                                  */
#define _CRYPTOACC_PK_HWCONFIG_PRIMEFIELD_SHIFT                     16                                                 /**< Shift value for CRYPTOACC_PRIMEFIELD        */
#define _CRYPTOACC_PK_HWCONFIG_PRIMEFIELD_MASK                      0x10000UL                                          /**< Bit mask for CRYPTOACC_PRIMEFIELD           */
#define _CRYPTOACC_PK_HWCONFIG_PRIMEFIELD_DEFAULT                   0x00000001UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_PRIMEFIELD_DEFAULT                    (_CRYPTOACC_PK_HWCONFIG_PRIMEFIELD_DEFAULT << 16)  /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/
#define CRYPTOACC_PK_HWCONFIG_BINARYFIELD                           (0x1UL << 17)                                      /**< BINARYFIELD                                 */
#define _CRYPTOACC_PK_HWCONFIG_BINARYFIELD_SHIFT                    17                                                 /**< Shift value for CRYPTOACC_BINARYFIELD       */
#define _CRYPTOACC_PK_HWCONFIG_BINARYFIELD_MASK                     0x20000UL                                          /**< Bit mask for CRYPTOACC_BINARYFIELD          */
#define _CRYPTOACC_PK_HWCONFIG_BINARYFIELD_DEFAULT                  0x00000000UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_BINARYFIELD_DEFAULT                   (_CRYPTOACC_PK_HWCONFIG_BINARYFIELD_DEFAULT << 17) /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/
#define CRYPTOACC_PK_HWCONFIG_P256                                  (0x1UL << 20)                                      /**< P256                                        */
#define _CRYPTOACC_PK_HWCONFIG_P256_SHIFT                           20                                                 /**< Shift value for CRYPTOACC_P256              */
#define _CRYPTOACC_PK_HWCONFIG_P256_MASK                            0x100000UL                                         /**< Bit mask for CRYPTOACC_P256                 */
#define _CRYPTOACC_PK_HWCONFIG_P256_DEFAULT                         0x00000001UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_P256_DEFAULT                          (_CRYPTOACC_PK_HWCONFIG_P256_DEFAULT << 20)        /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/
#define CRYPTOACC_PK_HWCONFIG_P384                                  (0x1UL << 21)                                      /**< P384                                        */
#define _CRYPTOACC_PK_HWCONFIG_P384_SHIFT                           21                                                 /**< Shift value for CRYPTOACC_P384              */
#define _CRYPTOACC_PK_HWCONFIG_P384_MASK                            0x200000UL                                         /**< Bit mask for CRYPTOACC_P384                 */
#define _CRYPTOACC_PK_HWCONFIG_P384_DEFAULT                         0x00000001UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_P384_DEFAULT                          (_CRYPTOACC_PK_HWCONFIG_P384_DEFAULT << 21)        /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/
#define CRYPTOACC_PK_HWCONFIG_P521                                  (0x1UL << 22)                                      /**< P521                                        */
#define _CRYPTOACC_PK_HWCONFIG_P521_SHIFT                           22                                                 /**< Shift value for CRYPTOACC_P521              */
#define _CRYPTOACC_PK_HWCONFIG_P521_MASK                            0x400000UL                                         /**< Bit mask for CRYPTOACC_P521                 */
#define _CRYPTOACC_PK_HWCONFIG_P521_DEFAULT                         0x00000001UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_P521_DEFAULT                          (_CRYPTOACC_PK_HWCONFIG_P521_DEFAULT << 22)        /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/
#define CRYPTOACC_PK_HWCONFIG_P192                                  (0x1UL << 23)                                      /**< P192                                        */
#define _CRYPTOACC_PK_HWCONFIG_P192_SHIFT                           23                                                 /**< Shift value for CRYPTOACC_P192              */
#define _CRYPTOACC_PK_HWCONFIG_P192_MASK                            0x800000UL                                         /**< Bit mask for CRYPTOACC_P192                 */
#define _CRYPTOACC_PK_HWCONFIG_P192_DEFAULT                         0x00000001UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_P192_DEFAULT                          (_CRYPTOACC_PK_HWCONFIG_P192_DEFAULT << 23)        /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/
#define CRYPTOACC_PK_HWCONFIG_X25519                                (0x1UL << 24)                                      /**< X25519                                      */
#define _CRYPTOACC_PK_HWCONFIG_X25519_SHIFT                         24                                                 /**< Shift value for CRYPTOACC_X25519            */
#define _CRYPTOACC_PK_HWCONFIG_X25519_MASK                          0x1000000UL                                        /**< Bit mask for CRYPTOACC_X25519               */
#define _CRYPTOACC_PK_HWCONFIG_X25519_DEFAULT                       0x00000001UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_X25519_DEFAULT                        (_CRYPTOACC_PK_HWCONFIG_X25519_DEFAULT << 24)      /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/
#define CRYPTOACC_PK_HWCONFIG_DISABLECM                             (0x1UL << 31)                                      /**< DISABLECM                                   */
#define _CRYPTOACC_PK_HWCONFIG_DISABLECM_SHIFT                      31                                                 /**< Shift value for CRYPTOACC_DISABLECM         */
#define _CRYPTOACC_PK_HWCONFIG_DISABLECM_MASK                       0x80000000UL                                       /**< Bit mask for CRYPTOACC_DISABLECM            */
#define _CRYPTOACC_PK_HWCONFIG_DISABLECM_DEFAULT                    0x00000000UL                                       /**< Mode DEFAULT for CRYPTOACC_PK_HWCONFIG      */
#define CRYPTOACC_PK_HWCONFIG_DISABLECM_DEFAULT                     (_CRYPTOACC_PK_HWCONFIG_DISABLECM_DEFAULT << 31)   /**< Shifted mode DEFAULT for CRYPTOACC_PK_HWCONFIG*/

/** @} End of group EFR32MG27_CRYPTOACC_BitFields */
/** @} End of group EFR32MG27_CRYPTOACC */
/** @} End of group Parts */

#endif /* EFR32MG27_CRYPTOACC_H */
