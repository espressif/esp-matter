
/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _IPC_SHARED_H_
#define _IPC_SHARED_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <bl60x_fw_api.h>
#include "ipc_compat.h"
#include "lmac_types.h"
#include "lmac_mac.h"

/*
 * DEFINES AND MACROS
 ****************************************************************************************
 */
#define CO_BIT(pos) (1U<<(pos)) 
#define NX_TXQ_CNT          4
#define CONFIG_USER_MAX     1

#define IPC_TXQUEUE_CNT     NX_TXQ_CNT
#define NX_TXDESC_CNT0      CFG_TXDESC
#define NX_TXDESC_CNT1      CFG_TXDESC
#define NX_TXDESC_CNT2      CFG_TXDESC
#define NX_TXDESC_CNT3      CFG_TXDESC
#define NX_TXDESC_CNT4      CFG_TXDESC
/*
 * Number of Host buffers available for Data Rx handling (through DMA)
 */
#define IPC_RXBUF_CNT       2

/*
 * Number of shared descriptors available for Data RX handling
 */
#define IPC_RXDESC_CNT      2

/*
 * Number of Host buffers available for Radar events handling (through DMA)
 */
#define IPC_RADARBUF_CNT       4

/*
 * RX Data buffers size (in bytes)
 */
#define IPC_RXBUF_SIZE 2048

/*
 * Number of Host buffers available for Emb->App MSGs sending (through DMA)
 */
#define IPC_MSGE2A_BUF_CNT       8

/*
 * Number of Host buffers available for Debug Messages sending (through DMA)
 */
#define IPC_DBGBUF_CNT       4

/*
 * Length used in MSGs structures
 */
#define IPC_A2E_MSG_BUF_SIZE    127 // size in 4-byte words
#define IPC_E2A_MSG_PARAM_SIZE   (257 - 4 - 8)// size in 4-byte words
/*
 * Debug messages buffers size (in bytes)
 */
#define IPC_DBG_PARAM_SIZE       256

/*
 * Define used for Rx hostbuf validity.
 * This value should appear only when hostbuf was used for a Reception.
 */
#define RX_DMA_OVER_PATTERN 0xAAAAAA00

/*
 * Define used for MSG buffers validity.
 * This value will be written only when a MSG buffer is used for sending from Emb to App.
 */
#define IPC_MSGE2A_VALID_PATTERN 0xADDEDE2A

/*
 * Define used for Debug messages buffers validity.
 * This value will be written only when a DBG buffer is used for sending from Emb to App.
 */
#define IPC_DBG_VALID_PATTERN 0x000CACA0

/*
 *  Length of the receive vectors, in bytes
 */
#define DMA_HDR_PHYVECT_LEN    36

/*
 * Maximum number of payload addresses and lengths present in the descriptor
 */
#define NX_TX_PAYLOAD_MAX      6

/*
 ****************************************************************************************
 */
// c.f LMAC/src/tx/tx_swdesc.h
/// Descriptor filled by the Host
struct hostdesc
{
    /// allocated pbuf
    uint32_t pbuf_addr;
    /// Pointer to packet payload
    uint32_t packet_addr;
    /// Size of the payload
    uint16_t packet_len;

    /// Address of the status descriptor in host memory (used for confirmation upload)
    uint32_t status_addr;
    /// Destination Address
    struct mac_addr eth_dest_addr;
    /// Source Address
    struct mac_addr eth_src_addr;
    /// Ethernet Type
    uint16_t ethertype;
    /// Buffer containing the PN to be used for this packet
    uint16_t pn[4];
    /// Sequence Number used for transmission of this MPDU
    uint16_t sn;
    /// Timestamp of first transmission of this MPDU
    uint16_t timestamp;
    /// Packet TID (0xFF if not a QoS frame)
    uint8_t tid;
    /// Interface Id
    uint8_t vif_idx;
    /// Station Id (0xFF if station is unknown)
    uint8_t staid;
    /// TX flags
    uint16_t flags;
    uint32_t pbuf_chained_ptr[4]; //max 4 chained pbuf for one output ethernet packet
    uint32_t pbuf_chained_len[4]; //max 4 chained pbuf for one output ethernet packet
};

