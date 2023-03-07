/**
 * @file
 * Handler for Command Class Color Switch.
 * @copyright 2020 Silicon Laboratories Inc.
 */

#include "CC_ColorSwitch.h"
#include <misc.h>
#include <string.h>
#include <CC_Supervision.h>
#include <ZAF_TSE.h>
#include "ZAF_tx_mutex.h"
//#define DEBUGPRINT
#include <DebugPrint.h>

/**< Pointer to array of supported colors defined in the app */
static s_colorComponent *pColorComponents;
/**< Number of supported colors in pColorComponents */
static uint8_t colorsSupportedCount = 0;
/**< bitmask of colors that changes currently. Used to check if there is active change for any color */
static uint16_t colorsChangingBitmask;

static uint16_t colorsComponentsMask(uint8_t endpoint);
static s_colorComponent* findColorComponentByColorID(uint8_t colorId, uint8_t endpoint);
static s_colorComponent* findColorComponentByObj(s_Actuator *pObj);

/**
 * Optional. Can be used as single cb function for all colors. Defined in the app.
 * Called when change takes place in driver and app needs to update LED status
 */
static void (*callback_refreshTheApp)(void) = NULL;
typedef void (*color_callback_t)(s_colorComponent *colorComponent);

void CC_ColorSwitch_Init(s_colorComponent *colors,
                         uint8_t length,
                         uint8_t durationDefault,
                         void (*CC_ColorSwitch_callback)(void))
{
  ASSERT(colors);
  colorsChangingBitmask = 0;
  pColorComponents = colors;
  if (0 == length || length > ECOLORCOMPONENT_COLOR_UNDEFINED)
  {
    DPRINTF("%s: Invalid number of colors %d!\n", __func__, length);
    ASSERT(false);
  }
  colorsSupportedCount = length;
  for (int i = 0; i < colorsSupportedCount; i++)
  {
    ZAF_Actuator_Init(&pColorComponents[i].obj, 0 , 0xFF, 20, durationDefault, &CC_ColorSwitch_ColorChanged_cb);
    ZAF_Actuator_Set(&pColorComponents[i].obj, 0xFF, 0);
  }

  if (NULL != CC_ColorSwitch_callback)
  {
    callback_refreshTheApp = CC_ColorSwitch_callback;
  }
}

