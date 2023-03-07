/***************************************************************************//**
 * @file app_process.c
 * @brief Implements application specific functions.
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

#include <stdint.h>
#include "app_init.h"
#include "gpd-components-common.h"
#include "sl_component_catalog.h"
#include "sl_sleeptimer.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "nvm3.h"
#include "sl_simple_button_btn0_config.h"
#include "sl_simple_button_btn1_config.h"
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "sl_zigbee_debug_print.h"
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define EMBER_GPD_NV_DATA_TAG 0xA9A1

#define GPD_BUTTON_COUNT 2
#define BUTTON_INSTANCE_0 sl_button_btn0
#define BUTTON_INSTANCE_1 sl_button_btn1

#define EM4WU_PORT              SL_SIMPLE_BUTTON_BTN1_PORT
#define EM4WU_PIN               SL_SIMPLE_BUTTON_BTN1_PIN

#if defined _SILICON_LABS_32B_SERIES_2_CONFIG_2   //For 4182, PB1
  #define EM4WU_EM4WUEN_NUM       (3)
  #define EM4WU_BASE_SHIFT_MASK   (16)
  #define EM4WU_EM4WUEN_MASK      (1 << (EM4WU_EM4WUEN_NUM + EM4WU_BASE_SHIFT_MASK))
#elif defined _SILICON_LABS_32B_SERIES_2_CONFIG_1 //For 4180, PB0
  #define EM4WU_EM4WUEN_NUM
  #define EM4WU_EM4WUEN_MASK
  #error "BRD4180 and BRD4181 currently not supported by this application. \
  For custom hardware please define the macros above"
#elif defined _SILICON_LABS_32B_SERIES_1      //For 4161, PF7
  #define EM4WU_EM4WUEN_NUM       (1)
  #define EM4WU_EM4WUEN_MASK      ((1 << EM4WU_EM4WUEN_NUM) << _GPIO_EM4WUEN_EM4WUEN_SHIFT)
#endif

// LED Indication
#define ACTIVITY_LED sl_led_led0 //BOARDLED1
#define COMMISSIONING_STATE_LED sl_led_led1 //BOARDLED0

#define BOARD_LED_ON(led) sl_led_turn_on(&led)
#define BOARD_LED_OFF(led) sl_led_turn_off(&led)

// App button press event types
enum {
  APP_EVENT_ACTION_IDLE = 0,
  APP_EVENT_ACTION_SEND_COMMISSION = 0x01,
  APP_EVENT_ACTION_SEND_DECOMMISSION = 0x02,
  APP_EVENT_ACTION_SEND_TOGGLE = 0x03,
  APP_EVENT_ACTION_SEND_REPORT = 0x04,
};
typedef uint8_t GpdAppEventActionType;

#ifndef APP_SINGLE_EVENT_COMMISSIONING_LOOP_TIME_MS
#define APP_SINGLE_EVENT_COMMISSIONING_LOOP_TIME_MS 3300
#endif

#if !defined APP_GPD_TIME_TO_ENTER_EM4_MS
#define APP_GPD_TIME_TO_ENTER_EM4_MS 10000
#endif

// If debug print is enabled
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#define gpdDebugPrintf sl_zigbee_core_debug_print
#else
#define gpdDebugPrintf(...)
#endif

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

static void processAppEvent(EmberGpd_t * gpd, GpdAppEventActionType *appAction);
static void leTimeCallback(sl_sleeptimer_timer_handle_t *handle, void *contextData);
static void appSingleEventCommissionTimer(sl_sleeptimer_timer_handle_t *handle, void *contextData);
static void appEm4TimerCallback(sl_sleeptimer_timer_handle_t *handle, void *contextData);
static void gpdStartEm4Timer(uint32_t countDownTimeInMs);
static void sendToggle(EmberGpd_t * gpd);
static void gpdEnterLowPowerMode(bool forceEm4);
static void gpdConfigGpiosForEm4(void);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

static GpdAppEventActionType appAction;
static bool sleepy = false;
static sl_sleeptimer_timer_handle_t le_timer_handle;
static sl_sleeptimer_timer_handle_t app_single_event_commission;
static sl_sleeptimer_timer_handle_t em4_timer_handle;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * CLI command to send a toggle
 *****************************************************************************/
