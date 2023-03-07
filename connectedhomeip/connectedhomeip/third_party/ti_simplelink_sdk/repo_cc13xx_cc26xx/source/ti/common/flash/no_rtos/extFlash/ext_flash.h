/******************************************************************************

 @file  ext_flash.h

 @brief External flash storage abstraction.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#ifndef EXT_FLASH_H
#define EXT_FLASH_H

#include <stdlib.h>
#include <stdbool.h>

#define EXT_FLASH_PAGE_SIZE   4096

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    uint32_t deviceSize; // bytes
    uint8_t manfId;      // manufacturer ID
    uint8_t devId;       // device ID
} ExtFlashInfo_t;

/**
* Initialize storage driver.
*
* @return True when successful.
*/
extern bool extFlashOpen(void);

/**
* Close the storage driver
*/
extern void extFlashClose(void);

/**
* Get flash information
*/
extern const ExtFlashInfo_t *extFlashInfo(void);

/**
* Read storage content
*
* @return True when successful.
*/
extern bool extFlashRead(size_t offset, size_t length, uint8_t *buf);

/**
* Erase storage sectors corresponding to the range.
*
* @return True when successful.
*/
extern bool extFlashErase(size_t offset, size_t length);

/**
* Write to storage sectors.
*
* @return True when successful.
*/
extern bool extFlashWrite(size_t offset, size_t length, const uint8_t *buf);

/**
* Test the flash (power on self-test)
*
* @return True when successful.
*/
extern bool extFlashTest(void);

#ifdef __cplusplus
}
#endif

#endif /* EXT_FLASH_H */
