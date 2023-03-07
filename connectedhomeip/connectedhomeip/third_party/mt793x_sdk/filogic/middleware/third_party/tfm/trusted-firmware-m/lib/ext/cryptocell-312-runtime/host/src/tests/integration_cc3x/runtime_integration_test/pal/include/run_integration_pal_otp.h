/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RUN_INTEGRATION_PAL_OTP_H_
#define _RUN_INTEGRATION_PAL_OTP_H_

/************************************************************
 *
 * macros
 *
 ************************************************************/
/** Write OTP value */
#define RUNIT_WRITE_OTP(wordOffset, val) { \
                volatile uint32_t ii1; \
                (*(volatile uint32_t *)(processMap.processTeeHwEnvBaseAddr + RUNIT_ENV_OTP_START_OFFSET+ ((wordOffset)*sizeof(uint32_t)))) = (uint32_t)(val); \
                for(ii1=0; ii1<500; ii1++);\
}

/** Read OTP value */
#define RUNIT_READ_OTP(wordOffset) \
                *(volatile uint32_t *)(processMap.processTeeHwEnvBaseAddr + RUNIT_ENV_OTP_START_OFFSET + ((wordOffset)*sizeof(uint32_t)))

#endif //_RUN_INTEGRATION_PAL_OTP_H_
