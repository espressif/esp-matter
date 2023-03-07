/**
 * @file
 * Provides support for board GPIO functionality
 * @copyright 2019 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <board.h>
//#define DEBUGPRINT
#include "DebugPrint.h"
#include <ZW_application_transport_interface.h>

#include "ev_man.h"
#ifdef ZWAVE_SERIES_800
#include <zpal_power_manager.h>
#endif
#include <AppTimer.h>

#include "gpiointerrupt.h"

#include "em_timer.h"
#include "em_emu.h"
#include "SizeOf.h"

#include <hal-config.h>
#include <system_startup.h>

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/* Button event durations (milliseconds) */
#define BUTTON_SHORT_PRESS_DURATION   20
#define BUTTON_HOLD_DURATION         300
#define BUTTON_LONG_PRESS_DURATION  5000

/* Stop m_button_timer if button down longer than 30s */
#define MAX_HOLD_TIME  30000

/** Timer update interval (ms) used for push-button de-bounce handling */
#define DEBOUNCE_FILTER_TIME_STEP 10

typedef enum
{
  LED_MONO,
  LED_RGB
} led_type_t;


/* Structure with LED config data */
typedef struct
{
  bool              is_available;
  led_type_t        type;
  const char*       label;
  GPIO_Port_TypeDef gpio_port;
  uint32_t          gpio_pin;
  uint32_t          on_value;
  uint32_t          letimer_loc;
} led_info_t;

/* Convenience macro to fill single element for standard LED in
 * array of led_info_t elements */
#define LED_INFO(led) \
  { \
    .is_available = led ## _AVAILABLE, \
    .type         = LED_MONO, \
    .label        = led ## _LABEL, \
    .gpio_port    = led ## _GPIO_PORT, \
    .gpio_pin     = led ## _GPIO_PIN, \
    .on_value     = led ## _ON_VALUE, \
    .letimer_loc  = led ## _LETIM0_OUT0_LOC \
  }

/* Convenience macro to fill single element for RGB LED in
 * array of led_info_t elements */
#define RGB_INFO(led, led_label) \
  { \
    .is_available = led ## _AVAILABLE, \
    .type         = LED_RGB, \
    .label        = (led_label), \
    .gpio_port    = led ## _GPIO_PORT, \
    .gpio_pin     = led ## _GPIO_PIN, \
    .on_value     = led ## _ON_VALUE, \
    .letimer_loc  = 0 \
  }

/* Structure with button config data */
typedef struct
{
  bool              is_available;
  button_type_t     type;
  const char*       label;
  GPIO_Port_TypeDef gpio_port;
  uint32_t          gpio_pin;
  uint32_t          on_value;
  uint8_t           int_no;
  bool              can_wakeup_em4;
  BUTTON_EVENT      event_base;
} button_info_t;

/* Convenience macro to fill single element in array of
 * button_info_t elements */
#define BUTTON_INFO(btn, btn_type) \
  { \
    .is_available   = btn ## _AVAILABLE, \
    .type           = (btn_type), \
    .label          = btn ## _LABEL, \
    .gpio_port      = btn ## _GPIO_PORT, \
    .gpio_pin       = btn ## _GPIO_PIN, \
    .on_value       = btn ## _ON_VALUE, \
    .int_no         = btn ## _INT_NO, \
    .can_wakeup_em4 = btn ## _CAN_WAKEUP_EM4, \
    .event_base     = EVENT_ ## btn ## _DOWN \
  }


#define GPIO_ID(port, pin) ((port)*16 + (pin))

/*************************************************************************/
/* Configure UART0                                                       */
/*************************************************************************/
#define UART0_TX_PORT     BSP_SERIAL_APP_TX_PORT
#define UART0_TX_PIN      BSP_SERIAL_APP_TX_PIN
#define UART0_RX_PORT     BSP_SERIAL_APP_RX_PORT
#define UART0_RX_PIN      BSP_SERIAL_APP_RX_PIN

/*************************************************************************/
/* Configure UART1                                                       */
/*************************************************************************/
#ifdef ZWAVE_SERIES_800

#define UART1_TX_PORT     PORTIO_EUSART1_TX_PORT
#define UART1_TX_PIN      PORTIO_EUSART1_TX_PIN
#define UART1_RX_PORT     PORTIO_EUSART1_RX_PORT
#define UART1_RX_PIN      PORTIO_EUSART1_RX_PIN

#else

#define UART1_TX_PORT     PORTIO_USART1_TX_PORT
#define UART1_TX_PIN      PORTIO_USART1_TX_PIN
#define UART1_RX_PORT     PORTIO_USART1_RX_PORT
#define UART1_RX_PIN      PORTIO_USART1_RX_PIN

#endif //ZWAVE_SERIES_800


/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/**
 * This variable stores the latest generated button or slider event for poll operations!
 */
uint8_t m_lastEventGenerated = EVENT_BTN_MAX;

static Board_button_callback_t m_button_callback;

/**
 * Timer used for push-button de-bounce. Timer is only running while one
 * or more buttons are pressed..
 */
static SSwTimer m_button_timer;
static volatile bool button_timer_is_running;

/**
 * Current value of button timer (ms) - updated by ButtonTimerCallback() every
 * DEBOUNCE_FILTER_TIME_STEP ms.
 */
static volatile uint32_t m_button_timer_value;

static button_state_t m_button_state[BOARD_BUTTON_COUNT];
static uint32_t       m_button_down_time[BOARD_BUTTON_COUNT];

/* GPIO EM4 wakeup flags (bitmask containing button(s) that woke us up from EM4)*/
static uint32_t g_gpioEm4Flags = 0;

/* Fill array with LED configuration data
 * Pulls in defines from extension_board_xxx.h radio_board_xxx.h
 */
static const led_info_t m_led_info[] = {
  LED_INFO(LED1),
  LED_INFO(LED2),
  LED_INFO(LED3),
  LED_INFO(LED4),
  RGB_INFO(RGB1_R, RGB1_LABEL),
  RGB_INFO(RGB1_G, RGB1_LABEL),
  RGB_INFO(RGB1_B, RGB1_LABEL)
};

/* Ensure enum led_id_t is the same size as m_led_info */
STATIC_ASSERT((sizeof_array(m_led_info) == BOARD_LED_COUNT),
              STATIC_ASSERT_FAILED_m_led_info_wrong_size);


/* Fill array with button configuration data
 * Pulls in defines from extension_board_xxx.h radio_board_xxx.h
 */
