/******************************************************************************

 @file dmm_policy.h

 @brief dmm policy Header

 Group: WCS LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2019-2021, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
/*!****************************************************************************
 *  @file  dmm_policy.h
 *
 *  @brief      Dual Mode Policy Manager
 *
 *  The DMMPolicy interface provides a service to for the stack applications to
 *  update the priority of the stack activities, which is then used to make scheduling decisions.
 *
 *  # Fields in the Policy Table #
 *  .state: the name of the application state
 *  .weight: amount of adjusted priority for stack activities specified in .appliedActivity
 *  .timingConstraint: reserved for future usage
 *  .pause: whether or not the specified stack's application is paused during the state
 *  .appliedActivity: Specified the activities of which priority is/are adjusted by .weight
 *    - Final Priority = GPT (Stack level priority) +  weight (Application Level)
 *  .balancedMode: Switch the .weight value between the two stacks based on the time information specified
 *
 *  # Usage #
 *
 *  To use the DMMPolicy module to set the scheduling policy, the application
 *  calls the following APIs:
 *    - DMMPolicy_init(): Initialize the DMMPolicy module/task.
 *    - DMMPolicy_Params_init():  Initialize a DMMPolicy_Params structure
 *      with default values.  Then change the parameters from non-default
 *      values as needed.
 *    - DMMPolicy_open():  Open an instance of the DMMPolicy module,
 *      passing the initialized parameters.
 *    - Stack A/B application - DMMPolicy_updateApplicationState: Update the application state.
 *                              The Policy Manager finds the matching policy that is used
 *                              when scheduling RF commands from stack A and B.
 *
 *   An example of a policy table (define in ti_dmm_application_policy.c, which is generated by SysConfig)
 *
 *   \code
 * DMMPolicy_Policy DMMPolicy_ApplicationPolicy[] = {
 *   // DMM Policy 0
 *   {
 *       // BLE Peripheral Policy
 *       .appState[BLE_STACK_POLICY_IDX] = {
 *           .state = DMMPOLICY_BLE_OAD,
 *           .weight = 25,
 *           .timingConstraint = DMMPOLICY_TIME_RESERVED,
 *           .pause = DMMPOLICY_NOT_PAUSED,
 *           .appliedActivity =  DMMPOLICY_APPLIED_ACTIVITY_BLE_CONNECTION,
 *            (note: the priority of BLE connection event is increased by .weight (25) during BLE_OAD)
 *       },
 *       // 15.4 Collector Policy
 *       .appState[TI154_STACK_POLICY_IDX] = {
 *           .state = ANY,
 *            .weight = 0,
 *            .timingConstraint = DMMPOLICY_TIME_RESERVED,
 *            .pause = DMMPOLICY_PAUSED,
 *            .appliedActivity =  DMMPOLICY_APPLIED_ACTIVITY_NONE,
 *        },
 *        //Balanced Mode Policy
 *        .balancedMode = DMMPOLICY_BALANCED_NONE,
 *    },
 *    // DMM Policy 1
 *    {
 *        // BLE Peripheral Policy
 *        .appState[BLE_STACK_POLICY_IDX] = {
 *            .state = DMMPOLICY_BLE_HIGH_BANDWIDTH,
 *            .weight = 25,
 *            .timingConstraint = DMMPOLICY_TIME_RESERVED,
 *            .pause = DMMPOLICY_NOT_PAUSED,
 *            .appliedActivity =  DMMPOLICY_APPLIED_ACTIVITY_BLE_CONNECTION,
 *             (note: the priority of BLE connection event is increased by .weight (25) during BLE_HIGH_BANDWIDTH)
 *        },
 *        // 15.4 Collector Policy
 *        .appState[TI154_STACK_POLICY_IDX] = {
 *            .state = ANY,
 *            .weight = 0,
 *            .timingConstraint = DMMPOLICY_TIME_RESERVED,
 *            .pause = DMMPOLICY_NOT_PAUSED,
 *            .appliedActivity =  DMMPOLICY_APPLIED_ACTIVITY_NONE,
 *        },
 *        //Balanced Mode Policy
 *        .balancedMode = DMMPOLICY_BALANCED_NONE,
 *    },
 *    // DMM Policy 2 (The last policy indicates the default priority of the two stacks)
 *    {
 *        // BLE Peripheral Policy
 *        .appState[BLE_STACK_POLICY_IDX] = {
 *            .state = ANY,
 *            .weight = 0,
 *            .timingConstraint = DMMPOLICY_TIME_RESERVED,
 *            .pause = DMMPOLICY_NOT_PAUSED,
 *            .appliedActivity =  DMMPOLICY_APPLIED_ACTIVITY_NONE,
 *        },
 *        // 15.4 Collector Policy
 *        .appState[TI154_STACK_POLICY_IDX] = {
 *            .state = ANY,
 *            .weight = 1,
 *            .timingConstraint = DMMPOLICY_TIME_RESERVED,
 *            .pause = DMMPOLICY_NOT_PAUSED,
 *            .appliedActivity =  DMMPOLICY_APPLIED_ACTIVITY_NONE,
 *        },
 *        //Balanced Mode Policy
 *        .balancedMode = DMMPOLICY_BALANCED_NONE,
 *    },
 *};
 *
 *DMMPolicy_PolicyTable DMMPolicy_ApplicationPolicyTable = {
 *    //Stack Roles
 *    .stackRole[BLE_STACK_POLICY_IDX] = DMMPolicy_StackRole_BlePeripheral,
 *    .stackRole[TI154_STACK_POLICY_IDX] = DMMPolicy_StackRole_154Sensor,
 *    //Policy table
 *    .policy = DMMPolicy_ApplicationPolicy,
 *    // Index Table for future use
 *    .indexTable = NULL,
 *};
 *
 * //! \brief The application policy table size
 * uint32_t DMMPolicy_ApplicationPolicySize = (sizeof(DMMPolicy_ApplicationPolicy) / sizeof(DMMPolicy_Policy));
 *
 *   \endcode
 *
 *********************************************************************************/

