/********************************************************************************************************
 * @file	ll_slave.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2020
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
#include "conn_stack.h"

#ifndef LL_SLAVE_H_
#define LL_SLAVE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      for user to initialize ACL connection slave role.
 * @param	   none
 * @return     none
 */
void 		blc_ll_initSlaveRole_module(void);


/**
 * @brief      for user to terminate an existing connection slave role.
 * @param[in]  reason - indicates the reason for ending the connection
 * @return     status, 0x00:  succeed
 * 			           other: failed
 */
ble_sts_t  	bls_ll_terminateConnection (u8 reason);


/**
 * @brief      for user to read current slave connection  interval
 * @param	   none
 * @return     0    :  LinkLayer not in connection state
 * 			   other:  connection interval, unit: 1.25mS
 */
u16			bls_ll_getConnectionInterval(void);


/**
 * @brief      for user to read current slave connection latency
 * @param	   none
 * @return     0    :  LinkLayer not in connection state
 * 			   other:  connection latency
 */
u16			bls_ll_getConnectionLatency(void);


/**
 * @brief      for user to read current slave connection supervision timeout
 * @param	   none
 * @return     0    :  LinkLayer not in connection state
 * 			   other:  connection supervision timeout, unit: 10 mS
 */
u16			bls_ll_getConnectionTimeout(void);

/**
 * @brief     for user to send LL_VERSION_IND.
 * @param     connHandle: BLS_CONN_HANDLE indicate slave role;
 * @return    status, 0x00 : succeed
 * 					  other: failed
 */
ble_sts_t	bls_ll_readRemoteVersion(u16 connHandle);

/**
 * @brief      used to set telink defined event mask for BLE module only.
 * @param[in]  evtMask - event mask
 * @return     status, 0x00:  succeed
 * 			           other: failed
 */
ble_sts_t 	bls_hci_mod_setEventMask_cmd(u32 evtMask);


#if(MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
	int			bls_ll_requestConnBrxEventDisable(void);
	void		bls_ll_disableConnBrxEvent(void);
	void		bls_ll_restoreConnBrxEvent(void);
#endif

#if (MCU_CORE_TYPE == MCU_CORE_9518)
	#define blc_ll_disconnect(conn, reason)     	bls_ll_terminateConnection(reason)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LL_SLAVE_H_ */
