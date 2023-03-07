/***************************************************************************//**
 * @file
 * @brief Si446x Radio driver
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stddef.h> // NULL

#include "em_bus.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "em_eusart.h"
#include "gpiointerrupt.h"

#include "sl_si446x_radio_eusart_config.h"
#include "sl_si446x_radio.h"

//== DEVICE CAPABILITIES ==

#ifdef  SL_SI446X_RADIO_PERIPHERAL

#if     (defined(SL_SI446X_RADIO_SDN_PORT) && defined(SL_SI446X_RADIO_SDN_PIN))
  #define SL_EXT_DEVICE_POWER_PORT         SL_SI446X_RADIO_SDN_PORT
  #define SL_EXT_DEVICE_POWER_PIN          SL_SI446X_RADIO_SDN_PIN
  #define SL_EXT_DEVICE_POWER_POLARITY     SL_EXT_DEVICE_POLARITY_INVERT
#endif//(defined(SL_SI446X_RADIO_SDN_PORT) && defined(SL_SI446X_RADIO_SDN_PIN))

#if     (defined(SL_SI446X_RADIO_CTS_PORT) && defined(SL_SI446X_RADIO_CTS_PIN))
  #define SL_EXT_DEVICE_READY_PORT         SL_SI446X_RADIO_CTS_PORT
  #define SL_EXT_DEVICE_READY_PIN          SL_SI446X_RADIO_CTS_PIN
  #define SL_EXT_DEVICE_READY_POLARITY     SL_EXT_DEVICE_POLARITY_NORMAL
static unsigned int ctsIntNo = INTERRUPT_UNAVAILABLE;
  #define SL_EXT_DEVICE_READY_IRQ          ctsIntNo
#endif//(defined(SL_SI446X_RADIO_CTS_PORT) && defined(SL_SI446X_RADIO_CTS_PIN))

#if     (defined(SL_SI446X_RADIO_CS_PORT) && defined(SL_SI446X_RADIO_CS_PIN))
  #define SL_EXT_DEVICE_SELECT_PORT        SL_SI446X_RADIO_CS_PORT
  #define SL_EXT_DEVICE_SELECT_PIN         SL_SI446X_RADIO_CS_PIN
  #define SL_EXT_DEVICE_SELECT_POLARITY    SL_EXT_DEVICE_POLARITY_INVERT
#endif//(defined(SL_SI446X_RADIO_CS_PORT) && defined(SL_SI446X_RADIO_CS_PIN))

#if     (defined(SL_SI446X_RADIO_INT_PORT) && defined(SL_SI446X_RADIO_INT_PIN))
  #define SL_EXT_DEVICE_INTERRUPT_PORT     SL_SI446X_RADIO_INT_PORT
  #define SL_EXT_DEVICE_INTERRUPT_PIN      SL_SI446X_RADIO_INT_PIN
  #define SL_EXT_DEVICE_INTERRUPT_POLARITY SL_EXT_DEVICE_POLARITY_INVERT
static unsigned int intIntNo = INTERRUPT_UNAVAILABLE;
  #define SL_EXT_DEVICE_INTERRUPT_IRQ      intIntNo
#endif//(defined(SL_SI446X_RADIO_INT_PORT) && defined(SL_SI446X_RADIO_INT_PIN))

#define SL_EXT_DEVICE_SPI_PORT             SL_SI446X_RADIO_PERIPHERAL

#define SL_EXT_DEVICE_SPI_MOSI_PORT        SL_SI446X_RADIO_TX_PORT
#define SL_EXT_DEVICE_SPI_MOSI_PIN         SL_SI446X_RADIO_TX_PIN

#define SL_EXT_DEVICE_SPI_MISO_PORT        SL_SI446X_RADIO_RX_PORT
#define SL_EXT_DEVICE_SPI_MISO_PIN         SL_SI446X_RADIO_RX_PIN

#define SL_EXT_DEVICE_SPI_CLK_PORT         SL_SI446X_RADIO_SCLK_PORT
#define SL_EXT_DEVICE_SPI_CLK_PIN          SL_SI446X_RADIO_SCLK_PIN

#define SL_EXT_DEVICE_SPI_CS_PORT          SL_SI446X_RADIO_CS_PORT
#define SL_EXT_DEVICE_SPI_CS_PIN           SL_SI446X_RADIO_CS_PIN

#ifdef  SL_SI446X_RADIO_SPI_BAUDRATE
#define SL_EXT_DEVICE_SPI_BAUDRATE         SL_SI446X_RADIO_SPI_BAUDRATE
#endif//SL_SI446X_RADIO_SPI_BAUDRATE

//== COMPATIBLILTY LEGEND (EmberHAL ext-device) ==

// #define halExtDeviceIntCB           sli_ext_device_interrupt_handler
// #define halExtDeviceIntCfgIrq       sli_ext_device_configure_interrupt_irq
// #define halExtDeviceIntLevel        sli_ext_device_interrupt_depth
// #define halExtDevicePowered         sli_ext_device_is_powered
// #define halExtDeviceRdyCB           sli_ext_device_ready_handler
// #define halExtDeviceRdyCfgIrq       sli_ext_device_configure_ready_irq
// #define halExtDeviceSelected        sli_ext_device_is_selected
// #define halIrqxIsr                  sli_ext_device_on_gpio_event

//== LOCAL STATE ==

/// Track device power state
static bool sli_ext_device_is_powered = false;
/// Track device selected state
static bool sli_ext_device_is_selected = false;
#ifdef  SL_EXT_DEVICE_READY_IRQ
/// App's ready handler
static sl_ext_device_handler_t sli_ext_device_ready_handler = NULL;
#endif//SL_EXT_DEVICE_READY_IRQ
#ifdef  SL_EXT_DEVICE_INTERRUPT_IRQ
/// App's interrupt handler
static sl_ext_device_handler_t sli_ext_device_interrupt_handler = NULL;
/// Track app's interrupt depth
static volatile sl_ext_device_interrupt_depth_t sli_ext_device_interrupt_depth = SL_EXT_DEVICE_INTERRUPT_UNCONFIGURED;
#endif//SL_EXT_DEVICE_INTERRUPT_IRQ

//== HELPER FUNCTIONS ==

#if     (defined(SL_EXT_DEVICE_READY_IRQ) || defined(SL_EXT_DEVICE_INTERRUPT_IRQ))

/** @brief GPIO event handler to map interrupt event to its handler.
 *
 * Assume only called from interrupt context.
 */
