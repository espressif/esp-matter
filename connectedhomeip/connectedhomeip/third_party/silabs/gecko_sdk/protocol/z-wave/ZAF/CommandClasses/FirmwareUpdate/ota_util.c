/**
 * @file
 * This module implements functions used in combination with command class firmware update.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <ota_util.h>
#include <ZW_TransportLayer.h>

#include <CRC.h>

#include <CC_FirmwareUpdate.h>
#include <CC_ManufacturerSpecific.h>

#include <AppTimer.h>
#include <SwTimer.h>

#include <string.h>
#include <ZAF_Common_interface.h>
#include <ZAF_transport.h>

#include <zpal_watchdog.h>
#include <zpal_misc.h>
#include <zpal_bootloader.h>
#include <zpal_power_manager.h>

#include "stdlib.h"
#include <ZAF_file_ids.h>
#include <ZAF_nvm_app.h>
#include <zaf_config_api.h>
#include <SizeOf.h>

//#define DEBUGPRINT
#include "DebugPrint.h"


/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/// Possible states of OTA update.
typedef enum _FW_STATE_
{
  FW_STATE_IDLE,          //!< OTA not active
  FW_STATE_READY,         //!< OTA is ready to start
  FW_STATE_AWAIT_REPORT,  //!< OTA is in progress and waiting for next MD Report
  FW_STATE_AWAIT_TIMEOUT, //!< FW installing is delayed waiting for timer timeout
} FW_STATE;

/// Possible Events of OTA update.
typedef enum _FW_EVENT_
{
  FW_EVENT_REQ_GET_RECEIVED_VALID,        //!< FW_EVENT_REQ_GET_RECEIVED_VALID
  FW_EVENT_REQ_GET_RECEIVED_INVALID,      //!< FW_EVENT_REQ_GET_RECEIVED_INVALID
  FW_EVENT_REQ_REPORT_GOT_ACK,            //!< FW_EVENT_REQ_REPORT_GOT_ACK
  FW_EVENT_REQ_REPORT_NO_ACK,             //!< FW_EVENT_REQ_REPORT_NO_ACK
  FW_EVENT_MAX_RETRIES_REACHED,           //!< FW_EVENT_MAX_RETRIES_REACHED
  FW_EVENT_REPORT_RECEIVED_VALID,         //!< FW_EVENT_REPORT_RECEIVED_VALID
  FW_EVENT_REPORT_RECEIVED_INVALID,       //!< FW_EVENT_REPORT_RECEIVED_INVALID
  FW_EVENT_REPORT_RECEIVED_BATCH,         //!< FW_EVENT_REPORT_RECEIVED_BATCH
  FW_EVENT_REPORT_RECEIVED_LAST,          //!< FW_EVENT_REPORT_RECEIVED_LAST
  FW_EVENT_REPORT_RECEIVED_LAST_INVALID,  //!< FW_EVENT_REPORT_RECEIVED_LAST_INVALID
  FW_EVENT_DELAY_FW_INSTALL,              //!< FW_EVENT_DELAY_FW_INSTALL
  FW_EVENT_START_FW_INSTALL               //!< FW_EVENT_START_FW_INSTALL
} FW_EVENT;

/// Possible actions during OTA update.
typedef void(*fw_action)(void);

/**
 * Single OTA transition.
 * When event happens, OTA makes the action and goes from state to new_state.
 */
typedef struct _OTA_transition_{
  FW_STATE state;     //!< current state
  FW_EVENT event;     //!< Defined event that might happen in the current state
  fw_action action;   //!< Action to make when event happens
  FW_STATE new_state; //!< next state to transition to.
} OTA_transition;


//This version number must be increased if we make changes in the struct SFirmwareUpdateFile
#define FIRMWARE_UPDATE_FILE_VERSION  0x01

//Sizes of SFirmwareUpdateFile and SFirmwareUpdateFile_DEPRECATED_V0 must not be equal for current automatic file migration to work
STATIC_ASSERT(sizeof(SFirmwareUpdateFile) > sizeof(SFirmwareUpdateFile_DEPRECATED_V0), STATIC_ASSERT_FAILED_file_migration_failure);

#define FIRMWARE_UPDATE_REQUEST_TIMEOUTS         10000   /* unit: 1 ms ticks */
#define FIRMWARE_UPDATE_MAX_RETRY                   10
/* Used for extending FIRMWARE_UPDATE_REQUEST_TIMEOUTS on MdGet retries - in 1ms ticks */
#define FIRMWARE_UPDATE_REQUEST_TIMEOUT_RETRY_INC 1500

#define ACTIVATION_SUPPORT_MASK_APP        0x80U
#define ACTIVATION_SUPPORT_MASK_INITIATOR  0x01U
#define ACTIVATION_SUPPORT_ENABLED_MASK    0x81U

#define OTA_CACHE_SIZE 200

/**
 * A set of defined Power Management lock periods for use during OTA.
 */
#define OTA_AWAKE_PERIOD_LONG_TERM         2400000  // 40 min
#define OTA_AWAKE_PERIOD_GRACEFUL_OFF      100      // 100 ms

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

// Used for keeping device awake during OTA.
static zpal_pm_handle_t m_radioPowerLock;

/**
 * Internal storage for incoming FW Update MD Reports.
 * Should be big enough to store at least two incoming frames.
 * If not, then frames are written directly to flash and the storage is not used
 */
static uint8_t mdReportsStorage[OTA_CACHE_SIZE] __attribute__((aligned(4)));

/**
 * Number of Reports to request in single FW Update MD Get. Minimum is 1.
 * It's value is calculated upon receiving FW Update MD Request Get.
 * mdGetNumberOfReports = sizeof(mdReportsStorage)/(single fragment size)
 */
static uint8_t mdGetNumberOfReports;

/// Actual fragment size calculated upon receiving REQUEST GET.
static uint8_t firmware_update_packetsize;

typedef struct _OTA_UTIL_
{
  CC_FirmwareUpdate_start_callback_t pOtaStart;
  CC_FirmwareUpdate_finish_callback_t pOtaFinish;
  FW_STATE currentState;
  OTA_STATUS finishStatus;
  uint16_t firmwareCrc;
  uint8_t fw_numOfRetries;
  SSwTimer timerFwUpdateFrameGet;
  SSwTimer timerOtaSuccess;
  uint16_t firmwareUpdateReportNumberPrevious;  /// The last received report number. Also total number of received fragments from start of OTA.
  uint16_t fw_crcrunning;                       /// The CRC of the entire image incrementally calculated as the fragments are being received.
  RECEIVE_OPTIONS_TYPE_EX rxOpt;
  bool NVM_valid;
  bool MdGetDone;          /// If false Transmit Abort can be transmitted if MdReport received
  uint8_t activation_enabled;
  uint8_t requestReport;   /// Status to send in FW Update Request Report
  uint8_t statusReport;    /// Status to send in FW Update MD Status Report
  uint8_t reportsReceived; /// counter to keep track of how many reports are received so far during one multiFrame session. (not from start)
} OTA_UTIL;

