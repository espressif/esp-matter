/***************************************************************************//**
 * @file
 * @brief USB Host - Generic Ohci Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @note             (1) The OHCI specification referred to throughout this file is the 'Open Host
 *                       Controller Interface (OHCI) Specification for USB', Release: 1.0a, released
 *                       09/14/99 2:33 PM. It is available through the USB.org website.
 *
 * @note             (3) Although the core has 4 available combinations for the USBH_CFG_INIT_ALLOC_EN and
 *                       USBH_CFG_OPTIMIZE_SPD_EN, this driver has 3 combinations. If
 *                       USBH_CFG_OPTIMIZE_SPD_EN is enabled, USBH_CFG_INIT_ALLOC_EN only affects the number
 *                       of initial data structures allocated by the dynamic memory pools. The code is not
 *                       affected.
 *
 * @note             (4) This driver has been developed and tested on little-endian platforms. Special care
 *                       may need to be taken if this driver is used on a big-endian platform.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_HCD_MODULE
#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <cpu/include/cpu_cache.h>

#include  <common/include/lib_mem.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/source/host/hcd/usbh_hcd_core_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_hub_priv.h>

#include  <usb/include/host/usbh_core_hub.h>
#include  <usb/include/host/usbh_core_handle.h>

#define  USBH_OHCI_DBG_TRAP_EN   DEF_DISABLED
#define  USBH_OHCI_DBG_STATS_EN  DEF_DISABLED

#if (USBH_OHCI_DBG_TRAP_EN == DEF_ENABLED)
#define  USBH_OHCI_DBG_TRAP()                               while (1) {; }
#else
#define  USBH_OHCI_DBG_TRAP()
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, HCD, OHCI)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

/********************************************************************************************************
 *                                       OHCI ED LIST NBR DEFINES
 ********************************************************************************************************
 * Note(s) : (1) See OHCI Specification section 5.2.6 'Host Controller Driver Internal Definitions' for
 *               more details.
 *******************************************************************************************************/

#define  USBH_OHCI_HCCA_INTERRUPT_TABLE_SIZE                    32u

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_DISABLED)
#define  USBH_OHCI_HCD_ED_LIST_CTRL                              0u

#define  USBH_OHCI_HCD_ED_LIST_BULK                              1u

#define  USBH_OHCI_HCD_ED_LIST_SIZE                              2u
#else
#define  USBH_OHCI_HCD_ED_LIST_ISOC                              0u

#define  USBH_OHCI_HCD_ED_LIST_INTR_32MS                         0u
#define  USBH_OHCI_HCD_ED_LIST_INTR_16MS                        32u
#define  USBH_OHCI_HCD_ED_LIST_INTR_08MS                        48u
#define  USBH_OHCI_HCD_ED_LIST_INTR_04MS                        56u
#define  USBH_OHCI_HCD_ED_LIST_INTR_02MS                        60u
#define  USBH_OHCI_HCD_ED_LIST_INTR_01MS                        62u

#define  USBH_OHCI_HCD_ED_LIST_CTRL                             63u

#define  USBH_OHCI_HCD_ED_LIST_BULK                             64u

#define  USBH_OHCI_HCD_ED_LIST_SIZE                             65u

#define  USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_32MS                   0
#define  USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_16MS                   1
#define  USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_08MS                   2
#define  USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_04MS                   3
#define  USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_02MS                   4
#define  USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_01MS                   5
#define  USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_MAX                    5

#define  USBH_OHCI_HCD_ED_LIST_INTR_NODE_IX_BIT_NBR_CONVERT(depth, bit_or_ix)  (USBH_OHCI_HC_ED_IxBitNbrConvTbl[(bit_or_ix)] >> (depth))

#define  USBH_OHCI_BIN_TREE_MSK_32MS                            DEF_BIT_FIELD(32u, 0u)
#define  USBH_OHCI_BIN_TREE_MSK_16MS                            DEF_BIT_FIELD(16u, 0u)
#define  USBH_OHCI_BIN_TREE_MSK_08MS                            DEF_BIT_FIELD(8u, 0u)
#define  USBH_OHCI_BIN_TREE_MSK_04MS                            DEF_BIT_FIELD(4u, 0u)
#define  USBH_OHCI_BIN_TREE_MSK_02MS                            DEF_BIT_FIELD(2u, 0u)
#define  USBH_OHCI_BIN_TREE_MSK_01MS                            DEF_BIT_00

#define  USBH_OHCI_BIN_TREE_ED_IX_NONE                          DEF_INT_08U_MAX_VAL

#define  USBH_OHCI_HCD_ED_IX_DEPTH_GET(hcd_ed_ix)              (CPU_CntLeadZeros08(63u - (hcd_ed_ix)) - 2u)
#endif

/********************************************************************************************************
 *                                           OHCI REG DEFINES
 ********************************************************************************************************
 * Note(s) : (1) See OHCI Specification section 7 'Operational Registers' for more details.
 *******************************************************************************************************/

//                                                                 -------------------- HcRevision --------------------
#define  USBH_OHCI_REVISION_MSK                                 DEF_BIT_FIELD(8u, 0u)

//                                                                 --------------------- HcControl --------------------
#define  USBH_OHCI_CTRL_CTRL_BULK_SERVICE_RATIO_MSK            (DEF_BIT_01 | DEF_BIT_00)
#define  USBH_OHCI_CTRL_CTRL_BULK_SERVICE_RATIO_1_1             DEF_BIT_NONE
#define  USBH_OHCI_CTRL_CTRL_BULK_SERVICE_RATIO_2_1             DEF_BIT_00
#define  USBH_OHCI_CTRL_CTRL_BULK_SERVICE_RATIO_3_1             DEF_BIT_01
#define  USBH_OHCI_CTRL_CTRL_BULK_SERVICE_RATIO_4_1            (DEF_BIT_01 | DEF_BIT_00)

#define  USBH_OHCI_CTRL_PERIODIC_LIST_EN                        DEF_BIT_02
#define  USBH_OHCI_CTRL_ISOC_EN                                 DEF_BIT_03
#define  USBH_OHCI_CTRL_CTRL_LIST_EN                            DEF_BIT_04
#define  USBH_OHCI_CTRL_BULK_LIST_EN                            DEF_BIT_05
#define  USBH_OHCI_CTRL_ALL_LISTS_EN                            DEF_BIT_FIELD(4u, 2u)

#define  USBH_OHCI_CTRL_HC_FUNCT_STATE_MSK                     (DEF_BIT_07 | DEF_BIT_06)
#define  USBH_OHCI_CTRL_HC_FUNCT_STATE_RESET                    DEF_BIT_NONE
#define  USBH_OHCI_CTRL_HC_FUNCT_STATE_RESUME                   DEF_BIT_06
#define  USBH_OHCI_CTRL_HC_FUNCT_STATE_OPERATIONAL              DEF_BIT_07
#define  USBH_OHCI_CTRL_HC_FUNCT_STATE_SUSPEND                 (DEF_BIT_07 | DEF_BIT_06)

#define  USBH_OHCI_CTRL_INT_ROUTING                             DEF_BIT_08

#define  USBH_OHCI_CTRL_REMOTE_WAKEUP_CONN                      DEF_BIT_09
#define  USBH_OHCI_CTRL_REMOTE_WAKEUP_EN                        DEF_BIT_10

//                                                                 ------------------ HcCommandStatus -----------------
#define  USBH_OHCI_CMD_STATUS_HC_RESET                          DEF_BIT_00
#define  USBH_OHCI_CMD_STATUS_CTRL_LIST_FILLED                  DEF_BIT_01
#define  USBH_OHCI_CMD_STATUS_BULK_LIST_FILLED                  DEF_BIT_02
#define  USBH_OHCI_CMD_STATUS_OWNERSHIP_CHANGE_REQ              DEF_BIT_03
#define  USBH_OHCI_CMD_STATUS_SCHED_OVERRUN_CNT                (DEF_BIT_16 | DEF_BIT_17)

//                                                                 ----------------- HcInterruptStatus ----------------
//                                                                 ----------------- HcInterruptEnable ----------------
//                                                                 ---------------- HcInterruptDisable ----------------
#define  USBH_OHCI_INT_SCHED_OVERRUN                            DEF_BIT_00
#define  USBH_OHCI_INT_WRITEBACK_DONE_HEAD                      DEF_BIT_01
#define  USBH_OHCI_INT_SOF                                      DEF_BIT_02
#define  USBH_OHCI_INT_RESUME_DETECTED                          DEF_BIT_03
#define  USBH_OHCI_INT_UNRECOVERABLE_ERR                        DEF_BIT_04
#define  USBH_OHCI_INT_FRAME_NBR_OVF                            DEF_BIT_05
#define  USBH_OHCI_INT_RH_STATUS_CHNG                           DEF_BIT_06
#define  USBH_OHCI_INT_OWNERSHIP_CHNG                           DEF_BIT_30
#define  USBH_OHCI_INT_MASTER_INT_EN                            DEF_BIT_31
#define  USBH_OHCI_INT_ALL                                     (DEF_BIT_FIELD(7u, 0u) | DEF_BIT_FIELD(2u, 30u))

//                                                                 -------------------- HcDoneHead --------------------
#define  USBH_OHCI_DONE_HEAD_MSK                                DEF_BIT_FIELD(28u, 4u)

//                                                                 ------------------- HcFmInterval -------------------
#define  USBH_OHCI_FRAME_INTERVAL_MSK                           DEF_BIT_FIELD(14u, 0u)
#define  USBH_OHCI_FRAME_INTERVAL_FS_LARGEST_DATA_PACKET        DEF_BIT_FIELD(15u, 16u)
#define  USBH_OHCI_FRAME_INTERVAL_TOGGLE                        DEF_BIT_31

//                                                                 ------------------- HcFmRemaining ------------------
#define  USBH_OHCI_FRAME_REM_MSK                                DEF_BIT_FIELD(14u, 0u)
#define  USBH_OHCI_FRAME_REM_TOGGLE                             DEF_BIT_31

//                                                                 -------------------- HcFmNumber --------------------
#define  USBH_OHCI_FRAME_NBR_MSK                                DEF_BIT_FIELD(16u, 0u)

//                                                                 ------------------ HcPeriodicStart -----------------
#define  USBH_OHCI_PERIODIC_START_MSK                           DEF_BIT_FIELD(14u, 0u)

//                                                                 ------------------- HcLSThreshold ------------------
#define  USBH_OHCI_LS_TH_MSK                                    DEF_BIT_FIELD(12u, 0u)

//                                                                 ------------------ HcRhDescriptorA -----------------
#define  USBH_OHCI_RH_DESC_A_NBR_PORTS                          DEF_BIT_FIELD(8u, 0u)
#define  USBH_OHCI_RH_DESC_A_PWR_SWITCHING_MODE                 DEF_BIT_08
#define  USBH_OHCI_RH_DESC_A_NO_PWR_SWITCHING                   DEF_BIT_09
#define  USBH_OHCI_RH_DESC_A_DEV_TYPE                           DEF_BIT_10
#define  USBH_OHCI_RH_DESC_A_OVER_CURR_PROT_MODE                DEF_BIT_11
#define  USBH_OHCI_RH_DESC_A_NO_OVER_CURR_PROT                  DEF_BIT_12
#define  USBH_OHCI_RH_DESC_A_PWR_ON_TO_PWR_GOOD_TIME            DEF_BIT_FIELD(8u, 24u)

//                                                                 ------------------ HcRhDescriptorB -----------------
#define  USBH_OHCI_RH_DESC_B_DEVICE_REMOVABLE                   DEF_BIT_FIELD(16u, 0u)
#define  USBH_OHCI_RH_DESC_B_PORT_PWR_CTRL_MSK                  DEF_BIT_FIELD(16u, 16u)

//                                                                 -------------------- HcRhStatus --------------------
#define  USBH_OHCI_RH_STATUS_LOCAL_PWR_STATUS                   DEF_BIT_00
#define  USBH_OHCI_RH_STATUS_GLOBAL_PWR_CLR                     DEF_BIT_00
#define  USBH_OHCI_RH_STATUS_OVER_CURR_INDICATOR                DEF_BIT_01
#define  USBH_OHCI_RH_STATUS_DEV_REMOTE_WAKEUP_EN               DEF_BIT_15
#define  USBH_OHCI_RH_STATUS_LOCAL_PWR_STATUS_CHNG              DEF_BIT_16
#define  USBH_OHCI_RH_STATUS_GLOBAL_PWR_SET                     DEF_BIT_16
#define  USBH_OHCI_RH_STATUS_OVER_CURR_INDICATOR_CHNG           DEF_BIT_17
#define  USBH_OHCI_RH_STATUS_CLR_REMOTE_WAKEUP_EN               DEF_BIT_31

//                                                                 ----------- HcRhPortStatus[1:Nbr Ports] ------------
#define  USBH_OHCI_RH_PORT_STATUS_CURR_CONN_STATUS              DEF_BIT_00
#define  USBH_OHCI_RH_PORT_STATUS_PORT_EN_STATUS                DEF_BIT_01
#define  USBH_OHCI_RH_PORT_STATUS_PORT_SUSPEND_STATUS           DEF_BIT_02
#define  USBH_OHCI_RH_PORT_STATUS_PORT_OVER_CURR_INDICATOR      DEF_BIT_03
#define  USBH_OHCI_RH_PORT_STATUS_PORT_RESET_STATUS             DEF_BIT_04
#define  USBH_OHCI_RH_PORT_STATUS_PORT_PWR_STATUS               DEF_BIT_08
#define  USBH_OHCI_RH_PORT_STATUS_PORT_PWR_SET                  DEF_BIT_08
#define  USBH_OHCI_RH_PORT_STATUS_LS_DEV_ATTACHED               DEF_BIT_09
#define  USBH_OHCI_RH_PORT_STATUS_PORT_PWR_CLR                  DEF_BIT_09
#define  USBH_OHCI_RH_PORT_STATUS_CONN_STATUS_CHNG              DEF_BIT_16
#define  USBH_OHCI_RH_PORT_STATUS_PORT_EN_STATUS_CHNG           DEF_BIT_17
#define  USBH_OHCI_RH_PORT_STATUS_PORT_SUSPEND_STATUS_CHNG      DEF_BIT_18
#define  USBH_OHCI_RH_PORT_STATUS_OVER_CURR_INDICATOR_CHNG      DEF_BIT_19
#define  USBH_OHCI_RH_PORT_STATUS_PORT_RESET_STATUS_CHNG        DEF_BIT_20

#define  USBH_OHCI_RH_PORT_STATUS_STATE_MSK                     DEF_BIT_FIELD(16u, 0u)
#define  USBH_OHCI_RH_PORT_STATUS_CHNG_MSK                      DEF_BIT_FIELD(16u, 16u)

#define  USBH_OHCI_RH_PORT_STATUS_STATE_GET(status)           ((status) & USBH_OHCI_RH_PORT_STATUS_STATE_MSK)
#define  USBH_OHCI_RH_PORT_STATUS_CHNG_GET(status)           (((status) & USBH_OHCI_RH_PORT_STATUS_CHNG_MSK) >> 16u)

/********************************************************************************************************
 *                                       OHCI EP DESC FIELD DEFINES
 ********************************************************************************************************
 * Note(s) : (1) See OHCI Specification section 4.2 'Endpoint Descriptor' for more details.
 *******************************************************************************************************/

//                                                                 -------------------- CTRL WORD ---------------------
#define  USBH_OHCI_ED_CTRL_FNCT_ADDR                            DEF_BIT_FIELD(7u, 0u)

#define  USBH_OHCI_ED_CTRL_EP_NBR_BIT_SHIFT                      7u
#define  USBH_OHCI_ED_CTRL_EP_NBR                               DEF_BIT_FIELD(4u, USBH_OHCI_ED_CTRL_EP_NBR_BIT_SHIFT)

#define  USBH_OHCI_ED_CTRL_DIR_MSK                             (DEF_BIT_12 | DEF_BIT_11)
#define  USBH_OHCI_ED_CTRL_DIR_TD                               DEF_BIT_NONE
#define  USBH_OHCI_ED_CTRL_DIR_OUT                              DEF_BIT_11
#define  USBH_OHCI_ED_CTRL_DIR_IN                               DEF_BIT_12

#define  USBH_OHCI_ED_CTRL_SPD_FULL                             DEF_BIT_NONE
#define  USBH_OHCI_ED_CTRL_SPD_LOW                              DEF_BIT_13

#define  USBH_OHCI_ED_CTRL_SKIP                                 DEF_BIT_14

#define  USBH_OHCI_ED_CTRL_FMT_GEN                              DEF_BIT_NONE
#define  USBH_OHCI_ED_CTRL_FMT_ISOC                             DEF_BIT_15

#define  USBH_OHCI_ED_CTRL_MAX_PKT_SIZE_BIT_SHIFT               16u
#define  USBH_OHCI_ED_CTRL_MAX_PKT_SIZE_MSK                     DEF_BIT_FIELD(11u, USBH_OHCI_ED_CTRL_MAX_PKT_SIZE_BIT_SHIFT)
#define  USBH_OHCI_ED_CTRL_MAX_PKT_SIZE_GET(ctrl)            (((ctrl) & USBH_OHCI_ED_CTRL_MAX_PKT_SIZE_MSK) >> USBH_OHCI_ED_CTRL_MAX_PKT_SIZE_BIT_SHIFT)

//                                                                 HC does not access this region of the ED. It is ...
//                                                                 ... used by the HCD to store the EP type.
#define  USBH_OHCI_ED_CTRL_EP_TYPE_BIT_SHIFT                    27u
#define  USBH_OHCI_ED_CTRL_EP_TYPE_MSK                          DEF_BIT_FIELD(2u, USBH_OHCI_ED_CTRL_EP_TYPE_BIT_SHIFT)
#define  USBH_OHCI_ED_CTRL_EP_TYPE_GET(ctrl)                 (((ctrl) & USBH_OHCI_ED_CTRL_EP_TYPE_MSK) >> USBH_OHCI_ED_CTRL_EP_TYPE_BIT_SHIFT)

//                                                                 ------------------- HEAD PTR WORD ------------------
#define  USBH_OHCI_ED_HEAD_PTR_HALT                             DEF_BIT_00
#define  USBH_OHCI_ED_HEAD_PTR_TOGGLE_CARRY                     DEF_BIT_01
//                                                                 ------------------- HEAD PTR WORD ------------------
//                                                                 ------------------- TAIL PTR WORD ------------------
//                                                                 ----------------- NEXT ED PTR WORD -----------------
#define  USBH_OHCI_ED_PTR_MASK                                  DEF_BIT_FIELD(28u, 4u)

/********************************************************************************************************
 *                                   OHCI XFER DESC (TD) FIELD DEFINES
 ********************************************************************************************************
 * Note(s) : (1) See OHCI Specification section 4.3 'Transfer Descriptors' for more details.
 *******************************************************************************************************/

//                                                                 ---------- OHCI GENERAL TD FIELD DEFINES -----------
//                                                                 See OHCI spec section 4.3.1.
//                                                                 -------------------- CTRL WORD ---------------------
//                                                                 HC does not access this region of the TD. It is ...
//                                                                 ... used by the HCD to store the HCD TD ix.
#define  USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MSK                DEF_BIT_FIELD(18u, 0u)
#define  USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MAX               (DEF_BIT_18 - 1u)

#define  USBH_OHCI_TD_CTRL_BUF_ROUNDING                         DEF_BIT_18

#define  USBH_OHCI_TD_CTRL_DIR_PID_MSK                         (DEF_BIT_20 | DEF_BIT_19)
#define  USBH_OHCI_TD_CTRL_DIR_PID_SETUP                        DEF_BIT_NONE
#define  USBH_OHCI_TD_CTRL_DIR_PID_OUT                          DEF_BIT_19
#define  USBH_OHCI_TD_CTRL_DIR_PID_IN                           DEF_BIT_20

#define  USBH_OHCI_TD_CTRL_DLY_INT_MSK                          DEF_BIT_FIELD(3u, 21u)
#define  USBH_OHCI_TD_CTRL_DLY_INT_NO_DLY                       DEF_BIT_NONE
#define  USBH_OHCI_TD_CTRL_DLY_INT_NO_INT                       DEF_BIT_FIELD(3u, 21u)

#define  USBH_OHCI_TD_CTRL_DATA_TOGGLE_MSK                     (DEF_BIT_25 | DEF_BIT_24)
#define  USBH_OHCI_TD_CTRL_DATA_TOGGLE_ED_CARRY                 DEF_BIT_NONE
#define  USBH_OHCI_TD_CTRL_DATA_TOGGLE_FORCE_DATA0              DEF_BIT_25
#define  USBH_OHCI_TD_CTRL_DATA_TOGGLE_FORCE_DATA1             (DEF_BIT_25 | DEF_BIT_24)

#define  USBH_OHCI_TD_CTRL_CMPL_CODE_MSK                        DEF_BIT_FIELD(4u, 28u)

//                                                                 ---------- OHCI GENERAL TD FIELD DEFINES -----------
//                                                                 See OHCI spec section 4.3.2.
#define  USBH_OHCI_ISOC_TD_CTRL_FRAME_CNT_BIT_SHIFT             24u
#define  USBH_OHCI_ISOC_TD_CTRL_FRAME_CNT_MSK                   DEF_BIT_FIELD(3u, OHCI_ISOC_TD_CTRL_FRAME_CNT_BIT_SHIFT)
#define  USBH_OHCI_ISOC_TD_CTRL_FRAME_CNT_GET(ctrl)          (((CPU_INT32U)ctrl) << OHCI_ISOC_TD_CTRL_FRAME_CNT_BIT_SHIFT)

#define  USBH_OHCI_ISOC_TD_CTRL_START_FRAME_MSK                 DEF_BIT_FIELD(15u, 0u)

/********************************************************************************************************
 *                                       OHCI CONDITION CODE DEFINES
 ********************************************************************************************************
 * Note(s) : (1) This excerpt from the OHCI documentation describes each completion code:
 *
 *               USBH_OHCI_CMPL_CODE_NO_ERROR                TD processed without errors.
 *               USBH_OHCI_CMPL_CODE_CRC                     Last data pkt from EP contained a CRC error.
 *               USBH_OHCI_CMPL_CODE_BIT_STUFFING            Last data pkt from EP contained a bitstuffing viol.
 *               USBH_OHCI_CMPL_CODE_DATA_TOGGLE_MISMATCH    Last pkt from EP had unexpected data toggle PID.
 *               USBH_OHCI_CMPL_CODE_STALL                   TD was moved to Done Queue because EP returned stall.
 *               USBH_OHCI_CMPL_CODE_DEV_NOT_RESPONDING      Dev didn't respond to token (IN) or provide handshake (OUT).
 *               USBH_OHCI_CMPL_CODE_PID_CHK_FAILURE         Chk bits on PID from EP failed on data PID or handshake.
 *               USBH_OHCI_CMPL_CODE_UNEXPECTED_PID          Rx PID was not valid or PID value not defined.
 *               USBH_OHCI_CMPL_CODE_DATA_OVERRUN            EP rtn'd more than max pkt size or more than buf.
 *               USBH_OHCI_CMPL_CODE_DATA_UNDERRUN           EP rtn'd less than max pkt size &  less than buf.
 *               USBH_OHCI_CMPL_CODE_BUF_OVERRUN             During IN,  HC could not write    data fast enough.
 *               USBH_OHCI_CMPL_CODE_BUF_UNDERRUN            During OUT, HC could not retrieve data fast enough.
 *               USBH_OHCI_CMPL_CODE_NOT_ACCESSED            Set by software before TD is placed on list.
 *
 *           (2) See OHCI Specification section 4.3.3 'Completion Codes' for more details.
 *
 *           (3) Field 'Condition Code' (CC) of a Transfer Descriptor (TD) contains one of these values.
 *******************************************************************************************************/

//                                                                 See Notes #1, #2 and #3 for more details.
#define  USBH_OHCI_CMPL_CODE_NO_ERROR                           DEF_BIT_FIELD_ENC(0x00u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_CRC                                DEF_BIT_FIELD_ENC(0x01u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_BIT_STUFFING                       DEF_BIT_FIELD_ENC(0x02u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_DATA_TOGGLE_MISMATCH               DEF_BIT_FIELD_ENC(0x03u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_STALL                              DEF_BIT_FIELD_ENC(0x04u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_DEV_NOT_RESPONDING                 DEF_BIT_FIELD_ENC(0x05u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_PID_CHK_FAILURE                    DEF_BIT_FIELD_ENC(0x06u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_UNEXPECTED_PID                     DEF_BIT_FIELD_ENC(0x07u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_DATA_OVERRUN                       DEF_BIT_FIELD_ENC(0x08u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_DATA_UNDERRUN                      DEF_BIT_FIELD_ENC(0x09u, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_BUF_OVERRUN                        DEF_BIT_FIELD_ENC(0x0Cu, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_BUF_UNDERRUN                       DEF_BIT_FIELD_ENC(0x0Du, DEF_BIT_FIELD(4u, 28u))
#define  USBH_OHCI_CMPL_CODE_NOT_ACCESSED                       DEF_BIT_FIELD_ENC(0x0Fu, DEF_BIT_FIELD(4u, 28u))

