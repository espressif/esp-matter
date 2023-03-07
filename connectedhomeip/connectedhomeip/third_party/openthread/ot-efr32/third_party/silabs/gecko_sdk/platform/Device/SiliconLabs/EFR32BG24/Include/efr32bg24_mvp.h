/**************************************************************************//**
 * @file
 * @brief EFR32BG24 MVP register and bit field definitions
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
#ifndef EFR32BG24_MVP_H
#define EFR32BG24_MVP_H
#define MVP_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32BG24_MVP MVP
 * @{
 * @brief EFR32BG24 MVP Register Declaration.
 *****************************************************************************/

/** MVP PERF Register Group Declaration. */
typedef struct {
  __IM uint32_t CNT;                                 /**< Run Counter                                        */
} MVP_PERF_TypeDef;

/** MVP ARRAYST Register Group Declaration. */
typedef struct {
  __IOM uint32_t INDEXSTATE;                         /**< Index State                                        */
} MVP_ARRAYST_TypeDef;

/** MVP LOOPST Register Group Declaration. */
typedef struct {
  __IOM uint32_t STATE;                              /**< Loop State                                         */
} MVP_LOOPST_TypeDef;

/** MVP ALU Register Group Declaration. */
typedef struct {
  __IOM uint32_t REGSTATE;                           /**< ALU Rn Register                                    */
} MVP_ALU_TypeDef;

/** MVP ARRAY Register Group Declaration. */
typedef struct {
  __IOM uint32_t ADDRCFG;                            /**< Array Base Address                                 */
  __IOM uint32_t DIM0CFG;                            /**< Dimension 0 Configuration                          */
  __IOM uint32_t DIM1CFG;                            /**< Dimension 1 Configuration                          */
  __IOM uint32_t DIM2CFG;                            /**< Dimension 2 Configuration                          */
} MVP_ARRAY_TypeDef;

/** MVP LOOP Register Group Declaration. */
typedef struct {
  __IOM uint32_t CFG;                                /**< Loop Configuration                                 */
  __IOM uint32_t RST;                                /**< Loop Reset                                         */
} MVP_LOOP_TypeDef;

/** MVP INSTR Register Group Declaration. */
typedef struct {
  __IOM uint32_t CFG0;                               /**< Instruction Configuration Word 0                   */
  __IOM uint32_t CFG1;                               /**< Instruction Configuration Word 1                   */
  __IOM uint32_t CFG2;                               /**< Instruction Configuration Word 2                   */
} MVP_INSTR_TypeDef;

/** MVP Register Declaration. */
typedef struct {
  __IM uint32_t       IPVERSION;                /**< IP Version                                         */
  __IOM uint32_t      EN;                       /**< Enable                                             */
  __IOM uint32_t      SWRST;                    /**< Software Reset                                     */
  __IOM uint32_t      CFG;                      /**< Configuration                                      */
  __IM uint32_t       STATUS;                   /**< Status                                             */
  MVP_PERF_TypeDef    PERF[2U];                 /**<                                                    */
  __IOM uint32_t      IF;                       /**< Interrupt Flags                                    */
  __IOM uint32_t      IEN;                      /**< Interrupt Enables                                  */
  __IM uint32_t       FAULTSTATUS;              /**< Fault Status                                       */
  __IM uint32_t       FAULTADDR;                /**< Fault Address                                      */
  __IOM uint32_t      PROGRAMSTATE;             /**< Program State                                      */
  MVP_ARRAYST_TypeDef ARRAYST[5U];              /**<                                                    */
  MVP_LOOPST_TypeDef  LOOPST[8U];               /**<                                                    */
  MVP_ALU_TypeDef     ALU[8U];                  /**<                                                    */
  MVP_ARRAY_TypeDef   ARRAY[5U];                /**<                                                    */
  MVP_LOOP_TypeDef    LOOP[8U];                 /**<                                                    */
  MVP_INSTR_TypeDef   INSTR[8U];                /**<                                                    */
  __IOM uint32_t      CMD;                      /**< Command Register                                   */
  uint32_t            RESERVED0[34U];           /**< Reserved for future use                            */
  __IOM uint32_t      DEBUGEN;                  /**< Debug Enable Register                              */
  __IOM uint32_t      DEBUGSTEPCNT;             /**< Debug Step Register                                */
  uint32_t            RESERVED1[894U];          /**< Reserved for future use                            */
  __IM uint32_t       IPVERSION_SET;            /**< IP Version                                         */
  __IOM uint32_t      EN_SET;                   /**< Enable                                             */
  __IOM uint32_t      SWRST_SET;                /**< Software Reset                                     */
  __IOM uint32_t      CFG_SET;                  /**< Configuration                                      */
  __IM uint32_t       STATUS_SET;               /**< Status                                             */
  MVP_PERF_TypeDef    PERF_SET[2U];             /**<                                                    */
  __IOM uint32_t      IF_SET;                   /**< Interrupt Flags                                    */
  __IOM uint32_t      IEN_SET;                  /**< Interrupt Enables                                  */
  __IM uint32_t       FAULTSTATUS_SET;          /**< Fault Status                                       */
  __IM uint32_t       FAULTADDR_SET;            /**< Fault Address                                      */
  __IOM uint32_t      PROGRAMSTATE_SET;         /**< Program State                                      */
  MVP_ARRAYST_TypeDef ARRAYST_SET[5U];          /**<                                                    */
  MVP_LOOPST_TypeDef  LOOPST_SET[8U];           /**<                                                    */
  MVP_ALU_TypeDef     ALU_SET[8U];              /**<                                                    */
  MVP_ARRAY_TypeDef   ARRAY_SET[5U];            /**<                                                    */
  MVP_LOOP_TypeDef    LOOP_SET[8U];             /**<                                                    */
  MVP_INSTR_TypeDef   INSTR_SET[8U];            /**<                                                    */
  __IOM uint32_t      CMD_SET;                  /**< Command Register                                   */
  uint32_t            RESERVED2[34U];           /**< Reserved for future use                            */
  __IOM uint32_t      DEBUGEN_SET;              /**< Debug Enable Register                              */
  __IOM uint32_t      DEBUGSTEPCNT_SET;         /**< Debug Step Register                                */
  uint32_t            RESERVED3[894U];          /**< Reserved for future use                            */
  __IM uint32_t       IPVERSION_CLR;            /**< IP Version                                         */
  __IOM uint32_t      EN_CLR;                   /**< Enable                                             */
  __IOM uint32_t      SWRST_CLR;                /**< Software Reset                                     */
  __IOM uint32_t      CFG_CLR;                  /**< Configuration                                      */
  __IM uint32_t       STATUS_CLR;               /**< Status                                             */
  MVP_PERF_TypeDef    PERF_CLR[2U];             /**<                                                    */
  __IOM uint32_t      IF_CLR;                   /**< Interrupt Flags                                    */
  __IOM uint32_t      IEN_CLR;                  /**< Interrupt Enables                                  */
  __IM uint32_t       FAULTSTATUS_CLR;          /**< Fault Status                                       */
  __IM uint32_t       FAULTADDR_CLR;            /**< Fault Address                                      */
  __IOM uint32_t      PROGRAMSTATE_CLR;         /**< Program State                                      */
  MVP_ARRAYST_TypeDef ARRAYST_CLR[5U];          /**<                                                    */
  MVP_LOOPST_TypeDef  LOOPST_CLR[8U];           /**<                                                    */
  MVP_ALU_TypeDef     ALU_CLR[8U];              /**<                                                    */
  MVP_ARRAY_TypeDef   ARRAY_CLR[5U];            /**<                                                    */
  MVP_LOOP_TypeDef    LOOP_CLR[8U];             /**<                                                    */
  MVP_INSTR_TypeDef   INSTR_CLR[8U];            /**<                                                    */
  __IOM uint32_t      CMD_CLR;                  /**< Command Register                                   */
  uint32_t            RESERVED4[34U];           /**< Reserved for future use                            */
  __IOM uint32_t      DEBUGEN_CLR;              /**< Debug Enable Register                              */
  __IOM uint32_t      DEBUGSTEPCNT_CLR;         /**< Debug Step Register                                */
  uint32_t            RESERVED5[894U];          /**< Reserved for future use                            */
  __IM uint32_t       IPVERSION_TGL;            /**< IP Version                                         */
  __IOM uint32_t      EN_TGL;                   /**< Enable                                             */
  __IOM uint32_t      SWRST_TGL;                /**< Software Reset                                     */
  __IOM uint32_t      CFG_TGL;                  /**< Configuration                                      */
  __IM uint32_t       STATUS_TGL;               /**< Status                                             */
  MVP_PERF_TypeDef    PERF_TGL[2U];             /**<                                                    */
  __IOM uint32_t      IF_TGL;                   /**< Interrupt Flags                                    */
  __IOM uint32_t      IEN_TGL;                  /**< Interrupt Enables                                  */
  __IM uint32_t       FAULTSTATUS_TGL;          /**< Fault Status                                       */
  __IM uint32_t       FAULTADDR_TGL;            /**< Fault Address                                      */
  __IOM uint32_t      PROGRAMSTATE_TGL;         /**< Program State                                      */
  MVP_ARRAYST_TypeDef ARRAYST_TGL[5U];          /**<                                                    */
  MVP_LOOPST_TypeDef  LOOPST_TGL[8U];           /**<                                                    */
  MVP_ALU_TypeDef     ALU_TGL[8U];              /**<                                                    */
  MVP_ARRAY_TypeDef   ARRAY_TGL[5U];            /**<                                                    */
  MVP_LOOP_TypeDef    LOOP_TGL[8U];             /**<                                                    */
  MVP_INSTR_TypeDef   INSTR_TGL[8U];            /**<                                                    */
  __IOM uint32_t      CMD_TGL;                  /**< Command Register                                   */
  uint32_t            RESERVED6[34U];           /**< Reserved for future use                            */
  __IOM uint32_t      DEBUGEN_TGL;              /**< Debug Enable Register                              */
  __IOM uint32_t      DEBUGSTEPCNT_TGL;         /**< Debug Step Register                                */
} MVP_TypeDef;
/** @} End of group EFR32BG24_MVP */

/**************************************************************************//**
 * @addtogroup EFR32BG24_MVP
 * @{
 * @defgroup EFR32BG24_MVP_BitFields MVP Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for MVP IPVERSION */
#define _MVP_IPVERSION_RESETVALUE                       0x00000001UL                            /**< Default value for MVP_IPVERSION             */
#define _MVP_IPVERSION_MASK                             0xFFFFFFFFUL                            /**< Mask for MVP_IPVERSION                      */
#define _MVP_IPVERSION_IPVERSION_SHIFT                  0                                       /**< Shift value for MVP_IPVERSION               */
#define _MVP_IPVERSION_IPVERSION_MASK                   0xFFFFFFFFUL                            /**< Bit mask for MVP_IPVERSION                  */
#define _MVP_IPVERSION_IPVERSION_DEFAULT                0x00000001UL                            /**< Mode DEFAULT for MVP_IPVERSION              */
#define MVP_IPVERSION_IPVERSION_DEFAULT                 (_MVP_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for MVP_IPVERSION      */

/* Bit fields for MVP EN */
#define _MVP_EN_RESETVALUE                              0x00000000UL                     /**< Default value for MVP_EN                    */
#define _MVP_EN_MASK                                    0x00000003UL                     /**< Mask for MVP_EN                             */
#define MVP_EN_EN                                       (0x1UL << 0)                     /**< Enable                                      */
#define _MVP_EN_EN_SHIFT                                0                                /**< Shift value for MVP_EN                      */
#define _MVP_EN_EN_MASK                                 0x1UL                            /**< Bit mask for MVP_EN                         */
#define _MVP_EN_EN_DEFAULT                              0x00000000UL                     /**< Mode DEFAULT for MVP_EN                     */
#define MVP_EN_EN_DEFAULT                               (_MVP_EN_EN_DEFAULT << 0)        /**< Shifted mode DEFAULT for MVP_EN             */
#define MVP_EN_DISABLING                                (0x1UL << 1)                     /**< Disablement Busy Status                     */
#define _MVP_EN_DISABLING_SHIFT                         1                                /**< Shift value for MVP_DISABLING               */
#define _MVP_EN_DISABLING_MASK                          0x2UL                            /**< Bit mask for MVP_DISABLING                  */
#define _MVP_EN_DISABLING_DEFAULT                       0x00000000UL                     /**< Mode DEFAULT for MVP_EN                     */
#define MVP_EN_DISABLING_DEFAULT                        (_MVP_EN_DISABLING_DEFAULT << 1) /**< Shifted mode DEFAULT for MVP_EN             */

/* Bit fields for MVP SWRST */
#define _MVP_SWRST_RESETVALUE                           0x00000000UL                        /**< Default value for MVP_SWRST                 */
#define _MVP_SWRST_MASK                                 0x00000003UL                        /**< Mask for MVP_SWRST                          */
#define MVP_SWRST_SWRST                                 (0x1UL << 0)                        /**< Software Reset Command                      */
#define _MVP_SWRST_SWRST_SHIFT                          0                                   /**< Shift value for MVP_SWRST                   */
#define _MVP_SWRST_SWRST_MASK                           0x1UL                               /**< Bit mask for MVP_SWRST                      */
#define _MVP_SWRST_SWRST_DEFAULT                        0x00000000UL                        /**< Mode DEFAULT for MVP_SWRST                  */
#define MVP_SWRST_SWRST_DEFAULT                         (_MVP_SWRST_SWRST_DEFAULT << 0)     /**< Shifted mode DEFAULT for MVP_SWRST          */
#define MVP_SWRST_RESETTING                             (0x1UL << 1)                        /**< Software Reset Busy Status                  */
#define _MVP_SWRST_RESETTING_SHIFT                      1                                   /**< Shift value for MVP_RESETTING               */
#define _MVP_SWRST_RESETTING_MASK                       0x2UL                               /**< Bit mask for MVP_RESETTING                  */
#define _MVP_SWRST_RESETTING_DEFAULT                    0x00000000UL                        /**< Mode DEFAULT for MVP_SWRST                  */
#define MVP_SWRST_RESETTING_DEFAULT                     (_MVP_SWRST_RESETTING_DEFAULT << 1) /**< Shifted mode DEFAULT for MVP_SWRST          */

