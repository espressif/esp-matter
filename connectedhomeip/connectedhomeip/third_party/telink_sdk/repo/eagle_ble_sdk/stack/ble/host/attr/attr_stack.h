/********************************************************************************************************
 * @file	attr_stack.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2020
 *
 * @par		Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd.
 *			All rights reserved.
 *
 *          The information contained herein is confidential property of Telink
 *          Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *          of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *          Co., Ltd. and the licensee or the terms described here-in. This heading
 *          MUST NOT be removed from this file.
 *
 *          Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
 *          alter) any information contained herein in whole or in part except as expressly authorized
 *          by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
 *          for any claim to the extent arising out of or relating to such deletion(s), modification(s)
 *          or alteration(s).
 *
 *          Licensees are granted free, non-transferable use of the information in this
 *          file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#ifndef STACK_BLE_ATTR_ATTR_STACK_H_
#define STACK_BLE_ATTR_ATTR_STACK_H_

#define ATT_MTU_SIZE                        23  //!< Minimum ATT MTU size

//TELINK MTU no longer than 256, so 1 byte is enough
typedef struct{
	u16 init_MTU;
	u16 effective_MTU;

	u8 *pPendingPkt;

	u8 Data_pending_time;    //10ms unit
	u8 mtu_exchange_pending;
}att_para_t;

extern att_para_t bltAtt;
extern u16	blt_indicate_handle;
extern u32 att_service_discover_tick;

#ifdef __cplusplus
extern "C" {
#endif

u8 *l2cap_att_handler(u16 connHandle, u8 * p);
u8 	blc_gatt_requestServiceAccess(u16 connHandle, int gatt_perm);

#if (MCU_CORE_TYPE == MCU_CORE_9518)
int			blt_att_sendMtuRequest (u16 connHandle);
#endif

#ifdef __cplusplus
}
#endif

#endif /* STACK_BLE_ATTR_ATTR_STACK_H_ */
