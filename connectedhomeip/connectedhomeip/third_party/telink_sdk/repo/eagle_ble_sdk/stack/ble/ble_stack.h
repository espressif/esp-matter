/********************************************************************************************************
 * @file	ble_stack.h
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
#ifndef BLE_STACK_H_
#define BLE_STACK_H_




/**
 *  @brief  Definition for Link Layer Feature Support
 */

#define LL_FEATURE_MASK_LL_ENCRYPTION                        			BIT(0) //core_4.0
#define LL_FEATURE_MASK_CONNECTION_PARA_REQUEST_PROCEDURE  	 			BIT(1) //core_4.1
#define LL_FEATURE_MASK_EXTENDED_REJECT_INDICATION           			BIT(2) //core_4.1
#define LL_FEATURE_MASK_SLAVE_INITIATED_FEATURES_EXCHANGE    			BIT(3) //core_4.1
#define LL_FEATURE_MASK_LE_PING                             	 		BIT(4) //core_4.1
#define LL_FEATURE_MASK_LE_DATA_LENGTH_EXTENSION             			BIT(5) //core_4.2
#define LL_FEATURE_MASK_LL_PRIVACY                           			BIT(6) //core_4.2
#define LL_FEATURE_MASK_EXTENDED_SCANNER_FILTER_POLICIES   	 			BIT(7) //core_4.2

#define LL_FEATURE_MASK_LE_2M_PHY         					 			BIT(8)  //core_5.0
#define LL_FEATURE_MASK_STABLE_MODULATION_INDEX_TX 			 			BIT(9)	//core_5.0
#define LL_FEATURE_MASK_STABLE_MODULATION_INDEX_RX 			 			BIT(10)	//core_5.0
#define LL_FEATURE_MASK_LE_CODED_PHY     					 			BIT(11)	//core_5.0
#define LL_FEATURE_MASK_LE_EXTENDED_ADVERTISING          	 			BIT(12)	//core_5.0
#define LL_FEATURE_MASK_LE_PERIODIC_ADVERTISING     		 			BIT(13)	//core_5.0
#define LL_FEATURE_MASK_CHANNEL_SELECTION_ALGORITHM2         			BIT(14)	//core_5.0
#define LL_FEATURE_MASK_LE_POWER_CLASS_1 					 			BIT(15)	//core_5.0
#define LL_FEATURE_MASK_MIN_USED_OF_USED_CHANNELS   	    	 		BIT(16)	//core_5.0

#define LL_FEATURE_MASK_CONNECTION_CTE_REQUEST  	    				BIT(17)	//core_5.1
#define LL_FEATURE_MASK_CONNECTION_CTE_RESPONSE  	    	 			BIT(18)	//core_5.1
#define LL_FEATURE_MASK_CONNECTIONLESS_CTE_TRANSMITTER  	 			BIT(19)	//core_5.1
#define LL_FEATURE_MASK_CONNECTIONLESS_CTE_RECEIVER  	     			BIT(20)	//core_5.1
#define LL_FEATURE_MASK_ANTENNA_SWITCHING_DURING_CTE_TRANSMISSION  	 	BIT(21)	//core_5.1
#define LL_FEATURE_MASK_ANTENNA_SWITCHING_DURING_CTE_RECEPTION  	 	BIT(22)	//core_5.1
#define LL_FEATURE_MASK_RECEIVING_CONSTANT_TONE_EXTENSIONS  	 		BIT(23)	//core_5.1
#define LL_FEATURE_MASK_PERIODIC_ADVERTISING_SYNC_TRANSFER_SENDOR  	 	BIT(24)	//core_5.1
#define LL_FEATURE_MASK_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECIPIENT  	BIT(25)	//core_5.1
#define LL_FEATURE_MASK_SLEEP_CLOCK_ACCURACY_UPDATES		   	 	    BIT(26)	//core_5.1
#define LL_FEATURE_MASK_REMOTE_PUBLIC_KEY_VALIDATION  					BIT(27)	//core_5.1

