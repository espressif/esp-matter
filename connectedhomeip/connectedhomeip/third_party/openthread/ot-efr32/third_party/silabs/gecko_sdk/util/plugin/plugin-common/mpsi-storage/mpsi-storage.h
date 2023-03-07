/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef MPSI_STORAGE_H
#define MPSI_STORAGE_H

// ------------------------------------------------------------------------------
// Includes

#include PLATFORM_HEADER
#ifdef EMBER_AF_API_AF_HEADER
 #include EMBER_AF_API_AF_HEADER
#endif

#include "mpsi-message-ids.h"

// ------------------------------------------------------------------------------
// Defines

// Return values
#define   MPSI_STORAGE_SUCCESS                0
#define   MPSI_STORAGE_ERROR                  1
#define   MPSI_STORAGE_PLUGIN_UNINITIALIZED   2
#define   MPSI_STORAGE_INVALID_PARAMETER      3
#define   MPSI_STORAGE_PAYLOAD_TOO_LONG       4

// We use three bytes of storage payload to indicate destination app (2 bytes)
// and message ID (2 bytes)
#define   MPSI_STORAGE_PAYLOAD_HEADER_LEN     0x04
#define   MPSI_STORAGE_PAYLOAD_LEN            0xFF

#if defined(EMBER_STACK_ZIGBEE)
 #define mpsiStoragePrintln(...)                  emberAfAppPrintln(__VA_ARGS__)
 #define mpsiStorageInit()                        store_init()
 #define mpsiStorageDelete(tag)                   store_delete(tag, 0xFFFF)
 #define mpsiStorageRead(tag, flags, len, value)  store_read(tag,   \
                                                             flags, \
                                                             len,   \
                                                             value)
 #define mpsiStorageWrite(tag, flags, len, value) store_write(tag,   \
                                                              flags, \
                                                              len,   \
                                                              value)

#elif defined(EMBER_STACK_BLE)
 #define mpsiStoragePrintln(...)                  do { printf(__VA_ARGS__); \
                                                       printf("\n"); } while (0)
 #define mpsiStorageInit()                        (void)0
errorcode_t mpsiStorageDelete(uint16_t tag);
errorcode_t mpsiStorageRead(uint16_t tag,
                            uint8_t  *flags,
                            uint8_t  *len,
                            void     **value);
errorcode_t mpsiStorageWrite(uint16_t  tag,
                             uint8_t    flags,
                             uint8_t    len,
                             const void *val);

#else
 #error "MPSI Storage: Stack is not supported!"
#endif

//------------------------------------------------------------------------------
// Public APIs

//------------------------------------------------------------------------------
// Internal APIs
void    emberAfPluginMpsiStorageInitCallback(void);
uint8_t emAfPluginMpsiStorageStoreMessage(MpsiMessage_t* mpsiMessage);

//------------------------------------------------------------------------------
// Private APIs
bool    verifyAllMpsiStorageMessagesSupported();
void    deleteAllMpsiMessagesInPsStore();

#endif  // MPSI_STORAGE_H
