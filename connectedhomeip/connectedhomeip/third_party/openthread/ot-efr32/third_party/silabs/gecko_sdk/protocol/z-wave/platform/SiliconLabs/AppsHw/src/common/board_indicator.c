/**
 * Indicator LED support.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "board_indicator.h"
#include "board.h"
//#define DEBUGPRINT
#include "DebugPrint.h"
#include "Assert.h"
#include "em_letimer.h"
#include "em_cmu.h"
#include <zpal_power_manager.h>
#include <FreeRTOSConfig.h>

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * Indicator clock frequency in Hz
 * Must be aligned with clock settings in Board_IndicatorInit().
 */
#define LETIMER_CLOCK_FREQ    1000
#ifdef ZWAVE_SERIES_800
#define INDICATOR_CLOCK_DIV   1
#else
#define INDICATOR_CLOCK_DIV   cmuClkDiv_4
#endif
#define INDICATOR_CLOCK_FREQ  (LETIMER_CLOCK_FREQ / INDICATOR_CLOCK_DIV)

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

static bool m_indicator_initialized    = false;
static bool m_indicator_active_from_cc = false;

/**
 * Power lock to keep the device from entering deep sleep (EM4) while the
 * indicator is active.
 */
static zpal_pm_handle_t indicator_io_power_lock;

/**
 * GPIO output value to deactivate the indicator LED
 * Valid values are 0 or 1.
 * Every indicator LED blink period starts by setting the indicator GPIO
 * to this value for off_time_ms (@ref Board_IndicatorControl).
 */
static uint8_t m_indicator_led_off_value = 0;

/****************************************************************************/
/*                      PRIVATE FUNCTIONS                                   */
/****************************************************************************/


/****************************************************************************/
/*                      PUBLIC FUNCTIONS                                    */
/****************************************************************************/


void Board_IndicateStatus(board_status_t status)
{

  switch(status)
  {
    case BOARD_STATUS_LEARNMODE_ACTIVE:
      /* Blink indicator at 1 Hz with LED on for 100 ms */
      Board_IndicatorControl(100, 900, 0, false);
      break;
    case BOARD_STATUS_POWER_DOWN:
      /* Don't really know what to do here...
       * Ideally the indicator should keep blinking if powering down. */
      break;
    default:
      /* Turn off the indicator LED */
      Board_IndicatorControl(0, 0, 0, false);
  }
}

void Board_IndicatorInit(void)
{
  Board_IndicatorInitLed(APP_LED_INDICATOR);
}

