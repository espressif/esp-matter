/********************************************************************************************************
 * @file	log_def_stack.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#ifndef	_LOG_DEF_STACK_H_
#define _LOG_DEF_STACK_H_

//	event: 0 for time stamp; 1 reserved; eid2 - eid31
#define			SLEV_timestamp				0
#define			SLEV_reserved				1
#define			SLEV_slot					2
#define			SLEV_btrx					3
#define			SLEV_set_clkn_fcnt   		4
#define			SLEV_rx_lmp   				5
#define			SLEV_rx_acl   				6
#define			SLEV_tx_tgl					7
#define			SLEV_host_conn_req			8
#define			SLEV_set_em					9
#define			SLEV_rx_end					10
#define			SLEV_tx_lmp					11
#define			SLEV_tx_acl					12
#define			SLEV_tx_lmp_full			13
#define			SLEV_tx_acl_full			14
#define			SLEV_tx_hci_full			15
#define			SLEV_sniff_slot_start		16
#define			SLEV_sniff_sub_slot_update	17
#define			SLEV_sniff_sub_start		18
#define			SLEV_sniff_transition		19
#define			SLEV_name_req				20
#define			SLEV_inq_timeout			21
#define			SLEV_page_timeout			22
#define			SLEV_pageresp_timeout		23
#define			SLEV_newconnect_timeout		24

// 1-bit data: 0/1 for hardware signal: PA4/PB1; bid2 - bid31
#define			SL01_TX_PA4					0
#define			SL01_RXSYNC_PB1				1
#define			SL01_ac_inq					2
#define			SL01_ac_inq_scan			3
#define			SL01_ac_page				4
#define			SL01_ac_page_scan			5
#define			SL01_ac_m_connect			6
#define			SL01_ac_s_connect			7

#define			SL01_task_system_tick   	8
#define			SL01_task_rx   				9
#define			SL01_tx_tgl_level   		10
#define			SL01_task_bt_ll_main   		11
#define			SL01_task_test   			12
#define			SL01_detach_timeout			13
// 8-bit data: cid0 - cid63
#define			SL08_ac_state				0
#define			SL08_state_pending_slot		1
#define			SL08_rx_lmp_code			2
#define			SL08_rx_poll_fcnt			3
#define			SL08_rx_poll_offset			4
#define			SL08_rx_poll_rxbit			5
#define			SL08_lmp_esc4				6
#define			SL08_acl_tx_type    		7

#define			SL08_hci_wptr    			8
#define			SL08_hci_rptr    			9
#define			SL08_valid_range			10
// 16-bit data: sid0 - sid63
#define			SL16_CLKN					0
#define			SL16_FCNT					1
#define			SL16_MCLK					2
#define			SL16_MCLKP					3
#define			SL16_RXBIT					4
#define			SL16_RxStat					5
#define			SL16_LNKCNTL				6
#define			SL16_acl_tx_size			7
#define			SL16_rx_acl_cid				8
#define			SL16_sniff_slot				9
#define			SL16_sniff_win				10
#define			SL16_sub_instant			11
#endif
