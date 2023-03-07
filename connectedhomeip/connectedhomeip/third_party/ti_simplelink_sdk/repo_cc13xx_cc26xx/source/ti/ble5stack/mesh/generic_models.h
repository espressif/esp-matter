/******************************************************************************

@file  generic_models.h

@brief  Generic models defines and APIs

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

#ifndef APPLICATION_GENERIC_MODELS_H_
#define APPLICATION_GENERIC_MODELS_H_

/*********************************************************************
* INCLUDES
*/
#include <zephyr.h>
#include <sys/printk.h>
#include <settings/settings.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

/*********************************************************************
* CONSTANTS
*/

/* Generic OnOff state values */
#define STATE_OFF       0x00
#define STATE_ON        0x01

/* Generic Power OnOff state value */
#define STATE_DEFAULT   0x01
#define STATE_RESTORE   0x02

/********** Models Operation Codes **********/
/* Generic OnOff Message Opcodes */
#define BT_MESH_MODEL_OP_GEN_ONOFF_GET                BT_MESH_MODEL_OP_2(0x82, 0x01)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET                BT_MESH_MODEL_OP_2(0x82, 0x02)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK          BT_MESH_MODEL_OP_2(0x82, 0x03)
#define BT_MESH_MODEL_OP_GEN_ONOFF_STATUS             BT_MESH_MODEL_OP_2(0x82, 0x04)

/* Generic Level Message Opcodes */
#define BT_MESH_MODEL_OP_GEN_LEVEL_GET                BT_MESH_MODEL_OP_2(0x82, 0x05)
#define BT_MESH_MODEL_OP_GEN_LEVEL_SET                BT_MESH_MODEL_OP_2(0x82, 0x06)
#define BT_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK          BT_MESH_MODEL_OP_2(0x82, 0x07)
#define BT_MESH_MODEL_OP_GEN_LEVEL_STATUS             BT_MESH_MODEL_OP_2(0x82, 0x08)
#define BT_MESH_MODEL_OP_GEN_DELTA_SET                BT_MESH_MODEL_OP_2(0x82, 0x09)
#define BT_MESH_MODEL_OP_GEN_DELTA_SET_UNACK          BT_MESH_MODEL_OP_2(0x82, 0x0A)
#define BT_MESH_MODEL_OP_GEN_MOVE_SET                 BT_MESH_MODEL_OP_2(0x82, 0x0B)
#define BT_MESH_MODEL_OP_GEN_MOVE_SET_UNACK           BT_MESH_MODEL_OP_2(0x82, 0x0C)

/* Generic Default Transition Time Message Opcodes */
#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_GET          BT_MESH_MODEL_OP_2(0x82, 0x0D)
#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_SET          BT_MESH_MODEL_OP_2(0x82, 0x0E)
#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_SET_UNACK    BT_MESH_MODEL_OP_2(0x82, 0x0F)
#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_STATUS       BT_MESH_MODEL_OP_2(0x82, 0x10)

/* Generic Power OnOff Message Opcodes */
#define BT_MESH_MODEL_GEN_ONPOWERUP_GET               BT_MESH_MODEL_OP_2(0x82, 0x11)
#define BT_MESH_MODEL_GEN_ONPOWERUP_SET               BT_MESH_MODEL_OP_2(0x82, 0x13)
#define BT_MESH_MODEL_GEN_ONPOWERUP_SET_UNACK         BT_MESH_MODEL_OP_2(0x82, 0x14)
#define BT_MESH_MODEL_GEN_ONPOWERUP_STATUS            BT_MESH_MODEL_OP_2(0x82, 0x12)

/* Generic Battery Message Opcodes */
#define BLE_MESH_MODEL_OP_GEN_BATTERY_GET             BT_MESH_MODEL_OP_2(0x82, 0x23)
#define BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS          BT_MESH_MODEL_OP_2(0x82, 0x24)

/* Number of milliseconds per second */
#define MSEC_PER_SEC 1000U

#define UNKNOWN_LEVEL_VALUE 0x3F

/*
 * Transition types enumerated
 */
enum transition_types_e {
    MOVE = 0x01,
    NON_MOVE,
    ONOFF,
    LEVEL,
    DELTA_LEVEL,
    MOVE_STATE,
};

/*
 * State value properties structure
 */
struct state_properties_t {
    uint16_t current;
    uint16_t target;
    uint16_t range_min;
    uint16_t range_max;
    int delta;
};

