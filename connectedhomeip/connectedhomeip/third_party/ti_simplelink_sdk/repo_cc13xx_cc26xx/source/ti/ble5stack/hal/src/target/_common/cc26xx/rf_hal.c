/******************************************************************************

 @file  rf_hal.c

 @brief This file contains the data structures and APIs for CC26xx
        RF Core Hardware Abstraction Layer (HAL).

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
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

/*******************************************************************************
 * INCLUDES
 */

#include "mb.h"
#include "rf_hal.h"
#include "hal_mcu.h"
#include "hal_assert.h"
#include "osal.h"
#include "osal_bufmgr.h"
//
#include "r2f_common_flash_jt.h"
#include "r2r_common_flash_jt.h"

/*******************************************************************************
 * MACROS
 */

#ifdef DEBUG
#define RFHAL_ASSERT(cond) {volatile uint8 i = (cond); while(!i);}
#else // !DEBUG
// Note: Use HALNODEBUG to eliminate HAL assert handling (i.e. no assert).
// Note: If HALNODEBUG is not used, use ASSERT_RESET to reset system on assert.
//       Otherwise, evaluation board hazard lights are used.
// Note: Unused input parameter possible when HALNODEBUG; PC-Lint error 715.
#define RFHAL_ASSERT(cond) HAL_ASSERT(cond)
#endif // DEBUG


/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

// radio firmware information
rfOpImmedCmd_ReqInfo_t fwInfoCmd;

// software RAT channels
ratChan_t ratChanInfo[ MAX_NUM_SW_RAT_CHANS ];

/*******************************************************************************
 * API
 */

/*
** RF Hardware Abstraction Layer Application Programming Interface
**
** Data Entry API
*/

/*******************************************************************************
 * @fn          RFHAL_InitRfHal API
 *
 * @brief       This function is used initialize the RF HAL. Currently, this
 *              means issuing a Radio FW Information command, and updating
 *              the available RAT channels for software.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
uint8 RFHAL_InitRfHal( void )
{
  uint8 i, j;
  fwInfoCmd.cmdNum = CMD_GET_FW_INFO;

  if ( MAP_MB_SendCommand( (uint32)&fwInfoCmd ) != CMDSTA_DONE )
  {
    return( RFHAL_ERROR_FW_INFO_FAILED );
  }

  // mark all RAT channels as invalid
  for (i=0; i<MAX_NUM_SW_RAT_CHANS; i++)
  {
    ratChanInfo[i].ratChanNum   = RAT_CHAN_INVALID;
    ratChanInfo[i].ratChanStat  = RAT_CHAN_INVALID;
    ratChanInfo[i].ratChanCBack = NULL;
  }

  // find available software RAT channels as indicated by the radio FW
  for (i=0, j=0; i<MAX_NUM_RAT_CHANS; i++)
  {
    // check if bit is set
    if ( fwInfoCmd.availRatChans & (1 << i) )
    {
      // it is, so this RAT channel belongs to software
      ratChanInfo[j].ratChanNum  = i;
      ratChanInfo[j].ratChanStat = RAT_CHAN_FREE;

      // check if we're done
      if ( ++j == MAX_NUM_SW_RAT_CHANS ) return( RFHAL_SUCCESS );
    }
  }

  return( RFHAL_SUCCESS );
}


/*******************************************************************************
 * @fn          RFHAL_AllocRatChan API
 *
 * @brief       This function is used to return the next available software RAT
 *              channel, if any.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      A software RAT channel (0..15), or RAT_CHAN_INVALID.
 */
uint8 RFHAL_AllocRatChan( void )
{
  uint8 i;

  // check for all available RAT channels
  for (i=0; i<MAX_NUM_SW_RAT_CHANS; i++)
  {
    // check if RAT channel is free
    if ( ratChanInfo[i].ratChanStat == RAT_CHAN_FREE )
    {
      // it is, so mark it busy and return it
      ratChanInfo[i].ratChanStat = RAT_CHAN_BUSY;

      // ALT: Return index.
      return( ratChanInfo[i].ratChanNum );
    }
  }

  // none available
  return( RAT_CHAN_INVALID );
}



/*******************************************************************************
 * @fn          RFHAL_FreeRatChan API
 *
 * @brief       This function is used to free the specified RAT channel.
 *
 * input parameters
 *
 * @param       ratChanNum - Software RAT channel to be freed.
 *              ALT: Use index instead.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      RFHAL_SUCCESS or RFHAL_ERROR_UNKNOWN_RAT_CHAN
 */