static const button_info_t m_button_info[] = {
  BUTTON_INFO(PB1, BUTTON_TYPE_PUSHBUTTON),
  BUTTON_INFO(PB2, BUTTON_TYPE_PUSHBUTTON),
  BUTTON_INFO(PB3, BUTTON_TYPE_PUSHBUTTON),
  BUTTON_INFO(PB4, BUTTON_TYPE_PUSHBUTTON),
  BUTTON_INFO(PB5, BUTTON_TYPE_PUSHBUTTON),
  BUTTON_INFO(PB6, BUTTON_TYPE_PUSHBUTTON),
  BUTTON_INFO(SLIDER1, BUTTON_TYPE_SLIDER)
};

#ifdef ZWAVE_SERIES_800
static zpal_pm_handle_t radio_power_lock;
#endif
/* Ensure enum button_id_t is the same size as m_button_info */
STATIC_ASSERT((sizeof_array(m_button_info) == BOARD_BUTTON_COUNT),
              STATIC_ASSERT_FAILED_m_button_info_wrong_size);


/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

/* ----------------------- MISC PRIVATE HELPERS --------------------------- */

/**
 * Send (button) event to application
 *
 * @param event Button event to send to application.
 * @param is_called_from_isr true if this function is called from an
 *                           interrupt handler. False otherwise.
 */
static void SendEventToApp(uint8_t event, bool is_called_from_isr)
{
#ifdef ZWAVE_SERIES_800
  zpal_pm_cancel(radio_power_lock);
#endif
  if (m_button_callback)
  {
    m_button_callback(event, is_called_from_isr);
  }

  // Used for polling the event manually
  m_lastEventGenerated = event;
}

uint8_t Board_GetLatestBtnEvent(void)
{
  uint8_t retValue = EVENT_BTN_MAX;
  // Is an event pending?
  if(m_lastEventGenerated != EVENT_BTN_MAX)
  {
    // Making atomic! (Both interrupts and tasks are using this variable)
    taskENTER_CRITICAL();
    retValue = m_lastEventGenerated;
    m_lastEventGenerated = EVENT_BTN_MAX;  // Use invalid value to filter, in case no event is generated between polls.
    taskEXIT_CRITICAL();
  }
  return retValue;
}

/**
 * Lookup TIM1_CCx location for a GPIO pin
 *
 * Used to configure PWM for RGB leds (using TIMER1, channel 1, 2, 3).
 * See Board_RgbLedEnablePwm().
 *
 * NB: This function might be overkill. To save a few bytes (and loosing a
 *     little bit of flexibility) the values can naturally be hardcoded in
 *     e.g. Board_RgbLedEnablePwm().
 *
 * @param port     GPIO port
 * @param pin      GPIO pin
 * @param chan_num Channel number.
 *
 * @return TIM1_CCx location value for the specified GPIO.
 */
#ifndef ZWAVE_SERIES_800 // Not yet ported to 800 series
static uint32_t GetTim1CcLocation(GPIO_Port_TypeDef port, uint32_t pin, uint32_t chan_num)
{
  uint32_t loc = 0;

  /* Lookup the TIM1_CC0 location for the GPIO
   * (See TIM1_CC0 in table 6.7 in efr32fg13 or efr32fg14 datasheet)
   */
  switch (GPIO_ID(port, pin))
  {
    case GPIO_ID(gpioPortA, 0): loc = 0; break;
    case GPIO_ID(gpioPortA, 1): loc = 1; break;
    case GPIO_ID(gpioPortA, 2): loc = 2; break;
    case GPIO_ID(gpioPortA, 3): loc = 3; break;
    case GPIO_ID(gpioPortA, 4): loc = 4; break;
    case GPIO_ID(gpioPortA, 5): loc = 5; break;

    case GPIO_ID(gpioPortB, 11): loc = 6; break;
    case GPIO_ID(gpioPortB, 12): loc = 7; break;
    case GPIO_ID(gpioPortB, 13): loc = 8; break;
    case GPIO_ID(gpioPortB, 14): loc = 9; break;
    case GPIO_ID(gpioPortB, 15): loc = 10; break;

    case GPIO_ID(gpioPortC, 6): loc = 11; break;
    case GPIO_ID(gpioPortC, 7): loc = 12; break;
    case GPIO_ID(gpioPortC, 8): loc = 13; break;
    case GPIO_ID(gpioPortC, 9): loc = 14; break;
    case GPIO_ID(gpioPortC, 10): loc = 15; break;
    case GPIO_ID(gpioPortC, 11): loc = 16; break;

    case GPIO_ID(gpioPortD, 9): loc = 17; break;
    case GPIO_ID(gpioPortD, 10): loc = 18; break;
    case GPIO_ID(gpioPortD, 11): loc = 19; break;
    case GPIO_ID(gpioPortD, 12): loc = 20; break;
    case GPIO_ID(gpioPortD, 13): loc = 21; break;
    case GPIO_ID(gpioPortD, 14): loc = 22; break;
    case GPIO_ID(gpioPortD, 15): loc = 23; break;

    case GPIO_ID(gpioPortF, 0): loc = 24; break;
    case GPIO_ID(gpioPortF, 1): loc = 25; break;
    case GPIO_ID(gpioPortF, 2): loc = 26; break;
    case GPIO_ID(gpioPortF, 3): loc = 27; break;
    case GPIO_ID(gpioPortF, 4): loc = 28; break;
    case GPIO_ID(gpioPortF, 5): loc = 29; break;
    case GPIO_ID(gpioPortF, 6): loc = 30; break;
    case GPIO_ID(gpioPortF, 7): loc = 31; break;
    default:
      ASSERT(false);
      break;
  }

  /* Generate the TIMER_ROUTELOC0_CC(chan_num)LOC_LOCxx
   * value from the location and channel number */

  uint32_t loc_num = (loc + 32 - chan_num) % 32;
  uint32_t loc_bit_value = loc_num << (8 * chan_num);

  return loc_bit_value;
}
#endif

/* --------------------------------- LEDS --------------------------------- */

/**
 * Checks if LED has been defined in currently used board configuration
 */
static inline bool Board_IsLedAvailable(led_id_t led)
{
  return (led < sizeof_array(m_led_info) && m_led_info[led].is_available);
}

/**
 * Calls Board_IsLedAvailable and asserts if LED is unavailable.
 */
