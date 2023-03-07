/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RUN_INTEGRATION_PAL_REG_H_
#define _RUN_INTEGRATION_PAL_REG_H_

/************************************************************
 *
 * macros
 *
 ************************************************************/
/** Write to register at offset (in words)*/
#define RUNIT_READ_REG(offset) \
        *(volatile uint32_t *)(processMap.processTeeHwRegBaseAddr + (offset))

/** Read from register at offset (in words) into val */
#define RUNIT_WRITE_REG(offset, val)  { \
    (*(volatile uint32_t *)(processMap.processTeeHwRegBaseAddr + (offset))) = (uint32_t)(val); \
}

#endif //_RUN_INTEGRATION_PAL_REG_H_
