// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_QUERY_H
#define PROVISIONING_SC_QUERY_H

#ifdef __cplusplus
extern "C" {
#include <cstdlib>
#endif /* __cplusplus */

#include <stdlib.h>

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "provisioning_sc_models.h"
#include "parson.h"

#define PROVISIONING_QUERY_SPECIFICATION_VERSION_1 1
#define NO_MAX_PAGE_SIZE 0

#define PROVISIONING_QUERY_TYPE_VALUES \
    QUERY_TYPE_INVALID, \
    QUERY_TYPE_INDIVIDUAL_ENROLLMENT, \
    QUERY_TYPE_ENROLLMENT_GROUP, \
    QUERY_TYPE_DEVICE_REGISTRATION_STATE

MU_DEFINE_ENUM_WITHOUT_INVALID(PROVISIONING_QUERY_TYPE, PROVISIONING_QUERY_TYPE_VALUES);

typedef struct PROVISIONING_QUERY_SPECIFICATION_TAG
{
    int version;
    const char* query_string;
    const char* registration_id;
    size_t page_size;
} PROVISIONING_QUERY_SPECIFICATION;

typedef struct PROVISIONING_QUERY_RESPONSE_TAG
{
    union {
        INDIVIDUAL_ENROLLMENT_HANDLE* ie;
        ENROLLMENT_GROUP_HANDLE* eg;
        DEVICE_REGISTRATION_STATE_HANDLE* drs;
    } response_arr;
    size_t response_arr_size;
    PROVISIONING_QUERY_TYPE response_arr_type;
} PROVISIONING_QUERY_RESPONSE;

MOCKABLE_FUNCTION(, void, queryResponse_free, PROVISIONING_QUERY_RESPONSE*, query_resp);

/*---INTERNAL USAGE ONLY---*/
MOCKABLE_FUNCTION(, PROVISIONING_QUERY_TYPE, queryType_stringToEnum, const char*, string);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_QUERY_H */
