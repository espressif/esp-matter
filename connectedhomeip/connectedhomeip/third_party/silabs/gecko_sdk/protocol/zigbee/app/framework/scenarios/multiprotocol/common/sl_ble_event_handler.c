/***************************************************************************//**
 * @file
 * @brief BLE related application related common code in the Zigbee BLE DMP sample apps
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_bluetooth.h"
#include "sl_bluetooth_advertiser_config.h"
#include "sl_bluetooth_connection_config.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_DISPLAY_PRESENT
#include "sl_dmp_ui.h"
#else
#include "sl_dmp_ui_stub.h"
#endif // SL_CATALOG_ZIGBEE_DISPLAY_PRESENT

#include "gatt_db.h"
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "sl_zigbee_debug_print.h"
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "sl_ble_event_handler.h"

sl_zigbee_event_t               attr_write_event;
#define attrWriteEvent          (&attr_write_event)
static void attrWriteEventHandler(SLXU_UC_EVENT);

void enableBleAdvertisements(void);
void BeaconAdvertisements(uint16_t devId);

void bleConnectionInfoTableInit(void);
uint8_t bleConnectionInfoTableFindUnused(void);
bool bleConnectionInfoTableIsEmpty(void);
void bleConnectionInfoTablePrintEntry(uint8_t index);
uint8_t bleConnectionInfoTableLookup(uint8_t connHandle);

#define SOURCE_ADDRESS_LEN 8
static uint8_t ble_lightState = DMP_UI_LIGHT_OFF;
static uint8_t ble_lastEvent = DMP_UI_DIRECTION_INVALID;
static uint8_t activeBleConnections = 0;

static gatt_client_config_flag_t ble_lightState_config = gatt_disable;
static gatt_client_config_flag_t ble_triggerSrc_config = gatt_disable;
static gatt_client_config_flag_t ble_bleSrc_config = gatt_disable;

static uint8_t SourceAddress[SOURCE_ADDRESS_LEN];

// Advertisement data
#define UINT16_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8))
#define UINT16_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT16_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))
// Ble TX test macros and functions
#define BLE_TX_TEST_DATA_SIZE   2
// We need to put the device name into a scan response packet,
// since it isn't included in the 'standard' beacons -
// I've included the flags, since certain apps seem to expect them
#define DEVNAME "DMP%02X%02X"
#define DEVNAME_LEN 8  // incl term null
#define UUID_LEN 16 // 128-bit UUID
#define SOURCE_ADDRESS_LEN 8

#define OTA_SCAN_RESPONSE_DATA        0x04
#define OTA_ADVERTISING_DATA          0x02
#define PUBLIC_DEVICE_ADDRESS         0
#define STATIC_RANDOM_ADDRESS         1
#define LE_GAP_NON_RESOLVABLE         0x04

// BLE CHARACTERISTIC RELATED  ---
/** GATT Server Attribute User Read Configuration.
 *  Structure to register handler functions to user read events. */
typedef struct {
  uint16_t charId; /**< ID of the Characteristic. */
  void (*fctn)(uint8_t connection); /**< Handler function. */
} AppCfgGattServerUserReadRequest_t;

/** GATT Server Attribute Value Write Configuration.
 *  Structure to register handler functions to characteristic write events. */
typedef struct {
  uint16_t charId; /**< ID of the Characteristic. */
  /**< Handler function. */
  void (*fctn)(uint8_t connection, uint8array * writeValue);
} AppCfgGattServerUserWriteRequest_t;

static const AppCfgGattServerUserReadRequest_t appCfgGattServerUserReadRequest[] =
{
  { gattdb_light_state, zb_ble_dmp_read_light_state },
  { gattdb_trigger_source, zb_ble_dmp_read_trigger_source },
  { gattdb_source_address, zb_ble_dmp_read_source_address },
  { 0, NULL }
};

static const AppCfgGattServerUserWriteRequest_t appCfgGattServerUserWriteRequest[] =
{
  { gattdb_light_state, zb_ble_dmp_write_light_state },
  { 0, NULL }
};

size_t appCfgGattServerUserReadRequestSize = COUNTOF(appCfgGattServerUserReadRequest) - 1;
size_t appCfgGattServerUserWriteRequestSize = COUNTOF(appCfgGattServerUserWriteRequest) - 1;
// --- BLE CHARACTERISTIC RELATED

