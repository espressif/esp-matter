/******************************************************************************

 @file  icall_hci_tl.h

 @brief This file contains the application HCI TL ICall function prototypes.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef ICALL_HCI_TL_H
#define ICALL_HCI_TL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
// Module Id Used to report system event (error or other)
#define HCI_TL_ID                                  0
#define HOST_TL_ID                                 1

#define HCI_LEGACY_CMD_STATUS_UNDEFINED            0
#define HCI_LEGACY_CMD_STATUS_BT4                  1
#define HCI_LEGACY_CMD_STATUS_BT5                  2

#ifdef BLE3_CMD
// Advertising event types
#define GAP_ADTYPE_ADV_IND                0x00  //!< Connectable undirected advertisement
#define GAP_ADTYPE_ADV_HDC_DIRECT_IND     0x01  //!< Connectable high duty cycle directed advertisement
#define GAP_ADTYPE_ADV_SCAN_IND           0x02  //!< Scannable undirected advertisement
#define GAP_ADTYPE_ADV_NONCONN_IND        0x03  //!< Non-Connectable undirected advertisement
#define GAP_ADTYPE_ADV_LDC_DIRECT_IND     0x04  //!< Connectable low duty cycle directed advertisement

#define GAP_AGAMA_BLE3_TYPE_ADV_IND            0x13
#define GAP_AGAMA_BLE3_TYPE_ADV_HDC_DIRECT_IND 0x1D
#define GAP_AGAMA_BLE3_TYPE_ADV_SCAN_IND       0x12
#define GAP_AGAMA_BLE3_TYPE_ADV_NONCONN_IND    0x10
#define GAP_AGAMA_BLE3_TYPE_ADV_LDC_DIRECT_IND 0x15

//Notice events
#define HCI_EXT_GAP_BLE3_CONN_EVT_NOTICE       0x0613
#define HCI_EXT_ADV_EVENT_NOTICE               0xFC17
#define HCI_EXT_ADV_EVENT_NOTICE_EVENT         0x0417
#define HCI_EXT_SCAN_EVENT_NOTICE              0xFC22
#define HCI_EXT_SCAN_EVENT_NOTICE_EVENT        0x0422

/**
 * BLE3 GAP_setParam - Param Ids
 */
#define TGAP_GEN_DISC_ADV_MIN          0
#define TGAP_LIM_ADV_TIMEOUT           1
#define TGAP_GEN_DISC_SCAN             2
#define TGAP_LIM_DISC_SCAN             3
#define TGAP_CONN_EST_ADV_TIMEOUT      4
#define TGAP_CONN_PARAM_TIMEOUT        5
#define TGAP_LIM_DISC_ADV_INT_MIN      6
#define TGAP_LIM_DISC_ADV_INT_MAX      7
#define TGAP_GEN_DISC_ADV_INT_MIN      8
#define TGAP_GEN_DISC_ADV_INT_MAX      9
#define TGAP_CONN_ADV_INT_MIN         10
#define TGAP_CONN_ADV_INT_MAX         11
#define TGAP_CONN_SCAN_INT            12
#define TGAP_CONN_SCAN_WIND           13
#define TGAP_CONN_HIGH_SCAN_INT       14
#define TGAP_CONN_HIGH_SCAN_WIND      15
#define TGAP_GEN_DISC_SCAN_INT        16
#define TGAP_GEN_DISC_SCAN_WIND       17
#define TGAP_LIM_DISC_SCAN_INT        18
#define TGAP_LIM_DISC_SCAN_WIND       19
#define TGAP_CONN_EST_ADV             20
#define TGAP_CONN_EST_INT_MIN         21
#define TGAP_CONN_EST_INT_MAX         22
#define TGAP_CONN_EST_SCAN_INT        23
#define TGAP_CONN_EST_SCAN_WIND       24
#define TGAP_CONN_EST_SUPERV_TIMEOUT  25
#define TGAP_CONN_EST_LATENCY         26
#define TGAP_CONN_EST_MIN_CE_LEN      27
#define TGAP_CONN_EST_MAX_CE_LEN      28
#define TGAP_PRIVATE_ADDR_INT         29
#define TGAP_CONN_PAUSE_CENTRAL       30
#define TGAP_CONN_PAUSE_PERIPHERAL    31
#define TGAP_SM_TIMEOUT               32
#define TGAP_SM_MIN_KEY_LEN           33
#define TGAP_SM_MAX_KEY_LEN           34
#define TGAP_FILTER_ADV_REPORTS       35
#define TGAP_SCAN_RSP_RSSI_MIN        36
#define TGAP_REJECT_CONN_PARAMS       37
#define TGAP_AUTH_TASK_ID             38
#define TGAP_VERIFY_CAR               39
#define TGAP_FAST_INTERVAL_2_INT_MIN  40
#define TGAP_FAST_INTERVAL_2_INT_MAX  41
#define TGAP_SET_SCAN_CHAN            44
#define TGAP_PARAMID_MAX              45  //!< ID MAX-valid Parameter ID
#endif// BLE3_CMD

