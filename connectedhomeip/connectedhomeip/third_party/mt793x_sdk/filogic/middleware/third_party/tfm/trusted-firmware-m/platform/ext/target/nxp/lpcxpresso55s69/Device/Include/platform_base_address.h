/*
 * Copyright (c) 2017-2019 Arm Limited. All rights reserved.
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

/**
 * \file platform_base_address.h
 * \brief This file defines all the peripheral base addresses for platform.
 */

#ifndef __PLATFORM_BASE_ADDRESS_H__
#define __PLATFORM_BASE_ADDRESS_H__

#include "platform_regs.h"           /* Platform registers */
#include "LPC55S69_cm33_core0.h"

/* Internal Flash memory */
#define FLASH0_BASE_S                 (0x10000000)
#define FLASH0_BASE_NS                (0x00000000)
#define FLASH0_SIZE                   (FLASH_TOTAL_SIZE)  /* 608 kB */
#define FLASH0_PAGE_SIZE              (0x00000200)  /* 512 B */

#endif  /* __PLATFORM_BASE_ADDRESS_H__ */