static OTA_UTIL myOta = {
    .currentState = FW_STATE_IDLE,
    .finishStatus = OTA_STATUS_DONE,
    .firmwareCrc = 0,
    .fw_numOfRetries = 0,
    .firmwareUpdateReportNumberPrevious = 0,
    .fw_crcrunning = 0, // fw_crcrunning
    .rxOpt = {0}, // rxOpt
    .MdGetDone = true,
    .activation_enabled = 0, // activation_enabled
    .requestReport = FIRMWARE_UPDATE_MD_REQUEST_REPORT_VALID_COMBINATION_V5, // requestReport
    .statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_V5, // statusReport
    .reportsReceived = 0 // reportsReceived
};

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/
static void initOTAState();
static void TimerCancelFwUpdateFrameGet();
static void TimerStartFwUpdateFrameGet();

static void ZCB_TimerOutFwUpdateFrameGet(SSwTimer* pTimer);
static void ZCB_FinishFwUpdate(TRANSMISSION_RESULT * pTransmissionResult);
static void ZCB_VerifyImage(SSwTimer* pTimer);

static void UpdateStatusSuccess();
static void SendFirmwareUpdateStatusReport();

static void handleEvent(uint8_t event);
static void fw_action_send_get(void);
static void fw_action_send_req_report(void);
static void fw_action_send_status_report(void);
static void fw_action_none(void);
static void fw_action_verify_image(void);
static void fw_action_reboot_and_Install(void);
#ifdef DEBUGPRINT
static char* getStateAsString(FW_STATE state);
static char* getEventAsString(FW_EVENT event);
#endif //DEBUGPRINT
static void resetReceivedReportsData();
static bool useMultiFrames();

/// Transition table with all supported state-event combinations.
static const OTA_transition OTA_transition_table[] = {
  {FW_STATE_IDLE,          FW_EVENT_DELAY_FW_INSTALL,             &fw_action_none,               FW_STATE_AWAIT_TIMEOUT},
  {FW_STATE_IDLE,          FW_EVENT_REQ_GET_RECEIVED_VALID,       &fw_action_send_req_report,    FW_STATE_READY},
  {FW_STATE_IDLE,          FW_EVENT_REQ_GET_RECEIVED_INVALID,     &fw_action_send_req_report,    FW_STATE_IDLE},
  {FW_STATE_IDLE,          FW_EVENT_START_FW_INSTALL,             &fw_action_reboot_and_Install, FW_STATE_IDLE},
  {FW_STATE_AWAIT_TIMEOUT, FW_EVENT_START_FW_INSTALL,             &fw_action_reboot_and_Install, FW_STATE_IDLE},
  {FW_STATE_READY,         FW_EVENT_REQ_REPORT_GOT_ACK,           &fw_action_send_get,           FW_STATE_AWAIT_REPORT},
  {FW_STATE_READY,         FW_EVENT_REQ_REPORT_NO_ACK,            &fw_action_none,               FW_STATE_IDLE},
  {FW_STATE_AWAIT_REPORT,  FW_EVENT_REPORT_RECEIVED_INVALID,      &fw_action_send_get,           FW_STATE_AWAIT_REPORT},
  {FW_STATE_AWAIT_REPORT,  FW_EVENT_REPORT_RECEIVED_VALID,        &fw_action_none,               FW_STATE_AWAIT_REPORT},
  {FW_STATE_AWAIT_REPORT,  FW_EVENT_MAX_RETRIES_REACHED,          &fw_action_send_status_report, FW_STATE_IDLE},
  {FW_STATE_AWAIT_REPORT,  FW_EVENT_REPORT_RECEIVED_BATCH,        &fw_action_send_get,           FW_STATE_AWAIT_REPORT},
  {FW_STATE_AWAIT_REPORT,  FW_EVENT_REPORT_RECEIVED_LAST,         &fw_action_verify_image,       FW_STATE_IDLE},
  {FW_STATE_AWAIT_REPORT,  FW_EVENT_REPORT_RECEIVED_LAST_INVALID, &fw_action_send_status_report, FW_STATE_IDLE},
};


static inline bool ActivationIsEnabled(void)
{
  return (ACTIVATION_SUPPORT_ENABLED_MASK == myOta.activation_enabled);
}

