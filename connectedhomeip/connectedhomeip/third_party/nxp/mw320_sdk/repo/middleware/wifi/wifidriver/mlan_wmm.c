/** @file mlan_wmm.c
 *
 *  @brief  This file provides functions for WMM
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
    10/24/2008: initial version
********************************************************/

#include <mlan_api.h>

/* Additional WMSDK header files */
#include <wmerrno.h>
#include <wm_os.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>
/********************************************************
    Local Variables
********************************************************/

/** Maximum value FW can accept for driver delay in packet transmission */
#define DRV_PKT_DELAY_TO_FW_MAX 512

/*
 * Upper and Lower threshold for packet queuing in the driver

 *    - When the number of packets queued reaches the upper limit,
 *      the driver will stop the net queue in the app/kernel space.

 *    - When the number of packets drops beneath the lower limit after
 *      having reached the upper limit, the driver will restart the net
 *      queue.
 */

/** Lower threshold for packet queuing in the driver.
 * When the number of packets drops beneath the lower limit after having
 * reached the upper limit, the driver will restart the net queue.
 */
#define WMM_QUEUED_PACKET_LOWER_LIMIT 180

/** Upper threshold for packet queuing in the driver.
 * When the number of packets queued reaches the upper limit, the driver
 * will stop the net queue in the app/kernel space.
 */
#define WMM_QUEUED_PACKET_UPPER_LIMIT 200

/** Offset for TOS field in the IP header */
#define IPTOS_OFFSET 5

/** WMM information IE */
static const t_u8 wmm_info_ie[] = {WMM_IE, 0x07, 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x00};

/**
 * This table will be used to store the tid values based on ACs.
 * It is initialized to default values per TID.
 */
t_u8 tos_to_tid[] = {
    /* TID DSCP_P2 DSCP_P1 DSCP_P0 WMM_AC */
    0x01, /* 0 1 0 AC_BK */
    0x02, /* 0 0 0 AC_BK */
    0x00, /* 0 0 1 AC_BE */
    0x03, /* 0 1 1 AC_BE */
    0x04, /* 1 0 0 AC_VI */
    0x05, /* 1 0 1 AC_VI */
    0x06, /* 1 1 0 AC_VO */
    0x07  /* 1 1 1 AC_VO */
};

/**
 * This table inverses the tos_to_tid operation to get a priority
 * which is in sequential order, and can be compared.
 * Use this to compare the priority of two different TIDs.
 */
t_u8 tos_to_tid_inv[] = {0x02, /* from tos_to_tid[2] = 0 */
                         0x00, /* from tos_to_tid[0] = 1 */
                         0x01, /* from tos_to_tid[1] = 2 */
                         0x03, 0x04, 0x05, 0x06, 0x07};

/**
 * This table will provide the tid value for given ac. This table does not
 * change and will be used to copy back the default values to tos_to_tid in
 * case of disconnect.
 */
const t_u8 ac_to_tid[4][2] = {{1, 2}, {0, 3}, {4, 5}, {6, 7}};

raListTbl *wlan_wmm_get_ralist_node(pmlan_private priv, t_u8 tid, t_u8 *ra_addr);

/********************************************************
    Local Functions
********************************************************/
#ifdef DEBUG_LEVEL2
/**
 *  @brief Debug print function to display the priority parameters for a WMM AC
 *
 *  @param pac_param	Pointer to the AC parameters to display
 *
 *  @return		N/A
 */
static void wlan_wmm_ac_debug_print(const IEEEtypes_WmmAcParameters_t *pac_param)
{
    const char *ac_str[] = {"BK", "BE", "VI", "VO"};

    ENTER();

    PRINTM(MINFO,
           "WMM AC_%s: ACI=%d, ACM=%d, Aifsn=%d, "
           "EcwMin=%d, EcwMax=%d, TxopLimit=%d\n",
           ac_str[wmm_aci_to_qidx_map[pac_param->aci_aifsn.aci]], pac_param->aci_aifsn.aci, pac_param->aci_aifsn.acm,
           pac_param->aci_aifsn.aifsn, pac_param->ecw.ecw_min, pac_param->ecw.ecw_max,
           wlan_le16_to_cpu(pac_param->tx_op_limit));

    LEAVE();
}

/** Print the WMM AC for debug purpose */
#define PRINTM_AC(pac_param) wlan_wmm_ac_debug_print(pac_param)
#else
/** Print the WMM AC for debug purpose */
#define PRINTM_AC(pac_param)
#endif

/**
 *  @brief  This function cleans Tx/Rx queues
 *
 *  @param priv		A pointer to mlan_private
 *
 *  @return		N/A
 */
t_void wlan_clean_txrx(pmlan_private priv)
{
    mlan_adapter *pmadapter = priv->adapter;
    t_u8 i                  = 0;

    ENTER();

    wlan_11n_cleanup_reorder_tbl(priv);

    pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle, priv->wmm.ra_list_spinlock);
    wlan_11n_deleteall_txbastream_tbl(priv);
    memcpy(pmadapter, tos_to_tid, ac_to_tid, sizeof(tos_to_tid));
    for (i = 0; i < MAX_NUM_TID; i++)
    {
        tos_to_tid_inv[tos_to_tid[i]] = (t_u8)i;
    }
    priv->num_drop_pkts = 0;
    pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle, priv->wmm.ra_list_spinlock);

    LEAVE();
}

/**
 *  @brief Set the WMM queue priorities to their default values
 *
 *  @param priv     Pointer to the mlan_private driver data struct
 *
 *  @return         N/A
 */