/* Advertising handles */
enum {
  HANDLE_DEMO = 0,
  HANDLE_IBEACON = 1,
  HANDLE_EDDYSTONE = 2,

  MAX_ADV_HANDLES = 3
};
uint8_t adv_handle[MAX_ADV_HANDLES];

struct {
  bool inUse;
  bool isMaster;
  uint8_t connectionHandle;
  uint8_t bondingHandle;
  uint8_t remoteAddress[6];
} bleConnectionTable[SL_BT_CONFIG_MAX_CONNECTIONS];

struct responseData_t {
  uint8_t flagsLen; /**< Length of the Flags field. */
  uint8_t flagsType; /**< Type of the Flags field. */
  uint8_t flags; /**< Flags field. */
  uint8_t shortNameLen; /**< Length of Shortened Local Name. */
  uint8_t shortNameType; /**< Shortened Local Name. */
  uint8_t shortName[DEVNAME_LEN]; /**< Shortened Local Name. */
  uint8_t uuidLength; /**< Length of UUID. */
  uint8_t uuidType; /**< Type of UUID. */
  uint8_t uuid[UUID_LEN]; /**< 128-bit UUID. */
};

static struct responseData_t responseData = { 2, /* length (incl type) */
                                              0x01, /* type */
                                              0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */
                                              DEVNAME_LEN + 1, // length of local name (incl type)
                                              0x08, // shortened local name
                                              { 'D', 'M', '0', '0', ':', '0', '0' },
                                              UUID_LEN + 1, // length of UUID data (incl type)
                                              0x06, // incomplete list of service UUID's
                                              // custom service UUID for silabs lamp in little-endian format
                                              { 0xc9, 0x1b, 0x80, 0x3d, 0x61, 0x50, 0x0c, 0x97, 0x8d, 0x45, 0x19,
                                                0x7d, 0x96, 0x5b, 0xe5, 0xba } };

// iBeacon structure and data
static struct {
  uint8_t flagsLen; /* Length of the Flags field. */
  uint8_t flagsType; /* Type of the Flags field. */
  uint8_t flags; /* Flags field. */
  uint8_t mandataLen; /* Length of the Manufacturer Data field. */
  uint8_t mandataType; /* Type of the Manufacturer Data field. */
  uint8_t compId[2]; /* Company ID field. */
  uint8_t beacType[2]; /* Beacon Type field. */
  uint8_t uuid[16]; /* 128-bit Universally Unique Identifier (UUID). The UUID is an identifier for the company using the beacon*/
  uint8_t majNum[2]; /* Beacon major number. Used to group related beacons. */
  uint8_t minNum[2]; /* Beacon minor number. Used to specify individual beacons within a group.*/
  uint8_t txPower; /* The Beacon's measured RSSI at 1 meter distance in dBm. See the iBeacon specification for measurement guidelines. */
} iBeaconData = {
/* Flag bits - See Bluetooth 4.0 Core Specification , Volume 3, Appendix C, 18.1 for more details on flags. */
  2, /* length  */
  0x01, /* type */
  0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */

/* Manufacturer specific data */
  26, /* length of field*/
  0xFF, /* type of field */

/* The first two data octets shall contain a company identifier code from
 * the Assigned Numbers - Company Identifiers document */
  { UINT16_TO_BYTES(0x004C) },

/* Beacon type */
/* 0x0215 is iBeacon */
  { UINT16_TO_BYTE1(0x0215), UINT16_TO_BYTE0(0x0215) },

/* 128 bit / 16 byte UUID - generated specially for the DMP Demo */
  { 0x00, 0x47, 0xe7, 0x0a, 0x5d, 0xc1, 0x47, 0x25, 0x87, 0x99, 0x83, 0x05, 0x44,
    0xae, 0x04, 0xf6 },

/* Beacon major number - not used for this application */
  { UINT16_TO_BYTE1(256), UINT16_TO_BYTE0(256) },

/* Beacon minor number  - not used for this application*/
  { UINT16_TO_BYTE1(0), UINT16_TO_BYTE0(0) },

/* The Beacon's measured RSSI at 1 meter distance in dBm */
/* 0xC3 is -61dBm */
// TBD: check?
  0xC3
};

