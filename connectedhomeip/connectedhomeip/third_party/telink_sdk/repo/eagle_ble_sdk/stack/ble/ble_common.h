/********************************************************************************************************
 * @file	ble_common.h
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
#ifndef BLE_COMMON_H
#define BLE_COMMON_H

#include "ble_config.h"
#include "ble_stack.h"
#include "tl_common.h"



typedef enum {
    BLE_SUCCESS = 0,

//// HCI Status, See the Core_v5.0(Vol 2/Part D/1.3 "list of Error Codes") for more information)
    HCI_ERR_UNKNOWN_HCI_CMD                                        = 0x01,
    HCI_ERR_UNKNOWN_CONN_ID                                        = 0x02,
    HCI_ERR_HW_FAILURE                                             = 0x03,
    HCI_ERR_PAGE_TIMEOUT                                           = 0x04,
    HCI_ERR_AUTH_FAILURE                                           = 0x05,
    HCI_ERR_PIN_KEY_MISSING                                        = 0x06,
    HCI_ERR_MEM_CAP_EXCEEDED                                       = 0x07,
    HCI_ERR_CONN_TIMEOUT                                           = 0x08,
    HCI_ERR_CONN_LIMIT_EXCEEDED                                    = 0x09,
    HCI_ERR_SYNCH_CONN_LIMIT_EXCEEDED                              = 0x0A,
    HCI_ERR_ACL_CONN_ALREADY_EXISTS                                = 0x0B,
    HCI_ERR_CMD_DISALLOWED                                         = 0x0C,
    HCI_ERR_CONN_REJ_LIMITED_RESOURCES                             = 0x0D,
    HCI_ERR_CONN_REJECTED_SECURITY_REASONS                         = 0x0E,
    HCI_ERR_CONN_REJECTED_UNACCEPTABLE_BDADDR                      = 0x0F,
    HCI_ERR_CONN_ACCEPT_TIMEOUT_EXCEEDED                           = 0x10,
    HCI_ERR_UNSUPPORTED_FEATURE_PARAM_VALUE                        = 0x11,
    HCI_ERR_INVALID_HCI_CMD_PARAMS                                 = 0x12,
    HCI_ERR_REMOTE_USER_TERM_CONN                                  = 0x13,
    HCI_ERR_REMOTE_DEVICE_TERM_CONN_LOW_RESOURCES                  = 0x14,
    HCI_ERR_REMOTE_DEVICE_TERM_CONN_POWER_OFF                      = 0x15,
    HCI_ERR_CONN_TERM_BY_LOCAL_HOST                                = 0x16,
    HCI_ERR_REPEATED_ATTEMPTS                                      = 0x17,
    HCI_ERR_PAIRING_NOT_ALLOWED                                    = 0x18,
    HCI_ERR_UNKNOWN_LMP_PDU                                        = 0x19,
    HCI_ERR_UNSUPPORTED_REMOTE_FEATURE                             = 0x1A,
    HCI_ERR_SCO_OFFSET_REJ                                         = 0x1B,
    HCI_ERR_SCO_INTERVAL_REJ                                       = 0x1C,
    HCI_ERR_SCO_AIR_MODE_REJ                                       = 0x1D,
    HCI_ERR_INVALID_LMP_PARAMS                                     = 0x1E,
    HCI_ERR_UNSPECIFIED_ERROR                                      = 0x1F,
    HCI_ERR_UNSUPPORTED_LMP_PARAM_VAL                              = 0x20,
    HCI_ERR_ROLE_CHANGE_NOT_ALLOWED                                = 0x21,
    HCI_ERR_LMP_LL_RESP_TIMEOUT                                    = 0x22,
    HCI_ERR_LMP_ERR_TRANSACTION_COLLISION                          = 0x23,
    HCI_ERR_LMP_PDU_NOT_ALLOWED                                    = 0x24,
    HCI_ERR_ENCRYPT_MODE_NOT_ACCEPTABLE                            = 0x25,
    HCI_ERR_LINK_KEY_CAN_NOT_BE_CHANGED                            = 0x26,
    HCI_ERR_REQ_QOS_NOT_SUPPORTED                                  = 0x27,
    HCI_ERR_INSTANT_PASSED                                         = 0x28,
    HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED                    = 0x29,
    HCI_ERR_DIFFERENT_TRANSACTION_COLLISION                        = 0x2A,
    HCI_ERR_RESERVED1                                              = 0x2B,
    HCI_ERR_QOS_UNACCEPTABLE_PARAM                                 = 0x2C,
    HCI_ERR_QOS_REJ                                                = 0x2D,
    HCI_ERR_CHAN_ASSESSMENT_NOT_SUPPORTED                          = 0x2E,
    HCI_ERR_INSUFFICIENT_SECURITY                                  = 0x2F,
    HCI_ERR_PARAM_OUT_OF_MANDATORY_RANGE                           = 0x30,
    HCI_ERR_RESERVED2                                              = 0x31,
    HCI_ERR_ROLE_SWITCH_PENDING                                    = 0x32,
    HCI_ERR_RESERVED3                                              = 0x33,
    HCI_ERR_RESERVED_SLOT_VIOLATION                                = 0x34,
    HCI_ERR_ROLE_SWITCH_FAILED                                     = 0x35,
    HCI_ERR_EXTENDED_INQUIRY_RESP_TOO_LARGE                        = 0x36,
    HCI_ERR_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST                   = 0x37,
    HCI_ERR_HOST_BUSY_PAIRING                                      = 0x38,
    HCI_ERR_CONN_REJ_NO_SUITABLE_CHAN_FOUND                        = 0x39,
    HCI_ERR_CONTROLLER_BUSY                                        = 0x3A,
    HCI_ERR_UNACCEPTABLE_CONN_INTERVAL                             = 0x3B,
    HCI_ERR_ADVERTISING_TIMEOUT                                    = 0x3C,
    HCI_ERR_CONN_TERM_MIC_FAILURE                                  = 0x3D,
    HCI_ERR_CONN_FAILED_TO_ESTABLISH                               = 0x3E,
    HCI_ERR_MAC_CONN_FAILED                                        = 0x3F,
    HCI_ERR_COARSE_CLOCK_ADJUSTMENT_REJECT						   = 0x40,
    HCI_ERR_TYPE0_SUBMAP_NOT_DEFINED							   = 0x41,
	HCI_ERR_UNKNOWN_ADV_IDENTIFIER								   = 0x42,
    HCI_ERR_LIMIT_REACHED										   = 0x43,
    HCI_ERR_OP_CANCELLED_BY_HOST								   = 0x44,
    HCI_ERR_PACKET_TOO_LONG										   = 0x45,
    




///////////////////////// TELINK define status /////////////////////////////

    //LL status
	LL_ERR_CONNECTION_NOT_ESTABLISH 							   = 0x80,
	LL_ERR_TX_FIFO_NOT_ENOUGH,
	LL_ERR_ENCRYPTION_BUSY,
	LL_ERR_CURRENT_STATE_NOT_SUPPORTED_THIS_CMD,
	LL_ERR_INVALID_PARAMETER,
	LL_ERR_UNKNOWN_OPCODE,

	LL_ERR_CIS_SYNC_FAIL,
	LL_ERR_CIS_DISCONNECT,


	//L2CAP status
    L2CAP_ERR_INVALID_PARAMETER 								   = 0x90,
    L2CAP_ERR_INVALID_HANDLE,
    L2CAP_ERR_INSUFFICIENT_RESOURCES,
    L2CAP_ERR_PSM_NOT_REGISTER,
    L2CAP_ERR_CONTROL_NOT_READY,
    L2CAP_ERR_PSM_HAVE_ESTABLISH,

    //SMP status
	SMP_ERR_INVALID_PARAMETER 									   = 0xA0,
	SMP_ERR_PAIRING_BUSY,

	//GATT status
	GATT_ERR_INVALID_PARAMETER 									   = 0xB0,
	GATT_ERR_PREVIOUS_INDICATE_DATA_HAS_NOT_CONFIRMED,
	GATT_ERR_SERVICE_DISCOVERY_TIEMOUT,
	GATT_ERR_NOTIFY_INDICATION_NOT_PERMITTED,
	GATT_ERR_DATA_PENDING_DUE_TO_SERVICE_DISCOVERY_BUSY,
	GATT_ERR_DATA_LENGTH_EXCEED_MTU_SIZE,

	//GAP status
	GAP_ERR_INVALID_PARAMETER 								   	   = 0xC0,
	//IAL
	IAL_ERR_SDU_LEN_EXCEED_SDU_MAX,
	IAL_ERR_LOSS_SDU_INTRVEL,
	IAL_ERR_ISO_TX_FIFO_NOT_ENOUGH,
	IAL_ERR_SDU_BUFF_INVALID,

	//Service status
	SERVICE_ERR_INVALID_PARAMETER 								   = 0xD0,

	//Application buffer check error code
	LL_ACL_RX_BUF_NO_INIT 							   	  		   = 0xE0,
	LL_ACL_RX_BUF_PARAM_INVALID,
	LL_ACL_RX_BUF_SIZE_NOT_MEET_MAX_RX_OCT,
	LL_ACL_TX_BUF_NO_INIT,
	LL_ACL_TX_BUF_PARAM_INVALID,
	LL_ACL_TX_BUF_SIZE_MUL_NUM_EXCEED_4K,
	LL_ACL_TX_BUF_SIZE_NOT_MEET_MAX_TX_OCT,

} ble_sts_t;











/////////////////////////////// BLE  MAC ADDRESS //////////////////////////////////////////////
#define BLE_ADDR_PUBLIC                  0
#define BLE_ADDR_RANDOM                  1
#define BLE_ADDR_INVALID                 0xff
#define BLE_ADDR_LEN                     6


//Definition for BLE Common Address Type
/*
 *
 *				  |--public  ..................................................... BLE_DEVICE_ADDRESS_PUBLIC
 *                |
 * Address Type --|		      |-- random static  ................................. BLE_DEVICE_ADDRESS_RANDOM_STATIC
 *           	  |           |
 *    			  |--random --|
 * 			   				  |				       |-- non_resolvable private  ... BLE_DEVICE_ADDRESS_NON_RESOLVABLE_PRIVATE
 * 			 				  |-- random private --|
 *           					                   |-- resolvable private  ....... BLE_DEVICE_ADDRESS_RESOLVABLE_PRIVATE
 *
 */