#ifndef DMMPolicy_H_
#define DMMPolicy_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include <ti/drivers/rf/RF.h>


//! \brief BLE stack is always the first policy
#define BLE_STACK_POLICY_IDX       0    ///< BLE stack is always the first policy

/**
 *  @name DMM Policy
 *  @anchor DMMPOLICY
 *
 *  Reserved values to define the DMM policy attributes and values
 *  @{
 */
#define DMMPOLICY_NUM_STACKS    2       ///< Number of RF driver clients supported

#define DMMPOLICY_PRIORITY_LOW     0    ///< Low priority used when using application states instead of GPT
#define DMMPOLICY_PRIORITY_HIGH    1    ///< High priority used when using application states instead of GPT

#define DMMPOLICY_TIME_NONE_CRITICAL    0   ///< Stack commands can be delayed when using application states instead of GPT
#define DMMPOLICY_TIME_CRITICAL         1   ///< Stack commands cannot be delayed when using application states instead of GPT
#define DMMPOLICY_TIME_RESERVED         1   ///< Stack commands allow delay settings are not overwritten.

#define DMMPOLICY_NOT_PAUSED            0           ///< Stack pause callback is not called for application states including this
#define DMMPOLICY_PAUSED                0x0001      ///< Stack pause callback is called for application states including this
#define DMMPOLICY_SCHEDULE_BLOCKED      0x0002      ///< reserve for future usage

#define DMMPOLICY_BALANCED_NONE             0       ///< Does not use balanced mode

#define DMMPOLICY_BALANCED_TIME_BM_1               0x80000000                                                                           ///< Balanced mode is time (MSB = 1)
#define DMMPOLICY_BALANCED_TIME_MODE_1(onMin, offMax)     (DMMPOLICY_BALANCED_TIME_BM_1 | (onMin & 0xFFF) | ((offMax & 0xFFF) << 12))   ///< macro for defining balanced mode on/off time
#define DMMPOLICY_BALANCED_TIME_MODE_1_ON_MIN(RatioTime)     (RatioTime & 0xFFF)                                                        ///< macro for defining balanced mode on time
#define DMMPOLICY_BALANCED_TIME_MODE_1_OFF_MAX(RatioTime)     ((RatioTime & 0xFFF000) >> 12)                                            ///< macro for defining balanced mode off time

