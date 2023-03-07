/***************************************************************************//**
 * @file
 * @brief Communication component implementing BLE Apploader OTA DFU protocol
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Includes

#include "btl_apploader.h"
#include "btl_apploader_callback.h"
#include "sl_apploader_lib_api.h"

#include "api/application_properties.h"
#include "config/btl_config.h"
#include "sl_device_init_clocks.h"

#if defined(BOOTLOADER_NONSECURE)
// NS headers
  #include "core/btl_bootload_ns.h"
  #include "core/btl_reset_ns.h"
#else
  #include "core/btl_bootload.h"
  #include "core/btl_reset.h"
#endif

#include <stdint.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Defines

#define USE_RANDOM_ADDRESS 0
#define USE_CUSTOM_ADDRESS 0
#define CUSTOM_ADDRESS "\x00\x11\x22\x33\x44\x55"
#define SET_RF_PATH 0
#define RF_PATH 255
#define TX_POWER 0

// Advertising data
#define DEVICE_NAME                   "OTA"
#define DEVICE_NAME_LENGTH            3
#define DEVICE_NAME_LENGTH_MAX        17
#define ADV_DATA                      "\x02\x01\x06" "\x04" "\x09"
#define ADV_DATA_LENGTH               5
#define ADV_DATA_SIZE_MAX             32
#define ADV_DATA_DEVICE_ADDR          "\x08\x1b"
#define ADV_DATA_DEVICE_ADDR_LENGTH   2
#define SCAN_RSP_DATA                 "\x02\x0a"
#define SCAN_RSP_DATA_LENGTH          3

// -----------------------------------------------------------------------------
// Static local variables

static volatile bool isConnected;
static volatile bool hasDisconnected;
static volatile bool started;

static ImageProperties_t *apploader_imageProps;
#if !defined (BOOTLOADER_NONSECURE)
static const BootloaderParserCallbacks_t *apploader_parseCb;
static void *apploader_parserContext;
static void *apploader_decryptContext;
static void *apploader_authContext;
#endif

// -----------------------------------------------------------------------------
// Static local functions

// -----------------------------------------------------------------------------
// Global Functions

// -----------------------------------------------------------------------------
// Callback functions called from AppLoader OTA DFU library

void bootloader_apploader_connection_complete()
{
  isConnected = true;
  hasDisconnected = false;
  started = false;
}

void bootloader_apploader_disconnection_complete()
{
  isConnected = false;
  hasDisconnected = true;

  if (apploader_imageProps->imageCompleted && apploader_imageProps->imageVerified) {
    if (apploader_imageProps->contents & BTL_IMAGE_CONTENT_SE) {
      if (bootload_checkSeUpgradeVersion(apploader_imageProps->seUpgradeVersion)) {
        // Install SE upgrade
#if defined(BOOTLOADER_NONSECURE)
        bootload_commitSeUpgrade();
#else
        bootload_commitSeUpgrade(BTL_UPGRADE_LOCATION);
#endif
      }
    }
    if (apploader_imageProps->contents & BTL_IMAGE_CONTENT_BOOTLOADER) {
      if (apploader_imageProps->bootloaderVersion > bootload_getBootloaderVersion()) {
        // Install bootloader upgrade
#if defined(BOOTLOADER_NONSECURE)
        bootload_commitBootloaderUpgrade(apploader_imageProps->bootloaderUpgradeSize);
#else
        bootload_commitBootloaderUpgrade(BTL_UPGRADE_LOCATION, apploader_imageProps->bootloaderUpgradeSize);
#endif
      }
    }
  }
}

uint32_t bootloader_apploader_get_bootloader_version()
{
  return bootload_getBootloaderVersion();
}

uint32_t bootloader_apploader_get_application_version()
{
  uint32_t appVersion = 0;
  if (bootload_getApplicationVersion(&appVersion)) {
    return appVersion;
  }

  return 0u;
}

int32_t bootloader_apploader_parse_gbl(uint8_t *data, size_t len)
{
  int32_t ret;
  started = true;
#if defined(BOOTLOADER_NONSECURE)
  ret = parser_parse(data,
                     len,
                     apploader_imageProps);
#else
  ret = parser_parse((ParserContext_t *)apploader_parserContext,
                     apploader_imageProps,
                     data,
                     len,
                     apploader_parseCb);
#endif
  return ret;
}

int32_t bootloader_apploader_parser_init()
{
#if defined(BOOTLOADER_NONSECURE)
  return parser_init(PARSER_FLAG_PARSE_CUSTOM_TAGS);
#else
  return parser_init((ParserContext_t *)apploader_parserContext,
                     (DecryptContext_t *)apploader_decryptContext,
                     (AuthContext_t *)apploader_authContext,
                     PARSER_FLAG_PARSE_CUSTOM_TAGS);
#endif
}

int32_t bootloader_apploader_parser_finish()
{
  int32_t ret;
  if (apploader_imageProps->imageCompleted && apploader_imageProps->imageVerified) {
    ret = BOOTLOADER_OK;
  } else if (!apploader_imageProps->imageVerified) {
    ret = BOOTLOADER_ERROR_PARSER_REJECTED;
  } else {
    ret = BOOTLOADER_ERROR_COMMUNICATION_IMAGE_ERROR;
  }

  return ret;
}

// -----------------------------------------------------------------------------
// Communication plugin functions

void bootloader_apploader_communication_init(void)
{
#if !defined(BOOTLOADER_NONSECURE)
  sl_device_init_clocks();
#endif

  // Configure Bluetooth
  static sl_apploader_config_t btConfig = {
    .txPower = TX_POWER,
    .address = NULL,
  };

  sl_apploader_address_t btAddress;
  if (USE_RANDOM_ADDRESS) {
    sl_apploader_get_static_random_device_address(&btAddress);
    btConfig.address = &btAddress;
  }
  if (USE_CUSTOM_ADDRESS) {
    memcpy(btAddress.address, CUSTOM_ADDRESS, 6);
    btAddress.type = sl_apploader_address_type_public;
    btConfig.address = &btAddress;
  }
  sl_apploader_init(&btConfig);

  if (SET_RF_PATH) {
    uint8_t rfPath = RF_PATH;
    sl_apploader_select_rf_path(rfPath);
  }
}

int32_t bootloader_apploader_communication_start(void)
{
  int32_t ret = BOOTLOADER_OK;

  //set advertising data
  uint8_t advData[ADV_DATA_SIZE_MAX];
  uint8_t advDataLen = 0;
  memcpy(advData, ADV_DATA, ADV_DATA_LENGTH);
  advData[3] = DEVICE_NAME_LENGTH + 1;
  memcpy(advData + ADV_DATA_LENGTH, DEVICE_NAME, DEVICE_NAME_LENGTH);
  advDataLen = ADV_DATA_LENGTH + DEVICE_NAME_LENGTH;

  //Set public device address to advertisement data
  sl_apploader_address_t deviceAddr;
  sl_apploader_get_device_address(&deviceAddr);
  memcpy(advData + advDataLen, ADV_DATA_DEVICE_ADDR, ADV_DATA_DEVICE_ADDR_LENGTH);
  advDataLen += ADV_DATA_DEVICE_ADDR_LENGTH;
  sl_apploader_address_type_t addrType = deviceAddr.type;
  memcpy(advData + advDataLen, &addrType, sizeof(sl_apploader_address_type_t));
  memcpy(advData + advDataLen + sizeof(sl_apploader_address_type_t), &deviceAddr.address, sizeof(deviceAddr.address));
  advDataLen += sizeof(sl_apploader_address_t);

  //set scan response data
  uint8_t scanRspData[ADV_DATA_SIZE_MAX];
  uint8_t scanRspDataLen = ADV_DATA_SIZE_MAX;
  memcpy(scanRspData, SCAN_RSP_DATA, SCAN_RSP_DATA_LENGTH);
  scanRspData[SCAN_RSP_DATA_LENGTH] = TX_POWER;
  scanRspDataLen = SCAN_RSP_DATA_LENGTH;

  //Create GATT DB
  sl_apploader_reset_database();
  sl_apploader_database_add_gatt_service();
  sl_apploader_database_add_gap_service(DEVICE_NAME, DEVICE_NAME_LENGTH);
  sl_apploader_database_add_ota_service();
  sl_apploader_set_database();

  sl_apploader_set_advertising_data(advData, advDataLen);
  sl_apploader_set_scan_response_data(scanRspData, scanRspDataLen);
  if (sl_apploader_enable_advertising(true) != SL_STATUS_OK) {
    ret = BOOTLOADER_ERROR_COMMUNICATION_INIT;
  }
  return ret;
}

int32_t bootloader_apploader_communication_main(ImageProperties_t *imageProps,
                                                void *parserContext,
                                                void *decryptContext,
                                                void *authContext,
                                                const BootloaderParserCallbacks_t *parseCb)
{
  apploader_imageProps = imageProps;
#if !defined (BOOTLOADER_NONSECURE)
  apploader_parserContext = parserContext;
  apploader_decryptContext = decryptContext;
  apploader_authContext = authContext;
  apploader_parseCb = parseCb;
#endif

  while (1) {
    if (isConnected) {
      sl_apploader_run();
    }

    if (hasDisconnected) {
      if (!started) {
        return BOOTLOADER_ERROR_COMMUNICATION_DONE;
      } else if (!apploader_imageProps->imageCompleted || !apploader_imageProps->imageVerified) {
        return BOOTLOADER_ERROR_COMMUNICATION_IMAGE_ERROR;
      } else {
        return BOOTLOADER_OK;
      }
    }
  }
}