static void sli_ext_device_on_gpio_event(uint8_t irq, void *ctx)
{
  (void) ctx;
 #ifdef  SL_EXT_DEVICE_INTERRUPT_IRQ
  if (irq == SL_EXT_DEVICE_INTERRUPT_IRQ) {
    // Acknowledge interrupt before callback assuming edge-triggered
    GPIO_IntClear(1U << SL_EXT_DEVICE_INTERRUPT_IRQ);
    if (sli_ext_device_interrupt_handler != NULL) {
      (*sli_ext_device_interrupt_handler)();
    }
    // Treat device interrupt as level-triggered rather than edge-triggered
    if (sl_ext_device_is_interrupt_pending()) {
      GPIO_IntSet(1U << SL_EXT_DEVICE_INTERRUPT_IRQ); // Repend INTERRUPT
    }
  }
 #endif//SL_EXT_DEVICE_INTERRUPT_IRQ
 #ifdef  SL_EXT_DEVICE_READY_IRQ
  if (irq == SL_EXT_DEVICE_READY_IRQ) {
    if (sli_ext_device_ready_handler != NULL) {
      (*sli_ext_device_ready_handler)();
    }
    // Acknowledge READY interrupt after callback in case it's level triggered
    GPIO_IntClear(1U << SL_EXT_DEVICE_READY_IRQ);
  }
 #endif//SL_EXT_DEVICE_READY_IRQ
}

#endif//(defined(SL_EXT_DEVICE_INTERRUPT_IRQ) || defined(SL_EXT_DEVICE_READY_IRQ))

/** @brief Configure and enable/disable the device ready IRQ
 */
