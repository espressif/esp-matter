#ifndef TRACEHALCONFIG_H
#define TRACEHALCONFIG_H

#include "hal-config.h"

#if defined(_SILICON_LABS_32B_SERIES_0)
// Series 0 SWO pin is on DBG route location
#define BSP_TRACE_SWO_LOCATION     BSP_TRACE_DBGROUTE_LOC
#else
// Series 1 SWO pin has its own route location
#define BSP_TRACE_SWO_LOCATION     BSP_TRACE_SWO_LOC
#endif

/* Enable output on pin - GPIO Port F, Pin 2. */
#define TRACE_ENABLE_PINS()           \
  GPIO_PinModeSet(BSP_TRACE_SWO_PORT, \
                  BSP_TRACE_SWO_PIN,  \
                  gpioModePushPull,   \
                  1)

#endif // TRACEHALCONFIG_H
