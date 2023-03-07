/**
* @file
*
*
* @startuml
* title Application <-> ZwStack interface
* box "Application task" #LightBlue
* participant Application
* end box
* participant TxQueue
* participant CommandQueue
* participant StatusQueue
* participant RxQueue
* box "ZwStack task" #Pink
* participant ZwStack
* end box
* participant Radio
* group Receiver task context
* == Tx ==
*   [->Application: Reason to send frame
*   activate Application
*   Application->TxQueue: Put frame on queue
*   activate ZwStack
*   TxQueue<->ZwStack: Take frame off queue
*   note left: OS context switches to ZwStack
*   ZwStack->Radio: Tx Frame
*   deactivate ZwStack
*   activate Radio
*   TxQueue->Application: Application continues\nfrom QueueSendToback()
*   note left: OS context swithces to Application
*   [<-Application: Finishes
*   deactivate Application
*   ...All tasks are sleeping...
*   Radio->ZwStack: Tx Complete
*   deactivate Radio
*   activate ZwStack
*   ZwStack->Radio: Listen for ack
*   deactivate ZwStack
*   activate Radio
*   ...All tasks are sleeping...
*   Radio->ZwStack: Ack received
*   deactivate Radio
*   activate ZwStack
*   ZwStack->StatusQueue: Put 'Frame successfully sent' on queue
*   deactivate ZwStack
*   activate Application
*   StatusQueue<->Application: Take status off queue
*   note left: Application awakened
*   [<-Application: Finished processing status
*   deactivate Application
* == Command ==
*   [->Application: Reason to send command
*   activate Application
*   Application->CommandQueue: Put command on queue
*   activate ZwStack
*   CommandQueue<->ZwStack: Take command off queue
*   note left: OS context switches to ZwStack
*   ...ZwStack processes command...
*   ZwStack->StatusQueue: Put 'Command status' on queue
*   deactivate ZwStack
*   StatusQueue->Application: Take status off queue
*   note left: OS context switches to Application
*   [<-Application: Finished processing status
*   deactivate Application
* == Rx ==
*   Radio->ZwStack: Frame received
*   note right: ZwStack awakened
*   activate ZwStack
*   ZwStack->RxQueue: Put frame on queue
*   deactivate ZwStack
*   activate Application
*   RxQueue<->Application: Take frame off queue
*   note left: Application awakened
*   ...Application thread processes frame...
* @enduml
*
*
* @copyright 2019 Silicon Laboratories Inc.
*/

#ifndef _ZW_APPLICATION_TRANSPORT_INTERFACE_H_
#define _ZW_APPLICATION_TRANSPORT_INTERFACE_H_

/*****************************************************************************
 *                              INCLUDE FILES
 ****************************************************************************/

#include <stddef.h>
#include <ZW_typedefs.h>
#include "ZW_classcmd.h"

#include "FreeRTOS.h" // needed for queue.h
#include "queue.h" // for QueueHandle_t
#include "QueueNotifying.h"
#include "ZW_transport_api.h"
#include "NodeMask.h"
#include "ZW_basis_api.h"
#include "Assert.h"


/*****************************************************************************
 *                             CONFIGURATIONS
 ****************************************************************************/

/**
 * Max theoretical Z-Wave frame payload size in a Z-Wave protocol using 3CH network
 * The real Z-Wave frame payload type depends on various parameters (routed, multicast, explore, security and/or number of RF channels)
 * Customer must not use this value in their application. They must use the value MaxPayloadSize from the SNetworkInfo structure.
*/
#define ZW_MAX_PAYLOAD_SIZE                         160

/*Numbers of nodes we can support when sending multicast frames on LR channel*/
#define MULTICAST_NODE_LIST_SIZE                    64

#define TX_BUFFER_SIZE                              170

//Maximum size for the node info frame contained in the SReceiveNodeUpdate struct.
#define MAX_NODE_INFO_LENGTH 159

#ifdef HOST_SECURITY_INCLUDED
/* Portable controller related definitions */
#define ZWAVE_DSK_LENGTH                                          16
#define ZWAVE_CONTROLLER_MAXIMUM_COMMAND_CLASS_LIST_LENGTH        64
#endif

/*****************************************************************************
 *                           CONSTANT DEFINITIONS
 ****************************************************************************/

#define APPLICATION_INTERFACE_TRANSMIT_ENUM_OFFSET  (0x00)
#define APPLICATION_INTERFACE_COMMAND_ENUM_OFFSET   (0x40)
#define APPLICATION_INTERFACE_RECEIVE_ENUM_OFFSET   (0x80)
#define APPLICATION_INTERFACE_STATUS_ENUM_OFFSET    (0xC0)

/*****************************************************************************
 *                           FORWARD DECLARATIONS
 ****************************************************************************/

/*****************************************************************************
 *                           TYPEDEF ENUMERATIONS
 ****************************************************************************/

typedef enum EProtocolType
{
  EPROTOCOLTYPE_ZWAVE = 0,
  EPROTOCOLTYPE_ZWAVE_AV,
  EPROTOCOLTYPE_ZWAVE_FOR_IP
} EProtocolType;

typedef enum ELibraryType
{
  ELIBRARYTYPE_CONTROLLER_STATIC     = 1, // DEPRECATED
  ELIBRARYTYPE_CONTROLLER_PORTABLE   = 2, // DEPRECATED
  ELIBRARYTYPE_SLAVE                 = 3, // Previously Slave Enhanced
  ELIBRARYTYPE_SLAVE_BEFORE_ENHANCED = 4, // DEPRECATED
  ELIBRARYTYPE_INSTALLER             = 5, // DEPRECATED
  ELIBRARYTYPE_SLAVE_ROUTING         = 6, // DEPRECATED
  ELIBRARYTYPE_CONTROLLER            = 7, // Previously Controller Bridge
  ELIBRARYTYPE_DUT                   = 8, // DEPRECATED
  ELIBRARYTYPE_AVREMOTE              = 10, // DEPRECATED
  ELIBRARYTYPE_AVDEVICE              = 11  // DEPRECATED
} ELibraryType;

typedef enum EInclusionState_t
{
  EINCLUSIONSTATE_EXCLUDED = 0,
  EINCLUSIONSTATE_UNSECURE_INCLUDED,
  EINCLUSIONSTATE_SECURE_INCLUDED
} EInclusionState_t;

typedef enum EListenBeforeTalkThreshold_t
{
  ELISTENBEFORETALKTRESHOLD_DEFAULT = 127
} EListenBeforeTalkThreshold_t;

typedef enum EtxPowerLevel_t
{
  ETXPOWERLEVEL_DEFAULT = 127
} EtxPowerLevel_t;

typedef enum EZwaveTransmitType
{
  EZWAVETRANSMITTYPE_STD = APPLICATION_INTERFACE_TRANSMIT_ENUM_OFFSET,
  EZWAVETRANSMITTYPE_EX,
  EZWAVETRANSMITTYPE_BRIDGE,
 // Multi types requires SZwaveTransmitPackage.NodeMask to be setup
  EZWAVETRANSMITTYPE_MULTI,
  EZWAVETRANSMITTYPE_MULTI_EX,
  EZWAVETRANSMITTYPE_MULTI_BRIDGE,
  EZWAVETRANSMITTYPE_EXPLOREINCLUSIONREQUEST,
  EZWAVETRANSMITTYPE_EXPLOREEXCLUSIONREQUEST,
  EZWAVETRANSMITTYPE_NETWORKUPDATEREQUEST,
  EZWAVETRANSMITTYPE_NODEINFORMATION,
  EZWAVETRANSMITTYPE_NODEINFORMATIONREQUEST,
  EZWAVETRANSMITTYPE_TESTFRAME,
  EZWAVETRANSMITTYPE_SETSUCNODEID,
  EZWAVETRANSMITTYPE_SENDSUCNODEID,
  EZWAVETRANSMITTYPE_ASSIGNRETURNROUTE,
  EZWAVETRANSMITTYPE_DELETERETURNROUTE,
  EZWAVETRANSMITTYPE_SENDREPLICATION,
  EZWAVETRANSMITTYPE_SENDREPLICATIONRECEIVECOMPLETE,
  EZWAVETRANSMITTYPE_REQUESTNEWROUTEDESTINATIONS,
  EZWAVETRANSMITTYPE_SEND_SLAVE_NODE_INFORMATION,
  EZWAVETRANSMITTYPE_SEND_SLAVE_DATA,
  EZWAVETRANSMITTYPE_INCLUDEDNODEINFORMATION,
  EZWAVETRANSMITTYPE_SECURE,
  EZWAVETRANSMITTYPE_NON_SECURE,
  NUM_EZWAVETRANSMITTYPE
} EZwaveTransmitType;

/**
 * Learn mode status updates.
 */
typedef enum ELearnStatus
{
  ELEARNSTATUS_ASSIGN_COMPLETE,             /**< Internal status. Not passed to application. */
  ELEARNSTATUS_ASSIGN_NODEID_DONE,          /**< Internal status. Node ID have been assigned */
  ELEARNSTATUS_ASSIGN_RANGE_INFO_UPDATE,    /**< Internal status. Node is doing Neighbor discovery */
  ELEARNSTATUS_ASSIGN_INFO_PENDING,         /**< Internal status. Not passed to application. */
  ELEARNSTATUS_ASSIGN_WAITING_FOR_FIND,     /**< Internal status. Not passed to application. */
  ELEARNSTATUS_SMART_START_IN_PROGRESS,     /**< Passed to application when Smart Start learn mode goes into progress. */
  ELEARNSTATUS_LEARN_IN_PROGRESS,           /**< Passed to application when classic learn mode goes into progress. */
  ELEARNSTATUS_LEARN_MODE_COMPLETED_TIMEOUT,/**< Passed to application if classic learn mode times out. */
  ELEARNSTATUS_LEARN_MODE_COMPLETED_FAILED  /**< Passed to application if learn mode failed. */
} ELearnStatus;