static bool Board_IsLedAvailable_WithAssert(led_id_t led)
{
  if (Board_IsLedAvailable(led))
  {
    return true;
  }
  else {
    ASSERT(false);
    return false;
  }
}

/**
 * Lookup GPIO port for LED
 */
static GPIO_Port_TypeDef Board_GetLedPort(led_id_t led)
{
  return (Board_IsLedAvailable_WithAssert(led)) ? m_led_info[led].gpio_port : 0;
}

/**
 * Lookup GPIO pin for LED
 */
static uint32_t Board_GetLedPin(led_id_t led)
{
  return (Board_IsLedAvailable_WithAssert(led)) ? m_led_info[led].gpio_pin : 0;
}

/* ------------------------------- BUTTONS -------------------------------- */

/**
 * Checks if button has been defined in currently used board configuration
 */
static inline bool Board_IsButtonAvailable(button_id_t btn)
{
  return (btn < sizeof_array(m_button_info) && m_button_info[btn].is_available);
}

/**
 * Calls Board_IsButtonAvailable and asserts if button is unavailable.
 */
static bool Board_IsButtonAvailable_WithAssert(button_id_t btn)
{
  if (Board_IsButtonAvailable(btn))
  {
    return true;
  }
  else {
    ASSERT(false);
    return false;
  }
}

/**
 * Lookup GPIO port for button
 */
static GPIO_Port_TypeDef Board_GetButtonPort(button_id_t btn)
{
  return (Board_IsButtonAvailable_WithAssert(btn)) ? m_button_info[btn].gpio_port : 0;
}

/**
 * Lookup GPIO pin for button
 */
static uint32_t Board_GetButtonPin(button_id_t btn)
{
  return (Board_IsButtonAvailable_WithAssert(btn)) ? m_button_info[btn].gpio_pin : 0;
}

/**
 * Lookup "ON" value (0 or 1) for button
 */
static uint32_t Board_GetButtonOnValue(button_id_t btn)
{
  return (Board_IsButtonAvailable_WithAssert(btn)) ? m_button_info[btn].on_value : 1;
}

/**
 * Lookup EM4 wakeup capability for button
 */
static bool Board_GetButtonCanWakeupEm4(button_id_t btn)
{
  return (Board_IsButtonAvailable_WithAssert(btn)) ? m_button_info[btn].can_wakeup_em4 : false;
}

/**
 * Lookup GPIO intNo for button
 */
static uint8_t Board_GetButtonIntNo(button_id_t btn)
{
  return (Board_IsButtonAvailable_WithAssert(btn)) ? m_button_info[btn].int_no : 0xFF;
}

/**
 * Checks if a button has been DOWN long enough to change state to HOLD
 *
 * NB: inline since called from timer call back every 10 ms.

 * @param btn             Button identifier.
 * @param btn_event_base  button event base. See @ref Board_GetButtonEventBase().
 *
 */
static inline void ButtonCheckHold(button_id_t btn, BUTTON_EVENT btn_event_base)
{
  if (BUTTON_DOWN == m_button_state[btn])
  {
    /* If the button has been down for more than BUTTON_HOLD_DURATION
     * Change its state to HOLD and report it to the application
     */
    if (BUTTON_HOLD_DURATION <= (m_button_timer_value - m_button_down_time[btn]))
    {
      m_button_state[btn] = BUTTON_HOLD;
      SendEventToApp(EV_BTN_EVENT_HOLD(btn_event_base), true);
    }
  }
}

/*************************************************************************/

/**
 * Button timer callback function
 *
 * Called periodically whenever m_button_timer runs out (every 10 ms, see
 * @ref DEBOUNCE_FILTER_TIME_STEP).
 *
 * Iterates all push buttons to check if their state should be changed to HOLD.
 *
 * @param pTimer  Not used.
 */
static void ButtonTimerCallback(SSwTimer *pTimer)
{
  UNUSED(pTimer);
  bool stop_timer = true;

  m_button_timer_value += DEBOUNCE_FILTER_TIME_STEP;

  /* The following function is inlined
   * NB: we could have used Board_GetButtonEventBase() but that
   *     would be a little bad for performance in this callback function
   */
  ButtonCheckHold(BOARD_BUTTON_PB1, EVENT_PB1_DOWN);
  ButtonCheckHold(BOARD_BUTTON_PB2, EVENT_PB2_DOWN);
  ButtonCheckHold(BOARD_BUTTON_PB3, EVENT_PB3_DOWN);
  ButtonCheckHold(BOARD_BUTTON_PB4, EVENT_PB4_DOWN);
  ButtonCheckHold(BOARD_BUTTON_PB5, EVENT_PB5_DOWN);
  ButtonCheckHold(BOARD_BUTTON_PB6, EVENT_PB6_DOWN);

  for (button_id_t btn = 0; btn < BOARD_BUTTON_SLIDER1; btn++)
  {
    if (BUTTON_UP != m_button_state[btn])
    {
      stop_timer = false;
      break;
    }
  }

  /* There's no need to keep the button timer running if all
   * buttons are UP or if the buttons has been activated for
   * more than MAX_HOLD_TIME.
   */
  if (stop_timer || (m_button_timer_value > MAX_HOLD_TIME))
  {
    TimerStop(&m_button_timer);
    button_timer_is_running = false;
  } 
  else 
  {
    TimerStart(&m_button_timer, DEBOUNCE_FILTER_TIME_STEP);
  }
}


/**
 * Push button debounce filtering.
 *
 * @param btn       Button identifier.
 * @param new_state The new button state reported by the interrupt handler.
 * @param is_called_from_isr true if this function is called from an
 *                           interrupt handler. False otherwise.
 */
static uint32_t PushbuttonDebounceFilter(button_id_t btn,
                                         button_state_t new_state,
                                         bool is_called_from_isr)
{
  DPRINTF("\nbtn: %u, new_state: %u, prev_state: %u\n", btn, new_state, m_button_state[btn]);

  if (BUTTON_DOWN == new_state)
  {
    if (BUTTON_UP == m_button_state[btn])
    {
      /* Button was UP and now it's DOWN. We need the button timer for debounce
       * handling. Start it if it's not already running
       */
      if (is_called_from_isr)
      {
        if(!button_timer_is_running)
        {
          m_button_timer_value = 0;
          TimerStartFromISR(&m_button_timer, DEBOUNCE_FILTER_TIME_STEP);
          button_timer_is_running = true;
        }
      }
      else
      {
        if (!TimerIsActive(&m_button_timer))
        {
          m_button_timer_value = 0;
          TimerStart(&m_button_timer, DEBOUNCE_FILTER_TIME_STEP);
          button_timer_is_running = true;
        }
      }
      /* Record the new state and the time button was pressed */
      m_button_state[btn] = BUTTON_DOWN;
      m_button_down_time[btn] = m_button_timer_value;
    }
  }
  else if (BUTTON_UP != m_button_state[btn])
  {
    /* Button was DOWN (or HOLD) and now it's UP */
    m_button_state[btn] = BUTTON_UP;

    /* Retun the number of ms the button was down */
    return (m_button_timer_value - m_button_down_time[btn]);
  }
  return 0;
}


