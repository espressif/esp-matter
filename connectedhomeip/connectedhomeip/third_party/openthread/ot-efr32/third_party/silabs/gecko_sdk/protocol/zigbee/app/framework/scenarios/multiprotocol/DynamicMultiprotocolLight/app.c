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

#include "app/framework/plugin/network-creator-security/network-creator-security.h"
#include "app/framework/plugin/network-creator/network-creator.h"
#include "app/framework/plugin/reporting/reporting.h"
#ifdef SL_CATALOG_ZIGBEE_BLE_EVENT_HANDLER_PRESENT
#include "sl_ble_event_handler.h"
#endif

#include "sl_component_catalog.h"
#include "app/util/common/uc-temp-macros.h"

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
static sl_zigbee_event_t button_event;
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT

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

//---------------------
// Forward declarations

static void startIdentifyOnAllChildNodes(uint16_t identifyTime);
static void setDefaultReportEntry(void);
static bool startPjoinAndIdentifying(uint16_t identifyTime);
#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
static void toggleOnoffAttribute(void);
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT

//--------------
// Event handler

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
static void button_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  if (lastButton == BUTTON0) {
    toggleOnoffAttribute();
  } else if (lastButton == BUTTON1) {
    EmberNetworkStatus state = emberAfNetworkState();
    if (state != EMBER_JOINED_NETWORK) {
      sl_dmp_ui_display_zigbee_state(DMP_UI_FORMING);
      status = emberAfPluginNetworkCreatorStart(true); // centralized
      sl_zigbee_app_debug_print("%s network %s: 0x%02X\n", "Form centralized", "start", status);
    } else {
      // joined on NWK
      if (longPress == false) {
        if (emberGetPermitJoining()) {
          emberPermitJoining(0);
        } else {
          if (startPjoinAndIdentifying(180)) {
            sl_dmp_ui_zigbee_permit_join(true);
            sl_zigbee_app_debug_print("pJoin for 180 sec\n");
          }
        }
      } else {
        status = emberLeaveNetwork();
        emberClearBindingTable();
        sl_zigbee_app_debug_print("leave NWK: 0x%02X\n", status);
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
  sl_zigbee_event_init(&button_event, button_event_handler);
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
  sl_dmp_ui_display_zigbee_state(DMP_UI_STATE_UNKNOWN);// UI will work out and display the present Nwk state.

  switch (status) {
    case EMBER_NETWORK_UP:
      setDefaultReportEntry();
      sl_dmp_ui_display_zigbee_state(DMP_UI_NETWORK_UP);
      if (startPjoinAndIdentifying(180)) {
        sl_dmp_ui_zigbee_permit_join(true);
        sl_zigbee_app_debug_print("%s network %s: 0x%02X\n", "Open", "for joining", status);
      }
      break;
    case EMBER_NETWORK_DOWN:
      sl_dmp_ui_display_zigbee_state(DMP_UI_NO_NETWORK);
#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
      longPress = false;
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT
      break;
    default:
      break;
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
    // Only print when rx prints are enabled
    if (emberAfPrintReceivedMessages) {
      sl_zigbee_app_debug_print(
        "SWITCH ZCL toggle/on/off EUI [%02X %02X %02X %02X %02X %02X %02X %02X]\n",
        SwitchEUI[7],
        SwitchEUI[6],
        SwitchEUI[5],
        SwitchEUI[4],
        SwitchEUI[3],
        SwitchEUI[2],
        SwitchEUI[1],
        SwitchEUI[0]);
    }
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
        sl_zigbee_app_debug_print("deleted binding entry: %d\n", i);
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
static void toggleOnoffAttribute(void)
{
  static EmberEUI64 lightEUI;
  EmberStatus status;
  int8u data;
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
    sl_zigbee_app_debug_print("read onoff attr: 0x%x\n", status);
  }

  status = emberAfWriteAttribute(emberAfPrimaryEndpoint(),
                                 ZCL_ON_OFF_CLUSTER_ID,
                                 ZCL_ON_OFF_ATTRIBUTE_ID,
                                 CLUSTER_MASK_SERVER,
                                 (int8u *) &data,
                                 ZCL_BOOLEAN_ATTRIBUTE_TYPE);
  sl_zigbee_app_debug_print("write to onoff attr: 0x%x\n", status);
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
  reportingEntry.data.reported.maxInterval = 0x0002;
  reportingEntry.data.reported.reportableChange = 0; //onoff is bool type so it is unused
  emberAfPluginReportingConfigureReportedAttribute(&reportingEntry);
}

static bool startPjoinAndIdentifying(uint16_t identifyTime)
{
  EmberStatus status;
  EmberEUI64 wildcardEui64 =
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };
  EmberKeyData centralizedKey = { { 0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41,
                                    0x6C, 0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39 } };

  emberAddTransientLinkKey(wildcardEui64, &centralizedKey);
  status = emberPermitJoining(identifyTime);

  emberAfWriteServerAttribute(emberAfPrimaryEndpoint(),
                              ZCL_IDENTIFY_CLUSTER_ID,
                              ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                              (uint8_t *) &identifyTime,
                              sizeof(identifyTime));

  startIdentifyOnAllChildNodes(identifyTime);

  return (status == EMBER_SUCCESS);
}

static void startIdentifyOnAllChildNodes(uint16_t identifyTime)
{
  // Attempt to start Identify on all connected child nodes.

  const uint8_t childTableSize = emberAfGetChildTableSize();
  uint8_t i;

  // Iterate through the child table and try to find the device's child data
  for (i = 0; i < childTableSize; ++i) {
    EmberChildData childData;
    if (emberAfGetChildData(i, &childData) == EMBER_SUCCESS) {
      // Write Identify Time attribute on child.
      uint8_t outgoingBuffer[9];
      uint8_t idx = 0;
      EmberApsFrame apsFrame = { 0 };

      apsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
      apsFrame.clusterId = ZCL_IDENTIFY_CLUSTER_ID;
      apsFrame.sourceEndpoint = 0x01;
      apsFrame.destinationEndpoint = 0x01;

      outgoingBuffer[idx++] = ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
      outgoingBuffer[idx++] = emberAfNextSequence();
      outgoingBuffer[idx++] = ZCL_WRITE_ATTRIBUTES_COMMAND_ID;
      outgoingBuffer[idx++] = LOW_BYTE(ZCL_IDENTIFY_TIME_ATTRIBUTE_ID);
      outgoingBuffer[idx++] = HIGH_BYTE(ZCL_IDENTIFY_TIME_ATTRIBUTE_ID);
      outgoingBuffer[idx++] = ZCL_INT16U_ATTRIBUTE_TYPE;
      outgoingBuffer[idx++] = LOW_BYTE(identifyTime);
      outgoingBuffer[idx++] = HIGH_BYTE(identifyTime);

      emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                         childData.id,
                         &apsFrame,
                         idx,
                         outgoingBuffer);
    }
  }
}
