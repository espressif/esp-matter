/**
 * @file
 * Handler for Command Class Association.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _CC_ASSOCIATION_H_
#define _CC_ASSOCIATION_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <ZW_classcmd.h>
#include "ZAF_types.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * @brief Returns the latest used association group.
 * @return Latest used association group.
 */
extern uint8_t ApplicationGetLastActiveGroupId(void);

/**
 * @brief Returns the maximum number of nodes that can be stored in a given association group for
 * a given endpoint.
 * @param[in] groupIden A given Group ID.
 * @param[in] ep A given endpoint.
 * @return The maximum number of nodes.
 */
extern uint8_t handleGetMaxNodesInGroup(
    uint8_t groupIden,
    uint8_t ep);

/**
 * @brief Handler for Association Set command.
 * @param[in] ep A given endpoint.
 * @param[in] pCmd A command containing the nodes to save in the association database.
 * @param[in] cmdLength Length of the command.
 * @param[in] commandClass Caller command class. This function may be called from multiple command classes.
 *            This parameter tells which one was it.
 * @return command handler return code
 */
extern e_cmd_handler_return_code_t handleAssociationSet(
    uint8_t ep,
    ZW_MULTI_CHANNEL_ASSOCIATION_SET_1BYTE_V2_FRAME* pCmd,
    uint8_t cmdLength,
    uint8_t commandClass);

#endif // _CC_ASSOCIATION_H_
