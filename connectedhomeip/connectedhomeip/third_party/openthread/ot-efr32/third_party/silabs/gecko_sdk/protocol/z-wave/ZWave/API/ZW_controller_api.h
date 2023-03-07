/**
 * @file
 * Z-Wave Controller node application interface
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef _ZW_CONTROLLER_API_H_
#define _ZW_CONTROLLER_API_H_

#ifdef ZW_CONTROLLER

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
/* These are a part of the standard controller API */
#include <ZW_basis_api.h>
#include <NodeMask.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/* Mode parameters to ZW_AddNodeToNetwork */
#define ADD_NODE_ANY          1
#define ADD_NODE_CONTROLLER   2
#define ADD_NODE_SLAVE        3
#define ADD_NODE_EXISTING     4
#define ADD_NODE_STOP         5
#define ADD_NODE_STOP_FAILED  6
#define ADD_NODE_RESERVED     7
#define ADD_NODE_HOME_ID      8
#define ADD_NODE_SMART_START  9

#define ADD_NODE_MODE_MASK                   0x0F
#define ADD_NODE_OPTION_NORMAL_POWER         0x80
#define ADD_NODE_OPTION_NETWORK_WIDE         0x40
#define ADD_NODE_OPTION_LR                   0x20

/* Callback states from ZW_AddNodeToNetwork */
#define ADD_NODE_STATUS_LEARN_READY          1
#define ADD_NODE_STATUS_NODE_FOUND           2
#define ADD_NODE_STATUS_ADDING_SLAVE         3
#define ADD_NODE_STATUS_ADDING_CONTROLLER    4
#define ADD_NODE_STATUS_PROTOCOL_DONE        5
#define ADD_NODE_STATUS_DONE                 6
#define ADD_NODE_STATUS_FAILED               7
#define ADD_NODE_STATUS_NOT_PRIMARY          0x23

/* Mode parameters to ZW_RemoveNodeFromNetwork/ZW_RemoveNodeIDFromNetwork */
#define REMOVE_NODE_ANY                     ADD_NODE_ANY
#define REMOVE_NODE_CONTROLLER              ADD_NODE_CONTROLLER
#define REMOVE_NODE_SLAVE                   ADD_NODE_SLAVE
#define REMOVE_NODE_STOP                    ADD_NODE_STOP

#define REMOVE_NODE_MODE_MASK               ADD_NODE_MODE_MASK
#define REMOVE_NODE_OPTION_NORMAL_POWER     ADD_NODE_OPTION_NORMAL_POWER
#define REMOVE_NODE_OPTION_NETWORK_WIDE     ADD_NODE_OPTION_NETWORK_WIDE

/* Node paramter to ZW_RemoveNodeIDFromNetwork */
#define REMOVE_NODE_ID_ANY                  0

/* Callback states from ZW_RemoveNodeFromNetwork/ZW_RemoveNodeIDFromNetwork */
#define REMOVE_NODE_STATUS_LEARN_READY          ADD_NODE_STATUS_LEARN_READY
#define REMOVE_NODE_STATUS_NODE_FOUND           ADD_NODE_STATUS_NODE_FOUND
#define REMOVE_NODE_STATUS_REMOVING_SLAVE       ADD_NODE_STATUS_ADDING_SLAVE
#define REMOVE_NODE_STATUS_REMOVING_CONTROLLER  ADD_NODE_STATUS_ADDING_CONTROLLER
#define REMOVE_NODE_STATUS_DONE                 ADD_NODE_STATUS_DONE
#define REMOVE_NODE_STATUS_FAILED               ADD_NODE_STATUS_FAILED

/* Mode parameters to ZW_CreateNewPrimary */
#define CREATE_PRIMARY_START                ADD_NODE_CONTROLLER
#define CREATE_PRIMARY_STOP                 ADD_NODE_STOP
#define CREATE_PRIMARY_STOP_FAILED          ADD_NODE_STOP_FAILED

/* Mode parameters to ZW_ControllerChange */
#define CONTROLLER_CHANGE_START             ADD_NODE_CONTROLLER
#define CONTROLLER_CHANGE_STOP              ADD_NODE_STOP
#define CONTROLLER_CHANGE_STOP_FAILED       ADD_NODE_STOP_FAILED

/* Callback states from ZW_SetLearnMode */
#define LEARN_MODE_STARTED                  ADD_NODE_STATUS_LEARN_READY
#define LEARN_MODE_DONE                     ADD_NODE_STATUS_DONE
#define LEARN_MODE_FAILED                   ADD_NODE_STATUS_FAILED

/* Callback states from ZW_REQUEST_NODE_NEIGHBOR_UPDATE */
#define REQUEST_NEIGHBOR_UPDATE_STARTED     0x21
#define REQUEST_NEIGHBOR_UPDATE_DONE        0x22
#define REQUEST_NEIGHBOR_UPDATE_FAILED      0x23

