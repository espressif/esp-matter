#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#include "hal-config-board.h"

// <<< Use Configuration Wizard in Context Menu >>>

// --------------------------------
// <h> Clock Settings

// <o HAL_CLK_HFCLK_SOURCE> High Frequency Clock Source
//   <HAL_CLK_HFCLK_SOURCE_HFRCO=> HFRCO (RC oscillator)
//   <HAL_CLK_HFCLK_SOURCE_HFXO=> HFXO (Crystal oscillator)
// <i> Clock source for the High Frequency clock tree
#define HAL_CLK_HFCLK_SOURCE            HAL_CLK_HFCLK_SOURCE_HFXO

// <o HAL_CLK_HFXO_AUTOSTART> Start HFXO Automatically on EM0/1 Entry
//   <HAL_CLK_HFXO_AUTOSTART_NONE=> Do not start automatically
//   <HAL_CLK_HFXO_AUTOSTART_START=> Start automatically
//   <HAL_CLK_HFXO_AUTOSTART_SELECT=> Start automatically and select as HFCLK
// <i> Automatically start HFXO on wakeup from sleep
#define HAL_CLK_HFXO_AUTOSTART          HAL_CLK_HFXO_AUTOSTART_NONE

// </h>

// <<< end of configuration section >>>

// TODO: Make VCOM enable configurable
#if defined(BSP_VCOM_ENABLE_PIN)
#define HAL_VCOM_ENABLE                 1
#endif

#endif // HAL_CONFIG_H
