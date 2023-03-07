/***************************************************************************//**
 * @file
 * @brief Silicon Labs implementation of FreeRTOS Bluetooth Low Energy library.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

/* Standard library includes */
#include <stddef.h>

/* FreeRTOS BLE HAL includes */
#include "bt_hal_gatt_client.h"

/* Silicon Labs includes */
#include "sl_bt_hal_common.h"
#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_malloc.h"

/* Register a BLE HAL client */
BTStatus_t prvRegisterBleHalClient( BTUuid_t * pxAppUuid,
                                    SlBtBleHalClient_t ** ppxClientListHead,
                                    uint8_t ucMaxClients,
                                    size_t xContextSize,
                                    SlBtBleHalClient_t ** ppxClient )
{
  /* Check parameters */
  if( ( pxAppUuid == NULL ) || ( ppxClientListHead == NULL ) || ( ppxClient == NULL ) )
  {
    return eBTStatusParamInvalid;
  }

  /* Initialize the outputs */
  *ppxClient = NULL;

  /* See if the app has already registered */
  SlBtBleHalClient_t * pxClient = *ppxClientListHead;
  while( pxClient != NULL )
  {
    if( prvIsMatchingUuid( &pxClient->xAppUuid, pxAppUuid ) )
    {
      /* App has already registered. Return an error. */
      return eBTStatusDone;
    }

    pxClient = pxClient->pxNext;
  }

  /* Find the smallest handle value that is not already in use. This is O(n^2), but the maximum
  number is small in practical configurations and registration is not a frequent operation. */
  uint8_t ucFirstFreeHandle = SL_BT_INVALID_IF_HANDLE;
  for( uint8_t ucCandidateHandle = 1; ucCandidateHandle <= ucMaxClients; ucCandidateHandle++ )
  {
    /* See if an existing client has this handle */
    pxClient = *ppxClientListHead;
    while( ( pxClient != NULL ) && ( pxClient->ucHandle != ucCandidateHandle ) )
    {
      pxClient = pxClient->pxNext;
    }

    /* If no existing client matched this handle, we found a free handle value */
    if( pxClient == NULL )
    {
      ucFirstFreeHandle = ucCandidateHandle;
      break;
    }
  }

  /* If we didn't find a handle within range, we've reached the maximum number of registrants */
  if( ucFirstFreeHandle == SL_BT_INVALID_IF_HANDLE )
  {
    return eBTStatusNoMem;
  }

  /* Allocate a context */
  SlBtBleHalClient_t * pxContext = sl_malloc( xContextSize );
  if( pxContext == NULL )
  {
    return eBTStatusNoMem;
  }

  /* Initialize and add it to the list */
  memset( pxContext, 0, xContextSize );
  pxContext->ucHandle = ucFirstFreeHandle;
  pxContext->xAppUuid = *pxAppUuid;
  pxContext->pxNext = *ppxClientListHead;
  *ppxClientListHead = pxContext;

  /* Set the output */
  *ppxClient = pxContext;

  return eBTStatusSuccess;
}

/* Free a BLE HAL client context */
void prvFreeBleHalClient( SlBtBleHalClient_t ** ppxClientListHead,
                          SlBtBleHalClient_t * pxClient )
{
  /* Find the entry that points to the client we're freeing */
  SlBtBleHalClient_t ** ppxPointerToClient = NULL;
  SlBtBleHalClient_t ** ppxPointerCandidate = ppxClientListHead;
  while( *ppxPointerCandidate != NULL )
  {
    if( *ppxPointerCandidate == pxClient )
    {
      /* This points to our client. Stop here. */
      ppxPointerToClient = ppxPointerCandidate;
      break;
    }

    SlBtBleHalClient_t * pxNextClient = *ppxPointerCandidate;
    ppxPointerCandidate = &pxNextClient->pxNext;
  }

  /* If the client was in the list, disconnect it */
  if( ppxPointerToClient != NULL )
  {
    *ppxPointerToClient = pxClient->pxNext;
  }

  /* Free the context memory */
  sl_free( pxClient );
}

/* Find a BLE HAL client context based on its handle */
SlBtBleHalClient_t * prvGetBleHalClientByHandle( uint8_t ucHandle,
                                                 SlBtBleHalClient_t * pxFirstClient )
{
  /* Find the client that has this handle */
  SlBtBleHalClient_t * pxClient = pxFirstClient;
  while( pxClient != NULL )
  {
    if( pxClient->ucHandle == ucHandle )
    {
      return pxClient;
    }

    pxClient = pxClient->pxNext;
  }

  /* No client found with this handle */
  return NULL;
}

