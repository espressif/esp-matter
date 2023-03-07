/***************************************************************************//**
 * @brief CMSIS RTOS IPC code.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include PLATFORM_HEADER
#include "cmsis-rtos-ipc-config.h"

#include "stack/include/ember.h"

#include "cmsis-rtos-support.h"

// TODO: This is for IAR, for GCC it should be "unsigned long"
typedef unsigned int PointerType;

osEventFlagsId_t emAfPluginCmsisRtosFlags;

osMutexId_t commandMutex;
osMessageQueueId_t callbackQueue;
static uint8_t apiCommandData[MAX_STACK_API_COMMAND_SIZE];
static EmberBuffer callbackBuffer[EMBER_AF_PLUGIN_CMSIS_RTOS_MAX_CALLBACK_QUEUE_SIZE];

//------------------------------------------------------------------------------
// Forward declarations

static uint16_t formatBinaryManagementCommand(uint8_t *buffer,
                                              uint16_t bufferSize,
                                              uint16_t identifier,
                                              const char *format,
                                              va_list argumentList);

static uint32_t pendCommandPendingFlag(void);
static void postCommandPendingFlag(void);
static void pendResponsePendingFlag(void);
static void postResponsePendingFlag(void);
static void postCallbackPendingFlag(void);

static void fetchParams(uint8_t *readPointer, PGM_P format, va_list args);

//------------------------------------------------------------------------------
// Internal APIs

void emAfPluginCmsisRtosIpcInit(void)
{
  uint8_t i;

  for (i = 0; i < EMBER_AF_PLUGIN_CMSIS_RTOS_MAX_CALLBACK_QUEUE_SIZE; i++) {
    callbackBuffer[i] = EMBER_NULL_BUFFER;
  }

  const osEventFlagsAttr_t rtosFlagsAttr = {
    "RTOS Flags",
    0,
    NULL,
    0
  };

  emAfPluginCmsisRtosFlags = osEventFlagsNew(&rtosFlagsAttr);
  assert(emAfPluginCmsisRtosFlags != NULL);

  commandMutex = osMutexNew(NULL);
  assert(commandMutex != NULL);

  callbackQueue = osMessageQueueNew(EMBER_AF_PLUGIN_CMSIS_RTOS_MAX_CALLBACK_QUEUE_SIZE,
                                    (sizeof(EmberBufferDesc) + 3) & (~3) /* align to 4 bytes */,
                                    NULL);
  assert(callbackQueue != NULL);
}

void emAfPluginCmsisRtosSendBlockingCommand(uint16_t identifier,
                                            const char *format,
                                            ...)
{
  va_list argumentList;

  // This API can not be called from the stack task (the stack task calls
  // stack APIs directly).
  assert(!emAfPluginCmsisRtosIsCurrentTaskStackTask());

  // Write the command in the command API buffer.
  va_start(argumentList, format);
  formatBinaryManagementCommand(apiCommandData,
                                sizeof(apiCommandData),
                                identifier,
                                format,
                                argumentList);
  va_end(argumentList);

  // Post the "command pending" flag, wake up the stack and pend for the
  // "response" pending flag.
  postCommandPendingFlag();
  emAfPluginCmsisRtosWakeUpConnectStackTask();
  pendResponsePendingFlag();
}

void emAfPluginCmsisRtosSendResponse(uint16_t identifier,
                                     const char *format,
                                     ...)
{
  va_list argumentList;

  // This API must be called from the stack task.
  assert(emAfPluginCmsisRtosIsCurrentTaskStackTask());

  // Write the command in the command API buffer.
  va_start(argumentList, format);
  formatBinaryManagementCommand(apiCommandData,
                                sizeof(apiCommandData),
                                identifier,
                                format,
                                argumentList);
  va_end(argumentList);

  postResponsePendingFlag();
}

