/**
 * Indicator command class version 3
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "CC_Indicator.h"
#include "CC_IndicatorPrivate.h"
//#define DEBUGPRINT
#include "DebugPrint.h"
#include <ZAF_TSE.h>
#include <string.h>
#include "ZAF_tx_mutex.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * Structure used to track received indicator property values
 */
typedef struct
{
  bool modified;
  uint8_t value;
} INDICATOR_VALUE_INFO;


/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/*
 * (global) Properties for the Node Identify indicator
 */
static uint8_t g_on_off_period_length = 0;  /**< Period length in 1/10 second */
static uint8_t g_on_off_num_cycles    = 0;  /**< Number of cycles */
static uint8_t g_on_time              = 0;  /**< On time in 1/10 second */

s_CC_indicator_data_t indicatorData;
cc_indicator_callback_t callback;


static void
prepare_report_v3(uint8_t indicatorId, ZW_APPLICATION_TX_BUFFER *pTxBuffer, size_t *payloadLen);

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

/**
 * Calls @ref cc_indicator_callback_t after calculating the parameters to pass.
 *
 * Uses the global variables @ref g_on_off_period_length, @ref g_on_off_num_cycles
 * and @ref g_on_time (set by Indicator Command Class commands) to calculate
 * the values to pass to @ref cc_indicator_callback_t.
 *
 * Should be called whenever a SET command is received.
 */
static void
UpdateIndicator(void)
{
  uint32_t on_time_ms = 0;
  uint32_t off_time_ms = 0;
  uint32_t blinker_cycles = 0;

  if (g_on_off_period_length > 0 && g_on_off_num_cycles > 0)
  {
    uint32_t on_off_period_length_ms = g_on_off_period_length * 100;

    /* The value 0x00 is special and means "symmetrical on/off blink" */
    if (0 == g_on_time)
    {
      on_time_ms = on_off_period_length_ms / 2;
    }
    else
    {
      on_time_ms = g_on_time * 100;
    }

    off_time_ms = on_off_period_length_ms - on_time_ms;

    /* The value 0xFF is special and means "run until stopped".
     * If num_cycles is 0xFF we simply leave blinker_cycles at 0
     * as cc_indicator_callback_t cycles as "run until
     * stopped"
     */
    if (g_on_off_num_cycles != 0xFF)
    {
       blinker_cycles = g_on_off_num_cycles;
    }
  }

  /* Tell the indicator to start (or stop) */
  if (callback)
  {
    callback(on_time_ms, off_time_ms, blinker_cycles);
  }
}


/**
 * Resets the indicator properties to their "off" values if the
 * indicator is no longer active.
 *
 * When starting a blink sequence we simply off load the blinking to a
 * board specific indicator LED implementation. The indicator command
 * class does not keep time on when the indicator LED has completed
 * it's blink sequence, instead it queries the board when a GET request
 * is received.
 */
void
CC_Indicator_RefreshIndicatorProperties(void)
{
  g_on_off_period_length = 0;
  g_on_off_num_cycles    = 0;
  g_on_time              = 0;
}


/**
 * Maps a single indicator value to the three version 3 indicator properties
 * stored in the global variables @ref g_on_off_period_length,
 * @ref g_on_off_num_cycles and @ref g_on_time.
 *
 * Used for backward compatibility with the version 1 command class.
 * The single indicator value is either the version 1 indicator value
 * or the version 3 Indicator0 value.
 *
 * @param value The value of the indicator. We're assuming Indicator0 to be a
 *        binary indicator (a more advanced extension could be to map to a
 *        multilevel indicator where the value controls e.g. the blink frequency).
 *        @b value is interpreted like this:
 *        - @b  0: Turn the indicator off
 *        - @b >0: Turn the indicator on
 */
static void
SetIndicator0Value(uint8_t value)
{
  if (0 == value)
  {
    g_on_off_period_length = 0;
    g_on_off_num_cycles    = 0;
    g_on_time              = 0;
  }
  else
  {
    g_on_off_period_length = 0xFF; /* Longest period possible */
    g_on_off_num_cycles    = 0xFF; /* Unlimited numer of periods (run until stopped) */
    g_on_time              = 0xFF; /* On the full period */
  }
}


/**
 * Maps the three version 3 indicator properties stored in the global variables
 * @ref g_on_off_period_length, @ref g_on_off_num_cycles and @ref g_on_time to
 * a single indicator value.
 *
 * @see SetIndicator0Value
 *
 * @return Single value representing the indicator state.
 */
