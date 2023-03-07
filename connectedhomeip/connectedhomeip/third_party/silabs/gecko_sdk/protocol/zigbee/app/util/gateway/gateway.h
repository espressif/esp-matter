/***************************************************************************//**
 * @file
 * @brief Functions for Gateway specific behavior for a host application.
 * In this case we assume our application is running on
 * a PC with Unix library support, connected to a 260 via serial uart.
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

// This function's signature will vary depending on the platform.
// Platforms that support a command-line (i.e. PCs) will have the
// standard "int argc, char* argv[]" while those without (i.e. the
// embedded ones) will use "void".  This allows easy compatibility
// cross-platform.
#ifndef MAIN_FUNCTION_PARAMETERS
#define MAIN_FUNCTION_PARAMETERS int argc, char* argv[]
#endif // MAIN_FUNCTION_PARAMETERS
EmberStatus gatewayInit(MAIN_FUNCTION_PARAMETERS);
EmberStatus gatewayBackchannelStart(void);
void        gatewayBackchannelStop(void);

#if defined COMMAND_INTERPRETER_SUPPORT
EmberStatus gatewayCommandInterpreterInit(const char * cliPrompt,
                                          EmberCommandEntry commands[]);
#else
EmberStatus gatewayCliInit(const char * cliPrompt,
                           cliSerialCmdEntry cliCmdList[],
                           int cliCmdListLength);
#endif
void gatewayWaitForEvents(void);
void gatewayWaitForEventsWithTimeout(uint32_t timeoutMs);

// The difference in seconds between the ZigBee Epoch: January 1st, 2000
// and the Unix Epoch: January 1st 1970.
#define UNIX_ZIGBEE_EPOCH_DELTA (uint32_t) 946684800UL
