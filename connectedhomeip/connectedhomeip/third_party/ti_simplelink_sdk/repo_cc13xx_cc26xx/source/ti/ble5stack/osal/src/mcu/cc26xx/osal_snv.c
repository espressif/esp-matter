/******************************************************************************

 @file  osal_snv.c

 @brief This module contains the OSAL simple non-volatile memory functions.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */

#include "hal_adc.h"
#include "hal_flash.h"
#include "hal_types.h"
#include "pwrmon.h"
#include "comdef.h"
#include "osal.h"
#include "osal_snv.h"
#include "hal_assert.h"
#include "saddr.h"
#include <driverlib/vims.h>
#include <driverlib/aon_batmon.h>

#ifdef OSAL_SNV_UINT16_ID
# error "This OSAL SNV implementation does not support the extended ID space"
#endif

#if !defined( NO_OSAL_SNV )

/*********************************************************************
 * CONSTANTS
 */

// Default byte value when flash is erased
#define OSAL_NV_ERASED                  0xFF

// NV page header size in bytes
#define OSAL_NV_PAGE_HDR_SIZE           4

// In case pages 0-1 are ever used, define a null page value.
#define OSAL_NV_PAGE_NULL               0

// In case item Id 0 is ever used, define a null item value.
#define OSAL_NV_ITEM_NULL               0

// Length in bytes of a flash word
#define OSAL_NV_WORD_SIZE               HAL_FLASH_WORD_SIZE

// NV page header offset within a page
#define OSAL_NV_PAGE_HDR_OFFSET         0

// Flag in a length field of an item header to indicate validity
// of the length field
#define OSAL_NV_INVALID_LEN_MARK        0x8000

// Flag in an ID field of an item header to indicate validity of
// the identifier field
#define OSAL_NV_INVALID_ID_MARK         0x8000

// Bit difference between active page state indicator value and
// transfer page state indicator value
#define OSAL_NV_ACTIVE_XFER_DIFF        0x00100000

// active page state indicator value
#define OSAL_NV_ACTIVE_PAGE_STATE       OSAL_NV_ACTIVE_XFER_DIFF

// transfer page state indicator value
#define OSAL_NV_XFER_PAGE_STATE         (OSAL_NV_ACTIVE_PAGE_STATE ^           \
                                         OSAL_NV_ACTIVE_XFER_DIFF)

#define OSAL_NV_MIN_COMPACT_THRESHOLD   70 // Minimum compaction threshold
#define OSAL_NV_MAX_COMPACT_THRESHOLD   95 // Maximum compaction threshold

/*********************************************************************
 * MACROS
 */

// Checks whether CC26xx voltage high enough to erase/write NV
#ifdef NV_VOLTAGE_CHECK
#define SNV_CHECK_VOLTAGE()  (PWRMON_check(MIN_VDD_FLASH))
#else
#define SNV_CHECK_VOLTAGE()  (TRUE)
#endif //NV_VOLTAGE_CHECK

/*********************************************************************
 * TYPEDEFS
 */

// NV item header structure
typedef struct
{
  uint16 id;
  uint16 len;
} osalNvItemHdr_t;
// Note that osalSnvId_t and osalSnvLen_t cannot be bigger than uint16

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

extern uint8* HalFlashGetAddress( uint8 pg, uint16 offset );

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Allocate SNV area in flash.
#if defined(__IAR_SYSTEMS_ICC__)
#if !defined(CC26X2) && !defined(CC13X2) && !defined(CC13X2P)
#pragma data_alignment=4096
#else //Agama CC26X2 || CC13X2 || CC13X2P
#pragma data_alignment=8192
#endif //Agama !CC26X2 && !CC13X2 && !CC13X2P
#pragma location = ".snvSectors"
const uint8 SNV_FLASH[OSAL_NV_PAGE_SIZE * OSAL_NV_PAGES_USED];
#elif defined(__TI_COMPILER_VERSION) || defined(__TI_COMPILER_VERSION__)
#pragma location = (OSAL_NV_PAGE_BEGIN * OSAL_NV_PAGE_SIZE);
const uint8 SNV_FLASH[OSAL_NV_PAGE_SIZE * OSAL_NV_PAGES_USED] = {0x00};
#else
#error "Unknown Compiler! Support for SNV not provided!"
#endif

// Flash Pages
static uint8 nvPageBeg;
static uint8 nvPageEnd;

// active page
static uint8 activePg;

// active page offset
static uint16 pgOff;

