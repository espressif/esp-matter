/***************************************************************************/
/**
 * @file
 * @brief Bluetooth CLI support
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#if SL_OPENTHREAD_BLE_CLI_ENABLE

#include <openthread/cli.h>
#include "common/code_utils.hpp"

#include "sl_bt_api.h"
#include "sl_ot_custom_cli.h"

static int Hex2Bin(const char *aHex, uint8_t *aBin, uint16_t aBinLength, bool aAllowTruncate);
static void helpCommand(void *context, uint8_t argc, char *argv[]);
void printBleAddress(bd_addr address);

static otError ParseUnsignedLong(char *aString, unsigned long *aLong)
{
    char *endptr;
    *aLong = strtoul(aString, &endptr, 0);
    return (*endptr == '\0') ? OT_ERROR_NONE : OT_ERROR_PARSE;
}

static void getAddressCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    sl_status_t status;
    bd_addr address;
    uint8_t type;
    status = sl_bt_system_get_identity_address(&address, &type);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("BLE address: ");
        printBleAddress(address);
        otCliOutputFormat("\r\n");
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }
}

static void createAdvSetCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    sl_status_t status;
    uint8_t handle;

    status = sl_bt_advertiser_create_set(&handle);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success handle=%d\r\n", handle);
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }
}

static void setAdvTimingCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long handle, interval_min, interval_max, duration, max_events;

    VerifyOrExit(argc == 5, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[0], &handle));
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &interval_min));
    SuccessOrExit(error = ParseUnsignedLong(argv[2], &interval_max));
    SuccessOrExit(error = ParseUnsignedLong(argv[3], &duration));
    SuccessOrExit(error = ParseUnsignedLong(argv[4], &max_events));

    status = sl_bt_advertiser_set_timing(handle, interval_min, interval_max, duration, max_events);
    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

static void setAdvRandomAddressCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long handle;
    bd_addr unusedAddress;

    VerifyOrExit(argc == 1, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[0], &handle));
    // TO DO: Check the third parameter.
    status = sl_bt_advertiser_set_random_address(handle, 2, unusedAddress, &unusedAddress);
    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
	    otCliOutputFormat("Random address: ");
	    printBleAddress(unusedAddress);
	    otCliOutputFormat("\r\n");
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

static void startAdvertisingCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long handle, discoverableMode, connectableMode;

    VerifyOrExit(argc == 3, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[0], &handle));
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &discoverableMode));
    SuccessOrExit(error = ParseUnsignedLong(argv[2], &connectableMode));

    /**
     *  Advertising discoverable mode handling
     */

    // In new Bluetooth advertiser APIs, discoverable mode handling is separated
    // from the starting advertising functionality. The discoverable mode is a
    // value carried by one flag field in the advertising data packet. In EFR
    // Bluetooth stack, the mode should either be set by the user application if
    // user-defined advertising data is used, or set by the Bluetooth stack
    // when it is asked to generate the advertising data for the application.
    // The code lines for generating advertising data in the stack (the
    // @ref sl_bt_legacy_advertiser_generate_data command) uses more than 800
    // bytes of flash, and thus it was refactored to own section so that
    // applications using user-defined advertising data can save some flash
    // space (Using user-defined advertising data is quite typical in customer
    // products, and in the Zigbee DMP sample app as well.)
    // Call the @ref sl_bt_legacy_advertiser_set_data command to set user-defined
    // advertising data, as the alternative of asking the stack to generate data.

    // If the discoverable mode passed in by the CLI is sl_bt_advertiser_user_data (4),
    // it is invalid for generating the advertising data, thus is ignored. As the
    // result, the advertiser will advertise with empty data payload if
    // sl_bt_legacy_advertiser_generate_data has not been called.
    if (discoverableMode <= sl_bt_advertiser_general_discoverable) {
      if (discoverableMode == sl_bt_advertiser_broadcast) {
        // The value sl_bt_advertiser_broadcast (3) is identical to
        // sl_bt_advertiser_non_discoverable in the context of generating
        // advertising data. And sl_bt_advertiser_broadcast is invalid in
        // sl_bt_legacy_advertiser_generate_data
        discoverableMode = sl_bt_advertiser_non_discoverable;
      }
      status = sl_bt_legacy_advertiser_generate_data(handle, discoverableMode);
      if (status == SL_STATUS_OK) {
          otCliOutputFormat("Generate advertising data success\r\n");
      } else {
          otCliOutputFormat("Generate advertising data error: 0x%04x\r\n", status);
      }
    }
    /* End of advertising discoverable mode handling */

    status = sl_bt_legacy_advertiser_start(handle, connectableMode);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

static void stopAdvertisingCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long handle;

    VerifyOrExit(argc == 1, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[0], &handle));

    status = sl_bt_advertiser_stop(handle);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

