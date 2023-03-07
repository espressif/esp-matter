/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_device.h"
#include "cslib_hwconfig.h"
#include "cslib_config.h"
#include "low_power_config.h"
#include "cslib.h"
#include "hardware_routines.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_acmp.h"

#if (defined(RTCC_PRESENT))
#include "em_rtcc.h"
void rtccSetup(uint16_t frequency);
#else
#include "em_rtc.h"
void rtcSetup(uint16_t frequency);
#endif

#define ACMP_CAPSENSE                           ACMP0
#define ACMP_CAPSENSE_CLKEN                     CMU_HFPERCLKEN0_ACMP0
#define PRS_CH_CTRL_SOURCESEL_ACMP_CAPSENSE     PRS_CH_CTRL_SOURCESEL_ACMP0
#define PRS_CH_CTRL_SIGSEL_ACMPOUT_CAPSENSE     PRS_CH_CTRL_SIGSEL_ACMP0OUT

/// @brief The current channel we are sensing.
static volatile uint8_t currentChannel;

/// @brief Flag for measurement completion.
static volatile bool measurementComplete;

/// @brief saves sensor value captured in timer ISR
static uint16_t scanResult;

/// @brief Flag used within library functions
uint16_t CSLIB_autoScanComplete;

/// @brief Temporarily saves sensor data before being pushed into CSLIB_node struct
volatile uint32_t autoScanBuffer[DEF_NUM_SENSORS];
/// @brief Buffer passed back to CSLIB to copy into CSLIB_node struct
volatile uint32_t CSLIB_autoScanBuffer[DEF_NUM_SENSORS];

/// @brief Configures whether sleep mode scan uses LESENSE or SENSE algo
uint16_t CSLIB_autoScan;

void configureRelaxOscActiveMode(void);

/**************************************************************************//**
 * @brief TIMER0 interrupt handler.
 *        When TIMER0 expires the number of pulses on TIMER1 is inserted into
 *        channelValues. If this values is bigger than what is recorded in
 *        channelMaxValues, channelMaxValues is updated.
 *        Finally, the next ACMP channel is selected.
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  unsigned long count;

  // Stop timers
  TIMER0->CMD = TIMER_CMD_STOP;
  TIMER1->CMD = TIMER_CMD_STOP;

  // Clear interrupt flag
  TIMER0->IFC = TIMER_IFC_OF;

  // Read out value of TIMER1
  count = TIMER1->CNT;

  // Store value in channelValues
  scanResult = count;

  measurementComplete = true;

  // Notify comms we have new data and it should update
  sendComms = true;
}

/**************************************************************************//**
 * @brief This function iterates through all the capsensors and reads and
 *        initiates a reading. Uses EM1 while waiting for the result from
 *        each sensor.
 *****************************************************************************/
uint32_t CSLIB_scanSensorCB(uint8_t index)
{
  // Use the default STK capacative sensing setup and enable it
  ACMP_Enable(ACMP_CAPSENSE);

  uint8_t ch;
  currentChannel = CSLIB_muxInput[CSLIB_muxValues[index]];

  // Set up this channel in the ACMP.
  ch = currentChannel;
  ACMP_CapsenseChannelSet(ACMP_CAPSENSE, (ACMP_Channel_TypeDef) ch);

  // Reset timers
  TIMER0->CNT = 0;
  TIMER1->CNT = 0;

  measurementComplete = false;

  // Start timers
  TIMER0->CMD = TIMER_CMD_START;
  TIMER1->CMD = TIMER_CMD_START;

  // Wait for measurement to complete
  while ( measurementComplete == false ) {
    EMU_EnterEM1();
  }

  // Disable ACMP while not sensing to reduce power consumption
  ACMP_Disable(ACMP_CAPSENSE);
  return scanResult;
}

/**************************************************************************//**
 * Pre baseline initialization callback
 *
 * Called before a baseline for a sensor has been initialized.
 *
 *****************************************************************************/
void CSLIB_baselineInitEnableCB(void)
{
}

/**************************************************************************//**
 * Post baseline initialization callback
 *
 * Called after a baseline for a sensor has been initialized.
 *
 *****************************************************************************/
