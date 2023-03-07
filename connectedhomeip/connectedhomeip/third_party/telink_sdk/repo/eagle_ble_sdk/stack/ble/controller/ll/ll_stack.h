/********************************************************************************************************
 * @file	ll_stack.h
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
#ifndef STACK_BLE_LL_LL_STACK_H_
#define STACK_BLE_LL_LL_STACK_H_


#include "stack/ble/ble_format.h"
#include "stack/ble/hci/hci_cmd.h"
#include "stack/ble/controller/ll/ll_resolvlist.h"



/******************************* ll start *************************************************************************/
/////////////////////////////////////////////////////////////////////////////
#if (MCU_CORE_TYPE == MCU_CORE_827x)
	//TX settle time
	#define			LL_ADV_TX_SETTLE				81
	#define 		LL_SCAN_TX_SETTLE				62
	#define 		LL_SLAVE_TX_SETTLE				82
	#define         LL_SCANRSP_TX_SETTLE            75//(LL_ADV_TX_SETTLE-6)
	#define 		LL_MASTER_TX_SETTLE				83

	#define			LL_ADV_TX_STL_2M				87
	#define			LL_ADV_TX_STL_CODED				119//multiple code is 121

	#define 		LL_SLAVE_TX_STL_2M				89
	#define 		LL_SLAVE_TX_STL_CODED			121

	#define			LL_MASTER_TX_STL_2M				90
	#define			LL_MASTER_TX_STL_CODED			122

#elif(MCU_CORE_TYPE == MCU_CORE_825x)
	//TX settle time
	#define			LL_ADV_TX_SETTLE				74
	#define 		LL_SCAN_TX_SETTLE				56
	#define 		LL_SLAVE_TX_SETTLE				75
	#define         LL_SCANRSP_TX_SETTLE            68//(LL_ADV_TX_SETTLE-6)
	#define 		LL_MASTER_TX_SETTLE				76

	#define			LL_ADV_TX_STL_2M				84
	#define			LL_ADV_TX_STL_CODED				116

	#define 		LL_SLAVE_TX_STL_2M				86
	#define 		LL_SLAVE_TX_STL_CODED			118

	#define			LL_MASTER_TX_STL_2M				86
	#define			LL_MASTER_TX_STL_CODED			119
#elif(MCU_CORE_TYPE == MCU_CORE_9518)
	#if RF_RX_SHORT_MODE_EN//open rx dly
		//TX settle time
		#define			LL_ADV_TX_SETTLE								84
		#define 		LL_SCAN_TX_SETTLE								63
		#define 		LL_INIT_TX_SETTLE								63
		#define 		LL_SCANRSP_TX_SETTLE							78  //84-6=78
		#define 		LL_SLAVE_TX_SETTLE								86
		#define 		LL_MASTER_TX_SETTLE								87

		#define			LL_ADV_TX_STL_2M								115
		#define			LL_SCANRSP_TX_STL_2M							109 //115-6=109
		#define			LL_ADV_TX_STL_CODED								124
		#define			LL_SCANRSP_TX_STL_CODED							118 //124-6=118

		#define 		LL_SLAVE_TX_STL_2M								117
		#define 		LL_SLAVE_TX_STL_CODED							125

		#define			LL_MASTER_TX_STL_2M								117
		#define			LL_MASTER_TX_STL_CODED							125
	#else// close rx dly
		//TX settle time
		#define			LL_ADV_TX_SETTLE								79
		#define 		LL_SCAN_TX_SETTLE								56
		#define 		LL_SLAVE_TX_SETTLE								80
		#define 		LL_MASTER_TX_SETTLE								81

		#define			LL_ADV_TX_STL_2M								84
		#define			LL_ADV_TX_STL_CODED								116

		#define 		LL_SLAVE_TX_STL_2M								86
		#define 		LL_SLAVE_TX_STL_CODED							118

		#define			LL_MASTER_TX_STL_2M								86
		#define			LL_MASTER_TX_STL_CODED							119
	#endif
#endif
/////////////////////////////////////////////////////////////////////////////
#define			BLE_STATE_ADV									1
#define			BLE_STATE_SCAN									2
#define			BLE_STATE_INIT									3
#define			BLE_STATE_BTX_S									4
#define			BLE_STATE_BTX_E									5
#define			BLE_STATE_BRX_S									6
#define			BLE_STATE_BRX_E									7
#define			BLE_STATE_ADV_IN_SLAVE							8
#define			BLE_STATE_SCAN_IN_SLAVE							9
#define			BLE_STATE_SCAN_IN_ADV							10





#define			EVENT_MASK_TERMINATE							BIT(BLT_EV_FLAG_TERMINATE)
#define			EVENT_MASK_RX_DATA_ABANDOM						BIT(BLT_EV_FLAG_RX_DATA_ABANDOM)
#define			EVENT_MASK_CHN_MAP_REQ							BIT(BLT_EV_FLAG_CHN_MAP_REQ)
#define			EVENT_MASK_CONN_PARA_REQ						BIT(BLT_EV_FLAG_CONN_PARA_REQ)
#define			EVENT_MASK_CHN_MAP_UPDATE						BIT(BLT_EV_FLAG_CHN_MAP_UPDATE)
#define			EVENT_MASK_CONN_PARA_UPDATE						BIT(BLT_EV_FLAG_CONN_PARA_UPDATE)
#define 		EVENT_MASK_PHY_UPDATE							BIT(BLT_EV_FLAG_PHY_UPDATE)
#define 		EVENT_MASK_DATA_LEN_UPDATE						BIT(BLT_EV_FLAG_DATA_LENGTH_EXCHANGE)


#define 		EVENT_MASK_CHN_SELECTION_ALGOTITHM				BIT(31)  //no event, only mask here




#if (MCU_CORE_TYPE == MCU_CORE_9518)
	#define			STOP_RF_STATE_MACHINE						( REG_ADDR8(0x80140a00) = 0x80 )
