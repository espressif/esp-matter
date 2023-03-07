/******************************************************************************

 @file flash_interface

    @brief An interface that abstracts flash operations for OAD
           this allows on-chip and off-chip OAD to use the same flash APIs

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
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

 ******************************************************************************
 
 
 *****************************************************************************/

/** ============================================================================
 *  @defgroup FLASH_INTERFACE Flash Interface
 *  @brief An interface that abstracts flash operations for OAD
 *         this allows on-chip and off-chip OAD to use the same flash APIs
 *
 *  @{
 *
 *  # Header Include #
 *  The flash interface header file should be included as follows:
 *  @code
 *  #include <common/cc26xx/flash_interface/flash_interface.h>
 *  @endcode
 *
 *  # Overview #
 *
 *  The flash interface is a layer of abstraction between the OAD application
 *  and protocol and the targeted flash implementation. The flash interface
 *  provides a common set of APIs for both internal flash and external flash.
 *  In this way, OAD applications can be written in the same way for both
 *  on-chip (internal flash) and off-chip (external flash) OAD.
 *
 *  ## Selecting an interface
 *    There are three implementations of the flash interface provided:
 *      - flash_interface_internal.c : Supports on-chip (internal flash) OAD
 *      - flash_interface_ext_rtos.c : Supports off-chip OAD (external flash)
 *        using TI Middleware driver
 *      - flash_interface_ext_rtos_NVS.c : Supports off-chip OAD
 *        (external flash) using NVS driver
 *
 *  To select the interface, the user should include one of the above .c
 *  files within their project
 *
 *  ## Initialzation
 *    Initialze the module as shown below
 *    @code
 *    // Open the flash interface
 *    flash_init();
 *    @endcode
 *
 *  ## Opening the module
 *    Open the module as shown below
 *    @code
 *    bool flashStat = flash_open();
 *
 *    if(flashStat != true)
 *    {
 *        // Report error
 *    }
 *    @endcode

 *  ## Using the module
 *    After initialzing and opening the module, the read and write APIs can be
 *    at any time. It is recommended to check the return value to see if the
 *    operation succeeded
 *
 *  ## Closing the module
 *    Open the module as shown below
 *    @code
 *    flash_close();
 *    @endcode
 *
 *  @file  flash_interface.h
 *
 *  @brief An interface that abstracts flash operations for OAD
 *         this allows on-chip and off-chip OAD to use the same flash APIs
 */
#ifndef FLASH_INTERFACE
#define FLASH_INTERFACE

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)

/*********************************************************************
 * CONSTANTS
 */

/*!
 * Flash operation succeeded
 */
#define FLASH_SUCCESS   0x00

/*!
 * Flash operation failed
 */
#define FLASH_FAILURE   0xFF

/*!
 * Maximum SPI read size, used for off-chip
 */
#define SPI_MAX_READ_SZ 1024

/*********************************************************************
 * MACROS
 */
#if defined(CC26XX_R2)
    #define FLASH_ADDRESS(page, offset)     (((page) << 12) + (offset))
    #define FLASH_PAGE(addr)                (addr >> 12)
    #define INTFLASH_PAGE_MASK              0xFFFFF000
    #define INTFLASH_PAGE_SIZE              0x1000
    #define MAX_ONCHIP_FLASH_PAGES          32
    #define MAX_OFFCHIP_METADATA_PAGES      MAX_ONCHIP_FLASH_PAGES
#elif defined(DeviceFamily_CC26X2) || defined(DeviceFamily_CC13X2) || \
      defined(DeviceFamily_CC26X2X7) || defined(DeviceFamily_CC13X2X7) || \
      defined(DeviceFamily_CC26X1) || defined(DeviceFamily_CC13X1)
    #define FLASH_ADDRESS(page, offset)     (((page) << 13) + (offset))
    #define FLASH_PAGE(addr)                (addr >> 13)
    #define INTFLASH_PAGE_MASK              0xFFFFE000
    #define INTFLASH_PAGE_SIZE              0x2000
#if defined(DeviceFamily_CC26X2X7) || defined(DeviceFamily_CC13X2X7)
    #define MAX_ONCHIP_FLASH_PAGES          88
