/******************************************************************************

 @file  oad_app_support.c

 @brief This file contains OAD support functions for the applications

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#ifdef OAD_SUPPORT

#include <ti/sysbios/knl/Task.h>
#include <ti/display/Display.h>
#include <icall.h>
#include "util.h"
#include <bcomdef.h>
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>
#include <ti_drivers_config.h>
#include "oad_app_support.h"
#include "onboard.h"

#include <menu/two_btn_menu.h>

#include "simple_mesh_node.h"
#include "simple_mesh_node_menu.h"

#include "ti_ble_config.h"

#ifdef OAD_SUPPORT
// Used for imgHdr_t structure
#include <common/cc26xx/oad/oad_image_header.h>
#include "oad.h"
#if defined(OAD_SUPPORT_ONCHIP)
// Used for OAD Reset Service APIs
#include "oad_reset_service.h"
#include <common/cc26xx/flash_interface/flash_interface.h>
#endif // OAD_SUPPORT_ONCHIP
#endif // OAD_SUPPORT

/*********************************************************************
 * EXTERNS
*/
extern ICall_SyncHandle appSyncEvent;
extern ICall_EntityID appSelfEntity;
extern Display_Handle dispHandle;
extern void    SimplePeripheral_connEvtCB(Gap_ConnEventRpt_t *pReport);
extern uint8_t bleApp_processOadEventsInAppContext(uint8_t *pEventData);
extern uint8_t bleApp_processOadResetInAppContext(uint8_t *pEventData);

#if defined(OAD_SUPPORT_ONCHIP)

static oadResetWriteCB_t SimplePeripheral_oadResetCBs =
{
  OadApp_processOadResetWriteCB // Write Callback.
};

//reset connection handle
uint16_t resetConnHandle = LINKDB_CONNHANDLE_INVALID;

void OadApp_addService()
{
  Reset_addService((oadUsrAppCBs_t *)&SimplePeripheral_oadResetCBs);
}

/*********************************************************************
 * @fn      OadApp_processOadResetEvt
 *
 * @brief   Process a write request to the OAD reset service
 *
 * @param   resetEvt - The oadResetWrite_t struct containing reset data
 *
 * @return  None.
 */
void OadApp_processOadResetEvt(oadResetWrite_t *resetEvt)
{
  /* We cannot reboot the device immediately after receiving
   * the enable command, we must allow the stack enough time
   * to process and responsd to the OAD_EXT_CTRL_ENABLE_IMG
   * command. The current implementation will wait one cxn evt
   */
  // Register for L2CAP Flow Control Events
  L2CAP_RegisterFlowCtrlTask(appSelfEntity);

  resetConnHandle = resetEvt->connHandle;

  uint8_t status = FLASH_FAILURE;
  //read the image validation bytes and set it appropriately.
  imgHdr_t imgHdr = {0};
  if(flash_open())
  {
    status = readFlash(0x0, (uint8_t *)&imgHdr, OAD_IMG_HDR_LEN);
  }

  if ((FLASH_SUCCESS == status) && ( imgHdr.fixedHdr.imgVld != 0))
  {
    if ( OAD_evenBitCount(imgHdr.fixedHdr.imgVld) )
    {
      imgHdr.fixedHdr.imgVld = imgHdr.fixedHdr.imgVld << 1;
      writeFlash((uint32_t)FLASH_ADDRESS(0, IMG_VALIDATION_OFFSET),
                 (uint8_t *)&(imgHdr.fixedHdr.imgVld), sizeof(imgHdr.fixedHdr.imgVld));
    }
  }
}

/*********************************************************************
 * @fn      OadApp_processOadResetWriteCB
 *
 * @brief   Process a write request to the OAD reset service
 *
 * @param   connHandle - the connection Handle this request is from.
 * @param   bim_var    - bim_var to set before resetting.
 *
 * @return  None.
 */
void OadApp_processOadResetWriteCB(uint16_t connHandle,
                                      uint16_t bim_var)
{
    // Allocate memory for OAD EVT payload, the app task must free this later
    oadResetWrite_t *oadResetWriteEvt = ICall_malloc(sizeof(oadResetWrite_t));

    oadResetWriteEvt->connHandle = connHandle;
    oadResetWriteEvt->bim_var = bim_var;

    bleApp_processOadResetInAppContext((uint8_t *)oadResetWriteEvt);
}

#endif // OAD_SUPPORT_ONCHIP

#if defined(OAD_SUPPORT_OFFCHIP)
static oadTargetCBs_t SimplePeripheral_oadCBs =
{
  OadApp_processOadWriteCB // Write Callback.
};
#endif

