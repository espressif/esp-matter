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
 *                                             Ex_AuthRights()
 *
 * @brief  Provides example on how to use the Auth sub-module of Common to manage rights.
 *******************************************************************************************************/
void Ex_AuthRights(void)
{
  AUTH_USER_HANDLE user_a_unvalidated_handle;
  AUTH_USER_HANDLE admin_validated_handle;
  AUTH_RIGHT       right;
  RTOS_ERR         err;

  //                                                               The user must have been created before, for this ...
  user_a_unvalidated_handle = Auth_GetUser("UserA", &err);      // call to be successful.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               If obtained, this only means that the user exists.

  admin_validated_handle = Auth_ValidateCredentials("admin", "admin", &err);
  if (err.Code == RTOS_ERR_INVALID_CREDENTIALS) {
    //                                                             Invalid user name/password combination.
    // At this point, we would normally return or indicate an error.
  } else if (err.Code != RTOS_ERR_NONE) {
    // Handle error.
  }
  // If credentials were good, we may continue and use the 'user_handle' obtained.

  right = Auth_GetUserRight(user_a_unvalidated_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  // At this point, 'right' does not contain any bit set, since no right has been granted to that user.

  Auth_GrantRight((AUTH_RIGHT_6 | AUTH_RIGHT_8), user_a_unvalidated_handle, admin_validated_handle, &err);
  if (err.Code == RTOS_ERR_PERMISSION) {
    // This would mean that the 'as_user_handle' does not have the right to add.
    // At this point, we would normally return or indicate an error.
  } else if (err.Code != RTOS_ERR_NONE) {
    // Handle error.
  }
  // If no error, rights 6 and 8 were added, let's confirm.

  right = Auth_GetUserRight(user_a_unvalidated_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  // At this point, 'right' has the AUTH_RIGHT_6 and AUTH_RIGHT_8 set, since they have been granted to that user.

  Auth_RevokeRight(AUTH_RIGHT_6, user_a_unvalidated_handle, admin_validated_handle, &err);
  if (err.Code == RTOS_ERR_PERMISSION) {
    // This would mean that the 'as_user_handle' does not have the right to revoke.
    // At this point, we would normally return or indicate an error.
  } else if (err.Code != RTOS_ERR_NONE) {
    // Handle error.
  }
  //                                                               If no error, right 6 was revoked.

  right = Auth_GetUserRight(user_a_unvalidated_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  // At this point, 'right' has ONLY the AUTH_RIGHT_8 set, since AUTH_RIGHT_6 has been revoked.

  PP_UNUSED_PARAM(right);
}
