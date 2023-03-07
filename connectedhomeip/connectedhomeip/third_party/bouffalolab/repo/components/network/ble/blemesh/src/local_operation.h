/*  Bluetooth Mesh */

#ifndef __LOCAL_OPERATION_H__
#define __LOCAL_OPERATION_H__

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

int bt_mesh_model_subscribe_group_addr(u16_t elem_addr, u16_t mod_id,
                                       u16_t cid, u16_t group_addr);

int bt_mesh_model_unsubscribe_group_addr(u16_t elem_addr, u16_t cid,
                                         u16_t mod_id, u16_t group_addr);

/** @def    bt_mesh_local_model_bind
 *
 *  @brief  bind all local model to appkey.
 *
 *  @note   When appkey is added, use this function to quickly bind
 *          all local model.
 *
 *  @param  net_idx The index of network key.
 *  @param  mod_app_idx The index of application key.
 *
 *  @return 0[success],[!0] Error code.
 */
int bt_mesh_local_model_bind(u16_t net_idx, u16_t mod_app_idx);

/** @def    bt_mesh_local_model_bind_direct
 *
 *  @brief  bind all local model to appkey, same as bt_mesh_local_model_bind.
    But don't use local loop.
 *
 *  @note   When appkey is added, use this function to quickly bind
 *          all local model.
 *
 *  @param  net_idx The index of network key.
 *  @param  mod_app_idx The index of application key.
 *
 *  @return 0[success],[!0] Error code.
 */
int bt_mesh_local_model_bind_direct(u16_t net_idx, u16_t mod_app_idx);

#ifdef __cplusplus
}
#endif

#endif /*__LOCAL_OPERATION_H__*/
