/***************************************************************************//**
 * @file network_management.c
 * @brief network_management.c
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "ZW_application_transport_interface.h"
#include <ZAF_Common_interface.h>
#include <ZW_classcmd.h>
#include <EventDistributor.h>
#include <ZAF_Common_interface.h>
#include <ZAF_PM_Wrapper.h>
#include <ZW_controller_api.h>
#include <SizeOf.h>
#include <AppTimer.h>
#include <ZW_system_startup_api.h>
#include <em_emu.h>
#include <ZW_controller_api.h>
#include <config_app.h>
#include <zw_config_rf.h>
#include <events.h>
#include <zaf_event_helper.h>
#include <zaf_job_helper.h>
#include "network_management.h"

//#define DEBUGPRINT
#include <DebugPrint.h>

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
static sl_network_management_states_t network_management_state = NETWORK_MANAGEMENT_STATE_IDLE;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
bool key_fob_start_inclusion()
{
  const SApplicationHandles* pAppHandle = ZAF_getAppHandle();
  EQueueNotifyingStatus status;
  SZwaveCommandPackage learnModeStart = {
    .eCommandType = EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_ADD_NODE
  };
  // Put the Command on queue (and dont wait for it, queue must be empty)
  status = QueueNotifyingSendToBack(pAppHandle->pZwCommandQueue, (uint8_t *)&learnModeStart, 0);
  if (EQUEUENOTIFYING_STATUS_SUCCESS == status)
  {
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_START_INCLUSION);
    return true;
  }
  return false;
}

bool key_fob_stop_inclusion()
{
  const SApplicationHandles* pAppHandle = ZAF_getAppHandle();
  EQueueNotifyingStatus status;
  SZwaveCommandPackage learnModeStart = {
    .eCommandType = EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_ABORT
  };
  // Put the Command on queue (and dont wait for it, queue must be empty)
  status = QueueNotifyingSendToBack(pAppHandle->pZwCommandQueue, (uint8_t *)&learnModeStart, 0);
  if (EQUEUENOTIFYING_STATUS_SUCCESS == status)
  {
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_IDLE);
    return true;
  }
  return false;
}

bool key_fob_start_exclusion()
{
  const SApplicationHandles* pAppHandle = ZAF_getAppHandle();
  EQueueNotifyingStatus status;
  SZwaveCommandPackage learnModeStart = {
    .eCommandType = EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_REMOVE_NODE
  };
  // Put the Command on queue (and dont wait for it, queue must be empty)
  status = QueueNotifyingSendToBack(pAppHandle->pZwCommandQueue, (uint8_t *)&learnModeStart, 0);
  if (EQUEUENOTIFYING_STATUS_SUCCESS == status)
  {
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_START_EXCLUSION);
    return true;
  }
  return false;
}

bool key_fob_stop_exclusion()
{
  const SApplicationHandles* pAppHandle = ZAF_getAppHandle();
  EQueueNotifyingStatus status;
  SZwaveCommandPackage learnModeStart = {
    .eCommandType = EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_ABORT
  };
  // Put the Command on queue (and don't wait for it, queue must be empty)
  status = QueueNotifyingSendToBack(pAppHandle->pZwCommandQueue, (uint8_t *)&learnModeStart, 0);
  if (EQUEUENOTIFYING_STATUS_SUCCESS == status)
  {
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_IDLE);
    return true;
  }
  return false;
}

bool key_fob_start_learnmode_include()
{
  const SApplicationHandles* pAppHandle = ZAF_getAppHandle();
  EQueueNotifyingStatus status;
  SZwaveCommandPackage learnModeStart = {
    .eCommandType = EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_LEARN_MODE_INCLUSION
  };
  // Put the Command on queue (and dont wait for it, queue must be empty)
  status = QueueNotifyingSendToBack(pAppHandle->pZwCommandQueue, (uint8_t *)&learnModeStart, 0);
  if (EQUEUENOTIFYING_STATUS_SUCCESS == status)
  {
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_LEARNMODE);
    return true;
  }
  return false;
}

bool key_fob_start_learnmode_exclude()
{
  const SApplicationHandles* pAppHandle = ZAF_getAppHandle();
  EQueueNotifyingStatus status;
  SZwaveCommandPackage learnModeStart = {
    .eCommandType = EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_LEARN_MODE_EXCLUSION
  };
  // Put the Command on queue (and dont wait for it, queue must be empty)
  status = QueueNotifyingSendToBack(pAppHandle->pZwCommandQueue, (uint8_t *)&learnModeStart, 0);
  if (EQUEUENOTIFYING_STATUS_SUCCESS == status)
  {
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_LEARNMODE);
    return true;
  }
  return false;
}

bool set_new_network_management_state( sl_network_management_states_t new_state)
{
  network_management_state = new_state;
  return true;
}

sl_network_management_states_t get_current_network_management_state()
{
  return network_management_state;
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

