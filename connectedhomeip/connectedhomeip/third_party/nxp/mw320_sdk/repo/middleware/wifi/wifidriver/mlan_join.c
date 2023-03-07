/** @file mlan_join.c
 *
 *  @brief  This file provides Functions implementing wlan infrastructure
 *  and adhoc join routines IOCTL handlers as well as command preparation
 *  and response routines for sending adhoc start, adhoc join, and
 *  association commands to the firmware.
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

/******************************************************
Change log:
    10/30/2008: initial version
******************************************************/
#include <mlan_api.h>

/* Additional WMSDK header files */
#include <wmerrno.h>
#include <wm_os.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>
/********************************************************
                Local Constants
********************************************************/

/********************************************************
                Local Variables
********************************************************/

/********************************************************
                Global Variables
********************************************************/

/********************************************************
                Local Functions
********************************************************/

/**
 *  @brief This function finds out the common rates between rate1 and rate2.
 *
 *  It will fill common rates in rate1 as output if found.
 *
 *  NOTE: Setting the MSB of the basic rates needs to be taken
 *   care of, either before or after calling this function
 *
 *  @param pmpriv      A pointer to mlan_private structure
 *  @param rate1       the buffer which keeps input and output
 *  @param rate1_size  the size of rate1 buffer
 *  @param rate2       the buffer which keeps rate2
 *  @param rate2_size  the size of rate2 buffer.
 *
 *  @return            MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status wlan_get_common_rates(
    IN mlan_private *pmpriv, IN t_u8 *rate1, IN t_u32 rate1_size, IN t_u8 *rate2, IN t_u32 rate2_size)
{
    mlan_status ret     = MLAN_STATUS_SUCCESS;
    mlan_callbacks *pcb = (mlan_callbacks *)&pmpriv->adapter->callbacks;
    t_u8 *ptr           = rate1;
    t_u8 *tmp           = MNULL;
    t_u32 i, j;

    ENTER();

    ret = pcb->moal_malloc(pmpriv->adapter->pmoal_handle, rate1_size, MLAN_MEM_DEF, &tmp);
    if (ret != MLAN_STATUS_SUCCESS || !tmp)
    {
        PRINTM(MERROR, "Failed to allocate buffer\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    memcpy(pmpriv->adapter, tmp, rate1, rate1_size);
    memset(pmpriv->adapter, rate1, 0, rate1_size);

    for (i = 0; rate2[i] && i < rate2_size; i++)
    {
        for (j = 0; tmp[j] && j < rate1_size; j++)
        {
            /* Check common rate, excluding the bit for basic rate */
            if ((rate2[i] & 0x7F) == (tmp[j] & 0x7F))
            {
                *rate1++ = tmp[j];
                break;
            }
        }
    }

    HEXDUMP("rate1 (AP) Rates", tmp, rate1_size);
    HEXDUMP("rate2 (Card) Rates", rate2, rate2_size);
    HEXDUMP("Common Rates", ptr, rate1 - ptr);
    PRINTM(MINFO, "Tx DataRate is set to 0x%X\n", pmpriv->data_rate);

    if (!pmpriv->is_data_rate_auto)
    {
        while (*ptr)
        {
            if ((*ptr & 0x7f) == pmpriv->data_rate)
            {
                ret = MLAN_STATUS_SUCCESS;
                goto done;
            }
            ptr++;
        }
        PRINTM(MMSG,
               "Previously set fixed data rate %#x is not "
               "compatible with the network\n",
               pmpriv->data_rate);

        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    ret = MLAN_STATUS_SUCCESS;
done:
    if (tmp)
        pcb->moal_mfree(pmpriv->adapter->pmoal_handle, tmp);

    LEAVE();
    return ret;
}