static uint8_t
GetIndicator0Value(void)
{
  /* We're not mapping the reported value 1:1 with the value (potentially)
   * set previously with SetIndicator0Value.
   * The indicator values could have been set with a V2 or V3 Set command
   * that provides more functionality than simply on and off.
   *
   * Here we report the indicator to be "ON" even if it's currently blinking
   * for a short time.
   */

  uint8_t value = 0;

  if (g_on_off_period_length > 0 && g_on_off_num_cycles > 0)
  {
    value = 0xFF;
  }

  return value;
}


/**
 * Command handler for INDICATOR_SET (version 1) commands.
 *
 * Validates that the received (version 1) value is valid and updates the
 * indicator LED accordingly.
 *
 * @param cmd        The incoming command frame.
 * @param cmd_length Length in bytes of the incoming frame.
 *
 * @return @ref e_cmd_handler_return_code_t representing the command handling status.
 *
 * @see handleCommandClassIndicator.
 */
static e_cmd_handler_return_code_t
IndicatorHandler_Set_V1(ZW_APPLICATION_TX_BUFFER *cmd,
                        uint8_t                   cmd_length)
{
  UNUSED(cmd_length);
  const ZW_INDICATOR_SET_FRAME *set_cmd = &(cmd->ZW_IndicatorSetFrame);

  /* Value MUST be in the range 0x00..0x63 or 0xFF */
  if (set_cmd->value < 0x64 || 0xFF == set_cmd->value)
  {
    SetIndicator0Value(set_cmd->value);
    UpdateIndicator();

    /* Supported indicator has been changed, making a call to ZAF_TSE: */
    indicatorData.indicatorId = INDICATOR_IND_NODE_IDENTIFY; /* Default indicator is mapped to Identify */
    /* Only 1 indicator ID supported, so we can set overwrite_previous_trigger to true */
    if (false == ZAF_TSE_Trigger(CC_Indicator_report_stx, (void *)&indicatorData, true))
    {
      DPRINTF("%s(): ZAF_TSE_Trigger failed\n", __func__);
    }

    return E_CMD_HANDLER_RETURN_CODE_HANDLED;
  }
  else
  {
    return E_CMD_HANDLER_RETURN_CODE_FAIL;
  }
}


/**
 * Command handler for INDICATOR_SET (version 3) commands.
 *
 * Validates that the received (version 1) or version 2-3 values are valid and
 * updates the indicator LED accordingly.
 *
 * @param cmd        The incoming command frame.
 * @param cmd_length Length in bytes of the incoming frame.
 *
 * @return @ref e_cmd_handler_return_code_t representing the command handling status.
 *
 * @see handleCommandClassIndicator.
 */