/********************************************************************************************************
 *                                       OHCI FRAME INTERVAL DEFINES
 ********************************************************************************************************
 * Note(s) : (1) See OHCI Specification sections 5.4 'FrameInterval Counter' and 7.3 'Frame Counter
 *               Partition' for more details.
 *******************************************************************************************************/

//                                                                 12000 bits per frame.
#define  USBH_OHCI_FRAME_INTERVAL_VAL                       (12000u - 1u)
#define  USBH_OHCI_FS_LARGEST_DATA_PKT_MAX_OVERHEAD            210u
#define  USBH_OHCI_FRAME_INTERVAL_REG_VAL_GET(interval, overhead)  ((((((interval) - (overhead)) * 6u) / 7u) << 16u) | (interval))

/********************************************************************************************************
 *                                       OHCI PERIODIC START DEFINES
 ********************************************************************************************************
 * Note(s) : (1) See OHCI Specification section 7.3.4 'HcPeriodicStart Register' for more details.
 *
 *           (2) OHCI spec recommends periodic start value to be 90% of frame interval value.
 *******************************************************************************************************/

//                                                                 See note #2.
#define  USBH_OHCI_FMINTERVAL_PERIODIC_START_GET(interval)   (((interval) * 9u) / 10u)

/********************************************************************************************************
 *                                           OHCI ALIGNMENT DEFINES
 *******************************************************************************************************/

#define  USBH_OHCI_ALIGN_HCCA                                  256u
#define  USBH_OHCI_ALIGN_HC_ED                                  16u
#define  USBH_OHCI_ALIGN_HC_TD                                  16u
#define  USBH_OHCI_ALIGN_HC_TD_ISOC                             32u
#define  USBH_OHCI_ALIGN_HCD_ED                                 sizeof(CPU_ALIGN)
#define  USBH_OHCI_ALIGN_HCD_TD                                 sizeof(CPU_ALIGN)

#define  USBH_OHCI_HC_TD_MAX_BUF_SIZE                         8192u
#define  USBH_OHCI_HC_TD_4K_BOUNDARY                        0x0FFFu

//                                                                 Max TD len is 8192 if buf is 4K-aligned.
//                                                                 Round down to nearest max pkt size.
#define  USBH_OHCI_HC_TD_MAX_LEN_GET(buf_addr, max_pkt_size)  ((USBH_OHCI_HC_TD_MAX_BUF_SIZE - ((buf_addr) & USBH_OHCI_HC_TD_4K_BOUNDARY)) \
                                                               - ((USBH_OHCI_HC_TD_MAX_BUF_SIZE - ((buf_addr) & USBH_OHCI_HC_TD_4K_BOUNDARY)) % (max_pkt_size)))

/********************************************************************************************************
 *                                       OHCI HCD TD XFER STATUS DEFINES
 * Note(s) : (1) The status field contains the xfer len and a flag to determine if other TDs are used by
 *               the (portion of) xfer described in this TD. It also contains a flag indicating if the
 *               xfer is finished or not.
 *******************************************************************************************************/

#define  USBH_OHCI_HCD_TD_XFER_STATUS_LEN_MSK                   DEF_BIT_FIELD(14u, 0u)
#define  USBH_OHCI_HCD_TD_XFER_STATUS_IS_FINISHED               DEF_BIT_14
#define  USBH_OHCI_HCD_TD_XFER_STATUS_IS_NOT_LAST               DEF_BIT_15

/********************************************************************************************************
 *                                           OHCI CACHE MGMT MACROS
 *******************************************************************************************************/

//                                                                 ----------------- HCCA CACHE MGMT ------------------
#define  USBH_OHCI_DCACHE_FLUSH_HCCA_INTERRUPT_TABLE(p_hcca)            CPU_DCACHE_RANGE_FLUSH((void *)&((p_hcca)->HccaInterruptTable[0u]), USBH_OHCI_HCCA_INTERRUPT_TABLE_SIZE * 4u)
#define  USBH_OHCI_DCACHE_FLUSH_HCCA_INTERRUPT_TABLE_IX(p_hcca, ix)     CPU_DCACHE_RANGE_FLUSH((void *)&((p_hcca)->HccaInterruptTable[(ix)]), 4u)

#define  USBH_OHCI_DCACHE_INV_HCCA_DONE_HEAD(p_hcca)                    CPU_DCACHE_RANGE_INV(  (void *)&((p_hcca)->HccaDoneHead), 4u)

//                                                                 ----------------- HC ED CACHE MGMT -----------------
#define  USBH_OHCI_DCACHE_FLUSH_HC_ED(p_hc_ed)                          CPU_DCACHE_RANGE_FLUSH((void *)  (p_hc_ed), 16u)
#define  USBH_OHCI_DCACHE_FLUSH_HC_ED_CTRL(p_hc_ed)                     CPU_DCACHE_RANGE_FLUSH((void *)&((p_hc_ed)->Ctrl), 4u)
#define  USBH_OHCI_DCACHE_FLUSH_HC_ED_TAIL_TD_PTR(p_hc_ed)              CPU_DCACHE_RANGE_FLUSH((void *)&((p_hc_ed)->TailHC_TD_Ptr), 4u)
#define  USBH_OHCI_DCACHE_FLUSH_HC_ED_HEAD_TD_PTR(p_hc_ed)              CPU_DCACHE_RANGE_FLUSH((void *)&((p_hc_ed)->HeadHC_TD_Ptr), 4u)
#define  USBH_OHCI_DCACHE_FLUSH_HC_ED_NEXT_ED_PTR(p_hc_ed)              CPU_DCACHE_RANGE_FLUSH((void *)&((p_hc_ed)->NextHC_ED_Ptr), 4u)

#define  USBH_OHCI_DCACHE_INV_HC_ED_HEAD_PTR(p_hc_ed)                   CPU_DCACHE_RANGE_INV(  (void *)&((p_hc_ed)->HeadHC_TD_Ptr), 4u)

//                                                                 ----------------- HC TD CACHE MGMT -----------------
#define  USBH_OHCI_DCACHE_FLUSH_HC_TD(p_hc_td)                          CPU_DCACHE_RANGE_FLUSH((void *)  (p_hc_td), 16u)
#define  USBH_OHCI_DCACHE_FLUSH_HC_TD_NEXT_TD_PTR(p_hc_td)              CPU_DCACHE_RANGE_FLUSH((void *)&((p_hc_td)->NextHC_TD_Ptr), 4u)

#define  USBH_OHCI_DCACHE_INV_HC_TD(p_hc_td)                            CPU_DCACHE_RANGE_INV((void *)  (p_hc_td), 16u)
#define  USBH_OHCI_DCACHE_INV_HC_TD_CTRL(p_hc_td)                       CPU_DCACHE_RANGE_INV((void *)&((p_hc_td)->Ctrl), 4u)

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DBG
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBH_OHCI_DBG_STATS_EN == DEF_ENABLED)
typedef CPU_INT32U USBH_DBG_STATS_CNT;                          // Adjust size of the stats cntrs.

typedef struct usbh_ohci_dbg_stats {                            // ---------------------- STATS -----------------------
  USBH_DBG_STATS_CNT HCD_StartCnt;
  USBH_DBG_STATS_CNT HCD_StopCnt;
  USBH_DBG_STATS_CNT HCD_SuspendExecCnt;
  USBH_DBG_STATS_CNT HCD_SuspendSuccessCnt;
  USBH_DBG_STATS_CNT HCD_ResumeExecCnt;
  USBH_DBG_STATS_CNT HCD_ResumeSuccessCnt;

  USBH_DBG_STATS_CNT EP_OpenExecCnt;
  USBH_DBG_STATS_CNT EP_OpenSuccessCnt;
  USBH_DBG_STATS_CNT EP_CloseExecCnt;
  USBH_DBG_STATS_CNT EP_CloseSuccessCnt;

  USBH_DBG_STATS_CNT URB_SubmitExecCnt;
  USBH_DBG_STATS_CNT URB_SubmitSuccessCnt;
  USBH_DBG_STATS_CNT URB_AbortExecCnt;
  USBH_DBG_STATS_CNT URB_CompleteExecCnt;
  USBH_DBG_STATS_CNT URB_CompleteSuccessCnt;

  USBH_DBG_STATS_CNT URB_DoneCallCnt;
  USBH_DBG_STATS_CNT URB_UnderrunDoneCallCnt;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_DBG_STATS_CNT URB_DoneTD_IxErrCnt;
#else
  USBH_DBG_STATS_CNT URB_DoneED_IxErrCnt;
#endif
  USBH_DBG_STATS_CNT URB_DoneHCD_TD_IsNullErrCnt;
} USBH_OHCI_DBG_STATS;

static USBH_OHCI_DBG_STATS USBH_DbgStats;

#define  USBH_OHCI_DBG_STATS_RESET()                        Mem_Clr((void *)&USBH_DbgStats, \
                                                                    (CPU_SIZE_T) sizeof(USBH_OHCI_DBG_STATS));

#define  USBH_OHCI_DBG_STATS_SET(stat, val)                 USBH_DbgStats.stat = val;
#define  USBH_OHCI_DBG_STATS_GET(stat)                      USBH_DbgStats.stat;
#define  USBH_OHCI_DBG_STATS_INC(stat)                      USBH_DbgStats.stat++;
#define  USBH_OHCI_DBG_STATS_INC_IF_TRUE(stat, bool)        if ((bool) == DEF_TRUE) { \
    USBH_OHCI_DBG_STATS_INC(stat);                                                    \
}
#else
#define  USBH_OHCI_DBG_STATS_RESET()
#define  USBH_OHCI_DBG_STATS_SET(stat, val)
#define  USBH_OHCI_DBG_STATS_GET(stat)
#define  USBH_OHCI_DBG_STATS_INC(stat)
#define  USBH_OHCI_DBG_STATS_INC_IF_TRUE(stat, bool)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef enum usbh_ohci_port_pwr {                               // --------------- OHCI PORT PWR STATUS ---------------
  USBH_OHCI_PORT_PWR_ALWAYS,                                    // Port always powered.
  USBH_OHCI_PORT_PWR_GLOBAL,                                    // Ports are globally     controllable.
  USBH_OHCI_PORT_PWR_INDIVIDUAL                                 // Ports are individually controllable.
} USBH_OHCI_PORT_PWR;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
typedef struct usbh_ohci_bin_tree_info {                        // ---------------- OHCI BIN TREE INFO ----------------
  CPU_INT32U Msk;                                               // Msk of all the bits in a given bin tree depth.
  CPU_INT08U BaseIx;                                            // HCD ED base ix for     a given bin tree depth.
} USBH_OHCI_BIN_TREE_INFO;

typedef struct usbh_ohci_bin_tree_status {                      // ----------- OHCI BIN TREE BROWSING STATUS ----------
                                                                // Bitmap of every depth level of the tree.
  CPU_INT32U NodesBitmap[USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_MAX + 1u];
  CPU_INT08S Depth;                                             // Cur depth at which the browsing is.
} USBH_OHCI_BIN_TREE_STATUS;
#endif

typedef struct usbh_ohci_hcd_ed USBH_OHCI_HCD_ED;
typedef struct usbh_ohci_hcd_td USBH_OHCI_HCD_TD;

typedef struct usbh_ohci_reg {                                  // --------------- HC REG MAP DATA TYPE ---------------
  CPU_REG32 HcRevision;                                         // HCI spec rev implemented by HC.
  CPU_REG32 HcControl;                                          // Operating modes for the HC.
  CPU_REG32 HcCommandStatus;                                    // Used by HC to rx cmds from HCD & reflect HC status.
  CPU_REG32 HcInterruptStatus;                                  // Provide status on events that cause HW ints.
  CPU_REG32 HcInterruptEnable;                                  // Used to en  HW ints.
  CPU_REG32 HcInterruptDisable;                                 // Used to dis HW ints.
  CPU_REG32 HcHCCA;                                             // Addr of HCCA.
  CPU_REG32 HcPeriodCurrentED;                                  // Addr of cur isoc or intr ED.
  CPU_REG32 HcControlHeadED;                                    // Addr of first ED of ctrl list.
  CPU_REG32 HcControlCurrentED;                                 // Addr of cur   ED of ctrl list.
  CPU_REG32 HcBulkHeadED;                                       // Addr of first ED of bulk list.
  CPU_REG32 HcBulkCurrentED;                                    // Addr of cur   ED of bulk list.
  CPU_REG32 HcDoneHead;                                         // Addr of last cmpl TD added to done q.
  CPU_REG32 HcFmInterval;                                       // Bit time interval in frame & max pkt size w/o ovrun.
  CPU_REG32 HcFmRemaining;                                      // Bit time remaining in cur frame.
  CPU_REG32 HcFmNumber;                                         // Frame nbr ctr.
  CPU_REG32 HcPeriodicStart;                                    // Earliest time HC should start processing per list.
  CPU_REG32 HcLSThreshold;                                      // Used by HC to determine whether to tx LS pkt or not.
  CPU_REG32 HcRhDescriptorA;                                    // Describes characteristics of the RH.
  CPU_REG32 HcRhDescriptorB;                                    // Describes characteristics of the RH.
  CPU_REG32 HcRhStatus;                                         // Hub  status & status change.
  CPU_REG32 HcRhPortStatus[];                                   // Port status & status change.
} USBH_OHCI_REG;

typedef struct usbh_ohci_hcca {                                 // ---------- HC COMMUNICATION AREA DATA TYPE ---------
                                                                // Periodic HC ED tbl.       HC can only Rd this field.
  CPU_REG32 HccaInterruptTable[USBH_OHCI_HCCA_INTERRUPT_TABLE_SIZE];
  CPU_REG16 HccaFrameNbr;                                       // Frame Nbr.                HC can only Wr this field.
  CPU_REG08 HccaPad1[2u];                                       // Pad1.
  CPU_REG32 HccaDoneHead;                                       // Done Head.                HC can only Wr this field.
  CPU_REG08 reserved[116u];                                     // Rsvd for use by HC.
} USBH_OHCI_HCCA;

typedef struct usbh_ohci_hc_ed {                                // ------------------ HC ED DATA TYPE -----------------
  CPU_REG32 Ctrl;                                               // EP desc ctrl.             HC can only Rd this field.
  CPU_REG32 TailHC_TD_Ptr;                                      // Addr of tail in TD list.  HC can only Rd this field.
  CPU_REG32 HeadHC_TD_Ptr;                                      // Addr of head in TD list.  HC can Rd/Wr   this field.
  CPU_REG32 NextHC_ED_Ptr;                                      // Addr of next ED.          HC can only Rd this field.
} USBH_OHCI_HC_ED;

typedef struct usbh_ohci_hc_td {                                // ------------------ HC TD DATA TYPE -----------------
  CPU_REG32 Ctrl;                                               // Xfer desc ctrl.           HC can Rd/Wr   this field.
  CPU_REG32 CurBufPtr;                                          // Addr of cur buf ptr.      HC can Rd/Wr   this field.
  CPU_REG32 NextHC_TD_Ptr;                                      // Addr of next TD.          HC can Rd/Wr   this field.
  CPU_REG32 BufEndPtr;                                          // Addr of end of buf ptr.   HC can only Rd this field.
} USBH_OHCI_HC_TD;

typedef struct usbh_ohci_data {                                 // ---------------- OHCI DRV DATA TYPE ----------------
  USBH_OHCI_HCCA   *HCCA_Ptr;                                   // DMA mem HCCA.

  USBH_OHCI_HCD_ED **HCD_ED_ListsPtrTbl;                        // Tbl containing ptrs to OHCI HCD ED list heads.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_OHCI_HCD_TD **HCD_TD_PtrTbl;                             // Look-up tbl of HCD TD ptrs, ix obtained from HC TD.
  CPU_INT32U       *HCD_TD_PtrTblIxBitmapTbl;
  CPU_INT32U       HCD_TD_PtrTblIxMax;                          // Max        ix at which a HCD TD ptr can be kept.
#endif

  MEM_DYN_POOL     HC_ED_MemPool;                               // Mem pool to alloc HC         EDs.
  MEM_DYN_POOL     HC_TD_MemPool;                               // Mem pool to alloc HC general TDs.

  MEM_DYN_POOL     HCD_ED_MemPool;                              // Mem pool to alloc OHCI drv         EDs.
  MEM_DYN_POOL     HCD_TD_MemPool;                              // Mem pool to alloc OHCI drv general TDs.
                                                                // Mem pool to alloc bufs. Used when dedicated mem ...
  MEM_DYN_POOL     XferBufPool;                                 // ... is used but that app buf is not within it.
  CPU_INT32U       DedicatedMemDataBufStartAddr;
  CPU_INT32U       DedicatedMemDataBufEndAddr;
  CPU_SIZE_T       DedicatedMemDataBufLen;
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  KAL_LOCK_HANDLE  PeriodicListLockHandle;                      // Handle to periodic list lock.

  USBH_OHCI_HC_ED  *HC_ED_PeriodicDummyPtr;                     // Ptr to dummy periodic HC ED, to fill empty EDs.
#endif
} USBH_OHCI_DATA;

struct usbh_ohci_hcd_ed {                                       // ----------------- HCD ED DATA TYPE -----------------
  USBH_OHCI_HC_ED  *HC_ED_Ptr;                                  // Ptr to associated HC ED.
  USBH_OHCI_HCD_ED *NextHCD_ED_Ptr;                             // Ptr to next HCD ED.

  USBH_OHCI_HCD_TD *HeadHCD_TD_Ptr;                             // Ptr to head of HCD ED's HCD TDs list.
  USBH_OHCI_HCD_TD *TailHCD_TD_Ptr;                             // Ptr to tail of HCD ED's HCD TDs list.

  USBH_DEV_HANDLE  DevHandle;                                   // Handle to dev, obtained from USBH core.
  USBH_EP_HANDLE   EP_Handle;                                   // Handle to EP,  obtained from USBH core.

  CPU_INT08U       Ix;
};

struct usbh_ohci_hcd_td {                                       // ----------------- HCD TD DATA TYPE -----------------
                                                                // Bit   15    : Indicates a linked TD is following.
                                                                // Bit   14    : Indicates if TD has cmpl'd.
  CPU_INT16U       XferStatus;                                  // Bits [13..0]: Len of cur TD.

  USBH_OHCI_HCD_ED *HCD_ED_Ptr;                                 // Ptr to HCD ED containing this HCD TD.

  USBH_OHCI_HC_TD  *HC_TD_Ptr;                                  // Ptr to associated HC TD.
  USBH_OHCI_HCD_TD *NextHCD_TD_Ptr;                             // Ptr to next HCD TD.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static const USBH_OHCI_BIN_TREE_INFO USBH_OHCI_BinTreeInfoTbl[USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_MAX + 1u] = {
  { USBH_OHCI_BIN_TREE_MSK_32MS, USBH_OHCI_HCD_ED_LIST_INTR_32MS },
  { USBH_OHCI_BIN_TREE_MSK_16MS, USBH_OHCI_HCD_ED_LIST_INTR_16MS },
  { USBH_OHCI_BIN_TREE_MSK_08MS, USBH_OHCI_HCD_ED_LIST_INTR_08MS },
  { USBH_OHCI_BIN_TREE_MSK_04MS, USBH_OHCI_HCD_ED_LIST_INTR_04MS },
  { USBH_OHCI_BIN_TREE_MSK_02MS, USBH_OHCI_HCD_ED_LIST_INTR_02MS },
  { USBH_OHCI_BIN_TREE_MSK_01MS, USBH_OHCI_HCD_ED_LIST_INTR_01MS }
};