typedef enum EZwaveCommandType
{
  /**
   * @brief Generate a specific number of random bytes
   * 
   * @param[in] GenerateRandom.iLength Number of random bytes to generate. A maximum length of 32 random bytes is supported
   * 
   * @param[out] GenerateRandomStatus.iLength Number of random bytes generated
   * @param[out] GenerateRandomStatus.aRandomNumber Pointer to the randomly generated bytes
   * 
   * @warning Trying to generate more than the supported amount at once will instead yield zero bytes
   */
  EZWAVECOMMANDTYPE_GENERATE_RANDOM = APPLICATION_INTERFACE_COMMAND_ENUM_OFFSET, // 64

  /**
   * @brief Returns the Node Information Frame without command classes from the NVM for a given node ID
   * 
   * @details
   * | Byte descriptor \ \n Bit number | 7                                                  | 6                             | 5               | 4                             | 3 | 2 | 1 | 0 |
   * |:-------------------------------:|:--------------------------------------------------:|:-----------------------------:|:---------------:|:-----------------------------:|:-:|:-:|:-:|:-:|
   * | Capability                      | Listening                                          | Z-Wave Protocol-Specific Part |||||||
   * | Security                        | Opt. \n Func.                                      | Sensor \n 1000ms              | Sensor \n 250ms | Z-Wave Protocol-Specific Part |||||
   * | Reserved                        | Z-Wave Protocol-Specific Part                      ||||||||
   * | Basic                           | Basic Device Class (Z-Wave Protocol-Specific Part) ||||||||
   * | Generic                         | Generic Device Class (Z-Wave Appl.-Specific Part)  ||||||||
   * | Specific                        | Specific Device Class (Z-Wave Appl.-Specific Part) ||||||||
   * 
   * All the Z-Wave protocol-specific fields are initialized by the protocol. The Listening flag, Generic, and 
   * Specific Device Class fields are initialized by the application.
   * 
   * @param[in] NodeInfo.NodeId Node ID
   * 
   * @param[out] NodeInfoStatus.NodeId Node ID
   * @param[out] NodeInfoStatus.extNodeInfo Node info buffer (see figure above)
   * 
   * @note If NodeInfoStatus.extNodeInfo.NodeInfo.nodeType.generic = 0 then the node does not exist
   */
  EZWAVECOMMANDTYPE_NODE_INFO, // 65

  /**
   * @brief Clear the current network statistics collected by the Z-Wave protocol
   */
  EZWAVECOMMANDTYPE_CLEAR_NETWORK_STATISTICS, // 66

  /**
   * @brief Enable/disable home/node ID learn mode.
   * @details
   * When learn mode is enabled, received "Assign ID's Command" are handled:
   *   - If the current stored ID's are zero, the received ID's will be stored.
   *   - If the received ID's are zero the stored ID's will be set to zero.
   * 
   * # Controller
   * @brief Add or remove the controller to/from a Z-Wave network.
   * 
   * @details
   * Instruct the controller to allow it to be added or removed from the network.
   * 
   * When a controller is added to the network the following things will happen:
   *   1. If the current stored ID's are zero and the assigned ID's are nonzero, the received ID's will be 
   *   stored (node was added to the network).
   *   2. If the received ID's are zero the stored ID's will be set to zero (node was removed from the 
   *   network).
   *   3. The controller receives updates to the node list and the routing table but the ID's remain 
   *   unchanged.
   * 
   * This function will probably change the capabilities of the controller so it is recommended that the 
   * application calls @ref EZWAVECOMMANDTYPE_GET_CONTROLLER_CAPABILITIES after completion to check the controller status.
   * The learnFunc is called as the "Assign" process progresses. The returned nodeID is the nodes new 
   * Node ID. If no "Assign" is received from the including controller the callback function will not be called. It 
   * is then up to the application code to switch of Learn mode. Once the assignment process has been 
   * started the callback function may be called more than once. The learn process is not complete before 
   * the callback function is called with LEARN_MODE_DONE.
   * Network wide inclusion (add) should always be used as the default mode in inclusion to ensure 
   * compability with all implementations of Z-Wave controllers.
   * 
   * @note Learn mode should only be enabled when necessary and disabled again as quickly as possible. 
   * It is recommended that learn mode is not enabling for more than 2 second in 
   * ZW_SET_LEARN_MODE_CLASSIC mode and 5 seconds in either ZW_SET_LEARN_MODE_NWI or 
   * ZW_SET_LEARN_MODE_NWE mode.
   * 
   * @note When the controller is already included into a network (secondary or inclusion controller) the 
   * callback status LEARN_MODE_STARTED will not be made but the LEARN_MODE_DONE/FAILED 
   * callback will be made as normal.
   * 
   * @warning The learn process should not be stopped with ZW_SetLearnMode(FALSE,..) between the 
   * LEARN_MODE_STARTED and the LEARN_MODE_DONE status callback.
   * 
   * @param[in] SetLearnMode.eLearnMode Learn mode
   * @parblock
   * The learn mode states are:
   * - ZW_SET_LEARN_MODE_CLASSIC - Start the learn mode on the 
   *                               controller and only accept being 
   *                               included and exluded in direct 
   *                               range.
   * 
   * - ZW_SET_LEARN_MODE_NWI     - Start the learn mode on the 
   *                               controller and accept routed 
   *                               inclusion (add). NWI mode must 
   *                               not be used for exclusion 
   *                               (remove).
   * 
   * - ZW_SET_LEARN_MODE_NWE     - Start the learn mode on the 
   *                               controller and accept routed 
   *                               exclusion (remove) as well as 
   *                               direct. NWE mode must not be 
   *                               used for inclusion (add).
   * 
   * - ZW_SET_LEARN_MODE_DISABLE - Stop learn mode on the
   *                               controller.
   * @endparblock
   * @param[in] SetLearnMode.useCB Use callback function
   * 
   * ### useCB If SetLearnMode.useCB is true
   * @param[out] LearnModeStatus.Status Status of learn mode
   * 
   * # Slave
   * @brief Add or remove the slave to/from a Z-Wave network.
   * 
   * @details
   * Add a new slave node to a Z-Wave network or to remove an already added node from the network again.
   * 
   * The Slave node must identify itself to the including controller node by sending a Node Information Frame 
   * (see @ref EZWAVECOMMANDTYPE_NODE_INFO).
   * 
   * When learn mode is enabled, the following two actions can be performed by the protocol:
   *   1. If the current stored ID's are zero and the assigned ID's are nonzero, the received ID's will be 
   *      stored (node was added to the network).
   *   2. If the received ID's are zero the stored ID's will be set to zero (node was removed from the network).
   * 
   * The learnFunc is called as the "Assign" process progresses. The returned nodeID is the nodes new 
   * Node ID. If no "Assign" is received from the including controller the callback function will not be called. It 
   * is then up to the application code to switch of Learn mode. Once the assignment process has been 
   * started the Callback function may be called more than once. The learn process is not complete before 
   * the callback function is called with ASSIGN_COMPLETE
   * 
   * @note Learn mode should only be enabled when necessary and disabled again as quickly as possible. 
   * It is recommended that learn mode is not enabled for more than 2 seconds in
   * ZW_SET_LEARN_MODE_CLASSIC mode and 5 seconds when in either 
   * ZW_SET_LEARN_MODE_NWI or ZW_SET_LEARN_MODE_NWE mode.
   * 
   * @param[in] SetLearnMode.eLearnMode Learn mode
   * @parblock
   * The learn mode states are:
   * - ZW_SET_LEARN_MODE_CLASSIC - Start the learn mode on the 
   *                               slave and only accept being 
   *                               included and exluded in direct 
   *                               range.
   * 
   * - ZW_SET_LEARN_MODE_NWI     - Start the learn mode on the 
   *                               slave and accept routed 
   *                               inclusion (add). NWI mode must 
   *                               not be used for exclusion 
   *                               (remove).
   * 
   * - ZW_SET_LEARN_MODE_NWE     - Start the learn mode on the 
   *                               slave and accept routed 
   *                               exclusion (remove). NWE mode
   *                               must not be used for inclusion
   *                               (add).
   * 
   * - ZW_SET_LEARN_MODE_DISABLE - Stop learn mode on the slave.
   * @endparblock
   * @param[in] SetLearnMode.useCB Use callback function
   * 
   * ### If SetLearnMode.useCB is true
   * @param[out] LearnModeStatus.Status Status of learn mode
   */
  EZWAVECOMMANDTYPE_SET_LEARN_MODE, // 67

  /**
   * @brief Set the device back to the factory default state.
   * 
   * # Controller
   * @details Set the Controller back to the factory default state. Erase all Nodes, routing information and 
   * assigned homeID/nodeID from the NVM. In case the previous home ID was randomly generated then a 
   * new random home ID written to the NVM (random range: 0xC0000000-0xFFFFFFFE). A home ID 
   * outside random range reuses the initially configured home ID (configured during production).
   *
   * # Slave
   * @details Set the slave back to the factory default state. Erase routing information and assigned
   * homeID/nodeID from the external NVM. Finally write a new random home ID to the external NVM.
   */
  EZWAVECOMMANDTYPE_SET_DEFAULT,  // 68

  /**
   * @brief Stop any application initiated transmits
   */
  EZWAVECOMMANDTYPE_SEND_DATA_ABORT, // 69

  /**
   * @brief Enable/disable the installer library promiscuous mode.
   * 
   * @details When promiscuous mode is enabled, all application layer frames will be passed
   * to the application layer regardless if the frames are addressed to another node. \n
   * When promiscuous mode is disabled, only application frames addressed to the node will be passed
   * to the application layer.
   * 
   * A Controller in promiscuous mode will transfer payload from a promiscuously received application frame 
   * destined and originated from nodes residing in same network (HomeID). The promiscuously received 
   * application frame (only end destination frame) will be transferred to the application through 
   * ApplicationCommandHandler / ApplicationCommandHandler_Bridge with the 
   * RECEIVE_STATUS_FOREIGN_FRAME bit set in rxStatus.
   * A promiscuously application received frame is defined as:
   * - A frame transmitted using the same HomeID as the promiscuous controller itself.
   * - A frame neither originated by nor destined the promiscuous controller itself.
   * - A frame containing the application command class payload
   * 
   * An end destination frame is defined as either:
   * - Direct frame, 
   * - Outgoing routed frame sent by last repeater,
   * - Multicast frame or
   * - Explore frame
   * 
   * With the following exceptions when the controller is a repeater itself:
   * - Explore frames the controller actively repeats is not transferred to the application.
   * - Routed frames with the controller as the last repeater - only one promiscuously received 
   *   application frame is transferred through ApplicationCommandHandler / 
   *   ApplicationCommandHandler_Bridge and is transferred on the receiving of the frame transmitted 
   *   by the previous Repeater. This is done even if the controller, as repeater, does retries. Acting as 
   *   repeater the controller will try transmitting up to 3 times before giving up -> Routed Error.
   * 
   * Promiscuously received frames are delivered to the application via the ApplicationCommandHandler / 
   * ApplicationCommandHandler_Bridge callback function.
   * 
   * @param[in] SetPromiscuousMode.Enable
   * @parblock
   * TRUE to enable promiscuous mode \n
   * FALSE to disable it
   * @endparblock
   */
  EZWAVECOMMANDTYPE_SET_PROMISCUOUS_MODE, // 70

  /**
   * @brief Initialize the Z-Wave RF chip.
   * 
   * @param[in] SetRfReceiveMode.mode
   * @parblock
   * TRUE to set the RF chip to receive mode and start the data sampling \n
   * FALSE to set the RF chip to power down mode
   * @endparblock
   * @param[out] SetRFReceiveModeStatus.result
   * @parblock
   * TRUE if operation was successful \n
   * FALSE if operation was not successful
   * @endparblock
   */
  EZWAVECOMMANDTYPE_SET_RF_RECEIVE_MODE, // 71

  EZWAVECOMMANDTYPE_IS_NODE_WITHIN_DIRECT_RANGE, // 72

  EZWAVECOMMANDTYPE_GET_NEIGHBOR_COUNT, // 73

  EZWAVECOMMANDTYPE_ARE_NODES_NEIGHBOURS, // 74

  /**
   * @brief Test if a node ID is stored in the failed node ID list.
   * 
   * @remarks The failed node list is limited to 5 entries.
   * If a node is added to a full list the node with the lowest
   * failed count will be replaced with the new node ID.
   * 
   * @param[in] IsFailedNodeID.nodeID The node ID to check
   * @param[out] FailedNodeIDStatus.result true if node is found in the failed node list else false
   */
  EZWAVECOMMANDTYPE_IS_FAILED_NODE_ID, // 75

  /**
   * @brief Read out neighbor information
   * 
   * @details This information can be used to ensure that all nodes have a sufficient number of neighbors and to 
   * ensure that the network is in fact one network.
   * The format of the data returned in the buffer pointed to by RoutingInfo is as follows:
   * 
   * | RoutingInfo[i] (0  i < (ZW_MAX_NODES/8) |||||||||
   * |:-------|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
   * | Bit    | 0     | 1     | 2     | 3     | 4     | 5     | 6     | 7     |
   * | NodeID | i*8+1 | i*8+2 | i*8+3 | i*8+4 | i*8+5 | i*8+6 | i*8+7 | i*8+8 |
   * 
   * If a bit n in RoutingInfo[i] is 1 it indicates that the node with nodeID has node (i*8)+n+1 as a neighbour. If n in 
   * RoutingInfo[i] is 0, node with nodeID cannot reach node (i*8)+n+1 directly.
   * 
   * @param[in]  GetRoutingInfo.nodeID      Node ID of the node that routing info is needed from
   * @param[in]  GetRoutingInfo.options
   * @parblock
   * Upper nibble is bit flag options, lower nibble is speed \n
   * Combine exactly one speed with any number of options \n
   * 
   * Bit flags options for upper nibble:
   *   - GET_ROUTING_INFO_REMOVE_BAD       - Remove bad link from routing info
   *   - GET_ROUTING_INFO_REMOVE_NON_REPS  - Remove non-repeaters from the routing info
   * 
   * Speed values for lower nibble:
   *   - ZW_GET_ROUTING_INFO_ANY           - Return all nodes regardless of speed
   *   - ZW_GET_ROUTING_INFO_9600          - Return nodes supporting 9.6k
   *   - ZW_GET_ROUTING_INFO_40K           - Return nodes supporting 40k
   *   - ZW_GET_ROUTING_INFO_100K          - Return nodes supporting 100k
   *   - ZW_GET_ROUTING_INFO_100KLR        - Return nodes supporting 100k LR
   * @endparblock
   * 
   * @param[out] GetRoutingInfoStatus.RoutingInfo Pointer to routing info
   */
  EZWAVECOMMANDTYPE_GET_ROUTING_TABLE_LINE, // 76

  EZWAVECOMMANDTYPE_SET_ROUTING_INFO, // 77

  EZWAVECOMMANDTYPE_STORE_NODE_INFO, // 78

  EZWAVECOMMANDTYPE_STORE_HOMEID, // 79

  /**
   * @brief If true then any attempt to purge a LastWorkingRoute entry is denied.
   * 
   * @param[in] LockRouteResponse.value Lock route response
   */
  EZWAVECOMMANDTYPE_LOCK_ROUTE_RESPONSE, // 80

  /**
   * @brief Get the route with the highest priority
   * 
   * @details
   * If a route has been set with @ref EZWAVECOMMANDTYPE_SET_PRIORITY_ROUTE
   * then that route will be returned. If no priority route has been set the call will return
   * either the Last Working Route (LWR) or the Next to Last Working Route (NLWR). The LWR is the
   * last successful route used between sender and destination node and the NLWR is the previous LWR.
   * The PriorityRoute, LWR and NLWR are all stored in NVM.
   * 
   * @param[in] GetPriorityRoute.nodeID The node ID specifies the destination node whom the Priority Route is wanted from.
   * 
   * @param[out] GetPriorityRouteStatus.bAnyRouteFound
   * @parblock
   * NON ZERO if a Priority Route is found. Priority route is either
   *   - an Application injected Route or a LWR.
   *   - ZW_PRIORITY_ROUTE_APP_PR - Route is an App defined Priority Route
   *   - ZW_PRIORITY_ROUTE_ZW_LWR - Route is a Last Working Route
   *   - ZW_PRIORITY_ROUTE_ZW_NLWR - Route is a Next to Last Working Route
   * FALSE if no Priority Route is found
   * @endparblock
   * @param[out] GetPriorityRouteStatus.repeaters
   * @parblock
   * If route is found then a pointer to a 5-byte array containing the Priority Route.
   * The first 4 bytes (index 0-3) contain the repeaters active in the route and
   * the last (index 4) byte contains the speed information.
   * First ZERO in repeaters (index 0-3) indicates no more repeaters in route.
   * A direct route is indicated by the first repeater (index 0) being ZERO.
   * The routespeed byte (index 4) can be either 
   * ZW_PRIORITY_ROUTE_SPEED_9600, 
   * ZW_PRIORITY_ROUTE_SPEED_40K 
   * or 
   * ZW_PRIORITY_ROUTE_SPEED_100K
   * @endparblock
   */
  EZWAVECOMMANDTYPE_GET_PRIORITY_ROUTE, // 81

  /**
   * @brief Set the Priority Route for a destination node.
   * 
   * @details
   * The Priority Route is the route that will be used as the first routing attempt
   * by the protocol when transmitting to a node. The Priority Route is stored in NVM.
   * 
   * @param[in] SetPriorityRoute.nodeID The node ID specifies the destination node for whom the Priority Route is to be set.
   * @param[in] SetPriorityRoute.repeaters
   * @parblock
   * Pointer for a 5-byte array containing the new Priority Route to be set.
   * If NULL then current APR is released (protocol now has 2 entries for LWRs).
   * The first 4 bytes (index 0-3) contains the repeaters active in the route
   * and last (index 4) byte contains the speed information.
   * The routespeed byte (index 4) can be either 
   * ZW_PRIORITY_ROUTE_SPEED_9600, 
   * ZW_PRIORITY_ROUTE_SPEED_40K or 
   * ZW_PRIORITY_ROUTE_SPEED_100K
   * @endparblock
   * @param[in] SetPriorityRoute.clearGolden if true then clear the Priority Route else set it

   * @param[out] SetPriorityRouteStatus.bRouteUpdated
   * TRUE if the Priority Route was successfully set to the specified route
   * FALSE if the specified node ID was not valid and no Priority Route was set
   */
  EZWAVECOMMANDTYPE_SET_PRIORITY_ROUTE, // 82

  /**
   * @brief Enable/disable home/node ID learn mode for virtual nodes.
   * When learn mode is enabled, received "Assign ID's Command" for the current virtual node 'node' are handled:
   * If 'node' is zero, the received nodeID will be stored as a new virtual node.
   * If the received nodeID is zero the virtual node 'node' will be removed.
   * 
   * @details Enable the possibility for enabling or disabling "Slave Learn Mode", which 
   * when enabled makes it possible for other controllers (primary or inclusion controllers) to add or remove a 
   * Virtual Slave Node to the Z-Wave network. Also is it possible for the bridge controller (only when primary 
   * or inclusion controller) to add or remove a Virtual Slave Node without involving other controllers. 
   * Available Slave Learn Modes are:
   *   - VIRTUAL_SLAVE_LEARN_MODE_DISABLE - Disables the Slave Learn Mode so that no Virtual 
   *     Slave Node can be added or removed.
   * 
   *   - VIRTUAL_SLAVE_LEARN_MODE_ENABLE - Enables the possibility for other Primary/Inclusion 
   *     controllers to add or remove a Virtual Slave Node. To add a new Virtual Slave node to the Z-Wave 
   *     Network the provided "node" ID must be ZERO and to make it possible to remove a specific 
   *     Virtual Slave Node the provided "node" ID must be the nodeID for this specific (locally present) 
   *     Virtual Slave Node. When the Slave Learn Mode has been enabled the Virtual Slave node must 
   *     identify itself to the external Primary/Inclusion Controller node by sending a "Node Information" 
   *     frame (see ZW_SendSlaveNodeInformation) to make the add/remove operation commence.
   * 
   *   - VIRTUAL_SLAVE_LEARN_MODE_ADD - Add Virtual Slave Node to the Z-Wave network 
   *     without involving any external controller. This Slave Learn Mode is only possible when bridge 
   *     controller is either a Primary controller or an Inclusion controller. 
   * 
   *   - VIRTUAL_SLAVE_LEARN_MODE_REMOVE - Remove a locally present Virtual Slave Node from 
   *     the Z-Wave network without involving any external controller. This Slave Learn Mode is only 
   *     possible when bridge controller is either a Primary controller or an Inclusion controller.
   * 
   * The callback function is called as the "Assign" process progresses. The returned "orgID" is the Virtual 
   * Slave node put into Slave Learn Mode, the "newID" is the new Node ID. If the Slave Learn Mode is 
   * VIRTUAL_SLAVE_LEARN_MODE_ENABLE and nothing is received from the assigning controller the 
   * callback function will not be called. It is then up to the main application code to switch of Slave Learn 
   * mode by setting the VIRTUAL_SLAVE_LEARN_MODE_DISABLE Slave Learn Mode. Once the 
   * assignment process has been started the callback function may be called more than once.
   * 
   * @param[in] NetworkManagement.mode Mode
   * @param[in] NetworkManagement.nodeID Node ID on virtual node to set in Learn Mode - for adding a virtual node then this must be ZERO
   * 
   * @param[out] SetSlaveLearnModeStatus.result true if successful or false if node is invalid or controller is primary
   * 
   * ### Callback
   * @param[out] NetworkManagementStatus.statusInfo Pointer to a 5-byte array
   * @parblock
   * - index 0: Status of the assign process:
   *   - ASSIGN_COMPLETE          - Is returned by the callback function when 
   *                                in the VIRTUAL_SLAVE_LEARN_MODE_ENABLE 
   *                                Slave Learn Mode and assignment is done. 
   *                                Now the Application can continue normal 
   *                                operation.
   * 
   *   - ASSIGN_NODEID_DONE       - Node ID have been assigned. The "orgID" 
   *                                contains the node ID on the Virtual Slave 
   *                                Node who was put into Slave Learn Mode. 
   *                                The "newID" contains the new node ID for 
   *                                "orgID". If "newID" is ZERO then the "orgID" 
   *                                Virtual Slave node has been deleted and the 
   *                                assign operation is completed. When this 
   *                                status is received the Slave Learn Mode is 
   *                                complete for all Slave Learn Modes except 
   *                                the VIRTUAL_SLAVE_LEARN_MODE_ENABLE mode.
   * 
   *   - ASSIGN_RANGE_INFO_UPDATE - Node is doing Neighbour discovery 
   *                                Application should not attempt to send any 
   *                                frames during this time, this is only applicable 
   *                                when in VIRTUAL_SLAVE_LEARN_MODE_ENABLE.
   * 
   * - index 1: org node ID MSB
   * - index 2: org node ID LSB
   *   - The original node ID that was put into Slave Learn Mode.
   * 
   * - index 3: new node ID MSB
   * - index 4: new node ID LSB
   *   - The new Node ID. Zero if "OrgID" was deleted from the Z-Wave network.
   * @endparblock
   */
  EZWAVECOMMANDTYPE_SET_SLAVE_LEARN_MODE, // 83

  /**
   * @brief Check if node is a Virtual Slave node.
   * 
   * @param[in] IsVirtualNode.value Node ID
   * @param[out] IsVirtualNodeStatus.result true if node is a Virtual Slave node else false
   */
  EZWAVECOMMANDTYPE_IS_VIRTUAL_NODE, // 84

  /**
   * @brief Read the virtual node flags for all nodes in the network.
   * Will read the flag from all available node data files.
   * 
   * @param[out] GetVirtualNodesStatus.vNodesMask Buffer to store the virtual node flags read from the nodes data files
   * the flags are stored as a bitmask array and the array size is 29 bytes
   */
  EZWAVECOMMANDTYPE_GET_VIRTUAL_NODES, // 85

  /**
   * @brief Returns the Controller capabilities
   * 
   * @details
   * The returned capability is a bitmask where the following bits are defined:
   *   - CONTROLLER_IS_SECONDARY          - If bit is set then the controller is a secondary controller
   * 
   *   - CONTROLLER_ON_OTHER_NETWORK      - If this bit is set then this controller is not using its built-in home ID
   * 
   *   - CONTROLLER_NODEID_SERVER_PRESENT - If this bit is set then there is a SUC ID server (SIS) in the 
   *                                        network and this controller can therefore include/exclude nodes 
   *                                        in the network. This is called an inclusion controller
   * 
   *   - CONTROLLER_IS_REAL_PRIMARY       - If this bit is set then this controller was the original primary controller 
   *                                        in the network before the SIS was added to the network
   * 
   *   - CONTROLLER_IS_SUC                - If this bit is set then this controller is a SUC
   * 
   *   - NO_NODES_INCUDED                 - If this bit is set then no nodes are included
   * 
   * @note Not all status bits are available on all controllers' types
   * 
   * @param[out] GetControllerCapabilitiesStatus.result Capability bitmask
   */
  EZWAVECOMMANDTYPE_GET_CONTROLLER_CAPABILITIES, // 86

  /**
   * @brief Set the maximum number of route tries which should be done before failing
   * or resorting to exploring if this is specified
   * 
   * @param[in] SetRoutingMax.value Maximum number of source routing attempts
   */
  EZWAVECOMMANDTYPE_SET_ROUTING_MAX, // 87

  /**
   * @brief Returns wether the controller is a primary controller
   * 
   * @param[out] IsPrimaryCtrlStatus.result true if the controller is a primary controller else false
   */
  EZWAVECOMMANDTYPE_IS_PRIMARY_CTRL, // 88

  // TODO:
  /**
   * @brief Add any type of node to the network.
   * 
   * @param[in] NetworkManagement.pHandle Use callback function
   * @param[in] NetworkManagement.mode Mode
   * @parblock
   * - ADD_NODE_ANY                 - Add any node to the network
   * - ADD_NODE_CONTROLLER          - Add a controller to the network
   * - ADD_NODE_SLAVE               - Add a slave node to the network
   * - ADD_NODE_SMART_START         - Listen for SmartStart node wanting to be included
   * - ADD_NODE_STOP                - Stop learn mode without reporting an error
   * - ADD_NODE_STOP_FAILED         - Stop learn mode and report an error to the new controller
   * - ADD_NODE_OPTION_NORMAL_POWER - Set this flag in mode for High Power inclusion
   * - ADD_NODE_OPTION_NETWORK_WIDE - Set this flag in mode for enabling Networkwide inclusion via explore frames
   * @endparblock
   * 
   * ### If NetworkManagement.pHandle is not NULL
   * @param[out] NetworkManagementStatus.statusInfo
   */
  EZWAVECOMMANDTYPE_ADD_NODE_TO_NETWORK, // 89

  // TODO:
  /**
   * @brief 
   * 
   * @param[in] NetworkManagement.mode Mode
   * 
   * @param[out] NetworkManagementStatus.statusInfo
   */
  EZWAVECOMMANDTYPE_REMOVE_NODE_FROM_NETWORK, // 90

  EZWAVECOMMANDTYPE_AES_ECB, // 91

  /**
   * @brief Returns the most recent background RSSI levels detected. The RSSI is only measured 
   * when the radio is in receive mode.
   * 
   * @param[out] GetBackgroundRssiStatus.rssi  Pointer to a struct containing the RSSI levels in dBms
   * 
   * The rssi levels struct is defined as
   * @code{.c}
   * typedef struct
   * {
   *   signed char rssi_dBm[NUM_CHANNELS_LR];
   * } RSSI_LEVELS;
   * @endcode
   * Each value in the rssi_dBm contains an RSSI value encoded according to the table below. NUM_CHANNELS_LR is set to 4.
   * 
   * The rssi level in a single channel is contained in a BYTE encoded as a signed integer:
   * | Hexadecimal | Decimal  | Description                                                         |
   * |-------------|----------|---------------------------------------------------------------------|
   * | 0x80..0xFF  | -128..-1 |            This value represents the measured RSSI in dBm           |
   * | 0x00..0x7C  | 0..124   |            This value represents the measured RSSI in dBm           |
   * | 0x7D        | 125      |       The RSSI is below sensitivity and could not be measured.      |
   * | 0x7E        | 126      | The radio receiver is saturated and the RSSI could not be measured. |
   * | 0x7F        | 127      |                      The RSSI is not available.                     |
   */
  EZWAVECOMMANDTYPE_GET_BACKGROUND_RSSI, // 92

  /**
   * @brief Remove a node from the failed node list
   * 
   * @details Remove a non-responding node from the routing table in the requesting controller.
   * A non-responding node is put onto the failed node ID list in the requesting controller. In case the node responds 
   * again at a later stage then it is removed from the failed node ID list. A node must be on the failed node ID 
   * list and as an extra precaution also fail to respond before it is removed. Responding nodes can't be 
   * removed. The call works on a primary controller and an inclusion controller.
   * 
   * @param[in] FailedNodeIDCmd.nodeID Node ID
   * 
   * @param[out] FailedNodeIDStatus.result
   * @parblock
   * Return value (If the replacing process started successfully then the function will return):
   *   - ZW_FAILED_NODE_REMOVE_STARTED - The removing process started
   *     
   *   Return values (If the replacing process cannot be started then the API function will return one or more 
   *   of the following flags):
   *   - ZW_NOT_PRIMARY_CONTROLLER          - The removing process was aborted 
   *                                          because the controller is not the 
   *                                          primary one.
   * 
   *   - ZW_NO_CALLBACK_FUNCTION            - The removing process was aborted 
   *                                          because no callback function is used.
   * 
   *   - ZW_FAILED_NODE_NOT_FOUND           - The requested process failed.
   *                                          The nodeID was not found in the 
   *                                          controller list of failing nodes.
   * 
   *   - ZW_FAILED_NODE_REMOVE_PROCESS_BUSY - The removing process is busy.
   * 
   *   - ZW_FAILED_NODE_REMOVE_FAIL         - The requested process failed. 
   *                                          Reasons include:
   *                                            - Controller is busy
   *                                            - The node responded to a NOP,
   *                                              therefore, the node is no longer 
   *                                              failing.
   * @endparblock
   */
  EZWAVECOMMANDTYPE_REMOVE_FAILED_NODE_ID, // 93

  /**
   * @brief Replace a node from the failed node list
   * 
   * @details Replace a non-responding node with a new one in the requesting controller.
   * A non-responding node is put onto the failed node ID list in the requesting controller. In case the node responds 
   * again at a later stage then it is removed from the failed node ID list. A node must be on the failed node ID 
   * list and as an extra precaution also fail to respond before it is removed. Responding nodes can't be 
   * replace. The call works on a primary controller and an inclusion controller.
   * 
   * @param[in] FailedNodeIDCmd.nodeID Node ID
   * 
   * @param[out] FailedNodeIDStatus.result
   * @parblock
   * Return value (If the replacing process started successfully then the function will return):
   *   - ZW_FAILED_NODE_REMOVE_STARTED The replacing process has started.
   * 
   * Return values (If the replacing process cannot be started then the API function will return one or more 
   * of the following flags:):
   *   - ZW_NOT_PRIMARY_CONTROLLER          - The replacing process was aborted 
   *                                          because the controller is not a 
   *                                          primary/inclusion/SIS controller.
   * 
   *   - ZW_NO_CALLBACK_FUNCTION            - The replacing process was aborted 
   *                                          because no callback function is used.
   * 
   *   - ZW_FAILED_NODE_NOT_FOUND           - The requested process failed.
   *                                          The nodeID was not found in the 
   *                                          controller list of failing nodes.
   * 
   *   - ZW_FAILED_NODE_REMOVE_PROCESS_BUSY - The removing process is busy.
   * 
   *   - ZW_FAILED_NODE_REMOVE_FAIL         - The requested process failed. 
   *                                          Reasons include:
   *                                            - Controller is busy
   *                                            - The node responded to a NOP,
   *                                              therefore, the node is no longer 
   *                                              failing.
   * @endparblock
   */
  EZWAVECOMMANDTYPE_REPLACE_FAILED_NODE_ID, // 94

  EZWAVECOMMANDTYPE_PM_STAY_AWAKE, // 95 - OBSOLETED
  EZWAVECOMMANDTYPE_PM_CANCEL, // 96 - OBSOLETED
  EZWAVECOMMANDTYPE_PM_REGISTER, // 97 - OBSOLETED

  // TODO:
  /**
   * @brief 
   * 
   * @details 
   * 
   * @param[in] UpdateCtrlNodeInformation.value
   */
  EZWAVECOMMANDTYPE_ZW_UPDATE_CTRL_NODE_INFORMATION, // 98

  /**
   * @brief Set the LBT RSSI level
   * 
   * @details
   * Set the "Listen Before Talk" threshold that controls at what RSSI level a Z-Wave node 
   * will refuse to transmit because of noise.
   * ZW_SetListenBeforeTalkThreshold stores the threshold values in RAM thus it needs to be called after 
   * every reset or power-up.
   * The RAM variables that stores the threshold values are initialized after ApplicationInitHW thus 
   * ZW_SetListenBeforeTalkThreshold should be called in ApplicationInitSW.
   * The default threshold value is set to a value corresponding to the RF regulatory requirements for a Z-Wave module in the specific country.
   * The appropriate value range goes from 34(dec) to 78(dec) and each threshold step corresponds to a 1.5dB input power step.
   * 
   * | Region                               | Default Threshold (dec) | dBm |
   * |--------------------------------------|-------------------------|-----|
   * | JP                                   | 50                      | -80 |
   * | KR                                   | 64                      | -65 |
   * | EU, US, HK, ANZ, CN, IL, IN, MY & RU | 64                      | -65 |
   * 
   * For instance, if a SAW filter with an insertion loss of 3dB is inserted between the antenna feed-point and 
   * the chip on a JP product, the threshold value should be set to 48(dec) .
   * 
   * @note In some contries (JP and KR) the value of the LBT threshold is specified in the RF regulatory 
   *       for the country and must be set to the value corresponding to the regulatory requirements.
   * 
   * @param[in] SetLBTThreshold.channel Channel number the threshold should be set for.
   *                                    Valid channel numbers are 0, 1 and 2
   * 
   * @param[in] SetLBTThreshold.level   LBT RSSI level in dBm.
   *                                    The threshold the RSSI should use. 
   *                                    Valid threshold range is from 34(dec) to 
   *                                    78(dec). 34 will set the threshold to the 
   *                                    lowest amount of noise which the node 
   *                                    will refuse to transmit and thus will only 
   *                                    transmit in a quiet RF environment. 78 
   *                                    sets the threshold to the highest amount 
   *                                    of noise and the node will transmit even 
   *                                    in the presence of a lot of RF noise.
   */
  EZWAVECOMMANDTYPE_ZW_SET_LBT_THRESHOLD, // 99

  // TODO:
  /**
   * @brief 
   * 
   * @param[in] NetworkManagementDSK.pHandle Use callback function
   * @param[in] NetworkManagementDSK.mode Mode
   * @param[in] NetworkManagementDSK.dsk DSK
   * 
   * ### If NetworkManagement.pHandle is not NULL
   * @param[out] NetworkManagementStatus.statusInfo
   */
  EZWAVECOMMANDTYPE_ADD_NODE_DSK_TO_NETWORK, // 100

  /**
   * @brief Enable/disable the Network Management module inclusion/exclusion mode sequence according to specified mode.
   * 
   * @param[in] SetSmartStartLearnMode.eLearnMode Mode
   * @parblock
   * - E_NETWORK_LEARN_MODE_DISABLE              - Disable learn process
   * - E_NETWORK_LEARN_MODE_INCLUSION            - Enable the learn process to do an inclusion
   * - E_NETWORK_LEARN_MODE_EXCLUSION            - Enable the learn process to do an exclusion
   * - E_NETWORK_LEARN_MODE_EXCLUSION_NWE        - Enable the learn process to do a Networkwide exclusion
   * - E_NETWORK_LEARN_MODE_INCLUSION_SMARTSTART - Enable the learn process to do a SmartStart inclusion
   * @endparblock
   * @param[in] SetSmartStartLearnMode.eWakeUpReason Wake up reason
   * 
   * @param[out] NetworkManagementStatus.statusInfo true if Requested Network Learn process is initiated else false 
   */
  EZWAVECOMMANDTYPE_NETWORK_LEARN_MODE_START, // 101

  EZWAVECOMMANDTYPE_CREAT_NEW_PRIMARY_CTRL, // 102

  /**
   * @brief Add a controller to the Z-Wave network and transfer the role as primary controller to it
   * 
   * @details
   * Has the same functionality as @ref EZWAVECOMMANDTYPE_ADD_NODE_TO_NETWORK with mode ADD_NODE_ANY, except that the new controller
   * will be a primary controller and the controller invoking the function will become secondary.
   * 
   * @param[in] NetworkManagement.pHandle Use callback function
   * @param[in] NetworkManagement.mode Mode
   * @parblock
   * - CONTROLLER_CHANGE_START       - Start the process of adding a controller to the network
   * - CONTROLLER_CHANGE_STOP        - Stop the controller change
   * - CONTROLLER_CHANGE_STOP_FAILED - Stop the controller change and report a failure
   * 
   * - ADD_NODE_OPTION_NORMAL_POWER  - Set this flag in mode for High Power exchange.
   * @endparblock
   * 
   * ### If NetworkManagement.pHandle is not NULL
   * @param[out] NetworkManagementStatus.statusInfo
   * @parblock
   * - index 0: Status of learn mode:
   *   - ADD_NODE_STATUS_LEARN_READY       - The controller is now ready to include a node into the network
   *   - ADD_NODE_STATUS_NODE_FOUND        - A node that wants to be included into the network has been found
   *   - ADD_NODE_STATUS_ADDING_CONTROLLER - A new controller has been added to the network
   *   - ADD_NODE_STATUS_PROTOCOL_DONE     - The protocol part of adding a controller is complete
   *   - ADD_NODE_STATUS_DONE              - The new node has now been included and the controller is ready to continue normal operation again
   *   - ADD_NODE_STATUS_FAILED            - The learn process failed
   * 
   * - index 1: Node ID MSB
   * - index 2: Node ID LSB
   *   - Node ID of the new node
   * 
   * - index 3: Node info length
   * - index 4: Pointer to Application Node information data. NULL if no information present.
   *            Only contains information when the length is not zero,
   *            so the information should be stored when that is the case. Regardless of the bStatus.
   * @endparblock
   */
  EZWAVECOMMANDTYPE_CONTROLLER_CHANGE, // 103

  /**
   * @brief Clear current Channel Transmit timers
   * 
   * @details Clear the protocol's internal tx timers.
   * The tx timers are updated by the protocol every time a frame is sent.
   */
  EZWAVECOMMANDTYPE_CLEAR_TX_TIMERS,  // 104

  /**
   * @brief Set the maximum interval between SmartStart inclusion requests
   * 
   * @param[in] SetMaxInclReqInterval.inclusionRequestInterval
   * @parblock
   * The maximum number of 128 sec ticks between SmartStart inclusion requests.
   * Valid range is 5-99, which corresponds to 640-12672 sec.
   * @endparblock
   * 
   * @param[out] NetworkManagementStatus.statusInfo true if interval was set to the provided value else false
   */
  EZWAVECOMMANDTYPE_ZW_SET_MAX_INCL_REQ_INTERVALS, // 105

  // TODO:
  /**
   * @brief 
   * 
   * @param[in] NvmBackupRestore.offset
   * @param[in] NvmBackupRestore.length
   * 
   * @param[out] NvmBackupRestoreStatus.status
   */
  EZWAVECOMMANDTYPE_NVM_BACKUP_OPEN, // 106

  // TODO:
  /**
   * @brief
   * @param[in] NvmBackupRestore.offset
   * @param[in] NvmBackupRestore.length
   * @param[in] NvmBackupRestore.nvmData
   * 
   * @param[out] NvmBackupRestoreStatus.status
   * 
   */
  EZWAVECOMMANDTYPE_NVM_BACKUP_READ, // 107

  // TOOD:
  /**
   * @brief
   * 
   * @param[in] NvmBackupRestore.offset
   * @param[in] NvmBackupRestore.length
   * @param[in] NvmBackupRestore.nvmData
   * 
   * @param[out] NvmBackupRestoreStatus.status
   */
  EZWAVECOMMANDTYPE_NVM_BACKUP_WRITE, // 108

  // TODO:
  /**
   * @brief 
   * 
   * 
   */
  EZWAVECOMMANDTYPE_NVM_BACKUP_CLOSE, // 109

  /**
   * @brief Register application functions to be called just before power down
   * 
   * @details The provided function will be called as the last steps before the chip is
   * forced into EM4 hibernate.
   * 
   * @note When the function is called the OS tick has been disabled and the FreeRTOS
   * scheduler is no longer running. OS features like events, queues and timers
   * are therefore unavailable.
   * 
   * @param[in] PMSetPowerDownCallback.callback Function to call on power down.
   * Set to NULL to unregister any previously registered function.
   * 
   * @param[out] SetPowerDownCallbackStatus.result true if function was added else false
   */
  EZWAVECOMMANDTYPE_PM_SET_POWERDOWN_CALLBACK, // 110

  /**
   * @brief Initiate the S2 inclusion engine
   * 
   * @param[in] SetSecurityKeys.keys Keys to request during inclusion
   */
  EZWAVECOMMANDTYPE_SET_SECURITY_KEYS, // 111

  // TODO:
  /**
   * @brief Perform a soft reset
   * 
   */
  EZWAVECOMMANDTYPE_SOFT_RESET, // 112

  // TODO:
  /**
   * @brief 
   * 
   */
  EZWAVECOMMANDTYPE_BOOTLOADER_REBOOT, // 113

  // TODO:
  /**
   * @brief Remove a specific node from a Z-Wave network
   * 
   * @param[in] NetworkManagement.pHandle Use callback function
   * @param[in] NetworkManagement.mode Mode
   * @param[in] NetworkManagement.nodeID Node ID
   * 
   * ### If NetworkManagement.pHandle is not NULL
   * @param[out] NetworkManagementStatus.statusInfo
   */
  EZWAVECOMMANDTYPE_REMOVE_NODEID_FROM_NETWORK, // 114

  /**
   * @brief Get the node IDs for the nodes included in the network
   * 
   * @param[out] GetIncludedNodes.node_id_list A bitmask list of the nodes included in the network
   */
  EZWAVECOMMANDTYPE_ZW_GET_INCLUDED_NODES, // 115

  /**
   * @brief Start neighbor discovery for node if primary and other nodes are present
   * 
   * @param[in] RequestNodeNeighborUpdate.Handle Callback function
   * @param[in] RequestNodeNeighborUpdate.NodeId Node ID
   * 
   * @param[out] RequestNodeNeigborUpdateStatus.result true if neighbor discovery started else false
   */
  EZWAVECOMMANDTYPE_REQUESTNODENEIGHBORUPDATE, // 116

  /**
   * @brief Initiate graceful shutdown
   * 
   * @details The API will stop the radio and cancel all timers and power locks.
   * The protocol will notfiy the applciation when ready to go into deepsleep by the provided callback
   * 
   * @param[in] InitiateShutdown.Handle Pointer to callback used to notify the application before ging into deep sleep
   * 
   * @param[out] InitiateShutdownStatus.result true
   */
  EZWAVECOMMANDTYPE_ZW_INITIATE_SHUTDOWN, // 117

  /**
   * @brief Get the node IDs for the long range nodes included in the network
   * 
   * @param[out] GetIncludedNodesLR.node_id_list A bitmask list of the long range nodes included in the network
   */
  EZWAVECOMMANDTYPE_ZW_GET_INCLUDED_LR_NODES, // 118

  /**
   * @brief Get the current Primary Long Range Channel
   * 
   * @param[out] GetLRChannel.result Current Primary Long Range Channel
   */
  EZWAVECOMMANDTYPE_ZW_GET_LR_CHANNEL, // 119

  /**
   * @brief Set the Primary Long Range Channel
   * 
   * @param[in] SetLRChannel.value Primary Long Range Channel
   */
  EZWAVECOMMANDTYPE_ZW_SET_LR_CHANNEL, // 120

  /**
   * @brief Enable Long Range virtual node IDs
   * 
   * @param[in] SetLRVirtualNodeIDs.value Bitmask. The lowest 4 bits enables node ID 4002, 4003, 4004 and 4005
   */
  EZWAVECOMMANDTYPE_ZW_SET_LR_VIRTUAL_IDS, // 121

  /**
   * Legacy command. Now unused.
   */
  EZWAVECOMMANDTYPE_UNUSED1, //122
  EZWAVECOMMANDTYPE_UNUSED2, //123

  /**
   * @brief Check if the PTI Zniffer functionality is enabled/disabled
   * 
   * @param[out] GetPTIconfig.result true if enabled else false
   */
  EZWAVECOMMANDTYPE_ZW_GET_PTI_CONFIG,  // 124

  /**
   * @brief Reduce Tx power of classic non-listening devices
   * 
   * @param[in] SetTxAttenuation.value Reduces the devices default Tx power in dB. Valid range: 0-9 dB
   * 
   * @param[out] SetTxAttenuation.result true when reduction is allowed / false for listening devices and out of range input
   */
  EZWAVECOMMANDTYPE_ZW_SET_TX_ATTENUATION, // 125


  /*********************************************
   * SECURE API interface functions used in apps.
   ********************************************/
  /**
   * @brief Add a new node to the network
   *
   * @details Used by Portable Controller application to start the process of including a new node to its network.
   * After successful inclusion the protocol will send an EZWAVECOMMANDSTATUS_SECURE_ON_NODE_ADDED event
   * to the application.
   */
  EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_ADD_NODE, // 126

  /**
   * @brief Abort Inclusion or Exclusion of node.
   *
   * @details Used by Portable Controller application to abort an Inclusion or Exclusion process it has started.
   */
  EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_ABORT, // 127

  /**
   * @brief Remove a node from the network
   *
   * @details Used by Portable Controller application to start the process of excluding a node from its network.
   * After successful exclusion the protocol will send an EZWAVECOMMANDSTATUS_SECURE_ON_NODE_DELETED event
   * to the application.
   */
  EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_REMOVE_NODE, // 128

  /**
   * @brief Unused. Data frames to network nodes are placed directly on the Zwave TxQueue
   */
  EZWAVECOMMANDTYPE_SECURE_SEND_DATA, // 129

  /**
   * @brief Put the application in Network Wide Inclusion mode so it can be included in a network.
   *
   * @details Used by Portable Controller application to get included by a different Controller.
   * After successful inclusion the protocol will send an EZWAVECOMMANDSTATUS_SECURE_ON_NEW_NETWORK_ENTERED event
   * to the application.
   */
  EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_LEARN_MODE_INCLUSION, // 130

  /**
   * @brief Put the application in Network Wide Exclusion mode so it can be excluded from a network.
   *
   * @details Used by Portable Controller application to get excluded from a network.
   * After successful exclusion the protocol will send an EZWAVECOMMANDSTATUS_SECURE_ON_NETWORK_MANAGEMENT_STATE_UPDATE
   * event to the application.
   */
  EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_LEARN_MODE_EXCLUSION, // 131

  /**
   * @brief Set security flags for a network node.
   *
   * @details Used by Portable Controller application to set security related flags for a node in its network.
   * S2 capable flag, S2 included flag, Secure included flag.
   */
  EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_SET_SECURITY_FLAGS, // 132

  NUM_EZWAVECOMMANDTYPE
} EZwaveCommandType;

