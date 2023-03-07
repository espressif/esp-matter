/***************************************************************************//**
 * @file
 * @brief  Ember serial functionality specific to a PC with Unix library
 *    support.
 *
 * See @ref serial_comm for documentation.
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

/** @addtogroup serial_comm
 *@{
 */

// The normal CLI is accessible via port 0 while port 1 is usable for
// raw input.  This is often used by applications to receive a 260
// image for bootloading.
#define SERIAL_PORT_RAW 0
#define SERIAL_PORT_CLI 1

void emberSerialSetPrompt(const char* thePrompt);
void emberSerialCleanup(void);
int emberSerialGetInputFd(uint8_t port);
void emberSerialSendReadyToRead(uint8_t port);

// For users of app/util/serial/command-interpreter.h
#ifndef UC_BUILD
void emberSerialCommandCompletionInit(EmberCommandEntry* listOfCommands);

#if defined(GATEWAY_APP) && !defined(EMBER_AF_PLUGIN_GATEWAY)
// For users of app/util/serial/cli.h
void emberSerialCommandCompletionInitCli(cliSerialCmdEntry* cliCmdList,
                                         int cliCmdListLength);
#endif

#endif // UC_BUILD
/** @} END addtogroup */