uint8 RFHAL_FreeRatChan( uint8 ratChanNum )
{
  uint8 i;

  // check for all allocated RAT channels
  for (i=0; i<MAX_NUM_SW_RAT_CHANS; i++)
  {
    // check if we have a matching RAT channel
    if ( ratChanInfo[i].ratChanNum == ratChanNum )
    {
      // it is, so mark it free, and clear callback (if any)
      ratChanInfo[i].ratChanStat  = RAT_CHAN_FREE;
      ratChanInfo[i].ratChanCBack = NULL;

      return( RFHAL_SUCCESS );
    }
  }

  // none available
  return( RFHAL_ERROR_UNKNOWN_RAT_CHAN );
}



/*******************************************************************************
 * @fn          RFHAL_MapRatChansToInt API
 *
 * @brief       This function is used to map allocated RAT channels to their
 *              corresponding bit positions in the HW interrupt mask.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      A 32 bit value containing a set bit for each allocated RAT
 *              channel.
 */
uint32 RFHAL_MapRatChansToInt( void )
{
  uint8 i;
  uint32 hwIntMask;

  // clear the mask
  hwIntMask = 0;

  // check for all allocated RAT channels
  for (i=0; i<MAX_NUM_SW_RAT_CHANS; i++)
  {
    // check if we have an allocated RAT channel
    if ( ratChanInfo[i].ratChanStat == RAT_CHAN_BUSY )
    {
      // set the corresponding bit in the HW interrupt mask
      hwIntMask |= (1 << (ratChanInfo[i].ratChanNum + 12));
    }
  }

  // none available
  return( hwIntMask );
}


/*******************************************************************************
 * @fn          RFHAL_RegisterRatChanCallback API
 *
 * @brief       This function is used to map an allocated RAT channel to a
 *              callback. This routine would be used in the HW ISR to direct
 *              a RAT channel interrupt to the correct processing.
 *
 * input parameters
 *
 * @param       ratChanNum   - Software RAT channel to map callback.
 * @param       ratChanCBack - Routine to handle RAT channel interrupt.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
uint8 RFHAL_RegisterRatChanCallback( uint8          ratChanNum,
                                     ratChanCBack_t ratChanCBack )
{
  uint8 i;

  // check for all allocated RAT channels
  for (i=0; i<MAX_NUM_SW_RAT_CHANS; i++)
  {
    // check if we have a matching RAT channel
    if ( ratChanInfo[i].ratChanNum == ratChanNum )
    {
      // it is, so map the callback
      ratChanInfo[i].ratChanCBack = ratChanCBack;

      return( RFHAL_SUCCESS );
    }
  }

  // none available
  return( RFHAL_ERROR_UNKNOWN_RAT_CHAN );
}


/*******************************************************************************
 * @fn          RFHAL_InitDataQueue API
 *
 * @brief       This function is used to clear the pointers of the data entry
 *              queue, as well as the internal next data entry pointer. This
 *              routine is primarily intended for statically allocated data
 *              entry queues, but can also be used with dynamically allocated
 *              data entry queues.
 *
 *              Note: It is assumed the data entry queue is really based on a
 *                    data queue.
 *
 * input parameters
 *
 * @param       pDataEntryQ - Pointer to data entry queue.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void RFHAL_InitDataQueue( dataEntryQ_t *pDataEntryQ )
{
  // ROM WORKAROUND - Get rid of all ifdefs. Add check for NULL?
#ifdef DEBUG
  RFHAL_ASSERT( pDataEntryQ != NULL );
#endif // DEBUG

  // clear Data Entry Queue pointers
  ((dataQ_t *)pDataEntryQ)->dataEntryQ.pCurEntry  = NULL;
  ((dataQ_t *)pDataEntryQ)->dataEntryQ.pLastEntry = NULL;

  // clear internal Data Entry pointer
  ((dataQ_t *)pDataEntryQ)->pNextDataEntry = NULL;

  // clear internal temp Data Entry pointer
  ((dataQ_t *)pDataEntryQ)->pTempDataEntry = NULL;

  return;
}


/*******************************************************************************
 * @fn          RFHAL_QueueRFDataEntries API
 *
 * @brief       This function is used to add all Pending Tx data entries on the
 *              internal TX data queue to the radio FW's TX data entry queue.
 *
 *              Note: It is assumed the data entry queue is really based on a
 *                    data queue.
 *
 * input parameters
 *
 * @param       pDataEntryQ   - Pointer to data entry queue.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      RFHAL Status
 */
