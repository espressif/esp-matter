/** @file mlan_misc.c
 *
 *  @brief  This file provides Miscellaneous functions for MLAN module
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
    05/11/2009: initial version
************************************************************/
#include <mlan_api.h>

/* Additional WMSDK header files */
#include <wmerrno.h>
#include <wm_os.h>
#include <mlan_fw.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>

/**
 *  @brief This function allocates a mlan_buffer.
 *
 *  @param pmadapter Pointer to mlan_adapter
 *  @param data_len   Data length
 *  @param head_room  head_room reserved in mlan_buffer
 *  @param malloc_flag  flag to user moal_malloc
 *  @return           mlan_buffer pointer or MNULL
 */
pmlan_buffer wlan_alloc_mlan_buffer(mlan_adapter *pmadapter, t_u32 data_len, t_u32 head_room, t_u32 malloc_flag)
{
    mlan_status ret    = MLAN_STATUS_SUCCESS;
    pmlan_buffer pmbuf = MNULL;
    t_u32 buf_size     = 0;
    /* t_u8 *tmp_buf = MNULL; */
    pmlan_callbacks pcb = &pmadapter->callbacks;

    ENTER();

    /* make sure that the data length is at least SDIO block size */
    data_len = ALIGN_SZ(data_len, MLAN_SDIO_BLOCK_SIZE);

    /* head_room is not implemented for malloc mlan buffer */
    if (malloc_flag == MTRUE)
    {
        buf_size = sizeof(mlan_buffer) + data_len + DMA_ALIGNMENT;
        ret      = pcb->moal_malloc(pmadapter->pmoal_handle, buf_size, MLAN_MEM_DEF | MLAN_MEM_DMA, (t_u8 **)&pmbuf);
        if ((ret != MLAN_STATUS_SUCCESS) || !pmbuf)
        {
            pmbuf = MNULL;
            goto exit;
        }
        memset(pmadapter, pmbuf, 0, sizeof(mlan_buffer));

        pmbuf->pdesc = MNULL;
        /* Align address */
        pmbuf->pbuf        = (t_u8 *)ALIGN_ADDR((t_u8 *)pmbuf + sizeof(mlan_buffer), DMA_ALIGNMENT);
        pmbuf->data_offset = 0;
        pmbuf->data_len    = data_len;
        pmbuf->flags |= MLAN_BUF_FLAG_MALLOC_BUF;
    }
exit:
    LEAVE();
    return pmbuf;
}

/**
 *  @brief This function frees a mlan_buffer.
 *
 *  @param pmadapter  Pointer to mlan_adapter
 *  @param pmbuf      Pointer to mlan_buffer
 *
 *  @return           N/A
 */
t_void wlan_free_mlan_buffer(mlan_adapter *pmadapter, pmlan_buffer pmbuf)
{
    return;
}

/**
 *  @brief Add Extended Capabilities IE
 *
 *  @param pmpriv             A pointer to mlan_private structure
 *  @param pptlv_out          A pointer to TLV to fill in
 *
 *  @return                   N/A
 */
void wlan_add_ext_capa_info_ie(IN mlan_private *pmpriv, OUT t_u8 **pptlv_out)
{
    MrvlIETypes_ExtCap_t *pext_cap = MNULL;

    ENTER();

    pext_cap = (MrvlIETypes_ExtCap_t *)*pptlv_out;
    memset(pmpriv->adapter, pext_cap, 0, sizeof(MrvlIETypes_ExtCap_t));
    pext_cap->header.type = wlan_cpu_to_le16(EXT_CAPABILITY);
    pext_cap->header.len  = wlan_cpu_to_le16(sizeof(ExtCap_t));
    if (((t_u8)(pmpriv->hotspot_cfg >> 8)) & HOTSPOT_ENABLE_INTERWORKING_IND)
        pext_cap->ext_cap.Interworking = 1;
    if (((t_u8)(pmpriv->hotspot_cfg >> 8)) & HOTSPOT_ENABLE_TDLS_IND)
        pext_cap->ext_cap.TDLSSupport = 1;
    *pptlv_out += sizeof(MrvlIETypes_ExtCap_t);

    LEAVE();
}

