/***************************************************************************//**
 * @file
 * @brief ZigBee 3.0 core test harness functionality
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

/**
 * @defgroup test-harness-z3 Test Harness Z3
 * @ingroup component
 * @brief API and Callbacks for the Test Harness Z3 Component
 *
 * This component contains the functionality necessary for creating a
 * ZigBee 3.0 test harness application.
 *
 */

/**
 * @addtogroup test-harness-z3
 * @{
 */

// -----------------------------------------------------------------------------
// Constants

#define TEST_HARNESS_Z3_PRINT_NAME "TestHarnessZ3"

// -----------------------------------------------------------------------------
// Globals

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmAfPluginTestHarnessZ3DeviceModeEnum
#else
enum
#endif
{
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_ZR_NOT_ADDRESS_ASSIGNABLE     = 0x00,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_ZR_ADDRESS_ASSIGNABLE         = 0x01,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_ZED_NOT_ADDRESS_ASSIGNABLE    = 0x02,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_ZED_ADDRESS_ASSIGNABLE        = 0x03,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_SLEEPY_ZED_NOT_ADDRESS_ASSIGNABLE = 0x04,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_SLEEPY_ZED_ADDRESS_ASSIGNABLE = 0x05,
};
typedef uint8_t EmAfPluginTestHarnessZ3DeviceMode;
extern EmAfPluginTestHarnessZ3DeviceMode emAfPluginTestHarnessZ3DeviceMode;
extern uint16_t emAfPluginTestHarnessZ3TouchlinkProfileId;

#define EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_MAX \
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_SLEEPY_ZED_ADDRESS_ASSIGNABLE
#define COMMAND_DATA_SIZE 128
#define EXTRA_COMMAND_BYTES 5

/** @} */ // end of comp-name

// -----------------------------------------------------------------------------
// Utility API

uint32_t emAfPluginTestHarnessZ3GetSignificantBit(uint8_t commandIndex);