bool CC_FirmwareUpdate_Init(
    CC_FirmwareUpdate_start_callback_t pOtaStart,
    CC_FirmwareUpdate_finish_callback_t pOtaFinish,
    bool support_activation)
{
  zpal_status_t retvalue;

  zpal_bootloader_info_t bloaderInfo;

  myOta.pOtaStart = pOtaStart;
  myOta.pOtaFinish = pOtaFinish;
  myOta.NVM_valid = true;

  mdGetNumberOfReports = 1;

  m_radioPowerLock = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);

  if (true == support_activation) {
    myOta.activation_enabled |= ACTIVATION_SUPPORT_MASK_APP;
  } else {
    myOta.activation_enabled &= (uint8_t)(~ACTIVATION_SUPPORT_MASK_APP);
  }


  retvalue = zpal_bootloader_init();
  if(retvalue != ZPAL_STATUS_OK)
  {
    DPRINTF("\r\nBootloader NOT OK! %x", retvalue);
    myOta.NVM_valid = false;
  }
  /* Checking the bootloader validity before proceed, if it is non silabs bootloader then make it non upgradable */
  zpal_bootloader_get_info(&bloaderInfo);
  if(bloaderInfo.type != ZPAL_BOOTLOADER_PRESENT)
  {
     DPRINTF("\r\nNo bootloader is present or non silabs bootloader hence it's not upgradable type =%x",bloaderInfo.type);
     myOta.NVM_valid = false;
  }
  /*Checking this bootloader has storage capablity or not, just in case a wrong bootloader been loaded into the device*/
  if(!(bloaderInfo.capabilities & ZPAL_BOOTLOADER_CAPABILITY_STORAGE))
  {
     DPRINT("\r\nThis bootloader do not have storage capablity hence it can't be used for OTA");
     myOta.NVM_valid = false;
  }

  zpal_status_t status = ZPAL_STATUS_FAIL;
  size_t dataLen = 0;
  zpal_nvm_handle_t pFileSystem = ZAF_GetFileSystemHandle();
  status = zpal_nvm_get_object_size(pFileSystem,
                                    ZAF_FILE_ID_CC_FIRMWARE_UPDATE,
                                    &dataLen);

  if (ZPAL_STATUS_OK != status)
  {
    DPRINT("\nFile default!");
    SFirmwareUpdateFile file;
    memset(&file, 0, sizeof(file));
    file.fileVersion = FIRMWARE_UPDATE_FILE_VERSION;
    dataLen = ZAF_FILE_SIZE_CC_FIRMWARE_UPDATE;
    status = zpal_nvm_write(pFileSystem,
                             ZAF_FILE_ID_CC_FIRMWARE_UPDATE,
                             &file,
                             dataLen);
    ASSERT(ZPAL_STATUS_OK == status);
  }

  bool updated_successfully = false;
  if (zpal_bootloader_is_first_boot(&updated_successfully))
  {
    DPRINT("\nFIRMWARE UPDATE DONE NOW!");

    SFirmwareUpdateFile file;
    status = zpal_nvm_read(pFileSystem,
                            ZAF_FILE_ID_CC_FIRMWARE_UPDATE,
                            &file,
                            dataLen);
    ASSERT(ZPAL_STATUS_OK == status);

    DPRINTF("\nF INIT: %x", file.activation_was_applied);

    if (sizeof(SFirmwareUpdateFile_DEPRECATED_V0) == dataLen)
    {
      //Do automatic file migration.
      SFirmwareUpdateFile_DEPRECATED_V0 oldFile;
      memcpy((uint8_t *)&oldFile, (uint8_t *)&file, sizeof(oldFile));

      file.activation_was_applied = oldFile.activation_was_applied;
      file.checksum    = oldFile.checksum;
      file.srcNodeID   = oldFile.srcNodeID;
      file.srcEndpoint = oldFile.srcEndpoint;
      file.rxStatus    = oldFile.rxStatus;
      file.securityKey = oldFile.securityKey;

      status = zpal_nvm_write(pFileSystem, ZAF_FILE_ID_CC_FIRMWARE_UPDATE, &file, ZAF_FILE_SIZE_CC_FIRMWARE_UPDATE);
      ASSERT(ZPAL_STATUS_OK == status);
    }

    RECEIVE_OPTIONS_TYPE_EX rxOpt;
    rxOpt.sourceNode.nodeId   = file.srcNodeID;
    /*
     * Ignore bitfield conversion warnings as there is no good solution other than stop
     * using bitfields.
     */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    rxOpt.sourceNode.endpoint = file.srcEndpoint;
#pragma GCC diagnostic pop
    rxOpt.rxStatus            = file.rxStatus;
    rxOpt.securityKey         = file.securityKey;
    rxOpt.destNode.endpoint   = 0; // Firmware update is part of the root device.

    if (ACTIVATION_SUPPORT_ENABLED_MASK == file.activation_was_applied)
    {
      // TX Activation Status Report including checksum
      DPRINT("\nTX Activation Status Report!");

      uint8_t status;
      if (updated_successfully)
      {
        status = FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_FIRMWARE_UPDATE_COMPLETED_SUCCESSFULLY_V5;
      }
      else
      {
        status = FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_ERROR_ACTIVATING_THE_FIRMWARE_V5;
      }
      CC_FirmwareUpdate_ActivationStatusReport_tx(&rxOpt, file.checksum, status);
    }
    else
    {
      uint8_t status;
      if (updated_successfully)
      {
        status = FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_V5;
      }
      else
      {
        status = FIRMWARE_UPDATE_MD_STATUS_REPORT_INVALID_FILE_HEADER_INFORMATION_V5;
      }
      // Tx Status Report
      DPRINT("\nTX Status Report!");
      CmdClassFirmwareUpdateMdStatusReport(&rxOpt,
                                           status,
                                           0,
                                           NULL);
    }
  }

  // Register timer for re-sending FW Update MD Get
  //DPRINT("Registering timer for timerFwUpdateFrameGeT\n");
  bool timerRegisterStatus = AppTimerRegister(&myOta.timerFwUpdateFrameGet,
                                              false,
                                              ZCB_TimerOutFwUpdateFrameGet);
  ASSERT(timerRegisterStatus);

  if(AppTimerRegister(&myOta.timerOtaSuccess, false, ZCB_VerifyImage))
  {
    DPRINT("\r\n**Registering timer OK for last report...**");
  }
  else
  {
    DPRINT("\r\n**Registering timer Failed for  the last report**");
  }
  DPRINTF("\r\nInit including bootloader init finished--bootloader init status 0x%x\n", retvalue);
  return myOta.NVM_valid;
}

/*======================== UpdateStatusSuccess ==========================
** Function to set ev state after successful verification of image.
**
** Side effects:
**
**-------------------------------------------------------------------------*/
static void UpdateStatusSuccess()
{
  DPRINT("OTA_SUCCESS_CB");

  if (ActivationIsEnabled())
  {
    myOta.statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_WAITING_FOR_ACTIVATION_V5;
  }
  else
  {
    /* send FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_V5 to controller.
       Device reboot itself*/
    myOta.statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_V5;
  }
  SendFirmwareUpdateStatusReport();
  myOta.finishStatus = OTA_STATUS_DONE;
}

/*======================== ZCB_VerifyImage ==========================
** Timer callback to start image verification *after* we have ack/routed-ack'ed
** the last fw update frame.
**
** Side effects:
**
**-------------------------------------------------------------------------*/
static void ZCB_VerifyImage(SSwTimer* pTimer)
{
  UNUSED(pTimer);
  if (FW_STATE_AWAIT_TIMEOUT == myOta.currentState) {
    handleEvent(FW_EVENT_START_FW_INSTALL);
  } else {
    handleEvent(FW_EVENT_REPORT_RECEIVED_LAST);
  }

}

static ESwTimerStatus
timerFwUpdateFrameGetStart(SSwTimer* pTimer)
{
  DPRINT("Start or reset Md Get CMD timer...\n");
  if (TimerIsActive(pTimer))
  {
    TimerStop(pTimer);
  }
  return (TimerStart(pTimer, (uint32_t)(FIRMWARE_UPDATE_REQUEST_TIMEOUTS +
                              (FIRMWARE_UPDATE_REQUEST_TIMEOUT_RETRY_INC * myOta.fw_numOfRetries))));
}

