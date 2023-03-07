/**
 * @file
 * Handler for Command Class Device Reset Locally.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <CC_DeviceResetLocally.h>
#include <ZW_TransportEndpoint.h>
#include <ZAF_Common_interface.h>
#include <CC_Common.h>

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                               FUNCTIONS                                  */
/****************************************************************************/
static uint8_t 
lifeline_reporting(ccc_pair_t * p_ccc_pair)
{
  p_ccc_pair->cmdClass = COMMAND_CLASS_DEVICE_RESET_LOCALLY;
  p_ccc_pair->cmd      = DEVICE_RESET_LOCALLY_NOTIFICATION;
  return 1;
}

void 
CC_DeviceResetLocally_notification_tx(void)
{
  const AGI_PROFILE lifelineProfile = {
      ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL,
      ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL_LIFELINE
  };
    
  transmission_result_t transmissionResult = {
    .nodeId = 0,
    .status = false,
    .isFinished = TRANSMISSION_RESULT_FINISHED,
  };

  if(!ZAF_GetNodeID())
  {
    CC_DeviceResetLocally_done(&transmissionResult);
  }
  else
  {
    cc_group_t cmdGrp = { 
      COMMAND_CLASS_DEVICE_RESET_LOCALLY, 
      DEVICE_RESET_LOCALLY_NOTIFICATION
    };

    if(JOB_STATUS_SUCCESS != cc_engine_multicast_request(
        &lifelineProfile,
        ENDPOINT_ROOT,
        &cmdGrp,
        NULL,
        0,
        false,
        CC_DeviceResetLocally_done))
    {
      CC_DeviceResetLocally_done(&transmissionResult);
    }
  }
}

REGISTER_CC_V3(COMMAND_CLASS_DEVICE_RESET_LOCALLY, DEVICE_RESET_LOCALLY_VERSION, NULL, NULL, NULL, lifeline_reporting, 0);