/**
 *  @brief Create the intersection of the rates supported by a target BSS and
 *         our pmadapter settings for use in an assoc/join command.
 *
 *  @param pmpriv           A pointer to mlan_private structure
 *  @param pbss_desc        BSS Descriptor whose rates are used in the setup
 *  @param pout_rates       Output: Octet array of rates common between the BSS
 *                          and the pmadapter supported rates settings
 *  @param pout_rates_size  Output: Number of rates/octets set in pout_rates
 *
 *  @return                 MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status wlan_setup_rates_from_bssdesc(IN mlan_private *pmpriv,
                                                 IN BSSDescriptor_t *pbss_desc,
                                                 OUT t_u8 *pout_rates,
                                                 OUT t_u32 *pout_rates_size)
{
    t_u8 card_rates[WLAN_SUPPORTED_RATES];
    t_u32 card_rates_size = 0;
    ENTER();
    /* Copy AP supported rates */
    memcpy(pmpriv->adapter, pout_rates, pbss_desc->supported_rates, WLAN_SUPPORTED_RATES);
    /* Get the STA supported rates */
    card_rates_size = wlan_get_active_data_rates(pmpriv, pmpriv->bss_mode, pmpriv->config_bands, card_rates);
    /* Get the common rates between AP and STA supported rates */
    if (wlan_get_common_rates(pmpriv, pout_rates, WLAN_SUPPORTED_RATES, card_rates, card_rates_size))
    {
        *pout_rates_size = 0;
        PRINTM(MERROR, "wlan_get_common_rates failed\n");
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    *pout_rates_size = MIN(wlan_strlen((char *)pout_rates), WLAN_SUPPORTED_RATES);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function updates RSN IE in the association request.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *
 *  @param ptlv_rsn_ie       A pointer to rsn_ie TLV
 */
mlan_status wlan_update_rsn_ie(mlan_private *pmpriv, MrvlIEtypes_RsnParamSet_t *ptlv_rsn_ie)
{
    t_u16 *prsn_cap;
    t_u16 *ptr;
    t_u16 *akm_suite_count_ptr;
    t_u16 pmf_mask = 0x00;
    t_u8 *temp;
    t_u16 pairwise_cipher_count = 0;
    t_u16 akm_suite_count       = 0;
    t_u16 temp_akm_suite_count  = 0;
    int found                   = 0;
    t_u8 sha_256_oui[4]         = {0x00, 0x0f, 0xac, 0x06};

    int ap_mfpc = 0, ap_mfpr = 0;
    mlan_status ret = MLAN_STATUS_SUCCESS;

    pmf_mask = (((pmpriv->pmfcfg.mfpc << MFPC_BIT) | (pmpriv->pmfcfg.mfpr << MFPR_BIT)) | (~PMF_MASK));
    /* prsn_cap = prsn_ie->rsn_ie + 2 bytes version + 4 bytes group_cipher_suite +
     *            2 bytes pairwise_cipher_count + pairwise_cipher_count * PAIRWISE_CIPHER_SUITE_LEN +
     *            2 bytes akm_suite_count + akm_suite_count * AKM_SUITE_LEN
     */
    ptr                   = (t_u16 *)(ptlv_rsn_ie->rsn_ie + sizeof(t_u16) + 4 * sizeof(t_u8));
    pairwise_cipher_count = wlan_le16_to_cpu(*ptr);
    ptr                   = (t_u16 *)(ptlv_rsn_ie->rsn_ie + sizeof(t_u16) + 4 * sizeof(t_u8) + sizeof(t_u16) +
                    pairwise_cipher_count * PAIRWISE_CIPHER_SUITE_LEN);
    temp_akm_suite_count  = wlan_le16_to_cpu(*ptr);
    akm_suite_count       = wlan_le16_to_cpu(*ptr);
    /* Save pointer to akm_suite_count in RSN IE to update it later */
    akm_suite_count_ptr = ptr;
    temp                = ptlv_rsn_ie->rsn_ie + sizeof(t_u16) + 4 * sizeof(t_u8) + sizeof(t_u16) +
           pairwise_cipher_count * PAIRWISE_CIPHER_SUITE_LEN + sizeof(t_u16);
    /* ptr now points to the 1st AKM suite */
    if (temp_akm_suite_count > 1)
    {
        while (temp_akm_suite_count)
        {
            if (!memcmp(pmadapter, temp, sha_256_oui, AKM_SUITE_LEN))
            {
                found = 1;
                break;
            }
            temp += AKM_SUITE_LEN;
            temp_akm_suite_count--;
        }
        if (found)
        {
            /* Copy SHA256 as AKM suite */
            memcpy(pmadapter,
                   ptlv_rsn_ie->rsn_ie + (sizeof(t_u16) + 4 * sizeof(t_u8) + sizeof(t_u16) +
                                          pairwise_cipher_count * PAIRWISE_CIPHER_SUITE_LEN + sizeof(t_u16)),
                   sha_256_oui, AKM_SUITE_LEN);
            /* Shift remaining bytes of RSN IE after this */
            memmove(pmadapter,
                    ptlv_rsn_ie->rsn_ie +
                        (sizeof(t_u16) + 4 * sizeof(t_u8) + sizeof(t_u16) +
                         pairwise_cipher_count * PAIRWISE_CIPHER_SUITE_LEN + sizeof(t_u16) + AKM_SUITE_LEN),
                    ptlv_rsn_ie->rsn_ie + (sizeof(t_u16) + 4 * sizeof(t_u8) + sizeof(t_u16) +
                                           pairwise_cipher_count * PAIRWISE_CIPHER_SUITE_LEN + sizeof(t_u16) +
                                           akm_suite_count * AKM_SUITE_LEN),
                    ptlv_rsn_ie->header.len - (sizeof(t_u16) + 4 * sizeof(t_u8) + sizeof(t_u16) +
                                               pairwise_cipher_count * PAIRWISE_CIPHER_SUITE_LEN + sizeof(t_u16) +
                                               akm_suite_count * AKM_SUITE_LEN));
            ptlv_rsn_ie->header.len = ptlv_rsn_ie->header.len - (akm_suite_count - 1) * AKM_SUITE_LEN;
            /* Update akm suite count */
            akm_suite_count      = 1;
            *akm_suite_count_ptr = akm_suite_count;
        }
    }
    ptr =
        (t_u16 *)(ptlv_rsn_ie->rsn_ie + sizeof(t_u16) + 4 * sizeof(t_u8) + sizeof(t_u16) +
                  pairwise_cipher_count * PAIRWISE_CIPHER_SUITE_LEN + sizeof(t_u16) + akm_suite_count * AKM_SUITE_LEN);
    prsn_cap = ptr;

    ap_mfpc = ((*prsn_cap & (0x1 << MFPC_BIT)) == (0x1 << MFPC_BIT));
    ap_mfpr = ((*prsn_cap & (0x1 << MFPR_BIT)) == (0x1 << MFPR_BIT));

    if ((!ap_mfpc && !ap_mfpr && pmpriv->pmfcfg.mfpr) || ((!ap_mfpc) && ap_mfpr) ||
        (ap_mfpc && ap_mfpr && (!pmpriv->pmfcfg.mfpc)))
    {
        PRINTF("Mismatch in PMF config of STA and AP, can't associate to AP\r\n");
        return MLAN_STATUS_FAILURE;
    }
    if ((pmpriv->pmfcfg.mfpr && pmpriv->pmfcfg.mfpc) || pmpriv->pmfcfg.mfpc)
    {
        *prsn_cap |= PMF_MASK;
        *prsn_cap &= pmf_mask;
    }

    return ret;
}

/********************************************************
                Global Functions
********************************************************/
/**
 *  @brief This function prepares command of association.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer cast of BSSDescriptor_t from the
 *                        scan table to assoc
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_cmd_802_11_associate(IN mlan_private *pmpriv, IN HostCmd_DS_COMMAND *cmd, IN t_void *pdata_buf)
{
    mlan_status ret                    = MLAN_STATUS_SUCCESS;
    mlan_adapter *pmadapter            = pmpriv->adapter;
    HostCmd_DS_802_11_ASSOCIATE *passo = &cmd->params.associate;
    BSSDescriptor_t *pbss_desc;
    MrvlIEtypes_SsIdParamSet_t *pssid_tlv;
    MrvlIEtypes_PhyParamSet_t *pphy_tlv;
    MrvlIEtypes_SsParamSet_t *pss_tlv;
    MrvlIEtypes_RatesParamSet_t *prates_tlv;
    MrvlIEtypes_AuthType_t *pauth_tlv;
    MrvlIEtypes_RsnParamSet_t *prsn_ie_tlv;
    MrvlIEtypes_ChanListParamSet_t *pchan_tlv;
    WLAN_802_11_RATES rates;
    t_u32 rates_size;
    t_u16 tmp_cap;
    t_u8 *pos;

    ENTER();

    pbss_desc = (BSSDescriptor_t *)pdata_buf;
    pos       = (t_u8 *)passo;

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_ASSOCIATE);

    /* Save so we know which BSS Desc to use in the response handler */
    pmpriv->pattempted_bss_desc = pbss_desc;

    memcpy(pmadapter, passo->peer_sta_addr, pbss_desc->mac_address, sizeof(passo->peer_sta_addr));
    pos += sizeof(passo->peer_sta_addr);

    /* fixme: Look at this value carefully later. The listen interval is given to AP during
     * this assoc. The listen_interval set later during IEEE PS should not (?) exceed this
     */
    /* Set the listen interval */
    passo->listen_interval = wlan_cpu_to_le16(pmpriv->listen_interval);
    /* Set the beacon period */
    passo->beacon_period = wlan_cpu_to_le16(pbss_desc->beacon_period);

    pos += sizeof(passo->cap_info);
    pos += sizeof(passo->listen_interval);
    pos += sizeof(passo->beacon_period);
    pos += sizeof(passo->dtim_period);

    pssid_tlv              = (MrvlIEtypes_SsIdParamSet_t *)pos;
    pssid_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_SSID);
    pssid_tlv->header.len  = (t_u16)pbss_desc->ssid.ssid_len;
    memcpy(pmadapter, pssid_tlv->ssid, pbss_desc->ssid.ssid, pssid_tlv->header.len);
    pos += sizeof(pssid_tlv->header) + pssid_tlv->header.len;
    pssid_tlv->header.len = wlan_cpu_to_le16(pssid_tlv->header.len);

    pphy_tlv              = (MrvlIEtypes_PhyParamSet_t *)pos;
    pphy_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_PHY_DS);
    pphy_tlv->header.len  = sizeof(pphy_tlv->fh_ds.ds_param_set);
    memcpy(pmadapter, &pphy_tlv->fh_ds.ds_param_set, &pbss_desc->phy_param_set.ds_param_set.current_chan,
           sizeof(pphy_tlv->fh_ds.ds_param_set));
    pos += sizeof(pphy_tlv->header) + pphy_tlv->header.len;
    pphy_tlv->header.len = wlan_cpu_to_le16(pphy_tlv->header.len);

    pss_tlv              = (MrvlIEtypes_SsParamSet_t *)pos;
    pss_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_CF);
    pss_tlv->header.len  = sizeof(pss_tlv->cf_ibss.cf_param_set);
    pos += sizeof(pss_tlv->header) + pss_tlv->header.len;
    pss_tlv->header.len = wlan_cpu_to_le16(pss_tlv->header.len);

    /* Get the common rates supported between the driver and the BSS Desc */
    if (wlan_setup_rates_from_bssdesc(pmpriv, pbss_desc, rates, &rates_size))
    {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /* Save the data rates into Current BSS state structure */
    pmpriv->curr_bss_params.num_of_rates = rates_size;
    memcpy(pmadapter, &pmpriv->curr_bss_params.data_rates, rates, rates_size);

    /* Setup the Rates TLV in the association command */
    prates_tlv              = (MrvlIEtypes_RatesParamSet_t *)pos;
    prates_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_RATES);
    prates_tlv->header.len  = wlan_cpu_to_le16((t_u16)rates_size);
    memcpy(pmadapter, prates_tlv->rates, rates, rates_size);
    pos += sizeof(prates_tlv->header) + rates_size;
    PRINTM(MINFO, "ASSOC_CMD: Rates size = %d\n", rates_size);

    /* Add the Authentication type to be used for Auth frames if needed */
    if ((pmpriv->sec_info.authentication_mode != MLAN_AUTH_MODE_AUTO))
    {
        pauth_tlv              = (MrvlIEtypes_AuthType_t *)pos;
        pauth_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_AUTH_TYPE);
        pauth_tlv->header.len  = sizeof(pauth_tlv->auth_type);
        if (pmpriv->sec_info.authentication_mode == MLAN_AUTH_MODE_SAE)
            pauth_tlv->auth_type = wlan_cpu_to_le16((t_u16)AssocAgentAuth_Wpa3Sae);
        else if (pmpriv->sec_info.wep_status == Wlan802_11WEPEnabled)
            pauth_tlv->auth_type = wlan_cpu_to_le16((t_u16)pmpriv->sec_info.authentication_mode);
        else
            pauth_tlv->auth_type = wlan_cpu_to_le16(MLAN_AUTH_MODE_OPEN);
        pos += sizeof(pauth_tlv->header) + pauth_tlv->header.len;
        pauth_tlv->header.len = wlan_cpu_to_le16(pauth_tlv->header.len);
    }

    if (IS_SUPPORT_MULTI_BANDS(pmadapter) && (pbss_desc->bss_band & pmpriv->config_bands) &&
        !(ISSUPP_11NENABLED(pmadapter->fw_cap_info) && (!pbss_desc->disable_11n) &&
          (pmpriv->config_bands & BAND_GN || pmpriv->config_bands & BAND_AN) && (pbss_desc->pht_cap)))
    {
        /* Append a channel TLV for the channel the attempted AP was found on */
        pchan_tlv              = (MrvlIEtypes_ChanListParamSet_t *)pos;
        pchan_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_CHANLIST);
        pchan_tlv->header.len  = wlan_cpu_to_le16(sizeof(ChanScanParamSet_t));

        memset(pmadapter, pchan_tlv->chan_scan_param, 0x00, sizeof(ChanScanParamSet_t));
        pchan_tlv->chan_scan_param[0].chan_number = (pbss_desc->phy_param_set.ds_param_set.current_chan);
        PRINTM(MINFO, "Assoc: TLV Chan = %d\n", pchan_tlv->chan_scan_param[0].chan_number);

        pchan_tlv->chan_scan_param[0].radio_type = wlan_band_to_radio_type((t_u8)pbss_desc->bss_band);

        PRINTM(MINFO, "Assoc: TLV Band = %d\n", pchan_tlv->chan_scan_param[0].radio_type);
        pos += sizeof(pchan_tlv->header) + sizeof(ChanScanParamSet_t);
    }

    /* fixme: The above 'if' is used instead of below 'if' for now since
       WPS module is mlan integrated yet. Fix after it is done.
    */
    /* if (!pmpriv->wps.session_enable) { */
    if ((pmpriv->sec_info.wpa_enabled || pmpriv->sec_info.wpa2_enabled))
    {
        prsn_ie_tlv              = (MrvlIEtypes_RsnParamSet_t *)pos;
        prsn_ie_tlv->header.type = (t_u16)pmpriv->wpa_ie[0]; /* WPA_IE
                                                                or
                                                                RSN_IE
                                                              */
        prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
        prsn_ie_tlv->header.type = wlan_cpu_to_le16(prsn_ie_tlv->header.type);
        prsn_ie_tlv->header.len  = (t_u16)pmpriv->wpa_ie[1];
        prsn_ie_tlv->header.len  = prsn_ie_tlv->header.len & 0x00FF;
        if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie) - 2))
        {
            memcpy(pmadapter, prsn_ie_tlv->rsn_ie, &pmpriv->wpa_ie[2], prsn_ie_tlv->header.len);
            if (pmpriv->sec_info.wpa2_enabled)
            {
                ret = wlan_update_rsn_ie(pmpriv, prsn_ie_tlv);
                if ((mlan_status)ret != MLAN_STATUS_SUCCESS)
                {
                    goto done;
                }
            }
        }
        else
        {
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
        HEXDUMP("ASSOC_CMD: RSN IE", (t_u8 *)prsn_ie_tlv, sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
        pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
        prsn_ie_tlv->header.len = wlan_cpu_to_le16(prsn_ie_tlv->header.len);
    }
    else if (pmpriv->sec_info.ewpa_enabled)
    {
        prsn_ie_tlv = (MrvlIEtypes_RsnParamSet_t *)pos;
        if (pbss_desc->pwpa_ie)
        {
            prsn_ie_tlv->header.type = (t_u16)(*(pbss_desc->pwpa_ie)).vend_hdr.element_id;
            prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
            prsn_ie_tlv->header.type = wlan_cpu_to_le16(prsn_ie_tlv->header.type);
            prsn_ie_tlv->header.len  = (t_u16)(*(pbss_desc->pwpa_ie)).vend_hdr.len;
            prsn_ie_tlv->header.len  = prsn_ie_tlv->header.len & 0x00FF;
            if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie)))
            {
                memcpy(pmadapter, prsn_ie_tlv->rsn_ie, &((*(pbss_desc->pwpa_ie)).vend_hdr.oui[0]),
                       prsn_ie_tlv->header.len);
            }
            else
            {
                ret = MLAN_STATUS_FAILURE;
                goto done;
            }

            HEXDUMP("ASSOC_CMD: RSN IE", (t_u8 *)prsn_ie_tlv, sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
            pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
            prsn_ie_tlv->header.len = wlan_cpu_to_le16(prsn_ie_tlv->header.len);
        }
        if (pbss_desc->prsn_ie)
        {
            prsn_ie_tlv              = (MrvlIEtypes_RsnParamSet_t *)pos;
            prsn_ie_tlv->header.type = (t_u16)(*(pbss_desc->prsn_ie)).ieee_hdr.element_id;
            prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
            prsn_ie_tlv->header.type = wlan_cpu_to_le16(prsn_ie_tlv->header.type);
            prsn_ie_tlv->header.len  = (t_u16)(*(pbss_desc->prsn_ie)).ieee_hdr.len;
            prsn_ie_tlv->header.len  = prsn_ie_tlv->header.len & 0x00FF;
            if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie)))
            {
                memcpy(pmadapter, prsn_ie_tlv->rsn_ie, &((*(pbss_desc->prsn_ie)).data[0]), prsn_ie_tlv->header.len);
            }
            else
            {
                ret = MLAN_STATUS_FAILURE;
                goto done;
            }

            HEXDUMP("ASSOC_CMD: RSN IE", (t_u8 *)prsn_ie_tlv, sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
            pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
            prsn_ie_tlv->header.len = wlan_cpu_to_le16(prsn_ie_tlv->header.len);
        }
    }

    if (ISSUPP_11NENABLED(pmadapter->fw_cap_info) && (!pbss_desc->disable_11n) &&
        (pmpriv->config_bands & BAND_GN || pmpriv->config_bands & BAND_AN) && wmsdk_is_11N_enabled() &&
        (!pmpriv->sec_info.is_wpa_tkip))
    {
        wlan_cmd_append_11n_tlv(pmpriv, pbss_desc, &pos);
    }
    else if (pmpriv->hotspot_cfg & HOTSPOT_ENABLED)
        wlan_add_ext_capa_info_ie(pmpriv, &pos);

    if (ISSUPP_11ACENABLED(pmadapter->fw_cap_info) && (!pbss_desc->disable_11n) &&
        wlan_11ac_bandconfig_allowed(pmpriv, pbss_desc->bss_band))
        wlan_cmd_append_11ac_tlv(pmpriv, pbss_desc, &pos);

    wlan_wmm_process_association_req(pmpriv, &pos, &pbss_desc->wmm_ie, pbss_desc->pht_cap);

    /* fixme: Currently not required */

    cmd->size = wlan_cpu_to_le16((t_u16)(pos - (t_u8 *)passo) + S_DS_GEN);

    /* Set the Capability info at last */
    memcpy(pmadapter, &tmp_cap, &pbss_desc->cap_info, sizeof(passo->cap_info));

    if (pmpriv->config_bands == BAND_B)
    {
        SHORT_SLOT_TIME_DISABLED(tmp_cap);
    }

    tmp_cap &= CAPINFO_MASK;
    PRINTM(MINFO, "ASSOC_CMD: tmp_cap=%4X CAPINFO_MASK=%4lX\n", tmp_cap, CAPINFO_MASK);
    tmp_cap = wlan_cpu_to_le16(tmp_cap);
    memcpy(pmadapter, &passo->cap_info, &tmp_cap, sizeof(passo->cap_info));

