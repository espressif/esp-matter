/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stdlib.h"
#include "stdint.h"
#include <string.h>

#include "cc_pal_types.h"
#include "test_pal_cli.h"
#include "dx_reg_base_host.h"

#include "cmpu_integration_test.h"

/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static int cmpuIt_cliCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/************************************************************
 *
 * static functions
 *
 ************************************************************/
/**
 * @brief A wrapper function for the "all" command
 */
static int cmpuIt_cliCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    CC_UNUSED_PARAM(pcWriteBuffer);
    CC_UNUSED_PARAM(xWriteBufferLen);
    CC_UNUSED_PARAM(pcCommandString);

    cmpuIt_all();

    return 0;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
/**
 * This function will be called by freertos before the scheduler starts running.
 * it's main responsibility is to register the test to the cli interface.
 *
 * @return    0 on success
 */
int libcmpu_integration_test_initLib(void)
{
    int rc = 0;

    static struct Test_PalCliCommand cmdAll =
    {
        "cmpu",
        "\r\nc: \r\n Run CMPU test\r\n",
        cmpuIt_cliCommand,
        0
    };

    static struct Test_PalCliCommand cmdAlias =
    {
        "c",
        "\r\nc: \r\n Run CMPU test\r\n",
        cmpuIt_cliCommand,
        0
    };

    rc += Test_PalCLIRegisterCommand(&cmdAll);
    rc += Test_PalCLIRegisterCommand(&cmdAlias);

    return rc;
}
