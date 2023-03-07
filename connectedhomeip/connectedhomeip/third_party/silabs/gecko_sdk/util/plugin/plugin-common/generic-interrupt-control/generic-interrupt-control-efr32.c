// *****************************************************************************
// * generic-interrupt-control-efr32.c
// *
// * Provides an interface for a generic interrupt controller.  Users specify
// * The port, pin, and irq number of the interrupt, and this plugin will
// * handle all configuration to enable the interrupt handler, call a user
// * specified interrupt service routine, and activate a user specified event
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL
#include "hal/hal.h"
#include "hal/micro/micro-common.h"

#include "em_gpio.h"
#include "em_core.h"
#include "em_cmu.h"
#include "gpiointerrupt.h"

// ------------------------------------------------------------------------------
// Plugin private macro definitions

// ------------------------------------------------------------------------------
// Forward Declaration of private functions
static void genericIsr(uint8_t pin);

// ------------------------------------------------------------------------------
// private global variables
static HalGenericInterruptControlIrqCfg irqConfigs[16] = { 0 };
static uint32_t em4WuPins = 0;
static uint32_t em4WuPolarities = 0;

// ------------------------------------------------------------------------------
// private plugin functions

// This function will be called whenever the hardware conditions are met to
// trigger an interrupt on a GIC controlled irq.  If the user has defined an
// interrupt subroutine using the @gicIrqISRAssignFxn, that function will be
// called.  If the user has defined an event to be associated with the irq using
// the @gicIRQEventRegister function, that event will be activated and run (in a
// non-interrupt context), regardless of whether an ISR function was also
// assigned to the IRQ.
//
// If no ISR has been assigned to the IRQ, this function will clear the
// interrupt flags and activate the user defined ember event (unless no
// event has been defined in which case no action will be taken).
static void genericIsr(uint8_t pin)
{
  if (irqConfigs[pin].irqISR != NULL) {
    irqConfigs[pin].irqISR();
  }

  // Activate the user specified event
  if (irqConfigs[pin].irqEventHandler != NULL) {
    emberEventControlSetActive(*(irqConfigs[pin].irqEventHandler));
  }
}

// ------------------------------------------------------------------------------
// public plugin functions

HalGenericInterruptControlIrqCfg* halGenericInterruptControlIrqCfgInitialize(
  uint8_t irqPin,
  uint8_t irqPort,
  uint8_t irqNum)
{
  HalGenericInterruptControlIrqCfg *config;
  GPIOINT_IrqCallbackPtr_t isr = &genericIsr;

  config = &(irqConfigs[irqPin]);

  // First, initialize variables based on GPIO port:
  //    irqInReg
  //    irqPin
  //    em4WuPinMask
  config->irqPin = irqPin;
  config->irqPort = (GPIO_Port_TypeDef)irqPort;

  if (irqNum) {
    config->em4WuPinMask = 1 << irqNum;
  } else {
    config->em4WuPinMask = 0;
  }

  // Finally, initialize all default variables:
  //    irqISR (user can define their own ISR if they so desire)
  //    irqEventHandler (user can define event to active on interrupt)
  //    irqEdgeCfg (by default, we trigger on all edges)
  config->irqISR = NULL;
  config->irqEventHandler = NULL;
  config->irqEdgeCfg = HAL_GIC_INT_CFG_EDGE_BOTH;
  config->irqEnabled = false;

  // Now that the config struct is populated, use the information therein to
  // configure the IRQ.  Do not enable yet.
  GPIOINT_CallbackRegister(irqPin, isr);

  // Configure the GPIO Interrupt port, pin, and configuration parameters:
  //   risingEdge, fallingEdge, and enable
  GPIO_ExtIntConfig((GPIO_Port_TypeDef)irqPort, irqPin, irqPin, true, true, false);

  return(config);
}