#define LL_FEATURE_MASK_CONNECTED_ISOCHRONOUS_STREAM_MASTER  			BIT(28)	//core_5.2
#define LL_FEATURE_MASK_CONNECTED_ISOCHRONOUS_STREAM_SLAVE  			BIT(29)	//core_5.2
#define LL_FEATURE_MASK_ISOCHRONOUS_BROADCASTER  						BIT(30)	//core_5.2
#define LL_FEATURE_MASK_SYNCHRONIZED_RECEIVER	  						BIT(31)	//core_5.2
#define LL_FEATURE_MASK_ISOCHRONOUS_CHANNELS	  						BIT(0)	//core_5.2
#define LL_FEATURE_MASK_LE_POWER_CTRL_REQUEST	  						BIT(1)	//core_5.2
#define LL_FEATURE_MASK_LE_POWER_CHANGE_INDICATION	  					BIT(2)	//core_5.2
#define LL_FEATURE_MASK_LE_PATH_LOSS_MONITORING	  						BIT(3)	//core_5.2




/////////////////////////////////////////////////////////////////////////////

#define         VENDOR_ID                       0x0211
#define         VENDOR_ID_HI_B                  U16_HI(VENDOR_ID)
#define         VENDOR_ID_LO_B                  U16_LO(VENDOR_ID)

#define			BLUETOOTH_VER_4_0				6
#define			BLUETOOTH_VER_4_1				7
#define			BLUETOOTH_VER_4_2				8
#define			BLUETOOTH_VER_5_0				9
#define			BLUETOOTH_VER_5_1				10
#define			BLUETOOTH_VER_5_2				11

#ifndef 		BLUETOOTH_VER
#define			BLUETOOTH_VER					BLUETOOTH_VER_5_0
#endif


#if (BLUETOOTH_VER == BLUETOOTH_VER_4_2)
	#define			BLUETOOTH_VER_SUBVER			0x22BB
#elif (BLUETOOTH_VER == BLUETOOTH_VER_5_0)
	#define			BLUETOOTH_VER_SUBVER			0x1C1C
#else
	#define			BLUETOOTH_VER_SUBVER			0x4103
#endif




#if (BLUETOOTH_VER == BLUETOOTH_VER_4_0)
	#define LL_FEATURE_ENABLE_LE_ENCRYPTION								1
	#define LL_CMD_MAX						   							LL_REJECT_IND

#elif (BLUETOOTH_VER == BLUETOOTH_VER_4_1)
	#define LL_FEATURE_ENABLE_LE_ENCRYPTION								1
	#define	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION				1
	#define	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE			1
	#define	LL_FEATURE_ENABLE_LE_PING									1

	#define LL_CMD_MAX						   							LL_PING_RSP

#elif (BLUETOOTH_VER == BLUETOOTH_VER_4_2)

	#define LL_FEATURE_ENABLE_LE_ENCRYPTION								1
	#define	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION				1
	#define	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE			1
	#define	LL_FEATURE_ENABLE_LE_PING									1
	#define	LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION					BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE
	#define	LL_FEATURE_ENABLE_LL_PRIVACY								0

	#define LL_CMD_MAX						  							LL_LENGTH_RSP

#elif (BLUETOOTH_VER == BLUETOOTH_VER_5_0)

	#define LL_FEATURE_ENABLE_LE_ENCRYPTION								1
	#define	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION				1
	#define	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE			1
	#define	LL_FEATURE_ENABLE_LE_PING									1
	#define	LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION					BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE
	#define	LL_FEATURE_ENABLE_LL_PRIVACY								0

	#define	LL_FEATURE_ENABLE_LE_2M_PHY									LL_FEATURE_SUPPORT_LE_2M_PHY
	#define	LL_FEATURE_ENABLE_LE_CODED_PHY								LL_FEATURE_SUPPORT_LE_CODED_PHY
	#define	LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING					LL_FEATURE_SUPPORT_LE_EXTENDED_ADVERTISING
	#define	LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING					LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING
	#define	LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2				LL_FEATURE_SUPPORT_CHANNEL_SELECTION_ALGORITHM2

	#define	LL_FEATURE_ENABLE_LE_EXTENDED_SCAN							0 //vendor define
	#define LL_CMD_MAX						   							LL_MIN_USED_CHN_IND

	

#elif (BLUETOOTH_VER == BLUETOOTH_VER_5_1)

