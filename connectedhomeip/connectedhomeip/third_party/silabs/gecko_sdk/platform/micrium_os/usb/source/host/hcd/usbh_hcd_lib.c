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

#define   HCD_LIB_MODULE
#define   MICRIUM_SOURCE
#include  <common/include/lib_math.h>
#include  <common/include/lib_mem.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_ep.h>

#include  <usb/source/host/hcd/usbh_hcd_lib_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_core_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_hub_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, HCD, LIB)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static CPU_INT16U USBH_HCD_BinTreeBranchRemBwGet(USBH_HCD_BIN_TREE_BW *p_bin_tree,
                                                 CPU_INT16U           branch_linear_ix,
                                                 RTOS_ERR             *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               BINARY TREE BANDWIDTH COMPUTATION FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HCD_BinTreeBwInit()
 *
 * @brief    Initializes binary tree bandwidth.
 *
 * @param    p_bin_tree                  Pointer to binary tree bandwidth structure.
 *
 * @param    p_mem_seg                   Pointer to memory segment from where to allocate bandwidth
 *                                       table.
 *
 * @param    bin_tree_branch_qty         Quantity of branch in binary tree.
 *
 * @param    hc_spd                      Host Controller speed.
 *
 * @param    branch_on_ufrm              Flag that indicates if branches represent uFrame or frame.
 *                                       DEF_YES     Branches represent uFrame.
 *                                       DEF_NO      Branches represent  Frame.
 *
 * @param    bin_tree_bw_get_fnct        Function to call to retrieve bandwidth available in a given
 *                                       branch. Can be null. See note (1).
 *
 * @param    p_bin_tree_bw_get_fnct_arg  Pointer to argument to pass to 'bin_tree_bw_get_fnct'.
 *
 * @param    p_err                       Pointer to variable that will receive the error code from this
 *                                       function.
 *
 * @note     (1) If a valid function pointer is passed to 'bin_tree_bw_get_fnct', no bandwidth table
 *               will be allocated and the bandwidth computation will rely on the HCD to retrieve the
 *               used bandwidth in a branch.
 *******************************************************************************************************/
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
void USBH_HCD_BinTreeBwInit(USBH_HCD_BIN_TREE_BW            *p_bin_tree,
                            MEM_SEG                         *p_mem_seg,
                            CPU_INT16U                      bin_tree_branch_qty,
                            USBH_DEV_SPD                    hc_spd,
                            CPU_BOOLEAN                     branch_on_ufrm,
                            USBH_HCD_BIN_TREE_BRANCH_BW_GET bin_tree_bw_get_fnct,
                            void                            *p_bin_tree_bw_get_fnct_arg,
                            RTOS_ERR                        *p_err)
{
  p_bin_tree->HC_Spd = hc_spd;
  p_bin_tree->BranchOnuFrm = branch_on_ufrm;
  p_bin_tree->BranchQty = bin_tree_branch_qty;
  p_bin_tree->BranchBwGetFnct = bin_tree_bw_get_fnct;
  p_bin_tree->BranchBwGetArgPtr = p_bin_tree_bw_get_fnct_arg;
  p_bin_tree->BranchMaxBw = USBH_HC_MAX_PERIODIC_BW_GET(hc_spd);

  if ((hc_spd == USBH_DEV_SPD_HIGH)
      && (branch_on_ufrm == DEF_NO)) {
    p_bin_tree->BranchMaxBw *= 8u;
  }

  if (bin_tree_bw_get_fnct == DEF_NULL) {
    CPU_INT16U bin_tree_branch_ix;

    p_bin_tree->BranchBwTbl = (CPU_INT16U *)Mem_SegAlloc("USBH - HCD Bin tree BW tbl",
                                                         p_mem_seg,
                                                         bin_tree_branch_qty * sizeof(CPU_INT16U),
                                                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    for (bin_tree_branch_ix = 0u; bin_tree_branch_ix < bin_tree_branch_qty; bin_tree_branch_ix++) {
      p_bin_tree->BranchBwTbl[bin_tree_branch_ix] = p_bin_tree->BranchMaxBw;
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }
}
#endif

/****************************************************************************************************//**
 *                                   USBH_HCD_BinTreeBwHighestPosFind()
 *
 * @brief    Finds best position to insert a new periodic endpoint in the binary tree.
 *
 * @param    p_bin_tree              Pointer to binary tree bandwidth structure.
 *
 * @param    p_hcd_ep_params         Pointer to Host Controller Driver endpoint parameters structure.
 *
 * @param    split_force_same_frm    Flag that indicates if, in case of split transaction, the SSPLIT
 *
 * @param    p_err                   Pointer to variable that will receive the error code from this
 *                                   function.
 *
 * @return   Position descriptor where to insert endpoint.
 *
 * @note     (1) The format of the position where to insert endpoint is given in terms of level and
 *               index (phase). Drawing below shows an example of a binary tree for a maximum
 *               interval of 8.
 *
 *           l      ->  0   1     2       3
 *           rval   ->  8   4     2       1
 *
 *                       O0
 *                          > O0
 *                       O1     \
 *                               > O0
 *                       O2     /     \
 *                         > O1        \
 *                       O3             \
 *                                       > O0
 *                       O4             /
 *                         > O2        /
 *                       O5     \     /
 *                               > O1
 *                       O6     /
 *                         > O3
 *                       O7
 *                       ^   ^     ^       ^
 *                       |___|_____|_______|
 *                               |
 *                           Indexes
 *******************************************************************************************************/

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
USBH_HCD_BIN_TREE_EP_DESC USBH_HCD_BinTreeBwHighestPosFind(USBH_HCD_BIN_TREE_BW *p_bin_tree,
                                                           USBH_HCD_EP_PARAMS   *p_hcd_ep_params,
                                                           CPU_BOOLEAN          split_force_same_frm,
                                                           RTOS_ERR             *p_err)
{
  CPU_INT08U                level;
  CPU_INT08U                level_qty;
  CPU_INT16U                interval_branch_qty;
  CPU_INT16U                max_bw_interval_ix = 0u;
  CPU_INT16U                interval_branch_cnt;
  CPU_INT16U                max_bw = 0u;
  CPU_INT16U                main_time_slot_needed_bw = 0u;
  CPU_INT16U                interval_cnt = 0u;
  CPU_INT16U                adjusted_interval;
  CPU_BOOLEAN               split_not_same_ufrm;
  CPU_BOOLEAN               split_same_frm = DEF_NO;
  CPU_BOOLEAN               split_other_is_ssplit;
  USBH_HCD_BIN_TREE_EP_DESC bin_tree_ep_desc = { 0xFFu, 0xFFu, 0u };

  //                                                               Compute needed bandwidth for given EP.
  bin_tree_ep_desc.MainTimeSlotNeededBW = USBH_HCD_EP_NeededBwGet(p_bin_tree,
                                                                  p_hcd_ep_params);

  main_time_slot_needed_bw = DEF_BIT_FIELD_RD(bin_tree_ep_desc.MainTimeSlotNeededBW,
                                              USBH_HCD_LIB_NEEDED_BW_VAL_MSK);

  //                                                               Round interval down to nearest pwr of 2.
  adjusted_interval = DEF_BIT(DEF_INT_16_NBR_BITS - CPU_CntLeadZeros16(p_hcd_ep_params->Interval) - 1u);
  level_qty = CPU_CntTrailZeros16(p_bin_tree->BranchQty) + 1u;
  level = level_qty - CPU_CntTrailZeros16(adjusted_interval) - 1;
  interval_branch_qty = DEF_BIT(level);

  split_not_same_ufrm = DEF_BIT_IS_SET(bin_tree_ep_desc.MainTimeSlotNeededBW,
                                       USBH_HCD_LIB_NEEDED_BW_OTHER_SPLIT_OTHER_UFRM);
  split_other_is_ssplit = DEF_BIT_IS_SET(bin_tree_ep_desc.MainTimeSlotNeededBW,
                                         USBH_HCD_LIB_NEEDED_BW_OTHER_SPLIT_SSPLIT);

  if ((split_force_same_frm == DEF_YES)
      && (split_not_same_ufrm == DEF_YES)) {
    split_same_frm = DEF_YES;
  }

  interval_cnt = 0u;

  //                                                               Find interval that has most avail BW.
  while (interval_cnt < adjusted_interval) {
    CPU_INT16U interval_min_bw = p_bin_tree->BranchMaxBw;

    //                                                             Find branch for cur interval that has less BW avail.
    for (interval_branch_cnt = 0u; interval_branch_cnt < interval_branch_qty; interval_branch_cnt++) {
      CPU_INT16U rem_bw;
      CPU_INT16U branch_linear_ix = interval_branch_cnt;

      if ((split_same_frm == DEF_NO)
          || (split_other_is_ssplit == DEF_NO)) {
        branch_linear_ix += (interval_cnt * interval_branch_qty);
      } else {
        //                                                         Skip first 2 intervals for SSPLIT.
        branch_linear_ix += ((interval_cnt + USBH_HCD_LIB_SPLIT_PHASE_GAP) * interval_branch_qty);
      }

      rem_bw = USBH_HCD_BinTreeBranchRemBwGet(p_bin_tree,
                                              branch_linear_ix,
                                              p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (bin_tree_ep_desc);
      }

      interval_min_bw = DEF_MIN(rem_bw, interval_min_bw);
    }

    if (max_bw < interval_min_bw) {                             // If more avail BW then prev intervals, keep this one.
      CPU_BOOLEAN other_split_bw_avail = DEF_YES;

      if (split_not_same_ufrm == DEF_YES) {                     // Ensure BW avail for other split phase if necessary.
        CPU_INT16U other_split_rem_bw;
        CPU_INT16U other_split_linear_ix;
        CPU_INT16U other_split_needed_bw = (split_other_is_ssplit == DEF_YES) ? USBH_HCD_LIB_BYTE_TIME_OTHER_SPLIT_SSPLIT
                                           : USBH_HCD_LIB_BYTE_TIME_OTHER_SPLIT_CSPLIT;

        if (split_other_is_ssplit == DEF_YES) {
          other_split_linear_ix = (interval_cnt * interval_branch_qty);
        } else {
          other_split_linear_ix = ((interval_cnt + USBH_HCD_LIB_SPLIT_PHASE_GAP) * interval_branch_qty);
          other_split_linear_ix %= p_bin_tree->BranchQty;
        }

        other_split_rem_bw = USBH_HCD_BinTreeBranchRemBwGet(p_bin_tree,
                                                            other_split_linear_ix,
                                                            p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return (bin_tree_ep_desc);
        }

        if (other_split_rem_bw < other_split_needed_bw) {
          other_split_bw_avail = DEF_NO;
        }
      }

      if (other_split_bw_avail == DEF_YES) {
        max_bw = interval_min_bw;
        max_bw_interval_ix = interval_cnt;
      }
    }

    //                                                             If all BW avail for given interval, stop searching.
    if (interval_min_bw >= p_bin_tree->BranchMaxBw) {
      break;
    }

    if ((split_same_frm == DEF_NO)
        || ((interval_cnt & 0x07u) < (7u - USBH_HCD_LIB_SPLIT_PHASE_GAP))) {
      interval_cnt++;
    } else {
      interval_cnt += USBH_HCD_LIB_SPLIT_PHASE_GAP + 1u;
    }
  }

  //                                                               Ensure enough BW avail for EP.
  if (((CPU_INT16S)(max_bw - main_time_slot_needed_bw)) < 0) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_EP_BW_NOT_AVAIL);
    return (bin_tree_ep_desc);
  }

  bin_tree_ep_desc.Level = level;
  bin_tree_ep_desc.IntervalLinearIx = max_bw_interval_ix;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (bin_tree_ep_desc);
}
#endif

