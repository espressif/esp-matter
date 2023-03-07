/***************************************************************************//**
 * @file
 * @brief Simulated EEPROM system for wear leveling token storage across flash.
 * See @ref simeepromcom for documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef __SIM_EEPROM_H__
#define __SIM_EEPROM_H__

//pull in the platform specific information here
#if defined(CORTEXM3) || defined(EMBER_TEST) || defined(TOKEN_MANAGER_TEST)
  #include "sl_sim_eeprom_cortexm3.h"
#else
  #error invalid sim_eeprom platform
#endif

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

/** @addtogroup simeepromcom Simulated EEPROM 1 and 2
 * @brief Simulated EEPROM 1 and 2 legacy storage.
 *
 * The Simulated EEPROM 1 and 2 system (typically referred to as SimEE) is designed to
 * operate under the @ref token_manager
 * API and provide a non-volatile storage system.  For this reason, the SimEE1
 * and SimEE2 components do not have any public functions to call.
 *
 * @note SimEE is a legacy storage
 * system so this component exists to enable the modern @ref token_manager to
 * access existing devices that already have data stored in SimEE.
 * Since the flash write cycles
 * are finite, the Simulated EEPROM's primary purpose is to perform wear
 * leveling across several hardware flash pages, ultimately increasing the
 * number of times tokens may be written before a hardware failure.
 *
 * SimEE1 is designed to consume 8kB of
 * upper flash within which it will perform wear leveling.
 * SimEE2 is designed to consume 36kB of
 * upper flash within which it will perform wear leveling.
 *
 * The Simulated EEPROM needs to periodically perform a page erase
 * operation to recover storage area for future token writes.  The page
 * erase operation requires an ATOMIC block of typically 21ms.  Since this is such a long
 * time to not be able to service any interrupts, the page erase operation is
 * under application control providing the application the opportunity to
 * decide when to perform the operation and complete any special handling
 * needed that might be needed.
 *
 * @note The best, safest, and recommended practice is for the application
 * to regularly and always perfom a page erase when the
 * application can expect and deal with the page erase delay.
 * Page erase will immediately return if there is nothing to
 * erase.  If there is something that needs to be erased, doing so
 * as regularly and as soon as possible will keep the SimEE in the
 * healthiest state possible.
 *
 *@{
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Note to reader:  this paragraph was removed from Doxygen since SimEE is
// legacy, should not be used, and provides informaton that is inconcistent
// with the Token Manager.
/* ::ERASE_CRITICAL_THRESHOLD is the metric the freePtr is compared against.
 * This metric is set to about 3/4 full.
 * The freePtr is a marker used internally by the Simulated EEPROM to track
 * where data ends and where available write space begins. If the freePtr
 * crosses this threhold, ::halSimEepromCallback() will be called with an
 * EmberStatus of ::EMBER_SIM_EEPROM_ERASE_PAGE_RED, indicating a critical
 * need for the application to call ::halSimEepromErasePage() which will erase a
 * hardware page and provide fresh storage for the Simulated EEPROM to write
 * token data.  If freePtr is less than the threshold, the callback will
 * have an EmberStatus of ::EMBER_SIM_EEPROM_ERASE_PAGE_GREEN indicating the
 * application should call ::halSimEepromErasePage() at its earliest convenience,
 * but doing so is not critically important at this time.
 */
#endif // DOXYGEN_SHOULD_SKIP_THIS

#ifndef DOXYGEN_SHOULD_SKIP_THIS
//Prototype for internal Startup for the public Init and Repair macros
EmberStatus halInternalSimEeStartup(bool forceRebuildAll);

//Properly round values when converting bytes to words
#define BYTES_TO_WORDS(x) (((x) + 1) / 2)

//application functions

