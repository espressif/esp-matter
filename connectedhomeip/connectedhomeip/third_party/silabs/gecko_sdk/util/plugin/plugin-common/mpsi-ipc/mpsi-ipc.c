/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include "mpsi.h"
#include "mpsi-ipc.h"
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define MPSI_IPC_TEMP_FILE            "/tmp/mpsi"
#define MPSI_KEY                      1
#define MPSI_IPC_QUEUE_PAYLOAD_SIZE   512

int messageQId = -1;

typedef struct {
  long    mtype;
  uint8_t mtext[MPSI_IPC_QUEUE_PAYLOAD_SIZE];
} MessageQBuffer_t;

void emberAfPluginMpsiIpcInitCallback(void)
{
  FILE *fptr;

  // Create a temporary file called /tmp/mpsi
  fptr = fopen(MPSI_IPC_TEMP_FILE, "w");
  if (fptr == NULL) {
    mpsiIpcPrintln("MPSI IPC: failed to create %s", MPSI_IPC_TEMP_FILE);
    return;
  }
  fclose(fptr);

  // Create the message queue; do nothing if it exists
  // Give the user read/write/execute permission (S_IRWXU)
  messageQId =
    msgget(ftok(MPSI_IPC_TEMP_FILE, MPSI_KEY), (IPC_CREAT | S_IRWXU));
  if (-1 == messageQId) {
    mpsiIpcPrintln("MPSI IPC error: failed to create or get message queue "
                   "(errno %d)", errno);
  }
}

void emberAfPluginMpsiIpcTickCallback(void)
{
  MessageQBuffer_t messageBuffer;
  ssize_t bytesReceived;
  MpsiMessage_t mpsiMessage;
  uint8_t bytesDeserialized;

  // If the message queue has been configured, check if there are messages on
  // it for us
  if (-1 != messageQId) {
    bytesReceived = msgrcv(messageQId,
                           &messageBuffer,
                           MPSI_IPC_QUEUE_PAYLOAD_SIZE,
                           MPSI_APP_ID,
                           IPC_NOWAIT | MSG_NOERROR);

    if (-1 != bytesReceived) {
      bytesDeserialized = emberAfPluginMpsiDeserialize(messageBuffer.mtext,
                                                       &mpsiMessage);

      if (bytesDeserialized != bytesReceived) {
        mpsiIpcPrintln("MPSI IPC warning: read %d bytes on message queue but "
                       "deserialized %d bytes (message ID 0x%2x)",
                       bytesReceived, bytesDeserialized, mpsiMessage.messageId);
      }

      emberAfPluginMpsiReceiveMessage(&mpsiMessage);
    }

#if (MPSI_APP_ID_BLE == MPSI_APP_ID)
    // If we are the BLE app, we need to pick off messages destined to the
    // Mobile App as well so that we can forward them
    bytesReceived = msgrcv(messageQId,
                           &messageBuffer,
                           MPSI_IPC_QUEUE_PAYLOAD_SIZE,
                           MPSI_APP_ID_MOBILE_APP,
                           IPC_NOWAIT | MSG_NOERROR);

    if (-1 != bytesReceived) {
      bytesDeserialized = emberAfPluginMpsiDeserialize(messageBuffer.mtext,
                                                       &mpsiMessage);

      if (bytesDeserialized != bytesReceived) {
        mpsiIpcPrintln("MPSI IPC warning: read %d bytes on message queue but "
                       "deserialized %d bytes (message ID 0x%2x) target(MA)",
                       bytesReceived, bytesDeserialized, mpsiMessage.messageId);
      }

      emberAfPluginMpsiReceiveMessage(&mpsiMessage);
    }
#endif // (MPSI_APP_ID_BLE == MPSI_APP_ID)
  }
}

uint8_t emAfPluginMpsiIpcSendMessage(MpsiMessage_t* mpsiMessage)
{
  MessageQBuffer_t messageBuffer;
  int rv;
  uint8_t bytesSerialized;

  if (!mpsiMessage) {
    return MPSI_IPC_INVALID_PARAMETER;
  }

  if (-1 == messageQId) {
    return MPSI_IPC_NO_RESOURCES;
  }

  messageBuffer.mtype = (long)mpsiMessage->destinationAppId;

  bytesSerialized =
    emberAfPluginMpsiSerialize(mpsiMessage, messageBuffer.mtext);
  if (0 == bytesSerialized) {
    mpsiIpcPrintln("MPSI IPC error: serialize failed for message ID 0x%2x",
                   mpsiMessage->messageId);
    return MPSI_IPC_ERROR;
  }

  rv = msgsnd(messageQId,
              &messageBuffer,
              bytesSerialized,
              IPC_NOWAIT);

  return (0 == rv) ? MPSI_IPC_SUCCESS : MPSI_IPC_ERROR;
}