void emberGpdSwitchToggleEvent(void)
{
  appAction = APP_EVENT_ACTION_SEND_TOGGLE;
}

/******************************************************************************
 * Sets the sleepy flag that will allow the module sleep untill a button
 * is pressed.
 *****************************************************************************/
void emberGpdSwitchSetSleep(void)
{
  // enable sleep
  sleepy = true;
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  // GPD state LED indication if enabled.
  if (gpd->gpdState < EMBER_GPD_APP_STATE_OPERATIONAL) {
    BOARD_LED_ON(COMMISSIONING_STATE_LED);
  } else {
    BOARD_LED_OFF(COMMISSIONING_STATE_LED);
  }
  // Process application actions
  if (appAction != APP_EVENT_ACTION_IDLE) {
    processAppEvent(gpd, &appAction);
  }
  // Enter sleep in sleepy mode, the wake up is on button activation or
  // or when periodic timer expires.
  if (sleepy) {
    gpdStartEm4Timer(APP_GPD_TIME_TO_ENTER_EM4_MS);
    gpdEnterLowPowerMode(false);
  }
}

void emberGpdLoadLeTimer(uint32_t timeInUs)
{
  // in Hz => Time period in micro sec T = 1000000/f
  uint32_t f = sl_sleeptimer_get_timer_frequency();
  // ticks needed = (timeout needed in mico sec)/(T in micro sec) =  ((timeout needed in micro sec) * f)/1000000
  uint32_t tick = (timeInUs * f) / 1000000;
  sl_sleeptimer_restart_timer(&le_timer_handle,
                              tick,
                              leTimeCallback,
                              NULL,
                              0,
                              0);
}

bool emberGpdLeTimerRunning(void)
{
  bool running = false;
  sl_sleeptimer_is_timer_running(&le_timer_handle, &running);
  return running;
}

// Application Commissioning that completes all the statest of the commissioning
void emberGpdAppSingleEventCommission(void)
{
  sl_sleeptimer_restart_periodic_timer_ms(&app_single_event_commission,
                                          APP_SINGLE_EVENT_COMMISSIONING_LOOP_TIME_MS,
                                          appSingleEventCommissionTimer,
                                          NULL,
                                          0,
                                          0);
}

/** @brief Called from the imcomming command handler context for all the incoming
 *         command before the command handler handles it. based on the return code
 *         of this callback the internal command handler skips the processing.
 *
 * @param gpdCommand CommandId.
 * @param length length of the command
 * @param commandPayload The pointer to the commissioning reply payload.
 * Ver.: always
 *
 * @return true if application handled it.
 */
bool emberGpdAfPluginIncomingCommandCallback(uint8_t gpdCommand,
                                             uint8_t length,
                                             uint8_t * commandPayload)
{
  gpdDebugPrintf("RX: cmd=%x len=%x payload[", gpdCommand, length);
  if (commandPayload != NULL
      && length > 0
      && length < 0xFE) {
    for (int i = 0; i < length; i++) {
      gpdDebugPrintf("%x", commandPayload[i]);
    }
  }
  gpdDebugPrintf("]\n");
  // Unused variable warning suppression when print is not available.
  (void)gpdCommand;
  (void)length;
  (void)commandPayload;
  return false;
}

// NVM Initialisation
void emberGpdAfPluginNvInitCallback(void)
{
  // Initialise the NV
  // use default NVM space and handle
  Ecode_t status = nvm3_open(nvm3_defaultHandle, nvm3_defaultInit);
  if (status != ECODE_NVM3_OK) {
    // Handle error
  }

  // Do re-packing if needed
  if (nvm3_repackNeeded(nvm3_defaultHandle)) {
    status = nvm3_repack(nvm3_defaultHandle);
    if (status != ECODE_NVM3_OK) {
      // Handle error
    }
  }
}