/* ApplicationcControllerUpdate status */
#define UPDATE_STATE_SUC_ID                                   0x10
#define UPDATE_STATE_DELETE_DONE                              0x20
#define UPDATE_STATE_NEW_ID_ASSIGNED                          0x40
#define UPDATE_STATE_ROUTING_PENDING                          0x80
#define UPDATE_STATE_NODE_INFO_REQ_FAILED                     0x81
#define UPDATE_STATE_NODE_INFO_REQ_DONE                       0x82
#define UPDATE_STATE_NOP_POWER_RECEIVED                       0x83
#define UPDATE_STATE_NODE_INFO_RECEIVED                       0x84
#define UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED     0x85
#define UPDATE_STATE_INCLUDED_NODE_INFO_RECEIVED              0x86
#define UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED_LR  0x87

/* ZW_GetNeighborCount special return values */
#define NEIGHBORS_ID_INVALID          0xFE
#define NEIGHBORS_COUNT_FAILED        0xFF  /* Could not access routing info try again later */


/* ZW_RemoveFailedNode and ZW_ReplaceFailedNode return value definitions */
#define  NOT_PRIMARY_CONTROLLER             1 /* The removing process was */
                                              /* aborted because the controller */
                                              /* is not the primary one */

#define  NO_CALLBACK_FUNCTION               2 /* The removing process was */
                                              /* aborted because no call back */
                                              /* function is used */
#define  FAILED_NODE_NOT_FOUND              3 /* The removing process aborted */
                                              /* because the node was node */
                                              /* found */
#define  FAILED_NODE_REMOVE_PROCESS_BUSY    4 /* The removing process is busy */
#define  FAILED_NODE_REMOVE_FAIL            5 /* The removing process could not */
                                              /* be started */

#ifndef HOST_SECURITY_INCLUDED
#define ZW_FAILED_NODE_REMOVE_STARTED       0 /* The removing/replacing failed node process started */
#define ZW_NOT_PRIMARY_CONTROLLER           (1 << NOT_PRIMARY_CONTROLLER)
#define ZW_NO_CALLBACK_FUNCTION             (1 << NO_CALLBACK_FUNCTION)
#define ZW_FAILED_NODE_NOT_FOUND            (1 << FAILED_NODE_NOT_FOUND)
#define ZW_FAILED_NODE_REMOVE_PROCESS_BUSY  (1 << FAILED_NODE_REMOVE_PROCESS_BUSY)
#define ZW_FAILED_NODE_REMOVE_FAIL          (1 << FAILED_NODE_REMOVE_FAIL)
#endif

/* ZW_RemoveFailedNode and ZW_ReplaceFailedNode callback status definitions */
#define ZW_NODE_OK                          0 /* The node is working properly (removed from the failed nodes list ) */

/* ZW_RemoveFailedNode callback status definitions */
#define ZW_FAILED_NODE_REMOVED              1 /* The failed node was removed from the failed nodes list */
#define ZW_FAILED_NODE_NOT_REMOVED          2 /* The failed node was not removed from the failing nodes list */

/* ZW_ReplaceFailedNode callback status definitions */
#define ZW_FAILED_NODE_REPLACE              3 /* The failed node are ready to be replaced and controller */
                                              /* is ready to add new node with nodeID of the failed node */
#define ZW_FAILED_NODE_REPLACE_DONE         4 /* The failed node has been replaced */
#define ZW_FAILED_NODE_REPLACE_FAILED       5 /* The failed node has not been replaced */


/* ZW_RequestNetworkUpdate callback values*/
#define ZW_SUC_UPDATE_DONE      0x00
#define ZW_SUC_UPDATE_ABORT     0x01
#define ZW_SUC_UPDATE_WAIT      0x02
#define ZW_SUC_UPDATE_DISABLED  0x03
#define ZW_SUC_UPDATE_OVERFLOW  0x04

#define ZW_SUC_SET_SUCCEEDED    0x05
#define ZW_SUC_SET_FAILED       0x06

/* SUC capabilities used in ZW_SetSUCNodeID */
#define ZW_SUC_FUNC_NODEID_SERVER   0x01

/* Defines for ZW_GetControllerCapabilities */
#define CONTROLLER_IS_SECONDARY                 0x01
#define CONTROLLER_ON_OTHER_NETWORK             0x02
#define CONTROLLER_NODEID_SERVER_PRESENT        0x04
#define CONTROLLER_IS_REAL_PRIMARY              0x08
#define CONTROLLER_IS_SUC                       0x10
#define NO_NODES_INCUDED                        0x20

/* Z-Wave RF speed definitions */
#define ZW_RF_SPEED_NONE                        0x0000
#define ZW_RF_SPEED_9600                        0x0001
#define ZW_RF_SPEED_40K                         0x0002
#define ZW_RF_SPEED_100K                        0x0003
#define ZW_RF_SPEED_100KLR						0x0004
#define ZW_RF_SPEED_MASK                        0x0007

/* Z-Wave Long Range Channel */
#define ZW_LR_CHANNEL_A                         0x01
#define ZW_LR_CHANNEL_B                         0x02

