/*******************************************************************************
* @file  rsi_hal_mcu_rtc.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
/**
 * @file       rsi_hal_mcu_rtc.c
 * @version    0.1
 * @date       13 May 2021
 *
 *
 *
 * @brief HAL RTC TIME: Functions related to HAL RTC Clock
 *
 * @section Description
 * This file contains the list of functions for initialization of RTC Clock source.
 *
 */


/**
 * Includes
 */
 
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_rtc.h"
#include "stm32f4xx_ll_pwr.h"

#include <stdio.h>
#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Define used to enable time-out management*/
#define USE_TIMEOUT       0

#define RTC_ERROR_NONE    0
#define RTC_ERROR_TIMEOUT 1

#define YEAR 	4
#define MONTH 	12
unsigned short days[YEAR][MONTH] =
{
    {   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335},
    { 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
    { 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065},
    {1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430},
};

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* IRQ Handler treatment.  */
#define RTC_TIMEOUT_VALUE          ((uint32_t)1000)  /* 1 s */
#define RTC_CLOCK_SOURCE_LSI       					 /* Defines related to Clock configuration */
#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)  /* ck_apre=LSIFreq/(ASYNC prediv + 1) with LSIFreq=32 kHz RC */
#define RTC_SYNCH_PREDIV           ((uint32_t)0x00F9)/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)   /*ck_apre=LSEFreq/(ASYNC prediv + 1) = 256Hz with LSEFreq=32768Hz */
#define RTC_SYNCH_PREDIV           ((uint32_t)0x00FF) /*ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#endif

#if (USE_TIMEOUT == 1)
uint32_t Timeout = 0; /* Variable used for Timeout management */
#endif /* USE_TIMEOUT */

/**
  * @brief  Enter in initialization mode
  * @note In this mode, the calendar counter is stopped and its value can be updated
  * @param  None
  * @retval RTC_ERROR_NONE if no error
  */
uint32_t Enter_RTC_InitMode(void)
{
  LL_RTC_EnableInitMode(RTC);  /* Set Initialization mode */
  
#if (USE_TIMEOUT == 1)
  Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

  while (LL_RTC_IsActiveFlag_INIT(RTC) != 1)  /*Check if the Initialization mode is set */
  {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
      return RTC_ERROR_TIMEOUT;
      }  
#endif /* USE_TIMEOUT */
  }
  
  return RTC_ERROR_NONE;
}
 /**
  * @brief  Wait until the RTC Time and Date registers (RTC_TR and RTC_DR) are
  *         synchronized with RTC APB clock.
  * @param  None
  * @retval RTC_ERROR_NONE if no error (RTC_ERROR_TIMEOUT will occur if RTC is 
  *         not synchronized)
***/
uint32_t WaitForSynchro_RTC(void)
{
  LL_RTC_ClearFlag_RS(RTC);  /* Clear RSF flag */
#if (USE_TIMEOUT == 1)
    Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
  while(LL_RTC_IsActiveFlag_RS(RTC) != 1) /* Wait the registers to be synchronised */
  {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
      return RTC_ERROR_TIMEOUT;
      }  
#endif /* USE_TIMEOUT */
  }
  return RTC_ERROR_NONE;
}
/**
  * @brief  Exit Initialization mode 
  * @param  None
  * @retval RTC_ERROR_NONE if no error
  */
  uint32_t Exit_RTC_InitMode(void)
  {
  LL_RTC_DisableInitMode(RTC);
  
  /* Wait for synchro */
  /* Note: Needed only if Shadow registers is enabled           */
  /*       LL_RTC_IsShadowRegBypassEnabled function can be used */
  return (WaitForSynchro_RTC());
  }

/**
  * @brief  Configure RTC.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void rsi_rtc_configure(void)
{
  /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
  /* To change the source clock of the RTC feature (LSE, LSI), you have to:
     - Enable the power clock
     - Enable write access to configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain
     - Configure the needed RTC clock source */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
  
  /*##-2- Configure LSE/LSI as RTC clock source ###############################*/
#ifdef RTC_CLOCK_SOURCE_LSE
  /* Enable LSE only if disabled.*/
  if (LL_RCC_LSE_IsReady() == 0)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_LSE_Enable();
#if (USE_TIMEOUT == 1)
    Timeout = LSE_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_LSE_IsReady() != 1)
    {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag()) 
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
        /* LSE activation error */
      }  
#endif /* USE_TIMEOUT */
    }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
    
    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */ 
    LL_RCC_EnableRTC();
  }
#elif defined(RTC_CLOCK_SOURCE_LSI)
  LL_RCC_LSI_Enable();  /* Enable LSI */
#if (USE_TIMEOUT == 1)
  Timeout = LSI_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
  while (LL_RCC_LSI_IsReady() != 1)
  {
#if (USE_TIMEOUT == 1)
    if (LL_SYSTICK_IsActiveCounterFlag()) 
    {
      Timeout --;
    }
    if (Timeout == 0)
    {
      /* LSI activation error */
    }  
#endif /* USE_TIMEOUT */
  }
  LL_RCC_ForceBackupDomainReset();
  LL_RCC_ReleaseBackupDomainReset();
  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);

  /*##-3- Enable RTC peripheral Clocks */
  /* Enable RTC Clock */ 
  LL_RCC_EnableRTC();