/** @brief Called to the application to give a chance to load or store the GPD Context
 *.        in a non volatile context. Thsi can help the application to use any other
 *         non volatile storage.
 *
 * @param nvmData The pointer to the data that needs saving or retrieving to or from
 *                the non-volatile memory.
 * @param sizeOfNvmData The size of the data non-volatile data.
 * @param loadStore indication wether to load or store the context.
 * Ver.: always
 *
 * @return true if application handled it.
 */
bool emberGpdAfPluginNvSaveAndLoadCallback(EmberGpd_t * gpd,
                                           uint8_t * nvmData,
                                           uint8_t sizeOfNvmData,
                                           EmebrGpdNvLoadStore_t loadStore)
{
  if (loadStore == EMEBER_GPD_AF_CALLBACK_LOAD_GPD_FROM_NVM) {
    uint32_t objectType;
    size_t dataLen;
    if (0 == nvm3_countObjects(nvm3_defaultHandle)
        || (nvm3_getObjectInfo(nvm3_defaultHandle,
                               EMBER_GPD_NV_DATA_TAG,
                               &objectType,
                               &dataLen) == ECODE_NVM3_OK
            && objectType != NVM3_OBJECTTYPE_DATA)) {
      // Fresh chip , erase, create a storage with default setting.
      // Erase all objects and write initial data to NVM3
      nvm3_eraseAll(nvm3_defaultHandle);
      // First write to the NVM shadow so that it updated with default ones
      emberGpdCopyToShadow(gpd);
      // Write the data to NVM
      nvm3_writeData(nvm3_defaultHandle,
                     EMBER_GPD_NV_DATA_TAG,
                     nvmData,
                     sizeOfNvmData);
    } else {
      nvm3_readData(nvm3_defaultHandle,
                    EMBER_GPD_NV_DATA_TAG,
                    nvmData,
                    sizeOfNvmData);
    }
  } else if (loadStore == EMEBER_GPD_AF_CALLBACK_STORE_GPD_TO_NVM) {
    nvm3_writeData(nvm3_defaultHandle,
                   EMBER_GPD_NV_DATA_TAG,
                   nvmData,
                   sizeOfNvmData);
  } else {
    // bad command
  }
  return false;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application event processing
 *****************************************************************************/
static void processAppEvent(EmberGpd_t * gpd, GpdAppEventActionType *appAction)
{
  if (*appAction != APP_EVENT_ACTION_IDLE) {
    BOARD_LED_ON(ACTIVITY_LED);
    if (*appAction == APP_EVENT_ACTION_SEND_DECOMMISSION) {
      emberGpdAfPluginDeCommission(gpd);
      emberGpdStoreSecDataToNV(gpd);
      gpdDebugPrintf("Decomm Cmd : ");
    } else if (*appAction == APP_EVENT_ACTION_SEND_COMMISSION) {
      emberGpdAfPluginCommission(gpd);
      emberGpdStoreSecDataToNV(gpd);
      gpdDebugPrintf("Comm. Cmd : ");
    } else if (*appAction == APP_EVENT_ACTION_SEND_TOGGLE) {
      sendToggle(gpd);
      emberGpdStoreSecDataToNV(gpd);
      gpdDebugPrintf("Toggle Cmd : ");
    }
    gpdDebugPrintf("Comm. State :%d\n", gpd->gpdState);
    *appAction = APP_EVENT_ACTION_IDLE;
    BOARD_LED_OFF(ACTIVITY_LED);
  }
}

static void leTimeCallback(sl_sleeptimer_timer_handle_t *handle, void *contextData)
{
  (void)contextData;
  sl_sleeptimer_stop_timer(handle);
}

static void appSingleEventCommissionTimer(sl_sleeptimer_timer_handle_t *handle, void *contextData)
{
  (void)contextData;
  EmberGpd_t * gpd = emberGpdGetGpd();
  if (gpd->gpdState < EMBER_GPD_APP_STATE_OPERATIONAL) {
    appAction = APP_EVENT_ACTION_SEND_COMMISSION;
  } else {
    sl_sleeptimer_stop_timer(handle);
  }
}

static void sendToggle(EmberGpd_t * gpd)
{
  uint8_t command[] = { GP_CMD_TOGGLE };
  gpd->rxAfterTx = false;
  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_DATA,
                  gpd,
                  command,
                  sizeof(command),
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
}

// Low power management routines
static void gpdStartEm4Timer(uint32_t countDownTimeInMs)
{
  sl_sleeptimer_stop_timer(&em4_timer_handle);

  sl_sleeptimer_start_timer_ms(&em4_timer_handle,
                               countDownTimeInMs,
                               appEm4TimerCallback,
                               NULL,
                               0,
                               0);
}

static void appEm4TimerCallback(sl_sleeptimer_timer_handle_t *handle, void *contextData)
{
  (void)handle;
  (void)contextData;
  gpdEnterLowPowerMode(true);
}

static void gpdConfigGpiosForEm4(void)
{
  // Turn on the clock for the GPIO
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_Unlock();
  GPIO_PinModeSet(EM4WU_PORT, EM4WU_PIN, gpioModeInputPullFilter, 1UL);
  GPIO_EM4EnablePinWakeup(EM4WU_EM4WUEN_MASK, 0);
  GPIO_IntClear(0xFFFFFFFF);
#if defined _SILICON_LABS_32B_SERIES_2_CONFIG_2
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);
#endif
}