//                                                                 Convert a HC ED ix to a bit nbr in the bin tree.
static const CPU_INT08U USBH_OHCI_HC_ED_IxBitNbrConvTbl[USBH_OHCI_HCCA_INTERRUPT_TABLE_SIZE] = {
  0u,
  16u,
  8u,
  24u,
  4u,
  20u,
  12u,
  28u,
  2u,
  18u,
  10u,
  26u,
  6u,
  22u,
  14u,
  30u,
  1u,
  17u,
  9u,
  25u,
  5u,
  21u,
  13u,
  29u,
  3u,
  19u,
  11u,
  27u,
  7u,
  23u,
  15u,
  31u
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           DRIVER API FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBH_HCD_Init(USBH_HC_DRV     *p_hc_drv,
                          MEM_SEG         *p_mem_seg,
                          MEM_SEG         *p_mem_seg_buf,
                          USBH_HC_CFG_EXT *p_hc_cfg_ext,
                          RTOS_ERR        *p_err);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_HCD_UnInit(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err);
#endif

static void USBH_HCD_Start(USBH_HC_DRV *p_hc_drv,
                           RTOS_ERR    *p_err);

static void USBH_HCD_Stop(USBH_HC_DRV *p_hc_drv,
                          RTOS_ERR    *p_err);

static void USBH_HCD_Suspend(USBH_HC_DRV *p_hc_drv,
                             RTOS_ERR    *p_err);

static void USBH_HCD_Resume(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err);

static CPU_INT16U USBH_HCD_FrameNbrGet(USBH_HC_DRV *p_hc_drv);

static void USBH_HCD_EP_Open(USBH_HC_DRV        *p_hc_drv,
                             USBH_DEV_HANDLE    dev_handle,
                             USBH_EP_HANDLE     ep_handle,
                             USBH_HCD_EP_PARAMS *p_hcd_ep_params,
                             void               **pp_hcd_ep_data,
                             RTOS_ERR           *p_err);

static void USBH_HCD_EP_Close(USBH_HC_DRV *p_hc_drv,
                              void        *p_hcd_ep_data,
                              RTOS_ERR    *p_err);

static void USBH_HCD_EP_Suspend(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN suspend,
                                RTOS_ERR    *p_err);

static void USBH_HCD_EP_HaltClr(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN data_toggle_clr,
                                RTOS_ERR    *p_err);

static void USBH_HCD_URB_Submit(USBH_HC_DRV         *p_hc_drv,
                                void                *p_hcd_ep_data,
                                void                **pp_hcd_urb_data,
                                USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                RTOS_ERR            *p_err);

static CPU_INT32U USBH_HCD_URB_Complete(USBH_HC_DRV         *p_hc_drv,
                                        void                *p_hcd_ep_data,
                                        void                *p_hcd_urb_data,
                                        USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                        RTOS_ERR            *p_err);

static void USBH_HCD_URB_Abort(USBH_HC_DRV         *p_hc_drv,
                               void                *p_hcd_ep_data,
                               void                *p_hcd_urb_data,
                               USBH_HCD_URB_PARAMS *p_hcd_urb_params);

static void USBH_HCD_ISR_Handle(USBH_HC_DRV *p_hc_drv);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       ROOT HUB API FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN USBH_HCD_RH_InfosGet(USBH_HC_DRV        *p_hc_drv,
                                        USBH_HUB_ROOT_INFO *p_rh_info);

static CPU_BOOLEAN USBH_HCD_RH_PortStatusGet(USBH_HC_DRV     *p_hc_drv,
                                             CPU_INT08U      port_nbr,
                                             USBH_HUB_STATUS *p_port_status);

static CPU_BOOLEAN USBH_HCD_RH_PortReq(USBH_HC_DRV *p_hc_drv,
                                       CPU_INT08U  port_nbr,
                                       CPU_INT08U  req,
                                       CPU_INT16U  feature);

static CPU_BOOLEAN USBH_HCD_RH_IntEn(USBH_HC_DRV *p_hc_drv,
                                     CPU_BOOLEAN en);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN USBH_HC_NextFrameNbrWait(USBH_OHCI_REG *p_reg);

static USBH_OHCI_PORT_PWR USBH_HCD_PortPwrModeGet(USBH_HC_DRV *p_hc_drv,
                                                  CPU_INT08U  port_nbr);

static CPU_BOOLEAN USBH_HC_ED_Skip(USBH_HC_DRV     *p_hc_drv,
                                   USBH_OHCI_HC_ED *p_hc_ed);

static void USBH_HCD_TD_Insert(USBH_HC_DRV      *p_hc_drv,
                               USBH_OHCI_HCD_ED *p_hcd_ed,
                               void             **pp_hcd_urb_data,
                               CPU_INT32U       hc_td_ctrl,
                               CPU_INT32U       buf_addr,
                               CPU_INT32U       buf_len,
                               RTOS_ERR         *p_err);

static void USBH_HCD_TD_DoneProcess(USBH_HC_DRV *p_hc_drv);

static void USBH_HCD_TD_Free(USBH_HC_DRV      *p_hc_drv,
                             USBH_OHCI_HCD_TD *p_hcd_td);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_BinTreeInit(USBH_OHCI_BIN_TREE_STATUS *browse_status,
                             CPU_INT08U                depth_lvl,
                             CPU_INT08U                hcd_ed_ix);

static void USBH_BinTreeUpdated(USBH_OHCI_BIN_TREE_STATUS *browse_status,
                                CPU_INT08U                ed_ix_updated);

static CPU_INT08U USBH_BinTreeNextED_IxGet(USBH_OHCI_BIN_TREE_STATUS *browse_status);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   INITIALIZED GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

USBH_HC_DRV_API USBH_HCD_API_OHCI = {                           // --------- OHCI HOST CONTROLLER DRIVER API ----------
  USBH_HCD_Init,
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  USBH_HCD_UnInit,
#else
  DEF_NULL,
#endif
  USBH_HCD_Start,
  USBH_HCD_Stop,
  USBH_HCD_Suspend,
  USBH_HCD_Resume,
  USBH_HCD_FrameNbrGet,

  USBH_HCD_EP_Open,
  USBH_HCD_EP_Close,
  USBH_HCD_EP_Suspend,
  USBH_HCD_EP_HaltClr,

  USBH_HCD_URB_Submit,
  USBH_HCD_URB_Complete,
  USBH_HCD_URB_Abort
};

USBH_HC_RH_API USBH_HCD_API_RH_OHCI = {                         // ----- OHCI HOST CONTROLLER ROOT HUB DRIVER API -----
  USBH_HCD_RH_InfosGet,

  USBH_HCD_RH_PortStatusGet,
  USBH_HCD_RH_PortReq,

  USBH_HCD_RH_IntEn
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                   HOST CONTROLLER DRIVER FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_HCD_Init()
 *
 * @brief    Initialize OHCI host controller driver.
 *
 * @param    p_hc_drv        Pointer to host controller driver.
 *
 * @param    p_mem_seg       Pointer to memory segment where USB data    will be allocated.
 *
 * @param    p_mem_seg_buf   Pointer to memory segment where USB buffers will be allocated.
 *
 * @param    p_err           Pointer to variable that will receive error code from this function :
 *                               - RTOS_ERR_NONE               Operation was successful.
 *                               - RTOS_ERR_NOT_AVAIL          Missing needed kernel feature(s).
 *                               - RTOS_ERR_DRV_EP_ALLOC       Endpoint descriptor allocation failed.
 *                               - RTOS_ERR_DRV_URB_ALLOC      Transfer descriptor allocation failed.
 *                               - RTOS_ERR_INIT               Call to BSP's Init() function failed.
 *                               - RTOS_ERR_POOL_EMPTY         OS objects pool empty.
 *                               - RTOS_ERR_SEG_OVF            Failed to allocate required data.
 *                               - RTOS_ERR_ISR                Cannot create OS objects in an ISR.
 *******************************************************************************************************/
static void USBH_HCD_Init(USBH_HC_DRV     *p_hc_drv,
                          MEM_SEG         *p_mem_seg,
                          MEM_SEG         *p_mem_seg_buf,
                          USBH_HC_CFG_EXT *p_hc_cfg_ext,
                          RTOS_ERR        *p_err)
{
  USBH_OHCI_DATA  *p_data;
  USBH_HC_BSP_API *p_bsp_api;
  MEM_SEG         *p_hc_desc_mem_seg_ptr;
  CPU_INT32U      hc_ed_nbr_max;
  CPU_INT32U      hcd_ed_nbr_max;
  CPU_INT32U      td_nbr_max;
  CPU_INT32U      hc_ed_nbr_init;
  CPU_INT32U      hcd_ed_nbr_init;
  CPU_INT32U      td_nbr_init;
  CPU_INT16U      hcca_align;
  CPU_INT16U      hc_ed_align;
  CPU_INT16U      hc_td_align;
  CPU_BOOLEAN     valid;
  CPU_BOOLEAN     ok;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  CPU_INT32U td_tbl_size;
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET(((p_hc_cfg_ext != DEF_NULL)
                           && (p_hc_cfg_ext->CfgOptimizeSpdPtr != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET(((p_hc_cfg_ext != DEF_NULL)
                           && (p_hc_cfg_ext->CfgInitAllocPtr != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR,; );
#endif

#if ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
  RTOS_ASSERT_DBG_ERR_SET((p_hc_cfg_ext->CfgInitAllocPtr->XferDescQty == p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescQty), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET((p_hc_cfg_ext->CfgInitAllocPtr->XferDescIsocQty == p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescIsocQty), *p_err, RTOS_ERR_INVALID_CFG,; );

  hc_ed_nbr_max = p_hc_cfg_ext->CfgInitAllocPtr->EP_DescQty + p_hc_cfg_ext->CfgInitAllocPtr->EP_DescIsocQty;
  hc_ed_nbr_init = hc_ed_nbr_max;
  td_nbr_max = p_hc_cfg_ext->CfgInitAllocPtr->XferDescQty + p_hc_cfg_ext->CfgInitAllocPtr->XferDescIsocQty;
  td_nbr_init = td_nbr_max;
  td_tbl_size = p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescQty + p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescIsocQty;
  if (td_tbl_size > USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MAX) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
    return;
  }
#elif (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  hc_ed_nbr_max = LIB_MEM_BLK_QTY_UNLIMITED;
  hc_ed_nbr_init = 0u;
  td_nbr_max = p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescQty + p_hc_cfg_ext->CfgOptimizeSpdPtr->XferDescIsocQty;
  td_nbr_init = 0u;
  td_tbl_size = td_nbr_max;
  if (td_tbl_size > USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MAX) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
    return;
  }
#elif (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  hc_ed_nbr_max = p_hc_cfg_ext->CfgInitAllocPtr->EP_DescQty  + p_hc_cfg_ext->CfgInitAllocPtr->EP_DescIsocQty;
  hc_ed_nbr_init = hc_ed_nbr_max;
  td_nbr_max = p_hc_cfg_ext->CfgInitAllocPtr->XferDescQty + p_hc_cfg_ext->CfgInitAllocPtr->XferDescIsocQty;
  td_nbr_init = td_nbr_max;
#else
  hc_ed_nbr_max = LIB_MEM_BLK_QTY_UNLIMITED;
  hc_ed_nbr_init = 0u;
  td_nbr_max = USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MAX;
  td_nbr_init = 0u;
#endif

  hcd_ed_nbr_max = hc_ed_nbr_max;
  hcd_ed_nbr_init = hc_ed_nbr_init;

#if ((USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED) \
  && (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED))                // Account for dummy HC ED reserved for periodic tbl.
  hc_ed_nbr_max++;
  hc_ed_nbr_init++;
#endif

  ok = KAL_FeatureQuery(KAL_FEATURE_DLY, KAL_OPT_DLY_NONE);
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_CREATE_NONE);
  ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_PEND_BLOCKING);
  ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_POST_NONE);
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  ok &= KAL_FeatureQuery(KAL_FEATURE_LOCK_DEL, KAL_OPT_DEL_NONE);
#endif
#endif
  if (ok != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    return;
  }

  LOG_VRB(("OHCI HCD's Allocation Started."));

  p_data = (USBH_OHCI_DATA *)Mem_SegAlloc("OHCI - Driver Data",
                                          p_mem_seg,
                                          sizeof(USBH_OHCI_DATA),
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_Clr(p_data, sizeof(USBH_OHCI_DATA));

  p_hc_drv->DataPtr = (void *)p_data;

  if (p_hc_drv->HW_InfoPtr->HW_DescDedicatedMemInfoPtr != DEF_NULL) { // ----------------- DEDICATED MEM SEG ----------------
    p_hc_desc_mem_seg_ptr = p_hc_drv->HW_InfoPtr->HW_DescDedicatedMemInfoPtr->MemSegPtr;

    hcca_align = DEF_MAX(USBH_OHCI_ALIGN_HCCA, p_hc_drv->HW_InfoPtr->HW_DescDedicatedMemInfoPtr->BufAlignOctets);
    hc_ed_align = DEF_MAX(USBH_OHCI_ALIGN_HC_ED, p_hc_drv->HW_InfoPtr->HW_DescDedicatedMemInfoPtr->BufAlignOctets);
    hc_td_align = DEF_MAX(USBH_OHCI_ALIGN_HC_TD, p_hc_drv->HW_InfoPtr->HW_DescDedicatedMemInfoPtr->BufAlignOctets);
  } else {                                                      // ------------------- MAIN MEM SEG -------------------
    p_hc_desc_mem_seg_ptr = p_mem_seg_buf;

    hcca_align = USBH_OHCI_ALIGN_HCCA;
    hc_ed_align = USBH_OHCI_ALIGN_HC_ED;
    hc_td_align = USBH_OHCI_ALIGN_HC_TD;
  }

  if (p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr != DEF_NULL) { // ----------------- DEDICATED MEM SEG ----------------
    MEM_SEG_INFO seg_info;

    Mem_DynPoolCreateHW("OHCI - Xfer Buf Pool",                 // Create pool of xfers bufs, if req'd.
                        &p_data->XferBufPool,
                        p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr->MemSegPtr,
                        p_hc_cfg_ext->DedicatedMemCfgPtr->DataBufLen,
                        p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr->BufAlignOctets,
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                        p_hc_cfg_ext->DedicatedMemCfgPtr->DataBufQty,
#else
                        0u,
#endif
                        p_hc_cfg_ext->DedicatedMemCfgPtr->DataBufQty,
                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    (void)Mem_SegRemSizeGet(p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr->MemSegPtr,
                            p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr->BufAlignOctets,
                            &seg_info,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_data->DedicatedMemDataBufStartAddr = seg_info.AddrBase;
    p_data->DedicatedMemDataBufEndAddr = p_data->DedicatedMemDataBufStartAddr + seg_info.TotalSize;
    p_data->DedicatedMemDataBufLen = p_hc_cfg_ext->DedicatedMemCfgPtr->DataBufLen;

    LOG_VRB(("OHCI Allocated dedicated memory buf pool."));
  } else {
    p_data->DedicatedMemDataBufLen = 0u;
  }

  //                                                               Get a single blk from dedicated mem for HCCA.
  p_data->HCCA_Ptr = (USBH_OHCI_HCCA *)Mem_SegAllocHW("OHCI - HCCA",
                                                      p_hc_desc_mem_seg_ptr,
                                                      sizeof(USBH_OHCI_HCCA),
                                                      hcca_align,
                                                      DEF_NULL,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_Clr(p_data->HCCA_Ptr, sizeof(USBH_OHCI_HCCA));            // Init HCCA struct.

  Mem_DynPoolCreateHW("OHCI - HC ED Pool",                      // Create pool of HC EDs.
                      &p_data->HC_ED_MemPool,
                      p_hc_desc_mem_seg_ptr,
                      sizeof(USBH_OHCI_HC_ED),
                      hc_ed_align,
                      hc_ed_nbr_init,
                      hc_ed_nbr_max,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);
    return;
  }

  Mem_DynPoolCreateHW("OHCI - HC TD Pool",                      // Create pool of HC TDs.
                      &p_data->HC_TD_MemPool,
                      p_hc_desc_mem_seg_ptr,
                      sizeof(USBH_OHCI_HC_TD),
                      hc_td_align,
                      td_nbr_init,
                      td_nbr_max,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
    return;
  }

  Mem_DynPoolCreate("OHCI - HCD ED Pool",                       // Create pool of HCD EDs.
                    &p_data->HCD_ED_MemPool,
                    p_mem_seg,                                  // HCD structures allocated from main mem seg.
                    sizeof(USBH_OHCI_HCD_ED),
                    USBH_OHCI_ALIGN_HCD_ED,
                    hcd_ed_nbr_init,
                    hcd_ed_nbr_max,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);
    return;
  }

  Mem_DynPoolCreate("OHCI - HCD TD Pool",                       // Create pool of HCD TDs.
                    &p_data->HCD_TD_MemPool,
                    p_mem_seg,                                  // HCD structures allocated from main mem seg.
                    sizeof(USBH_OHCI_HCD_TD),
                    USBH_OHCI_ALIGN_HCD_TD,
                    td_nbr_init,
                    td_nbr_max,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
    return;
  }

  //                                                               Alloc HCD ED ptrs tbl from main mem seg.
  p_data->HCD_ED_ListsPtrTbl = (USBH_OHCI_HCD_ED **)Mem_SegAlloc("OHCI - HCD ED Ptr Tbl",
                                                                 p_mem_seg,
                                                                 (sizeof(USBH_OHCI_HCD_ED *) * (USBH_OHCI_HCD_ED_LIST_SIZE)),
                                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U ix_tbl_size;
    CPU_INT08U unavail_ix_msk;

    //                                                             Alloc HCD TD look-up tbl, if req'd.
    p_data->HCD_TD_PtrTbl = (USBH_OHCI_HCD_TD **)Mem_SegAlloc("OHCI - HCD TD Ptr Tbl",
                                                              p_mem_seg,
                                                              (sizeof(USBH_OHCI_HCD_TD *) * (td_tbl_size)),
                                                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr(p_data->HCD_TD_PtrTbl, (sizeof(USBH_OHCI_HCD_TD *) * (td_tbl_size)));

    p_data->HCD_TD_PtrTblIxMax = td_tbl_size;

    //                                                             Calculate number of 32-bits bitmaps req'd.
    ix_tbl_size = (p_data->HCD_TD_PtrTblIxMax + (DEF_INT_32_NBR_BITS - 1u)) / DEF_INT_32_NBR_BITS;
    p_data->HCD_TD_PtrTblIxBitmapTbl = (CPU_INT32U *)Mem_SegAlloc("OHCI - HCD TD Ptr Tbl Ix Bitmap Tbl",
                                                                  p_mem_seg,
                                                                  (sizeof(CPU_INT32U) * (ix_tbl_size)),
                                                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Mem_Clr(p_data->HCD_TD_PtrTblIxBitmapTbl, (sizeof(CPU_INT32U) * (ix_tbl_size)));

    //                                                             Mark unavailable indexes, if any, as already used.
    unavail_ix_msk = p_data->HCD_TD_PtrTblIxMax % DEF_INT_32_NBR_BITS;
    if (unavail_ix_msk != 0u) {
      DEF_BIT_SET(p_data->HCD_TD_PtrTblIxBitmapTbl[ix_tbl_size - 1u], DEF_BIT_FIELD((DEF_INT_32_NBR_BITS - unavail_ix_msk), unavail_ix_msk));
    }
  }
#endif

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  p_data->PeriodicListLockHandle = KAL_LockCreate("OHCI Periodic List Lock",
                                                  DEF_NULL,
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Create dummy HC ED that indicates to HC to skip.
  p_data->HC_ED_PeriodicDummyPtr = (USBH_OHCI_HC_ED *)Mem_DynPoolBlkGet(&p_data->HC_ED_MemPool,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);
    return;
  }
  //                                                               Set dummy HC ED so that it is always skipped.
  p_data->HC_ED_PeriodicDummyPtr->Ctrl = USBH_OHCI_ED_CTRL_SKIP;
  p_data->HC_ED_PeriodicDummyPtr->TailHC_TD_Ptr = 0u;
  p_data->HC_ED_PeriodicDummyPtr->HeadHC_TD_Ptr = 0u;
  p_data->HC_ED_PeriodicDummyPtr->NextHC_ED_Ptr = 0u;
  USBH_OHCI_DCACHE_FLUSH_HC_ED(p_data->HC_ED_PeriodicDummyPtr);
#endif

  LOG_VRB(("OHCI HCD's Allocation Completed."));

  p_bsp_api = p_hc_drv->BSP_API_Ptr;
  if ((p_bsp_api != DEF_NULL)                                   // Call BSP init.
      && (p_bsp_api->Init != DEF_NULL)) {
    valid = p_bsp_api->Init(USBH_HCD_ISR_Handle, p_hc_drv);
    if (valid != DEF_OK) {
      LOG_ERR(("BSP's Init() call failed."));
      RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);
      return;
    }
  }

  USBH_OHCI_DBG_STATS_RESET();

  LOG_VRB(("Initialization of OHCI HCD completed successfully."));

  return;
}

/****************************************************************************************************//**
 *                                               USBH_HCD_UnInit()
 *
 * @brief    Un-initialize OHCI host controller driver.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    p_err       Pointer to variable that will receive error code from this function :
 *                           - RTOS_ERR_NONE   Operation was successful.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_HCD_UnInit(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err)
{
    #if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  {
    USBH_OHCI_DATA *p_data = (USBH_OHCI_DATA *)p_hc_drv->DataPtr;

    KAL_LockDel(p_data->PeriodicListLockHandle);
  }
    #else
  PP_UNUSED_PARAM(p_hc_drv);
    #endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_HCD_Start()
 *
 * @brief    Start OHCI host controller.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    p_err       Pointer to variable that will receive error code from this function :
 *                           - RTOS_ERR_NONE   Operation was successful.
 *                           - RTOS_ERR_IO     Call to BSP's Cfg() failed.
 *
 *           USBH_HC_Start(), via p_hc_drv->API_Ptr->Start().
 *******************************************************************************************************/
static void USBH_HCD_Start(USBH_HC_DRV *p_hc_drv,
                           RTOS_ERR    *p_err)
{
  USBH_OHCI_DATA  *p_data = (USBH_OHCI_DATA *)p_hc_drv->DataPtr;
  USBH_HC_BSP_API *p_bsp_api = p_hc_drv->BSP_API_Ptr;
  USBH_OHCI_REG   *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U      hc_ctrl = p_reg->HcControl;
  CPU_BOOLEAN     ok;

  USBH_OHCI_DBG_STATS_INC(HCD_StartCnt);

  if (p_bsp_api->IO_Cfg != DEF_NULL) {
    ok = p_bsp_api->IO_Cfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  if (p_bsp_api->PwrCfg != DEF_NULL) {
    ok = p_bsp_api->PwrCfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  if (p_bsp_api->ClkCfg != DEF_NULL) {
    ok = p_bsp_api->ClkCfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  if (p_bsp_api->IntCfg != DEF_NULL) {
    ok = p_bsp_api->IntCfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  p_reg->HcInterruptDisable = USBH_OHCI_INT_ALL;                // Dis all ints.
                                                                // --------------------- RESET HC ---------------------
  LOG_VRB(("Applying Hardware Reset on OHCI HCD."));

  DEF_BIT_SET(hc_ctrl, USBH_OHCI_CTRL_HC_FUNCT_STATE_RESET);    // Apply HW reset.

  p_reg->HcControl = hc_ctrl;
  p_reg->HcControlHeadED = 0u;                                  // Init ctrl list head.
  p_reg->HcBulkHeadED = 0u;                                     // Init bulk list head.
  p_reg->HcHCCA = 0u;                                           // Init HCCA int tbl.

  LOG_VRB(("Applying Software Reset on OHCI HCD."));

  p_reg->HcCommandStatus = USBH_OHCI_CMD_STATUS_HC_RESET;       // Apply software reset.

  KAL_Dly(1u);                                                  // HC software reset may take 10 us, wait 1 ms.

  //                                                               Wr frame interval & largest data pkt ctr.
  p_reg->HcFmInterval = USBH_OHCI_FRAME_INTERVAL_REG_VAL_GET(USBH_OHCI_FRAME_INTERVAL_VAL, USBH_OHCI_FS_LARGEST_DATA_PKT_MAX_OVERHEAD);
  //                                                               Wr periodic start.
  p_reg->HcPeriodicStart = USBH_OHCI_FMINTERVAL_PERIODIC_START_GET(USBH_OHCI_FRAME_INTERVAL_VAL);

    #if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  {
    CPU_INT08U ix;

    //                                                             Init HCCA int tbl with dummy HC ED.
    for (ix = 0u; ix < USBH_OHCI_HCCA_INTERRUPT_TABLE_SIZE; ix++) {
      p_data->HCCA_Ptr->HccaInterruptTable[ix] = (CPU_REG32)p_data->HC_ED_PeriodicDummyPtr;
    }
    USBH_OHCI_DCACHE_FLUSH_HCCA_INTERRUPT_TABLE(p_data->HCCA_Ptr);
  }
    #endif

  p_reg->HcHCCA = (CPU_REG32)p_data->HCCA_Ptr;

  if (DEF_BIT_IS_SET(hc_ctrl, USBH_OHCI_CTRL_REMOTE_WAKEUP_CONN) == DEF_YES) {
    hc_ctrl = USBH_OHCI_CTRL_REMOTE_WAKEUP_EN;                  // En remote wakeup.
  }

  //                                                               Put HC in operational state.
  DEF_BIT_CLR(hc_ctrl, USBH_OHCI_CTRL_HC_FUNCT_STATE_MSK);
  DEF_BIT_SET(hc_ctrl, USBH_OHCI_CTRL_HC_FUNCT_STATE_OPERATIONAL);

  p_reg->HcControl = hc_ctrl;

  LOG_VRB(("Enabling Interrupts in OHCI HCD."));

  if (p_bsp_api->Start != DEF_NULL) {
    ok = p_bsp_api->Start();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }
  //                                                               En int.
  p_reg->HcInterruptEnable = (USBH_OHCI_INT_MASTER_INT_EN
                              | USBH_OHCI_INT_WRITEBACK_DONE_HEAD
                              | USBH_OHCI_INT_RESUME_DETECTED
                              | USBH_OHCI_INT_SCHED_OVERRUN
                              | USBH_OHCI_INT_UNRECOVERABLE_ERR);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                               USBH_HCD_Stop()
 *
 * @brief    Stop OHCI host controller.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    p_err       Pointer to variable that will receive error code from this function :
 *                           - RTOS_ERR_NONE   Operation was successful.
 *                           - RTOS_ERR_IO     Call to BSP's Stop() failed.
 *******************************************************************************************************/
static void USBH_HCD_Stop(USBH_HC_DRV *p_hc_drv,
                          RTOS_ERR    *p_err)
{
  USBH_HC_BSP_API *p_bsp_api = p_hc_drv->BSP_API_Ptr;
  USBH_OHCI_REG   *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;

  USBH_OHCI_DBG_STATS_INC(HCD_StopCnt);

  p_reg->HcControl = USBH_OHCI_CTRL_HC_FUNCT_STATE_RESET;       // Apply hw reset.

  if (p_bsp_api->Stop != DEF_NULL) {
    CPU_BOOLEAN ok;

    ok = p_bsp_api->Stop();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_Suspend()
 *
 * @brief    Suspend OHCI host controller.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    p_err       Pointer to variable that will receive error code from this function :
 *                           - RTOS_ERR_NONE   Operation was successful.
 *                           - RTOS_ERR_FAIL   Host controller was in an unknown state or is not
 *                       processing frames correctly.
 *******************************************************************************************************/
static void USBH_HCD_Suspend(USBH_HC_DRV *p_hc_drv,
                             RTOS_ERR    *p_err)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U    hc_ctrl = p_reg->HcControl & USBH_OHCI_CTRL_HC_FUNCT_STATE_MSK;
  CPU_BOOLEAN   ok;

  USBH_OHCI_DBG_STATS_INC(HCD_SuspendExecCnt);

  switch (hc_ctrl) {
    case USBH_OHCI_CTRL_HC_FUNCT_STATE_RESET:
    case USBH_OHCI_CTRL_HC_FUNCT_STATE_SUSPEND:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                      // HC is already in Suspend state.
      return;

    case USBH_OHCI_CTRL_HC_FUNCT_STATE_RESUME:                  // Cur state is Resume or Operational, go in ...
    case USBH_OHCI_CTRL_HC_FUNCT_STATE_OPERATIONAL:             // ... Suspend state.
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      return;
  }
  //                                                               Stop list processing; take effect at the next frame.
  DEF_BIT_CLR(p_reg->HcControl, (USBH_OHCI_CTRL_PERIODIC_LIST_EN
                                 | USBH_OHCI_CTRL_ISOC_EN
                                 | USBH_OHCI_CTRL_CTRL_LIST_EN
                                 | USBH_OHCI_CTRL_BULK_LIST_EN));

  ok = USBH_HC_NextFrameNbrWait(p_reg);
  if (ok != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  }
  //                                                               En suspend.
  DEF_BIT_CLR(hc_ctrl, USBH_OHCI_CTRL_HC_FUNCT_STATE_MSK);
  DEF_BIT_SET(hc_ctrl, USBH_OHCI_CTRL_HC_FUNCT_STATE_SUSPEND);
  p_reg->HcControl = hc_ctrl;

  USBH_OHCI_DBG_STATS_INC(HCD_SuspendSuccessCnt);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                               USBH_HCD_Resume()
 *
 * @brief    Resume OHCI host controller.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    p_err       Pointer to variable that will receive error code from this function :
 *                           - RTOS_ERR_NONE   Operation was successful.
 *                           - RTOS_ERR_FAIL   Host controller was in an unknown state.
 *******************************************************************************************************/
static void USBH_HCD_Resume(USBH_HC_DRV *p_hc_drv,
                            RTOS_ERR    *p_err)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U    hc_ctrl = p_reg->HcControl & USBH_OHCI_CTRL_HC_FUNCT_STATE_MSK;

  USBH_OHCI_DBG_STATS_INC(HCD_ResumeExecCnt);

  switch (hc_ctrl) {
    case USBH_OHCI_CTRL_HC_FUNCT_STATE_RESUME:
    case USBH_OHCI_CTRL_HC_FUNCT_STATE_OPERATIONAL:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return;

    case USBH_OHCI_CTRL_HC_FUNCT_STATE_RESET:
    case USBH_OHCI_CTRL_HC_FUNCT_STATE_SUSPEND:
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      return;
  }

  DEF_BIT_CLR(hc_ctrl, USBH_OHCI_CTRL_HC_FUNCT_STATE_MSK);      // En Resume state.
  DEF_BIT_SET(hc_ctrl, USBH_OHCI_CTRL_HC_FUNCT_STATE_RESUME);
  p_reg->HcControl = hc_ctrl;

  DEF_BIT_CLR(hc_ctrl, USBH_OHCI_CTRL_HC_FUNCT_STATE_MSK);      // En Operational state.
  DEF_BIT_SET(hc_ctrl, USBH_OHCI_CTRL_HC_FUNCT_STATE_OPERATIONAL);
  p_reg->HcControl = hc_ctrl;

  USBH_OHCI_DBG_STATS_INC(HCD_ResumeSuccessCnt);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HCD_FrameNbrGet()
 *
 * @brief    Get current frame number value.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @return   Frame number.
 *
 *           USBH_HC_FrameNbrGet(), via p_hc_drv->API_Ptr->FrameNbrGet().
 *******************************************************************************************************/
static CPU_INT16U USBH_HCD_FrameNbrGet(USBH_HC_DRV *p_hc_drv)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT16U    frm_nbr = (CPU_INT16U)p_reg->HcFmNumber;

  return (frm_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_Open()
 *
 * @brief    Open endpoint on OHCI host controller.
 *
 * @param    p_hc_drv            Pointer to host controller driver.
 *
 * @param    dev_handle          Device handle.
 *
 * @param    ep_handle           Endpoint handle.
 *
 * @param    p_hcd_ep_params     Pointer to USBH endpoint parameters structure.
 *
 * @param    pp_hcd_ep_data      Pointer to memory location where a pointer to the host controller
 *                               driver's endpoint data will be kept.
 *
 * @param    p_err               Pointer to variable that will receive error code from this function :
 *                                   - RTOS_ERR_NONE             Operation was successful.
 *                                   - RTOS_ERR_DRV_EP_ALLOC     Endpoint descriptor allocation failed.
 *                                   - RTOS_ERR_DRV_URB_ALLOC    Transfer descriptor allocation failed.
 *                                   - RTOS_ERR_EP_BW_NOT_AVAIL  Not enough bandwidth available to open
 *                                                               periodic endpoint.
 *                                   - RTOS_ERR_EP_INVALID       Isoc endpoint not supported.
 *
 * @note     (1) Bulk and Control endpoint and transfer descriptors are organized this way:
 *            @verbatim
 *               HCD_ED_ListsPtrTbl[OHCI_HCD_ED_LIST_CTRL]           HcControlHeadED
 *                               or                                     or
 *               HCD_ED_ListsPtrTbl[OHCI_HCD_ED_LIST_BULK]           HcBulkHeadED
 *                   |                                              |
 *                   V                                              V
 *                   |----------|                           |---------|
 *                   |  HCD ED  |-------------------------->|  HC ED  |
 *                   |----------|                           |---------|
 *                   |  |  ^ |                               |   | |
 *                   |  |  |  \->|----------|  |---------|<-/    | |
 *                   |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                   |  |  |     |----------|  |---------|       | |
 *                   |  |  |          |             |            | |
 *                   |  |  |          |             |            | |
 *                   |  |  |          V             V            | |
 *                   |  |  |     |----------|  |---------|       | |
 *                   |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                   |  |  |     |----------|  |---------|       | |
 *                   |  |  |          |             |            | |
 *                   |  |  |          |             |            | |
 *                   |  |  |          V             V            | |
 *                   |  |  |     |----------|  |---------|       | |
 *                   |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                   |   \------>|----------|  |---------|<-----/  |
 *                   |                |             |              |
 *                   |                |             |              |
 *                   |                V             V              |
 *                   |               NULL          NULL            |
 *                   V                                             V
 *                   |----------|                           |---------|
 *                   |  HCD ED  |-------------------------->|  HC ED  |
 *                   |----------|                           |---------|
 *                   |  |  ^ |                               |   | |
 *                   |  |  |  \->|----------|  |---------|<-/    | |
 *                   |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                   |   \------>|----------|  |---------|<-----/  |
 *                   |                |             |              |
 *                   |                |             |              |
 *                   |                V             V              |
 *                   |               NULL          NULL            |
 *                   V                                             V
 *                   |----------|                           |---------|
 *                   |  HCD ED  |-------------------------->|  HC ED  |
 *                   |----------|                           |---------|
 *                   |  |  ^ |                               |   | |
 *                   |  |  |  \->|----------|  |---------|<-/    | |
 *                   |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                   |  |  |     |----------|  |---------|       | |
 *                   |  |  |          |             |            | |
 *                   |  |  |          |             |            | |
 *                   |  |  |          V             V            | |
 *                   |  |  |     |----------|  |---------|       | |
 *                   |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                   |   \------>|----------|  |---------|<-----/  |
 *                   |                |             |              |
 *                   |                |             |              |
 *                   |                V             V              |
 *                   |               NULL          NULL            |
 *                   V                                             V
 *                   NULL                                          NULL
 *            @endverbatim
 * @note     (2) OHCI and this driver use a binary tree with the following indexes:
 *                      @verbatim
 *               Depth     |  0   |   1   |   2   |   3   |   4   |   5   |
 *               bInterval | 32ms |  16ms |   8ms |   4ms |   2ms |   1ms |
 *                           0-----\
 *                           16------32-----\
 *                           8-----\        48----\
 *                           24------40-----/       \
 *                           4-----\                56--\
 *                           20------36-----\       /     \
 *                           12-----\        52----/       \
 *                           28------44-----/               \
 *                           2-----\                        60-\
 *                           18------34-----\               /    \
 *                           10-----\        50----\       /      \
 *                           26------42-----/       \     /        \
 *                           6-----\                58--/          \
 *                           22------38-----\       /                \
 *                           14-----\        54----/                  \
 *                           30------46-----/                          \
 *                           1-----\                                   62
 *                           17------33-----\                          /
 *                           9-----\        49----\                  /
 *                           25------41-----/       \                /
 *                           5-----\                57--\          /
 *                           21------37-----\       /     \        /
 *                           13-----\        53----/       \      /
 *                           29------45-----/               \    /
 *                           3-----\                        61-/
 *                           19------35-----\               /
 *                           11-----\        51----\       /
 *                           27------43-----/       \     /
 *                           7-----\                59--/
 *                           23------39-----\       /
 *                           15-----\        55----/
 *                           31------47-----/
 *            @venderbatim
 * @note     (3) The table below describes the relation between the HCD ED list (HCD_ED_ListsPtrTbl,
 *               ix 0 to 62) and the HC ED list (USBD_OHCI_HCCA->HccaInterruptTable, ix 0 to 31).
 *                              @verbatim
 *               bInterval   |        32 ms       |   16 ms   |  8 ms  |  4 ms  | 2 ms|1 ms|
 *               HCD ED List |0|1|2|3|4|5|..|30|31|32|33|..|47|48|..|55|56|..|59|60|61| 62 |
 *               HC  ED List |0|1|2|3|4|5|..|30|31|xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
 *                      @endverbatim
 * @note     (4) Interrupt endpoints are inserted in the binary tree at the HCD ED ix where the BW
 *               used is at its lowest in the group where the maximum BW is the lowest. For example a
 *               16 ms endpoint would be a children of the group of 2 32ms endpoints which have the
 *               smallest highest bandwidth within the group. If ix 0 has a used BW of 47 bytes; ix 1,
 *               12 bytes; ix 2, 17 bytes and ix 3, 34 bytes; the 16 ms bInterval endpoint would be
 *               a children of ix 2 and 3, since the highest bandwidth within the group composed of ix
 *               2 and 3, 34 bytes, is smaller than the 47 bytes of the group composed of ix 0 and 1,
 *               even if the ix with the smallest used bandwidth is in the other group.
 *
 * @note     (5) Interrupt endpoint and transfer descriptors are organized to avoid having to alloc
 *               HC EDs structures as placeholders. During init a single dummy HC ED (always empty and
 *               always set as 'Skip') is alloc'd and all indexes of the HccaInterruptTable are set to
 *               point to that HC ED, so that the HC can browse the periodic tree without error. When
 *               a periodic EP is opened, a HCD ED is added at the correct ix of the HCD ED list and
 *               in the corresponding HC ED ix of the HCCA Interrupt tbl, if bInterval is 32ms. If
 *               not, the HC ED will be added at the end of the HC EDs of its ancestor, if any. If
 *               not, its ancestors will be browsed until a non-empty is found or that we are at an ix
 *               which has 32 ms of bInterval value. The scenarios below show what the lists should
 *               look like in some situations.
 *               - (a) Scenario where 3 endpoints with a bInterval of 32ms (HCD/HC ED ix: 0 once and 1
 *                     twice), one of 16ms (HCD ED ix: 33) and one of 2ms (HCD ED ix: 61) are opened:
 *						@verbatim
 *                       HCD_ED_ListsPtrTbl[0]                                   HccaInterruptTable[0]
 *                       (bInterval 32)   |                                              |
 *                                       V                                              V
 *                                       |----------|                           |---------|
 *                                       |  HCD ED  |-------------------------->|  HC ED  |
 *                                       |----------|                           |---------|
 *                                       |  |  ^ |                               |   | |
 *                                       |  |  |  \->|----------|  |---------|<-/    | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |  |  |     |----------|  |---------|       | |
 *                                       |  |  |          |             |            | |
 *                                       |  |  |          |             |            | |
 *                                       |  |  |          V             V            | |
 *                                       |  |  |     |----------|  |---------|       | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |  |  |     |----------|  |---------|       | |
 *                                       |  |  |          |             |            | |
 *                                       |  |  |          |             |            | |
 *                                       |  |  |          V             V            | |
 *                                       |  |  |     |----------|  |---------|       | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |   \------>|----------|  |---------|<-----/  |
 *                                       |                |             |              |
 *                                       |                |             |              |
 *                                       |                V             V              |
 *                                       |               NULL          NULL            |
 *                                       V                                             V
 *                                       NULL                                   |---------|
 *                                                                               |  Dummy  | <- Alway set to 'Skip'.
 *                                                                               |  HC ED  |
 *                                                                               |---------|
 *                                                                                       |
 *                                                                                       V
 *                                                                                   NULL
 *
 *                       HCD_ED_ListsPtrTbl[1]                                   HccaInterruptTable[1]
 *                       (bInterval 32)   |                                              |
 *                                       V                                              V
 *                                       |----------|                           |---------|
 *                                       |  HCD ED  |-------------------------->|  HC ED  |
 *                                       |----------|                           |---------|
 *                                       |  |  ^ |                               |   | |
 *                                       |  |  |  \->|----------|  |---------|<-/    | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |   \------>|----------|  |---------|<-----/  |
 *                                       |                |             |              |
 *                                       |                |             |              |
 *                                       |                V             V              |
 *                                       |               NULL          NULL            |
 *                                       V                                             V
 *                                       |----------|                           |---------|
 *                                       |  HCD ED  |-------------------------->|  HC ED  |
 *                                       |----------|                           |---------|
 *                                       |  |  ^ |                               |   | |
 *                                       |  |  |  \->|----------|  |---------|<-/    | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |  |  |     |----------|  |---------|       | |
 *                                       |  |  |          |             |            | |
 *                                       |  |  |          |             |            | |
 *                                       |  |  |          V             V            | |
 *                                       |  |  |     |----------|  |---------|       | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |   \------>|----------|  |---------|<-----/  |
 *                                       |                |             |              |
 *                                       |                |             |              |
 *                                       |                V             V              |
 *                                       |               NULL          NULL            |
 *                                       V                                             |
 *                                       NULL                                           |
 *                                                                                       |
 *                       HCD_ED_ListsPtrTbl[33]                                         |
 *                       (bInterval 16)   |                                             |
 *                                       V                                             V
 *                                       |----------|                           |---------|
 *                                       |  HCD ED  |-------------------------->|  HC ED  |
 *                                       |----------|                           |---------|
 *                                       |  |  ^ |                               |   | |
 *                                       |  |  |  \->|----------|  |---------|<-/    | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |   \------>|----------|  |---------|<-----/  |
 *                                       |                |             |              |
 *                                       |                |             |              |
 *                                       |                V             V              |
 *                                       |               NULL          NULL            |
 *                                       V                                             |
 *                                       NULL                                           |
 *                                                                                       |
 *                       HCD_ED_ListsPtrTbl[61]                                         |
 *                       (bInterval 2)    |                                             |
 *                                       V                                             V
 *                                       |----------|                           |---------|
 *                                       |  HCD ED  |-------------------------->|  HC ED  |
 *                                       |----------|                           |---------|
 *                                       |  |  ^ |                               |   | |
 *                                       |  |  |  \->|----------|  |---------|<-/    | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |   \------>|----------|  |---------|<-----/  |
 *                                       |                |             |              |
 *                                       |                |             |              |
 *                                       |                V             V              |
 *                                       |               NULL          NULL            |
 *                                       V                                             V
 *                                       NULL                                   |---------|
 *                                                                               |  Dummy  | <- Alway set to 'Skip'.
 *                                                                               |  HC ED  |
 *                                                                               |---------|
 *                                                                                       |
 *                                                                                       V
 *                                                                                   NULL
 *                       The example above shows that even though the HCD ED list at ix 1 only has 2 HCD
 *                       EDs in it, the HC ED linked list points to other HC EDs that are children of the
 *                       HC ED, in the binary tree.
 *                              @endverbatim
 *               - (b) Scenario where 1 endpoint with a bInterval of 4ms (HCD ED ix: 56) and one of 1ms
 *                     (HCD ED ix: 62) are opened:
 *                                          @verbatim
 *                       HCD_ED_ListsPtrTbl[56]                                 HccaInterruptTable[0, 4, 8, 16, 20, 24, 28]
 *                       (bInterval 4)    |                                             |
 *                                       V                                             V
 *                                       |----------|                           |---------|
 *                                       |  HCD ED  |-------------------------->|  HC ED  |
 *                                       |----------|                           |---------|
 *                                       |  |  ^ |                               |   | |
 *                                       |  |  |  \->|----------|  |---------|<-/    | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |  |  |     |----------|  |---------|       | |
 *                                       |  |  |          |             |            | |
 *                                       |  |  |          |             |            | |
 *                                       |  |  |          V             V            | |
 *                                       |  |  |     |----------|  |---------|       | |
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |   \------>|----------|  |---------|<-----/  |
 *                                       |                |             |              |
 *                                       |                |             |              |
 *                                       |                V             V              |
 *                                       |               NULL          NULL            |
 *                                       V                                             |
 *                                       NULL                                           |
 *                                                                                       |
 *                       HCD_ED_ListsPtrTbl[62]                                         | HccaInterruptTable[ 1,  2,  3,
 *                       (bInterval 1)    |                                             |  /                  5,  6,  7,
 *                                       V                                             V L                   9, 10, 11,
 *                                       |----------|                           |---------|                  13, 14, 15,
 *                                       |  HCD ED  |-------------------------->|  HC ED  |                  17, 18, 19,
 *                                       |----------|                           |---------|                  21, 22, 23,
 *                                       |  |  ^ |                               |   | |                    25, 26, 27,
 *                                       |  |  |  \->|----------|  |---------|<-/    | |                    29, 30, 31]
 *                                       |  |  \<----|  HCD TD  |->|  HC TD  |       | |
 *                                       |   \------>|----------|  |---------|<-----/  |
 *                                       |                |             |              |
 *                                       |                |             |              |
 *                                       |                V             V              |
 *                                       |               NULL          NULL            |
 *                                       V                                             V
 *                                       NULL                                   |---------|
 *                                                                               |  Dummy  | <- Alway set to 'Skip'.
 *                                                                               |  HC ED  |
 *                                                                               |---------|
 *                                                                                       |
 *                                                                                       V
 *                                                                                   NULL
 *                       Other HCD_ED_ListPtrTbl indexes are set to DEF_NULL.
 *                       @endverbatim
 *                       The HccaInterruptTable indexes that are pointing to the bInterval 4 HC ED are
 *                       ancestors of this ix. The ones that are pointing directly to the bInterval 1 HC
 *                       ED are not ancestors of the 4ms one but are ancestors of the 1ms one, so are the
 *                       indexes pointing to the 4 ms one, which is why the 4 ms HC ED points to the 1 ms
 *                       one afterwards.
 *******************************************************************************************************/
static void USBH_HCD_EP_Open(USBH_HC_DRV        *p_hc_drv,
                             USBH_DEV_HANDLE    dev_handle,
                             USBH_EP_HANDLE     ep_handle,
                             USBH_HCD_EP_PARAMS *p_hcd_ep_params,
                             void               **pp_hcd_ep_data,
                             RTOS_ERR           *p_err)
{
  USBH_OHCI_DATA   *p_data = (USBH_OHCI_DATA *)p_hc_drv->DataPtr;
  USBH_OHCI_HCD_ED **p_hcd_ed_tbl = p_data->HCD_ED_ListsPtrTbl;
  USBH_OHCI_HC_ED  *p_hc_ed;
  USBH_OHCI_HC_TD  *p_hc_td;
  USBH_OHCI_HCD_ED *p_hcd_ed;
  USBH_OHCI_HCD_TD *p_hcd_td;
  USBH_OHCI_HCD_ED *p_cur_hcd_ed;
  USBH_DEV_SPD     dev_spd = p_hcd_ep_params->DevSpd;
  CPU_INT16U       ep_max_pkt_size = p_hcd_ep_params->MaxPktSize;
  CPU_INT08U       ep_type = p_hcd_ep_params->Type;
  CPU_INT08U       ep_log_nbr = USBH_EP_ADDR_TO_LOG(p_hcd_ep_params->Addr);
  CPU_INT32U       hc_ed_ctrl;
  CPU_INT32U       hcd_ed_ix;
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  CPU_INT08U depth;
#endif
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  CPU_INT32U hcd_td_ix;
#endif

  USBH_OHCI_DBG_STATS_INC(EP_OpenExecCnt);

    #if (USBH_CFG_ISOC_EN == DEF_ENABLED)
  if (ep_type == USBH_EP_TYPE_ISOC) {                           // Isoc EP not supported.
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }
    #endif
  //                                                               Alloc HC ED.
  p_hc_ed = (USBH_OHCI_HC_ED *)Mem_DynPoolBlkGet(&p_data->HC_ED_MemPool,
                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);
    return;
  }
  //                                                               Alloc HCD ED.
  p_hcd_ed = (USBH_OHCI_HCD_ED *)Mem_DynPoolBlkGet(&p_data->HCD_ED_MemPool,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_EP_ALLOC);
    goto err_free_hc_ed;
  }
  //                                                               Alloc HC TD.
  p_hc_td = (USBH_OHCI_HC_TD *)Mem_DynPoolBlkGet(&p_data->HC_TD_MemPool,
                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
    goto err_free_eds;
  }
  //                                                               Alloc HCD TD.
  p_hcd_td = (USBH_OHCI_HCD_TD *)Mem_DynPoolBlkGet(&p_data->HCD_TD_MemPool,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
    goto err_free_eds_hc_td;
  }

    #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U bitmap_ix_max = (p_data->HCD_TD_PtrTblIxMax + (DEF_INT_32_NBR_BITS - 1u)) / DEF_INT_32_NBR_BITS;
    CPU_INT08U bitmap_ix;
    CORE_DECLARE_IRQ_STATE;

    hcd_td_ix = (USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MAX + 1u);
    CORE_ENTER_ATOMIC();
    for (bitmap_ix = 0u; bitmap_ix < bitmap_ix_max; bitmap_ix++) {
      if (DEF_BIT_IS_CLR_ANY(p_data->HCD_TD_PtrTblIxBitmapTbl[bitmap_ix], DEF_INT_32_MASK) == DEF_YES) {
        hcd_td_ix = DEF_INT_32_NBR_BITS - 1u - CPU_CntLeadZeros(~p_data->HCD_TD_PtrTblIxBitmapTbl[bitmap_ix]);

        DEF_BIT_SET(p_data->HCD_TD_PtrTblIxBitmapTbl[bitmap_ix], DEF_BIT(hcd_td_ix));
        hcd_td_ix += DEF_INT_32_NBR_BITS * bitmap_ix;
        break;
      }
    }
    CORE_EXIT_ATOMIC();
    if (hcd_td_ix <= USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MAX) {
      p_data->HCD_TD_PtrTbl[hcd_td_ix] = p_hcd_td;
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
      goto err_free_all_structs;
    }
  }
    #endif
  //                                                               Init HC_ED struct.
  hc_ed_ctrl = p_hcd_ep_params->DevAddr;
  hc_ed_ctrl |= ep_log_nbr << USBH_OHCI_ED_CTRL_EP_NBR_BIT_SHIFT;
  hc_ed_ctrl |= ep_max_pkt_size << USBH_OHCI_ED_CTRL_MAX_PKT_SIZE_BIT_SHIFT;
  hc_ed_ctrl |= ep_type << USBH_OHCI_ED_CTRL_EP_TYPE_BIT_SHIFT;
  DEF_BIT_SET(hc_ed_ctrl, USBH_OHCI_ED_CTRL_DIR_TD);            // Defer 'dir' indication to TD.
  DEF_BIT_SET(hc_ed_ctrl, ((dev_spd == USBH_DEV_SPD_FULL) ? USBH_OHCI_ED_CTRL_SPD_FULL : USBH_OHCI_ED_CTRL_SPD_LOW));
  DEF_BIT_SET(hc_ed_ctrl, USBH_OHCI_ED_CTRL_FMT_GEN);

  p_hc_ed->Ctrl = hc_ed_ctrl;
  p_hc_ed->TailHC_TD_Ptr = (CPU_REG32)p_hc_td;
  p_hc_ed->HeadHC_TD_Ptr = (CPU_REG32)p_hc_td;
  p_hc_ed->NextHC_ED_Ptr = (CPU_REG32)0;
  USBH_OHCI_DCACHE_FLUSH_HC_ED(p_hc_ed);

  p_hcd_ed->HC_ED_Ptr = p_hc_ed;
  p_hcd_ed->HeadHCD_TD_Ptr = p_hcd_td;
  p_hcd_ed->TailHCD_TD_Ptr = p_hcd_td;
  p_hcd_ed->NextHCD_ED_Ptr = DEF_NULL;
  p_hcd_ed->DevHandle = dev_handle;
  p_hcd_ed->EP_Handle = ep_handle;

  //                                                               TD q is empty, create 1st placeholder.
  //                                                               Init HC TD.
  p_hc_td->CurBufPtr = 0u;
  p_hc_td->NextHC_TD_Ptr = 0u;
  p_hc_td->BufEndPtr = 0u;

  p_hcd_td->XferStatus = 0u;
  p_hcd_td->HCD_ED_Ptr = p_hcd_ed;
  p_hcd_td->HC_TD_Ptr = p_hc_td;
  p_hcd_td->NextHCD_TD_Ptr = DEF_NULL;

  *pp_hcd_ep_data = (void *)p_hcd_ed;

  if (ep_type == USBH_EP_TYPE_CTRL) {                           // Ctrl EP open.
    hcd_ed_ix = USBH_OHCI_HCD_ED_LIST_CTRL;
  } else if (ep_type == USBH_EP_TYPE_BULK) {                    // Bulk EP open.
    hcd_ed_ix = USBH_OHCI_HCD_ED_LIST_BULK;
  } else {                                                      // Intr EP open.
        #if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    CPU_INT16U req_bw;                                          // Min req BW.
    CPU_INT08U ordered_browsing_ix;                             // Ordered ix used to browse the 32ms tree lvl.
    CPU_INT08U insert_ordered_ix;                               // Ordered ix at which the HC ED will be inserted.
    CPU_INT08U cur_group_best_ordered_ix;                       // Ordered ix of lowest BW used in a group.
    CPU_INT16U cur_group_lowest_bw;                             // Lowest  BW used in a group.
    CPU_INT16U cur_group_highest_bw;                            // Highest BW used in a group.
                                                                // Lowest  BW used in worst case of group in which ...
    CPU_INT16U lowest_worst_case_bw;                            // ... HC ED will be inserted.

    //                                                             Find depth at which the HC ED needs to be inserted.
    depth = USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_01MS
            - DEF_MIN((15u - CPU_CntLeadZeros16(p_hcd_ep_params->Interval)), USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_01MS);

    KAL_LockAcquire(p_data->PeriodicListLockHandle,
                    KAL_OPT_PEND_NONE,
                    KAL_TIMEOUT_INFINITE,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto err_free_all;
    }

    req_bw = ep_max_pkt_size + USBH_HC_BW_OVERHEAD_GET(dev_spd, ep_type);
    lowest_worst_case_bw = USBH_HC_MAX_PERIODIC_BW_GET(dev_spd);
    cur_group_highest_bw = 0u;
    cur_group_lowest_bw = USBH_HC_MAX_PERIODIC_BW_GET(dev_spd);

    //                                                             Find at which tree ix the HC ED should be inserted.
    //                                                             See Note #4.
    for (ordered_browsing_ix = 0u; ordered_browsing_ix < USBH_OHCI_HCCA_INTERRUPT_TABLE_SIZE; ++ordered_browsing_ix) {
      USBH_OHCI_HC_ED *p_cur_hc_ed;
      CPU_INT08U      cur_hc_ed_ix;
      CPU_INT16U      cur_hc_ed_total_bw = 0u;

      cur_hc_ed_ix = USBH_OHCI_HC_ED_IxBitNbrConvTbl[ordered_browsing_ix];
      p_cur_hc_ed = (USBH_OHCI_HC_ED *)p_data->HCCA_Ptr->HccaInterruptTable[cur_hc_ed_ix];

      while ((p_cur_hc_ed != DEF_NULL)
             && (p_cur_hc_ed != p_data->HC_ED_PeriodicDummyPtr)) {
        //                                                         Loop through HC EDs to compute BW used by each path.
        //                                                         If HC ED is the dummy, no more BW is used in path.
        cur_hc_ed_total_bw += USBH_OHCI_ED_CTRL_MAX_PKT_SIZE_GET(p_cur_hc_ed->Ctrl);
        //                                                         Add overhead depending of EP type and dev spd.
        if (DEF_BIT_IS_CLR(p_cur_hc_ed->Ctrl, USBH_OHCI_ED_CTRL_FMT_ISOC) == DEF_YES) {
          if (DEF_BIT_IS_CLR(p_cur_hc_ed->Ctrl, USBH_OHCI_ED_CTRL_SPD_LOW) == DEF_YES) {
            cur_hc_ed_total_bw += USBH_HC_BW_OVERHEAD_GET(USBH_DEV_SPD_FULL, USBH_EP_TYPE_INTR);
          } else {
            cur_hc_ed_total_bw += USBH_HC_BW_OVERHEAD_GET(USBH_DEV_SPD_LOW, USBH_EP_TYPE_INTR);
          }
        } else {
          cur_hc_ed_total_bw += USBH_HC_BW_OVERHEAD_GET(USBH_DEV_SPD_FULL, USBH_EP_TYPE_ISOC);
        }

        p_cur_hc_ed = (USBH_OHCI_HC_ED *)p_cur_hc_ed->NextHC_ED_Ptr;
      }

      //                                                           Keep highest BW used of a group.
      if (cur_hc_ed_total_bw > cur_group_highest_bw) {
        cur_group_highest_bw = cur_hc_ed_total_bw;
      }

      if (cur_hc_ed_total_bw < cur_group_lowest_bw) {           // Keep lowest BW used of a group and its ix.
        cur_group_lowest_bw = cur_hc_ed_total_bw;
        cur_group_best_ordered_ix = ordered_browsing_ix;
      }

      if (((ordered_browsing_ix + 1u) % (1u << depth)) == 0u) {
        //                                                         If a group has been completed.
        //                                                         Check if group worst BW is better than prev worst.
        if (cur_group_highest_bw < lowest_worst_case_bw) {
          //                                                       Keep lowest worst BW and the ix at which to insert.
          lowest_worst_case_bw = cur_group_highest_bw;
          insert_ordered_ix = cur_group_best_ordered_ix;
        }

        if (lowest_worst_case_bw == 0u) {                       // If a branch has 0 BW used, exit early.
          break;
        }
        cur_group_highest_bw = 0u;                              // Reset lowest and highest group BW.
        cur_group_lowest_bw = USBH_HC_MAX_PERIODIC_BW_GET(dev_spd);
      }
    }

    if (req_bw > (USBH_HC_MAX_PERIODIC_BW_GET(dev_spd) - lowest_worst_case_bw)) {
      //                                                           No branch has enough BW avail to open EP, fail.
      KAL_LockRelease(p_data->PeriodicListLockHandle,
                      p_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(*p_err),; );

      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_BW_NOT_AVAIL);
      goto err_free_all;
    }

    //                                                             At this point, insert_ordered_ix contains the ix ...
    //                                                             ... of the 32ms bInterval HC ED located at the   ...
    //                                                             ... head of the path in which the EP must be added.
    hcd_ed_ix = (USBH_OHCI_HC_ED_IxBitNbrConvTbl[insert_ordered_ix] & DEF_BIT_FIELD((CPU_INT08U)(USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_MAX - depth), 0u)) + USBH_OHCI_BinTreeInfoTbl[depth].BaseIx;
        #endif
  }

    #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  //                                                               Keep ix of associated HCD TD in HC TD.
  p_hc_td->Ctrl = (hcd_td_ix & USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MSK);
    #else
  //                                                               Keep ix of associated HCD ED in HC TD.
  p_hc_td->Ctrl = (hcd_ed_ix & USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MSK);
    #endif
  USBH_OHCI_DCACHE_FLUSH_HC_TD(p_hc_td);

  p_hcd_ed->Ix = hcd_ed_ix;
  p_cur_hcd_ed = p_hcd_ed_tbl[hcd_ed_ix];

  if (p_cur_hcd_ed == DEF_NULL) {                               // If list is empty.
    p_hcd_ed_tbl[hcd_ed_ix] = p_hcd_ed;                         // Insert HCD ED at start of list.

        #if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    if (ep_type == USBH_EP_TYPE_INTR) {
      USBH_OHCI_BIN_TREE_STATUS browse_status;

      //                                                           Init bin tree status struct, based on ix to insert.
      USBH_BinTreeInit(&browse_status, depth, hcd_ed_ix);

      do {
        if ((browse_status.Depth == USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_32MS)
            && (p_cur_hcd_ed == DEF_NULL)) {
          //                                                       No HCD ED under this 32ms ix. Must insert it.
          //                                                       There could be another HC ED, though. Keep list ok.
          p_hc_ed->NextHC_ED_Ptr = (CPU_REG32)p_data->HCCA_Ptr->HccaInterruptTable[hcd_ed_ix];
          USBH_OHCI_DCACHE_FLUSH_HC_ED_NEXT_ED_PTR(p_hc_ed);

          p_data->HCCA_Ptr->HccaInterruptTable[hcd_ed_ix] = (CPU_REG32)p_hc_ed;
          USBH_OHCI_DCACHE_FLUSH_HCCA_INTERRUPT_TABLE_IX(p_data->HCCA_Ptr, hcd_ed_ix);
        } else if (p_cur_hcd_ed != DEF_NULL) {                  // A HCD ED is already set. Insert new one after it.
          while (p_cur_hcd_ed->NextHCD_ED_Ptr != DEF_NULL) {
            p_cur_hcd_ed = p_cur_hcd_ed->NextHCD_ED_Ptr;
          }
          //                                                       Insert HC ED between last HC ED q'd at this ix ...
          //                                                       ... and the following one, from another ix.
          //                                                       Transfer HC ED from prev HCD ED to the HC ED to add.
          p_hc_ed->NextHC_ED_Ptr = p_cur_hcd_ed->HC_ED_Ptr->NextHC_ED_Ptr;
          USBH_OHCI_DCACHE_FLUSH_HC_ED_NEXT_ED_PTR(p_hc_ed);

          p_cur_hcd_ed->HC_ED_Ptr->NextHC_ED_Ptr = (CPU_REG32)p_hc_ed;
          USBH_OHCI_DCACHE_FLUSH_HC_ED_NEXT_ED_PTR(p_cur_hcd_ed->HC_ED_Ptr);

          //                                                       Update bin tree status struct.
          USBH_BinTreeUpdated(&browse_status, hcd_ed_ix);
        }

        //                                                         Obtain next ix and HCD ED, based on bin tree status.
        hcd_ed_ix = USBH_BinTreeNextED_IxGet(&browse_status);
        p_cur_hcd_ed = p_hcd_ed_tbl[hcd_ed_ix];
      } while (hcd_ed_ix != USBH_OHCI_BIN_TREE_ED_IX_NONE);
    }
        #endif
  } else {
    while (p_cur_hcd_ed->NextHCD_ED_Ptr != DEF_NULL) {          // Iterate to end of ED list.
      p_cur_hcd_ed = p_cur_hcd_ed->NextHCD_ED_Ptr;
    }

    p_cur_hcd_ed->NextHCD_ED_Ptr = p_hcd_ed;                    // Insert HC and HCD ED at end of lists.
    p_hc_ed->NextHC_ED_Ptr = p_cur_hcd_ed->HC_ED_Ptr->NextHC_ED_Ptr;
    USBH_OHCI_DCACHE_FLUSH_HC_ED_NEXT_ED_PTR(p_hc_ed);

    p_cur_hcd_ed->HC_ED_Ptr->NextHC_ED_Ptr = (CPU_REG32)p_hc_ed;
    USBH_OHCI_DCACHE_FLUSH_HC_ED_NEXT_ED_PTR(p_cur_hcd_ed->HC_ED_Ptr);
  }

    #if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  if (ep_type == USBH_EP_TYPE_INTR) {                           // Release periodic lock if it was acquired.
    KAL_LockRelease(p_data->PeriodicListLockHandle,
                    p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(*p_err),; );
  }
    #endif

  USBH_OHCI_DBG_STATS_INC(EP_OpenSuccessCnt);

  return;

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
err_free_all:
    #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)               // If OPT_SPD is enabled, reset HCD TD ix.
  {
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    p_data->HCD_TD_PtrTbl[hcd_td_ix] = DEF_NULL;
    DEF_BIT_CLR(p_data->HCD_TD_PtrTblIxBitmapTbl[hcd_td_ix / 32u], DEF_BIT(hcd_td_ix % DEF_INT_32_NBR_BITS));
    CORE_EXIT_ATOMIC();
  }
    #endif
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
err_free_all_structs:
#endif
  {
    RTOS_ERR err_lib;

#if ((USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED) \
    || (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
    Mem_DynPoolBlkFree(&p_data->HCD_TD_MemPool,
                       (void *)p_hcd_td,
                       &err_lib);
    PP_UNUSED_PARAM(err_lib);
#endif

err_free_eds_hc_td:
    Mem_DynPoolBlkFree(&p_data->HC_TD_MemPool,
                       (void *)p_hc_td,
                       &err_lib);
    PP_UNUSED_PARAM(err_lib);

err_free_eds:
    Mem_DynPoolBlkFree(&p_data->HCD_ED_MemPool,
                       (void *)p_hcd_ed,
                       &err_lib);
    PP_UNUSED_PARAM(err_lib);

err_free_hc_ed:
    Mem_DynPoolBlkFree(&p_data->HC_ED_MemPool,
                       (void *)p_hc_ed,
                       &err_lib);
    PP_UNUSED_PARAM(err_lib);
  }

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_Close()
 *
 * @brief    Close endpoint on OHCI host controller.
 *
 * @param    p_hc_drv        Pointer to host controller driver.
 *
 * @param    p_hcd_ep_data   Pointer to host controller driver's endpoint data.
 *
 * @param    p_err           Pointer to variable that will receive error code from this function :
 *                               - RTOS_ERR_NONE         Operation was successful.
 *                               - RTOS_ERR_FAIL         Required endpoint was not found or unable to
 *                                                       halt it.
 *                               - RTOS_ERR_EP_INVALID   Isoc endpoint not supported.
 *
 * @note     (1) This function assumes that no transfers (other than the mandatory empty one) are
 *               currently queued for this endpoint. That is, the Core has called URB_Abort() for all
 *               the transfers that were queued, before calling EP_Close().
 *
 * @note     (2) See OHCI Specification section 5.2.7.1.2 'Bulk and control endpoint descriptors lists
 *               removal' and section 5.2.7.1.2 'Interrupt endpoint descriptor lists removal' for more
 *               details.
 *******************************************************************************************************/
static void USBH_HCD_EP_Close(USBH_HC_DRV *p_hc_drv,
                              void        *p_hcd_ep_data,
                              RTOS_ERR    *p_err)
{
  USBH_OHCI_DATA   *p_data = (USBH_OHCI_DATA *)p_hc_drv->DataPtr;
  USBH_OHCI_REG    *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  USBH_OHCI_HCD_ED *p_to_remove_hcd_ed = (USBH_OHCI_HCD_ED *)p_hcd_ep_data;
  USBH_OHCI_HCD_TD *p_to_remove_hcd_td = p_to_remove_hcd_ed->HeadHCD_TD_Ptr;
  USBH_OHCI_HCD_ED *p_prev_hcd_ed = DEF_NULL;
  USBH_OHCI_HCD_ED *p_iter_hcd_ed;
  CPU_INT08U       ep_type = USBH_OHCI_ED_CTRL_EP_TYPE_GET(p_to_remove_hcd_ed->HC_ED_Ptr->Ctrl);
  CPU_BOOLEAN      hcd_ed_found = DEF_NO;
  CPU_BOOLEAN      skip_ok;
  CPU_REG32        *p_cur_reg_hc_ed;
  CPU_INT32U       hcd_ed_list_ix = p_to_remove_hcd_ed->Ix;
  CPU_INT32U       hc_ed_list_proc_msk;
  RTOS_ERR         err_lib;

  USBH_OHCI_DBG_STATS_INC(EP_CloseExecCnt);

  if (ep_type == USBH_EP_TYPE_CTRL) {                           // Get ix of list(s) to search for HCD ED to remove.
    hc_ed_list_proc_msk = USBH_OHCI_CTRL_CTRL_LIST_EN;
    p_cur_reg_hc_ed = (CPU_REG32 *)p_reg->HcControlCurrentED;
  } else if (ep_type == USBH_EP_TYPE_BULK) {
    hc_ed_list_proc_msk = USBH_OHCI_CTRL_BULK_LIST_EN;
    p_cur_reg_hc_ed = (CPU_REG32 *)p_reg->HcBulkCurrentED;
  } else if (ep_type == USBH_EP_TYPE_INTR) {                    // Intr EPs supported if periodic xfers are enabled.
    hc_ed_list_proc_msk = USBH_OHCI_CTRL_PERIODIC_LIST_EN;
    p_cur_reg_hc_ed = (CPU_REG32 *)p_reg->HcPeriodCurrentED;
  } else {                                                      // Isoc EPs never supported.
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_INVALID);
    return;
  }

  //                                                               Get ptr to head HCD ED of the HCD ED ptr list.
  p_iter_hcd_ed = p_data->HCD_ED_ListsPtrTbl[hcd_ed_list_ix];
  while (p_iter_hcd_ed != DEF_NULL) {                           // Loop through all subsequent HCD ED, to find it.
    if (p_iter_hcd_ed == p_to_remove_hcd_ed) {
      hcd_ed_found = DEF_YES;
      break;
    }
    p_prev_hcd_ed = p_iter_hcd_ed;
    p_iter_hcd_ed = p_iter_hcd_ed->NextHCD_ED_Ptr;
  }

  if (hcd_ed_found == DEF_NO) {                                 // If HCD ED to remove is not found, err.
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  }

    #if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  if (ep_type == USBH_EP_TYPE_INTR) {
    RTOS_ERR err_kal;

    KAL_LockAcquire(p_data->PeriodicListLockHandle,
                    KAL_OPT_PEND_NONE,
                    KAL_TIMEOUT_INFINITE,
                    &err_kal);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_kal) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err_kal),; );
  }
    #endif

  DEF_BIT_CLR(p_reg->HcControl, hc_ed_list_proc_msk);           // Stop processing of this list.
                                                                // Skip processing of this HC ED.
  skip_ok = USBH_HC_ED_Skip(p_hc_drv, p_to_remove_hcd_ed->HC_ED_Ptr);
  if (skip_ok != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);

        #if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
    if (ep_type == USBH_EP_TYPE_INTR) {
      RTOS_ERR err_kal;

      KAL_LockRelease(p_data->PeriodicListLockHandle,
                      &err_kal);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_kal) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err_kal),; );
    }
        #endif

    return;
  }
  //                                                               Cur ED reg should not point to the ED to remove.
  if (*p_cur_reg_hc_ed == (CPU_REG32)p_to_remove_hcd_ed->HC_ED_Ptr) {
    *p_cur_reg_hc_ed = ((USBH_OHCI_HC_ED *)p_cur_reg_hc_ed)->NextHC_ED_Ptr;
  }

  //                                                               Unlink the ED from the list.
  if (p_prev_hcd_ed == DEF_NULL) {
    //                                                             Update head reg.
    if (ep_type == USBH_EP_TYPE_CTRL) {
      //                                                           ED is head of list, update head to point to next ED.
      p_data->HCD_ED_ListsPtrTbl[hcd_ed_list_ix] = p_to_remove_hcd_ed->NextHCD_ED_Ptr;
      if (p_data->HCD_ED_ListsPtrTbl[hcd_ed_list_ix] != DEF_NULL) {
        p_reg->HcControlHeadED = (CPU_REG32)p_data->HCD_ED_ListsPtrTbl[hcd_ed_list_ix]->HC_ED_Ptr;
      } else {
        p_reg->HcControlHeadED = 0u;
      }
    } else if (ep_type == USBH_EP_TYPE_BULK) {
      //                                                           ED is head of list, update head to point to next ED.
      p_data->HCD_ED_ListsPtrTbl[hcd_ed_list_ix] = p_to_remove_hcd_ed->NextHCD_ED_Ptr;
      if (p_data->HCD_ED_ListsPtrTbl[hcd_ed_list_ix] != DEF_NULL) {
        p_reg->HcBulkHeadED = (CPU_REG32)p_data->HCD_ED_ListsPtrTbl[hcd_ed_list_ix]->HC_ED_Ptr;
      } else {
        p_reg->HcBulkHeadED = 0u;
      }
    } else {
            #if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
      USBH_OHCI_BIN_TREE_STATUS browse_status;
      USBH_OHCI_HCD_ED          *p_cur_hcd_ed;
      USBH_OHCI_HC_ED           *p_to_remove_hc_ed = p_to_remove_hcd_ed->HC_ED_Ptr;
      CPU_INT08U                hcd_ed_list_ix_start = hcd_ed_list_ix;

      USBH_BinTreeInit(&browse_status,                          // Init bin tree browsing struct, based on depth and ix.
                       USBH_OHCI_HCD_ED_IX_DEPTH_GET(hcd_ed_list_ix),
                       hcd_ed_list_ix);

      while (hcd_ed_list_ix != USBH_OHCI_BIN_TREE_ED_IX_NONE) {
        p_cur_hcd_ed = p_data->HCD_ED_ListsPtrTbl[hcd_ed_list_ix];

        while (p_cur_hcd_ed != DEF_NULL) {                      // Iter through HC EDs to find the one to remove.
          if (p_cur_hcd_ed->HC_ED_Ptr->NextHC_ED_Ptr == (CPU_INT32U)p_to_remove_hc_ed) {
            p_cur_hcd_ed->NextHCD_ED_Ptr = p_to_remove_hcd_ed->NextHCD_ED_Ptr;
            p_cur_hcd_ed->HC_ED_Ptr->NextHC_ED_Ptr = p_to_remove_hc_ed->NextHC_ED_Ptr;
            USBH_OHCI_DCACHE_FLUSH_HC_ED_NEXT_ED_PTR(p_cur_hcd_ed->HC_ED_Ptr);
            //                                                     Indicate that this ix has been updated.
            USBH_BinTreeUpdated(&browse_status, hcd_ed_list_ix);
            break;
          }
          p_cur_hcd_ed = p_cur_hcd_ed->NextHCD_ED_Ptr;
        }
        //                                                         Check if depth is 32ms and HCD ED is still NULL.
        if ((browse_status.Depth == USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_32MS)
            && (p_cur_hcd_ed == DEF_NULL)) {
          USBH_OHCI_HC_ED *p_cur_hc_ed = (USBH_OHCI_HC_ED *)p_data->HCCA_Ptr->HccaInterruptTable[hcd_ed_list_ix];

          //                                                       If HC ED to rem is at head, update ptrs.
          if (p_cur_hc_ed == p_to_remove_hc_ed) {
            p_data->HCCA_Ptr->HccaInterruptTable[hcd_ed_list_ix] = p_to_remove_hc_ed->NextHC_ED_Ptr;
            USBH_OHCI_DCACHE_FLUSH_HCCA_INTERRUPT_TABLE_IX(p_data->HCCA_Ptr, hcd_ed_list_ix);
          } else {
            //                                                     Search HC EDs under the first one for the one to rem.
            while ((p_cur_hc_ed != DEF_NULL)
                   && (p_cur_hc_ed != p_data->HC_ED_PeriodicDummyPtr)) {
              if (p_cur_hc_ed->NextHC_ED_Ptr == (CPU_INT32U)p_to_remove_hc_ed) {
                p_cur_hc_ed->NextHC_ED_Ptr = p_to_remove_hc_ed->NextHC_ED_Ptr;
                USBH_OHCI_DCACHE_FLUSH_HC_ED_NEXT_ED_PTR(p_cur_hc_ed);
                break;
              }
              p_cur_hc_ed = (USBH_OHCI_HC_ED *)p_cur_hc_ed->NextHC_ED_Ptr;
            }
          }
        }
        //                                                         Get next HCD ED ix, based on bin tree status.
        hcd_ed_list_ix = USBH_BinTreeNextED_IxGet(&browse_status);
      }
      p_data->HCD_ED_ListsPtrTbl[hcd_ed_list_ix_start] = p_to_remove_hcd_ed->NextHCD_ED_Ptr;
            #endif
    }
  } else {                                                      // Cur ED found in middle of list. Update ptrs.
                                                                // Unlink ED. The prev ED should point to next of ...
                                                                // ... cur ED, for both HC and HCD EDs.
    p_prev_hcd_ed->NextHCD_ED_Ptr = p_to_remove_hcd_ed->NextHCD_ED_Ptr;
    p_prev_hcd_ed->HC_ED_Ptr->NextHC_ED_Ptr = p_to_remove_hcd_ed->HC_ED_Ptr->NextHC_ED_Ptr;
    USBH_OHCI_DCACHE_FLUSH_HC_ED_NEXT_ED_PTR(p_prev_hcd_ed->HC_ED_Ptr);
  }

  DEF_BIT_SET(p_reg->HcControl, hc_ed_list_proc_msk);           // Re-en list processing now that ED has been removed.

    #if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
  if (ep_type == USBH_EP_TYPE_INTR) {
    RTOS_ERR err_kal;

    KAL_LockRelease(p_data->PeriodicListLockHandle,
                    &err_kal);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_kal) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err_kal),; );
  }
    #endif

  while (p_to_remove_hcd_td != DEF_NULL) {                      // Iterate through TDs q'd under the ED and free them.
    USBH_OHCI_HCD_TD *p_next_hcd_td = p_to_remove_hcd_td->NextHCD_TD_Ptr;

    USBH_HCD_TD_Free(p_hc_drv, p_to_remove_hcd_td);
    p_to_remove_hcd_td = p_next_hcd_td;
  }

  Mem_DynPoolBlkFree(&p_data->HC_ED_MemPool,
                     (void *)p_to_remove_hcd_ed->HC_ED_Ptr,
                     &err_lib);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err_lib),; );

  p_to_remove_hcd_ed->DevHandle = USBH_DEV_HANDLE_INVALID;
  p_to_remove_hcd_ed->EP_Handle = USBH_EP_HANDLE_INVALID;

  Mem_DynPoolBlkFree(&p_data->HCD_ED_MemPool,
                     (void *)p_to_remove_hcd_ed,
                     &err_lib);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err_lib),; );

  USBH_OHCI_DBG_STATS_INC(EP_CloseSuccessCnt);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_Suspend()
 *
 * @brief    Suspends/resumes endpoint processing.
 *
 * @param    p_hc_drv        Pointer to host controller driver.
 *
 * @param    p_hcd_ep_data   Pointer to host controller driver's endpoint data.
 *
 * @param    suspend         Flag that indicates if endpoint is suspened or resumed.
 *                           DEF_YES                 Endpoint suspended.
 *                           DEF_NO                  Endpoint resumed.
 *
 * @param    p_err           Pointer to variable that will receive error code from this function :
 *                               - RTOS_ERR_NONE           Operation was successful.
 *******************************************************************************************************/