void
handleCmdClassFirmwareUpdateMdReport( uint16_t crc16Result,
                                      uint16_t firmwareUpdateReportNumber,
                                      uint8_t  properties,
                                      uint8_t* pData,
                                      uint8_t  fw_actualFrameSize)
{
  zpal_status_t zpal_status;
  DPRINT("handleCmdClassFirmwareUpdateMdReport()\n");

  /* Frame burst during OTA can cause watch dog to reset */
  zpal_feed_watchdog();

  // Ignore FW Update MD Report if OTA is not in progress
  // handleEvent() would handle any unexpected events anyway.
  // Purpose of this check is just to speed up the process.
  if( FW_STATE_AWAIT_REPORT != myOta.currentState)
  {
    /*Not correct state.. just stop*/
    DPRINTF("Received MD Report in wrong state %d, expected was %d. Stop.\n",
            myOta.currentState, FW_STATE_AWAIT_REPORT);
    return;
  }
  /*Check checksum*/
  DPRINTF(" (CRC----: 0x%04X)\r\n", crc16Result);
  if (0 != crc16Result)
  {
    DPRINT("CRC invalid\r\n");
    myOta.statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_WITHOUT_CHECKSUM_ERROR_V5;
    return;
  }

  // CRC of the received frame OK. Reset retries and continue.
  myOta.fw_numOfRetries = 0;

  /* Check report number */
  if (firmwareUpdateReportNumber != myOta.firmwareUpdateReportNumberPrevious + 1)
  {
    DPRINT("Report Number is not as expected (invalid)\n");
    // (firmwareUpdateReportNumber == myOta.firmwareUpdateReportNumberPrevious + 1) do not match.
    // Set Status value and let the timer handle retries
    myOta.statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_V5;
    /* 
     * FIXME With MultiFrame, the module will still be waiting for the report number that failed here.
     * Later reports with bumped report numbers will be discarted with error "report number not as expected".
     */
    return;  // Stop processing the received report and a timeout will be triggered for a new Md Get CMD.
  }

  /* Right report number, continue */
  if ((firmware_update_packetsize != fw_actualFrameSize) &&
      !(properties & FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_LAST_BIT_MASK))
  {
    DPRINTF("ERROR: Wrong fragment size. expected: %% 0x%x - got: 0x%x\r\n", firmware_update_packetsize, fw_actualFrameSize);
    // (firmware_update_packetsize != fw_actualFrameSize) and not last packet - do not match.
    // Set Status value and let the timer handle retries
    myOta.statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_V5;
    // All non-last fragments must have the same length. (This could also indicate a controller issue)
    return;  // Stop processing the received report and a timeout will be triggered for a new Md Get CMD.
  }

  if (false == myOta.MdGetDone)
  {
    /* As we have received a valid Md Report frame we can ask the protocol to Abort the ongoing
     * Md Get transmit that maybe is requesting the same report that has just been received. */
    ZAF_transportSendDataAbort();
    myOta.MdGetDone = true;
  }

  // Valid Report - Extend MdGet timeout (resetting timer for sending the next Md Get command)
  timerFwUpdateFrameGetStart(&myOta.timerFwUpdateFrameGet);

  /*
   * We have accepted the received report as valid and will utilize it.
   */

  uint32_t startAddress;

  /**
   * Store in buffer for multi-frame reception or immediately write to flash.
   */
  if (useMultiFrames())
  {
    // If mdReportsStorage size is > 1, then we expect multiple frames after single MD Get.
    // If so, write all incoming frames into mdReportsStorage,
    // and write entire content of it into NVM when all of them have been received
    startAddress = myOta.reportsReceived * firmware_update_packetsize;
    memcpy(&mdReportsStorage[startAddress], pData, fw_actualFrameSize);
  }
  else
  {
    // Otherwise, write data directly to flash
    // Using mdReportsStorage to ensure 32-bit alignment
    startAddress = ((uint32_t)(firmwareUpdateReportNumber - 1) * firmware_update_packetsize);
    memcpy(mdReportsStorage, pData, fw_actualFrameSize);
    zpal_status = zpal_bootloader_write_data(startAddress, mdReportsStorage, fw_actualFrameSize);
    ASSERT(zpal_status == ZPAL_STATUS_OK);
  }

  /**
   * Update OTA instance parameters to reflect the received Md Report CMD frame.
   */

  myOta.reportsReceived++;
  myOta.firmwareUpdateReportNumberPrevious = firmwareUpdateReportNumber;

  // Do CRC calculation incrementally as the fragments are being received.
  myOta.fw_crcrunning = CRC_CheckCrc16(myOta.fw_crcrunning, pData, fw_actualFrameSize);


  /**
   * Make use of the fragment that was received by storing it in buffer or flash.
   */

  // Is this the last report ?
  if (properties & FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_LAST_BIT_MASK)
  {
    /*check CRC for received dataBuffer*/
    if (myOta.fw_crcrunning != myOta.firmwareCrc)
    {
      DPRINTF("** OTA_FAIL complete image CRC!! ** 0x%x - 0x%x\r\n", myOta.fw_crcrunning, myOta.firmwareCrc);
      myOta.statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_WITHOUT_CHECKSUM_ERROR_V5;  // Also used for fragments CRC error.
      /* Last bit set, but CRC check failed */
      handleEvent(FW_EVENT_REPORT_RECEIVED_LAST_INVALID);  // Image transfer was completed, but the image CRC was a mismatch.
      /**
       * FW_EVENT_REPORT_RECEIVED_LAST_INVALID will terminate the OTA process and send the status report.
       */
      return;
    }

    DPRINT("** OTA_SUCCESS_CRC **");

    if (useMultiFrames())
    {
      // Calculate length: all reports except for last are full size
      // Last report might be shorter
      uint16_t len = (uint16_t)((myOta.reportsReceived - 1) * firmware_update_packetsize
          + fw_actualFrameSize);

      // Data is written to flash when the number of fragments as stated in mdGetNumberOfReports is received.

      // Find the starting address for writing to flash
      startAddress = ((uint32_t)(firmwareUpdateReportNumber - myOta.reportsReceived) * firmware_update_packetsize);
      zpal_status = zpal_bootloader_write_data(startAddress, mdReportsStorage, len);
      ASSERT(zpal_status == ZPAL_STATUS_OK);
    }
    // Delay verification of the firmware image
    // so we can transmit the ack or routed ack first
    if(ESWTIMER_STATUS_FAILED == TimerStart(&myOta.timerOtaSuccess, 100))
    {
      DPRINT("OTA_SUCCESS_NOTIMER");
      handleEvent(FW_EVENT_REPORT_RECEIVED_LAST);
    }
  }
  else  // Not the last report.
  {
    if (myOta.reportsReceived < mdGetNumberOfReports)  // Wait for additional reports? (mdReportsStorage is full)
    {
      // Waiting for more reports
      DPRINT("FW_EVENT_REPORT_RECEIVED_VALID\n");
      handleEvent(FW_EVENT_REPORT_RECEIVED_VALID);
    }
    else  // Don't wait for additional reports. The number of reports per Md Get request is received.
    {
      // If multi frames are used, write to flash now.
      if (useMultiFrames())
      {
        // Calculate length: number of received * size of one report
        uint16_t len = myOta.reportsReceived * firmware_update_packetsize;

        // Find the starting address for writing to flash
        startAddress = ((uint32_t)(firmwareUpdateReportNumber - myOta.reportsReceived) * firmware_update_packetsize);
        zpal_status = zpal_bootloader_write_data(startAddress, mdReportsStorage, len);
        ASSERT(zpal_status == ZPAL_STATUS_OK);
      }
      else
      {
        // MultiFrames were not used, therefore all fragments are already written to flash.
      }
      DPRINT("FW_EVENT_REPORT_RECEIVED_BATCH --> This triggers new Md Get CMD\n");
      handleEvent(FW_EVENT_REPORT_RECEIVED_BATCH);
    }
  }
}