// flag to indicate that an error has occurred while writing to or erasing the
// flash device. Once this flag indicates failure, it is unsafe to attempt
// another write or erase.
static uint8 failF;

// Flag to indicate that a non-fatal error occurred while writing to or erasing
// Flash memory. If flag is set, it's safe to attempt another write or erase.
// This flag is reset by any API calls that cause an erase/write to Flash.
static uint8 failW;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static uint8  initNV( void );
static void   setActivePage( uint8 pg );
static void   setXferPage(void);
static void   erasePage( uint8 pg );
static void   findOffset( void );
static void   compactPage( uint8 pg );
static void   writeWord( uint8 pg, uint16 offset, uint8 *pBuf, osalSnvLen_t cnt );

/*********************************************************************
 * @fn      enableCache
 *
 * @brief   enable cache.
 *
 * @param   state - the VIMS state returned from disableCache.
 *
 * @return  none.
 */
static void enableCache ( uint8 state )
{
  if ( state != VIMS_MODE_DISABLED )
  {
    // Enable the Cache.
    VIMSModeSet( VIMS_BASE, VIMS_MODE_ENABLED );
  }
}

/*********************************************************************
 * @fn      disableCache
 *
 * @brief   invalidate and disable cache.
 *
 * @param   none
 *
 * @return  VIMS state
 */
static uint8 disableCache ( void )
{
  uint8 state = VIMSModeGet( VIMS_BASE );

  // Check VIMS state
  if ( state != VIMS_MODE_DISABLED )
  {
    // Invalidate cache
    VIMSModeSet( VIMS_BASE, VIMS_MODE_DISABLED );

    // Wait for disabling to be complete
    while ( VIMSModeGet( VIMS_BASE ) != VIMS_MODE_DISABLED );

  }

  return state;
}

/*********************************************************************
 * @fn      initNV
 *
 * @brief   Initialize the NV flash pages.
 *
 * @param   none
 *
 * @return  TRUE if initialization succeeds. FALSE, otherwise.
 */
static uint8 initNV( void )
{
  uint8  pg;
  uint32 pgHdr;
  uint8  xferPg = OSAL_NV_PAGE_NULL;

  failF = failW = FALSE;
  activePg = OSAL_NV_PAGE_NULL;

  // Pick active page and clean up erased page if necessary
  for ( pg = nvPageBeg; pg <= nvPageEnd; pg++ )
  {
    HalFlashRead(pg, OSAL_NV_PAGE_HDR_OFFSET, (uint8 *)(&pgHdr), OSAL_NV_PAGE_HDR_SIZE);

    if (pgHdr == OSAL_NV_ACTIVE_PAGE_STATE)
    {
      if (activePg != OSAL_NV_PAGE_NULL)
      {
        // Both pages are active only when power failed during flash erase and
        // with very low probability.
        // As it is hard (code size intensive) to figure out which page is the real active page,
        // and theoretically impossible as well in lowest probability, erase both pages
        // in this case
        erasePage(activePg);
        erasePage(pg);
        activePg = OSAL_NV_PAGE_NULL;
      }
      else
      {
        activePg = pg;
      }
    }
    else if (pgHdr == OSAL_NV_XFER_PAGE_STATE)
    {
      // workaround for new NV system, both pages are 0x00'd.
      if (xferPg != NULL)
      {
        // erase everything!
        erasePage(xferPg);

      }
      xferPg = pg;
    }
    else
    {
      // Erase this page if it is not erased.
      // This is to ensure that any page that we're in the middle of
      // compacting gets erased.
      erasePage(pg);
    }
  }

  if (activePg == OSAL_NV_PAGE_NULL)
  {
    if (xferPg == OSAL_NV_PAGE_NULL)
    {
      // Both pages are erased. This must be initial state.
      // Pick one page as active page.
      setActivePage(nvPageBeg);
      pgOff = OSAL_NV_PAGE_HDR_SIZE;

      // If setting active page from a completely erased page failed,
      // it is not recommended to operate any further.
      // Other cases, even if non-active page is corrupt, NV module can still read
      // the active page content and hence this function could return TRUE.
      return (!failW);
    }
    else
    {
      // Compacting a page hasn't completed in previous power cycle.
      // Complete the compacting.
      activePg = xferPg;
      findOffset();

      compactPage(xferPg);
    }
  }
  else
  {
    if (xferPg != OSAL_NV_PAGE_NULL)
    {
      // Compacting has completed except for the final step of erasing
      // the xferPage.
      erasePage(xferPg);
    }

    // find the active page offset to write a new variable location item
    findOffset();
  }

  return !failW;
}

