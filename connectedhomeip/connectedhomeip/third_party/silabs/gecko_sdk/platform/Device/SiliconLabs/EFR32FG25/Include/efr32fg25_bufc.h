/**************************************************************************//**
 * @file
 * @brief EFR32FG25 BUFC register and bit field definitions
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
#ifndef EFR32FG25_BUFC_H
#define EFR32FG25_BUFC_H
#define BUFC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32FG25_BUFC BUFC
 * @{
 * @brief EFR32FG25 BUFC Register Declaration.
 *****************************************************************************/

/** BUFC BUF Register Group Declaration. */
typedef struct {
  __IOM uint32_t CTRL;                               /**< Buffer Control                                     */
  __IOM uint32_t ADDR;                               /**< Buffer Address                                     */
  __IOM uint32_t WRITEOFFSET;                        /**< Write Offset                                       */
  __IOM uint32_t READOFFSET;                         /**< Read Offset                                        */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IM uint32_t  READDATA;                           /**< Buffer Read Data                                   */
  __IOM uint32_t WRITEDATA;                          /**< Buffer Write Data                                  */
  __IOM uint32_t XWRITE;                             /**< Buffer XOR Write                                   */
  __IM uint32_t  STATUS;                             /**< Buffer Status Register                             */
  __IOM uint32_t THRESHOLDCTRL;                      /**< Threshold Control                                  */
  __IOM uint32_t CMD;                                /**< Buffer Command                                     */
  uint32_t       RESERVED1[1U];                      /**< Reserved for future use                            */
  __IM uint32_t  READDATA32;                         /**< Buffer Read Data                                   */
  __IOM uint32_t WRITEDATA32;                        /**< Buffer Write Data                                  */
  __IOM uint32_t XWRITE32;                           /**< Buffer XOR Write                                   */
  uint32_t       RESERVED2[1U];                      /**< Reserved for future use                            */
} BUFC_BUF_TypeDef;