typedef enum EZwaveCommandStatusType
{
  EZWAVECOMMANDSTATUS_TX = APPLICATION_INTERFACE_STATUS_ENUM_OFFSET,
  EZWAVECOMMANDSTATUS_GENERATE_RANDOM,
  EZWAVECOMMANDSTATUS_NODE_INFO,
  EZWAVECOMMANDSTATUS_LEARN_MODE_STATUS,
  EZWAVECOMMANDSTATUS_SET_DEFAULT,              // Received when protocol has finished starting up, and after receiving a set default command, has no content
  EZWAVECOMMANDSTATUS_INVALID_TX_REQUEST, // Received Tx Request that was not a EZWAVETRANSMITTYPE
  EZWAVECOMMANDSTATUS_INVALID_COMMAND,    // Receveid command that was not a EZWAVECOMMANDTYPE value
  EZWAVECOMMANDSTATUS_SET_RF_RECEIVE_MODE,
  EZWAVECOMMANDSTATUS_IS_NODE_WITHIN_DIRECT_RANGE,
  EZWAVECOMMANDSTATUS_GET_NEIGHBOR_COUNT,
  EZWAVECOMMANDSTATUS_ARE_NODES_NEIGHBOURS,
  EZWAVECOMMANDSTATUS_IS_FAILED_NODE_ID,
  EZWAVECOMMANDSTATUS_GET_ROUTING_TABLE_LINE,
  EZWAVECOMMANDSTATUS_SET_ROUTING_INFO,
  EZWAVECOMMANDSTATUS_STORE_NODE_INFO,
  EZWAVECOMMANDSTATUS_GET_PRIORITY_ROUTE,
  EZWAVECOMMANDSTATUS_SET_PRIORITY_ROUTE,
  EZWAVECOMMANDSTATUS_SET_SLAVE_LEARN_MODE,
  EZWAVECOMMANDSTATUS_SET_SLAVE_LEARN_MODE_RESULT,
  EZWAVECOMMANDSTATUS_IS_VIRTUAL_NODE,
  EZWAVECOMMANDSTATUS_GET_VIRTUAL_NODES,
  EZWAVECOMMANDSTATUS_GET_CONTROLLER_CAPABILITIES,
  EZWAVECOMMANDSTATUS_IS_PRIMARY_CTRL,
  EZWAVECOMMANDSTATUS_NETWORK_MANAGEMENT,
  EZWAVECOMMANDSTATUS_GET_BACKGROUND_RSSI,
  EZWAVECOMMANDSTATUS_AES_ECB,
  EZWAVECOMMANDSTATUS_REMOVE_FAILED_NODE_ID,
  EZWAVECOMMANDSTATUS_REPLACE_FAILED_NODE_ID,
  EZWAVECOMMANDSTATUS_NETWORK_LEARN_MODE_START,
  EZWAVECOMMANDSTATUS_ZW_SET_MAX_INCL_REQ_INTERVALS,
  EZWAVECOMMANDSTATUS_NVM_BACKUP_RESTORE,
  EZWAVECOMMANDSTATUS_PM_SET_POWERDOWN_CALLBACK,
  EZWAVECOMMANDSTATUS_ZW_GET_INCLUDED_NODES,
  EZWAVECOMMANDSTATUS_ZW_REQUESTNODENEIGHBORUPDATE,
  EZWAVECOMMANDSTATUS_ZW_INITIATE_SHUTDOWN,
  EZWAVECOMMANDSTATUS_ZW_GET_INCLUDED_LR_NODES,
  EZWAVECOMMANDSTATUS_ZW_GET_LR_CHANNEL,
  EZWAVECOMMANDSTATUS_UNUSED1,
  EZWAVECOMMANDSTATUS_UNUSED2,
  EZWAVECOMMANDSTATUS_ZW_GET_PTI_CONFIG,
  EZWAVECOMMANDSTATUS_ZW_SET_TX_ATTENUATION,

  /***********************************
   * SECURE notifications to app
   **********************************/
  EZWAVECOMMANDSTATUS_SECURE_ON_NODE_ADDED,    ///< The node is completely added using the ControllerPortable lib.
  EZWAVECOMMANDSTATUS_SECURE_ON_NODE_DELETED,  ///< The node is completely removed using the ControllerPortable lib.
  EZWAVECOMMANDSTATUS_SECURE_ON_NEW_NETWORK_ENTERED,              ///< Node entered a new network
  EZWAVECOMMANDSTATUS_SECURE_ON_NETWORK_MANAGEMENT_STATE_UPDATE,  ///< Secure network management changed state
  EZWAVECOMMANDSTATUS_SECURE_ON_NEW_SUC,                          ///< New SUC NodeID received
  EZWAVECOMMANDSTATUS_SECURE_ON_FRAME_TRANSMISSION,               ///< Frame transmission result
  EZWAVECOMMANDSTATUS_SECURE_ON_RX_FRAME_RECEIVED_INDICATOR,      ///< Frame received from NodeID indicator

  NUM_EZWAVECOMMANDSTATUS
} EZwaveCommandStatusType;

