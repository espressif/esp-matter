/***************************************************************************//**
 * @file mesh_serdeser.h
 * @brief Mesh generic model data serialization/deserialization functions
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

#ifndef MESH_SERDESER_H
#define MESH_SERDESER_H

/**
 * @brief Serialize model state to a byte array
 *
 * This is a function needed by the helper library; application does not
 * need to call it directly.
 *
 * @param current Current model state
 * @param target Target model state
 * @param msg_buf Buffer to write serialized state to
 * @param msg_len Length of buffer
 * @param msg_used Return parameter for storing the length of serialized data
 *
 * @return Zero for success; SL_STATUS_INVALID_PARAMETER for unknown type
 * or incorrect data length
 */
sl_status_t mesh_lib_serialize_state(const struct mesh_generic_state *current,
                                     const struct mesh_generic_state *target,
                                     uint8_t *msg_buf,
                                     size_t msg_len,
                                     size_t *msg_used);

/**
 * @brief Deserialize model state from a byte array
 *
 * This is a function needed by the helper library; application does not
 * need to call it directly.
 *
 * @param current Pointer to store current model state
 * @param target Pointer to store target model state
 * @param has_target Return parameter for storing whether buffer contains
 * only current or both current and target state
 * @param kind Type of data in buffer
 * @param msg_buf Buffer to read serialized state from
 * @param msg_len Length of buffer
 *
 * @return Zero for success; SL_STATUS_INVALID_PARAMETER for unknown type
 * or incorrect data length
 */
sl_status_t mesh_lib_deserialize_state(struct mesh_generic_state *current,
                                       struct mesh_generic_state *target,
                                       int *has_target,
                                       mesh_generic_state_t kind,
                                       const uint8_t *msg_buf,
                                       size_t msg_len);

/**
 * @brief Serialize model request to a byte array
 *
 * This is a function needed by the helper library; application does not
 * need to call it directly.
 *
 * @param req Request structure
 * @param msg_buf Buffer to write serialized request to
 * @param msg_len Length of buffer
 * @param msg_used Return parameter for storing the length of serialized data
 *
 * @return Zero for success; SL_STATUS_INVALID_PARAMETER for unknown type
 * or incorrect data length
 */
sl_status_t mesh_lib_serialize_request(const struct mesh_generic_request *req,
                                       uint8_t *msg_buf,
                                       size_t msg_len,
                                       size_t *msg_used);

/**
 * @brief Deserialize model request from a byte array
 *
 * This is a function needed by the helper library; application does not
 * need to call it directly.
 *
 * @param req Pointer to store request data
 * @param kind Type of data in buffer
 * @param msg_buf Buffer to read serialized state from
 * @param msg_len Length of buffer
 *
 * @return Zero for success; SL_STATUS_INVALID_PARAMETER for unknown type
 * or incorrect data length
 */
sl_status_t mesh_lib_deserialize_request(struct mesh_generic_request *req,
                                         mesh_generic_request_t kind,
                                         const uint8_t *msg_buf,
                                         size_t msg_len);

#endif // MESH_SERDESER_H
