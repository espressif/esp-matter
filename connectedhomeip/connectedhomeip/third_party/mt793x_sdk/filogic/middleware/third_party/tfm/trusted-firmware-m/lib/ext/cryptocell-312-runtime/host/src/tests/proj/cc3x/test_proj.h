/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TEST_PROJ_H_
#define _TEST_PROJ_H_

#include <stdint.h>
#include "test_proj_common.h"

#define TEST_PROJ_CC_REG_MAP_AREA_LEN     0x20000

struct ProcessMappingArea_t {
    unsigned long       processTeeHwRegBaseAddr;
    unsigned long       processTeeHwEnvBaseAddr;
    unsigned long       processTeeUnmanagedBaseAddr;
};

extern struct ProcessMappingArea_t processMap;

#define TEST_READ_TEE_CC_REG(offset) \
        *(volatile uint32_t *)(processMap.processTeeHwRegBaseAddr + (offset))

#define TEST_WRITE_TEE_CC_REG(offset, val)  { \
    volatile uint32_t ii1; \
        (*(volatile uint32_t *)(processMap.processTeeHwRegBaseAddr + (offset))) = (uint32_t)(val); \
        for(ii1=0; ii1<500; ii1++); \
}

#define TEST_READ_TEE_ENV_REG(offset) \
        *(volatile uint32_t *)(processMap.processTeeHwEnvBaseAddr + (offset))

#define TEST_WRITE_TEE_ENV_REG(offset, val)  { \
        volatile uint32_t ii1; \
        (*(volatile uint32_t *)(processMap.processTeeHwEnvBaseAddr + (offset))) = (uint32_t)(val); \
        for(ii1=0; ii1<500; ii1++); \
}


/****************************************************************************/
/*                              External API                                */
/****************************************************************************/
/*
 * @brief This function initializes platform, i.e maps its relevant memory regions.
 *
 * @param[in/out] *pProcessMap - mapping regions
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_ProjInit(void);

/****************************************************************************/
/*
 * @brief This function frees previously allocated resources
 *
 * @param[in/out] *pProcessMap - mapping regions
  *
 * @return rc - 0 for success, 1 for failure
 */
void Test_ProjFree(void);

#endif //_TEST_PROJ_H_