typedef enum EZwaveReceiveType
{
  EZWAVERECEIVETYPE_SINGLE = APPLICATION_INTERFACE_RECEIVE_ENUM_OFFSET,
  EZWAVERECEIVETYPE_MULTI,
  EZWAVERECEIVETYPE_NODE_UPDATE,
  EZWAVERECEIVETYPE_SECURITY_EVENT,
  EZWAVERECEIVETYPE_STAY_AWAKE,

  /******************************************
   * Events received from the SECURE module.
   *****************************************/
  EZWAVERECEIVETYPE_SECURE_FRAME_RECEIVED,

  NUM_EZWAVERECEIVETYPE
} EZwaveReceiveType;

typedef struct SNetworkUpdateRequest
{
  void            (*Handle)(void); // Placeholder for callback function
                                   // Will be returned with transmit status
                                   // Allows application to recognize frames
} SNetworkUpdateRequest;


typedef enum ELearnMode
{
  ELEARNMODE_DISABLED = 0,
  ELEARNMODE_CLASSIC = 1,
  ELEARNMODE_NETWORK_WIDE_INCLUSION = 2,
  ELEARNMODE_NETWORK_WIDE_EXCLUSION = 3
} ELearnMode;

/*****************************************************************************
 *                            TYPEDEF STRUCTURES
 ****************************************************************************/
