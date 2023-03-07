/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif

void emberAfPrint8ByteBlocks(uint8_t numBlocks,
                             const uint8_t *block,
                             bool crBetweenBlocks)
{
  uint8_t i;
  for (i = 0; i < numBlocks; i++) {
    emberAfCorePrintBuffer(block + 8 * i,
                           8,     // 8 bytes from the buffer
                           true); // withSpaces?
    // By moving the '%p' to a separate function call, we can
    // save CONST space.  The above string is duplicated elsewhere in the
    // code and therefore will be deadstripped.
    emberAfCorePrintln(" %p",
                       ((crBetweenBlocks
                         || ((i + 1) == numBlocks))
                        ? "\r\n"
                        : ""));
    emberAfCoreFlush();
  }
}