rfhalStat_t RFHAL_QueueRFDataEntries( dataEntryQ_t *pDataEntryQ )
{
  halIntState_t  cs;
  dataEntry_t   *pEntry;

  HAL_ENTER_CRITICAL_SECTION(cs);

  // get head of internal queue, or NULL
  pEntry = ((dataQ_t *)pDataEntryQ)->pNextDataEntry;

  while( pEntry != NULL )
  {
    // check if data entry is pending
    if ( pEntry->status == DATASTAT_PENDING )
    {
      rfOpImmedCmd_AddRemoveFlushQueue_t rfCmd;

      // setup a radio command to add to Tx RF queue
      rfCmd.cmdNum = CMD_ADD_DATA_ENTRY;
      rfCmd.pQueue = pDataEntryQ;
      rfCmd.pEntry = (uint8 *)pEntry;

      // issue immediate command to add the data entry to the Tx RF queue and
      // check if we were successful
      if ( MAP_MB_SendCommand( (uint32)&rfCmd ) != CMDSTA_DONE )
      {
        HAL_EXIT_CRITICAL_SECTION(cs);

        return( RFHAL_ERROR_ADD_TX_ENTRY_FAIL );
      }

      // move on to the next data entry
      pEntry = pEntry->pNextEntry;
    }
  }

  HAL_EXIT_CRITICAL_SECTION(cs);

  return( RFHAL_SUCCESS );
}


/*******************************************************************************
 * @fn          RFHAL_AddTxDataEntry API
 *
 * @brief       This function is used to add a Tx data entry to the internal
 *              Tx data entry queue, adjusting the internal number of entries.
 *              If Tx data is enabled, then the entry is also queued for Tx RF
 *              on the FW's data entry queue.
 *
 *              Note: It is assumed the number of allowed entries is managed by
 *                    the calling routine.
 *
 *              Note: It is assumed the data entry queue is really based on a
 *                    data queue.
 *
 * input parameters
 *
 * @param       pDataEntryQ   - Pointer to data entry queue.
 * @param       pDataEntry    - Pointer to data entry to add.
 * @param       rfCoreState   - RFCORE_STATE_IDLE or RFCORE_STATE_SLEEPING
 *                              placed on the radio FW's data entry queue.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      RFHAL Status
 */
rfhalStat_t RFHAL_AddTxDataEntry( dataEntryQ_t *pDataEntryQ,
                                  dataEntry_t  *pDataEntry,
                                  uint8         rfCoreState )
{
  halIntState_t                       cs;
  dataEntry_t                        *pEntry;
  rfOpImmedCmd_AddRemoveFlushQueue_t  rfCmd;

  HAL_ENTER_CRITICAL_SECTION(cs);

  // get head of internal queue, or NULL
  pEntry = ((dataQ_t *)pDataEntryQ)->pNextDataEntry;

  // check if internal queue is empty
  if ( pEntry == NULL )
  {
    // it is, so just add this entry
    ((dataQ_t *)pDataEntryQ)->pNextDataEntry = pDataEntry;
  }
  else // at least one entry on queue
  {
    // find last entry on internal queue
    while( pEntry->pNextEntry != NULL ) pEntry = pEntry->pNextEntry;

    // and add this packet
    pEntry->pNextEntry = pDataEntry;
  }

  // clear entries next pointer
  pDataEntry->pNextEntry = NULL;

  // check if the RF Core is sleeping
  if ( rfCoreState == RFCORE_STATE_SLEEPING )
  {
    // it is, so data entry has to be queued manually
    if ( pDataEntryQ->pCurEntry == NULL )
    {
      // queue is empty
      pDataEntryQ->pCurEntry = pDataEntry;
    }
    else // queue not empty
    {
      pDataEntryQ->pLastEntry->pNextEntry = pDataEntry;
    }

    pDataEntryQ->pLastEntry = pDataEntry;

    HAL_EXIT_CRITICAL_SECTION(cs);
  }
  else // radio interface is active
  {
    // setup a radio command to add to Tx RF queue
    rfCmd.cmdNum = CMD_ADD_DATA_ENTRY;
    rfCmd.pQueue = pDataEntryQ;
    rfCmd.pEntry = (uint8 *)pDataEntry;

    HAL_EXIT_CRITICAL_SECTION(cs);

    // issue immediate command to add the data entry to the Tx RF queue and
    // check if we were successful
    if ( MAP_MB_SendCommand( (uint32)&rfCmd ) != CMDSTA_DONE )
    {
      return( RFHAL_ERROR_ADD_TX_ENTRY_FAIL );
    }
  }


  return( RFHAL_SUCCESS );
}


