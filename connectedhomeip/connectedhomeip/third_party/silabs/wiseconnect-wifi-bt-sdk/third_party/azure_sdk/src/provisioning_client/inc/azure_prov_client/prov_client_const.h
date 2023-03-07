// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROV_CLIENT_CONST_H
#define PROV_CLIENT_CONST_H

#define PROV_DEVICE_CLIENT_VERSION "1.6.0"
#define PROV_GET_THROTTLE_TIME      1
#define MAX_PROV_GET_THROTTLE_TIME  5*60

static const char* const PROV_ASSIGNED_STATUS = "assigned";
static const char* const PROV_ASSIGNING_STATUS = "assigning";
static const char* const PROV_UNASSIGNED_STATUS = "unassigned";
static const char* const PROV_DISABLE_STATUS = "disabled";

static const char* const RETRY_AFTER_KEY_VALUE = "retry-after";

static const char* const PROV_API_VERSION = "2019-03-31";

#endif // PROV_CLIENT_CONST_H
