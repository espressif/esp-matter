/**************************************************************************//**
* This simulation device is only meant for internal use in the simulator.
*
* The simlator device is born out of existing code which used a reduced
* EM357 regs.h to model register behavior.
* Simple uint32_t arrays are used as the memory space to
* fake registers in the simulator.
*
* The only registers the simulator actually needs are the GPIO,
* serial controller, and serial controller's events.
*
* This file also takes the place of what would normally be defined as a
* distinct device.  But since such a small set of code
* is needed, all the code is placed in this file.
*
* This is a generated Device header using a subset of the same generation
* proccess that produces an EM357 Device.  The Simulation Device is closest
* to an EM357.
******************************************************************************/

#ifndef __EM_DEVICE_H
#define __EM_DEVICE_H

/**************************************************************************//**
* The following defines should be used for structure members.
* These defines are orignally from platform/CMSIS/Include/core_cm3.h.
* Because the Simulation Device manipulates registers to simulate peripherals,
* registers using "__IM" must be as permisive as possible instead of read only.
* Additionally, "volatile" is not necessary since there are no interrupts in
* the simulator and all behavior happens in sequence.
******************************************************************************/
#define     __IM     /*volatile const*/  /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

#include <stdint.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif

#ifndef   __ALIGNED
#define __ALIGNED(x)    __attribute__((aligned(x)))
#endif
#define __INLINE        inline
#define __STATIC_INLINE static inline
#ifndef __WEAK
#define __WEAK          __attribute__((weak))
#endif
#define __CLZ           count_leading_zeros
#define __RBIT          reverse_bits_in_word
#define __USED          __attribute__((used))
#define __NOP()         asm("NOP")
#define __isb()
#define __ISB()
#define __DMB()
#define __DSB()

typedef enum IRQn{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn   = -14,      /*!<  2 Non Maskable Interrupt                                */
  MemoryManagement_IRQn = -12,      /*!<  4 Cortex-M3 Memory Management Interrupt                 */
  BusFault_IRQn         = -11,      /*!<  5 Cortex-M3 Bus Fault Interrupt                         */
  UsageFault_IRQn       = -10,      /*!<  6 Cortex-M3 Usage Fault Interrupt                       */
  SVCall_IRQn           = -5,       /*!< 11 Cortex-M3 SV Call Interrupt                           */
  DebugMonitor_IRQn     = -4,       /*!< 12 Cortex-M3 Debug Monitor Interrupt                     */
  PendSV_IRQn           = -2,       /*!< 14 Cortex-M3 Pend SV Interrupt                           */
  SysTick_IRQn          = -1,       /*!< 15 Cortex-M3 System Tick Interrupt                       */

/******  EM35x Peripheral Interrupt Numbers ******************************************************/
  TIM1_IRQn             = 0,  /*!< 16+0  EM35x TIM1 Interrupt */
  TIM2_IRQn             = 1,  /*!< 16+1  EM35x TIM2 Interrupt */
  MGMT_IRQn             = 2,  /*!< 16+2  EM35x MGMT Interrupt */
  BB_IRQn               = 3,  /*!< 16+3  EM35x BB Interrupt */
  SLEEPTMR_IRQn         = 4,  /*!< 16+4  EM35x SLEEPTMR Interrupt */
  SC1_IRQn              = 5,  /*!< 16+5  EM35x SC1 Interrupt */
  SC2_IRQn              = 6,  /*!< 16+6  EM35x SC2 Interrupt */
  AESCCM_IRQn           = 7,  /*!< 16+7  EM35x AESCCM Interrupt */
  MACTMR_IRQn           = 8,  /*!< 16+8  EM35x MACTMR Interrupt */
  MACTX_IRQn            = 9,  /*!< 16+9  EM35x MACTX Interrupt */
  MACRX_IRQn            = 10, /*!< 16+10 EM35x MACRX Interrupt */
  ADC_IRQn              = 11, /*!< 16+11 EM35x ADC Interrupt */
  IRQA_IRQn             = 12, /*!< 16+12 EM35x IRQA Interrupt */
  IRQB_IRQn             = 13, /*!< 16+13 EM35x IRQB Interrupt */
  IRQC_IRQn             = 14, /*!< 16+14 EM35x IRQC Interrupt */
  IRQD_IRQn             = 15, /*!< 16+15 EM35x IRQD Interrupt */
  DEBUG_IRQn            = 16, /*!< 16+16 EM35x DEBUG Interrupt */
} IRQn_Type;

__STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  (void)IRQn;
}

__STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn)
{
  (void)IRQn;
}

__STATIC_INLINE int count_leading_zeros(uint32_t  x)
{
  if (x == 0u) {
    return (32u);
  } else {
#ifdef _MSC_VER
    return (int)__lzcnt(x);
#else
    return __builtin_clz(x);
#endif
  }
}

__STATIC_INLINE uint32_t reverse_bits_in_word(uint32_t value)
{
  uint32_t result;
  uint32_t s = (4U /*sizeof(v)*/ * 8U) - 1U; /* extra shift needed at end */

  result = value;                      /* r will be reversed bits of v; first get LSB of v */
  for (value >>= 1U; value != 0U; value >>= 1U)
  {
    result <<= 1U;
    result |= value & 1U;
    s--;
  }
  result <<= s;                        /* shift when v's highest bits are zero */
  return result;
}

#include "simulation_event_sc12.h"
#include "simulation_gpio_p.h"
#include "simulation_gpio.h"
#include "simulation_sc.h"

extern uint32_t simRegsEventSc12[];
extern uint32_t simRegsGpio[];
extern uint32_t simRegsSc1[];

#define EVENT_SC1_BASE  simRegsEventSc12
#define GPIO_BASE       simRegsGpio
#define SC1_BASE        simRegsSc1

#define EVENT_SC1       ((EVENT_SC12_TypeDef *) EVENT_SC1_BASE)
#define GPIO            ((GPIO_TypeDef *) GPIO_BASE)
#define SC1             ((SC_TypeDef *) SC1_BASE)

#endif
