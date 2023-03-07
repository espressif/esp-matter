/** @file mlan_sta_ioctl.c
 *
 *  @brief  This file provides functions for station ioctl
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
    10/21/2008: initial version
******************************************************/

#include <mlan_api.h>

/* Additional WMSDK header files */
#include <wmerrno.h>
#include <wm_os.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>
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
 *  @brief Start BSS
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_bss_ioctl_start(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret      = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_bss *bss     = (mlan_ds_bss *)pioctl_req->pbuf;
    t_s32 i              = -1;
    t_u8 zero_mac[]      = {0, 0, 0, 0, 0, 0};

    ENTER();

    /* Before ASSOC REQ, If "port ctrl" mode is enabled, move the port to
       CLOSED state */
    if (pmpriv->port_ctrl_mode == MTRUE)
    {
        PRINTM(MINFO, "bss_ioctl_start(): port_state=CLOSED\n");
        pmpriv->prior_port_status = pmpriv->port_open;
        pmpriv->port_open         = MFALSE;
    }
    pmpriv->scan_block = MFALSE;

    if (pmpriv->bss_mode == MLAN_BSS_MODE_INFRA)
    {
        if (!bss->param.ssid_bssid.idx || bss->param.ssid_bssid.idx > pmadapter->num_in_scan_table)
        {
            /* Search for the requested SSID in the scan table */
            if (bss->param.ssid_bssid.ssid.ssid_len)
            {
                if (memcmp(pmadapter, &bss->param.ssid_bssid.bssid, zero_mac, sizeof(zero_mac)))
                    i = wlan_find_ssid_in_list(pmpriv, &bss->param.ssid_bssid.ssid,
                                               (t_u8 *)&bss->param.ssid_bssid.bssid, MLAN_BSS_MODE_INFRA);
                else
                    i = wlan_find_ssid_in_list(pmpriv, &bss->param.ssid_bssid.ssid, MNULL, MLAN_BSS_MODE_INFRA);
            }
            else
            {
                i = wlan_find_bssid_in_list(pmpriv, (t_u8 *)&bss->param.ssid_bssid.bssid, MLAN_BSS_MODE_INFRA);
            }
        }
        else
        {
            /* fixme: Disabled for now since handling is done in legacy
             * code. It is IMPORTANT and needs to enabled.
             */
            i = bss->param.ssid_bssid.idx - 1;
        }
        if (i >= 0)
        {
            PRINTM(MINFO, "SSID found in scan list ... associating...\n");

            /* Clear any past association response stored for application
               retrieval */
            /* fixme: enable if needed later */
            /* pmpriv->assoc_rsp_size = 0; */
            ret = wlan_associate(pmpriv, pioctl_req, &pmadapter->pscan_table[i]);
            if (ret)
                goto start_ssid_done;
        }
        else
        { /* i >= 0 */
            /* fixme: The below seems related to Adhoc. Check later. */
        }
    }
    else
    {
    }

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

start_ssid_done:
    LEAVE();
    return ret;
}

