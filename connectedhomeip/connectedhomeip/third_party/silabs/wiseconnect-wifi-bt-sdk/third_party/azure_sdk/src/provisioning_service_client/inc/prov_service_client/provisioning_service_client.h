// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SERVICE_CLIENT_H
#define PROVISIONING_SERVICE_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/shared_util_options.h"

#include "provisioning_sc_models.h"
#include "provisioning_sc_query.h"
#include "provisioning_sc_bulk_operation.h"

#define TRACING_STATUS_VALUES \
        TRACING_STATUS_ON,\
        TRACING_STATUS_OFF
MU_DEFINE_ENUM_WITHOUT_INVALID(TRACING_STATUS, TRACING_STATUS_VALUES);

/** @brief  Handle to hide struct and use it in consequent APIs
*/
typedef struct PROVISIONING_SERVICE_CLIENT_TAG* PROVISIONING_SERVICE_CLIENT_HANDLE;

/** @brief  Creates a Provisioning Service Client handle for use in consequent APIs.
*
* @param    conn_string     A connection string used to establish connection with the Provisioning Service.
*
* @return   A non-NULL PROVISIONING_SERVICE_CLIENT_HANDLE value that is used when invoking other functions in the Provisioning Service Client
*           and NULL on failure.
*/
MOCKABLE_FUNCTION(, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_sc_create_from_connection_string, const char*, conn_string);

/** @brief  Disposes of resources allocated by creating a Provisioning Service Client handle.
*
* @param    prov_client     The handle created by a call to the create function.
*/
MOCKABLE_FUNCTION(, void, prov_sc_destroy, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client);

/** @brief  Sets tracing/logging of http communications on or off.
*
* @param    prov_client     The handle for the connection that should be traced.
* @param    status          The tracing status to set.
*/
MOCKABLE_FUNCTION(, void, prov_sc_set_trace, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, TRACING_STATUS, status);

/** @brief  Set the trusted certificate for HTTP communication with the Provisioning Service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    certificate     The trusted certificate to be used for HTTP connections. If given as NULL, will clear a previously set certificate.
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_set_certificate, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, const char*, certificate);

/** @brief  Set the proxy options for HTTP communication with the Provisioning Service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    proxy_options   A struct containing the desired proxy settings
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_set_proxy, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, HTTP_PROXY_OPTIONS*, proxy_options);

/** @brief Creates or updates an individual device enrollment record on the Provisioning Service, reflecting the changes in the given struct.
*
* @param    prov_client         The handle used for connecting to the Provisioning Service.
* @param    enrollment_ptr      Pointer to a handle for a new or updated individual enrollment (will be updated with new info from the Provisioning Service).
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_create_or_update_individual_enrollment, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, INDIVIDUAL_ENROLLMENT_HANDLE*, enrollment_ptr);

/** @brief  Deletes a individual device enrollment record on the Provisioning Service.
*
* @param    prov_client    The handle used for connecting to the Provisioning Service.
* @param    enrollment     The handle for the target individual enrollment. Will be matched based on registration id and etag.
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_delete_individual_enrollment, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);

/** @brief  Deletes an individual device enrollment record on the Provisioning Service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    reg_id          The registration id of the target individual enrollment.
* @param    etag            The etag of the target individual enrollment. If given as "*", will match any etag. If given as NULL, will be ignored.
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_delete_individual_enrollment_by_param, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, const char*, reg_id, const char*, etag);

/** @brief  Retreives an individual device enrollment record from the Provisioning Service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    reg_id          The registration id of the target individual enrollment.
* @param    enrollment      Pointer to a handle for an individual enrollment, to be filled with retreived data.
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_get_individual_enrollment, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, const char*, reg_id, INDIVIDUAL_ENROLLMENT_HANDLE*, enrollment_ptr);


/** @brief  Queries individual device enrollment records from the Provisioning Service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    query_spec      The query specification with query details and settings
* @param    cont_token_ptr  A pointer to a continuation token, which will be updated based on the response
* @param    query_resp_ptr  A pointer to a query response pointer, which will be filled with retrieved data
*
* @return   0 upon success, a non-zero number upon failure
*/
MOCKABLE_FUNCTION(, int, prov_sc_query_individual_enrollment, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, PROVISIONING_QUERY_SPECIFICATION*, query_spec, char**, cont_token_ptr, PROVISIONING_QUERY_RESPONSE**, query_resp_ptr);

