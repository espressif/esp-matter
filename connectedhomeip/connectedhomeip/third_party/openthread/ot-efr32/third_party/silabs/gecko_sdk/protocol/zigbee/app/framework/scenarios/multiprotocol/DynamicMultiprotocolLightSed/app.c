/***************************************************************************//**
 * @file app.c
 * @brief Callbacks implementation and application specific code.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER
#include "hal.h"
#include "ember.h"
#include "af.h"
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "sl_zigbee_debug_print.h"
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "app/framework/util/af-main.h"
#include "network-steering.h"

#include "app/framework/plugin/reporting/reporting.h"

#include "sl_component_catalog.h"
#include "app/util/common/uc-temp-macros.h"
#ifdef SL_CATALOG_ZIGBEE_BLE_EVENT_HANDLER_PRESENT
#include "sl_ble_event_handler.h"
#endif

#ifdef SL_CATALOG_ZIGBEE_DISPLAY_PRESENT
#include "sl_dmp_ui.h"
#else // !SL_CATALOG_ZIGBEE_DISPLAY_PRESENT
#include "sl_dmp_ui_stub.h"
#endif // SL_CATALOG_ZIGBEE_DISPLAY_PRESENT

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"

#define BUTTON0         0
#define BUTTON1         1

static uint8_t lastButton;
static bool longPress = false;
static bool leavingNwk = false;
static sl_zigbee_event_t button_event;
#endif //SL_CATALOG_SIMPLE_BUTTON_PRESENT

#if defined(SL_CATALOG_LED0_PRESENT)
#include "sl_led.h"
#include "sl_simple_led_instances.h"
#define led_turn_on(led) sl_led_turn_on(led)
#define led_turn_off(led) sl_led_turn_off(led)
#define led_toggle(led) sl_led_toggle(led)
#define LED0     (&sl_led_led0)
#define LED1     (&sl_led_led1)
#else // !SL_CATALOG_LED0_PRESENT
#define led_turn_on(led)
#define led_turn_off(led)
#define led_toggle(led)
#endif // SL_CATALOG_LED0_PRESENT

#define SOURCE_ADDRESS_LEN 8
static EmberEUI64 SwitchEUI;
static bool identifying = false;

//---------------------
// Forward declarations

static void startIdentifying(void);
static void stopIdentifying(void);
static void setDefaultReportEntry(void);
#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
static void toggleOnoffAttribute(void);
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT

//--------------
// Event handler

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
void buttonEventHandler(sl_zigbee_event_t *event)
{
  if (lastButton == BUTTON0) {
    toggleOnoffAttribute();
  } else if (lastButton == BUTTON1) {
    EmberNetworkStatus state = emberAfNetworkState();
    if (state == EMBER_NO_NETWORK) {
      EmberEUI64 wildcardEui64 = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };
      EmberKeyData centralizedKey = { { 0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C, 0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39 } };
      emberAddTransientLinkKey(wildcardEui64, &centralizedKey);
      emberAfPluginNetworkSteeringStart();
      sl_dmp_ui_display_zigbee_state(DMP_UI_JOINING);
    } else {
      if (!leavingNwk) { // Ignore button1 events while leaving.
        if (!longPress) {
          if (identifying) {
            emberAfAppPrintln("Button- Identify stop");
            stopIdentifying();
          } else if (state == EMBER_JOINED_NETWORK) {
            emberAfAppPrintln("Button- Identify start");
            startIdentifying();
          }
        } else {
          leavingNwk = true;
          emberAfAppPrintln("Button- Leave Nwk");
          emberLeaveNetwork();
          emberClearBindingTable();
        }
      }
    }
  }
}
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT

//----------------------
// Implemented Callbacks

/** @brief Init
 * Application init function
 */
