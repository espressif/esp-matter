/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Bluetooth Low Energy
 *   Declarations of the public functions and enumerations of gpBle.
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPBLE_H_
#define _GPBLE_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpHci_Includes.h"
#include "gpPd.h"
#include "gpHal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @macro GP_BLE_NR_OF_SOLICITED_EVENT_BUFFERS */
#define GP_BLE_NR_OF_SOLICITED_EVENT_BUFFERS         1
/** @macro GP_BLE_NR_OF_UNSOLICITED_EVENT_BUFFERS */

#ifndef GP_BLE_NR_OF_UNSOLICITED_EVENT_BUFFERS
#define GP_BLE_NR_OF_UNSOLICITED_EVENT_BUFFERS       6
#endif //GP_BLE_NR_OF_UNSOLICITED_EVENT_BUFFERS

#ifndef GP_BLE_NR_OF_CONNECTION_COMPLETE_EVENT_BUFFERS
// Should have been defined by the environment
#define GP_BLE_NR_OF_CONNECTION_COMPLETE_EVENT_BUFFERS  1
#endif //GP_BLE_NR_OF_CONNECTION_COMPLETE_EVENT_BUFFERS

/** @macro GP_BLE_NR_OF_EVENT_BUFFERS */
#define GP_BLE_NR_OF_EVENT_BUFFERS                   (GP_BLE_NR_OF_SOLICITED_EVENT_BUFFERS + GP_BLE_NR_OF_UNSOLICITED_EVENT_BUFFERS + GP_BLE_NR_OF_CONNECTION_COMPLETE_EVENT_BUFFERS)

// HCI buffer configuration

/** @macro GP_BLE_DIVERSITY_HCI_BUFFER_AMOUNT_TX */
#ifndef GP_BLE_DIVERSITY_HCI_BUFFER_AMOUNT_TX
#define GP_BLE_DIVERSITY_HCI_BUFFER_AMOUNT_TX       4
#endif // GP_BLE_DIVERSITY_HCI_BUFFER_AMOUNT_TX

/** @macro GP_BLE_DIVERSITY_HCI_BUFFER_AMOUNT_RX */
#ifndef GP_BLE_DIVERSITY_HCI_BUFFER_AMOUNT_RX
#define GP_BLE_DIVERSITY_HCI_BUFFER_AMOUNT_RX       4
#endif // GP_BLE_DIVERSITY_HCI_BUFFER_AMOUNT_RX

/** @macro GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_TX */
#ifndef GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_TX
#define GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_TX         255
#endif // GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_TX

/** @macro GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_RX */
#ifndef GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_RX
#define GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_RX         255
#endif // GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_RX

/** @macro GP_BLE_EVENT_HANDLE_IDX_INVALID */
#define GP_BLE_EVENT_HANDLE_IDX_INVALID              0xFF

#define BLE_MAX_NR_OF_PHYS 1

#define BLE_LLCP_MAX_NR_OF_VIRTUAL_CONNECTIONS  BLE_MAX_NR_OF_PHYS

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#define GP_BLEADDRESSRESOLVER_GET_GLOBALS_CONST() ((gpBleAddressResolver_ConstGlobalVars_t*) JumpTables_DataTable.gpBleAddressResolver_ConstGlobalVars_ptr)
#define BLE_LLCP_MAX_NR_OF_CONNECTIONS (GP_BLEADDRESSRESOLVER_GET_GLOBALS_CONST()->gpDiversityBleMaxNrOfSupportedConnections)
#define BLE_LLCP_MAX_NR_OF_SLAVE_CONNECTIONS (GP_BLEADDRESSRESOLVER_GET_GLOBALS_CONST()->gpDiversityBleMaxNrOfSupportedSlaveConnections)
#else
#define BLE_LLCP_MAX_NR_OF_CONNECTIONS          GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS
#define BLE_LLCP_MAX_NR_OF_SLAVE_CONNECTIONS    GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_SLAVE_CONNECTIONS
#endif

#define GPBLE_CONN_COMPLETE_BUFFER_OVERHEAD     ( offsetof(gpBle_EventBuffer_t, payload) + \
                                                  offsetof(gpHci_EventCbPayload_t, metaEventParams) + \
                                                  offsetof(gpHci_LEMetaEventParams_t,params) +  \
                                                  offsetof(gpHci_LEMetaInfo_t,enhancedConnectionComplete) )

// Space needed to store a full connection complete buffer
// 1 byte for the event code (+ overhead, see above) + size of the complete struct
#define GPBLE_BUFFERSIZE_CONNECTION_COMPLETE              (GPBLE_CONN_COMPLETE_BUFFER_OVERHEAD + sizeof(gpHci_LEConnectionCompleteEventParams_t))

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/** @macro GP_BLE_EVENT_HANDLE_ISVALID(handle) */
#define GP_BLE_EVENT_HANDLE_ISVALID(handle)          !(handle == GP_BLE_EVENT_HANDLE_IDX_INVALID)

