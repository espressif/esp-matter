/***************************************************************************//**
 * @file
 * @brief  CPC protocol Host functions
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "app/ezsp-host/cpc/cpc-host.h"
#include "app/util/ezsp/ezsp-enum.h"

bool ncpSleepEnabled = false;

EzspStatus hostError;                        // host error code