static struct {
  uint8_t flagsLen; /**< Length of the Flags field. */
  uint8_t flagsType; /**< Type of the Flags field. */
  uint8_t flags; /**< Flags field. */
  uint8_t serLen; /**< Length of Complete list of 16-bit Service UUIDs. */
  uint8_t serType; /**< Complete list of 16-bit Service UUIDs. */
  uint8_t serviceList[2]; /**< Complete list of 16-bit Service UUIDs. */
  uint8_t serDataLength; /**< Length of Service Data. */
  uint8_t serDataType; /**< Type of Service Data. */
  uint8_t uuid[2]; /**< 16-bit Eddystone UUID. */
  uint8_t frameType; /**< Frame type. */
  uint8_t txPower; /**< The Beacon's measured RSSI at 0 meter distance in dBm. */
  uint8_t urlPrefix; /**< URL prefix type. */
  uint8_t url[10]; /**< URL. */
} eddystone_data = {
/* Flag bits - See Bluetooth 4.0 Core Specification , Volume 3, Appendix C, 18.1 for more details on flags. */
  2, /* length  */
  0x01, /* type */
  0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */
/* Service field length */
  0x03,
/* Service field type */
  0x03,
/* 16-bit Eddystone UUID */
  { UINT16_TO_BYTES(0xFEAA) },
/* Eddystone-TLM Frame length */
  0x10,
/* Service Data data type value */
  0x16,
/* 16-bit Eddystone UUID */
  { UINT16_TO_BYTES(0xFEAA) },
/* Eddystone-URL Frame type */
  0x10,
/* Tx power */
  0x00,
/* URL prefix - standard */
  0x00,
/* URL */
  { 's', 'i', 'l', 'a', 'b', 's', '.', 'c', 'o', 'm' }
};

// to convert hex number to its ascii character
uint8_t ascii_lut[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void zb_ble_dmp_print_ble_address(uint8_t *address)
{
  emberAfCorePrint("\nBLE address: [%X %X %X %X %X %X]\n",
                   address[5], address[4], address[3],
                   address[2], address[1], address[0]);
}

void bleConnectionInfoTableInit(void)
{
  uint8_t i;
  for (i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    bleConnectionTable[i].inUse = false;
  }
}

uint8_t bleConnectionInfoTableFindUnused(void)
{
  uint8_t i;
  for (i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (!bleConnectionTable[i].inUse) {
      return i;
    }
  }
  return 0xFF;
}

bool bleConnectionInfoTableIsEmpty(void)
{
  uint8_t i;
  for (i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (bleConnectionTable[i].inUse) {
      return false;
    }
  }
  return true;
}

uint8_t bleConnectionInfoTableLookup(uint8_t connHandle)
{
  uint8_t i;
  for (i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (bleConnectionTable[i].inUse
        && bleConnectionTable[i].connectionHandle == connHandle) {
      return i;
    }
  }
  return 0xFF;
}

void bleConnectionInfoTablePrintEntry(uint8_t index)
{
  assert(index < SL_BT_CONFIG_MAX_CONNECTIONS
         && bleConnectionTable[index].inUse);
  emberAfCorePrintln("**** Connection Info index[%d]****", index);
  emberAfCorePrintln("connection handle 0x%x",
                     bleConnectionTable[index].connectionHandle);
  emberAfCorePrintln("bonding handle = 0x%x",
                     bleConnectionTable[index].bondingHandle);
  emberAfCorePrintln("local node is %s",
                     (bleConnectionTable[index].isMaster) ? "master" : "slave");
  emberAfCorePrint("remote address: ");
  zb_ble_dmp_print_ble_address(bleConnectionTable[index].remoteAddress);
  emberAfCorePrintln("");
  emberAfCorePrintln("*************************");
}

/* Characteristic read / write / notify handler functions */
void zb_ble_dmp_read_light_state(uint8_t connection)
{
  uint16_t sent_data_len;
  emberAfCorePrintln("Light state = %d\r\n", ble_lightState);
  /* Send response to read request */
  sl_status_t status =  sl_bt_gatt_server_send_user_read_response(connection,
                                                                  gattdb_light_state,
                                                                  SL_STATUS_OK,
                                                                  sizeof(ble_lightState),
                                                                  &ble_lightState,
                                                                  &sent_data_len);

  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Failed to zb_ble_dmp_read_light_state");
  }
}