/* Bit fields for MVP CFG */
#define _MVP_CFG_RESETVALUE                             0x00000000UL                                 /**< Default value for MVP_CFG                   */
#define _MVP_CFG_MASK                                   0x00FF000FUL                                 /**< Mask for MVP_CFG                            */
#define MVP_CFG_PERFCNTEN                               (0x1UL << 0)                                 /**< Performance Counter Enable                  */
#define _MVP_CFG_PERFCNTEN_SHIFT                        0                                            /**< Shift value for MVP_PERFCNTEN               */
#define _MVP_CFG_PERFCNTEN_MASK                         0x1UL                                        /**< Bit mask for MVP_PERFCNTEN                  */
#define _MVP_CFG_PERFCNTEN_DEFAULT                      0x00000000UL                                 /**< Mode DEFAULT for MVP_CFG                    */
#define MVP_CFG_PERFCNTEN_DEFAULT                       (_MVP_CFG_PERFCNTEN_DEFAULT << 0)            /**< Shifted mode DEFAULT for MVP_CFG            */
#define MVP_CFG_OUTCOMPRESSDIS                          (0x1UL << 1)                                 /**< ALU Output Stream Compression Disable       */
#define _MVP_CFG_OUTCOMPRESSDIS_SHIFT                   1                                            /**< Shift value for MVP_OUTCOMPRESSDIS          */
#define _MVP_CFG_OUTCOMPRESSDIS_MASK                    0x2UL                                        /**< Bit mask for MVP_OUTCOMPRESSDIS             */
#define _MVP_CFG_OUTCOMPRESSDIS_DEFAULT                 0x00000000UL                                 /**< Mode DEFAULT for MVP_CFG                    */
#define MVP_CFG_OUTCOMPRESSDIS_DEFAULT                  (_MVP_CFG_OUTCOMPRESSDIS_DEFAULT << 1)       /**< Shifted mode DEFAULT for MVP_CFG            */
#define MVP_CFG_INCACHEDIS                              (0x1UL << 2)                                 /**< ALU Input Word Cache Disable                */
#define _MVP_CFG_INCACHEDIS_SHIFT                       2                                            /**< Shift value for MVP_INCACHEDIS              */
#define _MVP_CFG_INCACHEDIS_MASK                        0x4UL                                        /**< Bit mask for MVP_INCACHEDIS                 */
#define _MVP_CFG_INCACHEDIS_DEFAULT                     0x00000000UL                                 /**< Mode DEFAULT for MVP_CFG                    */
#define MVP_CFG_INCACHEDIS_DEFAULT                      (_MVP_CFG_INCACHEDIS_DEFAULT << 2)           /**< Shifted mode DEFAULT for MVP_CFG            */
#define MVP_CFG_LOOPERRHALTDIS                          (0x1UL << 3)                                 /**< Loop Error Halt Disable                     */
#define _MVP_CFG_LOOPERRHALTDIS_SHIFT                   3                                            /**< Shift value for MVP_LOOPERRHALTDIS          */
#define _MVP_CFG_LOOPERRHALTDIS_MASK                    0x8UL                                        /**< Bit mask for MVP_LOOPERRHALTDIS             */
#define _MVP_CFG_LOOPERRHALTDIS_DEFAULT                 0x00000000UL                                 /**< Mode DEFAULT for MVP_CFG                    */
#define MVP_CFG_LOOPERRHALTDIS_DEFAULT                  (_MVP_CFG_LOOPERRHALTDIS_DEFAULT << 3)       /**< Shifted mode DEFAULT for MVP_CFG            */
#define _MVP_CFG_PERF0CNTSEL_SHIFT                      16                                           /**< Shift value for MVP_PERF0CNTSEL             */
#define _MVP_CFG_PERF0CNTSEL_MASK                       0xF0000UL                                    /**< Bit mask for MVP_PERF0CNTSEL                */
#define _MVP_CFG_PERF0CNTSEL_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for MVP_CFG                    */
#define _MVP_CFG_PERF0CNTSEL_RUN                        0x00000000UL                                 /**< Mode RUN for MVP_CFG                        */
#define _MVP_CFG_PERF0CNTSEL_CMD                        0x00000001UL                                 /**< Mode CMD for MVP_CFG                        */
#define _MVP_CFG_PERF0CNTSEL_STALL                      0x00000002UL                                 /**< Mode STALL for MVP_CFG                      */
#define _MVP_CFG_PERF0CNTSEL_NOOP                       0x00000003UL                                 /**< Mode NOOP for MVP_CFG                       */
#define _MVP_CFG_PERF0CNTSEL_ALUACTIVE                  0x00000004UL                                 /**< Mode ALUACTIVE for MVP_CFG                  */
#define _MVP_CFG_PERF0CNTSEL_PIPESTALL                  0x00000005UL                                 /**< Mode PIPESTALL for MVP_CFG                  */
#define _MVP_CFG_PERF0CNTSEL_IOFENCESTALL               0x00000006UL                                 /**< Mode IOFENCESTALL for MVP_CFG               */
#define _MVP_CFG_PERF0CNTSEL_LOAD0STALL                 0x00000007UL                                 /**< Mode LOAD0STALL for MVP_CFG                 */
#define _MVP_CFG_PERF0CNTSEL_LOAD1STALL                 0x00000008UL                                 /**< Mode LOAD1STALL for MVP_CFG                 */
#define _MVP_CFG_PERF0CNTSEL_STORESTALL                 0x00000009UL                                 /**< Mode STORESTALL for MVP_CFG                 */
#define _MVP_CFG_PERF0CNTSEL_BUSSTALL                   0x0000000AUL                                 /**< Mode BUSSTALL for MVP_CFG                   */
#define _MVP_CFG_PERF0CNTSEL_LOAD0AHBSTALL              0x0000000BUL                                 /**< Mode LOAD0AHBSTALL for MVP_CFG              */
#define _MVP_CFG_PERF0CNTSEL_LOAD1AHBSTALL              0x0000000CUL                                 /**< Mode LOAD1AHBSTALL for MVP_CFG              */
#define _MVP_CFG_PERF0CNTSEL_LOAD0FENCESTALL            0x0000000DUL                                 /**< Mode LOAD0FENCESTALL for MVP_CFG            */
#define _MVP_CFG_PERF0CNTSEL_LOAD1FENCESTALL            0x0000000EUL                                 /**< Mode LOAD1FENCESTALL for MVP_CFG            */
#define MVP_CFG_PERF0CNTSEL_DEFAULT                     (_MVP_CFG_PERF0CNTSEL_DEFAULT << 16)         /**< Shifted mode DEFAULT for MVP_CFG            */
#define MVP_CFG_PERF0CNTSEL_RUN                         (_MVP_CFG_PERF0CNTSEL_RUN << 16)             /**< Shifted mode RUN for MVP_CFG                */
#define MVP_CFG_PERF0CNTSEL_CMD                         (_MVP_CFG_PERF0CNTSEL_CMD << 16)             /**< Shifted mode CMD for MVP_CFG                */
#define MVP_CFG_PERF0CNTSEL_STALL                       (_MVP_CFG_PERF0CNTSEL_STALL << 16)           /**< Shifted mode STALL for MVP_CFG              */
#define MVP_CFG_PERF0CNTSEL_NOOP                        (_MVP_CFG_PERF0CNTSEL_NOOP << 16)            /**< Shifted mode NOOP for MVP_CFG               */
#define MVP_CFG_PERF0CNTSEL_ALUACTIVE                   (_MVP_CFG_PERF0CNTSEL_ALUACTIVE << 16)       /**< Shifted mode ALUACTIVE for MVP_CFG          */
#define MVP_CFG_PERF0CNTSEL_PIPESTALL                   (_MVP_CFG_PERF0CNTSEL_PIPESTALL << 16)       /**< Shifted mode PIPESTALL for MVP_CFG          */
#define MVP_CFG_PERF0CNTSEL_IOFENCESTALL                (_MVP_CFG_PERF0CNTSEL_IOFENCESTALL << 16)    /**< Shifted mode IOFENCESTALL for MVP_CFG       */
#define MVP_CFG_PERF0CNTSEL_LOAD0STALL                  (_MVP_CFG_PERF0CNTSEL_LOAD0STALL << 16)      /**< Shifted mode LOAD0STALL for MVP_CFG         */
#define MVP_CFG_PERF0CNTSEL_LOAD1STALL                  (_MVP_CFG_PERF0CNTSEL_LOAD1STALL << 16)      /**< Shifted mode LOAD1STALL for MVP_CFG         */
#define MVP_CFG_PERF0CNTSEL_STORESTALL                  (_MVP_CFG_PERF0CNTSEL_STORESTALL << 16)      /**< Shifted mode STORESTALL for MVP_CFG         */
#define MVP_CFG_PERF0CNTSEL_BUSSTALL                    (_MVP_CFG_PERF0CNTSEL_BUSSTALL << 16)        /**< Shifted mode BUSSTALL for MVP_CFG           */
#define MVP_CFG_PERF0CNTSEL_LOAD0AHBSTALL               (_MVP_CFG_PERF0CNTSEL_LOAD0AHBSTALL << 16)   /**< Shifted mode LOAD0AHBSTALL for MVP_CFG      */
#define MVP_CFG_PERF0CNTSEL_LOAD1AHBSTALL               (_MVP_CFG_PERF0CNTSEL_LOAD1AHBSTALL << 16)   /**< Shifted mode LOAD1AHBSTALL for MVP_CFG      */
#define MVP_CFG_PERF0CNTSEL_LOAD0FENCESTALL             (_MVP_CFG_PERF0CNTSEL_LOAD0FENCESTALL << 16) /**< Shifted mode LOAD0FENCESTALL for MVP_CFG    */
#define MVP_CFG_PERF0CNTSEL_LOAD1FENCESTALL             (_MVP_CFG_PERF0CNTSEL_LOAD1FENCESTALL << 16) /**< Shifted mode LOAD1FENCESTALL for MVP_CFG    */
#define _MVP_CFG_PERF1CNTSEL_SHIFT                      20                                           /**< Shift value for MVP_PERF1CNTSEL             */
#define _MVP_CFG_PERF1CNTSEL_MASK                       0xF00000UL                                   /**< Bit mask for MVP_PERF1CNTSEL                */
#define _MVP_CFG_PERF1CNTSEL_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for MVP_CFG                    */
#define _MVP_CFG_PERF1CNTSEL_RUN                        0x00000000UL                                 /**< Mode RUN for MVP_CFG                        */
#define _MVP_CFG_PERF1CNTSEL_CMD                        0x00000001UL                                 /**< Mode CMD for MVP_CFG                        */
#define _MVP_CFG_PERF1CNTSEL_STALL                      0x00000002UL                                 /**< Mode STALL for MVP_CFG                      */
#define _MVP_CFG_PERF1CNTSEL_NOOP                       0x00000003UL                                 /**< Mode NOOP for MVP_CFG                       */
#define _MVP_CFG_PERF1CNTSEL_ALUACTIVE                  0x00000004UL                                 /**< Mode ALUACTIVE for MVP_CFG                  */
#define _MVP_CFG_PERF1CNTSEL_PIPESTALL                  0x00000005UL                                 /**< Mode PIPESTALL for MVP_CFG                  */
#define _MVP_CFG_PERF1CNTSEL_IOFENCESTALL               0x00000006UL                                 /**< Mode IOFENCESTALL for MVP_CFG               */
#define _MVP_CFG_PERF1CNTSEL_LOAD0STALL                 0x00000007UL                                 /**< Mode LOAD0STALL for MVP_CFG                 */
#define _MVP_CFG_PERF1CNTSEL_LOAD1STALL                 0x00000008UL                                 /**< Mode LOAD1STALL for MVP_CFG                 */
#define _MVP_CFG_PERF1CNTSEL_STORESTALL                 0x00000009UL                                 /**< Mode STORESTALL for MVP_CFG                 */
#define _MVP_CFG_PERF1CNTSEL_BUSSTALL                   0x0000000AUL                                 /**< Mode BUSSTALL for MVP_CFG                   */
#define _MVP_CFG_PERF1CNTSEL_LOAD0AHBSTALL              0x0000000BUL                                 /**< Mode LOAD0AHBSTALL for MVP_CFG              */
#define _MVP_CFG_PERF1CNTSEL_LOAD1AHBSTALL              0x0000000CUL                                 /**< Mode LOAD1AHBSTALL for MVP_CFG              */
#define _MVP_CFG_PERF1CNTSEL_LOAD0FENCESTALL            0x0000000DUL                                 /**< Mode LOAD0FENCESTALL for MVP_CFG            */
#define _MVP_CFG_PERF1CNTSEL_LOAD1FENCESTALL            0x0000000EUL                                 /**< Mode LOAD1FENCESTALL for MVP_CFG            */
#define MVP_CFG_PERF1CNTSEL_DEFAULT                     (_MVP_CFG_PERF1CNTSEL_DEFAULT << 20)         /**< Shifted mode DEFAULT for MVP_CFG            */
#define MVP_CFG_PERF1CNTSEL_RUN                         (_MVP_CFG_PERF1CNTSEL_RUN << 20)             /**< Shifted mode RUN for MVP_CFG                */
#define MVP_CFG_PERF1CNTSEL_CMD                         (_MVP_CFG_PERF1CNTSEL_CMD << 20)             /**< Shifted mode CMD for MVP_CFG                */
#define MVP_CFG_PERF1CNTSEL_STALL                       (_MVP_CFG_PERF1CNTSEL_STALL << 20)           /**< Shifted mode STALL for MVP_CFG              */
#define MVP_CFG_PERF1CNTSEL_NOOP                        (_MVP_CFG_PERF1CNTSEL_NOOP << 20)            /**< Shifted mode NOOP for MVP_CFG               */
#define MVP_CFG_PERF1CNTSEL_ALUACTIVE                   (_MVP_CFG_PERF1CNTSEL_ALUACTIVE << 20)       /**< Shifted mode ALUACTIVE for MVP_CFG          */
#define MVP_CFG_PERF1CNTSEL_PIPESTALL                   (_MVP_CFG_PERF1CNTSEL_PIPESTALL << 20)       /**< Shifted mode PIPESTALL for MVP_CFG          */
#define MVP_CFG_PERF1CNTSEL_IOFENCESTALL                (_MVP_CFG_PERF1CNTSEL_IOFENCESTALL << 20)    /**< Shifted mode IOFENCESTALL for MVP_CFG       */
#define MVP_CFG_PERF1CNTSEL_LOAD0STALL                  (_MVP_CFG_PERF1CNTSEL_LOAD0STALL << 20)      /**< Shifted mode LOAD0STALL for MVP_CFG         */
#define MVP_CFG_PERF1CNTSEL_LOAD1STALL                  (_MVP_CFG_PERF1CNTSEL_LOAD1STALL << 20)      /**< Shifted mode LOAD1STALL for MVP_CFG         */
#define MVP_CFG_PERF1CNTSEL_STORESTALL                  (_MVP_CFG_PERF1CNTSEL_STORESTALL << 20)      /**< Shifted mode STORESTALL for MVP_CFG         */
#define MVP_CFG_PERF1CNTSEL_BUSSTALL                    (_MVP_CFG_PERF1CNTSEL_BUSSTALL << 20)        /**< Shifted mode BUSSTALL for MVP_CFG           */
#define MVP_CFG_PERF1CNTSEL_LOAD0AHBSTALL               (_MVP_CFG_PERF1CNTSEL_LOAD0AHBSTALL << 20)   /**< Shifted mode LOAD0AHBSTALL for MVP_CFG      */
#define MVP_CFG_PERF1CNTSEL_LOAD1AHBSTALL               (_MVP_CFG_PERF1CNTSEL_LOAD1AHBSTALL << 20)   /**< Shifted mode LOAD1AHBSTALL for MVP_CFG      */
#define MVP_CFG_PERF1CNTSEL_LOAD0FENCESTALL             (_MVP_CFG_PERF1CNTSEL_LOAD0FENCESTALL << 20) /**< Shifted mode LOAD0FENCESTALL for MVP_CFG    */
#define MVP_CFG_PERF1CNTSEL_LOAD1FENCESTALL             (_MVP_CFG_PERF1CNTSEL_LOAD1FENCESTALL << 20) /**< Shifted mode LOAD1FENCESTALL for MVP_CFG    */

/* Bit fields for MVP STATUS */
#define _MVP_STATUS_RESETVALUE                          0x00000004UL                       /**< Default value for MVP_STATUS                */
#define _MVP_STATUS_MASK                                0x00000007UL                       /**< Mask for MVP_STATUS                         */
#define MVP_STATUS_RUNNING                              (0x1UL << 0)                       /**< Running Status                              */
#define _MVP_STATUS_RUNNING_SHIFT                       0                                  /**< Shift value for MVP_RUNNING                 */
#define _MVP_STATUS_RUNNING_MASK                        0x1UL                              /**< Bit mask for MVP_RUNNING                    */
#define _MVP_STATUS_RUNNING_DEFAULT                     0x00000000UL                       /**< Mode DEFAULT for MVP_STATUS                 */
#define MVP_STATUS_RUNNING_DEFAULT                      (_MVP_STATUS_RUNNING_DEFAULT << 0) /**< Shifted mode DEFAULT for MVP_STATUS         */
#define MVP_STATUS_PAUSED                               (0x1UL << 1)                       /**< Paused Status                               */
#define _MVP_STATUS_PAUSED_SHIFT                        1                                  /**< Shift value for MVP_PAUSED                  */
#define _MVP_STATUS_PAUSED_MASK                         0x2UL                              /**< Bit mask for MVP_PAUSED                     */
#define _MVP_STATUS_PAUSED_DEFAULT                      0x00000000UL                       /**< Mode DEFAULT for MVP_STATUS                 */
#define MVP_STATUS_PAUSED_DEFAULT                       (_MVP_STATUS_PAUSED_DEFAULT << 1)  /**< Shifted mode DEFAULT for MVP_STATUS         */
#define MVP_STATUS_IDLE                                 (0x1UL << 2)                       /**< Idle Status                                 */
#define _MVP_STATUS_IDLE_SHIFT                          2                                  /**< Shift value for MVP_IDLE                    */
#define _MVP_STATUS_IDLE_MASK                           0x4UL                              /**< Bit mask for MVP_IDLE                       */
#define _MVP_STATUS_IDLE_DEFAULT                        0x00000001UL                       /**< Mode DEFAULT for MVP_STATUS                 */
#define MVP_STATUS_IDLE_DEFAULT                         (_MVP_STATUS_IDLE_DEFAULT << 2)    /**< Shifted mode DEFAULT for MVP_STATUS         */

/* Bit fields for MVP PERFCNT */
#define _MVP_PERFCNT_RESETVALUE                         0x00000000UL                      /**< Default value for MVP_PERFCNT               */
#define _MVP_PERFCNT_MASK                               0x00FFFFFFUL                      /**< Mask for MVP_PERFCNT                        */
#define _MVP_PERFCNT_COUNT_SHIFT                        0                                 /**< Shift value for MVP_COUNT                   */
#define _MVP_PERFCNT_COUNT_MASK                         0xFFFFFFUL                        /**< Bit mask for MVP_COUNT                      */
#define _MVP_PERFCNT_COUNT_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for MVP_PERFCNT                */
#define MVP_PERFCNT_COUNT_DEFAULT                       (_MVP_PERFCNT_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for MVP_PERFCNT        */

/* Bit fields for MVP IF */
#define _MVP_IF_RESETVALUE                              0x00000000UL                            /**< Default value for MVP_IF                    */
#define _MVP_IF_MASK                                    0x1F0FFDFFUL                            /**< Mask for MVP_IF                             */
#define MVP_IF_PROGDONE                                 (0x1UL << 0)                            /**< Program Done Interrupt Flags                */
#define _MVP_IF_PROGDONE_SHIFT                          0                                       /**< Shift value for MVP_PROGDONE                */
#define _MVP_IF_PROGDONE_MASK                           0x1UL                                   /**< Bit mask for MVP_PROGDONE                   */
#define _MVP_IF_PROGDONE_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_PROGDONE_DEFAULT                         (_MVP_IF_PROGDONE_DEFAULT << 0)         /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_LOOP0DONE                                (0x1UL << 1)                            /**< Loop Done Interrupt Flag                    */
#define _MVP_IF_LOOP0DONE_SHIFT                         1                                       /**< Shift value for MVP_LOOP0DONE               */
#define _MVP_IF_LOOP0DONE_MASK                          0x2UL                                   /**< Bit mask for MVP_LOOP0DONE                  */
#define _MVP_IF_LOOP0DONE_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_LOOP0DONE_DEFAULT                        (_MVP_IF_LOOP0DONE_DEFAULT << 1)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_LOOP1DONE                                (0x1UL << 2)                            /**< Loop Done Interrupt Flag                    */
#define _MVP_IF_LOOP1DONE_SHIFT                         2                                       /**< Shift value for MVP_LOOP1DONE               */
#define _MVP_IF_LOOP1DONE_MASK                          0x4UL                                   /**< Bit mask for MVP_LOOP1DONE                  */
#define _MVP_IF_LOOP1DONE_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_LOOP1DONE_DEFAULT                        (_MVP_IF_LOOP1DONE_DEFAULT << 2)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_LOOP2DONE                                (0x1UL << 3)                            /**< Loop Done Interrupt Flag                    */
#define _MVP_IF_LOOP2DONE_SHIFT                         3                                       /**< Shift value for MVP_LOOP2DONE               */
#define _MVP_IF_LOOP2DONE_MASK                          0x8UL                                   /**< Bit mask for MVP_LOOP2DONE                  */
#define _MVP_IF_LOOP2DONE_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_LOOP2DONE_DEFAULT                        (_MVP_IF_LOOP2DONE_DEFAULT << 3)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_LOOP3DONE                                (0x1UL << 4)                            /**< Loop Done Interrupt Flag                    */
#define _MVP_IF_LOOP3DONE_SHIFT                         4                                       /**< Shift value for MVP_LOOP3DONE               */
#define _MVP_IF_LOOP3DONE_MASK                          0x10UL                                  /**< Bit mask for MVP_LOOP3DONE                  */
#define _MVP_IF_LOOP3DONE_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_LOOP3DONE_DEFAULT                        (_MVP_IF_LOOP3DONE_DEFAULT << 4)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_LOOP4DONE                                (0x1UL << 5)                            /**< Loop Done Interrupt Flag                    */
#define _MVP_IF_LOOP4DONE_SHIFT                         5                                       /**< Shift value for MVP_LOOP4DONE               */
#define _MVP_IF_LOOP4DONE_MASK                          0x20UL                                  /**< Bit mask for MVP_LOOP4DONE                  */
#define _MVP_IF_LOOP4DONE_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_LOOP4DONE_DEFAULT                        (_MVP_IF_LOOP4DONE_DEFAULT << 5)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_LOOP5DONE                                (0x1UL << 6)                            /**< Loop Done Interrupt Flag                    */
#define _MVP_IF_LOOP5DONE_SHIFT                         6                                       /**< Shift value for MVP_LOOP5DONE               */
#define _MVP_IF_LOOP5DONE_MASK                          0x40UL                                  /**< Bit mask for MVP_LOOP5DONE                  */
#define _MVP_IF_LOOP5DONE_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_LOOP5DONE_DEFAULT                        (_MVP_IF_LOOP5DONE_DEFAULT << 6)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_LOOP6DONE                                (0x1UL << 7)                            /**< Loop Done Interrupt Flag                    */
#define _MVP_IF_LOOP6DONE_SHIFT                         7                                       /**< Shift value for MVP_LOOP6DONE               */
#define _MVP_IF_LOOP6DONE_MASK                          0x80UL                                  /**< Bit mask for MVP_LOOP6DONE                  */
#define _MVP_IF_LOOP6DONE_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_LOOP6DONE_DEFAULT                        (_MVP_IF_LOOP6DONE_DEFAULT << 7)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_LOOP7DONE                                (0x1UL << 8)                            /**< Loop Done Interrupt Flag                    */
#define _MVP_IF_LOOP7DONE_SHIFT                         8                                       /**< Shift value for MVP_LOOP7DONE               */
#define _MVP_IF_LOOP7DONE_MASK                          0x100UL                                 /**< Bit mask for MVP_LOOP7DONE                  */
#define _MVP_IF_LOOP7DONE_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_LOOP7DONE_DEFAULT                        (_MVP_IF_LOOP7DONE_DEFAULT << 8)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_ALUNAN                                   (0x1UL << 10)                           /**< Not-a-Number Interrupt Flag                 */
#define _MVP_IF_ALUNAN_SHIFT                            10                                      /**< Shift value for MVP_ALUNAN                  */
#define _MVP_IF_ALUNAN_MASK                             0x400UL                                 /**< Bit mask for MVP_ALUNAN                     */
#define _MVP_IF_ALUNAN_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_ALUNAN_DEFAULT                           (_MVP_IF_ALUNAN_DEFAULT << 10)          /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_R0POSREAL                                (0x1UL << 11)                           /**< R0 non-zero Interrupt Flag                  */
#define _MVP_IF_R0POSREAL_SHIFT                         11                                      /**< Shift value for MVP_R0POSREAL               */
#define _MVP_IF_R0POSREAL_MASK                          0x800UL                                 /**< Bit mask for MVP_R0POSREAL                  */
#define _MVP_IF_R0POSREAL_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_R0POSREAL_DEFAULT                        (_MVP_IF_R0POSREAL_DEFAULT << 11)       /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_ALUOF                                    (0x1UL << 12)                           /**< ALU Overflow on result                      */
#define _MVP_IF_ALUOF_SHIFT                             12                                      /**< Shift value for MVP_ALUOF                   */
#define _MVP_IF_ALUOF_MASK                              0x1000UL                                /**< Bit mask for MVP_ALUOF                      */
#define _MVP_IF_ALUOF_DEFAULT                           0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_ALUOF_DEFAULT                            (_MVP_IF_ALUOF_DEFAULT << 12)           /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_ALUUF                                    (0x1UL << 13)                           /**< ALU Underflow on result                     */
#define _MVP_IF_ALUUF_SHIFT                             13                                      /**< Shift value for MVP_ALUUF                   */
#define _MVP_IF_ALUUF_MASK                              0x2000UL                                /**< Bit mask for MVP_ALUUF                      */
#define _MVP_IF_ALUUF_DEFAULT                           0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_ALUUF_DEFAULT                            (_MVP_IF_ALUUF_DEFAULT << 13)           /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_STORECONVERTOF                           (0x1UL << 14)                           /**< Overflow during array store                 */
#define _MVP_IF_STORECONVERTOF_SHIFT                    14                                      /**< Shift value for MVP_STORECONVERTOF          */
#define _MVP_IF_STORECONVERTOF_MASK                     0x4000UL                                /**< Bit mask for MVP_STORECONVERTOF             */
#define _MVP_IF_STORECONVERTOF_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_STORECONVERTOF_DEFAULT                   (_MVP_IF_STORECONVERTOF_DEFAULT << 14)  /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_STORECONVERTUF                           (0x1UL << 15)                           /**< Underflow during array store conversion     */
#define _MVP_IF_STORECONVERTUF_SHIFT                    15                                      /**< Shift value for MVP_STORECONVERTUF          */
#define _MVP_IF_STORECONVERTUF_MASK                     0x8000UL                                /**< Bit mask for MVP_STORECONVERTUF             */
#define _MVP_IF_STORECONVERTUF_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_STORECONVERTUF_DEFAULT                   (_MVP_IF_STORECONVERTUF_DEFAULT << 15)  /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_STORECONVERTINF                          (0x1UL << 16)                           /**< Infinity encountered during array store conversion*/
#define _MVP_IF_STORECONVERTINF_SHIFT                   16                                      /**< Shift value for MVP_STORECONVERTINF         */
#define _MVP_IF_STORECONVERTINF_MASK                    0x10000UL                               /**< Bit mask for MVP_STORECONVERTINF            */
#define _MVP_IF_STORECONVERTINF_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_STORECONVERTINF_DEFAULT                  (_MVP_IF_STORECONVERTINF_DEFAULT << 16) /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_STORECONVERTNAN                          (0x1UL << 17)                           /**< NaN encountered during array store conversion*/
#define _MVP_IF_STORECONVERTNAN_SHIFT                   17                                      /**< Shift value for MVP_STORECONVERTNAN         */
#define _MVP_IF_STORECONVERTNAN_MASK                    0x20000UL                               /**< Bit mask for MVP_STORECONVERTNAN            */
#define _MVP_IF_STORECONVERTNAN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_STORECONVERTNAN_DEFAULT                  (_MVP_IF_STORECONVERTNAN_DEFAULT << 17) /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_PERFCNT0                                 (0x1UL << 18)                           /**< Run Count Overflow Interrupt Flag           */
#define _MVP_IF_PERFCNT0_SHIFT                          18                                      /**< Shift value for MVP_PERFCNT0                */
#define _MVP_IF_PERFCNT0_MASK                           0x40000UL                               /**< Bit mask for MVP_PERFCNT0                   */
#define _MVP_IF_PERFCNT0_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_PERFCNT0_DEFAULT                         (_MVP_IF_PERFCNT0_DEFAULT << 18)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_PERFCNT1                                 (0x1UL << 19)                           /**< Stall Count Overflow Interrupt Flag         */
#define _MVP_IF_PERFCNT1_SHIFT                          19                                      /**< Shift value for MVP_PERFCNT1                */
#define _MVP_IF_PERFCNT1_MASK                           0x80000UL                               /**< Bit mask for MVP_PERFCNT1                   */
#define _MVP_IF_PERFCNT1_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_PERFCNT1_DEFAULT                         (_MVP_IF_PERFCNT1_DEFAULT << 19)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_LOOPFAULT                                (0x1UL << 24)                           /**< Loop Fault Interrupt Flag                   */
#define _MVP_IF_LOOPFAULT_SHIFT                         24                                      /**< Shift value for MVP_LOOPFAULT               */
#define _MVP_IF_LOOPFAULT_MASK                          0x1000000UL                             /**< Bit mask for MVP_LOOPFAULT                  */
#define _MVP_IF_LOOPFAULT_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_LOOPFAULT_DEFAULT                        (_MVP_IF_LOOPFAULT_DEFAULT << 24)       /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_BUSERRFAULT                              (0x1UL << 25)                           /**< Bus Error Fault Interrupt Flag              */
#define _MVP_IF_BUSERRFAULT_SHIFT                       25                                      /**< Shift value for MVP_BUSERRFAULT             */
#define _MVP_IF_BUSERRFAULT_MASK                        0x2000000UL                             /**< Bit mask for MVP_BUSERRFAULT                */
#define _MVP_IF_BUSERRFAULT_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_BUSERRFAULT_DEFAULT                      (_MVP_IF_BUSERRFAULT_DEFAULT << 25)     /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_BUSALIGNFAULT                            (0x1UL << 26)                           /**< Bus Alignment Fault Interrupt Flag          */
#define _MVP_IF_BUSALIGNFAULT_SHIFT                     26                                      /**< Shift value for MVP_BUSALIGNFAULT           */
#define _MVP_IF_BUSALIGNFAULT_MASK                      0x4000000UL                             /**< Bit mask for MVP_BUSALIGNFAULT              */
#define _MVP_IF_BUSALIGNFAULT_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_BUSALIGNFAULT_DEFAULT                    (_MVP_IF_BUSALIGNFAULT_DEFAULT << 26)   /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_ALUFAULT                                 (0x1UL << 27)                           /**< ALU Fault Interrupt Flag                    */
#define _MVP_IF_ALUFAULT_SHIFT                          27                                      /**< Shift value for MVP_ALUFAULT                */
#define _MVP_IF_ALUFAULT_MASK                           0x8000000UL                             /**< Bit mask for MVP_ALUFAULT                   */
#define _MVP_IF_ALUFAULT_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_ALUFAULT_DEFAULT                         (_MVP_IF_ALUFAULT_DEFAULT << 27)        /**< Shifted mode DEFAULT for MVP_IF             */
#define MVP_IF_ARRAYFAULT                               (0x1UL << 28)                           /**< Array Fault Interrupt Flag                  */
#define _MVP_IF_ARRAYFAULT_SHIFT                        28                                      /**< Shift value for MVP_ARRAYFAULT              */
#define _MVP_IF_ARRAYFAULT_MASK                         0x10000000UL                            /**< Bit mask for MVP_ARRAYFAULT                 */
#define _MVP_IF_ARRAYFAULT_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for MVP_IF                     */
#define MVP_IF_ARRAYFAULT_DEFAULT                       (_MVP_IF_ARRAYFAULT_DEFAULT << 28)      /**< Shifted mode DEFAULT for MVP_IF             */