#elif (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
	#define			STOP_RF_STATE_MACHINE						( REG_ADDR8(0xf00) = 0x80 )
#endif

#define 		ADV_LEGACY_MASK									BIT(0)
#define 		ADV_EXTENDED_MASK								BIT(1)
#define 		SCAN_LEGACY_MASK								BIT(0)
#define 		SCAN_EXTENDED_MASK								BIT(1)








#if (MCU_CORE_TYPE == MCU_CORE_9518)
typedef	struct {
	u16		size;
	u8		rsvd;
	u8		depth;
	u8		num;
	u8		mask;
	u8		wptr;
	u8		rptr;
	u8*		p_default;
	u8*		p_base;
}	ll_fifo_t;

 extern  ll_fifo_t			blt_rxfifo;
 extern	ll_fifo_t			blt_txfifo;
#endif
//////////////////////////////////////


typedef struct {
	u8		macAddress_public[6];
	u8		macAddress_random[6];   //host may set this
}ll_mac_t;

extern ll_mac_t  bltMac;

typedef struct {
	u8		adv_en;
	u8		adv_extension_mask;
	u8		adv_scanReq_connReq;
	u8 		phy_en;


	u8		ll_recentAvgRSSI;
	u8		scan_hci_cmd;
	u8		tx_irq_proc_en;
  	u8		controller_stack_param_check; ///kite/vulture not use.

	u8      md_max_nums;  //supported max md nums, default value: 0
	u8		drop_rx_data;
	u8		adv_version;
	u8		adv_hci_cmd;

#if (MCU_CORE_TYPE == MCU_CORE_9518)
  	u8		ll_aclRxFifo_set;
  	u8		ll_aclTxFifo_set;
  	u8		acl_master_en;
  	u8		acl_slave_en;
#endif

  	u8		blm_btx_busy;
  	u8		blt_state;
  	u8		ble_state;
  	u8		blc_state;  //LiJing add on Eagle, special usage, Kite/Vulture no need

  	u8		blt_busy;
  	u8  	ll_encryption_busy;
  	u8 		blc_continue_adv_en;//default stop sending legacy advertising packets when receiving scan request in the current adv interval.
  	u8		acl_rx_dma_size;

  	// to solve the problem: when first run "blt_sdk_main_loop"  in  function "main_loop" in application layer, PM may be entered due to
  	//						 "bl t_busy" cleared in early BRX(e.g. 8258_module UI task too long, BRX is executed), then UI entry can not be executed
  	//this variable can not be in retention data area!this variable must clear to 0, when IC wake up from deep(deep,deepretention)!
  	// no need "bls_pm_setSuspendMask (SUSPEND_DISABLE)" fix in "user_init_deepRetn" now(customer may feel confused, it's too complex )
	u8   	sdk_mainLoop_run_flag;
	u8 		local_chSel;   //default  0x00: CHANNAL_SELECTION_ALGORITHM_1
	u8		peer_chSel;
	u8		u8_rsvd;

#if (MCU_CORE_TYPE == MCU_CORE_9518)
  	u8		maxRxOct;
  	u8		maxTxOct;
  	u8		u_rsvd[2];

  	u32		acl_rx_dma_buff;
#endif

	u32 	custom_access_code;



#if (LE_AUTHENTICATED_PAYLOAD_TIMEOUT_SUPPORT_EN)
	u32		tick_LE_Authenticated_Payload;    //timer start tick
	u32		to_us_LE_Authenticated_Payload;    //timer threshold
	u8		enable_LE_Authenticated_Payload;
#endif
} st_ll_para_t;

extern _attribute_aligned_(4) st_ll_para_t  bltParam;





typedef struct {
	u8		save_flg;
	u8		sn_nesn;
	u8		blt_dma_tx_rptr;

	// to solve the problem: ble flow control is managed by software replace of hardware,
	//						software can resolve HW CRC error,HW kick away txFIFo data mistakenly.
	//this variable must clear to 0, when IC wake up from deep(deep ,deepretention),because of IC wake up from deep(deep ,deepretention),
	//both hardware reg_dma_tx_rptr and reg_dma_tx_wptr will become 0, moreover IC can't enter deep until data in tx fifo have send over.
	u8 		blt_dma_tx_wptr;

#if (DEEP_RET_ENTRY_CONDITIONS_OPTIMIZATION_EN)
	u8		dma_tx_rcvry_dat_depth;
	u8 		dma_tx_data_depth;

	u16		dma_tx_ptr[8];//dma tx buffer ptr rcd
#endif

}bb_sts_t;

extern _attribute_aligned_(4) bb_sts_t	blt_bb;



typedef ble_sts_t (*ll_SetExtAdv_Enable_callback_t)(int en);
extern ll_SetExtAdv_Enable_callback_t pFunc_ll_SetAdv_Enable;

#if (MCU_CORE_TYPE == MCU_CORE_9518)
extern u8   adv_rx_buff[];
#endif
extern u32	blc_tlkEvent_pending;






typedef int (*l2cap_handler_t) (u16 conn, u8 * p);
extern l2cap_handler_t	blc_l2cap_handler;

typedef int (*ll_rpa_tmo_mainloop_callback_t)(void);
typedef int (*ll_host_mainloop_callback_t)(void);
typedef int (*ll_enc_done_callback_t)(u16 connHandle);

typedef void (*ll_irq_tx_callback_t)(void);

typedef int (*ll_irq_rx_data_callback_t)(u8 *, u32);
typedef int (*ll_irq_rx_post_callback_t)(void);

typedef void (*ll_irq_systemTick_conn_callback_t)(void);


typedef int (*blc_main_loop_data_callback_t)(u8 *);
typedef int (*blc_main_loop_post_callback_t)(void);




typedef 	int (*ll_conn_complete_handler_t)(u16 conn, u8 *p);
typedef 	int (*ll_conn_terminate_handler_t)(u16 conn, u8 *p);