void zb_ble_dmp_read_trigger_source(uint8_t connection)
{
  uint16_t sent_data_len;
  emberAfCorePrintln("Last event = %d\r\n", ble_lastEvent);

  /* Send response to read request */
  sl_status_t status =  sl_bt_gatt_server_send_user_read_response(connection,
                                                                  gattdb_trigger_source,
                                                                  SL_STATUS_OK,
                                                                  sizeof(ble_lastEvent),
                                                                  &ble_lastEvent,
                                                                  &sent_data_len);
  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Failed to zb_ble_dmp_read_trigger_source");
  }
}

void zb_ble_dmp_read_source_address(uint8_t connection)
{
  uint16_t sent_data_len;
  emberAfCorePrintln("zb_ble_dmp_read_source_address");

  /* Send response to read request */
  sl_status_t status =  sl_bt_gatt_server_send_user_read_response(connection,
                                                                  gattdb_source_address,
                                                                  SL_STATUS_OK,
                                                                  sizeof(SourceAddress),
                                                                  SourceAddress,
                                                                  &sent_data_len);

  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Failed to zb_ble_dmp_read_source_address");
  }
}

void zb_ble_dmp_write_light_state(uint8_t connection, uint8array *writeValue)
{
  emberAfCorePrintln("Light state write; %d\r\n", writeValue->data[0]);

  sl_dmp_ui_set_light_direction(DMP_UI_DIRECTION_BLUETOOTH);
  ble_lightState = writeValue->data[0];

  slxu_zigbee_event_set_active(attrWriteEvent);
  sl_zigbee_common_rtos_wakeup_stack_task();

  sl_status_t status = sl_bt_gatt_server_send_user_write_response(connection,
                                                                  gattdb_light_state,
                                                                  SL_STATUS_OK);

  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Failed to zb_ble_dmp_write_light_state");
    return;
  }

  uint8_t index = bleConnectionInfoTableLookup(connection);

  if (index != 0xFF) {
    (void) memset(SourceAddress, 0, sizeof(SourceAddress));
    for (int i = 0; i < SOURCE_ADDRESS_LEN - 2; i++) {
      SourceAddress[2 + i] =
        bleConnectionTable[index].remoteAddress[5 - i];
    }
  }
}

void zb_ble_dmp_notify_light(uint8_t lightState)
{
  ble_lightState = lightState;
  sl_status_t status;

  if (ble_lightState_config == gatt_indication) {
    emberAfCorePrintln("zb_ble_dmp_notify_light: Light state = %d\r\n", lightState);
    /* Send notification/indication data */
    for (int i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
      if (bleConnectionTable[i].inUse
          && bleConnectionTable[i].connectionHandle) {
        status = sl_bt_gatt_server_send_indication(bleConnectionTable[i].connectionHandle,
                                                   gattdb_light_state,
                                                   sizeof(lightState),
                                                   &lightState);

        if (status != SL_STATUS_OK) {
          emberAfCorePrintln("Failed to zb_ble_dmp_notify_light error : 0x%x", status);
          return;
        }
      }
    }
  }
}

void zb_ble_dmp_notify_trigger_source(uint8_t connection, uint8_t triggerSource)
{
  sl_status_t status;

  if (ble_triggerSrc_config == gatt_indication) {
    emberAfCorePrintln("zb_ble_dmp_notify_trigger_source :Last event = %d\r\n",
                       triggerSource);
    /* Send notification/indication data */
    status = sl_bt_gatt_server_send_indication(connection,
                                               gattdb_trigger_source,
                                               sizeof(triggerSource),
                                               &triggerSource);

    if (status != SL_STATUS_OK) {
      emberAfCorePrintln("Failed to zb_ble_dmp_notify_trigger_source");
      return;
    }
  }
}
void zb_ble_dmp_notify_source_address(uint8_t connection)
{
  sl_status_t status;

  if (ble_triggerSrc_config == gatt_indication) {
    /* Send notification/indication data */
    status = sl_bt_gatt_server_send_indication(connection,
                                               gattdb_source_address,
                                               sizeof(SourceAddress),
                                               SourceAddress);
    if (status != SL_STATUS_OK) {
      emberAfCorePrintln("Failed to zb_ble_dmp_notify_source_address");
      return;
    }
  }
}