/*******************************************************************************
 * @fn          RFHAL_FreeNextTxDataEntry API
 *
 * @brief       This function is used to free the next TX data entry based on
 *              the internal data entry queue pointer. This routine should be
 *              used after the radio FW indicates a TX Entry Done interrupt.
 *              Once freed, the internal data entry queue is updated to the
 *              next entry.
 *
 *              Note: It is assumed the data entry queue is really based on a
 *                    data queue.
 *
 * input parameters
 *
 * @param       pDataEntryQ - Pointer to data entry queue.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void RFHAL_FreeNextTxDataEntry( dataEntryQ_t *pDataEntryQ )
{
  halIntState_t cs;
  dataEntry_t   *pNextEntry;

  HAL_ENTER_CRITICAL_SECTION(cs);

  // get next data entry to free (i.e. head of internal queue)
  pNextEntry = ((dataQ_t *)pDataEntryQ)->pNextDataEntry;

  // update the internal next data entry pointer based on pCurEntry
  // Note: If this was the last data entry on the queue, then pCurEntry would
  //       be NULL, and so would pNextDataEntry. If this was not the last data
  //       entry on the queue, then pNextDataEntry should point to the current
  //       entry. So pNextEntry in either case.
  ((dataQ_t *)pDataEntryQ)->pNextDataEntry = pNextEntry->pNextEntry;

  // free the TX data entry given by the internal next data entry
  osal_bm_free( (void *)pNextEntry );

  HAL_EXIT_CRITICAL_SECTION(cs);

  return;
}


/*******************************************************************************
 * @fn          RFHAL_AllocDataEntryQueue API
 *
 * @brief       This function is used to dynamically allocate a Data Entry
 *              Queue, and initializes the first and last data entry pointers.
 *              If a ring buffer is used, then the last data entry pointer
 *              should be NULL. This routine also initializes an System internal
 *              pointer to the first entry. This pointer is used by System
 *              software to process the next entry in the data entry queue
 *              as the data entry queue pointers are used by the radio software.
 *
 * input parameters
 *
 * @param       pFirstDataEntry - Pointer to the first data entry in queue.
 * @param       pLastDataEntry  - Pointer to the last data entry in queue, or
 *                                NULL (for example, for ring buffers).
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to a Data Queue, or NULL.
 */
dataEntryQ_t *RFHAL_AllocDataEntryQueue( dataEntry_t *pFirstDataEntry,
                                         dataEntry_t *pLastDataEntry )
{
  dataQ_t *pDataQueue;

#ifdef DEBUG
  RFHAL_ASSERT( pFirstDataEntry != NULL );
#endif // DEBUG

  // allocate a data queue
  if ( (pDataQueue = (dataQ_t *)osal_mem_alloc( sizeof(dataQ_t) )) != NULL )
  {
    // set current and last entry
    pDataQueue->dataEntryQ.pCurEntry  = pFirstDataEntry;
    pDataQueue->dataEntryQ.pLastEntry = pLastDataEntry;

    // set the internal pointer to next data entry
    pDataQueue->pNextDataEntry = pFirstDataEntry;

    // return pointer to just the data entry queue
    return( (dataEntryQ_t *)&pDataQueue->dataEntryQ );
  }
  else // not enough memory left
  {
    return( NULL );
  }
}


/*******************************************************************************
 * @fn          RFHAL_FreeDataEntryQueue API
 *
 * @brief       This function is used to free a dynamically allocated Data
 *              Queue.
 *
 * input parameters
 *
 * @param
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to a Data Queue, or NULL.
 */
void RFHAL_FreeDataEntryQueue( dataEntryQ_t *pDataEntryQ )
{
#ifdef DEBUG
  RFHAL_ASSERT( pDataEntryQ != NULL );
#endif // DEBUG

  osal_mem_free( pDataEntryQ );

  return;
}


/*******************************************************************************
 * @fn          RFHAL_GetNextDataEntry API
 *
 * @brief       This function is used to return a pointer to the next data
 *              entry in the data entry queue that is available for System
 *              processing. Note that this does not necessarily mean the data
 *              entry has be Finished by the radio - to determine this, the
 *              data entry status would have to be first checked. This is only
 *              the data entry to would be processed next by System software.
 *
 * input parameters
 *
 * @param       dataEntryQueue_t - Pointer to data entry queue.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to next data entry of a data queue to be processed.
 */
dataEntry_t *RFHAL_GetNextDataEntry( dataEntryQ_t *pDataEntryQ )
{
#ifdef DEBUG
  RFHAL_ASSERT( pDataEntryQ != NULL );
#endif // DEBUG

  // return next data entry to may be processed by System software
  return( (dataEntry_t *)((dataQ_t *)pDataEntryQ)->pNextDataEntry );
}



/*******************************************************************************
 * @fn          RFHAL_GetTempDataEntry API
 *
 * @brief       This function is used to return a pointer to the next data
 *              entry in the temporary data entry queue that is available for System
 *              processing. Note that this does not necessarily mean the data
 *              entry has be Finished by the radio - to determine this, the
 *              data entry status would have to be first checked. This is only
 *              the data entry to would be processed next by System software.
 *
 * input parameters
 *
 * @param       dataEntryQueue_t - Pointer to data entry queue.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to next data entry of a data queue to be processed.
 */