/**
 * Button and slider event dispatch function
 *
 * Called by GPIO interrupt as configured by @ref Board_EnableButton().
 * Dispatches the event to the appropriate button type event handler.
 *
 * Uses the button GPIO pin configuration as defined in
 * extension_board_xxx.h or radio_board_xxx.h.
 *
 * @param intNo  Interrupt number of the GPIO causing the interrupt.
 */
static void ButtonPressCallback(uint8_t intNo)
{
  /* Called from interrupt */
  switch (intNo)
  {
    #if defined(PB1_GPIO_PIN)
    case PB1_INT_NO:
      Board_PushbuttonEventHandler(BOARD_BUTTON_PB1);
      break;
    #endif

    #if defined(PB2_GPIO_PIN)
    case PB2_INT_NO:
      Board_PushbuttonEventHandler(BOARD_BUTTON_PB2);
      break;
    #endif

    #if defined(PB3_GPIO_PIN)
    case PB3_INT_NO:
      Board_PushbuttonEventHandler(BOARD_BUTTON_PB3);
      break;
    #endif

    #if defined(PB4_GPIO_PIN)
    case PB4_INT_NO:
      Board_PushbuttonEventHandler(BOARD_BUTTON_PB4);
      break;
    #endif

    #if defined(PB5_GPIO_PIN)
    case PB5_INT_NO:
      Board_PushbuttonEventHandler(BOARD_BUTTON_PB5);
      break;
    #endif

    #if defined(PB6_GPIO_PIN)
    case PB6_INT_NO:
      Board_PushbuttonEventHandler(BOARD_BUTTON_PB6);
      break;
    #endif

    #if defined(SLIDER1_GPIO_PIN)
    case SLIDER1_INT_NO:
      Board_SliderEventHandler(BOARD_BUTTON_SLIDER1);
      break;
    #endif

    default:
      break;
  }
}


static uint32_t GetEm4Pin(GPIO_Port_TypeDef port, uint32_t pin)
{
  uint32_t wakeup_pin_mask = 0;
#ifdef ZWAVE_SERIES_800
  if ( GPIO_EM4WU0_PORT == port && GPIO_EM4WU0_PIN == pin )
  {
    wakeup_pin_mask = GPIO_IEN_EM4WUIEN0;
  }
  else if ( GPIO_EM4WU3_PORT == port && GPIO_EM4WU3_PIN == pin )
  {
    wakeup_pin_mask = GPIO_IEN_EM4WUIEN3;
  }
#if defined(GPIO_EM4WU4_PORT) && defined(GPIO_EM4WU4_PIN)
  else if ( GPIO_EM4WU4_PORT == port && GPIO_EM4WU4_PIN == pin )
  {
    wakeup_pin_mask = GPIO_IEN_EM4WUIEN4;
  }
#endif
  else if ( GPIO_EM4WU6_PORT == port && GPIO_EM4WU6_PIN == pin )
  {
    wakeup_pin_mask = GPIO_IEN_EM4WUIEN6;
  }
  else if ( GPIO_EM4WU7_PORT == port && GPIO_EM4WU7_PIN == pin )
  {
    wakeup_pin_mask = GPIO_IEN_EM4WUIEN7;
  }
  else if ( GPIO_EM4WU8_PORT == port && GPIO_EM4WU8_PIN == pin )
  {
    wakeup_pin_mask = GPIO_IEN_EM4WUIEN8;
  }
  else if ( GPIO_EM4WU9_PORT == port && GPIO_EM4WU9_PIN == pin )
  {
    wakeup_pin_mask = GPIO_IEN_EM4WUIEN9;
  }
#if defined(GPIO_EM4WU10_PORT) && defined(GPIO_EM4WU10_PIN)
  else if ( GPIO_EM4WU10_PORT == port && GPIO_EM4WU10_PIN == pin )
  {
    wakeup_pin_mask = GPIO_IEN_EM4WUIEN10;
  }
#endif
#else
  switch(port)
  {
    case gpioPortA:
      if (3 == pin)
      {
        wakeup_pin_mask = GPIO_EXTILEVEL_EM4WU8;
      }
      break;
    case gpioPortB:
      if (13 == pin)
      {
        wakeup_pin_mask = GPIO_EXTILEVEL_EM4WU9;
      }
      break;
    case gpioPortC:
      if (10 == pin)
      {
        wakeup_pin_mask = GPIO_EXTILEVEL_EM4WU12;
      }
      break;
    case gpioPortD:
      if (14 == pin)
      {
        wakeup_pin_mask = GPIO_EXTILEVEL_EM4WU4;
      }
      break;
    case gpioPortF:
      if (2 == pin)
      {
        wakeup_pin_mask = GPIO_EXTILEVEL_EM4WU0;
      }
      else if (7 == pin)
      {
        wakeup_pin_mask = GPIO_EXTILEVEL_EM4WU1;
      }
      break;
    default:
      break;
  }
#endif
  return wakeup_pin_mask;
}

/**
 * Configures a GPIO pin so it can wake up the system from deep sleep (EM4).
 *
 * Only available for a subset of pins.
 *
 * @param port      Port number
 * @param pin       Pin number
 * @param on_value  The pin value (0 or 1) that should trigger the wakeup.
 * @return          True if deep sleep wake up is possible for the
 *                  specified pin. Returns false otherwise.
 */
static bool ButtonEnableEM4PinWakeup(GPIO_Port_TypeDef port, uint32_t pin, uint32_t on_value)
{
  uint32_t wakeup_pin_mask = GetEm4Pin(port, pin);
  if (wakeup_pin_mask != 0)
  {
    uint32_t polarity_mask = 0;

    if (1 == on_value)
    {
      polarity_mask = wakeup_pin_mask;
    }

    GPIO_EM4EnablePinWakeup(wakeup_pin_mask , polarity_mask);
    return true;
  }
  return false;
}



