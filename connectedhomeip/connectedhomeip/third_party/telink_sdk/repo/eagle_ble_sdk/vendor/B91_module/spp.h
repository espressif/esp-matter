/********************************************************************************************************
 * @file	spp.h
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


#ifndef SPP_H_
#define SPP_H_


#define SPP_CMD_SET_ADV_INTV								0xFF01
#define SPP_CMD_SET_ADV_DATA								0xFF02

#define SPP_CMD_SET_ADV_ENABLE                             	0xFF0A
#define SPP_CMD_GET_BUF_SIZE								0xFF0C
#define SPP_CMD_SET_ADV_TYPE								0xFF0D
#define SPP_CMD_SET_ADV_ADDR_TYPE							0xFF0E
#define SPP_CMD_ADD_WHITE_LST_ENTRY							0xFF0F
#define SPP_CMD_DEL_WHITE_LST_ENTRY							0xFF10
#define SPP_CMD_RST_WHITE_LST								0xFF11
#define SPP_CMD_SET_FLT_POLICY								0xFF12
#define SPP_CMD_SET_DEV_NAME								0xFF13
#define SPP_CMD_GET_CONN_PARA								0xFF14
#define SPP_CMD_SET_CONN_PARA								0xFF15
#define SPP_CMD_GET_CUR_STATE								0xFF16
#define SPP_CMD_TERMINATE									0xFF17
#define SPP_CMD_RESTART_MOD									0xFF18
#define SPP_CMD_SET_ADV_DIRECT_ADDR							0xFF19
#define SPP_CMD_SEND_NOTIFY_DATA                            0xFF1C



typedef struct {
	u16 cmdId;
	u16 paramLen;
	u8  param[0];
} spp_cmd_t;


typedef struct {
	u8	token;
	u8  paramLen;
	u16 eventId;
	u8  param[0];
} spp_event_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief		this function is used to process rx uart data to remote device.
 * @param[in]   p - data pointer
 * @param[in]   n - data length
 * @return      0 is ok
 */
int bls_uart_handler (u8 *p, int n);
/**
 * @brief		this function is used to process tx uart data to remote device.
 * @param[in]   header - hci event type
 * @param[in]   pEvent - event data
 * @return      0 is ok
 */
int spp_send_data (u32 header, spp_event_t * pEvt);
/**
 * @brief		this function is used to restart module.
 * @param[in]	none
 * @return      none
 */
void spp_restart_proc(void);

#ifdef __cplusplus
}
#endif

#endif /* SPP_H_ */