typedef struct SNodeInfoRequest
{

  void            (*Handle)(void); // Placeholder for callback function    
                                   // Will be returned with transmit status
                                   // Allows application to recognize frames
  node_id_t DestNodeId;
} SNodeInfoRequest;

typedef struct SNodeInfo
{
  void            (*Handle)(void);  // Placeholder for callback function
                                    // Will be returned with transmit status
                                    // Allows application to recognize frames
  node_id_t DestNodeId;
  uint8_t TransmitOptions;
} SNodeInfo;


// Command structures START -------------------------------------------

typedef struct SCommandClassList_t
{
  uint8_t         iListLength;
  const uint8_t * pCommandClasses;
} SCommandClassList_t;

typedef struct SCommandClassSet_t
{
  SCommandClassList_t UnSecureIncludedCC;       /**< List of UNsecure supported command classes. Available when node is NOT included or UNsecure included */
  SCommandClassList_t SecureIncludedUnSecureCC; /**< List of UNsecure supported command classes in secure network. Available when node is secure included */
  SCommandClassList_t SecureIncludedSecureCC;   /**< List of Secure supported command classes. Available when node is secure included */
} SCommandClassSet_t;

typedef struct SCommandClassVersions
{
  uint8_t SecurityVersion;
  uint8_t Security2Version;
  uint8_t TransportServiceVersion;
} SCommandClassVersions;

// Generates true random word
typedef struct SCommandGenerateRandom
{
  uint8_t iLength;  // number of random bytes to generate
} SCommandGenerateRandom;

typedef struct SCommandNodeInfo
{
  node_id_t NodeId;
} SCommandNodeInfo;

typedef struct SCommandClearNetworkStatistics
{
  uint8_t Reserved;             // Not required set
} SCommandClearNetworkStatistics;

