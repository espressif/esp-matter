/***************************************************************************//**
 * @file mesh_sensor.h
 * @brief Mesh sensor model helpers
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
#ifndef MESH_SENSOR_H
#define MESH_SENSOR_H

#include "sl_status.h"
#include "sl_btmesh_device_properties.h"
#include "sl_btmesh_sensor_model_capi_types.h"

/**
 * @brief Sensor server init helper
 * @param elem_idx Index of the element containing the sensor server model
 * @param number_of_sensors Number of Sensor Descriptor states
 * @param descriptors Array of sensor_descriptor_t structs
 *
 * @return bg_err_success on success; an error code otherwise
 */
sl_status_t mesh_lib_sensor_server_init(uint16_t elem_idx, uint8_t number_of_sensors, const sensor_descriptor_t *descriptors);

/**
 * @brief Serialize sensor data entry
 * @param property_id Sensor Property ID
 * @param ptr buffer pointer
 * @param value Sensor Data encoded to mesh specific representation
 *
 * @return Length of encoded data
 */
uint8_t mesh_sensor_data_to_buf (uint16_t property_id, uint8_t *ptr, uint8_t *value);

/**
 * @brief Deserialize sensor data entry
 * @param property_id Sensor Property ID
 * @param ptr Buffer byte array pointer
 *
 * @return Property structure containing decoded data
 */
mesh_device_property_t mesh_sensor_data_from_buf (uint16_t property_id, const uint8_t *ptr);

/**
 * @brief Deserialize sensor descriptor states
 * @param descriptor Sensor Descriptor destination array
 * @param buf Byte array containing the serialized descriptors
 * @param input_len Length of the byte array
 *
 * @return bg_err_success on success; an error code otherwise
 */
sl_status_t mesh_lib_sensor_descriptors_from_buf(sensor_descriptor_t *descriptor, uint8_t *buf, int16_t input_len);

#endif
