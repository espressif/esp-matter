/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_crt_bundle.h>

#ifdef __cplusplus
#include <string>
#include <functional>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <cJSON.h>

extern "C" {
#endif

/**
 * @brief Group operation types for node management
 */
typedef enum {
    ESP_RAINMAKER_API_ADD_NODE_TO_GROUP = 0,    /* Add node to group */
    ESP_RAINMAKER_API_REMOVE_NODE_FROM_GROUP,   /* Remove node from group */
} esp_rainmaker_api_group_operation_type_t;

/**
 * @brief Node mapping operation types
 */
typedef enum {
    ESP_RAINMAKER_API_ADD_NODE_MAPPING = 0,    /* Add node mapping */
    ESP_RAINMAKER_API_REMOVE_NODE_MAPPING,     /* Remove node mapping */
} esp_rainmaker_api_node_mapping_operation_type_t;

/**
 * @brief Node mapping status types
 */
typedef enum {
    ESP_RAINMAKER_API_NODE_MAPPING_STATUS_REQUESTED = 0,    /* Node mapping status requested */
    ESP_RAINMAKER_API_NODE_MAPPING_STATUS_CONFIRMED,        /* Node mapping status confirmed */
    ESP_RAINMAKER_API_NODE_MAPPING_STATUS_TIMEOUT,          /* Node mapping status timeout */
    ESP_RAINMAKER_API_NODE_MAPPING_STATUS_DISCARDED,        /* Node mapping status discarded */
    ESP_RAINMAKER_API_NODE_MAPPING_STATUS_INTERNAL_ERROR,   /* Node mapping status internal error */
} esp_rainmaker_api_node_mapping_status_type_t;

/* Login to Rainmaker cloud using refresh token
 * This function attempts to login to the Rainmaker cloud using the stored refresh token.
 * If successful, it will store the access token for subsequent API calls.
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_login(void);

/* Get user ID
 * This function retrieves the user ID associated with the Rainmaker account.
 * Returns user ID string
 */
char *esp_rainmaker_api_get_user_id(void);

/* Get all nodes associated with the account
 * This function retrieves all nodes (devices) associated with the Rainmaker account.
 * It will automatically handle pagination and create corresponding device objects.
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_refresh_nodes(void);

/* Get nodes list associated with the account
 * This function retrieves all nodes (devices) associated with the Rainmaker account.
 * The caller is responsible for freeing the returned string.
 * Returns JSON string with nodes list (caller must free), nullptr on error
 */
char* esp_rainmaker_api_get_nodes_list(void);

/* Get node config
 * This function retrieves the config of a node (device) in the Rainmaker cloud.
 * The caller is responsible for freeing the returned string.
 * @param node_id Node ID
 * Returns JSON string with node config (caller must free), nullptr on error
 */
char* esp_rainmaker_api_get_node_config(const char* node_id);

/* Set node parameters
 * This function updates the parameters of a node (device) in the Rainmaker cloud.
 * The parameters should be provided as a JSON string.
 * @param payload JSON string containing parameter updates
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_set_node_params(const char* payload);

/* Get node parameters
 * This function retrieves the current parameters of a specific node (device).
 * The caller is responsible for freeing the returned string.
 * @param node_id ID of the node to query
 * Returns JSON string with node parameters (caller must free), nullptr on error
 */
char* esp_rainmaker_api_get_node_params(const char* node_id);

/* Set refresh token for authentication
 * This function stores the refresh token that will be used for authentication.
 * The refresh token is used to obtain access tokens for API calls.
 * @param refresh_token The refresh token string
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_set_refresh_token(const char* refresh_token);

/* Set base URL for Rainmaker API
 * This function sets the base URL for the Rainmaker API.
 * @param base_url The base URL string
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_set_base_url(const char* base_url);

/* Delete stored refresh token
 * This function clears the stored refresh token and access token.
 * This should be called when logging out or when the tokens are no longer valid.
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_delete_refresh_token(void);

/* Get all groups
 * This function retrieves all groups associated with the Rainmaker account.
 * The caller is responsible for freeing the returned string.
 * Returns JSON string with groups (caller must free), nullptr on error
 */
char* esp_rainmaker_api_get_group(void);

/* Create a new group
 * This function creates a new group in the Rainmaker cloud.
 * @param group_name Name of the group to create
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_create_group(const char* group_name);

/* Delete a group
 * This function deletes an existing group from the Rainmaker cloud.
 * @param group_id ID of the group to delete
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_delete_group(const char* group_id);

/* Add or remove node from group
 * This function adds or removes a node (device) from a group.
 * The operation type determines whether to add or remove the node.
 * @param node_id ID of the node to operate on
 * @param group_id ID of the target group
 * @param operation_type Operation type (add or remove)
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_operate_node_to_group(const char* node_id, const char* group_id,
                                                  esp_rainmaker_api_group_operation_type_t operation_type);

/* Set node mapping
 * This function sets the node mapping for a user.
 * @param user_id User ID
 * @param secret_key Secret key
 * @param node_id Node ID
 * @param operation_type Operation type (add or remove)
 * @param request_id Request ID to store the request ID
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_set_node_mapping(const char* user_id, const char* secret_key, const char* node_id,
                                             esp_rainmaker_api_node_mapping_operation_type_t operation_type, char *request_id);

/* Get node mapping status
 * This function retrieves the status of a node mapping request.
 * @param request_id Request ID
 * Returns node mapping status
 */
esp_rainmaker_api_node_mapping_status_type_t esp_rainmaker_api_get_node_mapping_status(const char *request_id);

/* Get node connection status
 * This function retrieves the connection status of a node.
 * @param node_id Node ID
 * @param connection_status Pointer to store connection status
 * Returns ESP_OK on success, error code otherwise
 */
esp_err_t esp_rainmaker_api_get_node_connection_status(const char *node_id, bool *connection_status);

#ifdef __cplusplus
}