static void startDiscoveryCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long mode;

    VerifyOrExit(argc == 1, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[0], &mode));

    status = sl_bt_scanner_start(1, mode); // 1=1M phy

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

static void setConnTimingCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long min_interval, max_interval, latency, timeout;

    VerifyOrExit(argc == 4, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[0], &min_interval));
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &max_interval));
    SuccessOrExit(error = ParseUnsignedLong(argv[2], &latency));
    SuccessOrExit(error = ParseUnsignedLong(argv[3], &timeout));

    status = sl_bt_connection_set_default_parameters(min_interval, max_interval, latency, timeout, 0, 0xffff);
    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

static void connOpenCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    bd_addr address;
    uint8_t len = sizeof(address.addr);
    unsigned long addressType;
    uint8_t handle;

    VerifyOrExit(argc == 2, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(Hex2Bin(argv[0], address.addr, len, false) >= 0, error = OT_ERROR_PARSE);
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &addressType));

    // Hex2Bin makes it little endian but it needs to be big endian
    for (size_t i = 0; i < len / 2; i++) {
      uint8_t temp = address.addr[len - i - 1];
      address.addr[len - i - 1] = address.addr[i];
      address.addr[i] = temp;
    }
    // TO DO: Check how is the handle returned?
    status = sl_bt_connection_open(address, addressType, 1, &handle); // 1=1M phy

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

static void connCloseCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long handle;

    VerifyOrExit(argc == 1, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[0], &handle));

    status = sl_bt_connection_close(handle);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%04x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliSetUserCommandError(error);
    }
}

static void gattDiscoverPrimaryServicesCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
}

static void gattDiscoverCharacteristicsCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
}

static void gattWriteCharacteristicCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
}

static void setTxPowerCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
}

static int Hex2Bin(const char *aHex, uint8_t *aBin, uint16_t aBinLength, bool aAllowTruncate)
{
    size_t      hexLength = strlen(aHex);
    const char *hexEnd    = aHex + hexLength;
    uint8_t *   cur       = aBin;
    uint8_t     numChars  = hexLength & 1;
    uint8_t     byte      = 0;
    int         len       = 0;
    int         rval;

    if (!aAllowTruncate)
    {
        VerifyOrExit((hexLength + 1) / 2 <= aBinLength, rval = -1);
    }

    while (aHex < hexEnd)
    {
        if ('A' <= *aHex && *aHex <= 'F')
        {
            byte |= 10 + (*aHex - 'A');
        }
        else if ('a' <= *aHex && *aHex <= 'f')
        {
            byte |= 10 + (*aHex - 'a');
        }
        else if ('0' <= *aHex && *aHex <= '9')
        {
            byte |= *aHex - '0';
        }
        else
        {
            ExitNow(rval = -1);
        }

        aHex++;
        numChars++;

        if (numChars >= 2)
        {
            numChars = 0;
            *cur++   = byte;
            byte     = 0;
            len++;

            if (len == aBinLength)
            {
                ExitNow(rval = len);
            }
        }
        else
        {
            byte <<= 4;
        }
    }

    rval = len;

exit:
    return rval;
}

void printBleAddress(bd_addr address)
{
    for (size_t i = 0; i < 6; i++) {
        otCliOutputFormat("%02x", address.addr[5-i]);
    }
}

//-----------------------------------------------------------------------------

static otCliCommand bleCommands[] = {
    {"help", &helpCommand},
    {"get_address", &getAddressCommand},
    {"create_adv_set", &createAdvSetCommand},
    {"set_adv_timing", &setAdvTimingCommand},
    {"set_adv_random_address", &setAdvRandomAddressCommand},
    {"start_adv", &startAdvertisingCommand},
    {"stop_adv", &stopAdvertisingCommand},
    {"start_discovery", &startDiscoveryCommand},
    {"set_conn_timing", &setConnTimingCommand},
    {"conn_open", &connOpenCommand},
    {"conn_close", &connCloseCommand},
    {"gatt_discover_primary_services", &gattDiscoverPrimaryServicesCommand},
    {"gatt_discover_characteristics", &gattDiscoverCharacteristicsCommand},
    {"gatt_write_characteristic", &gattWriteCharacteristicCommand},
    {"set_tx_power", &setTxPowerCommand},
};

void bleCommand(void *context, uint8_t argc, char *argv[])
{
    otError error = otCRPCHandleCommand(context, argc, argv, OT_ARRAY_LENGTH(bleCommands), bleCommands);

    if (error == OT_ERROR_INVALID_COMMAND)
    {
        helpCommand(NULL, 0, NULL);
    }
}

static void helpCommand(void *context, uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(context);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    for (size_t i = 0; i < OT_ARRAY_LENGTH(bleCommands); i++) {
      otCliOutputFormat("%s\r\n", bleCommands[i].mName);
    }
}
#endif // SL_OPENTHREAD_BLE_CLI_ENABLE