/* Bit fields for MVP IEN */
#define _MVP_IEN_RESETVALUE                             0x00000000UL                             /**< Default value for MVP_IEN                   */
#define _MVP_IEN_MASK                                   0x1F0FFDFFUL                             /**< Mask for MVP_IEN                            */
#define MVP_IEN_PROGDONE                                (0x1UL << 0)                             /**< Program Done Interrupt Enable               */
#define _MVP_IEN_PROGDONE_SHIFT                         0                                        /**< Shift value for MVP_PROGDONE                */
#define _MVP_IEN_PROGDONE_MASK                          0x1UL                                    /**< Bit mask for MVP_PROGDONE                   */
#define _MVP_IEN_PROGDONE_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_PROGDONE_DEFAULT                        (_MVP_IEN_PROGDONE_DEFAULT << 0)         /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_LOOP0DONE                               (0x1UL << 1)                             /**< Loop Done Interrupt Enable                  */
#define _MVP_IEN_LOOP0DONE_SHIFT                        1                                        /**< Shift value for MVP_LOOP0DONE               */
#define _MVP_IEN_LOOP0DONE_MASK                         0x2UL                                    /**< Bit mask for MVP_LOOP0DONE                  */
#define _MVP_IEN_LOOP0DONE_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_LOOP0DONE_DEFAULT                       (_MVP_IEN_LOOP0DONE_DEFAULT << 1)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_LOOP1DONE                               (0x1UL << 2)                             /**< Loop Done Interrupt Enable                  */
#define _MVP_IEN_LOOP1DONE_SHIFT                        2                                        /**< Shift value for MVP_LOOP1DONE               */
#define _MVP_IEN_LOOP1DONE_MASK                         0x4UL                                    /**< Bit mask for MVP_LOOP1DONE                  */
#define _MVP_IEN_LOOP1DONE_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_LOOP1DONE_DEFAULT                       (_MVP_IEN_LOOP1DONE_DEFAULT << 2)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_LOOP2DONE                               (0x1UL << 3)                             /**< Loop Done Interrupt Enable                  */
#define _MVP_IEN_LOOP2DONE_SHIFT                        3                                        /**< Shift value for MVP_LOOP2DONE               */
#define _MVP_IEN_LOOP2DONE_MASK                         0x8UL                                    /**< Bit mask for MVP_LOOP2DONE                  */
#define _MVP_IEN_LOOP2DONE_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_LOOP2DONE_DEFAULT                       (_MVP_IEN_LOOP2DONE_DEFAULT << 3)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_LOOP3DONE                               (0x1UL << 4)                             /**< Loop Done Interrupt Enable                  */
#define _MVP_IEN_LOOP3DONE_SHIFT                        4                                        /**< Shift value for MVP_LOOP3DONE               */
#define _MVP_IEN_LOOP3DONE_MASK                         0x10UL                                   /**< Bit mask for MVP_LOOP3DONE                  */
#define _MVP_IEN_LOOP3DONE_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_LOOP3DONE_DEFAULT                       (_MVP_IEN_LOOP3DONE_DEFAULT << 4)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_LOOP4DONE                               (0x1UL << 5)                             /**< Loop Done Interrupt Enable                  */
#define _MVP_IEN_LOOP4DONE_SHIFT                        5                                        /**< Shift value for MVP_LOOP4DONE               */
#define _MVP_IEN_LOOP4DONE_MASK                         0x20UL                                   /**< Bit mask for MVP_LOOP4DONE                  */
#define _MVP_IEN_LOOP4DONE_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_LOOP4DONE_DEFAULT                       (_MVP_IEN_LOOP4DONE_DEFAULT << 5)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_LOOP5DONE                               (0x1UL << 6)                             /**< Loop Done Interrupt Enable                  */
#define _MVP_IEN_LOOP5DONE_SHIFT                        6                                        /**< Shift value for MVP_LOOP5DONE               */
#define _MVP_IEN_LOOP5DONE_MASK                         0x40UL                                   /**< Bit mask for MVP_LOOP5DONE                  */
#define _MVP_IEN_LOOP5DONE_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_LOOP5DONE_DEFAULT                       (_MVP_IEN_LOOP5DONE_DEFAULT << 6)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_LOOP6DONE                               (0x1UL << 7)                             /**< Loop Done Interrupt Enable                  */
#define _MVP_IEN_LOOP6DONE_SHIFT                        7                                        /**< Shift value for MVP_LOOP6DONE               */
#define _MVP_IEN_LOOP6DONE_MASK                         0x80UL                                   /**< Bit mask for MVP_LOOP6DONE                  */
#define _MVP_IEN_LOOP6DONE_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_LOOP6DONE_DEFAULT                       (_MVP_IEN_LOOP6DONE_DEFAULT << 7)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_LOOP7DONE                               (0x1UL << 8)                             /**< Loop Done Interrupt Enable                  */
#define _MVP_IEN_LOOP7DONE_SHIFT                        8                                        /**< Shift value for MVP_LOOP7DONE               */
#define _MVP_IEN_LOOP7DONE_MASK                         0x100UL                                  /**< Bit mask for MVP_LOOP7DONE                  */
#define _MVP_IEN_LOOP7DONE_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_LOOP7DONE_DEFAULT                       (_MVP_IEN_LOOP7DONE_DEFAULT << 8)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_ALUNAN                                  (0x1UL << 10)                            /**< Not-a-Number Interrupt Enable               */
#define _MVP_IEN_ALUNAN_SHIFT                           10                                       /**< Shift value for MVP_ALUNAN                  */
#define _MVP_IEN_ALUNAN_MASK                            0x400UL                                  /**< Bit mask for MVP_ALUNAN                     */
#define _MVP_IEN_ALUNAN_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_ALUNAN_DEFAULT                          (_MVP_IEN_ALUNAN_DEFAULT << 10)          /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_R0POSREAL                               (0x1UL << 11)                            /**< R0 Non-Zero Interrupt Enable                */
#define _MVP_IEN_R0POSREAL_SHIFT                        11                                       /**< Shift value for MVP_R0POSREAL               */
#define _MVP_IEN_R0POSREAL_MASK                         0x800UL                                  /**< Bit mask for MVP_R0POSREAL                  */
#define _MVP_IEN_R0POSREAL_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_R0POSREAL_DEFAULT                       (_MVP_IEN_R0POSREAL_DEFAULT << 11)       /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_ALUOF                                   (0x1UL << 12)                            /**< ALU Overflow Interrupt Enable               */
#define _MVP_IEN_ALUOF_SHIFT                            12                                       /**< Shift value for MVP_ALUOF                   */
#define _MVP_IEN_ALUOF_MASK                             0x1000UL                                 /**< Bit mask for MVP_ALUOF                      */
#define _MVP_IEN_ALUOF_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_ALUOF_DEFAULT                           (_MVP_IEN_ALUOF_DEFAULT << 12)           /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_ALUUF                                   (0x1UL << 13)                            /**< ALU Underflow Interrupt Enable              */
#define _MVP_IEN_ALUUF_SHIFT                            13                                       /**< Shift value for MVP_ALUUF                   */
#define _MVP_IEN_ALUUF_MASK                             0x2000UL                                 /**< Bit mask for MVP_ALUUF                      */
#define _MVP_IEN_ALUUF_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_ALUUF_DEFAULT                           (_MVP_IEN_ALUUF_DEFAULT << 13)           /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_STORECONVERTOF                          (0x1UL << 14)                            /**< Store conversion Overflow Interrupt Enable  */
#define _MVP_IEN_STORECONVERTOF_SHIFT                   14                                       /**< Shift value for MVP_STORECONVERTOF          */
#define _MVP_IEN_STORECONVERTOF_MASK                    0x4000UL                                 /**< Bit mask for MVP_STORECONVERTOF             */
#define _MVP_IEN_STORECONVERTOF_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_STORECONVERTOF_DEFAULT                  (_MVP_IEN_STORECONVERTOF_DEFAULT << 14)  /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_STORECONVERTUF                          (0x1UL << 15)                            /**< Store Conversion Underflow Interrupt Enable */
#define _MVP_IEN_STORECONVERTUF_SHIFT                   15                                       /**< Shift value for MVP_STORECONVERTUF          */
#define _MVP_IEN_STORECONVERTUF_MASK                    0x8000UL                                 /**< Bit mask for MVP_STORECONVERTUF             */
#define _MVP_IEN_STORECONVERTUF_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_STORECONVERTUF_DEFAULT                  (_MVP_IEN_STORECONVERTUF_DEFAULT << 15)  /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_STORECONVERTINF                         (0x1UL << 16)                            /**< Store Conversion Infinity Interrupt Enable  */
#define _MVP_IEN_STORECONVERTINF_SHIFT                  16                                       /**< Shift value for MVP_STORECONVERTINF         */
#define _MVP_IEN_STORECONVERTINF_MASK                   0x10000UL                                /**< Bit mask for MVP_STORECONVERTINF            */
#define _MVP_IEN_STORECONVERTINF_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_STORECONVERTINF_DEFAULT                 (_MVP_IEN_STORECONVERTINF_DEFAULT << 16) /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_STORECONVERTNAN                         (0x1UL << 17)                            /**< Store Conversion NaN Interrupt Enable       */
#define _MVP_IEN_STORECONVERTNAN_SHIFT                  17                                       /**< Shift value for MVP_STORECONVERTNAN         */
#define _MVP_IEN_STORECONVERTNAN_MASK                   0x20000UL                                /**< Bit mask for MVP_STORECONVERTNAN            */
#define _MVP_IEN_STORECONVERTNAN_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_STORECONVERTNAN_DEFAULT                 (_MVP_IEN_STORECONVERTNAN_DEFAULT << 17) /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_PERFCNT0                                (0x1UL << 18)                            /**< Perf Counter 0 Overflow Interrupt Enable    */
#define _MVP_IEN_PERFCNT0_SHIFT                         18                                       /**< Shift value for MVP_PERFCNT0                */
#define _MVP_IEN_PERFCNT0_MASK                          0x40000UL                                /**< Bit mask for MVP_PERFCNT0                   */
#define _MVP_IEN_PERFCNT0_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_PERFCNT0_DEFAULT                        (_MVP_IEN_PERFCNT0_DEFAULT << 18)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_PERFCNT1                                (0x1UL << 19)                            /**< Perf Counter 1 Overflow Interrupt Enable    */
#define _MVP_IEN_PERFCNT1_SHIFT                         19                                       /**< Shift value for MVP_PERFCNT1                */
#define _MVP_IEN_PERFCNT1_MASK                          0x80000UL                                /**< Bit mask for MVP_PERFCNT1                   */
#define _MVP_IEN_PERFCNT1_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_PERFCNT1_DEFAULT                        (_MVP_IEN_PERFCNT1_DEFAULT << 19)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_LOOPFAULT                               (0x1UL << 24)                            /**< Loop Fault Interrupt Enable                 */
#define _MVP_IEN_LOOPFAULT_SHIFT                        24                                       /**< Shift value for MVP_LOOPFAULT               */
#define _MVP_IEN_LOOPFAULT_MASK                         0x1000000UL                              /**< Bit mask for MVP_LOOPFAULT                  */
#define _MVP_IEN_LOOPFAULT_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_LOOPFAULT_DEFAULT                       (_MVP_IEN_LOOPFAULT_DEFAULT << 24)       /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_BUSERRFAULT                             (0x1UL << 25)                            /**< Bus Error Fault Interrupt Enable            */
#define _MVP_IEN_BUSERRFAULT_SHIFT                      25                                       /**< Shift value for MVP_BUSERRFAULT             */
#define _MVP_IEN_BUSERRFAULT_MASK                       0x2000000UL                              /**< Bit mask for MVP_BUSERRFAULT                */
#define _MVP_IEN_BUSERRFAULT_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_BUSERRFAULT_DEFAULT                     (_MVP_IEN_BUSERRFAULT_DEFAULT << 25)     /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_BUSALIGNFAULT                           (0x1UL << 26)                            /**< Bus Alignment Fault Interrupt Enable        */
#define _MVP_IEN_BUSALIGNFAULT_SHIFT                    26                                       /**< Shift value for MVP_BUSALIGNFAULT           */
#define _MVP_IEN_BUSALIGNFAULT_MASK                     0x4000000UL                              /**< Bit mask for MVP_BUSALIGNFAULT              */
#define _MVP_IEN_BUSALIGNFAULT_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_BUSALIGNFAULT_DEFAULT                   (_MVP_IEN_BUSALIGNFAULT_DEFAULT << 26)   /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_ALUFAULT                                (0x1UL << 27)                            /**< ALU Input Fault Interrupt Enable            */
#define _MVP_IEN_ALUFAULT_SHIFT                         27                                       /**< Shift value for MVP_ALUFAULT                */
#define _MVP_IEN_ALUFAULT_MASK                          0x8000000UL                              /**< Bit mask for MVP_ALUFAULT                   */
#define _MVP_IEN_ALUFAULT_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_ALUFAULT_DEFAULT                        (_MVP_IEN_ALUFAULT_DEFAULT << 27)        /**< Shifted mode DEFAULT for MVP_IEN            */
#define MVP_IEN_ARRAYFAULT                              (0x1UL << 28)                            /**< Array Fault Interrupt Enable                */
#define _MVP_IEN_ARRAYFAULT_SHIFT                       28                                       /**< Shift value for MVP_ARRAYFAULT              */
#define _MVP_IEN_ARRAYFAULT_MASK                        0x10000000UL                             /**< Bit mask for MVP_ARRAYFAULT                 */
#define _MVP_IEN_ARRAYFAULT_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for MVP_IEN                    */
#define MVP_IEN_ARRAYFAULT_DEFAULT                      (_MVP_IEN_ARRAYFAULT_DEFAULT << 28)      /**< Shifted mode DEFAULT for MVP_IEN            */