#ifdef __cplusplus
extern "C" {
#endif

void 		blc_ll_registerConnectionCompleteHandler(ll_conn_complete_handler_t  handler);
void 		blc_ll_registerConnectionTerminateHandler(ll_conn_terminate_handler_t  handler);









/////////////////////// SN/NESN /////////////////////////////////////////////////////
static inline void	blt_save_snnesn()
{
#if (MCU_CORE_TYPE == MCU_CORE_9518)
								// SN									NESN
	blt_bb.sn_nesn = ((REG_ADDR8(0x80140a22) & BIT(0)) << 4) | ((REG_ADDR8(0x80140a23) & BIT(4)) << 1);
#elif (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
								// SN									NESN
	blt_bb.sn_nesn = ((REG_ADDR8(0xf22) & BIT(0)) << 4) | ((REG_ADDR8(0xf23) & BIT(4)) << 1);
#endif
}

static inline void	blt_restore_snnesn()
{
	reg_rf_ll_ctrl_1 &= ~(FLD_RF_BRX_SN_INIT | FLD_RF_BRX_NESN_INIT);
	reg_rf_ll_ctrl_1 |= blt_bb.sn_nesn;
}

/////////////////////// DMA Tx fifo rptr /////////////////////////////////////////////
static inline void	blt_save_dma_tx_rptr()
{
#if (MCU_CORE_TYPE == MCU_CORE_9518)
	//TX Fifo: 0x100501[0:4] means rptr, 0 ~ 31
	blt_bb.blt_dma_tx_rptr = reg_dma_tx_rptr & FLD_DMA_RPTR_MASK;  //
#elif (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
	//TX Fifo: 0xc2a[0:3] means rptr
	blt_bb.blt_dma_tx_rptr = reg_dma_tx_rptr & 0x0f;
#endif
}

static inline void	blt_restore_dma_tx_rptr()
{
#if (MCU_CORE_TYPE == MCU_CORE_9518)
	//pay attention: FLD_DMA_RPTR_SET
	reg_dma_tx_rptr = (FLD_DMA_RPTR_SET | blt_bb.blt_dma_tx_rptr);//restore tx_rptr
#elif (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
	//0xc2a[6] rptr set
	reg_dma_tx_rptr = (BIT(6) | blt_bb.blt_dma_tx_rptr);//restore tx_rptr
#endif
}

#if (MCU_CORE_TYPE == MCU_CORE_9518)

static inline void	blt_save_dma_tx_wptr()
{
	//TX Fifo: 0x100501[0:4] means rptr, 0 ~ 31
	blt_bb.blt_dma_tx_wptr = reg_dma_tx_wptr & FLD_DMA_RPTR_MASK;  //
}

static inline void	blt_restore_dma_tx_wptr()
{
	reg_dma_tx_wptr = blt_bb.blt_dma_tx_wptr & 0x1f;//restore tx_wptr
}
static inline void	blt_save_dma_tx_ptr()
{
	//TX Fifo: 0x100501[0:4] means rptr, 0 ~ 31
	blt_bb.blt_dma_tx_rptr = reg_dma_tx_rptr & FLD_DMA_RPTR_MASK;  //
	blt_bb.blt_dma_tx_wptr = reg_dma_tx_wptr & FLD_DMA_RPTR_MASK;  //
}
static inline void	blt_restore_dma_tx_ptr()
{
	//pay attention: FLD_DMA_RPTR_SET
	reg_dma_tx_rptr = (FLD_DMA_RPTR_SET | blt_bb.blt_dma_tx_rptr);//restore tx_rptr
	reg_dma_tx_wptr = blt_bb.blt_dma_tx_wptr & FLD_DMA_RPTR_MASK; //restore tx_wptr
}
#endif ///ending of "#if (MCU_CORE_TYPE == MCU_CORE_9518)"



void 			blc_ll_setIdleState(void);
ble_sts_t 		blc_hci_le_getLocalSupportedFeatures(u8 *features);

ble_sts_t 		blc_hci_le_readBufferSize_cmd(u8 *pData);


int 			blc_ll_encrypted_data(u8*key, u8*plaintextData, u8* encrypteTextData);

ble_sts_t 		blc_hci_setTxDataLength (u16 connHandle, u16 tx, u16 txtime);
ble_sts_t 		blc_hci_readSuggestedDefaultTxDataLength (u8 *tx, u8 *txtime);
ble_sts_t 		blc_hci_writeSuggestedDefaultTxDataLength (u16 tx, u16 txtime);
ble_sts_t		blc_hci_receiveHostACLData(u16 connHandle, u8 PB_Flag, u8 BC_Flag, u8 *pData );
ble_sts_t 		blc_ll_generateDHkey (u8* remote_public_key);
ble_sts_t 		blc_ll_getP256publicKeyStart (void);
int 			blc_ll_getRandomNumber (u8* randomNumber);
void 			blc_ll_registerRpaTmoMainloopCallback (ll_rpa_tmo_mainloop_callback_t cb);
void 			blc_ll_registerHostMainloopCallback (ll_host_mainloop_callback_t cb);
void 			blc_ll_registerConnectionEncryptionDoneCallback(ll_enc_done_callback_t  cb);

#if (MCU_CORE_TYPE == MCU_CORE_9518)
u8  			blt_ll_getRealTxFifoNumber (void);
#endif

ble_sts_t  		blc_hci_reset(void);





void 		blc_ll_init_max_md_nums(u8 num);

#if (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
//core4.2 data extension
void 			blc_ll_initDataLengthExtension (void);
#endif

static inline void blc_ll_resetInfoRSSI(void)
{
	bltParam.ll_recentAvgRSSI = 0;
}

static inline void blc_ll_recordRSSI(u8 rssi)
{
	if(bltParam.ll_recentAvgRSSI == 0) {
		bltParam.ll_recentAvgRSSI = rssi;
	}
	else {
		bltParam.ll_recentAvgRSSI = (bltParam.ll_recentAvgRSSI + rssi) >> 1;
	}
}

static inline u8* 	blc_ll_get_macAddrRandom(void)
{
	return bltMac.macAddress_random;
}

static inline u8* 	blc_ll_get_macAddrPublic(void)
{
	return bltMac.macAddress_public;
}

static inline void  blc_ll_setEncryptionBusy(u8 enc_busy)
{
	bltParam.ll_encryption_busy = enc_busy;
}

static inline int  blc_ll_isEncryptionBusy(void)
{
	return bltParam.ll_encryption_busy;
}

static inline void blt_ll_set_ble_access_code_adv(void)
{
#if (MCU_CORE_TYPE == MCU_CORE_9518)
	write_reg32 (0x80140808, bltParam.custom_access_code ? bltParam.custom_access_code : 0xd6be898e);
#elif (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
	write_reg32 (0x800408, bltParam.custom_access_code ? bltParam.custom_access_code : 0xd6be898e);
#endif
}









/******************************* ll end *************************************************************************/



















/******************************* ll_adv start ***********************************************************************/
/* Advertising Maximum data length */
#define 		ADV_MAX_DATA_LEN                    			31


#define			BLS_FLAG_ADV_IN_SLAVE_MODE						BIT(6)
#define 		BLC_FLAG_STK_ADV								BIT(24)







typedef struct {
	u8		adv_chn_mask;
	u8		adv_duration_en;
	u8		adv_type;
	u8 		adv_filterPolicy;

	u8		T_SCAN_RSP_INTVL;  //not used now
	u8      own_addr_type;     //own_addr_type
	u8		rsvd;
	u8		advInt_rand;

	u16		advInt_min;
	u16 	advInt_max;

	u32		adv_interval;     // system tick
	u32		adv_duration_us;
	u32		adv_begin_tick;

#if (LL_FEATURE_ENABLE_LL_PRIVACY)
	u8      advPeerAddrType;
	u8		advRpaTmoFlg;
	s8      advRpaRlIdx;
	u8      advRpaResoved;

	u16     rsvd1;
	u8		advPeerAddr[BLE_ADDR_LEN];
#endif
}st_ll_adv_t;



extern _attribute_aligned_(4) st_ll_adv_t  blta;


extern  rf_packet_adv_t	pkt_adv;


typedef int (*ll_adv2conn_callback_t)(u8 *, bool);   //rcvd conn_req, adv state to conn state
extern 	ll_adv2conn_callback_t ll_adv2conn_cb;


typedef int  (*ll_module_adv_callback_t)(void);



typedef int (*advertise_prepare_handler_t) (rf_packet_adv_t * p);


extern u32  blc_rcvd_connReq_tick;



static inline u32 	bls_ll_getConnectionCreateTime(void)
{
	return blc_rcvd_connReq_tick;
}




ble_sts_t 	bls_hci_le_setAdvParam(adv_para_t *para);
ble_sts_t 	bls_hci_le_readChannelMap(u16 connHandle, u8 *returnChannelMap);


ble_sts_t 	bls_ll_setAdvInterval(u16 intervalMin, u16 intervalMax);
ble_sts_t 	bls_ll_setAdvChannelMap(adv_chn_map_t adv_channelMap);
ble_sts_t 	bls_ll_setAdvFilterPolicy(adv_fp_type_t advFilterPolicy);


int  		blt_send_adv(void);

static inline u8 	blt_ll_getOwnAddrType(void)
{
	return blta.own_addr_type;
}




/******************************* ll_adv end *************************************************************************/














/******************************* ll_scan start ***********************************************************************/

#define			BLS_FLAG_SCAN_ENABLE							BIT(0)


#define			BLS_FLAG_SCAN_IN_ADV_MODE						BIT(5)
#define			BLS_FLAG_SCAN_IN_SLAVE_MODE						BIT(6)






typedef struct {
	u8		scan_en;
	u8		scan_type;
	u8		scan_filterPolicy;
	u8		filter_dup;

	u8		scanDevice_num;
	u8		scanRspDevice_num;
	u8		scan_extension_mask;
	u8 		rsvd;
//	s8		T_SCAN_REQ_INTVL;



	//u32		scan_interval;
}st_ll_scan_t;

extern st_ll_scan_t  blts;

extern u32		blts_scan_interval;

extern 			rf_packet_scan_req_t	pkt_scan_req;


typedef int (*ll_procScanPkt_callback_t)(u8 *, u8 *, u32);
typedef int (*ll_procScanDat_callback_t)(u8 *);

extern ll_procScanDat_callback_t  blc_ll_procScanDatCb;
extern ll_procScanPkt_callback_t  blc_ll_procScanPktCb;



int			blc_ll_filterAdvDevice (u8 type, u8 * mac);
int 		blc_ll_addScanRspDevice(u8 type, u8 *mac);
bool 		blc_ll_isScanRspReceived(u8 type, u8 *mac);
void 		blc_ll_clearScanRspDevice(void);


int  		blc_ll_procScanPkt(u8 *raw_pkt, u8 *new_pkt, u32 tick_now);
int  		blc_ll_procScanData(u8 *raw_pkt);

void 		blc_ll_switchScanChannel (int, int);

/******************************* ll_scan end *************************************************************************/





/******************************* ll_init start ***********************************************************************/

typedef struct {
	u8		init_en;
	u8		conn_policy;
	u8		conn_advType;
	u8		conn_established;

	u32		create_conn_startTick;

	u16     rsvd;
	u8		conn_mac[6];

}st_ll_init_t;


extern u32 blm_timeout_connectDevice;



extern rf_packet_ll_init_t	pkt_init;




typedef int  (*ll_module_init_callback_t)(u8 *, u32);





static inline u32 rf_get_ble_access_code_16to32 (u16 code)
{
	u32 r = 0;
	int i;
	for (i=0; i<4; i++) {
		r = r << 2;
		r |= code & BIT(i) ? 1 : 2;
	}

	for (i=4; i<8; i++) {  //no more 24 transitions
		r = r << 4;
		r |= code & BIT(i) ? 0x0c : 0x03;
	}

	for (i=12; i<16; i++) {
		r = r << 2;
		r |= code & BIT(i) ? 1 : 2;
	}

	return r;
}

u32 blt_ll_conn_calc_access_addr_v2(void);



ble_sts_t 	blc_ll_setInitEnable (u8 en);


/******************************* ll_init end *************************************************************************/






/******************************* ll_pm start ***********************************************************************/
#ifndef 	BLS_USER_TIMER_WAKEUP_ENABLE
#define		BLS_USER_TIMER_WAKEUP_ENABLE	1
#endif





typedef struct {
	u8		suspend_mask;
	u8		wakeup_src;
	u8		conn_no_suspend;
	u8 		timer_wakeup;

	u8		latency_off;  //used to protect BRX timing when erase flash in other MCU SDK, Eagle not need
	u8		no_latency;
	u16 	latency_en;

	u16		sys_latency;
	u16 	user_latency;
	u16		valid_latency;
	u16 	latency_use;

	u32     deepRet_advThresTick;
	u32     deepRet_connThresTick;
	u32     deepRet_earlyWakeupTick;

	u8 		deepRt_en;
	u8		deepRet_type;
	u8		appWakeup_en;
	u8		appWakeup_flg;

	u8		appWakeup_loop_noLatency;
	u8 		timing_miss;
	u8      timing_synced;
	u8 		rsvd;

	u32 	appWakeup_tick;

	u32     current_wakeup_tick; //The system wake-up tick of the actual transfer of the cpu_sleep_wakeup function.

	u16		pm_border_flag;
	u16     rsvd1;

}st_ll_pm_t;

extern _attribute_aligned_(4) st_ll_pm_t  bltPm;

/******************************* ll_pm end *************************************************************************/





/******************************* ll_whitelist start ***********************************************************************/



#if (MCU_CORE_TYPE == MCU_CORE_9518)
	#define 	IRK_REVERT_TO_SAVE_AES_TMIE_ENABLE				0
#else
	#define 	IRK_REVERT_TO_SAVE_AES_TMIE_ENABLE				1
#endif



typedef struct {
	u8 type;
	u8 address[BLE_ADDR_LEN];
	u8 reserved;
} wl_addr_t;

typedef struct {
	wl_addr_t  wl_addr_tbl[MAX_WHITE_LIST_SIZE];
	u8 	wl_addr_tbl_index;
	u8 	wl_irk_tbl_index;
} ll_whiteListTbl_t;



extern	ll_whiteListTbl_t		ll_whiteList_tbl;


bool 				blt_ll_resolvIrkIsNonzero(const u8 *irk);
void				blt_ll_resolvGetPrand(u8* prand);
void 				blt_ll_resolvCalcAh(const u8 irk[16], u8 r[3], u8 out[3]);
bool    			blt_ll_resolvChgIsAllowed(void);
void				blt_ll_resolvGenRpa(ll_resolv_list_t *rl, u8 local);
void				blt_ll_resolvRpaTmrCb(void);
int 				blt_ll_resolvRpaTmoLoopEvt (void);
int 				blt_ll_resolvIsAddrOnRl(const u8 *addr, u8 addrType);
ll_resolv_list_t* 	blt_ll_resolvFindRlEntry(const u8* addr, u8 addrType);

u8 * 				ll_searchAddr_in_WhiteList_and_ResolvingList(u8 type, u8 *addr);

u8 * 				ll_searchAddrInWhiteListTbl(u8 type, u8 *addr);
u8 * 				ll_searchAddrInResolvingListTbl(u8 *addr);  //addr must be RPA
bool 				smp_quickResolvPrivateAddr(u8 *key, u8 *addr);


/******************************* ll_whitelist end *************************************************************************/














/******************************* ll_ext_adv start ***********************************************************************/


#define			INVALID_ADVHD_FLAG								0xFF


#define 		TLK_T_MAFS										302  // T_MAFS >= 300 uS, add some margin
#define 		TLK_T_MAFS_30US_NUM								10

#define			TX_TX_DELAY_US									120 // TX settle(74 uS) + 5  preamble(40uS) + some other baseband cost,   and be 30*N




typedef struct
{
    u8 		maxNum_advSets;
    u8		useNum_advSets;
    u8		last_advSet;
    u8		last_advHand;


    u8		T_SCAN_RSP_INTVL;  //NOT USED
    u8 		custom_aux_chn;
    u8 		T_AUX_RSP_INTVL; //settle aux_scan_rsp/aux_conn_rsp's IFS 150s
    u8 		rsvd3;

    u32		rand_delay;

}ll_adv_mng_t;




typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	advA[6];			//address
	u8	data[31];			//0-31 byte
}rf_pkt_pri_scanrsp_t;







