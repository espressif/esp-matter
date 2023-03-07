/** @file mlan_init.c
 *
 *  @brief  This file provides initialization for FW and HW
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

/********************************************************
Change log:
    10/13/2008: initial version
********************************************************/

#include <mlan_api.h>

/* Additional WMSDK header files */
#include <wmerrno.h>
#include <wm_os.h>
#include "fsl_common.h"
#include "sdmmc_config.h"

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>
/********************************************************
        Global Variables
********************************************************/

/* We are allocating BSS list globally as we need heap for other purposes */
__attribute__((section(".wlan_data"))) static BSSDescriptor_t BSS_List[MRVDRV_MAX_BSSID_LIST];

//_IOBUFS_ALIGNED(SDIO_DMA_ALIGNMENT)
#if defined(SD8977) || defined(SD8978) || defined(SD8987) || defined(SD8997) || defined(SD9097) || defined(SD9098)
static t_u8 mp_regs_buffer[MAX_MP_REGS + DMA_ALIGNMENT];
#elif defined(SD8801)
SDK_ALIGN(uint8_t mp_regs_buffer[MAX_MP_REGS], BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE);
#endif

/********************************************************
        Local Functions
********************************************************/

/********************************************************
        Global Functions
********************************************************/

/**
 *  @brief This function allocates buffer for the members of adapter
 *  		structure like command buffer and BSSID list.
 *
 *  @param pmadapter A pointer to mlan_adapter structure
 *
 *  @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_allocate_adapter(pmlan_adapter pmadapter)
{
    // fixme: this function will need during migration of legacy code.
    pmadapter->pscan_table = BSS_List;
    /* wmsdk: Use a statically allocated DMA aligned buffer */
#if defined(SD8801)
    pmadapter->mp_regs = mp_regs_buffer;
#elif defined(SD8977) || defined(SD8978) || defined(SD8987) || defined(SD8997) || defined(SD9097) || defined(SD9098)
    pmadapter->mp_regs = (t_u8 *)ALIGN_ADDR(mp_regs_buffer, DMA_ALIGNMENT);
// mp_regs_buffer;
#endif

    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function initializes the private structure
 *  		and sets default values to the members of mlan_private.
 *
 *  @param priv    A pointer to mlan_private structure
 *
 *  @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_init_priv(pmlan_private priv)
{
    t_u32 i;
    /* pmlan_adapter pmadapter = priv->adapter; */
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    priv->media_connected = MFALSE;
    memset(pmadapter, priv->curr_addr, 0xff, MLAN_MAC_ADDR_LENGTH);

    priv->pkt_tx_ctrl = 0;
    priv->bss_mode    = MLAN_BSS_MODE_INFRA;

    priv->data_rate         = 0; /* Initially indicate the rate as auto */
    priv->data_rate_index   = -1;
    priv->is_data_rate_auto = MTRUE;
    priv->bcn_avg_factor    = DEFAULT_BCN_AVG_FACTOR;
    priv->data_avg_factor   = DEFAULT_DATA_AVG_FACTOR;

    priv->sec_info.wep_status          = Wlan802_11WEPDisabled;
    priv->sec_info.authentication_mode = MLAN_AUTH_MODE_AUTO;
    priv->sec_info.encryption_mode     = MLAN_ENCRYPTION_MODE_NONE;
    for (i = 0; i < sizeof(priv->wep_key) / sizeof(priv->wep_key[0]); i++)
        memset(pmadapter, &priv->wep_key[i], 0, sizeof(mrvl_wep_key_t));
    priv->wep_key_curr_index = 0;
    priv->ewpa_query         = MFALSE;
    priv->adhoc_aes_enabled  = MFALSE;
    priv->curr_pkt_filter    = HostCmd_ACT_MAC_RX_ON | HostCmd_ACT_MAC_TX_ON | HostCmd_ACT_MAC_ETHERNETII_ENABLE;

    memset(pmadapter, &priv->curr_bss_params, 0, sizeof(priv->curr_bss_params));
    priv->listen_interval = MLAN_DEFAULT_LISTEN_INTERVAL;
    wlan_11d_priv_init(priv);
    wlan_11h_priv_init(priv);

    priv->uap_bss_started = MFALSE;
    memset(pmadapter, &priv->uap_state_chan_cb, 0, sizeof(priv->uap_state_chan_cb));
    priv->num_drop_pkts = 0;

    priv->tx_bf_cap    = 0;
    priv->wmm_required = MTRUE;
    priv->wmm_enabled  = MFALSE;
    priv->wmm_qosinfo  = 0;
    priv->pmfcfg.mfpc  = 0;
    priv->pmfcfg.mfpr  = 0;

    for (i = 0; i < MAX_NUM_TID; i++)
        priv->addba_reject[i] = ADDBA_RSP_STATUS_ACCEPT;
    priv->max_amsdu = 0;

    priv->scan_block = MFALSE;

    if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA)
    {
        priv->port_ctrl_mode = MTRUE;
    }
    else
    {
        priv->port_ctrl_mode = MFALSE;
    }
    priv->port_open = MFALSE;
    LEAVE();
    return ret;
}