/**
 * @brief Used to indicate that an assertion happened and as Default_Handler on non-UC builds
 * 
 */
NO_RETURN static void 
Board_DefaultHandler() {
  while(1) {
    Board_SetLed(BOARD_LED2, LED_OFF);
    Board_SetLed(BOARD_LED3, LED_ON);

    for(int i=0; i< 1000000; i++) ;
    Board_SetLed(BOARD_LED2, LED_ON);
    Board_SetLed(BOARD_LED3, LED_OFF);

    for(int i=0; i< 1000000; i++) ;
  }
}

/****************************************************************************/
/*                            PUBLIC FUNCTIONS                              */
/*                        (documented in board.h)                           */
/****************************************************************************/

/* --------------------------------- LEDS --------------------------------- */

led_type_t Board_GetLedType(led_id_t led)
{
  return (Board_IsLedAvailable_WithAssert(led)) ? m_led_info[led].type : LED_MONO;
}

const char * Board_GetLedLabel(led_id_t led)
{
  return (Board_IsLedAvailable_WithAssert(led)) ? m_led_info[led].label : "";
}

/* Should ideally be a statuc function, but board_indicator.c needs to use it. */
uint32_t Board_GetLedOnValue(led_id_t led)
{
  return (Board_IsLedAvailable_WithAssert(led)) ? m_led_info[led].on_value : 1;
}

/* Should ideally be a static function, but board_indicator.c needs to use it. */
uint32_t Board_GetLedLeTimerLoc(led_id_t led)
{
  return (Board_IsLedAvailable_WithAssert(led)) ? m_led_info[led].letimer_loc : 0;
}

void Board_ConfigLed(led_id_t led, bool enable)
{
  if (Board_IsLedAvailable(led))
  {
    GPIO_Port_TypeDef port     = Board_GetLedPort(led);
    uint32_t          pin      = Board_GetLedPin(led);
    uint32_t          on_value = Board_GetLedOnValue(led);
    GPIO_Mode_TypeDef mode     = gpioModeInput; // Defaults to input i.e. LED is disabled

    /* GPIO pin should pull to OFF value */
    uint32_t out = (1 == on_value) ? 0 : 1;

    if (enable)
    {
      mode = gpioModePushPull;  // Set pin as output
    }

//     DPRINTF("GPIO_PinModeSet: led:%u, port:%u, pin:%u, mode:0x%x, out:%u\n", led, port, pin, mode, out);
    GPIO_PinModeSet(port, pin, mode, out);
  }
}

void Board_SetLed(led_id_t led, led_state_t state)
{
  if (Board_IsLedAvailable(led))
  {
    GPIO_Port_TypeDef port     = Board_GetLedPort(led);
    uint32_t          pin      = Board_GetLedPin(led);
    uint32_t          on_value = Board_GetLedOnValue(led);

    if ((LED_ON == state && 1 == on_value) ||
        (LED_OFF == state && 0 == on_value))
    {
      GPIO_PinOutSet(port, pin);
    }
    else
    {
      GPIO_PinOutClear(port, pin);
    }
  }
}

void Board_RgbLedInitPwmTimer(uint32_t maxCompareLevel)
{
#if !defined(ZWAVE_SERIES_800)
  CMU_ClockEnable(cmuClock_TIMER1, true);

  TIMER_TopSet(TIMER1, maxCompareLevel);

  // Set the PWM duty cycle to zero for all channels!
  TIMER_CompareBufSet(TIMER1, 0, 0);
  TIMER_CompareBufSet(TIMER1, 1, 0);
  TIMER_CompareBufSet(TIMER1, 2, 0);
  TIMER_CompareBufSet(TIMER1, 3, 0);

  // Create a timerInit object, based on the API default
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

  TIMER_Init(TIMER1, &timerInit);
#else
  UNUSED(maxCompareLevel);  
#endif /* !defined(ZWAVE_SERIES_800) */
}

void Board_RgbLedEnablePwm(led_id_t led)
{
  if (Board_GetLedType(led) != LED_RGB)
  {
    return;
  }

#ifndef ZWAVE_SERIES_800 // Not yet ported to 800 series
  uint32_t on_value = Board_GetLedOnValue(led);

  GPIO_Port_TypeDef port = Board_GetLedPort(led);
  uint32_t          pin  = Board_GetLedPin(led);

  uint32_t location = 0;

  // Create the timer count control object initializer
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode      = timerCCModePWM;
  timerCCInit.cmoa      = timerOutputActionToggle;
  timerCCInit.outInvert = (1 == on_value) ? false : true;

  switch (led)
  {
    case BOARD_RGB1_R:
      // Configure CC channel 1
      TIMER_InitCC(TIMER1, 1, &timerCCInit);
      location = GetTim1CcLocation(port, pin, 1);
      TIMER1->ROUTELOC0 = (TIMER1->ROUTELOC0 & ~_TIMER_ROUTELOC0_CC1LOC_MASK) | location;
      TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC1PEN;
      break;
    case BOARD_RGB1_G:
      // Configure CC channel 2
      TIMER_InitCC(TIMER1, 2, &timerCCInit);
      location = GetTim1CcLocation(port, pin, 2);
      TIMER1->ROUTELOC0 = (TIMER1->ROUTELOC0 & ~_TIMER_ROUTELOC0_CC2LOC_MASK) | location;
      TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC2PEN;
      break;
    case BOARD_RGB1_B:
      // Configure CC channel 3
      TIMER_InitCC(TIMER1, 3, &timerCCInit);
      location = GetTim1CcLocation(port, pin, 3);
      TIMER1->ROUTELOC0 = (TIMER1->ROUTELOC0 & ~_TIMER_ROUTELOC0_CC3LOC_MASK) | location;
      TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC3PEN;
      break;
    default:
      break;
  }
#endif
}

void Board_RgbLedDisablePwm(led_id_t led)
{
  if (Board_GetLedType(led) != LED_RGB)
  {
    return;
  }

#ifndef ZWAVE_SERIES_800 // Not yet ported to 800 series
  /*
   * NB: TIMER_ROUTEPEN_CCxPEN should be aligned with the channel
   *     selected in Board_RgbLedEnablePwm(). (ideally ROUTEPEN should
   *     be configurable instead of fixed/hardcoded)
   */
  switch (led)
  {
    case BOARD_RGB1_R:
      TIMER1->ROUTEPEN &= ~TIMER_ROUTEPEN_CC1PEN;
      break;
    case BOARD_RGB1_G:
      TIMER1->ROUTEPEN &= ~TIMER_ROUTEPEN_CC2PEN;
      break;
    case BOARD_RGB1_B:
      TIMER1->ROUTEPEN &= ~TIMER_ROUTEPEN_CC3PEN;
      break;
    default:
      break;
  }
#endif
}

