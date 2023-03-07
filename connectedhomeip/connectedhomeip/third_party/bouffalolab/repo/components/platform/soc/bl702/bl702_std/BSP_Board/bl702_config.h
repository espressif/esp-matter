/**
 * @file bl702_config.h
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#ifndef __BL702_CONFIG_H__
#define __BL702_CONFIG_H__

#if defined(bl706_avb)
#include "bl706_avb/peripheral_config.h"
#include "bl706_avb/clock_config.h"
#include "bl706_avb/pinmux_config.h"
#elif defined(bl706_iot)
#include "bl706_iot/peripheral_config.h"
#include "bl706_iot/clock_config.h"
#include "bl706_iot/pinmux_config.h"
#elif defined(bl706_lp)
#include "bl706_lp/peripheral_config.h"
#include "bl706_lp/clock_config.h"
#include "bl706_lp/pinmux_config.h"
#elif defined(bl706_emac)
#include "bl706_emac/peripheral_config.h"
#include "bl706_emac/clock_config.h"
#include "bl706_emac/pinmux_config.h"
#elif defined(bl706_lp_gpio_wakeup)
#include "bl706_lp_gpio_wakeup/peripheral_config.h"
#include "bl706_lp_gpio_wakeup/clock_config.h"
#include "bl706_lp_gpio_wakeup/pinmux_config.h"
#elif defined(bl702_evb)
#include "bl702_evb/peripheral_config.h"
#include "bl702_evb/clock_config.h"
#include "bl702_evb/pinmux_config.h"
#elif defined(bl702_debugger)
#include "bl702_debugger/peripheral_config.h"
#include "bl702_debugger/clock_config.h"
#include "bl702_debugger/pinmux_config.h"
#elif defined(bl706_hwc)
#include "bl706_hwc/peripheral_config.h"
#include "bl706_hwc/clock_config.h"
#include "bl706_hwc/pinmux_config.h"
#elif defined(bl702_boot2)
#include "bl702_boot2/peripheral_config.h"
#include "bl702_boot2/clock_config.h"
#include "bl702_boot2/pinmux_config.h"
#else
#include "bl702_custom/peripheral_config.h"
#include "bl702_custom/clock_config.h"
#include "bl702_custom/pinmux_config.h"
#endif

#endif