/**
 *  @brief Get rate index
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_rate_ioctl_get_rate_index(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret      = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];

    ENTER();

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_TX_RATE_CFG, HostCmd_ACT_GEN_GET, 0, (t_void *)pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Set rate index
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_rate_ioctl_set_rate_index(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    t_s32 rate_index;
    t_u32 rate_format;
    t_u32 i;
    mlan_ds_rate *ds_rate = MNULL;
    mlan_status ret       = MLAN_STATUS_FAILURE;
    mlan_private *pmpriv  = pmadapter->priv[pioctl_req->bss_index];
    t_u16 bitmap_rates[MAX_BITMAP_RATES_SIZE];

    ENTER();

    ds_rate = (mlan_ds_rate *)pioctl_req->pbuf;

    rate_format = ds_rate->param.rate_cfg.rate_format;
    rate_index  = ds_rate->param.rate_cfg.rate;

    if (ds_rate->param.rate_cfg.is_rate_auto)
    {
        memset(pmadapter, bitmap_rates, 0, sizeof(bitmap_rates));
        /* Rates talbe [0]: HR/DSSS;[1]: OFDM; [2..9] HT; */
        /* Support all HR/DSSS rates */
        bitmap_rates[0] = 0x000F;
        /* Support all OFDM rates */
        bitmap_rates[1] = 0x00FF;
        /* Support all HT-MCSs rate */
        for (i = 2; i < 9; i++)
            bitmap_rates[i] = 0xFFFF;
        bitmap_rates[9] = 0x3FFF;
    }
    else
    {
        PRINTM(MINFO, "Rate index is %d\n", rate_index);

        memset(pmadapter, bitmap_rates, 0, sizeof(bitmap_rates));
        if (rate_format == MLAN_RATE_FORMAT_LG)
        {
            /* Bitmap of HR/DSSS rates */
            if (rate_index <= MLAN_RATE_INDEX_HRDSSS3)
            {
                bitmap_rates[0] = 1 << rate_index;
                ret             = MLAN_STATUS_SUCCESS;
                /* Bitmap of OFDM rates */
            }
            else if ((rate_index >= MLAN_RATE_INDEX_OFDM0) && (rate_index <= MLAN_RATE_INDEX_OFDM7))
            {
                bitmap_rates[1] = 1 << (rate_index - MLAN_RATE_INDEX_OFDM0);
                ret             = MLAN_STATUS_SUCCESS;
            }
        }
        else if (rate_format == MLAN_RATE_FORMAT_HT)
        {
            if (rate_index <= MLAN_RATE_INDEX_MCS32)
            {
#ifdef SD8801
                rate_index -= MLAN_RATE_INDEX_MCS0;
#endif
                bitmap_rates[2 + (rate_index / 16)] = 1 << (rate_index % 16);
                ret                                 = MLAN_STATUS_SUCCESS;
            }
        }

        if (ret == MLAN_STATUS_FAILURE)
        {
            PRINTM(MERROR, "Invalid MCS index=%d. \n", rate_index);
            pioctl_req->status_code = MLAN_ERROR_INVALID_PARAMETER;
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
    }

    PRINTM(MINFO,
           "RateBitmap=%04x%04x%04x%04x%04x%04x%04x%04x"
           "%04x%04x%04x%04x%04x%04x%04x%04x%04x%04x, "
           "IsRateAuto=%d, DataRate=%d\n",
           bitmap_rates[17], bitmap_rates[16], bitmap_rates[15], bitmap_rates[14], bitmap_rates[13], bitmap_rates[12],
           bitmap_rates[11], bitmap_rates[10], bitmap_rates[9], bitmap_rates[8], bitmap_rates[7], bitmap_rates[6],
           bitmap_rates[5], bitmap_rates[4], bitmap_rates[3], bitmap_rates[2], bitmap_rates[1], bitmap_rates[0],
           pmpriv->is_data_rate_auto, pmpriv->data_rate);

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_TX_RATE_CFG, HostCmd_ACT_GEN_SET, 0, (t_void *)pioctl_req,
                           (t_void *)bitmap_rates);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Rate configuration command handler
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise fail
 */
mlan_status wlan_rate_ioctl_cfg(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status status = MLAN_STATUS_SUCCESS;

    ENTER();

    if (pioctl_req->action == MLAN_ACT_GET)
        status = wlan_rate_ioctl_get_rate_index(pmadapter, pioctl_req);
    else
        status = wlan_rate_ioctl_set_rate_index(pmadapter, pioctl_req);

    LEAVE();
    return status;
}

/**
 *  @brief This function prepares command of rf_antenna.
 *
 *  @param pmpriv   A pointer to mlan_private structure
 *  @param cmd      A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status wlan_cmd_802_11_rf_antenna(IN pmlan_private pmpriv,
                                       IN HostCmd_DS_COMMAND *cmd,
                                       IN t_u16 cmd_action,
                                       IN t_void *pdata_buf)
{
    HostCmd_DS_802_11_RF_ANTENNA *pantenna = &cmd->params.antenna;
    mlan_ds_ant_cfg_1x1 *ant_cfg_1x1       = (mlan_ds_ant_cfg_1x1 *)pdata_buf;

    ENTER();
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_RF_ANTENNA);
    cmd->size    = wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_RF_ANTENNA) + S_DS_GEN);

    if (cmd_action == HostCmd_ACT_GEN_SET)
    {
        pantenna->action        = wlan_cpu_to_le16(HostCmd_ACT_SET_BOTH);
        pantenna->antenna_mode  = wlan_cpu_to_le16((t_u16)ant_cfg_1x1->antenna);
        pantenna->evaluate_time = wlan_cpu_to_le16((t_u16)ant_cfg_1x1->evaluate_time);
    }
    else
    {
        pantenna->action = wlan_cpu_to_le16(HostCmd_ACT_GET_BOTH);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifdef WLAN_LOW_POWER_ENABLE
/**
 *  @brief Set/Get Low Power Mode
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param pioctl_req   A pointer to ioctl request buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status wlan_misc_ioctl_low_pwr_mode(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret        = MLAN_STATUS_SUCCESS;
    mlan_ds_misc_cfg *misc = MNULL;
    mlan_private *pmpriv   = pmadapter->priv[pioctl_req->bss_index];

    ENTER();

    misc = (mlan_ds_misc_cfg *)pioctl_req->pbuf;

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_LOW_POWER_MODE, HostCmd_ACT_GEN_SET, 0, (t_void *)pioctl_req,
                           &misc->param.low_pwr_mode);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}
#endif // WLAN_LOW_POWER_ENABLE