#define	BLE_DEVICE_ADDRESS_PUBLIC							1
#define BLE_DEVICE_ADDRESS_RANDOM_STATIC					2
#define BLE_DEVICE_ADDRESS_NON_RESOLVABLE_PRIVATE			3
#define BLE_DEVICE_ADDRESS_RESOLVABLE_PRIVATE				4



#define IS_PUBLIC_ADDR(Type, Addr)  					( (Type)==BLE_ADDR_PUBLIC) )
#define IS_RANDOM_STATIC_ADDR(Type, Addr)  				( (Type)==BLE_ADDR_RANDOM && (Addr[5] & 0xC0) == 0xC0 )
#define IS_NON_RESOLVABLE_PRIVATE_ADDR(Type, Addr)  	( (Type)==BLE_ADDR_RANDOM && (Addr[5] & 0xC0) == 0x00 )
#define IS_RESOLVABLE_PRIVATE_ADDR(Type, Addr)  		( (Type)==BLE_ADDR_RANDOM && (Addr[5] & 0xC0) == 0x40 )


#define		MAC_MATCH8(md,ms)	(md[0]==ms[0] && md[1]==ms[1] && md[2]==ms[2] && md[3]==ms[3] && md[4]==ms[4] && md[5]==ms[5])
#define		MAC_MATCH16(md,ms)	(md[0]==ms[0] && md[1]==ms[1] && md[2]==ms[2])
#define		MAC_MATCH32(md,ms)	(md[0]==ms[0] && md[1]==ms[1])
/////////////////////////////////////////////////////////////////////////////