// Variable used to store the number of messages pending once OAD completes
// The application cannot reboot until all pending messages are sent
static uint8_t numPendingMsgs = 0;
static bool oadWaitReboot = false;

// Flag to be stored in NV that tracks whether service changed
// indications needs to be sent out
static uint32_t  sendSvcChngdOnNextBoot = FALSE;

#ifdef LED_DEBUG
  void LedDebug_init();
  void LedDebug_set();
#endif //LED_DEBUG

void OadApp_init()
{
  uint8_t versionStr[OAD_SW_VER_LEN + 1];

#ifdef LED_DEBUG
  LedDebug_init();
#endif //LED_DEBUG

  // Read in the OAD Software version
  uint8_t swVer[OAD_SW_VER_LEN];
  OAD_getSWVersion(swVer, OAD_SW_VER_LEN);

  memcpy(versionStr, swVer, OAD_SW_VER_LEN);

  // Add in Null terminator
  versionStr[OAD_SW_VER_LEN] = 0;

  // Display Image version
  Display_print1(dispHandle, 0, 0, "SBP Off-chip OAD v%s",
                 versionStr);

#if defined(OAD_SUPPORT_OFFCHIP)
  // Open the OAD module and add the OAD service to the application
  if(OAD_SUCCESS != OAD_open(OAD_DEFAULT_INACTIVITY_TIME))
  {
    Display_print0(dispHandle, 0, 0, "OAD failed to open");
  }
  else
  {
    // Resiter the OAD callback with the application
    OAD_register(&SimplePeripheral_oadCBs);
  }
#elif defined(OAD_SUPPORT_ONCHIP)
  /*
   * When switching from persistent app back to the user application for the
   * for the first time after an OAD the device must send a service changed
   * indication. This will cause any peers to rediscover services.
   *
   * To prevent sending a service changed IND on every boot, a flag is stored
   * in NV to determine whether or not the service changed IND needs to be
   * sent
   */
  uint8_t status = osal_snv_read(BLE_NVID_CUST_START,
                                  sizeof(sendSvcChngdOnNextBoot),
                                  (uint8 *)&sendSvcChngdOnNextBoot);
  if(status != SUCCESS)
  {
    /*
     * On first boot the NV item will not have yet been initialzed, and the read
     * will fail. Do a write to set the initial value of the flash in NV
     */
     osal_snv_write(BLE_NVID_CUST_START, sizeof(sendSvcChngdOnNextBoot),
                    (uint8 *)&sendSvcChngdOnNextBoot);
  }
#endif
}

void OadApp_cancel()
{
#if defined(OAD_SUPPORT_OFFCHIP)
      OAD_cancel();
#elif defined(OAD_SUPPORT_ONCHIP)
      //OAD_cancel(); //ToDo: Remove: PK
#endif
}

void OadApp_processEvents(uint32_t events)
{
       // OAD events
      if(events & OAD_OUT_OF_MEM_EVT)
      {
        // The OAD module is unable to allocate memory, print failure, cancel OAD
        Display_print0(dispHandle, SP_ROW_STATUS_1, 0,
                        "OAD malloc fail, cancelling OAD");
        OAD_cancel();

#ifdef LED_DEBUG
        LedDebug_set();
#endif //LED_DEBUG
      }
      // OAD queue processing
      if(events & OAD_QUEUE_EVT)
      {
        // Process the OAD Message Queue
        uint8_t status = OAD_processQueue();

        // If the OAD state machine encountered an error, print it
        // Return codes can be found in oad_constants.h
        if(status == OAD_DL_COMPLETE)
        {
          Display_print0(dispHandle, SP_ROW_STATUS_1, 0, "OAD DL Complete, wait for Enable");
        }
        else if(status == OAD_IMG_ID_TIMEOUT)
        {
          Display_print0(dispHandle, SP_ROW_STATUS_1, 0, "ImgID Timeout, disconnecting");

          // This may be an attack, terminate the link,
          // Note HCI_DISCONNECT_REMOTE_USER_TERM seems to most closet reason for
          // termination at this state
          MAP_GAP_TerminateLinkReq(OAD_getactiveCxnHandle(), HCI_DISCONNECT_REMOTE_USER_TERM);
        }
        else if(status != OAD_SUCCESS)
        {
          Display_print1(dispHandle, SP_ROW_STATUS_1, 0, "OAD Error: %d", status);
        }

      }

      if(events & OAD_DL_COMPLETE_EVT)
      {
        // Register for L2CAP Flow Control Events
        L2CAP_RegisterFlowCtrlTask(appSelfEntity);
      }
}

