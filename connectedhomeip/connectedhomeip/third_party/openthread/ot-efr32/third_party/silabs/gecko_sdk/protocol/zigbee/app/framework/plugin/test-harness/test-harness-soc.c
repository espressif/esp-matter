/***************************************************************************//**
 * @file
 * @brief Test harness code specific to the SOC.
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

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
#include "app/framework/util/util.h"

#include "test-harness.h"

//------------------------------------------------------------------------------
// Globals

//------------------------------------------------------------------------------
// Forward Declarations

// Internal stack routines
void emResetApsFrameCounter(void);
void emTestHarnessAdvanceApsFrameCounter(void);

// This adds very verbose printing that is useful only for debugging the
// hash calculation
//#define PRINT_INTERMEDIATE_HASH_RESULTS

//------------------------------------------------------------------------------
// Functions

void emAfTestHarnessResetApsFrameCounter(void)
{
  // Reseting the outgoing APS frame counter is non-standard and not
  // a good idea, especially on the TC.  This is necessary for Smart Energy
  // Key establihsment tests 15.39 and 15.40.  It is only necessary for a test
  // harness device.
  emResetApsFrameCounter();
}

void emAfTestHarnessAdvanceApsFrameCounter(void)
{
  emTestHarnessAdvanceApsFrameCounter();
}

//------------------------------------------------------------------------------
