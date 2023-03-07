/**
 * @file
 * Helper module for Command Class Association Group Information.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <agi.h>
#include "config_app.h"
#include <association_plus.h>
#include <misc.h>
#include <string.h>
#include <CC_AssociationGroupInfo.h>
#include <ZAF_Common_interface.h>
#include "ZW_TransportSecProtocol.h"
#include "SizeOf.h"
#include "ZAF_CC_Invoker.h"
#include "Assert.h"

//#define DEBUGPRINT
#include "DebugPrint.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef struct _AGI_TABLE_EP_
{
  AGI_GROUP* pTable;
  uint8_t tableSize;
} AGI_TABLE_EP;

typedef struct _AGI_TABLE_
{
  AGI_TABLE_EP tableEndpoint[NUMBER_OF_ENDPOINTS + 1];
} AGI_TABLE;

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

static AGI_TABLE myAgi;
static uint8_t m_lastActiveGroupId = 1;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.
/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

void
AGI_Init(void)
{
  memset((uint8_t *)&myAgi, 0x00, sizeof(myAgi));

  m_lastActiveGroupId = 1;
}

void AGI_ResourceGroupSetup(AGI_GROUP const * const pTable, uint8_t tableSize, uint8_t endpoint)
{
  ASSERT(NULL != pTable);
  ASSERT(0 < tableSize);

  if (NUMBER_OF_ENDPOINTS >= endpoint)
  {
    myAgi.tableEndpoint[endpoint].tableSize = tableSize;
    myAgi.tableEndpoint[endpoint].pTable = (AGI_GROUP *)pTable;
  }
}

size_t
CC_AGI_groupNameGet_handler(
                 char * pGroupName,
                 uint8_t groupId,
                 uint8_t endpoint)
{
  size_t nameLength = 0;
  char * pSourceGroupName;
  const char errorText[] = "Invalid group";

  if ((IS_NULL(pGroupName)) || (NUMBER_OF_ENDPOINTS < endpoint))
  {
    return nameLength;
  }

  // tableSize does not include Lifeline. Hence, +1.
  if ((groupId > (myAgi.tableEndpoint[endpoint].tableSize + 1)) || (0 == groupId))
  {
    pSourceGroupName = (char *)errorText;
  }
  else
  {
    if (1 == groupId)
    {
      char lifelineText[] = "Lifeline";
      nameLength = strlen(lifelineText);
      memcpy(pGroupName, lifelineText, nameLength);
      return nameLength;
    }
    else
    {
      /*
       * myAgi.tableEndpoint[endpoint].pTable represents all groups not being lifeline groups.
       * Hence, the lowest possible group is 2 since lifeline is group 1.
       * the first index in array = given group - 2.
       */
      pSourceGroupName = myAgi.tableEndpoint[endpoint].pTable[groupId - 2].groupName;
    }
  }
  nameLength = strlen(pSourceGroupName);
  memcpy(
         (uint8_t * )pGroupName,
         (uint8_t * )pSourceGroupName,
         nameLength);

  return nameLength;
}

uint8_t CC_AGI_groupCount_handler(uint8_t endpoint)
{
  if (NUMBER_OF_ENDPOINTS < endpoint)
  {
    return 0; /** Error!!*/
  }

  if (ZAF_GetInclusionMode() == EINCLUSIONMODE_ZWAVE_LR)
  {
      return 1; // Lifeline group and no other groups. (Z-Wave LR only supports lifeline group.)
  }

  return 1 + myAgi.tableEndpoint[endpoint].tableSize; /* Lifeline group + grouptable size.*/
}

void CC_AGI_groupInfoGet_handler(
                 uint8_t groupId,
                 uint8_t endpoint,
                 VG_ASSOCIATION_GROUP_INFO_REPORT_VG* report)
{
  ZAF_CC_AGI_CorrectGroupIdIfInvalid(endpoint, &groupId);

  report->groupingIdentifier = groupId;

  if (1 == groupId)
  {
    report->profile1 = ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL; /* MSB */
    report->profile2 = ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL_LIFELINE; /* LSB */
  }
  else
  {
    report->profile1 = myAgi.tableEndpoint[endpoint].pTable[groupId - 2].profile.profile_MS;
    report->profile2 = myAgi.tableEndpoint[endpoint].pTable[groupId - 2].profile.profile_LS;
  }
}