static void sli_ext_device_configure_ready_irq(void)
{
 #ifdef  SL_EXT_DEVICE_READY_IRQ
  CORE_DECLARE_IRQ_STATE;

  // Disable interrupt and clear out anything stale.
  GPIO_IntDisable(1U << SL_EXT_DEVICE_READY_IRQ);
  GPIO_IntClear(1U << SL_EXT_DEVICE_READY_IRQ);
  // Disable interrupts because this might be called from non-GPIO ISRs
  // (protecting sli_ internal state), and GPIO_InputSenseSet() is not
  // safe to call in ISR context.
  CORE_ENTER_ATOMIC();
  // If device is powered and handler exists, configure & enable interrupt.
  if (sli_ext_device_is_powered && (sli_ext_device_ready_handler != NULL)) {
    SL_EXT_DEVICE_READY_IRQ = GPIOINT_CallbackRegisterExt(SL_EXT_DEVICE_READY_PIN, &sli_ext_device_on_gpio_event, NULL);
    // assert(SL_EXT_DEVICE_READY_IRQ != INTERRUPT_UNAVAILABLE);
    GPIO_ExtIntConfig((GPIO_Port_TypeDef) SL_EXT_DEVICE_READY_PORT,
                      SL_EXT_DEVICE_READY_PIN,
                      SL_EXT_DEVICE_READY_IRQ,
                      SL_EXT_DEVICE_READY_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL,  // risingEdge
                      SL_EXT_DEVICE_READY_POLARITY == SL_EXT_DEVICE_POLARITY_INVERT,  // fallingEdge
                      true);  // enable
  }
  CORE_EXIT_ATOMIC();
 #endif//SL_EXT_DEVICE_READY_IRQ
}

/** @brief Configure and enable/disable the device interrupt IRQ
 */
static void sli_ext_device_configure_interrupt_irq(void)
{
 #ifdef  SL_EXT_DEVICE_INTERRUPT_IRQ
  CORE_DECLARE_IRQ_STATE;

  // Disable interrupt and clear out anything stale.
  GPIO_IntDisable(1U << SL_EXT_DEVICE_INTERRUPT_IRQ);
  GPIO_IntClear(1U << SL_EXT_DEVICE_INTERRUPT_IRQ);
  // Disable interrupts because this might be called from non-GPIO ISRs
  // (protecting sli_ internal state), and GPIO_InputSenseSet() is not
  // safe to call in ISR context.
  CORE_ENTER_ATOMIC();
  sli_ext_device_interrupt_depth = SL_EXT_DEVICE_INTERRUPT_UNCONFIGURED;
  // If device is powered and handler exists, configure interrupt disabled
  if (sli_ext_device_is_powered && (sli_ext_device_interrupt_handler != NULL)) {
    SL_EXT_DEVICE_INTERRUPT_IRQ = GPIOINT_CallbackRegisterExt(SL_EXT_DEVICE_INTERRUPT_PIN, &sli_ext_device_on_gpio_event, NULL);
    // assert(SL_EXT_DEVICE_INTERRUPT_IRQ != INTERRUPT_UNAVAILABLE);
    GPIO_ExtIntConfig((GPIO_Port_TypeDef) SL_EXT_DEVICE_INTERRUPT_PORT,
                      SL_EXT_DEVICE_INTERRUPT_PIN,
                      SL_EXT_DEVICE_INTERRUPT_IRQ,
                      SL_EXT_DEVICE_INTERRUPT_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL,  // risingEdge
                      SL_EXT_DEVICE_INTERRUPT_POLARITY == SL_EXT_DEVICE_POLARITY_INVERT,  // fallingEdge
                      false);  // enable (leave disabled)
    sli_ext_device_interrupt_depth = SL_EXT_DEVICE_INTERRUPT_DEPTH_OFF;
    // Callers need to use sl_ext_device_enable_interrupt() to enable
  }
  CORE_EXIT_ATOMIC();
 #endif//SL_EXT_DEVICE_INTERRUPT_IRQ
}

//== GLOBAL STATE ==

#ifdef  SL_EXT_DEVICE_SPI_PORT

#include "spidrv.h"

// Default SPI baudrate to 4 MHz if not defined
#ifndef SL_EXT_DEVICE_SPI_BAUDRATE
#define SL_EXT_DEVICE_SPI_BAUDRATE   4000000U
#endif//SL_EXT_DEVICE_SPI_BAUDRATE

