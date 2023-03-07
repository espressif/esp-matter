/** @file mlan_11n_aggr.c
 *
 *  @brief  This file provides contains functions for 11n Aggregation
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
    11/10/2008: initial version
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

/********************************************************
    Global Variables
********************************************************/

/********************************************************
    Local Functions
********************************************************/

void wrapper_deliver_amsdu_subframe(pmlan_buffer amsdu_pmbuf, t_u8 *data, t_u16 pkt_len);

/**
 *  @brief Get number of aggregated packets
 *
 *  @param data			A pointer to packet data
 *  @param total_pkt_len	Total packet length
 *
 *  @return			Number of packets
 */
static int wlan_11n_get_num_aggrpkts(t_u8 *data, int total_pkt_len)
{
    int pkt_count = 0, pkt_len, pad;

    ENTER();
    while (total_pkt_len > 0)
    {
        /* Length will be in network format, change it to host */
        pkt_len = mlan_ntohs((*(t_u16 *)(data + (2 * MLAN_MAC_ADDR_LENGTH))));
        pad     = (((pkt_len + sizeof(Eth803Hdr_t)) & 3)) ? (4 - ((pkt_len + sizeof(Eth803Hdr_t)) & 3)) : 0;
        data += pkt_len + pad + sizeof(Eth803Hdr_t);
        total_pkt_len -= pkt_len + pad + sizeof(Eth803Hdr_t);
        ++pkt_count;
    }
    LEAVE();
    return pkt_count;
}

/********************************************************
    Global Functions
********************************************************/

/**
 *  @brief Deaggregate the received AMSDU packet
 *
 *  @param priv		A pointer to mlan_private structure
 *  @param pmbuf	A pointer to aggregated data packet
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status wlan_11n_deaggregate_pkt(mlan_private *priv, pmlan_buffer pmbuf)
{
    t_u16 pkt_len;
    int total_pkt_len;
    t_u8 *data;
    int pad;
    mlan_status ret = MLAN_STATUS_FAILURE;
    RxPacketHdr_t *prx_pkt;
    mlan_buffer *daggr_mbuf = MNULL;
    /* mlan_adapter *pmadapter = priv->adapter; */
    t_u8 rfc1042_eth_hdr[MLAN_MAC_ADDR_LENGTH] = {0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00};

    ENTER();

    data          = (t_u8 *)(pmbuf->pbuf + pmbuf->data_offset);
    total_pkt_len = pmbuf->data_len;

    /* Sanity test */
    if (total_pkt_len > MLAN_RX_DATA_BUF_SIZE)
    {
        PRINTM(MERROR,
               "Total packet length greater than tx buffer"
               " size %d\n",
               total_pkt_len);
        goto done;
    }

    pmbuf->use_count = wlan_11n_get_num_aggrpkts(data, total_pkt_len);

    while (total_pkt_len > 0)
    {
        prx_pkt = (RxPacketHdr_t *)data;
        /* Length will be in network format, change it to host */
        pkt_len = mlan_ntohs((*(t_u16 *)(data + (2 * MLAN_MAC_ADDR_LENGTH))));
        if (pkt_len > total_pkt_len)
        {
            PRINTM(MERROR, "Error in packet length: total_pkt_len = %d, pkt_len = %d\n", total_pkt_len, pkt_len);
            break;
        }

        pad = (((pkt_len + sizeof(Eth803Hdr_t)) & 3)) ? (4 - ((pkt_len + sizeof(Eth803Hdr_t)) & 3)) : 0;

        total_pkt_len -= pkt_len + pad + sizeof(Eth803Hdr_t);

        if (memcmp(pmadapter, &prx_pkt->rfc1042_hdr, rfc1042_eth_hdr, sizeof(rfc1042_eth_hdr)) == 0)
        {
            memmove(pmadapter, data + LLC_SNAP_LEN, data, (2 * MLAN_MAC_ADDR_LENGTH));
            data += LLC_SNAP_LEN;
            pkt_len += sizeof(Eth803Hdr_t) - LLC_SNAP_LEN;
        }
        else
        {
            *(t_u16 *)(data + (2 * MLAN_MAC_ADDR_LENGTH)) = (t_u16)0;
            pkt_len += sizeof(Eth803Hdr_t);
        }

        /* This part is customized for WMSDK. We do not need and will not
           allocate the mlan buffer. */
        wrapper_deliver_amsdu_subframe(pmbuf, data, pkt_len);
        ret = MLAN_STATUS_SUCCESS;

        switch (ret)
        {
            case MLAN_STATUS_PENDING:
                break;
            case MLAN_STATUS_FAILURE:
                PRINTM(MERROR, "Deaggr, send to moal failed\n");
                daggr_mbuf->status_code = MLAN_ERROR_PKT_INVALID;
            case MLAN_STATUS_SUCCESS:
                break;
            default:
                break;
        }

        data += pkt_len + pad;
    }

done:
    LEAVE();
    return ret;
}