#elif (BLUETOOTH_VER == BLUETOOTH_VER_5_2)

	#define LL_FEATURE_ENABLE_LE_ENCRYPTION								1
	#define	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION				1
	#define	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE			1
	#define	LL_FEATURE_ENABLE_LE_PING									1
	#define	LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION					BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE
	#define	LL_FEATURE_ENABLE_LE_2M_PHY									LL_FEATURE_SUPPORT_LE_2M_PHY
	#define	LL_FEATURE_ENABLE_LE_CODED_PHY								LL_FEATURE_SUPPORT_LE_CODED_PHY
	#define	LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING					LL_FEATURE_SUPPORT_LE_EXTENDED_ADVERTISING
	#define	LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING					LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING
	#define	LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2				LL_FEATURE_SUPPORT_CHANNEL_SELECTION_ALGORITHM2

	#define	LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_MASTER		LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_MASTER
	#define	LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_SLAVE		LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_SLAVE
	#define	LL_FEATURE_ENABLE_ISOCHRONOUS_BROADCASTER					LL_FEATURE_SUPPORT_ISOCHRONOUS_BROADCASTER
	#define	LL_FEATURE_ENABLE_SYNCHRONIZED_RECEIVER						LL_FEATURE_SUPPORT_SYNCHRONIZED_RECEIVER
	#define	LL_FEATURE_ENABLE_ISOCHRONOUS_CHANNELS						LL_FEATURE_SUPPORT_ISOCHRONOUS_CHANNELS


	#define LL_FEATURE_ENABLE_CONNECTED_ISO								(     LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_MASTER \
																			| LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_SLAVE     )

	#define LL_FEATURE_ENABLE_CONNECTIONLESS_ISO  						(	  LL_FEATURE_ENABLE_ISOCHRONOUS_BROADCASTER				\
																			| LL_FEATURE_ENABLE_SYNCHRONIZED_RECEIVER )

	#define LL_FEATURE_ENABLE_ISO										(     LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_MASTER \
																			| LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_SLAVE \
																			| LL_FEATURE_ENABLE_ISOCHRONOUS_BROADCASTER				\
																			| LL_FEATURE_ENABLE_SYNCHRONIZED_RECEIVER )

	#define	LL_FEATURE_ENABLE_LE_EXTENDED_SCAN							0  //Vendor define

	#define LL_CMD_MAX						   							LL_POWER_CHANGE_IND

	#define LL_FEATURE_BIT_NUMBER_ISOCHRONOUS_CHANNELS					32

#else


#endif




#ifndef		 LL_FEATURE_ENABLE_LE_ENCRYPTION
#define		 LL_FEATURE_ENABLE_LE_ENCRYPTION							0
#endif

#ifndef		 LL_FEATURE_ENABLE_CONNECTION_PARA_REQUEST_PROCEDURE
#define		 LL_FEATURE_ENABLE_CONNECTION_PARA_REQUEST_PROCEDURE		0
#endif

#ifndef		 LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION
#define		 LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION				0
#endif

#ifndef		 LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE
#define		 LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE		0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_PING
#define		 LL_FEATURE_ENABLE_LE_PING									0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION
#define		 LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION					0
#endif

#ifndef		 LL_FEATURE_ENABLE_LL_PRIVACY
#define		 LL_FEATURE_ENABLE_LL_PRIVACY								0
#endif

#ifndef		 LL_FEATURE_ENABLE_EXTENDED_SCANNER_FILTER_POLICIES
#define		 LL_FEATURE_ENABLE_EXTENDED_SCANNER_FILTER_POLICIES			0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_2M_PHY
#define		 LL_FEATURE_ENABLE_LE_2M_PHY								0
#endif

#ifndef		 LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_TX
#define		 LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_TX				0
#endif

#ifndef		 LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_RX
#define		 LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_RX				0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_CODED_PHY
#define		 LL_FEATURE_ENABLE_LE_CODED_PHY								0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING
#define		 LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING					0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING
#define		 LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING					0
#endif

#ifndef		 LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2
#define		 LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2				0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_POWER_CLASS_1
#define		 LL_FEATURE_ENABLE_LE_POWER_CLASS_1							0
#endif

#ifndef		 LL_FEATURE_ENABLE_MIN_USED_OF_USED_CHANNELS
#define		 LL_FEATURE_ENABLE_MIN_USED_OF_USED_CHANNELS				0
#endif