/*
 * State transition structure
 */
struct transition_t {
    enum transition_types_e type;
    uint8_t trans_time;
    uint8_t remain_time;
    uint8_t delay;
    uint32_t quo_tt;
    uint32_t num_of_hops;
    uint32_t hop_duration;
    uint32_t total_duration;
    int64_t start_timestamp;
};

/*
 * State properties structure
 */
struct ctl_state_t {
    uint8_t default_trans_time;
    uint8_t last_tid;
    uint16_t last_src_addr;
    uint16_t last_dst_addr;
    int64_t last_msg_timestamp;
    struct transition_t *transition;
    struct state_properties_t *state_prop;
};

/*
 * GenOnOff structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint8_t onOff;
    uint8_t tid;
    uint8_t transitionTime;
    uint8_t delay;
} genOnOffSet_t;

/*
 * GenLevel structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t level;
    uint8_t  tid;
    uint8_t  transitionTime;
    uint8_t  delay;
} genLevelSet_t;

/*
 * DeltaLevel structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint32_t deltaLevel;
    uint8_t  tid;
    uint8_t  transitionTime;
    uint8_t  delay;
} genDeltaLevelSet_t;

/*
 * Move structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t deltaLevel;
    uint8_t  tid;
    uint8_t  transitionTime;
    uint8_t  delay;
} genMoveSet_t;

/*
 * Transition time structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint8_t  transTime;
} genTransTimeSet_t;

/*
 * Generic Battery Server State
 */
ZEPHYR__PACKED(struct) batteryStatus_t
{
    uint8_t battery_level;           /* Value of Generic Battery Level state             */
    uint32_t time_to_discharge : 24; /* Value of Generic Battery Time to Discharge state */
    uint32_t time_to_charge    : 24; /* Value of Generic Battery Time to Charge state    */
    uint8_t flags;                   /* Value of Generic Battery Flags state             */
};

/*
 * Generic Power OnOff Server state
 */
struct genOnPowerUpState_t{
    uint8_t onPowerUp;
};

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      MeshModels_init
 *
 * @brief   Initialize the supported models
 *
 * @param   None
 *
 * @return  0 upon complete initialization
 */
int MeshModels_init();

