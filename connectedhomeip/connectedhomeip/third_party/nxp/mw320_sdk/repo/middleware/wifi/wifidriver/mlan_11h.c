/** @file mlan_11h.c
 *
 *  @brief  This file provides functions for 802.11H
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

/*************************************************************
Change Log:
    03/26/2009: initial version
************************************************************/

#include <mlan_api.h>

/* Additional WMSDK header files */
#include <wmerrno.h>
#include <wm_os.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>

/********************************************************
                Local Variables
********************************************************/
/** Default IBSS DFS recovery interval (in TBTTs); used for adhoc start */
#define WLAN_11H_DEFAULT_DFS_RECOVERY_INTERVAL 100

/** Default 11h power constraint used to offset the maximum transmit power */
#define WLAN_11H_TPC_POWERCONSTRAINT 0

/** 11h TPC Power capability minimum setting, sent in TPC_INFO command to fw */
#define WLAN_11H_TPC_POWERCAPABILITY_MIN 5

/** 11h TPC Power capability maximum setting, sent in TPC_INFO command to fw */
#define WLAN_11H_TPC_POWERCAPABILITY_MAX 20

/** Regulatory requirement for the duration of a channel availability check */
#define WLAN_11H_CHANNEL_AVAIL_CHECK_DURATION 60000 /* in ms */

/** Starting Frequency for 11A band */
#define START_FREQ_11A_BAND 5000 /* in MHz */

/** Regulatory requirement for the duration of a non-occupancy period */
#define WLAN_11H_NON_OCCUPANCY_PERIOD 1800 /* in sec (30mins) */

/** Maximum allowable age (seconds) on DFS report data */
#define MAX_DFS_REPORT_USABLE_AGE_SEC (120) // 2 minutes

/** Minimum delay for CHAN_SW IE to broadcast by FW */
#define MIN_RDH_CHAN_SW_IE_PERIOD_MSEC (500) // 5 beacons @ 100ms

/** Maximum delay for CHAN_SW IE to broadcast by FW */
#define MAX_RDH_CHAN_SW_IE_PERIOD_MSEC (3000) // 5 beacons @ 600ms

/** Maximum retries on selecting new random channel */
#define MAX_RANDOM_CHANNEL_RETRIES (20)

/** Maximum retries on selecting new random non-dfs channel */
#define MAX_SWITCH_CHANNEL_RETRIES (30)

/** Value for undetermined priv_curr_idx on first entry to new RDH stage */
#define RDH_STAGE_FIRST_ENTRY_PRIV_IDX (0xff)

/** Internally passed structure used to send a CMD_802_11_TPC_INFO command */
typedef struct
{
    t_u8 chan;             /**< Channel to which the power constraint applies */
    t_u8 power_constraint; /**< Local power constraint to send to firmware */
} wlan_11h_tpc_info_param_t;

/********************************************************
                Global Variables
********************************************************/

/********************************************************
                Local Functions
********************************************************/

/**
 *  @brief Initialize the 11h parameters and enable 11h when starting an IBSS
 *
 *  @param adapter mlan_adapter structure
 *
 *  @return      N/A
 */