void zb_ble_dmp_set_source_address(EmberEUI64 set_address)
{
  for (uint8_t i = 0; i < 8; i++) {
    SourceAddress[i] = set_address[(8 - 1) - i];
  }
}

void BeaconAdvertisements(uint16_t devId)
{
  static uint8_t *advData;
  static uint8_t advDataLen;
  sl_status_t status;

  iBeaconData.minNum[0] = UINT16_TO_BYTE1(devId);
  iBeaconData.minNum[1] = UINT16_TO_BYTE0(devId);

  advData = (uint8_t*) &iBeaconData;
  advDataLen = sizeof(iBeaconData);
  /* Set custom advertising data */
  status = sl_bt_legacy_advertiser_set_data(adv_handle[HANDLE_IBEACON], 0, advDataLen, advData);
  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Error sl_bt_legacy_advertiser_set_data code: 0x%0x", status);
    return;
  }

  status = sl_bt_advertiser_set_timing(adv_handle[HANDLE_IBEACON],   // handle
                                       (100 / 0.625), //100ms min adv interval in terms of 0.625ms
                                       (100 / 0.625), //100ms max adv interval in terms of 0.625ms
                                       0,   // duration : continue advertisement until stopped
                                       0);   // max_events :continue advertisement until stopped
  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Error iBeacon sl_bt_advertiser_set_timing code: 0x%0x", status);
    return;
  }

  status = sl_bt_advertiser_configure(adv_handle[HANDLE_IBEACON], LE_GAP_NON_RESOLVABLE);
  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Error iBeacon sl_bt_advertiser_configure code: 0x%0x", status);
    return;
  }

  status = sl_bt_legacy_advertiser_start(adv_handle[HANDLE_IBEACON],
                                         advertiser_non_connectable);
  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Error iBeacon sl_bt_legacy_advertiser_start code: 0x%0x", status);
    return;
  }

  advData = (uint8_t*) &eddystone_data;
  advDataLen = sizeof(eddystone_data);
  /* Set custom advertising data */
  status = sl_bt_legacy_advertiser_set_data(adv_handle[HANDLE_EDDYSTONE], 0, advDataLen, advData);
  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Error eddystone sl_bt_legacy_advertiser_set_data code: 0x%0x", status);
    return;
  }

  status = sl_bt_advertiser_set_timing(adv_handle[HANDLE_EDDYSTONE],   // handle
                                       (100 / 0.625), //100ms min adv interval in terms of 0.625ms
                                       (100 / 0.625), //100ms max adv interval in terms of 0.625ms
                                       0,   // duration : continue advertisement until stopped
                                       0);   // max_events :continue advertisement until stopped
  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Error eddystone sl_bt_advertiser_set_timing code: 0x%0x", status);
    return;
  }

  status = sl_bt_advertiser_configure(adv_handle[HANDLE_EDDYSTONE], LE_GAP_NON_RESOLVABLE);
  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Error eddystone sl_bt_advertiser_configure code: 0x%0x", status);
    return;
  }

  status = sl_bt_legacy_advertiser_start(adv_handle[HANDLE_EDDYSTONE],
                                         advertiser_non_connectable);
  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Error eddystone sl_bt_legacy_advertiser_start code: 0x%0x", status);
    return;
  }
}