void halGenericInterruptControlIrqEdgeConfig(
  HalGenericInterruptControlIrqCfg *config,
  uint8_t                          edge)
{
  bool risingEdge = true;
  bool fallingEdge = false;

  config->irqEdgeCfg = edge;

  switch (edge) {
    case HAL_GIC_INT_CFG_EDGE_BOTH:
      risingEdge = true;
      fallingEdge = true;
      break;
    case HAL_GIC_INT_CFG_EDGE_POS:
      risingEdge = true;
      fallingEdge = false;
      break;
    case HAL_GIC_INT_CFG_EDGE_NEG:
      risingEdge = false;
      fallingEdge = true;
      break;
    case HAL_GIC_INT_CFG_LEVEL_POS:
      risingEdge = true;
      fallingEdge = false;
      em4WuPolarities |= config->em4WuPinMask;
      GPIO_EM4EnablePinWakeup(em4WuPins, em4WuPolarities);
      break;
    case HAL_GIC_INT_CFG_LEVEL_NEG:
      risingEdge = false;
      fallingEdge = true;
      em4WuPolarities &= ~(config->em4WuPinMask);
      GPIO_EM4EnablePinWakeup(em4WuPins, em4WuPolarities);
      break;
  }
  GPIO_ExtIntConfig(config->irqPort,
                    config->irqPin,
                    config->irqPin,
                    risingEdge,
                    fallingEdge,
                    config->irqEnabled);
}

void halGenericInterruptControlIrqIsrAssignFxn(
  HalGenericInterruptControlIrqCfg *config,
  void (*isr)(void))
{
  config->irqISR = isr;
}

void halGenericInterruptControlIrqIsrRemoveFxn(
  HalGenericInterruptControlIrqCfg *config)
{
  config->irqISR = NULL;
}

void halGenericInterruptControlIrqEventRegister(
  HalGenericInterruptControlIrqCfg *config,
  EmberEventControl                *event)
{
  config->irqEventHandler = event;
}

void halGenericInterruptControlIrqClear(
  HalGenericInterruptControlIrqCfg *config)
{
  if (config->irqPin % 1) {
    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  } else {
    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  }
}

void halGenericInterruptControlIrqEnable(
  HalGenericInterruptControlIrqCfg *config)
{
  bool risingEdge = true;
  bool fallingEdge = false;

  config->irqEnabled = true;

  switch (config->irqEdgeCfg) {
    case HAL_GIC_INT_CFG_EDGE_BOTH:
      risingEdge = true;
      fallingEdge = true;
      break;
    case HAL_GIC_INT_CFG_EDGE_POS:
      risingEdge = true;
      fallingEdge = false;
      break;
    case HAL_GIC_INT_CFG_EDGE_NEG:
      risingEdge = false;
      fallingEdge = true;
      break;
    case HAL_GIC_INT_CFG_LEVEL_POS:
      risingEdge = true;
      fallingEdge = false;
      break;
    case HAL_GIC_INT_CFG_LEVEL_NEG:
      risingEdge = false;
      fallingEdge = true;
      break;
  }
  if (config->em4WuPinMask) {
    em4WuPins |= config->em4WuPinMask;
    GPIO_EM4EnablePinWakeup(em4WuPins, em4WuPolarities);
  }
  GPIO_ExtIntConfig(config->irqPort,
                    config->irqPin,
                    config->irqPin,
                    risingEdge,
                    fallingEdge,
                    true);
}

void halGenericInterruptControlIrqDisable(
  HalGenericInterruptControlIrqCfg *config)
{
  bool risingEdge = true;
  bool fallingEdge = false;

  config->irqEnabled = false;

  switch (config->irqEdgeCfg) {
    case HAL_GIC_INT_CFG_EDGE_BOTH:
      risingEdge = true;
      fallingEdge = true;
      break;
    case HAL_GIC_INT_CFG_EDGE_POS:
      risingEdge = true;
      fallingEdge = false;
      break;
    case HAL_GIC_INT_CFG_EDGE_NEG:
      risingEdge = false;
      fallingEdge = true;
      break;
  }
  GPIO_ExtIntConfig(config->irqPort,
                    config->irqPin,
                    config->irqPin,
                    risingEdge,
                    fallingEdge,
                    false);
  if (config->em4WuPinMask) {
    em4WuPins &= ~config->em4WuPinMask;
    GPIO_EM4EnablePinWakeup(em4WuPins, em4WuPolarities);
  }
}

uint8_t halGenericInterruptControlIrqReadGpio(
  HalGenericInterruptControlIrqCfg *config)
{
  if (GPIO_PinInGet(config->irqPort,
                    (config->irqPin))) {
    return true;
  }
  return false;
}
