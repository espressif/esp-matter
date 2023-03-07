/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __TST_COMMON_INIT_H__
#define __TST_COMMON_INIT_H__

#include "test_pal_thread.h"
#include "board_configs.h"


int tests_CC_libInit_wrap(CCRndContext_t* rndContext_ptr, CCRndWorkBuff_t * rndWorkBuff_ptr);


#endif /* __TST_COMMON_INIT_H__ */
