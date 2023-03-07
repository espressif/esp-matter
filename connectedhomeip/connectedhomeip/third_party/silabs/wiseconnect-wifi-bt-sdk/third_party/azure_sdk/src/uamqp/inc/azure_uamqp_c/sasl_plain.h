// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SASL_PLAIN_H
#define SASL_PLAIN_H

#include "azure_uamqp_c/sasl_mechanism.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

    typedef struct SASL_PLAIN_CONFIG_TAG
    {
        const char* authcid;
        const char* passwd;
        const char* authzid;
    } SASL_PLAIN_CONFIG;

    MOCKABLE_FUNCTION(, const SASL_MECHANISM_INTERFACE_DESCRIPTION*, saslplain_get_interface);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SASL_PLAIN_H */