void wlan_wmm_default_queue_priorities(pmlan_private priv)
{
    ENTER();

    /* Default queue priorities: VO->VI->BE->BK */
    priv->wmm.queue_priority[0] = WMM_AC_VO;
    priv->wmm.queue_priority[1] = WMM_AC_VI;
    priv->wmm.queue_priority[2] = WMM_AC_BE;
    priv->wmm.queue_priority[3] = WMM_AC_BK;

    LEAVE();
}

/**
 *  @brief Initialize the WMM state information and the WMM data path queues.
 *
 *  @param pmadapter  Pointer to the mlan_adapter data structure
 *
 *  @return         N/A
 */
t_void wlan_wmm_init(pmlan_adapter pmadapter)
{
    int i, j;
    pmlan_private priv;

    ENTER();

    for (j = 0; j < pmadapter->priv_num; ++j)
    {
        if ((priv = pmadapter->priv[j]))
        {
            for (i = 0; i < MAX_NUM_TID; ++i)
            {
                priv->aggr_prio_tbl[i].amsdu    = BA_STREAM_NOT_ALLOWED;
                priv->aggr_prio_tbl[i].ampdu_ap = priv->aggr_prio_tbl[i].ampdu_user = tos_to_tid_inv[i];
                priv->wmm.pkts_queued[i]                                            = 0;
                priv->wmm.tid_tbl_ptr[i].ra_list_curr                               = MNULL;
            }

            priv->aggr_prio_tbl[6].ampdu_ap = priv->aggr_prio_tbl[6].ampdu_user = BA_STREAM_NOT_ALLOWED;

            priv->aggr_prio_tbl[7].ampdu_ap = priv->aggr_prio_tbl[7].ampdu_user = BA_STREAM_NOT_ALLOWED;

            priv->add_ba_param.timeout = MLAN_DEFAULT_BLOCK_ACK_TIMEOUT;
            if (priv->bss_type == MLAN_BSS_TYPE_STA)
            {
                priv->add_ba_param.tx_win_size = MLAN_STA_AMPDU_DEF_TXWINSIZE;
                priv->add_ba_param.rx_win_size = MLAN_STA_AMPDU_DEF_RXWINSIZE;
            }
            if (priv->bss_type == MLAN_BSS_TYPE_UAP)
            {
                priv->add_ba_param.tx_win_size = MLAN_UAP_AMPDU_DEF_TXWINSIZE;
                priv->add_ba_param.rx_win_size = MLAN_UAP_AMPDU_DEF_RXWINSIZE;
            }
            priv->add_ba_param.tx_amsdu = MTRUE;
            priv->add_ba_param.rx_amsdu = MTRUE;
            memset(priv->adapter, priv->rx_seq, 0xff, sizeof(priv->rx_seq));
            wlan_wmm_default_queue_priorities(priv);
        }
    }

    LEAVE();
}

/**
 *  @brief Call back from the command module to allow insertion of a WMM TLV
 *
 *  If the BSS we are associating to supports WMM, add the required WMM
 *    Information IE to the association request command buffer in the form
 *    of a NXP extended IEEE IE.
 *
 *  @param priv        Pointer to the mlan_private driver data struct
 *  @param ppAssocBuf  Output parameter: Pointer to the TLV output buffer,
 *                     modified on return to point after the appended WMM TLV
 *  @param pWmmIE      Pointer to the WMM IE for the BSS we are joining
 *  @param pHTCap      Pointer to the HT IE for the BSS we are joining
 *
 *  @return Length of data appended to the association tlv buffer
 */
t_u32 wlan_wmm_process_association_req(pmlan_private priv,
                                       t_u8 **ppAssocBuf,
                                       IEEEtypes_WmmParameter_t *pWmmIE,
                                       IEEEtypes_HTCap_t *pHTCap)
{
    MrvlIEtypes_WmmParamSet_t *pwmm_tlv;
    t_u32 ret_len = 0;

    ENTER();

    /* Null checks */
    if (!ppAssocBuf)
    {
        LEAVE();
        return 0;
    }
    if (!(*ppAssocBuf))
    {
        LEAVE();
        return 0;
    }

    if (!pWmmIE)
    {
        LEAVE();
        return 0;
    }

    PRINTM(MINFO, "WMM: process assoc req: bss->wmmIe=0x%x\n", pWmmIE->vend_hdr.element_id);

    if ((priv->wmm_required || (pHTCap && (pHTCap->ieee_hdr.element_id == HT_CAPABILITY) &&
                                (priv->config_bands & BAND_GN || priv->config_bands & BAND_AN))) &&
        pWmmIE->vend_hdr.element_id == WMM_IE)
    {
        pwmm_tlv              = (MrvlIEtypes_WmmParamSet_t *)*ppAssocBuf;
        pwmm_tlv->header.type = (t_u16)wmm_info_ie[0];
        pwmm_tlv->header.type = wlan_cpu_to_le16(pwmm_tlv->header.type);
        pwmm_tlv->header.len  = (t_u16)wmm_info_ie[1];
        memcpy(priv->adapter, pwmm_tlv->wmm_ie, &wmm_info_ie[2], pwmm_tlv->header.len);
        if (pWmmIE->qos_info.qos_uapsd)
            memcpy(priv->adapter, (t_u8 *)(pwmm_tlv->wmm_ie + pwmm_tlv->header.len - sizeof(priv->wmm_qosinfo)),
                   &priv->wmm_qosinfo, sizeof(priv->wmm_qosinfo));

        ret_len              = sizeof(pwmm_tlv->header) + pwmm_tlv->header.len;
        pwmm_tlv->header.len = wlan_cpu_to_le16(pwmm_tlv->header.len);

        HEXDUMP("ASSOC_CMD: WMM IE", (t_u8 *)pwmm_tlv, ret_len);
        *ppAssocBuf += ret_len;
    }

    LEAVE();
    return ret_len;
}