done:
    LEAVE();
    return ret;
}

/**
 *  @brief Association firmware command response handler
 *
 *   The response buffer for the association command has the following
 *      memory layout.
 *
 *   For cases where an association response was not received (indicated
 *      by the CapInfo and AId field):
 *
 *     .------------------------------------------------------------.
 *     |  Header(4 * sizeof(t_u16)):  Standard command response hdr |
 *     .------------------------------------------------------------.
 *     |  cap_info/Error Return(t_u16):                             |
 *     |           0xFFFF(-1): Internal error for association       |
 *     |           0xFFFE(-2): Authentication unhandled message     |
 *     |           0xFFFD(-3): Authentication refused               |
 *     |           0xFFFC(-4): Timeout waiting for AP response      |
 *     |           0xFFFB(-5): Internal error for authentication    |
 *     .------------------------------------------------------------.
 *     |  status_code(t_u16):                                       |
 *     |        If cap_info is -1:                                  |
 *     |           An internal firmware failure prevented the       |
 *     |           command from being processed. The status code    |
 *     |           is 6 if associate response parameter invlaid,    |
 *     |           1 otherwise.                                     |
 *     |                                                            |
 *     |        If cap_info is -2:                                  |
 *     |           An authentication frame was received but was     |
 *     |           not handled by the firmware. IEEE Status code    |
 *     |           for the failure is returned.                     |
 *     |                                                            |
 *     |        If cap_info is -3:                                  |
 *     |           An authentication frame was received and the     |
 *     |           status_code is the IEEE Status reported in the   |
 *     |           response.                                        |
 *     |                                                            |
 *     |        If cap_info is -4:                                  |
 *     |           (1) Association response timeout                 |
 *     |           (2) Authentication response timeout              |
 *     |                                                            |
 *     |        If cap_info is -5:                                  |
 *     |           An internal firmware failure prevented the       |
 *     |           command from being processed. The status code    |
 *     |           is 6 if authentication parameter invlaid,        |
 *     |           1 otherwise.                                     |
 *     .------------------------------------------------------------.
 *     |  a_id(t_u16): 0xFFFF                                       |
 *     .------------------------------------------------------------.
 *
 *
 *   For cases where an association response was received, the IEEE
 *     standard association response frame is returned:
 *
 *     .------------------------------------------------------------.
 *     |  Header(4 * sizeof(t_u16)):  Standard command response hdr |
 *     .------------------------------------------------------------.
 *     |  cap_info(t_u16): IEEE Capability                          |
 *     .------------------------------------------------------------.
 *     |  status_code(t_u16): IEEE Status Code                      |
 *     .------------------------------------------------------------.
 *     |  a_id(t_u16): IEEE Association ID                          |
 *     .------------------------------------------------------------.
 *     |  IEEE IEs(variable): Any received IEs comprising the       |
 *     |                      remaining portion of a received       |
 *     |                      association response frame.           |
 *     .------------------------------------------------------------.
 *
 *  For simplistic handling, the status_code field can be used to determine
 *    an association success (0) or failure (non-zero).
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_ret_802_11_associate(IN mlan_private *pmpriv, IN HostCmd_DS_COMMAND *resp, IN t_void *pioctl_buf)
{
    mlan_status ret            = MLAN_STATUS_SUCCESS;
    mlan_ioctl_req *pioctl_req = (mlan_ioctl_req *)pioctl_buf;
    IEEEtypes_AssocRsp_t *passoc_rsp;
    BSSDescriptor_t *pbss_desc;
    /* t_u8 enable_data = MTRUE; */
    /* t_u8 event_buf[100]; */
    /* mlan_event *pevent = (mlan_event *) event_buf; */
    t_u8 cur_mac[MLAN_MAC_ADDR_LENGTH];
    t_u8 media_connected = pmpriv->media_connected;
    /* mlan_adapter *pmadapter = pmpriv->adapter; */

    ENTER();

    passoc_rsp              = (IEEEtypes_AssocRsp_t *)&resp->params;
    passoc_rsp->status_code = wlan_le16_to_cpu(passoc_rsp->status_code);
    if (pmpriv->media_connected == MTRUE)
        memcpy(pmpriv->adapter, cur_mac, pmpriv->curr_bss_params.bss_descriptor.mac_address, MLAN_MAC_ADDR_LENGTH);

    /* fixme: Enable this when req. We may not need to save the
       resp. buffer at all */

    /* fixme: This is not needed as of now. */

    /* Send a Media Connected event, according to the Spec */
    pmpriv->media_connected = MTRUE;

    /* Set the attempted BSSID Index to current */
    pbss_desc = pmpriv->pattempted_bss_desc;

    PRINTM(MINFO, "ASSOC_RESP: %s\n", pbss_desc->ssid.ssid);

    /* Make a copy of current BSSID descriptor */
    memcpy(pmpriv->adapter, &pmpriv->curr_bss_params.bss_descriptor, pbss_desc, sizeof(BSSDescriptor_t));

    /* Update curr_bss_params */
    pmpriv->curr_bss_params.bss_descriptor.channel = pbss_desc->phy_param_set.ds_param_set.current_chan;

    pmpriv->curr_bss_params.band = (t_u8)pbss_desc->bss_band;

    if (pbss_desc->wmm_ie.vend_hdr.element_id == WMM_IE)
        pmpriv->curr_bss_params.wmm_enabled = MTRUE;
    else
        pmpriv->curr_bss_params.wmm_enabled = MFALSE;

    if ((pmpriv->wmm_required || (pbss_desc->pht_cap && (pbss_desc->pht_cap->ieee_hdr.element_id == HT_CAPABILITY))) &&
        pmpriv->curr_bss_params.wmm_enabled)
        pmpriv->wmm_enabled = MTRUE;
    else
        pmpriv->wmm_enabled = MFALSE;

    /* fixme: Enable if req */
    if (pmpriv->sec_info.wpa_enabled || pmpriv->sec_info.wpa2_enabled)
        pmpriv->wpa_is_gtk_set = MFALSE;

    /* Reset SNR/NF/RSSI values */
    pmpriv->data_rssi_last = 0;
    pmpriv->data_nf_last   = 0;
    pmpriv->data_rssi_avg  = 0;
    pmpriv->data_nf_avg    = 0;
    pmpriv->bcn_rssi_last  = 0;
    pmpriv->bcn_nf_last    = 0;
    pmpriv->bcn_rssi_avg   = 0;
    pmpriv->bcn_nf_avg     = 0;
    pmpriv->rxpd_rate      = 0;