static uint16_t 
handleBootloaderFirmWareIdGet()
{
  zpal_bootloader_info_t bootloader_info;
  zpal_bootloader_get_info(&bootloader_info);
  return ((bootloader_info.version & ZPAL_BOOTLOADER_VERSION_BUGFIX_MASK) >> ZPAL_BOOTLOADER_VERSION_BUGFIX_SHIFT);
}

uint16_t 
handleFirmWareIdGetExtended(uint8_t n)
{
  if(zaf_config_get_bootloader_upgradable() && zaf_config_get_bootloader_target_id() == n) {
    return handleBootloaderFirmWareIdGet();    
  } else {
    return handleFirmWareIdGet(n);
  }  
}

void handleCmdClassFirmwareUpdateMdReqGet(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V5_FRAME * pFrame,
  uint8_t cmdLength,
  uint8_t* pStatus)
{
  uint8_t fwTarget = 0;
  uint32_t fragmentSize = 0xFFFFFFFF;

  /* Verify if the FragmentSize and FirmwareTarget fields are part of the command (V3 and onwards) */
  if (sizeof(ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V3_FRAME) <= cmdLength)
  {
    fwTarget = pFrame->firmwareTarget;
    fragmentSize = 0;
    fragmentSize +=  (((uint32_t)pFrame->fragmentSize1) << 8);
    fragmentSize +=  (((uint32_t)pFrame->fragmentSize2) & 0xff);
  }

  if(0 != fwTarget)
  {
    DPRINT("** External fwExtern is not supported, aborting.**");
    *pStatus = FIRMWARE_UPDATE_MD_REQUEST_REPORT_NOT_UPGRADABLE_V5;
    handleEvent(FW_EVENT_REQ_GET_RECEIVED_INVALID);
    return;
  }

  if (sizeof(ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V4_FRAME) <= cmdLength)
  {
    // Activation bit
    if (pFrame->properties1 & FIRMWARE_UPDATE_MD_REQUEST_GET_PROPERTIES1_ACTIVATION_BIT_MASK_V5)
    {
      myOta.activation_enabled |= ACTIVATION_SUPPORT_MASK_INITIATOR;
    }
    else myOta.activation_enabled &= (uint8_t)~ACTIVATION_SUPPORT_MASK_INITIATOR;
  }

  /* Validate the hardwareVersion field (V5 and onwards) */
  uint8_t hardwareVersion = zaf_config_get_hardware_version();
  if ((sizeof(ZW_FIRMWARE_UPDATE_MD_REQUEST_GET_V5_FRAME) <= cmdLength) &&
      (hardwareVersion != pFrame->hardwareVersion))
  {
    /* Invalid hardware version */
    *pStatus = FIRMWARE_UPDATE_MD_REQUEST_REPORT_INVALID_HARDWARE_VERSION_V5;
    myOta.requestReport = *pStatus;
    handleEvent(FW_EVENT_REQ_GET_RECEIVED_INVALID);
    return;
  }

  if (pFrame->firmwareTarget >= zaf_config_get_firmware_target_count())
  {
    /*wrong target!!*/
    *pStatus = FIRMWARE_UPDATE_MD_REQUEST_REPORT_NOT_UPGRADABLE_V5;
    myOta.requestReport = *pStatus;
    handleEvent(FW_EVENT_REQ_GET_RECEIVED_INVALID);
    return;
  }

  uint16_t manufacturerID = 0;
  uint16_t productID      = 0;
  CC_ManufacturerSpecific_ManufacturerSpecificGet_handler(&manufacturerID, &productID);

  uint32_t maxFragmentSize = (uint32_t)handleCommandClassFirmwareUpdateMaxFragmentSize() & 0x0000FFFF;

  if (0xFFFFFFFF == fragmentSize)
  {
    // The Request Get command did not contain a fragment size => Set it to max fragment size.
    fragmentSize = maxFragmentSize;
  }
  else if ((fragmentSize > maxFragmentSize) || (0 == fragmentSize))
  {
    /*
     * The fragment size given in Request Get was too high or zero.
     * Report status of invalid fragment size.
     */
    *pStatus = FIRMWARE_UPDATE_MD_REQUEST_REPORT_INVALID_FRAGMENT_SIZE_V5;
    myOta.requestReport = *pStatus;
    handleEvent(FW_EVENT_REQ_GET_RECEIVED_INVALID);
    return;
  }

  uint16_t manufacturerIdIncoming = (uint16_t)((((uint16_t)pFrame->manufacturerId1) << 8)
                                    | (uint16_t)pFrame->manufacturerId2);
  uint16_t firmwareIdIncoming = (uint16_t)((((uint16_t)pFrame->firmwareId1) << 8)
                                | (uint16_t)pFrame->firmwareId2);
  uint16_t firmwareId = handleFirmWareIdGetExtended(fwTarget);
  if ((manufacturerIdIncoming != manufacturerID) ||
      (firmwareIdIncoming != firmwareId))
  {
    *pStatus = FIRMWARE_UPDATE_MD_REQUEST_REPORT_INVALID_COMBINATION_V5;
    myOta.requestReport = *pStatus;
    handleEvent(FW_EVENT_REQ_GET_RECEIVED_INVALID);
    return;
  }

  if (false == myOta.NVM_valid)
  {
    *pStatus = FIRMWARE_UPDATE_MD_REQUEST_REPORT_NOT_UPGRADABLE_V5;
    myOta.requestReport = *pStatus;
    handleEvent(FW_EVENT_REQ_GET_RECEIVED_INVALID);
    return;
  }

  uint16_t checksumIncoming = (uint16_t)((((uint16_t)pFrame->checksum1) << 8)
                              | (uint16_t)pFrame->checksum2);

  /*Firmware valid.. ask OtaStart to start update*/
  if (NON_NULL(myOta.pOtaStart) &&
      (false == myOta.pOtaStart(handleFirmWareIdGetExtended(fwTarget), checksumIncoming)))
  {
    DPRINT("&");
    *pStatus = FIRMWARE_UPDATE_MD_REQUEST_REPORT_REQUIRES_AUTHENTICATION_V5;
    myOta.requestReport = *pStatus;
    handleEvent(FW_EVENT_REQ_GET_RECEIVED_INVALID);
    return;
  }

  // Keep awake for a long time, but not forever.
  zpal_pm_stay_awake(m_radioPowerLock, OTA_AWAKE_PERIOD_LONG_TERM);

  initOTAState();
  memcpy( (uint8_t*) &myOta.rxOpt, (uint8_t*)rxOpt, sizeof(RECEIVE_OPTIONS_TYPE_EX));

  // Save activation status, checksum and RX options
  SFirmwareUpdateFile file = {
                              .activation_was_applied = myOta.activation_enabled,
                              .fileVersion = FIRMWARE_UPDATE_FILE_VERSION,
                              .checksum = checksumIncoming,
                              .srcNodeID = rxOpt->sourceNode.nodeId,
                              .srcEndpoint = rxOpt->sourceNode.endpoint,
                              .rxStatus = rxOpt->rxStatus,
                              .securityKey = (uint32_t)rxOpt->securityKey
  };
  zpal_nvm_handle_t pFileSystem = ZAF_GetFileSystemHandle();
  const zpal_status_t status = zpal_nvm_write(pFileSystem, ZAF_FILE_ID_CC_FIRMWARE_UPDATE, &file, ZAF_FILE_SIZE_CC_FIRMWARE_UPDATE);
  ASSERT(ZPAL_STATUS_OK == status);

  DPRINTF("\nF: %x", file.activation_was_applied);

  myOta.firmwareCrc = checksumIncoming;
  firmware_update_packetsize = (uint8_t)fragmentSize;

  // At this point maxFragmentSize is known => calculate Number of Reports
  // If storage is not big enough for at least 2 reports, don't enable multi data frames
  if (OTA_MULTI_FRAME_ENABLED
      && sizeof(mdReportsStorage) >= (2 * firmware_update_packetsize))
  {
    mdGetNumberOfReports = sizeof(mdReportsStorage) / firmware_update_packetsize;
  }
  DPRINTF("FW Update MD Get - Number Of Reports requested: %d\n", mdGetNumberOfReports);

  *pStatus = FIRMWARE_UPDATE_MD_REQUEST_REPORT_VALID_COMBINATION_V5;
  myOta.requestReport = *pStatus;

  handleEvent(FW_EVENT_REQ_GET_RECEIVED_VALID);
}


