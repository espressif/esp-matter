/***************************************************************************//**
 * @file mesh_lib.h
 * @brief Mesh helper library
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef MESH_LIB_H
#define MESH_LIB_H

/** Request flags */
typedef enum {
  /** Send request as nonrelayed (with TTL zero); response, if any,
      should likewise be sent as nonrelayed */
  MESH_REQUEST_FLAG_NONRELAYED = 0x01,
  /** Request needs a response */
  MESH_REQUEST_FLAG_RESPONSE_REQUIRED = 0x02,
} mesh_request_flags_t;

/** Response flags */
typedef enum {
  /** Send response as nonrelayed (with TTL zero); to be used
      when responding to a request that was sent as nonrelayed */
  MESH_RESPONSE_FLAG_NONRELAYED = 0x01,
} mesh_response_flags_t;

/**
 * @brief Convert transition time state value to milliseconds
 *
 * Converts from resolution+count format used in Mesh messages to
 * milliseconds
 *
 * @param transition_time Transition time value in Mesh representation
 *
 * @return Value converted to milliseconds
 */
uint32_t mesh_lib_transition_time_to_ms(uint8_t transition_time);

/***
 *** Library initialization
 ***/

/**
 * @brief Initialize Mesh helper library
 *
 * This function needs to be called before using other helper library
 * functions.
 *
 * @param initial_num The initial number of entries which the array is
 * allocated for
 * @param increment The number of extra entries allocated when needed
 * event handlers will be registered; if 0, no reallocation occurs; see
 * @ref mesh_lib_generic_client_register_handler and
 * @ref mesh_lib_generic_server_register_handler
 *
 * @return SL_STATUS_OK on success
 * @return SL_STATUS_NO_MORE_RESOURCE if memory allocation failed
 */
sl_status_t mesh_lib_init(size_t initial_num, size_t increment);

/**
 * @brief Initialize Mesh helper library
 *
 * This function needs to be called to release resources allocated by the
 * helper library. After calling it other helper library functions are not
 * to be used until the library is again initialized.
 */
void mesh_lib_deinit(void);

/***
 *** Generic event handlers
 ***/

/**
 * @brief Process a generic server event
 *
 * When a generic server event is passed to this function, it will
 * unpack the data contained in the message and call the registered
 * event handler for the correct model.
 *
 * @param evt Event to process
 */
void mesh_lib_generic_server_event_handler(sl_btmesh_msg_t *evt);

/**
 * @brief Process a generic client event
 *
 * When a generic client event is passed to this function, it will
 * unpack the data contained in the message and call the registered
 * event handler for the correct model.
 *
 * @param evt Event to process
 */
void mesh_lib_generic_client_event_handler(sl_btmesh_msg_t *evt);

/***
 *** Generic Server
 ***/

/**
 * @brief Client request handler function for generic server model
 *
 * When a generic server client request event is passed to
 * @ref mesh_lib_generic_server_event_handler it will call
 * the request handler function that has been registered
 * for the model that received the message. It will have
 * unpacked the request data into a generic request
 * structure and other parameters.
 *
 * @param model_id Model that received the message
 * @param element_index Element where the model resides
 * @param client_addr Mesh address of the client node that sent the request
 * @param server_addr Mesh destination address of the request; may be a
 * group address in case of a multicast request
 * @param appkey_index Application key index of the key used to encrypt
 * the request; the response, if any, has to be encrypted with the same key.
 * @param req Request parameters
 * @param transition_ms Requested transition time in milliseconds, or zero
 * for immediate state transition
 * @param delay_ms Requested delay time in milliseconds before a state
 * transition or an immediate state change should occur
 * @param request_flags Request flags
 */
typedef void
(*mesh_lib_generic_server_client_request_cb)(uint16_t model_id,
                                             uint16_t element_index,
                                             uint16_t client_addr,
                                             uint16_t server_addr,
                                             uint16_t appkey_index,
                                             const struct mesh_generic_request *req,
                                             uint32_t transition_ms,
                                             uint16_t delay_ms,
                                             uint8_t request_flags);

/**
 * @brief Server state change handler function for generic server model
 *
 * When a generic server state change event is passed to
 * @ref mesh_lib_generic_server_event_handler it will call the request
 * handler function that has been registered for the model that
 * emitted the state change event. It will have unpacked the event
 * data into a generic request structure and other parameters.
 *
 * This typically happens when the server changes a model state
 * that is bound to other states; for instance, if the Light
 * Lightness Actual state is set to zero from a non-zero value,
 * it will change also the bound Generic On/Off state to off.
 *
 * @param model_id Model that generated the event
 * @param element_index Element where the model resides
 * @param current Current model state
 * @param target Target model state, towards which model is moving;
 * in case there is no state transition ongoing a NULL pointer will
 * be given
 * @param remaining_ms Time remaining for transition in milliseconds;
 * in case there is no state transition ongoing a value of zero will
 * be given
 */