/******************************************************  ADV_EXT_IND ************************************************************
												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData			Structure
																  	  Ptr  	Info   Power
Non-Connectable
Non-Scannable   Undirected without AUX		  	 M		 X		X	   X	 X		X	   X	 X			rf_pkt_adv_ext_ind_1

Non-Connectable
Non-Scannable   Undirected with    AUX			 X 	 	 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Non-Connectable
Non-Scannable	Directed   without AUX			 M		 M		X	   X	 X		X	   X	 X			rf_pkt_adv_ext_ind_1

Non-Connectable
Non-Scannable	Directed   with	   AUX			 X	 	 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2


Connectable 	Undirected					 	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Connectable 	Directed					 	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Scannable 		Undirected				     	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Scannable 		Directed					 	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	targetA[6];
}rf_pkt_adv_ext_ind_1;


typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	//ADI
	u16	did	:12;
	u16	sid : 4;

	//AuxPrt
	u8  chn_index    :6;
	u8  ca 		 	:1;
	u8  offset_unit	:1;
	u16 aux_offset  :13;
	u16	aux_phy		:3;

}rf_pkt_adv_ext_ind_2;




/******************************************************  AUX_ADV_IND ************************************************************
												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power
Non-Connectable
Non-Scannable   Undirected with    AUX			 M 	 	 X		M	   O	 X		X	   X	 O			rf_pkt_aux_adv_ind_1

Non-Connectable
Non-Scannable	Directed   with	   AUX			 M	 	 M		M	   O	 X		X	   X	 O			rf_pkt_aux_adv_ind_2

Connectable 	Undirected					 	 M		 X		M	   X	 X		X	   X	 O			rf_pkt_aux_adv_ind_1

Connectable 	Directed					 	 M		 M		M	   X	 X		X	   X	 O			rf_pkt_aux_adv_ind_2

Scannable 		Undirected				     	 M		 X		M	   X	 X		X	   X	 X			rf_pkt_aux_adv_ind_1

Scannable 		Directed					 	 M		 M		M	   X	 X		X	   X	 X			rf_pkt_aux_adv_ind_2
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];

	//ADI
	u16	did	:12;
	u16	sid : 4;

	u8	dat[1];   // AuxPtr/AdvData
}rf_pkt_aux_adv_ind_1;


typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	targetA[6];

	//ADI
	u16	did	:12;
	u16	sid : 4;

	u8	dat[1]; // AuxPtr/AdvData
}rf_pkt_aux_adv_ind_2;





typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];

	//ADI
	u16	did	:12;
	u16	sid : 4;

	//AuxPrt
	u8  chn_index    :6;
	u8  ca 		 	:1;
	u8  offset_unit	:1;
	u16 aux_offset  :13;
	u16	aux_phy		:3;

	u8	dat[1];   // AdvData
}rf_pkt_aux_adv_ind_3;




typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	targetA[6];

	//ADI
	u16	did	:12;
	u16	sid : 4;

	//AuxPrt
	u8  chn_index    :6;
	u8  ca 		 	:1;
	u8  offset_unit	:1;
	u16 aux_offset  :13;
	u16	aux_phy		:3;

	u8	dat[1]; // AdvData
}rf_pkt_aux_adv_ind_4;



/******************************************************  AUX_SCAN_RSP ***********************************************************

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power

								  	  	  	  	  M		 X		X	   O	 X		X	   X	 M
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	dat[1];   // AuxPtr/AdvData
}rf_pkt_aux_scan_rsp_t;

#define MAX_ADVDATA_NUM_AUX_SCANRSP									247   // 255 - 8( Extended Header Length(1) + Extended Header Flag(1) + AdvA(6) ) = 247




/******************************************************  AUX_CHAIN_IND ***********************************************************

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power

		Chained data:							  X		 X		C3	   O	 X		O	   X	 O

														    			||
																		||
														   	   	   	   \||/
																		\/

Chained data after AUX_ADV_IND  :				 X		 X		M	   O	 X		X	   X	 O			rf_pkt_aux_chain_ind_1

Chained data after AUX_SCAN_RSP :				 X		 X		X	   O	 X		X	   X	 O			rf_pkt_aux_chain_ind_2
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;


	//ADI
	u16	did	:12;
	u16	sid : 4;

	u8	dat[1]; // AuxPrt/AdvData
}rf_pkt_aux_chain_ind_1;

#define MAX_ADVDATA_NUM_AUX_CHAIN_IND_1									251   // 255 - 4( Extended Header Length(1) + Extended Header Flag(1) + ADI(2) ) = 251








/******************************************************  AUX_CONNECT_RSP *********************************************************

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power

								  	  	  	  	  M		 M		X	   X	 X		X	   X	 X
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	targetA[6];
}rf_pkt_aux_conn_rsp_t;




/****************************************************  SCAN_REQ / AUX_SCAN_REQ **************************************************

*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	scanA[6];
	u8	advA[6];
}rf_pkt_ext_scan_req_t;






ble_sts_t 	blc_hci_le_setExtAdvParam( hci_le_setExtAdvParam_cmdParam_t *para, u8 *pTxPower);
ble_sts_t 	blc_hci_le_setExtAdvEnable(u8 extAdv_en, u8 sets_num, u8 *pData);

u16 	  	blc_ll_readMaxAdvDataLength(void);
u8		  	blc_ll_readNumberOfSupportedAdvSets(void);


int  		blt_ext_adv_proc(void);
int  		blt_send_adv2(void);
int 		blt_send_legacy_adv(void);
int 		blt_send_extend_adv(void);
void 		blt_send_extend_no_aux_adv(void);
int 		blt_send_aux_adv(void);
ble_sts_t	blt_ll_clearAdvSets(void);

u8			blt_ll_searchExistingAdvSet(u8 advHandle);
u8 			blt_ll_searchAvailableAdvSet(u8 advHandle);
void		blt_ll_updateAdvState(void);
ble_sts_t   blt_ll_enableExtAdv(int adv_en);
void 		blt_ll_procAuxConnectReq(u8 * prx);
int  		blt_ll_updateAdvPacket(void);
void		blt_ll_reset_ext_adv(void);


//Set Extended ADV parameters
ble_sts_t	blc_ll_setAdvRandomAddr(u8 advHandle, u8* rand_addr);
ble_sts_t	blc_ll_setExtAdvEnable_n(u32 extAdv_en, u8 sets_num, u8 *pData);


ble_sts_t	blc_ll_removeAdvSet(u8 advHandle);
ble_sts_t	blc_ll_clearAdvSets(void);
//TODO
//void 		blt_clearAdvSetsParam(ll_ext_adv_t		*pEadv);

/******************************* ll_ext_adv end *************************************************************************/