/****************************************************************************************************//**
 *                                       USBH_HCD_BinTreeBwUpdate()
 *
 * @brief    Updates binary tree bandwidth table.
 *
 * @param    p_bin_tree          Pointer to binary tree bandwidth structure.
 *
 * @param    bin_tree_ep_desc    Position descriptor.
 *
 * @param    substract           Flag that indicates if bandwidth should be substracted or added.
 *                               DEF_YES     Substract bandwidth from branch.
 *                               DEF_NO      Add       bandwidth to   branch.
 *
 * @note     (1) Calling this function has no effect if a branch bandwidth get function was provided.
 *******************************************************************************************************/
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
void USBH_HCD_BinTreeBwUpdate(USBH_HCD_BIN_TREE_BW      *p_bin_tree,
                              USBH_HCD_BIN_TREE_EP_DESC bin_tree_ep_desc,
                              CPU_BOOLEAN               substract)
{
  if (p_bin_tree->BranchBwGetFnct == DEF_NULL) {
    CPU_BOOLEAN split_not_same_ufrm;
    CPU_BOOLEAN split_other_is_ssplit;
    CPU_INT16U  interval_branch_qty = DEF_BIT(bin_tree_ep_desc.Level);
    CPU_INT16U  interval_branch_start_ix;
    CPU_INT16U  interval_branch_cnt;
    CPU_INT16U  main_time_slot_needed_bw;

    main_time_slot_needed_bw = DEF_BIT_FIELD_RD(bin_tree_ep_desc.MainTimeSlotNeededBW,
                                                USBH_HCD_LIB_NEEDED_BW_VAL_MSK);
    split_not_same_ufrm = DEF_BIT_IS_SET(bin_tree_ep_desc.MainTimeSlotNeededBW,
                                         USBH_HCD_LIB_NEEDED_BW_OTHER_SPLIT_OTHER_UFRM);
    split_other_is_ssplit = DEF_BIT_IS_SET(bin_tree_ep_desc.MainTimeSlotNeededBW,
                                           USBH_HCD_LIB_NEEDED_BW_OTHER_SPLIT_SSPLIT);

    if ((split_not_same_ufrm == DEF_NO)
        || (split_other_is_ssplit == DEF_NO)) {
      interval_branch_start_ix = bin_tree_ep_desc.IntervalLinearIx * interval_branch_qty;
    } else {
      interval_branch_start_ix = (bin_tree_ep_desc.IntervalLinearIx + USBH_HCD_LIB_SPLIT_PHASE_GAP) * interval_branch_qty;
      interval_branch_start_ix %= p_bin_tree->BranchQty;
    }

    for (interval_branch_cnt = 0u; interval_branch_cnt < interval_branch_qty; interval_branch_cnt++) {
      CPU_INT16U bin_tree_ix;
      CPU_INT16U branch_linear_ix = interval_branch_start_ix + interval_branch_cnt;

      bin_tree_ix = USBH_HCD_BinTreeIxToggle(p_bin_tree->BranchQty,
                                             branch_linear_ix);

      p_bin_tree->BranchBwTbl[bin_tree_ix] += (substract == DEF_YES) ? -(CPU_INT16S)main_time_slot_needed_bw
                                              : main_time_slot_needed_bw;

      if (split_not_same_ufrm == DEF_YES) {
        CPU_INT16U other_split_start_ix;
        CPU_INT16U other_split_linear_ix;
        CPU_INT16U other_split_needed_bw;

        if (split_other_is_ssplit == DEF_YES) {
          other_split_start_ix = bin_tree_ep_desc.IntervalLinearIx * interval_branch_qty;
        } else {
          other_split_start_ix = (bin_tree_ep_desc.IntervalLinearIx + USBH_HCD_LIB_SPLIT_PHASE_GAP) * interval_branch_qty;
        }

        other_split_linear_ix = other_split_start_ix + interval_branch_cnt;
        other_split_linear_ix %= p_bin_tree->BranchQty;

        other_split_needed_bw = (split_other_is_ssplit == DEF_YES) ? USBH_HCD_LIB_BYTE_TIME_OTHER_SPLIT_SSPLIT
                                : USBH_HCD_LIB_BYTE_TIME_OTHER_SPLIT_CSPLIT;

        bin_tree_ix = USBH_HCD_BinTreeIxToggle(p_bin_tree->BranchQty,
                                               other_split_linear_ix);

        p_bin_tree->BranchBwTbl[bin_tree_ix] += (substract > 0u) ? -(CPU_INT16S)other_split_needed_bw
                                                : other_split_needed_bw;
      }
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                       USBH_HCD_BinTreeIxToggle()
 *
 * @brief    Toggles an index between a binary tree linear index and host controller index.
 *
 * @param    interval    Interval.
 *
 * @param    ix          Interval index (phase).
 *
 * @return   Toggled index.
 *
 * @note     (1) The interval index is a continuous index (0-1-2-...) retrieved after computation of
 *               the bandwidth in the branches. The index for this interval in the binary tree is, on
 *               the other hand, not continuous and organized in a way that helps periodic scheduling.
 *
 *               The following example stands for 16 intervals. This functions simply reverse the
 *               bits of the index.
 *
 *               Interval index    -> 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
 *               Binary tree index -> 0  8  4  12 2  10 6  14 1  9  5  13 3  11 7  15
 *******************************************************************************************************/
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
CPU_INT16U USBH_HCD_BinTreeIxToggle(CPU_INT16U interval,
                                    CPU_INT16U ix)
{
  CPU_INT16U toggled_ix = ix;
  CPU_INT08U depth;

  depth = CPU_CntTrailZeros16(interval);
  toggled_ix = (((toggled_ix & 0xAAAAu) >> 1u) | ((toggled_ix & 0x5555u) << 1u));
  toggled_ix = (((toggled_ix & 0xCCCCu) >> 2u) | ((toggled_ix & 0x3333u) << 2u));
  toggled_ix = (((toggled_ix & 0xF0F0u) >> 4u) | ((toggled_ix & 0x0F0Fu) << 4u));
  toggled_ix = ((toggled_ix            >> 8u) |  (toggled_ix << 8u));
  toggled_ix = (toggled_ix >> (DEF_INT_16_NBR_BITS - depth));

  return (toggled_ix);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HCD_EP_NeededBwGet()
 *
 * @brief    Computes needed bandwidth in bytes for given endpoint.
 *
 * @param    p_bin_tree          Pointer to binary tree structure.
 *
 * @param    p_hcd_ep_params     Pointer to Host Controller Driver endpoint parameters.
 *
 * @return   Needed bandwidth, in bytes.
 *
 * @note     (1) This function computes the needed bandwidth, in bytes, for given periodic endpoint.
 *               - (a) In case of a split transaction, if the binary tree branches hold bandwidth for
 *                     each uFrame, the function must take into account that the SSPLIT and CSPLIT
 *                     phases won't occur on the same uFrame.
 *               - (b) In case of a split transaction, this function will compute the needed bandwidth
 *                     for the uFrame in which the data phase will occur. The data phase occurence
 *                     depends on the direction of the endpoint:
 *                   - (1) For IN  endpoints, the data phase occurs in the CSPLIT phase along the
 *                         handshake packet.
 *                   - (2) For OUT endpoints, the data phase occurs in the SSPLIT phase along the
 *                         token packet.
 *******************************************************************************************************/
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
CPU_INT16U USBH_HCD_EP_NeededBwGet(USBH_HCD_BIN_TREE_BW *p_bin_tree,
                                   USBH_HCD_EP_PARAMS   *p_hcd_ep_params)
{
  CPU_BOOLEAN is_split = DEF_NO;
  CPU_BOOLEAN ep_is_in = USBH_EP_IS_IN(p_hcd_ep_params->Addr);
  CPU_INT16U  main_time_slot_needed_bw;
  CPU_INT16U  max_pkt_size = USBH_EP_MAX_PKT_SIZE_GET(p_hcd_ep_params->MaxPktSize);

  if ((p_hcd_ep_params->DevSpd != USBH_DEV_SPD_HIGH)
      && (p_bin_tree->HC_Spd == USBH_DEV_SPD_HIGH)) {
    is_split = DEF_YES;
  }

  //                                                               -------------------- PAYLOAD BW --------------------
  main_time_slot_needed_bw = USBH_EP_MAX_PKT_SIZE_GET(max_pkt_size);

  //                                                               Add overhead for bit stuffing.
  main_time_slot_needed_bw += 48u;
  if (USBH_EP_MAX_PKT_SIZE_GET(max_pkt_size) >= 128u) {
    main_time_slot_needed_bw += 128u;
  }

  //                                                               Add twice delay between consecutive host pkts.
  main_time_slot_needed_bw += USBH_HCD_LIB_BYTE_TIME_HOST_2_HOST_IPG * 2u;

  //                                                               Add turnaround.
  main_time_slot_needed_bw += (p_bin_tree->HC_Spd != USBH_DEV_SPD_HIGH) ? USBH_HCD_LIB_BYTE_TIME_TURNAROUND_FS
                              : USBH_HCD_LIB_BYTE_TIME_TURNAROUND_HS;

  if ((is_split == DEF_NO)
      || (ep_is_in == DEF_NO)
      || (p_bin_tree->BranchOnuFrm == DEF_NO)) {
    main_time_slot_needed_bw += USBH_HCD_LIB_BYTE_TIME_TOKEN;
  }

  if ((is_split == DEF_NO)
      || (ep_is_in == DEF_YES)
      || (p_bin_tree->BranchOnuFrm == DEF_NO)) {
    main_time_slot_needed_bw += USBH_HCD_LIB_BYTE_TIME_HANDSHAKE;
  }

  //                                                               Add split if needed.
  if (is_split == DEF_YES) {
    main_time_slot_needed_bw += USBH_HCD_LIB_BYTE_TIME_SPLIT;

    if (p_bin_tree->BranchOnuFrm == DEF_NO) {
      //                                                           Add another SPLIT for other SPLIT phase.
      main_time_slot_needed_bw += USBH_HCD_LIB_BYTE_TIME_SPLIT;
    }
  }

  if (p_bin_tree->BranchOnuFrm == DEF_YES) {
    //                                                             Multiply needed bw by nbr of transaction per ufrm.
    main_time_slot_needed_bw *= USBH_EP_NBR_TRANSACTION_GET(max_pkt_size) + 1u;

    if (is_split == DEF_YES) {
      DEF_BIT_SET(main_time_slot_needed_bw,
                  USBH_HCD_LIB_NEEDED_BW_OTHER_SPLIT_OTHER_UFRM);

      if (ep_is_in == DEF_YES) {
        DEF_BIT_SET(main_time_slot_needed_bw,
                    USBH_HCD_LIB_NEEDED_BW_OTHER_SPLIT_SSPLIT);
      }
    }
  }

  return (main_time_slot_needed_bw);
}
#endif

/****************************************************************************************************//**
 *                                       USBH_HCD_BinTreeBranchRemBwGet()
 *
 * @brief    Gets remaining bandwidth for given branch.
 *
 * @param    p_bin_tree          Pointer to binary tree structure.
 *
 * @param    branch_linear_ix    Linear index of branch.
 *
 * @param    p_err               Pointer to variable that will receive the error code from this
 *                               function.
 *
 * @return   Remaining bandwidth in bytes.
 *******************************************************************************************************/
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
static CPU_INT16U USBH_HCD_BinTreeBranchRemBwGet(USBH_HCD_BIN_TREE_BW *p_bin_tree,
                                                 CPU_INT16U           branch_linear_ix,
                                                 RTOS_ERR             *p_err)
{
  CPU_INT16U bin_tree_ix;
  CPU_INT16U rem_bw;

  bin_tree_ix = USBH_HCD_BinTreeIxToggle(p_bin_tree->BranchQty,
                                         branch_linear_ix);

  if (p_bin_tree->BranchBwGetFnct == DEF_NULL) {
    rem_bw = p_bin_tree->BranchBwTbl[bin_tree_ix];
  } else {
    CPU_INT16U used_bw;

    used_bw = p_bin_tree->BranchBwGetFnct(bin_tree_ix,
                                          p_bin_tree->BranchBwGetArgPtr,
                                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (0u);
    }

    rem_bw = p_bin_tree->BranchMaxBw - used_bw;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (rem_bw);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