#ifndef		 LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_MASTER
#define		 LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_MASTER		0
#endif

#ifndef		 LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_SLAVE
#define		 LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_SLAVE		0
#endif

#ifndef		 LL_FEATURE_ENABLE_ISOCHRONOUS_BROADCASTER
#define		 LL_FEATURE_ENABLE_ISOCHRONOUS_BROADCASTER					0
#endif

#ifndef		 LL_FEATURE_ENABLE_SYNCHRONIZED_RECEIVER
#define		 LL_FEATURE_ENABLE_SYNCHRONIZED_RECEIVER					0
#endif

#ifndef		 LL_FEATURE_ENABLE_ISOCHRONOUS_CHANNELS
#define		 LL_FEATURE_ENABLE_ISOCHRONOUS_CHANNELS						0
#endif





//BIT<0:31>
// feature below is conFiged by application layer or HCI command
// <8>  : LL_FEATURE_ENABLE_LE_2M_PHY
// <11> : LL_FEATURE_ENABLE_LE_CODED_PHY
// <12> : LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING
// <13> : LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING
// <14> : LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2

// <28> : LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_MASTER
// <29> : LL_FEATURE_ENABLE_CONNECTED_ISOCHRONOUS_STREAM_SLAVE
// <30> : LL_FEATURE_ENABLE_ISOCHRONOUS_BROADCASTER
// <31> : LL_FEATURE_ENABLE_SYNCHRONIZED_RECEIVER

#define LL_FEATURE_MASK_BASE0											(	LL_FEATURE_ENABLE_LE_ENCRYPTION 					<<0		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_CONNECTION_PARA_REQUEST_PROCEDURE <<1		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION 	   	<<2		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE <<3		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_PING 						   	<<4		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION 		   	<<5		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LL_PRIVACY 					   	<<6		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_EXTENDED_SCANNER_FILTER_POLICIES  <<7		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_TX 	   	<<9		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_RX  	   	<<10	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING 		   	<<13	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_POWER_CLASS_1 				   	<<15	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_MIN_USED_OF_USED_CHANNELS 		<<16 	)


//BIT<32:63>
// feature below is conFiged by application layer or HCI command
// <32> :	LL_FEATURE_ENABLE_ISOCHRONOUS_CHANNELS
#define LL_FEATURE_MASK_BASE1											0



extern u32 LL_FEATURE_MASK_0;
extern u32 LL_FEATURE_MASK_1;



#define LL_FEATURE_BYTE_0												U32_BYTE0(LL_FEATURE_MASK_0)
#define LL_FEATURE_BYTE_1												U32_BYTE1(LL_FEATURE_MASK_0)
#define LL_FEATURE_BYTE_2												U32_BYTE2(LL_FEATURE_MASK_0)
#define LL_FEATURE_BYTE_3												U32_BYTE3(LL_FEATURE_MASK_0)
#define LL_FEATURE_BYTE_4												U32_BYTE0(LL_FEATURE_MASK_1)
#define LL_FEATURE_BYTE_5												U32_BYTE1(LL_FEATURE_MASK_1)
#define LL_FEATURE_BYTE_6												U32_BYTE2(LL_FEATURE_MASK_1)
#define LL_FEATURE_BYTE_7												U32_BYTE3(LL_FEATURE_MASK_1)










/******************************************** Link Layer **************************************************************/
#define 				BLE_T_IFS					150	//unit: uS
#define 				BLE_T_MSS					150	//unit: uS


/**
 *  @brief  Definition for LLID of Data Physical Channel PDU header field
 */
#define 				LLID_RESERVED				0x00
#define 				LLID_DATA_CONTINUE			0x01
#define 				LLID_DATA_START				0x02
#define 				LLID_CONTROL				0x03










//Extended Header BIT
#define 		EXTHD_BIT_ADVA         							BIT(0)
#define 		EXTHD_BIT_TARGETA      							BIT(1)
#define			EXTHD_BIT_CTE_INFO         						BIT(2)
#define 		EXTHD_BIT_ADI		    						BIT(3)
#define 		EXTHD_BIT_AUX_PTR      							BIT(4)
#define 		EXTHD_BIT_SYNC_INFO	    						BIT(5)
#define 		EXTHD_BIT_TX_POWER	     						BIT(6)