/* Bit fields for MVP FAULTSTATUS */
#define _MVP_FAULTSTATUS_RESETVALUE                     0x00000000UL                                  /**< Default value for MVP_FAULTSTATUS           */
#define _MVP_FAULTSTATUS_MASK                           0x000F3707UL                                  /**< Mask for MVP_FAULTSTATUS                    */
#define _MVP_FAULTSTATUS_FAULTPC_SHIFT                  0                                             /**< Shift value for MVP_FAULTPC                 */
#define _MVP_FAULTSTATUS_FAULTPC_MASK                   0x7UL                                         /**< Bit mask for MVP_FAULTPC                    */
#define _MVP_FAULTSTATUS_FAULTPC_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for MVP_FAULTSTATUS            */
#define MVP_FAULTSTATUS_FAULTPC_DEFAULT                 (_MVP_FAULTSTATUS_FAULTPC_DEFAULT << 0)       /**< Shifted mode DEFAULT for MVP_FAULTSTATUS    */
#define _MVP_FAULTSTATUS_FAULTARRAY_SHIFT               8                                             /**< Shift value for MVP_FAULTARRAY              */
#define _MVP_FAULTSTATUS_FAULTARRAY_MASK                0x700UL                                       /**< Bit mask for MVP_FAULTARRAY                 */
#define _MVP_FAULTSTATUS_FAULTARRAY_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for MVP_FAULTSTATUS            */
#define MVP_FAULTSTATUS_FAULTARRAY_DEFAULT              (_MVP_FAULTSTATUS_FAULTARRAY_DEFAULT << 8)    /**< Shifted mode DEFAULT for MVP_FAULTSTATUS    */
#define _MVP_FAULTSTATUS_FAULTBUS_SHIFT                 12                                            /**< Shift value for MVP_FAULTBUS                */
#define _MVP_FAULTSTATUS_FAULTBUS_MASK                  0x3000UL                                      /**< Bit mask for MVP_FAULTBUS                   */
#define _MVP_FAULTSTATUS_FAULTBUS_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for MVP_FAULTSTATUS            */
#define _MVP_FAULTSTATUS_FAULTBUS_NONE                  0x00000000UL                                  /**< Mode NONE for MVP_FAULTSTATUS               */
#define _MVP_FAULTSTATUS_FAULTBUS_LOAD0STREAM           0x00000001UL                                  /**< Mode LOAD0STREAM for MVP_FAULTSTATUS        */
#define _MVP_FAULTSTATUS_FAULTBUS_LOAD1STREAM           0x00000002UL                                  /**< Mode LOAD1STREAM for MVP_FAULTSTATUS        */
#define _MVP_FAULTSTATUS_FAULTBUS_STORESTREAM           0x00000003UL                                  /**< Mode STORESTREAM for MVP_FAULTSTATUS        */
#define MVP_FAULTSTATUS_FAULTBUS_DEFAULT                (_MVP_FAULTSTATUS_FAULTBUS_DEFAULT << 12)     /**< Shifted mode DEFAULT for MVP_FAULTSTATUS    */
#define MVP_FAULTSTATUS_FAULTBUS_NONE                   (_MVP_FAULTSTATUS_FAULTBUS_NONE << 12)        /**< Shifted mode NONE for MVP_FAULTSTATUS       */
#define MVP_FAULTSTATUS_FAULTBUS_LOAD0STREAM            (_MVP_FAULTSTATUS_FAULTBUS_LOAD0STREAM << 12) /**< Shifted mode LOAD0STREAM for MVP_FAULTSTATUS*/
#define MVP_FAULTSTATUS_FAULTBUS_LOAD1STREAM            (_MVP_FAULTSTATUS_FAULTBUS_LOAD1STREAM << 12) /**< Shifted mode LOAD1STREAM for MVP_FAULTSTATUS*/
#define MVP_FAULTSTATUS_FAULTBUS_STORESTREAM            (_MVP_FAULTSTATUS_FAULTBUS_STORESTREAM << 12) /**< Shifted mode STORESTREAM for MVP_FAULTSTATUS*/
#define _MVP_FAULTSTATUS_FAULTLOOP_SHIFT                16                                            /**< Shift value for MVP_FAULTLOOP               */
#define _MVP_FAULTSTATUS_FAULTLOOP_MASK                 0xF0000UL                                     /**< Bit mask for MVP_FAULTLOOP                  */
#define _MVP_FAULTSTATUS_FAULTLOOP_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for MVP_FAULTSTATUS            */
#define MVP_FAULTSTATUS_FAULTLOOP_DEFAULT               (_MVP_FAULTSTATUS_FAULTLOOP_DEFAULT << 16)    /**< Shifted mode DEFAULT for MVP_FAULTSTATUS    */

/* Bit fields for MVP FAULTADDR */
#define _MVP_FAULTADDR_RESETVALUE                       0x00000000UL                            /**< Default value for MVP_FAULTADDR             */
#define _MVP_FAULTADDR_MASK                             0xFFFFFFFFUL                            /**< Mask for MVP_FAULTADDR                      */
#define _MVP_FAULTADDR_FAULTADDR_SHIFT                  0                                       /**< Shift value for MVP_FAULTADDR               */
#define _MVP_FAULTADDR_FAULTADDR_MASK                   0xFFFFFFFFUL                            /**< Bit mask for MVP_FAULTADDR                  */
#define _MVP_FAULTADDR_FAULTADDR_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for MVP_FAULTADDR              */
#define MVP_FAULTADDR_FAULTADDR_DEFAULT                 (_MVP_FAULTADDR_FAULTADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for MVP_FAULTADDR      */

/* Bit fields for MVP PROGRAMSTATE */
#define _MVP_PROGRAMSTATE_RESETVALUE                    0x00000000UL                        /**< Default value for MVP_PROGRAMSTATE          */
#define _MVP_PROGRAMSTATE_MASK                          0x00000007UL                        /**< Mask for MVP_PROGRAMSTATE                   */
#define _MVP_PROGRAMSTATE_PC_SHIFT                      0                                   /**< Shift value for MVP_PC                      */
#define _MVP_PROGRAMSTATE_PC_MASK                       0x7UL                               /**< Bit mask for MVP_PC                         */
#define _MVP_PROGRAMSTATE_PC_DEFAULT                    0x00000000UL                        /**< Mode DEFAULT for MVP_PROGRAMSTATE           */
#define MVP_PROGRAMSTATE_PC_DEFAULT                     (_MVP_PROGRAMSTATE_PC_DEFAULT << 0) /**< Shifted mode DEFAULT for MVP_PROGRAMSTATE   */

/* Bit fields for MVP ARRAYINDEXSTATE */
#define _MVP_ARRAYINDEXSTATE_RESETVALUE                 0x00000000UL                                   /**< Default value for MVP_ARRAYINDEXSTATE       */
#define _MVP_ARRAYINDEXSTATE_MASK                       0x3FFFFFFFUL                                   /**< Mask for MVP_ARRAYINDEXSTATE                */
#define _MVP_ARRAYINDEXSTATE_DIM0INDEX_SHIFT            0                                              /**< Shift value for MVP_DIM0INDEX               */
#define _MVP_ARRAYINDEXSTATE_DIM0INDEX_MASK             0x3FFUL                                        /**< Bit mask for MVP_DIM0INDEX                  */
#define _MVP_ARRAYINDEXSTATE_DIM0INDEX_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for MVP_ARRAYINDEXSTATE        */
#define MVP_ARRAYINDEXSTATE_DIM0INDEX_DEFAULT           (_MVP_ARRAYINDEXSTATE_DIM0INDEX_DEFAULT << 0)  /**< Shifted mode DEFAULT for MVP_ARRAYINDEXSTATE*/
#define _MVP_ARRAYINDEXSTATE_DIM1INDEX_SHIFT            10                                             /**< Shift value for MVP_DIM1INDEX               */
#define _MVP_ARRAYINDEXSTATE_DIM1INDEX_MASK             0xFFC00UL                                      /**< Bit mask for MVP_DIM1INDEX                  */
#define _MVP_ARRAYINDEXSTATE_DIM1INDEX_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for MVP_ARRAYINDEXSTATE        */
#define MVP_ARRAYINDEXSTATE_DIM1INDEX_DEFAULT           (_MVP_ARRAYINDEXSTATE_DIM1INDEX_DEFAULT << 10) /**< Shifted mode DEFAULT for MVP_ARRAYINDEXSTATE*/
#define _MVP_ARRAYINDEXSTATE_DIM2INDEX_SHIFT            20                                             /**< Shift value for MVP_DIM2INDEX               */
#define _MVP_ARRAYINDEXSTATE_DIM2INDEX_MASK             0x3FF00000UL                                   /**< Bit mask for MVP_DIM2INDEX                  */
#define _MVP_ARRAYINDEXSTATE_DIM2INDEX_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for MVP_ARRAYINDEXSTATE        */
#define MVP_ARRAYINDEXSTATE_DIM2INDEX_DEFAULT           (_MVP_ARRAYINDEXSTATE_DIM2INDEX_DEFAULT << 20) /**< Shifted mode DEFAULT for MVP_ARRAYINDEXSTATE*/

/* Bit fields for MVP LOOPSTATE */
#define _MVP_LOOPSTATE_RESETVALUE                       0x00000000UL                           /**< Default value for MVP_LOOPSTATE             */
#define _MVP_LOOPSTATE_MASK                             0x000713FFUL                           /**< Mask for MVP_LOOPSTATE                      */
#define _MVP_LOOPSTATE_CNT_SHIFT                        0                                      /**< Shift value for MVP_CNT                     */
#define _MVP_LOOPSTATE_CNT_MASK                         0x3FFUL                                /**< Bit mask for MVP_CNT                        */
#define _MVP_LOOPSTATE_CNT_DEFAULT                      0x00000000UL                           /**< Mode DEFAULT for MVP_LOOPSTATE              */
#define MVP_LOOPSTATE_CNT_DEFAULT                       (_MVP_LOOPSTATE_CNT_DEFAULT << 0)      /**< Shifted mode DEFAULT for MVP_LOOPSTATE      */
#define MVP_LOOPSTATE_ACTIVE                            (0x1UL << 12)                          /**< Loop Active                                 */
#define _MVP_LOOPSTATE_ACTIVE_SHIFT                     12                                     /**< Shift value for MVP_ACTIVE                  */
#define _MVP_LOOPSTATE_ACTIVE_MASK                      0x1000UL                               /**< Bit mask for MVP_ACTIVE                     */
#define _MVP_LOOPSTATE_ACTIVE_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for MVP_LOOPSTATE              */
#define MVP_LOOPSTATE_ACTIVE_DEFAULT                    (_MVP_LOOPSTATE_ACTIVE_DEFAULT << 12)  /**< Shifted mode DEFAULT for MVP_LOOPSTATE      */
#define _MVP_LOOPSTATE_PCBEGIN_SHIFT                    16                                     /**< Shift value for MVP_PCBEGIN                 */
#define _MVP_LOOPSTATE_PCBEGIN_MASK                     0x70000UL                              /**< Bit mask for MVP_PCBEGIN                    */
#define _MVP_LOOPSTATE_PCBEGIN_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for MVP_LOOPSTATE              */
#define MVP_LOOPSTATE_PCBEGIN_DEFAULT                   (_MVP_LOOPSTATE_PCBEGIN_DEFAULT << 16) /**< Shifted mode DEFAULT for MVP_LOOPSTATE      */

/* Bit fields for MVP ALUREGSTATE */
#define _MVP_ALUREGSTATE_RESETVALUE                     0x00000000UL                           /**< Default value for MVP_ALUREGSTATE           */
#define _MVP_ALUREGSTATE_MASK                           0xFFFFFFFFUL                           /**< Mask for MVP_ALUREGSTATE                    */
#define _MVP_ALUREGSTATE_FREAL_SHIFT                    0                                      /**< Shift value for MVP_FREAL                   */
#define _MVP_ALUREGSTATE_FREAL_MASK                     0xFFFFUL                               /**< Bit mask for MVP_FREAL                      */
#define _MVP_ALUREGSTATE_FREAL_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for MVP_ALUREGSTATE            */
#define MVP_ALUREGSTATE_FREAL_DEFAULT                   (_MVP_ALUREGSTATE_FREAL_DEFAULT << 0)  /**< Shifted mode DEFAULT for MVP_ALUREGSTATE    */
#define _MVP_ALUREGSTATE_FIMAG_SHIFT                    16                                     /**< Shift value for MVP_FIMAG                   */
#define _MVP_ALUREGSTATE_FIMAG_MASK                     0xFFFF0000UL                           /**< Bit mask for MVP_FIMAG                      */
#define _MVP_ALUREGSTATE_FIMAG_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for MVP_ALUREGSTATE            */
#define MVP_ALUREGSTATE_FIMAG_DEFAULT                   (_MVP_ALUREGSTATE_FIMAG_DEFAULT << 16) /**< Shifted mode DEFAULT for MVP_ALUREGSTATE    */

/* Bit fields for MVP ARRAYADDRCFG */
#define _MVP_ARRAYADDRCFG_RESETVALUE                    0x00000000UL                          /**< Default value for MVP_ARRAYADDRCFG          */
#define _MVP_ARRAYADDRCFG_MASK                          0xFFFFFFFFUL                          /**< Mask for MVP_ARRAYADDRCFG                   */
#define _MVP_ARRAYADDRCFG_BASE_SHIFT                    0                                     /**< Shift value for MVP_BASE                    */
#define _MVP_ARRAYADDRCFG_BASE_MASK                     0xFFFFFFFFUL                          /**< Bit mask for MVP_BASE                       */
#define _MVP_ARRAYADDRCFG_BASE_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for MVP_ARRAYADDRCFG           */
#define MVP_ARRAYADDRCFG_BASE_DEFAULT                   (_MVP_ARRAYADDRCFG_BASE_DEFAULT << 0) /**< Shifted mode DEFAULT for MVP_ARRAYADDRCFG   */

/* Bit fields for MVP ARRAYDIM0CFG */
#define _MVP_ARRAYDIM0CFG_RESETVALUE                    0x00002000UL                                /**< Default value for MVP_ARRAYDIM0CFG          */
#define _MVP_ARRAYDIM0CFG_MASK                          0x0FFF73FFUL                                /**< Mask for MVP_ARRAYDIM0CFG                   */
#define _MVP_ARRAYDIM0CFG_SIZE_SHIFT                    0                                           /**< Shift value for MVP_SIZE                    */
#define _MVP_ARRAYDIM0CFG_SIZE_MASK                     0x3FFUL                                     /**< Bit mask for MVP_SIZE                       */
#define _MVP_ARRAYDIM0CFG_SIZE_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for MVP_ARRAYDIM0CFG           */
#define MVP_ARRAYDIM0CFG_SIZE_DEFAULT                   (_MVP_ARRAYDIM0CFG_SIZE_DEFAULT << 0)       /**< Shifted mode DEFAULT for MVP_ARRAYDIM0CFG   */
#define _MVP_ARRAYDIM0CFG_BASETYPE_SHIFT                12                                          /**< Shift value for MVP_BASETYPE                */
#define _MVP_ARRAYDIM0CFG_BASETYPE_MASK                 0x3000UL                                    /**< Bit mask for MVP_BASETYPE                   */
#define _MVP_ARRAYDIM0CFG_BASETYPE_DEFAULT              0x00000002UL                                /**< Mode DEFAULT for MVP_ARRAYDIM0CFG           */
#define _MVP_ARRAYDIM0CFG_BASETYPE_UINT8                0x00000000UL                                /**< Mode UINT8 for MVP_ARRAYDIM0CFG             */
#define _MVP_ARRAYDIM0CFG_BASETYPE_INT8                 0x00000001UL                                /**< Mode INT8 for MVP_ARRAYDIM0CFG              */
#define _MVP_ARRAYDIM0CFG_BASETYPE_BINARY16             0x00000002UL                                /**< Mode BINARY16 for MVP_ARRAYDIM0CFG          */
#define _MVP_ARRAYDIM0CFG_BASETYPE_RESERVED             0x00000003UL                                /**< Mode RESERVED for MVP_ARRAYDIM0CFG          */
#define MVP_ARRAYDIM0CFG_BASETYPE_DEFAULT               (_MVP_ARRAYDIM0CFG_BASETYPE_DEFAULT << 12)  /**< Shifted mode DEFAULT for MVP_ARRAYDIM0CFG   */
#define MVP_ARRAYDIM0CFG_BASETYPE_UINT8                 (_MVP_ARRAYDIM0CFG_BASETYPE_UINT8 << 12)    /**< Shifted mode UINT8 for MVP_ARRAYDIM0CFG     */
#define MVP_ARRAYDIM0CFG_BASETYPE_INT8                  (_MVP_ARRAYDIM0CFG_BASETYPE_INT8 << 12)     /**< Shifted mode INT8 for MVP_ARRAYDIM0CFG      */
#define MVP_ARRAYDIM0CFG_BASETYPE_BINARY16              (_MVP_ARRAYDIM0CFG_BASETYPE_BINARY16 << 12) /**< Shifted mode BINARY16 for MVP_ARRAYDIM0CFG  */
#define MVP_ARRAYDIM0CFG_COMPLEX                        (0x1UL << 14)                               /**< Complex Data Type                           */
#define _MVP_ARRAYDIM0CFG_COMPLEX_SHIFT                 14                                          /**< Shift value for MVP_COMPLEX                 */
#define _MVP_ARRAYDIM0CFG_COMPLEX_MASK                  0x4000UL                                    /**< Bit mask for MVP_COMPLEX                    */
#define _MVP_ARRAYDIM0CFG_COMPLEX_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for MVP_ARRAYDIM0CFG           */
#define _MVP_ARRAYDIM0CFG_COMPLEX_SCALAR                0x00000000UL                                /**< Mode SCALAR for MVP_ARRAYDIM0CFG            */
#define _MVP_ARRAYDIM0CFG_COMPLEX_COMPLEX               0x00000001UL                                /**< Mode COMPLEX for MVP_ARRAYDIM0CFG           */
#define MVP_ARRAYDIM0CFG_COMPLEX_DEFAULT                (_MVP_ARRAYDIM0CFG_COMPLEX_DEFAULT << 14)   /**< Shifted mode DEFAULT for MVP_ARRAYDIM0CFG   */
#define MVP_ARRAYDIM0CFG_COMPLEX_SCALAR                 (_MVP_ARRAYDIM0CFG_COMPLEX_SCALAR << 14)    /**< Shifted mode SCALAR for MVP_ARRAYDIM0CFG    */
#define MVP_ARRAYDIM0CFG_COMPLEX_COMPLEX                (_MVP_ARRAYDIM0CFG_COMPLEX_COMPLEX << 14)   /**< Shifted mode COMPLEX for MVP_ARRAYDIM0CFG   */
#define _MVP_ARRAYDIM0CFG_STRIDE_SHIFT                  16                                          /**< Shift value for MVP_STRIDE                  */
#define _MVP_ARRAYDIM0CFG_STRIDE_MASK                   0xFFF0000UL                                 /**< Bit mask for MVP_STRIDE                     */
#define _MVP_ARRAYDIM0CFG_STRIDE_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for MVP_ARRAYDIM0CFG           */
#define MVP_ARRAYDIM0CFG_STRIDE_DEFAULT                 (_MVP_ARRAYDIM0CFG_STRIDE_DEFAULT << 16)    /**< Shifted mode DEFAULT for MVP_ARRAYDIM0CFG   */

/* Bit fields for MVP ARRAYDIM1CFG */
#define _MVP_ARRAYDIM1CFG_RESETVALUE                    0x00000000UL                             /**< Default value for MVP_ARRAYDIM1CFG          */
#define _MVP_ARRAYDIM1CFG_MASK                          0x0FFF03FFUL                             /**< Mask for MVP_ARRAYDIM1CFG                   */
#define _MVP_ARRAYDIM1CFG_SIZE_SHIFT                    0                                        /**< Shift value for MVP_SIZE                    */
#define _MVP_ARRAYDIM1CFG_SIZE_MASK                     0x3FFUL                                  /**< Bit mask for MVP_SIZE                       */
#define _MVP_ARRAYDIM1CFG_SIZE_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for MVP_ARRAYDIM1CFG           */
#define MVP_ARRAYDIM1CFG_SIZE_DEFAULT                   (_MVP_ARRAYDIM1CFG_SIZE_DEFAULT << 0)    /**< Shifted mode DEFAULT for MVP_ARRAYDIM1CFG   */
#define _MVP_ARRAYDIM1CFG_STRIDE_SHIFT                  16                                       /**< Shift value for MVP_STRIDE                  */
#define _MVP_ARRAYDIM1CFG_STRIDE_MASK                   0xFFF0000UL                              /**< Bit mask for MVP_STRIDE                     */
#define _MVP_ARRAYDIM1CFG_STRIDE_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for MVP_ARRAYDIM1CFG           */
#define MVP_ARRAYDIM1CFG_STRIDE_DEFAULT                 (_MVP_ARRAYDIM1CFG_STRIDE_DEFAULT << 16) /**< Shifted mode DEFAULT for MVP_ARRAYDIM1CFG   */