#define DMMPOLICY_APPLIED_ACTIVITY_NONE                     0           ///< Activity state where there is no activity
#define DMMPOLICY_APPLIED_ACTIVITY_ALL                      0xFFFF      ///< Activity state to define any activity
#define DMMPOLICY_APPLIED_ACTIVITY_BLE_CONNECTION           0x0001      ///< Activity state where BLE Connection is ongoing
#define DMMPOLICY_APPLIED_ACTIVITY_BLE_LINK_EST             0x0002      ///< Activity state where BLE link is being established
#define DMMPOLICY_APPLIED_ACTIVITY_BLE_BROADCASTING         0x0004      ///< Activity state where BLE is broadcasting
#define DMMPOLICY_APPLIED_ACTIVITY_BLE_OBSERVING            0x0008      ///< Activity state where BLE is observing

#define DMMPOLICY_APPLIED_ACTIVITY_THREAD_TX_DATA           0x0001      ///< Activity state where Thread data is being sent or received (Direct/Indirect)
#define DMMPOLICY_APPLIED_ACTIVITY_THREAD_TX_POLL           0x0002      ///< Activity state where Thread is transmitting polled data
#define DMMPOLICY_APPLIED_ACTIVITY_THREAD_RX_SCAN           0x0004      ///< Activity state where Thread is scanning (Energy Detect/Active)
#define DMMPOLICY_APPLIED_ACTIVITY_THREAD_RX_POLL           0x0008      ///< Activity state where Thread is receiving polled data
#define DMMPOLICY_APPLIED_ACTIVITY_THREAD_RX_IDLE           0x0010      ///< Activity state where Thread radio is Idle

#define DMMPOLICY_APPLIED_ACTIVITY_154_DATA                 0x0001      ///< Activity state where 15.4 data is being sent or received
#define DMMPOLICY_APPLIED_ACTIVITY_154_LINK_EST             0x0002      ///< Activity state where 15.4 link is being established
#define DMMPOLICY_APPLIED_ACTIVITY_154_TX_BEACON            0x0004      ///< Activity state where 15.4 is transmitting a beacon
#define DMMPOLICY_APPLIED_ACTIVITY_154_RX_BEACON            0x0008      ///< Activity state where 15.4 is receiving a beacon
#define DMMPOLICY_APPLIED_ACTIVITY_154_FH                   0x0010      ///< Activity state where 15.4 is ongoing frequency hopping
#define DMMPOLICY_APPLIED_ACTIVITY_154_SCAN                 0x0020      ///< Activity state where 15.4 scanning
#define DMMPOLICY_APPLIED_ACTIVITY_154_RXON                 0x0040      ///< Activity state where 15.4 is receiving

#define DMMPOLICY_APPLIED_ACTIVITY_WSN_RETRANSMIT           0x0001      ///< Activity state where WSN is retransmitting
#define DMMPOLICY_APPLIED_ACTIVITY_WSN_TRANSMIT             0x0002      ///< Activity state where WSN is transmitting
#define DMMPOLICY_APPLIED_ACTIVITY_WSN_RECEIVE              0x0004      ///< Activity state where WSN is receiving

/** @} */

//! \brief The number of priority for stack activities
#define PRIORITY_NUM                              3         ///< The number of priority for stack activities

//! \brief To maintain compatibility, define old DMMPolicy_updateStackState function as new DMMPolicy_updateApplicationState function.
#define DMMPolicy_updateStackState DMMPolicy_updateApplicationState

/**
 *  @name DMM Priority
 *  @anchor DMM_PRIORITY
 *
 *  Define the limit attributes of DMM Priority changing
 *  @{
 */
#define DMM_PRIORITY_MAX_LIMIT                    250       ///< Maximum stack priority
#define DMM_PRIORITY_MAX_CHANGE_LIMIT             185       ///< Maximum change limit of a stack priority
/** @}*/

//! \brief global priority table
#define DMM_GLOBAL_PRIORITY(activity, level, weight) {(activity << 16 | level), weight} ///< macro for getting the global priority

