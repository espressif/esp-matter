// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/envvariable.h"

const char* environment_get_variable(const char *variable_name)
{
    return getenv(variable_name);
}