/* Bit fields for MVP ARRAYDIM2CFG */
#define _MVP_ARRAYDIM2CFG_RESETVALUE                    0x00000000UL                             /**< Default value for MVP_ARRAYDIM2CFG          */
#define _MVP_ARRAYDIM2CFG_MASK                          0x0FFF03FFUL                             /**< Mask for MVP_ARRAYDIM2CFG                   */
#define _MVP_ARRAYDIM2CFG_SIZE_SHIFT                    0                                        /**< Shift value for MVP_SIZE                    */
#define _MVP_ARRAYDIM2CFG_SIZE_MASK                     0x3FFUL                                  /**< Bit mask for MVP_SIZE                       */
#define _MVP_ARRAYDIM2CFG_SIZE_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for MVP_ARRAYDIM2CFG           */
#define MVP_ARRAYDIM2CFG_SIZE_DEFAULT                   (_MVP_ARRAYDIM2CFG_SIZE_DEFAULT << 0)    /**< Shifted mode DEFAULT for MVP_ARRAYDIM2CFG   */
#define _MVP_ARRAYDIM2CFG_STRIDE_SHIFT                  16                                       /**< Shift value for MVP_STRIDE                  */
#define _MVP_ARRAYDIM2CFG_STRIDE_MASK                   0xFFF0000UL                              /**< Bit mask for MVP_STRIDE                     */
#define _MVP_ARRAYDIM2CFG_STRIDE_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for MVP_ARRAYDIM2CFG           */
#define MVP_ARRAYDIM2CFG_STRIDE_DEFAULT                 (_MVP_ARRAYDIM2CFG_STRIDE_DEFAULT << 16) /**< Shifted mode DEFAULT for MVP_ARRAYDIM2CFG   */

/* Bit fields for MVP LOOPCFG */
#define _MVP_LOOPCFG_RESETVALUE                         0x00000000UL                                /**< Default value for MVP_LOOPCFG               */
#define _MVP_LOOPCFG_MASK                               0x777773FFUL                                /**< Mask for MVP_LOOPCFG                        */
#define _MVP_LOOPCFG_NUMITERS_SHIFT                     0                                           /**< Shift value for MVP_NUMITERS                */
#define _MVP_LOOPCFG_NUMITERS_MASK                      0x3FFUL                                     /**< Bit mask for MVP_NUMITERS                   */
#define _MVP_LOOPCFG_NUMITERS_DEFAULT                   0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_NUMITERS_DEFAULT                    (_MVP_LOOPCFG_NUMITERS_DEFAULT << 0)        /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY0INCRDIM0                      (0x1UL << 12)                               /**< Increment Dimension 0                       */
#define _MVP_LOOPCFG_ARRAY0INCRDIM0_SHIFT               12                                          /**< Shift value for MVP_ARRAY0INCRDIM0          */
#define _MVP_LOOPCFG_ARRAY0INCRDIM0_MASK                0x1000UL                                    /**< Bit mask for MVP_ARRAY0INCRDIM0             */
#define _MVP_LOOPCFG_ARRAY0INCRDIM0_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY0INCRDIM0_DEFAULT              (_MVP_LOOPCFG_ARRAY0INCRDIM0_DEFAULT << 12) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY0INCRDIM1                      (0x1UL << 13)                               /**< Increment Dimension 1                       */
#define _MVP_LOOPCFG_ARRAY0INCRDIM1_SHIFT               13                                          /**< Shift value for MVP_ARRAY0INCRDIM1          */
#define _MVP_LOOPCFG_ARRAY0INCRDIM1_MASK                0x2000UL                                    /**< Bit mask for MVP_ARRAY0INCRDIM1             */
#define _MVP_LOOPCFG_ARRAY0INCRDIM1_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY0INCRDIM1_DEFAULT              (_MVP_LOOPCFG_ARRAY0INCRDIM1_DEFAULT << 13) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY0INCRDIM2                      (0x1UL << 14)                               /**< Increment Dimension 2                       */
#define _MVP_LOOPCFG_ARRAY0INCRDIM2_SHIFT               14                                          /**< Shift value for MVP_ARRAY0INCRDIM2          */
#define _MVP_LOOPCFG_ARRAY0INCRDIM2_MASK                0x4000UL                                    /**< Bit mask for MVP_ARRAY0INCRDIM2             */
#define _MVP_LOOPCFG_ARRAY0INCRDIM2_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY0INCRDIM2_DEFAULT              (_MVP_LOOPCFG_ARRAY0INCRDIM2_DEFAULT << 14) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY1INCRDIM0                      (0x1UL << 16)                               /**< Increment Dimension 0                       */
#define _MVP_LOOPCFG_ARRAY1INCRDIM0_SHIFT               16                                          /**< Shift value for MVP_ARRAY1INCRDIM0          */
#define _MVP_LOOPCFG_ARRAY1INCRDIM0_MASK                0x10000UL                                   /**< Bit mask for MVP_ARRAY1INCRDIM0             */
#define _MVP_LOOPCFG_ARRAY1INCRDIM0_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY1INCRDIM0_DEFAULT              (_MVP_LOOPCFG_ARRAY1INCRDIM0_DEFAULT << 16) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY1INCRDIM1                      (0x1UL << 17)                               /**< Increment Dimension 1                       */
#define _MVP_LOOPCFG_ARRAY1INCRDIM1_SHIFT               17                                          /**< Shift value for MVP_ARRAY1INCRDIM1          */
#define _MVP_LOOPCFG_ARRAY1INCRDIM1_MASK                0x20000UL                                   /**< Bit mask for MVP_ARRAY1INCRDIM1             */
#define _MVP_LOOPCFG_ARRAY1INCRDIM1_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY1INCRDIM1_DEFAULT              (_MVP_LOOPCFG_ARRAY1INCRDIM1_DEFAULT << 17) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY1INCRDIM2                      (0x1UL << 18)                               /**< Increment Dimension 2                       */
#define _MVP_LOOPCFG_ARRAY1INCRDIM2_SHIFT               18                                          /**< Shift value for MVP_ARRAY1INCRDIM2          */
#define _MVP_LOOPCFG_ARRAY1INCRDIM2_MASK                0x40000UL                                   /**< Bit mask for MVP_ARRAY1INCRDIM2             */
#define _MVP_LOOPCFG_ARRAY1INCRDIM2_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY1INCRDIM2_DEFAULT              (_MVP_LOOPCFG_ARRAY1INCRDIM2_DEFAULT << 18) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY2INCRDIM0                      (0x1UL << 20)                               /**< Increment Dimension 0                       */
#define _MVP_LOOPCFG_ARRAY2INCRDIM0_SHIFT               20                                          /**< Shift value for MVP_ARRAY2INCRDIM0          */
#define _MVP_LOOPCFG_ARRAY2INCRDIM0_MASK                0x100000UL                                  /**< Bit mask for MVP_ARRAY2INCRDIM0             */
#define _MVP_LOOPCFG_ARRAY2INCRDIM0_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY2INCRDIM0_DEFAULT              (_MVP_LOOPCFG_ARRAY2INCRDIM0_DEFAULT << 20) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY2INCRDIM1                      (0x1UL << 21)                               /**< Increment Dimension 1                       */
#define _MVP_LOOPCFG_ARRAY2INCRDIM1_SHIFT               21                                          /**< Shift value for MVP_ARRAY2INCRDIM1          */
#define _MVP_LOOPCFG_ARRAY2INCRDIM1_MASK                0x200000UL                                  /**< Bit mask for MVP_ARRAY2INCRDIM1             */
#define _MVP_LOOPCFG_ARRAY2INCRDIM1_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY2INCRDIM1_DEFAULT              (_MVP_LOOPCFG_ARRAY2INCRDIM1_DEFAULT << 21) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY2INCRDIM2                      (0x1UL << 22)                               /**< Increment Dimension 2                       */
#define _MVP_LOOPCFG_ARRAY2INCRDIM2_SHIFT               22                                          /**< Shift value for MVP_ARRAY2INCRDIM2          */
#define _MVP_LOOPCFG_ARRAY2INCRDIM2_MASK                0x400000UL                                  /**< Bit mask for MVP_ARRAY2INCRDIM2             */
#define _MVP_LOOPCFG_ARRAY2INCRDIM2_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY2INCRDIM2_DEFAULT              (_MVP_LOOPCFG_ARRAY2INCRDIM2_DEFAULT << 22) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY3INCRDIM0                      (0x1UL << 24)                               /**< Increment Dimension 0                       */
#define _MVP_LOOPCFG_ARRAY3INCRDIM0_SHIFT               24                                          /**< Shift value for MVP_ARRAY3INCRDIM0          */
#define _MVP_LOOPCFG_ARRAY3INCRDIM0_MASK                0x1000000UL                                 /**< Bit mask for MVP_ARRAY3INCRDIM0             */
#define _MVP_LOOPCFG_ARRAY3INCRDIM0_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY3INCRDIM0_DEFAULT              (_MVP_LOOPCFG_ARRAY3INCRDIM0_DEFAULT << 24) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY3INCRDIM1                      (0x1UL << 25)                               /**< Increment Dimension 1                       */
#define _MVP_LOOPCFG_ARRAY3INCRDIM1_SHIFT               25                                          /**< Shift value for MVP_ARRAY3INCRDIM1          */
#define _MVP_LOOPCFG_ARRAY3INCRDIM1_MASK                0x2000000UL                                 /**< Bit mask for MVP_ARRAY3INCRDIM1             */
#define _MVP_LOOPCFG_ARRAY3INCRDIM1_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY3INCRDIM1_DEFAULT              (_MVP_LOOPCFG_ARRAY3INCRDIM1_DEFAULT << 25) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY3INCRDIM2                      (0x1UL << 26)                               /**< Increment Dimension 2                       */
#define _MVP_LOOPCFG_ARRAY3INCRDIM2_SHIFT               26                                          /**< Shift value for MVP_ARRAY3INCRDIM2          */
#define _MVP_LOOPCFG_ARRAY3INCRDIM2_MASK                0x4000000UL                                 /**< Bit mask for MVP_ARRAY3INCRDIM2             */
#define _MVP_LOOPCFG_ARRAY3INCRDIM2_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY3INCRDIM2_DEFAULT              (_MVP_LOOPCFG_ARRAY3INCRDIM2_DEFAULT << 26) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY4INCRDIM0                      (0x1UL << 28)                               /**< Increment Dimension 0                       */
#define _MVP_LOOPCFG_ARRAY4INCRDIM0_SHIFT               28                                          /**< Shift value for MVP_ARRAY4INCRDIM0          */
#define _MVP_LOOPCFG_ARRAY4INCRDIM0_MASK                0x10000000UL                                /**< Bit mask for MVP_ARRAY4INCRDIM0             */
#define _MVP_LOOPCFG_ARRAY4INCRDIM0_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY4INCRDIM0_DEFAULT              (_MVP_LOOPCFG_ARRAY4INCRDIM0_DEFAULT << 28) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY4INCRDIM1                      (0x1UL << 29)                               /**< Increment Dimension 1                       */
#define _MVP_LOOPCFG_ARRAY4INCRDIM1_SHIFT               29                                          /**< Shift value for MVP_ARRAY4INCRDIM1          */
#define _MVP_LOOPCFG_ARRAY4INCRDIM1_MASK                0x20000000UL                                /**< Bit mask for MVP_ARRAY4INCRDIM1             */
#define _MVP_LOOPCFG_ARRAY4INCRDIM1_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY4INCRDIM1_DEFAULT              (_MVP_LOOPCFG_ARRAY4INCRDIM1_DEFAULT << 29) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */
#define MVP_LOOPCFG_ARRAY4INCRDIM2                      (0x1UL << 30)                               /**< Increment Dimension 2                       */
#define _MVP_LOOPCFG_ARRAY4INCRDIM2_SHIFT               30                                          /**< Shift value for MVP_ARRAY4INCRDIM2          */
#define _MVP_LOOPCFG_ARRAY4INCRDIM2_MASK                0x40000000UL                                /**< Bit mask for MVP_ARRAY4INCRDIM2             */
#define _MVP_LOOPCFG_ARRAY4INCRDIM2_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for MVP_LOOPCFG                */
#define MVP_LOOPCFG_ARRAY4INCRDIM2_DEFAULT              (_MVP_LOOPCFG_ARRAY4INCRDIM2_DEFAULT << 30) /**< Shifted mode DEFAULT for MVP_LOOPCFG        */

/* Bit fields for MVP LOOPRST */
#define _MVP_LOOPRST_RESETVALUE                         0x00000000UL                                 /**< Default value for MVP_LOOPRST               */
#define _MVP_LOOPRST_MASK                               0x77777000UL                                 /**< Mask for MVP_LOOPRST                        */
#define MVP_LOOPRST_ARRAY0RESETDIM0                     (0x1UL << 12)                                /**< Reset Dimension 0                           */
#define _MVP_LOOPRST_ARRAY0RESETDIM0_SHIFT              12                                           /**< Shift value for MVP_ARRAY0RESETDIM0         */
#define _MVP_LOOPRST_ARRAY0RESETDIM0_MASK               0x1000UL                                     /**< Bit mask for MVP_ARRAY0RESETDIM0            */
#define _MVP_LOOPRST_ARRAY0RESETDIM0_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY0RESETDIM0_DEFAULT             (_MVP_LOOPRST_ARRAY0RESETDIM0_DEFAULT << 12) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY0RESETDIM1                     (0x1UL << 13)                                /**< Reset Dimension 1                           */
#define _MVP_LOOPRST_ARRAY0RESETDIM1_SHIFT              13                                           /**< Shift value for MVP_ARRAY0RESETDIM1         */
#define _MVP_LOOPRST_ARRAY0RESETDIM1_MASK               0x2000UL                                     /**< Bit mask for MVP_ARRAY0RESETDIM1            */
#define _MVP_LOOPRST_ARRAY0RESETDIM1_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY0RESETDIM1_DEFAULT             (_MVP_LOOPRST_ARRAY0RESETDIM1_DEFAULT << 13) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY0RESETDIM2                     (0x1UL << 14)                                /**< Reset Dimension 2                           */
#define _MVP_LOOPRST_ARRAY0RESETDIM2_SHIFT              14                                           /**< Shift value for MVP_ARRAY0RESETDIM2         */
#define _MVP_LOOPRST_ARRAY0RESETDIM2_MASK               0x4000UL                                     /**< Bit mask for MVP_ARRAY0RESETDIM2            */
#define _MVP_LOOPRST_ARRAY0RESETDIM2_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY0RESETDIM2_DEFAULT             (_MVP_LOOPRST_ARRAY0RESETDIM2_DEFAULT << 14) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY1RESETDIM0                     (0x1UL << 16)                                /**< Reset Dimension 0                           */
#define _MVP_LOOPRST_ARRAY1RESETDIM0_SHIFT              16                                           /**< Shift value for MVP_ARRAY1RESETDIM0         */
#define _MVP_LOOPRST_ARRAY1RESETDIM0_MASK               0x10000UL                                    /**< Bit mask for MVP_ARRAY1RESETDIM0            */
#define _MVP_LOOPRST_ARRAY1RESETDIM0_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY1RESETDIM0_DEFAULT             (_MVP_LOOPRST_ARRAY1RESETDIM0_DEFAULT << 16) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY1RESETDIM1                     (0x1UL << 17)                                /**< Reset Dimension 1                           */
#define _MVP_LOOPRST_ARRAY1RESETDIM1_SHIFT              17                                           /**< Shift value for MVP_ARRAY1RESETDIM1         */
#define _MVP_LOOPRST_ARRAY1RESETDIM1_MASK               0x20000UL                                    /**< Bit mask for MVP_ARRAY1RESETDIM1            */
#define _MVP_LOOPRST_ARRAY1RESETDIM1_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY1RESETDIM1_DEFAULT             (_MVP_LOOPRST_ARRAY1RESETDIM1_DEFAULT << 17) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY1RESETDIM2                     (0x1UL << 18)                                /**< Reset Dimension 2                           */
#define _MVP_LOOPRST_ARRAY1RESETDIM2_SHIFT              18                                           /**< Shift value for MVP_ARRAY1RESETDIM2         */
#define _MVP_LOOPRST_ARRAY1RESETDIM2_MASK               0x40000UL                                    /**< Bit mask for MVP_ARRAY1RESETDIM2            */
#define _MVP_LOOPRST_ARRAY1RESETDIM2_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY1RESETDIM2_DEFAULT             (_MVP_LOOPRST_ARRAY1RESETDIM2_DEFAULT << 18) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY2RESETDIM0                     (0x1UL << 20)                                /**< Reset Dimension 0                           */
#define _MVP_LOOPRST_ARRAY2RESETDIM0_SHIFT              20                                           /**< Shift value for MVP_ARRAY2RESETDIM0         */
#define _MVP_LOOPRST_ARRAY2RESETDIM0_MASK               0x100000UL                                   /**< Bit mask for MVP_ARRAY2RESETDIM0            */
#define _MVP_LOOPRST_ARRAY2RESETDIM0_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY2RESETDIM0_DEFAULT             (_MVP_LOOPRST_ARRAY2RESETDIM0_DEFAULT << 20) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY2RESETDIM1                     (0x1UL << 21)                                /**< Reset Dimension 1                           */
#define _MVP_LOOPRST_ARRAY2RESETDIM1_SHIFT              21                                           /**< Shift value for MVP_ARRAY2RESETDIM1         */
#define _MVP_LOOPRST_ARRAY2RESETDIM1_MASK               0x200000UL                                   /**< Bit mask for MVP_ARRAY2RESETDIM1            */
#define _MVP_LOOPRST_ARRAY2RESETDIM1_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY2RESETDIM1_DEFAULT             (_MVP_LOOPRST_ARRAY2RESETDIM1_DEFAULT << 21) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY2RESETDIM2                     (0x1UL << 22)                                /**< Reset Dimension 2                           */
#define _MVP_LOOPRST_ARRAY2RESETDIM2_SHIFT              22                                           /**< Shift value for MVP_ARRAY2RESETDIM2         */
#define _MVP_LOOPRST_ARRAY2RESETDIM2_MASK               0x400000UL                                   /**< Bit mask for MVP_ARRAY2RESETDIM2            */
#define _MVP_LOOPRST_ARRAY2RESETDIM2_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY2RESETDIM2_DEFAULT             (_MVP_LOOPRST_ARRAY2RESETDIM2_DEFAULT << 22) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY3RESETDIM0                     (0x1UL << 24)                                /**< Reset Dimension 0                           */
#define _MVP_LOOPRST_ARRAY3RESETDIM0_SHIFT              24                                           /**< Shift value for MVP_ARRAY3RESETDIM0         */
#define _MVP_LOOPRST_ARRAY3RESETDIM0_MASK               0x1000000UL                                  /**< Bit mask for MVP_ARRAY3RESETDIM0            */
#define _MVP_LOOPRST_ARRAY3RESETDIM0_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY3RESETDIM0_DEFAULT             (_MVP_LOOPRST_ARRAY3RESETDIM0_DEFAULT << 24) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY3RESETDIM1                     (0x1UL << 25)                                /**< Reset Dimension 1                           */
#define _MVP_LOOPRST_ARRAY3RESETDIM1_SHIFT              25                                           /**< Shift value for MVP_ARRAY3RESETDIM1         */
#define _MVP_LOOPRST_ARRAY3RESETDIM1_MASK               0x2000000UL                                  /**< Bit mask for MVP_ARRAY3RESETDIM1            */
#define _MVP_LOOPRST_ARRAY3RESETDIM1_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY3RESETDIM1_DEFAULT             (_MVP_LOOPRST_ARRAY3RESETDIM1_DEFAULT << 25) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY3RESETDIM2                     (0x1UL << 26)                                /**< Reset Dimension 2                           */
#define _MVP_LOOPRST_ARRAY3RESETDIM2_SHIFT              26                                           /**< Shift value for MVP_ARRAY3RESETDIM2         */
#define _MVP_LOOPRST_ARRAY3RESETDIM2_MASK               0x4000000UL                                  /**< Bit mask for MVP_ARRAY3RESETDIM2            */
#define _MVP_LOOPRST_ARRAY3RESETDIM2_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY3RESETDIM2_DEFAULT             (_MVP_LOOPRST_ARRAY3RESETDIM2_DEFAULT << 26) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY4RESETDIM0                     (0x1UL << 28)                                /**< Reset Dimension 0                           */
#define _MVP_LOOPRST_ARRAY4RESETDIM0_SHIFT              28                                           /**< Shift value for MVP_ARRAY4RESETDIM0         */
#define _MVP_LOOPRST_ARRAY4RESETDIM0_MASK               0x10000000UL                                 /**< Bit mask for MVP_ARRAY4RESETDIM0            */
#define _MVP_LOOPRST_ARRAY4RESETDIM0_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY4RESETDIM0_DEFAULT             (_MVP_LOOPRST_ARRAY4RESETDIM0_DEFAULT << 28) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY4RESETDIM1                     (0x1UL << 29)                                /**< Reset Dimension 1                           */
#define _MVP_LOOPRST_ARRAY4RESETDIM1_SHIFT              29                                           /**< Shift value for MVP_ARRAY4RESETDIM1         */
#define _MVP_LOOPRST_ARRAY4RESETDIM1_MASK               0x20000000UL                                 /**< Bit mask for MVP_ARRAY4RESETDIM1            */
#define _MVP_LOOPRST_ARRAY4RESETDIM1_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY4RESETDIM1_DEFAULT             (_MVP_LOOPRST_ARRAY4RESETDIM1_DEFAULT << 29) /**< Shifted mode DEFAULT for MVP_LOOPRST        */
#define MVP_LOOPRST_ARRAY4RESETDIM2                     (0x1UL << 30)                                /**< Reset Dimension 2                           */
#define _MVP_LOOPRST_ARRAY4RESETDIM2_SHIFT              30                                           /**< Shift value for MVP_ARRAY4RESETDIM2         */
#define _MVP_LOOPRST_ARRAY4RESETDIM2_MASK               0x40000000UL                                 /**< Bit mask for MVP_ARRAY4RESETDIM2            */
#define _MVP_LOOPRST_ARRAY4RESETDIM2_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for MVP_LOOPRST                */
#define MVP_LOOPRST_ARRAY4RESETDIM2_DEFAULT             (_MVP_LOOPRST_ARRAY4RESETDIM2_DEFAULT << 30) /**< Shifted mode DEFAULT for MVP_LOOPRST        */

