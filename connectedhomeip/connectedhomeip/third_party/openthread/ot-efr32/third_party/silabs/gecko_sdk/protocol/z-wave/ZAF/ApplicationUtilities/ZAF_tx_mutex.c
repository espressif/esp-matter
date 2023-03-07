/**
 * @file
 * Transmit mutex
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <string.h>
#include <ZW_typedefs.h>
#include <ZAF_tx_mutex.h>
#include <ZW_transport_api.h>
#include <CC_Supervision.h>
#include <stdint.h>

//#define DEBUGPRINT
#include "DebugPrint.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef struct _MUTEX
{
  uint8_t mutexAppActive;
  VOID_CALLBACKFUNC(pAppJob)(TRANSMISSION_RESULT * pTransmissionResult);
  ZAF_TRANSPORT_TX_BUFFER reqTxBuf;
} MUTEX;

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

static MUTEX myMutex;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/
static bool MutexSet(uint8_t* pMutex);
static void MutexFree(uint8_t* pMutex);

void
mutex_init(void)
{
  memset((uint8_t *)&myMutex, 0x00, sizeof(myMutex));
}

ZW_APPLICATION_TX_BUFFER*
GetRequestBuffer( VOID_CALLBACKFUNC(completedFunc)(TRANSMISSION_RESULT * pTransmissionResult) )
{
  /*Set mutex*/
  if(false == MutexSet(&(myMutex.mutexAppActive)))
  {
    /*Mutex is not free.. stop current job*/
    return NULL;
  }
  myMutex.pAppJob = completedFunc;
  CommandClassSupervisionGetAdd(&(myMutex.reqTxBuf.supervisionGet));

  return &(myMutex.reqTxBuf.appTxBuf);
}

bool
RequestBufferSetPayloadLength(ZW_APPLICATION_TX_BUFFER* pPayload,  uint8_t payLoadlen)
{
  if(pPayload == &(myMutex.reqTxBuf.appTxBuf))
  {
    CommandClassSupervisionGetSetPayloadLength(&myMutex.reqTxBuf.supervisionGet, payLoadlen);
    return true;
  }
  return false;
}

bool
RequestBufferSupervisionPayloadActivate(
    ZW_APPLICATION_TX_BUFFER** ppPayload,
    size_t* pPayLoadlen,
    bool supervision)
{
  if(true == myMutex.mutexAppActive)
  {
    /*Rewrite SV-cmd if CCmultichannel has written in payload*/
    CommandClassSupervisionGetWrite(&(myMutex.reqTxBuf.supervisionGet));
    *pPayLoadlen = CommandClassSupervisionGetGetPayloadLength(&(myMutex.reqTxBuf.supervisionGet));
    if(true == supervision)
    {
      *ppPayload = (ZW_APPLICATION_TX_BUFFER*)&(myMutex.reqTxBuf.supervisionGet);
      *pPayLoadlen += sizeof(ZW_SUPERVISION_GET_FRAME);
    }
    else
    {
      *ppPayload = &(myMutex.reqTxBuf.appTxBuf);
    }
    return true;
  }
  return false;
}

void
ZCB_RequestJobStatus(TRANSMISSION_RESULT * pTransmissionResult)
{
  DPRINT("\r\nZCB_RequestJobStatus");
  if(NON_NULL( myMutex.pAppJob ))
  {
    myMutex.pAppJob(pTransmissionResult);
  }

  if (TRANSMISSION_RESULT_FINISHED == pTransmissionResult->isFinished)
  {
    DPRINT(" _FREE!_");
    FreeRequestBuffer();
  }
}

void
FreeRequestBuffer(void)
{
  DPRINT("\r\nFreeRequestBuffer");
  /*Remove application func-callback. User should not be called any more*/
  myMutex.pAppJob = NULL;
  /*Free Mutex*/
  MutexFree(&(myMutex.mutexAppActive));
}

/**
 * @brief Set mutex if it is not active
 * @param[in,out] pMutex pointer to the mutex-flag that should be changed.
 * @return true if mutex was set else false for mutex was not free.
 */
static bool
MutexSet(uint8_t* pMutex)
{
  if( false == *pMutex)
  {
    *pMutex = true;
    return true;
  }
  return false;
}

/**
 * @brief MutexFree
 * @param[out] pMutex pointer to the mutex-flag that should be changed.
 * Free mutex
 */
static void
MutexFree(uint8_t* pMutex)
{
  DPRINTF("MutexFree %d\r\n", (uint32_t)pMutex);
  *pMutex = false;
}

bool
ZAF_mutex_isActive(void)
{
  if(true == myMutex.mutexAppActive)
  {
    DPRINT("Mux");
    return true;
  }
  return false;
}
