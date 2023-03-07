/***************************************************************************//**
 * @file
 * @brief CLI commands for sending various messages.
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

#ifndef SILABS_ZCL_CLI_H
#define SILABS_ZCL_CLI_H

#ifndef UC_BUILD // In UC we only support generated CLI, no legacy
#if !defined(EMBER_AF_GENERATE_CLI)
void emAfCliSendCommand(void);
void emAfCliSendUsingMulticastBindingCommand(void);
void emAfCliBsendCommand(void);
void emAfCliReadCommand(void);
void emAfCliWriteCommand(void);
void emAfCliTimesyncCommand(void);
void emAfCliRawCommand(void);
void emAfCliAddReportEntryCommand(void);
#endif
extern EmberCommandEntry keysCommands[];
extern EmberCommandEntry interpanCommands[];
extern EmberCommandEntry printCommands[];
extern EmberCommandEntry zclCommands[];
extern EmberCommandEntry certificationCommands[];
#else
#define SL_ZCL_CLI_ARG_UINT8                          0x01u
#define SL_ZCL_CLI_ARG_UINT16                         0x02u
#define SL_ZCL_CLI_ARG_UINT24                         0x03u
#define SL_ZCL_CLI_ARG_UINT32                         0x04u
#define SL_ZCL_CLI_ARG_UINT40                         0x05u
#define SL_ZCL_CLI_ARG_UINT48                         0x06u
#define SL_ZCL_CLI_ARG_UINT56                         0x07u
#define SL_ZCL_CLI_ARG_UINT64                         0x08u
#define SL_ZCL_CLI_ARG_INT8                           0x11u
#define SL_ZCL_CLI_ARG_INT16                          0x12u
#define SL_ZCL_CLI_ARG_INT24                          0x13u
#define SL_ZCL_CLI_ARG_INT32                          0x14u
#define SL_ZCL_CLI_ARG_INT40                          0x15u
#define SL_ZCL_CLI_ARG_INT48                          0x16u
#define SL_ZCL_CLI_ARG_INT56                          0x17u
#define SL_ZCL_CLI_ARG_INT64                          0x18u
#define SL_ZCL_CLI_ARG_UINT8OPT                       0x21u
#define SL_ZCL_CLI_ARG_UINT16OPT                      0x22u
#define SL_ZCL_CLI_ARG_UINT24OPT                      0x23u
#define SL_ZCL_CLI_ARG_UINT32OPT                      0x24u
#define SL_ZCL_CLI_ARG_UINT40OPT                      0x25u
#define SL_ZCL_CLI_ARG_UINT48OPT                      0x26u
#define SL_ZCL_CLI_ARG_UINT56OPT                      0x27u
#define SL_ZCL_CLI_ARG_UINT64OPT                      0x28u
#define SL_ZCL_CLI_ARG_INT8OPT                        0x31u
#define SL_ZCL_CLI_ARG_INT16OPT                       0x32u
#define SL_ZCL_CLI_ARG_INT24OPT                       0x33u
#define SL_ZCL_CLI_ARG_INT32OPT                       0x34u
#define SL_ZCL_CLI_ARG_INT40OPT                       0x35u
#define SL_ZCL_CLI_ARG_INT48OPT                       0x36u
#define SL_ZCL_CLI_ARG_INT56OPT                       0x37u
#define SL_ZCL_CLI_ARG_INT64OPT                       0x38u
#define SL_ZCL_CLI_ARG_STRING                         0x41u
#define SL_ZCL_CLI_ARG_HEX                            0x42u
#define SL_ZCL_CLI_ARG_STRINGOPT                      0x43u
#define SL_ZCL_CLI_ARG_HEXOPT                         0x44u
#endif //UC_BUILD

void zclSimpleCommand(uint8_t frameControl,
                      uint16_t clusterId,
                      uint8_t commandId);

#define zclSimpleClientCommand(clusterId, commandId)                                  \
  zclSimpleCommand(ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER, \
                   (clusterId),                                                       \
                   (commandId))

#define zclSimpleServerCommand(clusterId, commandId)                                  \
  zclSimpleCommand(ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT, \
                   (clusterId),                                                       \
                   (commandId))

#endif // SILABS_ZCL_CLI_H
