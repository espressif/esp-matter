/***************************************************************************//**
 * @file    iot_pwm_hal.c
 * @brief   Silicon Labs implementation of Common I/O PWM API.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

/* Gecko SDK emlib includes */
#include "em_device.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_cmu.h"

/* Gecko SDK service layer */
#include "sl_power_manager.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "string.h"

/* PWM driver layer */
#include "iot_pwm_desc.h"
#include "iot_pwm_drv.h"

/* PWM interface layer */
#include "iot_pwm.h"

/*******************************************************************************
 *                            get_timer_clock()
 ******************************************************************************/

static CMU_Clock_TypeDef get_timer_clock(TIMER_TypeDef * timer)
{
  CMU_Clock_TypeDef timer_clock = (CMU_Clock_TypeDef) -1;

  switch ((uint32_t) timer) {
#if defined(TIMER0_BASE)
    case TIMER0_BASE:
      timer_clock = cmuClock_TIMER0;
      break;
#endif
#if defined(TIMER1_BASE)
    case TIMER1_BASE:
      timer_clock = cmuClock_TIMER1;
      break;
#endif
#if defined(TIMER2_BASE)
    case TIMER2_BASE:
      timer_clock = cmuClock_TIMER2;
      break;
#endif
#if defined(TIMER3_BASE)
    case TIMER3_BASE:
      timer_clock = cmuClock_TIMER3;
      break;
#endif
#if defined(TIMER4_BASE)
    case TIMER4_BASE:
      timer_clock = cmuClock_TIMER4;
      break;
#endif
#if defined(TIMER5_BASE)
    case TIMER5_BASE:
      timer_clock = cmuClock_TIMER5;
      break;
#endif
#if defined(TIMER6_BASE)
    case TIMER6_BASE:
      timer_clock = cmuClock_TIMER6;
      break;
#endif
#if defined(WTIMER0_BASE)
    case WTIMER0_BASE:
      timer_clock = cmuClock_WTIMER0;
      break;
#endif
#if defined(WTIMER1_BASE)
    case WTIMER1_BASE:
      timer_clock = cmuClock_WTIMER1;
      break;
#endif
#if defined(WTIMER2_BASE)
    case WTIMER2_BASE:
      timer_clock = cmuClock_WTIMER2;
      break;
#endif
#if defined(WTIMER3_BASE)
    case WTIMER3_BASE:
      timer_clock = cmuClock_WTIMER3;
      break;
#endif
    default:
      break;
  }

  return timer_clock;
}

/*******************************************************************************
 *                            initialize_pwm_hw()
 ******************************************************************************/

