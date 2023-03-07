/** @file mlan_uap_ioctl.c
 *
 *  @brief  This file provides handling of AP mode ioctls
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
    02/05/2009: initial version
********************************************************/

#include <mlan_api.h>

/* Additional WMSDK header files */
#include <wmerrno.h>
#include <wm_os.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>

/**
 *  @brief Set/Get deauth control.
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
mlan_status wlan_uap_snmp_mib_ctrl_deauth(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv  = pmadapter->priv[pioctl_req->bss_index];
    mlan_status ret       = MLAN_STATUS_SUCCESS;
    mlan_ds_snmp_mib *mib = (mlan_ds_snmp_mib *)pioctl_req->pbuf;
    t_u16 cmd_action      = 0;

    ENTER();

    mib = (mlan_ds_snmp_mib *)pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_SET)
    {
        cmd_action = HostCmd_ACT_GEN_SET;
    }
    else
    {
        cmd_action = HostCmd_ACT_GEN_GET;
    }

    /* Send command to firmware */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_SNMP_MIB, cmd_action, StopDeauth_i, (t_void *)pioctl_req,
                           &mib->param.deauthctrl);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief MLAN uap ioctl handler
 *
 *  @param adapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status wlan_ops_uap_ioctl(t_void *adapter, pmlan_ioctl_req pioctl_req)
{
    mlan_status status = MLAN_STATUS_SUCCESS;

    ENTER();

    switch (pioctl_req->req_id)
    {
        default:
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
            break;
    }
    LEAVE();
    return status;
}