static void USBH_HCD_EP_Suspend(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN suspend,
                                RTOS_ERR    *p_err)
{
  USBH_OHCI_REG    *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  USBH_OHCI_HCD_ED *p_hcd_ed = (USBH_OHCI_HCD_ED *)p_hcd_ep_data;
  USBH_OHCI_HC_ED  *p_hc_ed = p_hcd_ed->HC_ED_Ptr;

  if (suspend == DEF_YES) {                                     // Suspend EP processing.
    DEF_BIT_SET(p_hc_ed->Ctrl, USBH_OHCI_ED_CTRL_SKIP);
  } else {                                                      // Resume EP processing.
    CPU_INT08U ep_type = USBH_OHCI_ED_CTRL_EP_TYPE_GET(p_hcd_ed->HC_ED_Ptr->Ctrl);

    DEF_BIT_CLR(p_hc_ed->Ctrl, USBH_OHCI_ED_CTRL_SKIP);

    if (ep_type == USBH_EP_TYPE_CTRL) {
      DEF_BIT_SET(p_reg->HcCommandStatus, USBH_OHCI_CMD_STATUS_CTRL_LIST_FILLED);
    } else if (ep_type == USBH_EP_TYPE_BULK) {
      DEF_BIT_SET(p_reg->HcCommandStatus, USBH_OHCI_CMD_STATUS_BULK_LIST_FILLED);
    }
  }

  USBH_OHCI_DCACHE_FLUSH_HC_ED_CTRL(p_hc_ed);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_HaltClr()
 *
 * @brief    Clear halt condition on specified endpoint.
 *
 * @param    p_hc_drv            Pointer to host controller driver.
 *
 * @param    p_hcd_ep_data       Pointer to host controller driver's endpoint data.
 *
 * @param    data_toggle_clr     Flag that indicates if the data toggle needs to be cleared.
 *                               DEF_YES                 Data toggle must     be cleared.
 *                               DEF_NO                  Data toggle must not be cleared.
 *
 * @param    p_err               Pointer to variable that will receive error code from this function :
 *                                   - RTOS_ERR_NONE           Operation was successful.
 *******************************************************************************************************/
static void USBH_HCD_EP_HaltClr(USBH_HC_DRV *p_hc_drv,
                                void        *p_hcd_ep_data,
                                CPU_BOOLEAN data_toggle_clr,
                                RTOS_ERR    *p_err)
{
  USBH_OHCI_HCD_ED *p_hcd_ed = (USBH_OHCI_HCD_ED *)p_hcd_ep_data;
  USBH_OHCI_HC_ED  *p_hc_ed = p_hcd_ed->HC_ED_Ptr;

  (void)&p_hc_drv;

  USBH_OHCI_DCACHE_INV_HC_ED_HEAD_PTR(p_hc_ed);
  if (data_toggle_clr == DEF_YES) {                             // Reset data toggle if req'd.
    DEF_BIT_CLR(p_hc_ed->HeadHC_TD_Ptr, USBH_OHCI_ED_HEAD_PTR_TOGGLE_CARRY);
  }

  //                                                               Resume EP processing.
  DEF_BIT_CLR(p_hc_ed->Ctrl, USBH_OHCI_ED_CTRL_SKIP);
  DEF_BIT_CLR(p_hc_ed->HeadHC_TD_Ptr, USBH_OHCI_ED_HEAD_PTR_HALT);

  USBH_OHCI_DCACHE_FLUSH_HC_ED_CTRL(p_hc_ed);
  USBH_OHCI_DCACHE_FLUSH_HC_ED_HEAD_TD_PTR(p_hc_ed);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HCD_URB_Submit()
 *
 * @brief    Create the TD for the URB, insert the TDs into the appropriate endpoint descriptor,
 *           and enable the host controller to begin processing the lists
 *
 * @param    p_hc_drv            Pointer to host controller driver.
 *
 * @param    p_hcd_ep_data       Pointer to host controller driver's endpoint data.
 *
 * @param    pp_hcd_urb_data     Pointer to memory location where a pointer to the host controller
 *                               driver's URB data will be kept.
 *
 * @param    p_hcd_urb_params    Pointer to structure containing the URB parameters.
 *
 * @param    p_err               Pointer to variable that will receive error code from this function :
 *                                   - RTOS_ERR_NONE             Operation was successful.
 *                                   - RTOS_ERR_DRV_URB_ALLOC    Transfer descriptor allocation failed.
 *
 * @note     (1) If dedicated memory is required and that the buffer passed from the core is not
 *               within the dedicated memory boudaries, a buffer from the dedicated memory is needed.
 *
 *               - (a) If the transfer would span on more than one TD and dedicated memory is needed,
 *                     the call must fail. This is to prevent the cases where a short packet is received
 *                     and that associated TDs needs to be freed in the ISR. Since URB parameters are
 *                     not easily available in that context, it is not easy to determine if the buffer
 *                     comes from the dedicated memory buffer pool or not. To reduce complexity, it has
 *                     been decided to prevent having multiple-TDs-transfers when using dedicated
 *                     memory.
 *******************************************************************************************************/
static void USBH_HCD_URB_Submit(USBH_HC_DRV         *p_hc_drv,
                                void                *p_hcd_ep_data,
                                void                **pp_hcd_urb_data,
                                USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                RTOS_ERR            *p_err)
{
  USBH_OHCI_DATA   *p_data = (USBH_OHCI_DATA *)p_hc_drv->DataPtr;
  USBH_OHCI_REG    *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  USBH_OHCI_HCD_ED *p_hcd_ed = (USBH_OHCI_HCD_ED *)p_hcd_ep_data;
  CPU_INT08U       ep_type = USBH_OHCI_ED_CTRL_EP_TYPE_GET(p_hcd_ed->HC_ED_Ptr->Ctrl);
  CPU_INT32U       first_td_toggle;
  CPU_INT32U       token;

  USBH_OHCI_DBG_STATS_INC(URB_SubmitExecCnt);

  if (ep_type == USBH_EP_TYPE_CTRL) {                           // Adjust data toggle.
    if (p_hcd_urb_params->Token == USBH_TOKEN_SETUP) {
      first_td_toggle = USBH_OHCI_TD_CTRL_DATA_TOGGLE_FORCE_DATA0;
      token = USBH_OHCI_TD_CTRL_DIR_PID_SETUP;
    } else {
      token = (p_hcd_urb_params->Token == USBH_TOKEN_OUT)
              ? USBH_OHCI_TD_CTRL_DIR_PID_OUT : USBH_OHCI_TD_CTRL_DIR_PID_IN;
      first_td_toggle = USBH_OHCI_TD_CTRL_DATA_TOGGLE_FORCE_DATA1;
    }
  } else {
    token = (p_hcd_urb_params->Token == USBH_TOKEN_OUT)
            ? USBH_OHCI_TD_CTRL_DIR_PID_OUT : USBH_OHCI_TD_CTRL_DIR_PID_IN;
    //                                                             Get data toggle from ED.
    first_td_toggle = USBH_OHCI_TD_CTRL_DATA_TOGGLE_ED_CARRY;
  }

  if ((p_hcd_urb_params->Token == USBH_TOKEN_SETUP)
      || (p_hcd_urb_params->Token == USBH_TOKEN_OUT)) {
    CPU_DCACHE_RANGE_FLUSH((void *)p_hcd_urb_params->BufPtr, p_hcd_urb_params->BufLen);
  }

  USBH_HCD_TD_Insert(p_hc_drv,                                  // Insert TD into ED's TD list.
                     p_hcd_ed,
                     pp_hcd_urb_data,
                     (token | first_td_toggle | USBH_OHCI_CMPL_CODE_NOT_ACCESSED | USBH_OHCI_TD_CTRL_DLY_INT_NO_DLY),
                     (CPU_INT32U)p_hcd_urb_params->BufPtr,
                     p_hcd_urb_params->BufLen,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  switch (ep_type) {
    case USBH_EP_TYPE_CTRL:
      //                                                           Wr ctrl list addr to HcControlHeadED.
      p_reg->HcControlHeadED = (CPU_REG32)p_data->HCD_ED_ListsPtrTbl[USBH_OHCI_HCD_ED_LIST_CTRL]->HC_ED_Ptr;
      //                                                           En ctrl list filled and ctrl list proc.
      DEF_BIT_SET(p_reg->HcCommandStatus, USBH_OHCI_CMD_STATUS_CTRL_LIST_FILLED);
      DEF_BIT_SET(p_reg->HcControl, USBH_OHCI_CTRL_CTRL_LIST_EN);
      break;

    case USBH_EP_TYPE_BULK:
      //                                                           Wr ctrl list addr to BulkHeadED.
      p_reg->HcBulkHeadED = (CPU_REG32)p_data->HCD_ED_ListsPtrTbl[USBH_OHCI_HCD_ED_LIST_BULK]->HC_ED_Ptr;
      //                                                           En bulk list filled and bulk list proc.
      DEF_BIT_SET(p_reg->HcCommandStatus, USBH_OHCI_CMD_STATUS_BULK_LIST_FILLED);
      DEF_BIT_SET(p_reg->HcControl, USBH_OHCI_CTRL_BULK_LIST_EN);
      break;

    case USBH_EP_TYPE_INTR:
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
      DEF_BIT_SET(p_reg->HcControl, USBH_OHCI_CTRL_PERIODIC_LIST_EN);
      break;
#endif

    case USBH_EP_TYPE_ISOC:
    default:
      break;
  }

  USBH_OHCI_DBG_STATS_INC(URB_SubmitSuccessCnt);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HCD_URB_Complete()
 *
 * @brief    Determine the number bytes transfered in this URB, remove the TD containing this URB,
 *           set the transfer condition to successful or failed.
 *
 * @param    p_hc_drv            Pointer to host controller driver.
 *
 * @param    p_hcd_ep_data       Pointer to host controller driver's endpoint data.
 *
 * @param    p_hcd_urb_data      Pointer to host controller driver's URB data.
 *
 * @param    p_hcd_urb_params    Pointer to structure containing the URB parameters.
 *
 * @param    p_err               Pointer to variable that will receive error code from this function :
 *                                   - RTOS_ERR_NONE           Operation was successful.
 *                                   - RTOS_ERR_FAIL           Calling complete on a transfer that has not
 *                                                             completed yet.
 *                                   - RTOS_ERR_NOT_FOUND      TD was not found at top of list.
 *                                   - RTOS_ERR_IO
 *                                   - RTOS_ERR_RX_OVERRUN
 *                                   - RTOS_ERR_EP_STALL
 *                                   - RTOS_ERR_FAIL
 *
 * @return   Number of bytes transferred.
 *******************************************************************************************************/
static CPU_INT32U USBH_HCD_URB_Complete(USBH_HC_DRV         *p_hc_drv,
                                        void                *p_hcd_ep_data,
                                        void                *p_hcd_urb_data,
                                        USBH_HCD_URB_PARAMS *p_hcd_urb_params,
                                        RTOS_ERR            *p_err)
{
  USBH_OHCI_DATA   *p_data = (USBH_OHCI_DATA *)p_hc_drv->DataPtr;
  USBH_OHCI_HCD_ED *p_hcd_ed = (USBH_OHCI_HCD_ED *)p_hcd_ep_data;
  USBH_OHCI_HCD_TD *p_hcd_td = (USBH_OHCI_HCD_TD *)p_hcd_urb_data;
  CPU_INT32U       urb_buf_addr = (CPU_INT32U)(p_hcd_urb_params->BufPtr);
  CPU_INT32U       cmpl_code;
  CPU_INT32U       xfer_len = 0u;
  CPU_BOOLEAN      is_last_hcd_td = DEF_NO;
  CPU_BOOLEAN      dedicated_mem_use = DEF_NO;

  USBH_OHCI_DBG_STATS_INC(URB_CompleteExecCnt);

  if (DEF_BIT_IS_CLR(p_hcd_td->XferStatus, USBH_OHCI_HCD_TD_XFER_STATUS_IS_FINISHED) == DEF_YES) {
    LOG_ERR(("Trying to call URB_Complete() on TD not marked as finished."));
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return (0u);
  }

  if (p_hcd_td != p_hcd_ed->HeadHCD_TD_Ptr) {                   // TD should be at head of ED list.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    LOG_DBG(("Host Controller could not find HCD TD when completing ", (u)(CPU_INT32U)p_hcd_td));
    return (0u);
  }

  //                                                               Check if dedicated mem is used.
  if ((p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr != DEF_NULL)
      && (p_hcd_urb_params->BufLen != 0u)) {
    CPU_INT32U urb_buf_end_addr = urb_buf_addr + p_hcd_urb_params->BufLen - 1u;

    //                                                             Check if original buf is comprised within ded mem.
    if ((urb_buf_addr < p_data->DedicatedMemDataBufStartAddr)
        || (urb_buf_end_addr > p_data->DedicatedMemDataBufEndAddr)) {
      dedicated_mem_use = DEF_YES;
    }
  }

  do {
    USBH_OHCI_HCD_TD *p_cur_hcd_td = p_hcd_ed->HeadHCD_TD_Ptr;
    USBH_OHCI_HC_TD  *p_cur_hc_td = p_cur_hcd_td->HC_TD_Ptr;
    CPU_INT32U       td_len = 0u;
    CPU_INT32U       hc_td_buf_end_addr;

    USBH_OHCI_DCACHE_INV_HC_TD(p_cur_hc_td);
    hc_td_buf_end_addr = p_cur_hc_td->BufEndPtr;

    cmpl_code = (p_cur_hc_td->Ctrl & USBH_OHCI_TD_CTRL_CMPL_CODE_MSK);
    if (cmpl_code != USBH_OHCI_CMPL_CODE_NOT_ACCESSED) {
      CPU_INT32U hc_td_buf_cur_addr = p_cur_hc_td->CurBufPtr;

      td_len = p_cur_hcd_td->XferStatus & USBH_OHCI_HCD_TD_XFER_STATUS_LEN_MSK;
      if (hc_td_buf_cur_addr != 0u) {                           // TD partially xfer'd, compute len.
        td_len -= (hc_td_buf_end_addr - hc_td_buf_cur_addr + 1u);
      }
    }

    if (DEF_BIT_IS_CLR(p_cur_hcd_td->XferStatus, USBH_OHCI_HCD_TD_XFER_STATUS_IS_NOT_LAST) == DEF_YES) {
      is_last_hcd_td = DEF_YES;                                 // This is the last TD associated to this xfer/URB.
    }

    if (dedicated_mem_use == DEF_YES) {
      CPU_INT32U hc_td_buf_start_addr = hc_td_buf_end_addr - (p_cur_hcd_td->XferStatus & USBH_OHCI_HCD_TD_XFER_STATUS_LEN_MSK) + 1u;
      RTOS_ERR   err_lib;

      if (p_hcd_urb_params->Token == USBH_TOKEN_IN) {
        //                                                         Invalidate cache on buf used before copying.
        CPU_DCACHE_RANGE_INV((void *)hc_td_buf_start_addr, td_len);
        //                                                         Copy data rx'd to URB buf.
        Mem_Copy((void *)urb_buf_addr,
                 (void *)hc_td_buf_start_addr,
                 td_len);
        urb_buf_addr += td_len;
      }

      Mem_DynPoolBlkFree(&p_data->XferBufPool,
                         (void *)hc_td_buf_start_addr,
                         &err_lib);
      //                                                           Critical err if unable to free completed ...
      //                                                           dedicated memory block.
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err_lib), 0u);
    }

    xfer_len += td_len;
    //                                                             TD to remove is at head of ED list.
    p_hcd_ed->HeadHCD_TD_Ptr = p_cur_hcd_td->NextHCD_TD_Ptr;

    USBH_HCD_TD_Free(p_hc_drv, p_cur_hcd_td);
  } while (is_last_hcd_td != DEF_YES);

  if ((p_hcd_urb_params->Token == USBH_TOKEN_IN)                // Invalidate cache on whole xfer if no dedicated mem.
      && (dedicated_mem_use == DEF_NO)) {
    CPU_DCACHE_RANGE_INV((void *)p_hcd_urb_params->BufPtr, xfer_len);
  }

  switch (cmpl_code) {
    case USBH_OHCI_CMPL_CODE_NO_ERROR:
    case USBH_OHCI_CMPL_CODE_DATA_UNDERRUN:
    case USBH_OHCI_CMPL_CODE_NOT_ACCESSED:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case USBH_OHCI_CMPL_CODE_CRC:
    case USBH_OHCI_CMPL_CODE_BIT_STUFFING:
    case USBH_OHCI_CMPL_CODE_DATA_TOGGLE_MISMATCH:
    case USBH_OHCI_CMPL_CODE_PID_CHK_FAILURE:
    case USBH_OHCI_CMPL_CODE_UNEXPECTED_PID:
    case USBH_OHCI_CMPL_CODE_BUF_OVERRUN:
    case USBH_OHCI_CMPL_CODE_BUF_UNDERRUN:
    case USBH_OHCI_CMPL_CODE_DEV_NOT_RESPONDING:
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      break;

    case USBH_OHCI_CMPL_CODE_DATA_OVERRUN:
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX_OVERRUN);
      break;

    case USBH_OHCI_CMPL_CODE_STALL:
      RTOS_ERR_SET(*p_err, RTOS_ERR_EP_STALL);
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      break;
  }

  if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)              // Clr halt if req'd.
      && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_IO)) {
    RTOS_ERR local_err;

    USBH_HCD_EP_HaltClr(p_hc_drv,
                        (void *)p_hcd_ed,
                        DEF_NO,
                        &local_err);
    PP_UNUSED_PARAM(local_err);
  }

  USBH_OHCI_DBG_STATS_INC_IF_TRUE(URB_CompleteSuccessCnt, (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE));

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_URB_Abort()
 *
 * @brief    Abort specific URB on given endpoint.
 *
 * @param    p_hc_drv            Pointer to host controller driver.
 *
 * @param    p_hcd_ep_data       Pointer to host controller driver's endpoint data.
 *
 * @param    p_hcd_urb_data      Pointer to host controller driver's URB data.
 *
 * @param    p_hcd_urb_params    Pointer to structure containing the URB parameters.
 *******************************************************************************************************/
