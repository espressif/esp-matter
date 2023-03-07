/***************************************************************************//**
 * @file
 * @brief This file contains the EFR32 radio coexistence configuration.
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

#ifndef __COEXISTENCE_HAL_CONFIG_H__
#define __COEXISTENCE_HAL_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PLATFORM_HEADER
#define PHY_RAIL 1
#include <stdint.h>
typedef bool sl_status_t;
#define SL_STATUS_NOT_SUPPORTED (false)
#define SL_STATUS_OK            (true)
typedef uint8_t EmberCounterType;
#define EMBER_COUNTER_PTA_LO_PRI_TX_ABORTED (0U)
#define EMBER_COUNTER_PTA_HI_PRI_TX_ABORTED (1U)
#define EMBER_COUNTER_PTA_LO_PRI_DENIED     (2U)
#define EMBER_COUNTER_PTA_HI_PRI_DENIED     (3U)
#define EMBER_COUNTER_PTA_LO_PRI_REQUESTED  (4U)
#define EMBER_COUNTER_PTA_HI_PRI_REQUESTED  (5U)
#define UNUSED_VAR(x) (void)(x)
#define WEAK(x)       x
#define halCommonDelayMicroseconds(x) //no-op
#endif //!PLATFORM_HEADER

#define SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT                 (1)
#define SL_RAIL_UTIL_COEX_ENABLED                             (1)
#define SL_RAIL_UTIL_COEX_DP_TIMER                            (HAL_TIMER_TIMER1)
#define SL_RAIL_UTIL_COEX_PWM_PRIORITY                        (0)
#define SL_RAIL_UTIL_COEX_RETRYRX_ENABLE                      (0)
#define SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD                  (0U)
#define SL_RAIL_UTIL_COEX_REQ_WINDOW                          (500U)
#define SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED                 (0)
#define SL_RAIL_UTIL_COEX_TX_ABORT                            (0)
#define SL_RAIL_UTIL_COEX_RHO_ASSERT_LEVEL                    (1)
#define SL_RAIL_UTIL_COEX_REQ_SHARED                          (0)
#define SL_RAIL_UTIL_COEX_DP_PULSE_WIDTH_US                   (20U)
#define SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL                    (1)
#define SL_RAIL_UTIL_COEX_TX_HIPRI                            (1)
#define SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL                    (1)
#define SL_RAIL_UTIL_COEX_RX_HIPRI                            (1)
#define SL_RAIL_UTIL_COEX_GNT_ASSERT_LEVEL                    (1)
#define SL_RAIL_UTIL_COEX_CCA_THRESHOLD                       (4U)
#define SL_RAIL_UTIL_COEX_PWM_REQ_DUTYCYCLE                   (20U)
#define SL_RAIL_UTIL_COEX_RETRYRX_HIPRI                       (1)
#define SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT                     (16U)
#define SL_RAIL_UTIL_COEX_REQ_BACKOFF                         (15U)
#define SL_RAIL_UTIL_COEX_PRI_SHARED                          (0)
#define SL_RAIL_UTIL_COEX_DP_ENABLED                          (1)
#define SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE          (1)
#define SL_RAIL_UTIL_COEX_ACKHOLDOFF                          (1)
#define SL_RAIL_UTIL_COEX_PWM_REQ_PERIOD                      (78U)
#define SL_RAIL_UTIL_COEX_PHY_ENABLED                         (0)
#define SL_RAIL_UTIL_COEX_PWM_REQ_ASSERT_LEVEL                (1)
#define SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT                  (1)

#define SL_RAIL_UTIL_COEX_RX_ACTIVE_PIN                       (14U)
#define SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT                      (gpioPortD)
#define SL_RAIL_UTIL_COEX_RX_ACTIVE_ASSERT_LEVEL              (1)
#define SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL                   (5)
#define SL_RAIL_UTIL_COEX_RX_ACTIVE_LOC                       (4)

#define SL_RAIL_UTIL_COEX_PHY_SELECT_PIN                      (14U)
#define SL_RAIL_UTIL_COEX_PHY_SELECT_PORT                     (gpioPortC)
#define SL_RAIL_UTIL_COEX_PHY_SELECT_ASSERT_LEVEL             (1)
#define SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT          (10U)

#ifdef _SILICON_LABS_32B_SERIES_1
#define SL_RAIL_UTIL_COEX_GNT_PIN                             (9U)
#define SL_RAIL_UTIL_COEX_GNT_PORT                            (gpioPortC)

#define SL_RAIL_UTIL_COEX_PRI_PIN                             (13U)
#define SL_RAIL_UTIL_COEX_PRI_PORT                            (gpioPortD)

#define SL_RAIL_UTIL_COEX_DP_PIN                              (12U)
#define SL_RAIL_UTIL_COEX_DP_PORT                             (gpioPortD)
#define SL_RAIL_UTIL_COEX_DP_LOC                              (11U)

#define SL_RAIL_UTIL_COEX_REQ_PIN                             (10U)
#define SL_RAIL_UTIL_COEX_REQ_PORT                            (gpioPortC)

#define SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL              (4)
#define SL_RAIL_UTIL_COEX_DP_CHANNEL                          (3)
#define SL_RAIL_UTIL_COEX_DP_CC0_PIN                          (10U)
#define SL_RAIL_UTIL_COEX_DP_CC0_PORT                         (gpioPortC)
#define SL_RAIL_UTIL_COEX_DP_CC0_LOC                          (15U)
#else //!_SILICON_LABS_32B_SERIES_1
#define SL_RAIL_UTIL_COEX_GNT_PIN                             (3U)
#define SL_RAIL_UTIL_COEX_GNT_PORT                            (gpioPortC)

#define SL_RAIL_UTIL_COEX_PRI_PIN                             (8U)
#define SL_RAIL_UTIL_COEX_PRI_PORT                            (gpioPortD)

#define SL_RAIL_UTIL_COEX_REQ_PIN                             (10U)
#define SL_RAIL_UTIL_COEX_REQ_PORT                            (gpioPortC)

#define SL_RAIL_UTIL_COEX_DP_PIN                              (11U)
#define SL_RAIL_UTIL_COEX_DP_PORT                             (gpioPortD)
#define SL_RAIL_UTIL_COEX_DP_CHANNEL                          (3)
#endif //_SILICON_LABS_32B_SERIES_1

#ifdef __cplusplus
}
#endif

#endif //__COEXISTENCE_HAL_CONFIG_H__