/** This function sets bits in a 2byte value.
 *
 * @param  bf  The buffer that contains the frame control byte.
 * @param  bm  The shifted bit mask of the bits to set.
 * @param  idx The bit index of the bits to set.
 * @param  v   The value to set.
 */
#define BLE_BM_SET(bf,bm,idx,v)     bf &= ~bm; bf |= (((UInt16)v) << idx)
#define BLE_BM_GET(header, mask, idx)     ((header & mask) >> idx)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @enum gpBle_EventServer_t */
//@{
/** @brief  Event handler identifier  */
#define gpBle_EventServer_Wrapper                               0x01
#define gpBle_EventServer_Host                                  0x02
#define gpBle_EventServer_Invalid                               0xFF
typedef UInt8                             gpBle_EventServer_t;
//@}

typedef UInt8 gpBle_RxBufferHandle_t;

typedef struct {
    gpHci_EventCode_t eventCode;
    gpHci_EventCbPayload_t payload;
} gpBle_EventBuffer_t;

typedef UInt8 gpBle_EventBufferHandle_t;

typedef struct {
    gpBle_EventBufferHandle_t   eventHandle;
    gpBle_EventServer_t         eventServer;
} gpBle_EventBufferInfo_t;

typedef gpHci_Result_t (*gpBle_ActionFunc_t)(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pBuf);

typedef void (*gpBle_HciResetCallback_t)(void);

// Type of buffer to claim (solicited = in direct response to a host command, unsolicited = delayed response or not-host initiated)
#define Ble_EventBufferType_Solicited           0x00
#define Ble_EventBufferType_Unsolicited         0x01
#define Ble_EventBufferType_ConnectionComplete  0x02
#define Ble_EventBufferType_Invalid             0xff
typedef UInt8 Ble_EventBufferType_t;

typedef UInt32 gpBle_AccessAddress_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests
void gpBle_Init(gpHal_BleCallbacks_t* pCallbacks);
void gpBle_ResetBlock(Bool firstReset);
void gpBle_ExecuteCommand(gpBle_EventServer_t eventServer, gpHci_CommandOpCode_t opCode, gpBle_ActionFunc_t actionFunc, gpHci_CommandParameters_t* pParams);
void gpBle_SendEvent(gpBle_EventBuffer_t* pEventBuf);
Bool gpBle_ScheduleEvent(UInt32 delayUs, gpHci_EventCode_t eventCode, gpHci_EventCbPayload_t* pPayload);
Bool gpBle_SendEventWithPayload(gpHci_EventCode_t eventCode, gpHci_EventCbPayload_t* pPayload);

//Indications
void gpBle_cbEventIndication(gpBle_EventBufferInfo_t* pEventInfo);
void gpBle_HciEventConfirm(gpBle_EventBufferHandle_t eventBufferHandle);

// LLCP
#ifdef GP_DIVERSITY_BLE_MASTER
gpHci_Result_t gpBle_LeStartEncryption(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif // GP_DIVERSITY_BLE_MASTER



gpHci_Result_t gpBle_VsdSetTransmitPower(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_VsdDisableSlaveLatency(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_VsdOverruleRemoteMaxRxOctetsAndTime(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

gpHci_Result_t gpBle_VsdEnCBByDefault(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

gpHci_Result_t gpBle_LeReadBufferSize(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_VsdSetDataPumpParameters(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_VsdSetDataPumpEnable(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

void gpBle_CopyEventBuffer(gpBle_EventBuffer_t* pEventBuf, gpBle_EventBufferHandle_t eventHandle);

// Vendor specific commands
gpHci_Result_t gpBle_SetVsdTestParams(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

// Phy specific commands
gpHci_Result_t gpBle_LeReadPhy(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeSetDefaultPhy(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeSetPhy(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

// unknown opCode
gpHci_Result_t gpBle_UnknownOpCode(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

// Event buffer handling
gpBle_EventBuffer_t* gpBle_AllocateEventBuffer(Ble_EventBufferType_t bufferType);
gpBle_EventBufferHandle_t gpBle_EventBufferToHandle(gpBle_EventBuffer_t *pEventBuffer);
gpBle_EventBuffer_t * gpBle_EventHandleToBuffer(gpBle_EventBufferHandle_t eventHandle);

void gpBle_SendCommandCompleteEvent(gpHci_CommandCompleteParams_t* params);




//Indications


UInt16 gpBle_GetEcDifference(UInt16 ec1, UInt16 ec2);
Bool gpBle_IsEcEarlier(UInt16 expectedEarliestEc, UInt16 expectedLastEc);
void gpBle_GetEventCodes(gpBle_EventBufferHandle_t eventHandle, gpHci_EventCode_t *eventCode, gpHci_LEMetaSubEventCode_t *subEventCode, gpHci_ConnectionHandle_t *connectionHandle);

gpBle_AccessAddress_t gpBle_CreateAccessAddress(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPBLE_H_