static e_cmd_handler_return_code_t
IndicatorHandler_Set_V3(ZW_APPLICATION_TX_BUFFER *cmd,
                        uint8_t                   cmd_length)
{
  const ZW_INDICATOR_SET_1BYTE_V3_FRAME *set_cmd = &(cmd->ZW_IndicatorSet1byteV3Frame);
  uint32_t obj_count               = 0;
  uint32_t calculated_frame_length = 0;

  const uint32_t frame_size_fixed_part = offsetof(ZW_INDICATOR_SET_1BYTE_V3_FRAME,
                                             variantgroup1);

  /* Make sure we received at least full frame before proceeding */
  if (cmd_length < frame_size_fixed_part)
  {
    return E_CMD_HANDLER_RETURN_CODE_FAIL;
  }
  else
  {
    obj_count               = set_cmd->properties1 & INDICATOR_OBJECT_COUNT_MASK;
    calculated_frame_length = frame_size_fixed_part +
                              obj_count * sizeof(VG_INDICATOR_SET_V3_VG);

    if (cmd_length < calculated_frame_length)
    {
      return E_CMD_HANDLER_RETURN_CODE_FAIL;
    }
  }
  /* Frame size ok - we can continue */

  e_cmd_handler_return_code_t rc  = E_CMD_HANDLER_RETURN_CODE_HANDLED;

  if (0 == obj_count)
  {
    /* Only process indicator0Value here - ignore if obj_count > 0 */
    rc = IndicatorHandler_Set_V1(cmd, cmd_length);
  }
  else
  {
    const VG_INDICATOR_SET_V3_VG *vg_array = &(set_cmd->variantgroup1);

    /* Default values */
    INDICATOR_VALUE_INFO on_off_period_length = {0};
    INDICATOR_VALUE_INFO on_off_num_cycles    = {0};
    INDICATOR_VALUE_INFO on_time              = {0};

    for (uint32_t i = 0; i < obj_count; i++)
    {
      if (vg_array[i].indicatorId == INDICATOR_IND_NODE_IDENTIFY)
      {
        uint8_t value = vg_array[i].value;

        switch (vg_array[i].propertyId)
        {
          case INDICATOR_PROP_ON_OFF_PERIOD:
            on_off_period_length.value    = value;
            on_off_period_length.modified = true;
            break;

          case INDICATOR_PROP_ON_OFF_CYCLES:
            on_off_num_cycles.value    = value;
            on_off_num_cycles.modified = true;
            break;

          case INDICATOR_PROP_ON_TIME:
            on_time.value    = value;
            on_time.modified = true;
            break;

          default:
            /* Ignore unknown properties, but remember that something went
             * wrong for the Supervision Report
             */
            rc = E_CMD_HANDLER_RETURN_CODE_FAIL;
            break;
        }
      }
      else
      {
        /* Unsupported IndicatorID - we don't know yet if we received all
         * required properties for the supported indicator ID, but for
         * the supervision report we indicate that at least some of what
         * we received was an error.
         */
        rc = E_CMD_HANDLER_RETURN_CODE_FAIL;
      }
    }

    /* Did we receive the two required properties? */
    if (on_off_period_length.modified && on_off_num_cycles.modified)
    {
      g_on_off_period_length = on_off_period_length.value;
      g_on_off_num_cycles    = on_off_num_cycles.value;

      if (on_time.value > g_on_off_period_length)
      {
        /* Ignore (i.e. use default value) "on_time" if bigger than period length */
        g_on_time = 0;

        /* Even though we rejected the specified (invalid) on_time value we
         * accept the request and update the indicator. But for supervision
         * we will signal a failure
         */
        rc = E_CMD_HANDLER_RETURN_CODE_FAIL;
      }
      else
      {
        /* If on_time was unspecified, it defaults to 0 anyway */
        g_on_time = on_time.value;
      }

      UpdateIndicator();
      /* Supported indicator has been changed, making a call to ZAF_TSE: */
      indicatorData.indicatorId = INDICATOR_IND_NODE_IDENTIFY; /* Default indicator is mapped to Identify */
      /* Only 1 indicator ID supported, so we can set overwrite_previous_trigger to true */
      if (false == ZAF_TSE_Trigger(CC_Indicator_report_stx, (void *)&indicatorData, true))
      {
        DPRINTF("%s(): ZAF_TSE_Trigger failed\n", __func__);
      }
    }
    else
    {
      /* Minimum required parameters were not present */
      rc = E_CMD_HANDLER_RETURN_CODE_FAIL;
    }
  }

  return rc;
}


/**
 * Command handler for INDICATOR_GET (version 1) commands.
 *
 * Will send back an INDICATOR_REPORT (version 1) frame containing the
 * value of the indicator.
 *
 * @param rx_opt     Receive options
 * @param cmd        The incoming command frame.
 * @param cmd_length Length in bytes of the incoming frame.
 *
 * @return @ref received_frame_status_t representing the command handling status.
 *
 * @see handleCommandClassIndicator.
 */
static received_frame_status_t
IndicatorHandler_Get_V1(RECEIVE_OPTIONS_TYPE_EX  *rx_opt,
                        ZW_APPLICATION_TX_BUFFER *cmd,
                        uint8_t                   cmd_length)
{
  UNUSED(cmd);
  UNUSED(cmd_length);
  ZAF_TRANSPORT_TX_BUFFER          tx_buf = {0};
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *tx_opt = NULL;
  ZW_INDICATOR_REPORT_FRAME       *report = &(tx_buf.appTxBuf.ZW_IndicatorReportFrame);
  EZAF_EnqueueStatus_t             queue_status = ZAF_ENQUEUE_STATUS_SUCCESS;

  RxToTxOptions(rx_opt, &tx_opt);

  report->cmdClass    = COMMAND_CLASS_INDICATOR;
  report->cmd         = INDICATOR_REPORT;
  report->value       = GetIndicator0Value();

  queue_status = Transport_SendResponseEP((uint8_t *) report,
                                          sizeof(*report),
                                          tx_opt,
                                          NULL);

  if (ZAF_ENQUEUE_STATUS_SUCCESS != queue_status)
  {
    /* Could not send response - will not handle here.
     * The requesting node should retry the get if needed
     */
    DPRINT("Failed to add element to protocol queue\n");
  }

  return RECEIVED_FRAME_STATUS_SUCCESS;
}



