/********************************************************************************************************
 * @file	gap_stack.h
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
#ifndef STACK_BLE_GAP_STACK_H_
#define STACK_BLE_GAP_STACK_H_





typedef int (*host_ota_mian_loop_callback_t)(void);
typedef int (*host_ota_terminate_callback_t)(u16);
extern host_ota_mian_loop_callback_t    		host_ota_main_loop_cb;
extern host_ota_terminate_callback_t    		host_ota_terminate_cb;







/******************************* gap event start ************************************************************************/

extern u32		gap_eventMask;


#ifdef __cplusplus
extern "C" {
#endif

int blc_gap_send_event (u32 h, u8 *para, int n);

/******************************* gap event  end ************************************************************************/



/******************************* gap start ************************************************************************/



/**
 * @brief      this  function is used to initialize GAP Central
 * @param	   none
 * @return     none
 */
void 		blc_gap_central_init(void);



/******************************* gap end ************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GAP_STACK_H_ */