void ZCB_CmdClassFwUpdateMdReqReport(uint8_t txStatus)
{
  if (FIRMWARE_UPDATE_MD_REQUEST_REPORT_VALID_COMBINATION_V5 != myOta.requestReport)
  {
    // If the request get command failed, nothing was initiated. Hence, don't restart.
    return;
  }

  if ((TRANSMIT_COMPLETE_OK == txStatus) || (TRANSMIT_COMPLETE_VERIFIED == txStatus))
  {
    handleEvent(FW_EVENT_REQ_REPORT_GOT_ACK);
  }
  else{
    handleEvent(FW_EVENT_REQ_REPORT_NO_ACK);
  }
}


/**
 * Callback function receive status on Send data FIRMWARE_UPDATE_MD_GET_V5
 * @param pTransmissionResult : pointer to TRANSMISSION_RESULT structure containing transmit result.
 */
static void
ZCB_CmdClassFwUpdateMdGet(TRANSMISSION_RESULT * pTransmissionResult)
{
  zpal_feed_watchdog();

  DPRINT("Md Get CMD queued and now transmitted! Resetting Md Get CMD timer...\n");
  UNUSED(pTransmissionResult);
  /// MdGet transmission now done, SendDataAbort not needed anymore for this transmit
  myOta.MdGetDone = true;
  /// Start MdGet retry timer
  timerFwUpdateFrameGetStart(&myOta.timerFwUpdateFrameGet);
}


/// Sets/Resets myOta to initial values
static void initOTAState()
{
  myOta.currentState = FW_STATE_IDLE;
  myOta.fw_crcrunning = CRC_INITAL_VALUE;
  myOta.firmwareUpdateReportNumberPrevious = 0;
  myOta.fw_numOfRetries = 0;
  myOta.firmwareCrc = 0;
  myOta.statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_V5;
}


/**
 * Sends FW Update Status Report after OTA is done
 * Possible values defined in SDS13782, CC:007A.05.07.11.006
 * @param status Status to be sent.
 */
static void
SendFirmwareUpdateStatusReport()
{
  uint8_t waitTime = WAITTIME_FWU_FAIL;
  TimerCancelFwUpdateFrameGet();

  switch (myOta.statusReport)
  {
    case FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_STORED_V5:
      // The image is stored. Report it and wait for user reboot.
      waitTime = 0;
      break;
    case FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_V5:
      // Reboot right away and report afterwards.
      zpal_bootloader_reboot_and_install();
      return;
      break;
    case FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_WAITING_FOR_ACTIVATION_V5:
      // Image is stored. Wait for activation.
      waitTime = 0;
      break;
    default:
      // Do nothing - FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_V5 is handled here with nothing.
      DPRINTF("Not handled case: myOta.statusReport = %d!\n", myOta.statusReport);
      break;
  }

  if (JOB_STATUS_SUCCESS !=
      CmdClassFirmwareUpdateMdStatusReport(&myOta.rxOpt,
                                           myOta.statusReport,
                                           waitTime,
                                           ZCB_FinishFwUpdate))
  {
    /*Failed to send frame and we do not get a CB. Inform app we are finish*/
    ZCB_FinishFwUpdate(NULL);
  }
}

/// Callback Finish Fw update status to application.
static void ZCB_FinishFwUpdate(TRANSMISSION_RESULT * pTransmissionResult)
{
  UNUSED(pTransmissionResult);

  if (NULL != myOta.pOtaFinish)
  {
    myOta.pOtaFinish(myOta.finishStatus);
  }

  // Reboot if the firmware update went well and activation is disabled.
  if ((OTA_STATUS_DONE == myOta.finishStatus) &&
      !ActivationIsEnabled())
  {
    DPRINT("Reboot from ZCB_FinishFwUpdate\n");
    DPRINT("Now telling the bootloader new image to boot install\n");
    zpal_bootloader_reboot_and_install();
  }

  if (FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_V5 == myOta.statusReport)
  {
    // Device unable to receive new frames.
    // Reboot to avoid situation where device gets stuck for any unpredicted reason.
    zpal_reboot();
  }

  /*
   * Remove lock on power manager to allow going back to sleep with a delay
   * in case of buffered packets or ACK/NACK/RES that needs to be send.
   */
  zpal_pm_stay_awake(m_radioPowerLock, OTA_AWAKE_PERIOD_GRACEFUL_OFF);
  DPRINT(" --> OTA_UTIL.C TURNED OFF DEVICE! ---\n");
}

/// Cancel timer for retries on Get next firmware update frame.
static void
TimerCancelFwUpdateFrameGet(void)
{
  if (TimerIsActive(&myOta.timerFwUpdateFrameGet))
  {
    TimerStop(&myOta.timerFwUpdateFrameGet);
  }
  myOta.fw_numOfRetries = 0;
}


