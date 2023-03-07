/***************************************************************************//**
 * @file
 * @brief Command handlers for Manufacturing Library commands.
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

//------------------------------------------------------------------------------
// Ezsp Command Handlers

EmberStatus emberAfEzspMfglibStartCommandCallback(bool rxCallback);

EmberStatus emberAfEzspMfglibSendPacketCommandCallback(uint8_t packetLength,
                                                       uint8_t packetContents[]);
#ifndef UC_BUILD
void emberAfPluginEzspMfglibGetValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
void emberAfPluginEzspMfglibSetValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
#endif  // UC_BUILD