/* Map an sl_status_t value to a BTStatus_t */
BTStatus_t prvSlStatusToBTStatus( sl_status_t sl_status )
{
  BTStatus_t status = eBTStatusFail;

  switch( sl_status )
  {
    case SL_STATUS_OK:
      /* Success */
      status = eBTStatusSuccess;
      break;

    case SL_STATUS_NOT_READY:                                  // Fall-through
    case SL_STATUS_INVALID_STATE:
      /* Service/Driver/Peripheral not ready */
      status = eBTStatusNotReady;
      break;

    case SL_STATUS_NO_MORE_RESOURCE: // Fall-through
    case SL_STATUS_BT_CTRL_MEMORY_CAPACITY_EXCEEDED:
      /* Failed to allocate memory */
      status = eBTStatusNoMem;
      break;

    case SL_STATUS_BUSY:                      // Fall-through
    case SL_STATUS_IN_PROGRESS:               // Fall-through
    case SL_STATUS_BT_CTRL_HOST_BUSY_PAIRING: // Fall-through
    case SL_STATUS_BT_CTRL_CONTROLLER_BUSY:
      /* Service/Driver/Peripheral busy */
      status = eBTStatusBusy;
      break;

    case SL_STATUS_NOT_SUPPORTED:                                  // Fall-through
    case SL_STATUS_BT_CTRL_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE: // Fall-through
    case SL_STATUS_BT_CTRL_UNSUPPORTED_REMOTE_FEATURE:
      /* Unsupported request/call */
      status = eBTStatusUnsupported;
      break;

    case SL_STATUS_INVALID_PARAMETER:         // Fall-through
    case SL_STATUS_BT_ATT_INVALID_ATT_LENGTH: // Fall-through
    case SL_STATUS_COMMAND_TOO_LONG:
      /* Invalid parameter */
      status = eBTStatusParamInvalid;
      break;

    case SL_STATUS_BT_CTRL_AUTHENTICATION_FAILURE:
      /* Authentication procedure failed */
      status = eBTStatusAuthFailure;
      break;

    case SL_STATUS_BT_CTRL_REMOTE_USER_TERMINATED:                                   // Fall-through
    case SL_STATUS_BT_CTRL_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES: // Fall-through
    case SL_STATUS_BT_CTRL_REMOTE_POWERING_OFF:
      /* Remote Device Down */
      status = eBTStatusRMTDevDown;
      break;

    case SL_STATUS_BT_CTRL_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS:
      /* Authentication rejected */
      status = eBTStatusAuthRejected;
      break;

    case SL_STATUS_BT_CTRL_CONNECTION_TIMEOUT:
      /* Link lost */
      status = eBTStatusLinkLoss;
      break;

    default:
      /* Generic fail */
      status = eBTStatusFail;
      break;
  }

  return status;
}

/* Map an sl_status_t value to a BTGattStatus_t */
BTGattStatus_t prvSlStatusToBTGattStatus( sl_status_t sl_status )
{
  BTGattStatus_t status = eBTGattStatusError;

  switch( sl_status )
  {
    case SL_STATUS_OK:                              // Fall-through
      /* Remote user termination considered as success because there is no
      corresponding error code and it is not really an error. */
    case SL_STATUS_BT_CTRL_REMOTE_USER_TERMINATED:
      status = eBTGattStatusSuccess;
      break;

    case SL_STATUS_BT_CTRL_CONNECTION_TERMINATED_BY_LOCAL_HOST:
      status = eBTGattStatusLocalHostTerminatedConnection;
      break;

    case SL_STATUS_TIMEOUT:                         // Fall-through
    case SL_STATUS_BT_CTRL_CONNECTION_TIMEOUT:
      status = eBTGattStatusErrorConnTimeout;
      break;

    default:
      /* Generic fail */
      status = eBTGattStatusError;
      break;
  }

  return status;
}

/* Check if two UUIDs match */
bool prvIsMatchingUuid( BTUuid_t * pxUuid1, BTUuid_t * pxUuid2 )
{
  /* If type is different, consider it a mismatch */
  if( pxUuid1->ucType != pxUuid2->ucType )
  {
    return false;
  }

  switch( pxUuid1->ucType )
  {
    case eBTuuidType16:
      return( pxUuid1->uu.uu16 == pxUuid2->uu.uu16 );

    case eBTuuidType32:
      return( pxUuid1->uu.uu32 == pxUuid2->uu.uu32 );

    case eBTuuidType128:
      return( memcmp( pxUuid1->uu.uu128, pxUuid2->uu.uu128, sizeof(pxUuid1->uu.uu128) ) == 0 );
  }

  /* Return no match on invalid types */
  return false;
}
