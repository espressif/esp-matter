/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stdint.h"
#include <string.h>
#include <inttypes.h>

#include "run_integration_pal_log.h"
#include "run_integration_flash.h"
#include "run_integration_helper.h"

#include "test_pal_log.h"


/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static RunItError_t runIt_flashValidateInput(uint32_t addr, size_t len, uint8_t* buff);

/************************************************************
 *
 * variables
 *
 ************************************************************/
/** Holds the address of memory allocated for the flash chunk */
uint8_t *gFlashBaseAddr = NULL;

/** Holds the size of the allocated Flash size. */
uint32_t gFlashSize = 0;

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_flashValidateInput(uint32_t addr, size_t len, uint8_t* buff)
{
    int32_t remainder;

    /* validate null values */
    if (buff == NULL)
    {
        RUNIT_PRINT_ERROR("output buffer is NULL\n");
        return RUNIT_ERROR__FAIL;
    }

    /* validate address in range */
    remainder = ((int32_t)gFlashSize) - (addr + len);
    if (remainder < 0)
    {
        RUNIT_PRINT_ERROR("Attempting to read behind flash partition by %"PRId32" bytes\n", remainder * (-1));
        return RUNIT_ERROR__FAIL;
    }

    return RUNIT_ERROR__OK;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_flashFinalize(void)
{
    if (gFlashBaseAddr != NULL)
    {
        RUNIT_PRINT_DBG("finialize flash\n");

        RUNIT_TEST_PAL_FREE(gFlashBaseAddr);
        gFlashSize = 0;
        gFlashBaseAddr = NULL;
    }

    return RUNIT_ERROR__OK;

}

RunItError_t runIt_flashInit(size_t flashSize)
{
    /* if already allocated, print to log and return error */
    if (gFlashBaseAddr != NULL)
    {
        RUNIT_PRINT_DBG("Attempting to initialize flash 2nd time\n");
        return RUNIT_ERROR__OK;
    }

    RUNIT_PRINT_DBG("init flash\n");

    /* allocate mem */
    gFlashBaseAddr = (uint8_t*) RUNIT_TEST_PAL_ALLOC(flashSize);

    /* Validate */
    if (gFlashBaseAddr == NULL)
    {
        RUNIT_PRINT_ERROR("Failed to allocate %zu bytes for flash\n", flashSize);
        return RUNIT_ERROR__FAIL;
    }

    gFlashSize = flashSize;

    return RUNIT_ERROR__OK;
}

RunItError_t runIt_flashWrite(uint32_t addr, uint8_t* pBuff, size_t len)
{
    RunItError_t rc = runIt_flashValidateInput(addr, len, pBuff);
    if (rc != RUNIT_ERROR__OK)
    {
        RUNIT_PRINT_ERROR("Failed input validation\n");
        return RUNIT_ERROR__FAIL;
    }

    /* memcpy from buff */
    memcpy(gFlashBaseAddr + addr, pBuff, len);

    return rc;

}

RunItError_t runIt_flashRead(uint32_t addr, uint8_t* pBuff, size_t len)
{
    RunItError_t rc = runIt_flashValidateInput(addr, len, pBuff);
    if (rc != RUNIT_ERROR__OK)
    {
        RUNIT_PRINT_ERROR("Failed input validation\n");
        return RUNIT_ERROR__FAIL;
    }

    /* memcpy to buff */
    memcpy(pBuff, gFlashBaseAddr + addr, len);

    return rc;
}

