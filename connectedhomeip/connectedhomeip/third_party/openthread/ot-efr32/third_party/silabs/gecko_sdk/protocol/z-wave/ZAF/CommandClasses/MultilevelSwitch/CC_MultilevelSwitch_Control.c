/**
 * @file
 * Handler for Command Class Multilevel Switch.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <CC_MultilevelSwitch_Control.h>
#include <ZW_TransportLayer.h>
#include <agi.h>
#include "ZAF_tx_mutex.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef struct _multi_level_switch_set_t_
{
  uint8_t value;
  uint8_t dimmingDuration;
}multi_level_switch_set_t;


/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/
CMD_CLASS_GRP cmdGrp;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

JOB_STATUS
CmdClassMultilevelSwitchStartLevelChange(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult),
  CCMLS_PRIMARY_SWITCH_T primarySwitch,
  CCMLS_IGNORE_START_LEVEL_T fIgnoreStartLevel,
  CCMLS_SECONDARY_SWITCH_T secondarySwitch,
  uint8_t primarySwitchStartLevel,
  uint8_t duration,
  uint8_t secondarySwitchStepSize)
{
  ZW_APPLICATION_TX_BUFFER *pTxBuf = GetRequestBuffer(pCbFunc);
 if( IS_NULL( pTxBuf ) )
  {
    /*Ongoing job is active.. just stop current job*/
    return JOB_STATUS_BUSY;
  }
  else
  {
    TRANSMIT_OPTIONS_TYPE_EX* pTxOptionsEx = NULL;

    // ZW_classcmd.h:19423 -> ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V4_FRAME
    pTxBuf->ZW_SwitchMultilevelStartLevelChangeV4Frame.cmdClass = COMMAND_CLASS_SWITCH_MULTILEVEL_V4;
    pTxBuf->ZW_SwitchMultilevelStartLevelChangeV4Frame.cmd = SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V4;
    pTxBuf->ZW_SwitchMultilevelStartLevelChangeV4Frame.properties1 = (uint8_t)((primarySwitch << 6) | (fIgnoreStartLevel << 5) | (secondarySwitch << 3));
    pTxBuf->ZW_SwitchMultilevelStartLevelChangeV4Frame.startLevel = primarySwitchStartLevel;
    pTxBuf->ZW_SwitchMultilevelStartLevelChangeV4Frame.dimmingDuration = duration;
    pTxBuf->ZW_SwitchMultilevelStartLevelChangeV4Frame.stepSize = secondarySwitchStepSize;

    /*Get transmit options (node list)*/
    pTxOptionsEx = ReqNodeList( pProfile,
                          (CMD_CLASS_GRP*) &(pTxBuf->ZW_Common.cmdClass),
                          sourceEndpoint);
    if( IS_NULL( pTxOptionsEx ) )
    {
      /*Job failed, free transmit-buffer pTxBuf by cleaning mutex */
      FreeRequestBuffer();
      return JOB_STATUS_NO_DESTINATIONS;
    }

    if(ETRANSPORTMULTICAST_ADDED_TO_QUEUE != ZW_TransportMulticast_SendRequest(
        (uint8_t *)pTxBuf,
        sizeof(ZW_SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V4_FRAME),
        true, // Enable Supervision
        pTxOptionsEx,
        ZCB_RequestJobStatus))
    {
      /*Job failed, free transmit-buffer pTxBuf by cleaning mutex */
      FreeRequestBuffer();
     return JOB_STATUS_BUSY;
    }
  }
  return JOB_STATUS_SUCCESS;
}


JOB_STATUS
CmdClassMultilevelSwitchStopLevelChange(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult))
{
  cmdGrp.cmdClass = COMMAND_CLASS_SWITCH_MULTILEVEL_V4;
  cmdGrp.cmd = SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V4;

  return cc_engine_multicast_request(
      pProfile,
      sourceEndpoint,
      &cmdGrp,
      NULL,
      0,
      true,
      pCbFunc);
}


JOB_STATUS
CmdClassMultilevelSwitchSetTransmit(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult),
  uint8_t value,
  uint8_t duration)
{
  multi_level_switch_set_t multi_level_switch_set;
  multi_level_switch_set.value = value;
  multi_level_switch_set.dimmingDuration = duration;

  cmdGrp.cmdClass = COMMAND_CLASS_SWITCH_MULTILEVEL_V4;
  cmdGrp.cmd = SWITCH_MULTILEVEL_SET_V4;


  return cc_engine_multicast_request(
      pProfile,
      sourceEndpoint,
      &cmdGrp,
      (uint8_t*)&multi_level_switch_set,
      sizeof(multi_level_switch_set_t),
      true,
      pCbFunc);

}