/**
 *  @brief BSS command handler
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status wlan_bss_ioctl(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status status = MLAN_STATUS_SUCCESS;
    mlan_ds_bss *bss   = MNULL;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_bss))
    {
        PRINTM(MWARN, "MLAN bss IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed    = sizeof(mlan_ds_bss);
        pioctl_req->status_code       = MLAN_ERROR_INVALID_PARAMETER;
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }

    bss = (mlan_ds_bss *)pioctl_req->pbuf;

    switch (bss->sub_command)
    {
        case MLAN_OID_BSS_START:
            status = wlan_bss_ioctl_start(pmadapter, pioctl_req);
            break;
        default:
            status = MLAN_STATUS_FAILURE;
            break;
    }

    LEAVE();
    return status;
}

/**
 *  @brief Rate command handler
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_rate_ioctl(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status status = MLAN_STATUS_SUCCESS;
    mlan_ds_rate *rate = MNULL;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_rate))
    {
        PRINTM(MWARN, "MLAN bss IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed    = sizeof(mlan_ds_rate);
        pioctl_req->status_code       = MLAN_ERROR_INVALID_PARAMETER;
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }
    rate = (mlan_ds_rate *)pioctl_req->pbuf;
    switch (rate->sub_command)
    {
        case MLAN_OID_RATE_CFG:
            status = wlan_rate_ioctl_cfg(pmadapter, pioctl_req);
            break;
        default:
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
            status                  = MLAN_STATUS_FAILURE;
            break;
    }
    LEAVE();
    return status;
}

/**
 *  @brief Get Tx power configuration
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param cmd_no	Firmware command number used to retrieve power values
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_power_ioctl_get_power(IN pmlan_adapter pmadapter,
                                              IN t_u16 cmd_no,
                                              IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret      = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];

    ENTER();

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, cmd_no, HostCmd_ACT_GEN_GET, 0, (t_void *)pioctl_req, MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Set Tx power configuration
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_power_ioctl_set_power(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_ds_power_cfg *power = MNULL;
    mlan_status ret          = MLAN_STATUS_SUCCESS;

    HostCmd_DS_TXPWR_CFG *txp_cfg   = MNULL;
    MrvlTypes_Power_Group_t *pg_tlv = MNULL;
    Power_Group_t *pg               = MNULL;
    pmlan_callbacks pcb             = &pmadapter->callbacks;
    t_u8 *buf                       = MNULL;
    t_u16 dbm                       = 0;
    mlan_private *pmpriv            = pmadapter->priv[pioctl_req->bss_index];

    ENTER();

    power = (mlan_ds_power_cfg *)pioctl_req->pbuf;
    if (!power->param.power_cfg.is_power_auto)
    {
        dbm = (t_u16)power->param.power_cfg.power_level;
#if 0
        if ((dbm < pmpriv->min_tx_power_level) ||
            (dbm > pmpriv->max_tx_power_level)) {
            PRINTM(MERROR,
                   "The set txpower value %d dBm is out of range (%d dBm-%d dBm)!\n",
                   dbm, pmpriv->min_tx_power_level, pmpriv->max_tx_power_level);
            pioctl_req->status_code = MLAN_ERROR_INVALID_PARAMETER;
            ret = MLAN_STATUS_FAILURE;
            goto exit;
        }
#endif
    }

    ret = pcb->moal_malloc(pmadapter->pmoal_handle, MRVDRV_SIZE_OF_CMD_BUFFER, MLAN_MEM_DEF, &buf);
    if (ret != MLAN_STATUS_SUCCESS || buf == MNULL)
    {
        PRINTM(MERROR, "ALLOC_CMD_BUF: Failed to allocate command buffer\n");
        pioctl_req->status_code = MLAN_ERROR_NO_MEM;
        ret                     = MLAN_STATUS_FAILURE;
        goto exit;
    }
    memset(pmadapter, buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);
    txp_cfg         = (HostCmd_DS_TXPWR_CFG *)buf;
    txp_cfg->action = HostCmd_ACT_GEN_SET;
    if (!power->param.power_cfg.is_power_auto)
    {
        txp_cfg->mode  = 1;
        pg_tlv         = (MrvlTypes_Power_Group_t *)(buf + sizeof(HostCmd_DS_TXPWR_CFG));
        pg_tlv->type   = TLV_TYPE_POWER_GROUP;
        pg_tlv->length = 4 * sizeof(Power_Group_t);
        pg             = (Power_Group_t *)(buf + sizeof(HostCmd_DS_TXPWR_CFG) + sizeof(MrvlTypes_Power_Group_t));
        /* Power group for modulation class HR/DSSS */
        pg->first_rate_code  = 0x00;
        pg->last_rate_code   = 0x03;
        pg->modulation_class = MOD_CLASS_HR_DSSS;
        pg->power_step       = 0;
        pg->power_min        = (t_s8)dbm;
        pg->power_max        = (t_s8)dbm;
        pg++;
        /* Power group for modulation class OFDM */
        pg->first_rate_code  = 0x00;
        pg->last_rate_code   = 0x07;
        pg->modulation_class = MOD_CLASS_OFDM;
        pg->power_step       = 0;
        pg->power_min        = (t_s8)dbm;
        pg->power_max        = (t_s8)dbm;
        pg++;
        /* Power group for modulation class HTBW20 */
        pg->first_rate_code  = 0x00;
        pg->last_rate_code   = 0x20;
        pg->modulation_class = MOD_CLASS_HT;
        pg->power_step       = 0;
        pg->power_min        = (t_s8)dbm;
        pg->power_max        = (t_s8)dbm;
        pg->ht_bandwidth     = HT_BW_20;
        pg++;
        /* Power group for modulation class HTBW40 */
        pg->first_rate_code  = 0x00;
        pg->last_rate_code   = 0x20;
        pg->modulation_class = MOD_CLASS_HT;
        pg->power_step       = 0;
        pg->power_min        = (t_s8)dbm;
        pg->power_max        = (t_s8)dbm;
        pg->ht_bandwidth     = HT_BW_40;
    }

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_TXPWR_CFG, HostCmd_ACT_GEN_SET, 0, (t_void *)pioctl_req, buf);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    if (buf)
        pcb->moal_mfree(pmadapter->pmoal_handle, buf);

