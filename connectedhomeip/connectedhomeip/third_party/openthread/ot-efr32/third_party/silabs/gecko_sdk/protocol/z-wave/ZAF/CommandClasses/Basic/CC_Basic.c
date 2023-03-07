/**
 * @file
 * Handler for Command Class Basic.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZAF_Common_interface.h>
#include <ZW_TransportSecProtocol.h>
#include <ZW_TransportEndpoint.h>
#include <ZW_TransportMulticast.h>
#include <ZW_transport_api.h>
#include <CC_Basic.h>
#include <CC_Common.h>
#include <string.h>
#include "ZAF_tx_mutex.h"
#include "ZAF_CC_Invoker.h"

//#define DEBUGPRINT
#include "DebugPrint.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

/**
 * This function will be invoked when a Basic Set command is received.
 *
 * @attention This is a weak implementation of the Basic Set handler for when the application hasn't
 *            implemented this and there are no mapped CCs to Basic CC for the target endpoint.
 */
ZW_WEAK e_cmd_handler_return_code_t CC_Basic_Set_handler(uint8_t val, uint8_t endpoint)
{
  UNUSED(val);
  UNUSED(endpoint);
  return E_CMD_HANDLER_RETURN_CODE_NOT_SUPPORTED;
}


static received_frame_status_t
CC_Basic_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength)
{
  UNUSED(cmdLength);

  switch (pCmd->ZW_Common.cmd)
  {
      //Must be ignored to avoid unintentional operation. Cannot be mapped to another command class.
    case BASIC_SET:
      if ((0x63 < pCmd->ZW_BasicSetV2Frame.value) && (0xFF != pCmd->ZW_BasicSetV2Frame.value))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      /*
       * Call the legacy handler first. If it's defined in the application, the application will
       * take care of the mapping. If not, the weak implementation will return no support and the
       * CC linker section will be searched.
       */
      e_cmd_handler_return_code_t return_code = CC_Basic_Set_handler(pCmd->ZW_BasicSetFrame.value, rxOpt->destNode.endpoint);

      /* If handler has finished, call TSE */
      if (E_CMD_HANDLER_RETURN_CODE_FAIL == return_code || E_CMD_HANDLER_RETURN_CODE_HANDLED == return_code)
      {
        // Build up new CC data structure
        void* pZAF_TSE_basicData = CC_Basic_prepare_zaf_tse_data(rxOpt);
        if (false == ZAF_TSE_Trigger(CC_Basic_report_stx, pZAF_TSE_basicData, true))
        {
          DPRINTF("%s(): ZAF_TSE_Trigger failed\n", __func__);
        }
      }
      else if (E_CMD_HANDLER_RETURN_CODE_WORKING == return_code)
      {
       /* If the status is working, inform the application so it can make the TSE trigger later on */
        CC_Basic_report_notifyWorking(rxOpt);
      }

      /* Return the handler result */
      if (E_CMD_HANDLER_RETURN_CODE_FAIL == return_code)
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      if (E_CMD_HANDLER_RETURN_CODE_HANDLED == return_code ||
          E_CMD_HANDLER_RETURN_CODE_WORKING == return_code) {
        return RECEIVED_FRAME_STATUS_SUCCESS;
      }


      /*
       * 1. Get a list of supported CCs for the specific endpoint that received the Basic Set.
       *       (Check to see if this CC is supported for current level of security.)
       * 2. Match with the list of linked CCs that has a Basic Set mapper function.
       * 3. Invoke the Basic Set mapper function.
       * 4. Invoke the corresponding CC handler.
       */

      CMD_CLASS_LIST *pCmdClassList  = NULL;

      pCmdClassList = GetCommandClassList((0 != ZAF_GetNodeID()), rxOpt->securityKey, rxOpt->destNode.endpoint);
      ASSERT(NULL != pCmdClassList);  // FATAL: Empty CC list. We should not have reached this point. Unlikely scenario!

      // For each supported CC
      for ( uint8_t i = 0; i < pCmdClassList->size; i++)
      {
        /*
         * When a CC uses the REGISTER_CC() macro, the compiler creates a section in the code.
         * Also two variables are automatically created and these represent the beginning and the end
         * of the section. The variables can be used to loop through the section.
         */
        CC_handler_map_latest_t const *cc_Handler_Iter = &cc_handlers_start;

        // For each linked CC
        for (; cc_Handler_Iter < &cc_handlers_stop; ++cc_Handler_Iter)
        {
          if ((pCmdClassList->pList[i] == cc_Handler_Iter->CC) &&
              (NULL != cc_Handler_Iter->basic_set_mapper))
          {
            cc_Handler_Iter->basic_set_mapper(pCmd);

            /*
             * Invoke the corresponding handler. Since we know this is a set command, the output
             * parameters can be set to NULL.
             */
            return ZAF_CC_invoke_specific(cc_Handler_Iter, rxOpt, pCmd, cmdLength, NULL, NULL);
          }
        }
      }

      /*
       * At this point the Basic Set was not mapped by the application nor was a mapped function
       * found in the CC section.
       */
      return RECEIVED_FRAME_STATUS_NO_SUPPORT;
      break;

    case BASIC_GET:
      if (true == Check_not_legal_response_job(rxOpt))
      {
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      ZAF_TRANSPORT_TX_BUFFER  TxBuf;
      ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
      RxToTxOptions(rxOpt, &pTxOptionsEx);
      /* Controller wants the sensor level */
      pTxBuf->ZW_BasicReportFrame.cmdClass = COMMAND_CLASS_BASIC;
      pTxBuf->ZW_BasicReportFrame.cmd = BASIC_REPORT;

      pTxBuf->ZW_BasicReportV2Frame.currentValue =  CC_Basic_GetCurrentValue_handler(pTxOptionsEx->sourceEndpoint);
      pTxBuf->ZW_BasicReportV2Frame.targetValue =  CC_Basic_GetTargetValue_handler(pTxOptionsEx->sourceEndpoint);
      pTxBuf->ZW_BasicReportV2Frame.duration =  CC_Basic_GetDuration_handler(pTxOptionsEx->sourceEndpoint);
      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
          (uint8_t *)pTxBuf,
          sizeof(ZW_BASIC_REPORT_V2_FRAME),
          pTxOptionsEx,
          NULL))
      {
        /*Job failed */
        ;
      }
      break;

    default:
      return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }
  return RECEIVED_FRAME_STATUS_SUCCESS;
}

JOB_STATUS
CC_Basic_Report_tx(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  uint8_t bValue,
  void (*pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult))
{
  CMD_CLASS_GRP cmdGrp = {COMMAND_CLASS_BASIC, BASIC_REPORT};

  return cc_engine_multicast_request(
      pProfile,
      sourceEndpoint,
      &cmdGrp,
      &bValue,
      1,
      true,
      pCbFunc);
}

/**************************************************************************************************
 * Linker magic - Creates a section for an array of registered CCs and mapped CCs to the Basic CC.
 *************************************************************************************************/

REGISTER_CC(COMMAND_CLASS_BASIC, BASIC_VERSION_V2, CC_Basic_handler);
