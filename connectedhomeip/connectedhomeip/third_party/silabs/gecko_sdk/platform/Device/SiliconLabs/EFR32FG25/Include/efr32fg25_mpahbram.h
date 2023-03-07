/**************************************************************************//**
 * @file
 * @brief EFR32FG25 MPAHBRAM register and bit field definitions
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
#ifndef EFR32FG25_MPAHBRAM_H
#define EFR32FG25_MPAHBRAM_H
#define MPAHBRAM_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32FG25_MPAHBRAM MPAHBRAM
 * @{
 * @brief EFR32FG25 MPAHBRAM Register Declaration.
 *****************************************************************************/

/** MPAHBRAM Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  __IOM uint32_t CMD;                           /**< Command register                                   */
  __IOM uint32_t CTRL;                          /**< Control register                                   */
  __IM uint32_t  ECCERRADDR0;                   /**< ECC Error Address 0                                */
  __IM uint32_t  ECCERRADDR1;                   /**< ECC Error Address 1                                */
  uint32_t       RESERVED0[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  ECCMERRIND;                    /**< Multiple ECC error indication                      */
  __IOM uint32_t IF;                            /**< Interrupt Flags                                    */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  __IOM uint32_t RAMBANKSVALID;                 /**< New Register                                       */
  __IOM uint32_t CFGSRTOP;                      /**< Sequential Region on Top                           */
  __IOM uint32_t CFGSRMAP;                      /**< Sequential Region Map                              */
  __IOM uint32_t CFGIU0MAP;                     /**< Interleaving Unit 0 Map                            */
  __IOM uint32_t CFGIU1MAP;                     /**< Interleaving Unit 1 Map                            */
  uint32_t       RESERVED1[2U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED3[1006U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  __IOM uint32_t CMD_SET;                       /**< Command register                                   */
  __IOM uint32_t CTRL_SET;                      /**< Control register                                   */
  __IM uint32_t  ECCERRADDR0_SET;               /**< ECC Error Address 0                                */
  __IM uint32_t  ECCERRADDR1_SET;               /**< ECC Error Address 1                                */
  uint32_t       RESERVED4[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  ECCMERRIND_SET;                /**< Multiple ECC error indication                      */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable                                   */
  __IOM uint32_t RAMBANKSVALID_SET;             /**< New Register                                       */
  __IOM uint32_t CFGSRTOP_SET;                  /**< Sequential Region on Top                           */
  __IOM uint32_t CFGSRMAP_SET;                  /**< Sequential Region Map                              */
  __IOM uint32_t CFGIU0MAP_SET;                 /**< Interleaving Unit 0 Map                            */
  __IOM uint32_t CFGIU1MAP_SET;                 /**< Interleaving Unit 1 Map                            */
  uint32_t       RESERVED5[2U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED7[1006U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  __IOM uint32_t CMD_CLR;                       /**< Command register                                   */
  __IOM uint32_t CTRL_CLR;                      /**< Control register                                   */
  __IM uint32_t  ECCERRADDR0_CLR;               /**< ECC Error Address 0                                */
  __IM uint32_t  ECCERRADDR1_CLR;               /**< ECC Error Address 1                                */
  uint32_t       RESERVED8[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  ECCMERRIND_CLR;                /**< Multiple ECC error indication                      */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable                                   */
  __IOM uint32_t RAMBANKSVALID_CLR;             /**< New Register                                       */
  __IOM uint32_t CFGSRTOP_CLR;                  /**< Sequential Region on Top                           */
  __IOM uint32_t CFGSRMAP_CLR;                  /**< Sequential Region Map                              */
  __IOM uint32_t CFGIU0MAP_CLR;                 /**< Interleaving Unit 0 Map                            */
  __IOM uint32_t CFGIU1MAP_CLR;                 /**< Interleaving Unit 1 Map                            */
  uint32_t       RESERVED9[2U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED10[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED11[1006U];             /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  __IOM uint32_t CMD_TGL;                       /**< Command register                                   */
  __IOM uint32_t CTRL_TGL;                      /**< Control register                                   */
  __IM uint32_t  ECCERRADDR0_TGL;               /**< ECC Error Address 0                                */
  __IM uint32_t  ECCERRADDR1_TGL;               /**< ECC Error Address 1                                */
  uint32_t       RESERVED12[2U];                /**< Reserved for future use                            */
  __IM uint32_t  ECCMERRIND_TGL;                /**< Multiple ECC error indication                      */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable                                   */
  __IOM uint32_t RAMBANKSVALID_TGL;             /**< New Register                                       */
  __IOM uint32_t CFGSRTOP_TGL;                  /**< Sequential Region on Top                           */
  __IOM uint32_t CFGSRMAP_TGL;                  /**< Sequential Region Map                              */
  __IOM uint32_t CFGIU0MAP_TGL;                 /**< Interleaving Unit 0 Map                            */
  __IOM uint32_t CFGIU1MAP_TGL;                 /**< Interleaving Unit 1 Map                            */
  uint32_t       RESERVED13[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
} MPAHBRAM_TypeDef;
/** @} End of group EFR32FG25_MPAHBRAM */

/**************************************************************************//**
 * @addtogroup EFR32FG25_MPAHBRAM
 * @{
 * @defgroup EFR32FG25_MPAHBRAM_BitFields MPAHBRAM Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for MPAHBRAM IPVERSION */
#define _MPAHBRAM_IPVERSION_RESETVALUE                    0x00000002UL                                 /**< Default value for MPAHBRAM_IPVERSION        */
#define _MPAHBRAM_IPVERSION_MASK                          0x00000003UL                                 /**< Mask for MPAHBRAM_IPVERSION                 */
#define _MPAHBRAM_IPVERSION_IPVERSION_SHIFT               0                                            /**< Shift value for MPAHBRAM_IPVERSION          */
#define _MPAHBRAM_IPVERSION_IPVERSION_MASK                0x3UL                                        /**< Bit mask for MPAHBRAM_IPVERSION             */
#define _MPAHBRAM_IPVERSION_IPVERSION_DEFAULT             0x00000002UL                                 /**< Mode DEFAULT for MPAHBRAM_IPVERSION         */
#define MPAHBRAM_IPVERSION_IPVERSION_DEFAULT              (_MPAHBRAM_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_IPVERSION */

/* Bit fields for MPAHBRAM CMD */
#define _MPAHBRAM_CMD_RESETVALUE                          0x00000000UL                               /**< Default value for MPAHBRAM_CMD              */
#define _MPAHBRAM_CMD_MASK                                0x00000003UL                               /**< Mask for MPAHBRAM_CMD                       */
#define MPAHBRAM_CMD_CLEARECCADDR0                        (0x1UL << 0)                               /**< Clear ECCERRADDR0                           */
#define _MPAHBRAM_CMD_CLEARECCADDR0_SHIFT                 0                                          /**< Shift value for MPAHBRAM_CLEARECCADDR0      */
#define _MPAHBRAM_CMD_CLEARECCADDR0_MASK                  0x1UL                                      /**< Bit mask for MPAHBRAM_CLEARECCADDR0         */
#define _MPAHBRAM_CMD_CLEARECCADDR0_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for MPAHBRAM_CMD               */
#define MPAHBRAM_CMD_CLEARECCADDR0_DEFAULT                (_MPAHBRAM_CMD_CLEARECCADDR0_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_CMD       */
#define MPAHBRAM_CMD_CLEARECCADDR1                        (0x1UL << 1)                               /**< Clear ECCERRADDR1                           */
#define _MPAHBRAM_CMD_CLEARECCADDR1_SHIFT                 1                                          /**< Shift value for MPAHBRAM_CLEARECCADDR1      */
#define _MPAHBRAM_CMD_CLEARECCADDR1_MASK                  0x2UL                                      /**< Bit mask for MPAHBRAM_CLEARECCADDR1         */
#define _MPAHBRAM_CMD_CLEARECCADDR1_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for MPAHBRAM_CMD               */
#define MPAHBRAM_CMD_CLEARECCADDR1_DEFAULT                (_MPAHBRAM_CMD_CLEARECCADDR1_DEFAULT << 1) /**< Shifted mode DEFAULT for MPAHBRAM_CMD       */

/* Bit fields for MPAHBRAM CTRL */
#define _MPAHBRAM_CTRL_RESETVALUE                         0x00000040UL                                  /**< Default value for MPAHBRAM_CTRL             */
#define _MPAHBRAM_CTRL_MASK                               0x0000007FUL                                  /**< Mask for MPAHBRAM_CTRL                      */
#define MPAHBRAM_CTRL_ECCEN                               (0x1UL << 0)                                  /**< Enable ECC functionality                    */
#define _MPAHBRAM_CTRL_ECCEN_SHIFT                        0                                             /**< Shift value for MPAHBRAM_ECCEN              */
#define _MPAHBRAM_CTRL_ECCEN_MASK                         0x1UL                                         /**< Bit mask for MPAHBRAM_ECCEN                 */
#define _MPAHBRAM_CTRL_ECCEN_DEFAULT                      0x00000000UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define MPAHBRAM_CTRL_ECCEN_DEFAULT                       (_MPAHBRAM_CTRL_ECCEN_DEFAULT << 0)           /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */
#define MPAHBRAM_CTRL_ECCWEN                              (0x1UL << 1)                                  /**< Enable ECC syndrome writes                  */
#define _MPAHBRAM_CTRL_ECCWEN_SHIFT                       1                                             /**< Shift value for MPAHBRAM_ECCWEN             */
#define _MPAHBRAM_CTRL_ECCWEN_MASK                        0x2UL                                         /**< Bit mask for MPAHBRAM_ECCWEN                */
#define _MPAHBRAM_CTRL_ECCWEN_DEFAULT                     0x00000000UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define MPAHBRAM_CTRL_ECCWEN_DEFAULT                      (_MPAHBRAM_CTRL_ECCWEN_DEFAULT << 1)          /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */
#define MPAHBRAM_CTRL_ECCERRFAULTEN                       (0x1UL << 2)                                  /**< ECC Error bus fault enable                  */
#define _MPAHBRAM_CTRL_ECCERRFAULTEN_SHIFT                2                                             /**< Shift value for MPAHBRAM_ECCERRFAULTEN      */
#define _MPAHBRAM_CTRL_ECCERRFAULTEN_MASK                 0x4UL                                         /**< Bit mask for MPAHBRAM_ECCERRFAULTEN         */
#define _MPAHBRAM_CTRL_ECCERRFAULTEN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define MPAHBRAM_CTRL_ECCERRFAULTEN_DEFAULT               (_MPAHBRAM_CTRL_ECCERRFAULTEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_SHIFT              3                                             /**< Shift value for MPAHBRAM_AHBPORTPRIORITY    */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_MASK               0x38UL                                        /**< Bit mask for MPAHBRAM_AHBPORTPRIORITY       */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_NONE               0x00000000UL                                  /**< Mode NONE for MPAHBRAM_CTRL                 */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT0              0x00000001UL                                  /**< Mode PORT0 for MPAHBRAM_CTRL                */
#define _MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT1              0x00000002UL                                  /**< Mode PORT1 for MPAHBRAM_CTRL                */
#define MPAHBRAM_CTRL_AHBPORTPRIORITY_DEFAULT             (_MPAHBRAM_CTRL_AHBPORTPRIORITY_DEFAULT << 3) /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */
#define MPAHBRAM_CTRL_AHBPORTPRIORITY_NONE                (_MPAHBRAM_CTRL_AHBPORTPRIORITY_NONE << 3)    /**< Shifted mode NONE for MPAHBRAM_CTRL         */
#define MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT0               (_MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT0 << 3)   /**< Shifted mode PORT0 for MPAHBRAM_CTRL        */
#define MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT1               (_MPAHBRAM_CTRL_AHBPORTPRIORITY_PORT1 << 3)   /**< Shifted mode PORT1 for MPAHBRAM_CTRL        */
#define MPAHBRAM_CTRL_ADDRFAULTEN                         (0x1UL << 6)                                  /**< Address fault bus fault enable              */
#define _MPAHBRAM_CTRL_ADDRFAULTEN_SHIFT                  6                                             /**< Shift value for MPAHBRAM_ADDRFAULTEN        */
#define _MPAHBRAM_CTRL_ADDRFAULTEN_MASK                   0x40UL                                        /**< Bit mask for MPAHBRAM_ADDRFAULTEN           */
#define _MPAHBRAM_CTRL_ADDRFAULTEN_DEFAULT                0x00000001UL                                  /**< Mode DEFAULT for MPAHBRAM_CTRL              */
#define MPAHBRAM_CTRL_ADDRFAULTEN_DEFAULT                 (_MPAHBRAM_CTRL_ADDRFAULTEN_DEFAULT << 6)     /**< Shifted mode DEFAULT for MPAHBRAM_CTRL      */

/* Bit fields for MPAHBRAM ECCERRADDR0 */
#define _MPAHBRAM_ECCERRADDR0_RESETVALUE                  0x00000000UL                              /**< Default value for MPAHBRAM_ECCERRADDR0      */
#define _MPAHBRAM_ECCERRADDR0_MASK                        0xFFFFFFFFUL                              /**< Mask for MPAHBRAM_ECCERRADDR0               */
#define _MPAHBRAM_ECCERRADDR0_ADDR_SHIFT                  0                                         /**< Shift value for MPAHBRAM_ADDR               */
#define _MPAHBRAM_ECCERRADDR0_ADDR_MASK                   0xFFFFFFFFUL                              /**< Bit mask for MPAHBRAM_ADDR                  */
#define _MPAHBRAM_ECCERRADDR0_ADDR_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for MPAHBRAM_ECCERRADDR0       */
#define MPAHBRAM_ECCERRADDR0_ADDR_DEFAULT                 (_MPAHBRAM_ECCERRADDR0_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_ECCERRADDR0*/

/* Bit fields for MPAHBRAM ECCERRADDR1 */
#define _MPAHBRAM_ECCERRADDR1_RESETVALUE                  0x00000000UL                              /**< Default value for MPAHBRAM_ECCERRADDR1      */
#define _MPAHBRAM_ECCERRADDR1_MASK                        0xFFFFFFFFUL                              /**< Mask for MPAHBRAM_ECCERRADDR1               */
#define _MPAHBRAM_ECCERRADDR1_ADDR_SHIFT                  0                                         /**< Shift value for MPAHBRAM_ADDR               */
#define _MPAHBRAM_ECCERRADDR1_ADDR_MASK                   0xFFFFFFFFUL                              /**< Bit mask for MPAHBRAM_ADDR                  */
#define _MPAHBRAM_ECCERRADDR1_ADDR_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for MPAHBRAM_ECCERRADDR1       */
#define MPAHBRAM_ECCERRADDR1_ADDR_DEFAULT                 (_MPAHBRAM_ECCERRADDR1_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_ECCERRADDR1*/

/* Bit fields for MPAHBRAM ECCMERRIND */
#define _MPAHBRAM_ECCMERRIND_RESETVALUE                   0x00000000UL                           /**< Default value for MPAHBRAM_ECCMERRIND       */
#define _MPAHBRAM_ECCMERRIND_MASK                         0x00000003UL                           /**< Mask for MPAHBRAM_ECCMERRIND                */
#define MPAHBRAM_ECCMERRIND_P0                            (0x1UL << 0)                           /**< Multiple ECC errors on AHB port 0           */
#define _MPAHBRAM_ECCMERRIND_P0_SHIFT                     0                                      /**< Shift value for MPAHBRAM_P0                 */
#define _MPAHBRAM_ECCMERRIND_P0_MASK                      0x1UL                                  /**< Bit mask for MPAHBRAM_P0                    */
#define _MPAHBRAM_ECCMERRIND_P0_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_ECCMERRIND        */
#define MPAHBRAM_ECCMERRIND_P0_DEFAULT                    (_MPAHBRAM_ECCMERRIND_P0_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_ECCMERRIND*/
#define MPAHBRAM_ECCMERRIND_P1                            (0x1UL << 1)                           /**< Multiple ECC errors on AHB port 1           */
#define _MPAHBRAM_ECCMERRIND_P1_SHIFT                     1                                      /**< Shift value for MPAHBRAM_P1                 */
#define _MPAHBRAM_ECCMERRIND_P1_MASK                      0x2UL                                  /**< Bit mask for MPAHBRAM_P1                    */
#define _MPAHBRAM_ECCMERRIND_P1_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_ECCMERRIND        */
#define MPAHBRAM_ECCMERRIND_P1_DEFAULT                    (_MPAHBRAM_ECCMERRIND_P1_DEFAULT << 1) /**< Shifted mode DEFAULT for MPAHBRAM_ECCMERRIND*/

/* Bit fields for MPAHBRAM IF */
#define _MPAHBRAM_IF_RESETVALUE                           0x00000000UL                          /**< Default value for MPAHBRAM_IF               */
#define _MPAHBRAM_IF_MASK                                 0x00000033UL                          /**< Mask for MPAHBRAM_IF                        */
#define MPAHBRAM_IF_AHB0ERR1B                             (0x1UL << 0)                          /**< AHB0 1-bit ECC Error Interrupt Flag         */
#define _MPAHBRAM_IF_AHB0ERR1B_SHIFT                      0                                     /**< Shift value for MPAHBRAM_AHB0ERR1B          */
#define _MPAHBRAM_IF_AHB0ERR1B_MASK                       0x1UL                                 /**< Bit mask for MPAHBRAM_AHB0ERR1B             */
#define _MPAHBRAM_IF_AHB0ERR1B_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for MPAHBRAM_IF                */
#define MPAHBRAM_IF_AHB0ERR1B_DEFAULT                     (_MPAHBRAM_IF_AHB0ERR1B_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_IF        */
#define MPAHBRAM_IF_AHB1ERR1B                             (0x1UL << 1)                          /**< AHB1 1-bit ECC Error Interrupt Flag         */
#define _MPAHBRAM_IF_AHB1ERR1B_SHIFT                      1                                     /**< Shift value for MPAHBRAM_AHB1ERR1B          */
#define _MPAHBRAM_IF_AHB1ERR1B_MASK                       0x2UL                                 /**< Bit mask for MPAHBRAM_AHB1ERR1B             */
#define _MPAHBRAM_IF_AHB1ERR1B_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for MPAHBRAM_IF                */
#define MPAHBRAM_IF_AHB1ERR1B_DEFAULT                     (_MPAHBRAM_IF_AHB1ERR1B_DEFAULT << 1) /**< Shifted mode DEFAULT for MPAHBRAM_IF        */
#define MPAHBRAM_IF_AHB0ERR2B                             (0x1UL << 4)                          /**< AHB0 2-bit ECC Error Interrupt Flag         */
#define _MPAHBRAM_IF_AHB0ERR2B_SHIFT                      4                                     /**< Shift value for MPAHBRAM_AHB0ERR2B          */
#define _MPAHBRAM_IF_AHB0ERR2B_MASK                       0x10UL                                /**< Bit mask for MPAHBRAM_AHB0ERR2B             */
#define _MPAHBRAM_IF_AHB0ERR2B_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for MPAHBRAM_IF                */
#define MPAHBRAM_IF_AHB0ERR2B_DEFAULT                     (_MPAHBRAM_IF_AHB0ERR2B_DEFAULT << 4) /**< Shifted mode DEFAULT for MPAHBRAM_IF        */
#define MPAHBRAM_IF_AHB1ERR2B                             (0x1UL << 5)                          /**< AHB1 2-bit ECC Error Interrupt Flag         */
#define _MPAHBRAM_IF_AHB1ERR2B_SHIFT                      5                                     /**< Shift value for MPAHBRAM_AHB1ERR2B          */
#define _MPAHBRAM_IF_AHB1ERR2B_MASK                       0x20UL                                /**< Bit mask for MPAHBRAM_AHB1ERR2B             */
#define _MPAHBRAM_IF_AHB1ERR2B_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for MPAHBRAM_IF                */
#define MPAHBRAM_IF_AHB1ERR2B_DEFAULT                     (_MPAHBRAM_IF_AHB1ERR2B_DEFAULT << 5) /**< Shifted mode DEFAULT for MPAHBRAM_IF        */

/* Bit fields for MPAHBRAM IEN */
#define _MPAHBRAM_IEN_RESETVALUE                          0x00000000UL                           /**< Default value for MPAHBRAM_IEN              */
#define _MPAHBRAM_IEN_MASK                                0x00000033UL                           /**< Mask for MPAHBRAM_IEN                       */
#define MPAHBRAM_IEN_AHB0ERR1B                            (0x1UL << 0)                           /**< AHB0 1-bit ECC Error Interrupt Enable       */
#define _MPAHBRAM_IEN_AHB0ERR1B_SHIFT                     0                                      /**< Shift value for MPAHBRAM_AHB0ERR1B          */
#define _MPAHBRAM_IEN_AHB0ERR1B_MASK                      0x1UL                                  /**< Bit mask for MPAHBRAM_AHB0ERR1B             */
#define _MPAHBRAM_IEN_AHB0ERR1B_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_IEN               */
#define MPAHBRAM_IEN_AHB0ERR1B_DEFAULT                    (_MPAHBRAM_IEN_AHB0ERR1B_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_IEN       */
#define MPAHBRAM_IEN_AHB1ERR1B                            (0x1UL << 1)                           /**< AHB1 1-bit ECC Error Interrupt Enable       */
#define _MPAHBRAM_IEN_AHB1ERR1B_SHIFT                     1                                      /**< Shift value for MPAHBRAM_AHB1ERR1B          */
#define _MPAHBRAM_IEN_AHB1ERR1B_MASK                      0x2UL                                  /**< Bit mask for MPAHBRAM_AHB1ERR1B             */
#define _MPAHBRAM_IEN_AHB1ERR1B_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_IEN               */
#define MPAHBRAM_IEN_AHB1ERR1B_DEFAULT                    (_MPAHBRAM_IEN_AHB1ERR1B_DEFAULT << 1) /**< Shifted mode DEFAULT for MPAHBRAM_IEN       */
#define MPAHBRAM_IEN_AHB0ERR2B                            (0x1UL << 4)                           /**< AHB0 2-bit ECC Error Interrupt Enable       */
#define _MPAHBRAM_IEN_AHB0ERR2B_SHIFT                     4                                      /**< Shift value for MPAHBRAM_AHB0ERR2B          */
#define _MPAHBRAM_IEN_AHB0ERR2B_MASK                      0x10UL                                 /**< Bit mask for MPAHBRAM_AHB0ERR2B             */
#define _MPAHBRAM_IEN_AHB0ERR2B_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_IEN               */
#define MPAHBRAM_IEN_AHB0ERR2B_DEFAULT                    (_MPAHBRAM_IEN_AHB0ERR2B_DEFAULT << 4) /**< Shifted mode DEFAULT for MPAHBRAM_IEN       */
#define MPAHBRAM_IEN_AHB1ERR2B                            (0x1UL << 5)                           /**< AHB1 2-bit ECC Error Interrupt Enable       */
#define _MPAHBRAM_IEN_AHB1ERR2B_SHIFT                     5                                      /**< Shift value for MPAHBRAM_AHB1ERR2B          */
#define _MPAHBRAM_IEN_AHB1ERR2B_MASK                      0x20UL                                 /**< Bit mask for MPAHBRAM_AHB1ERR2B             */
#define _MPAHBRAM_IEN_AHB1ERR2B_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_IEN               */
#define MPAHBRAM_IEN_AHB1ERR2B_DEFAULT                    (_MPAHBRAM_IEN_AHB1ERR2B_DEFAULT << 5) /**< Shifted mode DEFAULT for MPAHBRAM_IEN       */

/* Bit fields for MPAHBRAM RAMBANKSVALID */
#define _MPAHBRAM_RAMBANKSVALID_RESETVALUE                0xFFFFFFFFUL                                          /**< Default value for MPAHBRAM_RAMBANKSVALID    */
#define _MPAHBRAM_RAMBANKSVALID_MASK                      0x0000FFFFUL                                          /**< Mask for MPAHBRAM_RAMBANKSVALID             */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_SHIFT       0                                                     /**< Shift value for MPAHBRAM_RAMBANKSVALID      */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_MASK        0xFFFFUL                                              /**< Bit mask for MPAHBRAM_RAMBANKSVALID         */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_DEFAULT     0xFFFFFFFFUL                                          /**< Mode DEFAULT for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0        0x00000001UL                                          /**< Mode BLK0 for MPAHBRAM_RAMBANKSVALID        */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO1     0x00000003UL                                          /**< Mode BLK0TO1 for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO2     0x00000007UL                                          /**< Mode BLK0TO2 for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO3     0x0000000FUL                                          /**< Mode BLK0TO3 for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO4     0x0000001FUL                                          /**< Mode BLK0TO4 for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO5     0x0000003FUL                                          /**< Mode BLK0TO5 for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO6     0x0000007FUL                                          /**< Mode BLK0TO6 for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO7     0x000000FFUL                                          /**< Mode BLK0TO7 for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO8     0x000001FFUL                                          /**< Mode BLK0TO8 for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO9     0x000003FFUL                                          /**< Mode BLK0TO9 for MPAHBRAM_RAMBANKSVALID     */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO10    0x000007FFUL                                          /**< Mode BLK0TO10 for MPAHBRAM_RAMBANKSVALID    */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO11    0x00000FFFUL                                          /**< Mode BLK0TO11 for MPAHBRAM_RAMBANKSVALID    */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO12    0x00001FFFUL                                          /**< Mode BLK0TO12 for MPAHBRAM_RAMBANKSVALID    */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO13    0x00003FFFUL                                          /**< Mode BLK0TO13 for MPAHBRAM_RAMBANKSVALID    */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO14    0x00007FFFUL                                          /**< Mode BLK0TO14 for MPAHBRAM_RAMBANKSVALID    */
#define _MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO15    0x0000FFFFUL                                          /**< Mode BLK0TO15 for MPAHBRAM_RAMBANKSVALID    */
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_DEFAULT      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_DEFAULT << 0)  /**< Shifted mode DEFAULT for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0         (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0 << 0)     /**< Shifted mode BLK0 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO1      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO1 << 0)  /**< Shifted mode BLK0TO1 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO2      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO2 << 0)  /**< Shifted mode BLK0TO2 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO3      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO3 << 0)  /**< Shifted mode BLK0TO3 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO4      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO4 << 0)  /**< Shifted mode BLK0TO4 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO5      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO5 << 0)  /**< Shifted mode BLK0TO5 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO6      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO6 << 0)  /**< Shifted mode BLK0TO6 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO7      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO7 << 0)  /**< Shifted mode BLK0TO7 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO8      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO8 << 0)  /**< Shifted mode BLK0TO8 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO9      (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO9 << 0)  /**< Shifted mode BLK0TO9 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO10     (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO10 << 0) /**< Shifted mode BLK0TO10 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO11     (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO11 << 0) /**< Shifted mode BLK0TO11 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO12     (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO12 << 0) /**< Shifted mode BLK0TO12 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO13     (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO13 << 0) /**< Shifted mode BLK0TO13 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO14     (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO14 << 0) /**< Shifted mode BLK0TO14 for MPAHBRAM_RAMBANKSVALID*/
#define MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO15     (_MPAHBRAM_RAMBANKSVALID_RAMBANKSVALID_BLK0TO15 << 0) /**< Shifted mode BLK0TO15 for MPAHBRAM_RAMBANKSVALID*/

/* Bit fields for MPAHBRAM CFGSRTOP */
#define _MPAHBRAM_CFGSRTOP_RESETVALUE                     0x00000000UL                            /**< Default value for MPAHBRAM_CFGSRTOP         */
#define _MPAHBRAM_CFGSRTOP_MASK                           0x00000001UL                            /**< Mask for MPAHBRAM_CFGSRTOP                  */
#define MPAHBRAM_CFGSRTOP_SRTOP                           (0x1UL << 0)                            /**< Sequential region on top                    */
#define _MPAHBRAM_CFGSRTOP_SRTOP_SHIFT                    0                                       /**< Shift value for MPAHBRAM_SRTOP              */
#define _MPAHBRAM_CFGSRTOP_SRTOP_MASK                     0x1UL                                   /**< Bit mask for MPAHBRAM_SRTOP                 */
#define _MPAHBRAM_CFGSRTOP_SRTOP_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for MPAHBRAM_CFGSRTOP          */
#define MPAHBRAM_CFGSRTOP_SRTOP_DEFAULT                   (_MPAHBRAM_CFGSRTOP_SRTOP_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_CFGSRTOP  */

/* Bit fields for MPAHBRAM CFGSRMAP */
#define _MPAHBRAM_CFGSRMAP_RESETVALUE                     0xFFFFFFFFUL                          /**< Default value for MPAHBRAM_CFGSRMAP         */
#define _MPAHBRAM_CFGSRMAP_MASK                           0x0000FFFFUL                          /**< Mask for MPAHBRAM_CFGSRMAP                  */
#define _MPAHBRAM_CFGSRMAP_MAP_SHIFT                      0                                     /**< Shift value for MPAHBRAM_MAP                */
#define _MPAHBRAM_CFGSRMAP_MAP_MASK                       0xFFFFUL                              /**< Bit mask for MPAHBRAM_MAP                   */
#define _MPAHBRAM_CFGSRMAP_MAP_DEFAULT                    0xFFFFFFFFUL                          /**< Mode DEFAULT for MPAHBRAM_CFGSRMAP          */
#define MPAHBRAM_CFGSRMAP_MAP_DEFAULT                     (_MPAHBRAM_CFGSRMAP_MAP_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_CFGSRMAP  */

/* Bit fields for MPAHBRAM CFGIU0MAP */
#define _MPAHBRAM_CFGIU0MAP_RESETVALUE                    0x00000000UL                           /**< Default value for MPAHBRAM_CFGIU0MAP        */
#define _MPAHBRAM_CFGIU0MAP_MASK                          0x0000FFFFUL                           /**< Mask for MPAHBRAM_CFGIU0MAP                 */
#define _MPAHBRAM_CFGIU0MAP_MAP_SHIFT                     0                                      /**< Shift value for MPAHBRAM_MAP                */
#define _MPAHBRAM_CFGIU0MAP_MAP_MASK                      0xFFFFUL                               /**< Bit mask for MPAHBRAM_MAP                   */
#define _MPAHBRAM_CFGIU0MAP_MAP_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_CFGIU0MAP         */
#define MPAHBRAM_CFGIU0MAP_MAP_DEFAULT                    (_MPAHBRAM_CFGIU0MAP_MAP_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_CFGIU0MAP */

/* Bit fields for MPAHBRAM CFGIU1MAP */
#define _MPAHBRAM_CFGIU1MAP_RESETVALUE                    0x00000000UL                           /**< Default value for MPAHBRAM_CFGIU1MAP        */
#define _MPAHBRAM_CFGIU1MAP_MASK                          0x0000FFFFUL                           /**< Mask for MPAHBRAM_CFGIU1MAP                 */
#define _MPAHBRAM_CFGIU1MAP_MAP_SHIFT                     0                                      /**< Shift value for MPAHBRAM_MAP                */
#define _MPAHBRAM_CFGIU1MAP_MAP_MASK                      0xFFFFUL                               /**< Bit mask for MPAHBRAM_MAP                   */
#define _MPAHBRAM_CFGIU1MAP_MAP_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for MPAHBRAM_CFGIU1MAP         */
#define MPAHBRAM_CFGIU1MAP_MAP_DEFAULT                    (_MPAHBRAM_CFGIU1MAP_MAP_DEFAULT << 0) /**< Shifted mode DEFAULT for MPAHBRAM_CFGIU1MAP */

/** @} End of group EFR32FG25_MPAHBRAM_BitFields */
/** @} End of group EFR32FG25_MPAHBRAM */
/** @} End of group Parts */

#endif /* EFR32FG25_MPAHBRAM_H */