void Board_IndicatorInitLed(led_id_t led)
{
  if (!m_indicator_initialized)
  {
    /* Configure LED */
    Board_ConfigLed(led, true);
    /* We need a EM3 power lock when the indicator is active.
     * (LETIMER0 will not run in EM4)
     */
    indicator_io_power_lock = zpal_pm_register(ZPAL_PM_TYPE_DEEP_SLEEP);

#ifdef ZWAVE_SERIES_700

    /* Select the 1000 Hz Ultra Low Frequency RC Oscillator (ULFRCO) as
     * clock source for LFACLK (which is the clock selected by LETIMER0).
     */
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
    /* The clock for Low Energy module bus interface and LETIMER0
     * must be enabled
     */
    CMU_ClockEnable(cmuClock_HFLE, true);
    /* The indicator command class uses 100 msec increments when configuring
     * the indicator.
     */
    /* Ideally we therefore only need a 10 Hz clock to blink
     * the indicator led. But since the prescale can only be 2^n values our
     * best option is to select a prescale of 4 to get a 250 Hz clock. (we
     * could also just skip prescaling and use the 1000 Hz clock as-is, but
     * lower frequency equals lower power usage)
     */
    CMU_ClockPrescSet(cmuClock_LETIMER0, INDICATOR_CLOCK_DIV);
#endif
    CMU_ClockEnable(cmuClock_LETIMER0, true);
    /* Enable repeat counter 0 interrupt flag */
    LETIMER_IntEnable(LETIMER0, LETIMER_IF_REP0);

    /* Since the LETIMER0 interrupt handler makes a FreeRTOS system call
     * we must set its priority lower (i.e. numerically higher) than
     * configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. Since a blinking
     * LED is not critical we assign it the lowest possible priority.
     */
    NVIC_SetPriority(LETIMER0_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    /* Enable LETIMER0 interrupt vector in NVIC*/
    NVIC_EnableIRQ(LETIMER0_IRQn);

    uint32_t letimer_routeloc_led = Board_GetLedLeTimerLoc(led);
    /* Disable output and route LETIMER0 OUT0 to indicator LED location
     */
#ifdef ZWAVE_SERIES_800
    GPIO->LETIMERROUTE.ROUTEEN &= ~GPIO_LETIMER_ROUTEEN_OUT0PEN;
    GPIO->LETIMERROUTE.OUT0ROUTE = (GPIO->LETIMERROUTE.OUT0ROUTE & ~_GPIO_LETIMER_OUT0ROUTE_MASK) | letimer_routeloc_led;
#else
    LETIMER0->ROUTEPEN &= ~LETIMER_ROUTEPEN_OUT0PEN;
    LETIMER0->ROUTELOC0 = (LETIMER0->ROUTELOC0 & ~_LETIMER_ROUTELOC0_OUT0LOC_MASK) | letimer_routeloc_led;
#endif
    m_indicator_led_off_value = (Board_GetLedOnValue(led) > 0) ? 0 : 1;

    m_indicator_initialized = true;
  }
}


bool Board_IndicatorControl(uint32_t on_time_ms,
                            uint32_t off_time_ms,
                            uint32_t num_cycles,
                            bool called_from_indicator_cc)
{
  DPRINTF("Board_IndicatorControl() on=%u, off=%u, num=%u\n", on_time_ms, off_time_ms, num_cycles);

  if (!m_indicator_initialized)
  {
    ASSERT(false);
    return false;
  }

  if (on_time_ms < (LETIMER_CLOCK_FREQ / INDICATOR_CLOCK_FREQ))
  {
    /* On time is zero (or less than one LETIMER0 clock tick)
     * --> turn off the indicator */
#ifdef ZWAVE_SERIES_800
    GPIO->LETIMERROUTE.ROUTEEN &= ~GPIO_LETIMER_ROUTEEN_OUT0PEN;
#else
    LETIMER0->ROUTEPEN &= ~LETIMER_ROUTEPEN_OUT0PEN;
#endif
    LETIMER_Enable(LETIMER0, false);
    //Reset LETIMER0 to avoid power consumption in EM2 sleep
    LETIMER_Reset(LETIMER0);

    m_indicator_active_from_cc = false;

    zpal_pm_cancel(indicator_io_power_lock);
  }
  else
  {
    // Calculate the top value (value CNT should start counting down from) based on clock source and prescale setting
    uint32_t clockFreq = CMU_ClockFreqGet(cmuClock_LETIMER0);
    uint32_t topValue = (clockFreq * (on_time_ms + off_time_ms)) / 1000;
    uint32_t onValue =  (clockFreq * on_time_ms) / 1000;

    LETIMER_RepeatMode_TypeDef repeat_mode  = letimerRepeatFree;
    uint32_t                   repeat_value = 1;

    if (num_cycles > 0)
    {
      repeat_mode  = letimerRepeatOneshot;
      repeat_value = num_cycles;
    }

    /* Enable repeat counter 0 interrupt flag */
    LETIMER_IntEnable(LETIMER0, LETIMER_IF_REP0);

#ifdef ZWAVE_SERIES_800
    // 800 series
    // Set COMP0 to control duty cycle - onValue
    LETIMER_CompareSet(LETIMER0, 0, onValue);
#else
    // 700 series
    // Set COMP1 to control duty cycle - onValue
    LETIMER_CompareSet(LETIMER0, 1, onValue);
#endif
    // Set REP0 to either one-shot (finite number of repeats) or free run
    LETIMER_RepeatSet(LETIMER0, 0, repeat_value);   /* REP0 */

    LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT; // LETIMER_INIT_DEFAULT value has enable=true
    // Load with topValue and reload CNT on underflow, PWM output, and run in repeat_mode - one-shot or free run
    letimerInit.enable = false;                       /* Enabling on Init can cause a crash so avoid it */
    letimerInit.comp0Top = true;                      /* Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
    letimerInit.ufoa0 = letimerUFOAPwm;               /* PWM output on output 0 */
    letimerInit.out0Pol = m_indicator_led_off_value;  /* Value which OUT0 will have from CNT being topValue until reaching onValue where OUT0 toggles to ON value */
    letimerInit.repMode = repeat_mode;
    letimerInit.topValue = topValue;

    /* Enable LETIMER0 OUT0 (location configured in Board_IndicatorInit() */
#ifdef ZWAVE_SERIES_800
    GPIO->LETIMERROUTE.ROUTEEN |= GPIO_LETIMER_ROUTEEN_OUT0PEN;
#else
    LETIMER0->ROUTEPEN |= LETIMER_ROUTEPEN_OUT0PEN;
#endif

    // Initialize LETIMER0
    LETIMER_Init(LETIMER0, &letimerInit);
    // Start LETIMER0
    LETIMER_Enable(LETIMER0, true);

    m_indicator_active_from_cc = called_from_indicator_cc;

    /* Stay awake until indicator is turned off explicitly with
     * Board_IndicatorControl() or LETIMER0_IRQHandler() is called on
     * blink cycle completion.
     */
    zpal_pm_stay_awake(indicator_io_power_lock, 0);
  }
  return true;
}


bool Board_IsIndicatorActive(void)
{
  DPRINT("Board_IsIndicatorActive()\n");

  return m_indicator_active_from_cc;
}


/**
 * Interrupt handler for LETIMER0.
 *
 * Configured in @ref BoardIndicatorInit to be called whenever the repeat
 * counter 0 of LETIMER0 reaches zero. I.e. whenever the configured number
 * of blink cycles has completed.
 *
 * We don't need to do anything to LETIMER0 here - it will stop automatically
 * when REP0 reaches zero, but we would like to make it possible for the
 * indicator command class to accurately report the status of the indicator.
 * Also we release the power lock to enable deep sleep (EM4).
 *
 */
void LETIMER0_IRQHandler(void)
{
  DPRINT("LETIMER0_IRQHandler()\n");
  uint32_t letimerInts = LETIMER_IntGet(LETIMER0);
  if (letimerInts & LETIMER_IF_REP0)
  {
    LETIMER_IntClear(LETIMER0, LETIMER_IF_REP0);
    DPRINT("LETIMER0_IRQHandler() IF = LETIMER_IF_REP0\n");

    // Turn of LETIMER OUT0 route to pin
#ifdef ZWAVE_SERIES_800
    GPIO->LETIMERROUTE.ROUTEEN &= ~GPIO_LETIMER_ROUTEEN_OUT0PEN;
#else
    LETIMER0->ROUTEPEN &= ~LETIMER_ROUTEPEN_OUT0PEN;
#endif
    LETIMER_Enable(LETIMER0, false);
    //Reset LETIMER0 to avoid power consumption in EM2 sleep
    LETIMER_Reset(LETIMER0);

    m_indicator_active_from_cc = false;

    /* We're done blinking and can go back to deep sleep */
    zpal_pm_cancel(indicator_io_power_lock);
  }
  // Clear any other LETIMER0 Int source
  LETIMER_IntClear(LETIMER0, letimerInts & ~LETIMER_IF_REP0);
}
