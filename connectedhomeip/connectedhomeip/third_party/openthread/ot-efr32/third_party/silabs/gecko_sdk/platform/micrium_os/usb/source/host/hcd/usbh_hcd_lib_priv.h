/***************************************************************************//**
 * @file
 * @brief USB Host - Host Controller Driver Library
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_HCD_LIB_PRIV_H_
#define  _USBH_HCD_LIB_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_err.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/source/host/hcd/usbh_hcd_core_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_HCD_LIB_NEEDED_BW_OTHER_SPLIT_OTHER_UFRM      DEF_BIT_15
#define  USBH_HCD_LIB_NEEDED_BW_OTHER_SPLIT_SSPLIT          DEF_BIT_14
#define  USBH_HCD_LIB_NEEDED_BW_VAL_MSK                     DEF_BIT_FIELD(13u, 0u)

#define  USBH_HCD_LIB_SPLIT_PHASE_GAP                   2u

#define  USBH_HCD_LIB_BYTE_TIME_SPLIT                   10
#define  USBH_HCD_LIB_BYTE_TIME_HOST_2_HOST_IPG         11
#define  USBH_HCD_LIB_BYTE_TIME_TOKEN                    8
#define  USBH_HCD_LIB_BYTE_TIME_DATA_PKT_OVERHEAD        8
#define  USBH_HCD_LIB_BYTE_TIME_TURNAROUND_FS            7
#define  USBH_HCD_LIB_BYTE_TIME_TURNAROUND_HS           90
#define  USBH_HCD_LIB_BYTE_TIME_HANDSHAKE                6

/********************************************************************************************************
 *                                       OTHER SPLIT NEEDED BANDWIDTH
 *
 * Note(s) : (1) In case of a split transaction, the "other split" phase means the split phase that does
 *               not contain the data phase.
 *******************************************************************************************************/

#define  USBH_HCD_LIB_BYTE_TIME_OTHER_SPLIT_SSPLIT      (USBH_HCD_LIB_BYTE_TIME_SPLIT \
                                                         + USBH_HCD_LIB_BYTE_TIME_TOKEN)
#define  USBH_HCD_LIB_BYTE_TIME_OTHER_SPLIT_CSPLIT      (USBH_HCD_LIB_BYTE_TIME_SPLIT \
                                                         + USBH_HCD_LIB_BYTE_TIME_HANDSHAKE)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ---------- BRANCH BANDWIDTH GET FUNCTION -----------
typedef CPU_INT16U (*USBH_HCD_BIN_TREE_BRANCH_BW_GET)   (CPU_INT16U branch_ix,
                                                         void       *p_arg,
                                                         RTOS_ERR   *p_err);

/********************************************************************************************************
 *                                   BINARY TREE POSITION DESCRIPTOR
 *
 * Note(s) : (1) Format of MainTimeSlotNeededBW:
 *
 *               15                   14                      13..0
 *           +--------------------+----------------------+------------------------------+
 *           | Other split phase  | Other split phase is | Bandwidth needed, in bytes.  |
 *           | not in same uFrm.  | Start Split (SSPLIT) |                              |
 *           +--------------------+----------------------+------------------------------+
 *
 *               (a) Field "Other split phase not in same uFrm" indicates, in case of a split, that the
 *                   other split phase is not part of the same uFrame and will have to be handled
 *                   seperately by the caller. This can only occur if binary tree holds bandwidth on
 *                   uFrame time slots. Split transaction can happen otherwise, but it will be transparent
 *                   to the caller.
 *
 *               (b) Field "Other split phase is Start Split (SSPLIT)" is only relevant if bit 15 is set.
 *                   Its purpose is to indicate which split phase does NOT contain the data phase of the
 *                   transaction and hence for which the needed bandwidth is not taken into account in
 *                   field "Bandwidth needed, in bytes". The HCD lib will ensure that bandwidth is
 *                   available for that split phase as well.
 *
 *                       0: Other split phase is a "Complete Split" (CSPLIT). Endpoint direction is OUT.
 *                       1: Other split phase is a "Start    Split" (SSPLIT). Endpoint direction is IN.
 *
 *               (c) Field "Bandwidth needed, in bytes", contain the needed bandwidth, in bytes for the
 *                   given endpoint. In case of split, all the bandwidth required for split transfers will
 *                   be taken into account.
 *******************************************************************************************************/

typedef struct usbh_hcd_bin_tree_ep_desc {
  CPU_INT08U Level;
  CPU_INT16U IntervalLinearIx;

  CPU_INT16U MainTimeSlotNeededBW;                              // Bandwidth needed for data packet. See Note 1.
} USBH_HCD_BIN_TREE_EP_DESC;

//                                                                 ------------ BINARY TREE BANDWIDTH TYPE ------------
typedef struct usbh_hcd_bin_tree_bw {
  USBH_DEV_SPD                    HC_Spd;
  CPU_BOOLEAN                     BranchOnuFrm;                 // Flag indicates if BW computed on ufrm or frm unit.

  CPU_INT16U                      BranchQty;

  CPU_INT16U                      *BranchBwTbl;
  CPU_INT16U                      BranchMaxBw;

  USBH_HCD_BIN_TREE_BRANCH_BW_GET BranchBwGetFnct;
  void                            *BranchBwGetArgPtr;
} USBH_HCD_BIN_TREE_BW;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
void USBH_HCD_BinTreeBwInit(USBH_HCD_BIN_TREE_BW            *p_bin_tree,
                            MEM_SEG                         *p_mem_seg,
                            CPU_INT16U                      bin_tree_branch_qty,
                            USBH_DEV_SPD                    hc_spd,
                            CPU_BOOLEAN                     branch_on_ufrm,
                            USBH_HCD_BIN_TREE_BRANCH_BW_GET bin_tree_bw_get_fnct,
                            void                            *p_bin_tree_bw_get_fnct_arg,
                            RTOS_ERR                        *p_err);

USBH_HCD_BIN_TREE_EP_DESC USBH_HCD_BinTreeBwHighestPosFind(USBH_HCD_BIN_TREE_BW *p_bin_tree,
                                                           USBH_HCD_EP_PARAMS   *p_hcd_ep_params,
                                                           CPU_BOOLEAN          split_force_same_frm,
                                                           RTOS_ERR             *p_err);

void USBH_HCD_BinTreeBwUpdate(USBH_HCD_BIN_TREE_BW      *p_bin_tree,
                              USBH_HCD_BIN_TREE_EP_DESC bin_tree_ep_desc,
                              CPU_BOOLEAN               substract);

CPU_INT16U USBH_HCD_BinTreeIxToggle(CPU_INT16U interval,
                                    CPU_INT16U ix);

CPU_INT16U USBH_HCD_EP_NeededBwGet(USBH_HCD_BIN_TREE_BW *p_bin_tree,
                                   USBH_HCD_EP_PARAMS   *p_hcd_ep_params);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