typedef struct
{
  CMD_CLASS_LIST * pCmdClassList;
  uint8_t ** ppGroupList;
}
context_t;

/**
 * Invokes the CC function that adds one or more CC / C pairs.
 *
 * This function will be invoked for each linked CC.
 *
 * @param[in] p_cc_entry Given CC entry.
 * @param[in,out] context Context containing the list to which the CC / C pair must be added.
 * @return Always returns false.
 */
static bool cc_callback(CC_handler_map_latest_t const * const p_cc_entry, zaf_cc_context_t context)
{
  // Continue on invalid CC.
  if (0xFF == p_cc_entry->CC) {
    return false;
  }
  // Continue if CC is not required to report via Lifeline.
  if (NULL == p_cc_entry->lifeline_report_get) {
    return false;
  }
  // Loop through each CC supported by the given endpoint (or root device).
  context_t * p_context = context;
  for ( uint8_t i = 0; i < p_context->pCmdClassList->size; i++) {

    // Check if there is a match between the two lists.
    if (p_context->pCmdClassList->pList[i] == p_cc_entry->CC) {
      // Match found!

      // Call the current CCs lifeline report function that will fill out the group list and increase the group list size.
      uint8_t size_add = p_cc_entry->lifeline_report_get((ccc_pair_t *)*(p_context->ppGroupList));
      *p_context->ppGroupList += 2 * size_add;
    }
  }

  return false;
}

bool GetApplGroupCommandList(
                             uint8_t * pGroupList,
                             size_t * pGroupListSize,
                             uint8_t groupId,
                             uint8_t endpoint)
{
  if (NUMBER_OF_ENDPOINTS < endpoint)
  {
    return false; /** Error!!*/
  }

  if (1 == groupId)
  {
    uint8_t * pGroupListStart = pGroupList;
    *pGroupListSize = 0;
    CMD_CLASS_LIST *pCmdClassList  = NULL; // -> struct

    /*
     * Invoke GetCommandClassList() setting "included" to true because this code will not be
     * processed if the node is not included.
     * Pass the highest granted key because this code will only be processed if the frame was
     * received with highest key.
     */
    pCmdClassList = GetCommandClassList(true, GetHighestSecureLevel(ZAF_GetSecurityKeys()), endpoint);
    ASSERT(NULL != pCmdClassList);  // FATAL: Empty CC list. We should not have reached this point. Unlikely scenario!

    context_t context = {
                         .pCmdClassList = pCmdClassList,
                         .ppGroupList = &pGroupList
    };

    ZAF_CC_foreach(cc_callback, (zaf_cc_context_t)&context);

    *pGroupListSize = (size_t)(pGroupList - pGroupListStart);
  }
  else
  {
    *pGroupListSize = sizeof(cc_group_t) * myAgi.tableEndpoint[endpoint].pTable[groupId - 2].cmdGrpSize;
    memcpy(pGroupList, (uint8_t* )&myAgi.tableEndpoint[endpoint].pTable[groupId - 2].cmdGrp,
           *pGroupListSize);

  }
  return true;
}

uint8_t
ApplicationGetLastActiveGroupId(void)
{
  return m_lastActiveGroupId;
}

/**
 * Returns whether the command class of a given command class / command pair occurs in a given list
 * of command class / command pairs.
 * @param cmdGrp Command class / command pair containing the command class to be found.
 * @param pCmdGrpList Pointer to an array of command class / command pairs.
 * @param listSize Number of items in the array.
 * @return Returns true if the command class is found and false otherwise.
 */
static bool SearchCmdClass(cc_group_t cmdGrp,
                    cc_group_t* pCmdGrpList,
                    uint8_t listSize)
{
  for (; listSize > 0; listSize--) {
    if (cmdGrp.cmdClass == pCmdGrpList[listSize - 1].cmdClass) {
      return true;
    }
  }
  return false;
}

/**
 * Returns the association group ID that matches the AGI profile, command class/command-pair to
 * be found in the given AGI table.
 * @param profile Profile to search for.
 * @param cmdGrp Command class / command pair to search for.
 * @param pAgiTable AGI table that must be searched.
 * @return Returns the group ID, if found. Otherwise 0xFF.
 */