/* ZW_GetRoutingInfo() options */
#define GET_ROUTING_INFO_REMOVE_BAD             0x80
#define GET_ROUTING_INFO_REMOVE_NON_REPS        0x40
#define ZW_GET_ROUTING_INFO_ANY                 ZW_RF_SPEED_NONE
#define ZW_GET_ROUTING_INFO_9600                ZW_RF_SPEED_9600
#define ZW_GET_ROUTING_INFO_40K                 ZW_RF_SPEED_40K
#define ZW_GET_ROUTING_INFO_100K                ZW_RF_SPEED_100K
#define ZW_GET_ROUTING_INFO_100KLR				ZW_RF_SPEED_100KLR
#define ZW_GET_ROUTING_INFO_SPEED_MASK          ZW_RF_SPEED_MASK

/* Listening bit in the NODEINFO capability byte */
#define NODEINFO_LISTENING_SUPPORT          0x80
/* Routing bit in the NODEINFO capability byte */
#define NODEINFO_ROUTING_SUPPORT            0x40

/* Optional functionality bit in the NODEINFO security byte*/
#define NODEINFO_OPTIONAL_FUNC_SUPPORT      0x80

/* Beam wakeup mode type bits in the NODEINFO security byte */
#define NODEINFO_ZWAVE_SENSOR_MODE_WAKEUP_1000   0x40
#define NODEINFO_ZWAVE_SENSOR_MODE_WAKEUP_250    0x20

/* Learn node state information passed by the call back function */
typedef struct _LEARN_INFO_T_
{
  uint8_t  bStatus;      /* Status of learn mode */
  node_id_t bSource;      /* Node id of the node that send node info */
  uint8_t  *pCmd;        /* Pointer to Application Node information */
  uint8_t  bLen;         /* Node info length                        */
} LEARN_INFO_T;

/* Learn node "Application Node information" passed by call back function */
/* to Application when controller is in SMART START mode and an aspiring */
/* nodeinformation frame is received */
typedef struct _LEARN_INFO_SMARTSTART_
{
  uint8_t      homeID[HOMEID_LENGTH];  /* HomeID the Nodeinfo was received with */
  uint8_t      nodeInfoLength;         /* Length of NodeInfo parameters */
  NODE_TYPE    nodeType;               /* Basic, Generic and Specific Device Type */
  uint8_t      nodeInfo[NODEPARM_MAX]; /* Device status */
} LEARN_INFO_SMARTSTART;

typedef void (*learn_mode_callback_t)(LEARN_INFO_T *);

/**
 *
 */
#define INIF_OPTIONS_TX_REASON_MASK 0x03

/**
 *
 */
typedef struct _CONTROLLER_UPDATE_INCLUDED_NODE_INFORMATION_FRAME_
{
  uint8_t  bINIFrxStatus;
  uint8_t  abINIFsmartStartNWIHomeID[HOMEID_LENGTH]; /* homeID used when using Smart Start */
} CONTROLLER_UPDATE_INCLUDED_NODE_INFORMATION_FRAME;


/* Route definitions defining Route structure used in  */
/* ZW_GetPriorityRoute/ZW_GetLastWorkingRoute and ZW_SetPriorityRoute/ZW_SetLastWorkingRoute */
#define ROUTECACHE_LINE_CONF_SIZE               1
#define ROUTECACHE_LINE_SIZE                    (MAX_REPEATERS + ROUTECACHE_LINE_CONF_SIZE)

/* PriorityRoute/LastWorkingRoute index definitions */
#define ROUTECACHE_LINE_REPEATER_0_INDEX        0
#define ROUTECACHE_LINE_REPEATER_1_INDEX        1
#define ROUTECACHE_LINE_REPEATER_2_INDEX        2
#define ROUTECACHE_LINE_REPEATER_3_INDEX        3
#define ROUTECACHE_LINE_CONF_INDEX              4

/* ZW_GetPriorityRoute and ZW_SetPriorityRoute speed definitions */
#define ZW_PRIORITY_ROUTE_SPEED_9600            ZW_RF_SPEED_9600
#define ZW_PRIORITY_ROUTE_SPEED_40K             ZW_RF_SPEED_40K
#define ZW_PRIORITY_ROUTE_SPEED_100K            ZW_RF_SPEED_100K

/* ZW_GetPriorityRoute function return value definitions */
/* Route returned is a Application defined Priority Route - APP_PR */
#define ZW_PRIORITY_ROUTE_APP_PR                0x10
/* Route returned is a Last Working Route - ZW_LWR */
#define ZW_PRIORITY_ROUTE_ZW_LWR                0x01
/* Route returned is a Next to Last Working Route - ZW_NLWR */
#define ZW_PRIORITY_ROUTE_ZW_NLWR               0x02

/* Obsolete - ZW_GetLastWorkingRoute and ZW_SetLastWorkingRoute speed definitions */
#define ZW_LAST_WORKING_ROUTE_SPEED_9600        ZW_PRIORITY_ROUTE_SPEED_9600
#define ZW_LAST_WORKING_ROUTE_SPEED_40K         ZW_PRIORITY_ROUTE_SPEED_40K
#define ZW_LAST_WORKING_ROUTE_SPEED_100K        ZW_PRIORITY_ROUTE_SPEED_100K

#endif /* ZW_CONTROLLER */

#endif /* _ZW_CONTROLLER_API_H_ */

