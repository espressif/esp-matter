/***************************************************************************//**
 * @file
 * @brief Radio coexistence EM3XX utilities
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

#include "coexistence-hal.h"

#ifndef COEX_HAL_DISABLED
#if COEX_HAL_SMALL_RHO
#ifdef  WAKE_ON_DFL_RHO_VAR // Only define this if needed per board header
uint8_t WAKE_ON_DFL_RHO_VAR = WAKE_ON_DFL_RHO;
#endif//WAKE_ON_DFL_RHO_VAR

extern void emRadioHoldOffIsr(boolean active);

#define RHO_ENABLED_MASK  0x01u // RHO is enabled
#define RHO_RADIO_ON_MASK 0x02u // Radio is on (not sleeping)
static uint8_t rhoState;

void COEX_HAL_Init(void)
{
  //stub
}

boolean halGetRadioHoldOff(void)
{
  return (!!(rhoState & RHO_ENABLED_MASK));
}

// Return active state of Radio HoldOff GPIO pin
static boolean halInternalRhoPinIsActive(void)
{
  return (!!(RHO_IN & BIT(RHO_GPIO & 7)) == !!RHO_ASSERTED);
}

void RHO_ISR(void)
{
  if (rhoState & RHO_ENABLED_MASK) {
    // Ack interrupt before reading GPIO to avoid potential of missing int
    INT_MISS = RHO_MISS_BIT;
    INT_GPIOFLAG = RHO_FLAG_BIT; // acknowledge the interrupt
    // Notify Radio land of state change
    emRadioHoldOffIsr(halInternalRhoPinIsActive());
  } else {
   #ifdef  RHO_ISR_FOR_DFL
    // Defer to default GPIO config's ISR
    extern void RHO_ISR_FOR_DFL(void);
    RHO_ISR_FOR_DFL(); // This ISR is expected to acknowledge the interrupt
   #else//!RHO_ISR_FOR_DFL
    INT_GPIOFLAG = RHO_FLAG_BIT; // acknowledge the interrupt
   #endif//RHO_ISR_FOR_DFL
  }
}

EmberStatus halSetRadioHoldOff(boolean enabled)
{
  // If enabling afresh or disabling after having been enabled
  // restart from a fresh state just in case.
  // Otherwise don't touch a setup that might already have been
  // put into place by the default 'DFL' use (e.g. a button).
  // When disabling after having been enabled, it is up to the
  // board header caller to reinit the default 'DFL' use if needed.
  if (enabled || (rhoState & RHO_ENABLED_MASK)) {
    RHO_INTCFG = 0;              //disable RHO triggering
    INT_CFGCLR = RHO_INT_EN_BIT; //clear RHO top level int enable
    INT_GPIOFLAG = RHO_FLAG_BIT; //clear stale RHO interrupt
    INT_MISS = RHO_MISS_BIT;     //clear stale missed RHO interrupt
  }

  rhoState = (rhoState & (uint8_t) ~RHO_ENABLED_MASK) | (enabled ? RHO_ENABLED_MASK : 0);

  // Reconfigure GPIOs for desired state
  ADJUST_GPIO_CONFIG_DFL_RHO(enabled);

  if (enabled) {
    // Only update radio if it's on, otherwise defer to when it gets turned on
    if (rhoState & RHO_RADIO_ON_MASK) {
      emRadioHoldOffIsr(halInternalRhoPinIsActive()); //Notify Radio land of current state
      INT_CFGSET = RHO_INT_EN_BIT; //set top level interrupt enable
      // Interrupt on now, ISR will maintain proper state
    }
  } else {
    emRadioHoldOffIsr(FALSE); //Notify Radio land of configured state
    // Leave interrupt state untouched (probably turned off above)
  }

  return EMBER_SUCCESS;
}

void halStackRadioHoldOffPowerDown(void)
{
  rhoState &= ~RHO_RADIO_ON_MASK;
  if (rhoState & RHO_ENABLED_MASK) {
    // When sleeping radio, no need to monitor RHO anymore
    INT_CFGCLR = RHO_INT_EN_BIT; //clear RHO top level int enable
  }
}

void halStackRadioHoldOffPowerUp(void)
{
  rhoState |= RHO_RADIO_ON_MASK;
  if (rhoState & RHO_ENABLED_MASK) {
    // When waking radio, set up initial state and resume monitoring
    INT_CFGCLR = RHO_INT_EN_BIT; //ensure RHO interrupt is off
    RHO_ISR(); // Manually call ISR to assess current state
    INT_CFGSET = RHO_INT_EN_BIT; //enable RHO interrupt
  }
}

void COEX_HAL_CallAtomic(COEX_AtomicCallback_t cb, void *arg)
{
  ATOMIC((*cb)(arg); )
}

#else //!COEX_HAL_SMALL_RHO

#ifdef PTA_GNT_GPIO
static void coexGntSel(void)
{
  PTA_GNT_SEL();
}
static COEX_HAL_GpioConfig_t ptaGntCfg = {
  .gpio = PTA_GNT_GPIO,           // PORTx_PIN(y) (x=A/B/C, y=0-7)
  .mode = PTA_GNT_GPIOCFG,         // GPIOCFG_IN_PUD[ASSERTED] or GPIOCFG_IN
  .polarity = PTA_GNT_ASSERTED,   // 0 if negated logic; 1 if positive logic
  .flagBit = PTA_GNT_FLAG_BIT,    // INT_IRQnFLAG (n=A/B/C/D)
  .intMissBit = PTA_GNT_MISS_BIT, // INT_MISSIRQn (n=A/B/C/D)
  .intCfg = &(PTA_GNT_INTCFG),    // GPIO_INTCFGn (n=A/B/C/D)
  .intEnBit = PTA_GNT_INT_EN_BIT, // INT_IRQn     (n=A/B/C/D)
  .intSel = &coexGntSel           // do { GPIO_IRQnSEL = PTA_GNT_GPIO; } while (0)
};
static void (*gntCallback)(void) = NULL;
bool COEX_HAL_ConfigGrant(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &PTA_GNT_ISR;
  status = COEX_ConfigGrant(gpioConfig);
  if (status) {
    gntCallback = gpioConfig->config.cb;
  }
  return status;
}
void PTA_GNT_ISR(void)
{
  gntCallback();
}
#else //!PTA_GNT_GPIO
bool COEX_HAL_ConfigGrant(COEX_HAL_GpioConfig_t *gpioConfig)
{
  (void)gpioConfig;
  return false;
}
#endif //PTA_GNT_GPIO

#ifdef PTA_REQ_GPIO
static void coexReqSel(void)
{
  PTA_REQ_SEL();
}
static COEX_HAL_GpioConfig_t ptaReqCfg = {
  .gpio = PTA_REQ_GPIO,           // PORTx_PIN(y) (x=A/B/C, y=0-7)
  .mode = PTA_REQ_GPIOCFG,         // GPIOCFG_IN_PUD[ASSERTED] or GPIOCFG_IN
  .polarity = PTA_REQ_ASSERTED,   // 0 if negated logic; 1 if positive logic
  .flagBit = PTA_REQ_FLAG_BIT,    // INT_IRQnFLAG (n=A/B/C/D)
  .intMissBit = PTA_REQ_MISS_BIT, // INT_MISSIRQn (n=A/B/C/D)
  .intCfg = &(PTA_REQ_INTCFG),    // GPIO_INTCFGn (n=A/B/C/D)
  .intEnBit = PTA_REQ_INT_EN_BIT, // INT_IRQn     (n=A/B/C/D)
  .intSel = &coexReqSel           // do { GPIO_IRQnSEL = PTA_GNT_GPIO; } while (0)
};
static void (*reqCallback)(void) = NULL;
void PTA_REQ_ISR(void)
{
  reqCallback();
}
bool COEX_HAL_ConfigRequest(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &PTA_REQ_ISR;
  status = COEX_ConfigRequest(gpioConfig);
  if (status) {
    reqCallback = gpioConfig->config.cb;
  }
  return status;
}
static void setGpio(COEX_GpioHandle_t gpioHandle, bool enabled)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;

    if (enabled == gpio->polarity) {
      halGpioSet(gpio->gpio);
    } else {
      halGpioClear(gpio->gpio);
    }
  }
}
static bool isGpioOutSet(COEX_GpioHandle_t gpioHandle, bool defaultValue)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
    return !!halGpioReadOutput(gpio->gpio) == !!gpio->polarity;
  }
  return defaultValue;
}
#else //!PTA_REQ_GPIO
static void setGpio(COEX_GpioHandle_t gpioHandle, bool enabled)
{
  (void)gpioHandle;
  (void)enabled;
}
static bool isGpioOutSet(COEX_GpioHandle_t gpioHandle, bool defaultValue)
{
  (void)gpioHandle;
  return defaultValue;
}
bool COEX_HAL_ConfigRequest(COEX_HAL_GpioConfig_t *gpioConfig)
{
  (void)gpioConfig;
  return false;
}
#endif //PTA_REQ_GPIO

#ifdef PTA_PRI_GPIO
static COEX_HAL_GpioConfig_t ptaPriCfg = {
  .gpio = PTA_PRI_GPIO,           // PORTx_PIN(y) (x=A/B/C, y=0-7)
  .mode = PTA_PRI_GPIOCFG,         // GPIOCFG_IN_PUD[ASSERTED] or GPIOCFG_IN
  .polarity = PTA_PRI_ASSERTED,   // 0 if negated logic; 1 if positive logic
};
#endif //PTA_PRI_GPIO

#ifdef RHO_GPIO
static void (*rhoCallback)(void) = NULL;
static void rhoSel(void)
{
  RHO_SEL();
}
static COEX_HAL_GpioConfig_t rhoCfg = {
  .gpio = RHO_GPIO,               // PORTx_PIN(y) (x=A/B/C, y=0-7)
  .mode = GPIO_P_CFGz_Pxy_IN_PUD, // GPIO_P_CFGz_Pxy_IN_PUD[ASSERTED] or GPIO_P_CFGz_Pxy_IN
  .polarity = RHO_ASSERTED,       // 0 if negated logic; 1 if positive logic
  .flagBit = RHO_FLAG_BIT,        // INT_IRQnFLAG (n=A/B/C/D)
  .intMissBit = RHO_MISS_BIT,     // INT_MISSIRQn (n=A/B/C/D)
  .intCfg = &(RHO_INTCFG),        // GPIO_INTCFGn (n=A/B/C/D)
  .intEnBit = RHO_INT_EN_BIT,     // INT_IRQn     (n=A/B/C/D)
  .intSel = &rhoSel               // do { GPIO_IRQnSEL = PTA_GNT_GPIO; } while (0)
};
// RHO_ISR definition might interfere with other definitions.
void RHO_ISR(void)
{
  rhoCallback();
}
bool COEX_HAL_ConfigRadioHoldOff(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &RHO_ISR;
  status = COEX_ConfigRadioHoldOff(gpioConfig);
  if (status) {
    rhoCallback = gpioConfig->config.cb;
  }
  return status;
}
#else //!RHO_ISR
bool COEX_HAL_ConfigRadioHoldOff(COEX_HAL_GpioConfig_t *gpioConfig)
{
  (void)gpioConfig;
  return false;
}
#endif //RHO_ISR

static void setGpioFlag(COEX_GpioHandle_t gpioHandle, bool enabled)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;

    if (enabled) {
      NVIC_SetPendingIRQ((IRQn_Type)gpio->intEnBit);
    } else {
      EVENT_MISS->MISS = gpio->intMissBit;
      EVENT_GPIO->FLAG = gpio->flagBit;
    }
  }
}

static void setInterruptConfig(COEX_GpioHandle_t gpioHandle)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
    COEX_GpioConfig_t *coexGpio = &(gpio->config);
    uint32_t interruptEdges = EVENT_GPIO_CFGA_FILT_DEFAULT; /* default = no filter  */
    bool intAsserted = (coexGpio->options & COEX_GPIO_OPTION_INT_ASSERTED) != 0U;
    bool intDeasserted = (coexGpio->options & COEX_GPIO_OPTION_INT_DEASSERTED) != 0U;

    if (gpio->polarity ? intAsserted : intDeasserted) {
      interruptEdges |= EVENT_GPIO_CFGx_MOD_RISING_EDGE;
    }
    if (gpio->polarity ? intDeasserted : intAsserted) {
      interruptEdges |= EVENT_GPIO_CFGx_MOD_FALLING_EDGE;
    }
    gpio->interruptEdges |= interruptEdges;
  }
}