/** @brief The Simulated EEPROM callback function, implemented by the
 * application.
 *
 * @param status  An ::EmberStatus error code indicating one of the conditions
 * described below.
 *
 * This callback will report an EmberStatus of
 * ::EMBER_SIM_EEPROM_ERASE_PAGE_GREEN whenever a token is set and a page needs
 * to be erased.  If the main application loop does not periodically
 * call halSimEepromErasePage(), it is best to then erase a page in
 * response to ::EMBER_SIM_EEPROM_ERASE_PAGE_GREEN.
 *
 * This callback will report an EmberStatus of ::EMBER_SIM_EEPROM_ERASE_PAGE_RED
 * when the pages <i>must</i> be erased to prevent data loss.
 * halSimEepromErasePage() needs to be called until it returns 0 to indicate
 * there are no more pages that need to be erased.  Ignoring
 * this indication and not erasing the pages will cause dropping the new data
 * trying to be written.
 *
 * This callback will report an EmberStatus of ::EMBER_SIM_EEPROM_FULL when
 * the new data cannot be written due to unerased pages.  <b>Not erasing
 * pages regularly, not erasing in response to
 * ::EMBER_SIM_EEPROM_ERASE_PAGE_GREEN, or not erasing in response to
 * ::EMBER_SIM_EEPROM_ERASE_PAGE_RED will cause
 * ::EMBER_SIM_EEPROM_FULL and the new data will be lost!.</b>  Any future
 * write attempts will be lost as well.
 *
 * This callback will report an EmberStatus of ::EMBER_SIM_EEPROM_REPAIRING
 * when the Simulated EEPROM needs to repair itself.  While there's nothing
 * for an app to do when the SimEE is going to repair itself (SimEE has to
 * be fully functional for the rest of the system to work), alert the
 * application to the fact that repairing is occuring.  There are debugging
 * scenarios where an app might want to know that repairing is happening;
 * such as monitoring frequency.
 * @note  Common situations will trigger an expected repair, such as using
 *        a new chip or changing token definitions.
 *
 * If the callback ever reports the status ::EMBER_ERR_FLASH_WRITE_INHIBITED or
 * ::EMBER_ERR_FLASH_VERIFY_FAILED, this indicates a catastrophic failure in
 * flash writing, meaning either the address being written is not empty or the
 * write itself has failed.  If ::EMBER_ERR_FLASH_WRITE_INHIBITED is
 * encountered, the function ::halInternalSimEeRepair(false) should be called
 * and the chip should then be reset to allow proper initialization to recover.
 * If ::EMBER_ERR_FLASH_VERIFY_FAILED is encountered the Simulated EEPROM (and
 * tokens) on the specific chip with this error should not be trusted anymore.
 *
 */
void halSimEepromCallback(EmberStatus status);

/** @brief Erases a hardware flash page, if needed.
 *
 * This function can be
 * called at anytime from anywhere in the application (except ISRs) and will
 * only take effect
 * if needed (otherwise it will return immediately).  Since this function takes
 * 21ms to erase a hardware page during which interrupts cannot be serviced,
 * it is preferable to call this function while in a state that can withstand
 * being unresponsive for so long.  The Simulated EEPROM will periodically
 * request through the ::halSimEepromCallback() that a page be erased.  The
 * Simulated EEPROM will never erase a page (which could result in data loss)
 * and relies entirely on the application to call this function to approve
 * a page erase (only one erase per call to this function).
 *
 * The Simulated EEPROM depends on the ability to move between two Virtual
 * Pages, which are comprised of multiple hardware pages.  Before moving to the
 * unused Virtual Page, all hardware pages comprising the unused Virtual Page
 * must be erased first.  The erase time of a hardware flash page is 21ms.
 * During this time the chip will be unresponsive and unable to service an
 * interrupt or execute any code (due to the flash being unavailable during
 * the erase procedure).  This function is used to trigger a page erase.
 *
 * @return  A count of how many hardware pages are left to be erased.  This
 * return value allows for calling code to easily loop over this function
 * until the function returns 0.
 */
uint8_t halSimEepromErasePage(void);

/** @brief Get count of pages to be erased.
 *
 * This function returns the same value halSimEepromErasePage() would return,
 * but without modifying/erasing any flash.
 *
 * @return  A count of how many hardware pages are left to be erased.  This
 * code assist with loops wanting to know how much is left to erase.
 */