/*********************************************************************
 * TYPEDEFS
 */
 #ifdef BLE3_CMD
typedef struct
{
  uint8_t evtType;
  uint8_t addrType;
  uint8_t addr[B_ADDR_LEN];
} deviceInfo_t;
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Callback for application to overwrite data of an HCI serial packet.
 * Note that some commands require overrides to function properly when called
 * from a remote processor.
 *
 * The embedded application may elect to not override these commands if it
 * does expect them to be called (or the application processor precludes these
 * commands from being sent), but if these commands are called and not overridden, unexpected
 * behavior may occur.
 */
typedef void (*HCI_TL_ParameterOverwriteCB_t)(uint8_t *pData);

/*
 * Callback for the application to send HCI Command Status events up to the
 * Remote Host.
 */
typedef void (*HCI_TL_CommandStatusCB_t)(uint8_t *pBuf, uint16_t len);

/*
 * Callback for the application to post and process Callback from LL.
 * This is needed to exit the Hwi/Swi context of the LL callback.
 */
typedef uint8_t (*HCI_TL_CalllbackEvtProcessCB_t)(void *pData, void* callbackFctPtr);

/*********************************************************************
 * @fn      HCI_TL_Init
 *
 * @brief   Initialize HCI TL.
 *
 * @param   overwriteCB     - callback used to allow custom override the contents of the
 *                            serial buffer.
 *          csCB            - Callback to process command status
 *          evtCB           - Callback to post event related to Callback from LL
 *          taskID - Task ID of task to process and forward messages to the TL.
 *
 * @return  none.
 */
extern void HCI_TL_Init(HCI_TL_ParameterOverwriteCB_t overwriteCB,
                        HCI_TL_CommandStatusCB_t csCB,
                        HCI_TL_CalllbackEvtProcessCB_t evtCB,
                        ICall_EntityID taskID);

/*********************************************************************
 * @fn      HCI_TL_sendSystemReport
 *
 * @brief   Used to return specific system error over UART.
 *
 * @param   id - id of the module reporting the error.
 *          status - type of error
 *          info -   more information linked to the error or the module
 * @return  none.
 */
void HCI_TL_sendSystemReport(uint8_t id, uint8_t status, uint16_t info);

/*********************************************************************
 * @fn      HCI_TL_compareAppLastOpcodeSent
 *
 * @brief   check if the opcode of an event received matches the last
 *          opcode of an HCI command called from the embedded application.
 *
 * @param   opcode - opcode of the received Stack event.
 *
 * @return  TRUE if opcode matches, FALSE otherwise.
 */
extern uint8_t HCI_TL_compareAppLastOpcodeSent(uint16_t opcode);

/*********************************************************************
 * @fn      HCI_TL_SendToStack
 *
 * @brief   Translate seriall buffer into it's corresponding function and
 *          parameterize the arguments to send to the Stack.
 *
 * @param   msgToParse - pointer to a serialized HCI command or data packet.
 *
 * @return  none.
 */
extern void HCI_TL_SendToStack(uint8_t *msgToParse);

/*********************************************************************
 * @fn      HCI_TL_processStructuredEvent
 *
 * @brief   Interprets a structured Event from the BLE Host and serializes it.
 *
 * @param   pEvt - structured event to serialize.
 *
 * @return  TRUE to deallocate pEvt message, False otherwise.
 */
extern uint8_t HCI_TL_processStructuredEvent(ICall_Hdr *pEvt);

#ifdef BLE3_CMD
status_t BLE3ToAgama_setParam( uint16_t id, uint16_t value );
uint16_t getBLE3ToAgamaEventProp( uint8_t eventType);
uint8_t getAgamaToBLE3EventProp( uint8_t eventType );

#endif

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ICALL_HCI_TL_H */