/*********************************************************************
 * @fn      setActivePage
 *
 * @brief   Set page header active state to be active.
 *
 * @param   pg - Valid NV page to activate.
 *
 * @return  none
 */
static void setActivePage( uint8 pg )
{
  uint32 pgHdr;

  pgHdr = OSAL_NV_ACTIVE_PAGE_STATE;

  // Write word to flash.
  writeWord( pg, OSAL_NV_PAGE_HDR_OFFSET, (uint8*) &pgHdr, 1 );

  if (!failF)
  {
    activePg = pg;
  }
}

/*********************************************************************
 * @fn      setXferPage
 *
 * @brief   Set active page header state to be transfer state.
 *
 * @param   none
 *
 * @return  none
 */
static void setXferPage(void)
{
  uint32 pgHdr;

  // erase difference bit between active state and xfer state
  pgHdr = OSAL_NV_XFER_PAGE_STATE;

  // Write word to flash.
  writeWord( activePg, OSAL_NV_PAGE_HDR_OFFSET, (uint8*)&pgHdr, 1);
}

/*********************************************************************
 * @fn      erasePage
 *
 * @brief   Erases a page in Flash.
 *
 * @param   pg - Valid NV page to erase.
 *
 * @return  none
 */
static void erasePage( uint8 pg )
{
  if (SNV_CHECK_VOLTAGE() == FALSE)
  {
    // Power monitor indicates low voltage
    failW = TRUE;
    return;
  }
  else
  {
    halIntState_t cs;
    uint8 state;
    uint32_t err;

    HAL_ENTER_CRITICAL_SECTION(cs);

    // Disable the cache.
    state = disableCache();

    // Erase the page.
    err = FlashSectorErase( (uint32)HalFlashGetAddress(pg, 0));

    // Enable the cache.
    enableCache(state);

    // Page erase failed, further usage is unsafe.
    if (err != FAPI_STATUS_SUCCESS)
    {
      failF = failW = TRUE;
    }

    HAL_EXIT_CRITICAL_SECTION(cs);
  }
}

/*********************************************************************
 * @fn      findOffset
 *
 * @brief   find an offset of an empty space in active page
 *          where to write a new item to.
 *
 * @param   None
 *
 * @return  none
 */
static void findOffset(void)
{
  uint16 offset;
  for (offset = OSAL_NV_PAGE_SIZE - OSAL_NV_WORD_SIZE;
       offset >= OSAL_NV_PAGE_HDR_SIZE;
       offset -= OSAL_NV_WORD_SIZE)
  {
    uint32 tmp;

    HalFlashRead(activePg, offset, (uint8 *)&tmp, OSAL_NV_WORD_SIZE);
    if (tmp != 0xFFFFFFFF)
    {
      break;
    }
  }

  pgOff = offset + OSAL_NV_WORD_SIZE;
}

/*********************************************************************
 * @fn      findItem
 *
 * @brief   find a valid item from a designated page and offset
 *
 * @param   pg       - NV page
 * @param   offset   - offset in the NV page from where to start
 *                     search up.
 *                     Usually this parameter is set to the empty space
 *                     offset.
 * @param   id       - NV item ID to search for
 *
 * @return  offset of the item, 0 when not found
 */
static uint16 findItem(uint8 pg, uint16 offset, osalSnvId_t id)
{
  offset -= OSAL_NV_WORD_SIZE;

  while (offset >= OSAL_NV_PAGE_HDR_SIZE)
  {
    osalNvItemHdr_t hdr;

    HalFlashRead(pg, offset, (uint8 *) &hdr, OSAL_NV_WORD_SIZE);

    if (hdr.id == id)
    {
      // item found
      // length field could be corrupt. Mask invalid length mark.
      osalSnvLen_t len = hdr.len & ~OSAL_NV_INVALID_LEN_MARK;
      return offset - len;
    }
    else if (hdr.len & OSAL_NV_INVALID_LEN_MARK)
    {
      offset -= OSAL_NV_WORD_SIZE;
    }
    else
    {
      // valid length field
      if (hdr.len + OSAL_NV_WORD_SIZE <= offset)
      {
        // valid length
        offset -= hdr.len + OSAL_NV_WORD_SIZE;
      }
      else
      {
        // active page is corrupt
        // This could happen if NV initialization failed upon failure to erase
        // page and active page is set to uncleanly erased page.
        HAL_ASSERT_FORCED();
        return 0;
      }
    }
  }

  return 0;
}

