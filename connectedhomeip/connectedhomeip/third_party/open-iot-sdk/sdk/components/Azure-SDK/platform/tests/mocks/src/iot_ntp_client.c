/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iot_ntp_client.h"

DEFINE_FAKE_VALUE_FUNC(time_t, iotNtpClientGetTime);
DEFINE_FAKE_VALUE_FUNC(iotNtpClientState_t, iotNtpClientGetState);