struct txdesc_host
{
    uint32_t ready;

#if defined(CFG_CHIP_BL808)
    uint32_t eth_packet[1600/4];
#endif

#if defined(CFG_CHIP_BL606P)
    uint32_t eth_packet[1600/4];
#endif

    /// API of the embedded part
    struct hostdesc host;

    uint32_t pad_txdesc[204/4];

    uint32_t pad_buf[400/4];
};

/// Structure containing the information about the PHY channel that is used
struct phy_channel_info
{
    /// PHY channel information 1
    uint32_t info1;
    /// PHY channel information 2
    uint32_t info2;
};

/**
 ****************************************************************************************
 *  @defgroup IPC_MISC IPC Misc
 *  @ingroup IPC
 *  @brief IPC miscellaneous functions
 ****************************************************************************************
 */
/// Message structure for MSGs from Emb to App
struct ipc_e2a_msg
{
    ke_msg_id_t id;                ///< Message id.
    ke_task_id_t dummy_dest_id;                ///<
    ke_task_id_t dummy_src_id;                ///<
    uint32_t param_len;         ///< Parameter embedded struct length.
    uint32_t param[IPC_E2A_MSG_PARAM_SIZE];  ///< Parameter embedded struct. Must be word-aligned.
    uint32_t pattern;           ///< Used to stamp a valid MSG buffer
};

/// Message structure for MSGs from App to Emb.
/// Actually a sub-structure will be used when filling the messages.
struct ipc_a2e_msg
{
    uint32_t dummy_word;                // used to cope with kernel message structure
    uint32_t msg[IPC_A2E_MSG_BUF_SIZE]; // body of the msg
};

// Indexes are defined in the MIB shared structure

struct ipc_shared_env_tag
{
    volatile struct ipc_a2e_msg msg_a2e_buf; // room for MSG to be sent from App to Emb

    /// Host buffer address for the TX payload descriptor pattern
    volatile uint32_t  pattern_addr;

    volatile struct txdesc_host txdesc0[NX_TXDESC_CNT0];
};

extern struct ipc_shared_env_tag ipc_shared_env;


/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */

/// IPC TX descriptor interrupt mask
#define IPC_IRQ_A2E_TXDESC          0xFF00

#define IPC_IRQ_A2E_TXDESC_FIRSTBIT (8)
#define IPC_IRQ_A2E_RXBUF_BACK      CO_BIT(5)
#define IPC_IRQ_A2E_RXDESC_BACK     CO_BIT(4)

#define IPC_IRQ_A2E_MSG             CO_BIT(1)
#define IPC_IRQ_A2E_DBG             CO_BIT(0)

#define IPC_IRQ_A2E_ALL             (IPC_IRQ_A2E_TXDESC|IPC_IRQ_A2E_MSG|IPC_IRQ_A2E_DBG)

// IRQs from emb to app
#define IPC_IRQ_E2A_TXCFM_POS   7

#define IPC_IRQ_E2A_TXCFM       ((1 << NX_TXQ_CNT) - 1 ) << IPC_IRQ_E2A_TXCFM_POS

#define IPC_IRQ_E2A_RADAR       CO_BIT(6)
#define IPC_IRQ_E2A_TBTT_SEC    CO_BIT(5)
#define IPC_IRQ_E2A_TBTT_PRIM   CO_BIT(4)
#define IPC_IRQ_E2A_RXDESC      CO_BIT(3)
#define IPC_IRQ_E2A_MSG_ACK     CO_BIT(2)
#define IPC_IRQ_E2A_MSG         CO_BIT(1)
#define IPC_IRQ_E2A_DBG         CO_BIT(0)

#define IPC_IRQ_E2A_ALL         ( IPC_IRQ_E2A_TXCFM         \
                                | IPC_IRQ_E2A_RXDESC        \
                                | IPC_IRQ_E2A_MSG_ACK       \
                                | IPC_IRQ_E2A_MSG           \
                                | IPC_IRQ_E2A_DBG           \
                                | IPC_IRQ_E2A_TBTT_PRIM     \
                                | IPC_IRQ_E2A_TBTT_SEC      \
                                | IPC_IRQ_E2A_RADAR)
#endif // _IPC_SHARED_H_
