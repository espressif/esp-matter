/******************************************************************************

@file  simple_mesh_node.h

@brief This file contains the mesh and peripheral sample application for use
with the CC2652 Bluetooth Low Energy Protocol Stack.

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

#ifndef SIMPLE_MESH_NODE_H_
#define SIMPLE_MESH_NODE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <xdc/std.h>


/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

// Maximum number of scan responses.
// Note: this value cannot be greater than the number of items reserved in
// mrMenuConnect (See multi_role_menu.c)
// This cannot exceed 27 (two-button menu's constraint)
#define DEFAULT_MAX_SCAN_RES                  15

// sample application support only element 0
#define SAMPLE_APP_MODELS_ELEMENT   0

#define UUID_MAX_LEN 16

#define NUM_OF_UUID_ADDRESSES 10


/*********************************************************************
 * FUNCTIONS
 */

extern struct bt_mesh_prov prov;
extern int k_sys_work_q_init(void *dev);
void simple_mesh_and_peripheral_createTask(void);
void bleApp_createTasks(void);

// CallBacks
void MeshApp_scanCB(uint32_t evt, void* pMsg, uintptr_t arg);

// Menu
int MeshApp_button_pressed(uint8_t onoff_state);
int MeshApp_get_status_pressed();
int MeshApp_send_large_msg(uint16 msg_size);
int MeshApp_notify_status_pressed(uint8_t led_status);
int MeshApp_genModels_onOff_pressed(uint8_t action);
int MeshApp_genModels_level_pressed(uint8_t action);
int MeshApp_genModels_transTime_pressed(uint8_t action);
int MeshApp_genModels_powerOnOff_pressed(uint8_t action, uint8_t value);
int MeshApp_genModels_battery_pressed(uint8_t action);
int MeshApp_sensorModelsClient_pressed(uint8_t action, uint8_t value);
int MeshApp_sensorModelsServer_pressed(uint8_t action, uint8_t value);
int MeshApp_init(uint16_t addr, uint8_t prov_type);
const uint8_t * MeshApp_getUuid();
void MeshApp_setUuid(uint8_t index);
void MeshApp_saveDefaultUuid();

enum
{
    UNPROVISIONED_BEACON = 0,
    LOAD_FROM_NV = 1
};


#ifdef ADD_SIMPLE_PERIPHERAL
/*
 * Functions for menu action
 */
void SimplePeripheral_setAutoConnect(uint8_t newAutoConnect);
void SimplePeripheral_setConnIndex(uint8_t index);
bool SimplePeripheral_setConnPhy(uint8 index);
uint16_t SimplePeripheral_getConnHandle(uint8_t index);

// Auto connect availble groups
enum
{
  AUTOCONNECT_DISABLE = 0,              // Disable
  AUTOCONNECT_GROUP_A = 1,              // Group A
  AUTOCONNECT_GROUP_B = 2               // Group B
};
#endif

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_MESH_NODE_H_ */