static void enableGpioInt(COEX_GpioHandle_t gpioHandle,
                          bool enabled,
                          bool *wasAsserted)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;

    // Disable triggering and clear any stale events
    *(gpio->intCfg) = 0;                  //disable triggering
    NVIC_DisableIRQ((IRQn_Type)gpio->intEnBit);      //clear top level int enable
    NVIC_ClearPendingIRQ((IRQn_Type)gpio->intEnBit); //clear any pended event
    setGpioFlag(gpio, false);
    if (enabled && gpio->isr != NULL) {
      if (wasAsserted != NULL) {
        *wasAsserted = false;
      }
      *(gpio->intCfg) = gpio->interruptEdges;
      gpio->intSel(); //point IRQ to the desired pin
      NVIC_EnableIRQ((IRQn_Type)gpio->intEnBit);  //set top level interrupt enable
    }
  }
}

static void configGpio(COEX_GpioHandle_t gpioHandle, COEX_GpioConfig_t *coexGpio)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
    bool defaultAsserted = (coexGpio->options & COEX_GPIO_OPTION_DEFAULT_ASSERTED) != 0U;
    gpio->config = *coexGpio;

    // Only configure GPIO if it was not set up prior
    if ((coexGpio->options & COEX_GPIO_OPTION_SHARED) != 0U) {
      gpio->mode = GPIO_P_CFGz_Pxy_OUT_OD;
    } else if ((coexGpio->options & COEX_GPIO_OPTION_OUTPUT) != 0U) {
      gpio->mode = GPIO_P_CFGz_Pxy_OUT;
    } else {
      gpio->mode = GPIO_P_CFGz_Pxy_IN_PUD;
    }
    halGpioSetConfig(gpio->gpio, (HalGpioCfg_t)gpio->mode);
    setGpio(gpio, defaultAsserted);
    setInterruptConfig(gpio);
  }
}

