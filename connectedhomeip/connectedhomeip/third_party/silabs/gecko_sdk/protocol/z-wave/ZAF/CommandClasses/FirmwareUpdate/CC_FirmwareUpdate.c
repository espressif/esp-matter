/**
* @file
* Command Class Firmware Update source file.
* @copyright 2018 Silicon Laboratories Inc.
*/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <CC_FirmwareUpdate.h>
#include <ZW_TransportLayer.h>
#include <ota_util.h>
#include <CRC.h>
#include <string.h>
#include <CC_ManufacturerSpecific.h>
#include "ZAF_tx_mutex.h"
#include <zaf_config_api.h>

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

static received_frame_status_t
CC_FirmwareUpdate_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength)
{
  if (true == Check_not_legal_response_job(rxOpt))
  {
    // None of the following commands support endpoint bit addressing.
    DPRINT("RECEIVED_FRAME_STATUS_FAIL\n");
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  switch (pCmd->ZW_Common.cmd)
  {
    case FIRMWARE_MD_GET_V5:
    {
      ZAF_TRANSPORT_TX_BUFFER  TxBuf;
      ZW_APPLICATION_TX_BUFFER* pTxBuf = &(TxBuf.appTxBuf);
      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      uint8_t * pData;
      TRANSMIT_OPTIONS_TYPE_SINGLE_EX * pTxOptionsEx;
      RxToTxOptions(rxOpt, &pTxOptionsEx);

      uint16_t manufacturerID = 0;
      uint16_t productID      = 0;
      uint8_t numberOfFirmwareTargets;
      CC_ManufacturerSpecific_ManufacturerSpecificGet_handler(&manufacturerID, &productID);

      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5;
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.cmd = FIRMWARE_MD_REPORT_V5;
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.manufacturerId1 = (uint8_t)(manufacturerID >> 8);
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.manufacturerId2 = (uint8_t)(manufacturerID & 0xFF);
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.firmware0Id1 = (uint8_t)(handleFirmWareIdGetExtended(0) >> 8);
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.firmware0Id2 = (uint8_t)(handleFirmWareIdGetExtended(0) & 0xff);
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.firmware0Checksum1 = 0x00; // Checksum N/A for SDK7
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.firmware0Checksum2 = 0x00; // Checksum N/A for SDK7
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.firmwareUpgradable = 0xFF; // Hardcode to upgradable.
      numberOfFirmwareTargets = zaf_config_get_firmware_target_count();
      if(zaf_config_get_bootloader_upgradable()) {
        numberOfFirmwareTargets++;
      }
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.numberOfFirmwareTargets = numberOfFirmwareTargets - 1; /* -1 : Firmware version 0 */
      uint16_t maxFragmentSize = handleCommandClassFirmwareUpdateMaxFragmentSize();
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.maxFragmentSize1 = (uint8_t)(maxFragmentSize >> 8);
      pTxBuf->ZW_FirmwareMdReport1byteV5Frame.maxFragmentSize2 = (uint8_t)maxFragmentSize;
      pData = (uint8_t *)pTxBuf;
      uint8_t  i;
      for (i = 1; i < numberOfFirmwareTargets; i++)
      {
        *(pData + 10 + (2 * i)) = (uint8_t)(handleFirmWareIdGetExtended(i) >> 8);
        *(pData + 10 + (2 * i) + 1) = (uint8_t)(handleFirmWareIdGetExtended(i) & 0xff);
      }
      // Fill out the hardwareVersion field, located right after the 0-n extra firmwareId fields
      *(pData + 10 + (2 * i)) = zaf_config_get_hardware_version();

      if ( ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
             (uint8_t *)pTxBuf,
             (size_t)(10 + (2 * i) + 1),
             pTxOptionsEx,
             NULL))
      {
        /*Job failed */
        ;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
    }
    return RECEIVED_FRAME_STATUS_FAIL;
    break;
    case FIRMWARE_UPDATE_MD_REPORT_V5:
      {
        DPRINT("FIRMWARE_UPDATE_MD_REPORT_V5 - Report received!\n");

        uint16_t crc16Result = CRC_CheckCrc16(CRC_INITAL_VALUE, &(pCmd->ZW_Common.cmdClass), cmdLength);
        DPRINTF("crc16Result = 0x%04x (valid CRC = %s)\n", crc16Result, (crc16Result == 0) ? "true" : "false");

        uint16_t  firmwareUpdateReportNumber = ((uint16_t)(pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.properties1 &
                                      FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_REPORT_NUMBER_1_MASK_V5) << 8) +
                                     (uint16_t)(pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.reportNumber2);
        DPRINTF("Report number = 0x%04x (dec: %u)\n", firmwareUpdateReportNumber, firmwareUpdateReportNumber);

        uint8_t fw_actualFrameSize =  cmdLength -
                                  /* Calculate length of actual data1 field */
                                  (uint8_t)(sizeof(pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.cmdClass) +
                                   sizeof(pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.cmd) +
                                   sizeof(pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.properties1) +
                                   sizeof(pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.reportNumber2) +
                                   sizeof(pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.checksum1) +
                                   sizeof(pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.checksum2));
        DPRINTF("fw_actualFrameSize = 0x%02x\n", fw_actualFrameSize);

        handleCmdClassFirmwareUpdateMdReport(crc16Result,
                                             firmwareUpdateReportNumber,
                                             pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.properties1,
                                             &(pCmd->ZW_FirmwareUpdateMdReport1byteV5Frame.data1),
                                             fw_actualFrameSize);


      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;
    case FIRMWARE_UPDATE_MD_REQUEST_GET_V5:
      {
        ZAF_TRANSPORT_TX_BUFFER  TxBuf;
        ZW_APPLICATION_TX_BUFFER* pTxBuf = &(TxBuf.appTxBuf);
        memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

        TRANSMIT_OPTIONS_TYPE_SINGLE_EX * pTxOptionsEx;
        RxToTxOptions(rxOpt, &pTxOptionsEx);

        uint8_t status;
        handleCmdClassFirmwareUpdateMdReqGet(
            rxOpt,
            (ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V5_FRAME *)pCmd,
            cmdLength,
            &status);

        pTxBuf->ZW_FirmwareUpdateMdRequestReportV5Frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5;
        pTxBuf->ZW_FirmwareUpdateMdRequestReportV5Frame.cmd = FIRMWARE_UPDATE_MD_REQUEST_REPORT_V5;
        pTxBuf->ZW_FirmwareUpdateMdRequestReportV5Frame.status = status;

        if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
                (uint8_t *)pTxBuf,
                sizeof(ZW_FIRMWARE_UPDATE_MD_REQUEST_REPORT_V5_FRAME),
                pTxOptionsEx,
                ZCB_CmdClassFwUpdateMdReqReport))
        {
          ZCB_CmdClassFwUpdateMdReqReport(TRANSMIT_COMPLETE_FAIL);
        }
        return RECEIVED_FRAME_STATUS_SUCCESS;
      }
      return RECEIVED_FRAME_STATUS_FAIL;
      break;

    case FIRMWARE_UPDATE_ACTIVATION_SET_V5:
    {
      uint8_t status;
      if (true != CC_FirmwareUpdate_ActivationSet_handler((ZW_FIRMWARE_UPDATE_ACTIVATION_SET_V5_FRAME *)pCmd, &status))
      {
        ZAF_TRANSPORT_TX_BUFFER  TxBuf;
        ZW_APPLICATION_TX_BUFFER* pTxBuf = &(TxBuf.appTxBuf);
        memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER));

        TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx;
        RxToTxOptions(rxOpt, &pTxOptionsEx);

        memcpy(
            ((uint8_t*)pTxBuf),
            ((uint8_t*)pCmd),
            sizeof(ZW_FIRMWARE_UPDATE_ACTIVATION_SET_V5_FRAME) - 1);

        pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.cmd = FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V5;
        pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.firmwareUpdateStatus = FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_INVALID_COMBINATION_V5;
        pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.hardwareVersion = pCmd->ZW_FirmwareUpdateActivationSetV5Frame.hardwareVersion;
        pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.firmwareUpdateStatus = status;

        /*
         * Transmit a response if the activation was not successful.
         * We do not care about the return value because if we reach this point, the activation
         * failed and we will let the initiator try again.
         */
        Transport_SendResponseEP(
            (uint8_t *)pTxBuf,
            sizeof(ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V5_FRAME),
            pTxOptionsEx,
            NULL);
      }
    }
      break;
    case FIRMWARE_UPDATE_MD_PREPARE_GET_V5:
    {
      ZAF_TRANSPORT_TX_BUFFER  TxBuf;
      ZW_APPLICATION_TX_BUFFER* pTxBuf = &(TxBuf.appTxBuf);

      TRANSMIT_OPTIONS_TYPE_SINGLE_EX * pTxOptions = NULL;
      RxToTxOptions(rxOpt, &pTxOptions);

      pTxBuf->ZW_FirmwareUpdateMdPrepareReportV5Frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5;
      pTxBuf->ZW_FirmwareUpdateMdPrepareReportV5Frame.cmd = FIRMWARE_UPDATE_MD_PREPARE_REPORT_V5;

      pTxBuf->ZW_FirmwareUpdateMdPrepareReportV5Frame.status = FIRMWARE_UPDATE_MD_PREPARE_REPORT_NOT_DOWNLOADABLE_V5;

      // The firmware checksum is irrelevant when we report that it's not downloadable.
      pTxBuf->ZW_FirmwareUpdateMdPrepareReportV5Frame.firmwareChecksum1 = 0x00;
      pTxBuf->ZW_FirmwareUpdateMdPrepareReportV5Frame.firmwareChecksum2 = 0x00;

      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
          (uint8_t *)pTxBuf,
          sizeof(ZW_FIRMWARE_UPDATE_MD_PREPARE_REPORT_V5_FRAME),
          pTxOptions,
          NULL))
      {
        /*Job failed */
        ;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
    }
      break;
    default:
      break;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}