// Low Power Mode with option to force EM4 mode.
static void gpdEnterLowPowerMode(bool forceEm4)
{
  if (forceEm4) {
    EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

    em4Init.pinRetentionMode = emuPinRetentionEm4Exit;
    em4Init.em4State = emuEM4Shutoff;
    gpdConfigGpiosForEm4();
    EMU_EM4Init(&em4Init);
    gpdDebugPrintf("EM4\n");
    EMU_EnterEM4();
  } else {
    gpdDebugPrintf("EM2\n");
    EMU_EnterEM2(true);
  }
}

// Button Events
#if GPD_BUTTON_COUNT == 2

void sl_button_on_change(const sl_button_t *handle)
{
  if ((&BUTTON_INSTANCE_0 == handle)
      || (&BUTTON_INSTANCE_1 == handle)) {
    sl_button_state_t botton0State = sl_button_get_state(&BUTTON_INSTANCE_0);
    sl_button_state_t botton1State = sl_button_get_state(&BUTTON_INSTANCE_1);
    if (botton0State == SL_SIMPLE_BUTTON_PRESSED && botton1State == SL_SIMPLE_BUTTON_PRESSED) {
      appAction = APP_EVENT_ACTION_SEND_DECOMMISSION;
    } else if (botton0State == SL_SIMPLE_BUTTON_PRESSED) {
      appAction = APP_EVENT_ACTION_SEND_COMMISSION;
    } else if (botton1State == SL_SIMPLE_BUTTON_PRESSED) {
      appAction = APP_EVENT_ACTION_SEND_TOGGLE;
    }
  }
}

#elif GPD_BUTTON_COUNT == 1
static sl_sleeptimer_timer_handle_t button_release_timer_handle;
static void buttonReleaseTimeout(sl_sleeptimer_timer_handle_t *handle, void *contextData)
{
  sl_sleeptimer_stop_timer(handle);
  appAction = APP_EVENT_ACTION_SEND_DECOMMISSION;
}
void sl_button_on_change(const sl_button_t *handle)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED
      && (&BUTTON_INSTANCE_0 == handle)) {
    if (gpd->gpdState < EMBER_GPD_APP_STATE_OPERATIONAL) {
      appAction = APP_EVENT_ACTION_SEND_COMMISSION;
    } else {
      appAction = APP_EVENT_ACTION_SEND_TOGGLE;
    }
    sl_sleeptimer_restart_timer_ms(&button_release_timer_handle,
                                   5000,
                                   buttonReleaseTimeout,
                                   NULL,
                                   0,
                                   0);
  } else {
    sl_sleeptimer_stop_timer(&button_release_timer_handle);
  }
}

#endif // GPD_BUTTON_COUNT
