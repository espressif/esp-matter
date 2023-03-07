/**
 * @file
 *
 * Board handling definitions and functions.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdbool.h>
#include "em_gpio.h"
#include "ev_man.h"
#include "em_cmu.h"
#include "target_boards.h"

/*************************************************************************/
/* TYPES AND MACROS                                                      */
/*************************************************************************/

/**
 * Convenience macros to derive all button event types from the DOWN event
 * in @ref BUTTON_EVENT for a specific button.
 *
 * NB: The EV_BTN_EVENT_xxx macros MUST be aligned with enum @ref BUTTON_EVENT
 *     in ev_man.h
 */
#define EV_BTN_EVENT_DOWN(event_base)        (event_base)
#define EV_BTN_EVENT_UP(event_base)          ((event_base)+1)
#define EV_BTN_EVENT_SHORT_PRESS(event_base) ((event_base)+2)
#define EV_BTN_EVENT_HOLD(event_base)        ((event_base)+3)
#define EV_BTN_EVENT_LONG_PRESS(event_base)  ((event_base)+4)

#define BTN_EVENT_DOWN(btn)        (EV_BTN_EVENT_DOWN(Board_GetButtonEventBase(btn)))
#define BTN_EVENT_UP(btn)          (EV_BTN_EVENT_UP(Board_GetButtonEventBase(btn)))
#define BTN_EVENT_SHORT_PRESS(btn) (EV_BTN_EVENT_SHORT_PRESS(Board_GetButtonEventBase(btn)))
#define BTN_EVENT_HOLD(btn)        (EV_BTN_EVENT_HOLD(Board_GetButtonEventBase(btn)))
#define BTN_EVENT_LONG_PRESS(btn)  (EV_BTN_EVENT_LONG_PRESS(Board_GetButtonEventBase(btn)))

/**
 * Led identifier type
 */
typedef enum
{
  BOARD_LED1,
  BOARD_LED2,
  BOARD_LED3,
  BOARD_LED4,
  BOARD_RGB1_R,
  BOARD_RGB1_G,
  BOARD_RGB1_B,
  BOARD_LED_COUNT
} led_id_t;


/**
 * Led state type
 */
typedef enum
{
  LED_ON,
  LED_OFF
} led_state_t;


/**
 * Button identifier type
 */
typedef enum
{
  BOARD_BUTTON_PB1,
  BOARD_BUTTON_PB2,
  BOARD_BUTTON_PB3,
  BOARD_BUTTON_PB4,
  BOARD_BUTTON_PB5,
  BOARD_BUTTON_PB6,
  BOARD_BUTTON_SLIDER1,
  BOARD_BUTTON_COUNT,
  BOARD_BUTTON_INVALID_ID
} button_id_t;

/**
 * Button types
 */
typedef enum
{
  BUTTON_TYPE_PUSHBUTTON,
  BUTTON_TYPE_SLIDER
} button_type_t;


/**
 * Button state type
 */
typedef enum
{
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_HOLD
} button_state_t;


/**
 * Button error type
 */
typedef enum _BOARD_ERROR_
{
  BOARD_ERROR_NVM
} BOARD_ERROR;

typedef void (*Board_button_callback_t)(BUTTON_EVENT event, bool is_called_from_isr);

/*************************************************************************/
/* FUNCTIONS                                                             */
/*************************************************************************/

/**
 * Setup of input/output pins, interrupts, UARTs etc.
 */
uint32_t Board_Initialize(void);

/**
 * Set buttons callback
 */
void Board_SetButtonCallback(Board_button_callback_t callback);

/**
 * Configures a gpio pin
 *
 * Should only be used by serial API.
 *
 * @param port    Port number
 * @param pin     Pin number
 * @param mode    Config of input or output
 * @param out     Default output
 */
void Board_GPIO_PinModeSet(GPIO_Port_TypeDef port,
                           unsigned int pin,
                           GPIO_Mode_TypeDef mode,
                           unsigned int out);

/**
 * Pulls a gpio pin to ground voltage
 *
 * Should only be used by serial API.
 *
 * @param port    Port number
 * @param pin     Pin number
 */
void Board_GPIO_PinOutClear(GPIO_Port_TypeDef port, unsigned int pin);

/**
 * Pulls a gpio pin to high voltage
 *
 * Should only be used by serial API.
 *
 * @param port    Port number
 * @param pin     Pin number
 */
void Board_GPIO_PinOutSet(GPIO_Port_TypeDef port, unsigned int pin);

/**
 * Init setup function for the BRD420x boards
 *
 *@param rfRegion RF country/region setting
 *@return         Setup success (true/false)
 */
uint32_t BRD420xBoardInit(uint8_t rfRegion);