/******************************************** ATT ***************************************************************/

/**
 *  @brief  Definition for Attribute protocol PDUs
 */
typedef enum{
	ATT_OP_ERROR_RSP					= 0x01,
	ATT_OP_EXCHANGE_MTU_REQ				= 0x02,
	ATT_OP_EXCHANGE_MTU_RSP				= 0x03,
	ATT_OP_FIND_INFORMATION_REQ			= 0x04,		ATT_OP_FIND_INFO_REQ = 0x04,
	ATT_OP_FIND_INFORMATION_RSP			= 0x05,		ATT_OP_FIND_INFO_RSP = 0x05,
	ATT_OP_FIND_BY_TYPE_VALUE_REQ		= 0x06,
	ATT_OP_FIND_BY_TYPE_VALUE_RSP		= 0x07,
	ATT_OP_READ_BY_TYPE_REQ				= 0x08,
	ATT_OP_READ_BY_TYPE_RSP				= 0x09,
	ATT_OP_READ_REQ						= 0x0A,
	ATT_OP_READ_RSP						= 0x0B,
	ATT_OP_READ_BLOB_REQ				= 0x0C,
	ATT_OP_READ_BLOB_RSP				= 0x0D,
	ATT_OP_READ_MULTIPLE_REQ			= 0x0E,		ATT_OP_READ_MULTI_REQ = 0x0E,
	ATT_OP_READ_MULTIPLE_RSP			= 0x0F,
	ATT_OP_READ_BY_GROUP_TYPE_REQ		= 0x10,
	ATT_OP_READ_BY_GROUP_TYPE_RSP		= 0x11,
	ATT_OP_WRITE_REQ					= 0x12,
	ATT_OP_WRITE_RSP					= 0x13,
	ATT_OP_PREPARE_WRITE_REQ			= 0x16,
	ATT_OP_PREPARE_WRITE_RSP			= 0x17,
	ATT_OP_EXECUTE_WRITE_REQ			= 0x18,
	ATT_OP_EXECUTE_WRITE_RSP			= 0x19,

	ATT_OP_HANDLE_VALUE_NTF				= 0x1B,		ATT_OP_HANDLE_VALUE_NOTI = 0x1B,
	ATT_OP_HANDLE_VALUE_IND				= 0x1D,
	ATT_OP_HANDLE_VALUE_CFM				= 0x1E,

	ATT_OP_READ_MULTIPLE_VARIABLE_REQ	= 0x20,	//core_5.2
	ATT_OP_READ_MULTIPLE_VARIABLE_RSP	= 0x21, //core_5.2
	ATT_OP_MULTIPLE_HANDLE_VALUE_NTF	= 0x23, //core_5.2

	ATT_OP_WRITE_CMD					= 0x52,
	ATT_OP_SIGNED_WRITE_CMD				= 0xD2,
}att_pdu_type;