/// Callback on timeout on Get next firmware update frame.
/// It retry to Send a new Get frame.
/// @param pTimer Fw Update Frame Get timer
static void ZCB_TimerOutFwUpdateFrameGet(SSwTimer* pTimer)
{
  DPRINTF("Timer expired. Send next Md Get CMD. %d retries made already\n", myOta.fw_numOfRetries);

  if (FIRMWARE_UPDATE_MAX_RETRY > ++myOta.fw_numOfRetries)
  {
    DPRINTF("Send MD GET (same or next report number), reportNo = %d\n", myOta.firmwareUpdateReportNumberPrevious + 1);

    zpal_feed_watchdog();
    if (JOB_STATUS_SUCCESS == CmdClassFirmwareUpdateMdGet( &myOta.rxOpt,
                                                           myOta.firmwareUpdateReportNumberPrevious + 1,
                                                           ZCB_CmdClassFwUpdateMdGet))
    {
      DPRINT("FW Md Get CMD queued for tx\n");
      /// FW Md Get CMD queued - SendDataAbort can be called if a Report is received prior to Ack for MdGet has been received
      myOta.MdGetDone = false;
    }
    else
    {
      DPRINT("Failed to queue FW Md Get CMD tx. Md Get timer reset for retry.\n");
      /// Start retry timer as Job could not be queued
      timerFwUpdateFrameGetStart(pTimer);
    }
  }
  else
  {
    DPRINT("FW Md Get CMD send retry limit is hit.\n");
    if (FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_V5 == myOta.statusReport)
    {
      // If we haven't set status value already, set it now.
      myOta.statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_V5;
    }
    handleEvent(FW_EVENT_MAX_RETRIES_REACHED);  // This will send the status report.
  }
}

/// Restart timer for retries on Get next firmware update frame.
static void
TimerStartFwUpdateFrameGet(void)
{
  ESwTimerStatus timerStatus = timerFwUpdateFrameGetStart(&myOta.timerFwUpdateFrameGet);
  if (ESWTIMER_STATUS_FAILED == timerStatus)
  {
    /* No timer! we update without a timer for retries */
    DPRINT("Failed to start timerFwUpdateFrameGetStart\n");
    ASSERT(0);
  }
}

/*
 * Maximum fragment size definitions.
 */
#define FIRMWARE_UPDATE_MD_REPORT_ENCAPSULATION_LENGTH 6

uint16_t handleCommandClassFirmwareUpdateMaxFragmentSize(void)
{
  uint16_t maxFragmentSize;

  maxFragmentSize = ZAF_getAppHandle()->pNetworkInfo->MaxPayloadSize - FIRMWARE_UPDATE_MD_REPORT_ENCAPSULATION_LENGTH;

  // Align with 32 bit due to the writing to flash.
  maxFragmentSize = (uint16_t)(maxFragmentSize - (maxFragmentSize % 4));

  return maxFragmentSize;
}

bool CC_FirmwareUpdate_ActivationSet_handler(
    ZW_FIRMWARE_UPDATE_ACTIVATION_SET_V5_FRAME * pFrame,
    uint8_t * pStatus)
{
  uint16_t firmwareId = handleFirmWareIdGetExtended(pFrame->firmwareTarget);
  uint16_t manufacturerID = 0;
  uint16_t productID      = 0;
  CC_ManufacturerSpecific_ManufacturerSpecificGet_handler(&manufacturerID, &productID);
  uint8_t hardwareVersion = zaf_config_get_hardware_version();

  uint16_t manufacturerIdIncoming = (uint16_t)((((uint16_t)pFrame->manufacturerId1) << 8)
                                    | (uint16_t)pFrame->manufacturerId2);
  uint16_t firmwareIdIncoming = (uint16_t)((((uint16_t)pFrame->firmwareId1) << 8)
                                | (uint16_t)pFrame->firmwareId2);
  uint16_t checksumIncoming = (uint16_t)((((uint16_t)pFrame->checksum1) << 8)
                              | (uint16_t)pFrame->checksum2);

  /* Either no checksum value has yet been calculated, or we are an Deep Sleeping device and
   * therefore need to restore the calculated value from file storage */
  if (0 == myOta.firmwareCrc)
  {
    SFirmwareUpdateFile file;
    zpal_nvm_handle_t pFileSystem = ZAF_GetFileSystemHandle();
    const zpal_status_t status = zpal_nvm_read(pFileSystem,
                                               ZAF_FILE_ID_CC_FIRMWARE_UPDATE,
                                               &file,
                                               ZAF_FILE_SIZE_CC_FIRMWARE_UPDATE);
    ASSERT(ZPAL_STATUS_OK == status);
    myOta.firmwareCrc = file.checksum;
  }

  if ((manufacturerIdIncoming != manufacturerID) ||
      (firmwareIdIncoming != firmwareId) ||
      (checksumIncoming != myOta.firmwareCrc) ||
      (pFrame->hardwareVersion != hardwareVersion))
  {
    *pStatus = FIRMWARE_UPDATE_ACTIVATION_STATUS_REPORT_INVALID_COMBINATION_V5;
    return false;
  }
  DPRINTF("\n manufacturerIdIncoming: %4x", manufacturerIdIncoming);
  DPRINTF("\n manufacturerID: %4x", manufacturerID);
  DPRINTF("\n firmwareIdIncoming: %4x", firmwareIdIncoming);
  DPRINTF("\n firmwareId: %4x", firmwareId);
  // Delay the call of zpal_bootloader_reboot_and_install()
  // so we can transmit the ack or routed ack first
  if(ESWTIMER_STATUS_FAILED == TimerStart(&myOta.timerOtaSuccess, 100)) {
    DPRINT("Failed to delay OTA!");
    handleEvent(FW_EVENT_START_FW_INSTALL);
  } else {
    handleEvent(FW_EVENT_DELAY_FW_INSTALL);
  } 
  return true;
}


/**
 * Goes through all transitions in @ref OTA_transition_table
 * and finds next action for the current event.
 * Called when @ref OTA_transition.event happens.
 * @param event Event to handle
 */
static
void handleEvent(uint8_t event)
{
  uint8_t len = sizeof_array(OTA_transition_table);
  int i = 0;
  for (; i < len; i++)
  {
    // Check if event exists in transition table
    if(OTA_transition_table[i].event == event)
    {
      DPRINTF("%s[%d]: Event %s\n", __func__,
                                   i,
                                   getEventAsString(event));
      // Check if event is defined for current state
      if (OTA_transition_table[i].state == myOta.currentState)
      {
        // Correct state-event combination found. Do Action and set new state.
        DPRINTF("%s: state %s ---> %s\n", __func__,
                getStateAsString(OTA_transition_table[i].state),
                getStateAsString(OTA_transition_table[i].new_state));
        fw_action fwaction = OTA_transition_table[i].action;
        fwaction();
        myOta.currentState = OTA_transition_table[i].new_state;
        return;
      }
      else
      {
        // State doesn't match. Check if there is another entry
        // with the same event, but another state
        DPRINTF("%s: State %s doesn't match. Continue.\n",
                __func__,
                getStateAsString(OTA_transition_table[i].state));
      }
    }
  }
  // Reached the end of the loop without finding matching state/event pair.
  DPRINTF("%s: Event %s doesn't exist or not expected in the current state %s. Ignore.\n",
           __func__,
           getEventAsString(event),
           getStateAsString(myOta.currentState));
  // Ignore invalid frame and continue.
  // For more strict control, comment out next line.
  // zpal_reboot();
}


