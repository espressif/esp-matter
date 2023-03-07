/***************************************************************************//**
 * @file
 * @brief Configuration file for CLI password protection.
 * @version x.y.z
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_CLI_SIMPLE_PASSWORD_CONFIG_H
#define SL_CLI_SIMPLE_PASSWORD_CONFIG_H

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

// <o SL_CLI_SIMPLE_PASSWORD_LENGTH_MAX> Password maximum length
// <i> Default: 64
#define SL_CLI_SIMPLE_PASSWORD_LENGTH_MAX        64

// <o SL_CLI_SIMPLE_PASSWORD_LENGTH_MIN> Password minimum length
// <i> Default: 4
#define SL_CLI_SIMPLE_PASSWORD_LENGTH_MIN         4

// <o SL_CLI_SIMPLE_PASSWORD_ECHO_PASSWORD> Echo provided password
// <i> Default: 1
// <i> If disabled, '*' will be written for every character received.
#define SL_CLI_SIMPLE_PASSWORD_ECHO_PASSWORD      1

// <o SL_CLI_SIMPLE_PASSWORD_MAX_RETRY> Maximum number of failed attempts reached.Instance is locked.
// <i> Default: 5
#define SL_CLI_SIMPLE_PASSWORD_MAX_RETRY          5

// <s SL_CLI_SIMPLE_PASSWORD_LOCK_OUT_TIMEOUT_SEC> Session lockout timeout
// <i> Default: 300
// <i> Timeout before being able to retry
#define SL_CLI_SIMPLE_PASSWORD_LOCK_OUT_TIMEOUT_SEC               (5 * 60)

// <s SL_CLI_SIMPLE_PASSWORD_WELCOME_MESSAGE> First message displayed at startup
// <i> Default: "Silicon Labs Terminal"
#define SL_CLI_SIMPLE_PASSWORD_WELCOME_MESSAGE                    "Silicon Labs Terminal"

// <s SL_CLI_SIMPLE_PASSWORD_ASK_PASSWORD_FIRST_START_MESSAGE> Message for asking to setup a first password
// <i> Default: "Please configure an initial password:"
#define SL_CLI_SIMPLE_PASSWORD_ASK_PASSWORD_FIRST_START_MESSAGE   "Please configure an initial password:"

// <s SL_CLI_SIMPLE_PASSWORD_NEW_PASSWORD_CONFIRMATION_MESSAGE> Message for asking password confirmation
// <i> Default: "Please confirm the initial password:"
#define SL_CLI_SIMPLE_PASSWORD_NEW_PASSWORD_CONFIRMATION_MESSAGE  "Please confirm the initial password:"

// <s SL_CLI_SIMPLE_PASSWORD_ASK_PASSWORD> Message for asking password
// <i> Default: "Enter password:"
#define SL_CLI_SIMPLE_PASSWORD_ASK_PASSWORD                       "Enter password:"

// <s SL_CLI_SIMPLE_PASSWORD_CONFIRMATION_DO_NOT_MATCH> Message for password doesn't match
// <i> Default: "Password doesn't match"
#define SL_CLI_SIMPLE_PASSWORD_CONFIRMATION_DO_NOT_MATCH          "Password doesn't match"

// <s SL_CLI_SIMPLE_PASSWORD_INVALID_PASSWORD> Message for invalid password
// <i> Default: "Invalid password"
#define SL_CLI_SIMPLE_PASSWORD_INVALID_PASSWORD                   "Invalid password"

// <s SL_CLI_SIMPLE_PASSWORD_CONFIGURED_BY_OTHER_INSTANCE> Message displayed when password configured from another instance
// <i> Default: "The Initial password has been configured by another instance"
#define SL_CLI_SIMPLE_PASSWORD_CONFIGURED_BY_OTHER_INSTANCE       "The Initial password has been configured by another instance"

// <s SL_CLI_SIMPLE_PASSWORD_NEW_PASSWORD_CONFIGURED> Message displayed when new password is configured
// <i> Default: "New password configured successfully"
#define SL_CLI_SIMPLE_PASSWORD_NEW_PASSWORD_CONFIGURED            "New password configured successfully"

// <s SL_CLI_SIMPLE_PASSWORD_PASSWORD_TOO_SHORT> Message displayed when provided password is too short
// <i> Default: "Password is too short"
#define SL_CLI_SIMPLE_PASSWORD_PASSWORD_TOO_SHORT                 "Password is too short"

// <s SL_CLI_SIMPLE_PASSWORD_LOCK_OUT_MESSAGE> Message displayed when locked out
// <i> Default: "Too many attempts; instance is locked"
#define SL_CLI_SIMPLE_PASSWORD_LOCK_OUT_MESSAGE                   "Too many attempts; instance is locked"

// <s SL_CLI_SIMPLE_PASSWORD_SECURITY_WARNING_FLAG> Message displayed when maximum of security flag has been triggered
// <i> Default: "Maximum retry has been reached"
// <i> Message will be displayed until security flag is cleared
#define SL_CLI_SIMPLE_PASSWORD_SECURITY_WARNING_FLAG              "Maximum retry has been reached"

// <s SL_CLI_SIMPLE_PASSWORD_LOGGED_IN> Message displayed when successfully logged in
// <i> Default: "Successfully logged in"
#define SL_CLI_SIMPLE_PASSWORD_LOGGED_IN                          "Successfully logged in"

// <s SL_CLI_SIMPLE_PASSWORD_ENTRY_TOO_LONG> Message displayed when provided password is too long
// <i> Default: "Provided Password too large; buffer will overflow"
#define SL_CLI_SIMPLE_PASSWORD_ENTRY_TOO_LONG                     "Provided Password too large; buffer will overflow"

#endif // SL_CLI_CONFIG_INSTANCE_H

// <<< end of configuration section >>>