JOB_STATUS
CmdClassFirmwareUpdateMdStatusReport(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  uint8_t status,
  uint16_t waitTime,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult))
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

  TRANSMIT_OPTIONS_TYPE_SINGLE_EX* pTxOptionsEx = NULL;
  RxToTxOptions(rxOpt, &pTxOptionsEx);
  /* Send status, when finished */

  pTxBuf->ZW_FirmwareUpdateMdStatusReportV5Frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5;
  pTxBuf->ZW_FirmwareUpdateMdStatusReportV5Frame.cmd = FIRMWARE_UPDATE_MD_STATUS_REPORT_V5;
  pTxBuf->ZW_FirmwareUpdateMdStatusReportV5Frame.status = status;
  pTxBuf->ZW_FirmwareUpdateMdStatusReportV5Frame.waittime1 = (uint8_t)(waitTime >> 8);
  pTxBuf->ZW_FirmwareUpdateMdStatusReportV5Frame.waittime2 = (uint8_t)(waitTime);
  if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendRequestEP(
      (uint8_t *)pTxBuf,
      sizeof(ZW_FIRMWARE_UPDATE_MD_STATUS_REPORT_V5_FRAME),
      pTxOptionsEx,
      pCbFunc))
  {
    TRANSMISSION_RESULT result = {0xff, TRANSMIT_COMPLETE_FAIL, TRANSMISSION_RESULT_FINISHED};
    pCbFunc(&result);
    return JOB_STATUS_BUSY;
  }
  return JOB_STATUS_SUCCESS;
}