typedef struct SCommandSetLearnMode
{
  ELearnMode  eLearnMode;
  uint8_t useCB;
} SCommandSetLearnMode;

typedef struct SCommandSetSmartStartLearnMode
{
  E_NETWORK_LEARN_MODE_ACTION  eLearnMode;
} SCommandSetSmartStartLearnMode;

// Controller  API
typedef struct SSetSucNodeId
{
  void            (*Handle)(void); // Placeholder for callback function
                                   // Will be returned with transmit status
                                   // Allows application to recognize frames
  node_id_t SucNodeId;
  bool      bSucEnable;
  bool      bTxLowPower;
  uint8_t   Capabilities;         /* The capabilities of the new SUC */
} SSetSucNodeId;

typedef struct SCommandSetRfPowerLevel
{

    uint8_t powerLevelDBm;
} SCommandSetRfPowerLevel;

typedef struct SSendSucNodeId
{

  void            (*Handle)(void); // Placeholder for callback function
                                   // Will be returned with transmit status
                                   // Allows application to recognize frames
  node_id_t DestNodeId;
  uint8_t TransmitOptions;
} SSendSucNodeId;


typedef struct SCommandSetPromiscuousMode
{
  uint8_t Enable;
} SCommandSetPromiscuousMode;

typedef struct SAssignReturnRoute
{
  void            (*Handle)(void);      // Placeholder for callback function    
                                        // Will be returned with transmit status
                                        // Allows application to recognize frames

  node_id_t ReturnRouteReceiverNodeId;  // Routing slave to recieve route
  node_id_t RouteDestinationNodeId;     // Destination of route (if 0 destination will be self). Destination can be a SUC.
  uint8_t   aPriorityRouteRepeaters[4]; // Route to be assigned as priority route - set to zeroes to NOT supply a priority route (recommended)
  uint8_t   PriorityRouteSpeed;
  uint8_t   isSucRoute;
} SAssignReturnRoute;

typedef struct SCommandSetRfReceiveMode
{

  uint8_t mode;
} SCommandSetRfReceiveMode;

typedef struct SDeleteReturnRoute
{
  void            (*Handle)(void); // Placeholder for callback function
                                   // Will be returned with transmit status
                                   // Allows application to recognize frames

  node_id_t DestNodeId;            // Node to have its return routes deleted..
  bool bDeleteSuc;                 // Delete SUC return routes only, or Delete standard return routes only */
} SDeleteReturnRoute;

typedef struct SCommandGeniric8bParameter
{
  uint8_t value;
} SCommandGeniric8bParameter;

typedef struct SCommandGeniric16bParameter
{
  uint16_t value;
} SCommandGeniric16bParameter;

typedef struct SCommandAreNodesNeighbours
{
  uint8_t NodeA;
  uint8_t NodeB;
} SCommandAreNodesNeighbours;

typedef struct SCommandGetRoutingInfo
{
  node_id_t nodeID;
  uint8_t   options;
} SCommandGetRoutingInfo;

typedef struct SSendSlaveNodeInformation
{
  void            (*Handle)(void); // Placeholder for callback function    
                                   // Will be returned with transmit status
                                   // Allows application to recognize frames
  node_id_t sourceId;
  node_id_t destinationId;
  uint8_t txOptions;
} SSendSlaveNodeInformation;


typedef struct SCommandSetRoutingInfo
{
  uint8_t nodeID;
  uint8_t length;
  uint8_t nodeMask[MAX_NODEMASK_LENGTH];
} SCommandSetRoutingInfo;

typedef struct SCommandStoreNodeInfo
{
  uint8_t nodeID;
  uint8_t nodeInfo[6];
} SCommandStoreNodeInfo;

typedef struct SCommandStoreHomeID
{
  uint8_t homeID[HOMEID_LENGTH];
  uint8_t nodeID;
} SCommandStoreHomeID;

typedef struct SCommandSetPriorityRoute
{
  node_id_t nodeID;
  uint8_t   repeaters[MAX_REPEATERS];
  uint8_t   routeSpeed;
  uint8_t   clearGolden;
} SCommandSetPriorityRoute;

typedef struct SCommandAesEcb
{
  uint8_t key[16];
  uint8_t inputData[16];
} SCommandAesEcb;


typedef struct SCommandGetPriorityRoute
{
  uint8_t   *pPriRouteBuffer;              // Will be returned with transmit status
  node_id_t nodeID;
} SCommandGetPriorityRoute;

typedef struct SCommandFailedNodeIDCmd
{
  node_id_t nodeID;
  uint8_t   normalPower;      //Only for replaced failed node api
} SCommandFailedNodeIDCmd;

typedef struct SCommandPMSetPowerDownCallback {
   void (*callback)(void);
} SCommandPMSetPowerDownCallback;

typedef struct SCommandSetLBTThreshold {
    uint8_t channel;
    int8_t level;
}
SCommandSetLBTThreshold;

typedef struct SCommandSetMaxInclReqInterval {
    uint32_t inclusionRequestInterval;
} SCommandSetMaxInclReqInterval;

typedef struct SCommandNvmBackupRestore {
   uint32_t offset;
   uint32_t length;
   uint8_t  *nvmData;
} SCommandNvmBackupRestore;

typedef struct SCommandSetSecurityKeys
{
  uint8_t keys;
} SCommandSetSecurityKeys;

// Command structures END ---------------------------------------------

typedef struct SProtocolVersion
{
  uint8_t Major;
  uint8_t Minor;
  uint8_t Revision;
} SProtocolVersion;

typedef struct SProtocolInfo
{
  SCommandClassVersions CommandClassVersions; /**< Versions of Command Classes supplied  by protocol */
  SProtocolVersion      ProtocolVersion;      /**< Protocol version */
  EProtocolType         eProtocolType;
  ELibraryType          eLibraryType;         /*< Protocol library type */
} SProtocolInfo;

typedef struct SNetworkInfo
{
  EInclusionState_t   eInclusionState;
  node_id_t           SucNodeId;
  uint8_t             SecurityKeys;     // Which security keys the node has
  node_id_t           NodeId;
  uint32_t            HomeId;
  uint16_t            MaxPayloadSize;
} SNetworkInfo;

typedef struct SLongRangeInfo
{
  uint16_t  MaxLongRangePayloadSize;
} SLongRangeInfo;

typedef struct t_ExtNodeInfo
{
   NODEINFO  NodeInfo;
   uint8_t   extInfo;
} t_ExtNodeInfo;

typedef struct SRadioStatus
{
  int8_t iRadioPowerLevel;  // Radio power in db. 0 -> Max power.
} SRadioStatus;

typedef struct SApplicationHandles
{
  SQueueNotifying*                  pZwTxQueue;    /**< Notifying Queue object (contains FreeRTOS queue)
                                                - Queue for ZW frames from application to Protocol
                                                for transmission */

  QueueHandle_t                     ZwRxQueue;     /**< FreeRTOS Queue handle - Queue for ZW frames
                                                forwarded from protocol to application */

  SQueueNotifying*                  pZwCommandQueue; /**< Notifying Queue object (contains FreeRTOS queue)
                                                  - Queue for commands from Application to
                                                  protocol */

  QueueHandle_t                     ZwCommandStatusQueue;   /**< FreeRTOS Queue handle - Queue for status
                                                         replies from protocol to application
                                                         (status on commands from application to
                                                         protocol and Tx request from App to
                                                         protocol) */

  const zpal_radio_network_stats_t* pNetworkStatistics;  /**< Network statistics supplied by protocol */

  const SProtocolInfo*              pProtocolInfo;

  const SNetworkInfo*               pNetworkInfo;

  const SLongRangeInfo*             pLongRangeInfo;  /**< Only used by Controller nodes. Content is set to 0 on Slave nodes. */

  const SRadioStatus*               pRadioStatus;

} SApplicationHandles;

typedef struct SRadioConfig_t
{
  int8_t iListenBeforeTalkThreshold;  /**< Db (negative) or EListenBeforeTalkThreshold_t */
  zpal_tx_power_t iTxPowerLevelMax;   /**< Db (negative) or EtxPowerLevel_t */
  zpal_tx_power_t iTxPowerLevelAdjust;/**< Db (negative) or EtxPowerLevel_t */
  int16_t iTxPowerLevelMaxLR;         /**< Maximum transmission power for Z-Wave LR */
  zpal_radio_region_t eRegion;        /**< RF Region setting */
  uint8_t radio_debug_enable;         /**< Enable radio PTI */
} SRadioConfig_t;

typedef struct SAppNodeInfo_t
{
  uint8_t             DeviceOptionsMask;
  APPL_NODE_TYPE      NodeType;
  SCommandClassSet_t  CommandClasses;
} SAppNodeInfo_t;

typedef struct SVirtualSlaveNodeInfo_t
{
  node_id_t           NodeId;
  bool                bListening; // True if this node is always on air
  APPL_NODE_TYPE      NodeType;
  SCommandClassList_t CommandClasses;
} SVirtualSlaveNodeInfo_t;

typedef struct SVirtualSlaveNodeInfoTable_t
{
  uint8_t                         iTableLength;
  const SVirtualSlaveNodeInfo_t ** ppNodeInfo;  // Array of pointers to node info. Pointers may be NULL
} SVirtualSlaveNodeInfoTable_t;                 // This allows "nulling" a pointer while modifying a virtual slave node info
                                                // Or changing pointer to point to a different one.

// This struct content must be set up by application before enabling protocol (enabling radio)
// Direct content (the pointers) may not be changed runtime, but the data they point to can be edited by application run time
typedef struct SProtocolConfig_t
{
  const SVirtualSlaveNodeInfoTable_t *        pVirtualSlaveNodeInfoTable; // NULL is acceptable if no virtual slave nodes
  const uint8_t *                             pSecureKeysRequested;   // values are ref SECURITY_KEY_S2_ACCESS_BIT, ref SECURITY_KEY_S2_AUTHENTICATED_BIT, ref SECURITY_KEY_S2_UNAUTHENTICATED_BIT,  - consider making a bit field struct for it
  const SAppNodeInfo_t *                      pNodeInfo;
  const SRadioConfig_t *                      pRadioConfig;
} SProtocolConfig_t;

typedef struct STransmitFrameConfig
{
  /* Will be returned with transmit status
   * Allows application to recognize frames */
  void (*Handle) (uint8_t txStatus, TX_STATUS_TYPE* extendedTxStatus);
  uint8_t TransmitOptions;
  uint8_t iFrameLength;
  uint8_t aFrame[TX_BUFFER_SIZE];
} STransmitFrameConfig;

// Basis API
typedef struct SExploreInclusionRequest
{
  uint8_t Reserved;               // Not required set
} SExploreInclusionRequest;

typedef struct SExploreExclusionRequest
{
  uint8_t Reserved;               // Not required set
} SExploreExclusionRequest;

/**
 * Contains info related to sending an INIF.
 */
typedef struct SIncludedNodeInfo
{
  void (*Handle)(uint8_t, TX_STATUS_TYPE*); /**< Callback handle that Will be invoked with transmit status. */
}
SIncludedNodeInfo;

typedef struct STest
{
  void (*Handle)(uint8_t, TX_STATUS_TYPE*); // Will be returned with transmit status
                                            // Allows application to recognize frames
  node_id_t DestNodeId;
  uint8_t PowerLevel;
} STest;

// Transport API
typedef struct SSendData
{
  STransmitFrameConfig FrameConfig;
  node_id_t DestNodeId;
} SSendData;

typedef struct SSendDataEx
{
  STransmitFrameConfig FrameConfig;
  node_id_t DestNodeId;
  node_id_t SourceNodeId;
  uint8_t TransmitSecurityOptions;
  uint8_t TransmitOptions2;
  enum SECURITY_KEY eKeyType;
} SSendDataEx;

typedef struct SSendDataBridge
{
  STransmitFrameConfig FrameConfig;
  node_id_t DestNodeId;
  node_id_t SourceNodeId;
} SSendDataBridge;

typedef struct SSendDataMulti
{
  STransmitFrameConfig FrameConfig;
  NODE_MASK_TYPE NodeMask;
} SSendDataMulti;

typedef struct SSendDataMultiEx
{
  STransmitFrameConfig FrameConfig;
  uint8_t SourceNodeId;
  uint8_t GroupId;
  enum SECURITY_KEY eKeyType;
} SSendDataMultiEx;

typedef struct SSendDataMultiBridge
{
  STransmitFrameConfig FrameConfig;
  uint8_t NodeMask[MULTICAST_NODE_LIST_SIZE];
  node_id_t      SourceNodeId;
  bool lr_nodeid_list;
} SSendDataMultiBridge;

// Controller  API
typedef struct SCommandNetworkManagement
{
  void            (*pHandle)(void); // Placeholder for callback function
                                   // Will be returned with transmit status
                                   // Allows application to recognize frames
  uint8_t   mode;
  node_id_t nodeID;
} SCommandNetworkManagement;


typedef struct SSendReplication
{

  STransmitFrameConfig FrameConfig;
  node_id_t DestNodeId;           // Destination NodeId - Single cast only.
} SSendReplication;

typedef struct SSendReplicationReceiveComplete
{
  uint8_t Reserved;             // Not required set
} SSendReplicationReceiveComplete;