const SPIDRV_Init_t sl_ext_device_spi_config = {
  .port            = (void *)SL_EXT_DEVICE_SPI_PORT,
 #if defined(_USART_ROUTELOC0_MASK)
  .portLocationTx  = SL_EXT_DEVICE_SPI_MOSI_LOC,
  .portLocationRx  = SL_EXT_DEVICE_SPI_MISO_LOC,
  .portLocationClk = SL_EXT_DEVICE_SPI_CLK_LOC,
 #ifndef SL_EXT_DEVICE_SELECT_PIN
  .portLocationCs  = SL_EXT_DEVICE_SPI_CS_LOC,
 #endif//SL_EXT_DEVICE_SELECT_PIN
 #elif defined(_GPIO_USART_ROUTEEN_MASK) || defined(_GPIO_EUSART_ROUTEEN_MASK)
  .portTx          = SL_EXT_DEVICE_SPI_MOSI_PORT,
  .pinTx           = SL_EXT_DEVICE_SPI_MOSI_PIN,
  .portRx          = SL_EXT_DEVICE_SPI_MISO_PORT,
  .pinRx           = SL_EXT_DEVICE_SPI_MISO_PIN,
  .portClk         = SL_EXT_DEVICE_SPI_CLK_PORT,
  .pinClk          = SL_EXT_DEVICE_SPI_CLK_PIN,
 #ifndef SL_EXT_DEVICE_SELECT_PIN
  .portCs          = SL_EXT_DEVICE_SPI_CS_PORT,
  .pinCs           = SL_EXT_DEVICE_SPI_CS_PIN,
 #endif//SL_EXT_DEVICE_SELECT_PIN
 #else
  .portLocation    =,  //@TODO trigger compilation error for now
 #endif
  .bitRate         = SL_EXT_DEVICE_SPI_BAUDRATE,
  .frameLength     = 8U,                     // DO NOT CHANGE!
  .dummyTxValue    = 0xFFU,                  // DO NOT CHANGE!
  .type            = spidrvMaster,           // DO NOT CHANGE!
  .bitOrder        = spidrvBitOrderMsbFirst, // DO NOT CHANGE!
  .clockMode       = spidrvClockMode0,       // DO NOT CHANGE!
 #ifdef  SL_EXT_DEVICE_SELECT_PIN
  .csControl       = spidrvCsControlApplication,
 #else//!SL_EXT_DEVICE_SELECT_PIN
  .csControl       = spidrvCsControlAuto,
 #endif//SL_EXT_DEVICE_SELECT_PIN
};

static SPIDRV_HandleData_t sli_ext_device_spi_handle_data;
SPIDRV_Handle_t sl_ext_device_spi_handle = &sli_ext_device_spi_handle_data;

#endif//SL_EXT_DEVICE_SPI_PORT

//== API FUNCTIONS ==

//-- External Device Initialization --