void emAfPluginCmsisRtosProcessIncomingApiCommand(void)
{
  uint32_t status = pendCommandPendingFlag();

  // This API must be called from the stack task.
  assert(emAfPluginCmsisRtosIsCurrentTaskStackTask());

  /* Need to check that the pend did not result in an error
   * and that the expected flag is present.
   */
  if (((status & CMSIS_RTOS_ERROR_MASK) == 0) && (status & FLAG_IPC_COMMAND_PENDING)) {
    uint16_t commandId =
      emberFetchHighLowInt16u(apiCommandData);

    emAfPluginCmsisRtosHandleIncomingApiCommand(commandId);
  }
}

void emAfPluginCmsisRtosSendCallbackCommand(uint16_t identifier,
                                            const char *format,
                                            ...)
{
  va_list argumentList;
  const char *formatFinger;
  uint8_t commandLength = 2; // command ID
  uint8_t i;
  EmberBufferDesc callbackBufferDescriptorPut = { NULL, 0 };

  // This API must be called from the stack task.
  assert(emAfPluginCmsisRtosIsCurrentTaskStackTask());

  // Compute the amount of memory we need to allocate.
  va_start(argumentList, format);
  for (formatFinger = (char *) format; *formatFinger != 0; formatFinger++) {
    switch (*formatFinger) {
      case 'u':
        va_arg(argumentList, unsigned int);
        commandLength++;
        break;
      case 's':
        va_arg(argumentList, int);
        commandLength++;
        break;
      case 'v':
        va_arg(argumentList, unsigned int);
        commandLength += sizeof(uint16_t);
        break;
      case 'w':
        va_arg(argumentList, uint32_t);
        commandLength += sizeof(uint32_t);
        break;
      case 'b': {
        va_arg(argumentList, const uint8_t*);
        commandLength += va_arg(argumentList, int) + 1;
        break;
      }
      default:
        assert(false);
        break;
    }
  }
  va_end(argumentList);

  // Enqueing the callback in the app framework task queue.
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
  for (i = 0; i < EMBER_AF_PLUGIN_CMSIS_RTOS_MAX_CALLBACK_QUEUE_SIZE; i++) {
    if (callbackBuffer[i] == EMBER_NULL_BUFFER) {
      callbackBuffer[i] = emberAllocateBuffer(commandLength);
      break;
    }
  }
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();

  // Queue is full or no memory available: either way just return.
  if (i >= EMBER_AF_PLUGIN_CMSIS_RTOS_MAX_CALLBACK_QUEUE_SIZE
      || callbackBuffer[i] == EMBER_NULL_BUFFER) {
    return;
  }

  // Write the callback command to the callback buffer.
  va_start(argumentList, format);
  formatBinaryManagementCommand(emberGetBufferPointer(callbackBuffer[i]),
                                commandLength,
                                identifier,
                                format,
                                argumentList);
  va_end(argumentList);

  callbackBufferDescriptorPut.buffer_addr = &callbackBuffer[i];
  callbackBufferDescriptorPut.buffer_length = commandLength;

  osMessageQueuePut(callbackQueue,
                    (void *)&callbackBufferDescriptorPut,
                    0, //TODO: priority ???
                    0);

  // Wake up the AF task.
  postCallbackPendingFlag();

  // TODO: once we implement stack callback dispatching to custom application
  // tasks, we should add the copy of the callback to the queue of every custom
  // application task that chose to listen to this specific callback.
}