/*********************************************************************
 * @fn      writeItem
 *
 * @brief   Write a data item to NV. Function can write an entire item to NV
 *
 * @param   pg         - Page number
 * @param   offset     - offset within the NV page where to write the new item
 * @param   id         - NV item ID
 * @param   alignedLen - Length of data to write in bytes, aligned in flash word
 *                       boundary
 * @param  *pBuf       - Data to write.
 *
 * @return  none
 */
static void writeItem( uint8 pg, uint16 offset, osalSnvId_t id, uint16 alignedLen, uint8 *pBuf )
{
  osalNvItemHdr_t hdr;

  hdr.id = 0xFFFF;
  hdr.len = alignedLen | OSAL_NV_INVALID_LEN_MARK;

  // Write the len portion of the header first
  writeWord(pg, offset + alignedLen, (uint8 *) &hdr, 1);

  // remove invalid len mark
  hdr.len &= ~OSAL_NV_INVALID_LEN_MARK;
  writeWord(pg, offset + alignedLen, (uint8 *) &hdr, 1);

  // Copy over the data, converting length to words.
  writeWord(pg, offset, pBuf, alignedLen / OSAL_NV_WORD_SIZE);

  // value is valid. Write header except for the most significant bit.
  hdr.id = id | OSAL_NV_INVALID_ID_MARK;
  writeWord(pg, offset + alignedLen, (uint8 *) &hdr, 1);

  // write the most significant bit
  hdr.id &= ~OSAL_NV_INVALID_ID_MARK;
  writeWord(pg, offset + alignedLen, (uint8 *) &hdr, 1);
}

/*********************************************************************
 * @fn      xferItem
 *
 * @brief   Copy an NV item from the active page to a designated page.
 *
 * @param   pg         - NV page where to copy the item to.
 * @param   offset     - NV page offset where to copy the item to.
 * @param   alignedLen - Length of data to write, aligned in flash word
 *                       boundary.
 * @param   srcOff     - NV page offset of the original data in active page
 *
 * @return  none.
 */
static void xferItem( uint8 pg, uint16 offset, uint16 alignedLen, uint16 srcOff )
{
  uint8 tmp[OSAL_NV_WORD_SIZE];
  uint16 i = 0;

  // Copy over the data
  while (i <= alignedLen)
  {
    HalFlashRead(activePg, srcOff + i, tmp, OSAL_NV_WORD_SIZE);

    writeWord(pg, offset + i, tmp, 1);

    i += OSAL_NV_WORD_SIZE;
  }
}

/*********************************************************************
 * @fn      compactPage
 *
 * @brief   Compacts the page specified.
 *
 * @param   srcPg - Valid NV page to compact from.
 *                  The page must have changed its state (header) to xfer state
 *                  prior to this function call. This function will not
 *                  modify the state of its header to xfer state before starting
 *                  to compact.
 *
 * @return  none.
 */
