/********************************************************************************************************
 * @file	gap_event.h
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
#ifndef GAP_EVENT_H_
#define GAP_EVENT_H_




/*****************************************  SMP message sequence and event chart  *******************************************************

																							  GAP_SMP EVENT
Situation 1:   SMP Standard Pair																    |
	Phase 1: Paring Feature Exchange																|
	Phase 2: Encryption( STK/LTK Generation )														|
	Phase 3: Key Distribution																		|
																									|
  	     Master														  Slave							|
			|															|							|
			|														    |							|
    ________|___________________________________________________________|_______					|
	| 																			|					|
	|						   Establish LL connection							|					|
	|___________________________________________________________________________|					|
			|															|							|
			|															|							|
	        |              Phase 1: Paring Feature Exchange				|							|
	 _______|___________________________________________________________|_______					|
	|		|													        |	    |					|
	|		|			        (Optional)Security_Request		    	|	    |					|
	|		|<----------------------------------------------------------|	    |					|
	|		|						Paring_Req							|	    |
	| 		|---------------------------------------------------------->|=======|=====>>>  GAP_EVT_SMP_PAIRING_BEGIN
	|		|						Paring_Rsp					    	|	    |
	|  		|<----------------------------------------------------------|       |					|
	|		|						....... 							|	    |					|
	|_______|___________________________________________________________|_______|					|
			|															|							|
			|															|							|
	        |                  Phase 2: Encryption					    |							|
	 _______|___________________________________________________________|_______					|
	|		|															|	    |					|
	|		|					    LL_ENC_REQ				            |	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|		|					    LL_ENC_RSP						    |	    |					|
	| 		|<----------------------------------------------------------|	    |					|
	|		|				     LL_START_ENC_REQ						|	    |					|
	|  		|<----------------------------------------------------------|       |					|
	|		|					 LL_START_ENC_RSP						|	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|       |				     LL_START_ENC_RSP	 					|	    |
	|		|<----------------------------------------------------------|=======|=====>>>  GAP_EVT_SMP_CONN_ENCRYPTION_DONE(re_connect = SMP_STANDARD_PAIR)
	|_______|___________________________________________________________|_______|
			|															|							|
			|															|							|
	        |              Phase 3: Key Distribution				    |							|
	 _______|___________________________________________________________|_______					|
	|		|															|	    |					|
	|		|					  Key Distribution	 			        |	    |					|
	|		|<----------------------------------------------------------|	    |					|
	|		|					  Key Distribution						|	    |					|
	| 		|<----------------------------------------------------------|	    |					|
	|		|				           ....... 					        |	    |					|
	|  		|															|       |					|
	|		|					  Key Distribution				        |	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|		|					  Key Distribution						|	    |					|
	| 		|---------------------------------------------------------->|	    |					|
	|		|				           ....... 					        |	    |					|
	|  		|															|       |
	|		|				 All Key Distribution Finish				|=======|=====>>>  GAP_EVT_SMP_PAIRING_SUCCESS
	|		|				 											|=======|=====>>>  GAP_EVT_SMP_SECURITY_PROCESS_DONE(re_connect = SMP_STANDARD_PAIR)
	|_______|___________________________________________________________|_______|
																									|
																									|
																									|
																									|
Situation 2:   SMP Fast Connect																		|
	Only 1 Phase: Encryption																	    |
																									|
  	     Master														  Slave							|
			|															|							|
			|														    |							|
    ________|___________________________________________________________|_______					|
	| 																			|					|
	|						  Establish LL connection							|					|
	|___________________________________________________________________________|					|
			|															|							|
			|															|							|
	        |                  Phase 2: Encryption					    |							|
	 _______|___________________________________________________________|_______					|
	|		|															|	    |					|
	|		|					    LL_ENC_REQ				            |	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|		|					    LL_ENC_RSP						    |	    |					|
	| 		|<----------------------------------------------------------|	    |					|
	|		|				     LL_START_ENC_REQ						|	    |					|
	|  		|<----------------------------------------------------------|       |					|
	|		|					 LL_START_ENC_RSP						|	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|       |				     LL_START_ENC_RSP	 					|	    |					|
	|		|<----------------------------------------------------------|=======|=====>>>  GAP_EVT_SMP_CONN_ENCRYPTION_DONE (re_connect = SMP_FAST_CONNECT)
	|		|				 											|=======|=====>>>  GAP_EVT_SMP_SECURITY_PROCESS_DONE(re_connect = SMP_FAST_CONNECT)
	|_______|___________________________________________________________|_______|


 *****************************************************************************************************************************/


/**
 * @brief      GAP event type
 */
#define GAP_EVT_SMP_PAIRING_BEGIN		                             0	// Refer to SMP message sequence and event chart above
#define GAP_EVT_SMP_PAIRING_SUCCESS			                         1	// Refer to SMP message sequence and event chart above
#define GAP_EVT_SMP_PAIRING_FAIL			                         2
#define GAP_EVT_SMP_CONN_ENCRYPTION_DONE							 3	// Refer to SMP message sequence and event chart above
#define GAP_EVT_SMP_SECURITY_PROCESS_DONE							 4	// Refer to SMP message sequence and event chart above

#define GAP_EVT_SMP_TK_DISPALY			                             8
#define GAP_EVT_SMP_TK_REQUEST_PASSKEY								 9
#define GAP_EVT_SMP_TK_REQUEST_OOB									 10
#define GAP_EVT_SMP_TK_NUMERIC_COMPARE								 11


