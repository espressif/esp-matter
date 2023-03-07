// *****************************************************************************
// * Generic Interrupt Control EFR32.h
// *
// * This contains the architecture specific definition of the configuration
// * structure used to interface with the generic interrupt control plugin
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#ifndef __GENERIC_INTERRUPT_CONTROL_EFR32_H__
#define __GENERIC_INTERRUPT_CONTROL_EFR32_H__

#include "em_gpio.h"

// ------------------------------------------------------------------------------
// Plugin public struct and enum definitions
typedef struct tIrqCfg{
  uint8_t irqPin;
  GPIO_Port_TypeDef irqPort;
  uint32_t em4WuPinMask;
  uint8_t irqEdgeCfg;
  bool irqEnabled;
  void (*irqISR)(void); // fxn pointer for non-default ISR
  EmberEventControl *irqEventHandler; // ember event for default delayed
                                      // handling
} HalGenericInterruptControlIrqCfg;

#endif // __GENERIC_INTERRUPT_CONTROL_EFR32_H__
