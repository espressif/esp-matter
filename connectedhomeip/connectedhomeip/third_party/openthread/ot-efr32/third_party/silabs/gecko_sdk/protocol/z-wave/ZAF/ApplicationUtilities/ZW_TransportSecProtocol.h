/**
 * @file
 * Implements parts of the Z-Wave Application Framework Transport Layer.
 * @copyright 2018 Silicon Laboratories Inc.
 */
#ifndef _TRANSPORT_SEC_PROTOCOL_H_
#define _TRANSPORT_SEC_PROTOCOL_H_


/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <ZW_basis_api.h>
#include <ZW_transport_api.h>
#include <ZW_TransportEndpoint.h>
#include <ZW_application_transport_interface.h>

/****************************************************************************/
/*                       PUBLIC TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * Specifies the basic node information also contained in the Node Information Frame.
 *
 * The descriptions below of each of the CC lists covers the endpoint CC lists as well.
 */
typedef struct
{
  /**
   * Must contain the CCs supported by the node when it's added non-securely to a network or when
   * it's not part of a Z-Wave network.
   *
   * CC Z-Wave+ Info must be the first CC in this list.
   */
  uint8_t *cmdClassListNonSecure;

  /**
   * Must contain the number of CCs in cmdClassListNonSecure.
   */
  uint8_t cmdClassListNonSecureCount;

  /**
   * This is a partial list of CCs. It must contain the CCs that are supported non-securely when
   * the node is added securely to a network. CCs in this list MUST NOT overlap with CCs in
   * cmdClassListSecure.
   *
   * CC Z-Wave+ Info must be the first CC in this list.
   */
  uint8_t *cmdClassListNonSecureIncludedSecure;

  /**
   * Must contain the number of CCs in cmdClassListNonSecureIncludedSecure.
   */
  uint8_t cmdClassListNonSecureIncludedSecureCount;

  /**
   * This is a partial list of CCs. It must contain the CCs that are supported securely when the
   * node is added securely to a network. CCs in this list MUST NOT overlap with CCs in
   * cmdClassListNonSecureIncludedSecure.
   */
  uint8_t *cmdClassListSecure;

  /**
   * Must contain the number of CCs in cmdClassListSecure.
   */
  uint8_t cmdClassListSecureCount;

  /**
   * This field must tell whether the application is non-listening, listening or frequently
   * listening. The values can be found in ZW_basic_api.h.
   */
  uint8_t deviceOptionsMask;

  /**
   * Specifies the generic and specific type of the node.
   */
  APPL_NODE_TYPE nodeType;
}
app_node_information_t;

/**
 * For backwards compatibility.
 */
typedef app_node_information_t APP_NODE_INFORMATION;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * Max number of command classes a command class list can contain.
 */
#define APPL_NODEPARM_MAX 35

/****************************************************************************/
/*                           IMPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * Called when the frame is received
 * @param pCmd  Payload from the received frame, the command is the very first byte
 * @param cmdLength Number of command bytes including the command
 * @param rxOpt Struct to use (may be partially filled out if parsing encapsulated command)
 */
extern void
Transport_ApplicationCommandHandler(
    ZW_APPLICATION_TX_BUFFER *pCmd,
    uint8_t cmdLength,
    RECEIVE_OPTIONS_TYPE *rxOpt);


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * Callback function triggered by Cmd Publisher when the frame is received from protocol
 * See ZAF_CmdPublisher.h for more details
 *
 * @param pSubscriberContext A handle to the Transport layer
 * @param pRxPackage Received package containing: payload from received frame, number of command bytes
 *                   (including the command) and rxOpt struct
 */
void
ApplicationCommandHandler(void *pSubscriberContext, SZwaveReceivePackage* pRxPackage);

/**
 * Initializes the ZAF transport layer.
 *
 * It must be invoked by the application on startup.
 *
 * @param pAppNode Pointer to a struct holding information about the node.
 * @param updateStayAwakePeriodFunc Function that will be invoked after each call
 * to a command handler and after learn mode completion. Used by battery operated
 * devices to set the period to stay awake after those activities.
 * If mains powered, set to NULL.
 * @return true if transport layer is initialized, false otherwise.
 */
uint8_t
Transport_OnApplicationInitSW(
  app_node_information_t * pAppNode,
  void (*updateStayAwakePeriodFunc)(void));

/**
 * @brief Register the new node id in the transport layer after learn mode completes.
 *
 * This function must be called in LearnCompleted application function
 *
 * @deprecated Use ZAF_Transport_OnLearnCompleted().
 *
 * @param nodeID The newly assigned node id.
 * @return true on success.
*/
uint8_t
Transport_OnLearnCompleted(node_id_t nodeID);

/**
 * Updates the active NIF depending on learn mode result.
 *
 * Must be invoked when learn mode has completed (on inclusion/exclusion).
 */
void ZAF_Transport_OnLearnCompleted(void);

/**
 * Returns whether a given command is supported.
 *
 * @param commandClass Command class to check for.
 * @param command Command to check for.
 * @param eKey Security key that the frame was received with.
 * @return Returns true if the command is supported and false otherwise.
 */
bool
TransportCmdClassSupported(uint8_t commandClass,
                           uint8_t command,
                           enum SECURITY_KEY eKey);

/**
 * @brief  Get highest secure level
 * @param protocolSecBits secure bits
 * @return secure level
 */
enum SECURITY_KEY
GetHighestSecureLevel(
    uint8_t protocolSecBits);

/**
 * @brief Get command class list from device
 * @param[in] included boolean value for node is included
 * @param[in] eKey security key of type security_key_t
 * @param[in] endpoint 0 - root 1 - X endpoint number
 */
CMD_CLASS_LIST*
GetCommandClassList(
    bool included,
    security_key_t eKey,
    uint8_t endpoint);

#endif /*_TRANSPORT_SEC_PROTOCOL_H_*/
