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
 *                                              Ex_AuthGet()
 *
 * @brief  Provides example on how to use the Auth sub-module of Common to get an existing user.
 *******************************************************************************************************/
void Ex_AuthGet(void)
{
  AUTH_USER_HANDLE user_a_unvalidated_handle;
  RTOS_ERR         err;

  //                                                               The user must have been created before, for this ...
  user_a_unvalidated_handle = Auth_GetUser("UserA", &err);      // call to be successful.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               If obtained, this merely means that the user exists.
  PP_UNUSED_PARAM(user_a_unvalidated_handle);
}