exit:
    LEAVE();
    return ret;
}

/**
 *  @brief Get modulation class from rate index
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param rate_index	Rate index
 *
 *  @return		0 fail, otherwise return modulation class
 */
static int wlan_get_modulation_class(pmlan_adapter pmadapter, int rate_index)
{
    ENTER();
    if (rate_index >= MLAN_RATE_INDEX_HRDSSS0 && rate_index <= MLAN_RATE_INDEX_HRDSSS3)
    {
        LEAVE();
        return MOD_CLASS_HR_DSSS;
    }
    else if (rate_index >= MLAN_RATE_INDEX_OFDM0 && rate_index <= MLAN_RATE_INDEX_OFDM7)
    {
        LEAVE();
        return MOD_CLASS_OFDM;
    }
    else if (rate_index >= MLAN_RATE_INDEX_MCS0 && rate_index <= MLAN_RATE_INDEX_MCS127)
    {
        LEAVE();
        return MOD_CLASS_HT;
    }
    PRINTM(MERROR, "Invalid rate index = %d supplied!\n", rate_index);

    LEAVE();
    return 0;
}

/**
 *  @brief Set extended power configuration
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_power_ioctl_set_power_ext(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret                 = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv            = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_power_cfg *power        = MNULL;
    pmlan_callbacks pcb             = &pmadapter->callbacks;
    t_u8 *buf                       = MNULL;
    HostCmd_DS_TXPWR_CFG *txp_cfg   = MNULL;
    MrvlTypes_Power_Group_t *pg_tlv = MNULL;
    Power_Group_t *pg               = MNULL;
    int mod_class;
    t_u32 data[4];
    t_u8 ht_bw;

    ENTER();

    power = (mlan_ds_power_cfg *)pioctl_req->pbuf;
    ret   = pcb->moal_malloc(pmadapter->pmoal_handle, MRVDRV_SIZE_OF_CMD_BUFFER, MLAN_MEM_DEF, &buf);
    if (ret != MLAN_STATUS_SUCCESS || buf == MNULL)
    {
        PRINTM(MERROR, "ALLOC_CMD_BUF: Failed to allocate command buffer\n");
        pioctl_req->status_code = MLAN_ERROR_NO_MEM;
        ret                     = MLAN_STATUS_FAILURE;
        goto exit;
    }
    memset(pmadapter, buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);
    txp_cfg         = (HostCmd_DS_TXPWR_CFG *)buf;
    txp_cfg->action = HostCmd_ACT_GEN_SET;
    memcpy(pmadapter, (t_u8 *)&data, (t_u8 *)power->param.power_ext.power_data, sizeof(data));
    switch (power->param.power_ext.len)
    {
        case 1:
            if (data[0] == 0xFF)
                txp_cfg->mode = 0;
            else
            {
                pioctl_req->status_code = MLAN_ERROR_INVALID_PARAMETER;
                ret                     = MLAN_STATUS_FAILURE;
            }
            break;
        case 2:
        case 4:
            ht_bw = (data[0] & TX_RATE_HT_BW40_BIT) ? HT_BW_40 : HT_BW_20;
            data[0] &= ~TX_RATE_HT_BW40_BIT;
            if (!(mod_class = wlan_get_modulation_class(pmadapter, data[0])))
            {
                pioctl_req->status_code = MLAN_ERROR_CMD_RESP_FAIL;
                ret                     = MLAN_STATUS_FAILURE;
                break;
            }
            if (ht_bw && mod_class != MOD_CLASS_HT)
            {
                pioctl_req->status_code = MLAN_ERROR_CMD_RESP_FAIL;
                ret                     = MLAN_STATUS_FAILURE;
                break;
            }
            txp_cfg->mode  = 1;
            pg_tlv         = (MrvlTypes_Power_Group_t *)(buf + sizeof(HostCmd_DS_TXPWR_CFG));
            pg_tlv->type   = TLV_TYPE_POWER_GROUP;
            pg_tlv->length = sizeof(Power_Group_t);
            pg             = (Power_Group_t *)(buf + sizeof(HostCmd_DS_TXPWR_CFG) + sizeof(MrvlTypes_Power_Group_t));
            pg->modulation_class = (t_u8)mod_class;
            pg->first_rate_code  = (t_u8)data[0];
            pg->last_rate_code   = (t_u8)data[0];
            if (mod_class == MOD_CLASS_OFDM)
            {
                pg->first_rate_code = (t_u8)(data[0] - MLAN_RATE_INDEX_OFDM0);
                pg->last_rate_code  = (t_u8)(data[0] - MLAN_RATE_INDEX_OFDM0);
            }
            else if (mod_class == MOD_CLASS_HT)
            {
                pg->first_rate_code = (t_u8)(data[0] - MLAN_RATE_INDEX_MCS0);
                pg->last_rate_code  = (t_u8)(data[0] - MLAN_RATE_INDEX_MCS0);
                pg->ht_bandwidth    = ht_bw;
            }
            pg->power_min = (t_s8)data[1];
            pg->power_max = (t_s8)data[1];
            if (power->param.power_ext.len == 4)
            {
                pg->power_max  = (t_s8)data[2];
                pg->power_step = (t_s8)data[3];
            }
            break;
        default:
            pioctl_req->status_code = MLAN_ERROR_INVALID_PARAMETER;
            ret                     = MLAN_STATUS_FAILURE;
            break;
    }
    if (ret == MLAN_STATUS_FAILURE)
    {
        if (buf)
            pcb->moal_mfree(pmadapter->pmoal_handle, buf);
        goto exit;
    }

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_TXPWR_CFG, HostCmd_ACT_GEN_SET, 0, (t_void *)pioctl_req, buf);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;
    if (buf)
        pcb->moal_mfree(pmadapter->pmoal_handle, buf);

exit:
    LEAVE();
    return ret;
}

/**
 *  @brief Power configuration command handler
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status wlan_power_ioctl(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status status       = MLAN_STATUS_SUCCESS;
    mlan_ds_power_cfg *power = MNULL;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_power_cfg))
    {
        PRINTM(MWARN, "MLAN bss IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed    = sizeof(mlan_ds_power_cfg);
        pioctl_req->status_code       = MLAN_ERROR_INVALID_PARAMETER;
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }
    power = (mlan_ds_power_cfg *)pioctl_req->pbuf;
    switch (power->sub_command)
    {
        case MLAN_OID_POWER_CFG:
            if (pioctl_req->action == MLAN_ACT_GET)
                status = wlan_power_ioctl_get_power(pmadapter, HostCmd_CMD_TXPWR_CFG, pioctl_req);
            else
                status = wlan_power_ioctl_set_power(pmadapter, pioctl_req);
            break;
        case MLAN_OID_POWER_CFG_EXT:
            if (pioctl_req->action == MLAN_ACT_GET)
                status = wlan_power_ioctl_get_power(pmadapter, HostCmd_CMD_TXPWR_CFG, pioctl_req);
            else
                status = wlan_power_ioctl_set_power_ext(pmadapter, pioctl_req);
            break;
        default:
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
            status                  = MLAN_STATUS_FAILURE;
            break;
    }
    LEAVE();
    return status;
}

/**
 *  @brief Get Random charactor
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *
 *  @return		random charactor
 */
