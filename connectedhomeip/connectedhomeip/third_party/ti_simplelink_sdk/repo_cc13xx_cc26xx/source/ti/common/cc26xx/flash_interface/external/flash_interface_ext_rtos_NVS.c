/*******************************************************************************

 @file  flash_interface_ext_rtos_NVS.c

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

#include <common/cc26xx/flash_interface/flash_interface.h>
#include <ti_drivers_config.h>
#include <ti/drivers/NVS.h>

/*******************************************************************************
 * Constants and macros
 */

/*******************************************************************************
 * PRIVATE VARIABLES
 */
static bool isOpen = false;
static NVS_Handle nvsHandle;
static NVS_Attrs regionAttrs;
static NVS_Params nvsParams;

/*******************************************************************************
 * PRIVATE FUNCTIONS
 */

/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @fn      flash_init
 *
 * @brief   Open access to flash
 *
 * @param   None.
 *
 * @return  TRUE if interface successfully opened
 */
void flash_init(void)
{
    NVS_init();
    NVS_Params_init(&nvsParams);
}

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
        nvsHandle = NVS_open(CONFIG_NVSEXTERNAL, &nvsParams);
        if (nvsHandle != NULL)
        {
            isOpen = true;
            // Also read the region's attribute
            NVS_getAttrs(nvsHandle, &regionAttrs);
        }
    }

    return (isOpen);
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
    NVS_close(nvsHandle);
    isOpen = false;
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
  uint8_t flashStat = FLASH_FAILURE;

  if(isOpen)
  {
    if(NVS_read(nvsHandle, EXT_FLASH_ADDRESS(page,offset), pBuf, len)
                == NVS_STATUS_SUCCESS)
    {
        flashStat = FLASH_SUCCESS;
    }
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
  uint8_t flashStat = FLASH_FAILURE;

  if(isOpen)
  {
    if(NVS_read(nvsHandle, addr, pBuf, len) == NVS_STATUS_SUCCESS)
    {
        flashStat = FLASH_SUCCESS;
    }
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
  uint8_t flashStat = FLASH_FAILURE;
  if(isOpen)
  {
    if(NVS_write(nvsHandle, (uint32_t)EXT_FLASH_ADDRESS(page, offset),
                          pBuf, len, NVS_WRITE_POST_VERIFY) == NVS_STATUS_SUCCESS)
    {
        flashStat = FLASH_SUCCESS;
    }
  }
  return (flashStat);
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
    uint8_t flashStat = FLASH_FAILURE;
    if(isOpen)
    {
        if(NVS_write(nvsHandle, addr,  pBuf, len, NVS_WRITE_PRE_VERIFY | NVS_WRITE_POST_VERIFY)
                 == NVS_STATUS_SUCCESS)
       {
           flashStat = FLASH_SUCCESS;
       }
    }
    return (flashStat);
}

/*********************************************************************
 * @fn      eraseFlashPg
 *
 * @brief   Erase selected flash page.
 *
 * @param   page - the page to erase.
 *
 * @return  status - FLASH_SUCCESS if programmed successfully or
 *                   FLASH_FAILURE if programming failed
 */
uint8_t eraseFlashPg(uint8_t page)
{
  uint8_t flashStat = FLASH_FAILURE;
  if(isOpen)
  {
      if(NVS_erase(nvsHandle, EXT_FLASH_ADDRESS(page,0), EFL_PAGE_SIZE)
         == NVS_STATUS_SUCCESS)
      {
          flashStat = FLASH_SUCCESS;
      }
  }

  return flashStat;
}