/**
 * Command handler for INDICATOR_GET (version 3) commands.
 *
 * Will send back an INDICATOR_REPORT (version 3) frame containing
 * the three values of the indicator properties.
 *
 * @param rx_opt     Receive options
 * @param cmd        The incoming command frame.
 * @param cmd_length Length in bytes of the incoming frame.
 *
 * @return @ref received_frame_status_t representing the command handling status.
 *
 * @see handleCommandClassIndicator.
 */
static received_frame_status_t
IndicatorHandler_Get_V3(RECEIVE_OPTIONS_TYPE_EX  *rx_opt,
                        ZW_APPLICATION_TX_BUFFER *cmd,
                        uint8_t                   cmd_length)
{
  if (cmd_length < sizeof(ZW_INDICATOR_GET_V3_FRAME))
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  const ZW_INDICATOR_GET_V3_FRAME *get_cmd = &(cmd->ZW_IndicatorGetV3Frame);
  ZAF_TRANSPORT_TX_BUFFER          tx_buf = {0};
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *tx_opt = NULL;
  EZAF_EnqueueStatus_t             queue_status = ZAF_ENQUEUE_STATUS_SUCCESS;
  size_t                           payload_len;

  RxToTxOptions(rx_opt, &tx_opt);
  prepare_report_v3(get_cmd->indicatorId, &tx_buf.appTxBuf, &payload_len);

  queue_status = Transport_SendResponseEP((uint8_t *) &tx_buf.appTxBuf,
                                          payload_len,
                                          tx_opt,
                                          NULL);

  if (ZAF_ENQUEUE_STATUS_SUCCESS != queue_status)
  {
    /* Could not send response - will not handle here.
     * The requesting node should retry the get if needed
     */
    DPRINT("Failed to add element to protocol queue\n");
  }

  return RECEIVED_FRAME_STATUS_SUCCESS;
}


/**
 * Command handler for INDICATOR_SUPPORTED_GET commands.
 *
 * Will send back an INDICATOR_SUPPORTED_REPORT frame containing
 * the three properties supported by the node identify indicator.
 *
 * If the command frame contains the "wildcard" indicator ID 0x00
 * used for discovery, then the Node Identify indicator ID will be
 * included as the indicator ID in the otherwise empty report.
 *
 * @param rx_opt     Receive options
 * @param cmd        The incoming command frame.
 * @param cmd_length Length in bytes of the incoming frame.
 *
 * @return @ref received_frame_status_t representing the command handling status.
 *
 * @see handleCommandClassIndicator.
 */