static uint8_t SearchProfile(AGI_PROFILE profile,
                             cc_group_t cmdGrp,
                             AGI_TABLE_EP* pAgiTable)
{
  uint8_t grpId = 0;

  for (grpId = 0; grpId < pAgiTable->tableSize; grpId++)
  {
    /*Find profile*/
    if ((profile.profile_MS == pAgiTable->pTable[grpId].profile.profile_MS) &&
        (profile.profile_LS == pAgiTable->pTable[grpId].profile.profile_LS))
    {
      for (uint8_t cmdGrpId = 0; cmdGrpId < pAgiTable->pTable[grpId].cmdGrpSize; cmdGrpId++)
      {
        if ( true == SearchCmdClass(cmdGrp, &pAgiTable->pTable[grpId].cmdGrp[cmdGrpId], 1))
        {
          // Add 2 to get the actual group ID in terms of specifications.
          return grpId + 2;
        }
      }
    }
  }
  return 0xFF;
}

/*
 * If the given AGI profile is the lifeline profile, the function will check if the given command
 * class / command pair is found in the lifeline list of command class / command pairs.
 * If it is found, it will request the list of nodes from the Association CC and return the TX
 * options.
 *
 * If the given AGI profile is something else than the lifeline profile, it will try to find the
 * AGI group that matches the given profile, command class / command pair and source endpoint.
 * If found, it will request the list of nodes from the Association CC and return the TX options.
 */
TRANSMIT_OPTIONS_TYPE_EX * ReqNodeList(
                                       AGI_PROFILE const * const pProfile,
                                       cc_group_t const * const pCurrentCmdGrp,
                                       const uint8_t sourceEndpoint)
{
  NODE_LIST_STATUS status;
  uint8_t grpId = 0;
  static TRANSMIT_OPTIONS_TYPE_EX txOptions;
  txOptions.txOptions = ZWAVE_PLUS_TX_OPTIONS;
  txOptions.sourceEndpoint = sourceEndpoint;

  if (NUMBER_OF_ENDPOINTS < sourceEndpoint)
  {
    return NULL; /** Error!!*/
  }

  // Used for LIFELINE transmission.

  if (ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL == pProfile->profile_MS &&
      ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL_LIFELINE == pProfile->profile_LS)
  {
    /*endpoint is always 0 for lifeline!!*/
    status = handleAssociationGetnodeList(LIFELINE_GROUP_ID,
                                          0,
                                          &(txOptions.pList),
                                          &(txOptions.list_length));
    if (status != NODE_LIST_STATUS_SUCCESS)
    {
      return NULL;
    }

    // Found one or more associations in the Lifeline association group.
    m_lastActiveGroupId = LIFELINE_GROUP_ID;
    txOptions.S2_groupID = LIFELINE_GROUP_ID + (uint8_t)(sourceEndpoint << 4);
    return &txOptions;
  }

  // Used for other profiles transmission other than lifeline.

  grpId = SearchProfile(*pProfile, *pCurrentCmdGrp, &myAgi.tableEndpoint[sourceEndpoint]);

  if (0xFF != grpId)
  {
    status = handleAssociationGetnodeList(grpId,
                                          sourceEndpoint,
                                          &(txOptions.pList),
                                          &(txOptions.list_length));
    if (status != NODE_LIST_STATUS_SUCCESS)
    {
      return NULL;
    }

    m_lastActiveGroupId = grpId;
    txOptions.S2_groupID = grpId + (uint8_t)(sourceEndpoint << 4);

    return &txOptions;
  }

  return NULL;
}


void ZAF_CC_AGI_CorrectGroupIdIfInvalid(uint8_t endpoint, uint8_t * pGroupId)
{
  /*
   * tableSize counts the number of groups excluding lifeline.
   * Hence, for an endpoint with 3 groups (group 2-4), the tableSize will be 3.
   * Since groups are one indexed, group 4 would be treated as an invalid group unless we add one
   * to tableSize before checking.
   *
   * 4 > 3 + 1 = false => group 4 is valid
   * 5 > 3 + 1 = true  => group 5 is invalid
   */
  if ((0 == *pGroupId) || (*pGroupId > myAgi.tableEndpoint[endpoint].tableSize + 1))
  {
    *pGroupId = 1;
  }
}