dataEntry_t *RFHAL_GetTempDataEntry( dataEntryQ_t *pDataEntryQ )
{
#ifdef DEBUG
  RFHAL_ASSERT( pDataEntryQ != NULL );
#endif // DEBUG

  // return next data entry to may be processed by System software
  return( (dataEntry_t *)((dataQ_t *)pDataEntryQ)->pTempDataEntry );
}


/*******************************************************************************
 * @fn          RFHAL_NextDataEntryDone API
 *
 * @brief       This function is used to mark the next System data entry on a
 *              data entry queue as Pending so that the radio can once again
 *              use it. It should be called after the user has processed the
 *              data entry.
 *
 * input parameters
 *
 * @param       dataEntryQueue_t - Pointer to data entry queue.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void RFHAL_NextDataEntryDone( dataEntryQ_t *pDataEntryQ )
{
  halIntState_t  cs;
  dataQ_t       *pDataQueue;

#ifdef DEBUG
  RFHAL_ASSERT( pDataEntryQ != NULL );
#endif // DEBUG

  // point to data queue
  pDataQueue = (dataQ_t *)pDataEntryQ;

  if ( pDataQueue->pNextDataEntry != NULL )
  {
    HAL_ENTER_CRITICAL_SECTION(cs);

    // mark the next System data entry as Pending
    pDataQueue->pNextDataEntry->status = DATASTAT_PENDING;

    // advance to the next data entry in the data entry queue
    pDataQueue->pNextDataEntry = pDataQueue->pNextDataEntry->pNextEntry;

    HAL_EXIT_CRITICAL_SECTION(cs);

    // return pointer to next entry, or NULL if there isn't one
    // Note: For a ring buffer, there is always another.
    return; //( pDataQueue->pNextDataEntry );
  }
  else // we are at the end of a linked list
  {
    // ALT: Could set pNextDataEntry to first entry, but could be problematic
    //       if the radio data queue commands are being used to add/remove
    //       data entries.
  }

  // return next data entry to may be processed by System software
  return;
}


/*******************************************************************************
 * @fn          RFHAL_AllocDataEntry API
 *
 * @brief       This function is used to allocate and initialize a data entry
 *              header and associated buffer.
 *
 * input parameters
 *
 * @param       entryType - DATA_ENTRY_TYPE_GENERAL, DATA_ENTRY_TYPE_EXTENDED,
 *                          DATA_ENTRY_TYPE_POINTER.
 * @param       lenSize   - Specifies the number of bytes of the length added
 *                          to the start of each entry element:
 *                          DATA_ENTRY_LEN_SIZE_0, DATA_ENTRY_LEN_SIZE_1,
 *                          DATA_ENTRY_LEN_SIZE_2.
 * @param       dataSize  - Number of bytes in data buffer.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to first data entry in linked list, or NULL if there
 *              are any errors with the parameters, or if the amount of needed
 *              heap memory is not available.
 */
uint8 *RFHAL_AllocDataEntry( uint8  entryType,
                             uint8  lenSize,
                             uint16 dataSize )
{
  dataEntry_t *pEntry;
  uint8        size;

  // verify data entry length size is valid
  if ( (lenSize != DATA_ENTRY_LEN_SIZE_0) &&
       (lenSize != DATA_ENTRY_LEN_SIZE_1) &&
       (lenSize != DATA_ENTRY_LEN_SIZE_2) )
  {
    return( NULL ); // RFHAL_ERROR_INVALID_PARAM
  }

  // check what type of data entry header is needed
  if ( entryType == DATA_ENTRY_TYPE_GENERAL )
  {
    // Entry Header Type: Normal
    size = sizeof( dataEntry_t ) + dataSize;
  }
  else if ( entryType == DATA_ENTRY_TYPE_EXTENDED )
  {
    // Entry Header Type: Extended
    size = sizeof( dataEntryExt_t ) + dataSize;
  }
  else if ( entryType == DATA_ENTRY_TYPE_POINTER )
  {
    // Entry Header Type: Pointer
    size = sizeof( dataEntryPtr_t ) + dataSize;
  }
  else  // unknown data entry type
  {
    return( NULL );  // RFHAL_ERROR_INVALID_PARAM
  }

  // Note: General and Extended data entry headers have common init fields.
  if ( (pEntry = (dataEntry_t *)osal_mem_alloc( size )) != NULL )
  {
    // initialize data entry header
    pEntry->pNextEntry  = NULL;
    pEntry->status      = DATASTAT_PENDING;
    pEntry->config      = entryType | lenSize;
    pEntry->length      = dataSize;

    // check and init Extended header fields
    if ( entryType == DATA_ENTRY_TYPE_EXTENDED )
    {
      ((dataEntryExt_t *)pEntry)->numElements = 0;
      ((dataEntryExt_t *)pEntry)->nextIndex   = 0;
    }
  }

  return( (uint8 *)pEntry );
}


