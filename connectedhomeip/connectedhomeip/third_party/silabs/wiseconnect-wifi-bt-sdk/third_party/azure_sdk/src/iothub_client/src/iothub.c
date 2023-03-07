// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_macro_utils/macro_utils.h"
#include "iothub.h"

int IoTHub_Init(void)
{
    int result;
    if (platform_init() != 0)
    {
        LogError("Platform initialization failed");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    return result;
}

void IoTHub_Deinit(void)
{
    platform_deinit();
}