static received_frame_status_t
IndicatorHandler_SupportedGet_V3(RECEIVE_OPTIONS_TYPE_EX  *rx_opt,
                                 ZW_APPLICATION_TX_BUFFER *cmd,
                                 uint8_t                   cmd_length)
{
  if (cmd_length < sizeof(ZW_INDICATOR_SUPPORTED_GET_V3_FRAME))
  {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  const ZW_INDICATOR_SUPPORTED_GET_V3_FRAME    *sget_cmd = &(cmd->ZW_IndicatorSupportedGetV3Frame);
  ZAF_TRANSPORT_TX_BUFFER                       tx_buf = {0};
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX              *tx_opt = NULL;
  ZW_INDICATOR_SUPPORTED_REPORT_1BYTE_V3_FRAME *report = &(tx_buf.appTxBuf.ZW_IndicatorSupportedReport1byteV3Frame);
  EZAF_EnqueueStatus_t                          queue_status = ZAF_ENQUEUE_STATUS_SUCCESS;
  size_t                                        send_data_length;

  RxToTxOptions(rx_opt, &tx_opt);

  if ((INDICATOR_IND_NODE_IDENTIFY == sget_cmd->indicatorId) || (INDICATOR_IND_NA == sget_cmd->indicatorId))
  {
    report->cmdClass                  = COMMAND_CLASS_INDICATOR_V3;
    report->cmd                       = INDICATOR_SUPPORTED_REPORT_V3;
    report->indicatorId               = INDICATOR_IND_NODE_IDENTIFY;
    report->nextIndicatorId           = INDICATOR_IND_NA;
    report->properties1               = 1;
    report->propertySupportedBitMask1 = (1 << INDICATOR_PROP_ON_OFF_PERIOD) |
                                        (1 << INDICATOR_PROP_ON_OFF_CYCLES) |
                                        (1 << INDICATOR_PROP_ON_TIME);

    send_data_length = sizeof(*report);
  }
  else
  {
    /* The indicator is not supported, send back an empty report. */

    /*
     * All the "Supported Report" structs have one or more bitmasks.
     * Here we must send one without the bitmask, so we use the struct
     * with one bitmask, but tell Transport_SendResponseEP to only send
     * the "fixed header" part (leaving out the bitmask).
     */

    report->cmdClass                  = COMMAND_CLASS_INDICATOR_V3;
    report->cmd                       = INDICATOR_SUPPORTED_REPORT_V3;
    report->indicatorId               = INDICATOR_IND_NA;
    report->nextIndicatorId           = INDICATOR_IND_NA;
    report->properties1               = 0;

    /* Leave out the bitmask when sending */
    send_data_length = offsetof(ZW_INDICATOR_SUPPORTED_REPORT_1BYTE_V3_FRAME,
                       propertySupportedBitMask1);
  }

  queue_status = Transport_SendResponseEP((uint8_t *) report,
                                          send_data_length,
                                          tx_opt,
                                          NULL);

  if (ZAF_ENQUEUE_STATUS_SUCCESS != queue_status)
  {
    /* Could not send response - will not handle here.
     * The requesting node should retry the get if needed
     */
    DPRINT("Failed to add element to protocol queue\n");
  }

  return RECEIVED_FRAME_STATUS_SUCCESS;
}


/****************************************************************************/
/*                             PUBLIC FUNCTIONS                             */
/****************************************************************************/

void CC_Indicator_Init(cc_indicator_callback_t callback_)
{
  callback = callback_;
}

/*
 * Entry point for the Indicator Command Class handler.
 * Dispatches incoming commands to the appropriate handler function.
 */
static received_frame_status_t
CC_Indicator_handler(RECEIVE_OPTIONS_TYPE_EX  *rx_opt,
                            ZW_APPLICATION_TX_BUFFER *cmd,
                            uint8_t                   cmd_length)
{
  received_frame_status_t rc = RECEIVED_FRAME_STATUS_NO_SUPPORT;
  e_cmd_handler_return_code_t set_handler_status; // return value of set handler

  switch (cmd->ZW_Common.cmd)
  {
    case INDICATOR_SET_V3:
      // Build up new CC data structure
      memset(&indicatorData, 0, sizeof(s_CC_indicator_data_t));
      indicatorData.rxOptions = *rx_opt;

      /* The parsing of Indicator V3 Set Command is complex, so the ZAF TSE Trigger is made from the command handler */
      if (cmd_length == sizeof(ZW_INDICATOR_SET_FRAME))
      {
        set_handler_status = IndicatorHandler_Set_V1(cmd, cmd_length);
      }
      else
      {
        set_handler_status = IndicatorHandler_Set_V3(cmd, cmd_length);
      }

      // Assumption is that Indicator handler returns success and fail only
      rc = (set_handler_status == E_CMD_HANDLER_RETURN_CODE_HANDLED) ?
          RECEIVED_FRAME_STATUS_SUCCESS : RECEIVED_FRAME_STATUS_FAIL;
      break;

    case INDICATOR_GET_V3:
      if (false == Check_not_legal_response_job(rx_opt))
      {
        if (cmd_length == sizeof(ZW_INDICATOR_GET_FRAME))
        {
          rc = IndicatorHandler_Get_V1(rx_opt, cmd, cmd_length);
        }
        else
        {
          rc = IndicatorHandler_Get_V3(rx_opt, cmd, cmd_length);
        }
      }
      else
      {
        rc = RECEIVED_FRAME_STATUS_FAIL;
      }
      break;

    case INDICATOR_SUPPORTED_GET_V3:
      if (false == Check_not_legal_response_job(rx_opt))
      {
        rc = IndicatorHandler_SupportedGet_V3(rx_opt, cmd, cmd_length);
      }
      else
      {
        rc = RECEIVED_FRAME_STATUS_FAIL;
      }
      break;

    default:
      rc = RECEIVED_FRAME_STATUS_NO_SUPPORT;
      break;
  }

  return rc;
}


void CC_Indicator_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void* pData)
{
  DPRINTF("* %s() *\n"
      "\ttxOpt.src = %d\n"
      "\ttxOpt.options %#02x\n"
      "\ttxOpt.secOptions %#02x\n",
      __func__, txOptions.sourceEndpoint, txOptions.txOptions, txOptions.txSecOptions);

  /* Prepare payload for report */
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  size_t payloadLen;
  s_CC_indicator_data_t* pIndicatorData = (s_CC_indicator_data_t*)pData;

  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

  prepare_report_v3(pIndicatorData->indicatorId, pTxBuf, &payloadLen);

  if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendRequestEP((uint8_t *)pTxBuf,
                                                            payloadLen,
                                                            &txOptions,
                                                            ZAF_TSE_TXCallback))
  {
    //sending request failed
    DPRINTF("%s(): Transport_SendRequestEP() failed. \n", __func__);
  }
}