/* Bit fields for MVP INSTRCFG0 */
#define _MVP_INSTRCFG0_RESETVALUE                       0x00000000UL                                    /**< Default value for MVP_INSTRCFG0             */
#define _MVP_INSTRCFG0_MASK                             0x70F7F7F7UL                                    /**< Mask for MVP_INSTRCFG0                      */
#define _MVP_INSTRCFG0_ALUIN0REGID_SHIFT                0                                               /**< Shift value for MVP_ALUIN0REGID             */
#define _MVP_INSTRCFG0_ALUIN0REGID_MASK                 0x7UL                                           /**< Bit mask for MVP_ALUIN0REGID                */
#define _MVP_INSTRCFG0_ALUIN0REGID_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN0REGID_DEFAULT               (_MVP_INSTRCFG0_ALUIN0REGID_DEFAULT << 0)       /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN0REALZERO                    (0x1UL << 4)                                    /**< Real Zero                                   */
#define _MVP_INSTRCFG0_ALUIN0REALZERO_SHIFT             4                                               /**< Shift value for MVP_ALUIN0REALZERO          */
#define _MVP_INSTRCFG0_ALUIN0REALZERO_MASK              0x10UL                                          /**< Bit mask for MVP_ALUIN0REALZERO             */
#define _MVP_INSTRCFG0_ALUIN0REALZERO_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN0REALZERO_DEFAULT            (_MVP_INSTRCFG0_ALUIN0REALZERO_DEFAULT << 4)    /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN0REALNEGATE                  (0x1UL << 5)                                    /**< Real Negate                                 */
#define _MVP_INSTRCFG0_ALUIN0REALNEGATE_SHIFT           5                                               /**< Shift value for MVP_ALUIN0REALNEGATE        */
#define _MVP_INSTRCFG0_ALUIN0REALNEGATE_MASK            0x20UL                                          /**< Bit mask for MVP_ALUIN0REALNEGATE           */
#define _MVP_INSTRCFG0_ALUIN0REALNEGATE_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN0REALNEGATE_DEFAULT          (_MVP_INSTRCFG0_ALUIN0REALNEGATE_DEFAULT << 5)  /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN0IMAGZERO                    (0x1UL << 6)                                    /**< Imaginary Not Zero                          */
#define _MVP_INSTRCFG0_ALUIN0IMAGZERO_SHIFT             6                                               /**< Shift value for MVP_ALUIN0IMAGZERO          */
#define _MVP_INSTRCFG0_ALUIN0IMAGZERO_MASK              0x40UL                                          /**< Bit mask for MVP_ALUIN0IMAGZERO             */
#define _MVP_INSTRCFG0_ALUIN0IMAGZERO_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN0IMAGZERO_DEFAULT            (_MVP_INSTRCFG0_ALUIN0IMAGZERO_DEFAULT << 6)    /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN0IMAGNEGATE                  (0x1UL << 7)                                    /**< Imaginary Negate                            */
#define _MVP_INSTRCFG0_ALUIN0IMAGNEGATE_SHIFT           7                                               /**< Shift value for MVP_ALUIN0IMAGNEGATE        */
#define _MVP_INSTRCFG0_ALUIN0IMAGNEGATE_MASK            0x80UL                                          /**< Bit mask for MVP_ALUIN0IMAGNEGATE           */
#define _MVP_INSTRCFG0_ALUIN0IMAGNEGATE_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN0IMAGNEGATE_DEFAULT          (_MVP_INSTRCFG0_ALUIN0IMAGNEGATE_DEFAULT << 7)  /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define _MVP_INSTRCFG0_ALUIN1REGID_SHIFT                8                                               /**< Shift value for MVP_ALUIN1REGID             */
#define _MVP_INSTRCFG0_ALUIN1REGID_MASK                 0x700UL                                         /**< Bit mask for MVP_ALUIN1REGID                */
#define _MVP_INSTRCFG0_ALUIN1REGID_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN1REGID_DEFAULT               (_MVP_INSTRCFG0_ALUIN1REGID_DEFAULT << 8)       /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN1REALZERO                    (0x1UL << 12)                                   /**< Real Zero                                   */
#define _MVP_INSTRCFG0_ALUIN1REALZERO_SHIFT             12                                              /**< Shift value for MVP_ALUIN1REALZERO          */
#define _MVP_INSTRCFG0_ALUIN1REALZERO_MASK              0x1000UL                                        /**< Bit mask for MVP_ALUIN1REALZERO             */
#define _MVP_INSTRCFG0_ALUIN1REALZERO_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN1REALZERO_DEFAULT            (_MVP_INSTRCFG0_ALUIN1REALZERO_DEFAULT << 12)   /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN1REALNEGATE                  (0x1UL << 13)                                   /**< Real Negate                                 */
#define _MVP_INSTRCFG0_ALUIN1REALNEGATE_SHIFT           13                                              /**< Shift value for MVP_ALUIN1REALNEGATE        */
#define _MVP_INSTRCFG0_ALUIN1REALNEGATE_MASK            0x2000UL                                        /**< Bit mask for MVP_ALUIN1REALNEGATE           */
#define _MVP_INSTRCFG0_ALUIN1REALNEGATE_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN1REALNEGATE_DEFAULT          (_MVP_INSTRCFG0_ALUIN1REALNEGATE_DEFAULT << 13) /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN1IMAGZERO                    (0x1UL << 14)                                   /**< Imaginary Not Zero                          */
#define _MVP_INSTRCFG0_ALUIN1IMAGZERO_SHIFT             14                                              /**< Shift value for MVP_ALUIN1IMAGZERO          */
#define _MVP_INSTRCFG0_ALUIN1IMAGZERO_MASK              0x4000UL                                        /**< Bit mask for MVP_ALUIN1IMAGZERO             */
#define _MVP_INSTRCFG0_ALUIN1IMAGZERO_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN1IMAGZERO_DEFAULT            (_MVP_INSTRCFG0_ALUIN1IMAGZERO_DEFAULT << 14)   /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN1IMAGNEGATE                  (0x1UL << 15)                                   /**< Imaginary Negate                            */
#define _MVP_INSTRCFG0_ALUIN1IMAGNEGATE_SHIFT           15                                              /**< Shift value for MVP_ALUIN1IMAGNEGATE        */
#define _MVP_INSTRCFG0_ALUIN1IMAGNEGATE_MASK            0x8000UL                                        /**< Bit mask for MVP_ALUIN1IMAGNEGATE           */
#define _MVP_INSTRCFG0_ALUIN1IMAGNEGATE_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN1IMAGNEGATE_DEFAULT          (_MVP_INSTRCFG0_ALUIN1IMAGNEGATE_DEFAULT << 15) /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define _MVP_INSTRCFG0_ALUIN2REGID_SHIFT                16                                              /**< Shift value for MVP_ALUIN2REGID             */
#define _MVP_INSTRCFG0_ALUIN2REGID_MASK                 0x70000UL                                       /**< Bit mask for MVP_ALUIN2REGID                */
#define _MVP_INSTRCFG0_ALUIN2REGID_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN2REGID_DEFAULT               (_MVP_INSTRCFG0_ALUIN2REGID_DEFAULT << 16)      /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN2REALZERO                    (0x1UL << 20)                                   /**< Real Zero                                   */
#define _MVP_INSTRCFG0_ALUIN2REALZERO_SHIFT             20                                              /**< Shift value for MVP_ALUIN2REALZERO          */
#define _MVP_INSTRCFG0_ALUIN2REALZERO_MASK              0x100000UL                                      /**< Bit mask for MVP_ALUIN2REALZERO             */
#define _MVP_INSTRCFG0_ALUIN2REALZERO_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN2REALZERO_DEFAULT            (_MVP_INSTRCFG0_ALUIN2REALZERO_DEFAULT << 20)   /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN2REALNEGATE                  (0x1UL << 21)                                   /**< Real Negate                                 */
#define _MVP_INSTRCFG0_ALUIN2REALNEGATE_SHIFT           21                                              /**< Shift value for MVP_ALUIN2REALNEGATE        */
#define _MVP_INSTRCFG0_ALUIN2REALNEGATE_MASK            0x200000UL                                      /**< Bit mask for MVP_ALUIN2REALNEGATE           */
#define _MVP_INSTRCFG0_ALUIN2REALNEGATE_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN2REALNEGATE_DEFAULT          (_MVP_INSTRCFG0_ALUIN2REALNEGATE_DEFAULT << 21) /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN2IMAGZERO                    (0x1UL << 22)                                   /**< Imaginary Not Zero                          */
#define _MVP_INSTRCFG0_ALUIN2IMAGZERO_SHIFT             22                                              /**< Shift value for MVP_ALUIN2IMAGZERO          */
#define _MVP_INSTRCFG0_ALUIN2IMAGZERO_MASK              0x400000UL                                      /**< Bit mask for MVP_ALUIN2IMAGZERO             */
#define _MVP_INSTRCFG0_ALUIN2IMAGZERO_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN2IMAGZERO_DEFAULT            (_MVP_INSTRCFG0_ALUIN2IMAGZERO_DEFAULT << 22)   /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define MVP_INSTRCFG0_ALUIN2IMAGNEGATE                  (0x1UL << 23)                                   /**< Imaginary Negate                            */
#define _MVP_INSTRCFG0_ALUIN2IMAGNEGATE_SHIFT           23                                              /**< Shift value for MVP_ALUIN2IMAGNEGATE        */
#define _MVP_INSTRCFG0_ALUIN2IMAGNEGATE_MASK            0x800000UL                                      /**< Bit mask for MVP_ALUIN2IMAGNEGATE           */
#define _MVP_INSTRCFG0_ALUIN2IMAGNEGATE_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUIN2IMAGNEGATE_DEFAULT          (_MVP_INSTRCFG0_ALUIN2IMAGNEGATE_DEFAULT << 23) /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */
#define _MVP_INSTRCFG0_ALUOUTREGID_SHIFT                28                                              /**< Shift value for MVP_ALUOUTREGID             */
#define _MVP_INSTRCFG0_ALUOUTREGID_MASK                 0x70000000UL                                    /**< Bit mask for MVP_ALUOUTREGID                */
#define _MVP_INSTRCFG0_ALUOUTREGID_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for MVP_INSTRCFG0              */
#define MVP_INSTRCFG0_ALUOUTREGID_DEFAULT               (_MVP_INSTRCFG0_ALUOUTREGID_DEFAULT << 28)      /**< Shifted mode DEFAULT for MVP_INSTRCFG0      */

/* Bit fields for MVP INSTRCFG1 */
#define _MVP_INSTRCFG1_RESETVALUE                       0x00000000UL                                         /**< Default value for MVP_INSTRCFG1             */
#define _MVP_INSTRCFG1_MASK                             0x3FFFFFFFUL                                         /**< Mask for MVP_INSTRCFG1                      */
#define _MVP_INSTRCFG1_ISTREAM0REGID_SHIFT              0                                                    /**< Shift value for MVP_ISTREAM0REGID           */
#define _MVP_INSTRCFG1_ISTREAM0REGID_MASK               0x7UL                                                /**< Bit mask for MVP_ISTREAM0REGID              */
#define _MVP_INSTRCFG1_ISTREAM0REGID_DEFAULT            0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM0REGID_DEFAULT             (_MVP_INSTRCFG1_ISTREAM0REGID_DEFAULT << 0)          /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_ISTREAM0LOAD                      (0x1UL << 3)                                         /**< Load register                               */
#define _MVP_INSTRCFG1_ISTREAM0LOAD_SHIFT               3                                                    /**< Shift value for MVP_ISTREAM0LOAD            */
#define _MVP_INSTRCFG1_ISTREAM0LOAD_MASK                0x8UL                                                /**< Bit mask for MVP_ISTREAM0LOAD               */
#define _MVP_INSTRCFG1_ISTREAM0LOAD_DEFAULT             0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM0LOAD_DEFAULT              (_MVP_INSTRCFG1_ISTREAM0LOAD_DEFAULT << 3)           /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYID_SHIFT            4                                                    /**< Shift value for MVP_ISTREAM0ARRAYID         */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYID_MASK             0x70UL                                               /**< Bit mask for MVP_ISTREAM0ARRAYID            */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYID_DEFAULT          0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM0ARRAYID_DEFAULT           (_MVP_INSTRCFG1_ISTREAM0ARRAYID_DEFAULT << 4)        /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM0             (0x1UL << 7)                                         /**< Increment Array Dimension 0                 */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM0_SHIFT      7                                                    /**< Shift value for MVP_ISTREAM0ARRAYINCRDIM0   */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM0_MASK       0x80UL                                               /**< Bit mask for MVP_ISTREAM0ARRAYINCRDIM0      */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM0_DEFAULT    0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM0_DEFAULT     (_MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM0_DEFAULT << 7)  /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM1             (0x1UL << 8)                                         /**< Increment Array Dimension 1                 */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM1_SHIFT      8                                                    /**< Shift value for MVP_ISTREAM0ARRAYINCRDIM1   */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM1_MASK       0x100UL                                              /**< Bit mask for MVP_ISTREAM0ARRAYINCRDIM1      */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM1_DEFAULT    0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM1_DEFAULT     (_MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM1_DEFAULT << 8)  /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM2             (0x1UL << 9)                                         /**< Increment Array Dimension 2                 */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM2_SHIFT      9                                                    /**< Shift value for MVP_ISTREAM0ARRAYINCRDIM2   */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM2_MASK       0x200UL                                              /**< Bit mask for MVP_ISTREAM0ARRAYINCRDIM2      */
#define _MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM2_DEFAULT    0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM2_DEFAULT     (_MVP_INSTRCFG1_ISTREAM0ARRAYINCRDIM2_DEFAULT << 9)  /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define _MVP_INSTRCFG1_ISTREAM1REGID_SHIFT              10                                                   /**< Shift value for MVP_ISTREAM1REGID           */
#define _MVP_INSTRCFG1_ISTREAM1REGID_MASK               0x1C00UL                                             /**< Bit mask for MVP_ISTREAM1REGID              */
#define _MVP_INSTRCFG1_ISTREAM1REGID_DEFAULT            0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM1REGID_DEFAULT             (_MVP_INSTRCFG1_ISTREAM1REGID_DEFAULT << 10)         /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_ISTREAM1LOAD                      (0x1UL << 13)                                        /**< Load register                               */
#define _MVP_INSTRCFG1_ISTREAM1LOAD_SHIFT               13                                                   /**< Shift value for MVP_ISTREAM1LOAD            */
#define _MVP_INSTRCFG1_ISTREAM1LOAD_MASK                0x2000UL                                             /**< Bit mask for MVP_ISTREAM1LOAD               */
#define _MVP_INSTRCFG1_ISTREAM1LOAD_DEFAULT             0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM1LOAD_DEFAULT              (_MVP_INSTRCFG1_ISTREAM1LOAD_DEFAULT << 13)          /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYID_SHIFT            14                                                   /**< Shift value for MVP_ISTREAM1ARRAYID         */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYID_MASK             0x1C000UL                                            /**< Bit mask for MVP_ISTREAM1ARRAYID            */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYID_DEFAULT          0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM1ARRAYID_DEFAULT           (_MVP_INSTRCFG1_ISTREAM1ARRAYID_DEFAULT << 14)       /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM0             (0x1UL << 17)                                        /**< Increment Array Dimension 0                 */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM0_SHIFT      17                                                   /**< Shift value for MVP_ISTREAM1ARRAYINCRDIM0   */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM0_MASK       0x20000UL                                            /**< Bit mask for MVP_ISTREAM1ARRAYINCRDIM0      */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM0_DEFAULT    0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM0_DEFAULT     (_MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM0_DEFAULT << 17) /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM1             (0x1UL << 18)                                        /**< Increment Array Dimension 1                 */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM1_SHIFT      18                                                   /**< Shift value for MVP_ISTREAM1ARRAYINCRDIM1   */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM1_MASK       0x40000UL                                            /**< Bit mask for MVP_ISTREAM1ARRAYINCRDIM1      */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM1_DEFAULT    0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM1_DEFAULT     (_MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM1_DEFAULT << 18) /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM2             (0x1UL << 19)                                        /**< Increment Array Dimension 2                 */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM2_SHIFT      19                                                   /**< Shift value for MVP_ISTREAM1ARRAYINCRDIM2   */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM2_MASK       0x80000UL                                            /**< Bit mask for MVP_ISTREAM1ARRAYINCRDIM2      */
#define _MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM2_DEFAULT    0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM2_DEFAULT     (_MVP_INSTRCFG1_ISTREAM1ARRAYINCRDIM2_DEFAULT << 19) /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define _MVP_INSTRCFG1_OSTREAMREGID_SHIFT               20                                                   /**< Shift value for MVP_OSTREAMREGID            */
#define _MVP_INSTRCFG1_OSTREAMREGID_MASK                0x700000UL                                           /**< Bit mask for MVP_OSTREAMREGID               */
#define _MVP_INSTRCFG1_OSTREAMREGID_DEFAULT             0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_OSTREAMREGID_DEFAULT              (_MVP_INSTRCFG1_OSTREAMREGID_DEFAULT << 20)          /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_OSTREAMSTORE                      (0x1UL << 23)                                        /**< Store to Register                           */
#define _MVP_INSTRCFG1_OSTREAMSTORE_SHIFT               23                                                   /**< Shift value for MVP_OSTREAMSTORE            */
#define _MVP_INSTRCFG1_OSTREAMSTORE_MASK                0x800000UL                                           /**< Bit mask for MVP_OSTREAMSTORE               */
#define _MVP_INSTRCFG1_OSTREAMSTORE_DEFAULT             0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_OSTREAMSTORE_DEFAULT              (_MVP_INSTRCFG1_OSTREAMSTORE_DEFAULT << 23)          /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define _MVP_INSTRCFG1_OSTREAMARRAYID_SHIFT             24                                                   /**< Shift value for MVP_OSTREAMARRAYID          */
#define _MVP_INSTRCFG1_OSTREAMARRAYID_MASK              0x7000000UL                                          /**< Bit mask for MVP_OSTREAMARRAYID             */
#define _MVP_INSTRCFG1_OSTREAMARRAYID_DEFAULT           0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_OSTREAMARRAYID_DEFAULT            (_MVP_INSTRCFG1_OSTREAMARRAYID_DEFAULT << 24)        /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_OSTREAMARRAYINCRDIM0              (0x1UL << 27)                                        /**< Increment Array Dimension 0                 */
#define _MVP_INSTRCFG1_OSTREAMARRAYINCRDIM0_SHIFT       27                                                   /**< Shift value for MVP_OSTREAMARRAYINCRDIM0    */
#define _MVP_INSTRCFG1_OSTREAMARRAYINCRDIM0_MASK        0x8000000UL                                          /**< Bit mask for MVP_OSTREAMARRAYINCRDIM0       */
#define _MVP_INSTRCFG1_OSTREAMARRAYINCRDIM0_DEFAULT     0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_OSTREAMARRAYINCRDIM0_DEFAULT      (_MVP_INSTRCFG1_OSTREAMARRAYINCRDIM0_DEFAULT << 27)  /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_OSTREAMARRAYINCRDIM1              (0x1UL << 28)                                        /**< Increment Array Dimension 1                 */
#define _MVP_INSTRCFG1_OSTREAMARRAYINCRDIM1_SHIFT       28                                                   /**< Shift value for MVP_OSTREAMARRAYINCRDIM1    */
#define _MVP_INSTRCFG1_OSTREAMARRAYINCRDIM1_MASK        0x10000000UL                                         /**< Bit mask for MVP_OSTREAMARRAYINCRDIM1       */
#define _MVP_INSTRCFG1_OSTREAMARRAYINCRDIM1_DEFAULT     0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_OSTREAMARRAYINCRDIM1_DEFAULT      (_MVP_INSTRCFG1_OSTREAMARRAYINCRDIM1_DEFAULT << 28)  /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */
#define MVP_INSTRCFG1_OSTREAMARRAYINCRDIM2              (0x1UL << 29)                                        /**< Increment Array Dimension 2                 */
#define _MVP_INSTRCFG1_OSTREAMARRAYINCRDIM2_SHIFT       29                                                   /**< Shift value for MVP_OSTREAMARRAYINCRDIM2    */
#define _MVP_INSTRCFG1_OSTREAMARRAYINCRDIM2_MASK        0x20000000UL                                         /**< Bit mask for MVP_OSTREAMARRAYINCRDIM2       */
#define _MVP_INSTRCFG1_OSTREAMARRAYINCRDIM2_DEFAULT     0x00000000UL                                         /**< Mode DEFAULT for MVP_INSTRCFG1              */
#define MVP_INSTRCFG1_OSTREAMARRAYINCRDIM2_DEFAULT      (_MVP_INSTRCFG1_OSTREAMARRAYINCRDIM2_DEFAULT << 29)  /**< Shifted mode DEFAULT for MVP_INSTRCFG1      */