t_u8 wlan_get_random_charactor(pmlan_adapter pmadapter)
{
    t_u32 sec, usec;
    t_u8 ch = 0;

    ENTER();

    sec  = 10; // wmtime_time_get_posix();
    usec = 0;

    sec  = (sec & 0xFFFF) + (sec >> 16);
    usec = (usec & 0xFFFF) + (usec >> 16);
    ch   = (((sec << 16) + usec) % 26) + 'a';
    LEAVE();
    return ch;
}

/**
 *  @brief Set WEP keys
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_sec_ioctl_set_wep_key(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret          = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv     = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_sec_cfg *sec     = MNULL;
    mrvl_wep_key_t *pwep_key = MNULL;
    int index;
    unsigned int i = 0;

    ENTER();

    if (pmpriv->wep_key_curr_index >= MRVL_NUM_WEP_KEY)
        pmpriv->wep_key_curr_index = 0;
    pwep_key = &pmpriv->wep_key[pmpriv->wep_key_curr_index];
    sec      = (mlan_ds_sec_cfg *)pioctl_req->pbuf;
    if (sec->param.encrypt_key.key_index == MLAN_KEY_INDEX_DEFAULT)
    {
        index                            = pmpriv->wep_key_curr_index;
        sec->param.encrypt_key.key_index = index;
    }
    else
    {
        if (sec->param.encrypt_key.key_index >= MRVL_NUM_WEP_KEY)
        {
            if ((sec->param.encrypt_key.key_remove == MTRUE) && (sec->param.encrypt_key.key_index <= 5))
            {
                /* call firmware remove key */
                ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_KEY_MATERIAL, HostCmd_ACT_GEN_SET, 0, MNULL,
                                       &sec->param.encrypt_key);
                goto exit;
            }
            PRINTM(MERROR, "Key_index is invalid\n");
            ret = MLAN_STATUS_FAILURE;
            goto exit;
        }
        index = sec->param.encrypt_key.key_index;
    }

    if ((sec->param.encrypt_key.key_disable == MTRUE) || (sec->param.encrypt_key.key_remove == MTRUE))
    {
        pmpriv->sec_info.wep_status = Wlan802_11WEPDisabled;
        /* remove key */
        if (sec->param.encrypt_key.key_remove == MTRUE)
        {
            memset(pmadapter, &pmpriv->wep_key[index], 0, sizeof(mrvl_wep_key_t));
            /* call firmware remove key */
            ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_KEY_MATERIAL, HostCmd_ACT_GEN_SET, 0, MNULL,
                                   &sec->param.encrypt_key);
            if (ret)
                goto exit;
        }
    }
    else
    {
        if (sec->param.encrypt_key.key_len)
        {
            if ((sec->param.encrypt_key.key_len != WEP_104_BIT_LEN) &&
                (sec->param.encrypt_key.key_len != WEP_40_BIT_LEN))
            {
                PRINTM(MERROR, "Invalid wep key len=%d\n", sec->param.encrypt_key.key_len);
                /* We will use random key to clear the key buffer in FW */
                if (sec->param.encrypt_key.key_len < WEP_40_BIT_LEN)
                    sec->param.encrypt_key.key_len = WEP_40_BIT_LEN;
                else
                    sec->param.encrypt_key.key_len = WEP_104_BIT_LEN;
                for (i = 0; i < sec->param.encrypt_key.key_len; i++)
                    sec->param.encrypt_key.key_material[i] = wlan_get_random_charactor(pmadapter);
            }
            pwep_key = &pmpriv->wep_key[index];
            /* Cleanup */
            memset(pmadapter, pwep_key, 0, sizeof(mrvl_wep_key_t));
            /* Copy the key in the driver */

            memcpy(pmadapter, pwep_key->key_material, sec->param.encrypt_key.key_material,
                   sec->param.encrypt_key.key_len);
            pwep_key->key_index  = index;
            pwep_key->key_length = sec->param.encrypt_key.key_len;
            if (pmpriv->sec_info.wep_status != Wlan802_11WEPEnabled)
            {
                /*
                 * The status is set as Key Absent
                 * so as to make sure we display the
                 * keys when iwlist mlanX key is used
                 */
                pmpriv->sec_info.wep_status = Wlan802_11WEPKeyAbsent;
            }
        }
        if (sec->param.encrypt_key.is_current_wep_key == MTRUE)
        {
            /* Copy the required key as the current key */
            pwep_key = &pmpriv->wep_key[index];
            if (!pwep_key->key_length)
            {
                if (0 || pmpriv->sec_info.wpa_enabled
#ifdef WPA2
                    || &pmpriv->sec_info.wpa2_enabled
#endif
                )
                {
                    ret = MLAN_STATUS_SUCCESS;
                    goto exit;
                }
                PRINTM(MERROR,
                       "Key %d not set,so "
                       "cannot enable it\n",
                       index);
                pioctl_req->status_code = MLAN_ERROR_CMD_RESP_FAIL;
                ret                     = MLAN_STATUS_FAILURE;
                goto exit;
            }
            pmpriv->wep_key_curr_index  = (t_u16)index;
            pmpriv->sec_info.wep_status = Wlan802_11WEPEnabled;
        }
        if (sec->param.encrypt_key.key_flags && pwep_key->key_length)
        {
            pmpriv->wep_key_curr_index  = (t_u16)index;
            pmpriv->sec_info.wep_status = Wlan802_11WEPEnabled;
        }
    }
    if (pmpriv->sec_info.wep_status == Wlan802_11WEPEnabled)
        pmpriv->curr_pkt_filter |= HostCmd_ACT_MAC_WEP_ENABLE;
    else
        pmpriv->curr_pkt_filter &= ~HostCmd_ACT_MAC_WEP_ENABLE;

    /* Send request to firmware */
    if (pmpriv->sec_info.wep_status == Wlan802_11WEPEnabled && pwep_key->key_length)
    {
        ret =
            wlan_prepare_cmd(pmpriv, HostCmd_CMD_MAC_CONTROL, HostCmd_ACT_GEN_SET, 0, MNULL, &pmpriv->curr_pkt_filter);
        if (ret)
            goto exit;
        if (!sec->param.encrypt_key.key_len)
        {
            sec->param.encrypt_key.key_index = pwep_key->key_index;
            sec->param.encrypt_key.key_len   = pwep_key->key_length;
            memcpy(pmadapter, sec->param.encrypt_key.key_material, pwep_key->key_material,
                   sec->param.encrypt_key.key_len);
        }
        ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_KEY_MATERIAL, HostCmd_ACT_GEN_SET, 0, (t_void *)pioctl_req,
                               &sec->param.encrypt_key);
    }
    else
    {
        if (pwep_key->key_length)
        {
            if (!sec->param.encrypt_key.key_len)
            {
                sec->param.encrypt_key.key_index = pwep_key->key_index;
                sec->param.encrypt_key.key_len   = pwep_key->key_length;
                memcpy(pmadapter, sec->param.encrypt_key.key_material, pwep_key->key_material,
                       sec->param.encrypt_key.key_len);
            }
            ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_KEY_MATERIAL, HostCmd_ACT_GEN_SET, 0, MNULL,
                                   &sec->param.encrypt_key);
            if (ret)
                goto exit;
        }
        ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_MAC_CONTROL, HostCmd_ACT_GEN_SET, 0, (t_void *)pioctl_req,
                               &pmpriv->curr_pkt_filter);
    }
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