static void USBH_HCD_URB_Abort(USBH_HC_DRV         *p_hc_drv,
                               void                *p_hcd_ep_data,
                               void                *p_hcd_urb_data,
                               USBH_HCD_URB_PARAMS *p_hcd_urb_params)
{
  USBH_OHCI_DATA   *p_data = (USBH_OHCI_DATA *) p_hc_drv->DataPtr;
  USBH_OHCI_HCD_ED *p_hcd_ed = (USBH_OHCI_HCD_ED *) p_hcd_ep_data;
  USBH_OHCI_HCD_TD *p_hcd_td = (USBH_OHCI_HCD_TD *) p_hcd_urb_data;
  USBH_OHCI_HCD_TD *p_cur_hcd_td = p_hcd_ed->HeadHCD_TD_Ptr;
  USBH_OHCI_HCD_TD *p_prev_hcd_td;
  CPU_INT32U       urb_buf_addr = (CPU_INT32U)(p_hcd_urb_params->BufPtr);
  CPU_BOOLEAN      is_last_hcd_td = DEF_NO;
  CPU_BOOLEAN      dedicated_mem_use = DEF_NO;
  RTOS_ERR         local_err;

  USBH_OHCI_DBG_STATS_INC(URB_AbortExecCnt);

  USBH_HC_ED_Skip(p_hc_drv,                                     // Pause ED, host ctrlr skips proc this ED.
                  p_hcd_ed->HC_ED_Ptr);

  if ((p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr != DEF_NULL)
      && (p_hcd_urb_params->BufLen != 0u)) {
    CPU_INT32U urb_buf_end_addr = urb_buf_addr + p_hcd_urb_params->BufLen - 1u;

    //                                                             Check if original buf is comprised within ded mem.
    if ((urb_buf_addr < p_data->DedicatedMemDataBufStartAddr)
        || (urb_buf_end_addr > p_data->DedicatedMemDataBufEndAddr)) {
      dedicated_mem_use = DEF_YES;
    }
  }

  while ((p_cur_hcd_td != DEF_NULL)                             // Loop through HCD TDs to find the right one to abort.
         && (p_cur_hcd_td != p_hcd_td)) {
    p_prev_hcd_td = p_cur_hcd_td;
    p_cur_hcd_td = p_cur_hcd_td->NextHCD_TD_Ptr;
  }

  if (p_cur_hcd_td != p_hcd_td) {                               // If TD was not found, return with err.
    LOG_DBG(("Host Controller could not find HCD TD in lists when aborting ", (u)(CPU_INT32U)p_hcd_td));
    p_cur_hcd_td = p_hcd_td;
  }

  do {
    USBH_OHCI_HCD_TD *p_next_hcd_td = p_cur_hcd_td->NextHCD_TD_Ptr;
    USBH_OHCI_HC_TD  *p_cur_hc_td = p_cur_hcd_td->HC_TD_Ptr;
    CPU_INT32U       hc_td_buf_end_addr;

    USBH_OHCI_DCACHE_INV_HC_TD(p_cur_hc_td);
    hc_td_buf_end_addr = p_cur_hc_td->BufEndPtr;

    if (DEF_BIT_IS_CLR(p_cur_hcd_td->XferStatus, USBH_OHCI_HCD_TD_XFER_STATUS_IS_NOT_LAST) == DEF_YES) {
      is_last_hcd_td = DEF_YES;                                 // This is the last TD associated to this xfer/URB.
    }

    if (dedicated_mem_use == DEF_YES) {
      CPU_INT32U hc_td_buf_start_addr = hc_td_buf_end_addr - (p_cur_hcd_td->XferStatus & USBH_OHCI_HCD_TD_XFER_STATUS_LEN_MSK) + 1u;
      RTOS_ERR   err_lib;

      Mem_DynPoolBlkFree(&p_data->XferBufPool,
                         (void *)hc_td_buf_start_addr,
                         &err_lib);
      //                                                           Critical err if unable to free completed ...
      //                                                           dedicated memory block.
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err_lib),; );
    }

    if (p_cur_hcd_td == p_hcd_ed->HeadHCD_TD_Ptr) {             // If TD to remove is at head of ED list.
      p_hcd_ed->HeadHCD_TD_Ptr = p_next_hcd_td;
      USBH_OHCI_DCACHE_INV_HC_ED_HEAD_PTR(p_hcd_ed->HC_ED_Ptr);
      p_hcd_ed->HC_ED_Ptr->HeadHC_TD_Ptr = (((CPU_REG32)p_next_hcd_td->HC_TD_Ptr)
                                            | ((CPU_REG32)p_hcd_ed->HC_ED_Ptr->HeadHC_TD_Ptr & USBH_OHCI_ED_HEAD_PTR_TOGGLE_CARRY));
      USBH_OHCI_DCACHE_FLUSH_HC_ED_HEAD_TD_PTR(p_hcd_ed->HC_ED_Ptr);
    } else {                                                    // If TD to remove is in the middle of ED list.
      p_prev_hcd_td->NextHCD_TD_Ptr = p_next_hcd_td;
      p_prev_hcd_td->HC_TD_Ptr->NextHC_TD_Ptr = (CPU_REG32)p_next_hcd_td->HC_TD_Ptr;
      USBH_OHCI_DCACHE_FLUSH_HC_TD_NEXT_TD_PTR(p_prev_hcd_td->HC_TD_Ptr);
    }

    USBH_HCD_TD_Free(p_hc_drv, p_cur_hcd_td);

    p_cur_hcd_td = p_next_hcd_td;
  } while (is_last_hcd_td != DEF_YES);

  USBH_HCD_EP_HaltClr(p_hc_drv,
                      (void *)p_hcd_ed,
                      DEF_NO,
                      &local_err);
  PP_UNUSED_PARAM(local_err);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HCD_ISR_Handle()
 *
 * @brief    Handle OHCI host controller interrupts.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *******************************************************************************************************/
