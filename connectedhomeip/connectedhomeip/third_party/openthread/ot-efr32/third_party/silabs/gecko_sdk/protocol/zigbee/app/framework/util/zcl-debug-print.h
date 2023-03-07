/***************************************************************************//**
 * @file
 * @brief Defines debug print macros for application framework and ZCL clusters
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
#if defined(UC_BUILD)

#include "sl_component_catalog.h"
#include "app/framework/util/print.h"
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "sl_zigbee_debug_print_config.h"
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
// Generic print area.
#define EMBER_AF_PRINT_GENERIC 0x0000

// EMBER_AF_PRINT_ constants are not used in print macros (e.g. emberAfCorePrint) with UC. However there's
// still some code outside of these macros that explicitly references these contants. TODO: This should be
// cleaned up at some point.
#define EMBER_AF_PRINT_CORE 0x0001

// Debug print macros for the application framework and ZCL clusters.
// We are using area 0x00 in the emberAfPrint* calls below because with UC we don't (yet) have
// the per-cluster print granularity, emberAfPrintEnabled() always returns TRUE.

// We have a separate config switch (ZIGBEE_DEBUG_PRINTS_AF_DEBUG) for AF Debug because we want it off by default
#if (defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT) && (SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 1) && (SL_ZIGBEE_DEBUG_PRINTS_ZCL_LEGACY_AF_DEBUG_ENABLED == 1))
// Prints messages for random debugging
#define emberAfDebugPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDebugPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDebugFlush()
#define emberAfDebugDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDebugPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDebugPrintString(buffer) emberAfPrintString(0x00, (buffer))

#else

#define emberAfDebugPrint(...)
#define emberAfDebugPrintln(...)
#define emberAfDebugFlush()
#define emberAfDebugDebugExec(x)
#define emberAfDebugPrintBuffer(buffer, len, withSpace)
#define emberAfDebugPrintString(buffer)

#endif // (defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT) && (SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 1) && (SL_ZIGBEE_DEBUG_PRINTS_ZCL_LEGACY_AF_DEBUG_ENABLED == 1))

#if (defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT) && (SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 1))
// Printing macros for Application
// Prints messages for application part
#define emberAfAppPrint(...)    emberAfPrint(0x00, __VA_ARGS__)
#define emberAfAppPrintln(...)  emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfAppFlush()
#define emberAfAppDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfAppPrintBuffer(buffer, len, withSpace)   emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfAppPrintString(buffer) emberAfPrintString(0x00, (buffer))

// Printing macros for Core
// Prints messages for global flow of the receive/send
#define emberAfCorePrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfCorePrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
// Blocking IO is enabled for all serial ports, therefore flush calls are unnecessary.
#define emberAfCoreFlush()
#define emberAfCoreDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfCorePrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfCorePrintString(buffer) emberAfPrintString(0x00, (buffer))

// Printing macros for Reporting
// Prints messages related to reporting
#define emberAfReportingPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfReportingPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
// Blocking IO is enabled for all serial ports, therefore flush calls are unnecessary.
#define emberAfReportingFlush()
#define emberAfReportingDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfReportingPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfReportingPrintString(buffer) emberAfPrintString(0x00, (buffer))

// Printing macros for Service discovery
// Prints messages related to service discovery
#define emberAfServiceDiscoveryPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfServiceDiscoveryPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
// Blocking IO is enabled for all serial ports, therefore flush calls are unnecessary.
#define emberAfServiceDiscoveryFlush()
#define emberAfServiceDiscoveryDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfServiceDiscoveryPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfServiceDiscoveryPrintString(buffer) emberAfPrintString(0x00, (buffer))

// Printing macros for Attributes
// Prints messages related to attributes
#define emberAfAttributesPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfAttributesPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
// Blocking IO is enabled for all serial ports, therefore flush calls are unnecessary.
#define emberAfAttributesFlush()
#define emberAfAttributesDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfAttributesPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfAttributesPrintString(buffer) emberAfPrintString(0x00, (buffer))

// Printing macros for Security
// Prints messages related to security
#define emberAfSecurityPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfSecurityPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
// Blocking IO is enabled for all serial ports, therefore flush calls are unnecessary.
#define emberAfSecurityFlush()
#define emberAfSecurityDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfSecurityPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfSecurityPrintString(buffer) emberAfPrintString(0x00, (buffer))

// Printing macros for ZDO (ZigBee Device Object)
// Prints messages related to ZDO functionality
#define emberAfZdoPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfZdoPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
// Blocking IO is enabled for all serial ports, therefore flush calls are unnecessary.
#define emberAfZdoFlush()
#define emberAfZdoDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfZdoPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfZdoPrintString(buffer) emberAfPrintString(0x00, (buffer))

// Printing macros for Registration
#define emberAfRegistrationPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfRegistrationPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
// Blocking IO is enabled for all serial ports, therefore flush calls are unnecessary.
#define emberAfRegistrationFlush()
#define emberAfRegistrationDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfRegistrationPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfRegistrationPrintString(buffer) emberAfPrintString(0x00, (buffer))

// Printing macros for clusters

#define emberAfBasicClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfBasicClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfBasicClusterFlush()
#define emberAfBasicClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfBasicClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfBasicClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPowerConfigClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPowerConfigClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPowerConfigClusterFlush()
#define emberAfPowerConfigClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPowerConfigClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPowerConfigClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfDeviceTempClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDeviceTempClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDeviceTempClusterFlush()
#define emberAfDeviceTempClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDeviceTempClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDeviceTempClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfIdentifyClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfIdentifyClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfIdentifyClusterFlush()
#define emberAfIdentifyClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfIdentifyClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfIdentifyClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfGroupsClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfGroupsClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfGroupsClusterFlush()
#define emberAfGroupsClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfGroupsClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfGroupsClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfScenesClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfScenesClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfScenesClusterFlush()
#define emberAfScenesClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfScenesClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfScenesClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfOnOffClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfOnOffClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfOnOffClusterFlush()
#define emberAfOnOffClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfOnOffClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfOnOffClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfOnOffSwitchConfigClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfOnOffSwitchConfigClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfOnOffSwitchConfigClusterFlush()
#define emberAfOnOffSwitchConfigClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfOnOffSwitchConfigClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfOnOffSwitchConfigClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfLevelControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfLevelControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfLevelControlClusterFlush()
#define emberAfLevelControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfLevelControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfLevelControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfAlarmClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfAlarmClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfAlarmClusterFlush()
#define emberAfAlarmClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfAlarmClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfAlarmClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfTimeClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfTimeClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfTimeClusterFlush()
#define emberAfTimeClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfTimeClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfTimeClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfRssiLocationClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfRssiLocationClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfRssiLocationClusterFlush()
#define emberAfRssiLocationClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfRssiLocationClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfRssiLocationClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfBinaryInputBasicClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfBinaryInputBasicClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfBinaryInputBasicClusterFlush()
#define emberAfBinaryInputBasicClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfBinaryInputBasicClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfBinaryInputBasicClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfCommissioningClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfCommissioningClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfCommissioningClusterFlush()
#define emberAfCommissioningClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfCommissioningClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfCommissioningClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPartitionClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPartitionClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPartitionClusterFlush()
#define emberAfPartitionClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPartitionClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPartitionClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfOtaBootloadClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfOtaBootloadClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfOtaBootloadClusterFlush()
#define emberAfOtaBootloadClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfOtaBootloadClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfOtaBootloadClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPowerProfileClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPowerProfileClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPowerProfileClusterFlush()
#define emberAfPowerProfileClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPowerProfileClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPowerProfileClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfApplianceControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfApplianceControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfApplianceControlClusterFlush()
#define emberAfApplianceControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfApplianceControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfApplianceControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPollControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPollControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPollControlClusterFlush()
#define emberAfPollControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPollControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPollControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPluginGasProxyFunctionPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPluginGasProxyFunctionPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPluginGasProxyFunctionDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPluginGasProxyFunctionPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPluginGasProxyFunctionPrintString(buffer)  emberAfPrintString(0x00, (buffer))

#define emberAfGreenPowerClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfGreenPowerClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfGreenPowerClusterFlush()
#define emberAfGreenPowerClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfGreenPowerClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfGreenPowerClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfKeepaliveClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfKeepaliveClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfKeepaliveClusterFlush()
#define emberAfKeepaliveClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfKeepaliveClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfKeepaliveClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfShadeConfigClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfShadeConfigClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfShadeConfigClusterFlush()
#define emberAfShadeConfigClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfShadeConfigClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfShadeConfigClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfDoorLockClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDoorLockClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDoorLockClusterFlush()
#define emberAfDoorLockClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDoorLockClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDoorLockClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfWindowCoveringClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfWindowCoveringClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfWindowCoveringClusterFlush()
#define emberAfWindowCoveringClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfWindowCoveringClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfWindowCoveringClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfBarrierControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfBarrierControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfBarrierControlClusterFlush()
#define emberAfBarrierControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfBarrierControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfBarrierControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPumpConfigControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPumpConfigControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPumpConfigControlClusterFlush()
#define emberAfPumpConfigControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPumpConfigControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPumpConfigControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfThermostatClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfThermostatClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfThermostatClusterFlush()
#define emberAfThermostatClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfThermostatClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfThermostatClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfFanControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfFanControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfFanControlClusterFlush()
#define emberAfFanControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfFanControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfFanControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfDehumidControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDehumidControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDehumidControlClusterFlush()
#define emberAfDehumidControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDehumidControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDehumidControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfThermostatUiConfigClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfThermostatUiConfigClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfThermostatUiConfigClusterFlush()
#define emberAfThermostatUiConfigClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfThermostatUiConfigClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfThermostatUiConfigClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfColorControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfColorControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfColorControlClusterFlush()
#define emberAfColorControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfColorControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfColorControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfBallastConfigurationClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfBallastConfigurationClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfBallastConfigurationClusterFlush()
#define emberAfBallastConfigurationClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfBallastConfigurationClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfBallastConfigurationClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfIllumMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfIllumMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfIllumMeasurementClusterFlush()
#define emberAfIllumMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfIllumMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfIllumMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfIllumLevelSensingClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfIllumLevelSensingClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfIllumLevelSensingClusterFlush()
#define emberAfIllumLevelSensingClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfIllumLevelSensingClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfIllumLevelSensingClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfTempMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfTempMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfTempMeasurementClusterFlush()
#define emberAfTempMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfTempMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfTempMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPressureMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPressureMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPressureMeasurementClusterFlush()
#define emberAfPressureMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPressureMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPressureMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfFlowMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfFlowMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfFlowMeasurementClusterFlush()
#define emberAfFlowMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfFlowMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfFlowMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfRelativeHumidityMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfRelativeHumidityMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfRelativeHumidityMeasurementClusterFlush()
#define emberAfRelativeHumidityMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfRelativeHumidityMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfRelativeHumidityMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfOccupancySensingClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfOccupancySensingClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfOccupancySensingClusterFlush()
#define emberAfOccupancySensingClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfOccupancySensingClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfOccupancySensingClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfCarbonMonoxideConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfCarbonMonoxideConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfCarbonMonoxideConcentrationMeasurementClusterFlush()
#define emberAfCarbonMonoxideConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfCarbonMonoxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfCarbonMonoxideConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfCarbonDioxideConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfCarbonDioxideConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfCarbonDioxideConcentrationMeasurementClusterFlush()
#define emberAfCarbonDioxideConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfCarbonDioxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfCarbonDioxideConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfEthyleneConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfEthyleneConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfEthyleneConcentrationMeasurementClusterFlush()
#define emberAfEthyleneConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfEthyleneConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfEthyleneConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfEthyleneOxideConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfEthyleneOxideConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfEthyleneOxideConcentrationMeasurementClusterFlush()
#define emberAfEthyleneOxideConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfEthyleneOxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfEthyleneOxideConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfHydrogenConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfHydrogenConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfHydrogenConcentrationMeasurementClusterFlush()
#define emberAfHydrogenConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfHydrogenConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfHydrogenConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfHydrogenSulphideConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfHydrogenSulphideConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfHydrogenSulphideConcentrationMeasurementClusterFlush()
#define emberAfHydrogenSulphideConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfHydrogenSulphideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfHydrogenSulphideConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfNitricOxideConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfNitricOxideConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfNitricOxideConcentrationMeasurementClusterFlush()
#define emberAfNitricOxideConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfNitricOxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfNitricOxideConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfNitrogenDioxideConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfNitrogenDioxideConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfNitrogenDioxideConcentrationMeasurementClusterFlush()
#define emberAfNitrogenDioxideConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfNitrogenDioxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfNitrogenDioxideConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfOxygenConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfOxygenConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfOxygenConcentrationMeasurementClusterFlush()
#define emberAfOxygenConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfOxygenConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfOxygenConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfOzoneConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfOzoneConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfOzoneConcentrationMeasurementClusterFlush()
#define emberAfOzoneConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfOzoneConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfOzoneConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfSulfurDioxideConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfSulfurDioxideConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfSulfurDioxideConcentrationMeasurementClusterFlush()
#define emberAfSulfurDioxideConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfSulfurDioxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfSulfurDioxideConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfDissolvedOxygenConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDissolvedOxygenConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDissolvedOxygenConcentrationMeasurementClusterFlush()
#define emberAfDissolvedOxygenConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDissolvedOxygenConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDissolvedOxygenConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfBromateConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfBromateConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfBromateConcentrationMeasurementClusterFlush()
#define emberAfBromateConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfBromateConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfBromateConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfChloraminesConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfChloraminesConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfChloraminesConcentrationMeasurementClusterFlush()
#define emberAfChloraminesConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfChloraminesConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfChloraminesConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfChlorineConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfChlorineConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfChlorineConcentrationMeasurementClusterFlush()
#define emberAfChlorineConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfChlorineConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfChlorineConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterFlush()
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfFluorideConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfFluorideConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfFluorideConcentrationMeasurementClusterFlush()
#define emberAfFluorideConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfFluorideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfFluorideConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfHaloaceticAcidsConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterFlush()
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterFlush()
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterFlush()
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfTurbidityConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfTurbidityConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfTurbidityConcentrationMeasurementClusterFlush()
#define emberAfTurbidityConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfTurbidityConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfTurbidityConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfCopperConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfCopperConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfCopperConcentrationMeasurementClusterFlush()
#define emberAfCopperConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfCopperConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfCopperConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfLeadConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfLeadConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfLeadConcentrationMeasurementClusterFlush()
#define emberAfLeadConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfLeadConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfLeadConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfManganeseConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfManganeseConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfManganeseConcentrationMeasurementClusterFlush()
#define emberAfManganeseConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfManganeseConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfManganeseConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfSulfateConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfSulfateConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfSulfateConcentrationMeasurementClusterFlush()
#define emberAfSulfateConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfSulfateConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfSulfateConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfBromodichloromethaneConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfBromodichloromethaneConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfBromodichloromethaneConcentrationMeasurementClusterFlush()
#define emberAfBromodichloromethaneConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfBromodichloromethaneConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfBromodichloromethaneConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfBromoformConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfBromoformConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfBromoformConcentrationMeasurementClusterFlush()
#define emberAfBromoformConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfBromoformConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfBromoformConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfChlorodibromomethaneConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterFlush()
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfChloroformConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfChloroformConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfChloroformConcentrationMeasurementClusterFlush()
#define emberAfChloroformConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfChloroformConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfChloroformConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfSodiumConcentrationMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfSodiumConcentrationMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfSodiumConcentrationMeasurementClusterFlush()
#define emberAfSodiumConcentrationMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfSodiumConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfSodiumConcentrationMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfIasZoneClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfIasZoneClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfIasZoneClusterFlush()
#define emberAfIasZoneClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfIasZoneClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfIasZoneClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfIasAceClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfIasAceClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfIasAceClusterFlush()
#define emberAfIasAceClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfIasAceClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfIasAceClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfIasWdClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfIasWdClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfIasWdClusterFlush()
#define emberAfIasWdClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfIasWdClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfIasWdClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfGenericTunnelClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfGenericTunnelClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfGenericTunnelClusterFlush()
#define emberAfGenericTunnelClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfGenericTunnelClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfGenericTunnelClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfBacnetProtocolTunnelClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfBacnetProtocolTunnelClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfBacnetProtocolTunnelClusterFlush()
#define emberAfBacnetProtocolTunnelClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfBacnetProtocolTunnelClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfBacnetProtocolTunnelClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAf11073ProtocolTunnelClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAf11073ProtocolTunnelClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAf11073ProtocolTunnelClusterFlush()
#define emberAf11073ProtocolTunnelClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAf11073ProtocolTunnelClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAf11073ProtocolTunnelClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfIso7816ProtocolTunnelClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfIso7816ProtocolTunnelClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfIso7816ProtocolTunnelClusterFlush()
#define emberAfIso7816ProtocolTunnelClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfIso7816ProtocolTunnelClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfIso7816ProtocolTunnelClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPriceClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPriceClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPriceClusterFlush()
#define emberAfPriceClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPriceClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPriceClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfDemandResponseLoadControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDemandResponseLoadControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDemandResponseLoadControlClusterFlush()
#define emberAfDemandResponseLoadControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDemandResponseLoadControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDemandResponseLoadControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfSimpleMeteringClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfSimpleMeteringClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfSimpleMeteringClusterFlush()
#define emberAfSimpleMeteringClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfSimpleMeteringClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfSimpleMeteringClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfMessagingClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfMessagingClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfMessagingClusterFlush()
#define emberAfMessagingClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfMessagingClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfMessagingClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfTunnelingClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfTunnelingClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfTunnelingClusterFlush()
#define emberAfTunnelingClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfTunnelingClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfTunnelingClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPrepaymentClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPrepaymentClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPrepaymentClusterFlush()
#define emberAfPrepaymentClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPrepaymentClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPrepaymentClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfEnergyManagementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfEnergyManagementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfEnergyManagementClusterFlush()
#define emberAfEnergyManagementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfEnergyManagementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfEnergyManagementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfCalendarClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfCalendarClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfCalendarClusterFlush()
#define emberAfCalendarClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfCalendarClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfCalendarClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfDeviceManagementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDeviceManagementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDeviceManagementClusterFlush()
#define emberAfDeviceManagementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDeviceManagementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDeviceManagementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfEventsClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfEventsClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfEventsClusterFlush()
#define emberAfEventsClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfEventsClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfEventsClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfMduPairingClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfMduPairingClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfMduPairingClusterFlush()
#define emberAfMduPairingClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfMduPairingClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfMduPairingClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfSubGhzClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfSubGhzClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfSubGhzClusterFlush()
#define emberAfSubGhzClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfSubGhzClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfSubGhzClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfKeyEstablishmentClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfKeyEstablishmentClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfKeyEstablishmentClusterFlush()
#define emberAfKeyEstablishmentClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfKeyEstablishmentClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfKeyEstablishmentClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfInformationClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfInformationClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfInformationClusterFlush()
#define emberAfInformationClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfInformationClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfInformationClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfDataSharingClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDataSharingClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDataSharingClusterFlush()
#define emberAfDataSharingClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDataSharingClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDataSharingClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfGamingClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfGamingClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfGamingClusterFlush()
#define emberAfGamingClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfGamingClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfGamingClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfDataRateControlClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDataRateControlClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDataRateControlClusterFlush()
#define emberAfDataRateControlClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDataRateControlClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDataRateControlClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfVoiceOverZigbeeClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfVoiceOverZigbeeClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfVoiceOverZigbeeClusterFlush()
#define emberAfVoiceOverZigbeeClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfVoiceOverZigbeeClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfVoiceOverZigbeeClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfChattingClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfChattingClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfChattingClusterFlush()
#define emberAfChattingClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfChattingClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfChattingClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfPaymentClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfPaymentClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfPaymentClusterFlush()
#define emberAfPaymentClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfPaymentClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfPaymentClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfBillingClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfBillingClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfBillingClusterFlush()
#define emberAfBillingClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfBillingClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfBillingClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfApplianceIdentificationClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfApplianceIdentificationClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfApplianceIdentificationClusterFlush()
#define emberAfApplianceIdentificationClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfApplianceIdentificationClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfApplianceIdentificationClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfMeterIdentificationClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfMeterIdentificationClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfMeterIdentificationClusterFlush()
#define emberAfMeterIdentificationClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfMeterIdentificationClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfMeterIdentificationClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfApplianceEventsAndAlertClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfApplianceEventsAndAlertClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfApplianceEventsAndAlertClusterFlush()
#define emberAfApplianceEventsAndAlertClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfApplianceEventsAndAlertClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfApplianceEventsAndAlertClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfApplianceStatisticsClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfApplianceStatisticsClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfApplianceStatisticsClusterFlush()
#define emberAfApplianceStatisticsClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfApplianceStatisticsClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfApplianceStatisticsClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfElectricalMeasurementClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfElectricalMeasurementClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfElectricalMeasurementClusterFlush()
#define emberAfElectricalMeasurementClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfElectricalMeasurementClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfElectricalMeasurementClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfDiagnosticsClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfDiagnosticsClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfDiagnosticsClusterFlush()
#define emberAfDiagnosticsClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfDiagnosticsClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfDiagnosticsClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfZllCommissioningClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfZllCommissioningClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfZllCommissioningClusterFlush()
#define emberAfZllCommissioningClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfZllCommissioningClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfZllCommissioningClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfSampleMfgSpecificClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfSampleMfgSpecificClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfSampleMfgSpecificClusterFlush()
#define emberAfSampleMfgSpecificClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfSampleMfgSpecificClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfSampleMfgSpecificClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfOtaConfigurationClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfOtaConfigurationClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfOtaConfigurationClusterFlush()
#define emberAfOtaConfigurationClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfOtaConfigurationClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfOtaConfigurationClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfMfglibClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfMfglibClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfMfglibClusterFlush()
#define emberAfMfglibClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfMfglibClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfMfglibClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#define emberAfSlWwahClusterPrint(...) emberAfPrint(0x00, __VA_ARGS__)
#define emberAfSlWwahClusterPrintln(...) emberAfPrintln(0x00, __VA_ARGS__)
#define emberAfSlWwahClusterFlush()
#define emberAfSlWwahClusterDebugExec(x) if ( emberAfPrintEnabled(0x00) ) { x; }
#define emberAfSlWwahClusterPrintBuffer(buffer, len, withSpace) emberAfPrintBuffer(0x00, (buffer), (len), (withSpace))
#define emberAfSlWwahClusterPrintString(buffer) emberAfPrintString(0x00, (buffer))

#else // (SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 1)

#define emberAfAppPrint(...)
#define emberAfAppPrintln(...)
#define emberAfAppFlush()
#define emberAfAppDebugExec(x)
#define emberAfAppPrintBuffer(buffer, len, withSpace)
#define emberAfAppPrintString(buffer)

#define emberAfCorePrint(...)
#define emberAfCorePrintln(...)
#define emberAfCoreFlush()
#define emberAfCoreDebugExec(x)
#define emberAfCorePrintBuffer(buffer, len, withSpace)
#define emberAfCorePrintString(buffer)

#define emberAfReportingPrint(...)
#define emberAfReportingPrintln(...)
#define emberAfReportingFlush()
#define emberAfReportingDebugExec(x)
#define emberAfReportingPrintBuffer(buffer, len, withSpace)
#define emberAfReportingPrintString(buffer)

#define emberAfServiceDiscoveryPrint(...)
#define emberAfServiceDiscoveryPrintln(...)
#define emberAfServiceDiscoveryFlush()
#define emberAfServiceDiscoveryDebugExec(x)
#define emberAfServiceDiscoveryPrintBuffer(buffer, len, withSpace)
#define emberAfServiceDiscoveryPrintString(buffer)

#define emberAfAttributesPrint(...)
#define emberAfAttributesPrintln(...)
#define emberAfAttributesFlush()
#define emberAfAttributesDebugExec(x)
#define emberAfAttributesPrintBuffer(buffer, len, withSpace)
#define emberAfAttributesPrintString(buffer)

#define emberAfSecurityPrint(...)
#define emberAfSecurityPrintln(...)
#define emberAfSecurityFlush()
#define emberAfSecurityDebugExec(x)
#define emberAfSecurityPrintBuffer(buffer, len, withSpace)
#define emberAfSecurityPrintString(buffer)

#define emberAfZdoPrint(...)
#define emberAfZdoPrintln(...)
#define emberAfZdoFlush()
#define emberAfZdoDebugExec(x)
#define emberAfZdoPrintBuffer(buffer, len, withSpace)
#define emberAfZdoPrintString(buffer)

// Printing macros for Registration
#define emberAfRegistrationPrint(...)
#define emberAfRegistrationPrintln(...)
#define emberAfRegistrationFlush()
#define emberAfRegistrationDebugExec(x)
#define emberAfRegistrationPrintBuffer(buffer, len, withSpace)
#define emberAfRegistrationPrintString(buffer)

// Printing macros for clusters

#define emberAfBasicClusterPrint(...)
#define emberAfBasicClusterPrintln(...)
#define emberAfBasicClusterFlush()
#define emberAfBasicClusterDebugExec(x)
#define emberAfBasicClusterPrintBuffer(buffer, len, withSpace)
#define emberAfBasicClusterPrintString(buffer)

#define emberAfPowerConfigClusterPrint(...)
#define emberAfPowerConfigClusterPrintln(...)
#define emberAfPowerConfigClusterFlush()
#define emberAfPowerConfigClusterDebugExec(x)
#define emberAfPowerConfigClusterPrintBuffer(buffer, len, withSpace)
#define emberAfPowerConfigClusterPrintString(buffer)

#define emberAfDeviceTempClusterPrint(...)
#define emberAfDeviceTempClusterPrintln(...)
#define emberAfDeviceTempClusterFlush()
#define emberAfDeviceTempClusterDebugExec(x)
#define emberAfDeviceTempClusterPrintBuffer(buffer, len, withSpace)
#define emberAfDeviceTempClusterPrintString(buffer)

#define emberAfIdentifyClusterPrint(...)
#define emberAfIdentifyClusterPrintln(...)
#define emberAfIdentifyClusterFlush()
#define emberAfIdentifyClusterDebugExec(x)
#define emberAfIdentifyClusterPrintBuffer(buffer, len, withSpace)
#define emberAfIdentifyClusterPrintString(buffer)

#define emberAfGroupsClusterPrint(...)
#define emberAfGroupsClusterPrintln(...)
#define emberAfGroupsClusterFlush()
#define emberAfGroupsClusterDebugExec(x)
#define emberAfGroupsClusterPrintBuffer(buffer, len, withSpace)
#define emberAfGroupsClusterPrintString(buffer)

#define emberAfScenesClusterPrint(...)
#define emberAfScenesClusterPrintln(...)
#define emberAfScenesClusterFlush()
#define emberAfScenesClusterDebugExec(x)
#define emberAfScenesClusterPrintBuffer(buffer, len, withSpace)
#define emberAfScenesClusterPrintString(buffer)

#define emberAfOnOffClusterPrint(...)
#define emberAfOnOffClusterPrintln(...)
#define emberAfOnOffClusterFlush()
#define emberAfOnOffClusterDebugExec(x)
#define emberAfOnOffClusterPrintBuffer(buffer, len, withSpace)
#define emberAfOnOffClusterPrintString(buffer)

#define emberAfOnOffSwitchConfigClusterPrint(...)
#define emberAfOnOffSwitchConfigClusterPrintln(...)
#define emberAfOnOffSwitchConfigClusterFlush()
#define emberAfOnOffSwitchConfigClusterDebugExec(x)
#define emberAfOnOffSwitchConfigClusterPrintBuffer(buffer, len, withSpace)
#define emberAfOnOffSwitchConfigClusterPrintString(buffer)

#define emberAfLevelControlClusterPrint(...)
#define emberAfLevelControlClusterPrintln(...)
#define emberAfLevelControlClusterFlush()
#define emberAfLevelControlClusterDebugExec(x)
#define emberAfLevelControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfLevelControlClusterPrintString(buffer)

#define emberAfAlarmClusterPrint(...)
#define emberAfAlarmClusterPrintln(...)
#define emberAfAlarmClusterFlush()
#define emberAfAlarmClusterDebugExec(x)
#define emberAfAlarmClusterPrintBuffer(buffer, len, withSpace)
#define emberAfAlarmClusterPrintString(buffer)

#define emberAfTimeClusterPrint(...)
#define emberAfTimeClusterPrintln(...)
#define emberAfTimeClusterFlush()
#define emberAfTimeClusterDebugExec(x)
#define emberAfTimeClusterPrintBuffer(buffer, len, withSpace)
#define emberAfTimeClusterPrintString(buffer)

#define emberAfRssiLocationClusterPrint(...)
#define emberAfRssiLocationClusterPrintln(...)
#define emberAfRssiLocationClusterFlush()
#define emberAfRssiLocationClusterDebugExec(x)
#define emberAfRssiLocationClusterPrintBuffer(buffer, len, withSpace)
#define emberAfRssiLocationClusterPrintString(buffer)

#define emberAfBinaryInputBasicClusterPrint(...)
#define emberAfBinaryInputBasicClusterPrintln(...)
#define emberAfBinaryInputBasicClusterFlush()
#define emberAfBinaryInputBasicClusterDebugExec(x)
#define emberAfBinaryInputBasicClusterPrintBuffer(buffer, len, withSpace)
#define emberAfBinaryInputBasicClusterPrintString(buffer)

#define emberAfCommissioningClusterPrint(...)
#define emberAfCommissioningClusterPrintln(...)
#define emberAfCommissioningClusterFlush()
#define emberAfCommissioningClusterDebugExec(x)
#define emberAfCommissioningClusterPrintBuffer(buffer, len, withSpace)
#define emberAfCommissioningClusterPrintString(buffer)

#define emberAfPartitionClusterPrint(...)
#define emberAfPartitionClusterPrintln(...)
#define emberAfPartitionClusterFlush()
#define emberAfPartitionClusterDebugExec(x)
#define emberAfPartitionClusterPrintBuffer(buffer, len, withSpace)
#define emberAfPartitionClusterPrintString(buffer)

#define emberAfOtaBootloadClusterPrint(...)
#define emberAfOtaBootloadClusterPrintln(...)
#define emberAfOtaBootloadClusterFlush()
#define emberAfOtaBootloadClusterDebugExec(x)
#define emberAfOtaBootloadClusterPrintBuffer(buffer, len, withSpace)
#define emberAfOtaBootloadClusterPrintString(buffer)

#define emberAfPowerProfileClusterPrint(...)
#define emberAfPowerProfileClusterPrintln(...)
#define emberAfPowerProfileClusterFlush()
#define emberAfPowerProfileClusterDebugExec(x)
#define emberAfPowerProfileClusterPrintBuffer(buffer, len, withSpace)
#define emberAfPowerProfileClusterPrintString(buffer)

#define emberAfApplianceControlClusterPrint(...)
#define emberAfApplianceControlClusterPrintln(...)
#define emberAfApplianceControlClusterFlush()
#define emberAfApplianceControlClusterDebugExec(x)
#define emberAfApplianceControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfApplianceControlClusterPrintString(buffer)

#define emberAfPollControlClusterPrint(...)
#define emberAfPollControlClusterPrintln(...)
#define emberAfPollControlClusterFlush()
#define emberAfPollControlClusterDebugExec(x)
#define emberAfPollControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfPollControlClusterPrintString(buffer)

#define emberAfPluginGasProxyFunctionPrint(...)
#define emberAfPluginGasProxyFunctionPrintln(...)
#define emberAfPluginGasProxyFunctionDebugExec(x)
#define emberAfPluginGasProxyFunctionPrintBuffer(buffer, len, withSpace)
#define emberAfPluginGasProxyFunctionPrintString(buffer)

#define emberAfGreenPowerClusterPrint(...)
#define emberAfGreenPowerClusterPrintln(...)
#define emberAfGreenPowerClusterFlush()
#define emberAfGreenPowerClusterDebugExec(x)
#define emberAfGreenPowerClusterPrintBuffer(buffer, len, withSpace)
#define emberAfGreenPowerClusterPrintString(buffer)

#define emberAfKeepaliveClusterPrint(...)
#define emberAfKeepaliveClusterPrintln(...)
#define emberAfKeepaliveClusterFlush()
#define emberAfKeepaliveClusterDebugExec(x)
#define emberAfKeepaliveClusterPrintBuffer(buffer, len, withSpace)
#define emberAfKeepaliveClusterPrintString(buffer)

#define emberAfShadeConfigClusterPrint(...)
#define emberAfShadeConfigClusterPrintln(...)
#define emberAfShadeConfigClusterFlush()
#define emberAfShadeConfigClusterDebugExec(x)
#define emberAfShadeConfigClusterPrintBuffer(buffer, len, withSpace)
#define emberAfShadeConfigClusterPrintString(buffer)

#define emberAfDoorLockClusterPrint(...)
#define emberAfDoorLockClusterPrintln(...)
#define emberAfDoorLockClusterFlush()
#define emberAfDoorLockClusterDebugExec(x)
#define emberAfDoorLockClusterPrintBuffer(buffer, len, withSpace)
#define emberAfDoorLockClusterPrintString(buffer)

#define emberAfWindowCoveringClusterPrint(...)
#define emberAfWindowCoveringClusterPrintln(...)
#define emberAfWindowCoveringClusterFlush()
#define emberAfWindowCoveringClusterDebugExec(x)
#define emberAfWindowCoveringClusterPrintBuffer(buffer, len, withSpace)
#define emberAfWindowCoveringClusterPrintString(buffer)

#define emberAfBarrierControlClusterPrint(...)
#define emberAfBarrierControlClusterPrintln(...)
#define emberAfBarrierControlClusterFlush()
#define emberAfBarrierControlClusterDebugExec(x)
#define emberAfBarrierControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfBarrierControlClusterPrintString(buffer)

#define emberAfPumpConfigControlClusterPrint(...)
#define emberAfPumpConfigControlClusterPrintln(...)
#define emberAfPumpConfigControlClusterFlush()
#define emberAfPumpConfigControlClusterDebugExec(x)
#define emberAfPumpConfigControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfPumpConfigControlClusterPrintString(buffer)

#define emberAfThermostatClusterPrint(...)
#define emberAfThermostatClusterPrintln(...)
#define emberAfThermostatClusterFlush()
#define emberAfThermostatClusterDebugExec(x)
#define emberAfThermostatClusterPrintBuffer(buffer, len, withSpace)
#define emberAfThermostatClusterPrintString(buffer)

#define emberAfFanControlClusterPrint(...)
#define emberAfFanControlClusterPrintln(...)
#define emberAfFanControlClusterFlush()
#define emberAfFanControlClusterDebugExec(x)
#define emberAfFanControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfFanControlClusterPrintString(buffer)

#define emberAfDehumidControlClusterPrint(...)
#define emberAfDehumidControlClusterPrintln(...)
#define emberAfDehumidControlClusterFlush()
#define emberAfDehumidControlClusterDebugExec(x)
#define emberAfDehumidControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfDehumidControlClusterPrintString(buffer)

#define emberAfThermostatUiConfigClusterPrint(...)
#define emberAfThermostatUiConfigClusterPrintln(...)
#define emberAfThermostatUiConfigClusterFlush()
#define emberAfThermostatUiConfigClusterDebugExec(x)
#define emberAfThermostatUiConfigClusterPrintBuffer(buffer, len, withSpace)
#define emberAfThermostatUiConfigClusterPrintString(buffer)

#define emberAfColorControlClusterPrint(...)
#define emberAfColorControlClusterPrintln(...)
#define emberAfColorControlClusterFlush()
#define emberAfColorControlClusterDebugExec(x)
#define emberAfColorControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfColorControlClusterPrintString(buffer)

#define emberAfBallastConfigurationClusterPrint(...)
#define emberAfBallastConfigurationClusterPrintln(...)
#define emberAfBallastConfigurationClusterFlush()
#define emberAfBallastConfigurationClusterDebugExec(x)
#define emberAfBallastConfigurationClusterPrintBuffer(buffer, len, withSpace)
#define emberAfBallastConfigurationClusterPrintString(buffer)

#define emberAfIllumMeasurementClusterPrint(...)
#define emberAfIllumMeasurementClusterPrintln(...)
#define emberAfIllumMeasurementClusterFlush()
#define emberAfIllumMeasurementClusterDebugExec(x)
#define emberAfIllumMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfIllumMeasurementClusterPrintString(buffer)

#define emberAfIllumLevelSensingClusterPrint(...)
#define emberAfIllumLevelSensingClusterPrintln(...)
#define emberAfIllumLevelSensingClusterFlush()
#define emberAfIllumLevelSensingClusterDebugExec(x)
#define emberAfIllumLevelSensingClusterPrintBuffer(buffer, len, withSpace)
#define emberAfIllumLevelSensingClusterPrintString(buffer)

#define emberAfTempMeasurementClusterPrint(...)
#define emberAfTempMeasurementClusterPrintln(...)
#define emberAfTempMeasurementClusterFlush()
#define emberAfTempMeasurementClusterDebugExec(x)
#define emberAfTempMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfTempMeasurementClusterPrintString(buffer)

#define emberAfPressureMeasurementClusterPrint(...)
#define emberAfPressureMeasurementClusterPrintln(...)
#define emberAfPressureMeasurementClusterFlush()
#define emberAfPressureMeasurementClusterDebugExec(x)
#define emberAfPressureMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfPressureMeasurementClusterPrintString(buffer)

#define emberAfFlowMeasurementClusterPrint(...)
#define emberAfFlowMeasurementClusterPrintln(...)
#define emberAfFlowMeasurementClusterFlush()
#define emberAfFlowMeasurementClusterDebugExec(x)
#define emberAfFlowMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfFlowMeasurementClusterPrintString(buffer)

#define emberAfRelativeHumidityMeasurementClusterPrint(...)
#define emberAfRelativeHumidityMeasurementClusterPrintln(...)
#define emberAfRelativeHumidityMeasurementClusterFlush()
#define emberAfRelativeHumidityMeasurementClusterDebugExec(x)
#define emberAfRelativeHumidityMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfRelativeHumidityMeasurementClusterPrintString(buffer)

#define emberAfOccupancySensingClusterPrint(...)
#define emberAfOccupancySensingClusterPrintln(...)
#define emberAfOccupancySensingClusterFlush()
#define emberAfOccupancySensingClusterDebugExec(x)
#define emberAfOccupancySensingClusterPrintBuffer(buffer, len, withSpace)
#define emberAfOccupancySensingClusterPrintString(buffer)

#define emberAfCarbonMonoxideConcentrationMeasurementClusterPrint(...)
#define emberAfCarbonMonoxideConcentrationMeasurementClusterPrintln(...)
#define emberAfCarbonMonoxideConcentrationMeasurementClusterFlush()
#define emberAfCarbonMonoxideConcentrationMeasurementClusterDebugExec(x)
#define emberAfCarbonMonoxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfCarbonMonoxideConcentrationMeasurementClusterPrintString(buffer)

#define emberAfCarbonDioxideConcentrationMeasurementClusterPrint(...)
#define emberAfCarbonDioxideConcentrationMeasurementClusterPrintln(...)
#define emberAfCarbonDioxideConcentrationMeasurementClusterFlush()
#define emberAfCarbonDioxideConcentrationMeasurementClusterDebugExec(x)
#define emberAfCarbonDioxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfCarbonDioxideConcentrationMeasurementClusterPrintString(buffer)

#define emberAfEthyleneConcentrationMeasurementClusterPrint(...)
#define emberAfEthyleneConcentrationMeasurementClusterPrintln(...)
#define emberAfEthyleneConcentrationMeasurementClusterFlush()
#define emberAfEthyleneConcentrationMeasurementClusterDebugExec(x)
#define emberAfEthyleneConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfEthyleneConcentrationMeasurementClusterPrintString(buffer)

#define emberAfEthyleneOxideConcentrationMeasurementClusterPrint(...)
#define emberAfEthyleneOxideConcentrationMeasurementClusterPrintln(...)
#define emberAfEthyleneOxideConcentrationMeasurementClusterFlush()
#define emberAfEthyleneOxideConcentrationMeasurementClusterDebugExec(x)
#define emberAfEthyleneOxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfEthyleneOxideConcentrationMeasurementClusterPrintString(buffer)

#define emberAfHydrogenConcentrationMeasurementClusterPrint(...)
#define emberAfHydrogenConcentrationMeasurementClusterPrintln(...)
#define emberAfHydrogenConcentrationMeasurementClusterFlush()
#define emberAfHydrogenConcentrationMeasurementClusterDebugExec(x)
#define emberAfHydrogenConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfHydrogenConcentrationMeasurementClusterPrintString(buffer)

#define emberAfHydrogenSulphideConcentrationMeasurementClusterPrint(...)
#define emberAfHydrogenSulphideConcentrationMeasurementClusterPrintln(...)
#define emberAfHydrogenSulphideConcentrationMeasurementClusterFlush()
#define emberAfHydrogenSulphideConcentrationMeasurementClusterDebugExec(x)
#define emberAfHydrogenSulphideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfHydrogenSulphideConcentrationMeasurementClusterPrintString(buffer)

#define emberAfNitricOxideConcentrationMeasurementClusterPrint(...)
#define emberAfNitricOxideConcentrationMeasurementClusterPrintln(...)
#define emberAfNitricOxideConcentrationMeasurementClusterFlush()
#define emberAfNitricOxideConcentrationMeasurementClusterDebugExec(x)
#define emberAfNitricOxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfNitricOxideConcentrationMeasurementClusterPrintString(buffer)

#define emberAfNitrogenDioxideConcentrationMeasurementClusterPrint(...)
#define emberAfNitrogenDioxideConcentrationMeasurementClusterPrintln(...)
#define emberAfNitrogenDioxideConcentrationMeasurementClusterFlush()
#define emberAfNitrogenDioxideConcentrationMeasurementClusterDebugExec(x)
#define emberAfNitrogenDioxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfNitrogenDioxideConcentrationMeasurementClusterPrintString(buffer)

#define emberAfOxygenConcentrationMeasurementClusterPrint(...)
#define emberAfOxygenConcentrationMeasurementClusterPrintln(...)
#define emberAfOxygenConcentrationMeasurementClusterFlush()
#define emberAfOxygenConcentrationMeasurementClusterDebugExec(x)
#define emberAfOxygenConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfOxygenConcentrationMeasurementClusterPrintString(buffer)

#define emberAfOzoneConcentrationMeasurementClusterPrint(...)
#define emberAfOzoneConcentrationMeasurementClusterPrintln(...)
#define emberAfOzoneConcentrationMeasurementClusterFlush()
#define emberAfOzoneConcentrationMeasurementClusterDebugExec(x)
#define emberAfOzoneConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfOzoneConcentrationMeasurementClusterPrintString(buffer)

#define emberAfSulfurDioxideConcentrationMeasurementClusterPrint(...)
#define emberAfSulfurDioxideConcentrationMeasurementClusterPrintln(...)
#define emberAfSulfurDioxideConcentrationMeasurementClusterFlush()
#define emberAfSulfurDioxideConcentrationMeasurementClusterDebugExec(x)
#define emberAfSulfurDioxideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfSulfurDioxideConcentrationMeasurementClusterPrintString(buffer)

#define emberAfDissolvedOxygenConcentrationMeasurementClusterPrint(...)
#define emberAfDissolvedOxygenConcentrationMeasurementClusterPrintln(...)
#define emberAfDissolvedOxygenConcentrationMeasurementClusterFlush()
#define emberAfDissolvedOxygenConcentrationMeasurementClusterDebugExec(x)
#define emberAfDissolvedOxygenConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfDissolvedOxygenConcentrationMeasurementClusterPrintString(buffer)

#define emberAfBromateConcentrationMeasurementClusterPrint(...)
#define emberAfBromateConcentrationMeasurementClusterPrintln(...)
#define emberAfBromateConcentrationMeasurementClusterFlush()
#define emberAfBromateConcentrationMeasurementClusterDebugExec(x)
#define emberAfBromateConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfBromateConcentrationMeasurementClusterPrintString(buffer)

#define emberAfChloraminesConcentrationMeasurementClusterPrint(...)
#define emberAfChloraminesConcentrationMeasurementClusterPrintln(...)
#define emberAfChloraminesConcentrationMeasurementClusterFlush()
#define emberAfChloraminesConcentrationMeasurementClusterDebugExec(x)
#define emberAfChloraminesConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfChloraminesConcentrationMeasurementClusterPrintString(buffer)

#define emberAfChlorineConcentrationMeasurementClusterPrint(...)
#define emberAfChlorineConcentrationMeasurementClusterPrintln(...)
#define emberAfChlorineConcentrationMeasurementClusterFlush()
#define emberAfChlorineConcentrationMeasurementClusterDebugExec(x)
#define emberAfChlorineConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfChlorineConcentrationMeasurementClusterPrintString(buffer)

#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterPrint(...)
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterPrintln(...)
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterFlush()
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterDebugExec(x)
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfFecalColiformAndEColiConcentrationMeasurementClusterPrintString(buffer)

#define emberAfFluorideConcentrationMeasurementClusterPrint(...)
#define emberAfFluorideConcentrationMeasurementClusterPrintln(...)
#define emberAfFluorideConcentrationMeasurementClusterFlush()
#define emberAfFluorideConcentrationMeasurementClusterDebugExec(x)
#define emberAfFluorideConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfFluorideConcentrationMeasurementClusterPrintString(buffer)

#define emberAfHaloaceticAcidsConcentrationMeasurementClusterPrint(...)
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterPrintln(...)
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterFlush()
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterDebugExec(x)
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfHaloaceticAcidsConcentrationMeasurementClusterPrintString(buffer)

#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterPrint(...)
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterPrintln(...)
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterFlush()
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterDebugExec(x)
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfTotalTrihalomethanesConcentrationMeasurementClusterPrintString(buffer)

#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterPrint(...)
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterPrintln(...)
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterFlush()
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterDebugExec(x)
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfTotalColiformBacteriaConcentrationMeasurementClusterPrintString(buffer)

#define emberAfTurbidityConcentrationMeasurementClusterPrint(...)
#define emberAfTurbidityConcentrationMeasurementClusterPrintln(...)
#define emberAfTurbidityConcentrationMeasurementClusterFlush()
#define emberAfTurbidityConcentrationMeasurementClusterDebugExec(x)
#define emberAfTurbidityConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfTurbidityConcentrationMeasurementClusterPrintString(buffer)

#define emberAfCopperConcentrationMeasurementClusterPrint(...)
#define emberAfCopperConcentrationMeasurementClusterPrintln(...)
#define emberAfCopperConcentrationMeasurementClusterFlush()
#define emberAfCopperConcentrationMeasurementClusterDebugExec(x)
#define emberAfCopperConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfCopperConcentrationMeasurementClusterPrintString(buffer)

#define emberAfLeadConcentrationMeasurementClusterPrint(...)
#define emberAfLeadConcentrationMeasurementClusterPrintln(...)
#define emberAfLeadConcentrationMeasurementClusterFlush()
#define emberAfLeadConcentrationMeasurementClusterDebugExec(x)
#define emberAfLeadConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfLeadConcentrationMeasurementClusterPrintString(buffer)

#define emberAfManganeseConcentrationMeasurementClusterPrint(...)
#define emberAfManganeseConcentrationMeasurementClusterPrintln(...)
#define emberAfManganeseConcentrationMeasurementClusterFlush()
#define emberAfManganeseConcentrationMeasurementClusterDebugExec(x)
#define emberAfManganeseConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfManganeseConcentrationMeasurementClusterPrintString(buffer)

#define emberAfSulfateConcentrationMeasurementClusterPrint(...)
#define emberAfSulfateConcentrationMeasurementClusterPrintln(...)
#define emberAfSulfateConcentrationMeasurementClusterFlush()
#define emberAfSulfateConcentrationMeasurementClusterDebugExec(x)
#define emberAfSulfateConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfSulfateConcentrationMeasurementClusterPrintString(buffer)

#define emberAfBromodichloromethaneConcentrationMeasurementClusterPrint(...)
#define emberAfBromodichloromethaneConcentrationMeasurementClusterPrintln(...)
#define emberAfBromodichloromethaneConcentrationMeasurementClusterFlush()
#define emberAfBromodichloromethaneConcentrationMeasurementClusterDebugExec(x)
#define emberAfBromodichloromethaneConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfBromodichloromethaneConcentrationMeasurementClusterPrintString(buffer)

#define emberAfBromoformConcentrationMeasurementClusterPrint(...)
#define emberAfBromoformConcentrationMeasurementClusterPrintln(...)
#define emberAfBromoformConcentrationMeasurementClusterFlush()
#define emberAfBromoformConcentrationMeasurementClusterDebugExec(x)
#define emberAfBromoformConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfBromoformConcentrationMeasurementClusterPrintString(buffer)

#define emberAfChlorodibromomethaneConcentrationMeasurementClusterPrint(...)
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterPrintln(...)
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterFlush()
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterDebugExec(x)
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfChlorodibromomethaneConcentrationMeasurementClusterPrintString(buffer)

#define emberAfChloroformConcentrationMeasurementClusterPrint(...)
#define emberAfChloroformConcentrationMeasurementClusterPrintln(...)
#define emberAfChloroformConcentrationMeasurementClusterFlush()
#define emberAfChloroformConcentrationMeasurementClusterDebugExec(x)
#define emberAfChloroformConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfChloroformConcentrationMeasurementClusterPrintString(buffer)

#define emberAfSodiumConcentrationMeasurementClusterPrint(...)
#define emberAfSodiumConcentrationMeasurementClusterPrintln(...)
#define emberAfSodiumConcentrationMeasurementClusterFlush()
#define emberAfSodiumConcentrationMeasurementClusterDebugExec(x)
#define emberAfSodiumConcentrationMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfSodiumConcentrationMeasurementClusterPrintString(buffer)

#define emberAfIasZoneClusterPrint(...)
#define emberAfIasZoneClusterPrintln(...)
#define emberAfIasZoneClusterFlush()
#define emberAfIasZoneClusterDebugExec(x)
#define emberAfIasZoneClusterPrintBuffer(buffer, len, withSpace)
#define emberAfIasZoneClusterPrintString(buffer)

#define emberAfIasAceClusterPrint(...)
#define emberAfIasAceClusterPrintln(...)
#define emberAfIasAceClusterFlush()
#define emberAfIasAceClusterDebugExec(x)
#define emberAfIasAceClusterPrintBuffer(buffer, len, withSpace)
#define emberAfIasAceClusterPrintString(buffer)

#define emberAfIasWdClusterPrint(...)
#define emberAfIasWdClusterPrintln(...)
#define emberAfIasWdClusterFlush()
#define emberAfIasWdClusterDebugExec(x)
#define emberAfIasWdClusterPrintBuffer(buffer, len, withSpace)
#define emberAfIasWdClusterPrintString(buffer)

#define emberAfGenericTunnelClusterPrint(...)
#define emberAfGenericTunnelClusterPrintln(...)
#define emberAfGenericTunnelClusterFlush()
#define emberAfGenericTunnelClusterDebugExec(x)
#define emberAfGenericTunnelClusterPrintBuffer(buffer, len, withSpace)
#define emberAfGenericTunnelClusterPrintString(buffer)

#define emberAfBacnetProtocolTunnelClusterPrint(...)
#define emberAfBacnetProtocolTunnelClusterPrintln(...)
#define emberAfBacnetProtocolTunnelClusterFlush()
#define emberAfBacnetProtocolTunnelClusterDebugExec(x)
#define emberAfBacnetProtocolTunnelClusterPrintBuffer(buffer, len, withSpace)
#define emberAfBacnetProtocolTunnelClusterPrintString(buffer)

#define emberAf11073ProtocolTunnelClusterPrint(...)
#define emberAf11073ProtocolTunnelClusterPrintln(...)
#define emberAf11073ProtocolTunnelClusterFlush()
#define emberAf11073ProtocolTunnelClusterDebugExec(x)
#define emberAf11073ProtocolTunnelClusterPrintBuffer(buffer, len, withSpace)
#define emberAf11073ProtocolTunnelClusterPrintString(buffer)

#define emberAfIso7816ProtocolTunnelClusterPrint(...)
#define emberAfIso7816ProtocolTunnelClusterPrintln(...)
#define emberAfIso7816ProtocolTunnelClusterFlush()
#define emberAfIso7816ProtocolTunnelClusterDebugExec(x)
#define emberAfIso7816ProtocolTunnelClusterPrintBuffer(buffer, len, withSpace)
#define emberAfIso7816ProtocolTunnelClusterPrintString(buffer)

#define emberAfPriceClusterPrint(...)
#define emberAfPriceClusterPrintln(...)
#define emberAfPriceClusterFlush()
#define emberAfPriceClusterDebugExec(x)
#define emberAfPriceClusterPrintBuffer(buffer, len, withSpace)
#define emberAfPriceClusterPrintString(buffer)

#define emberAfDemandResponseLoadControlClusterPrint(...)
#define emberAfDemandResponseLoadControlClusterPrintln(...)
#define emberAfDemandResponseLoadControlClusterFlush()
#define emberAfDemandResponseLoadControlClusterDebugExec(x)
#define emberAfDemandResponseLoadControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfDemandResponseLoadControlClusterPrintString(buffer)

#define emberAfSimpleMeteringClusterPrint(...)
#define emberAfSimpleMeteringClusterPrintln(...)
#define emberAfSimpleMeteringClusterFlush()
#define emberAfSimpleMeteringClusterDebugExec(x)
#define emberAfSimpleMeteringClusterPrintBuffer(buffer, len, withSpace)
#define emberAfSimpleMeteringClusterPrintString(buffer)

#define emberAfMessagingClusterPrint(...)
#define emberAfMessagingClusterPrintln(...)
#define emberAfMessagingClusterFlush()
#define emberAfMessagingClusterDebugExec(x)
#define emberAfMessagingClusterPrintBuffer(buffer, len, withSpace)
#define emberAfMessagingClusterPrintString(buffer)

#define emberAfTunnelingClusterPrint(...)
#define emberAfTunnelingClusterPrintln(...)
#define emberAfTunnelingClusterFlush()
#define emberAfTunnelingClusterDebugExec(x)
#define emberAfTunnelingClusterPrintBuffer(buffer, len, withSpace)
#define emberAfTunnelingClusterPrintString(buffer)

#define emberAfPrepaymentClusterPrint(...)
#define emberAfPrepaymentClusterPrintln(...)
#define emberAfPrepaymentClusterFlush()
#define emberAfPrepaymentClusterDebugExec(x)
#define emberAfPrepaymentClusterPrintBuffer(buffer, len, withSpace)
#define emberAfPrepaymentClusterPrintString(buffer)

#define emberAfEnergyManagementClusterPrint(...)
#define emberAfEnergyManagementClusterPrintln(...)
#define emberAfEnergyManagementClusterFlush()
#define emberAfEnergyManagementClusterDebugExec(x)
#define emberAfEnergyManagementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfEnergyManagementClusterPrintString(buffer)

#define emberAfCalendarClusterPrint(...)
#define emberAfCalendarClusterPrintln(...)
#define emberAfCalendarClusterFlush()
#define emberAfCalendarClusterDebugExec(x)
#define emberAfCalendarClusterPrintBuffer(buffer, len, withSpace)
#define emberAfCalendarClusterPrintString(buffer)

#define emberAfDeviceManagementClusterPrint(...)
#define emberAfDeviceManagementClusterPrintln(...)
#define emberAfDeviceManagementClusterFlush()
#define emberAfDeviceManagementClusterDebugExec(x)
#define emberAfDeviceManagementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfDeviceManagementClusterPrintString(buffer)

#define emberAfEventsClusterPrint(...)
#define emberAfEventsClusterPrintln(...)
#define emberAfEventsClusterFlush()
#define emberAfEventsClusterDebugExec(x)
#define emberAfEventsClusterPrintBuffer(buffer, len, withSpace)
#define emberAfEventsClusterPrintString(buffer)

#define emberAfMduPairingClusterPrint(...)
#define emberAfMduPairingClusterPrintln(...)
#define emberAfMduPairingClusterFlush()
#define emberAfMduPairingClusterDebugExec(x)
#define emberAfMduPairingClusterPrintBuffer(buffer, len, withSpace)
#define emberAfMduPairingClusterPrintString(buffer)

#define emberAfSubGhzClusterPrint(...)
#define emberAfSubGhzClusterPrintln(...)
#define emberAfSubGhzClusterFlush()
#define emberAfSubGhzClusterDebugExec(x)
#define emberAfSubGhzClusterPrintBuffer(buffer, len, withSpace)
#define emberAfSubGhzClusterPrintString(buffer)

#define emberAfKeyEstablishmentClusterPrint(...)
#define emberAfKeyEstablishmentClusterPrintln(...)
#define emberAfKeyEstablishmentClusterFlush()
#define emberAfKeyEstablishmentClusterDebugExec(x)
#define emberAfKeyEstablishmentClusterPrintBuffer(buffer, len, withSpace)
#define emberAfKeyEstablishmentClusterPrintString(buffer)

#define emberAfInformationClusterPrint(...)
#define emberAfInformationClusterPrintln(...)
#define emberAfInformationClusterFlush()
#define emberAfInformationClusterDebugExec(x)
#define emberAfInformationClusterPrintBuffer(buffer, len, withSpace)
#define emberAfInformationClusterPrintString(buffer)

#define emberAfDataSharingClusterPrint(...)
#define emberAfDataSharingClusterPrintln(...)
#define emberAfDataSharingClusterFlush()
#define emberAfDataSharingClusterDebugExec(x)
#define emberAfDataSharingClusterPrintBuffer(buffer, len, withSpace)
#define emberAfDataSharingClusterPrintString(buffer)

#define emberAfGamingClusterPrint(...)
#define emberAfGamingClusterPrintln(...)
#define emberAfGamingClusterFlush()
#define emberAfGamingClusterDebugExec(x)
#define emberAfGamingClusterPrintBuffer(buffer, len, withSpace)
#define emberAfGamingClusterPrintString(buffer)

#define emberAfDataRateControlClusterPrint(...)
#define emberAfDataRateControlClusterPrintln(...)
#define emberAfDataRateControlClusterFlush()
#define emberAfDataRateControlClusterDebugExec(x)
#define emberAfDataRateControlClusterPrintBuffer(buffer, len, withSpace)
#define emberAfDataRateControlClusterPrintString(buffer)

#define emberAfVoiceOverZigbeeClusterPrint(...)
#define emberAfVoiceOverZigbeeClusterPrintln(...)
#define emberAfVoiceOverZigbeeClusterFlush()
#define emberAfVoiceOverZigbeeClusterDebugExec(x)
#define emberAfVoiceOverZigbeeClusterPrintBuffer(buffer, len, withSpace)
#define emberAfVoiceOverZigbeeClusterPrintString(buffer)

#define emberAfChattingClusterPrint(...)
#define emberAfChattingClusterPrintln(...)
#define emberAfChattingClusterFlush()
#define emberAfChattingClusterDebugExec(x)
#define emberAfChattingClusterPrintBuffer(buffer, len, withSpace)
#define emberAfChattingClusterPrintString(buffer)

#define emberAfPaymentClusterPrint(...)
#define emberAfPaymentClusterPrintln(...)
#define emberAfPaymentClusterFlush()
#define emberAfPaymentClusterDebugExec(x)
#define emberAfPaymentClusterPrintBuffer(buffer, len, withSpace)
#define emberAfPaymentClusterPrintString(buffer)

#define emberAfBillingClusterPrint(...)
#define emberAfBillingClusterPrintln(...)
#define emberAfBillingClusterFlush()
#define emberAfBillingClusterDebugExec(x)
#define emberAfBillingClusterPrintBuffer(buffer, len, withSpace)
#define emberAfBillingClusterPrintString(buffer)

#define emberAfApplianceIdentificationClusterPrint(...)
#define emberAfApplianceIdentificationClusterPrintln(...)
#define emberAfApplianceIdentificationClusterFlush()
#define emberAfApplianceIdentificationClusterDebugExec(x)
#define emberAfApplianceIdentificationClusterPrintBuffer(buffer, len, withSpace)
#define emberAfApplianceIdentificationClusterPrintString(buffer)

#define emberAfMeterIdentificationClusterPrint(...)
#define emberAfMeterIdentificationClusterPrintln(...)
#define emberAfMeterIdentificationClusterFlush()
#define emberAfMeterIdentificationClusterDebugExec(x)
#define emberAfMeterIdentificationClusterPrintBuffer(buffer, len, withSpace)
#define emberAfMeterIdentificationClusterPrintString(buffer)

#define emberAfApplianceEventsAndAlertClusterPrint(...)
#define emberAfApplianceEventsAndAlertClusterPrintln(...)
#define emberAfApplianceEventsAndAlertClusterFlush()
#define emberAfApplianceEventsAndAlertClusterDebugExec(x)
#define emberAfApplianceEventsAndAlertClusterPrintBuffer(buffer, len, withSpace)
#define emberAfApplianceEventsAndAlertClusterPrintString(buffer)

#define emberAfApplianceStatisticsClusterPrint(...)
#define emberAfApplianceStatisticsClusterPrintln(...)
#define emberAfApplianceStatisticsClusterFlush()
#define emberAfApplianceStatisticsClusterDebugExec(x)
#define emberAfApplianceStatisticsClusterPrintBuffer(buffer, len, withSpace)
#define emberAfApplianceStatisticsClusterPrintString(buffer)

#define emberAfElectricalMeasurementClusterPrint(...)
#define emberAfElectricalMeasurementClusterPrintln(...)
#define emberAfElectricalMeasurementClusterFlush()
#define emberAfElectricalMeasurementClusterDebugExec(x)
#define emberAfElectricalMeasurementClusterPrintBuffer(buffer, len, withSpace)
#define emberAfElectricalMeasurementClusterPrintString(buffer)

#define emberAfDiagnosticsClusterPrint(...)
#define emberAfDiagnosticsClusterPrintln(...)
#define emberAfDiagnosticsClusterFlush()
#define emberAfDiagnosticsClusterDebugExec(x)
#define emberAfDiagnosticsClusterPrintBuffer(buffer, len, withSpace)
#define emberAfDiagnosticsClusterPrintString(buffer)

#define emberAfZllCommissioningClusterPrint(...)
#define emberAfZllCommissioningClusterPrintln(...)
#define emberAfZllCommissioningClusterFlush()
#define emberAfZllCommissioningClusterDebugExec(x)
#define emberAfZllCommissioningClusterPrintBuffer(buffer, len, withSpace)
#define emberAfZllCommissioningClusterPrintString(buffer)

#define emberAfSampleMfgSpecificClusterPrint(...)
#define emberAfSampleMfgSpecificClusterPrintln(...)
#define emberAfSampleMfgSpecificClusterFlush()
#define emberAfSampleMfgSpecificClusterDebugExec(x)
#define emberAfSampleMfgSpecificClusterPrintBuffer(buffer, len, withSpace)
#define emberAfSampleMfgSpecificClusterPrintString(buffer)

#define emberAfOtaConfigurationClusterPrint(...)
#define emberAfOtaConfigurationClusterPrintln(...)
#define emberAfOtaConfigurationClusterFlush()
#define emberAfOtaConfigurationClusterDebugExec(x)
#define emberAfOtaConfigurationClusterPrintBuffer(buffer, len, withSpace)
#define emberAfOtaConfigurationClusterPrintString(buffer)

#define emberAfMfglibClusterPrint(...)
#define emberAfMfglibClusterPrintln(...)
#define emberAfMfglibClusterFlush()
#define emberAfMfglibClusterDebugExec(x)
#define emberAfMfglibClusterPrintBuffer(buffer, len, withSpace)
#define emberAfMfglibClusterPrintString(buffer)

#define emberAfSlWwahClusterPrint(...)
#define emberAfSlWwahClusterPrintln(...)
#define emberAfSlWwahClusterFlush()
#define emberAfSlWwahClusterDebugExec(x)
#define emberAfSlWwahClusterPrintBuffer(buffer, len, withSpace)
#define emberAfSlWwahClusterPrintString(buffer)

#endif // (defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT) && (SL_ZIGBEE_DEBUG_ZCL_GROUP_ENABLED == 1))

#endif // UC_BUILD