void CSLIB_baselineInitDisableCB(void)
{
}

/**************************************************************************//**
 * @brief Initializes the capacative sense system.
 *        Capacative sensing uses two timers: TIMER0 and TIMER1 as well as ACMP.
 *        ACMP is set up in cap-sense (oscialltor mode).
 *        TIMER1 counts the number of pulses generated by ACMP_CAPSENSE.
 *        When TIMER0 expires it generates an interrupt.
 *        The number of pulses counted by TIMER1 is then stored in channelValues
 *****************************************************************************/
void CAPSENSE_Init(void)
{
  // Use the default STK capacative sensing setup
  ACMP_CapsenseInit_TypeDef capsenseInit = ACMP_CAPSENSE_INIT_DEFAULT;

  // Indicates that sleep mode scanning with ACMP should be used in library code
  CSLIB_autoScan = 0;
  // Enable TIMER0, TIMER1, ACMP_CAPSENSE and PRS clock
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
  CMU_ClockEnable(cmuClock_TIMER1, true);
#if defined(ACMP_CAPSENSE_CMUCLOCK)
  CMU_ClockEnable(ACMP_CAPSENSE_CMUCLOCK, true);
#else
  CMU->HFPERCLKEN0 |= ACMP_CAPSENSE_CLKEN;
#endif
  CMU_ClockEnable(cmuClock_PRS, true);

  // Initialize TIMER0 - Prescaler 2^9, top value 10, interrupt on overflow
  TIMER0->CTRL = TIMER_CTRL_PRESC_DIV512;
  TIMER0->TOP  = 10;
  TIMER0->IEN  = TIMER_IEN_OF;
  TIMER0->CNT  = 0;

  // Initialize TIMER1 - Prescaler 2^10, clock source CC1, top value 0xFFFF
  TIMER1->CTRL = TIMER_CTRL_PRESC_DIV1024 | TIMER_CTRL_CLKSEL_CC1;
  TIMER1->TOP  = 0xFFFF;

  // Set up TIMER1 CC1 to trigger on PRS channel 0
  // Input capture
  TIMER1->CC[1].CTRL = TIMER_CC_CTRL_MODE_INPUTCAPTURE
                       | TIMER_CC_CTRL_PRSSEL_PRSCH0 /* PRS channel 0      */
                       | TIMER_CC_CTRL_INSEL_PRS /* PRS input selected */
                       | TIMER_CC_CTRL_ICEVCTRL_RISING /* PRS on rising edge */
                       | TIMER_CC_CTRL_ICEDGE_BOTH; /* PRS on rising edge */

  // Set up PRS channel 0 to trigger on ACMP1 output
  PRS->CH[0].CTRL = PRS_CH_CTRL_EDSEL_POSEDGE      /* Posedge triggers action */
                    | PRS_CH_CTRL_SOURCESEL_ACMP_CAPSENSE /* PRS source */
                    | PRS_CH_CTRL_SIGSEL_ACMPOUT_CAPSENSE; /* PRS source */

  // Set up ACMP1 in capsense mode
  ACMP_CapsenseInit(ACMP_CAPSENSE, &capsenseInit);

  // Enable TIMER0 interrupt
  NVIC_EnableIRQ(TIMER0_IRQn);
}

/**************************************************************************//**
 * Ready CS0 for active mode, unbound sensor scanning
 *
 * This is a top-level call to configure the sensor to its operational state
 * during active mode.
 *
 *****************************************************************************/
void CSLIB_configureSensorForActiveModeCB(void)
{
  configureRelaxOscActiveMode();
}

/**************************************************************************//**
 * Configure CS0 block for active scanning
 *
 * Configure capsense input block for unbound, sensor-by-sensor scanning.
 *
 *****************************************************************************/
void configureRelaxOscActiveMode(void)
{
  CAPSENSE_Init();
  CSLIB_configureTimerForActiveModeCB();
}

#if (defined(_EFM32_PEARL_FAMILY))
void RTCC_IRQHandler(void)
{
  timerTick = 1;
  RTCC_IntClear(RTCC_IFC_CC1);
}

