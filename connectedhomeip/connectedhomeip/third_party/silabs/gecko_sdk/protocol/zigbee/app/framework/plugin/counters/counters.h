/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Counters plugin.
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

#ifndef SILABS_PLUGIN_COUNTERS_H
#define SILABS_PLUGIN_COUNTERS_H

#ifdef UC_BUILD
#include "counters-config.h"
#if (EMBER_AF_PLUGIN_COUNTERS_OTA == 1)
#define OTA_COUNTERS_ENABLED
#endif
#else //!UC_BUILD
#ifdef EMBER_AF_PLUGIN_COUNTERS_OTA
#define OTA_COUNTERS_ENABLED
#endif
#endif // UC_BUILD

#define EMBER_APPLICATION_HAS_COUNTER_ROLLOVER_HANDLER
/**
 * The ith entry in this array is the count of events of EmberCounterType i.
 */
extern uint16_t emberCounters[EMBER_COUNTER_TYPE_COUNT];
/**
 * The ith entry in this array is the count of threshold values set for
 * the corresponding ith event in emberCounters. The default value is set to
 * 0xFFFF and can be changed by an application by calling
 * emberSet
 */
#ifdef EMBER_APPLICATION_HAS_COUNTER_ROLLOVER_HANDLER
extern uint16_t emberCountersThresholds[EMBER_COUNTER_TYPE_COUNT];
#endif

#if !defined(EMBER_MULTI_NETWORK_STRIPPED)
#define MULTI_NETWORK_COUNTER_TYPE_COUNT 17
/**
 * The value at the position [n,i] in this matrix is the count of events of
 * per-network EmberCounterType i for network n.
 */
extern uint16_t emberMultiNetworkCounters[EMBER_SUPPORTED_NETWORKS]
[MULTI_NETWORK_COUNTER_TYPE_COUNT];
#endif // EMBER_MULTI_NETWORK_STRIPPED

/** Reset the counters to zero. */
void emberAfPluginCountersClear(void);
/** Resets counter thresholds to 0xFFFF. **/
void emberAfPluginCountersResetThresholds(void);
/** Set the threshold for a particular counter type **/
void emberAfPluginCountersSetThreshold(EmberCounterType type, uint16_t threshold);

/** @brief Rollover
 *
 * This function is called every time a counter exceeds its threshold.
 *
 * @param type The counter that rolled over Ver.: always
 */
void emberAfPluginCountersRolloverCallback(EmberCounterType type);

#endif // SILABS_PLUGIN_COUNTERS_H
