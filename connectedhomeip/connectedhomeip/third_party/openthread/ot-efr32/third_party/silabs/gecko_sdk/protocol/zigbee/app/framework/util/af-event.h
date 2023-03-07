/***************************************************************************//**
 * @file
 * @brief Interface for the application event mechanism
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

#ifndef SILABS_AF_EVENT_H
#define SILABS_AF_EVENT_H

#include "../include/af.h"

#define MAX_TIMER_UNITS_HOST 0x7fff
#define MAX_TIMER_MILLISECONDS_HOST (MAX_TIMER_UNITS_HOST * MILLISECOND_TICKS_PER_MINUTE)

#if !defined(ATTRIBUTE_STORAGE_CONFIGURATION) && defined(EMBER_TEST)
  #include "af-event-test.h"
#else
#ifdef UC_BUILD
  #include "zap-event.h"
#else
  #include "af-gen-event.h"
#endif
#endif

#ifdef UC_BUILD

extern uint16_t sli_zigbee_app_event_context_length;
extern sl_zigbee_event_context_t sli_zigbee_app_event_context[];
#ifndef EMBER_AF_NCP
extern EmberAfEventSleepControl emberAfGetCurrentSleepControlCallback(void);
#endif //#ifndef EMBER_AF_NCP
#else
// Task ids referenced in the sleep code
extern EmberTaskId emAfTaskId;

extern uint16_t emAfAppEventContextLength;
extern EmberAfEventContext emAfAppEventContext[];

extern const char * emAfEventStrings[];
extern EmberEventData emAfEvents[];

#endif // UC_BUILD

// A function used to retrieve the proper NCP timer duration and unit based on a given
// passed number of milliseconds.
void emAfGetTimerDurationAndUnitFromMS(uint32_t durationMs,
                                       uint16_t *duration,
                                       EmberEventUnits *units);

// A function (inverse of the above) to retrieve the number of milliseconds
// represented by a given timer duration and unit.
uint32_t emAfGetMSFromTimerDurationAndUnit(uint16_t duration,
                                           EmberEventUnits units);

//A function to initialize the Event mechanism used to drive the application framework.
void emAfInitEvents(void);

uint32_t emberAfMsToNextEventExtended(uint32_t maxMs, uint8_t* returnIndex);

const char * emberAfGetEventString(uint8_t index);

#endif // SILABS_AF_EVENT_H