#else
#error "configure clock for RTC"
#endif

  /*##-4- Disable RTC registers write protection */
  LL_RTC_DisableWriteProtection(RTC);

  /*##-5- Enter in initialization mode */
  if (Enter_RTC_InitMode() != RTC_ERROR_NONE)   
  {
    /* Initialization Error */
    //printf("Initialization Error"); // To be enabled only for debugging
  }

  /*##-6- Configure RTC */
  /* Configure RTC prescaler and RTC data registers */
  /* Set Hour Format */
  LL_RTC_SetHourFormat(RTC, LL_RTC_HOURFORMAT_AMPM);
  /* Set Asynch Prediv (value according to source clock) */
  LL_RTC_SetAsynchPrescaler(RTC, RTC_ASYNCH_PREDIV);
  /* Set Synch Prediv (value according to source clock) */
  LL_RTC_SetSynchPrescaler(RTC, RTC_SYNCH_PREDIV);

  /*##-7- Exit of initialization mode */
  Exit_RTC_InitMode();
  
  /*##-8- Enable RTC registers write protection */
  LL_RTC_EnableWriteProtection(RTC);
}

/**
  * @brief  Configure the current time and date.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @param  None
  * @retval None
  */
void rsi_rtc_configure_time(uint32_t Epoch)
{
  /*##-1- Disable RTC registers write protection ############################*/
  LL_RTC_DisableWriteProtection(RTC);

  /*##-2- Enter in initialization mode ######################################*/
  if (Enter_RTC_InitMode() != RTC_ERROR_NONE)
  {
    /* Initialization Error */
    //printf("Initialization Error"); //To be enabled only for debugging
  }
	uint32_t years 	  	= 0;
	uint8_t year	  		= 0;
	uint8_t month	  		= 0;
	uint8_t dayOfWeek 	= 4; //starting day of Week 1/1/1970 is Thursday
	uint8_t Second,Minute,Hour,Year,Month,Day,DayOfWeek;
	Second = Epoch%60;
	Epoch /= 60;
	Minute = Epoch%60; 
	Epoch /= 60;
	Hour   = Epoch%24; 
	Epoch /= 24;

	years = Epoch/(365*4+1)*4; 
	Epoch %= 365*4+1;

	for (year=3; year>0; year--)
	{
		if (Epoch >= days[year][0])
			break;
	}

	for (month=11; month>0; month--)
	{
		if (Epoch >= days[year][month])
			break;
	}
	Year  				= (uint8_t)years + (uint8_t)year;
	Month  				= month+1;
	Day   				= Epoch-days[year][month]+1;
	DayOfWeek    	= (dayOfWeek += month < 3 ? year-- : year - 2, 23*month/9 + dayOfWeek + 4 + year/4- year/100 + year/400)%7;
  /*##-3- Configure the Date ################################################*/
  /* Note: __LL_RTC_CONVERT_BIN2BCD helper macro can be used if user wants to*/
  /*       provide directly the decimal value:                               */
  /*       LL_RTC_DATE_Config(RTC, LL_RTC_WEEKDAY_MONDAY,                    */
  /*                          __LL_RTC_CONVERT_BIN2BCD(31), (...))           */
  /* Set Date: Friday December 29th 2016 */
  //LL_RTC_DATE_Config(RTC, LL_RTC_WEEKDAY_FRIDAY, 0x29, LL_RTC_MONTH_DECEMBER, 0x16);
  //======//LL_RTC_DATE_Config(RTC, DayOfWeek, Day, Month, Year1);
	LL_RTC_DATE_Config(RTC, __LL_RTC_CONVERT_BIN2BCD(DayOfWeek), __LL_RTC_CONVERT_BIN2BCD(Day), __LL_RTC_CONVERT_BIN2BCD(Month), __LL_RTC_CONVERT_BIN2BCD(Year));
  /*##-4- Configure the Time ################################################*/
  /* Set Time: 11:59:55 PM*/
  //LL_RTC_TIME_Config(RTC, LL_RTC_TIME_FORMAT_PM, 0x11, 0x59, 0x55);
	//======//LL_RTC_TIME_Config(RTC, LL_RTC_TIME_FORMAT_PM, Hour, Minute, Second);
	LL_RTC_TIME_Config(RTC, LL_RTC_TIME_FORMAT_PM, __LL_RTC_CONVERT_BIN2BCD(Hour), __LL_RTC_CONVERT_BIN2BCD(Minute), __LL_RTC_CONVERT_BIN2BCD(Second));
  if (Exit_RTC_InitMode() != RTC_ERROR_NONE)   
  {
    /* Initialization Error */
    //printf("Initialization Error");// To be enabled only for debugging
  }
	
  LL_RTC_EnableWriteProtection(RTC);
}
 
void rsi_rtc_systemclock_config(void)
{
  /* Enable HSE oscillator */
  LL_RCC_HSE_EnableBypass();
  LL_RCC_HSE_Enable();
  while(LL_RCC_HSE_IsReady() != 1)
  {
  };

  /* Set FLASH latency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 400, LL_RCC_PLLP_DIV_4);
  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };

  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };

  /* Set APB1 & APB2 prescaler */
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms */
  SysTick_Config(100000000 / 1000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  SystemCoreClock = 100000000;
}

uint32_t rsi_rtc_set_time(uint32_t Epoch)
{
  /*##-1- Configure the RTC peripheral*/
  rsi_rtc_configure();
	
  /*Configure RTC Alarm */
  rsi_rtc_configure_time(Epoch);

  return 0;
}

uint32_t rsi_rtc_get_time(void)
{
  uint8_t Second,Minute,Hour,Year,Month,Day;
  Hour   = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));
  Minute = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
  Second = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));
  Day	   = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
  Month  = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC));
  Year   = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC));
  // converting rtc time to EPOCH time
  uint32_t epoch1 = (((Year/4*(365*4+1)+days[Year%4][Month -1]+(Day -1))*24+Hour)*60+Minute)*60+Second; 
  return epoch1; 
}