#else
    #define MAX_ONCHIP_FLASH_PAGES          44
#endif
    #define MAX_OFFCHIP_METADATA_PAGES      MAX_ONCHIP_FLASH_PAGES
#else
    #error
#endif

/**
 * @defgroup EXT_FLASH_MACROS Macros and constants for external flash
 * @{
 */

/*!
 * MMacro to return an address based on an external flash page and offset into
 * the page
 */
#define EXT_FLASH_ADDRESS(page, offset)     (((page) << 12) + (offset))

/*!
 * Macro to return a page based on an external flash page and offset into
 * the page
 */
#define EXT_FLASH_PAGE(addr)                (addr >> 12)

/*!
 * Define for masking on external flash page size
 */
#define EXTFLASH_PAGE_MASK                  0xFFFFF000

/*!
 * Page size of external flash
 */
#define EFL_PAGE_SIZE                       0x1000

/*!
 * Total size of external flash
 */
#define EFL_FLASH_SIZE                      0x100000

/** @} End EXT_FLASH_MACROS */

/*!
 * Initialize flash interface
 */
extern void flash_init(void);

/*!
 * Open access to flash through flash_interface
 *
 * @return  true if interface successfully opened, false otherwise
 */
extern bool flash_open(void);

/*!
 * Close/cleanup access to flash
 */
extern void flash_close(void);

/*!
 * Check if the interface is built for external flash
 *
 * @return  true if the target has external flash, false otherwise
 */
extern bool hasExternalFlash(void);

/*!
 * Read data from flash using address
 *
 * @param   addr   - address to read from
 * @param   pBuf   - pointer to buffer into which data is read.
 * @param   len    - length of data to read in bytes.
 *
 * @return  @ref FLASH_SUCCESS if read succeeded
 *          @ref FLASH_FAILURE if the flash returned an error
 */
extern uint8_t readFlash(uint_least32_t addr, uint8_t *pBuf, size_t len);

/*!
 * Read data from flash using page and offset
 *
 * @param   page   - page to read from in flash
 * @param   offset - offset into flash page to begin reading
 * @param   pBuf   - pointer to buffer into which data is read.
 * @param   len    - length of data to read in bytes.
 *
 * @return  status - @ref FLASH_SUCCESS if programmed successfully or
 *                   @ref FLASH_FAILURE if programming failed
 */
extern uint8_t readFlashPg(uint8_t page, uint32_t offset, uint8_t *pBuf,
                            uint16_t len);

/*!
 * Write data to flash using address
 *
 * @param   addr   - address to write to in flash
 * @param   pBuf   - pointer to buffer of data to write
 * @param   len    - length of data to write in bytes
 *
 * @return  status - @ref FLASH_SUCCESS if programmed successfully or
 *                   @ref FLASH_FAILURE if programming failed
 */
extern uint8_t writeFlash(uint_least32_t addr, uint8_t *pBuf, size_t len);

/*!
 * Write data to flash using page and offset
 *
 * @param   page   - page to write to in flash
 * @param   offset - offset into flash page to begin writing
 * @param   pBuf   - pointer to buffer of data to write
 * @param   len    - length of data to write in bytes
 *
 * @return  status - @ref FLASH_SUCCESS if programmed successfully or
 *                   @ref FLASH_FAILURE if programming failed
 */
extern uint8_t writeFlashPg(uint8_t page, uint32_t offset, uint8_t *pBuf,
                            uint16_t len);

/*!
 * Erase selected flash page.
 *
 * @param   page - the page to erase.
 *
 * @return  status - @ref FLASH_SUCCESS if page erased successfully or
 *                   @ref FLASH_FAILURE if erase failed
 */
extern uint8_t eraseFlashPg(uint8_t page);

/*!
 * Erase flash page.
 *
 * @param   page - the page to erase.
 *
 * @return  status - @ref FLASH_SUCCESS if page erased successfully or
 *                   @ref FLASH_FAILURE if erase failed
 */
extern uint8_t eraseFlash(uint8_t page);

/*********************************************************************
*********************************************************************/

/** @} End FLASH_INTERFACE */

#ifdef __cplusplus
}
#endif

#endif /* FLASH_INTERFACE */