void emberAfMainInitCallback(void)
{
  #if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
  sl_zigbee_event_init(&button_event, buttonEventHandler);
  #endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action. The framework
 * will always process the stack status after the callback returns.
 */
void emberAfStackStatusCallback(EmberStatus status)
{
  EmberNetworkStatus nwkState = emberAfNetworkState();
  emberAfCorePrintln("Stack status=0x%X, nwkState=%d", status, emberAfNetworkState());

  switch (nwkState) {
    case EMBER_JOINED_NETWORK:
      sl_dmp_ui_display_zigbee_state(DMP_UI_NETWORK_UP);
      startIdentifying();
      break;
    case EMBER_NO_NETWORK:
      sl_dmp_ui_display_zigbee_state(DMP_UI_NO_NETWORK);
#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
      leavingNwk = false; // leave has completed.
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT
      stopIdentifying();
      break;
    case EMBER_JOINED_NETWORK_NO_PARENT:
      stopIdentifying();
      break;
    default:
      break;
  }
}

/** @brief Start feedback.
 *
 * This function is called by the Identify plugin when identification begins.
 * It informs the Identify Feedback plugin that it should begin providing its
 * implemented feedback functionality (e.g., LED blinking, buzzer sounding,
 * etc.) until the Identify plugin tells it to stop. The identify time is
 * purely a matter of informational convenience. This plugin does not need to
 * know how long it will identify (the Identify plugin will perform the
 * necessary timekeeping.)
 *
 * @param endpoint The identifying endpoint Ver.: always
 * @param identifyTime The identify time Ver.: always
 */
void emberAfPluginIdentifyStartFeedbackCallback(uint8_t endpoint,
                                                uint16_t identifyTime)
{
  if (identifyTime > 0) {
    identifying = true;
    emberAfAppPrintln("Start Identifying for %dS", identifyTime);
    sl_dmp_ui_zigbee_permit_join(true);
    emberAfSetDefaultPollControlCallback(EMBER_AF_SHORT_POLL);  // Use short poll while identifying.
  }
}

/** @brief Stop feedback.
 *
 * This function is called by the Identify plugin when identification is
 * finished. It tells the Identify Feedback plugin to stop providing its
 * implemented feedback functionality.
 *
 * @param endpoint The identifying endpoint Ver.: always
 */
void emberAfPluginIdentifyStopFeedbackCallback(uint8_t endpoint)
{
  if (identifying) {
    identifying = false;
    emberAfAppPrintln("Stop Identifying");
    sl_dmp_ui_zigbee_permit_join(false);
    emberAfSetDefaultPollControlCallback(EMBER_AF_LONG_POLL); // Revert to long poll when we stop identifying.
  }
}

/** @brief Complete network steering.
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 *
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 *
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
  if (status == EMBER_SUCCESS) {
    setDefaultReportEntry();
    startIdentifying();
    sl_dmp_ui_display_zigbee_state(DMP_UI_NETWORK_UP);
  } else {
    stopIdentifying();
    sl_dmp_ui_display_zigbee_state(DMP_UI_NO_NETWORK);
  }
}

/** @brief Post Attribute Change
 *
 * This function is called by the application framework after it changes an
 * attribute value. The value passed into this callback is the value to which
 * the attribute was set by the framework.
 */
void emberAfPostAttributeChangeCallback(uint8_t endpoint,
                                        EmberAfClusterId clusterId,
                                        EmberAfAttributeId attributeId,
                                        uint8_t mask,
                                        uint16_t manufacturerCode,
                                        uint8_t type,
                                        uint8_t size,
                                        uint8_t* value)
{
  if (clusterId == ZCL_ON_OFF_CLUSTER_ID
      && attributeId == ZCL_ON_OFF_ATTRIBUTE_ID
      && mask == CLUSTER_MASK_SERVER) {
    uint8_t data;
    EmberStatus status = emberAfReadAttribute(endpoint,
                                              ZCL_ON_OFF_CLUSTER_ID,
                                              ZCL_ON_OFF_ATTRIBUTE_ID,
                                              CLUSTER_MASK_SERVER,
                                              (int8u*) &data,
                                              sizeof(data),
                                              NULL);

    if (status == EMBER_ZCL_STATUS_SUCCESS) {
      if (data == 0x00) {
        led_turn_off(LED0);
        led_turn_off(LED1);
        sl_dmp_ui_light_off();
#ifdef SL_CATALOG_ZIGBEE_BLE_EVENT_HANDLER_PRESENT
        zb_ble_dmp_notify_light(DMP_UI_LIGHT_OFF);
#endif
      } else {
        led_turn_on(LED0);
        led_turn_on(LED1);
#ifdef SL_CATALOG_ZIGBEE_BLE_EVENT_HANDLER_PRESENT
        zb_ble_dmp_notify_light(DMP_UI_LIGHT_ON);
#endif
        sl_dmp_ui_light_on();
      }
      if ((sl_dmp_ui_get_light_direction() == DMP_UI_DIRECTION_BLUETOOTH)
          || (sl_dmp_ui_get_light_direction() == DMP_UI_DIRECTION_SWITCH)) {
        sl_dmp_ui_update_direction(sl_dmp_ui_get_light_direction());
      } else {
        sl_dmp_ui_update_direction(DMP_UI_DIRECTION_ZIGBEE);
#ifdef SL_CATALOG_ZIGBEE_BLE_EVENT_HANDLER_PRESENT
        zb_ble_dmp_set_source_address(SwitchEUI);
#endif
      }

      sl_dmp_ui_set_light_direction(DMP_UI_DIRECTION_INVALID);
    }
  }
}

/** @brief Pre Command Received
 *
 * This callback is the second in the Application Framework's message processing
 * chain. At this point in the processing of incoming over-the-air messages, the
 * application has determined that the incoming message is a ZCL command. It
 * parses enough of the message to populate an EmberAfClusterCommand struct. The
 * Application Framework defines this struct value in a local scope to the
 * command processing but also makes it available through a global pointer
 * called emberAfCurrentCommand, in app/framework/util/util.c. When command
 * processing is complete, this pointer is cleared.
 */
bool emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
  if ((cmd->commandId == ZCL_ON_COMMAND_ID)
      || (cmd->commandId == ZCL_OFF_COMMAND_ID)
      || (cmd->commandId == ZCL_TOGGLE_COMMAND_ID)) {
    (void) memset(SwitchEUI, 0, SOURCE_ADDRESS_LEN);
    emberLookupEui64ByNodeId(cmd->source, SwitchEUI);
    emberAfCorePrintln(
      "SWITCH ZCL toggle/on/off EUI [%02X %02X %02X %02X %02X %02X %02X %02X]",
      SwitchEUI[7],
      SwitchEUI[6],
      SwitchEUI[5],
      SwitchEUI[4],
      SwitchEUI[3],
      SwitchEUI[2],
      SwitchEUI[1],
      SwitchEUI[0]);
  }
  return false;
}