void palClockSetup(CMU_Clock_TypeDef clock)
{
  // Enable LE domain registers
  CMU_ClockEnable(cmuClock_CORELE, true);

  // Enable LF(A|E)CLK in CMU (will also enable LFRCO oscillator if not enabled)
  CMU_ClockSelectSet(clock, cmuSelect_LFRCO);
}
/**************************************************************************//**
 * @brief Enables LFECLK and selects clock source for RTCC
 *        Sets up the RTCC to generate an interrupt every second.
 *****************************************************************************/
void rtccSetup(uint16_t frequency)
{
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_1;

  palClockSetup(cmuClock_LFE);
  // Enable RTCC clock
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Initialize RTC

  // Do not start RTC after initialization is complete.
  rtccInit.enable   = false;

  // Halt RTC when debugging.
  rtccInit.debugRun = false;
  // Wrap around on CCV1 match.
  rtccInit.cntWrapOnCCV1 = true;
  RTCC_Init(&rtccInit);

  // Interrupt at given frequency.
  RTCC_CCChConf_TypeDef ccchConf = RTCC_CH_INIT_COMPARE_DEFAULT;
  ccchConf.compMatchOutAction = rtccCompMatchOutActionToggle;
  RTCC_ChannelInit(1, &ccchConf);
  RTCC_ChannelCCVSet(1, (CMU_ClockFreqGet(cmuClock_RTCC) / frequency) - 1);

  // Enable interrupt
  NVIC_EnableIRQ(RTCC_IRQn);
  RTCC_IntEnable(RTCC_IEN_CC1);

  RTCC->CNT = _RTCC_CNT_RESETVALUE;
  // Start Countee
  RTCC_Enable(true);
}
#else

/**************************************************************************//**
 * @brief   Setup clocks necessary to drive RTC/RTCC for EXTCOM GPIO pin.
 *
 * @return  N/A
 *****************************************************************************/
static void palClockSetup(CMU_Clock_TypeDef clock)
{
  // Enable LE domain registers
  CMU_ClockEnable(cmuClock_CORELE, true);

  // Enable LF(A|E)CLK in CMU (will also enable LFRCO oscillator if not enabled)
  CMU_ClockSelectSet(clock, cmuSelect_LFRCO);
}
/**************************************************************************//**
 * @brief Enables LFACLK and selects LFXO as clock source for RTC
 *        Sets up the RTC to generate an interrupt every second.
 *****************************************************************************/
void rtcSetup(uint16_t frequency)
{
  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  palClockSetup(cmuClock_LFA);

  // Set the prescaler.
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_2);

  // Enable RTC clock
  CMU_ClockEnable(cmuClock_RTC, true);

  // Initialize RTC

  // Do not start RTC after initialization is complete.
  rtcInit.enable   = false;
  // Halt RTC when debugging.
  rtcInit.debugRun = false;
  // Wrap around on COMP0 match.
  rtcInit.comp0Top = true;

  RTC_Init(&rtcInit);

  // Interrupt at given frequency.
  RTC_CompareSet(0, ((CMU_ClockFreqGet(cmuClock_RTC) / frequency) - 1) & _RTC_COMP0_MASK);

  // Enable interrupt
  NVIC_EnableIRQ(RTC_IRQn);
  RTC_IntEnable(RTC_IEN_COMP0);

  RTC_CounterReset();
  // Start Counter
  RTC_Enable(true);
}

/**************************************************************************//**
 * @brief Provides timebase to track active mode and sleep mode periods
 *****************************************************************************/
void RTC_IRQHandler(void)
{
  timerTick = 1;
  RTC_IntClear(RTC_IEN_COMP0);
}
#endif

/**************************************************************************//**
 * Find highest valid gain setting
 *
 * @returns CS0MD1 gain setting that is valid
 *
 * Executed before entering sleep mode.  used to find highest gain setting
 * possible for all bound CS0 inputs without saturating sensor and gives
 * margin for sleep touch qualification.
 *
 *****************************************************************************/
uint8_t determine_highest_gain(void)
{
  return 0;
}
