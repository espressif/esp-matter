/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#include <stdio.h>
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "test_pal_cli.h"

/******************************************************************************/
uint32_t Test_PalCLIRegisterCommand(struct Test_PalCliCommand *commandToRegister)
{
    if (FreeRTOS_CLIRegisterCommand(
        (CLI_Command_Definition_t *)commandToRegister) == pdFAIL)
        return 1;

    return 0;
}

/******************************************************************************/
const char *Test_PalCLIGetParameter(const char *commandString,
            uint32_t wantedParamIndx, uint32_t *paramStringLength)
{
    return FreeRTOS_CLIGetParameter(commandString, wantedParamIndx,
                    (BaseType_t *)paramStringLength);
}
