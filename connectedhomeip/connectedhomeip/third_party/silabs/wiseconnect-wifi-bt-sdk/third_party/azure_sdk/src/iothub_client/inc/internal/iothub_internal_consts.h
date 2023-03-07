// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_INTERNAL_CONSTS_H
#define IOTHUB_INTERNAL_CONSTS_H

#ifdef __cplusplus
extern "C"
{
#endif

    static const char* IOTHUB_API_VERSION = "2020-09-30";
	static const char* SECURITY_INTERFACE_ID_MQTT = "ifid";
    static const char* SECURITY_INTERFACE_ID_VALUE = "urn:azureiot:Security:SecurityAgent:1"; 
    /* For future usage 
	static const char* SECURITY_INTERFACE_INTERNAL_ID = "iothub-interface-internal-id";
    static const char* SECURITY_INTERFACE_INTERNAL_ID_VALUE = "security*azureiot*com^SecurityAgent^1*0*0";
    static const char* SECURITY_INTERFACE_ID = "iothub-interface-id";
    static const char* SECURITY_MESSAGE_SCHEMA = "iothub-message-schema";
    static const char* SECURITY_MESSAGE_SCHEMA_VALUE = "sevent";
	*/
#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_INTERNAL_CONSTS_H */