//Extended Header Length
#define			EXTHD_LEN_6_ADVA								6
#define			EXTHD_LEN_6_TARGETA								6
#define			EXTHD_LEN_1_CTE									1
#define			EXTHD_LEN_2_ADI									2
#define			EXTHD_LEN_3_AUX_PTR								3
#define			EXTHD_LEN_18_SYNC_INFO							18
#define			EXTHD_LEN_1_TX_POWER							1


#define 		LL_EXTADV_MODE_NON_CONN_NON_SCAN    			(0x00)
#define 		LL_EXTADV_MODE_CONN        						(0x01)		//connectable, none_scannable
#define 		LL_EXTADV_MODE_SCAN        						(0x02)      //scannable,   none_connectable




#define			EXT_ADV_PDU_AUXPTR_OFFSET_UNITS_30_US			0
#define			EXT_ADV_PDU_AUXPTR_OFFSET_UNITS_300_US			1

#define			EXT_ADV_PDU_AUXPTR_CA_51_500_PPM				0
#define			EXT_ADV_PDU_AUXPTR_CA_0_50_PPM					1








// Advertise channel PDU Type
typedef enum advChannelPDUType_e {
	LL_TYPE_ADV_IND 		 = 0x00,
	LL_TYPE_ADV_DIRECT_IND 	 = 0x01,
	LL_TYPE_ADV_NONCONN_IND  = 0x02,
	LL_TYPE_SCAN_REQ 		 = 0x03,		LL_TYPE_AUX_SCAN_REQ 	 = 0x03,
	LL_TYPE_SCAN_RSP 		 = 0x04,
	LL_TYPE_CONNNECT_REQ 	 = 0x05,		LL_TYPE_AUX_CONNNECT_REQ = 0x05,
	LL_TYPE_ADV_SCAN_IND 	 = 0x06,

	LL_TYPE_ADV_EXT_IND		 = 0x07,		LL_TYPE_AUX_ADV_IND 	 = 0x07,	LL_TYPE_AUX_SCAN_RSP = 0x07,	LL_TYPE_AUX_SYNC_IND = 0x07,	LL_TYPE_AUX_CHAIN_IND = 0x07,
	LL_TYPE_AUX_CONNNECT_RSP = 0x08,
} advChannelPDUType_t;




/**
 *  @brief  Definition for LLID of Connected Isochronous PDU header field
 */
typedef enum{
	CIS_LLID_UNFRAMED_PDU_END_FRAGMENT_SDU			= 0x00, //Unframed CIS Data PDU; end fragment of an SDU or a complete SDU
	CIS_LLID_UNFRAMED_PDU_START_CONTI_FRAGMENT_SDU  = 0x01,	//Unframed CIS Data PDU; start or continuation fragment of an SDU
	CIS_LLID_FRAMED_PDU_SEGMENT_SDU					= 0x02, //Framed CIS Data PDU; one or more segments of an SDU
	CIS_LLID_RESERVED 								= 0x03,
}cis_llid_type;




/**
 *  @brief  Definition for BIG Control PDU Opcode
 */
typedef enum{
	BIG_CHANNEL_MAP_IND	= 0x00,
	BIG_TERMINATE_IND	= 0x01,
}big_ctrl_op;





/******************************************** L2CAP **************************************************************/

// l2cap pb flag type
#define L2CAP_FRIST_PKT_H2C              0x00
#define L2CAP_CONTINUING_PKT             0x01
#define L2CAP_FIRST_PKT_C2H              0x02







/**
 *  @brief  Definition for Error Response of signal packet
 *  See the Core_v5.0(Vol 3/Part A/4.1) for more information.
 */
typedef enum{
	SIG_CMD_NOT_UNDERSTAND	= 0,
	SIG_MTU_EXCEEDED		= 1,
	SIG_INVALID_CID_REQUEST	= 2,
}l2cap_sig_cmd_reject_reason;