sl_ext_device_config_t sl_ext_device_init(sl_ext_device_handler_t interrupt_handler,
                                          sl_ext_device_handler_t ready_handler)
{
  CORE_DECLARE_IRQ_STATE;

 #ifdef  SL_EXT_DEVICE_READY_IRQ
  sli_ext_device_ready_handler = ready_handler;
 #else//!SL_EXT_DEVICE_READY_IRQ
  (void) ready_handler;
 #endif//SL_EXT_DEVICE_READY_IRQ
 #ifdef  SL_EXT_DEVICE_INTERRUPT_IRQ
  sli_ext_device_interrupt_handler = interrupt_handler;
 #else//!SL_EXT_DEVICE_INTERRUPT_IRQ
  (void) interrupt_handler;
 #endif//SL_EXT_DEVICE_INTERRUPT_IRQ

  // GPIOINT_Init() and GPIO_PinModeSet() are NOT interrupt-safe.
  CORE_ENTER_ATOMIC();

  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

 #if     (defined(SL_EXT_DEVICE_READY_IRQ) || defined(SL_EXT_DEVICE_INTERRUPT_IRQ))
  GPIOINT_Init();
 #endif//(defined(SL_EXT_DEVICE_READY_IRQ) || defined(SL_EXT_DEVICE_INTERRUPT_IRQ))

 #ifdef  SL_EXT_DEVICE_POWER_PIN
  GPIO_PinModeSet((GPIO_Port_TypeDef) SL_EXT_DEVICE_POWER_PORT,
                  SL_EXT_DEVICE_POWER_PIN,
                  gpioModePushPull,
                  (SL_EXT_DEVICE_POWER_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL
                   ? 0U : 1U)); // init unpowered
 #endif//SL_EXT_DEVICE_POWER_PIN

 #ifdef  SL_EXT_DEVICE_SELECT_PIN
  GPIO_PinModeSet((GPIO_Port_TypeDef) SL_EXT_DEVICE_SELECT_PORT,
                  SL_EXT_DEVICE_SELECT_PIN,
                  gpioModePushPull,
                  (SL_EXT_DEVICE_SELECT_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL
                   ? 0U : 1U)); // init deselected
 #endif//SL_EXT_DEVICE_SELECT_PIN

 #ifdef  SL_EXT_DEVICE_READY_PIN
  GPIO_PinModeSet((GPIO_Port_TypeDef) SL_EXT_DEVICE_READY_PORT,
                  SL_EXT_DEVICE_READY_PIN,
                  gpioModeInputPull,
                  (SL_EXT_DEVICE_READY_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL
                   ? 0U : 1U)); // init pull to not ready
 #endif//SL_EXT_DEVICE_READY_PIN

 #ifdef  SL_EXT_DEVICE_INTERRUPT_PIN
  GPIO_PinModeSet((GPIO_Port_TypeDef) SL_EXT_DEVICE_INTERRUPT_PORT,
                  SL_EXT_DEVICE_INTERRUPT_PIN,
                  gpioModeInputPull,
                  (SL_EXT_DEVICE_INTERRUPT_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL
                   ? 0U : 1U)); // init pull to not interrupt
 #endif//SL_EXT_DEVICE_INTERRUPT_PIN

  CORE_EXIT_ATOMIC();

  // When powered just update the IRQ callbacks.
  // When not powered, ensure device is not powered and not selected.
  if (sli_ext_device_is_powered) {
    sli_ext_device_configure_ready_irq();
    sli_ext_device_configure_interrupt_irq();
  } else {
    sl_ext_device_deselect();
    sl_ext_device_power_down();
  }

  return (sl_ext_device_config_t) (0U
                                  #ifdef  SL_EXT_DEVICE_POWER_PIN
                                   | SL_EXT_DEVICE_HAS_POWER
                                  #endif//SL_EXT_DEVICE_POWER_PIN
                                  #ifdef  SL_EXT_DEVICE_READY_PIN
                                   | SL_EXT_DEVICE_HAS_READY
                                  #ifdef  SL_EXT_DEVICE_READY_IRQ
                                   | SL_EXT_DEVICE_HAS_READY_IRQ
                                  #endif//SL_EXT_DEVICE_READY_IRQ
                                  #endif//SL_EXT_DEVICE_READY_PIN
                                  #ifdef  SL_EXT_DEVICE_SELECT_PIN
                                   | SL_EXT_DEVICE_HAS_SELECT
                                  #endif//SL_EXT_DEVICE_SELECT_PIN
                                  #ifdef  SL_EXT_DEVICE_INTERRUPT_PIN
                                   | SL_EXT_DEVICE_HAS_INTERRUPT
                                  #ifdef  SL_EXT_DEVICE_INTERRUPT_IRQ
                                   | SL_EXT_DEVICE_HAS_INTERRUPT_IRQ
                                  #endif//SL_EXT_DEVICE_INTERRUPT_IRQ
                                  #endif//SL_EXT_DEVICE_INTERRUPT_PIN
                                   );
}

//-- External Device Power --