exit:
    LEAVE();
    return ret;
}

/**
 *  @brief Set WPA key
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_sec_ioctl_set_wpa_key(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret      = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_sec_cfg *sec = MNULL;

    ENTER();

    sec = (mlan_ds_sec_cfg *)pioctl_req->pbuf;
    /* Current driver only supports key length of up to 32 bytes */
    if (sec->param.encrypt_key.key_len > MLAN_MAX_KEY_LENGTH)
    {
        PRINTM(MERROR, "Key length is incorrect\n");
        pioctl_req->status_code = MLAN_ERROR_INVALID_PARAMETER;
        ret                     = MLAN_STATUS_FAILURE;
        goto exit;
    }

    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_KEY_MATERIAL, HostCmd_ACT_GEN_SET, 0, (t_void *)pioctl_req,
                           &sec->param.encrypt_key);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

exit:
    LEAVE();
    return ret;
}

/**
 *  @brief Set security key(s)
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_sec_ioctl_encrypt_key(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status status   = MLAN_STATUS_SUCCESS;
    mlan_ds_sec_cfg *sec = MNULL;
    ENTER();
    sec = (mlan_ds_sec_cfg *)pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_SET)
    {
        if (sec->param.encrypt_key.key_len > MAX_WEP_KEY_SIZE)
            status = wlan_sec_ioctl_set_wpa_key(pmadapter, pioctl_req);
        else
            status = wlan_sec_ioctl_set_wep_key(pmadapter, pioctl_req);
    }
    else
    {
        /* fixme: enable when needed */
        /* status = wlan_sec_ioctl_get_key(pmadapter, pioctl_req); */
    }
    LEAVE();
    return status;
}

