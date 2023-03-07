/***************************************************************************//**
 * @file
 * @brief LZMA decompression functionality for Gecko Bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "btl_decompress_lzma.h"

#include "api/btl_errorcode.h"
#include "debug/btl_debug.h"

#include <string.h>

// --------------------------------
// Configuration

// LZMA probability model counter arrays
// From lzma-specification.txt:
//
// The size of the probability model counter arrays is calculated with the
// following formula:
//
//   size_of_prob_arrays = 1846 + 768 * (1 << (lp + lc))
//
// Each probability model counter is 11-bit unsigned integer.
// If we use 16-bit integer variables (2-byte integers) for these probability
// model counters, the RAM usage required by probability model counter arrays
// can be estimated with the following formula:
//
//   RAM = 4 KiB + 1.5 KiB * (1 << (lp + lc))
//
// For example, for default LZMA parameters (lp = 0 and lc = 3), the RAM usage is
//
//   RAM_lc3_lp0 = 4 KiB + 1.5 KiB * 8 = 16 KiB
//
#define DECOMPRESSOR_HEAP_SIZE      ((LZMA_COUNTER_SIZE_KB) * 1024UL)
// Max dict size to keep memory consumption reasonable
#define DECOMPRESSOR_MAX_DICT_SIZE  ((LZMA_DICT_SIZE_KB) * 1024UL)
// 512 B input buffer to give LZMA lib a good chunk of work at a time
#define INPUT_BUFFER_SIZE           (512UL)
// Output buffer >> input buffer, so that most input will be consumed every time
#define OUTPUT_BUFFER_SIZE          (768UL)

// --------------------------------
// Prototypes

static int32_t decompressAndFlash(ParserContext_t                   *ctx,
                                  const BootloaderParserCallbacks_t *callbacks,
                                  bool                              finish);
static int32_t decompressData(uint8_t          *dstBuffer,
                              size_t           *dstBufferLen,
                              uint8_t          *srcBuffer,
                              size_t           *srcBufferLen,
                              ELzmaStatus      *status);
static void *lzmaAlloc(ISzAllocPtr p, size_t size);
static void lzmaFree(ISzAllocPtr p, void *address);

// --------------------------------
// Static variables

// Heap for decompressor
SL_ALIGN(4)
static uint8_t heapArray[DECOMPRESSOR_HEAP_SIZE] SL_ATTRIBUTE_ALIGN(4);
// Dictionary buffer
SL_ALIGN(4)
static uint8_t dict[DECOMPRESSOR_MAX_DICT_SIZE] SL_ATTRIBUTE_ALIGN(4);

// Output buffer needs to be bigger than input buffer
SL_ALIGN(4)
static uint8_t inputBuffer[INPUT_BUFFER_SIZE] SL_ATTRIBUTE_ALIGN(4);
static size_t inputBufferPos;

SL_ALIGN(4)
static uint8_t outputBuffer[OUTPUT_BUFFER_SIZE] SL_ATTRIBUTE_ALIGN(4);
static size_t outputBufferPos;

static CLzmaDec decompressorState;
static bool firstCallInProgTag;

static ISzAlloc lzmaAllocator = { &lzmaAlloc, &lzmaFree };
static int allocSeq = 0;

// --------------------------------
// LZMA Allocators

static void *lzmaAlloc(ISzAllocPtr p, size_t size)
{
  BTL_DEBUG_PRINT("LZMA: Allocate 0x");
  BTL_DEBUG_PRINT_WORD_HEX(size);
  BTL_DEBUG_PRINTLN(" bytes");
  (void)p;

  if (allocSeq == 0) {
    if (size > DECOMPRESSOR_HEAP_SIZE) {
      return NULL;
    }
    allocSeq++;
    return (void*)heapArray;
  } else if (allocSeq == 1) {
    if (size > DECOMPRESSOR_MAX_DICT_SIZE) {
      return NULL;
    }
    allocSeq++;
    return (void*)dict;
  } else {
    return NULL;
  }
}

static void lzmaFree(ISzAllocPtr p, void *address)
{
  (void)p;
  (void)address;

  if ((size_t)address == 0) {
    return;
  }

  BTL_DEBUG_PRINT("LZMA: Free from address 0x");
  BTL_DEBUG_PRINT_WORD_HEX((uint32_t)address);
  BTL_DEBUG_PRINT_LF();

  if (allocSeq > 0) {
    allocSeq--;
  }
}

// --------------------------------
// LZMA decompression implementation

static int32_t decompressData(uint8_t          *dstBuffer,
                              size_t           *dstBufferLen,
                              uint8_t          *srcBuffer,
                              size_t           *srcBufferLen,
                              ELzmaStatus      *status)
{
  SRes res;

  // We might not have large enough output buffer and need "more decompression rounds",
  // Use "LZMA_FINISH_ANY" mode and get so much of data asked for.
  res = LzmaDec_DecodeToBuf(&decompressorState,
                            dstBuffer,
                            dstBufferLen,
                            srcBuffer,
                            srcBufferLen,
                            LZMA_FINISH_ANY,
                            status);

  BTL_DEBUG_PRINT("Decompressed ");
  BTL_DEBUG_PRINT_WORD_HEX(*srcBufferLen);
  BTL_DEBUG_PRINT(" bytes into ");
  BTL_DEBUG_PRINT_WORD_HEX(*dstBufferLen);
  BTL_DEBUG_PRINTLN(" bytes");
  BTL_DEBUG_PRINT("  Status: ");
  switch (*status) {
    case LZMA_STATUS_NOT_SPECIFIED:
      BTL_DEBUG_PRINTLN("NOT_SPECIFIED");
      break;
    case LZMA_STATUS_FINISHED_WITH_MARK:
      BTL_DEBUG_PRINTLN("FINISHED_WITH_MARK");
      break;
    case LZMA_STATUS_NOT_FINISHED:
      BTL_DEBUG_PRINTLN("NOT_FINISHED");
      break;
    case LZMA_STATUS_NEEDS_MORE_INPUT:
      BTL_DEBUG_PRINTLN("NEEDS_MORE_INPUT");
      break;
    case LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK:
      BTL_DEBUG_PRINTLN("MAYBE_FINISHED_WITHOUT_MARK");
      break;
  }
  if (res == SZ_OK) {
    BTL_DEBUG_PRINTLN("  Result: OK");
  } else {
    BTL_DEBUG_PRINTLN("  Result: ERROR_DATA");
  }

  if (res != SZ_OK) {
    return BOOTLOADER_ERROR_COMPRESSION_DATA;
  }
  if (*status == LZMA_STATUS_NOT_SPECIFIED) {
    return BOOTLOADER_ERROR_COMPRESSION_STATE;
  }

  return BOOTLOADER_OK;
}

static int32_t decompressAndFlash(ParserContext_t                   *ctx,
                                  const BootloaderParserCallbacks_t *callbacks,
                                  bool                              finish)
{
  int32_t ret = BOOTLOADER_OK;
  ELzmaStatus status = LZMA_STATUS_NOT_FINISHED;
  while (status == LZMA_STATUS_NOT_FINISHED) {
    // outputPos starts out as the size of the output buffer less any leftover
    // data already in the buffer, or the remaining expected input size,
    // whichever is smaller
    unsigned int outputPos = OUTPUT_BUFFER_SIZE - outputBufferPos;
    // inputPos starts out as the size of the available input data
    unsigned int inputPos = inputBufferPos;

    BTL_DEBUG_PRINT("Decompress ");
    BTL_DEBUG_PRINT_WORD_HEX(inputPos);
    BTL_DEBUG_PRINT(" bytes to output buffer offset ");
    BTL_DEBUG_PRINT_WORD_HEX(outputBufferPos);
    BTL_DEBUG_PRINT_LF();

    // Decompress data. The output buffer starts after any unaligned data left
    // from a previous iteration.
    ret = decompressData(&outputBuffer[outputBufferPos],
                         &outputPos,
                         inputBuffer,
                         &inputPos,
                         &status);
    if (ret != BOOTLOADER_OK) {
      return ret;
    }
    // outputPos is now the actual size of the decompressed data
    // inputPos is now the position in the input buffer after processing

    // Flash word-aligned decompressed data, consisting of any leftover data
    // from a previous iteration + newly decompressed data
    size_t availableData = outputBufferPos + outputPos;
    size_t alignedAvailableData = availableData & ~3UL;
    ret = gbl_writeProgData(ctx, outputBuffer, alignedAvailableData, callbacks);

    // Store any unaligned bytes at the beginning of the output buffer
    if ((availableData - alignedAvailableData) > 0) {
      BTL_DEBUG_PRINT("  Output data remaining: 0x");
      BTL_DEBUG_PRINT_WORD_HEX(availableData - alignedAvailableData);
      BTL_DEBUG_PRINT_LF();

      memmove(&outputBuffer[0],
              &outputBuffer[alignedAvailableData],
              availableData - alignedAvailableData);
    }
    outputBufferPos = availableData - alignedAvailableData;

    // Move remaining input to the beginning of the input buffer
    if (inputBufferPos > inputPos) {
      BTL_DEBUG_PRINT("  Input data remaining: 0x");
      BTL_DEBUG_PRINT_WORD_HEX(inputBufferPos - inputPos);
      BTL_DEBUG_PRINT_LF();

      memmove(&inputBuffer[0],
              &inputBuffer[inputPos],
              inputBufferPos - inputPos);
    }
    inputBufferPos -= inputPos;
  }

  // Verify that decompression finished if this was expected to be the last data
  if (finish
      && (status != LZMA_STATUS_FINISHED_WITH_MARK)) {
    ret = BOOTLOADER_ERROR_COMPRESSION_STATE;
  }

  return ret;
}

// -----------------------------------------------------------------------------
// GBL tag implementation

int32_t gbl_lzmaEnterProgTag(ParserContext_t *ctx)
{
  (void) ctx;
  BTL_DEBUG_PRINTLN("LZMA: Enter tag");

  // Reset state variables
  memset(&decompressorState, 0, sizeof(CLzmaDec));
  LzmaDec_Construct(&decompressorState);
  firstCallInProgTag = true;
  outputBufferPos = 0;
  inputBufferPos = 0;

  return BOOTLOADER_OK;
}

int32_t gbl_lzmaParseProgTag(ParserContext_t *ctx,
                             void *data,
                             size_t length,
                             const BootloaderParserCallbacks_t *callbacks)
{
  uint8_t *dataArray = (uint8_t *)data;
  size_t dataOffset = 0UL;
  int32_t ret;

  if (callbacks->applicationCallback == NULL) {
    // Nothing to do
    return BOOTLOADER_OK;
  }

  if (firstCallInProgTag) {
    BTL_ASSERT(length >= 17UL);
    firstCallInProgTag = false;

    // First call to function contains programming address in first word
    ctx->programmingAddress = *(uint32_t *)data;

    // Parse first 5 bytes of payload to get dict size, allocate memory
    SRes res = LzmaDec_Allocate(&decompressorState,
                                &dataArray[4U],
                                LZMA_PROPS_SIZE,
                                &lzmaAllocator);
    if (res != SZ_OK) {
      return BOOTLOADER_ERROR_COMPRESSION_MEM;
    }
    LzmaDec_Init(&decompressorState);

    // 4 bytes address + 5 byte header + 8 byte file length should be skipped
    dataOffset = 17UL;
  }

  size_t remainingInputBytes = length - dataOffset;

  // Accumulate input data until the input buffer is nearly full
  if (remainingInputBytes > (INPUT_BUFFER_SIZE - inputBufferPos)) {
    // Input buffer full, decompress the data
    ret = decompressAndFlash(ctx, callbacks, false);
    if (ret != BOOTLOADER_OK) {
      return ret;
    }

    // If we can't fit new data after flushing what we have, we can't do
    // much -- need to increase output buffer size compared to input buffer
    // size, or try decompressing again until status != NOT_FINISHED
    if (remainingInputBytes > (INPUT_BUFFER_SIZE - inputBufferPos)) {
      return BOOTLOADER_ERROR_COMPRESSION_DATALEN;
    }
  }

  // Put new input data into input buffer
  memcpy(&inputBuffer[inputBufferPos],
         &dataArray[dataOffset],
         remainingInputBytes);
  inputBufferPos += remainingInputBytes;

  return BOOTLOADER_OK;
}

size_t gbl_lzmaNumBytesRequired(ParserContext_t *ctx)
{
  // If this is the first data in the tag, we need:
  //  - a full word to set the programming address correctly
  //  - 5 bytes containing LZMA_PROPS (lc, lp, dict size)
  //  - 8 bytes containing size of decompressed data
  if (ctx->offsetInTag == 0) {
    return 17UL;
  } else {
    return 1UL;
  }
}

int32_t gbl_lzmaExitProgTag(ParserContext_t *ctx,
                            const BootloaderParserCallbacks_t *callbacks)
{
  (void) ctx;
  (void) callbacks;
  int32_t ret;
  BTL_DEBUG_PRINTLN("LZMA: Exit tag");

  if (callbacks->applicationCallback == NULL) {
    // Nothing to do
    return BOOTLOADER_OK;
  }

  // Finish decompressing remaining data
  ret = decompressAndFlash(ctx, callbacks, true);
  if (ret != BOOTLOADER_OK) {
    return ret;
  }

  if (outputBufferPos > 0) {
    // We have some remaining unaligned data
    for (size_t i = outputBufferPos; i < 4UL; i++) {
      outputBuffer[i] = 0xFFU;
    }
    BTL_DEBUG_PRINT("Finish: data len = ");
    BTL_DEBUG_PRINT_WORD_HEX(outputBufferPos);
    BTL_DEBUG_PRINT(" aligned len = ");
    BTL_DEBUG_PRINT_WORD_HEX(4UL);
    BTL_DEBUG_PRINT_LF();
    ret = gbl_writeProgData(ctx, outputBuffer, 4UL, callbacks);
    if (ret != BOOTLOADER_OK) {
      return ret;
    }
  }

  // Free decompressor memory (heap and dict allocations)
  LzmaDec_Free(&decompressorState, &lzmaAllocator);

  return ret;
}