/*******************************************************************************
 * @fn          RFHAL_FreeDataEntry API
 *
 * @brief       This function is used to free a dynamically allocated data
 *              entry.
 *
 * input parameters
 *
 * @param       pDataEntry - Pointer to data entry.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void RFHAL_FreeDataEntry( uint8 *pDataEntry )
{
#ifdef DEBUG
  RFHAL_ASSERT( pDataEntry != NULL );
#endif // DEBUG

  osal_mem_free( pDataEntry );

  return;
}


/*******************************************************************************
 * @fn          RFHAL_BuildRingBuffer API
 *
 * @brief       This function is used to build a ring buffer of arbitrary size,
 *              using fixed sized queue entries. When done, a pointer to an
 *              arbitrary data entry is returned.
 *
 *              Note: Might be easier to just create an array of data entries
 *                    so if the malloc fails, it is easier to just free them.
 *                    And if the memory is there, connecting them could be
 *                    faster too.
 *              Note: Might be easier to have separate routines for Normal
 *                    and Extended data entry headers.
 *
 * input parameters
 *
 * @param       entryType  - DATA_ENTRY_TYPE_GENERAL, DATA_ENTRY_TYPE_EXTENDED,
 *                           DATA_ENTRY_TYPE_POINTER
 * @param       lenSize    - Specifies the number of bytes of the length added
 *                           to the start of each entry element:
 *                           DATA_ENTRY_LEN_SIZE_0, DATA_ENTRY_LEN_SIZE_1,
 *                           DATA_ENTRY_LEN_SIZE_2.
 * @param       numEntries - Number of data entries in ring buffer.
 * @param       dataSize   - Number of bytes in data buffer.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to a data entry in ring buffer, or NULL if there
 *              are any errors with the parameters, or if the amount of needed
 *              heap memory is not available.
 */
uint8 *RFHAL_BuildRingBuffer( uint8  entryType,
                              uint8  lenSize,  // how about addLenSize?
                              uint8  numEntries,
                              uint16 dataSize )
{
  uint8 i;
  uint8 *nHdr;
  uint16 size;
  uint8 *pHdr = NULL;

  // verify data entry length size is valid
  if ( (lenSize != DATA_ENTRY_LEN_SIZE_0) &&
       (lenSize != DATA_ENTRY_LEN_SIZE_1) &&
       (lenSize != DATA_ENTRY_LEN_SIZE_2) )
  {
    return( NULL ); // RFHAL_ERROR_INVALID_PARAM
  }

  // check what type of data entry header is needed
  if ( entryType == DATA_ENTRY_TYPE_GENERAL )
  {
    // Entry Header Type: Normal
    size = sizeof( dataEntry_t ) + dataSize;
  }
  else if ( entryType == DATA_ENTRY_TYPE_EXTENDED )
  {
    // Entry Header Type: Extended
    size = sizeof( dataEntryExt_t ) + dataSize;
  }
  else if ( entryType == DATA_ENTRY_TYPE_POINTER )
  {
    // Entry Header Type: Pointer
    size = sizeof( dataEntryPtr_t ) + dataSize;
  }
  else  // unknown data entry type
  {
    return( NULL );  // RFHAL_ERROR_INVALID_PARAM
  }

  // build ring buffer of entry headers
  // Note: General and Extended Data Headers both use pNextEntry.
  for( i=0; i<numEntries; i++)
  {
    // allocate memory for data entry with buffer
    if ( (nHdr = (uint8 *)osal_mem_alloc( size )) == NULL )
    {
      // check if any data entries have already been allocated
      if ( pHdr != NULL )
      {
        // there's at least one previously allocated data entry and buffer, and
        // since we are unable to complete the ring buffer, we will unwind and
        // free all previously allocated data entries

        // first, make it look like a linked list
        nHdr = (uint8 *)(((dataEntry_t *)pHdr)->pNextEntry);
        ((dataEntry_t *)pHdr)->pNextEntry = NULL;

        // now free up each buffer
        do
        {
          pHdr = nHdr;
          nHdr = (uint8 *)(((dataEntry_t *)pHdr)->pNextEntry);
          osal_mem_free( (void *)pHdr );
        } while ( nHdr != NULL );

        // and indicate we can't create the ring buffer
        pHdr = NULL;
      }

      break;
    }

    // initialize data entry header
    ((dataEntry_t *)nHdr)->pNextEntry = NULL;
    ((dataEntry_t *)nHdr)->status     = DATASTAT_PENDING;
    ((dataEntry_t *)nHdr)->config     = entryType | lenSize;
    ((dataEntry_t *)nHdr)->length     = dataSize;

    // check and init Extended header fields
    if ( entryType == DATA_ENTRY_TYPE_EXTENDED )
    {
      ((dataEntryExt_t *)nHdr)->numElements = 0;
      ((dataEntryExt_t *)nHdr)->nextIndex   = 0;
    }

    // check if this is the first data entry
    if ( pHdr == NULL )
    {
      // it is, so point to self
      pHdr = nHdr;
      ((dataEntry_t *)pHdr)->pNextEntry = (dataEntry_t *)nHdr;
    }
    else // there's at least one previously allocated data entry
    {
      // so
      ((dataEntry_t *)nHdr)->pNextEntry = ((dataEntry_t *)pHdr)->pNextEntry;
      ((dataEntry_t *)pHdr)->pNextEntry = (dataEntry_t *)nHdr;
      pHdr = nHdr;
    }
  }

  // use pHdr so if the numEntries is zero, NULL will be returned
  // Note: Since it is a ring buffer, the head of the ring can be any data
  //       header entry. If the first allocated data header entry has to be
  //       returned, then pHdr has to be checked for NULL; if so, return NULL,
  //       otherwise, return (uint8 *)((dataEntry_t *)nHdr)->pNextEntry.
  return( pHdr );
}


