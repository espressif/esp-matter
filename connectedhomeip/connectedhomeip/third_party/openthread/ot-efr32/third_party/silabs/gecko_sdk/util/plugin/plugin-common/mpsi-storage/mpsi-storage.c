/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include "mpsi-storage.h"
#include "mpsi.h"
#if !defined(EMBER_STACK_BLE)
#include "psstore/store.h"
#endif

// This mask identifies which messages in PS Store are MPSI messages
#define MPSI_PS_STORE_ID    0x3000
#define MPSI_PS_STORE_MASK  0xF000

static bool   gMpsiStoragePluginInitialized = false;
static int8_t gMpsiMessageNumber = 0;

#if defined(EMBER_STACK_BLE)
errorcode_t mpsiStorageDelete(uint16_t tag)
{
  struct gecko_msg_flash_ps_erase_rsp_t *rsp;
  rsp = gecko_cmd_flash_ps_erase(tag);

  return (errorcode_t)rsp->result;
}

errorcode_t mpsiStorageRead(uint16_t tag,
                            uint8_t  *flags,
                            uint8_t  *len,
                            void     **value)
{
  struct gecko_msg_flash_ps_load_rsp_t *rsp;
  rsp = gecko_cmd_flash_ps_load(tag);

  *len = rsp->value.len;
  *value = rsp->value.data;
  (void)flags;

  return (errorcode_t)rsp->result;
}

errorcode_t mpsiStorageWrite(uint16_t   tag,
                             uint8_t    flags,
                             uint8_t    len,
                             const void *val)
{
  struct gecko_msg_flash_ps_save_rsp_t *rsp;

  rsp = gecko_cmd_flash_ps_save(tag, len, val);
  (void)flags;

  return (errorcode_t)rsp->result;
}
#endif // EMBER_STACK_BLE

void emberAfPluginMpsiStorageInitCallback(void)
{
  uint16_t      tag = MPSI_PS_STORE_ID;
  uint8_t       flags;
  uint8_t       len;
  uint8_t*      value;
  MpsiMessage_t mpsiMessage;
  errorcode_t   errorCode;
  uint16_t      bytesDeserialized;

  // Initialize the storage medium and process messages
  mpsiStorageInit();

  // We only process messages if and only if we support every single MPSI
  // message in the storage medium
  if (true == verifyAllMpsiStorageMessagesSupported()) {
    errorCode = mpsiStorageRead(tag, &flags, &len, (void*)&value);
    while (bg_err_success == errorCode) {
      // Convert the PS Store payload to an MpsiMessage_t struct
      bytesDeserialized = emberAfPluginMpsiDeserialize(value, &mpsiMessage);

      if (bytesDeserialized) {
        // Process the MPSI message
        emberAfPluginMpsiReceiveMessage(&mpsiMessage);
      } else {
        mpsiStoragePrintln("MPSI Storage: failed to deserialize message "
                           "(tag 0x%2x len %d)", tag, len);
      }

      tag++;
      errorCode = mpsiStorageRead(tag, &flags, &len, (void*)&value);
    }
  }

#ifndef MPSI_STORAGE_DO_NOT_WIPE_STORAGE_AFTER_INIT
  // Wipe all messages in the mailbox
  deleteAllMpsiMessagesInPsStore();
#endif

  // We've read and cleared the mailbox; we now mark ourselves ready to accept
  // new messages to write into the storage medium
  gMpsiStoragePluginInitialized = true;
}