/* -------------------------- LED FUNCTIONS ---------------------------- */

uint32_t Board_GetLedOnValue(led_id_t led);

uint32_t Board_GetLedLeTimerLoc(led_id_t led);

/**
 * Configures a LED for use.
 *
 * The LED GPIO port and pin configuration as defined in extension_board_xxx.h
 * or radio_board_xxx.h are used implicitly.
 *
 * @param  led     Led identifier.
 * @param  enable  Enable or disable the GPIO pin (set as output or input)
 */
void Board_ConfigLed(led_id_t led, bool enable);

/**
 * Get the label printed next to the LED on the PCB.
 * Can be used to print a list of pin assignments (in debug mode).
 *
 * @param  led  Led identifier.
 * @return      the label string
 */
const char * Board_GetLedLabel(led_id_t led);

/**
 * Set led on/off
 *
 * @param  led     Led identifier.
 * @param  state   The on/off state to set the led to.
 */
void Board_SetLed(led_id_t led, led_state_t state);

/**
 * Enables timer that is necessary for PWM.
 *
 * @param   maxCompareLevel   Number of levels used in PWM compare counter. 0-0xffff.
 */
void Board_RgbLedInitPwmTimer(uint32_t maxCompareLevel);

/**
 * Enables PWM for dimming on a LED output. Takes over the LED output
 * so calls to Board_SetLed() have no effect.
 *
 * @param  led   Led identifier.
 */
void Board_RgbLedEnablePwm(led_id_t led);

/**
 * Disables PWM for dimming on a LED output. Gives back control of the LED
 * output so calls to Board_SetLed() have effect.
 *
 * @param  led   Led identifier.
 */
void Board_RgbLedDisablePwm(led_id_t led);

/**
 * Set pulse width modulation for led.
 *
 * @param  led   Led identifier.
 * @param pwm    PWM duty cycle. Duty_cycle = pwm/level_max
 */
void Board_RgbLedSetPwm(led_id_t led, uint32_t pwm);

/**
 * Configure/initialize the indicator LED
 *
 * @note
 *   This function must *not* be called until *after* the queue to the
 *   protocol thread has been created (the indicator uses a power lock
 *   that is initialized by calling zpal_pm_register() which sends a
 *   message to the PM module in the protocol thread).
 *
 * @param led  The LED to use for the indicator.
 */
void Board_IndicatorInitLed(led_id_t led);

/* ------------------------- BUTTON FUNCTIONS -------------------------- */

/**
 * Get a bitmask containing GPIO pins (buttons) that woke up the device
 * from EM4.
 *
 * Can be used with @ref Board_ProcessEm4PinWakeupFlags().
 *
 * @return  GPIO EM4 wakeup flags
 */
uint32_t Board_GetGpioEm4Flags(void);

/**
 * Trigger button press handling from a bitmask containing GPIO pins that
 * woke up the device from EM4.
 *
 * The bitmask can be fetched with @ref Board_GetGpioEm4Flags().
 *
 * @return  false if the bitmask is empty or does not contain any known buttons.
 */
bool Board_ProcessEm4PinWakeupFlags(uint32_t wakeup_pin_flags);

/**
 * Decodes a bitmask containing GPIO pins (buttons) that woke up the device
 * from EM4.
 *
 * Simply DPRINTS the button label corresponding to the button(s) referenced
 * in the bitmask.
 *
 * The bitmask can be fetched with @ref Board_GetGpioEm4Flags().
 *
 * @return  false if the bitmask is empty or does not contain any known buttons.
 */
bool Board_DebugPrintEm4WakeupFlags(uint32_t wakeup_pin_flags);

/**
 * Get the label printed next to the button on the PCB.
 * Can be used to print a list of pin assignments (in debug mode).
 *
 * @param   btn Button identifier.
 * @return      the label string
 */
const char * Board_GetButtonLabel(button_id_t btn);

/**
 * Get the type of a button
 * Currently knows about push buttons and slider buttons.
 *
 * @param   btn Button identifier.
 * @return      Button type
 */
button_type_t Board_GetButtonType(button_id_t btn);

/**
 * Get the button id corresponding to a GPIO port and pin
 *
 * @param   port GPIO port
 * @param   pin  GPIO pin
 * @return       Button identifier (BOARD_BUTTON_INVALID_ID if unknown)
 */
button_id_t Board_GetButtonId(GPIO_Port_TypeDef port, uint32_t pin);

/**
 * Lookup the button id from a bitmask containing GPIO pins that woke up
 * the device from EM4.
 *
 * The bitmask can be fetched with @ref Board_GetGpioEm4Flags().
 *
 * @return  Button identifier.
 * @return  BOARD_BUTTON_INVALID_ID if the bitmask is empty or does not
 *          contain any known buttons.
 */
