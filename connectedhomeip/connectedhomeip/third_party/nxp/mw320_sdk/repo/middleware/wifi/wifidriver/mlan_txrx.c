/** @file mlan_txrx.c
 *
 *  @brief  This file provides the handling of TX/RX in MLAN
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

/********************************************************
                Global Functions
********************************************************/
/**
 *   @brief This function processes the received buffer
 *
 *   @param pmadapter A pointer to mlan_adapter
 *   @param pmbuf     A pointer to the received buffer
 *
 *   @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_handle_rx_packet(pmlan_adapter pmadapter, pmlan_buffer pmbuf)
{
    mlan_status ret    = MLAN_STATUS_SUCCESS;
    pmlan_private priv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
    RxPD *prx_pd;
#ifdef DEBUG_LEVEL1
    t_u32 sec, usec;
#endif

    ENTER();

    prx_pd = (RxPD *)(pmbuf->pbuf + pmbuf->data_offset);
    /* Get the BSS number from RxPD, get corresponding priv */
    priv = wlan_get_priv_by_id(pmadapter, prx_pd->bss_num & BSS_NUM_MASK, prx_pd->bss_type);
    if (!priv)
        priv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
    pmbuf->bss_index = priv->bss_index;
    PRINTM_GET_SYS_TIME(MDATA, &sec, &usec);
    PRINTM_NETINTF(MDATA, priv);
    /* PRINTM(MDATA, "%lu.%06lu : Data <= FW\n", sec, usec); */
    ret = priv->ops.process_rx_packet(pmadapter, pmbuf);

    LEAVE();
    return ret;
}

/**
 *  @brief This function checks the conditions and sends packet to device
 *
 *  @param priv	   A pointer to mlan_private structure
 *  @param pmbuf   A pointer to the mlan_buffer for process
 *  @param tx_param A pointer to mlan_tx_param structure
 *
 *  @return 	    MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise failure
 */
