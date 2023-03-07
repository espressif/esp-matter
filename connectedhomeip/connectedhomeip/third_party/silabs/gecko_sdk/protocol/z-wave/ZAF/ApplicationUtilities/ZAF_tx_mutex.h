/**
 * @file
 * @brief Transmit mutex.
 *
 * @details Protected transmit buffers used to send data. Use this module to get
 * a transmit buffer and release the buffer again when data is sent and application
 * is notified with a callback. There is 1 buffer for unsolicited
 * commands.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _ZAF_MUTEX_H_
#define _ZAF_MUTEX_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <stdint.h>
#include <ZW_typedefs.h>
#include <ZW_classcmd.h>
#include "ZAF_types.h"
#include <ZW_transport_api.h>
/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * Allocates space for Supervision when used to declare a buffer.
 */
typedef struct
{
  uint8_t multichanCmdEncap[4]; //4 = sizeof(ZW_MULTI_CHANNEL_CMD_ENCAP_V2_FRAME) - sizeof(ALL_EXCEPT_ENCAP)
  ZW_SUPERVISION_GET_FRAME supervisionGet;
  ZW_APPLICATION_TX_BUFFER appTxBuf;
} ZAF_TRANSPORT_TX_BUFFER;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * @brief Initializes the TX mutexes.
 */
void
mutex_init(void);

/**
 * @brief Get pointer to Application transmit buffer. If return NULL is a job busy and
 * current action should be cancel.
 * @param completedFunc function-pointer to return status on job.
 * @return pointer to transmit-buffer. NULL is it not free.
 */
ZW_APPLICATION_TX_BUFFER*
GetRequestBuffer( VOID_CALLBACKFUNC(completedFunc)(TRANSMISSION_RESULT * pTransmissionResult) );

/**
 * @brief Free transmit buffer by clear mutex and remove callback. This should be
 * called if ZW_SendData() return false.
 */
void
FreeRequestBuffer(void);

/**
 * @brief This function must be used a Call-back status function for GetAppCmdFramePointer
 * when calling Z-Wave API ZW_SendData().
 * @param pTransmissionResult Transmission result.
 */
void
ZCB_RequestJobStatus(TRANSMISSION_RESULT * pTransmissionResult);

/**
 * @brief Returns whether any of the mutexes are occupied.
 * @return true if one or more mutexes are occupied, otherwise false.
 */
bool
ZAF_mutex_isActive(void);

/**
 * @brief Set payload length in Supervision cmd.
 * @param[in] pPayload pointer to request buffer payload.
 * @param[in] payLoadlen add Supervision command.
 * @return boolean if ppPayload is legal.
 */
bool
RequestBufferSetPayloadLength(ZW_APPLICATION_TX_BUFFER* pPayload,  uint8_t payLoadlen);

/**
 * @brief Get Requst buffer payload and payload length.
 * @param[in,out] ppPayload Pointer to payload. Pointer is changed to point on payload!
 * @param[in,out] pPayLoadlen Length of payload.
 * @param[in] supervision Specifies whether Supervision should be activated.
 * @return boolean if ppPayload is legal.
 */
bool
RequestBufferSupervisionPayloadActivate(
    ZW_APPLICATION_TX_BUFFER** ppPayload,
    size_t * pPayLoadlen,
    bool supervision);

#endif /* _ZAF_MUTEX_H_ */

