/*
 * Copyright (c) 2019 Arm Limited. All rights reserved.
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

#ifndef __CMSIS_DRIVER_CONFIG_H__
#define __CMSIS_DRIVER_CONFIG_H__

#include "platform_description.h"
#include "device_definition.h"

#define UART0_CMSDK_DEV     ARM_UART0_DEV_NS

#define MPC_ISRAM0_DEV      MPC_ISRAM0_DEV_S
#define MPC_SSRAM1_DEV      MPC_SSRAM1_DEV_S
#define MPC_SSRAM2_DEV      MPC_SSRAM2_DEV_S
#define MPC_SSRAM3_DEV      MPC_SSRAM3_DEV_S

#define PPC_SSE123_AHB_EXP0_DEV     PPC_SSE123_AHB_EXP0_DEV_S
#define PPC_SSE123_APB_DEV          PPC_SSE123_APB_DEV_S
#define PPC_SSE123_APB_EXP0_DEV     PPC_SSE123_APB_EXP0_DEV_S
#define PPC_SSE123_APB_EXP1_DEV     PPC_SSE123_APB_EXP1_DEV_S
#define PPC_SSE123_APB_EXP2_DEV     PPC_SSE123_APB_EXP2_DEV_S
#define PPC_SSE123_APB_EXP3_DEV     PPC_SSE123_APB_EXP3_DEV_S

#endif  /* __CMSIS_DRIVER_CONFIG_H__ */
