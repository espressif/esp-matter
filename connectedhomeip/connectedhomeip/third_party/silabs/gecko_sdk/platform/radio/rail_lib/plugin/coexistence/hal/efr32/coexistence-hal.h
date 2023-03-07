/***************************************************************************//**
 * @file
 * @brief This file contains the EFR32 radio coexistence interface.
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

#ifndef __COEXISTENCE_HAL_H__
#define __COEXISTENCE_HAL_H__
#include "em_device.h"
#include "em_prs.h"

#ifdef HAL_CONFIG
  #include "hal-config.h"
#else
  #include "sl_rail_util_coex_config.h"
#endif // HAL_CONFIG
#ifdef TEST_COEX_HAL_CONFIG
  #include "coexistence-hal-config.h"
#endif //TEST_COEX_HAL_CONFIG
#include "coexistence.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "gpiointerrupt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct COEX_HAL_GpioConfig {
  /** GPIO signal **/
  uint32_t signal;

  /** GPIO source **/
  uint32_t source;

  /** GPIO port */
  uint8_t port;

  /** GPIO pin */
  uint8_t pin;

  /** GPIO interrupt number */
  uint8_t intNo;

  /** GPIO assert polarity */
  bool polarity;

  /** GPIO PWM enabled */
  bool pwmEnabled;

  /** GPIO mode */
  uint8_t mode;

  /** GPIO ISR */
  GPIOINT_IrqCallbackPtrExt_t isr;

  /** GPIO config */
  COEX_GpioConfig_t config;
} COEX_HAL_GpioConfig_t;