button_id_t Board_GetButtonIdFromEm4PinWakeupFlags(uint32_t *wakeup_pin_flags);

/**
 * @brief Returns the last returned event via the zaf_event_helper.h module!
 *
 * @attention The function delete the event, so subsequent calls will not be
 * able to fetch the event. For this reason, only one UserTask should ideally
 * use this function.
 *
 * @return  uint8_t representing @ref BUTTON_EVENT
 */
uint8_t Board_GetLatestBtnEvent(void);

/**
 * Enables interrupts for the specified button
 *
 * @param   btn Button identifier.
 */
void Board_EnableButton(button_id_t btn);

/**
 * Disables interrupts for the specified button
 *
 * @param   btn Button identifier.
 */
void Board_DisableButton(button_id_t btn);

/**
 * Get the state of a button
 *
 * @param   btn Button identifier.
 * @return      Button state as @ref button_state_t.
 */
button_state_t Board_GetButtonState(button_id_t btn);

/**
 * Get the button event base.
 *
 * The "event base" is the first @ref BUTTON_EVENT for the specified button
 * (currently the DOWN event). Used by the EV_BTN_EVENT_xxx() and
 * BTN_EVENT_xxx() macros.
 *
 * @param   btn Button identifier.
 * @return      First button event for the button.
 */
BUTTON_EVENT Board_GetButtonEventBase(button_id_t btn);

/**
 * Push button interrupt event handler function
 *
 * Sends SHORT_PRESS, DOWN, UP, or LONG_PRESS event to application.
 * To be used from GPIO IRQ callback functions.
 *
 * NB: HOLD events are emitted from ButtonCheckHold()
 *     (called from ButtonTimerCallback).
 *
 * @param btn                Button identifier.
 */
void Board_PushbuttonEventHandler(button_id_t btn);

/**
 * Push button event handler function
 *
 * Sends SHORT_PRESS, DOWN, UP, or LONG_PRESS event to application.
 *
 * NB: HOLD events are emitted from ButtonCheckHold()
 *     (called from ButtonTimerCallback).
 *
 * @param btn                  Button identifier.
 * @param is_called_from_isr   True if this function is called from an
 *                             interrupt handler. False otherwise.
 * @param was_activated_in_em4 True if the event handler is activated because
 *                             the button was pressed while sleeping in EM4.
 */
void Board_PushbuttonEventHandlerEx(button_id_t btn,
                                    bool is_called_from_isr,
                                    bool was_activated_in_em4);

/**
 * Slider interrupt event handler function
 *
 * Sends UP or DOWN event to application.
 * To be used from GPIO IRQ callback functions.
 *
 * @param btn                Button identifier.
 */
void Board_SliderEventHandler(button_id_t btn);

/**
 * Slider event handler function
 *
 * Sends UP or DOWN event to application.
 *
 * @param btn                  Button identifier.
 * @param is_called_from_isr   True if this function is called from an
 *                             interrupt handler. False otherwise.
 * @param was_activated_in_em4 True if the event handler is activated because the
 *                             slider was activated while sleeping in EM4.
 */
void Board_SliderEventHandlerEx(button_id_t btn,
                                bool is_called_from_isr,
                                bool was_activated_in_em4);

/*************************************************************************/
/* Below we'll create a number of convenience macros, so we can keep     */
/* preprocessor conditionals to a minimum in board.c                     */
/*************************************************************************/

/* -------------------------------- LEDS ------------------------------- */

#if defined(LED1_GPIO_PORT)
  #define LED1_AVAILABLE       true
#else
  #define LED1_AVAILABLE       false
  #define LED1_LABEL           "-"
  #define LED1_GPIO_PORT       101
  #define LED1_GPIO_PIN        201
  #define LED1_ON_VALUE        0
  #define LED1_LETIM0_OUT0_LOC 0
#endif

#if defined(LED2_GPIO_PORT)
  #define LED2_AVAILABLE       true
#else
  #define LED2_AVAILABLE       false
  #define LED2_LABEL           "-"
  #define LED2_GPIO_PORT       102
  #define LED2_GPIO_PIN        202
  #define LED2_ON_VALUE        0
  #define LED2_LETIM0_OUT0_LOC 0
#endif

#if defined(LED3_GPIO_PORT)
  #define LED3_AVAILABLE       true
#else
  #define LED3_AVAILABLE       false
  #define LED3_LABEL           "-"
  #define LED3_GPIO_PORT       103
  #define LED3_GPIO_PIN        203
  #define LED3_ON_VALUE        0
  #define LED3_LETIM0_OUT0_LOC 0
#endif

#if defined(LED4_GPIO_PORT)
  #define LED4_AVAILABLE       true