static void compactPage( uint8 srcPg )
{
  uint16 srcOff, dstOff;
  uint8 dstPg;
  osalSnvId_t lastId = (osalSnvId_t) 0xFFFF;

  dstPg = (srcPg == nvPageBeg)? nvPageEnd : nvPageBeg;

  dstOff = OSAL_NV_PAGE_HDR_SIZE;

  // Read from the latest value
  srcOff = pgOff - sizeof(osalNvItemHdr_t);

  while (srcOff >= OSAL_NV_PAGE_HDR_SIZE)
  {
    osalNvItemHdr_t hdr;

    if (failW)
    {
      // Failure during transfer item will make next findItem error prone.
      return;
    }

    HalFlashRead(srcPg, srcOff, (uint8 *) &hdr, OSAL_NV_WORD_SIZE);

    if (hdr.id == 0xFFFF)
    {
      // Invalid entry. Skip this one.
      if (hdr.len & OSAL_NV_INVALID_LEN_MARK)
      {
        srcOff -= OSAL_NV_WORD_SIZE;
      }
      else
      {
        if (hdr.len + OSAL_NV_WORD_SIZE <= srcOff)
        {
          srcOff -= hdr.len + OSAL_NV_WORD_SIZE;
        }
        else
        {
          // invalid length. Source page must be a corrupt page.
          // This is possible only if the NV initialization failed upon erasing
          // what is selected as active page.
          // This is supposed to be a very rare case, as power should be
          // shutdown exactly during erase and then the page header is
          // still retained as either the Xfer or the Active state.

          // For production code, it might be useful to attempt to erase the page
          // so that at next power cycle at least the device is runnable
          // (with all entries removed).
          // However, it might be still better not to attempt erasing the page
          // just to see if this very rare case actually happened.
          //erasePage(srcPg);

          HAL_ASSERT_FORCED();
          return;
        }
      }

      continue;
    }

    // Consider only valid item
    if (!(hdr.id & OSAL_NV_INVALID_ID_MARK) && hdr.id != lastId)
    {
      // lastId is used to speed up compacting in case the same item ID
      // items were neighboring each other contiguously.
      lastId = (osalSnvId_t) hdr.id;

      // Check if the latest value of the item was already written
      if (findItem(dstPg, dstOff, lastId) == 0)
      {
        // This item was not copied over yet.
        // This must be the latest value.
        // Write the latest value to the destination page

        xferItem(dstPg, dstOff, hdr.len, srcOff - hdr.len);

        dstOff += hdr.len + OSAL_NV_WORD_SIZE;
      }
    }
    srcOff -= hdr.len + OSAL_NV_WORD_SIZE;
  }

  // All items copied.
  // Activate the new page
  setActivePage(dstPg);

  if (failW)
  {
    // Something bad happened when trying to compact the page
    return;
  }

  pgOff = dstOff; // update active page offset

  // Erase the currently active page
  erasePage(srcPg);
}

/*********************************************************************
 * @fn      verifyWordM
 *
 * @brief   verify the data written can be read back and is consistent.
 *
 * @param   pg     - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   pBuf   - Pointer to source buffer.
 * @param   cnt    - Number of 4-byte blocks to verify.
 *
 * @return  none
 */
static void verifyWordM( uint8 pg, uint16 offset, uint8 *pBuf, osalSnvLen_t cnt )
{
  uint8 tmp[OSAL_NV_WORD_SIZE];

  while (cnt--)
  {
    // Reading byte per byte will reduce code size but will slow down
    // and not sure it will meet the timing requirements.
    HalFlashRead(pg, offset, tmp, OSAL_NV_WORD_SIZE);

    if (FALSE == osal_memcmp(tmp, pBuf, OSAL_NV_WORD_SIZE))
    {
      // Read-back failed, NV is not safe to use.
      failF = failW = TRUE;
      return;
    }

    offset += OSAL_NV_WORD_SIZE;
    pBuf += OSAL_NV_WORD_SIZE;
  }
}

/*********************************************************************
 * @fn      writeWord
 *
 * @brief   Writes data to NV.
 *
 * @param   pg     - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   buf    - Pointer to source buffer.
 * @param   cnt    - Number of 4-byte blocks to write.
 *
 * @return  none
 */
static void writeWord( uint8 pg, uint16 offset, uint8 *buf, osalSnvLen_t cnt )
{
  if ( !failF && SNV_CHECK_VOLTAGE() == TRUE )
  {
    uint32 addr;
    halIntState_t cs;
    uint8 state;

    // Enter Critical Section
    HAL_ENTER_CRITICAL_SECTION(cs);

    // Disable Cache
    state = disableCache();

    // Write data.
    addr = (uint32) HalFlashGetAddress(pg, offset);
    HalFlashWrite(addr, buf, cnt * (OSAL_NV_WORD_SIZE) );

    // Enable cache.
    enableCache(state);

    verifyWordM(pg, offset, buf, cnt);

    HAL_EXIT_CRITICAL_SECTION(cs);
  }
}
#endif // !NO_OSAL_SNV

/*********************************************************************
 * @fn      osal_snv_init
 *
 * @brief   Initialize NV service.
 *
 * @return  SUCCESS if initialization succeeds. FAILURE, otherwise.
 */
