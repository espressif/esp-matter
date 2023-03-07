/** @file mlan_sta_cmdresp.c
 *
 *  @brief  This file provides the handling of command
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

void dump_rf_channel_info(HostCmd_DS_802_11_RF_CHANNEL *prf_channel);

/********************************************************
                Local Variables
********************************************************/

/********************************************************
                Global Variables
********************************************************/

/********************************************************
                Local Functions
********************************************************/
#ifdef CONFIG_RF_TEST_MODE
/**
 *  @brief This function prepares command resp of MFG Cmd
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status wlan_ret_mfg(pmlan_private pmpriv, HostCmd_DS_COMMAND *resp, void *pioctl_buf)
{
    HostCmd_DS_MFG_CMD_GENERIC_CFG *mcmd = (HostCmd_DS_MFG_CMD_GENERIC_CFG *)&resp->params.mfg_generic_cfg;
    mlan_ds_mfg_cmd_generic_cfg *cfg     = MNULL;
    mlan_status ret                      = MLAN_STATUS_SUCCESS;

    ENTER();
    if (!pioctl_buf)
    {
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    switch (wlan_le32_to_cpu(mcmd->mfg_cmd))
    {
        case MFG_CMD_SET_TEST_MODE:
        case MFG_CMD_UNSET_TEST_MODE:
        case MFG_CMD_TX_ANT:
        case MFG_CMD_RX_ANT:
        case MFG_CMD_RF_CHAN:
        case MFG_CMD_CLR_RX_ERR:
        case MFG_CMD_RF_BAND_AG:
        case MFG_CMD_RF_CHANNELBW:
            break;
        default:
            ret = MLAN_STATUS_FAILURE;
            goto cmd_mfg_done;
    }
    cfg = (mlan_ds_mfg_cmd_generic_cfg *)pioctl_buf;

    cfg->error = wlan_le32_to_cpu(mcmd->error);
    cfg->data1 = wlan_le32_to_cpu(mcmd->data1);
    cfg->data2 = wlan_le32_to_cpu(mcmd->data2);
    cfg->data3 = wlan_le32_to_cpu(mcmd->data3);
cmd_mfg_done:
    LEAVE();
    return ret;
}
#endif

/**
 *  @brief This function handles the command response of snmp_mib
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status wlan_ret_802_11_snmp_mib(IN pmlan_private pmpriv,
                                            IN HostCmd_DS_COMMAND *resp,
                                            IN mlan_ioctl_req *pioctl_buf)
{
    HostCmd_DS_802_11_SNMP_MIB *psmib = &resp->params.smib;
    t_u16 oid                         = wlan_le16_to_cpu(psmib->oid);
    t_u16 query_type                  = wlan_le16_to_cpu(psmib->query_type);
    t_u32 ul_temp;

    if (query_type == HostCmd_ACT_GEN_GET)
    {
        /* wmsdk: GET is not used. Disable */
    }
    else
    { /* (query_type == HostCmd_ACT_GEN_SET) */
        /* Update state for 11d */
        if (oid == Dot11D_i)
        {
            ul_temp = wlan_le16_to_cpu(*((t_u16 *)(psmib->value)));
            /* Set 11d state to private */
            pmpriv->state_11d.enable_11d = (state_11d_t)ul_temp;
            /* Set user enable flag if called from ioctl */
            if (pioctl_buf)
                pmpriv->state_11d.user_enable_11d = (state_11d_t)ul_temp;
        }
        /* Update state for 11h */
        if (oid == Dot11H_i)
        {
            ul_temp = wlan_le16_to_cpu(*((t_u16 *)(psmib->value)));
            /* Set 11h state to priv */
            pmpriv->intf_state_11h.is_11h_active = (ul_temp & ENABLE_11H_MASK);
            /* Set radar_det state to adapter */
            pmpriv->adapter->state_11h.is_master_radar_det_active = (ul_temp & MASTER_RADAR_DET_MASK) ? MTRUE : MFALSE;
            pmpriv->adapter->state_11h.is_slave_radar_det_active  = (ul_temp & SLAVE_RADAR_DET_MASK) ? MTRUE : MFALSE;
        }
    }

    if (pioctl_buf)
    {
        /* Indicate ioctl complete */
        pioctl_buf->data_read_written = sizeof(mlan_ds_snmp_mib);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of get_log
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status wlan_ret_get_log(IN pmlan_private pmpriv, IN HostCmd_DS_COMMAND *resp, IN mlan_ioctl_req *pioctl_buf)
{
    HostCmd_DS_802_11_GET_LOG *pget_log = (HostCmd_DS_802_11_GET_LOG *)&resp->params.get_log;
    mlan_ds_get_info *pget_info         = MNULL;

    ENTER();
    if (pioctl_buf)
    {
        pget_info                               = (mlan_ds_get_info *)pioctl_buf->pbuf;
        pget_info->param.stats.mcast_tx_frame   = wlan_le32_to_cpu(pget_log->mcast_tx_frame);
        pget_info->param.stats.failed           = wlan_le32_to_cpu(pget_log->failed);
        pget_info->param.stats.retry            = wlan_le32_to_cpu(pget_log->retry);
        pget_info->param.stats.multi_retry      = wlan_le32_to_cpu(pget_log->multiretry);
        pget_info->param.stats.frame_dup        = wlan_le32_to_cpu(pget_log->frame_dup);
        pget_info->param.stats.rts_success      = wlan_le32_to_cpu(pget_log->rts_success);
        pget_info->param.stats.rts_failure      = wlan_le32_to_cpu(pget_log->rts_failure);
        pget_info->param.stats.ack_failure      = wlan_le32_to_cpu(pget_log->ack_failure);
        pget_info->param.stats.rx_frag          = wlan_le32_to_cpu(pget_log->rx_frag);
        pget_info->param.stats.mcast_rx_frame   = wlan_le32_to_cpu(pget_log->mcast_rx_frame);
        pget_info->param.stats.fcs_error        = wlan_le32_to_cpu(pget_log->fcs_error);
        pget_info->param.stats.tx_frame         = wlan_le32_to_cpu(pget_log->tx_frame);
        pget_info->param.stats.wep_icv_error[0] = wlan_le32_to_cpu(pget_log->wep_icv_err_cnt[0]);
        pget_info->param.stats.wep_icv_error[1] = wlan_le32_to_cpu(pget_log->wep_icv_err_cnt[1]);
        pget_info->param.stats.wep_icv_error[2] = wlan_le32_to_cpu(pget_log->wep_icv_err_cnt[2]);
        pget_info->param.stats.wep_icv_error[3] = wlan_le32_to_cpu(pget_log->wep_icv_err_cnt[3]);
        /* Indicate ioctl complete */
        pioctl_buf->data_read_written = sizeof(mlan_ds_get_info);
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Get power level and rate index
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pdata_buf    Pointer to the data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status wlan_get_power_level(pmlan_private pmpriv, void *pdata_buf)
{
    int length = -1, max_power = -1, min_power = -1;
    MrvlTypes_Power_Group_t *ppg_tlv = MNULL;
    Power_Group_t *pg                = MNULL;

    ENTER();

    if (pdata_buf)
    {
        ppg_tlv = (MrvlTypes_Power_Group_t *)((t_u8 *)pdata_buf + sizeof(HostCmd_DS_TXPWR_CFG));
        pg      = (Power_Group_t *)((t_u8 *)ppg_tlv + sizeof(MrvlTypes_Power_Group_t));
        length  = ppg_tlv->length;
        if (length > 0)
        {
            max_power = pg->power_max;
            min_power = pg->power_min;
            length -= sizeof(Power_Group_t);
        }
        while (length)
        {
            pg++;
            if (max_power < pg->power_max)
            {
                max_power = pg->power_max;
            }
            if (min_power > pg->power_min)
            {
                min_power = pg->power_min;
            }
            length -= sizeof(Power_Group_t);
        }
        if (ppg_tlv->length > 0)
        {
            pmpriv->min_tx_power_level = (t_u8)min_power;
            pmpriv->max_tx_power_level = (t_u8)max_power;
        }
    }
    else
    {
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
/**
 *  @brief This function handles the command response of tx_power_cfg
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status wlan_ret_tx_power_cfg(IN pmlan_private pmpriv,
                                         IN HostCmd_DS_COMMAND *resp,
                                         IN mlan_ioctl_req *pioctl_buf)
{
    HostCmd_DS_TXPWR_CFG *ptxp_cfg   = &resp->params.txp_cfg;
    MrvlTypes_Power_Group_t *ppg_tlv = MNULL;
    Power_Group_t *pg                = MNULL;
    t_u16 action                     = wlan_le16_to_cpu(ptxp_cfg->action);
    mlan_ds_power_cfg *power         = MNULL;
    t_u32 data[5];

    ENTER();

    ppg_tlv = (MrvlTypes_Power_Group_t *)((t_u8 *)ptxp_cfg + sizeof(HostCmd_DS_TXPWR_CFG));
    pg      = (Power_Group_t *)((t_u8 *)ppg_tlv + sizeof(MrvlTypes_Power_Group_t));

    switch (action)
    {
        case HostCmd_ACT_GEN_GET:
            ppg_tlv->length = wlan_le16_to_cpu(ppg_tlv->length);
            if (pmpriv->adapter->hw_status == WlanHardwareStatusInitializing)
                wlan_get_power_level(pmpriv, ptxp_cfg);
            pmpriv->tx_power_level = (t_u16)pg->power_min;
            PRINTM(MMSG, "The Sta tx power level: %d\r\n", pmpriv->tx_power_level);
            break;

        case HostCmd_ACT_GEN_SET:
            if (wlan_le32_to_cpu(ptxp_cfg->mode))
            {
                if (pg->power_max == pg->power_min)
                    pmpriv->tx_power_level = (t_u16)pg->power_min;
            }
            break;
        default:
            PRINTM(MERROR, "CMD_RESP: unknown command action %d\n", action);
            LEAVE();
            return MLAN_STATUS_SUCCESS;
    }

    PRINTM(MINFO, "Current TxPower Level = %d,Max Power=%d, Min Power=%d\n", pmpriv->tx_power_level,
           pmpriv->max_tx_power_level, pmpriv->min_tx_power_level);

    if (pioctl_buf)
    {
        power = (mlan_ds_power_cfg *)pioctl_buf->pbuf;
        if (action == HostCmd_ACT_GEN_GET)
        {
            if (power->sub_command == MLAN_OID_POWER_CFG)
            {
                pioctl_buf->data_read_written      = sizeof(mlan_power_cfg_t) + MLAN_SUB_COMMAND_SIZE;
                power->param.power_cfg.power_level = pmpriv->tx_power_level;
                if (wlan_le32_to_cpu(ptxp_cfg->mode))
                    power->param.power_cfg.is_power_auto = 0;
                else
                    power->param.power_cfg.is_power_auto = 1;
            }
            else
            {
                power->param.power_ext.len = 0;
                while (ppg_tlv->length)
                {
                    data[0] = pg->first_rate_code;
                    data[1] = pg->last_rate_code;
                    if (pg->modulation_class == MOD_CLASS_OFDM)
                    {
                        data[0] += MLAN_RATE_INDEX_OFDM0;
                        data[1] += MLAN_RATE_INDEX_OFDM0;
                    }
                    else if (pg->modulation_class == MOD_CLASS_HT)
                    {
                        data[0] += MLAN_RATE_INDEX_MCS0;
                        data[1] += MLAN_RATE_INDEX_MCS0;
                        if (pg->ht_bandwidth == HT_BW_40)
                        {
                            data[0] |= TX_RATE_HT_BW40_BIT;
                            data[1] |= TX_RATE_HT_BW40_BIT;
                        }
                    }
                    data[2] = pg->power_min;
                    data[3] = pg->power_max;
                    data[4] = pg->power_step;
                    memcpy(pmpriv->adapter, (t_u8 *)(&power->param.power_ext.power_data[power->param.power_ext.len]),
                           (t_u8 *)data, sizeof(data));
                    power->param.power_ext.len += 5;
                    pg++;
                    ppg_tlv->length -= sizeof(Power_Group_t);
                }
                pioctl_buf->data_read_written = sizeof(mlan_power_cfg_ext) + MLAN_SUB_COMMAND_SIZE;
            }
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of deauthenticate
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status wlan_ret_802_11_deauthenticate(IN pmlan_private pmpriv,
                                           IN HostCmd_DS_COMMAND *resp,
                                           IN mlan_ioctl_req *pioctl_buf)
{
    ENTER();
    wlan_reset_connect_state(pmpriv, MTRUE);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of rf_channel
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status wlan_ret_802_11_rf_channel(IN pmlan_private pmpriv,
                                              IN HostCmd_DS_COMMAND *resp,
                                              IN mlan_ioctl_req *pioctl_buf)
{
    ENTER();
    /* fixme: enable this part when needed */
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/********************************************************
                Global Functions
********************************************************/
/**
 *  @brief This function handles the station command response
 *
 *  @param priv             A pointer to mlan_private structure
 *  @param cmdresp_no       cmd no
 *  @param pcmd_buf         cmdresp buf
 *  @param pioctl           A pointer to ioctl buf
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_ops_sta_process_cmdresp(IN t_void *priv, IN t_u16 cmdresp_no, IN t_void *pcmd_buf, IN t_void *pioctl)
{
    mlan_status ret            = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv       = (mlan_private *)priv;
    HostCmd_DS_COMMAND *resp   = (HostCmd_DS_COMMAND *)pcmd_buf;
    mlan_ioctl_req *pioctl_buf = (mlan_ioctl_req *)pioctl;
    /* mlan_adapter *pmadapter = pmpriv->adapter; */
    /* int ctr; */

    ENTER();

    /* fixme: enable below code when required */
    /* Command successful, handle response */
    switch (cmdresp_no)
    {
        case HostCmd_CMD_GET_HW_SPEC:
            ret = wlan_ret_get_hw_spec(pmpriv, resp, pioctl_buf);
            break;
        case HostCmd_CMD_TXPWR_CFG:
            ret = wlan_ret_tx_power_cfg(pmpriv, resp, pioctl_buf);
            break;
        case HostCmd_CMD_TX_RATE_CFG:
            ret = wlan_ret_tx_rate_cfg(pmpriv, resp, pioctl);
            break;
        case HostCmd_CMD_802_11_SCAN:
            ret        = wlan_ret_802_11_scan(pmpriv, resp, pioctl_buf);
            pioctl_buf = MNULL;
            /* pmadapter->curr_cmd->pioctl_buf = MNULL; */
            break;
        case HostCmd_CMD_802_11_ASSOCIATE:
            ret = wlan_ret_802_11_associate(pmpriv, resp, pioctl_buf);
            break;
        case HostCmd_CMD_802_11_GET_LOG:
            ret = wlan_ret_get_log(pmpriv, resp, pioctl_buf);
            break;
        case HostCmd_CMD_802_11_SNMP_MIB:
            ret = wlan_ret_802_11_snmp_mib(pmpriv, resp, pioctl_buf);
            break;
        case HostCmd_CMD_802_11_TX_RATE_QUERY:
            ret = wlan_ret_802_11_tx_rate_query(pmpriv, resp, pioctl_buf);
            break;
        case HostCmd_CMD_802_11_RF_CHANNEL:
            ret = wlan_ret_802_11_rf_channel(pmpriv, resp, pioctl_buf);
            break;
#ifdef CONFIG_RF_TEST_MODE
        case HostCmd_CMD_MFG_COMMAND:
            ret = wlan_ret_mfg(pmpriv, resp, pioctl_buf);
            break;
#endif
#ifdef OTP_CHANINFO
        case HostCmd_CMD_CHAN_REGION_CFG:
            ret = wlan_ret_chan_region_cfg(pmpriv, resp, pioctl_buf);
            break;
#endif
        default:
            PRINTM(MERROR, "CMD_RESP: Unknown command response %#x\n", resp->command);
            break;
    }

    LEAVE();
    return ret;
}
