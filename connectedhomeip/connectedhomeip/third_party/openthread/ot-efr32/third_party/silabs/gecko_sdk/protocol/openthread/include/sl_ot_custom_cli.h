/***************************************************************************//**
 * @file
 * @brief Provides definitions required to support CLI in both SoC and RCP
 * builds
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <openthread/cli.h>
#include <openthread/coprocessor_rpc.h>

extern otCliCommand sl_ot_custom_commands[];
extern const size_t sl_ot_custom_commands_count;

/**
 * Print all commands in @p commands
 *
 * @param[in]  commands         list of commands
 * @param[in]  commandCount     number of commands in @p commands
 *
 */
void printCommands(otCliCommand commands[], size_t commandCount);