/**
 *  @brief This function initializes the adapter structure
 *  		and sets default values to the members of adapter.
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *
 *  @return		N/A
 */
t_void wlan_init_adapter(pmlan_adapter pmadapter)
{
    pmadapter->mp_rd_bitmap = 0;

    /*
     * wmsdk: fixme: Originally mp_wr_bitmap was set to 0. We have changed
     * it only for legacy reason. Please check it and restore on high
     * priority.
     */
    pmadapter->mp_wr_bitmap = 0;
#if defined(SD8801)
    pmadapter->curr_rd_port = 1;
    pmadapter->curr_wr_port = 1;
#elif defined(SD8977) || defined(SD8978) || defined(SD8987) || defined(SD8997) || defined(SD9097) || defined(SD9098)
    pmadapter->curr_rd_port = 0;
    pmadapter->curr_wr_port = 0;
#endif
    pmadapter->mp_data_port_mask = DATA_PORT_MASK;

    /* Scan type */
    pmadapter->scan_type = MLAN_SCAN_TYPE_ACTIVE;
    /* Scan mode */
    pmadapter->scan_mode = HostCmd_BSS_MODE_ANY;
    /* Scan time */
    pmadapter->specific_scan_time = MRVDRV_SPECIFIC_SCAN_CHAN_TIME;
    pmadapter->active_scan_time   = MRVDRV_ACTIVE_SCAN_CHAN_TIME;
    pmadapter->passive_scan_time  = MRVDRV_PASSIVE_SCAN_CHAN_TIME;

    pmadapter->num_in_scan_table = 0;

    pmadapter->ecsa_enable = MFALSE;

    /* fixme: enable this later when required */
    pmadapter->scan_probes = DEFAULT_PROBES;

    /* fixme: enable this later when required */
    pmadapter->multiple_dtim         = MRVDRV_DEFAULT_MULTIPLE_DTIM;
    pmadapter->local_listen_interval = 0; /* default value in firmware
                                             will be used */

    pmadapter->delay_to_ps      = DELAY_TO_PS_DEFAULT;
    pmadapter->enhanced_ps_mode = PS_MODE_AUTO;

    pmadapter->hw_dot_11n_dev_cap     = 0;
    pmadapter->hw_dev_mcs_support     = 0;
    pmadapter->usr_dot_11n_dev_cap_bg = 0;
    pmadapter->usr_dot_11n_dev_cap_a  = 0;
    pmadapter->usr_dev_mcs_support    = 0;
    pmadapter->chan_bandwidth         = 0;
    pmadapter->adhoc_11n_enabled      = MFALSE;

    pmadapter->hw_dot_11ac_dev_cap       = 0;
    pmadapter->hw_dot_11ac_mcs_support   = 0;
    pmadapter->usr_dot_11ac_opermode_bw  = 0;
    pmadapter->usr_dot_11ac_opermode_nss = 0;

    /* Initialize 802.11d */
    wlan_11d_init(pmadapter);
    wlan_11h_init(pmadapter);

    wlan_wmm_init(pmadapter);
    pmadapter->null_pkt_interval = 0;
    pmadapter->fw_bands          = 0;
    pmadapter->config_bands      = 0;
    pmadapter->adhoc_start_band  = 0;
    /* pmadapter->pscan_channels = MNULL; */
    pmadapter->fw_release_number = 0;
    pmadapter->fw_cap_info       = 0;
    memset(pmadapter, &pmadapter->region_channel, 0, sizeof(pmadapter->region_channel));
    pmadapter->region_code = MRVDRV_DEFAULT_REGION_CODE;
    memcpy(pmadapter, pmadapter->country_code, MRVDRV_DEFAULT_COUNTRY_CODE, COUNTRY_CODE_LEN);
    pmadapter->bcn_miss_time_out  = DEFAULT_BCN_MISS_TIMEOUT;
    pmadapter->adhoc_awake_period = 0;
    return;
}

