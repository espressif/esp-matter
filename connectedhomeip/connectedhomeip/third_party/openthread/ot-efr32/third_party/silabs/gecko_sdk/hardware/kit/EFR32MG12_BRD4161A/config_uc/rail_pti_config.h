#ifndef RAIL_PTI_CONFIG_H
#define RAIL_PTI_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o RAIL_PTI_MODE> PTI Mode
// <RAIL_PTI_MODE_DISABLED=> Disabled
// <RAIL_PTI_MODE_SPI=> SPI
// <RAIL_PTI_MODE_UART=> UART
// <RAIL_PTI_MODE_UART_ONEWIRE=> One-wire
// <i> Default: RAIL_PTI_MODE_UART
#define RAIL_PTI_MODE       RAIL_PTI_MODE_UART

// <o RAIL_PTI_BAUDRATE> PTI Baudrate
// <i> Default: 1600000
#define RAIL_PTI_BAUDRATE   1600000

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <pti signal=DOUT,(DFRAME),(DCLK)> RAIL_PTI
// $[PTI_RAIL_PTI]
#define RAIL_PTI_DOUT_PORT      gpioPortB
#define RAIL_PTI_DOUT_PIN       12
#define RAIL_PTI_DOUT_LOC       6

#define RAIL_PTI_DFRAME_PORT    gpioPortB
#define RAIL_PTI_DFRAME_PIN     13
#define RAIL_PTI_DFRAME_LOC     6
// [PTI_RAIL_PTI]$

// <<< sl:end pin_tool >>>

// Map settings to config struct

// RAIL PTI init wants a DCLK location even if mode is UART
#ifndef RAIL_PTI_DCLK_PORT
  #define RAIL_PTI_DCLK_PORT  gpioPortA
  #define RAIL_PTI_DCLK_PIN   0
  #define RAIL_PTI_DCLK_LOC   0
#endif

// RAIL PTI init wants a location even on devices that don't have one
#ifndef RAIL_PTI_DCLK_LOC
#define RAIL_PTI_DCLK_LOC     0
#endif
#ifndef RAIL_PTI_DOUT_LOC
#define RAIL_PTI_DOUT_LOC     0
#endif
#ifndef RAIL_PTI_DFRAME_LOC
#define RAIL_PTI_DFRAME_LOC   0
#endif

#define RAIL_PTI_CONFIG   \
  {                       \
    RAIL_PTI_MODE,        \
    RAIL_PTI_BAUDRATE,    \
    RAIL_PTI_DOUT_LOC,    \
    RAIL_PTI_DOUT_PORT,   \
    RAIL_PTI_DOUT_PIN,    \
    RAIL_PTI_DCLK_LOC,    \
    RAIL_PTI_DCLK_PORT,   \
    RAIL_PTI_DCLK_PIN,    \
    RAIL_PTI_DFRAME_LOC,  \
    RAIL_PTI_DFRAME_PORT, \
    RAIL_PTI_DFRAME_PIN,  \
  }

#endif // RAIL_PTI_CONFIG_H
