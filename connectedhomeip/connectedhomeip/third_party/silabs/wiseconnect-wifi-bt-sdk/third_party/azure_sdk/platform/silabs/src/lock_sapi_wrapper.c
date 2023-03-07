/*******************************************************************************
* @file  lock_sapi_wrapper.c
* @brief 
*******************************************************************************
* Copyright (c) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE file in the project root for full license information.
*
*******************************************************************************
*
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

/**
 * Includes
 */
#ifdef AZURE_ENABLE

#include "azure_c_shared_utility/lock.h"
#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_macro_utils/macro_utils.h"
#include "rsi_driver.h"

LOCK_HANDLE Lock_Init(void)
{
  //! Codes_SRS_LOCK_10_002: [Lock_Init on success shall return a valid lock handle which should be a non NULL value] 
  //! Codes_SRS_LOCK_10_003: [Lock_Init on error shall return NULL ] 
  rsi_mutex_handle_t *result = (rsi_mutex_handle_t *)malloc(sizeof(rsi_mutex_handle_t));
  if (result == NULL) {
    LogError("malloc failed.");
  } else {
    if (rsi_mutex_create((uint32_t *)result) != RSI_ERROR_NONE) {
      LogError("rsi_mutex_create failed.");
      free(result);
      result = NULL;
    }
  }

  return (LOCK_HANDLE)result;
}

LOCK_RESULT Lock_Deinit(LOCK_HANDLE handle)
{
  LOCK_RESULT result;
  if (handle == NULL) {
    //! Codes_SRS_LOCK_10_007: [Lock_Deinit on NULL handle passed returns LOCK_ERROR] 
    LogError("Invalid argument; handle is NULL.");
    result = LOCK_ERROR;
  } else {
    //! Codes_SRS_LOCK_10_012: [Lock_Deinit frees the memory pointed by handle] 
    if (rsi_mutex_destroy((rsi_mutex_handle_t *)handle) == RSI_ERROR_NONE) {
      free(handle);
      handle = NULL;
      result = LOCK_OK;
    } else {
      LogError("pthread_mutex_destroy failed;");
      result = LOCK_ERROR;
    }
  }

  return result;
}

LOCK_RESULT Lock(LOCK_HANDLE handle)
{
  LOCK_RESULT result;
  if (handle == NULL) {
    //! Codes_SRS_LOCK_10_007: [Lock on NULL handle passed returns LOCK_ERROR] 
    LogError("Invalid argument; handle is NULL.");
    result = LOCK_ERROR;
  } else {
    //! AcquireSRWLockExclusive((SRWLOCK*)handle);
    if (rsi_mutex_lock((uint32_t *)handle) == RSI_ERROR_NONE) {
      //! Codes_SRS_LOCK_10_005: [Lock on success shall return LOCK_OK] 
      result = LOCK_OK;
    } else {
      //! Codes_SRS_LOCK_10_006: [Lock on error shall return LOCK_ERROR] 
      LogError("pthread_mutex_lock failed.");
      result = LOCK_ERROR;
    }
  }

  return result;
}

LOCK_RESULT Unlock(LOCK_HANDLE handle)
{
  LOCK_RESULT result;
  if (handle == NULL) {
    //! Codes_SRS_LOCK_10_007: [Unlock on NULL handle passed returns LOCK_ERROR] 
    LogError("Invalid argument; handle is NULL.");
    result = LOCK_ERROR;
  } else {
    if (rsi_mutex_unlock((uint32_t *)handle) == RSI_ERROR_NONE) {
      //! Codes_SRS_LOCK_10_009: [Unlock on success shall return LOCK_OK] 
      result = LOCK_OK;
    } else {
      //! Codes_SRS_LOCK_10_010: [Unlock on error shall return LOCK_ERROR] 
      LogError("pthread_mutex_unlock failed.");
      result = LOCK_ERROR;
    }
  }

  return result;
}
#endif