/**
 *  @brief This function intializes the lock variables and
 *  the list heads.
 *
 *  @param pmadapter  A pointer to a mlan_adapter structure
 *
 *  @return           MLAN_STATUS_SUCCESS -- on success,
 *                    otherwise MLAN_STATUS_FAILURE
 *
 */
mlan_status wlan_init_lock_list(IN pmlan_adapter pmadapter)
{
    mlan_status ret    = MLAN_STATUS_SUCCESS;
    pmlan_private priv = MNULL;
    /* pmlan_callbacks pcb = &pmadapter->callbacks; */
    t_s32 i = 0;
    t_u32 j = 0;

    ENTER();
    for (i = 0; i < pmadapter->priv_num; i++)
    {
        if (pmadapter->priv[i])
        {
            priv = pmadapter->priv[i];
            for (j = 0; j < MAX_NUM_TID; ++j)
            {
                util_init_list_head((t_void *)pmadapter->pmoal_handle, &priv->wmm.tid_tbl_ptr[j].ra_list, MTRUE,
                                    priv->adapter->callbacks.moal_init_lock);
            }
            util_init_list_head((t_void *)pmadapter->pmoal_handle, &priv->tx_ba_stream_tbl_ptr, MTRUE,
                                pmadapter->callbacks.moal_init_lock);
            util_init_list_head((t_void *)pmadapter->pmoal_handle, &priv->rx_reorder_tbl_ptr, MTRUE,
                                pmadapter->callbacks.moal_init_lock);
            util_scalar_init((t_void *)pmadapter->pmoal_handle, &priv->wmm.tx_pkts_queued, 0,
                             priv->wmm.ra_list_spinlock, pmadapter->callbacks.moal_init_lock);
            util_scalar_init((t_void *)pmadapter->pmoal_handle, &priv->wmm.highest_queued_prio, HIGH_PRIO_TID,
                             priv->wmm.ra_list_spinlock, pmadapter->callbacks.moal_init_lock);
        }
    }

    /* error: */
    LEAVE();
    return ret;
}

/**
 *  @brief  This function initializes firmware
 *
 *  @param pmadapter		A pointer to mlan_adapter
 *
 *  @return		MLAN_STATUS_SUCCESS, MLAN_STATUS_PENDING or MLAN_STATUS_FAILURE
 */
mlan_status wlan_init_fw(IN pmlan_adapter pmadapter)
{
    mlan_status ret    = MLAN_STATUS_SUCCESS;
    pmlan_private priv = MNULL;
    t_u8 i             = 0;

    ENTER();

    /* Initialize adapter structure */
    wlan_init_adapter(pmadapter);

    for (i = 0; i < pmadapter->priv_num; i++)
    {
        if (pmadapter->priv[i])
        {
            priv = pmadapter->priv[i];

            /* Initialize private structure */
            if ((ret = wlan_init_priv(priv)))
            {
                ret = MLAN_STATUS_FAILURE;
                goto done;
            }
        }
    }
    /* Issue firmware initialize commands for first BSS, for other
       interfaces it will be called after getting the last init command
       response of previous interface */
done:
    LEAVE();
    return ret;
}