//! \brief the stack roles supported
typedef enum
{
    DMMPolicy_StackRole_invalid = 0,            ///< invalid stack role
    DMMPolicy_StackRole_BlePeripheral,          ///< stack role for a BLE Simple Peripheral
    DMMPolicy_StackRole_WsnNode,                ///< stack role for an EasyLink Wireless Sensor Network Node
    DMMPolicy_StackRole_threadMtd,              ///< stack role reserved for a customers proprietary stack 
    DMMPolicy_StackRole_threadFtd,              ///< stack role reserved for a customers proprietary stack 
    DMMPolicy_StackRole_154Sensor,              ///< stack role for a 15.4 Sensor
    DMMPolicy_StackRole_154Collector,           ///< stack role for a 15.4 Collector
    DMMPolicy_StackRole_ZigbeeEndDevice,        ///< stack role for a Zigbee End Device
    DMMPolicy_StackRole_ZigbeeRouter,           ///< stack role for a Zigbee Router
    DMMPolicy_StackRole_ZigbeeCoordinator,      ///< stack role for a Zigbee Coordinator
    DMMPolicy_StackRole_custom1,                ///< stack role reserved for a customers proprietary stack
    DMMPolicy_StackRole_custom2,                ///< stack role reserved for a customers proprietary stack
} DMMPolicy_StackRole;

//! \brief Structure used to define a DMM Policy
typedef struct
{
    uint32_t     state;                         ///< application state of a policy
    uint8_t      weight;                        ///< amount of adjusted priority for stack activities specified in .appliedActivity
    uint16_t     timingConstraint;              ///< reserved for future usage
    uint32_t     appliedActivity;               ///< Specified the activities of which priority is/are adjusted by .weight
                                                ///< final Priority = GPT (Stack level priority) +  weight (Application Level)
    uint16_t     pause;                         ///< whether or not the specified stack's application is paused during the state:0 not paused, 0x0001 paused
} DMMPolicy_State;

//! \brief Structure used to decide the policy for a particular stack state
typedef struct
{
    DMMPolicy_State appState[DMMPOLICY_NUM_STACKS];     ///< structure used to define a DMM policy
    uint32_t        balancedMode;                       ///< 0x0 = no ratio mode, 0x0000xxyy = stack 1:stack 2 = xx:yy, 0x80xxxyyy = Hi Pri Stack xxx ms min on yyy ms max off.
} DMMPolicy_Policy;

//! \brief Structure used to define a stack command index table. Reserved for future usage.
typedef struct
{
    uint8_t *CmdIndex;          ///< Command table
    uint8_t tableSize;          ///< Size of the table
}DMMPolicy_StackCmdIndexTable;

//! \brief policy table entry
typedef struct
{
    DMMPolicy_StackRole stackRole[DMMPOLICY_NUM_STACKS];    ///< policy roles for policy entry
    DMMPolicy_Policy* policy;                               ///< pointer to the policy
    DMMPolicy_StackCmdIndexTable *indexTable;               ///< reserved for future use
} DMMPolicy_PolicyTable;

//! \brief Stack Activity data struct
typedef struct {
    uint32_t activity;          ///< stack command activity
    uint16_t globalPriority;    ///< stack command priority
} StackActivity;

//! \brief Stack Activity Priority
typedef enum
{
    DMM_StackPNormal =0,        ///< Normal priority
    DMM_StackPHigh,             ///< High priority
    DMM_StackPUrgent,           ///< Urgent priority
} PriorityDef;

//! \brief Global Priority Table data struct
typedef struct {
    StackActivity *globalTableArray;    ///< global table array
    uint8_t tableSize;                  ///< size of the table
    DMMPolicy_StackRole stackRole;      ///< stack application role
} GlobalTable;

//! \brief Policy Information dynamically updated
typedef struct {
    uint32_t stackID;           ///< ID to define the stack using current policy
    uint8_t currentWeight;      ///< The current weight value of the policy
    uint8_t defaultPriority;    ///< The default priority of the policy
} DMMPolicy_CurrentInfo;

/** @brief RF parameter struct
 *  DMM Scheduler parameters are used with the DMMPolicy_open() and DMMPolicy_Params_init() call.
 */
typedef struct {
    DMMPolicy_PolicyTable policyTable;                                  ///< policy table to be used for the DMM use case
    uint32_t numPolicyTableEntries;                                     ///< entries in policy table
    GlobalTable *globalPriorityTable;                                   ///< global priority table to be used for the DMM use case
    DMMPolicy_CurrentInfo  policyCurrentInfo[DMMPOLICY_NUM_STACKS];     ///< policy information
} DMMPolicy_Params;

/** @brief Status codes for various DMM Policy functions.
 *
 *  RF_Stat is reported as return value for DMM Policy functions.
 */
