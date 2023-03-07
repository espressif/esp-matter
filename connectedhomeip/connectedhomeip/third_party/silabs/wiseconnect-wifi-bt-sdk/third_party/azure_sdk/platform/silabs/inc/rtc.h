/**
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_Alarm/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_APP_H
#define __RTC_APP_H
#include <time.h>

int32_t set_time(uint32_t Epoch);
time_t get_time(time_t *p);

#endif /* __RTC_APP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