JOB_STATUS
CmdClassFirmwareUpdateMdGet(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  uint16_t firmwareUpdateReportNumber,
  ZAF_TX_Callback_t pCbFunc)
{
  DPRINTF("Transmit Md Get CMD with report number %d\n", firmwareUpdateReportNumber);

  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

  TRANSMIT_OPTIONS_TYPE_SINGLE_EX* pTxOptionsEx = NULL;
  RxToTxOptions(rxOpt, &pTxOptionsEx);

  /* Ask for the next report */
  pTxBuf->ZW_FirmwareUpdateMdGetV5Frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5;
  pTxBuf->ZW_FirmwareUpdateMdGetV5Frame.cmd = FIRMWARE_UPDATE_MD_GET_V5;
  pTxBuf->ZW_FirmwareUpdateMdGetV5Frame.numberOfReports = getFWUpdateMDGetNumberOfReports();
  pTxBuf->ZW_FirmwareUpdateMdGetV5Frame.properties1 = (uint8_t)(firmwareUpdateReportNumber >> 8);
  pTxBuf->ZW_FirmwareUpdateMdGetV5Frame.reportNumber2 = (uint8_t)(firmwareUpdateReportNumber);
  if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendRequestEP(
      (uint8_t *)pTxBuf,
      sizeof(ZW_FIRMWARE_UPDATE_MD_GET_V5_FRAME),
      pTxOptionsEx,
      pCbFunc))
  {
    return JOB_STATUS_BUSY;
  }
  return JOB_STATUS_SUCCESS;
}

JOB_STATUS CC_FirmwareUpdate_ActivationStatusReport_tx(
    RECEIVE_OPTIONS_TYPE_EX * rxOpt,
    uint16_t checksum,
    uint8_t status)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

  TRANSMIT_OPTIONS_TYPE_SINGLE_EX* pTxOptionsEx = NULL;
  RxToTxOptions(rxOpt, &pTxOptionsEx);

  uint16_t manufacturerID = 0;
  uint16_t productID      = 0;
  CC_ManufacturerSpecific_ManufacturerSpecificGet_handler(&manufacturerID, &productID);

  // Activation supports the Z-Wave firmware only.
  const uint8_t FIRMWARE_TARGET = 0;

  uint16_t firmwareID = handleFirmWareIdGetExtended(FIRMWARE_TARGET);

  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.cmdClass = COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5;
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.cmd = FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V5;
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.manufacturerId1 = (uint8_t)(manufacturerID >> 8);
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.manufacturerId2 = (uint8_t)(manufacturerID & 0xFF);
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.firmwareId1 = (uint8_t)(firmwareID >> 8);
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.firmwareId2 = (uint8_t)(firmwareID & 0xff);
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.checksum1 = (uint8_t)(checksum >> 8);
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.checksum2 = (uint8_t)(checksum);
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.firmwareTarget = FIRMWARE_TARGET;
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.firmwareUpdateStatus = status;
  pTxBuf->ZW_FirmwareUpdateActivationStatusReportV5Frame.hardwareVersion = zaf_config_get_hardware_version();

  if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendRequestEP(
      (uint8_t *)pTxBuf,
      sizeof(ZW_FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_V5_FRAME),
      pTxOptionsEx,
      NULL))
  {
    return JOB_STATUS_BUSY;
  }
  return JOB_STATUS_SUCCESS;
}

REGISTER_CC(COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5, FIRMWARE_UPDATE_MD_VERSION_V5, CC_FirmwareUpdate_handler);