/******************************************** L2CAP ***************************************************************/

/**
 *  @brief  Definition for L2CAP CID name space for the LE-U
 */
typedef enum{
	L2CAP_CID_NULL				= 0x0000,
	L2CAP_CID_ATTR_PROTOCOL		= 0x0004,
	L2CAP_CID_SIG_CHANNEL		= 0x0005,
	L2CAP_CID_SMP				= 0x0006,
}l2cap_cid_type;

/**
 *  @brief  Definition for L2CAP signal packet formats
 */
typedef enum{
	L2CAP_COMMAND_REJECT_RSP           		= 0x01,
	L2CAP_CONNECTION_REQ                 	= 0x02,
	L2CAP_CONNECTION_RSP                 	= 0x03,
	L2CAP_CONFIGURATION_REQ                	= 0x04,
	L2CAP_CONFIGURATION_RSP           		= 0x05,
	L2CAP_DISCONNECTION_REQ           		= 0x06,
	L2CAP_DISCONNECTION_RSP           		= 0x07,
	L2CAP_ECHO_REQ          		 		= 0x08,
	L2CAP_ECHO_RSP           				= 0x09,
	L2CAP_INFORMATION_REQ           		= 0x0A,
	L2CAP_INFORMATION_RSP           		= 0x0B,
	L2CAP_CREATE_CHANNEL_REQ          		= 0x0C,
	L2CAP_CREATE_CHANNEL_RSP           		= 0x0D,
	L2CAP_MOVE_CHANNEL_REQ           		= 0x0E,
	L2CAP_MOVE_CHANNEL_RSP           		= 0x0F,
	L2CAP_MOVE_CHANNEL_CONFIRMATION_REQ		= 0x10,
	L2CAP_MOVE_CHANNEL_CONFIRMATION_RSP     = 0x11,
	L2CAP_CONNECTION_PARAMETER_UPDATE_REQ	= 0x12,		L2CAP_CMD_CONN_UPD_PARA_REQ		= 0x12,
	L2CAP_CONNECTION_PARAMETER_UPDATE_RSP	= 0x13,		L2CAP_CMD_CONN_UPD_PARA_RESP 	= 0x13,
	L2CAP_LE_CREDIT_BASED_CONNECTION_REQ 	= 0x14,
	L2CAP_LE_CREDIT_BASED_CONNECTION_RSP 	= 0x15,
	L2CAP_FLOW_CONTROL_CREDIT_IND 			= 0x16,
	L2CAP_CREDIT_BASED_CONNECTION_REQ 		= 0x17,	//core_5.2
	L2CAP_CREDIT_BASED_CONNECTION_RSP 		= 0x18,	//core_5.2
	L2CAP_CREDIT_BASED_RECONFIGURE_REQ 		= 0x19,	//core_5.2
	L2CAP_CREDIT_BASED_RECONFIGURE_RSP 		= 0x1A,	//core_5.2
}l2cap_sig_pkt_format;

/******************************************** LINKLAYER ***************************************************************/

/**
 *  @brief  Definition for LL Control PDU Opcode
 */																		// rf_len without MIC
#define					LL_CONNECTION_UPDATE_REQ	0x00							// 12
#define					LL_CHANNEL_MAP_REQ			0x01							//	8
#define					LL_TERMINATE_IND			0x02							//	2

#define					LL_ENC_REQ					0x03	// encryption			// 23
#define					LL_ENC_RSP					0x04	// encryption			// 13
#define					LL_START_ENC_REQ			0x05	// encryption			//	1
#define					LL_START_ENC_RSP			0x06	// encryption			//	1

