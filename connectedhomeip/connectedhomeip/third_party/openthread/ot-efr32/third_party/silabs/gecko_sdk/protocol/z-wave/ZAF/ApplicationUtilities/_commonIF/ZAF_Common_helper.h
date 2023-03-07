/**
 * @file
 * @brief Common stuff used by apps.
 * @details It's purpose is to minimize function calls from end applications.
 *  Shouldn't be called directly from CC or utils, to avoid mutual dependencies
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _ZAF_COMMON_HELPER_H_
#define _ZAF_COMMON_HELPER_H_

#include "ZAF_Common_interface.h"


/**
 * Initialization function called from slave devices
 * Does all the common stuff - as many functions as possible should be called from here
 * Reduces the number of initializations directly from applications
 *
 * @param pAppTaskHandle Task handle
 * @param pAppHandle Application handle
 * @param pAppProtocolConfig Protocol Configuration struct
 * @param updateStayAwakePeriodFunc Callback function used for Power Management module.
 *  Must be NULL if not relevant
 */
void ZAF_Init(TaskHandle_t pAppTaskHandle,
              SApplicationHandles* pAppHandle,
              const SProtocolConfig_t * pAppProtocolConfig,
              void (*updateStayAwakePeriodFunc)(void)
              );

/**
 * Reset ZAF and command classes that have registered a reset function.
 */
void ZAF_Reset(void);

/**
 * Callback function for FLiRS nodes.
 * Used to handle power management
 *
 */
void ZAF_FLiRS_StayAwake();

#endif /* _ZAF_COMMON_HELPER_H_ */