/*******************************************************************************
 * @fn          RFHAL_BuildDataEntryRingBuffer API
 *
 * @brief       This function is used to build a ring buffer of arbitrary size,
 *              using fixed sized queue entries. When done, a pointer to an
 *              arbitrary data entry is returned.
 *
 *              Note: Might be easier to just create an array of data entries
 *                    so if the malloc fails, it is easier to just free them.
 *                    And if the memory is there, connecting them could be
 *                    faster too.
 *
 * input parameters
 *
 * @param       numEntries - Number of data entries in ring buffer.
 * @param       prefixSize - Number of bytes before the header.
 * @param       dataSize   - Number of bytes in data buffer.
 * @param       suffixSize - Number of bytes after the payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to a data entry in ring buffer, or NULL if there
 *              are any errors with the parameters, or if the amount of needed
 *              heap memory is not available.
 */
dataEntry_t *RFHAL_BuildDataEntryRingBuffer( uint8  numEntries,
                                             uint8  prefixSize,
                                             uint16 dataSize,
                                             uint8  suffixSize )
{
  uint8  i;
  uint16  totalEntrySize;
  uint8  *nHdr;
  uint8  *pHdr = NULL;

  // determine total size of data entry, including headers, prefix, and suffix
  totalEntrySize = sizeof( dataEntry_t ) + prefixSize + suffixSize + dataSize;

  // round data entry size to a multiple of four bytes
  totalEntrySize += (totalEntrySize % 4)?(4 - (totalEntrySize % 4)):0;

  // build ring buffer of general entry headers
  for( i=0; i<numEntries; i++)
  {
    // allocate memory for data entry with buffer
    if ( (nHdr = (uint8 *)osal_mem_alloc( totalEntrySize )) == NULL )
    {
      // check if any data entries have already been allocated
      if ( pHdr != NULL )
      {
        // there's at least one previously allocated data entry and buffer, and
        // since we are unable to complete the ring buffer, we will unwind and
        // free all previously allocated data entries

        // first, make it look like a linked list
        nHdr = (uint8 *)(((dataEntry_t *)pHdr)->pNextEntry);
        ((dataEntry_t *)pHdr)->pNextEntry = NULL;

        // now free up each buffer
        do
        {
          pHdr = nHdr;
          nHdr = (uint8 *)(((dataEntry_t *)pHdr)->pNextEntry);
          osal_mem_free( (void *)pHdr );
        } while ( nHdr != NULL );

        // and indicate we can't create the ring buffer
        pHdr = NULL;
      }

      break;
    }

    // initialize data entry header
    ((dataEntry_t *)nHdr)->pNextEntry = NULL;
    ((dataEntry_t *)nHdr)->status     = DATASTAT_PENDING;
    ((dataEntry_t *)nHdr)->config     = DATA_ENTRY_TYPE_GENERAL | DATA_ENTRY_LEN_SIZE_0;
    ((dataEntry_t *)nHdr)->length     = dataSize + suffixSize;

    // check if this is the first data entry
    if ( pHdr == NULL )
    {
      // it is, so point to self
      pHdr = nHdr;
      ((dataEntry_t *)pHdr)->pNextEntry = (dataEntry_t *)nHdr;
    }
    else // there's at least one previously allocated data entry
    {
      // so
      ((dataEntry_t *)nHdr)->pNextEntry = ((dataEntry_t *)pHdr)->pNextEntry;
      ((dataEntry_t *)pHdr)->pNextEntry = (dataEntry_t *)nHdr;
      pHdr = nHdr;
    }
  }

  // use pHdr so if the numEntries is zero, NULL will be returned
  // Note: Since it is a ring buffer, the head of the ring can be any data
  //       header entry. If the first allocated data header entry has to be
  //       returned, then pHdr has to be checked for NULL; if so, return NULL,
  //       otherwise, return (uint8 *)((dataEntry_t *)nHdr)->pNextEntry.
  return( (dataEntry_t *)pHdr );
}


