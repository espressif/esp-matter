/***************************************************************************//**
 * @file
 * @brief Internal APIs for the Command Relay plugin.
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

#ifndef SILABS_COMMAND_RELAY_LOCAL_H
#define SILABS_COMMAND_RELAY_LOCAL_H

void emAfPluginCommandRelayAddBind(EmberEUI64 inEui,
                                   uint8_t inEndpoint,
                                   EmberEUI64 outEui,
                                   uint8_t outEndpoint);

void emAfPluginCommandRelayPrint(void);
void emAfPluginCommandRelayRemove(EmberAfPluginCommandRelayDeviceEndpoint* inDeviceEndpoint,
                                  EmberAfPluginCommandRelayDeviceEndpoint* outDeviceEndpoint);
void emAfPluginCommandRelayRemoveDeviceByEui64(EmberEUI64 eui64);

#endif //__COMMAND_RELAY_LOCAL_H