void Board_RgbLedSetPwm(led_id_t led, uint32_t pwm)
{

#ifndef ZWAVE_SERIES_800 // Not yet ported to 800 series
  if (Board_GetLedType(led) != LED_RGB)
  {
    return;
  }

  switch(led)
  {
    case BOARD_RGB1_R:
      TIMER_CompareBufSet(TIMER1, 1, pwm);
      break;
    case BOARD_RGB1_G:
      TIMER_CompareBufSet(TIMER1, 2, pwm);
      break;
    case BOARD_RGB1_B:
      TIMER_CompareBufSet(TIMER1, 3, pwm);
      break;
    default:
      break;
  }
#else
  UNUSED(led);
  UNUSED(pwm);
#endif
}

/* ------------------------------- BUTTONS -------------------------------- */

uint32_t Board_GetGpioEm4Flags(void)
{
  return g_gpioEm4Flags;
}

button_type_t Board_GetButtonType(button_id_t btn)
{
  return (Board_IsButtonAvailable_WithAssert(btn)) ? m_button_info[btn].type : BUTTON_TYPE_PUSHBUTTON;
}

const char * Board_GetButtonLabel(button_id_t btn)
{
  return (Board_IsButtonAvailable_WithAssert(btn)) ? m_button_info[btn].label : "";
}

BUTTON_EVENT Board_GetButtonEventBase(button_id_t btn)
{
  return (Board_IsButtonAvailable_WithAssert(btn)) ? m_button_info[btn].event_base : DEFINE_EVENT_KEY_NBR;
}

button_id_t Board_GetButtonId(GPIO_Port_TypeDef port, uint32_t pin)
{
  for (button_id_t btn = 0; btn < BOARD_BUTTON_COUNT; btn++)
  {
    if (Board_IsButtonAvailable(btn))
    {
      if (Board_GetButtonPort(btn) == port && Board_GetButtonPin(btn) == pin)
      {
        return btn;
      }
    }
  }
  return BOARD_BUTTON_INVALID_ID;
}

void Board_EnableButton(button_id_t btn)
{
  static bool gpioInterruptEnabled = false;

  DPRINTF("Board_EnableButton: %u\n", btn);

  if (Board_IsButtonAvailable(btn))
  {
    if (false == gpioInterruptEnabled)
    {
#ifdef ZWAVE_SERIES_800
      radio_power_lock = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);
#endif
      // No auto-reload so the timer doesn't flood the app task with
      // notifications every 10 ms if the app task doesn't get executed
      AppTimerRegister(&m_button_timer, false, ButtonTimerCallback);
      GPIOINT_Init();
      NVIC_SetPriority(GPIO_ODD_IRQn, 5);
      NVIC_SetPriority(GPIO_EVEN_IRQn, 5);
      button_timer_is_running = false;

      gpioInterruptEnabled = true;
    }

    GPIO_Port_TypeDef port     = Board_GetButtonPort(btn);
    uint32_t          pin      = Board_GetButtonPin(btn);
    uint32_t          on_value = Board_GetButtonOnValue(btn);
    uint8_t           intNo    = Board_GetButtonIntNo(btn);

    /* GPIO pin should pull to OFF value */
    uint32_t          out  = (1 == on_value) ? 0 : 1;

    GPIO_PinModeSet(port, pin, gpioModeInputPullFilter, out);

    m_button_state[btn] = BUTTON_UP;
    m_button_down_time[btn] = 0;

#ifdef ZWAVE_SERIES_800
     uint32_t pinMask =  GetEm4Pin(port, pin);
    if (Board_GetButtonCanWakeupEm4(btn))
    {
      ButtonEnableEM4PinWakeup(port, pin, on_value);
    // since only pins on port a and b can function normally in em2 we need to setup
    // the gpios that can wake from em4 as level triggered so they can generates interrupt from em2
      GPIO->IEN_SET = pinMask;          /*enable interrupt generation on em4 wakeup pins*/      
    }
     GPIO_ExtIntConfig(port, pin, intNo, true, true, true);      
#else
    if (Board_GetButtonCanWakeupEm4(btn))
    {
      ButtonEnableEM4PinWakeup(port, pin, on_value);
    }
    /* Interrupt num is equal to pin num */
    GPIO_ExtIntConfig(port, pin, intNo, true, true, true);
#endif
    GPIOINT_CallbackRegister(intNo, ButtonPressCallback);
  }
}

void Board_DisableButton(button_id_t btn)
{
  if (Board_IsButtonAvailable(btn))
  {
    uint32_t pin = Board_GetButtonPin(btn);

    GPIO_IntDisable(1 << pin);
  }
}

button_state_t Board_GetButtonState(button_id_t btn)
{
  if (Board_IsButtonAvailable(btn))
  {
    GPIO_Port_TypeDef port     = Board_GetButtonPort(btn);
    uint32_t          pin      = Board_GetButtonPin(btn);
    uint32_t          on_value = Board_GetButtonOnValue(btn);
    uint32_t          btn_val  = GPIO_PinInGet(port, pin);

    if (btn_val == on_value)
    {
      return BUTTON_DOWN;
    }
  }
  return BUTTON_UP;
}

void Board_PushbuttonEventHandler(button_id_t btn)
{
#ifdef ZWAVE_SERIES_800
  zpal_pm_stay_awake(radio_power_lock, 0);
#endif
  Board_PushbuttonEventHandlerEx(btn, true, false);
}