static received_frame_status_t
CC_ColorSwitch_handler(RECEIVE_OPTIONS_TYPE_EX *pRxOpt,
                       ZW_APPLICATION_TX_BUFFER *pCmd,
                       uint8_t cmdLength)
{
  switch (pCmd->ZW_Common.cmd)
  {
    case SWITCH_COLOR_SUPPORTED_GET:
    {
      if(true == Check_not_legal_response_job(pRxOpt))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      ZAF_TRANSPORT_TX_BUFFER  TxBuf;
      ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );
      TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
      RxToTxOptions(pRxOpt, &pTxOptionsEx);

      pTxBuf->ZW_SwitchColorSupportedReportV3Frame.cmdClass = COMMAND_CLASS_SWITCH_COLOR;
      pTxBuf->ZW_SwitchColorSupportedReportV3Frame.cmd = SWITCH_COLOR_SUPPORTED_REPORT;
      uint16_t colorCompMask = colorsComponentsMask(pRxOpt->destNode.endpoint);
      pTxBuf->ZW_SwitchColorSupportedReportV3Frame.colorComponentMask1 = (uint8_t)(colorCompMask & 0xFF);
      pTxBuf->ZW_SwitchColorSupportedReportV3Frame.colorComponentMask2 = (uint8_t)(colorCompMask >> 8);

      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP((uint8_t *)pTxBuf,
                                                                    sizeof(ZW_SWITCH_COLOR_SUPPORTED_REPORT_V3_FRAME),
                                                                    pTxOptionsEx,
                                                                    NULL))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
    }
    break;
    case SWITCH_COLOR_GET:
    {
      if(true == Check_not_legal_response_job(pRxOpt))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      ZAF_TRANSPORT_TX_BUFFER  TxBuf;
      ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );
      TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
      RxToTxOptions(pRxOpt, &pTxOptionsEx);

      s_colorComponent *color = findColorComponentByColorID(pCmd->ZW_SwitchColorGetV3Frame.colorComponentId,
                                                            pRxOpt->destNode.endpoint);
      if (NULL == color)
      {
        // If requested color was not found, send REPORT of first defined color.
        color = findColorComponentByColorID(pColorComponents->colorId, pRxOpt->destNode.endpoint);
      }
      DPRINTF("Prepare report for %d color\n", color->colorId);
      pTxBuf->ZW_SwitchColorReportV3Frame.cmdClass = COMMAND_CLASS_SWITCH_COLOR;
      pTxBuf->ZW_SwitchColorReportV3Frame.cmd = SWITCH_COLOR_REPORT_V3;
      pTxBuf->ZW_SwitchColorReportV3Frame.colorComponentId = color->colorId;
      pTxBuf->ZW_SwitchColorReportV3Frame.currentValue = ZAF_Actuator_GetCurrentValue(&color->obj);
      pTxBuf->ZW_SwitchColorReportV3Frame.targetValue = ZAF_Actuator_GetTargetValue(&color->obj);
      pTxBuf->ZW_SwitchColorReportV3Frame.duration = ZAF_Actuator_GetDurationRemaining(&color->obj);

      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP((uint8_t *)pTxBuf,
                                                                    sizeof(ZW_SWITCH_COLOR_REPORT_V3_FRAME),
                                                                    pTxOptionsEx,
                                                                    NULL))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
    }
    break;
    case SWITCH_COLOR_SET:
    {
      // Read five least significant bits (4-0) from ZW_SwitchColorSet4byteV3Frame.properties1 to get Color Count
      uint8_t colorCount = pCmd->ZW_SwitchColorSet4byteV3Frame.properties1 & 0x1F;
      if (0 == colorCount)
      {
        DPRINTF("%s SET: colorCount = 0, nothing to do.\n", __func__);
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      uint8_t duration;
      if ((3 + 2*colorCount) == cmdLength)// (CC + CMD + colorCount + 2*colorCount)
      {
        duration = 0;
      }
      else if ((3 + 2*colorCount + 1) == cmdLength)
      {
        duration = *((uint8_t *)(&pCmd->ZW_Common.cmdClass) + cmdLength - 1);
      }
      else
      {
        DPRINTF("SWITCH_COLOR_SET: Unknown frame length = %#d\n", cmdLength);
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      uint8_t *pRxFrame = (uint8_t *)&(pCmd->ZW_SwitchColorSet4byteV3Frame.variantgroup1);
      EColorComponents colorChangeInProgress = ECOLORCOMPONENT_COLOR_UNDEFINED;
      uint8_t status = EACTUATOR_NOT_CHANGING;
      s_colorComponent *color;
      for (int i = 0; i < colorCount; i++)
      {
        // save the values to be passed to set function
        EColorComponents colorID = *(pRxFrame++);
        uint8_t value = *(pRxFrame++);
        color = findColorComponentByColorID(colorID, pRxOpt->destNode.endpoint);
        if (NULL == color)
        {
          DPRINTF("Color %x not supported!\n", colorID);
          return RECEIVED_FRAME_STATUS_FAIL;
        }
        color->rxOpt = *pRxOpt;
        colorsChangingBitmask |= (uint16_t)(1 << color->colorId);
        status = ZAF_Actuator_Set(&color->obj, value, duration);
        if ((EACTUATOR_CHANGING != status) && (colorsChangingBitmask & (1 << color->colorId))) {
          // Current color is in its final state. If there was any change, the flag in colorsChangingBitmask
          // would have been cleared in CC_ColorSwitch_ColorChanged_cb(). If flag is still on, just clear it.
          colorsChangingBitmask &= (uint16_t)~((uint16_t)1 << color->colorId);
        }
        if (EACTUATOR_FAILED == status) {
          return RECEIVED_FRAME_STATUS_FAIL;
        }
        if (EACTUATOR_CHANGING == status)
        {
          colorChangeInProgress = colorID;
        }
      }
      if (ECOLORCOMPONENT_COLOR_UNDEFINED == colorChangeInProgress)
      {
        DPRINT("No ongoing color change, done.\n");
        // Nothing else to do, so just break here. Function will return default value in case of success
        break;
      }
      if (!pRxOpt->bSupervisionActive)
      {
        DPRINT("Supervision not requested, done.\n");
        // Nothing else to do, so just break here. Function will return default value in case of success
        break;
      }
      color = findColorComponentByColorID((uint8_t)colorChangeInProgress, pRxOpt->destNode.endpoint);
      uint8_t durationRemaining = ZAF_Actuator_GetDurationRemaining(&color->obj);
      DPRINTF("Send Supervision Report WORKING with duration %#2X, superv properties = %X\n", durationRemaining,
              color->rxOpt.sessionId | (color->rxOpt.statusUpdate << 7));

      /* Ideally function would just return CC_SUPERVISION_STATUS_WORKING from here
           But since Supervision CC don't know duration that should be sent, report needs to be sent from here. */
      if (!is_multicast(&color->rxOpt)) {
        TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
        RxToTxOptions(&color->rxOpt, &pTxOptionsEx);
        // Only send supervision report if the set command was not send with a multicast.
        CmdClassSupervisionReportSend(pTxOptionsEx,
                                      (uint8_t)(color->rxOpt.sessionId | (color->rxOpt.statusUpdate << 7)),
                                      CC_SUPERVISION_STATUS_WORKING,
                                      durationRemaining);
      }
      return RECEIVED_FRAME_STATUS_CANCEL;
    }
    case SWITCH_COLOR_START_LEVEL_CHANGE:
    {
      uint8_t upDown = pCmd->ZW_SwitchColorStartLevelChangeV3Frame.properties1 & (1 << 6);
      uint8_t ignoreStartLevel = pCmd->ZW_SwitchColorStartLevelChangeV3Frame.properties1 & (1 << 5);
      s_colorComponent *color = findColorComponentByColorID(
          pCmd->ZW_SwitchColorStartLevelChangeV3Frame.colorComponentId,
          pRxOpt->destNode.endpoint);
      if( NULL == color)
      {
        DPRINTF("Color %x not supported!\n", pCmd->ZW_SwitchColorStartLevelChangeV3Frame.colorComponentId);
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      uint8_t startLevel = pCmd->ZW_SwitchColorStartLevelChangeV3Frame.startLevel;
      uint8_t duration;
      if (5 == cmdLength) // (CC + CMD + properties + colorId + startLevel (+ duration))
      {
        duration = 0xFF; // Notify Actuator to use factory default duration
      }
      else if (6 == cmdLength)
      {
        duration = pCmd->ZW_SwitchColorStartLevelChangeV3Frame.duration;
      }
      else
      {
        DPRINTF("SWITCH_COLOR_START_LEVEL_CHANGE: Unknown frame length = %#d\n", cmdLength);
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      color->rxOpt = *pRxOpt;
      colorsChangingBitmask |= (uint16_t)(1 << color->colorId);
      uint8_t status = ZAF_Actuator_StartChange(&color->obj, ignoreStartLevel, upDown, startLevel, duration);
      if ((EACTUATOR_CHANGING != status) && (colorsChangingBitmask & (1 << color->colorId))) {
        // Current color is in its final state. If there was any change, the flag in colorsChangingBitmask
        // would have been cleared in CC_ColorSwitch_ColorChanged_cb(). If flag is still on, just clear it.
        colorsChangingBitmask &= (uint16_t)~((uint16_t)1 << color->colorId);
      }
      if (EACTUATOR_FAILED == status) {
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      // All done with SUCCESS. If needed, Supervision CC will take care of sending Supervision report,
      // so clear supervision flag here.
      color->rxOpt.bSupervisionActive = 0;
    }
    break;
    case SWITCH_COLOR_STOP_LEVEL_CHANGE:
    {
      if (3 != cmdLength)
      {
        DPRINTF("%s(): Invalid frame length %d\n", __func__, cmdLength);
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      s_colorComponent *color = findColorComponentByColorID(pCmd->ZW_SwitchColorStopLevelChangeV3Frame.colorComponentId,
                                                            pRxOpt->destNode.endpoint);
      if (NULL == color)
      {
        DPRINTF("Color %x not supported!\n", pCmd->ZW_SwitchColorStartLevelChangeV3Frame.colorComponentId);
        return RECEIVED_FRAME_STATUS_FAIL;
      }
      if (ZAF_Actuator_StopChange(&color->obj))
      {
        colorsChangingBitmask &= (uint16_t)~((uint16_t)1 << color->colorId);
        if (false == ZAF_TSE_Trigger(CC_ColorSwitch_report_stx,
                                     (void *)color,
                                     false))
        {
          DPRINTF("%s(): ZAF_TSE_Trigger failed\n", __func__);
        }
      }
    }
    break;
    default:
      return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }
  return RECEIVED_FRAME_STATUS_SUCCESS;
}

/**
 * Color Component mask. Used for SWITCH_COLOR_SUPPORTED_REPORT
 * @param Specific endpoint
 * @return bitmask with all supported colors (n-th bit =n-th color. See @ref EColorComponents)
 */
static uint16_t colorsComponentsMask(uint8_t endpoint)
{
  ASSERT(NULL != pColorComponents);
  ASSERT(0 != colorsSupportedCount);
  uint8_t colorMask = 0;
  s_colorComponent *pColor = pColorComponents;
  for(uint8_t i = 0; i < colorsSupportedCount; i++, pColor++)
  {
    if (endpoint == pColor->ep)
    {
      colorMask |= (uint8_t)(1 << pColor->colorId);
    }
  }
  return colorMask;
}

/**
 * Return index of color given by colorId in the array s_colorComponents
 * Used to fill in Report
 * @param Specific endpoint
 * @param colorId Unique color ID
 * @return pointer to the color structure that contains colorId color.
 */
static s_colorComponent* findColorComponentByColorID(uint8_t colorId, uint8_t endpoint)
{
  ASSERT(NULL != pColorComponents);
  ASSERT(0 != colorsSupportedCount);
  s_colorComponent *pColor = pColorComponents;
  for(uint8_t i = 0; i < colorsSupportedCount; i++, pColor++)
  {
    if (colorId == pColor->colorId && endpoint == pColor->ep)
    {
      return pColor;
    }
  }
  return NULL;
}

static s_colorComponent* findColorComponentByObj(s_Actuator *pObj)
{
  ASSERT(NULL != pColorComponents);
  ASSERT(0 != colorsSupportedCount);
  s_colorComponent *pColor = pColorComponents;
  for(uint8_t i = 0; i < colorsSupportedCount; i++, pColor++)
  {
    if (pObj == &pColor->obj)
    {
      return pColor;
    }
  }
  return NULL;
}

void CC_ColorSwitch_ColorChanged_cb(s_Actuator *pObj)
{
  s_colorComponent *color = findColorComponentByObj(pObj);
  // We ended up here so it must be that current value has changed. Inform the app about it.
  if(NULL != color->cb)
  {
    color_callback_t cb = color->cb;
    cb(color);
  }
  else if(NULL != callback_refreshTheApp)
  {
   callback_refreshTheApp();
  }

  if(ZAF_Actuator_GetTargetValue(&color->obj) == ZAF_Actuator_GetCurrentValue(&color->obj))
  {
    if (0 == colorsChangingBitmask)
    {
      // Shouldn't ever happen, but indicates that callback function was called more times than predicted
      return;
    }
    colorsChangingBitmask &= (uint16_t)~((uint16_t)1 << color->colorId);
    DPRINTF("Trigger TSE for color %x\n", color->colorId);
    if (false == ZAF_TSE_Trigger(CC_ColorSwitch_report_stx,
                                 (void *)color,
                                 false))
    {
      DPRINTF("%s(): ZAF_TSE_Trigger failed\n", __func__);
    }
    if (0 == colorsChangingBitmask)
    {
      if(color->rxOpt.bSupervisionActive && color->rxOpt.statusUpdate && !is_multicast(&color->rxOpt))
      {
        TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
        RxToTxOptions(&color->rxOpt, &pTxOptionsEx);
        // Send Supervision Report Success if not triggered with a multicast.
        CmdClassSupervisionReportSend(pTxOptionsEx,
                                      color->rxOpt.sessionId, // This is last status update, no need to set anything.
                                      CC_SUPERVISION_STATUS_SUCCESS,
                                      0); // durationRemaining should always be 0 at this point
        DPRINTF("%sSupervision report success sent.\n", __func__);
      }
    }
  }
}

void CC_ColorSwitch_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void *pData)
{
  DPRINTF("* %s() *\n"
      "\ttxOpt.src = %d\n"
      "\ttxOpt.options %#02x\n"
      "\ttxOpt.secOptions %#02x\n",
      __func__, txOptions.sourceEndpoint, txOptions.txOptions, txOptions.txSecOptions);

  /* Prepare payload for report */
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

  s_colorComponent *color = (s_colorComponent *)pData;
  DPRINTF("Sending report for colorId: %u\n", color->colorId);

  pTxBuf->ZW_SwitchColorReportV3Frame.cmdClass = COMMAND_CLASS_SWITCH_COLOR;
  pTxBuf->ZW_SwitchColorReportV3Frame.cmd = SWITCH_COLOR_REPORT_V3;
  pTxBuf->ZW_SwitchColorReportV3Frame.colorComponentId = color->colorId;
  pTxBuf->ZW_SwitchColorReportV3Frame.currentValue = ZAF_Actuator_GetCurrentValue(&color->obj);
  pTxBuf->ZW_SwitchColorReportV3Frame.targetValue = ZAF_Actuator_GetTargetValue(&color->obj);
  pTxBuf->ZW_SwitchColorReportV3Frame.duration = ZAF_Actuator_GetDurationRemaining(&color->obj);

  if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendRequestEP((uint8_t *)pTxBuf,
                                                                sizeof(ZW_SWITCH_COLOR_REPORT_V3_FRAME),
                                                                &txOptions,
                                                                ZAF_TSE_TXCallback))
  {
    DPRINTF("%s(): Transport_SendRequestEP() failed. \n", __func__);
  }
}

static uint8_t lifeline_reporting(ccc_pair_t * p_ccc_pair)
{
  p_ccc_pair->cmdClass = COMMAND_CLASS_SWITCH_COLOR;
  p_ccc_pair->cmd      = SWITCH_COLOR_REPORT;
  return 1;
}

/*
 * This will map this CC to Basic CC, so set commands by Basic CC can be routed to this Command Class for execution.
 */
REGISTER_CC_V3(COMMAND_CLASS_SWITCH_COLOR, SWITCH_COLOR_VERSION_V3, CC_ColorSwitch_handler, NULL, NULL, lifeline_reporting, 0);