void COEX_HAL_Init(void);
bool COEX_HAL_ConfigRequest(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigExternalRequest(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigPwmRequest(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigRadioHoldOff(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigPriority(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigGrant(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigPhySelect(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigDp(uint8_t pulseWidthUs);
uint8_t COEX_HAL_GetDpPulseWidth(void);
bool COEX_HAL_SetDpPulseWidth(uint8_t pulseWidthUs);
uint16_t COEX_HAL_GetPseudoRandom(uint16_t min_value, uint16_t max_value);
#ifdef SL_RAIL_UTIL_COEX_WIFI_TX_PORT
bool COEX_HAL_ConfigWifiTx(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_GetWifiTx(void);
#endif
void sli_coex_enableGpioInt(COEX_GpioHandle_t gpioHandle,
                            bool enabled,
                            bool *wasAsserted);

#define PRS_GPIO_SIGNAL(pin) ((pin) & 7U)

#ifdef _SILICON_LABS_32B_SERIES_1
#define PRS_GPIO_SOURCE(pin)                  \
  (((pin) > 7U) ? PRS_CH_CTRL_SOURCESEL_GPIOH \
   : PRS_CH_CTRL_SOURCESEL_GPIOL)
#define PRS_CHANNEL_SOURCE(ch)              \
  (((ch) > 7U) ? PRS_CH_CTRL_SOURCESEL_PRSH \
   : PRS_CH_CTRL_SOURCESEL_PRSL)
#else //!_SILICON_LABS_32B_SERIES_1
#define PRS_GPIO_SOURCE(pin) (PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO)
#endif //_SILICON_LABS_32B_SERIES_1

#define PRS_CHANNEL_SIGNAL(ch) ((ch) & 7U)

#define INVALID_INTERRUPT 0xFF
#define INVALID_SOURCE 0U
#define INVALID_SIGNAL 0U

#ifdef SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
extern COEX_HAL_GpioConfig_t sli_coex_phySelectCfg;
#endif //SL_RAIL_UTIL_COEX_PHY_SELECT_PORT

#ifdef SL_RAIL_UTIL_COEX_GNT_PORT
extern COEX_HAL_GpioConfig_t sli_coex_ptaGntCfg;
#endif //SL_RAIL_UTIL_COEX_GNT_PORT

#ifdef SL_RAIL_UTIL_COEX_PRI_PORT
extern COEX_HAL_GpioConfig_t sli_coex_ptaPriCfg;
#endif //SL_RAIL_UTIL_COEX_PRI_PORT

#ifdef SL_RAIL_UTIL_COEX_REQ_PORT
extern COEX_HAL_GpioConfig_t sli_coex_ptaReqCfg;
#endif //SL_RAIL_UTIL_COEX_REQ_PORT

#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
extern COEX_HAL_GpioConfig_t sli_coex_ptaPwmReqCfg;
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT

#ifdef SL_RAIL_UTIL_COEX_RHO_PORT
extern COEX_HAL_GpioConfig_t sli_coex_rhoCfg;
#endif //SL_RAIL_UTIL_COEX_RHO_PORT

#ifdef SL_RAIL_UTIL_COEX_WIFI_TX_PORT
extern COEX_HAL_GpioConfig_t sli_coex_wifiTxCfg;
#endif //SL_RAIL_UTIL_COEX_WIFI_TX_PORT

#if defined(HAL_COEX_OVERRIDE_GPIO_INPUT) && !defined(SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT)
#define SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT HAL_COEX_OVERRIDE_GPIO_INPUT
#endif //defined(HAL_COEX_OVERRIDE_GPIO_INPUT) and !defined(SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT)
#if defined(HAL_COEX_DP_TIMER) && !defined(SL_RAIL_UTIL_COEX_DP_TIMER)
#define SL_RAIL_UTIL_COEX_DP_TIMER HAL_COEX_DP_TIMER
#endif //defined(HAL_COEX_DP_TIMER) and !defined(SL_RAIL_UTIL_COEX_DP_TIMER)
#if defined(HAL_COEX_PWM_PRIORITY) && !defined(SL_RAIL_UTIL_COEX_PWM_PRIORITY)
#define SL_RAIL_UTIL_COEX_PWM_PRIORITY HAL_COEX_PWM_PRIORITY
#endif //defined(HAL_COEX_PWM_PRIORITY) and !defined(SL_RAIL_UTIL_COEX_PWM_PRIORITY)
#if defined(HAL_COEX_RETRYRX_ENABLE) && !defined(SL_RAIL_UTIL_COEX_RETRYRX_ENABLE)
#define SL_RAIL_UTIL_COEX_RETRYRX_ENABLE HAL_COEX_RETRYRX_ENABLE
#endif //defined(HAL_COEX_RETRYRX_ENABLE) and !defined(SL_RAIL_UTIL_COEX_RETRYRX_ENABLE)
#if defined(HAL_COEX_MAC_FAIL_THRESHOLD) && !defined(SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD)
#define SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD HAL_COEX_MAC_FAIL_THRESHOLD
#endif //defined(HAL_COEX_MAC_FAIL_THRESHOLD) and !defined(SL_RAIL_UTIL_COEX_MAC_FAIL_THRESHOLD)
#if defined(HAL_COEX_REQ_WINDOW) && !defined(SL_RAIL_UTIL_COEX_REQ_WINDOW)
#define SL_RAIL_UTIL_COEX_REQ_WINDOW HAL_COEX_REQ_WINDOW
#endif //defined(HAL_COEX_REQ_WINDOW) and !defined(SL_RAIL_UTIL_COEX_REQ_WINDOW)
#if defined(HAL_COEX_PWM_DEFAULT_ENABLED) && !defined(SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED)
#define SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED HAL_COEX_PWM_DEFAULT_ENABLED
#endif //defined(HAL_COEX_PWM_DEFAULT_ENABLED) and !defined(SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED)
#if defined(HAL_COEX_TX_ABORT) && !defined(SL_RAIL_UTIL_COEX_IEEE802154_TX_ABORT)
#define SL_RAIL_UTIL_COEX_IEEE802154_TX_ABORT HAL_COEX_TX_ABORT
#endif //defined(HAL_COEX_TX_ABORT) and !defined(SL_RAIL_UTIL_COEX_TX_ABORT)
#if defined(BSP_COEX_RHO_ASSERT_LEVEL) && !defined(SL_RAIL_UTIL_COEX_RHO_ASSERT_LEVEL)
#define SL_RAIL_UTIL_COEX_RHO_ASSERT_LEVEL BSP_COEX_RHO_ASSERT_LEVEL
#endif //defined(BSP_COEX_RHO_ASSERT_LEVEL) and !defined(SL_RAIL_UTIL_COEX_RHO_ASSERT_LEVEL)
#if defined(HAL_COEX_REQ_SHARED) && !defined(SL_RAIL_UTIL_COEX_REQ_SHARED)
#define SL_RAIL_UTIL_COEX_REQ_SHARED HAL_COEX_REQ_SHARED
#endif //defined(HAL_COEX_REQ_SHARED) and !defined(SL_RAIL_UTIL_COEX_REQ_SHARED)
#if defined(HAL_COEX_DP_PULSE_WIDTH_US) && !defined(SL_RAIL_UTIL_COEX_DP_PULSE_WIDTH_US)
#define SL_RAIL_UTIL_COEX_DP_PULSE_WIDTH_US HAL_COEX_DP_PULSE_WIDTH_US
#endif //defined(HAL_COEX_DP_PULSE_WIDTH_US) and !defined(SL_RAIL_UTIL_COEX_DP_PULSE_WIDTH_US)
#if defined(BSP_COEX_PRI_ASSERT_LEVEL) && !defined(SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL)
#define SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL BSP_COEX_PRI_ASSERT_LEVEL
#endif //defined(BSP_COEX_PRI_ASSERT_LEVEL) and !defined(SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL)
#if defined(HAL_COEX_TX_HIPRI) && !defined(SL_RAIL_UTIL_COEX_TX_HIPRI)
#define SL_RAIL_UTIL_COEX_TX_HIPRI HAL_COEX_TX_HIPRI
#endif //defined(HAL_COEX_TX_HIPRI) and !defined(SL_RAIL_UTIL_COEX_TX_HIPRI)
#if defined(BSP_COEX_REQ_ASSERT_LEVEL) && !defined(SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL)
#define SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL BSP_COEX_REQ_ASSERT_LEVEL
#endif //defined(BSP_COEX_REQ_ASSERT_LEVEL) and !defined(SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL)
#if defined(HAL_COEX_RX_HIPRI) && !defined(SL_RAIL_UTIL_COEX_RX_HIPRI)
#define SL_RAIL_UTIL_COEX_RX_HIPRI HAL_COEX_RX_HIPRI
#endif //defined(HAL_COEX_RX_HIPRI) and !defined(SL_RAIL_UTIL_COEX_RX_HIPRI)
#if defined(BSP_COEX_GNT_ASSERT_LEVEL) && !defined(SL_RAIL_UTIL_COEX_GNT_ASSERT_LEVEL)
#define SL_RAIL_UTIL_COEX_GNT_ASSERT_LEVEL BSP_COEX_GNT_ASSERT_LEVEL
#endif //defined(BSP_COEX_GNT_ASSERT_LEVEL) and !defined(SL_RAIL_UTIL_COEX_GNT_ASSERT_LEVEL)
#if defined(HAL_COEX_CCA_THRESHOLD) && !defined(SL_RAIL_UTIL_COEX_CCA_THRESHOLD)
#define SL_RAIL_UTIL_COEX_CCA_THRESHOLD HAL_COEX_CCA_THRESHOLD
#endif //defined(HAL_COEX_CCA_THRESHOLD) and !defined(SL_RAIL_UTIL_COEX_CCA_THRESHOLD)
#if defined(HAL_COEX_PWM_REQ_DUTYCYCLE) && !defined(SL_RAIL_UTIL_COEX_PWM_REQ_DUTYCYCLE)
#define SL_RAIL_UTIL_COEX_PWM_REQ_DUTYCYCLE HAL_COEX_PWM_REQ_DUTYCYCLE
#endif //defined(HAL_COEX_PWM_REQ_DUTYCYCLE) and !defined(SL_RAIL_UTIL_COEX_PWM_REQ_DUTYCYCLE)
#if defined(HAL_COEX_RETRYRX_HIPRI) && !defined(SL_RAIL_UTIL_COEX_RETRYRX_HIPRI)
#define SL_RAIL_UTIL_COEX_RETRYRX_HIPRI HAL_COEX_RETRYRX_HIPRI
#endif //defined(HAL_COEX_RETRYRX_HIPRI) and !defined(SL_RAIL_UTIL_COEX_RETRYRX_HIPRI)
#if defined(HAL_COEX_RETRYRX_TIMEOUT) && !defined(SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT)
#define SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT HAL_COEX_RETRYRX_TIMEOUT
#endif //defined(HAL_COEX_RETRYRX_TIMEOUT) and !defined(SL_RAIL_UTIL_COEX_RETRYRX_TIMEOUT)
#if defined(HAL_COEX_REQ_BACKOFF) && !defined(SL_RAIL_UTIL_COEX_REQ_BACKOFF)
#define SL_RAIL_UTIL_COEX_REQ_BACKOFF HAL_COEX_REQ_BACKOFF
#endif //defined(HAL_COEX_REQ_BACKOFF) and !defined(SL_RAIL_UTIL_COEX_REQ_BACKOFF)
#if defined(HAL_COEX_PRI_SHARED) && !defined(SL_RAIL_UTIL_COEX_PRI_SHARED)
#define SL_RAIL_UTIL_COEX_PRI_SHARED HAL_COEX_PRI_SHARED
#endif //defined(HAL_COEX_PRI_SHARED) and !defined(SL_RAIL_UTIL_COEX_PRI_SHARED)
#if defined(HAL_COEX_DP_ENABLED) && !defined(SL_RAIL_UTIL_COEX_DP_ENABLED)
#define SL_RAIL_UTIL_COEX_DP_ENABLED HAL_COEX_DP_ENABLED
#endif //defined(HAL_COEX_DP_ENABLED) and !defined(SL_RAIL_UTIL_COEX_DP_ENABLED)
#if defined(HAL_COEX_PRIORITY_ESCALATION_ENABLE) && !defined(SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE)
#define SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE HAL_COEX_PRIORITY_ESCALATION_ENABLE
#endif //defined(HAL_COEX_PRIORITY_ESCALATION_ENABLE) and !defined(SL_RAIL_UTIL_COEX_PRIORITY_ESCALATION_ENABLE)
#if defined(HAL_COEX_ACKHOLDOFF) && !defined(SL_RAIL_UTIL_COEX_ACKHOLDOFF)
#define SL_RAIL_UTIL_COEX_ACKHOLDOFF HAL_COEX_ACKHOLDOFF
#endif //defined(HAL_COEX_ACKHOLDOFF) and !defined(SL_RAIL_UTIL_COEX_ACKHOLDOFF)
#if defined(BSP_COEX_PWM_REQ_PIN) && !defined(SL_RAIL_UTIL_COEX_PWM_REQ_PIN)
#define SL_RAIL_UTIL_COEX_PWM_REQ_PIN BSP_COEX_PWM_REQ_PIN
#endif //defined(BSP_COEX_PWM_REQ_PIN) and !defined(SL_RAIL_UTIL_COEX_PWM_REQ_PIN)
#if defined(BSP_COEX_PWM_REQ_PORT) && !defined(SL_RAIL_UTIL_COEX_PWM_REQ_PORT)
#define SL_RAIL_UTIL_COEX_PWM_REQ_PORT BSP_COEX_PWM_REQ_PORT
#endif //defined(BSP_COEX_PWM_REQ_PORT) and !defined(SL_RAIL_UTIL_COEX_PWM_REQ_PORT)
#if defined(HAL_COEX_PWM_REQ_PERIOD) && !defined(SL_RAIL_UTIL_COEX_PWM_REQ_PERIOD)
#define SL_RAIL_UTIL_COEX_PWM_REQ_PERIOD HAL_COEX_PWM_REQ_PERIOD
#endif //defined(HAL_COEX_PWM_REQ_PERIOD) and !defined(SL_RAIL_UTIL_COEX_PWM_REQ_PERIOD)
#if defined(HAL_COEX_PHY_ENABLED) && !defined(SL_RAIL_UTIL_COEX_PHY_ENABLED)
#define SL_RAIL_UTIL_COEX_PHY_ENABLED HAL_COEX_PHY_ENABLED
#endif //defined(HAL_COEX_PHY_ENABLED) and !defined(SL_RAIL_UTIL_COEX_PHY_ENABLED)
#if defined(BSP_COEX_PWM_REQ_ASSERT_LEVEL) && !defined(SL_RAIL_UTIL_COEX_PWM_REQ_ASSERT_LEVEL)
#define SL_RAIL_UTIL_COEX_PWM_REQ_ASSERT_LEVEL BSP_COEX_PWM_REQ_ASSERT_LEVEL
#endif //defined(BSP_COEX_PWM_REQ_ASSERT_LEVEL) and !defined(SL_RAIL_UTIL_COEX_PWM_REQ_ASSERT_LEVEL)
#if defined(HAL_COEX_RUNTIME_PHY_SELECT) && !defined(SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT)
#define SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT HAL_COEX_RUNTIME_PHY_SELECT
#endif //defined(HAL_COEX_RUNTIME_PHY_SELECT) and !defined(SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT)
#if defined(BSP_COEX_RX_ACTIVE_PIN) && !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_PIN)
#define SL_RAIL_UTIL_COEX_RX_ACTIVE_PIN BSP_COEX_RX_ACTIVE_PIN
#endif //defined(BSP_COEX_RX_ACTIVE_PIN) and !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_PIN)
#if defined(BSP_COEX_RX_ACTIVE_PORT) && !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT)
#define SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT BSP_COEX_RX_ACTIVE_PORT
#endif //defined(BSP_COEX_RX_ACTIVE_PORT) and !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT)
#if defined(BSP_COEX_RX_ACTIVE_ASSERT_LEVEL) && !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_ASSERT_LEVEL)
#define SL_RAIL_UTIL_COEX_RX_ACTIVE_ASSERT_LEVEL BSP_COEX_RX_ACTIVE_ASSERT_LEVEL
#endif //defined(BSP_COEX_RX_ACTIVE_ASSERT_LEVEL) and !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_ASSERT_LEVEL)
#if defined(BSP_COEX_RX_ACTIVE_CHANNEL) && !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL)
#define SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL BSP_COEX_RX_ACTIVE_CHANNEL
#endif //defined(BSP_COEX_RX_ACTIVE_CHANNEL) and !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL)
#if defined(BSP_COEX_RX_ACTIVE_LOC) && !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_LOC)
#define SL_RAIL_UTIL_COEX_RX_ACTIVE_LOC BSP_COEX_RX_ACTIVE_LOC
#endif //defined(BSP_COEX_RX_ACTIVE_LOC) and !defined(SL_RAIL_UTIL_COEX_RX_ACTIVE_LOC)
#if defined(BSP_COEX_PHY_SELECT_PIN) && !defined(SL_RAIL_UTIL_COEX_PHY_SELECT_PIN)
#define SL_RAIL_UTIL_COEX_PHY_SELECT_PIN BSP_COEX_PHY_SELECT_PIN
#endif //defined(BSP_COEX_PHY_SELECT_PIN) and !defined(SL_RAIL_UTIL_COEX_PHY_SELECT_PIN)
#if defined(BSP_COEX_PHY_SELECT_PORT) && !defined(SL_RAIL_UTIL_COEX_PHY_SELECT_PORT)
#define SL_RAIL_UTIL_COEX_PHY_SELECT_PORT BSP_COEX_PHY_SELECT_PORT
#endif //defined(BSP_COEX_PHY_SELECT_PORT) and !defined(SL_RAIL_UTIL_COEX_PHY_SELECT_PORT)
#if defined(BSP_COEX_PHY_SELECT_ASSERT_LEVEL) && !defined(SL_RAIL_UTIL_COEX_PHY_SELECT_ASSERT_LEVEL)
#define SL_RAIL_UTIL_COEX_PHY_SELECT_ASSERT_LEVEL BSP_COEX_PHY_SELECT_ASSERT_LEVEL
#endif //defined(BSP_COEX_PHY_SELECT_ASSERT_LEVEL) and !defined(SL_RAIL_UTIL_COEX_PHY_SELECT_ASSERT_LEVEL)
#if defined(HAL_COEX_DEFAULT_PHY_SELECT_TIMEOUT) && !defined(SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT)
#define SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT HAL_COEX_DEFAULT_PHY_SELECT_TIMEOUT
#endif //defined(HAL_COEX_DEFAULT_PHY_SELECT_TIMEOUT) and !defined(SL_RAIL_UTIL_COEX_DEFAULT_PHY_SELECT_TIMEOUT)
#if defined(BSP_COEX_GNT_PIN) && !defined(SL_RAIL_UTIL_COEX_GNT_PIN)
#define SL_RAIL_UTIL_COEX_GNT_PIN BSP_COEX_GNT_PIN
#endif //defined(BSP_COEX_GNT_PIN) and !defined(SL_RAIL_UTIL_COEX_GNT_PIN)
#if defined(BSP_COEX_GNT_PORT) && !defined(SL_RAIL_UTIL_COEX_GNT_PORT)
#define SL_RAIL_UTIL_COEX_GNT_PORT BSP_COEX_GNT_PORT
#endif //defined(BSP_COEX_GNT_PORT) and !defined(SL_RAIL_UTIL_COEX_GNT_PORT)
#if defined(BSP_COEX_PRI_PIN) && !defined(SL_RAIL_UTIL_COEX_PRI_PIN)
#define SL_RAIL_UTIL_COEX_PRI_PIN BSP_COEX_PRI_PIN
#endif //defined(BSP_COEX_PRI_PIN) and !defined(SL_RAIL_UTIL_COEX_PRI_PIN)
#if defined(BSP_COEX_PRI_PORT) && !defined(SL_RAIL_UTIL_COEX_PRI_PORT)
#define SL_RAIL_UTIL_COEX_PRI_PORT BSP_COEX_PRI_PORT
#endif //defined(BSP_COEX_PRI_PORT) and !defined(SL_RAIL_UTIL_COEX_PRI_PORT)
#if defined(BSP_COEX_DP_PIN) && !defined(SL_RAIL_UTIL_COEX_DP_OUT_PIN)
#define SL_RAIL_UTIL_COEX_DP_OUT_PIN BSP_COEX_DP_PIN
#endif //defined(BSP_COEX_DP_PIN) and !defined(SL_RAIL_UTIL_COEX_DP_OUT_PIN)
#if defined(BSP_COEX_DP_PORT) && !defined(SL_RAIL_UTIL_COEX_DP_OUT_PORT)
#define SL_RAIL_UTIL_COEX_DP_OUT_PORT BSP_COEX_DP_PORT
#endif //defined(BSP_COEX_DP_PORT) and !defined(SL_RAIL_UTIL_COEX_DP_OUT_PORT)
#if defined(BSP_COEX_DP_LOC) && !defined(SL_RAIL_UTIL_COEX_DP_OUT_LOC)
#define SL_RAIL_UTIL_COEX_DP_OUT_LOC BSP_COEX_DP_LOC
#endif //defined(BSP_COEX_DP_LOC) and !defined(SL_RAIL_UTIL_COEX_DP_OUT_LOC)
#if defined(BSP_COEX_REQ_PIN) && !defined(SL_RAIL_UTIL_COEX_REQ_PIN)
#define SL_RAIL_UTIL_COEX_REQ_PIN BSP_COEX_REQ_PIN
#endif //defined(BSP_COEX_REQ_PIN) and !defined(SL_RAIL_UTIL_COEX_REQ_PIN)
#if defined(BSP_COEX_REQ_PORT) && !defined(SL_RAIL_UTIL_COEX_REQ_PORT)
#define SL_RAIL_UTIL_COEX_REQ_PORT BSP_COEX_REQ_PORT
#endif //defined(BSP_COEX_REQ_PORT) and !defined(SL_RAIL_UTIL_COEX_REQ_PORT)
#if defined(BSP_COEX_DP_REQUEST_INV_CHANNEL) && !defined(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL)
#define SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL BSP_COEX_DP_REQUEST_INV_CHANNEL
#endif //defined(BSP_COEX_DP_REQUEST_INV_CHANNEL) and !defined(SL_RAIL_UTIL_COEX_DP_REQUEST_INV_CHANNEL)
#if defined(BSP_COEX_DP_CHANNEL) && !defined(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL)
#define SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL BSP_COEX_DP_CHANNEL
#endif //defined(BSP_COEX_DP_CHANNEL) and !defined(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL)
#if defined(BSP_COEX_DP_CC0_PIN) && !defined(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN)
#define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN BSP_COEX_DP_CC0_PIN
#endif //defined(BSP_COEX_DP_CC0_PIN) and !defined(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PIN)
#if defined(BSP_COEX_DP_CC0_PORT) && !defined(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT)
#define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT BSP_COEX_DP_CC0_PORT
#endif //defined(BSP_COEX_DP_CC0_PORT) and !defined(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_PORT)
#if defined(BSP_COEX_DP_CC0_LOC) && !defined(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_LOC)
#define SL_RAIL_UTIL_COEX_DP_TIMER_CC0_LOC BSP_COEX_DP_CC0_LOC
#endif //defined(BSP_COEX_DP_CC0_LOC) and !defined(SL_RAIL_UTIL_COEX_DP_TIMER_CC0_LOC)

#define WRAP_PRS_ASYNC(ch) (((ch) + PRS_ASYNC_CHAN_COUNT) % PRS_ASYNC_CHAN_COUNT)

#if defined(PER_REG_BLOCK_SET_OFFSET)
#define COEX_HAL_GPIO_ADDR(port, polarity) ((uint32_t)&GPIO->P[port].DOUT \
                                            + (polarity                   \
                                               ? PER_REG_BLOCK_SET_OFFSET \
                                               : PER_REG_BLOCK_CLR_OFFSET))

#elif defined(PER_BITSET_MEM_BASE)
#define COEX_HAL_GPIO_ADDR(port, polarity) ((uint32_t)&GPIO->P[port].DOUT           \
                                            - PER_MEM_BASE + (polarity              \
                                                              ? PER_BITSET_MEM_BASE \
                                                              : PER_BITCLR_MEM_BASE))

#endif //defined(PER_REG_BLOCK_SET_OFFSET) || defined(PER_BITSET_MEM_BASE)

#if defined(SL_RAIL_UTIL_COEX_REQ_PORT) && defined(COEX_HAL_GPIO_ADDR)
#ifdef SL_CATALOG_RAIL_UTIL_COEX_WIFI_SIMULATOR_PRESENT
#define COEX_HAL_FAST_REQUEST 0
#else
#define COEX_HAL_FAST_REQUEST 1
#endif
#define COEX_HAL_ReadGpio(port,                  \
                          pin,                   \
                          polarity)              \
  (!!GPIO_PinInGet((GPIO_Port_TypeDef)port, pin) \
   == !!polarity)

#define COEX_HAL_SetGpio(port,                                              \
                         pin,                                               \
                         polarity)                                          \
  do {                                                                      \
    *((volatile uint32_t *)COEX_HAL_GPIO_ADDR(port, polarity)) = 1U << pin; \
  } while (0)

#define COEX_HAL_ReadRequest()                  \
  COEX_HAL_ReadGpio(SL_RAIL_UTIL_COEX_REQ_PORT, \
                    SL_RAIL_UTIL_COEX_REQ_PIN,  \
                    SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL)
#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#define COEX_HAL_SetPwmRequest()                   \
  COEX_HAL_SetGpio(SL_RAIL_UTIL_COEX_PWM_REQ_PORT, \
                   SL_RAIL_UTIL_COEX_PWM_REQ_PIN,  \
                   SL_RAIL_UTIL_COEX_PWM_REQ_ASSERT_LEVEL)
#define COEX_HAL_ClearPwmRequest()                 \
  COEX_HAL_SetGpio(SL_RAIL_UTIL_COEX_PWM_REQ_PORT, \
                   SL_RAIL_UTIL_COEX_PWM_REQ_PIN,  \
                   !SL_RAIL_UTIL_COEX_PWM_REQ_ASSERT_LEVEL)
#else //!SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#define COEX_HAL_SetPwmRequest() //no-op
#define COEX_HAL_ClearPwmRequest() //no-op
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT

#define COEX_HAL_SetRequest()                  \
  COEX_HAL_SetGpio(SL_RAIL_UTIL_COEX_REQ_PORT, \
                   SL_RAIL_UTIL_COEX_REQ_PIN,  \
                   SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL)
#define COEX_HAL_ClearRequest()                \
  COEX_HAL_SetGpio(SL_RAIL_UTIL_COEX_REQ_PORT, \
                   SL_RAIL_UTIL_COEX_REQ_PIN,  \
                   !SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL)
#if defined(SL_RAIL_UTIL_COEX_PRI_PORT)
#define COEX_HAL_SetPriority()                 \
  COEX_HAL_SetGpio(SL_RAIL_UTIL_COEX_PRI_PORT, \
                   SL_RAIL_UTIL_COEX_PRI_PIN,  \
                   SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL)
#define COEX_HAL_ClearPriority()               \
  COEX_HAL_SetGpio(SL_RAIL_UTIL_COEX_PRI_PORT, \
                   SL_RAIL_UTIL_COEX_PRI_PIN,  \
                   !SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL)
#elif defined(SL_RAIL_UTIL_COEX_DP_OUT_PORT) //!defined(SL_RAIL_UTIL_COEX_PRI_PORT)
#ifdef _SILICON_LABS_32B_SERIES_1
#define COEX_HAL_DP_PRS_ON  (PRS_CH_CTRL_ORPREV | PRS_CH_CTRL_INV)
#define COEX_HAL_DP_PRS_OFF (PRS_CH_CTRL_INV)
#define COEX_HAL_CONFIG_DP_PRS(ch, ctrl)                                \
  do {                                                                  \
    PRS->CH[ch].CTRL = (PRS->CH[ch].CTRL & ~COEX_HAL_DP_PRS_ON) | ctrl; \
  } while (0)
#define COEX_HAL_GET_DP_PRS(ch) ((PRS->CH[ch].CTRL & PRS_CH_CTRL_ORPREV) == PRS_CH_CTRL_ORPREV)
#else //!_SILICON_LABS_32B_SERIES_1
#define COEX_HAL_DP_PRS_ON  (prsLogic_A_NOR_B)
#define COEX_HAL_DP_PRS_OFF (prsLogic_NOT_A)
#define COEX_HAL_CONFIG_DP_PRS(ch, logic)                                              \
  do {                                                                                 \
    PRS->ASYNC_CH[ch].CTRL = (PRS->ASYNC_CH[ch].CTRL & ~_PRS_ASYNC_CH_CTRL_FNSEL_MASK) \
                             | ((uint32_t)logic << _PRS_ASYNC_CH_CTRL_FNSEL_SHIFT);    \
  } while (0)
#define COEX_HAL_GET_DP_PRS(ch) ((PRS->ASYNC_CH[ch].CTRL & _PRS_ASYNC_CH_CTRL_FNSEL_MASK) == COEX_HAL_DP_PRS_ON)
#endif //_SILICON_LABS_32B_SERIES_1
#define COEX_HAL_SetPriority() COEX_HAL_CONFIG_DP_PRS(WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1), COEX_HAL_DP_PRS_ON)
#define COEX_HAL_ClearPriority() COEX_HAL_CONFIG_DP_PRS(WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1), COEX_HAL_DP_PRS_OFF)
#define COEX_HAL_GetPriority() COEX_HAL_GET_DP_PRS(WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_DP_OUT_CHANNEL - 1))
#else //!(defined(SL_RAIL_UTIL_COEX_PRI_PORT))
#define COEX_HAL_SetPriority()   //no-op
#define COEX_HAL_ClearPriority() //no-op
#endif //defined(SL_RAIL_UTIL_COEX_PRI_PORT)
#endif //defined(SL_RAIL_UTIL_COEX_REQ_PORT) && defined(COEX_HAL_GPIO_ADDR)

#ifdef __cplusplus
}
#endif

#endif  // __COEXISTENCE_HAL_H__
