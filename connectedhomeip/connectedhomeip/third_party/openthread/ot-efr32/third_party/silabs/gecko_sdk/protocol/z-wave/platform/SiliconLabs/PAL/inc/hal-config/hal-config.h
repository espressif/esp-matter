/**
 * @file
 * @brief Hal config header file
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#ifdef ZWAVE_SERIES_800
#include "hal-config-board-800.h"
#else
#include "hal-config-board-700.h"
#endif

#endif /* HAL_CONFIG_H */