//Result values for the L2CAP_CREDIT_BASED_CONNECTION_RSP packet
typedef enum{
	L2CAP_ALL_CONN_SUCESSFULL=0,
	L2CAP_ALL_CONN_REFUSED_SPSM_NOT_SUPPORT=2,
	L2CAP_SOME_CONN_REFUSED_INSUFFICIENT_RESOURCES_AVAILABLE=4,
	L2CAP_ALL_CONN_REFUSED_INSUFFICIENT_AUTHENTICATION=5,
	L2CAP_ALL_CONN_REFUSED_INSUFFICIENT_AUTHORIZATION=6,
	L2CAP_ALL_CONN_REFUSED_INSUFFICIENT_ENCRYPTION_KEY_SIZE=7,
	L2CAP_ALL_CONN_REFUSED_INSUFFICIENT_ENCRYPTION=8,
	L2CAP_SOME_CONN_REFUSED_INVALID_SOURCE_CID=9,
	L2CAP_SOME_CONN_REFUSED_SOURCE_CID_ALREADY_ALLOCATED=0x0a,
	L2CAP_ALL_CONN_REFUSED_UNACCEPTABLE_PARAMETERS=0x0b,
	L2CAP_ALL_CONN_REFUSED_INVALID_PARAMETERS=0x0c,
}l2cap_credit_based_conn_rsp_result;
typedef enum{
	L2CAP_RECONFIG_SUCCESSFUL = 0,
	L2CAP_RECONFIG_FAIL_REDUCTION_MTU = 1,
	L2CAP_RECONFIG_FAIL_REDUCTION_MPS = 2,
	L2CAP_RECONFIG_FAIL_ONE_MORE_DCID_INVALID = 3,
	L2CAP_RECONFIG_FAIL_OTHER_UNACCEPTABLE_PARAMETERS = 4,
}l2cap_credit_based_reconfigure_rsp_result;


/******************************************** ATT ***************************************************************/





/**
 *  @brief  Definition for Error Response of ATTRIBUTE PROTOCOL PDUS
 *  See the Core_v5.0(Vol 3/Part F/3.4.1.1, "Error Response") for more information.
 */
typedef enum {

	ATT_SUCCESS = 0,

    ATT_ERR_INVALID_HANDLE,                              //!< The attribute handle given was not valid on this server
    ATT_ERR_READ_NOT_PERMITTED,                          //!< The attribute cannot be read
    ATT_ERR_WRITE_NOT_PERMITTED,                         //!< The attribute cannot be written
    ATT_ERR_INVALID_PDU,                                 //!< The attribute PDU was invalid
    ATT_ERR_INSUFFICIENT_AUTH,                           //!< The attribute requires authentication before it can be read or written
    ATT_ERR_REQ_NOT_SUPPORTED,                           //!< Attribute server does not support the request received from the client
    ATT_ERR_INVALID_OFFSET,                              //!< Offset specified was past the end of the attribute
    ATT_ERR_INSUFFICIENT_AUTHOR,                         //!< The attribute requires authorization before it can be read or written
    ATT_ERR_PREPARE_QUEUE_FULL,                          //!< Too many prepare writes have been queued
    ATT_ERR_ATTR_NOT_FOUND,                              //!< No attribute found within the given attribute handle range
    ATT_ERR_ATTR_NOT_LONG,                               //!< The attribute cannot be read or written using the Read Blob Request
    ATT_ERR_INSUFFICIENT_KEY_SIZE,                       //!< The Encryption Key Size used for encrypting this link is insufficient
    ATT_ERR_INVALID_ATTR_VALUE_LEN,                      //!< The attribute value length is invalid for the operation
    ATT_ERR_UNLIKELY_ERR,                                //!< The attribute request that was requested has encountered an error that was unlikely, and therefore could not be completed as requested
    ATT_ERR_INSUFFICIENT_ENCRYPT,                        //!< The attribute requires encryption before it can be read or written
    ATT_ERR_UNSUPPORTED_GRP_TYPE,                        //!< The attribute type is not a supported grouping attribute as defined by a higher layer specification
    ATT_ERR_INSUFFICIENT_RESOURCES,                      //!< Insufficient Resources to complete the request



}att_err_t;

#ifdef __cplusplus
extern "C" {
#endif

void smemset(void * dest, int val, int len);
void smemcpy(void *pd, void *ps, int len);
int smemcmp(void * m1, void * m2, int len);

#ifdef __cplusplus
}
#endif

#endif /* BLE_STACK_H_ */
