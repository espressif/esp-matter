/***************************************************************************//**
 * @brief Zigbee Concentrator Support component configuration header.
 *\n*******************************************************************************
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee Concentrator Support configuration

// <o EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE> Concentrator Type
// <i> Concentrator Type
// <LOW_RAM_CONCENTRATOR => Low RAM Concentrator
// <HIGH_RAM_CONCENTRATOR => High RAM Concentrator
// <i> Default: LOW_RAM_CONCENTRATOR
// <i> The type of concentrator that the node will advertise itself as.  A low ram concentrator will receive route record messages every time a device wishes to send to it.  A high ram concentrator will only receive route record messages after a new MTORR broadcast.
#define EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE   LOW_RAM_CONCENTRATOR

// <o EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS> Minimum Time between broadcasts (in seconds) <1-60>
// <i> Default: 10
// <i> The minimum amount of time that must pass between MTORR broadcasts.
#define EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS   10

// <o EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS> Maximum Time between broadcasts (in seconds) <30-300>
// <i> Default: 60
// <i> The maximum amount of time that can pass between MTORR broadcasts.
#define EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS   60

// <o EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD> Route Error Threshold <1-100>
// <i> Default: 3
// <i> The number of route errors that will trigger a re-broadcast of the MTORR.
#define EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD   3

// <o EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD> Delivery Failure Threshold <1-100>
// <i> Default: 1
// <i> The number of APS delivery failures that will trigger a re-broadcast of the MTORR.
#define EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD   1

// <o EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS> Maximum number of hops for Broadcast <0-30>
// <i> Default: 0
// <i> The maximum number of hops that the MTORR broadcast will be allowed to have.  A value of 0 will be converted to the EMBER_MAX_HOPS value set by the stack.
#define EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS   0

// <o EMBER_AF_PLUGIN_CONCENTRATOR_DEFAULT_ROUTER_BEHAVIOR> Default concentrator router behavior
// <i> Default concentrator router behavior
// <EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_NONE=> None
// <EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL=> Full
// <i> Default: EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL
// <i> This sets the initial value for the concentrator router behavior. The behavior can be set at runtime with emberAfPluginConcentratorSetRouterBehaviorCommand.
#define EMBER_AF_PLUGIN_CONCENTRATOR_DEFAULT_ROUTER_BEHAVIOR   EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL

// </h>

// <<< end of configuration section >>>