/**
 *  @brief Set/Get WPA passphrase from embedded supplicant
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_sec_ioctl_passphrase(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret      = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_sec_cfg *sec = MNULL;
    t_u16 cmd_action     = 0;
    /* BSSDescriptor_t *pbss_desc; */
    /* int i = 0; */

    ENTER();

    sec = (mlan_ds_sec_cfg *)pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_SET)
    {
        if (sec->param.passphrase.psk_type == MLAN_PSK_CLEAR)
            cmd_action = HostCmd_ACT_GEN_REMOVE;
        else
            cmd_action = HostCmd_ACT_GEN_SET;
    }
    else
    {
        if (sec->param.passphrase.psk_type == MLAN_PSK_QUERY)
        {
            if (sec->param.passphrase.ssid.ssid_len == 0)
            {
                /* fixme: We do not need this functionality right now. */
            }
            else
                memset(pmadapter, &sec->param.passphrase.bssid, 0, MLAN_MAC_ADDR_LENGTH);
        }
        cmd_action = HostCmd_ACT_GEN_GET;
    }

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_SUPPLICANT_PMK, cmd_action, 0, (t_void *)pioctl_req,
                           &sec->param.passphrase);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Set/Get WPA3 SAE password from embedded supplicant
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_sec_ioctl_password(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret      = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_sec_cfg *sec = MNULL;
    t_u16 cmd_action     = 0;
    /* BSSDescriptor_t *pbss_desc; */
    /* int i = 0; */

    ENTER();

    sec = (mlan_ds_sec_cfg *)pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_SET)
    {
        if (sec->param.passphrase.psk_type == MLAN_PSK_CLEAR)
            cmd_action = HostCmd_ACT_GEN_REMOVE;
        else
            cmd_action = HostCmd_ACT_GEN_SET;
    }
    else
    {
        if (sec->param.passphrase.psk_type == MLAN_PSK_QUERY)
        {
            if (sec->param.passphrase.ssid.ssid_len == 0)
            {
                /* fixme: We do not need this functionality right now. */
            }
            else
                memset(pmadapter, &sec->param.passphrase.bssid, 0, MLAN_MAC_ADDR_LENGTH);
        }
        cmd_action = HostCmd_ACT_GEN_GET;
    }

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_SUPPLICANT_PMK, cmd_action, 0, (t_void *)pioctl_req,
                           &sec->param.passphrase);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Security configuration handler
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_sec_cfg_ioctl(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status status   = MLAN_STATUS_SUCCESS;
    mlan_ds_sec_cfg *sec = MNULL;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_sec_cfg))
    {
        PRINTM(MWARN, "MLAN bss IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed    = sizeof(mlan_ds_sec_cfg);
        pioctl_req->status_code       = MLAN_ERROR_INVALID_PARAMETER;
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }
    sec = (mlan_ds_sec_cfg *)pioctl_req->pbuf;
    switch (sec->sub_command)
    {
        case MLAN_OID_SEC_CFG_ENCRYPT_KEY:
            status = wlan_sec_ioctl_encrypt_key(pmadapter, pioctl_req);
            break;
        case MLAN_OID_SEC_CFG_PASSPHRASE:
            status = wlan_sec_ioctl_passphrase(pmadapter, pioctl_req);
            break;
        case MLAN_OID_SEC_CFG_PASSWORD:
            status = wlan_sec_ioctl_password(pmadapter, pioctl_req);
            break;
        default:
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
            status                  = MLAN_STATUS_FAILURE;
            break;
    }
    LEAVE();
    return status;
}