static bool isGpioInSet(COEX_GpioHandle_t gpioHandle, bool defaultValue)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
    return !!halGpioRead(gpio->gpio) == !!gpio->polarity;
  }
  return defaultValue;
}

static const COEX_HalCallbacks_t coexHalCallbacks = {
  .setGpio = &setGpio,
  .setGpioFlag = &setGpioFlag,
  .enableGpioInt = &enableGpioInt,
  .configGpio = &configGpio,
  .isGpioOutSet = &isGpioOutSet,
  .isGpioInSet = &isGpioInSet
};

void COEX_HAL_CallAtomic(COEX_AtomicCallback_t cb, void *arg)
{
  ATOMIC((*cb)(arg); )
}

bool COEX_HAL_ConfigPriority(COEX_HAL_GpioConfig_t *gpioConfig)
{
  return COEX_ConfigPriority(gpioConfig);
}

bool COEX_HAL_ConfigPwmRequest(COEX_HAL_GpioConfig_t *gpioConfig)
{
  (void)gpioConfig;
  return false;
}

uint8_t COEX_HAL_GetDpPulseWidth(void)
{
  return 0;
}

bool COEX_HAL_SetDpPulseWidth(uint8_t pulseWidthUs)
{
  return false;
}

void COEX_HAL_Init(void)
{
  COEX_SetHalCallbacks(&coexHalCallbacks);
  COEX_Options_t options = COEX_OPTION_NONE;

  #ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  COEX_HAL_ConfigPwmRequest(&ptaPwmReqCfg);
  #endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  #ifdef RHO_GPIO
  options |= COEX_OPTION_RHO_ENABLED;
  #endif //RHO_GPIO
  #ifdef ENABLE_PTA
  options |= COEX_OPTION_COEX_ENABLED;
  #endif //ENABLE_PTA
  #ifdef PTA_REQ_MAX_BACKOFF_MASK
  options |= (COEX_OPTION_MAX_REQ_BACKOFF_MASK
              & PTA_REQ_MAX_BACKOFF_MASK);
  #endif //PTA_REQ_MAX_BACKOFF_MASK
  #if PTA_REQ_GPIOCFG == GPIOCFG_OUT_OD
  options |= COEX_OPTION_REQ_SHARED;
  #endif //PTA_REQ_GPIOCFG == GPIOCFG_OUT_OD
  #if PTA_PRI_GPIOCFG == GPIOCFG_OUT_OD
  options |= COEX_OPTION_PRI_SHARED;
  #endif //PTA_PRI_GPIOCFG == GPIOCFG_OUT_OD
  #ifdef ENABLE_PTA_OPT_ABORT_TX
  options |= COEX_OPTION_TX_ABORT;
  #endif //ENABLE_PTA_OPT_ABORT_TX
  COEX_SetOptions(options);

  #ifdef PTA_REQ_GPIO
  COEX_HAL_ConfigRequest(&ptaReqCfg);
  #endif //PTA_REQ_GPIO
  #ifdef PTA_PRI_GPIO
  COEX_HAL_ConfigPriority(&ptaPriCfg);
  #endif //PTA_PRI_GPIO
  #ifdef PTA_PWM_REQ_GPIO
  COEX_HAL_ConfigPwmRequest(&ptaPwmReqCfg);
  #endif //PTA_PWM_REQ_GPIO
  #ifdef PTA_GNT_GPIO
  COEX_HAL_ConfigGrant(&ptaGntCfg);
  #endif //PTA_GNT_GPIO
  #ifdef RHO_GPIO
  COEX_HAL_ConfigRadioHoldOff(&rhoCfg);
  #endif //RHO_GPIO
}
#endif //COEX_HAL_SMALL_RHO
#endif //COEX_HAL_DISABLED