void Board_PushbuttonEventHandlerEx(button_id_t btn,
                                    bool is_called_from_isr,
                                    bool was_activated_in_em4)
{
  if (true == Board_GetButtonCanWakeupEm4(btn))
  {
#ifdef ZWAVE_SERIES_800
    GPIO_IntClear(_GPIO_EM4WUPOL_MASK); // Clear interrupts from EM4
#else
    GPIO_IntClear(_GPIO_EXTILEVEL_MASK); // Clear interrupts from EM4
#endif
  }

  if (BUTTON_DOWN == Board_GetButtonState(btn))  // Button pressed down
  {
    PushbuttonDebounceFilter(btn, BUTTON_DOWN, is_called_from_isr);
  }
  else
  {
    /* We got here because either
     * a) the button was pressed DOWN (briefly) while in EM4 causing the
     *    device to wake up but now it is UP (the UP event happened during
     *    startup from EM4 and was not registered).
     *    Here we simply send a SHORT_PRESS event to the application.
     * --OR--
     * b) the button returned from DOWN to UP state.
     *    Here we give the event the full de-bounce treatment.
     */

    BUTTON_EVENT btn_event_base = Board_GetButtonEventBase(btn);

    if (was_activated_in_em4)
    {
      DPRINTF("btn_id: %u event_base: %u SHORT_PRESS\n", btn, btn_event_base);
      SendEventToApp(EV_BTN_EVENT_SHORT_PRESS(btn_event_base), is_called_from_isr);
    }
    else
    {
      uint32_t btn_down_duration = PushbuttonDebounceFilter(btn, BUTTON_UP, is_called_from_isr);

      if (BUTTON_LONG_PRESS_DURATION < btn_down_duration)
      {
        DPRINTF("btn_id: %u event_base: %u LONG_PRESS\n", btn, btn_event_base);
        SendEventToApp(EV_BTN_EVENT_LONG_PRESS(btn_event_base), is_called_from_isr);
      }
      else if (BUTTON_SHORT_PRESS_DURATION < btn_down_duration)
      {
        if (BUTTON_HOLD_DURATION > btn_down_duration)
        {
          DPRINTF("btn_id: %u event_base: %u SHORT_PRESS\n", btn, btn_event_base);
          SendEventToApp(EV_BTN_EVENT_SHORT_PRESS(btn_event_base), is_called_from_isr);
        }
        else
        {
          DPRINTF("btn_id: %u event_base: %u UP\n", btn, btn_event_base);
          SendEventToApp(EV_BTN_EVENT_UP(btn_event_base), is_called_from_isr);
        }
      } else {
        /*if the BUTTON_UP event occured before the debounce period expired then we cancel the power lock*/
#ifdef ZWAVE_SERIES_800
        zpal_pm_cancel(radio_power_lock);
#endif
      }
    }
  }
}

void Board_SliderEventHandler(button_id_t btn)
{
#ifdef ZWAVE_SERIES_800
  zpal_pm_stay_awake(radio_power_lock, 0);
#endif
  Board_SliderEventHandlerEx(btn, true, false);
}

void Board_SliderEventHandlerEx(button_id_t btn,
                                bool is_called_from_isr,
                                bool was_activated_in_em4)
{
  /* Currently only one slider is supported - hence we only
   * have one lock variable
   */
  static bool event_lock = false;

  DPRINTF("SliderEventHandler btn_id: %u\n", btn);

  if (is_called_from_isr)
  {
    if (true == Board_GetButtonCanWakeupEm4(btn))
    {
#ifdef ZWAVE_SERIES_800
    GPIO_IntClear(_GPIO_EM4WUPOL_MASK); // Clear interrupts from EM4
#else
    GPIO_IntClear(_GPIO_EXTILEVEL_MASK); // Clear interrupts from EM4
#endif
    }
  }

  if (BUTTON_DOWN == Board_GetButtonState(btn))
  {
    if (!event_lock)
    {
      DPRINTF("btn_id: %u event: %u DOWN\n", btn, BTN_EVENT_DOWN(btn));
      SendEventToApp(BTN_EVENT_DOWN(btn), is_called_from_isr);
      event_lock = true;
    }
  }
  else
  {
    if (was_activated_in_em4)
    {
      /* The slider was (very briefly) activated while in EM4. Now it is no
       * longer active, but to tell the application the full story we send
       * it the DOWN event before the UP event.
       */
      DPRINTF("btn_id: %u event: %u DOWN\n", btn, BTN_EVENT_DOWN(btn));
      SendEventToApp(BTN_EVENT_DOWN(btn), is_called_from_isr);
    }

    DPRINTF("btn_id: %u event: %u UP\n", btn, BTN_EVENT_UP(btn));
    SendEventToApp(BTN_EVENT_UP(btn), is_called_from_isr);
    event_lock = false;
  }
}

/* ---------------------- GPIO ACCESS VIA SERIAL API ---------------------- */

void Board_GPIO_PinModeSet(GPIO_Port_TypeDef port,
                           unsigned int pin,
                           GPIO_Mode_TypeDef mode,
                           unsigned int out)
{
  GPIO_PinModeSet(port, pin, mode, out);
}


void Board_GPIO_PinOutClear(GPIO_Port_TypeDef port, unsigned int pin)
{
  GPIO_PinOutClear(port, pin);
}


void Board_GPIO_PinOutSet(GPIO_Port_TypeDef port, unsigned int pin)
{
  GPIO_PinOutSet(port, pin);
}

/* ------------------------------ BOARD INIT ------------------------------ */

uint32_t Board_Initialize()
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  m_button_timer_value = 0;

  g_gpioEm4Flags = getWakeUpFlags();

  for (uint32_t led = 0; led < BOARD_LED_COUNT; led++)
  {
    Board_ConfigLed(led, true);
  }

  Assert_SetCb(&Board_DefaultHandler);

  return 0;
}

void Board_SetButtonCallback(Board_button_callback_t callback)
{
  m_button_callback = callback;
}