uint8_t halSimEepromPagesRemainingToBeErased(void);

/** @brief Provides two basic statistics.
 *  - The number of unused words until SimEE is full
 *  - The total page use count
 *
 * There is a lot
 * of management and state processing involved with the Simulated EEPROM,
 * and most of it has no practical purpose in the application.  These two
 * parameters provide a simple metric for knowing how soon the Simulated
 * EEPROM will be full (::freeWordsUntilFull)
 * and how many times (approximatly) SimEE has rotated pysical flash
 * pages (::totalPageUseCount).
 *
 * @param freeWordsUntilFull  Number of unused words
 * available to SimEE until the SimEE is full and would trigger an
 * ::EMBER_SIM_EEPROM_ERASE_PAGE_RED then ::EMBER_SIM_EEPROM_FULL callback.
 *
 * @param totalPageUseCount  The value of the highest page counter
 * indicating how many times the Simulated EEPROM
 * has rotated physical flash pages (and approximate write cycles).
 */
void halSimEepromStatus(uint16_t *freeWordsUntilFull, uint16_t *totalPageUseCount);

#endif // DOXYGEN_SHOULD_SKIP_THIS

#ifndef DOXYGEN_SHOULD_SKIP_THIS

//NOTE: halInternal functions must not be called directly.

/** @brief  Initializes the Simulated EEPROM system.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameters.
 *
 * This function must
 * be called before any non-manufacturing token or SimEE access.
 * This function scans the Virtual Pages, verifies the Page Management,
 * compares the stored token
 * definitions versus the compile time definitions for differences, and walks
 * the Virtual Page(s) (starting at the Base Tokens) building up the RAM Cache.
 * If any discrepancies or errors are found in the management information, this
 * function will automatically trigger halInternalSimEeRepair() which will
 * fix the errors while recovering as much data as possible.
 *
 * Since the SimEE is dependant on the RAM Cache for operation, this function
 * must be called first.  Since this function builds up the RAM Cache and the
 * RAM Cache is always kept up to date, it is not harmful to call this
 * function multiple times.
 *
 * @return   An ::EMBER_SUCCESS if the initialization succeeds, and
 * ::EMBER_ERR_FATAL if a write to flash is both required and fails.
 * halSimEepromCallback() is automatically called if a write fails.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus halInternalSimEeInit(void);

#else
#define halInternalSimEeInit() (halInternalSimEeStartup(false))
#endif

/** @brief Gets the token data.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameters.
 *
 * The Simulated EEPROM uses a RAM Cache
 * to hold the current state of the Simulated EEPROM, including the location
 * of the freshest token data.  The GetData function simply uses the ID and
 * index parameters to access the pointer RAM Cache.  The flash absolute
 * address stored in the pointer RAM Cache is extracted and passed
 * along with the data and length parameter to the flash read function.
 *
 * @param vdata  A pointer to where the data being read should be placed.
 *
 * @param compileId    The ID of the token to get data from.  Since the token
 * system is designed to enumerate the token names down to a simple 8 bit ID,
 * the TOKEN_name is the parameter used when invoking this function.
 *
 * @param index If token being accessed is indexed, this parameter is
 * combined with the ID to formulate the RAM Cache lookup.  If the token
 * is not an indexed token, this parameter is ignored.
 *
 * @param len   The number of bytes being worked on.  This should always be
 * the size of the token (available from both the sizeof() intrinsic as well
 * as the tokenSize[] array).
 *
 */
void halInternalSimEeGetData(void *vdata,
                             uint8_t compileId,
                             uint8_t index,
                             uint8_t len);

