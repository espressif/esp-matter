/******************************************************************************

 @file  ll_scheduler.h

 @brief This file contains the types, constants, API's etc. for the
        Link Layer (LL) task scheduler.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

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

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR.
 *
 */

#ifndef LL_SCHEDULER_H
#define LL_SCHEDULER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * CONSTANTS
 */
// Scheduler Constants
#define LL_SCHED_START_IMMED        0
#define LL_SCHED_START_EVENT        1
#define LL_SCHED_START_PRIMARY      2
//
#define LL_SCHED_START_IMMED_PAD    (3 *  RAT_TICKS_IN_625US)
#define LL_SCHED_PRE_CUTOFF         (10 * RAT_TICKS_IN_625US)
#define LL_SCHED_POST_CUTOFF        (4 * RAT_TICKS_IN_625US)
#define LL_SCHED_OVERHEAD           (LL_SCHED_PRE_CUTOFF + LL_SCHED_POST_CUTOFF)

// Task ID
#define LL_TASK_ID_ADVERTISER                    0x01
#define LL_TASK_ID_SCANNER                       0x02
#define LL_TASK_ID_INITIATOR                     0x04
#define LL_TASK_ID_PERIODIC_ADVERTISER           0x08
#define LL_TASK_ID_PERIODIC_SCANNER              0x10
#define LL_TASK_ID_SLAVE                         0x40
#define LL_TASK_ID_MASTER                        0x80
#define LL_TASK_ID_NONE                          0xFF

// Task ID Masks
#define LL_TASK_ID_SECONDARY_TASKS               (LL_TASK_ID_ADVERTISER      | \
                                                  LL_TASK_ID_SCANNER         | \
                                                  LL_TASK_ID_INITIATOR       | \
                                                  LL_TASK_ID_PERIODIC_ADVERTISER | \
                                                  LL_TASK_ID_PERIODIC_SCANNER)

// Task State
#define LL_TASK_STATE_INACTIVE                   0
#define LL_TASK_STATE_ACTIVE                     1
#define LL_TASK_STATE_UNDEFINED                  0xFF

// Save State Data
#define ACCESS_ADDR_LEN                          4
#define CRC_INIT_LEN                             3

// The number of slots (i.e. 625us timer ticks) is the amount of time
// each master has to execute including post-processing and scheduling.
// It determines the relative offset of each additional connection
// interval, and thus limits the number of allowed concurrent connections.
// For example, if the value is four, then three connections can fit
// without affecting each other or the minimum connection interval
// (i.e. 7.5ms). But if the value is 12, then the minimum connection
// interval for three connections is 22.5ms. For five is 37.5ms. Etc.
#define NUM_SLOTS_PER_MASTER                     8

// The number of task blocks needed depends on the build configuration. The
// Non-connectable Adv and Scan would each require a task block if supported.
// If a Slave connection is supported, then a task is needed for the connectable
// Adv. This task is then reused when a Slave connection is formed. If a Master
// connection is supported, then a task is needed for Init. This task is then
// reused when a Master connection is formed. Thus, the total number of needed
// tasks is at most two plus the number of connections.
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_NCONN_CFG)
#define NUM_TASK_BLOCKS_ADV_NCONN_CFG            2 //Adv + Periodic Adv
#else
#define NUM_TASK_BLOCKS_ADV_NCONN_CFG            0
#endif // CTRL_CONFIG=ADV_NCONN_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
#define NUM_TASK_BLOCKS_SCAN_CFG                 2 //scanner + periodic scanner
#else
#define NUM_TASK_BLOCKS_SCAN_CFG                 0
#endif // CTRL_CONFIG=SCAN_CFG

// collect the total number of non-connection related task blocks based config
// Note: The number of connections is defined in the llConfigTable.
#define LL_NUM_TASK_BLOCKS                    (NUM_TASK_BLOCKS_ADV_NCONN_CFG + \
                                               NUM_TASK_BLOCKS_SCAN_CFG + 1)

/*******************************************************************************
 * TYPEDEFS
 */

// Task Information Data Type
// Note: Needed for forward reference to taskInfo_t from taskInfo_t.
typedef struct taskInfo_t taskInfo_t;

// Task Information
struct taskInfo_t
{
  uint8  taskID;                 // type of LL task
  uint8  taskState;              // whether task is active or inactive
  uint32 command;                // address to radio operation
  uint32 startTime;              // initial RF command start time; used by PM
  uint32 anchorPoint;            // anchor point
  uint32 lastStartTime;          // previous start time
  void   (*setup)(taskInfo_t *); // function used for specific setup operations
  uint32 rfEvents;               // RF events
};

// Task List
typedef struct
{
  uint8       numTasks;          // number of active tasks
  uint8       activeTasks;       // bits to indicate which tasks are active
  uint8       lastSecTask;       // the last secondary task; used for combo states
  uint8       lastActiveTasks;   // the active tasks checked last time
  taskInfo_t *curTask;           // currently active task
  taskInfo_t *llTasks;           // array of tasks
} taskList_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * Functions
 */

extern taskList_t  llTaskList;
//
extern void        llSchedulerInit( void );
extern void        llScheduler( void );
extern void        llScheduler_hook( void );
extern void        llScheduler_no_INIT_CFG_no_SCAN_CFG( void );
extern void        llScheduler_no_INIT_CFG( void );
extern void        llScheduleTask( taskInfo_t *llTask );
extern uint8       llFindStartType( taskInfo_t *secTask, taskInfo_t *primTask );
extern uint8       llFindStartTypeNoQos( taskInfo_t *secTask, taskInfo_t *primTask );
extern taskInfo_t *llFindNextSecTask( uint8 secTaskID );
extern taskInfo_t *llAllocTask( uint8 llTaskID );
extern void        llFreeTask( taskInfo_t **llTask );
extern taskInfo_t *llGetCurrentTask( void );
extern taskInfo_t *llGetTask( uint8 llTaskID );
extern uint8       llGetTaskState( uint8 llTaskID );
extern uint8       llActiveTask( uint8 llTaskID );
extern uint8       llGetActiveTasks( void );
extern uint8       llGetNumTasks( void );
extern void        llSetupRatCompare( taskInfo_t *llTask );
extern void        llClearRatCompare( void );

//
extern void        llSetupAdv( void );
extern void        llSetupDirectedAdvEvt( void );
extern void        llSetupUndirectedAdvEvt( void );
extern void        llSetupNonConnectableAdvEvt( void );
extern void        llSetupDiscoverableAdvEvt( void );
extern void        llSetupScan( void );
extern void        llSetupInit( uint8 );
extern void        llSetupConn( uint8 );
//
extern void        llExtAdvSchedSetup( taskInfo_t *llTask );
extern void        llExtScanSchedSetup( taskInfo_t *llTask );
extern void        llExtInitSchedSetup( taskInfo_t *llTask );
extern void        llLinkSchedSetup( taskInfo_t *llTask );
extern void        llPeriodicScanSchedSetup( taskInfo_t *llTask );
extern void        llPeriodicAdvSchedSetup( taskInfo_t *llTask );

#ifdef __cplusplus
}
#endif

#endif /* LL_SCHEDULER_H */