#ifdef SD8801
    pmpriv->rxpd_htinfo = 0;
#else
    pmpriv->rxpd_rate_info = 0;
#endif
    if (pbss_desc->pht_cap)
    {
        if (GETHT_MAXAMSDU(pbss_desc->pht_cap->ht_cap.ht_cap_info))
            pmpriv->max_amsdu = MLAN_TX_DATA_BUF_SIZE_8K;
        else
            pmpriv->max_amsdu = MLAN_TX_DATA_BUF_SIZE_4K;
    }

    /* Add the ra_list here for infra mode as there will be only 1 ra always */
    if (media_connected)
    {
        wlan_11n_cleanup_reorder_tbl(pmpriv);
    }
    if (!pmpriv->sec_info.wpa_enabled && !pmpriv->sec_info.wpa2_enabled && !pmpriv->sec_info.ewpa_enabled &&
        !pmpriv->sec_info.wapi_enabled)
    {
        /* We are in Open/WEP mode, open port immediately */
        if (pmpriv->port_ctrl_mode == MTRUE)
        {
            pmpriv->port_open = MTRUE;
            PRINTM(MINFO, "ASSOC_RESP: port_status = OPEN\n");
        }
    }

    if (pmpriv->sec_info.wpa_enabled || pmpriv->sec_info.wpa2_enabled || pmpriv->sec_info.ewpa_enabled ||
        pmpriv->sec_info.wapi_enabled)
        pmpriv->scan_block = MTRUE;

    /* done: */
    /* Need to indicate IOCTL complete */
    if (pioctl_req != MNULL)
    {
        if (ret != MLAN_STATUS_SUCCESS)
        {
            if (passoc_rsp->status_code)
                pioctl_req->status_code = wlan_le16_to_cpu(passoc_rsp->status_code);
            else
                pioctl_req->status_code = MLAN_ERROR_CMD_ASSOC_FAIL;
        }
        else
        {
            pioctl_req->status_code = MLAN_ERROR_NO_ERROR;
        }
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Associated to a specific BSS discovered in a scan
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pioctl_buf   A pointer to MLAN IOCTL Request buffer
 *  @param pbss_desc     A pointer to the BSS descriptor to associate with.
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_associate(IN mlan_private *pmpriv, IN t_void *pioctl_buf, IN BSSDescriptor_t *pbss_desc)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u8 current_bssid[MLAN_MAC_ADDR_LENGTH];
    pmlan_ioctl_req pioctl_req = (mlan_ioctl_req *)pioctl_buf;

    ENTER();

    /* Return error if the pmadapter or table entry is not marked as infra */
    if ((pmpriv->bss_mode != MLAN_BSS_MODE_INFRA) || (pbss_desc->bss_mode != MLAN_BSS_MODE_INFRA))
    {
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    memcpy(pmpriv->adapter, &current_bssid, &pmpriv->curr_bss_params.bss_descriptor.mac_address, sizeof(current_bssid));

    /* Clear any past association response stored for application retrieval */
    /* fixme: enable this later if assoc rsp buffer is needed */
    /* pmpriv->assoc_rsp_size = 0; */

    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_ASSOCIATE, HostCmd_ACT_GEN_SET, 0, pioctl_buf, pbss_desc);

    LEAVE();
    return ret;
}

/**
 *  @brief Convert band to radio type used in channel TLV
 *
 *  @param band		Band enumeration to convert to a channel TLV radio type
 *
 *  @return		Radio type designator for use in a channel TLV
 */
t_u8 wlan_band_to_radio_type(IN t_u8 band)
{
    t_u8 ret_radio_type;

    ENTER();

    switch (band)
    {
        case BAND_A:
        case BAND_AN:
        case BAND_A | BAND_AN:
            ret_radio_type = HostCmd_SCAN_RADIO_TYPE_A;
            break;
        case BAND_B:
        case BAND_G:
        case BAND_B | BAND_G:
        default:
            ret_radio_type = HostCmd_SCAN_RADIO_TYPE_BG;
            break;
    }

    LEAVE();
    return ret_radio_type;
}
