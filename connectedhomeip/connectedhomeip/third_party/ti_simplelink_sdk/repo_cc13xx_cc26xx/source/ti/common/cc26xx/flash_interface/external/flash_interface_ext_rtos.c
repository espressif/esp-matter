/*******************************************************************************

 @file  flash_interface_ext_rtos.c

 @brief Interface to external SPI flash accessed via RTOS SPI driver

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 *******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
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

 *******************************************************************************
 
 
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <string.h>
#include "hal_board.h"
#include "flash_interface.h"
#include "ExtFlash.h"
#include "ext_flash_layout.h"

/*******************************************************************************
 * Constants and macros
 */

/*******************************************************************************
 * PRIVATE VARIABLES
 */
static bool isOpen = false;

/*******************************************************************************
 * PRIVATE FUNCTIONS
 */

/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @fn      flash_open
 *
 * @brief   Open access to flash
 *
 * @param   None.
 *
 * @return  TRUE if interface successfully opened
 */
bool flash_open(void)
{
    if (!isOpen)
    {
        isOpen = ExtFlash_open();
    }

    return (isOpen ? TRUE : FALSE);
}


/*********************************************************************
 * @fn      flash_close
 *
 * @brief   Close/cleanup access to flash
 *
 * @param   None.
 *
 * @return  None.
 */
void flash_close(void)
{
  if (isOpen)
  {
    isOpen = false;
    ExtFlash_close();
  }
}

/*********************************************************************
 * @fn      hasExternalFlash
 *
 * @brief   Check if the interface is built for external flash
 *
 * @param   None.
 *
 * @return  TRUE if the target has external flash
 */
bool hasExternalFlash(void)
{
  return (true);
}

/*********************************************************************
 * @fn      readFlashPg
 *
 * @brief   Read data from flash.
 *
 * @param   page   - page to read from in flash
 * @param   offset - offset into flash page to begin reading
 * @param   pBuf   - pointer to buffer into which data is read.
 * @param   len    - length of data to read in bytes.
 *
 * @return  None.
 */
uint8_t readFlashPg(uint8_t page, uint32_t offset, uint8_t *pBuf, uint16_t len)
{
  uint8_t flashStat = FLASH_SUCCESS;

  if(isOpen)
  {
    ExtFlash_read(FLASH_ADDRESS(page,offset), len, pBuf);
  }
  else
  {
    flashStat = FLASH_FAILURE;
  }

  return (flashStat);
}

/*********************************************************************
 * @fn      readFlash
 *
 * @brief   Read data from flash.
 *
 * @param   addr   - address to read from
 * @param   pBuf   - pointer to buffer into which data is read.
 * @param   len    - length of data to read in bytes.
 *
 * @return  None.
 */
uint8_t readFlash(uint_least32_t addr, uint8_t *pBuf, size_t len)
{
  uint8_t flashStat = FLASH_SUCCESS;

  if(isOpen)
  {
    ExtFlash_read(addr, len, pBuf);
  }
  else
  {
    flashStat = FLASH_FAILURE;
  }

  return (flashStat);
}

/*********************************************************************
 * @fn      writeFlashPg
 *
 * @brief   Write data to flash.
 *
 * @param   page   - page to write to in flash
 * @param   offset - offset into flash page to begin writing
 * @param   pBuf   - pointer to buffer of data to write
 * @param   len    - length of data to write in bytes
 *
 * @return  status - FLASH_SUCCESS if programmed successfully or
 *                   FLASH_FAILURE if programming failed
 */
uint8_t writeFlashPg(uint8_t page, uint32_t offset, uint8_t *pBuf, uint16_t len)
{
  bool flashStat;
  if(isOpen)
  {
    flashStat = ExtFlash_write((uint32_t)FLASH_ADDRESS(page, offset), len, pBuf);
  }
  else
  {
    flashStat = false;
  }

  return (flashStat == true) ? FLASH_SUCCESS : FLASH_FAILURE;
}

/*********************************************************************
 * @fn      writeFlash
 *
 * @brief   Write data to flash.
 *
 * @param   addr   - address to write to in flash
 * @param   pBuf   - pointer to buffer of data to write
 * @param   len    - length of data to write in bytes
 *
 * @return  status - FLASH_SUCCESS if programmed successfully or
 *                   FLASH_FAILURE if programming failed
 */
uint8_t writeFlash(uint_least32_t addr, uint8_t *pBuf, size_t len)
{
  // This is unsupported on external flash
  return (FLASH_FAILURE);
}

/*********************************************************************
 * @fn      eraseFlash
 *
 * @brief   Erase selected flash page.
 *
 * @param   page - the page to erase.
 *
 * @return  None.
 */
uint8_t eraseFlash(uint8_t page)
{
  bool flashStat;

  if(isOpen)
  {
    flashStat = ExtFlash_erase(FLASH_ADDRESS(page,0), HAL_FLASH_PAGE_SIZE);
  }
  else
  {
    flashStat = false;
  }

  return ((flashStat == true) ? FLASH_SUCCESS : FLASH_FAILURE);
}