/**
 * @brief Rainmaker API client class (Singleton)
 *
 * This class provides a unified interface for interacting with ESP Rainmaker cloud services.
 * It handles authentication, node management, and group operations.
 */
class RainmakerApi {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to the singleton instance
     */
    static RainmakerApi &GetInstance()
    {
        static RainmakerApi instance;
        return instance;
    }

    /* Disable copy constructor and assignment operator */
    RainmakerApi(const RainmakerApi &) = delete;
    RainmakerApi &operator=(const RainmakerApi &) = delete;

    /**
     * @brief Login to Rainmaker cloud using refresh token
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t Login(void);

    /**
     * @brief Get user info
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t GetUserInfo(void);

    /**
     * @brief Get all nodes associated with the account
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t RefreshNodes(void);

    /**
     * @brief Get nodes list associated with the account
     * @return JSON string with node list (caller must free), nullptr on error
     */
    char* GetNodeList(void);

    /**
     * @brief Get node config
     * @param node_id Node ID
     * @return JSON string with node config (caller must free), nullptr on error
     */
    char* GetNodeConfig(const char* node_id);

    /**
     * @brief Set node parameters
     * @param payload JSON payload containing parameter updates
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t SetNodeParams(const char* payload);

    /**
     * @brief Get node parameters
     * @param node_id Node ID to query
     * @return JSON string with node parameters (caller must free), nullptr on error
     */
    char* GetNodeParams(const char* node_id);

    /**
     * @brief Set refresh token for authentication
     * @param refresh_token The refresh token string
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t SetRefreshToken(const char* refresh_token);

    /**
     * @brief Set base URL for Rainmaker API
     * @param base_url The base URL string
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t SetBaseUrl(const char* base_url);

    /**
     * @brief Delete stored refresh token
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t DeleteRefreshToken(void);

    /**
     * @brief Get all groups
     * @return JSON string with groups (caller must free), nullptr on error
     */
    char* GetGroup(void);

    /**
     * @brief Create a new group
     * @param group_name Name of the group to create
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t CreateGroup(const char* group_name);

    /**
     * @brief Delete a group
     * @param group_id ID of the group to delete
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t DeleteGroup(const char* group_id);

    /**
     * @brief Add or remove node from group
     * @param node_id ID of the node
     * @param group_id ID of the group
     * @param operation_type Operation type (add or remove)
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t OperateNodeToGroup(const char* node_id, const char* group_id,
                                 esp_rainmaker_api_group_operation_type_t operation_type);

    /**
     * @brief Set node mapping
     * @param user_id User ID
     * @param secret_key Secret key
     * @param node_id Node ID
     * @param operation_type Operation type (add or remove)
     * @param request_id Request ID to store the request ID
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t SetNodeMapping(const char* user_id, const char* secret_key, const char* node_id,
                             esp_rainmaker_api_node_mapping_operation_type_t operation_type, char *request_id);

    /**
     * @brief Get node mapping status
     * @param request_id Request ID
     * @return Node mapping status
     */
    esp_rainmaker_api_node_mapping_status_type_t GetNodeMappingStatus(const char *request_id);

    /**
     * @brief Get node connection status
     * @param node_id Node ID
     * @param connection_status Pointer to store connection status
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t GetNodeConnectionStatus(const char *node_id, bool *connection_status);

    /**
     * @brief Get user id string (for C API)
     */
    const std::string &GetUserId() const;

private:
    /**
     * @brief Private constructor for singleton pattern
     */
    RainmakerApi();

    /**
     * @brief Private destructor
     */
    ~RainmakerApi();

    std::string access_token_;   /* Current access token */
    std::string refresh_token_;  /* Stored refresh token */
    std::string base_url_;       /* Base URL for Rainmaker API */
    std::string user_id_;        /* User ID */

    /**
     * @brief Recursively get nodes with pagination
     * @param start_id Starting node ID for pagination
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t GetNodesRecursive(const char* start_id);

    /**
     * @brief Cleanup HTTP client resources
     * @param client HTTP client handle
     * @param post_data Optional POST data to free
     */
    static void CleanupHttpClient(void* client, char* post_data = nullptr);

    /**
     * @brief Make HTTP request
     * @param client HTTP client handle
     * @param post_data Optional POST data
     * @return ESP_OK on success, error code otherwise
     */
    static esp_err_t MakeHttpRequest(esp_http_client_handle_t client, const char* post_data = nullptr);

    /**
     * @brief Handle HTTP response and check for authentication errors
     * @param client HTTP client handle
     * @param response_data Pointer to store response data
     * @param retry_func Optional retry function to call on authentication error
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t HandleHttpResponse(esp_http_client_handle_t client, char** response_data,
                                 std::function<esp_err_t()> retry_func);

    /**
     * @brief Read HTTP response data
     * @param client HTTP client handle
     * @param response_data Pointer to store response data (caller must free)
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t ReadHttpResponse(esp_http_client_handle_t client, char** response_data);
};

#endif /* __cplusplus */