/**
 *  @brief 11D configuration handler
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise fail
 */
mlan_status wlan_11d_cfg_ioctl(IN mlan_private *pmpriv, IN pmlan_ioctl_req pioctl_req)
{
    mlan_adapter *pmadapter   = pmpriv->adapter;
    mlan_status status        = MLAN_STATUS_SUCCESS;
    mlan_ds_11d_cfg *pcfg_11d = MNULL;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_11d_cfg))
    {
        PRINTM(MWARN, "MLAN bss IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed    = sizeof(mlan_ds_11d_cfg);
        pioctl_req->status_code       = MLAN_ERROR_INVALID_PARAMETER;
        status                        = MLAN_STATUS_RESOURCE;
        goto exit;
    }

    pcfg_11d = (mlan_ds_11d_cfg *)pioctl_req->pbuf;
    switch (pcfg_11d->sub_command)
    {
        case MLAN_OID_11D_DOMAIN_INFO:
            if (pmpriv->support_11d_APIs)
                status = pmpriv->support_11d_APIs->wlan_11d_cfg_domain_info_p(pmadapter, pioctl_req);
            break;
        default:
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
            status                  = MLAN_STATUS_FAILURE;
            break;
    }

exit:
    LEAVE();
    return status;
}

/**
 *  @brief Set/Get region code
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
/* static */ mlan_status wlan_misc_ioctl_region(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret        = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv   = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_misc_cfg *misc = MNULL;
    int i;

    ENTER();

    misc = (mlan_ds_misc_cfg *)pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_GET)
    {
        misc->param.region_code = pmadapter->region_code;
    }
    else
    {
#ifdef OTP_CHANINFO
        if (pmadapter->otp_region && pmadapter->otp_region->force_reg)
        {
            PRINTF(
                "ForceRegionRule is set in the on-chip OTP"
                " memory\r\n");
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
#endif
        for (i = 0; i < MRVDRV_MAX_REGION_CODE; i++)
        {
            /* Use the region code to search for the index */
            if (misc->param.region_code == region_code_index[i])
            {
                pmadapter->region_code = (t_u16)misc->param.region_code;
                break;
            }
        }
        /* It's unidentified region code */
        if (i >= MRVDRV_MAX_REGION_CODE)
        {
            PRINTM(MERROR, "Region Code not identified\n");
            pioctl_req->status_code = MLAN_ERROR_INVALID_PARAMETER;
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
        pmadapter->cfp_code_bg = misc->param.region_code;
        pmadapter->cfp_code_a  = misc->param.region_code;
        if (wlan_set_regiontable(pmpriv, (t_u8)pmadapter->region_code,
                                 pmadapter->config_bands | pmadapter->adhoc_start_band))
        {
            pioctl_req->status_code = MLAN_ERROR_IOCTL_FAIL;
            ret                     = MLAN_STATUS_FAILURE;
        }
    }
    pioctl_req->data_read_written = sizeof(t_u32) + MLAN_SUB_COMMAND_SIZE;

    LEAVE();
    return ret;
}

/**
 *  @brief Miscellaneous configuration handler
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_misc_cfg_ioctl(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status status     = MLAN_STATUS_SUCCESS;
    mlan_ds_misc_cfg *misc = MNULL;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_misc_cfg))
    {
        PRINTM(MWARN, "MLAN bss IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed    = sizeof(mlan_ds_misc_cfg);
        pioctl_req->status_code       = MLAN_ERROR_INVALID_PARAMETER;
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }
    if ((pioctl_req == MNULL) || (pioctl_req->pbuf == MNULL))
    {
        PRINTM(MERROR, "Request buffer not found!\n");
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    misc = (mlan_ds_misc_cfg *)pioctl_req->pbuf;
    switch (misc->sub_command)
    {
#ifdef WLAN_LOW_POWER_ENABLE
        case MLAN_OID_MISC_LOW_PWR_MODE:
            status = wlan_misc_ioctl_low_pwr_mode(pmadapter, pioctl_req);
            break;
#endif // WLAN_LOW_POWER_ENABLE
        default:
            if (pioctl_req)
                pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
            status = MLAN_STATUS_FAILURE;
            break;
    }
    LEAVE();
    return status;
}

/**
 *  @brief MLAN station ioctl handler
 *
 *  @param adapter 	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise fail
 */
mlan_status wlan_ops_sta_ioctl(t_void *adapter, pmlan_ioctl_req pioctl_req)
{
    pmlan_private pmpriv    = MNULL;
    pmlan_adapter pmadapter = (pmlan_adapter)adapter;
    mlan_status status      = MLAN_STATUS_SUCCESS;

    ENTER();

    if (pioctl_req != MNULL)
    {
        pmpriv = pmadapter->priv[pioctl_req->bss_index];
    }
    else
    {
        PRINTM(MERROR, "MLAN IOCTL information is not present\n");
        return MLAN_STATUS_FAILURE;
    }

    switch (pioctl_req->req_id)
    {
        case MLAN_IOCTL_BSS:
            status = wlan_bss_ioctl(pmadapter, pioctl_req);
            break;
        case MLAN_IOCTL_SEC_CFG:
            status = wlan_sec_cfg_ioctl(pmadapter, pioctl_req);
            break;
        case MLAN_IOCTL_RATE:
            status = wlan_rate_ioctl(pmadapter, pioctl_req);
            break;
        case MLAN_IOCTL_POWER_CFG:
            status = wlan_power_ioctl(pmadapter, pioctl_req);
            break;
        case MLAN_IOCTL_11N_CFG:
            status = wlan_11n_cfg_ioctl(pmadapter, pioctl_req);
            break;
        case MLAN_IOCTL_11D_CFG:
            if (pmpriv->support_11d_APIs)
                status = pmpriv->support_11d_APIs->wlan_11d_cfg_ioctl_p(pmpriv, pioctl_req);
            break;
        case MLAN_IOCTL_MISC_CFG:
            status = wlan_misc_cfg_ioctl(pmadapter, pioctl_req);
            break;
        default:
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
            status                  = MLAN_STATUS_FAILURE;
            break;
    }
    LEAVE();
    return status;
}