/**************************************************************************************************************************************
ADV_EXT_IND:
												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power
Non-Connectable
Non-Scannable   Undirected without AUX		  	 M		 X		X	   X	 X		O	   X	 X

Non-Connectable
Non-Scannable   Undirected with    AUX			 C1 	 X		M	   M	 X		C1	   X	 X
										 telink:(X)(by sihui)
Non-Connectable
Non-Scannable	Directed   without AUX			 M		 M		X	   X	 X		O	   X	 X

Non-Connectable
Non-Scannable	Directed   with	   AUX			 C1	 	 C1		M	   M	 X		C1	   X	 X
									 	 telink:(X)		(X) (by sihui)

Connectable 	Undirected					 	 X		 X		M	   M	 X		C1	   X	 X

Connectable 	Directed					 	 X		 X		M	   M	 X		C1	   X	 X

Scannable 		Undirected				     	 X		 X		M	   M	 X		C1	   X	 X

Scannable 		Directed					 	 X		 X		M	   M	 X		C1	   X	 X
																					|
																		telink: txPower "X"

														    			||
																		||
														   	   	   	   \||/
																		\/

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData			Structure
																  	  Ptr  	Info   Power
Non-Connectable
Non-Scannable   Undirected without AUX		  	 M		 X		X	   X	 X		X	   X	 X			rf_pkt_adv_ext_ind_1

Non-Connectable
Non-Scannable   Undirected with    AUX			 X 	 	 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Non-Connectable
Non-Scannable	Directed   without AUX			 M		 M		X	   X	 X		X	   X	 X			rf_pkt_adv_ext_ind_1

Non-Connectable
Non-Scannable	Directed   with	   AUX			 X	 	 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2


Connectable 	Undirected					 	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Connectable 	Directed					 	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Scannable 		Undirected				     	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Scannable 		Directed					 	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2





AUX_ADV_IND:
												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power
Non-Connectable
Non-Scannable   Undirected with    AUX			 C4 	 X		M	   O	 O		O	   O	 O

Non-Connectable
Non-Scannable	Directed   with	   AUX			 C4	 	 C2		M	   O	 O		O	   O	 O
		 	 	 	 	 	 	 	 	 	 	 telink:(M)(by sihui)

Connectable 	Undirected					 	 M		 X		M	   X	 X		O	   O	 O

Connectable 	Directed					 	 M		 M		M	   X	 X		O	   O	 O

Scannable 		Undirected				     	 M		 X		M	   X	 X		O	   O	 X

Scannable 		Directed					 	 M		 M		M	   X	 X		O	   O	 X

														    			||
																		||
														   	   	   	   \||/
																		\/

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power
Non-Connectable
Non-Scannable   Undirected with    AUX			 C4 	 X		M	   O	 X		X	   X	 O

Non-Connectable
Non-Scannable	Directed   with	   AUX			 C4	 	 M		M	   O	 X		X	   X	 O

Connectable 	Undirected					 	 M		 X		M	   X	 X		X	   X	 O

Connectable 	Directed					 	 M		 M		M	   X	 X		X	   X	 O

Scannable 		Undirected				     	 M		 X		M	   X	 X		X	   X	 X

Scannable 		Directed					 	 M		 M		M	   X	 X		X	   X	 X

						"C4" here determined by "ADV_EVT_PROP_EXTENDED_MASK_ANONYMOUS_ADV"  (sihui)
														Now we do not support anonymous ADV
														    			||
																		||
														   	   	   	   \||/
																		\/

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power
Non-Connectable
Non-Scannable   Undirected with    AUX			 M 	 	 X		M	   O	 X		X	   X	 O			rf_pkt_aux_adv_ind_1

Non-Connectable
Non-Scannable	Directed   with	   AUX			 M	 	 M		M	   O	 X		X	   X	 O			rf_pkt_aux_adv_ind_2

Connectable 	Undirected					 	 M		 X		M	   X	 X		X	   X	 O			rf_pkt_aux_adv_ind_1

Connectable 	Directed					 	 M		 M		M	   X	 X		X	   X	 O			rf_pkt_aux_adv_ind_2

Scannable 		Undirected				     	 M		 X		M	   X	 X		X	   X	 X			rf_pkt_aux_adv_ind_1

Scannable 		Directed					 	 M		 M		M	   X	 X		X	   X	 X			rf_pkt_aux_adv_ind_2






AUX_CHAIN_IND:

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power

		Chained data:							  X		 X		C3	   O	 X		O	   X	 O

														    			||
																		||
														   	   	   	   \||/
																		\/

Chained data after AUX_ADV_IND  :				 X		 X		M	   O	 X		X	   X	 O

Chained data after AUX_SCAN_RSP :				 X		 X		X	   O	 X		X	   X	 O




AUX_SCAN_RSP:
												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power

								  	  	  	  	  M		 X		X	   O	 X		O	   O	 M
														    			||
																		||
														   	   	   	   \||/
																		\/
								  	  	  	  	  M		 X		X	   O	 X		X	   X	 M






AUX_CONNECT_RSP:
												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power

								  	  	  	  	  M		 M		X	   X	 X		X	   X	 X

******************************************************************************************************************************************/






