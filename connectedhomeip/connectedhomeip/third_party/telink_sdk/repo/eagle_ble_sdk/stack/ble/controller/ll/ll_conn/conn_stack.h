/********************************************************************************************************
 * @file	conn_stack.h
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
#ifndef STACK_BLE_LL_LL_CONN_CONN_STACK_H_
#define STACK_BLE_LL_LL_CONN_CONN_STACK_H_

#include "stack/ble/ble_stack.h"
#include "stack/ble/ble_format.h"
#include "stack/ble/controller/phy/phy_stack.h"
#include "algorithm/aes_ccm/aes_ccm.h"


/******************************* ll_conn start ************************************************************************/




#define			LMP_PROCEDURE_RESPONSE_TIMEOUT					40000000 //40S LL response timeout

#define			HANDLE_STK_FLAG									BIT(15)

/////////////////////////////////////////////////////////////////////////////
#define			MAX_OCTETS_DATA_LEN_27							27
#define			MAX_OCTETS_DATA_LEN_EXTENSION					251


#define			LL_PACKET_OCTET_TIME(n)							((n) * 8 + 112)

#define 		DATA_LENGTH_REQ_PENDING							1
#define			DATA_LENGTH_REQ_DONE							2

/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
#define 		FLG_RF_CONN_DONE 		 						(FLD_RF_IRQ_CMD_DONE | FLD_RF_IRQ_FSM_TIMEOUT | FLD_RF_IRQ_FIRST_TIMEOUT | FLD_RF_IRQ_RX_TIMEOUT | FLD_RF_IRQ_RX_CRC_2)



#define			BLE_STACK_USED_TX_FIFIO_NUM						2

typedef struct {
	u8		connState;
	u8		conn_update;
	u8		last_rf_len;
	u8		conn_peer_sn;

	u8		conn_rx_num; 			 	//RX number (regardless of CRC correct or wrong)
	u8		conn_receive_packet;		//RX with CRC correct
	u8		conn_receive_new_packet;	//RX with CRC correct & new SN
	u8		rsvd1;

	u8		rsvd2;
	u8		conn_snnesn;
	u8		conn_chnsel;
	u8		chn_idx;

	u8		conn_chn;
	u8		conn_chn_hop;
	u8		conn_chn_map[5];
	u8		conn_chn_map_next[5];


	u16		connHandle;
	u16 	channel_id;

	u16		conn_inst;
	u16		conn_inst_next;

	u32		conn_tick;

	u32		conn_access_code;
	u32		conn_access_code_revert;
	u32		conn_crc;

	u8		chn_tbl[40];

}st_ll_conn_t ;
extern _attribute_aligned_(4) st_ll_conn_t		blttcon;
extern st_ll_conn_t	   *blt_pconn;


typedef bool (*ll_push_fifo_handler_t) (int, u8 *);
extern  ll_push_fifo_handler_t				ll_push_tx_fifo_handler;



extern u8 blt_tx_empty_packet[6];