bool emAfPluginCmsisRtosProcessIncomingCallbackCommand(void)
{
  uint16_t messageLength;
  EmberBuffer *callbackBufferPtr = NULL;
  EmberBufferDesc callbackBufferDescriptorGet = { NULL, 0 };
  uint8_t msgPrio;

  // This API can not be called from the stack task (the stack task calls
  // stack APIs directly).
  assert(!emAfPluginCmsisRtosIsCurrentTaskStackTask());

  osMessageQueueGet(callbackQueue,
                    (void *)&callbackBufferDescriptorGet,
                    &msgPrio,
                    0);

  callbackBufferPtr = callbackBufferDescriptorGet.buffer_addr;
  messageLength = callbackBufferDescriptorGet.buffer_length;

  if (callbackBufferPtr) {
    uint8_t callbackData[MAX_STACK_CALLBACK_COMMAND_SIZE];
    uint16_t commandId;

    // Lock the buffer system to prevent defragmentation while we access the
    // buffer.
    emberAfPluginCmsisRtosAcquireBufferSystemMutex();
    MEMCOPY(callbackData,
            emberGetBufferPointer(*callbackBufferPtr),
            messageLength);
    // This assignment is safe because the vNCP acquires the buffer system mutex
    // before manipulating any entry in callbackBuffer[].
    *callbackBufferPtr = EMBER_NULL_BUFFER;
    emberAfPluginCmsisRtosReleaseBufferSystemMutex();

    commandId = emberFetchHighLowInt16u(callbackData);

    emAfPluginCmsisRtosHandleIncomingCallbackCommand(commandId,
                                                     callbackData + 2);

    return true;
  }

  return false;
}

bool emAfPluginCmsisRtosIsCurrentTaskStackTask(void)
{
  return (osThreadGetId() == emAfPluginCmsisRtosGetStackTcb());
}

void emAfPluginCmsisRtosAcquireCommandMutex(void)
{
  assert(!emAfPluginCmsisRtosIsCurrentTaskStackTask());

  assert(osMutexAcquire(commandMutex, osWaitForever) == osOK);
}

void emAfPluginCmsisRtosReleaseCommandMutex(void)
{
  assert(!emAfPluginCmsisRtosIsCurrentTaskStackTask());

  assert(osMutexRelease(commandMutex) == osOK);
}

void emAfPluginCmsisRtosFetchApiParams(PGM_P format, ...)
{
  va_list argumentList;

  va_start(argumentList, format);
  // Start fetching right after the command ID
  fetchParams(apiCommandData + 2, format, argumentList);
  va_end(argumentList);
}

void emAfPluginCmsisRtosFetchCallbackParams(uint8_t *callbackParams,
                                            PGM_P format,
                                            ...)
{
  va_list argumentList;
  va_start(argumentList, format);
  fetchParams(callbackParams, format, argumentList);
  va_end(argumentList);
}

//------------------------------------------------------------------------------
// Internal callbacks

void emAfPluginCmsisRtosMarkBuffersCallback(void)
{
  uint8_t i;

  for (i = 0; i < EMBER_AF_PLUGIN_CMSIS_RTOS_MAX_CALLBACK_QUEUE_SIZE; i++) {
    emberMarkBuffer(&callbackBuffer[i]);
  }
}

//------------------------------------------------------------------------------
// Static functions

// TODO: the two functions below can be consolidated

static void fetchParams(uint8_t *readPointer, PGM_P format, va_list args)
{
  char *c = (char *)format;
  void* ptr;

  while (*c) {
    ptr = va_arg(args, void*);

    switch (*c) {
      case 's': {
        uint8_t *realPointer = (uint8_t *)ptr;
        if (ptr) {
          *realPointer = (uint8_t)*readPointer;
        }
        readPointer++;
      }
      break;
      case 'u': {
        uint8_t *realPointer = (uint8_t *)ptr;
        if (ptr) {
          *realPointer = (uint8_t)*readPointer;
        }
        readPointer++;
      }
      break;
      case 'v': {
        uint16_t *realPointer = (uint16_t *)ptr;
        *realPointer = emberFetchHighLowInt16u(readPointer);
        readPointer += 2;
      }
      break;
      case 'w': {
        uint32_t *realPointer = (uint32_t *)ptr;
        if (ptr) {
          *realPointer = emberFetchHighLowInt32u(readPointer);
        }
        readPointer += 4;
      }
      break;
      case 'b': {
        uint8_t *realArray = (uint8_t *)ptr;
        uint8_t *lengthPointer = (uint8_t *)va_arg(args, void*);
        *lengthPointer = *readPointer;
        readPointer++;

        if (ptr) {
          MEMMOVE(realArray, readPointer, *lengthPointer);
        }
        readPointer += *lengthPointer;
      }
      break;
      case 'p': {
        uint8_t **realPointer = (uint8_t **)ptr;
        uint8_t *lengthPointer = (uint8_t *)va_arg(args, void*);
        *lengthPointer = *readPointer;
        readPointer++;

        if (ptr) {
          *realPointer = readPointer;
        }
        readPointer += *lengthPointer;
      }
      break;
      default:
        ; // meh
    }
    c++;
  }
}

