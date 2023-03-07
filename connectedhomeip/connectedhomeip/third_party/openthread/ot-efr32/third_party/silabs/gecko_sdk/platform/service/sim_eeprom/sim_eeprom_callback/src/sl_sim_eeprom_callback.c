/***************************************************************************//**
 * @file
 * @brief Simulated EEPROM Callback.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include PLATFORM_HEADER
#include "stack/config/ember-configuration-defaults.h"
#include "error-def.h"
#include "hal/hal.h"
#include "sl_sim_eeprom.h"
#include "sl_token_manager.h"

#ifdef TOKEN_MANAGER_TEST
// This test checks to make sure halSimEepromCallback() is triggered after
// the first token init on an erased flash which will induce a repair.
uint8_t simee_callback_fired = 0;
#endif // TOKEN_MANAGER_TEST

void halSimEepromCallback(EmberStatus status)
{
#ifdef TOKEN_MANAGER_TEST
  simee_callback_fired++;
#endif // TOKEN_MANAGER_TEST

  switch (status) {
    case EMBER_SIM_EEPROM_ERASE_PAGE_GREEN:
      //SimEE is asking for one page to be erased.
      (void)halSimEepromErasePage();
      break;
    case EMBER_SIM_EEPROM_ERASE_PAGE_RED:
    case EMBER_SIM_EEPROM_FULL:
    { //SimEE says we're out of room!  Erase all pages now or data
      //currently being written will be dropped.
      if (halSimEepromPagesRemainingToBeErased() > 0) {
        //If there is at least one HW page to erase, loop over ErasePage until
        //all HW pages are erased.  Without this explicit check before
        //ErasePage, when halSimEepromErasePage() returns 0 the code wont
        //know if a page was actually erased or not and properly break out.
        while (halSimEepromErasePage() != 0U) {
        }
        // A fresh virtual page now exists to use
        break;
      }
      //If there are still pages to erase, then we have a situation where page
      //rotation is stuck because live tokens still exist in the
      //page we want to erase.  In this case we must do a repair to
      //get all live tokens into one virtual page. [BugzId:14392]
      //This bug pertains to SimEE2.
      //Fall into...
    }
    // fall through
    case EMBER_ERR_FLASH_WRITE_INHIBITED:
    case EMBER_ERR_FLASH_VERIFY_FAILED:
    { //Something went wrong while writing a token.  There is stale data and the
      //token the app expected to write did not get written.  Also there may
      //now be "stray" data written in the flash that could inhibit future token
      //writes.  To deal with stray/stale data, we must repair the Simulated
      //EEPROM.  Because the expected token write failed and will not be retried,
      //it is best to reset the chip and let normal boot sequences take over.
      //Since halInternalSimEeRepair() could potentially result in another write
      //failure, we use a simple semaphore to prevent recursion.
      static bool repairActive = false;
      if (!repairActive) {
        repairActive = true;
        halInternalSimEeRepair(false);
        switch (status) {
          case EMBER_SIM_EEPROM_ERASE_PAGE_RED:
          case EMBER_SIM_EEPROM_FULL:
            //Don't reboot - return to let SimEE code retry the token write
            //[BugzId:14392]
            break;
          case EMBER_ERR_FLASH_VERIFY_FAILED:
            //[[WBB350FIXME -It would be nice to have a unified, ]]
            //[[             platform independant reboot machanism]]
        #if defined (CORTEXM3)
            halInternalSysReset(RESET_FLASH_VERIFY);
        #else
            assert(0);
        #endif
            break;
          case EMBER_ERR_FLASH_WRITE_INHIBITED:
        #if defined (CORTEXM3)
            halInternalSysReset(RESET_FLASH_INHIBIT);
        #else
            assert(0);
        #endif
            break;
          default:
            assert(0);
            break;
        }
        repairActive = false;
      }
      break;
    }
    case EMBER_SIM_EEPROM_REPAIRING:
      // While there's nothing for an app to do when the SimEE is going to
      // repair itself (SimEE has to be fully functional for the rest of the
      // system to work), alert the application to the fact that repairing
      // is occuring.  There are debugging scenarios where an app might want
      // to know that repairing is happening; such as monitoring frequency.
      // NOTE:  Common situations will trigger an expected repair, such as
      //        using an erased chip or changing token definitions.
      break;
    default:
      // this condition indicates an unexpected problem.
      assert(0);
      break;
  }
}