t_void wlan_11h_init(mlan_adapter *adapter)
{
    wlan_11h_device_state_t *pstate_11h      = &adapter->state_11h;
    IEEEtypes_Quiet_t *pquiet                = &adapter->state_11h.quiet_ie;
    wlan_dfs_device_state_t *pstate_dfs      = &adapter->state_dfs;
    wlan_radar_det_hndlg_state_t *pstate_rdh = &adapter->state_rdh;

    ENTER();

    /* Initialize 11H struct */
    pstate_11h->usr_def_power_constraint        = WLAN_11H_TPC_POWERCONSTRAINT;
    pstate_11h->min_tx_power_capability         = WLAN_11H_TPC_POWERCAPABILITY_MIN;
    pstate_11h->max_tx_power_capability         = WLAN_11H_TPC_POWERCAPABILITY_MAX;
    pstate_11h->recvd_chanswann_event           = MFALSE;
    pstate_11h->master_radar_det_enable_pending = MFALSE;
    pstate_11h->slave_radar_det_enable_pending  = MFALSE;
    pstate_11h->is_master_radar_det_active      = MFALSE;
    pstate_11h->is_slave_radar_det_active       = MFALSE;

    /* Initialize quiet_ie */
    memset(adapter, pquiet, 0, sizeof(IEEEtypes_Quiet_t));
    pquiet->element_id = QUIET;
    pquiet->len        = (sizeof(pquiet->quiet_count) + sizeof(pquiet->quiet_period) + sizeof(pquiet->quiet_duration) +
                   sizeof(pquiet->quiet_offset));

    /* Initialize DFS struct */
    pstate_dfs->dfs_check_pending   = MFALSE;
    pstate_dfs->dfs_radar_found     = MFALSE;
    pstate_dfs->dfs_check_channel   = 0;
    pstate_dfs->dfs_report_time_sec = 0;
    util_init_list((pmlan_linked_list)&pstate_dfs->dfs_ts_head);

    /* Initialize RDH struct */
    pstate_rdh->stage           = RDH_OFF;
    pstate_rdh->priv_list_count = 0;
    pstate_rdh->priv_curr_idx   = 0;
    pstate_rdh->curr_channel    = 0;
    pstate_rdh->new_channel     = 0;
    pstate_rdh->uap_band_cfg    = 0;
    pstate_rdh->max_bcn_dtim_ms = 0;
    memset(adapter, pstate_rdh->priv_list, 0, sizeof(pstate_rdh->priv_list));

    LEAVE();
}

/**
 *  @brief Initialize the 11h parameters and enable 11h when starting an IBSS
 *
 *  @param pmpriv Pointer to mlan_private structure
 *
 *  @return      N/A
 */
t_void wlan_11h_priv_init(mlan_private *pmpriv)
{
    wlan_11h_interface_state_t *pistate_11h = &pmpriv->intf_state_11h;

    ENTER();

    pistate_11h->is_11h_enabled      = MTRUE;
    pistate_11h->is_11h_active       = MFALSE;
    pistate_11h->adhoc_auto_sel_chan = MTRUE;
    pistate_11h->tx_disabled         = MFALSE;

    LEAVE();
}

/**
 *  @brief Check if the current region's regulations require the input channel
 *         to be scanned for radar.
 *
 *  Based on statically defined requirements for sub-bands per regulatory
 *    agency requirements.
 *
 *  Used in adhoc start to determine if channel availability check is required
 *
 *  @param priv    Private driver information structure
 *  @param channel Channel to determine radar detection requirements
 *
 *  @return
 *    - MTRUE if radar detection is required
 *    - MFALSE otherwise
 */
/**  @sa wlan_11h_issue_radar_detect
 */
t_bool wlan_11h_radar_detect_required(mlan_private *priv, t_u8 channel)
{
    t_bool required = MFALSE;

    ENTER();

    /*
     *   No checks for 11h or measurement code being enabled is placed here
     *   since regulatory requirements exist whether we support them or not.
     */

    required = wlan_get_cfp_radar_detect(priv, channel);

    if (!priv->adapter->region_code)
        PRINTM(MINFO,
               "11h: Radar detection in CFP code[BG:%#x, A:%#x] "
               "is %srequired for channel %d\n",
               priv->adapter->cfp_code_bg, priv->adapter->cfp_code_a, (required ? "" : "not "), channel);
    else
        PRINTM(MINFO,
               "11h: Radar detection in region %#02x "
               "is %srequired for channel %d\n",
               priv->adapter->region_code, (required ? "" : "not "), channel);

    if (required == MTRUE && priv->media_connected == MTRUE && priv->curr_bss_params.bss_descriptor.channel == channel)
    {
        required = MFALSE;

        PRINTM(MINFO,
               "11h: Radar detection not required. "
               "Already operating on the channel\n");
    }

    LEAVE();
    return required;
}