typedef void
(*mesh_lib_generic_server_change_cb)(uint16_t model_id,
                                     uint16_t element_index,
                                     const struct mesh_generic_state *current,
                                     const struct mesh_generic_state *target,
                                     uint32_t remaining_ms);

/**
 * @brief Server state recall handler function for generic server model
 *
 * When a generic server state recall event is passed to
 * @ref mesh_lib_generic_server_event_handler it will call the request
 * handler function that has been registered for the model that
 * emitted the state recall event. It will have unpacked the event
 * data into a generic request structure and other parameters.
 *
 * This happens when node has received state recall reques
 *
 * @param model_id Model that generated the event
 * @param element_index Element where the model resides
 * @param current Current model state
 * @param target Target model state, towards which model is moving;
 * in case there is no state transition ongoing a NULL pointer will
 * be given
 * @param transition_time_ms Time remaining for transition in milliseconds;
 * in case there is no state transition ongoing a value of zero will
 * be given
 */
typedef void
(*mesh_lib_generic_server_recall_cb)(uint16_t model_id,
                                     uint16_t element_index,
                                     const struct mesh_generic_state *current,
                                     const struct mesh_generic_state *target,
                                     uint32_t transition_time_ms);

/**
 * @brief Send a response to a client request
 *
 * When a server model has received a client request that needs to be
 * responsed to this function has to be called to push a status
 * message to the client, as well as update the stack internal state
 * to correspond to the new state.
 *
 * @param model_id Model that received the request being responded to
 * @param element_index Element where the model resides
 * @param client_addr Mesh address of the client node that sent the request
 * @param appkey_index Application key index of the key used to encrypt
 * the response; has to be the same key that was used to encrypt the request
 * @param current Current model state
 * @param target Target model state, towards which model is moving;
 * in case there is no state transition ongoing a NULL pointer should
 * be given
 * @param remaining_ms Time remaining for transition in milliseconds;
 * in case there is no state transition ongoing a zero value should be
 * given
 * @param response_flags Response flags
 *
 * @return bg_err_success if response was sent; an error otherwise
 */
sl_status_t
mesh_lib_generic_server_respond(uint16_t model_id,
                                uint16_t element_index,
                                uint16_t client_addr,
                                uint16_t appkey_index,
                                const struct mesh_generic_state *current,
                                const struct mesh_generic_state *target,
                                uint32_t remaining_ms,
                                uint8_t response_flags);

/**
 * @brief Update server state without sending a response to a client request
 *
 * When a server model has received a client request that does not
 * require a response, or if server model state needs to be updated
 * for some other reason (such as local action by a user, timed
 * activity, or transitioning towards target state) this function has
 * to be called to update the stack internal state to correspond to
 * the new state.
 *
 * @param model_id Model that is being updated
 * @param element_index Element where the model resides
 * @param current Current model state
 * @param target Target model state, towards which model is moving;
 * in case there is no state transition ongoing a NULL pointer should
 * be given
 * @param remaining_ms Time remaining for transition in milliseconds;
 * in case there is no state transition ongoing a zero value should be
 * given
 *
 * @return bg_err_success if update was done; an error otherwise
 */
sl_status_t
mesh_lib_generic_server_update(uint16_t model_id,
                               uint16_t element_index,
                               const struct mesh_generic_state *current,
                               const struct mesh_generic_state *target,
                               uint32_t remaining_ms);

/**
 * @brief Publish current server state
 *
 * This function can be called when the server model needs to
 * publish its state, for instance because of a local action
 * by a user or because of some pre-programmed event.
 *
 * @param model_id Model that is being published
 * @param element_index Element where the model resides
 * @param kind Type of data to be published; some models have only
 * one type of data to publish while others may have multiple
 *
 * @return bg_err_success if publishing was done; an error otherwise
 */
sl_status_t
mesh_lib_generic_server_publish(uint16_t model_id,
                                uint16_t element_index,
                                mesh_generic_state_t kind);

/**
 * @brief Register handler functions for a server model
 *
 * After this function is called @ref mesh_lib_generic_server_event_handler
 * will start passing client requests and server state changes to the
 * registered functions.
 *
 * @note It is possible to have any (but not all at once) callback to be NULL.
 * NULL function pointers are handled appropriately in
 * @ref mesh_lib_generic_server_event_handler.
 *
 * @param model_id Model for which functions are being registered
 * @param element_index Element where the model resides
 * @param request Function for client requests
 * @param change Function for server state changes
 * @param recall  Function for server state recall
 *
 * @return SL_STATUS_OK if registration succeeded
 * @return SL_STATUS_NULL_POINTER if all three callback function pointer inputs
 * are NULL
 * @return SL_STATUS_INVALID_STATE if model and element ID pair is already
 * registered
 * @return SL_STATUS_NO_MORE_RESOURCE if no empty registry entry could be
 * attained
 */
sl_status_t
mesh_lib_generic_server_register_handler(uint16_t model_id,
                                         uint16_t element_index,
                                         mesh_lib_generic_server_client_request_cb request,
                                         mesh_lib_generic_server_change_cb change,
                                         mesh_lib_generic_server_recall_cb recall);

