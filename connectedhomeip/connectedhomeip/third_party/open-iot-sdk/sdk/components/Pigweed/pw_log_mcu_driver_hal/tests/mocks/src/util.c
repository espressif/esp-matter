/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pw_log_mdh/util.h"

DEFINE_FAKE_VOID_FUNC(_pw_log_init_lock);
DEFINE_FAKE_VOID_FUNC(_pw_log_lock);
DEFINE_FAKE_VOID_FUNC(_pw_log_unlock);