void enableBleAdvertisements(void)
{
  sl_status_t status;

  /* Create the device Id and name based on the 16-bit truncated bluetooth address
     Copy to the local GATT database - this will be used by the BLE stack
     to put the local device name into the advertisements, but only if we are
     using default advertisements */
  uint8_t type;
  bd_addr ble_address;
  static char devName[DEVNAME_LEN];

  status = sl_bt_system_get_identity_address(&ble_address, &type);
  if ( status != SL_STATUS_OK ) {
    emberAfCorePrintln("Unable to get BLE address. Errorcode: 0x%x", status);
    return;
  }
  uint16_t devId = ((uint16_t)ble_address.addr[1] << 8) + (uint16_t)ble_address.addr[0];

  devName[0] = 'D';
  devName[1] = 'M';
  devName[2] = 'P';
  devName[3] = ascii_lut[( (ble_address.addr[1] & 0xF0) >> 4)];
  devName[4] = ascii_lut[(ble_address.addr[1] & 0x0F)];
  devName[5] = ascii_lut[( (ble_address.addr[0] & 0xF0) >> 4)];
  devName[6] = ascii_lut[(ble_address.addr[0] & 0x0F)];
  devName[7] = '\0';

  emberAfCorePrintln("devName = %s", devName);
  status = sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
                                                   0,
                                                   strlen(devName),
                                                   (uint8_t *)devName);

  if ( status != SL_STATUS_OK ) {
    emberAfCorePrintln("Unable to sl_bt_gatt_server_write_attribute_value device name. Errorcode: 0x%x", status);
    return;
  }

  sl_dmp_ui_set_ble_device_name(devName);   //LCD display

  /* Copy the shortened device name to the response data, overwriting
     the default device name which is set at compile time */
  MEMCOPY(((uint8_t*) &responseData) + 5, devName, 8);

  /* Set the advertising data and scan response data*/
  /* Note that the Wireless Gecko mobile app filters by a specific UUID and
     if the advertising data is not set, the device will not be found on the app*/
  status = sl_bt_legacy_advertiser_set_data(adv_handle[HANDLE_DEMO],
                                            0,      //advertising packets
                                            sizeof(responseData),
                                            (uint8_t*) &responseData);

  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Unable to set adv data sl_bt_legacy_advertiser_set_data. Errorcode: 0x%x", status);
    return;
  }

  status = sl_bt_legacy_advertiser_set_data(adv_handle[HANDLE_DEMO],
                                            1,      //scan response packets
                                            sizeof(responseData),
                                            (uint8_t*) &responseData);

  if (status != SL_STATUS_OK) {
    emberAfCorePrintln("Unable to set scan response data sl_bt_legacy_advertiser_set_data. Errorcode: 0x%x", status);
    return;
  }

  status = sl_bt_advertiser_set_timing(adv_handle[HANDLE_DEMO],
                                       (100 / 0.625), //100ms min adv interval in terms of 0.625ms
                                       (100 / 0.625), //100ms max adv interval in terms of 0.625ms
                                       0,   // duration : continue advertisement until stopped
                                       0);   // max_events :continue advertisement until stopped
  if (status != SL_STATUS_OK) {
    return;
  }
  status = sl_bt_advertiser_set_report_scan_request(adv_handle[HANDLE_DEMO], 1);   //scan request reported as events
  if (status != SL_STATUS_OK) {
    return;
  }
  /* Start advertising in user mode and enable connections*/
  status = sl_bt_legacy_advertiser_start(adv_handle[HANDLE_DEMO],
                                         advertiser_connectable_scannable);
  if ( status ) {
    emberAfCorePrintln("sl_bt_legacy_advertiser_start ERROR : status = 0x%0X", status);
  } else {
    emberAfCorePrintln("BLE custom advertisements enabled");
  }
  if (SL_BT_CONFIG_USER_ADVERTISERS >= 3) {
    BeaconAdvertisements(devId);
  }
}

/** @brief
 *
 * This function is called from the BLE stack to notify the application of a
 * stack event.
 */
