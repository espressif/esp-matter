/******************************************************************************

@file  static_prov.c

@brief This file contains static provisioning configurations and performance.

Group: WCS, BTS
Target Device: cc13xx_cc26xx

******************************************************************************

 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************


*****************************************************************************/

#ifdef USE_STATIC_PROVISIONING
/*********************************************************************
* INCLUDES
*/

#include "static_prov.h"
#include "mesh_erpc_wrapper.h"
#include "ti_device_composition.h"
#include <zephyr.h>
#include <sys/printk.h>
#include <settings/settings.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

/*********************************************************************
 * MACROS
 */
#define MOD_LF 0x0000
#define GROUP_ADDR 0xc000

/*********************************************************************
* CONSTANTS
*/

/*********************************************************************
* TYPEDEFS
*/

/*********************************************************************
* GLOBAL VARIABLES
*/

/*********************************************************************
* LOCAL VARIABLES
*/
static const uint16_t net_idx;
static const uint32_t iv_index;
static uint8_t prov_flags = 0;
static const uint8_t net_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};
static const uint8_t dev_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};
static const uint8_t app_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void MeshApp_provConfigure(uint16_t addr, uint16_t app_idx);

/*********************************************************************
 * EXTERN FUNCTIONS
*/

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      MeshApp_static_provisioning
 *
 * @brief   Perform Static Provisioning by calling bt_mesh_provision
 *          and MeshApp_provConfigure
 *
 * @param   addr
 * @param   app_idx
 */
int MeshApp_static_provisioning(uint16_t addr, uint16_t app_idx)
{
    int status;
    status = bt_mesh_provision(net_key, net_idx, prov_flags, iv_index, addr, dev_key);
    if (status)
    {
        return status;
    }

    MeshApp_provConfigure(addr, app_idx);

  return status;
}

/*********************************************************************
 * @fn      MeshApp_provConfigure
 *
 * @brief   Configure the Mesh Node
 *
 * @param   addr
 * @param   app_idx
 */
static void MeshApp_provConfigure(uint16_t addr, uint16_t app_idx)
{
  /* Add Application Key */
  bt_mesh_cfg_app_key_add_wrapper(net_idx, addr, net_idx, app_idx, app_key);

  /* Bind to vendor model */
  bt_mesh_cfg_mod_app_bind_vnd_wrapper(net_idx, addr, addr, app_idx,
                               MOD_LF, CONFIG_BT_COMPANY_ID);

  /* Bind to Health model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_HEALTH_SRV);

  /* Bind to Generic OnOff server model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_ONOFF_SRV);

  /* When using static provisioning, the loopback buffer is used
   * when the device sends messages.
   * The amount of the loopback buffers is determined by
   * CONFIG_BT_MESH_LOOPBACK_BUFS which is defined in autoconf.h.
   * Sleep is needed for the loopback buffers to be freed.
   */
  k_sleep(K_MSEC(100));

  /* Bind to Generic OnOff client model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_ONOFF_CLI);
  /* Bind to Generic Level server model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_LEVEL_SRV);
  /* Bind to Generic Level client model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_LEVEL_CLI);

  k_sleep(K_MSEC(100));

  /* Bind to Generic Transition Time server model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV);
  /* Bind to Generic Transition Time client model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI);
  /* Bind to Generic Power OnOff server model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV);

  k_sleep(K_MSEC(100));

  /* Bind to Generic Power OnOff client model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI);
  /* Bind to Generic Power OnOff Setup server model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV);
  /* Bind to Generic Battery server model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_BATTERY_SRV);

  k_sleep(K_MSEC(100));

  /* Bind to Generic Battery client model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_GEN_BATTERY_CLI);
  /* Bind to Sensor server model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_SENSOR_SRV);
  /* Bind to Sensor client model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_SENSOR_CLI);

  k_sleep(K_MSEC(100));

  /* Bind to Sensor setup server model */
  bt_mesh_cfg_mod_app_bind_wrapper(net_idx, addr, addr, app_idx,
                           BT_MESH_MODEL_ID_SENSOR_SETUP_SRV);

  /* Add model subscription */
  bt_mesh_cfg_mod_sub_add_vnd_wrapper(net_idx, addr, addr, GROUP_ADDR,
                              MOD_LF, CONFIG_BT_COMPANY_ID);
}

#endif // USE_STATIC_PROVISIONING