#define					LL_UNKNOWN_RSP				0x07							//	2
#define 				LL_FEATURE_REQ              0x08							//	9
#define 				LL_FEATURE_RSP              0x09							//	9

#define					LL_PAUSE_ENC_REQ			0x0A	// encryption			//	1
#define					LL_PAUSE_ENC_RSP			0x0B	// encryption			//	1

#define 				LL_VERSION_IND              0x0C							//	6
#define 				LL_REJECT_IND         		0x0D							//	2
#define 				LL_SLAVE_FEATURE_REQ        0x0E	//core_4.1				//	9
#define 				LL_CONNECTION_PARAM_REQ		0x0F	//core_4.1				// 24
#define 				LL_CONNECTION_PARAM_RSP		0x10	//core_4.1				// 24
#define					LL_REJECT_IND_EXT			0x11	//core_4.1				//	3
#define 				LL_PING_REQ					0x12    //core_4.1				//	1
#define					LL_PING_RSP					0x13    //core_4.1				//	1
#define 				LL_LENGTH_REQ				0x14    //core_4.2				//	9
#define					LL_LENGTH_RSP				0x15    //core_4.2				//	9
#define 				LL_PHY_REQ					0x16	//core_5.0				//	3
#define 				LL_PHY_RSP					0x17	//core_5.0				//	3
#define 				LL_PHY_UPDATE_IND			0x18	//core_5.0				//	5
#define 				LL_MIN_USED_CHN_IND			0x19	//core_5.0				//	3

#define 				LL_CTE_REQ					0x1A	//core_5.1				//	2
#define 				LL_CTE_RSP					0x1B	//core_5.1				//	2
#define 				LL_PERIODIC_SYNC_IND		0x1C	//core_5.1				// 35
#define 				LL_CLOCK_ACCURACY_REQ		0x1D	//core_5.1				//	2
#define 				LL_CLOCK_ACCURACY_RSP		0x1E	//core_5.1				//	2


#define 				LL_CIS_REQ					0x1F	//core_5.2				//	36
#define 				LL_CIS_RSP					0x20	//core_5.2				//	9
#define 				LL_CIS_IND					0x21	//core_5.2				//  16
#define 				LL_CIS_TERMINATE_IND		0x22	//core_5.2				//	4
#define 				LL_POWER_CONTROL_REQ		0x23	//core_5.2				//	4
#define 				LL_POWER_CONTROL_RSP		0x24	//core_5.2				//	5
#define 				LL_POWER_CHANGE_IND			0x25	//core_5.2				//	5



/******************************************** GAP ***************************************************************/

// https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
// EIR Data Type, Advertising Data Type (AD Type) and OOB Data Type Definitions

typedef enum {
	DT_FLAGS								= 0x01,		//	Flag
	DT_INCOMPLT_LIST_16BIT_SERVICE_UUID		= 0x02,		//	Incomplete List of 16-bit Service Class UUIDs
	DT_COMPLETE_LIST_16BIT_SERVICE_UUID	    = 0x03,		//	Complete List of 16-bit Service Class UUIDs
	DT_INCOMPLT_LIST_32BIT_SERVICE_UUID    	= 0x04,		//	Incomplete List of 32-bit Service Class UUIDs
	DT_COMPLETE_LIST_32BIT_SERVICE_UUID		= 0x05,		//	Complete List of 32-bit Service Class UUIDs
	DT_INCOMPLT_LIST_128BIT_SERVICE_UUID   	= 0x06,		//	Incomplete List of 128-bit Service Class UUIDs
	DT_COMPLETE_LIST_128BIT_SERVICE_UUID	= 0x07,		//	Complete List of 128-bit Service Class UUIDs
	DT_SHORTENED_LOCAL_NAME					= 0x08,		//	Shortened Local Name
	DT_COMPLETE_LOCAL_NAME					= 0x09,		//	Complete Local Name
	DT_TX_POWER_LEVEL						= 0x0A,		//	Tx Power Level

	DT_CLASS_OF_DEVICE						= 0x0D,		//	Class of Device

	DT_APPEARANCE							= 0x19,		//	Appearance

	DT_BIGINFO								= 0x2C,		//	BIGInfo
	DT_BROADCAST_CODE						= 0x2D,		// 	Broadcast_Code
	DT_3D_INFORMATION_DATA					= 0x3D,		//	3D Information Data

	DATA_TYPE_MANUFACTURER_SPECIFIC_DATA 	= 0xFF,     //	Manufacturer Specific Data
}data_type_t;


#endif
