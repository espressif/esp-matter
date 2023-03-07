/***************************************************************************//**
 * @file
 * @brief Command Line Interface configuration file
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef CLI_CONFIG_H
#define CLI_CONFIG_H

#include <stdio.h>

#define CLI_CONFIG_DEFAULT_VSPRINTF_FUNCTION    vsprintf

#define CLI_CONFIG_CMDLINE_MAX_STRING_LENGTH    80
#define CLI_CONFIG_CMDLINE_MAX_ARGUMENT_COUNT   16
#define CLI_CONFIG_CMDLINE_HISTORY_COUNT        5
#define CLI_CONFIG_CMDLINE_DEFAULT_TIMEOUT      1
#define CLI_CONFIG_MAX_INLINE_HELP_CMDS         3

#define CLI_CONFIG_PRINTF_BUFFER_SIZE           256

#define CLI_CONFIG_MAX_STRING_LENGTH            256

#define CLI_CONFIG_CMD_MAX_STRING_LENGTH        16

#define CLI_CONFIG_LOGIN_ATTEMPT_COUNT          3
#define CLI_CONFIG_LOGOUT_FUNC_MAX_ARGS         3

#define CLI_CONFIG_USER_LEVEL_MAX               10
#define CLI_CONFIG_USER_MAX_COUNT               1
#define CLI_CONFIG_USER_NAME_MAX_LENGTH         8
#define CLI_CONFIG_USER_PASS_MAX_LENGTH         8

#define CLI_CONFIG_PROMPT_STRING_LENGTH         8

#define CLI_CONFIG_MAX_CMDS                     128

#define CLI_CONFIG_CMDLINE_CHAR_COMPUTER_PREFIX ':'

#endif // CLI_CONFIG_H
