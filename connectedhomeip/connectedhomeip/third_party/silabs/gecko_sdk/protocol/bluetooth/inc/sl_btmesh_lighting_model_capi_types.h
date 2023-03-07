/******************************************************************************/
/**
 * @file   sl_btmesh_lighting_model_capi_types.h
 * @brief  Silicon Labs Bluetooth Mesh Lighting Model API
 *
 * Please see the @ref mesh_generic "generic model API"
 *
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

/*
 * C API for lighting models
 */

#ifndef SL_BTMESH_LIGHTING_MODEL_CAPI_TYPES_H
#define SL_BTMESH_LIGHTING_MODEL_CAPI_TYPES_H

#include "sl_btmesh_model_specification_defs.h"

/** Light CTL Temperature Minimum */
#define MESH_LIGHTING_CTL_TEMPERATURE_MIN 0x0320

/** Light CTL Temperature Maximum */
#define MESH_LIGHTING_CTL_TEMPERATURE_MAX 0x4e20

/**
 * API for handling state binding between LC and Light Lightness model
 */
typedef void (*mesh_lc_light_lightness_linear_binding_fn) (mesh_model_t model);

#endif /* SL_BTMESH_LIGHTING_MODEL_CAPI_TYPES_H */
