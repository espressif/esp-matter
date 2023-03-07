/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include "test_proj.h"
#include "test_proj_defs.h"

#include "dx_reg_base_host.h"

#include "test_pal_map_addrs.h"
#include "test_pal_log.h"
#include "test_pal_mem.h"
#include "test_pal_mem_s.h"
#include "board_configs.h"


struct ProcessMappingArea_t processMap;

uint32_t Test_ProjMap(void)
{
    uint32_t error = 0;

    /* Initialize value */
    memset(&processMap, 0, sizeof(struct ProcessMappingArea_t));

    /* Init platform specific memories: DMAble & unmanaged */
    error = Test_HalBoardInit();
    if (error != 0) {
        TEST_PRINTF_ERROR("Failed to Test_HalBoardInit 0x%x", error);
        return error;
    }
#ifdef ARCH_V8M
    processMap.processTeeUnmanagedBaseAddr = Test_PalGetUnmanagedBaseAddr_s();
#else
    processMap.processTeeUnmanagedBaseAddr = Test_PalGetUnmanagedBaseAddr();
#endif

    /* Set relevant mapping regions for CC312-r1 tests */
    processMap.processTeeHwRegBaseAddr = (unsigned long)Test_PalIOMap((void *)DX_BASE_CC,
                                                                        TEST_PROJ_CC_REG_MAP_AREA_LEN);
    processMap.processTeeHwEnvBaseAddr = (unsigned long)Test_PalIOMap((void *)DX_BASE_ENV_REGS,
                                                                       TEST_PROJ_CC_REG_MAP_AREA_LEN);

    /* Verify all Maps succeeded */
    if ((!VALID_MAPPED_ADDR(processMap.processTeeHwRegBaseAddr)) ||
        (!VALID_MAPPED_ADDR(processMap.processTeeHwEnvBaseAddr)) ||
    (!VALID_MAPPED_ADDR(processMap.processTeeUnmanagedBaseAddr))) {
        TEST_PRINTF_ERROR("Failed to map, processTeeHwRegBaseAddr 0x%lx, processTeeHwEnvBaseAddr 0x%lx, \
                 processTeeUnmanagedBaseAddr 0x%lx\n",
                    processMap.processTeeHwRegBaseAddr,
                    processMap.processTeeHwEnvBaseAddr,
                     processMap.processTeeUnmanagedBaseAddr);
        goto end_with_error;

    }
    return TEST_OK;

end_with_error:
    Test_ProjUnmap();
    return TEST_MAPPING_ERR;

}

void Test_ProjUnmap(void)
{
    Test_PalUnmapAddr((void *)processMap.processTeeHwRegBaseAddr, TEST_PROJ_CC_REG_MAP_AREA_LEN);
    Test_PalUnmapAddr((void *)processMap.processTeeHwEnvBaseAddr, TEST_PROJ_CC_REG_MAP_AREA_LEN);
    Test_HalBoardFree();

    memset((uint8_t *)&processMap, 0, sizeof(struct ProcessMappingArea_t));
    return;
}