/****************************** (ble1m,2m,500k,125k)RF RX/TX packet format ********************************************
RF RX packet format:
  b0          b3    b4         b5       b6   b(5+w) b(6+w) b(8+w) b(9+w) b(12+w)  b(13+w)    b(14+w)  b(15+w)                      b(16+w)
*---------------*---------*-----------*------------*------------*---------------*-------------------*----------*--------------------------------------------------*
|  DMA_len(4B)  | type(1B)| Rf_len(1B)| payload(wB)|   CRC(3B)  | time_stamp(4B)|  Fre_offset(2B)   | Rssi(1B) |           pkt status indicator(1B)               |
| (b0,b1 valid) |        Header       |   Payload  |            |               |                   | rssi-110 |[0]:crc err;[1]:sfd err;[2]:ll err;[4]:pwr err;   |
|               |<--           PDU              -->|            |               |                   |          |[4]:long range 125k;[6:5]:N/A;[7]:nordic NACK ind |
*---------------*----------------------------------*------------*---------------*-------------------*----------*--------------------------------------------------*
|<--- 4byte --->|<------ 2 byte ----->|<- Rf_len ->|<- 3 byte ->|<----------------------------------- 8 byte ---------------------------------------------------->|
note:       b4       ->  type(1B): llid(2bit) nesn(1bit) sn(1bit) md(1bit).
we can see: DMA_len     =   rx[0] = w(Rf_len)+13 = rx[5]+13.
            CRC_OK      =   DMA_buffer[rx[0]+3] == 0x00 ? True : False.

******
RF TX packet format:
 b0          b3      b4         b5       b6   b(5+w)
*---------------*----------*-----------*------------*
|  DMA_len(4B)  | type(1B) | Rf_len(1B)| payload(wB)|
| (b0,b1 valid) |         Header       |   Payload  |
|               |<--               PDU           -->|
*---------------*-----------------------------------*
note:       b4      ->  type(1B): llid(2bit) nesn(1bit) sn(1bit) md(1bit).Here type only means that llid, other bit is automatically populated when sent by hardware
we can see: DMA_len = rx[0]= w(Rf_len) + 2.
**********************************************************************************************************************/