#else
  #define LED4_AVAILABLE       false
  #define LED4_LABEL           "-"
  #define LED4_GPIO_PORT       104
  #define LED4_GPIO_PIN        204
  #define LED4_ON_VALUE        0
  #define LED4_LETIM0_OUT0_LOC 0
#endif

#if !defined(RGB1_LABEL)
  #define RGB1_LABEL           "-"
#endif

#if defined(RGB1_R_GPIO_PORT)
  #define RGB1_R_AVAILABLE     true
#else
  #define RGB1_R_AVAILABLE     false
  #define RGB1_R_GPIO_PORT     111
  #define RGB1_R_GPIO_PIN      211
  #define RGB1_R_ON_VALUE      1
#endif

#if defined(RGB1_G_GPIO_PORT)
  #define RGB1_G_AVAILABLE     true
#else
  #define RGB1_G_AVAILABLE     false
  #define RGB1_G_GPIO_PORT     112
  #define RGB1_G_GPIO_PIN      212
  #define RGB1_G_ON_VALUE      1
#endif

#if defined(RGB1_B_GPIO_PORT)
  #define RGB1_B_AVAILABLE    true
#else
  #define RGB1_B_AVAILABLE    false
  #define RGB1_B_GPIO_PORT    113
  #define RGB1_B_GPIO_PIN     213
  #define RGB1_B_ON_VALUE     1
#endif

/* ------------------------------ BUTTONS ------------------------------ */

#if defined(PB1_GPIO_PORT)
  #define PB1_AVAILABLE       true
#else
  #define PB1_AVAILABLE       false
  #define PB1_LABEL           "-"
  #define PB1_GPIO_PORT       101
  #define PB1_GPIO_PIN        201
  #define PB1_ON_VALUE        0
  #define PB1_INT_NO          PB1_GPIO_PIN
  #define PB1_CAN_WAKEUP_EM4  false
#endif

#if defined(PB2_GPIO_PORT)
  #define PB2_AVAILABLE       true
#else
  #define PB2_AVAILABLE       false
  #define PB2_LABEL           "-"
  #define PB2_GPIO_PORT       102
  #define PB2_GPIO_PIN        202
  #define PB2_ON_VALUE        0
  #define PB2_INT_NO          PB2_GPIO_PIN
  #define PB2_CAN_WAKEUP_EM4  false
#endif

#if defined(PB3_GPIO_PORT)
  #define PB3_AVAILABLE       true
#else
  #define PB3_AVAILABLE       false
  #define PB3_LABEL           "-"
  #define PB3_GPIO_PORT       103
  #define PB3_GPIO_PIN        203
  #define PB3_ON_VALUE        0
  #define PB3_INT_NO          PB3_GPIO_PIN
  #define PB3_CAN_WAKEUP_EM4  false
#endif

#if defined(PB4_GPIO_PORT)
  #define PB4_AVAILABLE       true
#else
  #define PB4_AVAILABLE       false
  #define PB4_LABEL           "-"
  #define PB4_GPIO_PORT       104
  #define PB4_GPIO_PIN        204
  #define PB4_ON_VALUE        0
  #define PB4_INT_NO          PB4_GPIO_PORT
  #define PB4_CAN_WAKEUP_EM4  false
#endif

#if defined(PB5_GPIO_PORT)
  #define PB5_AVAILABLE       true
#else
  #define PB5_AVAILABLE       false
  #define PB5_LABEL           "-"
  #define PB5_GPIO_PORT       105
  #define PB5_GPIO_PIN        205
  #define PB5_ON_VALUE        0
  #define PB5_INT_NO          PB5_GPIO_PIN
  #define PB5_CAN_WAKEUP_EM4  false
#endif

#if defined(PB6_GPIO_PORT)
  #define PB6_AVAILABLE       true
#else
  #define PB6_AVAILABLE       false
  #define PB6_LABEL           "-"
  #define PB6_GPIO_PORT       106
  #define PB6_GPIO_PIN        206
  #define PB6_ON_VALUE        0
  #define PB6_INT_NO          PB6_GPIO_PIN
  #define PB6_CAN_WAKEUP_EM4  false
#endif

#if defined(SLIDER1_GPIO_PORT)
  #define SLIDER1_AVAILABLE      true
#else
  #define SLIDER1_AVAILABLE      false
  #define SLIDER1_LABEL          "-"
  #define SLIDER1_GPIO_PORT      111
  #define SLIDER1_GPIO_PIN       211
  #define SLIDER1_ON_VALUE       0
  #define SLIDER1_INT_NO         SLIDER1_GPIO_PIN
  #define SLIDER1_CAN_WAKEUP_EM4 false
#endif

#endif /* _BOARD_H_ */