#define GAP_EVT_ATT_EXCHANGE_MTU									 16
#define GAP_EVT_GATT_HANDLE_VLAUE_CONFIRM							 17

#if(MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
	///Correct spelling errors, but need to be compatible with the previous version
	#define GAP_EVT_SMP_PARING_BEAGIN		                             GAP_EVT_SMP_PAIRING_BEGIN	// Refer to SMP message sequence and event chart above
	#define GAP_EVT_SMP_PARING_SUCCESS			                         GAP_EVT_SMP_PAIRING_SUCCESS	// Refer to SMP message sequence and event chart above
	#define GAP_EVT_SMP_PARING_FAIL			                             GAP_EVT_SMP_PAIRING_FAIL
#endif

/**
 * @brief      GAP event mask
 */
#define GAP_EVT_MASK_NONE                                        	 0x00000000
#define GAP_EVT_MASK_SMP_PAIRING_BEGIN                          	 (1<<GAP_EVT_SMP_PAIRING_BEGIN)
#define GAP_EVT_MASK_SMP_PAIRING_SUCCESS                           	 (1<<GAP_EVT_SMP_PAIRING_SUCCESS)
#define GAP_EVT_MASK_SMP_PAIRING_FAIL                           	 (1<<GAP_EVT_SMP_PAIRING_FAIL)

#define GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE                     	 (1<<GAP_EVT_SMP_CONN_ENCRYPTION_DONE)
#define GAP_EVT_MASK_SMP_SECURITY_PROCESS_DONE                     	 (1<<GAP_EVT_SMP_SECURITY_PROCESS_DONE)

#define GAP_EVT_MASK_SMP_TK_DISPALY                  				 (1<<GAP_EVT_SMP_TK_DISPALY)
#define GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY                  		 (1<<GAP_EVT_SMP_TK_REQUEST_PASSKEY)
#define GAP_EVT_MASK_SMP_TK_REQUEST_OOB	                     		 (1<<GAP_EVT_SMP_TK_REQUEST_OOB)
#define GAP_EVT_MASK_SMP_TK_NUMERIC_COMPARE                     	 (1<<GAP_EVT_SMP_TK_NUMERIC_COMPARE)

#define GAP_EVT_MASK_ATT_EXCHANGE_MTU                     	 		 (1<<GAP_EVT_ATT_EXCHANGE_MTU)
#define GAP_EVT_MASK_GATT_HANDLE_VLAUE_CONFIRM                     	 (1<<GAP_EVT_GATT_HANDLE_VLAUE_CONFIRM)


#define GAP_EVT_MASK_DEFAULT										( GAP_EVT_MASK_SMP_TK_DISPALY 			| \
																	  GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY   | \
																	  GAP_EVT_MASK_SMP_TK_REQUEST_OOB		| \
																	  GAP_EVT_MASK_ATT_EXCHANGE_MTU )


#if(MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
	///Correct spelling errors, but need to be compatible with the previous version
	#define GAP_EVT_MASK_SMP_PARING_BEAGIN                               GAP_EVT_MASK_SMP_PAIRING_BEGIN
	#define GAP_EVT_MASK_SMP_PARING_SUCCESS                              GAP_EVT_MASK_SMP_PAIRING_SUCCESS
	#define GAP_EVT_MASK_SMP_PARING_FAIL                                 GAP_EVT_MASK_SMP_PAIRING_FAIL
#endif

/**
 * @brief      data structure of GAP event callback data
 */
typedef struct {
	u16	connHandle;
	u8	secure_conn;
	u8	tk_method;
} gap_smp_paringBeginEvt_t;


typedef struct {
	u16	connHandle;
	u8	bonding;
	u8	bonding_result;
} gap_smp_paringSuccessEvt_t;

typedef struct {
	u16	connHandle;
	u8  reason;
} gap_smp_paringFailEvt_t;

typedef struct {
	u16	connHandle;
	u8	re_connect;   //1: re_connect, encrypt with previous distributed LTK;   0: paring , encrypt with STK
} gap_smp_connEncDoneEvt_t;

typedef struct {
	u16	connHandle;
	u8	re_connect;   //1: re_connect, encrypt with previous distributed LTK;   0: paring , encrypt with STK
} gap_smp_securityProcessDoneEvt_t;



typedef struct {
	u16	connHandle;
	u32	tk_pincode;
} gap_smp_TkDisplayEvt_t;

typedef struct {
	u16	connHandle;
} gap_smp_TkReqPassKeyEvt_t;

typedef struct {
	u16	connHandle;
	u16	peer_MTU;
	u16	effective_MTU;
} gap_gatt_mtuSizeExchangeEvt_t;






/**
 * @brief     GAP event callback function declaration
 */
typedef int (*gap_event_handler_t) (u32 h, u8 *para, int n);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      set event mask for specific gap event
 * @param[in]  evtMask - event mask
 * @return     none
 */
void 	blc_gap_setEventMask(u32 evtMask);


/**
 * @brief      register public enter for all gap event callback
 * @param[in]  handler - public enter function
 * @return     none
 */
void 	blc_gap_registerHostEventHandler (gap_event_handler_t  handler);

#ifdef __cplusplus
}
#endif

#endif /* GAP_EVENT_H_ */