typedef enum {
    DMMPolicy_StatusError,          ///< Error
    DMMPolicy_StatusNoPolicyError,  ///< Error with policy table
    DMMPolicy_StatusParamError,     ///< Parameter Error
    DMMPolicy_StatusSuccess         ///< Function finished with success
} DMMPolicy_Status;

//! \brief Callback function type for app pause/reseume
typedef void (*DMMPolicy_appPauseCb_t)(uint16_t pause);

//! \brief Structure for app callbacks
typedef struct
{
    DMMPolicy_appPauseCb_t appPauseCb;      ///< Callback function when app in paused state
} DMMPolicy_AppCbs_t;

/** @brief  Function to initialize the DMMPolicy_Params struct to its defaults
 *
 *  @param  params      An pointer to RF_Params structure for
 *                      initialization
 */
extern void DMMPolicy_Params_init(DMMPolicy_Params *params);

/** @brief   Register the application policy callbacks
 *
 *  @param  AppCbs      application callback take
 *  @param  StackRole   application stack role
 */
extern void DMMPolicy_registerAppCbs(DMMPolicy_AppCbs_t AppCbs, DMMPolicy_StackRole StackRole);

/** @brief  Function that initializes the DMMPolicy module
 *
 */
extern void DMMPolicy_init(void);

/** @brief  Function to open the DMMPolicy module
 *
 *  @param  params      An pointer to RF_Params structure for initialization
 *
 *  @return DMMPolicy_Stat status
 */
extern DMMPolicy_Status DMMPolicy_open(DMMPolicy_Params *params);

/** @brief  Updates the policy used to make scheduling decisions
 *
 *  @param  StackRole     The stack role that has changed state
 *  @param  newState      The state the stack has changed to
 *
 *  @return DMMPolicy_Stat status
 */
extern DMMPolicy_Status DMMPolicy_updateApplicationState(DMMPolicy_StackRole StackRole, uint32_t newState);

/** @brief  Get the global activity based on stack activity
 *
 *  @param activity     Stack Activity
 *  @param stackID      Stack ID
 *
 *  @return Global Priority Value
 */
extern uint16_t DMMPolicy_getGlobalPriority (uint32_t activity, uint32_t stackID);

/** @brief Get the default priority
 *
 *  @param stackID      Stack ID (uint32_t)
 *
 *  @return default priority value
 */
extern uint8_t DMMPolicy_getDefaultPriority (uint32_t stackID);

/** @brief Get the global activity based on stack activity
 *
 *  @param stackID      Stack ID
 *  @param StackRole    DMMPolicy_StackRole
 *
 *  @return Global Priority Value
 */
extern void DMMPolicy_setStackID (uint32_t stackID, DMMPolicy_StackRole StackRole);

/** @brief Get the pause value from the current policy
 *
 *  @param stackID       Stack ID
 *
 *  @return pause Value
 */
extern uint16_t DMMPolicy_getPauseValue (uint32_t stackID);

/** @brief Get the time constraint value from the current policy
 *
 *  @param stackID       Stack ID
 *
 *  @return TimeConstraint
 */
extern uint16_t DMMPolicy_getTimeConstraintValue (uint32_t stackID);

/** @brief  check if the global priority table is available
 *
 *  @return bool true => GPT is available, false => GPT is not available (legacy policy table mode)
 */
extern bool DMMPolicy_getGPTStatus (void);

/** @brief  Turn on Block mode
 *
 *  @param  StackRole   stack role associated with Task handle
 *  @return true: success, false: the stack role cannot be found
 */
bool DMMPolicy_setBlockModeOn(DMMPolicy_StackRole StackRole);

/** @brief  Turn off Block mode
 *
 *  @param  StackRole   stack role associated with Task handle
 *  @return true: success, false: the stack role cannot be found
 */
bool DMMPolicy_setBlockModeOff(DMMPolicy_StackRole StackRole);

/** @brief  Get Block mode status
 *
 *  @param  StackRole   stack role associated with Task handle
 *
 *  return  True: Block Mode is On, False: Block Mode is Off
 */
bool DMMPolicy_getBlockModeStatus(DMMPolicy_StackRole StackRole);

#ifdef __cplusplus
}
#endif

#endif /* DMMPolicy_H_ */