/*********************************************************************
 * @fn      gen_OnOff_get
 *
 * @brief   Reports the generic OnOff state of the element
 *
 * @param   model - Generic OnOff server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_OnOff_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_OnOff_set_unack
 *
 * @brief   Sets the Generic OnOff state of an element and publishes
 *          a status message when the transition to the new state has
 *          completed.
 *          Stores the new value of the OnOff state to NV
 *
 * @param   model - Generic OnOff server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_OnOff_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_OnOff_set
 *
 * @brief   Sets the Generic OnOff state of an element and publishes
 *          a status message when the transition to the new state has
 *          completed.
 *          Sends OnOff Status message as response to the OnOff Set
 *          message.
 *          Stores the new value of the OnOff state to NV
 *
 * @param   model - Generic OnOff server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_OnOff_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_OnOff_Status
 *
 * @brief   Displays the present generic OnOff state of the element.
 *          If the element is in a process of transition to a target
 *          OnOff state value, the target OnOff state that the element
 *          is to reach, and the time it will take the element to
 *          complete the transition to the target OnOff state will
 *          also be displayed.
 *
 * @param   model - Generic OnOff server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_OnOff_Status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Level_get
 *
 * @brief   Reports the generic Level state of the element
 *
 * @param   model - Generic Level server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Level_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Level_set_unack
 *
 * @brief   Sets the Generic Level state of an element and publishes
 *          a status message when the transition to the new state has
 *          completed.
 *
 * @param   model - Generic Level server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Level_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Level_set
 *
 * @brief   Sets the Generic Level state of an element and publishes
 *          a status message when the transition to the new state has
 *          completed.
 *          Sends Level Status message as response to the Level Set
 *          message.
 *
 * @param   model - Generic Level server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Level_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Delta_set_unack
 *
 * @brief   Sets the Generic Level state of an element by a relative
 *          (delta) value and publishes a status message when the
 *          transition to the new state has completed.
 *
 * @param   model - Generic Level server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Delta_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Delta_set
 *
 * @brief   Sets the Generic Level state of an element by a relative
 *          (delta) value and publishes a status message when the
 *          transition to the new state has completed.
 *          Sends Level Status message as response to the Delta Set
 *          message.
 *
 * @param   model - Generic Level server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Delta_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Move_set_unack
 *
 * @brief   Starts a process of changing the Generic Level state of
 *          an element with a transition speed that is calculated by
 *          dividing the Delta Level by the Transition Time.
 *          i.e, the Generic Level state will be changing by a value
 *          of the Delta Level in time of the Transition Time.
 *          When the transition to the new state has completed, a
 *          status message with the new Generic Level value will be
 *          published.
 *
 * @param   model - Generic Level server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Move_set_unack(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Move_set
 *
 * @brief   Starts a process of changing the Generic Level state of
 *          an element with a transition speed that is calculated by
 *          dividing the Delta Level by the Transition Time.
 *          i.e, the Generic Level state will be changing by a value
 *          of the Delta Level in time of the Transition Time.
 *          When the transition to the new state has completed, a
 *          status message with the new Generic Level value will be
 *          published.
 *          Sends Level Status message as response to the Move Set
 *          message.
 *
 * @param   model - Generic Level server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Move_set(struct bt_mesh_model *model,
             struct bt_mesh_msg_ctx *ctx,
             struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Level_Status
 *
 * @brief   Displays the present generic Level state of the element.
 *          If the element is in a process of transition to a target
 *          Level state value, the target Level state that the element
 *          is to reach, and the time it will take the element to
 *          complete the transition to the target Level state will
 *          also be displayed.
 *
 * @param   model - Generic Level server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Level_Status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Default_Transition_Time_get
 *
 * @brief   Reports the generic Default Transition Time state of the
 *          element
 *
 * @param   model - Generic Default Transition Time server model
 *                  instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Default_Transition_Time_get(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Default_Transition_Time_set_unack
 *
 * @brief   Sets the Generic Default Transition Time state of an
 *          element and publishes a status message with the new state
 *          value.
 *          Stores the new value of the Default Transition Time state
 *          to NV.
 *
 * @param   model - Generic Default Transition Time server model
 *                  instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Default_Transition_Time_set_unack(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Default_Transition_Time_set
 *
 * @brief   Sets the Generic Default Transition Time state of an
 *          element and publishes a status message with the new state
 *          value.
 *          Sends Default Transition Time Status message as response
 *          to the Default Transition Set message.
 *          Stores the new value of the Default Transition state to NV
 *
 * @param   model - Generic Default Transition server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Default_Transition_Time_set(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Default_Transition_Time_Status
 *
 * @brief   Displays the present generic Default Transition Time state
 *          of the element.
 *          If the element is in a process of transition to a target
 *          Default Transition Time state value, the target state that
 *          the element is to reach, and the time it will take the
 *          element to complete the transition to the target Default
 *          Transition Time state will also be displayed.
 *
 * @param   model - Generic Default Transition Time server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Default_Transition_Time_Status(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_OnPowerUp_get
 *
 * @brief   Reports the OnPowerUp state of the element
 *
 * @param   model - Generic Power OnOff server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_OnPowerUp_get(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_OnPowerUp_set_unack
 *
 * @brief   Sets the OnPowerUp state of an element.
 *          Stores the new OnPowerUp value to NV.
 *
 * @param   model - Generic Power OnOff server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_OnPowerUp_set_unack(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_OnPowerUp_set
 *
 * @brief   Sets the OnPowerUp state of an element and publishes
 *          a status message if the OnPowerUp value of the element has
 *          changed.
 *          Sends OnPowerUp Status message as response to the OnPowerUp
 *          Set message.
 *          Stores the new value of the OnPowerUp state to NV.
 *
 * @param   model - Generic Power OnOff server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_OnPowerUp_set(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_OnPowerUp_Status
 *
 * @brief   Displays the present OnPowerUp state of the element
 *
 * @param   model - Generic Power OnOff server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_OnPowerUp_Status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Battery_get
 *
 * @brief   Reports the generic Battery state of the element
 *
 * @param   model - Generic Battery server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Battery_get(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);

/*********************************************************************
 * @fn      gen_Battery_status
 *
 * @brief   Displays the generic Battery state properties of the
 *          element
 *
 * @param   model - Generic Battery server model instance
 * @param   ctx   - Message sending context
 * @param   buf   - Network buffer
 *
 * @return  None
 */
void gen_Battery_status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf);
#endif /* APPLICATION_SIG_MODELS_H_ */