static void USBH_HCD_ISR_Handle(USBH_HC_DRV *p_hc_drv)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U    int_en = p_reg->HcInterruptEnable;
  CPU_INT32U    int_status = (p_reg->HcInterruptStatus & int_en);

  if (int_status == 0u) {
    return;
  }

  if (DEF_BIT_IS_SET(int_status, USBH_OHCI_INT_SCHED_OVERRUN) == DEF_YES) {
    LOG_DBG(("!! WARNING !! -> Schedule overrun."));
  }

  if (DEF_BIT_IS_SET(int_status, USBH_OHCI_INT_UNRECOVERABLE_ERR) == DEF_YES) {
    LOG_ERR(("Host Controller suffered from unrecoverable error."));
    CPU_SW_EXCEPTION(; );
  }

  if (DEF_BIT_IS_SET(int_status, USBH_OHCI_INT_RH_STATUS_CHNG) == DEF_YES) {
    //                                                             RH status change int.
    if (DEF_BIT_IS_SET(p_reg->HcRhStatus, USBH_OHCI_RH_STATUS_OVER_CURR_INDICATOR_CHNG) == DEF_YES) {
      USBH_HUB_RootEvent(p_hc_drv->HostNbr,
                         p_hc_drv->Nbr,
                         USBH_HUB_PORT_HUB_STATUS);
      DEF_BIT_SET(p_reg->HcRhStatus, USBH_OHCI_RH_STATUS_OVER_CURR_INDICATOR_CHNG);
    } else {
      CPU_INT08U port_status_chng = 0u;
      CPU_INT08U nbr_ports;
      CPU_INT08U ix;

      nbr_ports = p_reg->HcRhDescriptorA & USBH_OHCI_RH_DESC_A_NBR_PORTS;
      for (ix = 0u; ix < nbr_ports; ix++) {
        if (DEF_BIT_IS_SET_ANY(p_reg->HcRhPortStatus[ix], USBH_OHCI_RH_PORT_STATUS_CHNG_MSK) == DEF_YES) {
          DEF_BIT_SET(port_status_chng, DEF_BIT(ix + 1u));
        }
      }
      if (DEF_BIT_IS_SET_ANY(port_status_chng, DEF_BIT_FIELD(nbr_ports, 1u)) == DEF_YES) {
        USBH_HUB_RootEvent(p_hc_drv->HostNbr,
                           p_hc_drv->Nbr,
                           port_status_chng);
      }
    }
  }

  if (DEF_BIT_IS_SET(int_status, USBH_OHCI_INT_WRITEBACK_DONE_HEAD) == DEF_YES) {
    USBH_HCD_TD_DoneProcess(p_hc_drv);                          // Handle TD done q.
  }

  p_reg->HcInterruptStatus = int_status;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                               HOST CONTROLLER DRIVER ROOT HUB FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HCD_RH_InfosGet()
 *
 * @brief    Retrieve port status changes and port status.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    p_rh_info   Pointer to root HUB info structure to fill.
 *
 * @return   DEF_OK.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_InfosGet(USBH_HC_DRV        *p_hc_drv,
                                        USBH_HUB_ROOT_INFO *p_rh_info)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT16U    characteristics = 0u;

  //                                                               Port Pwr switching.
  if ((DEF_BIT_IS_SET(p_reg->HcRhDescriptorA, USBH_OHCI_RH_DESC_A_NO_PWR_SWITCHING) == DEF_YES)
      || (DEF_BIT_IS_CLR(p_reg->HcRhDescriptorA, USBH_OHCI_RH_DESC_A_PWR_SWITCHING_MODE) == DEF_YES)) {
    DEF_BIT_SET(characteristics, USBH_HUB_PWR_MODE_GANGED);
  } else {
    DEF_BIT_SET(characteristics, USBH_HUB_PWR_MODE_INDIVIDUAL);
  }
  //                                                               Over-current prot.
  if (DEF_BIT_IS_SET(p_reg->HcRhDescriptorA, USBH_OHCI_RH_DESC_A_NO_OVER_CURR_PROT) == DEF_YES) {
    DEF_BIT_SET(characteristics, USBH_HUB_OVER_CUR_NONE);
  } else {
    if (DEF_BIT_IS_SET(p_reg->HcRhDescriptorA, USBH_OHCI_RH_DESC_A_OVER_CURR_PROT_MODE) == DEF_YES) {
      DEF_BIT_SET(characteristics, USBH_HUB_OVER_CUR_INDIVIDUAL);
    } else {
      DEF_BIT_SET(characteristics, USBH_HUB_OVER_CUR_GLOBAL);
    }
  }

  p_rh_info->NbrPort = (p_reg->HcRhDescriptorA & USBH_OHCI_RH_DESC_A_NBR_PORTS);
  p_rh_info->Characteristics = characteristics;
  p_rh_info->PwrOn2PwrGood = (p_reg->HcRhDescriptorA & USBH_OHCI_RH_DESC_A_PWR_ON_TO_PWR_GOOD_TIME) >> 24u;

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       USBH_HCD_RH_PortStatusGet()
 *
 * @brief    Get status of root hub's port.
 *
 * @param    p_hc_drv        Pointer to host controller driver.
 *
 * @param    port_nbr        Port number (1-based).
 *
 * @param    p_port_status   Pointer to USBH_HUB_PORT_STATUS struct that will be filled.
 *
 * @return   DEF_OK.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_PortStatusGet(USBH_HC_DRV     *p_hc_drv,
                                             CPU_INT08U      port_nbr,
                                             USBH_HUB_STATUS *p_port_status)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U    port_status;

  if (port_nbr == USBH_HUB_PORT_HUB_STATUS) {
    port_status = p_reg->HcRhStatus;
  } else {
    port_status = p_reg->HcRhPortStatus[port_nbr - 1u];
  }

  p_port_status->Status = USBH_OHCI_RH_PORT_STATUS_STATE_GET(port_status);
  p_port_status->Chng = USBH_OHCI_RH_PORT_STATUS_CHNG_GET(port_status);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_RH_PortReq()
 *
 * @brief    Execute request on root hub's port.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    port_nbr    Port number (1-based).
 *
 * @param    req         Type of request.
 *
 * @param    feature     Type of HUB feature.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, if invalid port number.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_PortReq(USBH_HC_DRV *p_hc_drv,
                                       CPU_INT08U  port_nbr,
                                       CPU_INT08U  req,
                                       CPU_INT16U  feature)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_INT08U    pwr_mode;
  CPU_BOOLEAN   ok;

  ok = DEF_OK;
  port_nbr--;

  switch (req) {
    case USBH_DEV_REQ_SET_FEATURE:
      switch (feature) {
        case USBH_HUB_FEATURE_SEL_PORT_SUSPEND:
          p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_PORT_SUSPEND_STATUS;
          break;

        case USBH_HUB_FEATURE_SEL_PORT_RESET:
          p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_PORT_RESET_STATUS;
          break;

        case USBH_HUB_FEATURE_SEL_PORT_PWR:
          pwr_mode = USBH_HCD_PortPwrModeGet(p_hc_drv, port_nbr);
          if (pwr_mode == USBH_OHCI_PORT_PWR_GLOBAL) {
            p_reg->HcRhStatus = USBH_OHCI_RH_STATUS_GLOBAL_PWR_SET;
          } else if (pwr_mode == USBH_OHCI_PORT_PWR_INDIVIDUAL) {
            p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_PORT_PWR_SET;
          }
          break;

        case USBH_HUB_FEATURE_SEL_PORT_CONN:
        case USBH_HUB_FEATURE_SEL_PORT_EN:
        case USBH_HUB_FEATURE_SEL_PORT_OVER_CUR:
        case USBH_HUB_FEATURE_SEL_PORT_LOW_SPD:
        case USBH_HUB_FEATURE_SEL_PORT_TEST:
        case USBH_HUB_FEATURE_SEL_PORT_INDICATOR:
        case USBH_HUB_FEATURE_SEL_C_PORT_CONN:
        case USBH_HUB_FEATURE_SEL_C_PORT_EN:
        case USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND:
        case USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR:
        case USBH_HUB_FEATURE_SEL_C_PORT_RESET:
        default:
          break;
      }
      break;

    case USBH_DEV_REQ_CLR_FEATURE:
      switch (feature) {
        case USBH_HUB_FEATURE_SEL_PORT_EN:
          p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_CURR_CONN_STATUS;
          break;

        case USBH_HUB_FEATURE_SEL_PORT_SUSPEND:
          p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_PORT_OVER_CURR_INDICATOR;
          break;

        case USBH_HUB_FEATURE_SEL_PORT_PWR:
          pwr_mode = USBH_HCD_PortPwrModeGet(p_hc_drv, port_nbr);
          if (pwr_mode == USBH_OHCI_PORT_PWR_GLOBAL) {
            p_reg->HcRhStatus = USBH_OHCI_RH_STATUS_GLOBAL_PWR_CLR;
          } else if (pwr_mode == USBH_OHCI_PORT_PWR_INDIVIDUAL) {
            p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_PORT_PWR_CLR;
          }
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_CONN:
          p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_CONN_STATUS_CHNG;
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_EN:
          p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_PORT_EN_STATUS_CHNG;
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND:
          p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_PORT_SUSPEND_STATUS_CHNG;
          break;

        case USBH_HUB_FEATURE_SEL_C_PORT_RESET:
          p_reg->HcRhPortStatus[port_nbr] = USBH_OHCI_RH_PORT_STATUS_PORT_RESET_STATUS_CHNG;
          break;

        case USBH_HUB_FEATURE_SEL_PORT_CONN:
        case USBH_HUB_FEATURE_SEL_PORT_OVER_CUR:
        case USBH_HUB_FEATURE_SEL_PORT_RESET:
        case USBH_HUB_FEATURE_SEL_PORT_LOW_SPD:
        case USBH_HUB_FEATURE_SEL_PORT_TEST:
        case USBH_HUB_FEATURE_SEL_PORT_INDICATOR:
        case USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR:
        default:
          break;
      }
      break;

    case USBH_DEV_REQ_GET_STATUS:
    case USBH_DEV_REQ_SET_ADDR:
    case USBH_DEV_REQ_GET_DESC:
    case USBH_DEV_REQ_SET_DESC:
    case USBH_DEV_REQ_GET_CFG:
    case USBH_DEV_REQ_SET_CFG:
    case USBH_DEV_REQ_GET_IF:
    case USBH_DEV_REQ_SET_IF:
    case USBH_DEV_REQ_SYNCH_FRAME:
      break;

    default:
      ok = DEF_FAIL;
      break;
  }

  return (ok);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_RH_IntEn()
 *
 * @brief    Enable/disable root hub interrupts.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    en          DEF_YES, if interrupts are to be enabled.
 *                       DEF_NO,  if interrupts are to be disabled.
 *
 * @return   DEF_OK.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HCD_RH_IntEn(USBH_HC_DRV *p_hc_drv,
                                     CPU_BOOLEAN en)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;

  if (en == DEF_YES) {
    p_reg->HcInterruptEnable = USBH_OHCI_INT_RH_STATUS_CHNG;
  } else {
    p_reg->HcInterruptDisable = USBH_OHCI_INT_RH_STATUS_CHNG;
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_HC_NextFrameNbrWait()
 *
 * @brief    Wait for current frame to complete.
 *
 * @param    p_reg   Pointer to OHCI registers structure.
 *
 * @return   DEF_OK,   if successful,
 *           DEF_FAIL, if error occurred.
 *
 *           USBH_HC_ED_Skip(),
 *           USBH_HCD_Suspend().
 *
 * @note     (1) This function should never be called from critical sections or interrupts.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HC_NextFrameNbrWait(USBH_OHCI_REG *p_reg)
{
  CPU_INT32U cur_frame_nbr = p_reg->HcFmNumber;
  CPU_INT32U cnt = 0u;

  while (p_reg->HcFmNumber == cur_frame_nbr) {
    ++cnt;

    if (cnt > 3u) {
      LOG_DBG(("OHCI HCD frame number does not increment."));
      return (DEF_FAIL);
    }
    KAL_Dly(1u);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_PortPwrModeGet()
 *
 * @brief    Get power mode of root hub's ports.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    port_nbr    Port number (1-based).
 *
 * @return   OHCI_PORT_PWR_ALWAYS        Port is always       powered. No switching.
 *           OHCI_PORT_PWR_INDIVIDUAL    Port is individually powered.
 *           OHCI_PORT_PWR_GLOBAL        Port is globally     powered.
 *******************************************************************************************************/
static USBH_OHCI_PORT_PWR USBH_HCD_PortPwrModeGet(USBH_HC_DRV *p_hc_drv,
                                                  CPU_INT08U  port_nbr)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;

  if (DEF_BIT_IS_SET(p_reg->HcRhDescriptorA, USBH_OHCI_RH_DESC_A_NO_PWR_SWITCHING) == DEF_YES) {
    return (USBH_OHCI_PORT_PWR_ALWAYS);                         // Ports are always powered.
  }

  if (DEF_BIT_IS_SET(p_reg->HcRhDescriptorA, USBH_OHCI_RH_DESC_A_PWR_SWITCHING_MODE) == DEF_YES) {
    CPU_INT16U ppc_mask = (p_reg->HcRhDescriptorB & USBH_OHCI_RH_DESC_B_PORT_PWR_CTRL_MSK) >> 16u;

    //                                                             Pwr switching.
    if ((ppc_mask & (1u << port_nbr)) != 0u) {
      return (USBH_OHCI_PORT_PWR_INDIVIDUAL);                   // Port is controlled individually.
    }
  }

  return (USBH_OHCI_PORT_PWR_GLOBAL);                           // Port is global powered.
}

/****************************************************************************************************//**
 *                                               USBH_HC_ED_Skip()
 *
 * @brief    Make OHCI host controller skip this Host Controller Endpoint Descriptor (HC ED).
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    p_hc_ed     Pointer to host controller endpoint descriptor (HC ED) to skip.
 *
 * @note     (1) This function should not be called from critical sections or interrupts.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HC_ED_Skip(USBH_HC_DRV     *p_hc_drv,
                                   USBH_OHCI_HC_ED *p_hc_ed)
{
  USBH_OHCI_REG *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
  CPU_BOOLEAN   ep_halt;
  CPU_BOOLEAN   ok;

  USBH_OHCI_DCACHE_INV_HC_ED_HEAD_PTR(p_hc_ed);
  ep_halt = DEF_BIT_IS_SET(p_hc_ed->HeadHC_TD_Ptr, USBH_OHCI_ED_HEAD_PTR_HALT);
  if (ep_halt == DEF_TRUE) {                                    // If EP is already paused, return immediately.
    return (DEF_OK);
  }

  DEF_BIT_SET(p_hc_ed->Ctrl, USBH_OHCI_ED_CTRL_SKIP);           // HC skips this EP & continues to the next ED.
  USBH_OHCI_DCACHE_FLUSH_HC_ED_CTRL(p_hc_ed);

  ok = USBH_HC_NextFrameNbrWait(p_reg);

  return (ok);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_TD_Insert()
 *
 * @brief    Insert HCD and HC TDs under appropriate HCD and HC EDs.
 *
 * @param    p_hc_drv            Pointer to host controller driver.
 *
 * @param    p_hcd_ed            Pointer to HCD ED structure in which to insert the TD.
 *
 * @param    pp_hcd_urb_data     Pointer to memory location where a pointer to the host controller
 *                               driver's URB data will be kept.
 *
 * @param    hc_td_ctrl          Value to be written in the 'Ctrl' field of the HC TDs.
 *
 * @param    buf_addr            Start address of buffer.
 *
 * @param    buf_len             Length of the buffer (in bytes).
 *
 * @param    p_err               Pointer to variable that will receive error code from this function :
 *                                   - RTOS_ERR_NONE               Operation was successful.
 *                                   - RTOS_ERR_DRV_URB_ALLOC  Transfer descriptor allocation failed.
 *
 * @note     (1) A transfer can be spread on up to two 4k-page. That is, if the buffer is 4k-aligned,
 *               the transfer length can be of 8192 bytes. If the buffer is not aligned on 4k, the
 *               maximal transfer length is reduced of the difference between the start address of
 *               the buffer and the previous 4k-page's address.
 *
 * @note     (2) The 'bufferRounding' bit can be used to trigger an error condition when a short
 *               packet is received. It is used in cases where an URB spans multiple TDs. If a TD that
 *               is not the last receives less than what was expected, it should cause an error to
 *               halt the endpoint. This allows the Host Controller Driver to remove any subsequent
 *               TDs associated to this URB and to then un-halt the endpoint. The fact that the
 *               endpoint is halted during this operation means that it is guaranteed that the data
 *               received afterwards goes into the right buffer, and not in the buffer of the
 *               multiple-TD-URB. See OHCI Specification section 4.3.1.2 'General Transfer Descriptor
 *               Field Definitions' for more details.
 *******************************************************************************************************/
static void USBH_HCD_TD_Insert(USBH_HC_DRV      *p_hc_drv,
                               USBH_OHCI_HCD_ED *p_hcd_ed,
                               void             **pp_hcd_urb_data,
                               CPU_INT32U       hc_td_ctrl,
                               CPU_INT32U       buf_addr,
                               CPU_INT32U       buf_len,
                               RTOS_ERR         *p_err)
{
  USBH_OHCI_DATA   *p_data = (USBH_OHCI_DATA *)p_hc_drv->DataPtr;
  USBH_OHCI_HCD_TD *p_orig_tail_hcd_td = p_hcd_ed->TailHCD_TD_Ptr;
  USBH_OHCI_HCD_TD *p_new_head_hcd_td = DEF_NULL;
  USBH_OHCI_HCD_TD *p_cur_hcd_td = p_orig_tail_hcd_td;
  USBH_OHCI_HCD_TD *p_new_hcd_td;
  USBH_OHCI_HC_TD  *p_cur_hc_td;
  void             *p_ded_mem_buf;
  CPU_INT16U       ep_max_pkt_size = USBH_OHCI_ED_CTRL_MAX_PKT_SIZE_GET(p_hcd_ed->HC_ED_Ptr->Ctrl);
  CPU_INT32U       rem_buf_len = buf_len;
  CPU_INT32U       cur_urb_buf_addr = buf_addr;
  CPU_INT32U       cur_hc_td_buf_addr;
  CPU_INT32U       cur_hc_td_ctrl = hc_td_ctrl;
  CPU_INT32U       ix;
  CPU_BOOLEAN      dedicated_mem_use = DEF_NO;
  CPU_BOOLEAN      dedicated_mem_copy = DEF_NO;
  RTOS_ERR         err_free;
  CORE_DECLARE_IRQ_STATE;

  if ((hc_td_ctrl & USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MSK) != 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
    LOG_ERR(("Host Controller cannot add this transfer descriptor to endpoint, it would overwrite the index used to re-associate HC and HCD TDs."));
    USBH_OHCI_DBG_TRAP();
    return;
  }

  if ((p_hc_drv->HW_InfoPtr->DataBufDedicatedMemInfoPtr != DEF_NULL)
      && (buf_len != 0u)) {
    CPU_INT32U urb_buf_end_addr = buf_addr + buf_len - 1u;

    //                                                             Buf is not comprised within dedicated mem region.
    if ((buf_addr < p_data->DedicatedMemDataBufStartAddr)
        || (urb_buf_end_addr > p_data->DedicatedMemDataBufEndAddr)) {
      dedicated_mem_use = DEF_YES;
    }
  }

  if (((hc_td_ctrl & USBH_OHCI_TD_CTRL_DIR_PID_MSK) == USBH_OHCI_TD_CTRL_DIR_PID_SETUP)
      || ((hc_td_ctrl & USBH_OHCI_TD_CTRL_DIR_PID_MSK) == USBH_OHCI_TD_CTRL_DIR_PID_OUT)) {
    if (dedicated_mem_use == DEF_NO) {
      CPU_DCACHE_RANGE_FLUSH((void *)buf_addr, buf_len);
    } else {
      dedicated_mem_copy = DEF_YES;
    }
  }

    #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  ix = p_hcd_ed->Ix;
    #endif

  *pp_hcd_urb_data = (void *)p_orig_tail_hcd_td;

  p_cur_hcd_td = p_orig_tail_hcd_td;
  p_cur_hc_td = p_orig_tail_hcd_td->HC_TD_Ptr;

  do {                                                          // Alloc and insert HCD/HC TD(s) in list.
    CPU_INT32U max_td_len;
    CPU_INT32U td_len;

    if (dedicated_mem_use == DEF_NO) {
      //                                                           Find xfer max avail len (see Note #1).

      max_td_len = USBH_OHCI_HC_TD_MAX_LEN_GET(cur_urb_buf_addr, ep_max_pkt_size);
      td_len = DEF_MIN(rem_buf_len, max_td_len);
      cur_hc_td_buf_addr = cur_urb_buf_addr;
    } else {                                                    // Alloc dedicated mem if req'd.
                                                                // Get buf from dedicated mem seg.
      p_ded_mem_buf = Mem_DynPoolBlkGet(&p_data->XferBufPool,
                                        p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
        goto err_alloc;
      }

      max_td_len = USBH_OHCI_HC_TD_MAX_LEN_GET((CPU_INT32U)p_ded_mem_buf, ep_max_pkt_size);
      td_len = DEF_MIN(rem_buf_len, max_td_len);
      if (td_len > p_data->DedicatedMemDataBufLen) {
        //                                                         If not last xfer, len must be mult of max pkt size.
        td_len = (p_data->DedicatedMemDataBufLen - (p_data->DedicatedMemDataBufLen % ep_max_pkt_size));
      }

      if (dedicated_mem_copy == DEF_YES) {                      // Copy data if a tx is req'd.
        Mem_Copy(p_ded_mem_buf,
                 (void *)cur_urb_buf_addr,
                 td_len);
        CPU_DCACHE_RANGE_FLUSH((void *)p_ded_mem_buf, td_len);
      }
      cur_hc_td_buf_addr = (CPU_INT32U)p_ded_mem_buf;
    }

    //                                                             Alloc HCD TD.
    p_new_hcd_td = (USBH_OHCI_HCD_TD *)Mem_DynPoolBlkGet(&p_data->HCD_TD_MemPool,
                                                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
      goto err_alloc_free_buf;
    }
    //                                                             Alloc HC TD.
    p_new_hcd_td->HC_TD_Ptr = (USBH_OHCI_HC_TD *)Mem_DynPoolBlkGet(&p_data->HC_TD_MemPool,
                                                                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
      goto err_alloc_free_buf_hcd_td;
    }

        #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    {                                                           // Find tbl ix in which to keep ref to HCD TD.
      CPU_INT08U bitmap_ix_max = (p_data->HCD_TD_PtrTblIxMax + (DEF_INT_32_NBR_BITS - 1u)) / DEF_INT_32_NBR_BITS;
      CPU_INT08U bitmap_ix;

      ix = (USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MAX + 1u);
      CORE_ENTER_ATOMIC();
      for (bitmap_ix = 0u; bitmap_ix < bitmap_ix_max; bitmap_ix++) {
        if (DEF_BIT_IS_CLR_ANY(p_data->HCD_TD_PtrTblIxBitmapTbl[bitmap_ix], DEF_INT_32_MASK) == DEF_YES) {
          ix = DEF_INT_32_NBR_BITS - 1u - CPU_CntLeadZeros(~p_data->HCD_TD_PtrTblIxBitmapTbl[bitmap_ix]);

          DEF_BIT_SET(p_data->HCD_TD_PtrTblIxBitmapTbl[bitmap_ix], DEF_BIT(ix));
          ix += DEF_INT_32_NBR_BITS * bitmap_ix;
          break;
        }
      }
      CORE_EXIT_ATOMIC();
      if (ix <= USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MAX) {
        p_data->HCD_TD_PtrTbl[ix] = p_new_hcd_td;
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_DRV_URB_ALLOC);
        goto err_alloc_free_all;
      }
    }
        #endif

    p_new_hcd_td->HCD_ED_Ptr = p_hcd_ed;

    //                                                             Since HC never accesses this field, it is OK to ...
    //                                                             ... set it even if alloc is not cmpl'd.
    p_new_hcd_td->HC_TD_Ptr->Ctrl = (ix & USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MSK);

    if (p_cur_hcd_td == p_orig_tail_hcd_td) {                   // If first iteration in loop, keep new head HCD TD.
      p_new_head_hcd_td = p_new_hcd_td;
    } else {
      p_cur_hcd_td->NextHCD_TD_Ptr = p_new_hcd_td;              // Otherwise, link new HCD TD to prev.
    }

    rem_buf_len -= td_len;
    cur_urb_buf_addr += td_len;

    p_cur_hcd_td->XferStatus = td_len;
    if (rem_buf_len > 0u) {
      //                                                           Set bit to indicate this TD is not the last of xfer.
      DEF_BIT_SET(p_cur_hcd_td->XferStatus, USBH_OHCI_HCD_TD_XFER_STATUS_IS_NOT_LAST);
      //                                                           Short packet will cause err and halt EP if TD is ...
      //                                                           ... not last of xfer (see Note #2).
      DEF_BIT_CLR(cur_hc_td_ctrl, USBH_OHCI_TD_CTRL_BUF_ROUNDING);
    } else {
      //                                                           Xfer < than expected will not cause err on last TD.
      DEF_BIT_SET(cur_hc_td_ctrl, USBH_OHCI_TD_CTRL_BUF_ROUNDING);
    }

    p_cur_hc_td->Ctrl |= cur_hc_td_ctrl;                        // Write latest data while keeping ix val.
    if (buf_len != 0u) {
      p_cur_hc_td->CurBufPtr = cur_hc_td_buf_addr;
      p_cur_hc_td->BufEndPtr = cur_hc_td_buf_addr + td_len - 1u;
    } else {
      p_cur_hc_td->CurBufPtr = 0u;
      p_cur_hc_td->BufEndPtr = 0u;
    }
    p_cur_hc_td->NextHC_TD_Ptr = (CPU_REG32)p_new_hcd_td->HC_TD_Ptr;
    USBH_OHCI_DCACHE_FLUSH_HC_TD(p_cur_hc_td);

    //                                                             Force following TDs to inherit of EP's data toggle.
    DEF_BIT_CLR(cur_hc_td_ctrl, USBH_OHCI_TD_CTRL_DATA_TOGGLE_MSK);
    DEF_BIT_SET(cur_hc_td_ctrl, USBH_OHCI_TD_CTRL_DATA_TOGGLE_ED_CARRY);

    p_cur_hcd_td = p_new_hcd_td;
    p_cur_hc_td = p_new_hcd_td->HC_TD_Ptr;
  } while (rem_buf_len > 0u);

  //                                                               Init empty tail HCD TD.
  p_cur_hcd_td->NextHCD_TD_Ptr = DEF_NULL;
  p_cur_hcd_td->XferStatus = 0u;

  p_cur_hc_td->CurBufPtr = 0u;                                  // Fill fields of last HC TD (the empty one).
  p_cur_hc_td->NextHC_TD_Ptr = 0u;
  p_cur_hc_td->BufEndPtr = 0u;
  USBH_OHCI_DCACHE_FLUSH_HC_TD(p_cur_hc_td);

  CORE_ENTER_ATOMIC();                                          // Add new HCD TD(s) at the tail of existing list.
  p_orig_tail_hcd_td->NextHCD_TD_Ptr = p_new_head_hcd_td;
  p_hcd_ed->TailHCD_TD_Ptr = p_cur_hcd_td;
  //                                                               Update HC ED tail ptr to new empty tail HC TD.
  //                                                               This must be the last operation.
  p_orig_tail_hcd_td->HC_TD_Ptr->NextHC_TD_Ptr = (CPU_REG32)p_new_head_hcd_td->HC_TD_Ptr;
  p_hcd_ed->HC_ED_Ptr->TailHC_TD_Ptr = (CPU_REG32)p_cur_hc_td;
  CORE_EXIT_ATOMIC();
  USBH_OHCI_DCACHE_FLUSH_HC_TD_NEXT_TD_PTR(p_orig_tail_hcd_td->HC_TD_Ptr);
  USBH_OHCI_DCACHE_FLUSH_HC_ED_TAIL_TD_PTR(p_hcd_ed->HC_ED_Ptr);

  return;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
err_alloc_free_all:                                             // Err handling when allocating bufs, HC and HCD TDs.

  Mem_DynPoolBlkFree(&p_data->HC_TD_MemPool,
                     (void *)p_new_hcd_td->HC_TD_Ptr,
                     &err_free);
  PP_UNUSED_PARAM(err_free);
#endif

err_alloc_free_buf_hcd_td:
  Mem_DynPoolBlkFree(&p_data->HCD_TD_MemPool,
                     (void *)p_new_hcd_td,
                     &err_free);
  PP_UNUSED_PARAM(err_free);

err_alloc_free_buf:
  if (dedicated_mem_use == DEF_YES) {
    Mem_DynPoolBlkFree(&p_data->XferBufPool,
                       p_ded_mem_buf,
                       &err_free);
    PP_UNUSED_PARAM(err_free);
  }

err_alloc:
  //                                                               At this point, the list of HCD TDs starting from ...
  //                                                               ... p_orig_tail_hcd_td is built correctly. It is ...
  //                                                               ... possible to start from there to reset  and   ...
  //                                                               ... de-alloc everything.

  //                                                               Clear tail HCD TD vals, except for HC_TD_Ptr. It ...
  //                                                               ... must be kept for the future. This HCD (and   ...
  //                                                               ... its  corresponding HC TD) must be kept as    ...
  //                                                               ... the end of their respective chained lists.
  p_cur_hcd_td = p_orig_tail_hcd_td;
  p_cur_hcd_td->NextHCD_TD_Ptr = DEF_NULL;
  p_cur_hcd_td->XferStatus = 0u;

  p_cur_hc_td = p_cur_hcd_td->HC_TD_Ptr;
  DEF_BIT_CLR(p_cur_hc_td->Ctrl, ~USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MSK);
  p_cur_hc_td->CurBufPtr = 0u;
  p_cur_hc_td->BufEndPtr = 0u;
  p_cur_hc_td->NextHC_TD_Ptr = 0u;

  p_cur_hcd_td = p_new_head_hcd_td;

  while (p_cur_hcd_td != DEF_NULL) {                            // Free everything that was alloc'd.
    USBH_OHCI_HCD_TD *p_next_hcd_td;

    p_next_hcd_td = p_cur_hcd_td->NextHCD_TD_Ptr;

        #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    USBH_OHCI_DCACHE_INV_HC_TD_CTRL(p_cur_hc_td);
    ix = p_cur_hcd_td->HC_TD_Ptr->Ctrl & USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MSK;
    CORE_ENTER_ATOMIC();
    p_data->HCD_TD_PtrTbl[ix] = DEF_NULL;
    DEF_BIT_CLR(p_data->HCD_TD_PtrTblIxBitmapTbl[ix / 32u], DEF_BIT(ix % DEF_INT_32_NBR_BITS));
    CORE_EXIT_ATOMIC();
        #endif

    if ((dedicated_mem_use == DEF_YES)
        && (p_next_hcd_td != DEF_NULL)) {                       // Free dedicated mem buf only if another HCD TD is ...
                                                                // ... present. Otherwise, it has not been set yet.
      Mem_DynPoolBlkFree(&p_data->XferBufPool,
                         (void *)p_cur_hcd_td->HC_TD_Ptr->CurBufPtr,
                         &err_free);
    }

    Mem_DynPoolBlkFree(&p_data->HC_TD_MemPool,
                       (void *)p_cur_hcd_td->HC_TD_Ptr,
                       &err_free);
    PP_UNUSED_PARAM(err_free);

    Mem_DynPoolBlkFree(&p_data->HCD_TD_MemPool,
                       (void *)p_cur_hcd_td,
                       &err_free);
    PP_UNUSED_PARAM(err_free);

    p_cur_hcd_td = p_next_hcd_td;
  }

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HCD_TD_DoneProcess()
 *
 * @brief    Process transfer descriptor done queue. For each completed transfer, USBH_URB_Done() is
 *           called.
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @note     (1) See OHCI Specification section 5.2.9 'Done Queue'for more details.
 *
 * @note     (2) This function is called from an ISR context.
 *******************************************************************************************************/
static void USBH_HCD_TD_DoneProcess(USBH_HC_DRV *p_hc_drv)
{
  USBH_OHCI_DATA   *p_data = (USBH_OHCI_DATA *)p_hc_drv->DataPtr;
  CPU_REG32        cur_hc_td_addr;
  CPU_REG32        prev_hc_td_addr = 0u;
  CPU_REG32        next_hc_td_addr;
  USBH_OHCI_HC_TD  *p_cur_hc_td;
  USBH_OHCI_HCD_TD *p_cur_hcd_td;
  CPU_INT32U       ix;

  USBH_OHCI_DCACHE_INV_HCCA_DONE_HEAD(p_data->HCCA_Ptr);
  cur_hc_td_addr = (p_data->HCCA_Ptr->HccaDoneHead & USBH_OHCI_DONE_HEAD_MSK);

  do {
    p_cur_hc_td = (USBH_OHCI_HC_TD *)cur_hc_td_addr;
    USBH_OHCI_DCACHE_INV_HC_TD(p_cur_hc_td);
    next_hc_td_addr = p_cur_hc_td->NextHC_TD_Ptr;
    p_cur_hc_td->NextHC_TD_Ptr = prev_hc_td_addr;
    //                                                             No need to flush cache, even if writing ...
    //                                                             NextHC_TD_Ptr. HC will not re-access this TD.
    prev_hc_td_addr = cur_hc_td_addr;
    cur_hc_td_addr = next_hc_td_addr;
  } while (cur_hc_td_addr != 0u);

  p_cur_hc_td = (USBH_OHCI_HC_TD *)prev_hc_td_addr;

  do {                                                          // Iter through ordered list.
    ix = p_cur_hc_td->Ctrl & USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MSK;
        #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    if (ix >= p_data->HCD_TD_PtrTblIxMax) {                     // Confirm ix is in valid range.
      LOG_ERR(("Obtained invalid HCD TD ix from HC TD."));

      USBH_OHCI_DBG_STATS_INC(URB_DoneTD_IxErrCnt);

      break;
    }
    //                                                             Get HCD TD associated with HC TD.
    p_cur_hcd_td = p_data->HCD_TD_PtrTbl[ix];
        #else
    {
      USBH_OHCI_HCD_ED *p_cur_hcd_ed;
      CPU_BOOLEAN      found = DEF_NO;

      if (ix >= USBH_OHCI_HCD_ED_LIST_SIZE) {
        LOG_ERR(("Obtained invalid HCD ED ix from HC TD."));

        USBH_OHCI_DBG_STATS_INC(URB_DoneED_IxErrCnt);

        break;
      }
      //                                                           Iter all HCD EDs q'd under this ix to find HCD TD.
      p_cur_hcd_ed = p_data->HCD_ED_ListsPtrTbl[ix];
      while (p_cur_hcd_ed != DEF_NULL) {
        p_cur_hcd_td = p_cur_hcd_ed->HeadHCD_TD_Ptr;

        while ((p_cur_hcd_td != DEF_NULL)
               && (p_cur_hcd_td->HC_TD_Ptr != p_cur_hc_td)) {
          p_cur_hcd_td = p_cur_hcd_td->NextHCD_TD_Ptr;
        }
        if (p_cur_hcd_td->HC_TD_Ptr == p_cur_hc_td) {           // HCD TD is found. Exit loop.
          found = DEF_YES;
          break;
        }

        p_cur_hcd_ed = p_cur_hcd_ed->NextHCD_ED_Ptr;
      }
      if (found != DEF_YES) {                                   // If HCD TD was not found, err.
        LOG_ERR(("Host Controller was unable to find HCD TD associated to HC TD ", (u)(CPU_INT32U)p_cur_hc_td));
        USBH_OHCI_DBG_TRAP();
        break;
      }
    }
        #endif

    if (p_cur_hcd_td == DEF_NULL) {
      //                                                           IF HC TD is valid, HCD TD should be valid, too.
      LOG_ERR(("Host Controller was unable to find HCD TD associated to HC TD ", (u)(CPU_INT32U)p_cur_hc_td));
      USBH_OHCI_DBG_STATS_INC(URB_DoneHCD_TD_IsNullErrCnt);
      break;
    }

    DEF_BIT_SET(p_cur_hcd_td->XferStatus, USBH_OHCI_HCD_TD_XFER_STATUS_IS_FINISHED);

    if (DEF_BIT_IS_CLR(p_cur_hcd_td->XferStatus, USBH_OHCI_HCD_TD_XFER_STATUS_IS_NOT_LAST) == DEF_YES) {
      //                                                           Last TD of xfer.
      USBH_OHCI_DBG_STATS_INC(URB_DoneCallCnt);
      USBH_URB_Done(p_cur_hcd_td->HCD_ED_Ptr->DevHandle,
                    p_cur_hcd_td->HCD_ED_Ptr->EP_Handle);
    } else {                                                    // Other TD(s) is/are associated to this one for xfer.
                                                                // No need to invalidate Ctrl. Already done above.
      CPU_INT32U cmpl_code = (p_cur_hc_td->Ctrl & USBH_OHCI_TD_CTRL_CMPL_CODE_MSK);

      if (cmpl_code == USBH_OHCI_CMPL_CODE_DATA_UNDERRUN) {
        //                                                         Short-packet rx'd when xfer is set on multiple TDs.
        USBH_OHCI_REG    *p_reg = (USBH_OHCI_REG *)p_hc_drv->HW_InfoPtr->BaseAddr;
        USBH_OHCI_HCD_ED *p_hcd_ed = p_cur_hcd_td->HCD_ED_Ptr;
        USBH_OHCI_HCD_TD *p_to_remove_hcd_td = p_cur_hcd_td->NextHCD_TD_Ptr;
        CPU_INT08U       ep_type;
        CPU_BOOLEAN      is_last_hcd_td = DEF_NO;
        RTOS_ERR         local_err;

        //                                                         Remove all others HC TDs associated to xfer.
        do {
          if (DEF_BIT_IS_CLR(p_to_remove_hcd_td->XferStatus, USBH_OHCI_HCD_TD_XFER_STATUS_IS_NOT_LAST) == DEF_YES) {
            is_last_hcd_td = DEF_YES;                           // This is the last TD associated to this xfer/URB.
                                                                // Update head ptr of HC ED.
            USBH_OHCI_DCACHE_INV_HC_ED_HEAD_PTR(p_hcd_ed->HC_ED_Ptr);
            p_hcd_ed->HC_ED_Ptr->HeadHC_TD_Ptr = (((CPU_REG32)p_to_remove_hcd_td->NextHCD_TD_Ptr->HC_TD_Ptr)
                                                  | ((CPU_REG32)p_hcd_ed->HC_ED_Ptr->HeadHC_TD_Ptr & USBH_OHCI_ED_HEAD_PTR_TOGGLE_CARRY));
            USBH_OHCI_DCACHE_FLUSH_HC_ED_HEAD_TD_PTR(p_hcd_ed->HC_ED_Ptr);
          }

          p_to_remove_hcd_td = p_to_remove_hcd_td->NextHCD_TD_Ptr;
        } while (is_last_hcd_td != DEF_YES);

        USBH_HCD_EP_HaltClr(p_hc_drv,                           // Resume EP processing.
                            (void *)p_hcd_ed,
                            DEF_NO,
                            &local_err);
        PP_UNUSED_PARAM(local_err);

        ep_type = USBH_OHCI_ED_CTRL_EP_TYPE_GET(p_hcd_ed->HC_ED_Ptr->Ctrl);
        if (ep_type == USBH_EP_TYPE_CTRL) {                     // Tell HC that list has been updated.
          DEF_BIT_SET(p_reg->HcCommandStatus, USBH_OHCI_CMD_STATUS_CTRL_LIST_FILLED);
        } else if (ep_type == USBH_EP_TYPE_BULK) {
          DEF_BIT_SET(p_reg->HcCommandStatus, USBH_OHCI_CMD_STATUS_BULK_LIST_FILLED);
        }

        USBH_OHCI_DBG_STATS_INC(URB_UnderrunDoneCallCnt);

        USBH_URB_Done(p_cur_hcd_td->HCD_ED_Ptr->DevHandle,
                      p_cur_hcd_td->HCD_ED_Ptr->EP_Handle);
      } else if (cmpl_code == USBH_OHCI_CMPL_CODE_NOT_ACCESSED) {
        LOG_ERR(("Host Controller should not remove 'not accessed' HCD TDs."));
        USBH_OHCI_DBG_TRAP();
      }
    }
    //                                                             No need to inv cache, HC did not access TD since ...
    //                                                             ... start of fnct.
    p_cur_hc_td = (USBH_OHCI_HC_TD *)p_cur_hc_td->NextHC_TD_Ptr;
  } while (p_cur_hc_td != 0u);
}

