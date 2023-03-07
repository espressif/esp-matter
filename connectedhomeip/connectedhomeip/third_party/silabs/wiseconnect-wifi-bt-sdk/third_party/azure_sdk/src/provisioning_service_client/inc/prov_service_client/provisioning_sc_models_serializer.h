// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_MODELS_SERIALIZER_H
#define PROVISIONING_SC_MODELS_SERIALIZER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "provisioning_sc_enrollment.h"
#include "provisioning_sc_query.h"
#include "provisioning_sc_bulk_operation.h"

/** @brief  Serializes an Individual Enrollment into a JSON String.
*
* @param    enrollment      A handle for the Individual Enrollment to be serialized.
*
* @return   A non-NULL string containing the serialized JSON String, and NULL on failure.
*/
MOCKABLE_FUNCTION(, char*, individualEnrollment_serializeToJson, const INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);

/** @brief  Deserializes a JSON String representation of an Individual Enrollment.
*
* @param    json_string     A JSON String representing an Individual Enrollment.
*
* @return   A non-NULL handle representing an Individual Enrollment, and NULL on failure.
*/
MOCKABLE_FUNCTION(, INDIVIDUAL_ENROLLMENT_HANDLE, individualEnrollment_deserializeFromJson, const char*, json_string);

/** @brief  Serializes an Enrollment Group into a JSON String.
*
* @param    enrollment      A handle for the Enrollment Group to be serialized.
*
* @return   A non-NULL string containing the serialized JSON String, and NULL on failure.
*/
MOCKABLE_FUNCTION(, char*, enrollmentGroup_serializeToJson, const ENROLLMENT_GROUP_HANDLE, enrollment);

/** @brief  Deserializes a JSON String representation of an Enrollment Group.
*
* @param    json_string     A JSON String representing an Enrollment Group.
*
* @return   A non-NULL handle representing an Enrollment Group, and NULL on failure.
*/
MOCKABLE_FUNCTION(, ENROLLMENT_GROUP_HANDLE, enrollmentGroup_deserializeFromJson, const char*, json_string);

/** @brief  Deserializes a JSON String representation of a Device Registration State.
*
* @param    json_string     A JSON String representing a Device Registration State.
*
* @return   A non-NULL handle representing a Device Registration State, and NULL on failure.
*/
MOCKABLE_FUNCTION(, DEVICE_REGISTRATION_STATE_HANDLE, deviceRegistrationState_deserializeFromJson, const char*, json_string);

/** @brief  Serializes a Bulk Operation into a JSON String.
*
* @param    bulk_op      A pointer to a Bulk Operation structure
*
* @return   A non-NULL string containing the serialized JSON String, and NULL on failure.
*/
MOCKABLE_FUNCTION(, char*, bulkOperation_serializeToJson, const PROVISIONING_BULK_OPERATION*, bulk_op);

/** @brief  Deserializes a JSON String representation of a Bulk Operation Result.
*
* @param    json_string     A JSON String representing an Bulk Operation Result.
*
* @return   A non-NULL pointer to a Bulk Operation Result and NULL on failure.
*/
MOCKABLE_FUNCTION(, PROVISIONING_BULK_OPERATION_RESULT*, bulkOperationResult_deserializeFromJson, const char*, json_string);

/** @brief  Serializes a Query Specification into a JSON String.
*
* @param    query_spec      A pointer to a Query Specification structure
*
* @return   A non NULL string containing the serialized JSON String, and NULL on failure.
*/
MOCKABLE_FUNCTION(, char*, querySpecification_serializeToJson, const PROVISIONING_QUERY_SPECIFICATION*, query_spec);

/** @brief  Deserializes a JSON String representation of a Query Response.
*
* @param    json_string     A JSON String representing a Query Response.
* @param    type            The type of model the query is being done upon
*
* @return   A non-NULL pointer to a Query Response and NULL on failure.
*/
MOCKABLE_FUNCTION(, PROVISIONING_QUERY_RESPONSE*, queryResponse_deserializeFromJson, const char*, json_string, PROVISIONING_QUERY_TYPE, type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_MODELS_SERIALIZER_H */