/** @brief Sets token data.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameters.
 *
 * Like GetData, the passed parameters are
 * used to access into the RAM Cache.  Once the
 * basic token and SimEE parameters involved are stored locally, SetData
 * triggers the halSimEepromCallback, if needed, indicating the state of
 * erased pages
 *
 * If there is enough room to store the data, the data will be written
 * into the next available location.
 *
 * If there is not enough room to store the data, the SimEE callback will
 * be called with a status code indicating the severity of the situation.
 * If there is not enough room and the application does not erase pages,
 * the data would be lost.
 *
 * As the current Virtual Page fills, existing data will be moved to the
 * next Vitual Page and the new data will be written to the next
 * Vitual Page.  When the next Virtual Page has all the active date,
 * the current Virtual Page will be marked for erasure and the next
 * Virtual Page becomes the current Virtual Page.
 *
 * @param compileId    The ID of the token to set data for.  Since the token
 * system is designed to enumerate the token names down to a simple 8 bit ID,
 * generally the TOKEN_name is the parameter used when invoking this function.
 *
 * @param vdata  A pointer to the data that should be written.
 *
 * @param index If token being accessed is indexed, this parameter is
 * combined with the ID to formulate the RAM Cache lookup.  If the token
 * is not an indexed token, this parameter is ignored.
 *
 * @param len   The number of bytes being worked on.  This should always be
 * the size of the token (available from both the sizeof() intrinsic as well
 * as the tokenSize[] array).
 *
 */
void halInternalSimEeSetData(uint8_t compileId,
                             void *vdata,
                             uint8_t index,
                             uint8_t len);

/** @brief Increments the value of a token that is a counter.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameter.
 *
 * This is more efficient than just setting the token because the
 * SimEE stores just a +1 mark which takes up less space and improves
 * write cycles.
 *
 * The padding size if 50 bytes (25 words).  When this padding is full of +1's,
 * the next increment call will trigger a full SetData that moves the total
 * value to a new location and begins again.  Because the full SetData
 * function is triggered, this will handle switching pages as well.
 *
 * When the SimEE switches to a new page, it will automatically truncate the
 * +1 marks into a new number and write that number into the new page.
 *
 * @note This function will only work on tokens that have the IsCnt flag
 * in their TOKEN_DEF set and only on tokens that are simple scalars (uint32_t).
 *
 * @param compileId  The ID of the token to increment.  Since the token system
 * is designed to enumerate the token names down to a simple 8 bit ID,
 * generally the TOKEN_name is the parameter used when invoking this function.
 *
 */
void halInternalSimEeIncrementCounter(uint8_t compileId);

/** @brief Repairs the Simulated EEPROM.
 *
 * @note There is no public version of the this functon since this
 * function is not intended for normal application use.
 *
 * This function is automatically
 * triggered if there are any problems found.  This function can also be
 * triggered manually by test functions to forcefully rebuild the Simulated
 * EEPROM.
 *
 * The Virtual Pages' management and token definitions are examined and
 * then the token data is scanned to find as much recoverable data as possible.
 * If a token is
 * found intact, it is copied over to a fresh page.  If a token is not found,
 * or does not match the compiled definition of the token, the compiled token
 * is written, with its default values, to the fresh page.  Any tokens that
 * exist in the old Virtual Page(s) and do not exist in the compiled definitions
 * are simply ignored.  Once all tokens have had their management information,
 * stored definitions, and data writen to the new page, the Page Management
 * information is written.  The next time halInternalSimEeInit is called, the
 * new, fresh, valid Virtual Page will be used.
 *
 * @param forceRebuildAll  If true, this repair function will erase the entire
 * Simulated EEPROM and reload it all from defaults.  If false, the repair
 * function will operate normally.
 *
 * @return   An ::EMBER_SUCCESS if the repair succeeds, and
 * ::EMBER_ERR_FATAL if a write to flash fails.  halSimEepromCallback() is
 * automatically called if a write fails.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus halInternalSimEeRepair(bool forceRebuildAll);

#else
#define halInternalSimEeRepair(rebuild) (halInternalSimEeStartup(rebuild))
#endif

#endif //DOXYGEN_SHOULD_SKIP_THIS

#endif //__SIM_EEPROM_H__

/** @} (end addtogroup simeepromcom) */

/** @} (end addtogroup legacyhal) */