/* Bit fields for MVP INSTRCFG2 */
#define _MVP_INSTRCFG2_RESETVALUE                       0x00000000UL                              /**< Default value for MVP_INSTRCFG2             */
#define _MVP_INSTRCFG2_MASK                             0x9FF0FFFFUL                              /**< Mask for MVP_INSTRCFG2                      */
#define MVP_INSTRCFG2_LOOP0BEGIN                        (0x1UL << 0)                              /**< Loop Begin                                  */
#define _MVP_INSTRCFG2_LOOP0BEGIN_SHIFT                 0                                         /**< Shift value for MVP_LOOP0BEGIN              */
#define _MVP_INSTRCFG2_LOOP0BEGIN_MASK                  0x1UL                                     /**< Bit mask for MVP_LOOP0BEGIN                 */
#define _MVP_INSTRCFG2_LOOP0BEGIN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP0BEGIN_DEFAULT                (_MVP_INSTRCFG2_LOOP0BEGIN_DEFAULT << 0)  /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP0END                          (0x1UL << 1)                              /**< Loop End                                    */
#define _MVP_INSTRCFG2_LOOP0END_SHIFT                   1                                         /**< Shift value for MVP_LOOP0END                */
#define _MVP_INSTRCFG2_LOOP0END_MASK                    0x2UL                                     /**< Bit mask for MVP_LOOP0END                   */
#define _MVP_INSTRCFG2_LOOP0END_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP0END_DEFAULT                  (_MVP_INSTRCFG2_LOOP0END_DEFAULT << 1)    /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP1BEGIN                        (0x1UL << 2)                              /**< Loop Begin                                  */
#define _MVP_INSTRCFG2_LOOP1BEGIN_SHIFT                 2                                         /**< Shift value for MVP_LOOP1BEGIN              */
#define _MVP_INSTRCFG2_LOOP1BEGIN_MASK                  0x4UL                                     /**< Bit mask for MVP_LOOP1BEGIN                 */
#define _MVP_INSTRCFG2_LOOP1BEGIN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP1BEGIN_DEFAULT                (_MVP_INSTRCFG2_LOOP1BEGIN_DEFAULT << 2)  /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP1END                          (0x1UL << 3)                              /**< Loop End                                    */
#define _MVP_INSTRCFG2_LOOP1END_SHIFT                   3                                         /**< Shift value for MVP_LOOP1END                */
#define _MVP_INSTRCFG2_LOOP1END_MASK                    0x8UL                                     /**< Bit mask for MVP_LOOP1END                   */
#define _MVP_INSTRCFG2_LOOP1END_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP1END_DEFAULT                  (_MVP_INSTRCFG2_LOOP1END_DEFAULT << 3)    /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP2BEGIN                        (0x1UL << 4)                              /**< Loop Begin                                  */
#define _MVP_INSTRCFG2_LOOP2BEGIN_SHIFT                 4                                         /**< Shift value for MVP_LOOP2BEGIN              */
#define _MVP_INSTRCFG2_LOOP2BEGIN_MASK                  0x10UL                                    /**< Bit mask for MVP_LOOP2BEGIN                 */
#define _MVP_INSTRCFG2_LOOP2BEGIN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP2BEGIN_DEFAULT                (_MVP_INSTRCFG2_LOOP2BEGIN_DEFAULT << 4)  /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP2END                          (0x1UL << 5)                              /**< Loop End                                    */
#define _MVP_INSTRCFG2_LOOP2END_SHIFT                   5                                         /**< Shift value for MVP_LOOP2END                */
#define _MVP_INSTRCFG2_LOOP2END_MASK                    0x20UL                                    /**< Bit mask for MVP_LOOP2END                   */
#define _MVP_INSTRCFG2_LOOP2END_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP2END_DEFAULT                  (_MVP_INSTRCFG2_LOOP2END_DEFAULT << 5)    /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP3BEGIN                        (0x1UL << 6)                              /**< Loop Begin                                  */
#define _MVP_INSTRCFG2_LOOP3BEGIN_SHIFT                 6                                         /**< Shift value for MVP_LOOP3BEGIN              */
#define _MVP_INSTRCFG2_LOOP3BEGIN_MASK                  0x40UL                                    /**< Bit mask for MVP_LOOP3BEGIN                 */
#define _MVP_INSTRCFG2_LOOP3BEGIN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP3BEGIN_DEFAULT                (_MVP_INSTRCFG2_LOOP3BEGIN_DEFAULT << 6)  /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP3END                          (0x1UL << 7)                              /**< Loop End                                    */
#define _MVP_INSTRCFG2_LOOP3END_SHIFT                   7                                         /**< Shift value for MVP_LOOP3END                */
#define _MVP_INSTRCFG2_LOOP3END_MASK                    0x80UL                                    /**< Bit mask for MVP_LOOP3END                   */
#define _MVP_INSTRCFG2_LOOP3END_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP3END_DEFAULT                  (_MVP_INSTRCFG2_LOOP3END_DEFAULT << 7)    /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP4BEGIN                        (0x1UL << 8)                              /**< Loop Begin                                  */
#define _MVP_INSTRCFG2_LOOP4BEGIN_SHIFT                 8                                         /**< Shift value for MVP_LOOP4BEGIN              */
#define _MVP_INSTRCFG2_LOOP4BEGIN_MASK                  0x100UL                                   /**< Bit mask for MVP_LOOP4BEGIN                 */
#define _MVP_INSTRCFG2_LOOP4BEGIN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP4BEGIN_DEFAULT                (_MVP_INSTRCFG2_LOOP4BEGIN_DEFAULT << 8)  /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP4END                          (0x1UL << 9)                              /**< Loop End                                    */
#define _MVP_INSTRCFG2_LOOP4END_SHIFT                   9                                         /**< Shift value for MVP_LOOP4END                */
#define _MVP_INSTRCFG2_LOOP4END_MASK                    0x200UL                                   /**< Bit mask for MVP_LOOP4END                   */
#define _MVP_INSTRCFG2_LOOP4END_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP4END_DEFAULT                  (_MVP_INSTRCFG2_LOOP4END_DEFAULT << 9)    /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP5BEGIN                        (0x1UL << 10)                             /**< Loop Begin                                  */
#define _MVP_INSTRCFG2_LOOP5BEGIN_SHIFT                 10                                        /**< Shift value for MVP_LOOP5BEGIN              */
#define _MVP_INSTRCFG2_LOOP5BEGIN_MASK                  0x400UL                                   /**< Bit mask for MVP_LOOP5BEGIN                 */
#define _MVP_INSTRCFG2_LOOP5BEGIN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP5BEGIN_DEFAULT                (_MVP_INSTRCFG2_LOOP5BEGIN_DEFAULT << 10) /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP5END                          (0x1UL << 11)                             /**< Loop End                                    */
#define _MVP_INSTRCFG2_LOOP5END_SHIFT                   11                                        /**< Shift value for MVP_LOOP5END                */
#define _MVP_INSTRCFG2_LOOP5END_MASK                    0x800UL                                   /**< Bit mask for MVP_LOOP5END                   */
#define _MVP_INSTRCFG2_LOOP5END_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP5END_DEFAULT                  (_MVP_INSTRCFG2_LOOP5END_DEFAULT << 11)   /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP6BEGIN                        (0x1UL << 12)                             /**< Loop Begin                                  */
#define _MVP_INSTRCFG2_LOOP6BEGIN_SHIFT                 12                                        /**< Shift value for MVP_LOOP6BEGIN              */
#define _MVP_INSTRCFG2_LOOP6BEGIN_MASK                  0x1000UL                                  /**< Bit mask for MVP_LOOP6BEGIN                 */
#define _MVP_INSTRCFG2_LOOP6BEGIN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP6BEGIN_DEFAULT                (_MVP_INSTRCFG2_LOOP6BEGIN_DEFAULT << 12) /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP6END                          (0x1UL << 13)                             /**< Loop End                                    */
#define _MVP_INSTRCFG2_LOOP6END_SHIFT                   13                                        /**< Shift value for MVP_LOOP6END                */
#define _MVP_INSTRCFG2_LOOP6END_MASK                    0x2000UL                                  /**< Bit mask for MVP_LOOP6END                   */
#define _MVP_INSTRCFG2_LOOP6END_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP6END_DEFAULT                  (_MVP_INSTRCFG2_LOOP6END_DEFAULT << 13)   /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP7BEGIN                        (0x1UL << 14)                             /**< Loop Begin                                  */
#define _MVP_INSTRCFG2_LOOP7BEGIN_SHIFT                 14                                        /**< Shift value for MVP_LOOP7BEGIN              */
#define _MVP_INSTRCFG2_LOOP7BEGIN_MASK                  0x4000UL                                  /**< Bit mask for MVP_LOOP7BEGIN                 */
#define _MVP_INSTRCFG2_LOOP7BEGIN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP7BEGIN_DEFAULT                (_MVP_INSTRCFG2_LOOP7BEGIN_DEFAULT << 14) /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_LOOP7END                          (0x1UL << 15)                             /**< Loop End                                    */
#define _MVP_INSTRCFG2_LOOP7END_SHIFT                   15                                        /**< Shift value for MVP_LOOP7END                */
#define _MVP_INSTRCFG2_LOOP7END_MASK                    0x8000UL                                  /**< Bit mask for MVP_LOOP7END                   */
#define _MVP_INSTRCFG2_LOOP7END_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_LOOP7END_DEFAULT                  (_MVP_INSTRCFG2_LOOP7END_DEFAULT << 15)   /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define _MVP_INSTRCFG2_ALUOP_SHIFT                      20                                        /**< Shift value for MVP_ALUOP                   */
#define _MVP_INSTRCFG2_ALUOP_MASK                       0x1FF00000UL                              /**< Bit mask for MVP_ALUOP                      */
#define _MVP_INSTRCFG2_ALUOP_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define _MVP_INSTRCFG2_ALUOP_NOOP                       0x00000000UL                              /**< Mode NOOP for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_CLEAR                      0x00000001UL                              /**< Mode CLEAR for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_COPY                       0x00000041UL                              /**< Mode COPY for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_SWAP                       0x00000042UL                              /**< Mode SWAP for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_DBL                        0x00000043UL                              /**< Mode DBL for MVP_INSTRCFG2                  */
#define _MVP_INSTRCFG2_ALUOP_FANA                       0x00000044UL                              /**< Mode FANA for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_FANB                       0x00000045UL                              /**< Mode FANB for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_RELU2                      0x00000046UL                              /**< Mode RELU2 for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_NRELU2                     0x00000047UL                              /**< Mode NRELU2 for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_INC2                       0x00000048UL                              /**< Mode INC2 for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_DEC2                       0x00000049UL                              /**< Mode DEC2 for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_ADDR                       0x0000004AUL                              /**< Mode ADDR for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_MAX                        0x0000004BUL                              /**< Mode MAX for MVP_INSTRCFG2                  */
#define _MVP_INSTRCFG2_ALUOP_MIN                        0x0000004CUL                              /**< Mode MIN for MVP_INSTRCFG2                  */
#define _MVP_INSTRCFG2_ALUOP_RSQR2B                     0x00000124UL                              /**< Mode RSQR2B for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_ADDC                       0x0000014EUL                              /**< Mode ADDC for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_MAX2A                      0x00000153UL                              /**< Mode MAX2A for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_MIN2A                      0x00000154UL                              /**< Mode MIN2A for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_XREALC2                    0x0000015EUL                              /**< Mode XREALC2 for MVP_INSTRCFG2              */
#define _MVP_INSTRCFG2_ALUOP_XIMAGC2                    0x0000015FUL                              /**< Mode XIMAGC2 for MVP_INSTRCFG2              */
#define _MVP_INSTRCFG2_ALUOP_ADDR2B                     0x00000161UL                              /**< Mode ADDR2B for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_MAX2B                      0x00000162UL                              /**< Mode MAX2B for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_MIN2B                      0x00000163UL                              /**< Mode MIN2B for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_MULC                       0x0000018DUL                              /**< Mode MULC for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_MULR2A                     0x00000197UL                              /**< Mode MULR2A for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_MULR2B                     0x00000198UL                              /**< Mode MULR2B for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_ADDR4                      0x0000019AUL                              /**< Mode ADDR4 for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_MAX4                       0x0000019BUL                              /**< Mode MAX4 for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_MIN4                       0x0000019CUL                              /**< Mode MIN4 for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_SQRMAGC2                   0x0000019DUL                              /**< Mode SQRMAGC2 for MVP_INSTRCFG2             */
#define _MVP_INSTRCFG2_ALUOP_PRELU2B                    0x000001A0UL                              /**< Mode PRELU2B for MVP_INSTRCFG2              */
#define _MVP_INSTRCFG2_ALUOP_MACC                       0x000001CDUL                              /**< Mode MACC for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_AACC                       0x000001CEUL                              /**< Mode AACC for MVP_INSTRCFG2                 */
#define _MVP_INSTRCFG2_ALUOP_ELU2A                      0x000001CFUL                              /**< Mode ELU2A for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_ELU2B                      0x000001D0UL                              /**< Mode ELU2B for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_IFR2A                      0x000001D1UL                              /**< Mode IFR2A for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_IFR2B                      0x000001D2UL                              /**< Mode IFR2B for MVP_INSTRCFG2                */
#define _MVP_INSTRCFG2_ALUOP_MAXAC2                     0x000001D3UL                              /**< Mode MAXAC2 for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_MINAC2                     0x000001D4UL                              /**< Mode MINAC2 for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_CLIP2A                     0x000001D5UL                              /**< Mode CLIP2A for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_CLIP2B                     0x000001D6UL                              /**< Mode CLIP2B for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_MACR2A                     0x000001D7UL                              /**< Mode MACR2A for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_MACR2B                     0x000001D8UL                              /**< Mode MACR2B for MVP_INSTRCFG2               */
#define _MVP_INSTRCFG2_ALUOP_IFC                        0x000001D9UL                              /**< Mode IFC for MVP_INSTRCFG2                  */
#define MVP_INSTRCFG2_ALUOP_DEFAULT                     (_MVP_INSTRCFG2_ALUOP_DEFAULT << 20)      /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_ALUOP_NOOP                        (_MVP_INSTRCFG2_ALUOP_NOOP << 20)         /**< Shifted mode NOOP for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_CLEAR                       (_MVP_INSTRCFG2_ALUOP_CLEAR << 20)        /**< Shifted mode CLEAR for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_COPY                        (_MVP_INSTRCFG2_ALUOP_COPY << 20)         /**< Shifted mode COPY for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_SWAP                        (_MVP_INSTRCFG2_ALUOP_SWAP << 20)         /**< Shifted mode SWAP for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_DBL                         (_MVP_INSTRCFG2_ALUOP_DBL << 20)          /**< Shifted mode DBL for MVP_INSTRCFG2          */
#define MVP_INSTRCFG2_ALUOP_FANA                        (_MVP_INSTRCFG2_ALUOP_FANA << 20)         /**< Shifted mode FANA for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_FANB                        (_MVP_INSTRCFG2_ALUOP_FANB << 20)         /**< Shifted mode FANB for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_RELU2                       (_MVP_INSTRCFG2_ALUOP_RELU2 << 20)        /**< Shifted mode RELU2 for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_NRELU2                      (_MVP_INSTRCFG2_ALUOP_NRELU2 << 20)       /**< Shifted mode NRELU2 for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_INC2                        (_MVP_INSTRCFG2_ALUOP_INC2 << 20)         /**< Shifted mode INC2 for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_DEC2                        (_MVP_INSTRCFG2_ALUOP_DEC2 << 20)         /**< Shifted mode DEC2 for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_ADDR                        (_MVP_INSTRCFG2_ALUOP_ADDR << 20)         /**< Shifted mode ADDR for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_MAX                         (_MVP_INSTRCFG2_ALUOP_MAX << 20)          /**< Shifted mode MAX for MVP_INSTRCFG2          */
#define MVP_INSTRCFG2_ALUOP_MIN                         (_MVP_INSTRCFG2_ALUOP_MIN << 20)          /**< Shifted mode MIN for MVP_INSTRCFG2          */
#define MVP_INSTRCFG2_ALUOP_RSQR2B                      (_MVP_INSTRCFG2_ALUOP_RSQR2B << 20)       /**< Shifted mode RSQR2B for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_ADDC                        (_MVP_INSTRCFG2_ALUOP_ADDC << 20)         /**< Shifted mode ADDC for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_MAX2A                       (_MVP_INSTRCFG2_ALUOP_MAX2A << 20)        /**< Shifted mode MAX2A for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_MIN2A                       (_MVP_INSTRCFG2_ALUOP_MIN2A << 20)        /**< Shifted mode MIN2A for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_XREALC2                     (_MVP_INSTRCFG2_ALUOP_XREALC2 << 20)      /**< Shifted mode XREALC2 for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_ALUOP_XIMAGC2                     (_MVP_INSTRCFG2_ALUOP_XIMAGC2 << 20)      /**< Shifted mode XIMAGC2 for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_ALUOP_ADDR2B                      (_MVP_INSTRCFG2_ALUOP_ADDR2B << 20)       /**< Shifted mode ADDR2B for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_MAX2B                       (_MVP_INSTRCFG2_ALUOP_MAX2B << 20)        /**< Shifted mode MAX2B for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_MIN2B                       (_MVP_INSTRCFG2_ALUOP_MIN2B << 20)        /**< Shifted mode MIN2B for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_MULC                        (_MVP_INSTRCFG2_ALUOP_MULC << 20)         /**< Shifted mode MULC for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_MULR2A                      (_MVP_INSTRCFG2_ALUOP_MULR2A << 20)       /**< Shifted mode MULR2A for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_MULR2B                      (_MVP_INSTRCFG2_ALUOP_MULR2B << 20)       /**< Shifted mode MULR2B for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_ADDR4                       (_MVP_INSTRCFG2_ALUOP_ADDR4 << 20)        /**< Shifted mode ADDR4 for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_MAX4                        (_MVP_INSTRCFG2_ALUOP_MAX4 << 20)         /**< Shifted mode MAX4 for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_MIN4                        (_MVP_INSTRCFG2_ALUOP_MIN4 << 20)         /**< Shifted mode MIN4 for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_SQRMAGC2                    (_MVP_INSTRCFG2_ALUOP_SQRMAGC2 << 20)     /**< Shifted mode SQRMAGC2 for MVP_INSTRCFG2     */
#define MVP_INSTRCFG2_ALUOP_PRELU2B                     (_MVP_INSTRCFG2_ALUOP_PRELU2B << 20)      /**< Shifted mode PRELU2B for MVP_INSTRCFG2      */
#define MVP_INSTRCFG2_ALUOP_MACC                        (_MVP_INSTRCFG2_ALUOP_MACC << 20)         /**< Shifted mode MACC for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_AACC                        (_MVP_INSTRCFG2_ALUOP_AACC << 20)         /**< Shifted mode AACC for MVP_INSTRCFG2         */
#define MVP_INSTRCFG2_ALUOP_ELU2A                       (_MVP_INSTRCFG2_ALUOP_ELU2A << 20)        /**< Shifted mode ELU2A for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_ELU2B                       (_MVP_INSTRCFG2_ALUOP_ELU2B << 20)        /**< Shifted mode ELU2B for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_IFR2A                       (_MVP_INSTRCFG2_ALUOP_IFR2A << 20)        /**< Shifted mode IFR2A for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_IFR2B                       (_MVP_INSTRCFG2_ALUOP_IFR2B << 20)        /**< Shifted mode IFR2B for MVP_INSTRCFG2        */
#define MVP_INSTRCFG2_ALUOP_MAXAC2                      (_MVP_INSTRCFG2_ALUOP_MAXAC2 << 20)       /**< Shifted mode MAXAC2 for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_MINAC2                      (_MVP_INSTRCFG2_ALUOP_MINAC2 << 20)       /**< Shifted mode MINAC2 for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_CLIP2A                      (_MVP_INSTRCFG2_ALUOP_CLIP2A << 20)       /**< Shifted mode CLIP2A for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_CLIP2B                      (_MVP_INSTRCFG2_ALUOP_CLIP2B << 20)       /**< Shifted mode CLIP2B for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_MACR2A                      (_MVP_INSTRCFG2_ALUOP_MACR2A << 20)       /**< Shifted mode MACR2A for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_MACR2B                      (_MVP_INSTRCFG2_ALUOP_MACR2B << 20)       /**< Shifted mode MACR2B for MVP_INSTRCFG2       */
#define MVP_INSTRCFG2_ALUOP_IFC                         (_MVP_INSTRCFG2_ALUOP_IFC << 20)          /**< Shifted mode IFC for MVP_INSTRCFG2          */
#define MVP_INSTRCFG2_ENDPROG                           (0x1UL << 31)                             /**< End of Program                              */
#define _MVP_INSTRCFG2_ENDPROG_SHIFT                    31                                        /**< Shift value for MVP_ENDPROG                 */
#define _MVP_INSTRCFG2_ENDPROG_MASK                     0x80000000UL                              /**< Bit mask for MVP_ENDPROG                    */
#define _MVP_INSTRCFG2_ENDPROG_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for MVP_INSTRCFG2              */
#define MVP_INSTRCFG2_ENDPROG_DEFAULT                   (_MVP_INSTRCFG2_ENDPROG_DEFAULT << 31)    /**< Shifted mode DEFAULT for MVP_INSTRCFG2      */