/************************************** Link Layer pkt format *********************************************************
Link Layer pak format(BLE4.2 spec):
*-------------*-------------------*-------------------------------*-------------------*
| preamble(1B)| Access Address(4B)|          PDU(2~257B)          |      CRC(3B)      |
|             |                   |  Header(2B) | payload(0~255B) |                   |
*-------------*-------------------*-------------------------------*-------------------*
1.ADV Channel, payload:0~37bytes = 6bytes AdvAdd + [maximum 31bytes adv packet payload]
2.Data Channel, payload:0~255bytes = 0~251bytes + 4bytes MIC(may include MIC feild)[The payload in ble4.2 can reach 251 bytes].
  Protocol overhead: 10bytes(preamble\Access Address\Header\CRC) + L2CAP header 4bytes = 14bytes, all LL data contains 14 bytes of overhead,
  For att, opCode is also needed, 1bytes + handle 2bytes = 3bytes, 251-4-3=[final 247-3bytes available to users].
******
Link Layer pak format(BLE4.0\4.1 spec):
*-------------*-------------------*-------------------------------*-------------------*
| preamble(1B)| Access Address(4B)|          PDU(2~39B)           |      CRC(3B)      |
|             |                   |  Header(2B) | payload(0~37B)  |                   |
*-------------*-------------------*-------------------------------*-------------------*
1.ADV Channel, payload:0~37bytes = 6bytes AdvAdd + [maximum 31bytes adv packet payload]
2.Data Channel, payload:0~31bytes = 0~27bytes + 4bytes MIC(may include MIC feild)[The payload in ble4.0/4.1 is 27 bytes].
  Protocol overhead: 10bytes(preamble\Access Address\Header\CRC) + L2CAP header 4bytes = 14bytes,all LL data contains 14 bytes of overhead,
  For att, opCode is also needed, 1bytes + handle 2bytes = 3bytes, 27-4-3=[final 23-3bytes available to users],This is why the default mtu size is 23 in the ble4.0 protocol.
**********************************************************************************************************************/


