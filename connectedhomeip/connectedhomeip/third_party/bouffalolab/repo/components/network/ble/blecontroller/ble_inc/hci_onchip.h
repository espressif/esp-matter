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
#ifndef HCI_ONCHIP_H_
#define HCI_ONCHIP_H_

enum{
    BT_HCI_CMD,
    BT_HCI_ACL_DATA,
    BT_HCI_CMD_CMP_EVT,
    BT_HCI_CMD_STAT_EVT,
    BT_HCI_LE_EVT,
    BT_HCI_EVT
};

typedef struct{
    uint16_t opcode;
    uint8_t *params;
    uint8_t param_len;
}bl_hci_cmd_struct;

typedef struct {
    /// connection handle
    uint16_t    conhdl;
    /// broadcast and packet boundary flag
    uint8_t     pb_bc_flag;
    /// length of the data
    uint16_t    len;
    uint8_t* buffer;
}bl_hci_acl_data_tx;

typedef struct{
    union{
        bl_hci_cmd_struct hci_cmd;
        bl_hci_acl_data_tx acl_data;
    }p;
}hci_pkt_struct;

typedef void (*bt_hci_recv_cb)(uint8_t pkt_type, uint16_t src_id, uint8_t *param, uint8_t param_len);

uint8_t bt_onchiphci_interface_init(bt_hci_recv_cb cb);
int8_t bt_onchiphci_send(uint8_t pkt_type, uint16_t dest_id, hci_pkt_struct *pkt);
uint8_t bt_onchiphci_hanlde_rx_acl(void *param, uint8_t *host_buf_data);

#endif
