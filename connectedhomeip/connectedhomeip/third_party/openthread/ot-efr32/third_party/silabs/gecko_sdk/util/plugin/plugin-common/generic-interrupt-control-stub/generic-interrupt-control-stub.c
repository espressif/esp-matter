//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL

HalGenericInterruptControlIrqCfg* halGenericInterruptControlIrqCfgInitialize(
  uint8_t irqPin,
  uint8_t irqPort,
  uint8_t irqNum)
{
  return NULL;
}

void halGenericInterruptControlIrqEdgeConfig(
  HalGenericInterruptControlIrqCfg *config,
  uint8_t                          edge)
{
}

void halGenericInterruptControlIrqIsrAssignFxn(
  HalGenericInterruptControlIrqCfg *config,
  void (*isr)(void))
{
}

void halGenericInterruptControlIrqIsrRemoveFxn(
  HalGenericInterruptControlIrqCfg *config)
{
}

void halGenericInterruptControlIrqEventRegister(
  HalGenericInterruptControlIrqCfg *config,
  EmberEventControl                *event)
{
}

void halGenericInterruptControlIrqEnable(
  HalGenericInterruptControlIrqCfg *config)
{
}

void halGenericInterruptControlIrqDisable(
  HalGenericInterruptControlIrqCfg *config)
{
}

uint8_t halGenericInterruptControlIrqReadGpio(
  HalGenericInterruptControlIrqCfg *config)
{
  return 0;
}
