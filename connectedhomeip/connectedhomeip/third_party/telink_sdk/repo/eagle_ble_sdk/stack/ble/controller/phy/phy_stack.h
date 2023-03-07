/********************************************************************************************************
 * @file	phy_stack.h
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
#ifndef PHY_STACK_H_
#define PHY_STACK_H_


#include "tl_common.h"
#include "stack/ble/hci/hci_cmd.h"


/******************************* phy start *************************************************************************/


/********************* Macro & Enumeration variables for Stack, user can not use!!!!  **********************/
#define PHY_USED_AUXPTR_LE_1M									0
#define PHY_USED_AUXPTR_LE_2M									1
#define PHY_USED_AUXPTR_LE_CODED								2



typedef struct{
	u8	llid;
	u8  rf_len;
	u8	opcode;
	u8	tx_phys;
	u8	rx_phys;
}rf_pkt_ll_phy_req_rsp_t;   //phy_req, phy_rsp


typedef struct{
	u8	llid;
	u8  rf_len;
	u8	opcode;
	u8	m_to_s_phy;
	u8	s_to_m_phy;
	u8 	instant0;
	u8 	instant1;
}rf_pkt_ll_phy_update_ind_t;   //phy_req, phy_rsp



typedef enum{
	LE_CODED_S2 = 2,
	LE_CODED_S8 = 8,
}le_coding_ind_t;



typedef struct {
	u8	dft_tx_prefer_phys;
	u8 	dft_rx_prefer_phys;
	u8	dft_prefer_phy;
	u8	dft_CI;

	//for Extended ADV
	u8	cur_llPhy;
	u8	cur_CI;  //cur_coding_ind
	u8	rsvd;
	u8	oneByte_us; //1M: 8uS;  2M: 4uS; Coded S2: 16uS; Coded S8: 64uS

	/* preamble + access_code:  1M: 5*8=40uS;  2M: 6*4=24uS;  Coded: 80+256=336uS */
	/* AD convert delay : timing cost on RF analog to digital convert signal process:
	 * 					Eagle	1M: 20uS	   2M: 10uS;      500K(S2): 14uS    125K(S8):  14uS
	 *					Jaguar	1M: 20uS	   2M: 10uS;      500K(S2): 14uS    125K(S8):  14uS
	 *					data is come from Xuqiang.Zhang
	 *
	 *	prmb_ac_us + convert delay:
	 *	         1M: 40 + 20 = 60 uS
	 *	         2M: 24 + 10 = 34 uS
	 *	      Coded: 336 + 14 = 350 uS
	 * */
	u16 prmb_ac_us; //
}ll_phy_t;
extern _attribute_aligned_(4) ll_phy_t		bltPHYs;



//do not support Asymmetric PHYs, conn_phys = tx_phys & rx_phys
typedef struct {
	u8	conn_prefer_phys;  // conn_prefer_phys = tx_prefer_phys & rx_prefer_phys
	u8	conn_cur_phy;	   //
	u8	conn_next_phy;	   //
	u8	conn_cur_CI;	   // CI: coding_ind

	u8	conn_next_CI;
	u8	phy_req_trigger;  // 1: means current device triggers phy_req, due to API "blc_ll_setPhy" called by Host or Application
	u8	phy_req_pending;
	u8	phy_update_pending;

	u32	conn_update_phy;

}ll_conn_phy_t;




typedef ll_conn_phy_t* (*ll_get_conn_phy_ptr_callback_t)(u16);
extern ll_get_conn_phy_ptr_callback_t  ll_get_conn_phy_ptr_cb;


extern u8   tx_settle_adv[4];
extern u8	tx_settle_slave[4];
extern u8	tx_settle_master[4];


#ifdef __cplusplus
extern "C" {
#endif

int 	blt_reset_conn_phy_param(ll_conn_phy_t* pconn_phy);
void 	rf_ble_switch_phy(le_phy_type_t phy, le_coding_ind_t coding_ind);

typedef void (*ll_phy_switch_callback_t)(le_phy_type_t, le_coding_ind_t);
extern ll_phy_switch_callback_t		ll_phy_switch_cb;

ble_sts_t 	blc_ll_setDefaultPhy(le_phy_prefer_mask_t all_phys, le_phy_prefer_type_t tx_phys, le_phy_prefer_type_t rx_phys);
ble_sts_t	blc_ll_readPhy(u16 connHandle, hci_le_readPhyCmd_retParam_t *);


/**
 * @brief       this function is used to set PHY type for connection
 * @param[in]   *para -  Command Parameters for "7.8.49 LE Set PHY command"
 * @return      0 : success
 * 				other : fail
 */
ble_sts_t blc_hci_le_setPhy(hci_le_setPhyCmd_param_t* para);

/******************************* phy end ***************************************************************************/






/******************************* phy_test start *************************************************************************/
int 	  blt_phyTest_main_loop(void);
int 	  blt_phytest_cmd_handler (u8 *p, int n);

ble_sts_t blt_phyTest_setReceiverTest_V1 (u8 rx_chn);
ble_sts_t blt_phyTest_hci_setTransmitterTest_V1 (u8 tx_chn, u8 length, u8 pkt_type);

ble_sts_t blt_phyTest_hci_setReceiverTest_V2 (u8 rx_chn,u8 phy_mode,u8 modulation_index);
ble_sts_t blt_phyTest_hci_setTransmitterTest_V2 (u8 tx_chn, u8 length, u8 pkt_type,u8 phy_mode);

ble_sts_t blt_phyTest_setTestEnd(u8 *pkt_num);

void blt_InitPhyTestDriver(rf_mode_e rf_mode);

/******************************* phy_test end ***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PHY_STACK_H_ */