/** @brief Trust Center Join
 *
 * This callback is called from within the application framework's
 * implementation of emberTrustCenterJoinHandler or ezspTrustCenterJoinHandler.
 * This callback provides the same arguments passed to the
 * TrustCenterJoinHandler. For more information about the TrustCenterJoinHandler
 * please see documentation included in stack/include/trust-center.h.
 */
void emberAfTrustCenterJoinCallback(EmberNodeId newNodeId,
                                    EmberEUI64 newNodeEui64,
                                    EmberNodeId parentOfNewNode,
                                    EmberDeviceUpdate status,
                                    EmberJoinDecision decision)
{
  if (status == EMBER_DEVICE_LEFT) {
    for (uint8_t i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
      EmberBindingTableEntry entry;
      emberGetBinding(i, &entry);
      if ((entry.type == EMBER_UNICAST_BINDING)
          && (entry.clusterId == ZCL_ON_OFF_CLUSTER_ID)
          && ((MEMCOMPARE(entry.identifier, newNodeEui64, EUI64_SIZE)
               == 0))) {
        emberDeleteBinding(i);
        emberAfAppPrintln("deleted binding entry: %d", i);
        break;
      }
    }
  }
}

/** @brief
 *
 * Application framework equivalent of ::emberRadioNeedsCalibratingHandler
 */
void emberAfRadioNeedsCalibratingCallback(void)
{
  sl_mac_calibrate_current_channel();
}

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
#define BUTTON_LONG_PRESS_TIME_MSEC    3000