uint8 osal_snv_init( void )
{
#if !defined( NO_OSAL_SNV )
  uint8 stat = SUCCESS;

  // get the first and last page of SNV
  // Note: Can also get this information from the FCFG; done this way to stay
  //       consistent with the flash addresses obtained from the Linker.
  //       nvPages    = *((uint8 *)(FCFG1_BASE + FCFG_FLASH_SIZE_OFFSET));
  //       nvPageBeg  = nvPages - (OSAL_NV_PAGES_USED + CCFG_PAGES_USED);
  //       nvPageEnd  = nvPageBeg + OSAL_NV_PAGES_USED - 1;
  // Where FCFG_FLASH_SIZE_OFFSET = 0x2B1, and CCFG_PAGES_USED = 1.
  nvPageBeg = (uint32)SNV_FLASH / OSAL_NV_PAGE_SIZE; //(uint32)OSAL_NV_PAGE_BEGIN;
  nvPageEnd = nvPageBeg + OSAL_NV_PAGES_USED - 1;

  if (!initNV())
  {
    // NV initialization failed
    HAL_ASSERT_FORCED();

    stat = FAILURE;
  }

  return stat;

#else // NO_OSAL_SNV
  // NV initialization failed
  return( FAILURE );
#endif // !NO_OSAL_SNV
}

/*********************************************************************
 * @fn      osal_snv_write
 *
 * @brief   Write a data item to NV.
 *
 * @param   id    - Valid NV item Id.
 * @param   len   - Length of data to write.
 * @param   *pBuf - Data to write.
 *
 * @return  SUCCESS if successful, NV_OPER_FAILED if failed.
 */
uint8 osal_snv_write( osalSnvId_t id, osalSnvLen_t len, void *pBuf )
{
#if !defined( NO_OSAL_SNV )
  uint16 alignedLen;

  // Reset failW
  failW = FALSE;

  {
    uint16 offset = findItem(activePg, pgOff, id);

    if (offset > 0)
    {
      uint8 tmp;
      osalSnvLen_t i;

      for (i = 0; i < len; i++)
      {
        HalFlashRead(activePg, offset, &tmp, 1);
        if (tmp != ((uint8 *)pBuf)[i])
        {
          break;
        }
        offset++;
      }

      if (i == len)
      {
        // Changed value is the same value as before.
        // Return here instead of re-writing the same value to NV.
        return SUCCESS;
      }
    }
  }

  alignedLen = ((len + OSAL_NV_WORD_SIZE - 1) / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE;

  if ( pgOff + alignedLen + OSAL_NV_WORD_SIZE > OSAL_NV_PAGE_SIZE )
  {
    setXferPage();
    compactPage(activePg);
  }

  // pBuf shall be referenced beyond its valid length to save code size.
  writeItem(activePg, pgOff, id, alignedLen, pBuf);

  if (failW)
  {
    return NV_OPER_FAILED;
  }

  pgOff += alignedLen + OSAL_NV_WORD_SIZE;

  return SUCCESS;
#else

  return NV_OPER_FAILED;
#endif // !NO_OSAL_SNV
}

/*********************************************************************
 * @fn      osal_snv_read
 *
 * @brief   Read data from NV.
 *
 * @param   id    - Valid NV item Id.
 * @param   len   - Length of data to read.
 * @param   *pBuf - Data is read into this buffer.
 *
 * @return  SUCCESS if successful.
 *          Otherwise, NV_OPER_FAILED for failure.
 */
uint8 osal_snv_read( osalSnvId_t id, osalSnvLen_t len, void *pBuf )
{
#if !defined( NO_OSAL_SNV )
  uint16 offset = findItem(activePg, pgOff, id);

  if (offset != 0)
  {
    HalFlashRead(activePg, offset, pBuf, len);

    return SUCCESS;
  }
#endif // !NO_OSAL_SNV

  return NV_OPER_FAILED;
}

/*********************************************************************
 * @fn      osal_snv_compact
 *
 * @brief   Compacts NV if its usage has reached a specific threshold.
 *
 * @param   threshold - compaction threshold
 *
 * @return  SUCCESS if successful,
 *          NV_OPER_FAILED if failed, or
 *          INVALIDPARAMETER if threshold invalid.
 */
uint8 osal_snv_compact( uint8 threshold )
{
#if !defined( NO_OSAL_SNV )
  if ( ( threshold < OSAL_NV_MIN_COMPACT_THRESHOLD ) ||
       ( threshold > OSAL_NV_MAX_COMPACT_THRESHOLD ) )
  {
    return INVALIDPARAMETER;
  }

  // See if NV active page usage has reached compaction threshold
  if ( !failF &&
       ( (uint32)pgOff * 100 ) >= ( OSAL_NV_PAGE_SIZE * (uint32)threshold ) )
  {
    // Reset failW
    failW = FALSE;

    setXferPage();

    compactPage(activePg);

    return SUCCESS;
  }
#endif // !NO_OSAL_SNV

  return NV_OPER_FAILED;
}


/*********************************************************************
*********************************************************************/