void sl_bt_on_event(sl_bt_msg_t* evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    /* This event indicates that a remote GATT client is attempting to read a value of an
     *  attribute from the local GATT database, where the attribute was defined in the GATT
     *  XML firmware configuration file to have type="user". */

    case sl_bt_evt_gatt_server_user_read_request_id:
      for (uint32_t i = 0; i < appCfgGattServerUserReadRequestSize; i++) {
        if ((appCfgGattServerUserReadRequest[i].charId
             == evt->data.evt_gatt_server_user_read_request.characteristic)
            && (appCfgGattServerUserReadRequest[i].fctn)) {
          appCfgGattServerUserReadRequest[i].fctn(
            evt->data.evt_gatt_server_user_read_request.connection);
        }
      }
      break;

    /* This event indicates that a remote GATT client is attempting to write a value of an
     * attribute in to the local GATT database, where the attribute was defined in the GATT
     * XML firmware configuration file to have type="user".  */

    case sl_bt_evt_gatt_server_user_write_request_id:
      for (uint32_t i = 0; i < appCfgGattServerUserWriteRequestSize; i++) {
        if ((appCfgGattServerUserWriteRequest[i].charId
             == evt->data.evt_gatt_server_characteristic_status.characteristic)
            && (appCfgGattServerUserWriteRequest[i].fctn)) {
          appCfgGattServerUserWriteRequest[i].fctn(
            evt->data.evt_gatt_server_user_read_request.connection,
            &(evt->data.evt_gatt_server_attribute_value.value));
        }
      }
      break;

    case sl_bt_evt_system_boot_id: {
      bd_addr ble_address;
      uint8_t type;
      sl_status_t status = sl_bt_system_hello();
      emberAfCorePrintln("BLE hello: %s",
                         (status == SL_STATUS_OK) ? "success" : "error");

      status = sl_bt_system_get_identity_address(&ble_address, &type);
      zb_ble_dmp_print_ble_address(ble_address.addr);

      status = sl_bt_advertiser_create_set(&adv_handle[HANDLE_DEMO]);
      if (status) {
        emberAfCorePrintln("sl_bt_advertiser_create_set status 0x%x", status);
      }

      status = sl_bt_advertiser_create_set(&adv_handle[HANDLE_IBEACON]);
      if (status) {
        emberAfCorePrintln("sl_bt_advertiser_create_set status 0x%x", status);
      }

      status = sl_bt_advertiser_create_set(&adv_handle[HANDLE_EDDYSTONE]);
      if (status) {
        emberAfCorePrintln("sl_bt_advertiser_create_set status 0x%x", status);
      }

      // start advertising
      enableBleAdvertisements();
    }
    break;
    case sl_bt_evt_gatt_server_characteristic_status_id: {
      sl_bt_evt_gatt_server_characteristic_status_t *StatusEvt =
        (sl_bt_evt_gatt_server_characteristic_status_t*) &(evt->data);
      if (StatusEvt->status_flags == gatt_server_confirmation) {
        emberAfCorePrintln(
          "characteristic= %d , GAT_SERVER_CLIENT_CONFIG_FLAG = %d\r\n",
          StatusEvt->characteristic, StatusEvt->client_config_flags);
        if (StatusEvt->characteristic == gattdb_light_state) {
          zb_ble_dmp_notify_trigger_source(StatusEvt->connection, ble_lastEvent);
        } else if (StatusEvt->characteristic == gattdb_trigger_source) {
          zb_ble_dmp_notify_source_address(StatusEvt->connection);
        }
      } else if (StatusEvt->status_flags == gatt_server_client_config) {
        if (StatusEvt->characteristic == gattdb_light_state) {
          ble_lightState_config = (gatt_client_config_flag_t)StatusEvt->client_config_flags;
        } else if (StatusEvt->characteristic == gattdb_trigger_source) {
          ble_triggerSrc_config = (gatt_client_config_flag_t)StatusEvt->client_config_flags;
        } else if (StatusEvt->characteristic == gattdb_source_address) {
          ble_bleSrc_config = (gatt_client_config_flag_t)StatusEvt->client_config_flags;
        }
        emberAfCorePrintln(
          "SERVER : ble_lightState_config= %d , ble_triggerSrc_config = %d , ble_bleSrc_config = %d\r\n",
          ble_lightState_config,
          ble_triggerSrc_config,
          ble_bleSrc_config);
      }
    }
    break;
    case sl_bt_evt_connection_opened_id: {
      emberAfCorePrintln("sl_bt_evt_connection_opened_id \n");
      sl_bt_evt_connection_opened_t *conn_evt =
        (sl_bt_evt_connection_opened_t*) &(evt->data);
      uint8_t index = bleConnectionInfoTableFindUnused();
      if (index == 0xFF) {
        emberAfCorePrintln("MAX active BLE connections");
        assert(index < 0xFF);
      } else {
        bleConnectionTable[index].inUse = true;
        bleConnectionTable[index].isMaster = (conn_evt->master > 0);
        bleConnectionTable[index].connectionHandle = conn_evt->connection;
        bleConnectionTable[index].bondingHandle = conn_evt->bonding;
        (void) memcpy(bleConnectionTable[index].remoteAddress,
                      conn_evt->address.addr, 6);

        activeBleConnections++;
        //preferred phy 1: 1M phy, 2: 2M phy, 4: 125k coded phy, 8: 500k coded phy
        //accepted phy 1: 1M phy, 2: 2M phy, 4: coded phy, ff: any
        sl_bt_connection_set_preferred_phy(conn_evt->connection, test_phy_1m, 0xff);
        enableBleAdvertisements();
        emberAfCorePrintln("BLE connection opened");
        bleConnectionInfoTablePrintEntry(index);
        emberAfCorePrintln("%d active BLE connection",
                           activeBleConnections);
      }
    }
    break;
    case sl_bt_evt_connection_phy_status_id: {
      sl_bt_evt_connection_phy_status_t *conn_evt =
        (sl_bt_evt_connection_phy_status_t *)&(evt->data);
      // indicate the PHY that has been selected
      emberAfCorePrintln("now using the %dMPHY\r\n",
                         conn_evt->phy);
    }
    break;
    case sl_bt_evt_connection_closed_id: {
      sl_bt_evt_connection_closed_t *conn_evt =
        (sl_bt_evt_connection_closed_t*) &(evt->data);
      uint8_t index = bleConnectionInfoTableLookup(conn_evt->connection);
      assert(index < 0xFF);

      bleConnectionTable[index].inUse = false;
      if ( activeBleConnections ) {
        --activeBleConnections;
      }
      // restart advertising, set connectable
      enableBleAdvertisements();
      if (bleConnectionInfoTableIsEmpty()) {
        sl_dmp_ui_bluetooth_connected(false);
      }
      emberAfCorePrintln(
        "BLE connection closed, handle=0x%x, reason=0x%2x : [%d] active BLE connection",
        conn_evt->connection, conn_evt->reason, activeBleConnections);
    }
    break;
    case sl_bt_evt_scanner_scan_report_id: {
      sl_bt_evt_scanner_scan_report_t *scan_evt =
        (sl_bt_evt_scanner_scan_report_t*) &(evt->data);
      emberAfCorePrint("Scan response, address type=0x%x, address: ",
                       scan_evt->address_type);
      zb_ble_dmp_print_ble_address(scan_evt->address.addr);
      emberAfCorePrintln("");
    }
    break;
    case sl_bt_evt_connection_parameters_id: {
      sl_bt_evt_connection_parameters_t* param_evt =
        (sl_bt_evt_connection_parameters_t*) &(evt->data);
      emberAfCorePrintln(
        "BLE connection parameters are updated, handle=0x%x, interval=0x%2x, latency=0x%2x, timeout=0x%2x, security=0x%x, txsize=0x%2x",
        param_evt->connection,
        param_evt->interval,
        param_evt->latency,
        param_evt->timeout,
        param_evt->security_mode,
        param_evt->txsize);
      sl_dmp_ui_bluetooth_connected(true);
    }
    break;

    case sl_bt_evt_gatt_service_id: {
      sl_bt_evt_gatt_service_t* service_evt =
        (sl_bt_evt_gatt_service_t*) &(evt->data);
      uint8_t i;
      emberAfCorePrintln(
        "GATT service, conn_handle=0x%x, service_handle=0x%4x",
        service_evt->connection, service_evt->service);
      emberAfCorePrint("UUID=[");
      for (i = 0; i < service_evt->uuid.len; i++) {
        emberAfCorePrint("0x%x ", service_evt->uuid.data[i]);
      }
      emberAfCorePrintln("]");
    }
    break;

    default:
      break;
  }
}

// Initialization of all application code
void sli_ble_application_init(uint8_t init_level)
{
  switch (init_level) {
    case SL_ZIGBEE_INIT_LEVEL_EVENT:
    {
      slxu_zigbee_event_init(attrWriteEvent, attrWriteEventHandler);
      break;
    }

    case SL_ZIGBEE_INIT_LEVEL_LOCAL_DATA:
    {
      bleConnectionInfoTableInit();
      break;
    }
  }
}

void attrWriteEventHandler(SLXU_UC_EVENT)
{
  slxu_zigbee_event_set_inactive(attrWriteEvent);

  (void) emberAfWriteAttribute(emberAfPrimaryEndpoint(),
                               ZCL_ON_OFF_CLUSTER_ID,
                               ZCL_ON_OFF_ATTRIBUTE_ID,
                               CLUSTER_MASK_SERVER,
                               &ble_lightState,
                               ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}

void zb_ble_dmp_print_ble_connections()
{
  uint8_t i;
  for (i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (bleConnectionTable[i].inUse) {
      bleConnectionInfoTablePrintEntry(i);
    }
  }
}
