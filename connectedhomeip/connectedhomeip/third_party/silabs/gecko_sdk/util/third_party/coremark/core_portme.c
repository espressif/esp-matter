/***************************************************************************//**
 * @file
 * @brief Setup board specific parameters for CoreMark test
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
/***************************************************************************//**
 * @author Shay Gal-On, EEMBC
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "core_portme.h"
#include "coremark.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_timer.h"

//#define HFXO
#define REPORT_RESULT 1
#define VERIFY_TICKCOUNT 0

#if VALIDATION_RUN
	volatile ee_s32 seed1_volatile=0x3415;
	volatile ee_s32 seed2_volatile=0x3415;
	volatile ee_s32 seed3_volatile=0x66;
#endif
#if PERFORMANCE_RUN
	volatile ee_s32 seed1_volatile=0x0;
	volatile ee_s32 seed2_volatile=0x0;
	volatile ee_s32 seed3_volatile=0x66;
#endif
#if PROFILE_RUN
	volatile ee_s32 seed1_volatile=0x8;
	volatile ee_s32 seed2_volatile=0x8;
	volatile ee_s32 seed3_volatile=0x8;
#endif
	volatile ee_s32 seed4_volatile=ITERATIONS;
	volatile ee_s32 seed5_volatile=0;
/* Porting : Timing functions
	How to capture time and convert to seconds must be ported to whatever is supported by the platform.
	e.g. Read value from on board RTC, read value from cpu clock cycles performance counter etc. 
	Sample implementation for standard time.h and windows.h definitions included.
*/
/* Define : TIMER_RES_DIVIDER
	Divider to trade off timer resolution and total time that can be measured.

	Use lower values to increase resolution, but make sure that overflow does not occur.
	If there are issues with the return value overflowing, increase this value.
	*/

#define CORETIMETYPE unsigned int 
#define GETMYTIME(_t) (*_t=efm32_ticks())
#define MYTIMEDIFF(fin,ini) ((fin)-(ini))
#define TIMER_RES_DIVIDER 1
#define SAMPLE_TIME_IMPLEMENTATION 1
#define EE_TICKS_PER_SEC (NSECS_PER_SEC / TIMER_RES_DIVIDER)

void efm32_timerInit(void)
{
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
#if REPORT_RESULT
  CMU_ClockEnable(cmuClock_TIMER0, true);
  CMU_ClockEnable(cmuClock_TIMER1, true);
#endif
  TIMER_TopSet(TIMER0, 0xFFFF);
  TIMER_Init( TIMER0, &timerInit );
  timerInit.sync = true;
  timerInit.clkSel = timerClkSelCascade;
  TIMER_TopSet(TIMER1, 0xFFFF);
  TIMER_Init( TIMER1, &timerInit );

}

uint32_t efm32_ticks(void)
{
  uint32_t t0,t1,tmp;

  t1 = 1;
  tmp = 0;
  /* Make sure we don't have a counter wrap in the middle here */
  while( t1 != tmp ) 
  {
    t1 = TIMER_CounterGet( TIMER1 );
    t0 = TIMER_CounterGet( TIMER0 );
    tmp = TIMER_CounterGet( TIMER1 );
  }
  
  tmp = t1<<16|t0;
  
  return tmp;
}


/** Define Host specific (POSIX), or target specific global time variables. */
static CORETIMETYPE start_time_val, stop_time_val;

/* Function : start_time
	This function will be called right before starting the timed portion of the benchmark.

	Implementation may be capturing a system timer (as implemented in the example code) 
	or zeroing some system parameters - e.g. setting the cpu clocks cycles to 0.
*/
void start_time(void) {
	GETMYTIME(&start_time_val );      
}
/* Function : stop_time
	This function will be called right after ending the timed portion of the benchmark.

	Implementation may be capturing a system timer (as implemented in the example code) 
	or other system parameters - e.g. reading the current value of cpu cycles counter.
*/
void stop_time(void) {
	GETMYTIME(&stop_time_val );
}
/* Function : get_time
	Return an abstract "ticks" number that signifies time on the system.
	
	Actual value returned may be cpu cycles, milliseconds or any other value,
	as long as it can be converted to seconds by <time_in_secs>.
	This methodology is taken to accomodate any hardware or simulated platform.
	The sample implementation returns millisecs by default, 
	and the resolution is controlled by <TIMER_RES_DIVIDER>
*/
CORE_TICKS get_time(void) {
	CORE_TICKS elapsed=(CORE_TICKS)(MYTIMEDIFF(stop_time_val, start_time_val));
	return elapsed;
}

ee_u32 default_num_contexts=1;

/* Function : portable_init
	Target specific initialization code 
	Test for some common mistakes.
*/
void portable_init(core_portable *p, int *argc, char *argv[])
{
  (void) argc;
  (void) argv;
#if VERIFY_TICKCOUNT
  uint32_t tstart, tstop;
#endif
  /* Chip errata */
  CHIP_Init();

  /* Initialize TIMER */
  efm32_timerInit();
        
#if !defined( __CROSSWORKS_ARM ) && defined( __GNUC__ )
  /* Set unbuffered mode for stdout (newlib) */
  setvbuf( stdout, NULL, _IONBF, 0 );
#endif
	p->portable_id=1;
                
}
/* Function : portable_fini
	Target specific final code 
*/
void portable_fini(core_portable *p)
{
	p->portable_id=0;
}


