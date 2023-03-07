/**
 * @file
 * Helper module for Command Class Association Group Information.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _AGI_H_
#define _AGI_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <ZW_TransportEndpoint.h>

/****************************************************************************/
/*                              CONFIGURATIONS                              */
/****************************************************************************/

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

#define AGI_GROUP_NAME_LEN                  (42 + 1)  // + 1 to account for the needed termination character.

/**
 * Enum type NODE_LIST_STATUS is used for return status on API call AGI_NodeIdListGetNext.
 * Enum types from NODE_LIST_STATUS_SUCCESS to NODE_LIST_STATUS_ERROR_LIST deliver status
 * on the call and after NODE_LIST_STATUS_ERROR_LIST deliver an error identifiers pointing
 * to a problem in application AGI/association configuarion.
 */
typedef enum
{
  NODE_LIST_STATUS_SUCCESS = 0,
  NODE_LIST_STATUS_NO_MORE_NODES,
  NODE_LIST_STATUS_ASSOCIATION_LIST_EMPTY,
  NODE_LIST_STATUS_ERROR_LIST,              /**< enum values higher than this is error identifiers*/
  NODE_LIST_STATUS_ERR_NO_TABLE_ENDPOINT,
  NODE_LIST_STATUS_ERR_UNKNOWN_PROFILE,
  NODE_LIST_STATUS_ERR_ENDPOINT_OUT_OF_RANGE,
  NODE_LIST_STATUS_ERR_GROUP_NBR_NOT_LEGAL,
  NODE_LIST_STATUS_ERR_LIFELINE_PROFILE_NOT_SUPPORTED,
  NODE_LIST_STATUS_ERR_LIFELINE_SUPPORT_NOT_CC_BASIC,
  NODE_LIST_STATUS_ERR_PROFILE_LIFELINE_ONLY_SUPPORT_IN_GRP_1,
} NODE_LIST_STATUS;

/**
 * NODE_LIST is used to control current AGI profile job.
 */
typedef struct _NODE_LIST_
{
  uint8_t sourceEndpoint;       /**< Active endpoint handling AGI profile job */
  MULTICHAN_NODE_ID* pNodeList; /**< pointer to the node list for AGI profile group */
  uint8_t len;                  /**< Length of the node list */
} NODE_LIST;

/**
 * Structure for an AGI group including profile, command class groups and group name.
 */
typedef struct _AGI_GROUP_
{
  AGI_PROFILE   profile;
  uint8_t       cmdGrpSize;           /**< Number of AGI Profile cmd classes in cmdGrp */
  CMD_CLASS_GRP cmdGrp[2];            /**< AGI Profile cmd class group (size = maximum number of command classes required per group) */
  char groupName[AGI_GROUP_NAME_LEN]; /**< AGI Profile group-name UTF-8 format */
} AGI_GROUP;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * Initializes all AGI parameters. This MUST be called before calling other AGI functions.
 */
//@ [AGI_Init]
void AGI_Init(void);
//@ [AGI_Init]

/**
 * Setup AGI Lifeline command classes and commands for each endpoint.
 *
 * @deprecated  This function has no effect. The information previously configured by this
 *              function will be reported automatically by ZAF. The function is marked for removal
 *              in a later release.
 */
static inline void CC_AGI_LifeLineGroupSetup(
    cc_group_t const * const pCmdGrpList,
    uint8_t listSize,
    uint8_t endpoint)
{
  UNUSED(pCmdGrpList);
  UNUSED(listSize);
  UNUSED(endpoint);
}

 /**
 * Sets up the AGI table of groups for a given endpoint (or root device).
 * @param[in] pTable[]  AGI table for one endpoint.
 * @param[in] tableSize Number of groups in table.
 * @param[in] endpoint  Device endpoint number for the AGI table.
 *                      Endpoint numbers 0 is treated as 1 (lifeline).
 */
//@ [AGI_ResourceGroupSetup]
void AGI_ResourceGroupSetup(
    AGI_GROUP const * const pTable,
    uint8_t tableSize,
    uint8_t endpoint);
//@ [AGI_ResourceGroupSetup]

/**
 * Returns TX options containing a list of nodes found in the association group that matches the
 * given AGI profile, command class / command pair and source endpoint.
 *
 * This function must be called to construct the TX options for Transport_SendRequestEP().
 * @param[in] pProfile Pointer to AGI profile.
 * @param[in] pCurrentCmdGrp Pointer to command class / command pair.
 * @param[in] sourceEndpoint The endpoint for which the nodes are associated.
 * @return transmit option pointer of type TRANSMIT_OPTIONS_TYPE_EX. Return NULL if something vent
 * wrong.
 */
TRANSMIT_OPTIONS_TYPE_EX * ReqNodeList(
                                       AGI_PROFILE const * const pProfile,
                                       cc_group_t const * const pCurrentCmdGrp,
                                       const uint8_t sourceEndpoint);

#endif /* _AGI_H_ */