// Slave API
typedef struct SRequestNewRouteDestinations
{
  void    (*Handle)(void);      // Will be returned with transmit status
                                // Allows application to recognize frames
  uint8_t iDestinationCount;    // Number of new destinations
                                // Array containing new destinations
  uint8_t aNewDestinations[ZW_MAX_CACHED_RETURN_ROUTE_DESTINATIONS];                 // Will be returned with transmit status
} SRequestNewRouteDestinations;

typedef struct SCommandNetworkManagementDSK
{
  void           (*pHandle)(void); // Placeholder for callback function    
                                   // Will be returned with transmit status
                                   // Allows application to recognize frames
  uint8_t mode;
  uint8_t dsk[8];            // Dsk bytes from 0 to 7
} SCommandNetworkManagementDSK;

typedef struct SZWaveTransmitStatus
{
  void            (*Handle)(void); // Placeholder for callback function
  bool            bIsTxFrameLegal;  // False if frame rejected by protocol, can be due to content/configuration or due to timing (e.g. inclusion request when not in learn mode)
  uint8_t         TxStatus;
  TX_STATUS_TYPE  ExtendedTxStatus;
} SZWaveTransmitStatus;

typedef struct SZWaveGenerateRandomStatus
{
  uint8_t iLength;
  uint8_t aRandomNumber[32];
} SZWaveGenerateRandomStatus;

typedef struct SZWaveNodeInfoStatus
{
  node_id_t NodeId;
  t_ExtNodeInfo extNodeInfo;  // if NodeInfo.nodeType.generic = 0, node does not exist.
} SZWaveNodeInfoStatus;

typedef struct SZWaveLearnModeStatus
{
  ELearnStatus Status;   /* Status of learn mode */
} SZWaveLearnModeStatus;

typedef struct SZWaveInvalidTxRequestStatus
{
  uint8_t InvalidTxRequest;   /* Invalid value received (value should have been a valid EZWAVETRANSMITTYPE) */
} SZWaveInvalidTxRequestStatus;

typedef struct SZWaveInvalidCommandStatus
{
  EZwaveCommandType InvalidCommand;     /* Invalid value received (value should have been a valid EZWAVECOMMANDTYPE) */
} SZWaveInvalidCommandStatus;

typedef struct SZWaveGeneric8bStatus
{
  uint8_t result;     /* generic value of any API that uses a byte as a return value*/
} SZWaveGeneric8bStatus;

typedef struct SZWaveGenericBoolStatus
{

  bool result;     /* generic value of any API that uses a boolean as a return value*/
} SZWaveGenericBoolStatus;

typedef struct SCommandRequestNodeNeighborUpdate
{
  void            (*Handle)(void); // Placeholder for callback function
                                   // Will be returned with transmit status
                                   // Allows application to recognize frames

  node_id_t NodeId;                // Node to have its neighbors discovered..
} SCommandRequestNodeNeighborUpdate;


typedef struct SZWaveGetRoutingInfoStatus
{

  uint8_t RoutingInfo[MAX_NODEMASK_LENGTH];
} SZWaveGetRoutingInfoStatus;


typedef struct SCommandInitiateShutdown
{
  void (*Handle) (void);      // Placeholder for callback function

} SCommandInitiateShutdown;


typedef struct SCommandSetSecurityFlags
{
  node_id_t nodeID;
  bool nodeS2Capable;
  bool nodeS2Included;
  bool nodeSecureIncluded;
} SCommandSetSecurityFlags;


typedef struct SZWaveGetPriorityRouteStatus
{
  uint8_t bAnyRouteFound;
  uint8_t repeaters[MAX_REPEATERS];
  uint8_t routeSpeed;
} SZWaveGetPriorityRouteStatus;

typedef struct SZWaveSetPriorityRouteStatus
{
  uint8_t bRouteUpdated;
} SZWaveSetPriorityRouteStatus;

typedef struct SZWaveGetVirtualNodesStatus
{
  uint8_t vNodesMask[MAX_NODEMASK_LENGTH];
} SZWaveGetVirtualNodesStatus;

typedef struct SZWaveAesEcbStatus
{
  uint8_t outputData[16];
} SZWaveAesEcbStatus;

typedef struct SZWaveGetBackgroundRssiStatus
{
  int8_t rssi[NUM_CHANNELS_LR];
} SZWaveGetBackgroundRssiStatus;

typedef struct SZWaveNetworkManagementStatus
{
  void *pHandle;
  /*learn status can be sourceID, destinationID , data length and data can be up to NODEPARM_MAX*/
  uint8_t statusInfo[5 + NODEPARM_MAX];
} SZWaveNetworkManagementStatus;

typedef struct SNvmBackupRestoreStatus
{
  uint8_t status;
} SNvmBackupRestoreStatus;

typedef struct SZWaveGetIncludedNodes
{
  NODE_MASK_TYPE node_id_list;
} SZWaveGetIncludedNodes;

typedef struct SZWaveGetIncludedNodesLR
{
  LR_NODE_MASK_TYPE node_id_list;
} SZWaveGetIncludedNodesLR;

// Receive structures -----------------------------------

/**
*
* The Z Wave protocol MAY notify an application by sending
* \ref SReceiveNodeUpdate when a Node Information Frame has been received.
* The Z Wave protocol MAY refrain from sending the information if the protocol
* is currently expecting node information.
*
* A controller application MAY use the information provided by
* \ref SReceiveNodeUpdate to update local data structures.
*
* The Z Wave protocol MUST notify a controller application by calling
* \ref SReceiveNodeUpdate when a new node has been added or deleted
* from the controller through the network management features.
*
* The Z Wave protocol MUST pass \ref SReceiveNodeUpdate to application in
* response to \ref SNodeInfoRequest being passed to protocol by the controller
* application.
* The Z Wave protocol MAY notify a controller application by sending
* \ref SReceiveNodeUpdate when a Node Information Frame has been received.
* The Z Wave protocol MAY refrain from sending the information if the protocol
* is currently expecting a Node Information frame.
*
* \ref SReceiveNodeUpdate MUST be sent in a controller node operating
* as SIS each time a node is added or deleted by the primary controller.
* \ref SReceiveNodeUpdate MUST be sent in a controller node operating
* as SIS each time a node is added/deleted by an inclusion controller.
*
* A controller application MAY send a ZW_RequestNetWorkUpdate command
* to a SIS or SIS node. In response, the SIS MUST return update information
* for each node change since the last update handled by the requesting
* controller node.
* The application of the requesting controller node MAY receive multiple instances
* of \ref SReceiveNodeUpdate in response to application passing
* \ref SNetworkUpdateRequest to protocol.
*
* The Z Wave protocol MUST NOT send \ref SReceiveNodeUpdate in a
* controller node acting as primary controller or inclusion controller
* when a node is added or deleted.
*
* Any controller application MUST implement this function.
*
*
* \param[in] bNodeID
* \param[in] pCmd Pointer of the updated node's node info.
* \param[in] bLen The length of the pCmd parameter.
*
* serialapi{ZW->HOST: REQ | 0x49 | bStatus | bNodeID | bLen | basic | generic | specific | commandclasses[ ]}
*
* \ref SReceiveNodeUpdate via the Serial API also have the possibility for
* receiving the status UPDATE_STATE_NODE_INFO_REQ_FAILED, which means that a node
* did not acknowledge a \ref SNodeInfoRequest .
*
*/
typedef struct SReceiveNodeUpdate
{
  uint8_t Status;   // The status of the update process, value could be one of the following :
                    // \ref UPDATE_STATE_ADD_DONE A new node has been added to the network.
                    // \ref UPDATE_STATE_DELETE_DONE A node has been deleted from the network.
                    // \ref UPDATE_STATE_NODE_INFO_RECEIVED A node has sent its node info either unsolicited
                    // or as a response to a \ref NodeInfoRequest being passed to protocol.
                    // -\ref UPDATE_STATE_SUC_ID The SIS node Id was updated.

  node_id_t NodeId; // The updated node's node ID (1..232).
  uint8_t iLength;                          // length of aPayload.
  uint8_t aPayload[MAX_NODE_INFO_LENGTH]; // the updated node's node info.
} SReceiveNodeUpdate;


/* Used by protocol to request/inform Application
* of Security based Events.Currently only an event for Client Side Authentication(CSA)
* has been defined - E_APPLICATION_SECURITY_EVENT_S2_INCLUSION_REQUEST_DSK_CSA.
*
* \ref E_APPLICATION_SECURITY_EVENT_S2_INCLUSION_REQUEST_DSK_CSA Security Event
*   Is posted by protocol when in S2 inclusion with CSA enabled and the
*   Server side DSK is needed.
*   Application must call ZW_SetSecurityS2InclusionCSA_DSK(s_SecurityS2InclusionCSAPublicDSK_t *)
*   with the retrieved Server / Controller side DSK.
*/
typedef struct SReceiveSecurityEvent
{
  e_application_security_event_t Event; // Event type
  uint8_t iLength;                      // Length of security event payload
  uint8_t aEventData[48];               // Security Event payload
} SReceiveSecurityEvent;

#ifdef HOST_SECURITY_INCLUDED

/* Portable controller related definitions */
typedef uint8_t zwave_dsk_t[ZWAVE_DSK_LENGTH];
typedef uint8_t zwave_keyset_t;

/**
 * @brief Structure holding a Z-Wave node info frame.
 *
 */
typedef struct {
  /// This represents the first byte of the NIF, also known as "capability"
  /// It is composed of several subfields that can be read using the
  /// ZWAVE_NODE_INFO_LISTENING_PROTOCOL_* masks. For a complete field
  /// description, refer to @ref application_cc_spec, section Node Information Frame.
  uint8_t listening_protocol;

  /// This represents the first byte of the NIF, also known as "security"
  /// It is composed of several subfields that can be read using the
  /// ZWAVE_NODE_INFO_OPTIONAL_PROTOCOL_* masks. For a complete field
  /// description, refer to @ref application_cc_spec, section Node Information Frame.
  uint8_t optional_protocol;

  /// This field indicates the Basic Device Class of the actual node.
  /// The Basic Device Classes are listed in @ref device_class_spec.
  /// This byte is omitted by End Nodes and will be set to 0x00 in this case.
  uint8_t basic_device_class;

  /// This field indicates the Generic Device Class of the actual node.
  /// The Generic Device Classes are listed in @ref device_class_spec for
  /// Z-Wave and @ref device_type_spec_v2, @ref device_type_spec for Z-Wave Plus
  uint8_t generic_device_class;

  /// This field indicates the Specific Device Class of the actual node.
  /// The Specific Device Classes are listed in @ref device_class_spec for
  /// Z-Wave and @ref device_type_spec, @ref device_type_spec_v2 for Z-Wave Plus
  uint8_t specific_device_class;

  /// Length of the command class list
  uint8_t command_class_list_length;

  /// List of command classes supported by the device.
  /// This can be removed based on review comment. TODO
  uint16_t 
    command_class_list[ZWAVE_CONTROLLER_MAXIMUM_COMMAND_CLASS_LIST_LENGTH];
} zw_node_info_t;

typedef struct SSecureNodeAddedNotification
{
  uint32_t status;
  zw_node_info_t          nif;
  node_id_t               node_id;
  zwave_dsk_t             dsk;
  zwave_keyset_t          granted_keys;
  int32_t                 kex_fail_type;
  int32_t                 inclusion_protocol;
} SSecureNodeAddedNotification;


typedef struct SSecureNodeDeletedNotification
{
  node_id_t   node_id;
} SSecureNodeDeletedNotification;

typedef struct SSecureNodeEnteredNetwork
{
  uint32_t        home_id;
  node_id_t       node_id;
  zwave_keyset_t  granted_keys;
  int32_t         kex_fail_type;
} SSecureNodeEnteredNetwork;

typedef struct SSecureNetworkManagement
{
  uint32_t  state;
} SSecureNetworkManagement;

typedef struct SSecureNewSucId
{
  node_id_t   node_id;
} SSecureNewSucId;

typedef struct SSecureNodeFrameTransmission
{
  bool transmission_successful;
  TX_STATUS_TYPE tx_report;
  node_id_t node_id;
} SSecureNodeFrameTransmission;

typedef struct SSecureFrameReceivedIndicator
{
  node_id_t   node_id;
} SSecureFrameReceivedIndicator;

/**
 * Used by SECURE to notify application.
 */
typedef union {
  SSecureNodeAddedNotification     nodeAddedNotification;
  SSecureNodeDeletedNotification   nodeDeletedNotification;
  SSecureNodeEnteredNetwork        nodeEnteredNetwork;
  SSecureNetworkManagement         nodeNetworkManagement;
  SSecureNewSucId                  nodeNewSucId;
  SSecureNodeFrameTransmission     nodeFrameTransmission;
  SSecureFrameReceivedIndicator    nodeFrameReceivedIndicator;
}USecureAppNotification;

/*********************************************************
 * EZWAVETRANSMITTYPE_SECURE command parameter.
 ********************************************************/

