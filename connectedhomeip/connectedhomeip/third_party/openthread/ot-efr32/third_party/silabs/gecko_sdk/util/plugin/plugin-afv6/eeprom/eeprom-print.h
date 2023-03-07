/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef EEPROM_PRINT_H
#define EEPROM_PRINT_H

/**
 * Print a number of contiguous 8-byte blocks in a readable format.
 *
 * @param numBlocks The number of 8-byte blocks to print
 * @param block A pointer to the first byte in the first block
 * @param crBetweenBlocks Whether or not to add a newline between blocks
 */
void emberAfPrint8ByteBlocks(uint8_t numBlocks,
                             const uint8_t *block,
                             bool crBetweenBlocks);

#endif // EEPROM_PRINT_H
