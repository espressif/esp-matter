/***************************************************************************//**
 * @file
 * @brief Application code
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include "app.h"
#include "sl_wisun_api.h"
#ifdef SL_CATALOG_WISUN_APP_CORE_PRESENT
  #include "sl_wisun_app_core.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
#ifndef SL_CATALOG_WISUN_EVENT_MGR_PRESENT // Event Manager also defines this handler
/*Wi-SUN event handler*/
void sl_wisun_on_event(sl_wisun_evt_t *evt)
{
  (void) evt->header.id;

  /////////////////////////////////////////////////////////////////////////////
  // Put your Wi-SUN event handling here!                                    //
  // This is called from Wi-SUN stack.                                       //
  // Do not call blocking functions from here!                               //
  // Protect your data during event handling!                                //
  /////////////////////////////////////////////////////////////////////////////
}
#endif

/*App task function*/
void app_task(void *args)
{
  (void) args;

  printf("\nWi-SUN Empty application\n");

#ifdef SL_CATALOG_WISUN_APP_CORE_PRESENT
  // connect to the wisun network
  app_wisun_connect_and_wait();
#endif

  while (1) {
    ///////////////////////////////////////////////////////////////////////////
    // Put your application code here!                                       //
    ///////////////////////////////////////////////////////////////////////////
    osDelay(1);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