/***
 *** Generic Client
 ***/

/**
 * @brief Server response handler function for generic client model
 *
 * When a generic client server response event is passed to
 * @ref mesh_lib_generic_client_event_handler it will call the response
 * handler function that has been registered for the model that
 * received the message. It will have unpacked the response data into
 * generic state structures and other parameters.
 *
 * @param model_id Model that received the message
 * @param element_index Element where the model resides
 * @param client_addr Mesh address of the client node that received the response
 * @param server_addr Mesh address of the server that sent the response
 * @param current Current server state
 * @param target Target server state, towards which server is moving;
 * in case there is no state transition ongoing a NULL pointer will
 * be given
 * @param remaining_ms Time remaining for transition in milliseconds;
 * in case there is no state transition ongoing a value of zero will
 * be given
 * @param response_flags Response flags
 */
typedef void
(*mesh_lib_generic_client_server_response_cb)(uint16_t model_id,
                                              uint16_t element_index,
                                              uint16_t client_addr,
                                              uint16_t server_addr,
                                              const struct mesh_generic_state *current,
                                              const struct mesh_generic_state *target,
                                              uint32_t remaining_ms,
                                              uint8_t response_flags);

/**
 * @brief Issue a get request
 *
 * A get request will be sent to server or servers; responses from
 * servers will be reported as generic client server response events.
 *
 * @param model_id Client model
 * @param element_index Element where the model resides
 * @param server_addr Address of the server to send the request to; may
 * be a group address in case of a multicast request
 * @param appkey_index Index of the application key used to encrypt the request
 * @param kind Type of state to get; some server models may have only
 * one kind of state to query while others may have several
 *
 * @return bg_err_success if request was made, an error code otherwise
 */
sl_status_t
mesh_lib_generic_client_get(uint16_t model_id,
                            uint16_t element_index,
                            uint16_t server_addr,
                            uint16_t appkey_index,
                            mesh_generic_state_t kind);

/**
 * @brief Issue a set request
 *
 * A set request will be sent to server or servers; responses from
 * servers will be reported as generic client server response events
 * if the request flag #MESH_REQUEST_FLAG_RESPONSE_REQUIRED is set.
 *
 * @param model_id Client model
 * @param element_index Element where the model resides
 * @param server_addr Address of the server to send the request to; may
 * be a group address in case of a multicast request
 * @param appkey_index Index of the application key used to encrypt the request
 * @param transaction_id Transaction ID, used for messages that are defined
 * to contain one and ignored for others
 * @param req Request structure
 * @param transition_ms Requested transition time in milliseconds,
 * or zero for immediate state transition
 * @param delay_ms Requested delay before starting transition or
 * setting the immediate value, in milliseconds
 * @param request_flags Request flags
 *
 * @return bg_err_success if request was made, an error code otherwise
 */
sl_status_t
mesh_lib_generic_client_set(uint16_t model_id,
                            uint16_t element_index,
                            uint16_t server_addr,
                            uint16_t appkey_index,
                            uint8_t transaction_id,
                            const struct mesh_generic_request *req,
                            uint32_t transition_ms,
                            uint16_t delay_ms,
                            uint8_t request_flags);

/**
 * @brief Publish a client request
 *
 * This function can be called when the client model needs to
 * publish a request, for instance because of a local action
 * by a user or because of some pre-programmed event.
 *
 * @param model_id Model that is being published
 * @param element_index Element where the model resides
 * @param transaction_id Transaction ID, used for messages that are defined
 * to contain one and ignored for others
 * @param req Request structure
 * @param transition_ms Requested transition time in milliseconds,
 * or zero for immediate state transition
 * @param delay_ms Requested delay before starting transition or
 * setting the immediate value, in milliseconds
 * @param request_flags Request flags
 *
 * @return bg_err_success if publishing was done; an error otherwise
 */
sl_status_t
mesh_lib_generic_client_publish(uint16_t model_id,
                                uint16_t element_index,
                                uint8_t transaction_id,
                                const struct mesh_generic_request *req,
                                uint32_t transition_ms,
                                uint16_t delay_ms,
                                uint8_t request_flags);

/**
 * @brief Register handler functions for a client model
 *
 * After this function is called
 * @ref mesh_lib_generic_client_event_handler will start passing server
 * response events to the registered functions.
 *
 * @param model_id Model for which functions are being registered
 * @param element_index Element where the model resides
 * @param response Function for server responses
 *
 * @return SL_STATUS_OK if registration succeeded
 * @return SL_STATUS_NULL_POINTER if all three callback function pointer inputs
 * are NULL
 * @return SL_STATUS_INVALID_STATE if model and element ID pair is already
 * registered
 * @return SL_STATUS_NO_MORE_RESOURCE if no empty registry entry could be
 * attained
 */
sl_status_t
mesh_lib_generic_client_register_handler(uint16_t model_id,
                                         uint16_t element_index,
                                         mesh_lib_generic_client_server_response_cb response);

#endif