static void initialize_pwm_hw(IotPwmHandle_t pxPwmHandle)
{
  TIMER_Init_TypeDef timer_init  = TIMER_INIT_DEFAULT;
  CMU_Clock_TypeDef  timer_clock = (CMU_Clock_TypeDef) 0;

  /* retrieve timer clock */
  timer_clock = get_timer_clock(pxPwmHandle->pxPeripheral);

  /* Enable PWM Timer and GPIO clocks */
  CMU_ClockEnable(timer_clock, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize TIMER and put it in a disabled state */
  timer_init.enable = false;
  TIMER_Init(pxPwmHandle->pxPeripheral, &timer_init);
}

/*******************************************************************************
 *                            deinitialize_pwm_hw()
 ******************************************************************************/

static void deinitialize_pwm_hw(IotPwmHandle_t pxPwmHandle)
{
  CMU_Clock_TypeDef  timer_clock = (CMU_Clock_TypeDef) 0;

  /* retrieve timer clock */
  timer_clock = get_timer_clock(pxPwmHandle->pxPeripheral);

  /* Disable PWM clock */
  CMU_ClockEnable(timer_clock, false);
}

/*******************************************************************************
 *                            start_pwm_hw()
 ******************************************************************************/

static void start_pwm_hw(IotPwmHandle_t pxPwmHandle)
{
  TIMER_InitCC_TypeDef channel_init = TIMER_INITCC_DEFAULT;
  CMU_Clock_TypeDef    timer_clock  = (CMU_Clock_TypeDef) 0;
  uint32_t             timer_freq   = 0;
  uint32_t             top_value    = 0;
  uint32_t             cmp_value    = 0;
  GPIO_Port_TypeDef    gpio_port    = (GPIO_Port_TypeDef) 0;
  uint32_t             gpio_pin     = 0;
  uint32_t             gpio_loc     = 0;

  /* Retrieve timer clock */
  timer_clock = get_timer_clock(pxPwmHandle->pxPeripheral);

  /* retrieve timer frequency */
  timer_freq = CMU_ClockFreqGet(timer_clock);

  /* Compute top value to overflow at the desired PWM frequency */
  top_value = (timer_freq / pxPwmHandle->pwm_frequency);

  /* Compute compare value for duty cycle */
  cmp_value = (top_value * pxPwmHandle->dutycycle) / 100;

  /* Set top value register */
  TIMER_TopSet(pxPwmHandle->pxPeripheral, top_value);

  /* Set compare value register */
  TIMER_CompareSet(pxPwmHandle->pxPeripheral, pxPwmHandle->channel, cmp_value);

  /* Enable the configured channel */
  channel_init.mode = timerCCModePWM;
  channel_init.cmoa = timerOutputActionToggle;
  channel_init.edge = timerEdgeBoth;
  channel_init.outInvert = (pxPwmHandle->polarity_low == 1);

  /* Apply initialization value to the channel */
  TIMER_InitCC(pxPwmHandle->pxPeripheral, pxPwmHandle->channel, &channel_init);

  /* Get GPIO port, pin, and loc */
  switch (pxPwmHandle->channel) {
    case 0:
      gpio_port = pxPwmHandle->xCC0Port;
      gpio_pin  = pxPwmHandle->ucCC0Pin;
      gpio_loc  = pxPwmHandle->ucCC0Loc;
      break;
    case 1:
      gpio_port = pxPwmHandle->xCC1Port;
      gpio_pin  = pxPwmHandle->ucCC1Pin;
      gpio_loc  = pxPwmHandle->ucCC1Loc;
      break;
    case 2:
      gpio_port = pxPwmHandle->xCC2Port;
      gpio_pin  = pxPwmHandle->ucCC2Pin;
      gpio_loc  = pxPwmHandle->ucCC2Loc;
      break;
    default:
      break;
  }

  /* Configure GPIO for PWM output */
  GPIO_PinModeSet(gpio_port, gpio_pin, gpioModePushPull, 0);

  /* Configure CC channel pinout */
#if (_SILICON_LABS_32B_SERIES == 0)
  /* SERIES 0 */
  switch (pxPwmHandle->channel) {
    /* channel 0 */
    case 0:
      pxPwmHandle->pxPeripheral->ROUTE |=
        (TIMER_ROUTE_CC0PEN | (gpio_loc << _TIMER_ROUTE_LOCATION_SHIFT));
      break;
    /* channel 1 */
    case 1:
      pxPwmHandle->pxPeripheral->ROUTE |=
        (TIMER_ROUTE_CC1PEN | (gpio_loc << _TIMER_ROUTE_LOCATION_SHIFT));
      break;
    /* channel 2 */
    case 2:
      pxPwmHandle->pxPeripheral->ROUTE |=
        (TIMER_ROUTE_CC2PEN | (gpio_loc << _TIMER_ROUTE_LOCATION_SHIFT));
      break;
    /* invalid channel */
    default:
      break;
  }
#elif (_SILICON_LABS_32B_SERIES == 1)
  /* SERIES 1 */
  switch (pxPwmHandle->channel) {
    /* channel 0 */
    case 0:
      pxPwmHandle->pxPeripheral->ROUTELOC0 |=
        (gpio_loc << _TIMER_ROUTELOC0_CC0LOC_SHIFT);
      pxPwmHandle->pxPeripheral->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
      break;
    /* channel 1 */
    case 1:
      pxPwmHandle->pxPeripheral->ROUTELOC0 |=
        (gpio_loc << _TIMER_ROUTELOC0_CC1LOC_SHIFT);
      pxPwmHandle->pxPeripheral->ROUTEPEN |= TIMER_ROUTEPEN_CC1PEN;
      break;
    /* channel 2 */
    case 2:
      pxPwmHandle->pxPeripheral->ROUTELOC0 |=
        (gpio_loc << _TIMER_ROUTELOC0_CC2LOC_SHIFT);
      pxPwmHandle->pxPeripheral->ROUTEPEN |= TIMER_ROUTEPEN_CC2PEN;
      break;
    /* invalid channel */
    default:
      break;
  }
#elif (_SILICON_LABS_32B_SERIES == 2)
  /* SERIES 2 */
  switch (pxPwmHandle->channel) {
    /* channel 0 */
    case 0:
      GPIO->TIMERROUTE[TIMER_NUM(pxPwmHandle->pxPeripheral)].CC0ROUTE =
        (gpio_port << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT) |
        (gpio_pin  << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT );
      GPIO->TIMERROUTE_SET[TIMER_NUM(pxPwmHandle->pxPeripheral)].ROUTEEN =
        1 << (_GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
      break;
    /* channel 1 */
    case 1:
      GPIO->TIMERROUTE[TIMER_NUM(pxPwmHandle->pxPeripheral)].CC1ROUTE =
        (gpio_port << _GPIO_TIMER_CC1ROUTE_PORT_SHIFT) |
        (gpio_pin  << _GPIO_TIMER_CC1ROUTE_PIN_SHIFT );
      GPIO->TIMERROUTE_SET[TIMER_NUM(pxPwmHandle->pxPeripheral)].ROUTEEN =
        1 << (_GPIO_TIMER_ROUTEEN_CC1PEN_SHIFT);
      break;
    /* channel 2 */
    case 2:
      GPIO->TIMERROUTE[TIMER_NUM(pxPwmHandle->pxPeripheral)].CC2ROUTE =
        (gpio_port << _GPIO_TIMER_CC2ROUTE_PORT_SHIFT) |
        (gpio_pin  << _GPIO_TIMER_CC2ROUTE_PIN_SHIFT );
      GPIO->TIMERROUTE_SET[TIMER_NUM(pxPwmHandle->pxPeripheral)].ROUTEEN =
        1 << (_GPIO_TIMER_ROUTEEN_CC2PEN_SHIFT);
      break;
    /* invalid channel */
    default:
      break;
  }
  /* location is not used in series 2 */
  (void) gpio_loc;
#endif

  /* add EM1 requirement */
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);

  /* Enable TIMER */
  TIMER_Enable(pxPwmHandle->pxPeripheral, true);

  /* update flag */
  pxPwmHandle->is_running = true;
}

/*******************************************************************************
 *                            stop_pwm_output()
 ******************************************************************************/

static void stop_pwm_hw(IotPwmHandle_t pxPwmHandle)
{
  TIMER_InitCC_TypeDef channel_init = TIMER_INITCC_DEFAULT;
  GPIO_Port_TypeDef    gpio_port    = (GPIO_Port_TypeDef) 0;
  uint32_t             gpio_pin     = 0;
  uint32_t             gpio_loc     = 0;

  /* Disable TIMER */
  TIMER_Enable(pxPwmHandle->pxPeripheral, false);

  /* Disable the configured channel */
  channel_init.mode = timerCCModeOff;

  /* Apply initialization value to the channel */
  TIMER_InitCC(pxPwmHandle->pxPeripheral, pxPwmHandle->channel, &channel_init);

  /* Get GPIO port and pin */
  switch (pxPwmHandle->channel) {
    case 0:
      gpio_port = pxPwmHandle->xCC0Port;
      gpio_pin  = pxPwmHandle->ucCC0Pin;
      gpio_loc  = pxPwmHandle->ucCC0Loc;
      break;
    case 1:
      gpio_port = pxPwmHandle->xCC1Port;
      gpio_pin  = pxPwmHandle->ucCC1Pin;
      gpio_loc  = pxPwmHandle->ucCC1Loc;
      break;
    case 2:
      gpio_port = pxPwmHandle->xCC2Port;
      gpio_pin  = pxPwmHandle->ucCC2Pin;
      gpio_loc  = pxPwmHandle->ucCC2Loc;
      break;
    default:
      break;
  }

  /* Unconfigure CC channel pinout */
#if (_SILICON_LABS_32B_SERIES == 0)
  /* SERIES 0 */
  switch (pxPwmHandle->channel) {
    /* channel 0 */
    case 0:
      pxPwmHandle->pxPeripheral->ROUTE &=
        ~(TIMER_ROUTE_CC0PEN | (gpio_loc << _TIMER_ROUTE_LOCATION_SHIFT));
      break;
    /* channel 1 */
    case 1:
      pxPwmHandle->pxPeripheral->ROUTE &=
        ~(TIMER_ROUTE_CC1PEN | (gpio_loc << _TIMER_ROUTE_LOCATION_SHIFT));
      break;
    /* channel 2 */
    case 2:
      pxPwmHandle->pxPeripheral->ROUTE &=
        ~(TIMER_ROUTE_CC2PEN | (gpio_loc << _TIMER_ROUTE_LOCATION_SHIFT));
      break;
    /* invalid channel */
    default:
      break;
  }
#elif (_SILICON_LABS_32B_SERIES == 1)
  /* SERIES 1 */
  switch (pxPwmHandle->channel) {
    /* channel 0 */
    case 0:
      pxPwmHandle->pxPeripheral->ROUTELOC0 &=
        ~(gpio_loc << _TIMER_ROUTELOC0_CC0LOC_SHIFT);
      pxPwmHandle->pxPeripheral->ROUTEPEN &= ~(TIMER_ROUTEPEN_CC0PEN);
      break;
    /* channel 1 */
    case 1:
      pxPwmHandle->pxPeripheral->ROUTELOC0 &=
        ~(gpio_loc << _TIMER_ROUTELOC0_CC1LOC_SHIFT);
      pxPwmHandle->pxPeripheral->ROUTEPEN &= ~(TIMER_ROUTEPEN_CC1PEN);
      break;
    /* channel 2 */
    case 2:
      pxPwmHandle->pxPeripheral->ROUTELOC0 &=
        ~(gpio_loc << _TIMER_ROUTELOC0_CC2LOC_SHIFT);
      pxPwmHandle->pxPeripheral->ROUTEPEN &= ~(TIMER_ROUTEPEN_CC2PEN);
      break;
    /* invalid channel */
    default:
      break;
  }
#elif (_SILICON_LABS_32B_SERIES == 2)
  /* SERIES 2 */
  switch (pxPwmHandle->channel) {
    /* channel 0 */
    case 0:
      GPIO->TIMERROUTE[TIMER_NUM(pxPwmHandle->pxPeripheral)].CC0ROUTE = 0;
      GPIO->TIMERROUTE_CLR[TIMER_NUM(pxPwmHandle->pxPeripheral)].ROUTEEN =
        1 << (_GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
      break;
    /* channel 1 */
    case 1:
      GPIO->TIMERROUTE[TIMER_NUM(pxPwmHandle->pxPeripheral)].CC1ROUTE = 0;
      GPIO->TIMERROUTE_CLR[TIMER_NUM(pxPwmHandle->pxPeripheral)].ROUTEEN =
        1 << (_GPIO_TIMER_ROUTEEN_CC1PEN_SHIFT);
      break;
    /* channel 2 */
    case 2:
      GPIO->TIMERROUTE[TIMER_NUM(pxPwmHandle->pxPeripheral)].CC2ROUTE = 0;
      GPIO->TIMERROUTE_CLR[TIMER_NUM(pxPwmHandle->pxPeripheral)].ROUTEEN =
        1 << (_GPIO_TIMER_ROUTEEN_CC2PEN_SHIFT);
      break;
    /* invalid channel */
    default:
      break;
  }
  /* location is not used in series 2 */
  (void) gpio_loc;
#endif

  /* Reset GPIO */
  GPIO_PinModeSet(gpio_port, gpio_pin, gpioModeDisabled, 0);

  /* remove  EM1 requirement */
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);

  /* update flag */
  pxPwmHandle->is_running = false;
}

/*******************************************************************************
 *                             iot_pwm_open()
 ******************************************************************************/

/**
 * @brief   iot_pwm_open is used to initialize the PWM driver instance.
 *          this API will configure PWM and may reset the PWM hardware.
 *
 * @param[in]   lPwmInstance   The instance of the PWM to initialize.
 *                              PWM is output only.
 *
 * @return
 *   - Handle to PWM interface on success
 *   - NULL if
 *      - invalid instance
 *      - instance already open
 */
IotPwmHandle_t iot_pwm_open(int32_t lPwmInstance)
{
  IotPwmHandle_t pxPwmHandle = NULL;

  /* retrieve pxPwmHandle by instance number */
  pxPwmHandle = iot_pwm_desc_get(lPwmInstance);

  /* validate pxPwmHandle */
  if (pxPwmHandle == NULL) {
    return NULL;
  }

  /* pxPwmHandle shouldn't be open */
  if (pxPwmHandle->is_open) {
    return NULL;
  }

  portENTER_CRITICAL();

  initialize_pwm_hw(pxPwmHandle);
  pxPwmHandle->is_open = true;

  portEXIT_CRITICAL();

  return pxPwmHandle;
}

/*******************************************************************************
 *                            iot_pwm_set_config()
 ******************************************************************************/

/**
 * @brief   iot_pwm_set_config sets up the PWM frequency and duty cycle
 *          to generate the PWM pulses required.
 *
 * @param[in]   pxPwmHandle  Handle to PWM driver returned in
 *                          iot_pwm_open
 * @param[in]   xConfig     PWM configuration to be setup.
 *
 * @return
 *   - IOT_PWM_SUCCESS on success
 *   - IOT_PWM_INVALID_VALUE if pxPwmHandle == NULL or invalid config setting
 */
int32_t iot_pwm_set_config(IotPwmHandle_t const pxPwmHandle,
                           const IotPwmConfig_t xConfig)
{
  uint32_t ulClkFreq;

  /* pxPwmHandle is NULL */
  if (pxPwmHandle == NULL) {
    return IOT_PWM_INVALID_VALUE;
  }

  /* pxPwmHandle is not open */
  if (!pxPwmHandle->is_open) {
    return IOT_PWM_INVALID_VALUE;
  }

  /* invalid configuration in xConfig */
  ulClkFreq = CMU_ClockFreqGet(get_timer_clock(pxPwmHandle->pxPeripheral));
  if (( xConfig.ulPwmFrequency > ulClkFreq) ||
      ( xConfig.ucPwmDutyCycle > 100) ||
      ( xConfig.ucPwmChannel > 2)) {
    return IOT_PWM_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  /* Set PWM configuration */
  pxPwmHandle->pwm_frequency = xConfig.ulPwmFrequency;
  pxPwmHandle->dutycycle = xConfig.ucPwmDutyCycle;
  pxPwmHandle->channel = xConfig.ucPwmChannel;

  /* PWM is configured and PWM is running */
  if (pxPwmHandle->is_running) {
    /* Update PWM frequency and Dutycycle*/
    stop_pwm_hw(pxPwmHandle);
    start_pwm_hw(pxPwmHandle);
  }

  /* update flag */
  pxPwmHandle->is_configured = true;

  portEXIT_CRITICAL();

  return IOT_PWM_SUCCESS;
}

/*******************************************************************************
 *                            iot_pwm_get_config()
 ******************************************************************************/

/**
 * @brief   iot_pwm_get_config returns the current PWM configuration
 *
 * @param[in]   pxPwmHandle  Handle to PWM driver returned in
 *                          iot_pwm_open
 *
 * @return
 *   - pointer to current PWM configuration on success
 *   - NULL if pxPwmHandle == NULL
 */
IotPwmConfig_t * iot_pwm_get_config(IotPwmHandle_t const pxPwmHandle)
{
  /* pxPwmHandle is NULL */
  if (pxPwmHandle == NULL) {
    return NULL;
  }

  /* pxPwmHandle is not open */
  if (!pxPwmHandle->is_open) {
    return NULL;
  }

  portENTER_CRITICAL();

  pxPwmHandle->pwm_config.ucPwmChannel   = pxPwmHandle -> channel;
  pxPwmHandle->pwm_config.ucPwmDutyCycle = pxPwmHandle -> dutycycle;
  pxPwmHandle->pwm_config.ulPwmFrequency = pxPwmHandle -> pwm_frequency;

  portEXIT_CRITICAL();

  return &pxPwmHandle->pwm_config;
}

/*******************************************************************************
 *                             iot_pwm_start()
 ******************************************************************************/

/*!
 * @brief   Start the PWM hardware. PWM configuration must be
 *          set before PWM is started.  PWM signal availability
 *          on the configured output based on the PWMChannel configured
 *          in iot_pwm_set_config().
 *
 * @param[in]   pxPwmHandle  Handle to PWM driver returned in
 *                          iot_pwm_open
 *
 * @return
 *   - IOT_PWM_SUCCESS on success
 *   - IOT_PWM_INVALID_VALUE if pxPwmHandle == NULL
 *   - IOT_PWM_NOT_CONFIGURED if iot_pwm_set_config hasn't been called.
 */
int32_t iot_pwm_start(IotPwmHandle_t const pxPwmHandle)
{
  /* pxPwmHandle is NULL */
  if (pxPwmHandle == NULL) {
    return IOT_PWM_INVALID_VALUE;
  }

  /* pxPwmHandle is not open */
  if (!pxPwmHandle->is_open) {
    return IOT_PWM_INVALID_VALUE;
  }

  /* pxPwmHandle is cofigured */
  if (!pxPwmHandle->is_configured) {
    return IOT_PWM_NOT_CONFIGURED;
  }

  /* pxPwmHandle is running already */
  if (pxPwmHandle->is_running) {
    return IOT_PWM_SUCCESS;
  }

  portENTER_CRITICAL();

  /* Start PWM HW */
  start_pwm_hw(pxPwmHandle);

  portEXIT_CRITICAL();

  return IOT_PWM_SUCCESS;
}

/*******************************************************************************
 *                             iot_pwm_stop()
 ******************************************************************************/

/*!
 * @brief Stop the PWM hardware.
 *
 * @param[in]   pxPwmHandle  Handle to PWM driver returned in
 *                          iot_pwm_open
 *
 * @return
 *   - IOT_PWM_SUCCESS on success
 *   - IOT_PWM_INVALID_VALUE if pxPwmHandle == NULL
 */
int32_t iot_pwm_stop(IotPwmHandle_t const pxPwmHandle)
{
  /* pxPwmHandle is NULL */
  if (pxPwmHandle == NULL) {
    return IOT_PWM_INVALID_VALUE;
  }

  /* pxPwmHandle is not open */
  if (!pxPwmHandle->is_open) {
    return IOT_PWM_INVALID_VALUE;
  }

  /* pxPwmHandle is not running */
  if (!pxPwmHandle->is_running) {
    return IOT_PWM_SUCCESS;
  }

  portENTER_CRITICAL();

  stop_pwm_hw(pxPwmHandle);

  portEXIT_CRITICAL();

  return IOT_PWM_SUCCESS;
}

/*******************************************************************************
 *                             iot_pwm_close()
 ******************************************************************************/

/**
 * @brief iot_pwm_close de-initializes the PWM.
 *
 * @param[in]   pxPwmHandle  Handle to PWM driver returned in
 *                          iot_pwm_open
 *
 * @return
 *   - IOT_PWM_SUCCESS on success
 *   - IOT_PWM_INVALID_VALUE if
 *      - pxPwmHandle == NULL
 *      - not in open state (already closed).
 *
 */
int32_t iot_pwm_close(IotPwmHandle_t  const pxPwmHandle)
{
  /* pxPwmHandle is NULL */
  if (pxPwmHandle == NULL) {
    return IOT_PWM_INVALID_VALUE;
  }

  /* pxPwmHandle is not open */
  if (!pxPwmHandle->is_open) {
    return IOT_PWM_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  if ((pxPwmHandle -> is_running)) {
    stop_pwm_hw(pxPwmHandle);
  }

  /* De-initialize PWM HW */
  deinitialize_pwm_hw(pxPwmHandle);

  /* Mark the descriptor as closed */
  pxPwmHandle->is_open = false;
  pxPwmHandle->is_configured = false;

  portEXIT_CRITICAL();

  return IOT_PWM_SUCCESS;
}