/**
 * Prepare payload to be sent in Incicator Report V3
 */
static void
prepare_report_v3(uint8_t indicatorId, ZW_APPLICATION_TX_BUFFER *pTxBuffer, size_t *payloadLen)
{
  if(INDICATOR_IND_NODE_IDENTIFY == indicatorId)
  {
    DPRINTF("LINE: %d INDICATOR_IND_NODE_IDENTIFY\n", __LINE__);
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.cmdClass = COMMAND_CLASS_INDICATOR_V3;
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.cmd = INDICATOR_REPORT_V3;
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.indicator0Value = GetIndicator0Value();
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.properties1     = 3;  /* Number of property objects */

    pTxBuffer->ZW_IndicatorReport3byteV3Frame.variantgroup1.indicatorId = INDICATOR_IND_NODE_IDENTIFY;
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.variantgroup1.propertyId  = INDICATOR_PROP_ON_OFF_PERIOD;
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.variantgroup1.value       = g_on_off_period_length;

    pTxBuffer->ZW_IndicatorReport3byteV3Frame.variantgroup2.indicatorId = INDICATOR_IND_NODE_IDENTIFY;
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.variantgroup2.propertyId  = INDICATOR_PROP_ON_OFF_CYCLES;
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.variantgroup2.value       = g_on_off_num_cycles;

    pTxBuffer->ZW_IndicatorReport3byteV3Frame.variantgroup3.indicatorId = INDICATOR_IND_NODE_IDENTIFY;
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.variantgroup3.propertyId  = INDICATOR_PROP_ON_TIME;
    pTxBuffer->ZW_IndicatorReport3byteV3Frame.variantgroup3.value       = g_on_time;

    *payloadLen = sizeof(ZW_INDICATOR_REPORT_3BYTE_V3_FRAME);
  }
  else
  {
    DPRINTF("LINE: %d Unsupported indicator ID\n", __LINE__);
    pTxBuffer->ZW_IndicatorReport1byteV3Frame.cmdClass        = COMMAND_CLASS_INDICATOR_V3;
    pTxBuffer->ZW_IndicatorReport1byteV3Frame.cmd             = INDICATOR_REPORT_V3;
    pTxBuffer->ZW_IndicatorReport1byteV3Frame.indicator0Value = GetIndicator0Value();
    pTxBuffer->ZW_IndicatorReport1byteV3Frame.properties1     = 1;  /* Number of property objects */

    pTxBuffer->ZW_IndicatorReport1byteV3Frame.variantgroup1.indicatorId = INDICATOR_REPORT_NA_V3;
    pTxBuffer->ZW_IndicatorReport1byteV3Frame.variantgroup1.propertyId  = 0;
    pTxBuffer->ZW_IndicatorReport1byteV3Frame.variantgroup1.value       = 0;

    *payloadLen = sizeof(ZW_INDICATOR_REPORT_1BYTE_V3_FRAME);
  }
}

static uint8_t lifeline_reporting(ccc_pair_t * p_ccc_pair)
{
  p_ccc_pair->cmdClass = COMMAND_CLASS_INDICATOR;
  p_ccc_pair->cmd      = INDICATOR_REPORT;
  return 1;
}

REGISTER_CC_V3(COMMAND_CLASS_INDICATOR, INDICATOR_VERSION_V3, CC_Indicator_handler, NULL, NULL, lifeline_reporting, 0);
