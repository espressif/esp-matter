/***************************************************************************//**
 * @file
 * @brief CLI for the GBCS Message Controller plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "gbz-message-controller.h"

// Functions

// plugin gbz-message-controller clear
void emAfPluginGbzMessageControllerCliClear(void)
{
}

// plugin gbz-message-controller append
// This function will "look" into the ZCL buffer and copy over the current
// filled command into the GBZ command payload buffer.
void emAfPluginGbzMessageControllerCliAppend(void)
{
}

// plugin gbz-message-controller encryption <encryptionEnabled:1>
void emAfPluginGbzMessageControllerCliEncryption(void)
{
}