void sl_ext_device_power_down(void)
{
  // Shut off ISRs before removing power, in case of glitches
  sli_ext_device_is_powered = false;
  sli_ext_device_configure_ready_irq();
  sli_ext_device_configure_interrupt_irq();

 #ifdef  SL_EXT_DEVICE_POWER_PIN
 #if     (SL_EXT_DEVICE_POWER_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
  GPIO_PinOutClear((GPIO_Port_TypeDef) SL_EXT_DEVICE_POWER_PORT,
                   SL_EXT_DEVICE_POWER_PIN);
 #else//!(SL_EXT_DEVICE_POWER_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
  GPIO_PinOutSet((GPIO_Port_TypeDef) SL_EXT_DEVICE_POWER_PORT,
                 SL_EXT_DEVICE_POWER_PIN);
 #endif//(SL_EXT_DEVICE_POWER_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
 #endif//SL_EXT_DEVICE_POWER_PIN
}

void sl_ext_device_power_up(void)
{
  // Enable ISRs before powering so they can notify when device is ready
  sli_ext_device_is_powered = true;
  sli_ext_device_configure_ready_irq();
  sli_ext_device_configure_interrupt_irq();

 #ifdef  SL_EXT_DEVICE_POWER_PIN
 #if     (SL_EXT_DEVICE_POWER_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
  GPIO_PinOutSet((GPIO_Port_TypeDef) SL_EXT_DEVICE_POWER_PORT,
                 SL_EXT_DEVICE_POWER_PIN);
 #else//!(SL_EXT_DEVICE_POWER_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
  GPIO_PinOutClear((GPIO_Port_TypeDef) SL_EXT_DEVICE_POWER_PORT,
                   SL_EXT_DEVICE_POWER_PIN);
 #endif//(SL_EXT_DEVICE_POWER_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
 #endif//SL_EXT_DEVICE_POWER_PIN
}

//-- External Device Ready --

bool sl_ext_device_is_ready(void)
{
 #ifdef  SL_EXT_DEVICE_READY_PIN
  return (sli_ext_device_is_powered
          && ((bool)GPIO_PinInGet(SL_EXT_DEVICE_READY_PORT, SL_EXT_DEVICE_READY_PIN)
              == SL_EXT_DEVICE_READY_POLARITY));
 #else//!SL_EXT_DEVICE_READY_PIN
  return sli_ext_device_is_powered; // Assume ready only when powered
 #endif//SL_EXT_DEVICE_READY_PIN
}

extern inline bool sl_ext_device_not_ready(void);

void sl_ext_device_wait_ready(void)
{
  // Beware of watchdog
  while (sl_ext_device_not_ready()) {
    // spin
  }
}

//-- External Device Selection --

void sl_ext_device_select(void)
{
  sli_ext_device_is_selected = true;

 #ifdef  SL_EXT_DEVICE_SELECT_PIN
 #if     (SL_EXT_DEVICE_SELECT_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
  GPIO_PinOutSet((GPIO_Port_TypeDef) SL_EXT_DEVICE_SELECT_PORT,
                 SL_EXT_DEVICE_SELECT_PIN);
 #else//!(SL_EXT_DEVICE_SELECT_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
  GPIO_PinOutClear((GPIO_Port_TypeDef) SL_EXT_DEVICE_SELECT_PORT,
                   SL_EXT_DEVICE_SELECT_PIN);
 #endif//(SL_EXT_DEVICE_SELECT_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
 #endif//SL_EXT_DEVICE_SELECT_PIN
}

void sl_ext_device_deselect(void)
{
  sli_ext_device_is_selected = false;

 #ifdef  SL_EXT_DEVICE_SELECT_PIN
 #if     (SL_EXT_DEVICE_SELECT_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
  GPIO_PinOutClear((GPIO_Port_TypeDef) SL_EXT_DEVICE_SELECT_PORT,
                   SL_EXT_DEVICE_SELECT_PIN);
 #else//!(SL_EXT_DEVICE_SELECT_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
  GPIO_PinOutSet((GPIO_Port_TypeDef) SL_EXT_DEVICE_SELECT_PORT,
                 SL_EXT_DEVICE_SELECT_PIN);
 #endif//(SL_EXT_DEVICE_SELECT_POLARITY == SL_EXT_DEVICE_POLARITY_NORMAL)
 #endif//SL_EXT_DEVICE_SELECT_PIN
}

