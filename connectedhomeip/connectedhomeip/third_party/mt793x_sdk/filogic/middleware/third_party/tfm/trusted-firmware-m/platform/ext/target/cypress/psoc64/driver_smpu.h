/*
 * Copyright (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
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

#ifndef __DRIVER_SMPU_H__
#define __DRIVER_SMPU_H__

#include "cy_prot.h"
#include "RTE_Device.h"

typedef struct smpu_resources SMPU_Resources;

/* API functions */
cy_en_prot_status_t SMPU_Configure(const SMPU_Resources *smpu_dev);
cy_en_prot_status_t protect_unconfigured_smpus(void);
void SMPU_Print_Config(const SMPU_Resources *smpu_dev);

/* Exported per-SMPU macros */
#define DECLARE_SMPU(N) extern const SMPU_Resources SMPU##N##_Resources;

#if (RTE_SMPU0)
DECLARE_SMPU(0)
#endif /* RTE_SMPU0 */

#if (RTE_SMPU1)
DECLARE_SMPU(1)
#endif /* RTE_SMPU1 */

#if (RTE_SMPU2)
DECLARE_SMPU(2)
#endif /* RTE_SMPU2 */

#if (RTE_SMPU3)
DECLARE_SMPU(3)
#endif /* RTE_SMPU3 */

#if (RTE_SMPU4)
DECLARE_SMPU(4)
#endif /* RTE_SMPU4 */

#if (RTE_SMPU5)
DECLARE_SMPU(5)
#endif /* RTE_SMPU5 */

#if (RTE_SMPU6)
DECLARE_SMPU(6)
#endif /* RTE_SMPU6 */

#if (RTE_SMPU7)
DECLARE_SMPU(7)
#endif /* RTE_SMPU7 */

#if (RTE_SMPU8)
DECLARE_SMPU(8)
#endif /* RTE_SMPU8 */

#if (RTE_SMPU9)
DECLARE_SMPU(9)
#endif /* RTE_SMPU9 */

#if (RTE_SMPU10)
DECLARE_SMPU(10)
#endif /* RTE_SMPU10 */

#if (RTE_SMPU11)
DECLARE_SMPU(11)
#endif /* RTE_SMPU11 */

#if (RTE_SMPU12)
DECLARE_SMPU(12)
#endif /* RTE_SMPU12 */

#if (RTE_SMPU13)
DECLARE_SMPU(13)
#endif /* RTE_SMPU13 */

/* Note that SMPUs 14 and 15 are fixed by romboot */

#endif /* __DRIVER_SMPU_H__ */