/*********************************************************************
 * @fn      OadApp_processL2capMsg
 *
 * @brief   Process L2CAP messages and events.
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
uint8_t OadApp_processL2CAPMsg(l2capSignalEvent_t *pMsg)
{
  uint8_t safeToDealloc = TRUE;
  static bool firstRun = TRUE;

  switch (pMsg->opcode)
  {
    case L2CAP_NUM_CTRL_DATA_PKT_EVT:
    {
      /*
      * We cannot reboot the device immediately after receiving
      * the enable command, we must allow the stack enough time
      * to process and respond to the OAD_EXT_CTRL_ENABLE_IMG
      * command. This command will determine the number of
      * packets currently queued up by the LE controller.
      * BIM var is already set via OadPersistApp_processOadWriteCB
      */
      if(firstRun)
      {
        firstRun = false;

        // We only want to set the numPendingMsgs once
        numPendingMsgs = MAX_NUM_PDU - pMsg->cmd.numCtrlDataPktEvt.numDataPkt;

        // Wait until all PDU have been sent on cxn events
        Gap_RegisterConnEventCb(SimplePeripheral_connEvtCB,
                                  GAP_CB_REGISTER,
#if defined(OAD_SUPPORT_OFFCHIP)
                                  OAD_getactiveCxnHandle());
#else
                                  resetConnHandle);
#endif

        /* Set the flag so that the connection event callback will
         * be processed in the context of a pending OAD reboot
         */
        oadWaitReboot = true;
      }

      break;
    }
    default:
      // do nothing
      break;
  }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      OadApp_processOadWriteCB
 *
 * @brief   Process a write request to the OAD reset service
 *
 * @param   connHandle - the connection Handle this request is from.
 * @param   bim_var    - bim_var to set before resetting.
 *
 * @return  None.
 */
void OadApp_processOadWriteCB(uint8_t event, uint16_t arg)
{
#ifdef ICALL_NO_APP_EVENTS
  uint8_t *pEventData = ICall_malloc(sizeof(uint8_t));
  pEventData[0] = event;
  bleApp_processOadEventsInAppContext(pEventData);
#else
  Event_post(appSyncEvent, event);
#endif
}

bool OadApp_processConnEvt()
{
  /* If we are waiting for an OAD Reboot, process connection events to ensure
   * that we are not waiting to send data before restarting
   */
  if(oadWaitReboot)
  {
    // Wait until all pending messages are sent
    if(numPendingMsgs == 0)
    {
      // Store the flag to indicate that a service changed IND will
      // be sent at the next boot
      sendSvcChngdOnNextBoot = TRUE;

      uint8_t status = osal_snv_write(BLE_NVID_CUST_START,
                                      sizeof(sendSvcChngdOnNextBoot),
                                      (uint8 *)&sendSvcChngdOnNextBoot);
      if(status != SUCCESS)
      {
        Display_print1(dispHandle, 5, 0, "SNV WRITE FAIL: %d", status);
      }

      // Reset the system
      SystemReset();
    }
    else
    {
      numPendingMsgs--;
    }
  }
  else
  {
    return FALSE;
  }

  return TRUE;
}

#endif // OAD_SUPPORT

#ifdef LED_DEBUG

#include <ti/drivers/PIN.h>

// State variable for debugging LEDs
static PIN_State sbpLedState;

// Pin table for LED debug pins
static const PIN_Config sbpLedPins[] = {
    CONFIG_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    CONFIG_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

void LedDebug_set()
{
  // Diplay is not enabled in persist app so use LED
  if(PIN_open(&sbpLedState, sbpLedPins))
  {
    PIN_setOutputValue(&sbpLedState, CONFIG_PIN_RLED, 1);
  }
}

void LedDebug_init()
{
  // Open the LED debug pins
  if (!PIN_open(&sbpLedState, sbpLedPins))
  {
    Display_print0(dispHandle, 0, 0, "Debug PINs failed to open");
  }
  else
  {
    PIN_Id activeLed;
    uint8_t blinkCnt = 9;

    PIN_setOutputValue(&sbpLedState, CONFIG_PIN_RLED, 0);
    PIN_setOutputValue(&sbpLedState, CONFIG_PIN_GLED, 0);

    if (blinkCnt < 12)
    {
      activeLed = CONFIG_PIN_RLED;
    }
    else
    {
      activeLed = CONFIG_PIN_GLED;
    }

    for(uint8_t numBlinks = 0; numBlinks < blinkCnt; ++numBlinks)
    {
      PIN_setOutputValue(&sbpLedState, activeLed, !PIN_getOutputValue(activeLed));

      // Sleep for 100ms, sys-tick for BLE-Stack is 10us,
      // Task sleep is in # of ticks
      Task_sleep(10000);
    }

    // Close the pins after using
    PIN_close(&sbpLedState);
  }
}
#endif //LED_DEBUG
