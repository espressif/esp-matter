/** @file mlan_sta_event.c
 *
 *  @brief  This file provides Function for STA event handling
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

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>

/**
 *  @brief This function handles disconnect event, reports disconnect
 *  		to upper layer, cleans tx/rx packets,
 *  		resets link state etc.
 *
 *  @param priv            A pointer to mlan_private structure
 *  @param drv_disconnect  Flag indicating the driver should disconnect
 *                         and flush pending packets.
 *
 *  @return        N/A
 */
t_void wlan_reset_connect_state(pmlan_private priv, t_u8 drv_disconnect)
{
    ENTER();

    PRINTM(MINFO, "Handles disconnect event.\n");

    if (drv_disconnect)
    {
        priv->media_connected = MFALSE;
    }

    if (priv->port_ctrl_mode == MTRUE)
    {
        /* Close the port on Disconnect */
        PRINTM(MINFO, "DISC: port_status = CLOSED\n");
        priv->port_open = MFALSE;
    }
    priv->scan_block = MFALSE;

    /* Reset SNR/NF/RSSI values */
    priv->data_rssi_last = 0;
    priv->data_nf_last   = 0;
    priv->data_rssi_avg  = 0;
    priv->data_nf_avg    = 0;
    priv->bcn_rssi_last  = 0;
    priv->bcn_nf_last    = 0;
    priv->bcn_rssi_avg   = 0;
    priv->bcn_nf_avg     = 0;
    priv->rxpd_rate      = 0;
#ifdef SD8801
    priv->rxpd_htinfo = 0;
#else
    priv->rxpd_rate_info = 0;
#endif
    priv->max_amsdu = 0;

    /* Enable auto data rate */
    priv->is_data_rate_auto = MTRUE;
    priv->data_rate         = 0;

    if (priv->bss_mode == MLAN_BSS_MODE_IBSS)
    {
        priv->adhoc_state                        = ADHOC_IDLE;
        priv->adhoc_is_link_sensed               = MFALSE;
        priv->intf_state_11h.adhoc_auto_sel_chan = MTRUE;
    }

    if (drv_disconnect)
    {
        /* Free Tx and Rx packets, report disconnect to upper layer */
        wlan_clean_txrx(priv);

        /* Need to erase the current SSID and BSSID info */
        memset(priv->adapter, &priv->curr_bss_params, 0x00, sizeof(priv->curr_bss_params));
    }

    LEAVE();
}

/**
 *  @brief This function handles link lost, deauth and
 *  		disassoc events.
 *
 *  @param priv    A pointer to mlan_private structure
 *  @return        N/A
 */
t_void wlan_handle_disconnect_event(pmlan_private pmpriv)
{
    ENTER();

    wlan_reset_connect_state(pmpriv, MTRUE);

    LEAVE();
}