/* Bit fields for MVP CMD */
#define _MVP_CMD_RESETVALUE                             0x00000000UL                    /**< Default value for MVP_CMD                   */
#define _MVP_CMD_MASK                                   0x0000000FUL                    /**< Mask for MVP_CMD                            */
#define MVP_CMD_START                                   (0x1UL << 0)                    /**< Start Command                               */
#define _MVP_CMD_START_SHIFT                            0                               /**< Shift value for MVP_START                   */
#define _MVP_CMD_START_MASK                             0x1UL                           /**< Bit mask for MVP_START                      */
#define _MVP_CMD_START_DEFAULT                          0x00000000UL                    /**< Mode DEFAULT for MVP_CMD                    */
#define MVP_CMD_START_DEFAULT                           (_MVP_CMD_START_DEFAULT << 0)   /**< Shifted mode DEFAULT for MVP_CMD            */
#define MVP_CMD_HALT                                    (0x1UL << 1)                    /**< Halt Command                                */
#define _MVP_CMD_HALT_SHIFT                             1                               /**< Shift value for MVP_HALT                    */
#define _MVP_CMD_HALT_MASK                              0x2UL                           /**< Bit mask for MVP_HALT                       */
#define _MVP_CMD_HALT_DEFAULT                           0x00000000UL                    /**< Mode DEFAULT for MVP_CMD                    */
#define MVP_CMD_HALT_DEFAULT                            (_MVP_CMD_HALT_DEFAULT << 1)    /**< Shifted mode DEFAULT for MVP_CMD            */
#define MVP_CMD_STEP                                    (0x1UL << 2)                    /**< Step Command                                */
#define _MVP_CMD_STEP_SHIFT                             2                               /**< Shift value for MVP_STEP                    */
#define _MVP_CMD_STEP_MASK                              0x4UL                           /**< Bit mask for MVP_STEP                       */
#define _MVP_CMD_STEP_DEFAULT                           0x00000000UL                    /**< Mode DEFAULT for MVP_CMD                    */
#define MVP_CMD_STEP_DEFAULT                            (_MVP_CMD_STEP_DEFAULT << 2)    /**< Shifted mode DEFAULT for MVP_CMD            */
#define MVP_CMD_INIT                                    (0x1UL << 3)                    /**< Initialization Command/Qualifier            */
#define _MVP_CMD_INIT_SHIFT                             3                               /**< Shift value for MVP_INIT                    */
#define _MVP_CMD_INIT_MASK                              0x8UL                           /**< Bit mask for MVP_INIT                       */
#define _MVP_CMD_INIT_DEFAULT                           0x00000000UL                    /**< Mode DEFAULT for MVP_CMD                    */
#define MVP_CMD_INIT_DEFAULT                            (_MVP_CMD_INIT_DEFAULT << 3)    /**< Shifted mode DEFAULT for MVP_CMD            */

/* Bit fields for MVP DEBUGEN */
#define _MVP_DEBUGEN_RESETVALUE                         0x00000000UL                                     /**< Default value for MVP_DEBUGEN               */
#define _MVP_DEBUGEN_MASK                               0x7003FDFEUL                                     /**< Mask for MVP_DEBUGEN                        */
#define MVP_DEBUGEN_BKPTLOOP0DONE                       (0x1UL << 1)                                     /**< Enable Breakpoint on Loop Done              */
#define _MVP_DEBUGEN_BKPTLOOP0DONE_SHIFT                1                                                /**< Shift value for MVP_BKPTLOOP0DONE           */
#define _MVP_DEBUGEN_BKPTLOOP0DONE_MASK                 0x2UL                                            /**< Bit mask for MVP_BKPTLOOP0DONE              */
#define _MVP_DEBUGEN_BKPTLOOP0DONE_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTLOOP0DONE_DEFAULT               (_MVP_DEBUGEN_BKPTLOOP0DONE_DEFAULT << 1)        /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTLOOP1DONE                       (0x1UL << 2)                                     /**< Enable Breakpoint on Loop Done              */
#define _MVP_DEBUGEN_BKPTLOOP1DONE_SHIFT                2                                                /**< Shift value for MVP_BKPTLOOP1DONE           */
#define _MVP_DEBUGEN_BKPTLOOP1DONE_MASK                 0x4UL                                            /**< Bit mask for MVP_BKPTLOOP1DONE              */
#define _MVP_DEBUGEN_BKPTLOOP1DONE_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTLOOP1DONE_DEFAULT               (_MVP_DEBUGEN_BKPTLOOP1DONE_DEFAULT << 2)        /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTLOOP2DONE                       (0x1UL << 3)                                     /**< Enable Breakpoint on Loop Done              */
#define _MVP_DEBUGEN_BKPTLOOP2DONE_SHIFT                3                                                /**< Shift value for MVP_BKPTLOOP2DONE           */
#define _MVP_DEBUGEN_BKPTLOOP2DONE_MASK                 0x8UL                                            /**< Bit mask for MVP_BKPTLOOP2DONE              */
#define _MVP_DEBUGEN_BKPTLOOP2DONE_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTLOOP2DONE_DEFAULT               (_MVP_DEBUGEN_BKPTLOOP2DONE_DEFAULT << 3)        /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTLOOP3DONE                       (0x1UL << 4)                                     /**< Enable Breakpoint on Loop Done              */
#define _MVP_DEBUGEN_BKPTLOOP3DONE_SHIFT                4                                                /**< Shift value for MVP_BKPTLOOP3DONE           */
#define _MVP_DEBUGEN_BKPTLOOP3DONE_MASK                 0x10UL                                           /**< Bit mask for MVP_BKPTLOOP3DONE              */
#define _MVP_DEBUGEN_BKPTLOOP3DONE_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTLOOP3DONE_DEFAULT               (_MVP_DEBUGEN_BKPTLOOP3DONE_DEFAULT << 4)        /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTLOOP4DONE                       (0x1UL << 5)                                     /**< Enable Breakpoint on Loop Done              */
#define _MVP_DEBUGEN_BKPTLOOP4DONE_SHIFT                5                                                /**< Shift value for MVP_BKPTLOOP4DONE           */
#define _MVP_DEBUGEN_BKPTLOOP4DONE_MASK                 0x20UL                                           /**< Bit mask for MVP_BKPTLOOP4DONE              */
#define _MVP_DEBUGEN_BKPTLOOP4DONE_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTLOOP4DONE_DEFAULT               (_MVP_DEBUGEN_BKPTLOOP4DONE_DEFAULT << 5)        /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTLOOP5DONE                       (0x1UL << 6)                                     /**< Enable Breakpoint on Loop Done              */
#define _MVP_DEBUGEN_BKPTLOOP5DONE_SHIFT                6                                                /**< Shift value for MVP_BKPTLOOP5DONE           */
#define _MVP_DEBUGEN_BKPTLOOP5DONE_MASK                 0x40UL                                           /**< Bit mask for MVP_BKPTLOOP5DONE              */
#define _MVP_DEBUGEN_BKPTLOOP5DONE_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTLOOP5DONE_DEFAULT               (_MVP_DEBUGEN_BKPTLOOP5DONE_DEFAULT << 6)        /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTLOOP6DONE                       (0x1UL << 7)                                     /**< Enable Breakpoint on Loop Done              */
#define _MVP_DEBUGEN_BKPTLOOP6DONE_SHIFT                7                                                /**< Shift value for MVP_BKPTLOOP6DONE           */
#define _MVP_DEBUGEN_BKPTLOOP6DONE_MASK                 0x80UL                                           /**< Bit mask for MVP_BKPTLOOP6DONE              */
#define _MVP_DEBUGEN_BKPTLOOP6DONE_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTLOOP6DONE_DEFAULT               (_MVP_DEBUGEN_BKPTLOOP6DONE_DEFAULT << 7)        /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTLOOP7DONE                       (0x1UL << 8)                                     /**< Enable Breakpoint on Loop Done              */
#define _MVP_DEBUGEN_BKPTLOOP7DONE_SHIFT                8                                                /**< Shift value for MVP_BKPTLOOP7DONE           */
#define _MVP_DEBUGEN_BKPTLOOP7DONE_MASK                 0x100UL                                          /**< Bit mask for MVP_BKPTLOOP7DONE              */
#define _MVP_DEBUGEN_BKPTLOOP7DONE_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTLOOP7DONE_DEFAULT               (_MVP_DEBUGEN_BKPTLOOP7DONE_DEFAULT << 8)        /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTALUNAN                          (0x1UL << 10)                                    /**< Enable Breakpoint on ALUNAN                 */
#define _MVP_DEBUGEN_BKPTALUNAN_SHIFT                   10                                               /**< Shift value for MVP_BKPTALUNAN              */
#define _MVP_DEBUGEN_BKPTALUNAN_MASK                    0x400UL                                          /**< Bit mask for MVP_BKPTALUNAN                 */
#define _MVP_DEBUGEN_BKPTALUNAN_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTALUNAN_DEFAULT                  (_MVP_DEBUGEN_BKPTALUNAN_DEFAULT << 10)          /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTR0POSREAL                       (0x1UL << 11)                                    /**< Enable Breakpoint on R0POSREAL              */
#define _MVP_DEBUGEN_BKPTR0POSREAL_SHIFT                11                                               /**< Shift value for MVP_BKPTR0POSREAL           */
#define _MVP_DEBUGEN_BKPTR0POSREAL_MASK                 0x800UL                                          /**< Bit mask for MVP_BKPTR0POSREAL              */
#define _MVP_DEBUGEN_BKPTR0POSREAL_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTR0POSREAL_DEFAULT               (_MVP_DEBUGEN_BKPTR0POSREAL_DEFAULT << 11)       /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTALUOF                           (0x1UL << 12)                                    /**< Enable Breakpoint on ALUOF                  */
#define _MVP_DEBUGEN_BKPTALUOF_SHIFT                    12                                               /**< Shift value for MVP_BKPTALUOF               */
#define _MVP_DEBUGEN_BKPTALUOF_MASK                     0x1000UL                                         /**< Bit mask for MVP_BKPTALUOF                  */
#define _MVP_DEBUGEN_BKPTALUOF_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTALUOF_DEFAULT                   (_MVP_DEBUGEN_BKPTALUOF_DEFAULT << 12)           /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTALUUF                           (0x1UL << 13)                                    /**< Enable Breakpoint on ALUUF                  */
#define _MVP_DEBUGEN_BKPTALUUF_SHIFT                    13                                               /**< Shift value for MVP_BKPTALUUF               */
#define _MVP_DEBUGEN_BKPTALUUF_MASK                     0x2000UL                                         /**< Bit mask for MVP_BKPTALUUF                  */
#define _MVP_DEBUGEN_BKPTALUUF_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTALUUF_DEFAULT                   (_MVP_DEBUGEN_BKPTALUUF_DEFAULT << 13)           /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTSTORECONVERTOF                  (0x1UL << 14)                                    /**< Enable Breakpoint on STORECONVERTOF         */
#define _MVP_DEBUGEN_BKPTSTORECONVERTOF_SHIFT           14                                               /**< Shift value for MVP_BKPTSTORECONVERTOF      */
#define _MVP_DEBUGEN_BKPTSTORECONVERTOF_MASK            0x4000UL                                         /**< Bit mask for MVP_BKPTSTORECONVERTOF         */
#define _MVP_DEBUGEN_BKPTSTORECONVERTOF_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTSTORECONVERTOF_DEFAULT          (_MVP_DEBUGEN_BKPTSTORECONVERTOF_DEFAULT << 14)  /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTSTORECONVERTUF                  (0x1UL << 15)                                    /**< Enable Breakpoint on STORECONVERTUF         */
#define _MVP_DEBUGEN_BKPTSTORECONVERTUF_SHIFT           15                                               /**< Shift value for MVP_BKPTSTORECONVERTUF      */
#define _MVP_DEBUGEN_BKPTSTORECONVERTUF_MASK            0x8000UL                                         /**< Bit mask for MVP_BKPTSTORECONVERTUF         */
#define _MVP_DEBUGEN_BKPTSTORECONVERTUF_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTSTORECONVERTUF_DEFAULT          (_MVP_DEBUGEN_BKPTSTORECONVERTUF_DEFAULT << 15)  /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTSTORECONVERTINF                 (0x1UL << 16)                                    /**< Enable Breakpoint on STORECONVERTINF        */
#define _MVP_DEBUGEN_BKPTSTORECONVERTINF_SHIFT          16                                               /**< Shift value for MVP_BKPTSTORECONVERTINF     */
#define _MVP_DEBUGEN_BKPTSTORECONVERTINF_MASK           0x10000UL                                        /**< Bit mask for MVP_BKPTSTORECONVERTINF        */
#define _MVP_DEBUGEN_BKPTSTORECONVERTINF_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTSTORECONVERTINF_DEFAULT         (_MVP_DEBUGEN_BKPTSTORECONVERTINF_DEFAULT << 16) /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_BKPTSTORECONVERTNAN                 (0x1UL << 17)                                    /**< Enable Breakpoint on STORECONVERTNAN        */
#define _MVP_DEBUGEN_BKPTSTORECONVERTNAN_SHIFT          17                                               /**< Shift value for MVP_BKPTSTORECONVERTNAN     */
#define _MVP_DEBUGEN_BKPTSTORECONVERTNAN_MASK           0x20000UL                                        /**< Bit mask for MVP_BKPTSTORECONVERTNAN        */
#define _MVP_DEBUGEN_BKPTSTORECONVERTNAN_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_BKPTSTORECONVERTNAN_DEFAULT         (_MVP_DEBUGEN_BKPTSTORECONVERTNAN_DEFAULT << 17) /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_DEBUGSTEPCNTEN                      (0x1UL << 28)                                    /**< Debug Step Count Enable                     */
#define _MVP_DEBUGEN_DEBUGSTEPCNTEN_SHIFT               28                                               /**< Shift value for MVP_DEBUGSTEPCNTEN          */
#define _MVP_DEBUGEN_DEBUGSTEPCNTEN_MASK                0x10000000UL                                     /**< Bit mask for MVP_DEBUGSTEPCNTEN             */
#define _MVP_DEBUGEN_DEBUGSTEPCNTEN_DEFAULT             0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_DEBUGSTEPCNTEN_DEFAULT              (_MVP_DEBUGEN_DEBUGSTEPCNTEN_DEFAULT << 28)      /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_DEBUGBKPTALLEN                      (0x1UL << 29)                                    /**< Trigger Breakpoint  when ALL conditions match*/
#define _MVP_DEBUGEN_DEBUGBKPTALLEN_SHIFT               29                                               /**< Shift value for MVP_DEBUGBKPTALLEN          */
#define _MVP_DEBUGEN_DEBUGBKPTALLEN_MASK                0x20000000UL                                     /**< Bit mask for MVP_DEBUGBKPTALLEN             */
#define _MVP_DEBUGEN_DEBUGBKPTALLEN_DEFAULT             0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_DEBUGBKPTALLEN_DEFAULT              (_MVP_DEBUGEN_DEBUGBKPTALLEN_DEFAULT << 29)      /**< Shifted mode DEFAULT for MVP_DEBUGEN        */
#define MVP_DEBUGEN_DEBUGBKPTANYEN                      (0x1UL << 30)                                    /**< Enable Breakpoint when ANY conditions match */
#define _MVP_DEBUGEN_DEBUGBKPTANYEN_SHIFT               30                                               /**< Shift value for MVP_DEBUGBKPTANYEN          */
#define _MVP_DEBUGEN_DEBUGBKPTANYEN_MASK                0x40000000UL                                     /**< Bit mask for MVP_DEBUGBKPTANYEN             */
#define _MVP_DEBUGEN_DEBUGBKPTANYEN_DEFAULT             0x00000000UL                                     /**< Mode DEFAULT for MVP_DEBUGEN                */
#define MVP_DEBUGEN_DEBUGBKPTANYEN_DEFAULT              (_MVP_DEBUGEN_DEBUGBKPTANYEN_DEFAULT << 30)      /**< Shifted mode DEFAULT for MVP_DEBUGEN        */

/* Bit fields for MVP DEBUGSTEPCNT */
#define _MVP_DEBUGSTEPCNT_RESETVALUE                    0x00000000UL                                  /**< Default value for MVP_DEBUGSTEPCNT          */
#define _MVP_DEBUGSTEPCNT_MASK                          0x00FFFFFFUL                                  /**< Mask for MVP_DEBUGSTEPCNT                   */
#define _MVP_DEBUGSTEPCNT_DEBUGSTEPCNT_SHIFT            0                                             /**< Shift value for MVP_DEBUGSTEPCNT            */
#define _MVP_DEBUGSTEPCNT_DEBUGSTEPCNT_MASK             0xFFFFFFUL                                    /**< Bit mask for MVP_DEBUGSTEPCNT               */
#define _MVP_DEBUGSTEPCNT_DEBUGSTEPCNT_DEFAULT          0x00000000UL                                  /**< Mode DEFAULT for MVP_DEBUGSTEPCNT           */
#define MVP_DEBUGSTEPCNT_DEBUGSTEPCNT_DEFAULT           (_MVP_DEBUGSTEPCNT_DEBUGSTEPCNT_DEFAULT << 0) /**< Shifted mode DEFAULT for MVP_DEBUGSTEPCNT   */

/** @} End of group EFR32BG24_MVP_BitFields */
/** @} End of group EFR32BG24_MVP */
/** @} End of group Parts */

#endif /* EFR32BG24_MVP_H */