button_id_t Board_GetButtonIdFromEm4PinWakeupFlags(uint32_t *wakeup_pin_flags)
{
  GPIO_Port_TypeDef port;
  uint32_t          pin = 0;

  /* Multiple flags can be set - here we simply clear the first flag we
   * encounter and return the associated button.
   * Call this function multiple times (with the modified flag variable)
   * to process all flags
   */

#ifdef ZWAVE_SERIES_800
  if ((NULL != wakeup_pin_flags) && (0 != *wakeup_pin_flags))
  {
    if (*wakeup_pin_flags & GPIO_IEN_EM4WUIEN0)
    {
      port = GPIO_EM4WU0_PORT;
      pin  = GPIO_EM4WU0_PIN;
      *wakeup_pin_flags &= ~GPIO_IEN_EM4WUIEN0;
    }
    else if (*wakeup_pin_flags & GPIO_IEN_EM4WUIEN3)
    {
      port = GPIO_EM4WU3_PORT;
      pin  = GPIO_EM4WU3_PIN;
      *wakeup_pin_flags &= ~GPIO_IEN_EM4WUIEN3;
    }
#if defined(GPIO_EM4WU4_PORT) && defined(GPIO_EM4WU4_PIN) && defined(GPIO_IEN_EM4WUIEN4)
    else if (*wakeup_pin_flags & GPIO_IEN_EM4WUIEN4)
    {
      port = GPIO_EM4WU4_PORT;
      pin  = GPIO_EM4WU4_PIN;
      *wakeup_pin_flags &= ~GPIO_IEN_EM4WUIEN4;
    }
#endif
    else if (*wakeup_pin_flags & GPIO_IEN_EM4WUIEN6)
    {
      port = GPIO_EM4WU6_PORT;
      pin  = GPIO_EM4WU6_PIN;
      *wakeup_pin_flags &= ~GPIO_IEN_EM4WUIEN6;
    }
    else if (*wakeup_pin_flags & GPIO_IEN_EM4WUIEN7)
    {
      port = GPIO_EM4WU7_PORT;
      pin  = GPIO_EM4WU7_PIN;
      *wakeup_pin_flags &= ~GPIO_IEN_EM4WUIEN7;
    }
    else if (*wakeup_pin_flags & GPIO_IEN_EM4WUIEN8)
    {
      port = GPIO_EM4WU8_PORT;
      pin  = GPIO_EM4WU8_PIN;
      *wakeup_pin_flags &= ~GPIO_IEN_EM4WUIEN8;
    }
    else if (*wakeup_pin_flags & GPIO_IEN_EM4WUIEN9)
    {
      port = GPIO_EM4WU9_PORT;
      pin  = GPIO_EM4WU9_PIN;
      *wakeup_pin_flags &= ~GPIO_IEN_EM4WUIEN9;
    }
#if defined(GPIO_EM4WU10_PORT) && defined(GPIO_EM4WU10_PIN) && defined(GPIO_IEN_EM4WUIEN10)
    else if (*wakeup_pin_flags & GPIO_IEN_EM4WUIEN10)
    {
      port = GPIO_EM4WU10_PORT;
      pin  = GPIO_EM4WU10_PIN;
      *wakeup_pin_flags &= ~GPIO_IEN_EM4WUIEN10;
    }
#endif
    if (0 != pin)
    {
      return Board_GetButtonId(port, pin);
    }
  }
  return BOARD_BUTTON_INVALID_ID;

#else

  if ((NULL != wakeup_pin_flags) && (0 != *wakeup_pin_flags))
  {
    if (*wakeup_pin_flags & GPIO_EXTILEVEL_EM4WU8)
    {
      port = gpioPortA;
      pin  = 3;
      *wakeup_pin_flags &= ~GPIO_EXTILEVEL_EM4WU8;
    }
    else if (*wakeup_pin_flags & GPIO_EXTILEVEL_EM4WU9)
    {
      port = gpioPortB;
      pin  =  13;
      *wakeup_pin_flags &= ~GPIO_EXTILEVEL_EM4WU9;
    }
    else if (*wakeup_pin_flags & GPIO_EXTILEVEL_EM4WU12)
    {
      port = gpioPortC;
      pin  = 10;
      *wakeup_pin_flags &= ~GPIO_EXTILEVEL_EM4WU12;
    }
    else if (*wakeup_pin_flags & GPIO_EXTILEVEL_EM4WU4)
    {
      port = gpioPortD;
      pin  = 14;
      *wakeup_pin_flags &= ~GPIO_EXTILEVEL_EM4WU4;
    }
    else if (*wakeup_pin_flags & GPIO_EXTILEVEL_EM4WU0)
    {
      port = gpioPortF;
      pin  = 2;
      *wakeup_pin_flags &= ~GPIO_EXTILEVEL_EM4WU0;
    }
    else if (*wakeup_pin_flags & GPIO_EXTILEVEL_EM4WU1)
    {
      port = gpioPortF;
      pin  = 7;
      *wakeup_pin_flags &= ~GPIO_EXTILEVEL_EM4WU1;
    }

    if (0 != pin)
    {
      return Board_GetButtonId(port, pin);
    }
  }
  return BOARD_BUTTON_INVALID_ID;
#endif
}

bool Board_DebugPrintEm4WakeupFlags(uint32_t wakeup_pin_flags)
{
  while (0 != wakeup_pin_flags)
  {
    /* Multiple buttons could have been activated causing multiple
     * wakeup pin flags to be set. On every call
     * Board_GetButtonIdFromEm4PinWakeupFlags() clears the bit
     * corresponding to the returned button id.
     */
    button_id_t btn = Board_GetButtonIdFromEm4PinWakeupFlags(&wakeup_pin_flags);
    if (BOARD_BUTTON_INVALID_ID != btn)
    {
      DPRINTF("Wakeup button = %s (%d)\n", Board_GetButtonLabel(btn), btn);
    }
    else
    {
      return false;
    }
  }
  return true;
}

bool Board_ProcessEm4PinWakeupFlags(uint32_t wakeup_pin_flags)
{
  /* More than one button could essentially have been causing the EM4
   * wakeup resulting in multiple non-zero bits in wakeup_pin_flags.
   *
   * Here we only call Board_GetButtonIdFromEm4PinWakeupFlags() once
   * to fetch the first button. Must be called multiple times if we
   * want to process multiple buttons.
   */

  button_id_t em4_wakeup_btn = Board_GetButtonIdFromEm4PinWakeupFlags(&wakeup_pin_flags);
  if (BOARD_BUTTON_INVALID_ID != em4_wakeup_btn)
  {
    /* Trigger the de-bounce processing to get proper button event
     * reporting to the application.
     * NB: If the em4_wakeup_btn is no longer down (following a very quick
     *     press/release) the button event handler will not do anything.
     */

    if (Board_GetButtonType(em4_wakeup_btn) == BUTTON_TYPE_SLIDER)
    {
      Board_SliderEventHandlerEx(em4_wakeup_btn, false, true);
    }
    else
    {
      Board_PushbuttonEventHandlerEx(em4_wakeup_btn, false, true);
    }
    return true;
  }
  return false;
}

void Board_ResetHandler(void)
{
  NVIC_SystemReset();
}

#ifdef ZWAVE_SERIES_800
void HardFault_Handler() {
  while(1) {
    Board_SetLed(BOARD_LED2, LED_OFF);
    for(int i=0; i< 1000000; i++) ;
    Board_SetLed(BOARD_LED2, LED_ON);
    for(int i=0; i< 1000000; i++) ;
  }
}
#endif
