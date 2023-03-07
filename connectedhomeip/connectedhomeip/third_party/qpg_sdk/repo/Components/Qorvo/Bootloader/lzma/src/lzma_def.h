#include "lzma.h"
// From lzma-decompression-v4
lzma_result lzma_decode(CLzmaDecoderState* vs, void* inStream, UInt32 inSize, UInt32* inSizeProcessed, void* outStream, UInt32 outSize, UInt32* outSizeProcessed);

// From lzma_gpHal_Flash
/**
 * @brief Resets the lzma_gpHal_Flash module,
 *        Should be called BEFORE running lzma_decode()
 */
void lzma_gpHal_Flash_ResetStatus( void );

/**
 * @brief Function to retrieve the status of the lzma_gpHal_Flash module
 *        Provides feedback AFTER running lzma_decode() if the flash writes have
 *        been successful
 * return  gpHal_FlashError_Success if all
 *         last error returned by gpHal_FlashWrite() when
 */
gpHal_FlashError_t lzma_gpHal_Flash_GetStatus( void );