/***************************************************************************//**
 * A callback called in interrupt context whenever a button changes its state.
 *
 * @remark Can be implemented by the application if required. This function
 * can contain the functionality to be executed in response to changes of state
 * in each of the buttons, or callbacks to appropriate functionality.
 *
 * @note The button state should not be updated in this function, it is updated
 * by specific button driver prior to arriving here
 *
   @param[out] handle             Pointer to button instance
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  static uint16_t buttonPressTime;
  uint16_t currentTime = 0;
  if ( sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (SL_SIMPLE_BUTTON_INSTANCE(BUTTON1) == handle) {
      buttonPressTime = halCommonGetInt16uMillisecondTick();
    }
  } else if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
    if (SL_SIMPLE_BUTTON_INSTANCE(BUTTON1) == handle) {
      currentTime = halCommonGetInt16uMillisecondTick();
      lastButton = BUTTON1;
      if ((currentTime - buttonPressTime) > BUTTON_LONG_PRESS_TIME_MSEC) {
        longPress = true;
      }
    } else {
      lastButton = BUTTON0;
    }

    sl_zigbee_common_rtos_wakeup_stack_task();
    sl_zigbee_event_set_active(&button_event);
  }
}
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT

//-----------------
// Static functions

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
static EmberEUI64 lightEUI;

static void toggleOnoffAttribute(void)
{
  EmberStatus status;
  uint8_t data;
  status = emberAfReadAttribute(emberAfPrimaryEndpoint(),
                                ZCL_ON_OFF_CLUSTER_ID,
                                ZCL_ON_OFF_ATTRIBUTE_ID,
                                CLUSTER_MASK_SERVER,
                                (int8u*) &data,
                                sizeof(data),
                                NULL);

  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if (data == 0x00) {
      data = 0x01;
    } else {
      data = 0x00;
    }

    sl_dmp_ui_set_light_direction(DMP_UI_DIRECTION_SWITCH);
    emberAfGetEui64(lightEUI);
#ifdef SL_CATALOG_ZIGBEE_BLE_EVENT_HANDLER_PRESENT
    zb_ble_dmp_set_source_address(lightEUI);
#endif
  } else {
    emberAfAppPrintln("read onoff attr: 0x%x", status);
  }

  status = emberAfWriteAttribute(emberAfPrimaryEndpoint(),
                                 ZCL_ON_OFF_CLUSTER_ID,
                                 ZCL_ON_OFF_ATTRIBUTE_ID,
                                 CLUSTER_MASK_SERVER,
                                 (int8u *) &data,
                                 ZCL_BOOLEAN_ATTRIBUTE_TYPE);
  emberAfAppPrintln("write to onoff attr: 0x%x", status);
}
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT

static void setDefaultReportEntry(void)
{
  EmberAfPluginReportingEntry reportingEntry;
  emberAfClearReportTableCallback();
  reportingEntry.direction = EMBER_ZCL_REPORTING_DIRECTION_REPORTED;
  reportingEntry.endpoint = emberAfPrimaryEndpoint();
  reportingEntry.clusterId = ZCL_ON_OFF_CLUSTER_ID;
  reportingEntry.attributeId = ZCL_ON_OFF_ATTRIBUTE_ID;
  reportingEntry.mask = CLUSTER_MASK_SERVER;
  reportingEntry.manufacturerCode = EMBER_AF_NULL_MANUFACTURER_CODE;
  reportingEntry.data.reported.minInterval = 0x0001;
  reportingEntry.data.reported.maxInterval = 0x001E; // 30S report interval for SED.
  reportingEntry.data.reported.reportableChange = 0; // onoff is bool type so it is unused
  emberAfPluginReportingConfigureReportedAttribute(&reportingEntry);
}

static bool writeIdentifyTime(uint16_t identifyTime)
{
  EmberAfStatus status =
    emberAfWriteServerAttribute(emberAfPrimaryEndpoint(),
                                ZCL_IDENTIFY_CLUSTER_ID,
                                ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                                (uint8_t *)&identifyTime,
                                sizeof(identifyTime));

  return (status == EMBER_ZCL_STATUS_SUCCESS);
}

static void startIdentifying(void)
{
  writeIdentifyTime(180);
}

static void stopIdentifying(void)
{
  writeIdentifyTime(0);

  sl_dmp_ui_zigbee_permit_join(false);
}