/****************************************************************************************************//**
 *                                           USBH_HCD_TD_Free()
 *
 * @brief    Free Host Controller Driver's Transfer Descriptor (HCD TD) and its associated Host
 *           Controller Transfer Descriptor (HC TD).
 *
 * @param    p_hc_drv    Pointer to host controller driver.
 *
 * @param    p_hcd_td    Pointer to HCD TD to free.
 *******************************************************************************************************/
static void USBH_HCD_TD_Free(USBH_HC_DRV      *p_hc_drv,
                             USBH_OHCI_HCD_TD *p_hcd_td)
{
  USBH_OHCI_DATA *p_data = (USBH_OHCI_DATA *)p_hc_drv->DataPtr;
  RTOS_ERR       err_lib;

    #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT32U ix;
    CORE_DECLARE_IRQ_STATE;

    USBH_OHCI_DCACHE_INV_HC_TD_CTRL(p_hcd_td->HC_TD_Ptr);
    ix = p_hcd_td->HC_TD_Ptr->Ctrl & USBH_OHCI_TD_CTRL_HCD_TD_PTR_TBL_IX_MSK;
    if (ix > p_data->HCD_TD_PtrTblIxMax) {
      CPU_SW_EXCEPTION();
      USBH_OHCI_DBG_TRAP();
    }
    CORE_ENTER_ATOMIC();
    p_data->HCD_TD_PtrTbl[ix] = DEF_NULL;
    DEF_BIT_CLR(p_data->HCD_TD_PtrTblIxBitmapTbl[ix / 32u], DEF_BIT(ix % DEF_INT_32_NBR_BITS));
    CORE_EXIT_ATOMIC();
  }
    #endif

  Mem_DynPoolBlkFree(&p_data->HC_TD_MemPool,
                     (void *)p_hcd_td->HC_TD_Ptr,
                     &err_lib);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err_lib),; );

  Mem_DynPoolBlkFree(&p_data->HCD_TD_MemPool,
                     (void *)p_hcd_td,
                     &err_lib);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_lib) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err_lib),; );
}