/*******************************************************************************
 * @fn          RFHAL_BuildLinkedBuffer API
 *
 * @brief       This function is used to build a linked list buffer of arbitrary
 *              size, using fixed sized queue entries. When done, a pointer to
 *              the first data entry is returned, and the last data entry
 *              points to NULL.
 *
 * input parameters
 *
 * @param       entryType  - DATA_ENTRY_TYPE_GENERAL, DATA_ENTRY_TYPE_EXTENDED,
 *                           DATA_ENTRY_TYPE_POINTER
 * @param       lenSize    - Specifies the number of bytes of the length added
 *                           to the start of each entry element:
 *                           DATA_ENTRY_LEN_SIZE_0, DATA_ENTRY_LEN_SIZE_1,
 *                           DATA_ENTRY_LEN_SIZE_2.
 * @param       numEntries - Number of data entries in ring buffer.
 * @param       dataSize   - Number of bytes in data buffer.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to first data entry in linked list, or NULL if there
 *              are any errors with the parameters, or if the amount of needed
 *              heap memory is not available.
 */
uint8 *RFHAL_BuildLinkedBuffer( uint8  entryType,
                                uint8  lenSize,
                                uint8  numEntries,
                                uint16 dataSize )
{
  uint8 *pHdr = NULL;
  uint8 *nHdr;
  uint8  size;
  uint8  i;

  // verify data entry length size is valid
  if ( (lenSize != DATA_ENTRY_LEN_SIZE_0) &&
       (lenSize != DATA_ENTRY_LEN_SIZE_1) &&
       (lenSize != DATA_ENTRY_LEN_SIZE_2) )
  {
    return( NULL ); // RFHAL_ERROR_INVALID_PARAM
  }

  // check what type of data entry header is needed
  if ( entryType == DATA_ENTRY_TYPE_GENERAL )
  {
    // Entry Header Type: Normal
    size = sizeof( dataEntry_t ) + dataSize;
  }
  else if ( entryType == DATA_ENTRY_TYPE_EXTENDED )
  {
    // Entry Header Type: Extended
    size = sizeof( dataEntryExt_t ) + dataSize;
  }
  else if ( entryType == DATA_ENTRY_TYPE_POINTER )
  {
    // Entry Header Type: Pointer
    size = sizeof( dataEntryPtr_t ) + dataSize;
  }
  else  // unknown data entry type
  {
    return( NULL );  // RFHAL_ERROR_INVALID_PARAM
  }

  // build linked list of entry headers
  // Note: General and Extended Data Headers both use pNextEntry.
  for( i=0; i<numEntries; i++)
  {
    // allocate memory for data entry with buffer
    if ( (nHdr = (uint8 *)osal_mem_alloc( size )) == NULL )
    {
      // check if any data entries have already been allocated
      if ( pHdr != NULL )
      {
        // there's at least one previously allocated data entry and buffer, and
        // since we are unable to complete the ring buffer, we will unwind and
        // free all previously allocated data entries

        // free up each buffer
        do
        {
          pHdr = nHdr;
          nHdr = (uint8 *)(((dataEntry_t *)pHdr)->pNextEntry);
          osal_mem_free( (void *)pHdr );
        } while ( nHdr != NULL );

        // and indicate we can't create the ring buffer
        pHdr = NULL;
      }

      break;
    }

    // initialize data entry header
    ((dataEntry_t *)nHdr)->pNextEntry = NULL;
    ((dataEntry_t *)nHdr)->status     = DATASTAT_PENDING;
    ((dataEntry_t *)nHdr)->config     = entryType | lenSize;
    ((dataEntry_t *)nHdr)->length     = dataSize;

    // check and init Extended header fields
    if ( entryType == DATA_ENTRY_TYPE_EXTENDED )
    {
      ((dataEntryExt_t *)nHdr)->numElements = 0;
      ((dataEntryExt_t *)nHdr)->nextIndex   = 0;
    }

    // work backwards, hooking in next data entry
    ((dataEntry_t *)nHdr)->pNextEntry = (dataEntry_t *)pHdr;
    pHdr = nHdr;
  }

  // use pHdr so if the numEntries is zero, NULL will be returned
  return( pHdr );
}

/*******************************************************************************
 */