#ifdef __cplusplus
extern "C" {
#endif

int 		blt_ll_connect_common(st_ll_conn_t *pc, rf_packet_connect_t * pInit);
int 		blt_ll_start_common  (st_ll_conn_t *pc);

//encryption
ble_sts_t 	blc_hci_ltkRequestNegativeReply (u16 connHandle);
ble_sts_t  	blc_hci_ltkRequestReply (u16 connHandle,  u8*ltk);


typedef int (*blt_LTK_req_callback_t)(u16 handle, u8* rand, u16 ediv);
void 		blc_ll_registerLtkReqEvtCb(blt_LTK_req_callback_t evtCbFunc);

int  		blt_ll_conn_main_loop_post(void);
void		blc_procPendingEvent(void);


ble_sts_t	blt_ll_unknown_rsp(u16 connHandle, u8 op_code );

void 		blt_ll_channelTable_calc(u8* chm, u8 hop, u8 *ptbl);



/******************************* ll_conn end **************************************************************************/



















/******************************* ll_slave start ************************************************************************/


#define					SLAVE_LL_ENC_OFF						0
#define					SLAVE_LL_ENC_REQ						1
#define					SLAVE_LL_ENC_RSP_T						2
#define					SLAVE_LL_ENC_START_REQ_T				3
#define					SLAVE_LL_ENC_START_RSP					4
#define					SLAVE_LL_ENC_START_RSP_T				5
#define					SLAVE_LL_ENC_PAUSE_REQ					6
#define					SLAVE_LL_ENC_PAUSE_RSP_T				7
#define					SLAVE_LL_ENC_PAUSE_RSP					8
#define					SLAVE_LL_REJECT_IND_T					9


#define 				TRY_FIX_ERR_BY_ADD_BRX_WAIT				1



typedef struct {
	u8		time_update_st;
	u8 		last_rf_len;
	u8		remoteFeatureReq;
	u8 		long_suspend;

	u8		interval_level;
	u8		ll_recentAvgRSSI;
	u8		conn_sn_master;
	u8		master_not_ack_slaveAckUpReq;

	u8		conn_rcvd_ack_pkt;
	u8		conn_new_param;
	u8		conn_winsize_next;
	u8		remoteVersionFlag;

#if (LL_FEATURE_ENABLE_LL_PRIVACY)
	u8	    conn_peer_addr_type; //host event use addr type: 0,1,2,3
	u8      rsvd2;
	u8		conn_peer_addr[6];   //host event use addr
#endif

	u8 		conn_master_terminate;
	u8		conn_terminate_reason;
	u8 		conn_slave_terminate;
	u8		conn_terminate_pending;   // terminate_pending = master_terminate || slave_terminate


	u16		conn_update_inst_diff;
	u16		connHandle;
	u16     conn_offset_next;
	u16		conn_interval_next; //standard value,  not * 1.25ms
	u16		conn_latency_next;
	u16		conn_timeout_next;  //standard value,  not *10ms


	u32		conn_access_code_revert;
	u32		connExpectTime;
	int		conn_interval_adjust;
	u32		conn_timeout;
	u32		conn_interval;
	u32		conn_latency;
	u32		conn_duration;

	u32 	ll_remoteFeature; 	 //feature mask <0:31>,  not only one for BLE master, use connHandle to identify
	//u32   ll_remoteFeature_2;  //feature mask <32:63> for future use

	u32		conn_start_tick;

	int		conn_tolerance_time;

	u32		tick_1st_rx;
	u32		conn_brx_tick;



	u32 	conn_slaveTerminate_time;


	u32		conn_pkt_rcvd;
	u32		conn_pkt_rcvd_no;
	u8 *	conn_pkt_dec_pending;
	int		conn_enc_dec_busy;
	int		conn_stop_brx;

	u32		conn_fsm_timeout;   // unit:  uS

#if	(BQB_5P0_TEST_ENABLE)
	u32		conn_establish_pending_timeout;;
	u32		conn_establish_pending_flag;
#endif


} st_ll_conn_slave_t;




extern _attribute_aligned_(4) st_ll_conn_slave_t		bltc;
#define 	IS_LL_CONNECTION_VALID(handle)  ( bltc.connHandle == (handle) && bltc.connHandle != BLE_INVALID_CONNECTION_HANDLE )





bool		bls_ll_pushTxFifo (int handle, u8 *p);
ble_sts_t  	bls_hci_reset(void);

ble_sts_t	bls_hci_receiveHostACLData(u16 connHandle, u8 PB_Flag, u8 BC_Flag, u8 *pData );
ble_sts_t 	bls_hci_le_getRemoteSupportedFeatures(u16 connHandle);

void		blt_push_fifo_hold (u8 *p);
void 		blt_terminate_proc(void);



extern ble_crypt_para_t 	blc_cyrpt_para;
static inline int  bls_ll_isConnectionEncrypted(u16 connHandle)
{
	return blc_cyrpt_para.enable;
}

#if (TRY_FIX_ERR_BY_ADD_BRX_WAIT)
	extern int CONN_SHORT_TOLERANCE_TIME;
	static inline void blc_pm_modefy_brx_early_set(int us)
	{
		CONN_SHORT_TOLERANCE_TIME = us * SYSTEM_TIMER_TICK_1US;
	}

	static inline int blc_pm_get_brx_early_time(void)
	{
		return CONN_SHORT_TOLERANCE_TIME;
	}
#endif





/******************************* ll_slave end **************************************************************************/












/******************************* ll_master start ***********************************************************************/
#if (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
#define			BLE_DATA_CHANNEL_EN				1
#endif

#define			BLE_MASTER_CONNECTION_REQ		1
#define			BLM_CONN_HANDLE_CANCEL			BIT(8)
#define			BLM_CONN_MASTER_TERMINATE		BIT(4)
#define			BLM_CONN_SLAVE_TERMINATE		BIT(5)
#define			BLM_CONN_TERMINATE_SEND			BIT(0)

#define			BLM_CONN_ENC_CHANGE				BIT(9)
#define			BLM_CONN_ENC_REFRESH			BIT(10)
#define			BLM_CONN_ENC_REFRESH_T			BIT(11)


#define			MASTER_LL_ENC_OFF				0
#define			MASTER_LL_ENC_REQ				1
#define			MASTER_LL_ENC_RSP_T				2
#define			MASTER_LL_ENC_START_REQ_T		3
#define			MASTER_LL_ENC_START_RSP			4
#define			MASTER_LL_ENC_START_RSP_T		5
#define			MASTER_LL_ENC_PAUSE_REQ			6
#define			MASTER_LL_ENC_PAUSE_RSP_T		7
#define			MASTER_LL_ENC_PAUSE_RSP			8
#define			MASTER_LL_REJECT_IND_T			9
#define			MASTER_LL_ENC_SMP_INFO_S		10
#define			MASTER_LL_ENC_SMP_INFO_E		11


#define  		BLM_WINSIZE						4
#define  		BLM_WINOFFSET					5
#define  		BLM_MID_WINSIZE					2500
#define 		CONN_REQ_WAIT_ACK_NUM			6


#define 		TX_FIFO_DEFINED_IN_APP			1
#define 		STACK_FIFO_NUM					2  //user 6, stack 2
//#define			BLM_TX_FIFO_NUM				8
//#define			BLM_TX_FIFO_SIZE			40

typedef struct {
	u8		save_flg;
	u8		sn_nesn;
	u8 	    dma_tx_rptr;
	u8 		rsvd;
}bb_msts_t;

typedef struct {
#if (!TX_FIFO_DEFINED_IN_APP)
	u32		tx_fifo[BLM_TX_FIFO_NUM][BLM_TX_FIFO_SIZE>>2];
	u8		tx_wptr;
	u8		tx_rptr;
	u8		rsvd00;
	u8		rsvd01;
#endif
	u8		chn_tbl[40];

	u8 		newRx;
	u8		tx_num;
	u8		remoteFeatureReq;
	u8 		adv_filterPolicy;

	u8		conn_Req_waitAck_enable;
	u8		conn_terminate_reason;
	u8		slave_terminate_conn_flag;
	u8	 	master_terminate_conn_flag;

	u8		conn_terminate_pending;   // terminate_pending = master_terminate || slave_terminate
	u8		remote_version;
	u8		connParaUpReq_pending;
	u8		conn_sn; // softwre rcrd sn to filter retry rx data


	u8 		conn_btx_not_working;;
	u8		conn_rcvd_slave_pkt;
	u8		peer_adr_type;
	u8		rsvdd;

	u16		connHandle;
	u8		peer_adr[6];
	u8		macAddress_public[6];
	u8		macAddress_random[6];   //host may set this


	bb_msts_t blm_bb;// hardware rcrd sn nesn ect.

	u32 	ll_remoteFeature; 	 //feature mask <0:31>
	//u32   ll_remoteFeature_2;  //feature mask <32:63> for future use

	u32		conn_timeout;
	u32		conn_interval;
	u32		conn_software_timeout;

	u32		conn_winsize_next;
	u32		conn_timeout_next;
	u32		conn_offset_next;
	u32		conn_interval_next;

	u16		conn_latency;
	u16		conn_latency_next;


	u32		conn_Req_noAck_timeout;

	u16		enc_ediv;

	u32		enc_ivs;
	u8		enc_random[8];
	u8 		enc_skds[8];
	ble_crypt_para_t	crypt;

#if(BQB_5P0_TEST_ENABLE)
	u32		master_teminate_time;
#endif
} st_ll_conn_master_t;


extern 	int		blm_create_connection;

ble_sts_t	blm_ll_enc_proc_disconnect(u16 handle, u8 reason);
ble_sts_t	blm_hci_reset(void);

bool		blm_ll_deviceIsConnState (u8 addr_type, u8* mac_addr);
bool		blm_push_fifo (int connHandle, u8 *dat);
void		blm_main_loop (void);

int			blm_ll_startEncryption (u8 connhandle ,u16 ediv, u8* random, u8* ltk);
void		blm_ll_startDistributeKey (u8 connhandle );


int			blm_l2cap_packet_receive (u16 conn, u8 * raw_pkt);
ble_sts_t	blm_hci_receiveHostACLData(u16 connHandle, u8 PB_Flag, u8 BC_Flag, u8 *pData );

ble_sts_t	blm_ll_readRemoteFeature (u16 handle);

/******************************* ll_master end *************************************************************************/














/******************************* ll_conn_phy start *********************************************************************/


#if (LL_FEATURE_ENABLE_LE_2M_PHY | LL_FEATURE_ENABLE_LE_CODED_PHY)

ll_conn_phy_t*	blt_ll_get_conn_phy_ptr(u16 connHandle);
extern _attribute_aligned_(4) ll_conn_phy_t blt_conn_phy;


typedef int (*ll_conn_phy_update_callback_t)(void);
typedef int (*ll_conn_phy_switch_callback_t)(void);

extern 	ll_conn_phy_update_callback_t	ll_conn_phy_update_cb;
extern 	ll_conn_phy_switch_callback_t	ll_conn_phy_swicth_cb;

#endif




//2M/Coded PHY
void 		blt_ll_sendPhyReq(void);
int 		blt_ll_updateConnPhy(void);
int 		blt_ll_switchConnPhy(void);


/******************************* ll_conn_phy end ***********************************************************************/






/******************************* ll_conn_csa start *********************************************************************/

//See the Core_v5.0(Vol 6/Part B/4.5.8, "Data Channel Index Selection") for more information.
typedef enum {
	CHANNAL_SELECTION_ALGORITHM_1      	=	0x00,
	CHANNAL_SELECTION_ALGORITHM_2      	=	0x01,
} channel_algorithm_t;




typedef u8 (*ll_chn_index_calc_callback_t)(u8*, u16, u16);
extern ll_chn_index_calc_callback_t	ll_chn_index_calc_cb;



u8	blc_calc_remapping_table(u8 chm[5]);

u8	blc_ll_channel_index_calc_csa2(u8 chm[5], u16 event_cntr, u16 channel_id);


/******************************* ll_conn_csa end ***********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* STACK_BLE_LL_LL_CONN_CONN_STACK_H_ */