/// Sends FW Update MD Get
static void fw_action_send_get(void)
{
  DPRINTF(">> %s()\n", __func__);

  resetReceivedReportsData();
  myOta.fw_numOfRetries = 0;
  
  zpal_feed_watchdog();
  if (JOB_STATUS_SUCCESS != CmdClassFirmwareUpdateMdGet(&myOta.rxOpt,
                                                        myOta.firmwareUpdateReportNumberPrevious + 1,  // The next report number.
                                                        ZCB_CmdClassFwUpdateMdGet))
  {
    TimerStartFwUpdateFrameGet();
  }
}
/// Handles sending FW Update Request Report.
static void fw_action_send_req_report(void)
{
  DPRINTF(">> %s(), requestReport status: %d\n", __func__, myOta.requestReport);

  if (FIRMWARE_UPDATE_MD_REQUEST_REPORT_VALID_COMBINATION_V5 == myOta.requestReport)
  {
    myOta.fw_crcrunning = CRC_INITAL_VALUE;
    myOta.firmwareUpdateReportNumberPrevious = 0;
    TimerCancelFwUpdateFrameGet();
  }
  // Done. Actual Request Report is sent from CC.
}
/// OTA done. Sends FW Update Status Report.
static void fw_action_send_status_report()
{
  DPRINTF(">> %s(), send status report[%d] and end.\n", __func__, myOta.statusReport);

  // OTA should be finished after this point. Send Status report.
  // SendFirmwareUpdateStatusReport() will also stop any running timers
  myOta.finishStatus = OTA_STATUS_ABORT;
  SendFirmwareUpdateStatusReport();
}
/// Verifies image and sends FW Update Status Report.
static void fw_action_verify_image(void)
{
  DPRINTF(">> %s() \n", __func__);
  resetReceivedReportsData();
  if(ZPAL_STATUS_OK == zpal_bootloader_verify_image())
  {
    UpdateStatusSuccess();
  }
  else
  {
    DPRINT("FIRMWARE_UPDATE_MD_STATUS_REPORT_DOES_NOT_MATCH_THE_FIRMWARE_TARGET_V5");
    myOta.finishStatus = OTA_STATUS_ABORT;
    myOta.statusReport = FIRMWARE_UPDATE_MD_STATUS_REPORT_DOES_NOT_MATCH_THE_FIRMWARE_TARGET_V5;
    SendFirmwareUpdateStatusReport();
  }
}

static void fw_action_reboot_and_Install(void)
{
  DPRINTF(">> %s() \n", __func__);
  zpal_bootloader_reboot_and_install();
}
/// No action needed.
static void fw_action_none(void)
{
  DPRINTF(">> %s() - Nothing to do.\n", __func__);
}

#ifdef DEBUGPRINT
/// Helper function to provide more user friendly debug output
static char* __attribute ((used)) getStateAsString(FW_STATE state)
{
  switch(state)
  {
    case FW_STATE_IDLE:
      return "FW_STATE_IDLE";
    case FW_STATE_READY:
      return "FW_STATE_READY";
    case FW_STATE_AWAIT_REPORT:
      return "FW_STATE_AWAIT_REPORT";
    case FW_STATE_AWAIT_TIMEOUT:
      return "FW_STATE_AAWAIT_TIMEOUT";

    default:
      return "-STATE UNKNOWN-";
  }
  return NULL;
}
/// Helper function to provide more user friendly debug output
static char* __attribute ((used)) getEventAsString(FW_EVENT event)
{
  switch(event)
  {
    case FW_EVENT_REQ_GET_RECEIVED_VALID:
      return "FW_EVENT_REQ_GET_RECEIVED_VALID";
    case FW_EVENT_REQ_GET_RECEIVED_INVALID:
      return "FW_EVENT_REQ_GET_RECEIVED_INVALID";
    case FW_EVENT_REQ_REPORT_GOT_ACK:
      return "FW_EVENT_REQ_REPORT_GOT_ACK";
    case FW_EVENT_REQ_REPORT_NO_ACK:
      return "FW_EVENT_REQ_REPORT_NO_ACK";
    case FW_EVENT_MAX_RETRIES_REACHED:
      return "FW_EVENT_MAX_RETRIES_REACHED";
    case FW_EVENT_REPORT_RECEIVED_VALID:
      return "FW_EVENT_REPORT_RECEIVED_VALID";
    case FW_EVENT_REPORT_RECEIVED_INVALID:
      return "FW_EVENT_REPORT_RECEIVED_INVALID";
    case FW_EVENT_REPORT_RECEIVED_BATCH:
      return "FW_EVENT_REPORT_RECEIVED_BATCH";
    case FW_EVENT_REPORT_RECEIVED_LAST:
      return "FW_EVENT_REPORT_RECEIVED_LAST";
    case FW_EVENT_REPORT_RECEIVED_LAST_INVALID:
      return "FW_EVENT_REPORT_RECEIVED_LAST_INVALID";
    case FW_EVENT_DELAY_FW_INSTALL:
      return "FW_EVENT_DELAY_FW_INSTALL";
    case FW_EVENT_START_FW_INSTALL:
      return "FW_EVENT_START_FW_INSTALL";
    default:
      return "-EVENT UNKNOWN-";
  }
  return NULL;
}
#endif //DEBUGPRINT


uint8_t getFWUpdateMDGetNumberOfReports()
{
  // Number of reports to request = Max number of reports - number or reports already received
  // Device should keep receiving MD reports as long as mdReportsStorage is not full.
  // If some received report is invalid, then next MD Get should request as many reports,
  // as it is needed to fill mdReportsStorage completely.

  DPRINTF(">> %s() - Ask for %d reports. Received so far: %d\n",
          __func__,
          mdGetNumberOfReports - myOta.reportsReceived,
          myOta.reportsReceived);

  return mdGetNumberOfReports - myOta.reportsReceived;
}

/// Called after all reports requested in one GET are received,
/// or entire transfer is complete.
static void resetReceivedReportsData()
{
  DPRINTF(">> %s() - reset data\n", __func__);
  myOta.reportsReceived = 0;
  memset(mdReportsStorage, 0x00, sizeof(mdReportsStorage));
}

/// Checks whether multi frames should be used. To use it, Multi frames option
/// must be enabled, and number of reports should be greater than 1
static bool useMultiFrames()
{
  return OTA_MULTI_FRAME_ENABLED && ( mdGetNumberOfReports > 1 );
}

void otaModuleReset(void)
{
  // Additional resets prior to initOTAState()
  myOta.finishStatus = OTA_STATUS_DONE;
  myOta.MdGetDone = true;
  myOta.activation_enabled = 0;
  myOta.requestReport = FIRMWARE_UPDATE_MD_REQUEST_REPORT_VALID_COMBINATION_V5;
  myOta.reportsReceived = 0;

  initOTAState();
}