bool sl_ext_device_is_selected(void)
{
  return sli_ext_device_is_selected;
}

extern inline bool sl_ext_device_not_selected(void);

//-- External Device Interrupts --

bool sl_ext_device_is_interrupt_pending(void)
{
 #ifdef  SL_EXT_DEVICE_INTERRUPT_PIN
  // This is pure GPIO level -- treat device interrupt as level-triggered
  return (sli_ext_device_is_powered
          && ((bool)GPIO_PinInGet((GPIO_Port_TypeDef) SL_EXT_DEVICE_INTERRUPT_PORT,
                                  SL_EXT_DEVICE_INTERRUPT_PIN)
              == SL_EXT_DEVICE_INTERRUPT_POLARITY));
 #else//!SL_EXT_DEVICE_INTERRUPT_PIN
  return false; // Assume never pending
 #endif//SL_EXT_DEVICE_INTERRUPT_PIN
}

extern inline bool sl_ext_device_no_interrupt_pending(void);

sl_ext_device_interrupt_depth_t sl_ext_device_disable_interrupt(void)
{
 #ifdef  SL_EXT_DEVICE_INTERRUPT_IRQ
  CORE_DECLARE_IRQ_STATE;
  sl_ext_device_interrupt_depth_t orig_depth;

  GPIO_IntDisable(1U << SL_EXT_DEVICE_INTERRUPT_IRQ);

  // Disable interrupts because this might be called from non-GPIO ISRs
  // (protecting sli_ext_device_interupt_depth).
  CORE_ENTER_ATOMIC();
  orig_depth = sli_ext_device_interrupt_depth;
  if (orig_depth != SL_EXT_DEVICE_INTERRUPT_UNCONFIGURED) {
    sli_ext_device_interrupt_depth += 1U;
  }
  CORE_EXIT_ATOMIC();

  return orig_depth;
 #else//!SL_EXT_DEVICE_INTERRUPT_IRQ
  return SL_EXT_DEVICE_INTERRUPT_UNCONFIGURED;
 #endif//SL_EXT_DEVICE_INTERRUPT_IRQ
}

sl_ext_device_interrupt_depth_t sl_ext_device_enable_interrupt(bool clear_pending)
{
 #ifdef  SL_EXT_DEVICE_INTERRUPT_IRQ
  CORE_DECLARE_IRQ_STATE;
  sl_ext_device_interrupt_depth_t orig_depth;
  bool just_enabled = false;

  // Disable interrupts because this might be called from non-GPIO ISRs
  // (protecting sli_ext_device_interupt_depth).
  CORE_ENTER_ATOMIC();
  orig_depth = sli_ext_device_interrupt_depth;
  if (orig_depth != SL_EXT_DEVICE_INTERRUPT_UNCONFIGURED) {
    if (orig_depth > SL_EXT_DEVICE_INTERRUPT_DEPTH_ON) { // Peg at DEPTH_ON
      sli_ext_device_interrupt_depth -= 1U;
      just_enabled = (sli_ext_device_interrupt_depth == SL_EXT_DEVICE_INTERRUPT_DEPTH_ON);
    }
  }
  CORE_EXIT_ATOMIC();

  if (clear_pending) {
    // Clear out any stale state
    GPIO_IntClear(1U << SL_EXT_DEVICE_INTERRUPT_IRQ);
  }

  if (just_enabled) {
    // In case we missed edge of level interrupt
    if (sl_ext_device_is_interrupt_pending()) {
      GPIO_IntSet(1U << SL_EXT_DEVICE_INTERRUPT_IRQ);
    }
    GPIO_IntEnable(1U << SL_EXT_DEVICE_INTERRUPT_IRQ);
  }

  return orig_depth;
 #else//!SL_EXT_DEVICE_INTERRUPT_IRQ
  return SL_EXT_DEVICE_INTERRUPT_UNCONFIGURED;
 #endif//SL_EXT_DEVICE_INTERRUPT_IRQ
}

#else//!SL_SI446X_RADIO_PERIPHERAL

// Do NOT provide stub API implementation; force linking to fail instead.

#endif//SL_SI446X_RADIO_PERIPHERAL