/****************************************************************************************************//**
 *                                           USBH_BinTreeInit()
 *
 * @brief    Initialize binary tree browsing status structure, depending on where the HCD ED is being
 *           added.
 *
 * @param    p_browse_status     Pointer to binary tree browsing status structure.
 *
 * @param    depth_lvl           Level of depth at which to insert the ED.
 *
 * @param    hcd_ed_ix           HCD ED ix at which the ED will be inserted.
 *
 * @note     (1) The diagram below shows the bit nbr associated in the bitmap of each depth level of
 *               the HCD ED binary tree. The bit nbr is in parentheses.
 *				@verbatim
 *               0(00)-----\
 *               16(01)------32(00)-----\
 *               8(02)-----\            48(0)----\
 *               24(03)------40(01)-----/          \
 *               4(04)-----\                       56(0)--\
 *               20(05)------36(02)-----\          /        \
 *               12(06)-----\            52(1)----/          \
 *               28(07)------44(03)-----/                     \
 *               2(08)-----\                                  60(0)-\
 *               18(09)------34(04)-----\                     /       \
 *               10(10)-----\            50(2)----\          /         \
 *               26(11)------42(05)-----/          \        /           \
 *               6(12)-----\                       58(1)--/             \
 *               22(13)------38(06)-----\          /                      \
 *               14(14)-----\            54(3)----/                        \
 *               30(15)------46(07)-----/                                   \
 *               1(16)-----\                                                62(0)
 *               17(17)------33(08)-----\                                   /
 *               9(18)-----\            49(4)----\                        /
 *               25(19)------41(09)-----/          \                      /
 *               5(20)-----\                       57(2)--\             /
 *               21(21)------37(10)-----\          /        \           /
 *               13(22)-----\            53(5)----/          \         /
 *               29(23)------45(11)-----/                     \       /
 *               3(24)-----\                                  61(1)-/
 *               19(25)------35(12)-----\                     /
 *               11(26)-----\            51(6)----\          /
 *               27(27)------43(13)-----/          \        /
 *               7(28)-----\                       59(3)--/
 *               23(29)------39(14)-----\          /
 *               15(30)-----\            55(7)----/
 *               31(31)------47(15)-----/
 *				@endverbatim
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_BinTreeInit(USBH_OHCI_BIN_TREE_STATUS *p_browse_status,
                             CPU_INT08U                depth_lvl,
                             CPU_INT08U                hcd_ed_ix)
{
  CPU_INT08U depth_ix;
  CPU_INT08U bit_nbr;
  CPU_INT08U bit_shift;

  p_browse_status->Depth = depth_lvl;
  //                                                               Clr all the status bits.
  for (depth_ix = 0u; depth_ix <= USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_MAX; ++depth_ix) {
    p_browse_status->NodesBitmap[depth_ix] = 0u;
  }

  //                                                               Browse every prev depth level(s).
  bit_nbr = (1u << depth_lvl);
  bit_shift = USBH_OHCI_HC_ED_IxBitNbrConvTbl[hcd_ed_ix - USBH_OHCI_BinTreeInfoTbl[p_browse_status->Depth].BaseIx];
  for (depth_ix = USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_32MS; depth_ix < depth_lvl; depth_ix++) {
    //                                                             Set bits of every ancestor ix: they must be browsed.
    DEF_BIT_SET(p_browse_status->NodesBitmap[depth_ix], DEF_BIT_FIELD(bit_nbr, bit_shift));
    bit_nbr = bit_nbr   >> 1u;
    bit_shift = bit_shift >> 1u;
  }
}
#endif

/****************************************************************************************************//**
 *                                           USBH_BinTreeUpdated()
 *
 * @brief    Update binary tree browsing status structure, if an ED has been found before the 32ms
 *           interval.
 *
 * @param    p_browse_status     Pointer to binary tree browsing status structure.
 *
 * @param    ed_ix_updated       HC/HCD ED ix that was updated.
 *
 * @note     (1) The ancestors of a node where a HCD ED was found does not need to be checked too,
 *               since the HCD ED 'covers' all its ancestors. This function clears the bits associated
 *               with every ancestor of a HCD ED ix. For example, if HCD ED ix 48 was updated, HCD ED
 *               indexes 0, 16, 8, 24, 32 and 40 need to be cleared. If HCD ED ix 35 was updated, HCD
 *               ED indexes 3 and 19 do not need to be checked.
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static void USBH_BinTreeUpdated(USBH_OHCI_BIN_TREE_STATUS *p_browse_status,
                                CPU_INT08U                ed_ix_updated)
{
  CPU_INT08U cur_bit_nbr;
  CPU_INT08U depth_ix;
  CPU_INT08U bit_ix;

  //                                                               Since HC/HCD ED have been added at this ix, all ...
  //                                                               ... ancestors of this ix do not need to be checked.

  //                                                               Find which bit nbr at the current depth the cur ...
  //                                                               ... HC/HCD ED ix is mapped to.
  cur_bit_nbr = USBH_OHCI_HCD_ED_LIST_INTR_NODE_IX_BIT_NBR_CONVERT(p_browse_status->Depth, (ed_ix_updated - USBH_OHCI_BinTreeInfoTbl[p_browse_status->Depth].BaseIx));
  //                                                               Iterate through all depth levels above the cur one.
  for (depth_ix = p_browse_status->Depth; depth_ix > USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_32MS; depth_ix--) {
    //                                                             Find prev depth level lowest bit nbr associated ...
    //                                                             ... to the depth level at which we were.
    cur_bit_nbr = cur_bit_nbr * 2u;
    //                                                             Iterate through all bits associated with cur     ...
    //                                                             ... depth bit nbr. This number doubles with each ...
    //                                                             ... difference in depth levels between the cur   ...
    //                                                             ... one and the one we are currently updating.
    for (bit_ix = 0u; bit_ix < (1u << (p_browse_status->Depth - (depth_ix - 1))); ++bit_ix) {
      //                                                           Clr bit, to mark it as 'checked'.
      DEF_BIT_CLR(p_browse_status->NodesBitmap[(depth_ix - 1)], DEF_BIT(cur_bit_nbr + bit_ix));
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                       USBH_BinTreeNextED_IxGet()
 *
 * @brief    Get the next ED ix that needs to be checked during the browsing of the binary tree.
 *
 * @param    p_browse_status     Pointer to binary tree browsing status structure.
 *
 * @return   Next ED ix,                    if there is one remaining.
 *           USBH_OHCI_BIN_TREE_ED_IX_NONE, otherwise.
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static CPU_INT08U USBH_BinTreeNextED_IxGet(USBH_OHCI_BIN_TREE_STATUS *p_browse_status)
{
  CPU_INT08S depth_ix;
  CPU_INT08U next_ed_ix = USBH_OHCI_BIN_TREE_ED_IX_NONE;

  //                                                               Starting at current depth, find what is the next ...
  //                                                               ... ix that needs to be checked.
  //                                                               Browse tree starting from smaller bInterval.
  for (depth_ix = p_browse_status->Depth; depth_ix >= USBH_OHCI_HCD_ED_LIST_INTR_DEPTH_32MS; depth_ix--) {
    //                                                             If this depth level still has un-checked nodes.
    if (DEF_BIT_IS_SET_ANY(p_browse_status->NodesBitmap[depth_ix], USBH_OHCI_BinTreeInfoTbl[depth_ix].Msk) == DEF_YES) {
      CPU_INT08U trail_zeros;

      p_browse_status->Depth = depth_ix;                        // Update depth.
                                                                // Find which bit of the bitmap is still set.
      trail_zeros = CPU_CntTrailZeros32(p_browse_status->NodesBitmap[depth_ix]);
      //                                                           Convert bit nbr to HC/HCD ED ix of this depth level.
      next_ed_ix = USBH_OHCI_HCD_ED_LIST_INTR_NODE_IX_BIT_NBR_CONVERT(depth_ix, trail_zeros) + USBH_OHCI_BinTreeInfoTbl[depth_ix].BaseIx;
      //                                                           Mark this bit/ix as checked.
      DEF_BIT_CLR(p_browse_status->NodesBitmap[depth_ix], DEF_BIT((CPU_INT32U)trail_zeros));

      break;
    }
  }

  return (next_ed_ix);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