typedef struct {
  union {
    /// Node Id
    node_id_t node_id;

    /// Multicast node-list to use when sending multicast messages.
    NODE_MASK_TYPE_ALL nodeList;
  } address;

  /// Endpoint Id. Set to 0 if a message is sent or received  without
  /// Multi Channel encapsulation
  /// Note that bit 7 is a multi-endpoint mask addressing. If bit 7 is set to 1
  /// the endpoint_id field MUST be interpreted as a bitmask, with bit 0
  /// representing endpoint 1, bit 1, representing endpoint 2, ...,
  /// bit 6 representing endpoint 7.
  uint8_t endpoint_id;

  /// This flag is set to true if it should be sent as a multicast frame.
  bool is_multicast;

} ZW_controller_endpoint_t;

/**
 * @brief Structure holding information about the source and destination
 * when transmitting and receiving Z-Wave frames.
 */
typedef struct {
  /// Local end of this connection, ie this device.
  /// Only the endpoint needs to be set!
  ZW_controller_endpoint_t local;

  /// remote end of this connection, ie not this device.
  ZW_controller_endpoint_t remote;

} ZW_controller_connection_info_t;

typedef struct {
  /// The underlying services will wait for the expected responses to a frame before
  /// transmitting the next frame. It will time out and resume TX operations after a
  /// recommended backoff time for the expected number of responses.
  /// If S2 uses the supervision mechanism, the number of expected responses should be
  /// equal or greater than 1.
  /// It is possible to specify that multiple responses are expected.
  /// In case of Supervision with status update flag, the number_of_responses should
  /// be set to 1, as only one frame will be returned immediately for sure, the other(s) one will
  /// come later after an arbitrary time.
  ///
  /// Generally, for GET commands, set to 1, for SET commands set to 0.
  uint8_t number_of_responses;

  /// Maximum time in ms this transmission is allowed to spend in queue waiting
  /// to be processed before it is dropped. Discard timeout of 0 means to never
  /// drop the frame. (It won't be dropped while in queue, but can be dropped due to
  /// other mechanisms.)
  uint32_t discard_timeout_ms;

  /// This flag indicates if the frame is to be sent as a test frame
  /// Test frame was intended to be used to test link reliability, the
  /// Z-Wave API will send a test frame without any routing and with 9600 kbit/s
  /// transmission speed. The payload will also be ignored.
  bool is_test_frame;

  /// This value indicates if the a test frame must be sent
  /// with a particular Tx Power. This value will be ignored if the
  /// is_test_frame flag is set to false.
  zpal_radio_tx_power_t rf_power;
} ZW_tx_options_t;

typedef struct {
  ZW_controller_connection_info_t  connection;
  ZW_tx_options_t                  tx_options;
  zwave_keyset_t                   tx_keys;
  uint16_t                         data_length;
  uint8_t                          data[TX_BUFFER_SIZE];
  void                             (*ptxCompleteCallback)(uint8_t, const TX_STATUS_TYPE*);
}SSecureSendData;
#endif  // HOST_SECURITY_INCLUDED

/*****************************************************************************
 *                             TYPEDEF UNIONS
 ****************************************************************************/

typedef union UTransmitParameters
{
  // Basis API
  SExploreInclusionRequest        ExploreInclusionRequest;
  SExploreExclusionRequest        ExploreExclusionRequest;
  SNetworkUpdateRequest           NetworkUpdateRequest;
  SNodeInfoRequest                NodeInfoRequest;
  SNodeInfo                       NodeInfo;
  SIncludedNodeInfo               IncludedNodeInfo;
  STest                           Test;
  // Transport API
  SSendData                       SendData;
  SSendDataEx                     SendDataEx;
  SSendDataBridge                 SendDataBridge;
  SSendDataMulti                  SendDataMulti;
  SSendDataMultiEx                SendDataMultiEx;
  SSendDataMultiBridge            SendDataMultiBridge;
  // Controller API
  SSetSucNodeId                   SetSucNodeId;
  SSendSucNodeId                  SendSucNodeId;
  SAssignReturnRoute              AssignReturnRoute;
  SDeleteReturnRoute              DeleteReturnRoute;
  SSendReplication                SendReplication;
  SSendReplicationReceiveComplete SendReplicationReceiveComplete;
  SSendSlaveNodeInformation       SendSlaveNodeInformation;
  // Slave API
  SRequestNewRouteDestinations    RequestNewRouteDestinations;
#ifdef HOST_SECURITY_INCLUDED
  /***************************************
   * Needed structures for APP data and
   * delivery to UNIFY.
   ***************************************/
  SSecureSendData                  SendDataParams;
#endif  // HOST_SECURITY_INCLUDED
} UTransmitParameters;

typedef union UCommandStatus
{
  SZWaveTransmitStatus          TxStatus;
  SZWaveGenerateRandomStatus    GenerateRandomStatus;
  SZWaveNodeInfoStatus          NodeInfoStatus;
  SZWaveLearnModeStatus         LearnModeStatus;
  SZWaveInvalidTxRequestStatus  InvalidTxRequestStatus;
  SZWaveInvalidCommandStatus    InvalidCommandStatus;
  SZWaveGeneric8bStatus         SetRFReceiveModeStatus;
  SZWaveGeneric8bStatus         IsNodeWithinDirectRange;
  SZWaveGeneric8bStatus         GetNeighborCountStatus;
  SZWaveGeneric8bStatus         AreNodesNeighborStatus;
  SZWaveGeneric8bStatus         IsFailedNodeIDStatus;
  SZWaveGeneric8bStatus         ReplaceFailedNodeStatus;
  SZWaveGetRoutingInfoStatus    GetRoutingInfoStatus;
  SZWaveGeneric8bStatus         SetRoutingInfoStatus;
  SZWaveGeneric8bStatus         StoreNodeInfoStatus;
  SZWaveGetPriorityRouteStatus  GetPriorityRouteStatus;
  SZWaveSetPriorityRouteStatus  SetPriorityRouteStatus;
  SZWaveGeneric8bStatus         SetSlaveLearnModeStatus;
  SZWaveGeneric8bStatus         IsVirtualNodeStatus;
  SZWaveGetVirtualNodesStatus   GetVirtualNodesStatus;
  SZWaveGeneric8bStatus         GetControllerCapabilitiesStatus;
  SZWaveGeneric8bStatus         IsPrimaryCtrlStatus;
  SZWaveAesEcbStatus            AesEcbStatus;
  SZWaveGetBackgroundRssiStatus GetBackgroundRssiStatus;
  SZWaveNetworkManagementStatus NetworkManagementStatus;
  SZWaveGeneric8bStatus         FailedNodeIDStatus;
  SNvmBackupRestoreStatus       NvmBackupRestoreStatus;
  SZWaveGenericBoolStatus       SetPowerDownCallbackStatus;
  SZWaveGetIncludedNodes        GetIncludedNodes;
  SZWaveGetIncludedNodesLR      GetIncludedNodesLR;
  SZWaveGeneric8bStatus         RequestNodeNeigborUpdateStatus;
  SZWaveGeneric8bStatus         InitiateShutdownStatus;
  SZWaveGeneric8bStatus         GetLRChannel;
  SZWaveGeneric8bStatus         GetPTIconfig;
  SZWaveGenericBoolStatus       SetTxAttenuation;

#ifdef HOST_SECURITY_INCLUDED
  /***************************************
   * Needed structures for SECURE data and
   * notification delivery to application.
   ***************************************/
  USecureAppNotification        USecureAppNotification;          // 163 bytes
#endif  // HOST_SECURITY_INCLUDED
} UCommandStatus;

typedef union UReceiveCmdPayload
{
  ZW_APPLICATION_TX_BUFFER rxBuffer;
  uint8_t padding[ZW_MAX_PAYLOAD_SIZE];
} UReceiveCmdPayload;

typedef union UCommandParameters
{
  SCommandGenerateRandom              GenerateRandom;
  SCommandNodeInfo                    NodeInfo;
  SCommandClearNetworkStatistics      ClearNetworkStatistics;
  SCommandSetLearnMode                SetLearnMode;
  SCommandSetSmartStartLearnMode      SetSmartStartLearnMode;
  SCommandSetPromiscuousMode          SetPromiscuousMode;
  SCommandSetRfReceiveMode            SetRfReceiveMode;
  SCommandGeniric8bParameter          IsNodeWithinDirectRange;
  SCommandGeniric8bParameter          GetNeighborCount;
  SCommandAreNodesNeighbours          AreNodesNeighbours;
  SCommandNetworkManagement           IsFailedNodeID;
  SCommandNetworkManagement           SetSlaveLearnMode;
  SCommandGetRoutingInfo              GetRoutingInfo;
  SCommandSetRoutingInfo              SetRoutingInfo;
  SCommandStoreNodeInfo               StoreNodeInfo;
  SCommandStoreHomeID                 StoreHomeID;
  SCommandGeniric8bParameter          LockRouteResponse;
  SCommandGetPriorityRoute            GetPriorityRoute;
  SCommandSetPriorityRoute            SetPriorityRoute;
  SCommandGeniric16bParameter         IsVirtualNode;
  SCommandGeniric8bParameter          SetRoutingMax;
  SCommandNetworkManagement           NetworkManagement;
  SCommandNetworkManagementDSK        NetworkManagementDSK;
  SCommandFailedNodeIDCmd             FailedNodeIDCmd;
  SCommandAesEcb                      AesEcb;
  SCommandPMSetPowerDownCallback      PMSetPowerDownCallback;
  SCommandGeniric8bParameter          UpdateCtrlNodeInformation;
  SCommandSetLBTThreshold             SetLBTThreshold;
  SCommandSetRfPowerLevel             SetRfPowerLevel;
  SCommandSetMaxInclReqInterval       SetMaxInclReqInterval;
  SCommandNvmBackupRestore            NvmBackupRestore;
  SCommandSetSecurityKeys             SetSecurityKeys;
  SCommandRequestNodeNeighborUpdate   RequestNodeNeighborUpdate;
  SCommandInitiateShutdown            InitiateShutdown;
  SCommandGeniric8bParameter          SetLRChannel;
  SCommandGeniric8bParameter          SetLRVirtualNodeIDs;
  SCommandGeniric8bParameter          SetTxAttenuation;
  SCommandSetSecurityFlags            SetSecurityFlags;
} UCommandParameters;

/**************************************************************************
 * Due to forward declaration issue, these structures and unions
 * must be placed here. They are all dependent on the unions above.
 *************************************************************************/

typedef struct SZwaveTransmitPackage
{
  EZwaveTransmitType eTransmitType;
  UTransmitParameters uTransmitParams;
} SZwaveTransmitPackage;

typedef struct SZwaveCommandStatusPackage
{
  EZwaveCommandStatusType eStatusType;
  UCommandStatus Content;
} SZwaveCommandStatusPackage;

typedef struct SZwaveCommandPackage
{
  EZwaveCommandType eCommandType;
  UCommandParameters uCommandParams;
} SZwaveCommandPackage;

// Receive structures ------------------------------

typedef struct SReceiveSingle
{
  UReceiveCmdPayload Payload;
  uint8_t iLength;
  RECEIVE_OPTIONS_TYPE RxOptions;
} SReceiveSingle;

typedef struct SReceiveMulti
{
  NODE_MASK_TYPE NodeMask;
  UReceiveCmdPayload Payload;
  uint8_t iCommandLength;
  RECEIVE_OPTIONS_TYPE RxOptions;
} SReceiveMulti;

// This one union is dependent on the structures above it.
typedef union UReceiveParameters
{
  SReceiveSingle Rx;
  SReceiveMulti RxMulti;
  SReceiveNodeUpdate RxNodeUpdate;
  SReceiveSecurityEvent RxSecurityEvent;
} UReceiveParameters;

typedef struct SZwaveReceivePackage
{
  EZwaveReceiveType eReceiveType;
  UReceiveParameters uReceiveParams;
} SZwaveReceivePackage;


// Each item type to be put on queues between app and protocol, has been given a value range for ID enums.
// They do not overlap, as this makes it easy to detect if an item has errournously been put on the wrong queue.
STATIC_ASSERT(NUM_EZWAVETRANSMITTYPE < (APPLICATION_INTERFACE_TRANSMIT_ENUM_OFFSET + (APPLICATION_INTERFACE_COMMAND_ENUM_OFFSET)),
    STATIC_ASSERT_FAILED_interface_tx_enum_overlap);
STATIC_ASSERT(NUM_EZWAVECOMMANDTYPE < (APPLICATION_INTERFACE_COMMAND_ENUM_OFFSET + (APPLICATION_INTERFACE_RECEIVE_ENUM_OFFSET)),
    STATIC_ASSERT_FAILED_interface_command_enum_overlap);
STATIC_ASSERT(NUM_EZWAVERECEIVETYPE < (APPLICATION_INTERFACE_RECEIVE_ENUM_OFFSET + (APPLICATION_INTERFACE_STATUS_ENUM_OFFSET)),
    STATIC_ASSERT_FAILED_interface_receive_enum_overlap);
STATIC_ASSERT(NUM_EZWAVECOMMANDSTATUS < (APPLICATION_INTERFACE_STATUS_ENUM_OFFSET + 0x40),
    STATIC_ASSERT_FAILED_interface_status_enum_overlap);



#endif /* _ZW_APPLICATION_TRANSPORT_INTERFACE_H_ */