uint8_t emAfPluginMpsiStorageStoreMessage(MpsiMessage_t* mpsiMessage)
{
  uint16_t    tag;
  uint8_t     flags, len, payload[MPSI_STORAGE_PAYLOAD_LEN];
  errorcode_t errorCode;
  uint16_t    bytesSerialized;

  // We cannot write to the storage medium if we have yet to read, process, and
  // wipe the contents of it first
  if (!gMpsiStoragePluginInitialized) {
    return MPSI_STORAGE_PLUGIN_UNINITIALIZED;
  }

  if (!mpsiMessage) {
    return MPSI_STORAGE_INVALID_PARAMETER;
  }

  // Though an MPSI message payload and a PS Store entry's payload can fit 255
  // bytes, we use four bytes of the PS Store entry's payload to contain the
  // destination app (1 byte), message ID (2 bytes), and payload length (1 byte)
  // So if the MPSI message's payload length is too large to fit it and the
  // other MPSI message fields into a PS Store entry, throw an error
  if (mpsiMessage->payloadLength
      > (MPSI_STORAGE_PAYLOAD_LEN - MPSI_STORAGE_PAYLOAD_HEADER_LEN)) {
    return MPSI_STORAGE_PAYLOAD_TOO_LONG;
  }

  // Write to PS Store
  tag = (MPSI_PS_STORE_ID | gMpsiMessageNumber);
  flags = 0;
  len = MPSI_STORAGE_PAYLOAD_HEADER_LEN + mpsiMessage->payloadLength;
  bytesSerialized = emberAfPluginMpsiSerialize(mpsiMessage, payload);
  if (len != bytesSerialized) {
    mpsiStoragePrintln("MPSI Storage: failed to serialize MPSI message "
                       "(len %d serializedLen %d tag 0x%2x destAppId 0x%x "
                       "messageId 0x%2x payloadLength %d)",
                       len, bytesSerialized, tag, mpsiMessage->destinationAppId,
                       mpsiMessage->messageId, mpsiMessage->payloadLength);
    return MPSI_STORAGE_ERROR;
  }

  errorCode = mpsiStorageWrite(tag, flags, len, (void *)payload);
  if (bg_err_success == errorCode) {
    gMpsiMessageNumber++;
  } else {
    mpsiStoragePrintln("MPSI Storage: failed to write tag (0x%2X) with len (%d) "
                       "into PS Store (error %d)",
                       tag,
                       len,
                       errorCode);
    return MPSI_STORAGE_ERROR;
  }

  return MPSI_STORAGE_SUCCESS;
}

bool verifyAllMpsiStorageMessagesSupported()
{
  uint16_t      tag = MPSI_PS_STORE_ID;
  uint8_t       flags;
  uint8_t       len;
  uint8_t*      value;
  MpsiMessage_t mpsiMessage;
  errorcode_t   errorCode;
  uint16_t      bytesDeserialized;

  errorCode = mpsiStorageRead(tag, &flags, &len, (void*)&value);

  if (bg_err_success != errorCode) {
    return false;
  }

  while (bg_err_success == errorCode) {
    if (len < MPSI_STORAGE_PAYLOAD_HEADER_LEN) {
      mpsiStoragePrintln("MPSI Storage: corrupt MPSI message with tag 0x%2X and"
                         " length %d", tag, len);
      return false;
    }

    // Convert the PS Store payload to an MpsiMessage_t struct
    bytesDeserialized = emberAfPluginMpsiDeserialize(value, &mpsiMessage);

    if (!bytesDeserialized) {
      mpsiStoragePrintln("MPSI Storage: failed to deserialize message "
                         "(tag 0x%2x len %d)", tag, len);
      return false;
    }

    // If we don't understand the message and it's not meant for the Mobile App,
    // return false
    if ((MPSI_SUCCESS
         != emAfPluginMpsiMessageIdSupportedByLocalStack(mpsiMessage.messageId))
        && (MPSI_APP_ID_MOBILE_APP != mpsiMessage.destinationAppId)) {
      mpsiStoragePrintln("MPSI Storage: found unsupported MPSI message in "
                         "storage destAppId(0x%X) ID(0x%2X)",
                         mpsiMessage.destinationAppId, mpsiMessage.messageId);
#ifdef MPSI_STORAGE_SKIP_UNSUPPORTED_MESSAGES
      mpsiStoragePrintln("Skipping unsupported message");
#else
      return false;
#endif
    }

    tag++;
    errorCode = mpsiStorageRead(tag, &flags, &len, (void*)&value);
  }

  return true;
}

void deleteAllMpsiMessagesInPsStore()
{
  uint16_t      tag = MPSI_PS_STORE_ID;
  uint8_t       flags;
  uint8_t       len;
  uint8_t*      value;
  errorcode_t   errorCode = mpsiStorageRead(tag, &flags, &len, (void*)&value);

  while (bg_err_success == errorCode) {
    errorCode = mpsiStorageDelete(tag);

    if (bg_err_success != errorCode) {
      mpsiStoragePrintln("MPSI Storage: failed to delete tag (0x%2X) in PS "
                         "Store (error %d)", tag, errorCode);
      // Keep going. This shouldn't ever happen and when we overwrite this tag
      // later, if we do, it'll delete this existing tag anyways
    } else {
      mpsiStoragePrintln("Debug MPSI Storage: deleted tag 0x%2x", tag);
    }

    tag++;
    errorCode = mpsiStorageRead(tag, &flags, &len, (void*)&value);
  }
}