/** BUFC Register Declaration. */
typedef struct {
  __IM uint32_t    IPVERSION;                   /**< IP Version                                         */
  __IOM uint32_t   EN;                          /**< Enable peripheral clock to this module             */
  __IOM uint32_t   LPMODE;                      /**< Low power mode control                             */
  BUFC_BUF_TypeDef BUF[4U];                     /**< Data Buffer                                        */
  uint32_t         RESERVED0[2U];               /**< Reserved for future use                            */
  __IOM uint32_t   IF;                          /**< BUFC Interrupt Flags                               */
  __IOM uint32_t   IEN;                         /**< Interrupt Enable Register                          */
  __IOM uint32_t   SEQIF;                       /**< SEQ BUFC Interrupt Flags                           */
  __IOM uint32_t   SEQIEN;                      /**< SEQ Interrupt Enable Register                      */
  __IOM uint32_t   SFMIF;                       /**< Soft Modem BUFC Interrupt Flags                    */
  __IOM uint32_t   SFMIEN;                      /**< Soft Modem BUFC interrupt Enables                  */
  uint32_t         RESERVED1[949U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_SET;               /**< IP Version                                         */
  __IOM uint32_t   EN_SET;                      /**< Enable peripheral clock to this module             */
  __IOM uint32_t   LPMODE_SET;                  /**< Low power mode control                             */
  BUFC_BUF_TypeDef BUF_SET[4U];                 /**< Data Buffer                                        */
  uint32_t         RESERVED2[2U];               /**< Reserved for future use                            */
  __IOM uint32_t   IF_SET;                      /**< BUFC Interrupt Flags                               */
  __IOM uint32_t   IEN_SET;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   SEQIF_SET;                   /**< SEQ BUFC Interrupt Flags                           */
  __IOM uint32_t   SEQIEN_SET;                  /**< SEQ Interrupt Enable Register                      */
  __IOM uint32_t   SFMIF_SET;                   /**< Soft Modem BUFC Interrupt Flags                    */
  __IOM uint32_t   SFMIEN_SET;                  /**< Soft Modem BUFC interrupt Enables                  */
  uint32_t         RESERVED3[949U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_CLR;               /**< IP Version                                         */
  __IOM uint32_t   EN_CLR;                      /**< Enable peripheral clock to this module             */
  __IOM uint32_t   LPMODE_CLR;                  /**< Low power mode control                             */
  BUFC_BUF_TypeDef BUF_CLR[4U];                 /**< Data Buffer                                        */
  uint32_t         RESERVED4[2U];               /**< Reserved for future use                            */
  __IOM uint32_t   IF_CLR;                      /**< BUFC Interrupt Flags                               */
  __IOM uint32_t   IEN_CLR;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   SEQIF_CLR;                   /**< SEQ BUFC Interrupt Flags                           */
  __IOM uint32_t   SEQIEN_CLR;                  /**< SEQ Interrupt Enable Register                      */
  __IOM uint32_t   SFMIF_CLR;                   /**< Soft Modem BUFC Interrupt Flags                    */
  __IOM uint32_t   SFMIEN_CLR;                  /**< Soft Modem BUFC interrupt Enables                  */
  uint32_t         RESERVED5[949U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_TGL;               /**< IP Version                                         */
  __IOM uint32_t   EN_TGL;                      /**< Enable peripheral clock to this module             */
  __IOM uint32_t   LPMODE_TGL;                  /**< Low power mode control                             */
  BUFC_BUF_TypeDef BUF_TGL[4U];                 /**< Data Buffer                                        */
  uint32_t         RESERVED6[2U];               /**< Reserved for future use                            */
  __IOM uint32_t   IF_TGL;                      /**< BUFC Interrupt Flags                               */
  __IOM uint32_t   IEN_TGL;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   SEQIF_TGL;                   /**< SEQ BUFC Interrupt Flags                           */
  __IOM uint32_t   SEQIEN_TGL;                  /**< SEQ Interrupt Enable Register                      */
  __IOM uint32_t   SFMIF_TGL;                   /**< Soft Modem BUFC Interrupt Flags                    */
  __IOM uint32_t   SFMIEN_TGL;                  /**< Soft Modem BUFC interrupt Enables                  */
} BUFC_TypeDef;
/** @} End of group EFR32FG25_BUFC */

/**************************************************************************//**
 * @addtogroup EFR32FG25_BUFC
 * @{
 * @defgroup EFR32FG25_BUFC_BitFields BUFC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for BUFC IPVERSION */
#define _BUFC_IPVERSION_RESETVALUE                           0x00000003UL                             /**< Default value for BUFC_IPVERSION            */
#define _BUFC_IPVERSION_MASK                                 0xFFFFFFFFUL                             /**< Mask for BUFC_IPVERSION                     */
#define _BUFC_IPVERSION_IPVERSION_SHIFT                      0                                        /**< Shift value for BUFC_IPVERSION              */
#define _BUFC_IPVERSION_IPVERSION_MASK                       0xFFFFFFFFUL                             /**< Bit mask for BUFC_IPVERSION                 */
#define _BUFC_IPVERSION_IPVERSION_DEFAULT                    0x00000003UL                             /**< Mode DEFAULT for BUFC_IPVERSION             */
#define BUFC_IPVERSION_IPVERSION_DEFAULT                     (_BUFC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_IPVERSION     */

/* Bit fields for BUFC EN */
#define _BUFC_EN_RESETVALUE                                  0x00000000UL               /**< Default value for BUFC_EN                   */
#define _BUFC_EN_MASK                                        0x00000001UL               /**< Mask for BUFC_EN                            */
#define BUFC_EN_EN                                           (0x1UL << 0)               /**< Enable peripheral clock to this module      */
#define _BUFC_EN_EN_SHIFT                                    0                          /**< Shift value for BUFC_EN                     */
#define _BUFC_EN_EN_MASK                                     0x1UL                      /**< Bit mask for BUFC_EN                        */
#define _BUFC_EN_EN_DEFAULT                                  0x00000000UL               /**< Mode DEFAULT for BUFC_EN                    */
#define BUFC_EN_EN_DEFAULT                                   (_BUFC_EN_EN_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_EN            */

/* Bit fields for BUFC LPMODE */
#define _BUFC_LPMODE_RESETVALUE                              0x00000000UL                     /**< Default value for BUFC_LPMODE               */
#define _BUFC_LPMODE_MASK                                    0x00000003UL                     /**< Mask for BUFC_LPMODE                        */
#define _BUFC_LPMODE_LPEN_SHIFT                              0                                /**< Shift value for BUFC_LPEN                   */
#define _BUFC_LPMODE_LPEN_MASK                               0x3UL                            /**< Bit mask for BUFC_LPEN                      */
#define _BUFC_LPMODE_LPEN_DEFAULT                            0x00000000UL                     /**< Mode DEFAULT for BUFC_LPMODE                */
#define BUFC_LPMODE_LPEN_DEFAULT                             (_BUFC_LPMODE_LPEN_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_LPMODE        */

/* Bit fields for BUFC BUF_CTRL */
#define _BUFC_BUF_CTRL_RESETVALUE                            0x00000000UL                        /**< Default value for BUFC_BUF_CTRL             */
#define _BUFC_BUF_CTRL_MASK                                  0x00000007UL                        /**< Mask for BUFC_BUF_CTRL                      */
#define _BUFC_BUF_CTRL_SIZE_SHIFT                            0                                   /**< Shift value for BUFC_SIZE                   */
#define _BUFC_BUF_CTRL_SIZE_MASK                             0x7UL                               /**< Bit mask for BUFC_SIZE                      */
#define _BUFC_BUF_CTRL_SIZE_DEFAULT                          0x00000000UL                        /**< Mode DEFAULT for BUFC_BUF_CTRL              */
#define _BUFC_BUF_CTRL_SIZE_SIZE64                           0x00000000UL                        /**< Mode SIZE64 for BUFC_BUF_CTRL               */
#define _BUFC_BUF_CTRL_SIZE_SIZE128                          0x00000001UL                        /**< Mode SIZE128 for BUFC_BUF_CTRL              */
#define _BUFC_BUF_CTRL_SIZE_SIZE256                          0x00000002UL                        /**< Mode SIZE256 for BUFC_BUF_CTRL              */
#define _BUFC_BUF_CTRL_SIZE_SIZE512                          0x00000003UL                        /**< Mode SIZE512 for BUFC_BUF_CTRL              */
#define _BUFC_BUF_CTRL_SIZE_SIZE1024                         0x00000004UL                        /**< Mode SIZE1024 for BUFC_BUF_CTRL             */
#define _BUFC_BUF_CTRL_SIZE_SIZE2048                         0x00000005UL                        /**< Mode SIZE2048 for BUFC_BUF_CTRL             */
#define _BUFC_BUF_CTRL_SIZE_SIZE4096                         0x00000006UL                        /**< Mode SIZE4096 for BUFC_BUF_CTRL             */
#define BUFC_BUF_CTRL_SIZE_DEFAULT                           (_BUFC_BUF_CTRL_SIZE_DEFAULT << 0)  /**< Shifted mode DEFAULT for BUFC_BUF_CTRL      */
#define BUFC_BUF_CTRL_SIZE_SIZE64                            (_BUFC_BUF_CTRL_SIZE_SIZE64 << 0)   /**< Shifted mode SIZE64 for BUFC_BUF_CTRL       */
#define BUFC_BUF_CTRL_SIZE_SIZE128                           (_BUFC_BUF_CTRL_SIZE_SIZE128 << 0)  /**< Shifted mode SIZE128 for BUFC_BUF_CTRL      */
#define BUFC_BUF_CTRL_SIZE_SIZE256                           (_BUFC_BUF_CTRL_SIZE_SIZE256 << 0)  /**< Shifted mode SIZE256 for BUFC_BUF_CTRL      */
#define BUFC_BUF_CTRL_SIZE_SIZE512                           (_BUFC_BUF_CTRL_SIZE_SIZE512 << 0)  /**< Shifted mode SIZE512 for BUFC_BUF_CTRL      */
#define BUFC_BUF_CTRL_SIZE_SIZE1024                          (_BUFC_BUF_CTRL_SIZE_SIZE1024 << 0) /**< Shifted mode SIZE1024 for BUFC_BUF_CTRL     */
#define BUFC_BUF_CTRL_SIZE_SIZE2048                          (_BUFC_BUF_CTRL_SIZE_SIZE2048 << 0) /**< Shifted mode SIZE2048 for BUFC_BUF_CTRL     */
#define BUFC_BUF_CTRL_SIZE_SIZE4096                          (_BUFC_BUF_CTRL_SIZE_SIZE4096 << 0) /**< Shifted mode SIZE4096 for BUFC_BUF_CTRL     */

/* Bit fields for BUFC BUF_ADDR */
#define _BUFC_BUF_ADDR_RESETVALUE                            0x20000000UL                       /**< Default value for BUFC_BUF_ADDR             */
#define _BUFC_BUF_ADDR_MASK                                  0xFFFFFFFCUL                       /**< Mask for BUFC_BUF_ADDR                      */
#define _BUFC_BUF_ADDR_ADDR_SHIFT                            2                                  /**< Shift value for BUFC_ADDR                   */
#define _BUFC_BUF_ADDR_ADDR_MASK                             0xFFFFFFFCUL                       /**< Bit mask for BUFC_ADDR                      */
#define _BUFC_BUF_ADDR_ADDR_DEFAULT                          0x08000000UL                       /**< Mode DEFAULT for BUFC_BUF_ADDR              */
#define BUFC_BUF_ADDR_ADDR_DEFAULT                           (_BUFC_BUF_ADDR_ADDR_DEFAULT << 2) /**< Shifted mode DEFAULT for BUFC_BUF_ADDR      */

/* Bit fields for BUFC BUF_WRITEOFFSET */
#define _BUFC_BUF_WRITEOFFSET_RESETVALUE                     0x00000000UL                                     /**< Default value for BUFC_BUF_WRITEOFFSET      */
#define _BUFC_BUF_WRITEOFFSET_MASK                           0x00001FFFUL                                     /**< Mask for BUFC_BUF_WRITEOFFSET               */
#define _BUFC_BUF_WRITEOFFSET_WRITEOFFSET_SHIFT              0                                                /**< Shift value for BUFC_WRITEOFFSET            */
#define _BUFC_BUF_WRITEOFFSET_WRITEOFFSET_MASK               0x1FFFUL                                         /**< Bit mask for BUFC_WRITEOFFSET               */
#define _BUFC_BUF_WRITEOFFSET_WRITEOFFSET_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for BUFC_BUF_WRITEOFFSET       */
#define BUFC_BUF_WRITEOFFSET_WRITEOFFSET_DEFAULT             (_BUFC_BUF_WRITEOFFSET_WRITEOFFSET_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_WRITEOFFSET*/

/* Bit fields for BUFC BUF_READOFFSET */
#define _BUFC_BUF_READOFFSET_RESETVALUE                      0x00000000UL                                   /**< Default value for BUFC_BUF_READOFFSET       */
#define _BUFC_BUF_READOFFSET_MASK                            0x00001FFFUL                                   /**< Mask for BUFC_BUF_READOFFSET                */
#define _BUFC_BUF_READOFFSET_READOFFSET_SHIFT                0                                              /**< Shift value for BUFC_READOFFSET             */
#define _BUFC_BUF_READOFFSET_READOFFSET_MASK                 0x1FFFUL                                       /**< Bit mask for BUFC_READOFFSET                */
#define _BUFC_BUF_READOFFSET_READOFFSET_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for BUFC_BUF_READOFFSET        */
#define BUFC_BUF_READOFFSET_READOFFSET_DEFAULT               (_BUFC_BUF_READOFFSET_READOFFSET_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_READOFFSET*/

/* Bit fields for BUFC BUF_READDATA */
#define _BUFC_BUF_READDATA_RESETVALUE                        0x00000000UL                               /**< Default value for BUFC_BUF_READDATA         */
#define _BUFC_BUF_READDATA_MASK                              0x000000FFUL                               /**< Mask for BUFC_BUF_READDATA                  */
#define _BUFC_BUF_READDATA_READDATA_SHIFT                    0                                          /**< Shift value for BUFC_READDATA               */
#define _BUFC_BUF_READDATA_READDATA_MASK                     0xFFUL                                     /**< Bit mask for BUFC_READDATA                  */
#define _BUFC_BUF_READDATA_READDATA_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for BUFC_BUF_READDATA          */
#define BUFC_BUF_READDATA_READDATA_DEFAULT                   (_BUFC_BUF_READDATA_READDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_READDATA  */

/* Bit fields for BUFC BUF_WRITEDATA */
#define _BUFC_BUF_WRITEDATA_RESETVALUE                       0x00000000UL                                 /**< Default value for BUFC_BUF_WRITEDATA        */
#define _BUFC_BUF_WRITEDATA_MASK                             0x000000FFUL                                 /**< Mask for BUFC_BUF_WRITEDATA                 */
#define _BUFC_BUF_WRITEDATA_WRITEDATA_SHIFT                  0                                            /**< Shift value for BUFC_WRITEDATA              */
#define _BUFC_BUF_WRITEDATA_WRITEDATA_MASK                   0xFFUL                                       /**< Bit mask for BUFC_WRITEDATA                 */
#define _BUFC_BUF_WRITEDATA_WRITEDATA_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for BUFC_BUF_WRITEDATA         */
#define BUFC_BUF_WRITEDATA_WRITEDATA_DEFAULT                 (_BUFC_BUF_WRITEDATA_WRITEDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_WRITEDATA */

/* Bit fields for BUFC BUF_XWRITE */
#define _BUFC_BUF_XWRITE_RESETVALUE                          0x00000000UL                                 /**< Default value for BUFC_BUF_XWRITE           */
#define _BUFC_BUF_XWRITE_MASK                                0x000000FFUL                                 /**< Mask for BUFC_BUF_XWRITE                    */
#define _BUFC_BUF_XWRITE_XORWRITEDATA_SHIFT                  0                                            /**< Shift value for BUFC_XORWRITEDATA           */
#define _BUFC_BUF_XWRITE_XORWRITEDATA_MASK                   0xFFUL                                       /**< Bit mask for BUFC_XORWRITEDATA              */
#define _BUFC_BUF_XWRITE_XORWRITEDATA_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for BUFC_BUF_XWRITE            */
#define BUFC_BUF_XWRITE_XORWRITEDATA_DEFAULT                 (_BUFC_BUF_XWRITE_XORWRITEDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_XWRITE    */

/* Bit fields for BUFC BUF_STATUS */
#define _BUFC_BUF_STATUS_RESETVALUE                          0x00000000UL                                   /**< Default value for BUFC_BUF_STATUS           */
#define _BUFC_BUF_STATUS_MASK                                0x01111FFFUL                                   /**< Mask for BUFC_BUF_STATUS                    */
#define _BUFC_BUF_STATUS_BYTES_SHIFT                         0                                              /**< Shift value for BUFC_BYTES                  */
#define _BUFC_BUF_STATUS_BYTES_MASK                          0x1FFFUL                                       /**< Bit mask for BUFC_BYTES                     */
#define _BUFC_BUF_STATUS_BYTES_DEFAULT                       0x00000000UL                                   /**< Mode DEFAULT for BUFC_BUF_STATUS            */
#define BUFC_BUF_STATUS_BYTES_DEFAULT                        (_BUFC_BUF_STATUS_BYTES_DEFAULT << 0)          /**< Shifted mode DEFAULT for BUFC_BUF_STATUS    */
#define BUFC_BUF_STATUS_THRESHOLDFLAG                        (0x1UL << 20)                                  /**< Buffer Threshold Flag                       */
#define _BUFC_BUF_STATUS_THRESHOLDFLAG_SHIFT                 20                                             /**< Shift value for BUFC_THRESHOLDFLAG          */
#define _BUFC_BUF_STATUS_THRESHOLDFLAG_MASK                  0x100000UL                                     /**< Bit mask for BUFC_THRESHOLDFLAG             */
#define _BUFC_BUF_STATUS_THRESHOLDFLAG_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for BUFC_BUF_STATUS            */
#define BUFC_BUF_STATUS_THRESHOLDFLAG_DEFAULT                (_BUFC_BUF_STATUS_THRESHOLDFLAG_DEFAULT << 20) /**< Shifted mode DEFAULT for BUFC_BUF_STATUS    */

/* Bit fields for BUFC BUF_THRESHOLDCTRL */
#define _BUFC_BUF_THRESHOLDCTRL_RESETVALUE                   0x00000000UL                                              /**< Default value for BUFC_BUF_THRESHOLDCTRL    */
#define _BUFC_BUF_THRESHOLDCTRL_MASK                         0x00003FFFUL                                              /**< Mask for BUFC_BUF_THRESHOLDCTRL             */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLD_SHIFT              0                                                         /**< Shift value for BUFC_THRESHOLD              */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLD_MASK               0x1FFFUL                                                  /**< Bit mask for BUFC_THRESHOLD                 */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLD_DEFAULT            0x00000000UL                                              /**< Mode DEFAULT for BUFC_BUF_THRESHOLDCTRL     */
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLD_DEFAULT             (_BUFC_BUF_THRESHOLDCTRL_THRESHOLD_DEFAULT << 0)          /**< Shifted mode DEFAULT for BUFC_BUF_THRESHOLDCTRL*/
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE                 (0x1UL << 13)                                             /**< Buffer Threshold Mode                       */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_SHIFT          13                                                        /**< Shift value for BUFC_THRESHOLDMODE          */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_MASK           0x2000UL                                                  /**< Bit mask for BUFC_THRESHOLDMODE             */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_DEFAULT        0x00000000UL                                              /**< Mode DEFAULT for BUFC_BUF_THRESHOLDCTRL     */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LARGER         0x00000000UL                                              /**< Mode LARGER for BUFC_BUF_THRESHOLDCTRL      */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LESSOREQUAL    0x00000001UL                                              /**< Mode LESSOREQUAL for BUFC_BUF_THRESHOLDCTRL */
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_DEFAULT         (_BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_DEFAULT << 13)     /**< Shifted mode DEFAULT for BUFC_BUF_THRESHOLDCTRL*/
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LARGER          (_BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LARGER << 13)      /**< Shifted mode LARGER for BUFC_BUF_THRESHOLDCTRL*/
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LESSOREQUAL     (_BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LESSOREQUAL << 13) /**< Shifted mode LESSOREQUAL for BUFC_BUF_THRESHOLDCTRL*/

/* Bit fields for BUFC BUF_CMD */
#define _BUFC_BUF_CMD_RESETVALUE                             0x00000000UL                          /**< Default value for BUFC_BUF_CMD              */
#define _BUFC_BUF_CMD_MASK                                   0x0000000FUL                          /**< Mask for BUFC_BUF_CMD                       */
#define BUFC_BUF_CMD_CLEAR                                   (0x1UL << 0)                          /**< Buffer Clear                                */
#define _BUFC_BUF_CMD_CLEAR_SHIFT                            0                                     /**< Shift value for BUFC_CLEAR                  */
#define _BUFC_BUF_CMD_CLEAR_MASK                             0x1UL                                 /**< Bit mask for BUFC_CLEAR                     */
#define _BUFC_BUF_CMD_CLEAR_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_BUF_CMD               */
#define BUFC_BUF_CMD_CLEAR_DEFAULT                           (_BUFC_BUF_CMD_CLEAR_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_BUF_CMD       */
#define BUFC_BUF_CMD_PREFETCH                                (0x1UL << 1)                          /**< Prefetch                                    */
#define _BUFC_BUF_CMD_PREFETCH_SHIFT                         1                                     /**< Shift value for BUFC_PREFETCH               */
#define _BUFC_BUF_CMD_PREFETCH_MASK                          0x2UL                                 /**< Bit mask for BUFC_PREFETCH                  */
#define _BUFC_BUF_CMD_PREFETCH_DEFAULT                       0x00000000UL                          /**< Mode DEFAULT for BUFC_BUF_CMD               */
#define BUFC_BUF_CMD_PREFETCH_DEFAULT                        (_BUFC_BUF_CMD_PREFETCH_DEFAULT << 1) /**< Shifted mode DEFAULT for BUFC_BUF_CMD       */

/* Bit fields for BUFC BUF_READDATA32 */
#define _BUFC_BUF_READDATA32_RESETVALUE                      0x00000000UL                                   /**< Default value for BUFC_BUF_READDATA32       */
#define _BUFC_BUF_READDATA32_MASK                            0xFFFFFFFFUL                                   /**< Mask for BUFC_BUF_READDATA32                */
#define _BUFC_BUF_READDATA32_READDATA32_SHIFT                0                                              /**< Shift value for BUFC_READDATA32             */
#define _BUFC_BUF_READDATA32_READDATA32_MASK                 0xFFFFFFFFUL                                   /**< Bit mask for BUFC_READDATA32                */
#define _BUFC_BUF_READDATA32_READDATA32_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for BUFC_BUF_READDATA32        */
#define BUFC_BUF_READDATA32_READDATA32_DEFAULT               (_BUFC_BUF_READDATA32_READDATA32_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_READDATA32*/

/* Bit fields for BUFC BUF_WRITEDATA32 */
#define _BUFC_BUF_WRITEDATA32_RESETVALUE                     0x00000000UL                                     /**< Default value for BUFC_BUF_WRITEDATA32      */
#define _BUFC_BUF_WRITEDATA32_MASK                           0xFFFFFFFFUL                                     /**< Mask for BUFC_BUF_WRITEDATA32               */
#define _BUFC_BUF_WRITEDATA32_WRITEDATA32_SHIFT              0                                                /**< Shift value for BUFC_WRITEDATA32            */
#define _BUFC_BUF_WRITEDATA32_WRITEDATA32_MASK               0xFFFFFFFFUL                                     /**< Bit mask for BUFC_WRITEDATA32               */
#define _BUFC_BUF_WRITEDATA32_WRITEDATA32_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for BUFC_BUF_WRITEDATA32       */
#define BUFC_BUF_WRITEDATA32_WRITEDATA32_DEFAULT             (_BUFC_BUF_WRITEDATA32_WRITEDATA32_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_WRITEDATA32*/

/* Bit fields for BUFC BUF_XWRITE32 */
#define _BUFC_BUF_XWRITE32_RESETVALUE                        0x00000000UL                                     /**< Default value for BUFC_BUF_XWRITE32         */
#define _BUFC_BUF_XWRITE32_MASK                              0xFFFFFFFFUL                                     /**< Mask for BUFC_BUF_XWRITE32                  */
#define _BUFC_BUF_XWRITE32_XORWRITEDATA32_SHIFT              0                                                /**< Shift value for BUFC_XORWRITEDATA32         */
#define _BUFC_BUF_XWRITE32_XORWRITEDATA32_MASK               0xFFFFFFFFUL                                     /**< Bit mask for BUFC_XORWRITEDATA32            */
#define _BUFC_BUF_XWRITE32_XORWRITEDATA32_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for BUFC_BUF_XWRITE32          */
#define BUFC_BUF_XWRITE32_XORWRITEDATA32_DEFAULT             (_BUFC_BUF_XWRITE32_XORWRITEDATA32_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_XWRITE32  */

/* Bit fields for BUFC IF */
#define _BUFC_IF_RESETVALUE                                  0x00000000UL                      /**< Default value for BUFC_IF                   */
#define _BUFC_IF_MASK                                        0x9F1F1F1FUL                      /**< Mask for BUFC_IF                            */
#define BUFC_IF_BUF0OF                                       (0x1UL << 0)                      /**< Buffer 0 Overflow                           */
#define _BUFC_IF_BUF0OF_SHIFT                                0                                 /**< Shift value for BUFC_BUF0OF                 */
#define _BUFC_IF_BUF0OF_MASK                                 0x1UL                             /**< Bit mask for BUFC_BUF0OF                    */
#define _BUFC_IF_BUF0OF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF0OF_DEFAULT                               (_BUFC_IF_BUF0OF_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF0UF                                       (0x1UL << 1)                      /**< Buffer 0 Underflow                          */
#define _BUFC_IF_BUF0UF_SHIFT                                1                                 /**< Shift value for BUFC_BUF0UF                 */
#define _BUFC_IF_BUF0UF_MASK                                 0x2UL                             /**< Bit mask for BUFC_BUF0UF                    */
#define _BUFC_IF_BUF0UF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF0UF_DEFAULT                               (_BUFC_IF_BUF0UF_DEFAULT << 1)    /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF0THR                                      (0x1UL << 2)                      /**< Buffer 0 Threshold Event                    */
#define _BUFC_IF_BUF0THR_SHIFT                               2                                 /**< Shift value for BUFC_BUF0THR                */
#define _BUFC_IF_BUF0THR_MASK                                0x4UL                             /**< Bit mask for BUFC_BUF0THR                   */
#define _BUFC_IF_BUF0THR_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF0THR_DEFAULT                              (_BUFC_IF_BUF0THR_DEFAULT << 2)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF0CORR                                     (0x1UL << 3)                      /**< Buffer 0 Corrupt                            */
#define _BUFC_IF_BUF0CORR_SHIFT                              3                                 /**< Shift value for BUFC_BUF0CORR               */
#define _BUFC_IF_BUF0CORR_MASK                               0x8UL                             /**< Bit mask for BUFC_BUF0CORR                  */
#define _BUFC_IF_BUF0CORR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF0CORR_DEFAULT                             (_BUFC_IF_BUF0CORR_DEFAULT << 3)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF0NWA                                      (0x1UL << 4)                      /**< Buffer 0 Not Word-Aligned                   */
#define _BUFC_IF_BUF0NWA_SHIFT                               4                                 /**< Shift value for BUFC_BUF0NWA                */
#define _BUFC_IF_BUF0NWA_MASK                                0x10UL                            /**< Bit mask for BUFC_BUF0NWA                   */
#define _BUFC_IF_BUF0NWA_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF0NWA_DEFAULT                              (_BUFC_IF_BUF0NWA_DEFAULT << 4)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF1OF                                       (0x1UL << 8)                      /**< Buffer 1 Overflow                           */
#define _BUFC_IF_BUF1OF_SHIFT                                8                                 /**< Shift value for BUFC_BUF1OF                 */
#define _BUFC_IF_BUF1OF_MASK                                 0x100UL                           /**< Bit mask for BUFC_BUF1OF                    */
#define _BUFC_IF_BUF1OF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF1OF_DEFAULT                               (_BUFC_IF_BUF1OF_DEFAULT << 8)    /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF1UF                                       (0x1UL << 9)                      /**< Buffer 1 Underflow                          */
#define _BUFC_IF_BUF1UF_SHIFT                                9                                 /**< Shift value for BUFC_BUF1UF                 */
#define _BUFC_IF_BUF1UF_MASK                                 0x200UL                           /**< Bit mask for BUFC_BUF1UF                    */
#define _BUFC_IF_BUF1UF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF1UF_DEFAULT                               (_BUFC_IF_BUF1UF_DEFAULT << 9)    /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF1THR                                      (0x1UL << 10)                     /**< Buffer 1 Threshold Event                    */
#define _BUFC_IF_BUF1THR_SHIFT                               10                                /**< Shift value for BUFC_BUF1THR                */
#define _BUFC_IF_BUF1THR_MASK                                0x400UL                           /**< Bit mask for BUFC_BUF1THR                   */
#define _BUFC_IF_BUF1THR_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF1THR_DEFAULT                              (_BUFC_IF_BUF1THR_DEFAULT << 10)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF1CORR                                     (0x1UL << 11)                     /**< Buffer 1 Corrupt                            */
#define _BUFC_IF_BUF1CORR_SHIFT                              11                                /**< Shift value for BUFC_BUF1CORR               */
#define _BUFC_IF_BUF1CORR_MASK                               0x800UL                           /**< Bit mask for BUFC_BUF1CORR                  */
#define _BUFC_IF_BUF1CORR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF1CORR_DEFAULT                             (_BUFC_IF_BUF1CORR_DEFAULT << 11) /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF1NWA                                      (0x1UL << 12)                     /**< Buffer 1 Not Word-Aligned                   */
#define _BUFC_IF_BUF1NWA_SHIFT                               12                                /**< Shift value for BUFC_BUF1NWA                */
#define _BUFC_IF_BUF1NWA_MASK                                0x1000UL                          /**< Bit mask for BUFC_BUF1NWA                   */
#define _BUFC_IF_BUF1NWA_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF1NWA_DEFAULT                              (_BUFC_IF_BUF1NWA_DEFAULT << 12)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF2OF                                       (0x1UL << 16)                     /**< Buffer 2 Overflow                           */
#define _BUFC_IF_BUF2OF_SHIFT                                16                                /**< Shift value for BUFC_BUF2OF                 */
#define _BUFC_IF_BUF2OF_MASK                                 0x10000UL                         /**< Bit mask for BUFC_BUF2OF                    */
#define _BUFC_IF_BUF2OF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF2OF_DEFAULT                               (_BUFC_IF_BUF2OF_DEFAULT << 16)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF2UF                                       (0x1UL << 17)                     /**< Buffer 2 Underflow                          */
#define _BUFC_IF_BUF2UF_SHIFT                                17                                /**< Shift value for BUFC_BUF2UF                 */
#define _BUFC_IF_BUF2UF_MASK                                 0x20000UL                         /**< Bit mask for BUFC_BUF2UF                    */
#define _BUFC_IF_BUF2UF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF2UF_DEFAULT                               (_BUFC_IF_BUF2UF_DEFAULT << 17)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF2THR                                      (0x1UL << 18)                     /**< Buffer 2 Threshold Event                    */
#define _BUFC_IF_BUF2THR_SHIFT                               18                                /**< Shift value for BUFC_BUF2THR                */
#define _BUFC_IF_BUF2THR_MASK                                0x40000UL                         /**< Bit mask for BUFC_BUF2THR                   */
#define _BUFC_IF_BUF2THR_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF2THR_DEFAULT                              (_BUFC_IF_BUF2THR_DEFAULT << 18)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF2CORR                                     (0x1UL << 19)                     /**< Buffer 2 Corrupt                            */
#define _BUFC_IF_BUF2CORR_SHIFT                              19                                /**< Shift value for BUFC_BUF2CORR               */
#define _BUFC_IF_BUF2CORR_MASK                               0x80000UL                         /**< Bit mask for BUFC_BUF2CORR                  */
#define _BUFC_IF_BUF2CORR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF2CORR_DEFAULT                             (_BUFC_IF_BUF2CORR_DEFAULT << 19) /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF2NWA                                      (0x1UL << 20)                     /**< Buffer 2 Not Word-Aligned                   */
#define _BUFC_IF_BUF2NWA_SHIFT                               20                                /**< Shift value for BUFC_BUF2NWA                */
#define _BUFC_IF_BUF2NWA_MASK                                0x100000UL                        /**< Bit mask for BUFC_BUF2NWA                   */
#define _BUFC_IF_BUF2NWA_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF2NWA_DEFAULT                              (_BUFC_IF_BUF2NWA_DEFAULT << 20)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF3OF                                       (0x1UL << 24)                     /**< Buffer 3 Overflow                           */
#define _BUFC_IF_BUF3OF_SHIFT                                24                                /**< Shift value for BUFC_BUF3OF                 */
#define _BUFC_IF_BUF3OF_MASK                                 0x1000000UL                       /**< Bit mask for BUFC_BUF3OF                    */
#define _BUFC_IF_BUF3OF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF3OF_DEFAULT                               (_BUFC_IF_BUF3OF_DEFAULT << 24)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF3UF                                       (0x1UL << 25)                     /**< Buffer 3 Underflow                          */
#define _BUFC_IF_BUF3UF_SHIFT                                25                                /**< Shift value for BUFC_BUF3UF                 */
#define _BUFC_IF_BUF3UF_MASK                                 0x2000000UL                       /**< Bit mask for BUFC_BUF3UF                    */
#define _BUFC_IF_BUF3UF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF3UF_DEFAULT                               (_BUFC_IF_BUF3UF_DEFAULT << 25)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF3THR                                      (0x1UL << 26)                     /**< Buffer 3 Threshold Event                    */
#define _BUFC_IF_BUF3THR_SHIFT                               26                                /**< Shift value for BUFC_BUF3THR                */
#define _BUFC_IF_BUF3THR_MASK                                0x4000000UL                       /**< Bit mask for BUFC_BUF3THR                   */
#define _BUFC_IF_BUF3THR_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF3THR_DEFAULT                              (_BUFC_IF_BUF3THR_DEFAULT << 26)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF3CORR                                     (0x1UL << 27)                     /**< Buffer 3 Corrupt                            */
#define _BUFC_IF_BUF3CORR_SHIFT                              27                                /**< Shift value for BUFC_BUF3CORR               */
#define _BUFC_IF_BUF3CORR_MASK                               0x8000000UL                       /**< Bit mask for BUFC_BUF3CORR                  */
#define _BUFC_IF_BUF3CORR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF3CORR_DEFAULT                             (_BUFC_IF_BUF3CORR_DEFAULT << 27) /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF3NWA                                      (0x1UL << 28)                     /**< Buffer 3 Not Word-Aligned                   */
#define _BUFC_IF_BUF3NWA_SHIFT                               28                                /**< Shift value for BUFC_BUF3NWA                */
#define _BUFC_IF_BUF3NWA_MASK                                0x10000000UL                      /**< Bit mask for BUFC_BUF3NWA                   */
#define _BUFC_IF_BUF3NWA_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF3NWA_DEFAULT                              (_BUFC_IF_BUF3NWA_DEFAULT << 28)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUSERROR                                     (0x1UL << 31)                     /**< Bus Error                                   */
#define _BUFC_IF_BUSERROR_SHIFT                              31                                /**< Shift value for BUFC_BUSERROR               */
#define _BUFC_IF_BUSERROR_MASK                               0x80000000UL                      /**< Bit mask for BUFC_BUSERROR                  */
#define _BUFC_IF_BUSERROR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUSERROR_DEFAULT                             (_BUFC_IF_BUSERROR_DEFAULT << 31) /**< Shifted mode DEFAULT for BUFC_IF            */

/* Bit fields for BUFC IEN */
#define _BUFC_IEN_RESETVALUE                                 0x00000000UL                       /**< Default value for BUFC_IEN                  */
#define _BUFC_IEN_MASK                                       0x9F1F1F1FUL                       /**< Mask for BUFC_IEN                           */
#define BUFC_IEN_BUF0OF                                      (0x1UL << 0)                       /**< BUF0OF Interrupt Enable                     */
#define _BUFC_IEN_BUF0OF_SHIFT                               0                                  /**< Shift value for BUFC_BUF0OF                 */
#define _BUFC_IEN_BUF0OF_MASK                                0x1UL                              /**< Bit mask for BUFC_BUF0OF                    */
#define _BUFC_IEN_BUF0OF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF0OF_DEFAULT                              (_BUFC_IEN_BUF0OF_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF0UF                                      (0x1UL << 1)                       /**< BUF0UF Interrupt Enable                     */
#define _BUFC_IEN_BUF0UF_SHIFT                               1                                  /**< Shift value for BUFC_BUF0UF                 */
#define _BUFC_IEN_BUF0UF_MASK                                0x2UL                              /**< Bit mask for BUFC_BUF0UF                    */
#define _BUFC_IEN_BUF0UF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF0UF_DEFAULT                              (_BUFC_IEN_BUF0UF_DEFAULT << 1)    /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF0THR                                     (0x1UL << 2)                       /**< BUF0THR Interrupt Enable                    */
#define _BUFC_IEN_BUF0THR_SHIFT                              2                                  /**< Shift value for BUFC_BUF0THR                */
#define _BUFC_IEN_BUF0THR_MASK                               0x4UL                              /**< Bit mask for BUFC_BUF0THR                   */
#define _BUFC_IEN_BUF0THR_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF0THR_DEFAULT                             (_BUFC_IEN_BUF0THR_DEFAULT << 2)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF0CORR                                    (0x1UL << 3)                       /**< BUF0CORR Interrupt Enable                   */
#define _BUFC_IEN_BUF0CORR_SHIFT                             3                                  /**< Shift value for BUFC_BUF0CORR               */
#define _BUFC_IEN_BUF0CORR_MASK                              0x8UL                              /**< Bit mask for BUFC_BUF0CORR                  */
#define _BUFC_IEN_BUF0CORR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF0CORR_DEFAULT                            (_BUFC_IEN_BUF0CORR_DEFAULT << 3)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF0NWA                                     (0x1UL << 4)                       /**< BUF0NWA Interrupt Enable                    */
#define _BUFC_IEN_BUF0NWA_SHIFT                              4                                  /**< Shift value for BUFC_BUF0NWA                */
#define _BUFC_IEN_BUF0NWA_MASK                               0x10UL                             /**< Bit mask for BUFC_BUF0NWA                   */
#define _BUFC_IEN_BUF0NWA_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF0NWA_DEFAULT                             (_BUFC_IEN_BUF0NWA_DEFAULT << 4)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF1OF                                      (0x1UL << 8)                       /**< BUF1OF Interrupt Enable                     */
#define _BUFC_IEN_BUF1OF_SHIFT                               8                                  /**< Shift value for BUFC_BUF1OF                 */
#define _BUFC_IEN_BUF1OF_MASK                                0x100UL                            /**< Bit mask for BUFC_BUF1OF                    */
#define _BUFC_IEN_BUF1OF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF1OF_DEFAULT                              (_BUFC_IEN_BUF1OF_DEFAULT << 8)    /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF1UF                                      (0x1UL << 9)                       /**< BUF1UF Interrupt Enable                     */
#define _BUFC_IEN_BUF1UF_SHIFT                               9                                  /**< Shift value for BUFC_BUF1UF                 */
#define _BUFC_IEN_BUF1UF_MASK                                0x200UL                            /**< Bit mask for BUFC_BUF1UF                    */
#define _BUFC_IEN_BUF1UF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF1UF_DEFAULT                              (_BUFC_IEN_BUF1UF_DEFAULT << 9)    /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF1THR                                     (0x1UL << 10)                      /**< BUF1THR Interrupt Enable                    */
#define _BUFC_IEN_BUF1THR_SHIFT                              10                                 /**< Shift value for BUFC_BUF1THR                */
#define _BUFC_IEN_BUF1THR_MASK                               0x400UL                            /**< Bit mask for BUFC_BUF1THR                   */
#define _BUFC_IEN_BUF1THR_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF1THR_DEFAULT                             (_BUFC_IEN_BUF1THR_DEFAULT << 10)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF1CORR                                    (0x1UL << 11)                      /**< BUF1CORR Interrupt Enable                   */
#define _BUFC_IEN_BUF1CORR_SHIFT                             11                                 /**< Shift value for BUFC_BUF1CORR               */
#define _BUFC_IEN_BUF1CORR_MASK                              0x800UL                            /**< Bit mask for BUFC_BUF1CORR                  */
#define _BUFC_IEN_BUF1CORR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF1CORR_DEFAULT                            (_BUFC_IEN_BUF1CORR_DEFAULT << 11) /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF1NWA                                     (0x1UL << 12)                      /**< BUF1NWA Interrupt Enable                    */
#define _BUFC_IEN_BUF1NWA_SHIFT                              12                                 /**< Shift value for BUFC_BUF1NWA                */
#define _BUFC_IEN_BUF1NWA_MASK                               0x1000UL                           /**< Bit mask for BUFC_BUF1NWA                   */
#define _BUFC_IEN_BUF1NWA_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF1NWA_DEFAULT                             (_BUFC_IEN_BUF1NWA_DEFAULT << 12)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF2OF                                      (0x1UL << 16)                      /**< BUF2OF Interrupt Enable                     */
#define _BUFC_IEN_BUF2OF_SHIFT                               16                                 /**< Shift value for BUFC_BUF2OF                 */
#define _BUFC_IEN_BUF2OF_MASK                                0x10000UL                          /**< Bit mask for BUFC_BUF2OF                    */
#define _BUFC_IEN_BUF2OF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF2OF_DEFAULT                              (_BUFC_IEN_BUF2OF_DEFAULT << 16)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF2UF                                      (0x1UL << 17)                      /**< BUF2UF Interrupt Enable                     */
#define _BUFC_IEN_BUF2UF_SHIFT                               17                                 /**< Shift value for BUFC_BUF2UF                 */
#define _BUFC_IEN_BUF2UF_MASK                                0x20000UL                          /**< Bit mask for BUFC_BUF2UF                    */
#define _BUFC_IEN_BUF2UF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF2UF_DEFAULT                              (_BUFC_IEN_BUF2UF_DEFAULT << 17)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF2THR                                     (0x1UL << 18)                      /**< BUF2THR Interrupt Enable                    */
#define _BUFC_IEN_BUF2THR_SHIFT                              18                                 /**< Shift value for BUFC_BUF2THR                */
#define _BUFC_IEN_BUF2THR_MASK                               0x40000UL                          /**< Bit mask for BUFC_BUF2THR                   */
#define _BUFC_IEN_BUF2THR_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF2THR_DEFAULT                             (_BUFC_IEN_BUF2THR_DEFAULT << 18)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF2CORR                                    (0x1UL << 19)                      /**< BUF2CORR Interrupt Enable                   */
#define _BUFC_IEN_BUF2CORR_SHIFT                             19                                 /**< Shift value for BUFC_BUF2CORR               */
#define _BUFC_IEN_BUF2CORR_MASK                              0x80000UL                          /**< Bit mask for BUFC_BUF2CORR                  */
#define _BUFC_IEN_BUF2CORR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF2CORR_DEFAULT                            (_BUFC_IEN_BUF2CORR_DEFAULT << 19) /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF2NWA                                     (0x1UL << 20)                      /**< BUF2NWA Interrupt Enable                    */
#define _BUFC_IEN_BUF2NWA_SHIFT                              20                                 /**< Shift value for BUFC_BUF2NWA                */
#define _BUFC_IEN_BUF2NWA_MASK                               0x100000UL                         /**< Bit mask for BUFC_BUF2NWA                   */
#define _BUFC_IEN_BUF2NWA_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF2NWA_DEFAULT                             (_BUFC_IEN_BUF2NWA_DEFAULT << 20)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF3OF                                      (0x1UL << 24)                      /**< BUF3OF Interrupt Enable                     */
#define _BUFC_IEN_BUF3OF_SHIFT                               24                                 /**< Shift value for BUFC_BUF3OF                 */
#define _BUFC_IEN_BUF3OF_MASK                                0x1000000UL                        /**< Bit mask for BUFC_BUF3OF                    */
#define _BUFC_IEN_BUF3OF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF3OF_DEFAULT                              (_BUFC_IEN_BUF3OF_DEFAULT << 24)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF3UF                                      (0x1UL << 25)                      /**< BUF3UF Interrupt Enable                     */
#define _BUFC_IEN_BUF3UF_SHIFT                               25                                 /**< Shift value for BUFC_BUF3UF                 */
#define _BUFC_IEN_BUF3UF_MASK                                0x2000000UL                        /**< Bit mask for BUFC_BUF3UF                    */
#define _BUFC_IEN_BUF3UF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF3UF_DEFAULT                              (_BUFC_IEN_BUF3UF_DEFAULT << 25)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF3THR                                     (0x1UL << 26)                      /**< BUF3THR Interrupt Enable                    */
#define _BUFC_IEN_BUF3THR_SHIFT                              26                                 /**< Shift value for BUFC_BUF3THR                */
#define _BUFC_IEN_BUF3THR_MASK                               0x4000000UL                        /**< Bit mask for BUFC_BUF3THR                   */
#define _BUFC_IEN_BUF3THR_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF3THR_DEFAULT                             (_BUFC_IEN_BUF3THR_DEFAULT << 26)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF3CORR                                    (0x1UL << 27)                      /**< BUF3CORR Interrupt Enable                   */
#define _BUFC_IEN_BUF3CORR_SHIFT                             27                                 /**< Shift value for BUFC_BUF3CORR               */
#define _BUFC_IEN_BUF3CORR_MASK                              0x8000000UL                        /**< Bit mask for BUFC_BUF3CORR                  */
#define _BUFC_IEN_BUF3CORR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF3CORR_DEFAULT                            (_BUFC_IEN_BUF3CORR_DEFAULT << 27) /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF3NWA                                     (0x1UL << 28)                      /**< BUF3NWA Interrupt Enable                    */
#define _BUFC_IEN_BUF3NWA_SHIFT                              28                                 /**< Shift value for BUFC_BUF3NWA                */
#define _BUFC_IEN_BUF3NWA_MASK                               0x10000000UL                       /**< Bit mask for BUFC_BUF3NWA                   */
#define _BUFC_IEN_BUF3NWA_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF3NWA_DEFAULT                             (_BUFC_IEN_BUF3NWA_DEFAULT << 28)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUSERROR                                    (0x1UL << 31)                      /**< BUSERROR Interrupt Enable                   */
#define _BUFC_IEN_BUSERROR_SHIFT                             31                                 /**< Shift value for BUFC_BUSERROR               */
#define _BUFC_IEN_BUSERROR_MASK                              0x80000000UL                       /**< Bit mask for BUFC_BUSERROR                  */
#define _BUFC_IEN_BUSERROR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUSERROR_DEFAULT                            (_BUFC_IEN_BUSERROR_DEFAULT << 31) /**< Shifted mode DEFAULT for BUFC_IEN           */

/* Bit fields for BUFC SEQIF */
#define _BUFC_SEQIF_RESETVALUE                               0x00000000UL                         /**< Default value for BUFC_SEQIF                */
#define _BUFC_SEQIF_MASK                                     0x9F1F1F1FUL                         /**< Mask for BUFC_SEQIF                         */
#define BUFC_SEQIF_BUF0OF                                    (0x1UL << 0)                         /**< Buffer 0 Overflow                           */
#define _BUFC_SEQIF_BUF0OF_SHIFT                             0                                    /**< Shift value for BUFC_BUF0OF                 */
#define _BUFC_SEQIF_BUF0OF_MASK                              0x1UL                                /**< Bit mask for BUFC_BUF0OF                    */
#define _BUFC_SEQIF_BUF0OF_DEFAULT                           0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF0OF_DEFAULT                            (_BUFC_SEQIF_BUF0OF_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF0UF                                    (0x1UL << 1)                         /**< Buffer 0 Underflow                          */
#define _BUFC_SEQIF_BUF0UF_SHIFT                             1                                    /**< Shift value for BUFC_BUF0UF                 */
#define _BUFC_SEQIF_BUF0UF_MASK                              0x2UL                                /**< Bit mask for BUFC_BUF0UF                    */
#define _BUFC_SEQIF_BUF0UF_DEFAULT                           0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF0UF_DEFAULT                            (_BUFC_SEQIF_BUF0UF_DEFAULT << 1)    /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF0THR                                   (0x1UL << 2)                         /**< Buffer 0 Threshold Event                    */
#define _BUFC_SEQIF_BUF0THR_SHIFT                            2                                    /**< Shift value for BUFC_BUF0THR                */
#define _BUFC_SEQIF_BUF0THR_MASK                             0x4UL                                /**< Bit mask for BUFC_BUF0THR                   */
#define _BUFC_SEQIF_BUF0THR_DEFAULT                          0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF0THR_DEFAULT                           (_BUFC_SEQIF_BUF0THR_DEFAULT << 2)   /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF0CORR                                  (0x1UL << 3)                         /**< Buffer 0 Corrupt                            */
#define _BUFC_SEQIF_BUF0CORR_SHIFT                           3                                    /**< Shift value for BUFC_BUF0CORR               */
#define _BUFC_SEQIF_BUF0CORR_MASK                            0x8UL                                /**< Bit mask for BUFC_BUF0CORR                  */
#define _BUFC_SEQIF_BUF0CORR_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF0CORR_DEFAULT                          (_BUFC_SEQIF_BUF0CORR_DEFAULT << 3)  /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF0NWA                                   (0x1UL << 4)                         /**< Buffer 0 Not Word-Aligned                   */
#define _BUFC_SEQIF_BUF0NWA_SHIFT                            4                                    /**< Shift value for BUFC_BUF0NWA                */
#define _BUFC_SEQIF_BUF0NWA_MASK                             0x10UL                               /**< Bit mask for BUFC_BUF0NWA                   */
#define _BUFC_SEQIF_BUF0NWA_DEFAULT                          0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF0NWA_DEFAULT                           (_BUFC_SEQIF_BUF0NWA_DEFAULT << 4)   /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF1OF                                    (0x1UL << 8)                         /**< Buffer 1 Overflow                           */
#define _BUFC_SEQIF_BUF1OF_SHIFT                             8                                    /**< Shift value for BUFC_BUF1OF                 */
#define _BUFC_SEQIF_BUF1OF_MASK                              0x100UL                              /**< Bit mask for BUFC_BUF1OF                    */
#define _BUFC_SEQIF_BUF1OF_DEFAULT                           0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF1OF_DEFAULT                            (_BUFC_SEQIF_BUF1OF_DEFAULT << 8)    /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF1UF                                    (0x1UL << 9)                         /**< Buffer 1 Underflow                          */
#define _BUFC_SEQIF_BUF1UF_SHIFT                             9                                    /**< Shift value for BUFC_BUF1UF                 */
#define _BUFC_SEQIF_BUF1UF_MASK                              0x200UL                              /**< Bit mask for BUFC_BUF1UF                    */
#define _BUFC_SEQIF_BUF1UF_DEFAULT                           0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF1UF_DEFAULT                            (_BUFC_SEQIF_BUF1UF_DEFAULT << 9)    /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF1THR                                   (0x1UL << 10)                        /**< Buffer 1 Threshold Event                    */
#define _BUFC_SEQIF_BUF1THR_SHIFT                            10                                   /**< Shift value for BUFC_BUF1THR                */
#define _BUFC_SEQIF_BUF1THR_MASK                             0x400UL                              /**< Bit mask for BUFC_BUF1THR                   */
#define _BUFC_SEQIF_BUF1THR_DEFAULT                          0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF1THR_DEFAULT                           (_BUFC_SEQIF_BUF1THR_DEFAULT << 10)  /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF1CORR                                  (0x1UL << 11)                        /**< Buffer 1 Corrupt                            */
#define _BUFC_SEQIF_BUF1CORR_SHIFT                           11                                   /**< Shift value for BUFC_BUF1CORR               */
#define _BUFC_SEQIF_BUF1CORR_MASK                            0x800UL                              /**< Bit mask for BUFC_BUF1CORR                  */
#define _BUFC_SEQIF_BUF1CORR_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF1CORR_DEFAULT                          (_BUFC_SEQIF_BUF1CORR_DEFAULT << 11) /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF1NWA                                   (0x1UL << 12)                        /**< Buffer 1 Not Word-Aligned                   */
#define _BUFC_SEQIF_BUF1NWA_SHIFT                            12                                   /**< Shift value for BUFC_BUF1NWA                */
#define _BUFC_SEQIF_BUF1NWA_MASK                             0x1000UL                             /**< Bit mask for BUFC_BUF1NWA                   */
#define _BUFC_SEQIF_BUF1NWA_DEFAULT                          0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF1NWA_DEFAULT                           (_BUFC_SEQIF_BUF1NWA_DEFAULT << 12)  /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF2OF                                    (0x1UL << 16)                        /**< Buffer 2 Overflow                           */
#define _BUFC_SEQIF_BUF2OF_SHIFT                             16                                   /**< Shift value for BUFC_BUF2OF                 */
#define _BUFC_SEQIF_BUF2OF_MASK                              0x10000UL                            /**< Bit mask for BUFC_BUF2OF                    */
#define _BUFC_SEQIF_BUF2OF_DEFAULT                           0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF2OF_DEFAULT                            (_BUFC_SEQIF_BUF2OF_DEFAULT << 16)   /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF2UF                                    (0x1UL << 17)                        /**< Buffer 2 Underflow                          */
#define _BUFC_SEQIF_BUF2UF_SHIFT                             17                                   /**< Shift value for BUFC_BUF2UF                 */
#define _BUFC_SEQIF_BUF2UF_MASK                              0x20000UL                            /**< Bit mask for BUFC_BUF2UF                    */
#define _BUFC_SEQIF_BUF2UF_DEFAULT                           0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF2UF_DEFAULT                            (_BUFC_SEQIF_BUF2UF_DEFAULT << 17)   /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF2THR                                   (0x1UL << 18)                        /**< Buffer 2 Threshold Event                    */
#define _BUFC_SEQIF_BUF2THR_SHIFT                            18                                   /**< Shift value for BUFC_BUF2THR                */
#define _BUFC_SEQIF_BUF2THR_MASK                             0x40000UL                            /**< Bit mask for BUFC_BUF2THR                   */
#define _BUFC_SEQIF_BUF2THR_DEFAULT                          0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF2THR_DEFAULT                           (_BUFC_SEQIF_BUF2THR_DEFAULT << 18)  /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF2CORR                                  (0x1UL << 19)                        /**< Buffer 2 Corrupt                            */
#define _BUFC_SEQIF_BUF2CORR_SHIFT                           19                                   /**< Shift value for BUFC_BUF2CORR               */
#define _BUFC_SEQIF_BUF2CORR_MASK                            0x80000UL                            /**< Bit mask for BUFC_BUF2CORR                  */
#define _BUFC_SEQIF_BUF2CORR_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF2CORR_DEFAULT                          (_BUFC_SEQIF_BUF2CORR_DEFAULT << 19) /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF2NWA                                   (0x1UL << 20)                        /**< Buffer 2 Not Word-Aligned                   */
#define _BUFC_SEQIF_BUF2NWA_SHIFT                            20                                   /**< Shift value for BUFC_BUF2NWA                */
#define _BUFC_SEQIF_BUF2NWA_MASK                             0x100000UL                           /**< Bit mask for BUFC_BUF2NWA                   */
#define _BUFC_SEQIF_BUF2NWA_DEFAULT                          0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF2NWA_DEFAULT                           (_BUFC_SEQIF_BUF2NWA_DEFAULT << 20)  /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF3OF                                    (0x1UL << 24)                        /**< Buffer 3 Overflow                           */
#define _BUFC_SEQIF_BUF3OF_SHIFT                             24                                   /**< Shift value for BUFC_BUF3OF                 */
#define _BUFC_SEQIF_BUF3OF_MASK                              0x1000000UL                          /**< Bit mask for BUFC_BUF3OF                    */
#define _BUFC_SEQIF_BUF3OF_DEFAULT                           0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF3OF_DEFAULT                            (_BUFC_SEQIF_BUF3OF_DEFAULT << 24)   /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF3UF                                    (0x1UL << 25)                        /**< Buffer 3 Underflow                          */
#define _BUFC_SEQIF_BUF3UF_SHIFT                             25                                   /**< Shift value for BUFC_BUF3UF                 */
#define _BUFC_SEQIF_BUF3UF_MASK                              0x2000000UL                          /**< Bit mask for BUFC_BUF3UF                    */
#define _BUFC_SEQIF_BUF3UF_DEFAULT                           0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF3UF_DEFAULT                            (_BUFC_SEQIF_BUF3UF_DEFAULT << 25)   /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF3THR                                   (0x1UL << 26)                        /**< Buffer 3 Threshold Event                    */
#define _BUFC_SEQIF_BUF3THR_SHIFT                            26                                   /**< Shift value for BUFC_BUF3THR                */
#define _BUFC_SEQIF_BUF3THR_MASK                             0x4000000UL                          /**< Bit mask for BUFC_BUF3THR                   */
#define _BUFC_SEQIF_BUF3THR_DEFAULT                          0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF3THR_DEFAULT                           (_BUFC_SEQIF_BUF3THR_DEFAULT << 26)  /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF3CORR                                  (0x1UL << 27)                        /**< Buffer 3 Corrupt                            */
#define _BUFC_SEQIF_BUF3CORR_SHIFT                           27                                   /**< Shift value for BUFC_BUF3CORR               */
#define _BUFC_SEQIF_BUF3CORR_MASK                            0x8000000UL                          /**< Bit mask for BUFC_BUF3CORR                  */
#define _BUFC_SEQIF_BUF3CORR_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF3CORR_DEFAULT                          (_BUFC_SEQIF_BUF3CORR_DEFAULT << 27) /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUF3NWA                                   (0x1UL << 28)                        /**< Buffer 3 Not Word-Aligned                   */
#define _BUFC_SEQIF_BUF3NWA_SHIFT                            28                                   /**< Shift value for BUFC_BUF3NWA                */
#define _BUFC_SEQIF_BUF3NWA_MASK                             0x10000000UL                         /**< Bit mask for BUFC_BUF3NWA                   */
#define _BUFC_SEQIF_BUF3NWA_DEFAULT                          0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUF3NWA_DEFAULT                           (_BUFC_SEQIF_BUF3NWA_DEFAULT << 28)  /**< Shifted mode DEFAULT for BUFC_SEQIF         */
#define BUFC_SEQIF_BUSERROR                                  (0x1UL << 31)                        /**< Bus Error                                   */
#define _BUFC_SEQIF_BUSERROR_SHIFT                           31                                   /**< Shift value for BUFC_BUSERROR               */
#define _BUFC_SEQIF_BUSERROR_MASK                            0x80000000UL                         /**< Bit mask for BUFC_BUSERROR                  */
#define _BUFC_SEQIF_BUSERROR_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for BUFC_SEQIF                 */
#define BUFC_SEQIF_BUSERROR_DEFAULT                          (_BUFC_SEQIF_BUSERROR_DEFAULT << 31) /**< Shifted mode DEFAULT for BUFC_SEQIF         */

/* Bit fields for BUFC SEQIEN */
#define _BUFC_SEQIEN_RESETVALUE                              0x00000000UL                          /**< Default value for BUFC_SEQIEN               */
#define _BUFC_SEQIEN_MASK                                    0x9F1F1F1FUL                          /**< Mask for BUFC_SEQIEN                        */
#define BUFC_SEQIEN_BUF0OF                                   (0x1UL << 0)                          /**< BUF0OF Interrupt Enable                     */
#define _BUFC_SEQIEN_BUF0OF_SHIFT                            0                                     /**< Shift value for BUFC_BUF0OF                 */
#define _BUFC_SEQIEN_BUF0OF_MASK                             0x1UL                                 /**< Bit mask for BUFC_BUF0OF                    */
#define _BUFC_SEQIEN_BUF0OF_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF0OF_DEFAULT                           (_BUFC_SEQIEN_BUF0OF_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF0UF                                   (0x1UL << 1)                          /**< BUF0UF Interrupt Enable                     */
#define _BUFC_SEQIEN_BUF0UF_SHIFT                            1                                     /**< Shift value for BUFC_BUF0UF                 */
#define _BUFC_SEQIEN_BUF0UF_MASK                             0x2UL                                 /**< Bit mask for BUFC_BUF0UF                    */
#define _BUFC_SEQIEN_BUF0UF_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF0UF_DEFAULT                           (_BUFC_SEQIEN_BUF0UF_DEFAULT << 1)    /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF0THR                                  (0x1UL << 2)                          /**< BUF0THR Interrupt Enable                    */
#define _BUFC_SEQIEN_BUF0THR_SHIFT                           2                                     /**< Shift value for BUFC_BUF0THR                */
#define _BUFC_SEQIEN_BUF0THR_MASK                            0x4UL                                 /**< Bit mask for BUFC_BUF0THR                   */
#define _BUFC_SEQIEN_BUF0THR_DEFAULT                         0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF0THR_DEFAULT                          (_BUFC_SEQIEN_BUF0THR_DEFAULT << 2)   /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF0CORR                                 (0x1UL << 3)                          /**< BUF0CORR Interrupt Enable                   */
#define _BUFC_SEQIEN_BUF0CORR_SHIFT                          3                                     /**< Shift value for BUFC_BUF0CORR               */
#define _BUFC_SEQIEN_BUF0CORR_MASK                           0x8UL                                 /**< Bit mask for BUFC_BUF0CORR                  */
#define _BUFC_SEQIEN_BUF0CORR_DEFAULT                        0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF0CORR_DEFAULT                         (_BUFC_SEQIEN_BUF0CORR_DEFAULT << 3)  /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF0NWA                                  (0x1UL << 4)                          /**< BUF0NWA Interrupt Enable                    */
#define _BUFC_SEQIEN_BUF0NWA_SHIFT                           4                                     /**< Shift value for BUFC_BUF0NWA                */
#define _BUFC_SEQIEN_BUF0NWA_MASK                            0x10UL                                /**< Bit mask for BUFC_BUF0NWA                   */
#define _BUFC_SEQIEN_BUF0NWA_DEFAULT                         0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF0NWA_DEFAULT                          (_BUFC_SEQIEN_BUF0NWA_DEFAULT << 4)   /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF1OF                                   (0x1UL << 8)                          /**< BUF1OF Interrupt Enable                     */
#define _BUFC_SEQIEN_BUF1OF_SHIFT                            8                                     /**< Shift value for BUFC_BUF1OF                 */
#define _BUFC_SEQIEN_BUF1OF_MASK                             0x100UL                               /**< Bit mask for BUFC_BUF1OF                    */
#define _BUFC_SEQIEN_BUF1OF_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF1OF_DEFAULT                           (_BUFC_SEQIEN_BUF1OF_DEFAULT << 8)    /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF1UF                                   (0x1UL << 9)                          /**< BUF1UF Interrupt Enable                     */
#define _BUFC_SEQIEN_BUF1UF_SHIFT                            9                                     /**< Shift value for BUFC_BUF1UF                 */
#define _BUFC_SEQIEN_BUF1UF_MASK                             0x200UL                               /**< Bit mask for BUFC_BUF1UF                    */
#define _BUFC_SEQIEN_BUF1UF_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF1UF_DEFAULT                           (_BUFC_SEQIEN_BUF1UF_DEFAULT << 9)    /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF1THR                                  (0x1UL << 10)                         /**< BUF1THR Interrupt Enable                    */
#define _BUFC_SEQIEN_BUF1THR_SHIFT                           10                                    /**< Shift value for BUFC_BUF1THR                */
#define _BUFC_SEQIEN_BUF1THR_MASK                            0x400UL                               /**< Bit mask for BUFC_BUF1THR                   */
#define _BUFC_SEQIEN_BUF1THR_DEFAULT                         0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF1THR_DEFAULT                          (_BUFC_SEQIEN_BUF1THR_DEFAULT << 10)  /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF1CORR                                 (0x1UL << 11)                         /**< BUF1CORR Interrupt Enable                   */
#define _BUFC_SEQIEN_BUF1CORR_SHIFT                          11                                    /**< Shift value for BUFC_BUF1CORR               */
#define _BUFC_SEQIEN_BUF1CORR_MASK                           0x800UL                               /**< Bit mask for BUFC_BUF1CORR                  */
#define _BUFC_SEQIEN_BUF1CORR_DEFAULT                        0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF1CORR_DEFAULT                         (_BUFC_SEQIEN_BUF1CORR_DEFAULT << 11) /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF1NWA                                  (0x1UL << 12)                         /**< BUF1NWA Interrupt Enable                    */
#define _BUFC_SEQIEN_BUF1NWA_SHIFT                           12                                    /**< Shift value for BUFC_BUF1NWA                */
#define _BUFC_SEQIEN_BUF1NWA_MASK                            0x1000UL                              /**< Bit mask for BUFC_BUF1NWA                   */
#define _BUFC_SEQIEN_BUF1NWA_DEFAULT                         0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF1NWA_DEFAULT                          (_BUFC_SEQIEN_BUF1NWA_DEFAULT << 12)  /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF2OF                                   (0x1UL << 16)                         /**< BUF2OF Interrupt Enable                     */
#define _BUFC_SEQIEN_BUF2OF_SHIFT                            16                                    /**< Shift value for BUFC_BUF2OF                 */
#define _BUFC_SEQIEN_BUF2OF_MASK                             0x10000UL                             /**< Bit mask for BUFC_BUF2OF                    */
#define _BUFC_SEQIEN_BUF2OF_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF2OF_DEFAULT                           (_BUFC_SEQIEN_BUF2OF_DEFAULT << 16)   /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF2UF                                   (0x1UL << 17)                         /**< BUF2UF Interrupt Enable                     */
#define _BUFC_SEQIEN_BUF2UF_SHIFT                            17                                    /**< Shift value for BUFC_BUF2UF                 */
#define _BUFC_SEQIEN_BUF2UF_MASK                             0x20000UL                             /**< Bit mask for BUFC_BUF2UF                    */
#define _BUFC_SEQIEN_BUF2UF_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF2UF_DEFAULT                           (_BUFC_SEQIEN_BUF2UF_DEFAULT << 17)   /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF2THR                                  (0x1UL << 18)                         /**< BUF2THR Interrupt Enable                    */
#define _BUFC_SEQIEN_BUF2THR_SHIFT                           18                                    /**< Shift value for BUFC_BUF2THR                */
#define _BUFC_SEQIEN_BUF2THR_MASK                            0x40000UL                             /**< Bit mask for BUFC_BUF2THR                   */
#define _BUFC_SEQIEN_BUF2THR_DEFAULT                         0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF2THR_DEFAULT                          (_BUFC_SEQIEN_BUF2THR_DEFAULT << 18)  /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF2CORR                                 (0x1UL << 19)                         /**< BUF2CORR Interrupt Enable                   */
#define _BUFC_SEQIEN_BUF2CORR_SHIFT                          19                                    /**< Shift value for BUFC_BUF2CORR               */
#define _BUFC_SEQIEN_BUF2CORR_MASK                           0x80000UL                             /**< Bit mask for BUFC_BUF2CORR                  */
#define _BUFC_SEQIEN_BUF2CORR_DEFAULT                        0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF2CORR_DEFAULT                         (_BUFC_SEQIEN_BUF2CORR_DEFAULT << 19) /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF2NWA                                  (0x1UL << 20)                         /**< BUF2NWA Interrupt Enable                    */
#define _BUFC_SEQIEN_BUF2NWA_SHIFT                           20                                    /**< Shift value for BUFC_BUF2NWA                */
#define _BUFC_SEQIEN_BUF2NWA_MASK                            0x100000UL                            /**< Bit mask for BUFC_BUF2NWA                   */
#define _BUFC_SEQIEN_BUF2NWA_DEFAULT                         0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF2NWA_DEFAULT                          (_BUFC_SEQIEN_BUF2NWA_DEFAULT << 20)  /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF3OF                                   (0x1UL << 24)                         /**< BUF3OF Interrupt Enable                     */
#define _BUFC_SEQIEN_BUF3OF_SHIFT                            24                                    /**< Shift value for BUFC_BUF3OF                 */
#define _BUFC_SEQIEN_BUF3OF_MASK                             0x1000000UL                           /**< Bit mask for BUFC_BUF3OF                    */
#define _BUFC_SEQIEN_BUF3OF_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF3OF_DEFAULT                           (_BUFC_SEQIEN_BUF3OF_DEFAULT << 24)   /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF3UF                                   (0x1UL << 25)                         /**< BUF3UF Interrupt Enable                     */
#define _BUFC_SEQIEN_BUF3UF_SHIFT                            25                                    /**< Shift value for BUFC_BUF3UF                 */
#define _BUFC_SEQIEN_BUF3UF_MASK                             0x2000000UL                           /**< Bit mask for BUFC_BUF3UF                    */
#define _BUFC_SEQIEN_BUF3UF_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF3UF_DEFAULT                           (_BUFC_SEQIEN_BUF3UF_DEFAULT << 25)   /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF3THR                                  (0x1UL << 26)                         /**< BUF3THR Interrupt Enable                    */
#define _BUFC_SEQIEN_BUF3THR_SHIFT                           26                                    /**< Shift value for BUFC_BUF3THR                */
#define _BUFC_SEQIEN_BUF3THR_MASK                            0x4000000UL                           /**< Bit mask for BUFC_BUF3THR                   */
#define _BUFC_SEQIEN_BUF3THR_DEFAULT                         0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF3THR_DEFAULT                          (_BUFC_SEQIEN_BUF3THR_DEFAULT << 26)  /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF3CORR                                 (0x1UL << 27)                         /**< BUF3CORR Interrupt Enable                   */
#define _BUFC_SEQIEN_BUF3CORR_SHIFT                          27                                    /**< Shift value for BUFC_BUF3CORR               */
#define _BUFC_SEQIEN_BUF3CORR_MASK                           0x8000000UL                           /**< Bit mask for BUFC_BUF3CORR                  */
#define _BUFC_SEQIEN_BUF3CORR_DEFAULT                        0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF3CORR_DEFAULT                         (_BUFC_SEQIEN_BUF3CORR_DEFAULT << 27) /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUF3NWA                                  (0x1UL << 28)                         /**< BUF3NWA Interrupt Enable                    */
#define _BUFC_SEQIEN_BUF3NWA_SHIFT                           28                                    /**< Shift value for BUFC_BUF3NWA                */
#define _BUFC_SEQIEN_BUF3NWA_MASK                            0x10000000UL                          /**< Bit mask for BUFC_BUF3NWA                   */
#define _BUFC_SEQIEN_BUF3NWA_DEFAULT                         0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUF3NWA_DEFAULT                          (_BUFC_SEQIEN_BUF3NWA_DEFAULT << 28)  /**< Shifted mode DEFAULT for BUFC_SEQIEN        */
#define BUFC_SEQIEN_BUSERROR                                 (0x1UL << 31)                         /**< BUSERROR Interrupt Enable                   */
#define _BUFC_SEQIEN_BUSERROR_SHIFT                          31                                    /**< Shift value for BUFC_BUSERROR               */
#define _BUFC_SEQIEN_BUSERROR_MASK                           0x80000000UL                          /**< Bit mask for BUFC_BUSERROR                  */
#define _BUFC_SEQIEN_BUSERROR_DEFAULT                        0x00000000UL                          /**< Mode DEFAULT for BUFC_SEQIEN                */
#define BUFC_SEQIEN_BUSERROR_DEFAULT                         (_BUFC_SEQIEN_BUSERROR_DEFAULT << 31) /**< Shifted mode DEFAULT for BUFC_SEQIEN        */

/* Bit fields for BUFC SFMIF */
#define _BUFC_SFMIF_RESETVALUE                               0x00000000UL                              /**< Default value for BUFC_SFMIF                */
#define _BUFC_SFMIF_MASK                                     0x9F1F1F1FUL                              /**< Mask for BUFC_SFMIF                         */
#define BUFC_SFMIF_BUF0OFSFMIF                               (0x1UL << 0)                              /**< Buffer 0 Overflow                           */
#define _BUFC_SFMIF_BUF0OFSFMIF_SHIFT                        0                                         /**< Shift value for BUFC_BUF0OFSFMIF            */
#define _BUFC_SFMIF_BUF0OFSFMIF_MASK                         0x1UL                                     /**< Bit mask for BUFC_BUF0OFSFMIF               */
#define _BUFC_SFMIF_BUF0OFSFMIF_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF0OFSFMIF_DEFAULT                       (_BUFC_SFMIF_BUF0OFSFMIF_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF0UFSFMIF                               (0x1UL << 1)                              /**< Buffer 0 Underflow                          */
#define _BUFC_SFMIF_BUF0UFSFMIF_SHIFT                        1                                         /**< Shift value for BUFC_BUF0UFSFMIF            */
#define _BUFC_SFMIF_BUF0UFSFMIF_MASK                         0x2UL                                     /**< Bit mask for BUFC_BUF0UFSFMIF               */
#define _BUFC_SFMIF_BUF0UFSFMIF_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF0UFSFMIF_DEFAULT                       (_BUFC_SFMIF_BUF0UFSFMIF_DEFAULT << 1)    /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF0THRSFMIF                              (0x1UL << 2)                              /**< Buffer 0 Threshold Event                    */
#define _BUFC_SFMIF_BUF0THRSFMIF_SHIFT                       2                                         /**< Shift value for BUFC_BUF0THRSFMIF           */
#define _BUFC_SFMIF_BUF0THRSFMIF_MASK                        0x4UL                                     /**< Bit mask for BUFC_BUF0THRSFMIF              */
#define _BUFC_SFMIF_BUF0THRSFMIF_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF0THRSFMIF_DEFAULT                      (_BUFC_SFMIF_BUF0THRSFMIF_DEFAULT << 2)   /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF0CORRSFMIF                             (0x1UL << 3)                              /**< Buffer 0 Corrupt                            */
#define _BUFC_SFMIF_BUF0CORRSFMIF_SHIFT                      3                                         /**< Shift value for BUFC_BUF0CORRSFMIF          */
#define _BUFC_SFMIF_BUF0CORRSFMIF_MASK                       0x8UL                                     /**< Bit mask for BUFC_BUF0CORRSFMIF             */
#define _BUFC_SFMIF_BUF0CORRSFMIF_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF0CORRSFMIF_DEFAULT                     (_BUFC_SFMIF_BUF0CORRSFMIF_DEFAULT << 3)  /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF0NWASFMIF                              (0x1UL << 4)                              /**< Buffer 0 Not Word-Aligned                   */
#define _BUFC_SFMIF_BUF0NWASFMIF_SHIFT                       4                                         /**< Shift value for BUFC_BUF0NWASFMIF           */
#define _BUFC_SFMIF_BUF0NWASFMIF_MASK                        0x10UL                                    /**< Bit mask for BUFC_BUF0NWASFMIF              */
#define _BUFC_SFMIF_BUF0NWASFMIF_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF0NWASFMIF_DEFAULT                      (_BUFC_SFMIF_BUF0NWASFMIF_DEFAULT << 4)   /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF1OFSFMIF                               (0x1UL << 8)                              /**< Buffer 1 Overflow                           */
#define _BUFC_SFMIF_BUF1OFSFMIF_SHIFT                        8                                         /**< Shift value for BUFC_BUF1OFSFMIF            */
#define _BUFC_SFMIF_BUF1OFSFMIF_MASK                         0x100UL                                   /**< Bit mask for BUFC_BUF1OFSFMIF               */
#define _BUFC_SFMIF_BUF1OFSFMIF_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF1OFSFMIF_DEFAULT                       (_BUFC_SFMIF_BUF1OFSFMIF_DEFAULT << 8)    /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF1UFSFMIF                               (0x1UL << 9)                              /**< Buffer 1 Underflow                          */
#define _BUFC_SFMIF_BUF1UFSFMIF_SHIFT                        9                                         /**< Shift value for BUFC_BUF1UFSFMIF            */
#define _BUFC_SFMIF_BUF1UFSFMIF_MASK                         0x200UL                                   /**< Bit mask for BUFC_BUF1UFSFMIF               */
#define _BUFC_SFMIF_BUF1UFSFMIF_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF1UFSFMIF_DEFAULT                       (_BUFC_SFMIF_BUF1UFSFMIF_DEFAULT << 9)    /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF1THRSFMIF                              (0x1UL << 10)                             /**< Buffer 1 Threshold Event                    */
#define _BUFC_SFMIF_BUF1THRSFMIF_SHIFT                       10                                        /**< Shift value for BUFC_BUF1THRSFMIF           */
#define _BUFC_SFMIF_BUF1THRSFMIF_MASK                        0x400UL                                   /**< Bit mask for BUFC_BUF1THRSFMIF              */
#define _BUFC_SFMIF_BUF1THRSFMIF_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF1THRSFMIF_DEFAULT                      (_BUFC_SFMIF_BUF1THRSFMIF_DEFAULT << 10)  /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF1CORRSFMIF                             (0x1UL << 11)                             /**< Buffer 1 Corrupt                            */
#define _BUFC_SFMIF_BUF1CORRSFMIF_SHIFT                      11                                        /**< Shift value for BUFC_BUF1CORRSFMIF          */
#define _BUFC_SFMIF_BUF1CORRSFMIF_MASK                       0x800UL                                   /**< Bit mask for BUFC_BUF1CORRSFMIF             */
#define _BUFC_SFMIF_BUF1CORRSFMIF_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF1CORRSFMIF_DEFAULT                     (_BUFC_SFMIF_BUF1CORRSFMIF_DEFAULT << 11) /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF1NWASFMIF                              (0x1UL << 12)                             /**< Buffer 1 Not Word-Aligned                   */
#define _BUFC_SFMIF_BUF1NWASFMIF_SHIFT                       12                                        /**< Shift value for BUFC_BUF1NWASFMIF           */
#define _BUFC_SFMIF_BUF1NWASFMIF_MASK                        0x1000UL                                  /**< Bit mask for BUFC_BUF1NWASFMIF              */
#define _BUFC_SFMIF_BUF1NWASFMIF_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF1NWASFMIF_DEFAULT                      (_BUFC_SFMIF_BUF1NWASFMIF_DEFAULT << 12)  /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF2OFSFMIF                               (0x1UL << 16)                             /**< Buffer 2 Overflow                           */
#define _BUFC_SFMIF_BUF2OFSFMIF_SHIFT                        16                                        /**< Shift value for BUFC_BUF2OFSFMIF            */
#define _BUFC_SFMIF_BUF2OFSFMIF_MASK                         0x10000UL                                 /**< Bit mask for BUFC_BUF2OFSFMIF               */
#define _BUFC_SFMIF_BUF2OFSFMIF_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF2OFSFMIF_DEFAULT                       (_BUFC_SFMIF_BUF2OFSFMIF_DEFAULT << 16)   /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF2UFSFMIF                               (0x1UL << 17)                             /**< Buffer 2 Underflow                          */
#define _BUFC_SFMIF_BUF2UFSFMIF_SHIFT                        17                                        /**< Shift value for BUFC_BUF2UFSFMIF            */
#define _BUFC_SFMIF_BUF2UFSFMIF_MASK                         0x20000UL                                 /**< Bit mask for BUFC_BUF2UFSFMIF               */
#define _BUFC_SFMIF_BUF2UFSFMIF_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF2UFSFMIF_DEFAULT                       (_BUFC_SFMIF_BUF2UFSFMIF_DEFAULT << 17)   /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF2THRSFMIF                              (0x1UL << 18)                             /**< Buffer 2 Threshold Event                    */
#define _BUFC_SFMIF_BUF2THRSFMIF_SHIFT                       18                                        /**< Shift value for BUFC_BUF2THRSFMIF           */
#define _BUFC_SFMIF_BUF2THRSFMIF_MASK                        0x40000UL                                 /**< Bit mask for BUFC_BUF2THRSFMIF              */
#define _BUFC_SFMIF_BUF2THRSFMIF_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF2THRSFMIF_DEFAULT                      (_BUFC_SFMIF_BUF2THRSFMIF_DEFAULT << 18)  /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF2CORRSFMIF                             (0x1UL << 19)                             /**< Buffer 2 Corrupt                            */
#define _BUFC_SFMIF_BUF2CORRSFMIF_SHIFT                      19                                        /**< Shift value for BUFC_BUF2CORRSFMIF          */
#define _BUFC_SFMIF_BUF2CORRSFMIF_MASK                       0x80000UL                                 /**< Bit mask for BUFC_BUF2CORRSFMIF             */
#define _BUFC_SFMIF_BUF2CORRSFMIF_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF2CORRSFMIF_DEFAULT                     (_BUFC_SFMIF_BUF2CORRSFMIF_DEFAULT << 19) /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF2NWASFMIF                              (0x1UL << 20)                             /**< Buffer 2 Not Word-Aligned                   */
#define _BUFC_SFMIF_BUF2NWASFMIF_SHIFT                       20                                        /**< Shift value for BUFC_BUF2NWASFMIF           */
#define _BUFC_SFMIF_BUF2NWASFMIF_MASK                        0x100000UL                                /**< Bit mask for BUFC_BUF2NWASFMIF              */
#define _BUFC_SFMIF_BUF2NWASFMIF_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF2NWASFMIF_DEFAULT                      (_BUFC_SFMIF_BUF2NWASFMIF_DEFAULT << 20)  /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF3OFSFMIF                               (0x1UL << 24)                             /**< Buffer 3 Overflow                           */
#define _BUFC_SFMIF_BUF3OFSFMIF_SHIFT                        24                                        /**< Shift value for BUFC_BUF3OFSFMIF            */
#define _BUFC_SFMIF_BUF3OFSFMIF_MASK                         0x1000000UL                               /**< Bit mask for BUFC_BUF3OFSFMIF               */
#define _BUFC_SFMIF_BUF3OFSFMIF_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF3OFSFMIF_DEFAULT                       (_BUFC_SFMIF_BUF3OFSFMIF_DEFAULT << 24)   /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF3UFSFMIF                               (0x1UL << 25)                             /**< Buffer 3 Underflow                          */
#define _BUFC_SFMIF_BUF3UFSFMIF_SHIFT                        25                                        /**< Shift value for BUFC_BUF3UFSFMIF            */
#define _BUFC_SFMIF_BUF3UFSFMIF_MASK                         0x2000000UL                               /**< Bit mask for BUFC_BUF3UFSFMIF               */
#define _BUFC_SFMIF_BUF3UFSFMIF_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF3UFSFMIF_DEFAULT                       (_BUFC_SFMIF_BUF3UFSFMIF_DEFAULT << 25)   /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF3THRSFMIF                              (0x1UL << 26)                             /**< Buffer 3 Threshold Event                    */
#define _BUFC_SFMIF_BUF3THRSFMIF_SHIFT                       26                                        /**< Shift value for BUFC_BUF3THRSFMIF           */
#define _BUFC_SFMIF_BUF3THRSFMIF_MASK                        0x4000000UL                               /**< Bit mask for BUFC_BUF3THRSFMIF              */
#define _BUFC_SFMIF_BUF3THRSFMIF_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF3THRSFMIF_DEFAULT                      (_BUFC_SFMIF_BUF3THRSFMIF_DEFAULT << 26)  /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF3CORRSFMIF                             (0x1UL << 27)                             /**< Buffer 3 Corrupt                            */
#define _BUFC_SFMIF_BUF3CORRSFMIF_SHIFT                      27                                        /**< Shift value for BUFC_BUF3CORRSFMIF          */
#define _BUFC_SFMIF_BUF3CORRSFMIF_MASK                       0x8000000UL                               /**< Bit mask for BUFC_BUF3CORRSFMIF             */
#define _BUFC_SFMIF_BUF3CORRSFMIF_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF3CORRSFMIF_DEFAULT                     (_BUFC_SFMIF_BUF3CORRSFMIF_DEFAULT << 27) /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUF3NWASFMIF                              (0x1UL << 28)                             /**< Buffer 3 Not Word-Aligned                   */
#define _BUFC_SFMIF_BUF3NWASFMIF_SHIFT                       28                                        /**< Shift value for BUFC_BUF3NWASFMIF           */
#define _BUFC_SFMIF_BUF3NWASFMIF_MASK                        0x10000000UL                              /**< Bit mask for BUFC_BUF3NWASFMIF              */
#define _BUFC_SFMIF_BUF3NWASFMIF_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUF3NWASFMIF_DEFAULT                      (_BUFC_SFMIF_BUF3NWASFMIF_DEFAULT << 28)  /**< Shifted mode DEFAULT for BUFC_SFMIF         */
#define BUFC_SFMIF_BUSERRORSFMIF                             (0x1UL << 31)                             /**< Bus Error                                   */
#define _BUFC_SFMIF_BUSERRORSFMIF_SHIFT                      31                                        /**< Shift value for BUFC_BUSERRORSFMIF          */
#define _BUFC_SFMIF_BUSERRORSFMIF_MASK                       0x80000000UL                              /**< Bit mask for BUFC_BUSERRORSFMIF             */
#define _BUFC_SFMIF_BUSERRORSFMIF_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for BUFC_SFMIF                 */
#define BUFC_SFMIF_BUSERRORSFMIF_DEFAULT                     (_BUFC_SFMIF_BUSERRORSFMIF_DEFAULT << 31) /**< Shifted mode DEFAULT for BUFC_SFMIF         */

/* Bit fields for BUFC SFMIEN */
#define _BUFC_SFMIEN_RESETVALUE                              0x00000000UL                                /**< Default value for BUFC_SFMIEN               */
#define _BUFC_SFMIEN_MASK                                    0x9F1F1F1FUL                                /**< Mask for BUFC_SFMIEN                        */
#define BUFC_SFMIEN_BUF0OFSFMIEN                             (0x1UL << 0)                                /**< BUF0OF Interrupt Enable                     */
#define _BUFC_SFMIEN_BUF0OFSFMIEN_SHIFT                      0                                           /**< Shift value for BUFC_BUF0OFSFMIEN           */
#define _BUFC_SFMIEN_BUF0OFSFMIEN_MASK                       0x1UL                                       /**< Bit mask for BUFC_BUF0OFSFMIEN              */
#define _BUFC_SFMIEN_BUF0OFSFMIEN_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF0OFSFMIEN_DEFAULT                     (_BUFC_SFMIEN_BUF0OFSFMIEN_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF0UFSFMIEN                             (0x1UL << 1)                                /**< BUF0UF Interrupt Enable                     */
#define _BUFC_SFMIEN_BUF0UFSFMIEN_SHIFT                      1                                           /**< Shift value for BUFC_BUF0UFSFMIEN           */
#define _BUFC_SFMIEN_BUF0UFSFMIEN_MASK                       0x2UL                                       /**< Bit mask for BUFC_BUF0UFSFMIEN              */
#define _BUFC_SFMIEN_BUF0UFSFMIEN_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF0UFSFMIEN_DEFAULT                     (_BUFC_SFMIEN_BUF0UFSFMIEN_DEFAULT << 1)    /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF0THRSFMIEN                            (0x1UL << 2)                                /**< BUF0THR Interrupt Enable                    */
#define _BUFC_SFMIEN_BUF0THRSFMIEN_SHIFT                     2                                           /**< Shift value for BUFC_BUF0THRSFMIEN          */
#define _BUFC_SFMIEN_BUF0THRSFMIEN_MASK                      0x4UL                                       /**< Bit mask for BUFC_BUF0THRSFMIEN             */
#define _BUFC_SFMIEN_BUF0THRSFMIEN_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF0THRSFMIEN_DEFAULT                    (_BUFC_SFMIEN_BUF0THRSFMIEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF0CORRSFMIEN                           (0x1UL << 3)                                /**< BUF0CORR Interrupt Enable                   */
#define _BUFC_SFMIEN_BUF0CORRSFMIEN_SHIFT                    3                                           /**< Shift value for BUFC_BUF0CORRSFMIEN         */
#define _BUFC_SFMIEN_BUF0CORRSFMIEN_MASK                     0x8UL                                       /**< Bit mask for BUFC_BUF0CORRSFMIEN            */
#define _BUFC_SFMIEN_BUF0CORRSFMIEN_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF0CORRSFMIEN_DEFAULT                   (_BUFC_SFMIEN_BUF0CORRSFMIEN_DEFAULT << 3)  /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF0NWASFMIEN                            (0x1UL << 4)                                /**< BUF0NWA Interrupt Enable                    */
#define _BUFC_SFMIEN_BUF0NWASFMIEN_SHIFT                     4                                           /**< Shift value for BUFC_BUF0NWASFMIEN          */
#define _BUFC_SFMIEN_BUF0NWASFMIEN_MASK                      0x10UL                                      /**< Bit mask for BUFC_BUF0NWASFMIEN             */
#define _BUFC_SFMIEN_BUF0NWASFMIEN_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF0NWASFMIEN_DEFAULT                    (_BUFC_SFMIEN_BUF0NWASFMIEN_DEFAULT << 4)   /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF1OFSFMIEN                             (0x1UL << 8)                                /**< BUF1OF Interrupt Enable                     */
#define _BUFC_SFMIEN_BUF1OFSFMIEN_SHIFT                      8                                           /**< Shift value for BUFC_BUF1OFSFMIEN           */
#define _BUFC_SFMIEN_BUF1OFSFMIEN_MASK                       0x100UL                                     /**< Bit mask for BUFC_BUF1OFSFMIEN              */
#define _BUFC_SFMIEN_BUF1OFSFMIEN_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF1OFSFMIEN_DEFAULT                     (_BUFC_SFMIEN_BUF1OFSFMIEN_DEFAULT << 8)    /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF1UFSFMIEN                             (0x1UL << 9)                                /**< BUF1UF Interrupt Enable                     */
#define _BUFC_SFMIEN_BUF1UFSFMIEN_SHIFT                      9                                           /**< Shift value for BUFC_BUF1UFSFMIEN           */
#define _BUFC_SFMIEN_BUF1UFSFMIEN_MASK                       0x200UL                                     /**< Bit mask for BUFC_BUF1UFSFMIEN              */
#define _BUFC_SFMIEN_BUF1UFSFMIEN_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF1UFSFMIEN_DEFAULT                     (_BUFC_SFMIEN_BUF1UFSFMIEN_DEFAULT << 9)    /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF1THRSFMIEN                            (0x1UL << 10)                               /**< BUF1THR Interrupt Enable                    */
#define _BUFC_SFMIEN_BUF1THRSFMIEN_SHIFT                     10                                          /**< Shift value for BUFC_BUF1THRSFMIEN          */
#define _BUFC_SFMIEN_BUF1THRSFMIEN_MASK                      0x400UL                                     /**< Bit mask for BUFC_BUF1THRSFMIEN             */
#define _BUFC_SFMIEN_BUF1THRSFMIEN_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF1THRSFMIEN_DEFAULT                    (_BUFC_SFMIEN_BUF1THRSFMIEN_DEFAULT << 10)  /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF1CORRSFMIEN                           (0x1UL << 11)                               /**< BUF1CORR Interrupt Enable                   */
#define _BUFC_SFMIEN_BUF1CORRSFMIEN_SHIFT                    11                                          /**< Shift value for BUFC_BUF1CORRSFMIEN         */
#define _BUFC_SFMIEN_BUF1CORRSFMIEN_MASK                     0x800UL                                     /**< Bit mask for BUFC_BUF1CORRSFMIEN            */
#define _BUFC_SFMIEN_BUF1CORRSFMIEN_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF1CORRSFMIEN_DEFAULT                   (_BUFC_SFMIEN_BUF1CORRSFMIEN_DEFAULT << 11) /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF1NWASFMIEN                            (0x1UL << 12)                               /**< BUF1NWA Interrupt Enable                    */
#define _BUFC_SFMIEN_BUF1NWASFMIEN_SHIFT                     12                                          /**< Shift value for BUFC_BUF1NWASFMIEN          */
#define _BUFC_SFMIEN_BUF1NWASFMIEN_MASK                      0x1000UL                                    /**< Bit mask for BUFC_BUF1NWASFMIEN             */
#define _BUFC_SFMIEN_BUF1NWASFMIEN_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF1NWASFMIEN_DEFAULT                    (_BUFC_SFMIEN_BUF1NWASFMIEN_DEFAULT << 12)  /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF2OFSFMIEN                             (0x1UL << 16)                               /**< BUF2OF Interrupt Enable                     */
#define _BUFC_SFMIEN_BUF2OFSFMIEN_SHIFT                      16                                          /**< Shift value for BUFC_BUF2OFSFMIEN           */
#define _BUFC_SFMIEN_BUF2OFSFMIEN_MASK                       0x10000UL                                   /**< Bit mask for BUFC_BUF2OFSFMIEN              */
#define _BUFC_SFMIEN_BUF2OFSFMIEN_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF2OFSFMIEN_DEFAULT                     (_BUFC_SFMIEN_BUF2OFSFMIEN_DEFAULT << 16)   /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF2UFSFMIEN                             (0x1UL << 17)                               /**< BUF2UF Interrupt Enable                     */
#define _BUFC_SFMIEN_BUF2UFSFMIEN_SHIFT                      17                                          /**< Shift value for BUFC_BUF2UFSFMIEN           */
#define _BUFC_SFMIEN_BUF2UFSFMIEN_MASK                       0x20000UL                                   /**< Bit mask for BUFC_BUF2UFSFMIEN              */
#define _BUFC_SFMIEN_BUF2UFSFMIEN_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF2UFSFMIEN_DEFAULT                     (_BUFC_SFMIEN_BUF2UFSFMIEN_DEFAULT << 17)   /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF2THRSFMIEN                            (0x1UL << 18)                               /**< BUF2THR Interrupt Enable                    */
#define _BUFC_SFMIEN_BUF2THRSFMIEN_SHIFT                     18                                          /**< Shift value for BUFC_BUF2THRSFMIEN          */
#define _BUFC_SFMIEN_BUF2THRSFMIEN_MASK                      0x40000UL                                   /**< Bit mask for BUFC_BUF2THRSFMIEN             */
#define _BUFC_SFMIEN_BUF2THRSFMIEN_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF2THRSFMIEN_DEFAULT                    (_BUFC_SFMIEN_BUF2THRSFMIEN_DEFAULT << 18)  /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF2CORRSFMIEN                           (0x1UL << 19)                               /**< BUF2CORR Interrupt Enable                   */
#define _BUFC_SFMIEN_BUF2CORRSFMIEN_SHIFT                    19                                          /**< Shift value for BUFC_BUF2CORRSFMIEN         */
#define _BUFC_SFMIEN_BUF2CORRSFMIEN_MASK                     0x80000UL                                   /**< Bit mask for BUFC_BUF2CORRSFMIEN            */
#define _BUFC_SFMIEN_BUF2CORRSFMIEN_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF2CORRSFMIEN_DEFAULT                   (_BUFC_SFMIEN_BUF2CORRSFMIEN_DEFAULT << 19) /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF2NWASFMIEN                            (0x1UL << 20)                               /**< BUF2NWA Interrupt Enable                    */
#define _BUFC_SFMIEN_BUF2NWASFMIEN_SHIFT                     20                                          /**< Shift value for BUFC_BUF2NWASFMIEN          */
#define _BUFC_SFMIEN_BUF2NWASFMIEN_MASK                      0x100000UL                                  /**< Bit mask for BUFC_BUF2NWASFMIEN             */
#define _BUFC_SFMIEN_BUF2NWASFMIEN_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF2NWASFMIEN_DEFAULT                    (_BUFC_SFMIEN_BUF2NWASFMIEN_DEFAULT << 20)  /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF3OFSFMIEN                             (0x1UL << 24)                               /**< BUF3OF Interrupt Enable                     */
#define _BUFC_SFMIEN_BUF3OFSFMIEN_SHIFT                      24                                          /**< Shift value for BUFC_BUF3OFSFMIEN           */
#define _BUFC_SFMIEN_BUF3OFSFMIEN_MASK                       0x1000000UL                                 /**< Bit mask for BUFC_BUF3OFSFMIEN              */
#define _BUFC_SFMIEN_BUF3OFSFMIEN_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF3OFSFMIEN_DEFAULT                     (_BUFC_SFMIEN_BUF3OFSFMIEN_DEFAULT << 24)   /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF3UFSFMIEN                             (0x1UL << 25)                               /**< BUF3UF Interrupt Enable                     */
#define _BUFC_SFMIEN_BUF3UFSFMIEN_SHIFT                      25                                          /**< Shift value for BUFC_BUF3UFSFMIEN           */
#define _BUFC_SFMIEN_BUF3UFSFMIEN_MASK                       0x2000000UL                                 /**< Bit mask for BUFC_BUF3UFSFMIEN              */
#define _BUFC_SFMIEN_BUF3UFSFMIEN_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF3UFSFMIEN_DEFAULT                     (_BUFC_SFMIEN_BUF3UFSFMIEN_DEFAULT << 25)   /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF3THRSFMIEN                            (0x1UL << 26)                               /**< BUF3THR Interrupt Enable                    */
#define _BUFC_SFMIEN_BUF3THRSFMIEN_SHIFT                     26                                          /**< Shift value for BUFC_BUF3THRSFMIEN          */
#define _BUFC_SFMIEN_BUF3THRSFMIEN_MASK                      0x4000000UL                                 /**< Bit mask for BUFC_BUF3THRSFMIEN             */
#define _BUFC_SFMIEN_BUF3THRSFMIEN_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF3THRSFMIEN_DEFAULT                    (_BUFC_SFMIEN_BUF3THRSFMIEN_DEFAULT << 26)  /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF3CORRSFMIEN                           (0x1UL << 27)                               /**< BUF3CORR Interrupt Enable                   */
#define _BUFC_SFMIEN_BUF3CORRSFMIEN_SHIFT                    27                                          /**< Shift value for BUFC_BUF3CORRSFMIEN         */
#define _BUFC_SFMIEN_BUF3CORRSFMIEN_MASK                     0x8000000UL                                 /**< Bit mask for BUFC_BUF3CORRSFMIEN            */
#define _BUFC_SFMIEN_BUF3CORRSFMIEN_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF3CORRSFMIEN_DEFAULT                   (_BUFC_SFMIEN_BUF3CORRSFMIEN_DEFAULT << 27) /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUF3NWASFMIEN                            (0x1UL << 28)                               /**< BUF3NWA Interrupt Enable                    */
#define _BUFC_SFMIEN_BUF3NWASFMIEN_SHIFT                     28                                          /**< Shift value for BUFC_BUF3NWASFMIEN          */
#define _BUFC_SFMIEN_BUF3NWASFMIEN_MASK                      0x10000000UL                                /**< Bit mask for BUFC_BUF3NWASFMIEN             */
#define _BUFC_SFMIEN_BUF3NWASFMIEN_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUF3NWASFMIEN_DEFAULT                    (_BUFC_SFMIEN_BUF3NWASFMIEN_DEFAULT << 28)  /**< Shifted mode DEFAULT for BUFC_SFMIEN        */
#define BUFC_SFMIEN_BUSERRORSFMIEN                           (0x1UL << 31)                               /**< BUSERROR Interrupt Enable                   */
#define _BUFC_SFMIEN_BUSERRORSFMIEN_SHIFT                    31                                          /**< Shift value for BUFC_BUSERRORSFMIEN         */
#define _BUFC_SFMIEN_BUSERRORSFMIEN_MASK                     0x80000000UL                                /**< Bit mask for BUFC_BUSERRORSFMIEN            */
#define _BUFC_SFMIEN_BUSERRORSFMIEN_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for BUFC_SFMIEN                */
#define BUFC_SFMIEN_BUSERRORSFMIEN_DEFAULT                   (_BUFC_SFMIEN_BUSERRORSFMIEN_DEFAULT << 31) /**< Shifted mode DEFAULT for BUFC_SFMIEN        */

/** @} End of group EFR32FG25_BUFC_BitFields */
/** @} End of group EFR32FG25_BUFC */
/** @} End of group Parts */

#endif /* EFR32FG25_BUFC_H */