static uint16_t formatBinaryManagementCommand(uint8_t *buffer,
                                              uint16_t bufferSize,
                                              uint16_t identifier,
                                              const char *format,
                                              va_list argumentList)
{
  uint8_t *finger = buffer;
  const char *formatFinger;

  MEMSET(buffer, 0, bufferSize);

  // store the identifier
  emberStoreHighLowInt16u(finger, identifier);
  finger += sizeof(uint16_t);

  // We use primitive types to retrieve va_args because gcc on some
  // platforms (eg., unix hosts) complains about automatic variable
  // promotion in variadic functions.
  // [eg: warning: 'uint8_t' is promoted to 'int' when passed through '...']
  //
  // An exception is made for the uint32_t case:  Here we need to check the size
  // of unsigned int, which can be smaller on platforms such as the c8051.

  for (formatFinger = (char *) format; *formatFinger != 0; formatFinger++) {
    switch (*formatFinger) {
      case 'u':
        *finger++ = va_arg(argumentList, unsigned int);
        break;
      case 's':
        *finger++ = va_arg(argumentList, int);
        break;
      case 'v':
        emberStoreHighLowInt16u(finger, va_arg(argumentList, unsigned int));
        finger += sizeof(uint16_t);
        break;
      case 'w':
        if (sizeof(unsigned int) < sizeof(uint32_t)) {
          emberStoreHighLowInt32u(finger, va_arg(argumentList, uint32_t));
        } else {
          emberStoreHighLowInt32u(finger, va_arg(argumentList, unsigned int));
        }
        finger += sizeof(uint32_t);
        break;
      case 'b': {
        const uint8_t *data = va_arg(argumentList, const uint8_t*);
        // store the size, must fit into a byte
        uint8_t dataSize = va_arg(argumentList, int);
        *finger++ = dataSize;

        if (dataSize > 0) {
          // Checking for NULL here save's every caller from checking.  We assume
          // the if dataSize is not zero then we should send all zeroes.
          if (data != NULL) {
            MEMCOPY(finger, data, dataSize);
          } else {
            MEMSET(finger, 0, dataSize);
          }
        }

        finger += dataSize;
        break;
      }
      default:
        // confused!
        assert(false);
        break;
    }
  }

  uint16_t length = finger - buffer;
  // sanity check
  assert(length <= bufferSize);

  return length;
}

static uint32_t pendCommandPendingFlag(void)
{
  return osEventFlagsWait(emAfPluginCmsisRtosFlags,
                          FLAG_IPC_COMMAND_PENDING,
                          osFlagsWaitAny,
                          0);
}

static void postCommandPendingFlag(void)
{
  assert((osEventFlagsSet(emAfPluginCmsisRtosFlags,
                          FLAG_IPC_COMMAND_PENDING) & CMSIS_RTOS_ERROR_MASK) == 0);
}

static void pendResponsePendingFlag(void)
{
  assert((osEventFlagsWait(emAfPluginCmsisRtosFlags,
                           FLAG_IPC_RESPONSE_PENDING,
                           osFlagsWaitAny,
                           osWaitForever) & CMSIS_RTOS_ERROR_MASK) == 0);
}

static void postResponsePendingFlag(void)
{
  assert((osEventFlagsSet(emAfPluginCmsisRtosFlags,
                          FLAG_IPC_RESPONSE_PENDING) & CMSIS_RTOS_ERROR_MASK) == 0);
}

static void postCallbackPendingFlag(void)
{
  assert((osEventFlagsSet(emAfPluginCmsisRtosFlags,
                          FLAG_STACK_CALLBACK_PENDING) & CMSIS_RTOS_ERROR_MASK) == 0);
}
