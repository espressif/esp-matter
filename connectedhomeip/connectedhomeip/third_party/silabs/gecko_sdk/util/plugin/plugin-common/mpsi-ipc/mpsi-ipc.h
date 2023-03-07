/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef __MPSI_IPC_H__
#define __MPSI_IPC_H__

// ------------------------------------------------------------------------------
// Includes

#ifdef EMBER_AF_API_AF_HEADER
 #include EMBER_AF_API_AF_HEADER
#endif

#include "mpsi-message-ids.h"

// ------------------------------------------------------------------------------
// Defines

#if defined(EMBER_STACK_ZIGBEE)
 #define mpsiIpcPrint(...)   emberAfAppPrint(__VA_ARGS__)
 #define mpsiIpcPrintln(...) emberAfAppPrintln(__VA_ARGS__)
#elif defined(EMBER_STACK_BLE)
 #define mpsiIpcPrint(...)   do { printf(__VA_ARGS__); } while (0)
 #define mpsiIpcPrintln(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)
#else
 #error "Stack not defined"
#endif

// Return values
#define   MPSI_IPC_SUCCESS              0
#define   MPSI_IPC_ERROR                1
#define   MPSI_IPC_INVALID_PARAMETER    2
#define   MPSI_IPC_NO_RESOURCES         3

// ------------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * Send a MPSI message over the IPC mechanism.
 ******************************************************************************/
uint8_t emAfPluginMpsiIpcSendMessage(MpsiMessage_t* mpsiMessage);

#endif // __MPSI_IPC_H__
