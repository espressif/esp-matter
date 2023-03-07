/*
 * Copyright (c) 2016-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SYSTEM_CMSDK_MPS2_AN521_H__
#define __SYSTEM_CMSDK_MPS2_AN521_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t SystemCoreClock;  /*!< System Clock Frequency (Core Clock)  */
extern uint32_t PeripheralClock;  /*!< Peripheral Clock Frequency */

/**
 * \brief  Initializes the system
 */
void SystemInit(void);

/**
 * \brief  Restores system core clock
 */
void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_CMSDK_MPS2_AN521_H__ */
