/***************************************************************************//**
 * @file
 * @brief Common Auth Example
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>
#include  <cpu/include/cpu.h>
#include  <common/include/auth.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            Ex_AuthValidate()
 *
 * @brief  Provides example on how to use the Auth sub-module of Common to validate credentials.
 *******************************************************************************************************/
void Ex_AuthValidate(void)
{
  AUTH_USER_HANDLE user_handle;
  RTOS_ERR         err;

  (void)Auth_CreateUser("UserB", "PwdB", &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  user_handle = Auth_ValidateCredentials("UserB", "Pwd0", &err);
  if (err.Code == RTOS_ERR_INVALID_CREDENTIALS) {
    //                                                             Invalid user name/password combination.
    // At this point, we would normally return or indicate an error.
  } else if (err.Code != RTOS_ERR_NONE) {
    // Handle error.
  }

  user_handle = Auth_ValidateCredentials("UserB", "PwdB", &err);
  if (err.Code == RTOS_ERR_INVALID_CREDENTIALS) {
    //                                                             Invalid user name/password combination.
  } else if (err.Code != RTOS_ERR_NONE) {
    // Handle error.
  }
  // If credentials were good, we may continue and use the 'user_handle' obtained.

  PP_UNUSED_PARAM(user_handle);
}