/** @brief  Performs a bulk operation on individual device enrollment records from the provisioning service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    bulk_op         A pointer to a bulk operation structure with details about the bulk operation.
* @param    bulk_res_ptr    A pointer to a bulk operation result pointer that will be filled with the results upon completion
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_run_individual_enrollment_bulk_operation, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, PROVISIONING_BULK_OPERATION*, bulk_op, PROVISIONING_BULK_OPERATION_RESULT**, bulk_res_ptr);

/** @brief  Creates or updates a device enrollment group record on the Provisioning Service.
*
* @param    prov_client         The handle used for connecting to the Provisioning Service.
* @param    enrollment_ptr      Pointer to a handle for a new or updated enrollment group.
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_create_or_update_enrollment_group, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, ENROLLMENT_GROUP_HANDLE*, enrollment_ptr);

/** @brief  Deletes a device enrollment group record on the Provisioning Service.
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    enrollment      The handle for the target enrollment group
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_delete_enrollment_group, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, ENROLLMENT_GROUP_HANDLE, enrollment);

/** @brief  Deletes a device enrollment group record on the Provisioning Service.
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    group_id        The enrollment group id of the target enrollment group.
* @param    etag            The etag of the target enrollment group. If given as "*", will match any etag.
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_delete_enrollment_group_by_param, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, const char*, group_id, const char*, etag);

/** @brief  Retreives a device enrollment group record from the Provisioning Service.
*
* @param    prov_client         The handle used for connecting to the Provisioning Service.
* @param    group_id            The enrollment group id of the target enrollment group.
* @param    enrollment_ptr      A pointer to a handle for an enrollment group, to be filled with the retreived data.
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_get_enrollment_group, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, const char*, group_id, ENROLLMENT_GROUP_HANDLE*, enrollment_ptr);

/** @brief  Queries enrollment group records from the Provisioning Service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    query_spec      The query specification with query details and settings
* @param    cont_token_ptr  A pointer to a continuation token, which will be updated based on the response
* @param    query_resp_ptr  A pointer to a query response pointer, which will be filled with retrieved data
*
* @return   0 upon success, a non-zero number upon failure
*/
MOCKABLE_FUNCTION(, int, prov_sc_query_enrollment_group, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, PROVISIONING_QUERY_SPECIFICATION*, query_spec, char**, cont_token_ptr, PROVISIONING_QUERY_RESPONSE**, query_resp_ptr);

/** @brief  Deletes a device registration state on the Provisioning Service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    reg_state       The handle for the target device registration state.
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_delete_device_registration_state, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, DEVICE_REGISTRATION_STATE_HANDLE, reg_state);

/** @brief  Deletes a device registration state on the Provisioning Service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    reg_id          The registration id of the target registration state.
* @param    etag            The etag of the target registration state
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_delete_device_registration_state_by_param, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, const char*, reg_id, const char*, etag);

/** @brief  Retreives a device registration state from the Provisioning Service.
*
* @param    prov_client     A handle used for connecting to the Provisioning Service.
* @param    reg_id          The registration id of the target registration status.
* @param    reg_state_ptr   A pointer to a handle for a registration state, to be filled with retreived data.
*
* @return   0 upon success, a non-zero number upon failure.
*/
MOCKABLE_FUNCTION(, int, prov_sc_get_device_registration_state, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, const char*, reg_id, DEVICE_REGISTRATION_STATE_HANDLE*, reg_state_ptr);

/** @brief  Queries device registration state records from the Provisioning Service.
*
* @param    prov_client     The handle used for connecting to the Provisioning Service.
* @param    query_spec      The query specification with query details and settings
* @param    cont_token_ptr  A pointer to a continuation token, which will be updated based on the response
* @param    query_resp_ptr  A pointer to a query response pointer, which will be filled with retrieved data
*
* @return   0 upon success, a non-zero number upon failure
*/
MOCKABLE_FUNCTION(, int, prov_sc_query_device_registration_state, PROVISIONING_SERVICE_CLIENT_HANDLE, prov_client, PROVISIONING_QUERY_SPECIFICATION*, query_spec, char**, cont_token_ptr, PROVISIONING_QUERY_RESPONSE**, query_resp_ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SERVICE_CLIENT_H */
