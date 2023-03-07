// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#include "azure_c_shared_utility/gb_rand.h"

/*this is rand*/
int gb_rand(void)
{
    return rand();
}