/*********************************** Advertising channel PDU : Header *************************************************
Header(2B):[Advertising channel PDU Header](BLE4.0\4.1 spec):
*--------------*----------*------------*-------------*-------------*----------*
|PDU Type(4bit)| RFU(2bit)| TxAdd(1bit)| RxAdd(1bit) |Length(6bits)| RFU(2bit)|
*--------------*----------*------------*-------------*-------------*----------*
public (TxAdd = 0) or random (TxAdd = 1).
**********************************************************************************************************************/


/******************************************* Data channel PDU : Header ************************************************
Header(2B):[Data channel PDU Header](BLE4.2 spec):(BLE4.0\4.1 spec):
*----------*-----------*---------*----------*----------*-------------*----------*
|LLID(2bit)| NESN(1bit)| SN(1bit)| MD(1bit) | RFU(3bit)|Length(5bits)| RFU(3bit)|
*----------*-----------*---------*----------*----------*-------------*----------*
******
Header(2B):[Data channel PDU Header](BLE4.2 spec):
*----------*-----------*---------*----------*----------*------------------------*
|LLID(2bit)| NESN(1bit)| SN(1bit)| MD(1bit) | RFU(3bit)|       Length(8bits)    |
*----------*-----------*---------*----------*----------*------------------------*
start    pkt:  llid 2 -> 0x02
continue pkt:  llid 1 -> 0x01
control  pkt:  llid 3 -> 0x03
***********************************************************************************************************************/


/*********************************** DATA channel PDU ******************************************************************
*------------------------------------- ll data pkt -------------------------------------------*
|             |llid nesn sn md |  pdu-len   | l2cap_len(2B)| chanId(2B)|  opCode(1B)|data(xB) |
| DMA_len(4B) |   type(1B)     | rf_len(1B) |       L2CAP header       |       value          |
|             |          data_headr         |                        payload                  |
*-------------*-----------------------------*-------------------------------------------------*
*--------------------------------- ll control pkt ----------------------------*
| DMA_len(4B) |llid nesn sn md |  pdu-len   | LL Opcode(1B) |  CtrData(0~22B) |
|             |   type(1B)     | rf_len(1B) |               |      value      |
|             |          data_headr         |            payload              |
*-------------*-----------------------------*---------------------------------*
***********************************************************************************************************************/

#ifdef __cplusplus
}
#endif




#endif /* STACK_BLE_LL_LL_STACK_H_ */
