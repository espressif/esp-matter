/**
 * @file
 * Platform abstraction for Serial API application
 * 
 * @copyright 2022 Silicon Laboratories Inc.
 */
#include <SerialAPI_hw.h>
#include <config_app.h>
#include <zw_region_config.h>
#include <board.h>
//#define DEBUGPRINT
#include "DebugPrint.h"
#include <utils.h>

#include "em_emu.h"
#include "gpiointerrupt.h"

#ifdef USB_SUSPEND_SUPPORT
// Ocelot
#ifdef ZWAVE_SERIES_800
#define EM4WU_USB_SUSPEND                       GPIO_IEN_EM4WUIEN9 // wakeup for UZB stick
#define GPIO_NUMBER_FOR_UZB_SUSPEND             GPIO_EM4WU9_PIN    // in uzb stick suspend signal is connected to PB13 of FG14 chip
#define GPIO_INTERRUPT_PRIORITY                 5
#define GPIO_INTERRUPT_NO                       5   // Interrup No = pin No
#define GPIO_NUMBER_FOR_EMULATED_UZBSUSPEND     5   // in uzb stick suspend signal is emulated for testing on extension board board_8029a - EXP15
#define GPIO_PORT_OF_UZBSUSPEND                 GPIO_EM4WU9_PORT
#define GPIO_PORT_OF_EMULATED_UZBSUSPEND        (gpioPortC)
#else
// 700s
#define EM4WU_USB_SUSPEND                       GPIO_EXTILEVEL_EM4WU9   // port B pin 13 wake up for UZB stick
#define GPIO_NUMBER_FOR_UZB_SUSPEND             13   // in uzb stick suspend signal is connected to PB13 of FG14 chip
#define GPIO_INTERRUPT_PRIORITY                 5
#define GPIO_INTERRUPT_NO                       13   // Interrup No = pin No
#define GPIO_NUMBER_FOR_EMULATED_UZBSUSPEND     10   // in uzb stick suspend signal is emulated for testing on extension board board_8029a - EXP15
#define GPIO_PORT_OF_UZBSUSPEND                 (gpioPortB)
#define GPIO_PORT_OF_EMULATED_UZBSUSPEND        (gpioPortC)
#endif

SerialAPI_hw_usb_suspend_callback_t callback;

static void gpioCallbackuzbsuspend(uint8_t pin);

#endif /*USB_SUSPEND_SUPPORT*/

void SerialAPI_hw_init(void)
{
  /* Unlatch EM4 GPIO pin states after wakeup (OK to call even if not EM4 wakeup) */
  /*If a device waked up from EM4 then all GPIO pins are isolated and we need to connected them to the GPIO registers.*/
  EMU_UnlatchPinRetention();

  /* Initialize the radio board (SAW filter setup, etc) */
  CMU_ClockEnable(cmuClock_GPIO, true);
  BRD420xBoardInit(ZW_REGION);

#ifdef  USB_SUSPEND_SUPPORT // OCELOT_TO_TEST - not tested for Ocelot
  //EM4 implementation for UZB stick
  GPIOINT_Init();
  NVIC_SetPriority(GPIO_ODD_IRQn, GPIO_INTERRUPT_PRIORITY);
  NVIC_SetPriority(GPIO_EVEN_IRQn, GPIO_INTERRUPT_PRIORITY);

  GPIO_PinModeSet(GPIO_PORT_OF_UZBSUSPEND, GPIO_NUMBER_FOR_UZB_SUSPEND, gpioModeInputPullFilter, 1); //wake up on suspend signal goes to 1

  GPIOINT_CallbackRegister(GPIO_NUMBER_FOR_UZB_SUSPEND, gpioCallbackuzbsuspend);

  /* Interrupt num is equal to pin num */
  GPIO_ExtIntConfig(GPIO_PORT_OF_UZBSUSPEND, GPIO_NUMBER_FOR_UZB_SUSPEND, GPIO_NUMBER_FOR_UZB_SUSPEND, true, true, true); //Only rising edge is em4 wakeup

  GPIO_EM4EnablePinWakeup(EM4WU_USB_SUSPEND , EM4WU_USB_SUSPEND);
#endif /*USB_SUSPEND_SUPPORT*/
}

#ifdef USB_SUSPEND_SUPPORT
void SerialAPI_set_usb_supend_callback(SerialAPI_hw_usb_suspend_callback_t callback_)
{
  callback = callback_;
}

void SerialAPI_hw_usb_suspend_handler(void)
{
  EMU_EnterEM4S();
}

/* function name : gpioCallbackuzbsuspend
  In this design we called em4shutoff mode from an application state.
  Newly added application state is called Appsuspendstate, which is set when suspend signal has been activated from the UZB host.
  In the interrupt service routine we notify the application task that suspend has been triggered.
  As the application task running in low priority it is assumed we will enter
  em4 when all higher priority task has been finished or blocked on resource
  */
static void gpioCallbackuzbsuspend(uint8_t pin)
{
#ifdef ZWAVE_SERIES_800
  GPIO_IntClear(_GPIO_IF_EM4WU_MASK); // Clear interrupts from EM4
#else
  GPIO_IntClear(_GPIO_EXTILEVEL_MASK); // Clear interrupts from EM4
#endif
  switch(pin)
  {
    case GPIO_NUMBER_FOR_UZB_SUSPEND:
      if(!GPIO_PinInGet(gpioPortB, GPIO_NUMBER_FOR_UZB_SUSPEND))
    {
      //This means suspend signal is low from the microcontrolller in the uzb stick --usb host driver requested a suspend
      if (callback)
      {
        callback();
      }
      TriggerNotification(EAPPLICATIONEVENT_STATECHANGE);

    }
    break;

    default:
    break;
  }
}
#endif /* USB_SUSPEND_SUPPORT*/
