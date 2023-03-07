/**
* @file
* Implements functions for transporting frames over the native Z-Wave Network
* @copyright 2018 Silicon Laboratories Inc.
*/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_TransportSecProtocol.h>
#include <ZAF_command_class_utils.h>
#include <ZW_transport_api.h>
#include <ZAF_Common_interface.h>
#include <ZAF_tx_mutex.h>

//#define DEBUGPRINT
#include "DebugPrint.h"
/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef struct _ST_TRANSPORT_NODE_INFORMATION_
{
  union
  {
    app_node_information_t *pNifs;
    CMD_CLASS_LIST_3_LIST *pCmdLists;
  }u;
  CMD_CLASS_LIST activeNonsecureList;
} TRANSPORT_NODE_INFORMATION;


/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/
#ifdef ZW_CONTROLLER
uint8_t SECURITY_KEY = 0;
#endif

static TRANSPORT_NODE_INFORMATION m_AppInfo =
{
  {(app_node_information_t*)NULL},
  {(uint8_t*)NULL, 0} /**> CMD_CLASS_LIST data*/
};

static uint8_t cmd_class_buffer[APPL_NODEPARM_MAX];

static VOID_CALLBACKFUNC(cbUpdateStayAwakePeriod)();

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/


static void SetupActiveNIF(void);

uint8_t
Transport_OnApplicationInitSW(
    app_node_information_t* pAppNode,
  void (*updateStayAwakePeriodFunc)(void))
{
  m_AppInfo.u.pNifs = pAppNode;
  cbUpdateStayAwakePeriod = updateStayAwakePeriodFunc;

  SetupActiveNIF();

  DPRINT("SecAppInit\r\n");

#ifndef SERIAL_API_APP
  mutex_init();
#endif

  return true;
}

/**
 * @brief Get highest secure level
 */
enum SECURITY_KEY
GetHighestSecureLevel(uint8_t protocolSecBits)
{
  if(SECURITY_KEY_S2_ACCESS_BIT & protocolSecBits)
  {
    return SECURITY_KEY_S2_ACCESS;
  }
  else if(SECURITY_KEY_S2_AUTHENTICATED_BIT & protocolSecBits)
  {
    return SECURITY_KEY_S2_AUTHENTICATED;
  }
  else if(SECURITY_KEY_S2_UNAUTHENTICATED_BIT & protocolSecBits)
  {
    return SECURITY_KEY_S2_UNAUTHENTICATED;
  }
  else if(SECURITY_KEY_S0_BIT & protocolSecBits)
  {
    return SECURITY_KEY_S0;
  }

  return SECURITY_KEY_NONE;
}



CMD_CLASS_LIST*
GetCommandClassList(
    bool included,
    security_key_t eKey,
    uint8_t endpoint)
{
  static CMD_CLASS_LIST cmd_class_list;
  static CMD_CLASS_LIST* pCmdClassList = NULL;
  uint8_t i = 0;
  DPRINTF("\r\nCommandsSuppported(%d, %d, %d)\r\n", included, eKey, endpoint);
  if (true == included)
  {

    if (SECURITY_KEY_NONE == eKey)
    {
      uint8_t keys = ZAF_GetSecurityKeys();

      /*Check non secure command class list*/
      if(0 == endpoint)
      {
        if(SECURITY_KEY_NONE_MASK == keys)
        {
          /*Non-secure included, non-secure cmd class list*/
          pCmdClassList =  &(m_AppInfo.u.pCmdLists->unsecList);
        }
        else{
          /*secure included, non-secure cmd class list*/
          pCmdClassList =  &(m_AppInfo.u.pCmdLists->sec.unsecList);
        }
      }
#ifndef SERIAL_API_APP

      else
      {
        pCmdClassList = GetEndpointcmdClassList(false, endpoint);
      }
#endif
      cmd_class_list.pList = cmd_class_buffer;
      cmd_class_list.size = 0;

      for(i = 0; i < pCmdClassList->size; i++)
      {
        cmd_class_buffer[cmd_class_list.size] = *(pCmdClassList->pList + i);
        cmd_class_list.size++;
      }
      pCmdClassList = &cmd_class_list;
    }
    else
    {
      /*Check secure command class list*/

      /*If eKey not is supported, return NULL pointer!!*/
      if(eKey == GetHighestSecureLevel(ZAF_GetSecurityKeys()) )
      {
        if(0 == endpoint)
        {
          pCmdClassList = &(m_AppInfo.u.pCmdLists->sec.secList);
        }
  #ifndef SERIAL_API_APP
        else
        {
          pCmdClassList = GetEndpointcmdClassList(true, endpoint);
        }
  #endif
        /*Remove marker and commands*/
        if( (SECURITY_KEY_S2_UNAUTHENTICATED == eKey) ||
            (SECURITY_KEY_S2_AUTHENTICATED == eKey) ||
            (SECURITY_KEY_S2_ACCESS == eKey)
          )
        {
          cmd_class_list.pList = cmd_class_buffer;
          cmd_class_list.size = 0;
          while(((uint8_t)*(pCmdClassList->pList + cmd_class_list.size) != COMMAND_CLASS_MARK) &&
                (cmd_class_list.size < pCmdClassList->size))
          {
            cmd_class_buffer[cmd_class_list.size] = *(pCmdClassList->pList + cmd_class_list.size);
            cmd_class_list.size++;
          }
          pCmdClassList = &cmd_class_list;
        }
      }
      else
      {
        /*not included. Deliver empty list*/
        cmd_class_list.pList = NULL;
        cmd_class_list.size = 0;
        pCmdClassList = &cmd_class_list;
      }
    }
  }
  else
  {
    /*Not included!*/
    if(0 == endpoint)
    {
      if (SECURITY_KEY_NONE == eKey)
      {
        pCmdClassList = &(m_AppInfo.u.pCmdLists->unsecList);
      }
      else
      {
        /*not included. Deliver empty list*/
        cmd_class_list.pList = NULL;
        cmd_class_list.size = 0;
        pCmdClassList = &cmd_class_list;
      }
    }
#ifndef SERIAL_API_APP
    else
    {
      pCmdClassList = GetEndpointcmdClassList(false, endpoint);
    }
#endif
  }

  return pCmdClassList;
}

uint8_t
Transport_OnLearnCompleted(node_id_t nodeID)
{
  UNUSED(nodeID);

  SetupActiveNIF();

  if (NULL != cbUpdateStayAwakePeriod)
  {
    cbUpdateStayAwakePeriod();
  }

  return true;
}

void ZAF_Transport_OnLearnCompleted(void)
{
  Transport_OnLearnCompleted(0);
}


void
ApplicationCommandHandler(void *pSubscriberContext, SZwaveReceivePackage* pRxPackage)
{
  UNUSED(pSubscriberContext);
  ZW_APPLICATION_TX_BUFFER *pCmd = &pRxPackage->uReceiveParams.Rx.Payload.rxBuffer;
  uint8_t cmdLength = pRxPackage->uReceiveParams.Rx.iLength;
  RECEIVE_OPTIONS_TYPE *rxOpt = &pRxPackage->uReceiveParams.Rx.RxOptions;

  DPRINTF("\r\nAppCmdH  %d %d %d", rxOpt->securityKey, pCmd->ZW_Common.cmdClass, pCmd->ZW_Common.cmd);
  DPRINTF("\r\nm_AppInfo size %d %d\r\n", m_AppInfo.u.pNifs->cmdClassListSecureCount, m_AppInfo.activeNonsecureList.size);

#ifndef ACCEPT_ALL_CMD_CLASSES
  /* Check if cmd Class are supported in current mode (unsecure or secure) */
  if (true == CmdClassSupported(rxOpt->securityKey,
                                pCmd->ZW_Common.cmdClass,
                                pCmd->ZW_Common.cmd,
                                m_AppInfo.u.pNifs->cmdClassListSecure,
                                m_AppInfo.u.pNifs->cmdClassListSecureCount,
                                m_AppInfo.activeNonsecureList.pList,
                                m_AppInfo.activeNonsecureList.size))
#endif /* ACCEPT_ALL_CMD_CLASSES */
  {
    Transport_ApplicationCommandHandler(pCmd, cmdLength, rxOpt);

    if (NULL != cbUpdateStayAwakePeriod)
    {
      DPRINT("ApplicationCommandHandler: cbUpdateStayAwakePeriod \r\n");
      cbUpdateStayAwakePeriod();
    }
  }
#ifndef ACCEPT_ALL_CMD_CLASSES
  else
  {
    DPRINT("\r\nCmdCl not supported :(\r\n");
  }
#endif /* ACCEPT_ALL_CMD_CLASSES */
}

bool
TransportCmdClassSupported(uint8_t commandClass,
                           uint8_t command,
                           enum SECURITY_KEY eKey)
{
  return CmdClassSupported(eKey,
                           commandClass,
                           command,
                           m_AppInfo.u.pNifs->cmdClassListSecure,
                           m_AppInfo.u.pNifs->cmdClassListSecureCount,
                           m_AppInfo.activeNonsecureList.pList,
                           m_AppInfo.activeNonsecureList.size);
}

/**
 * @brief Sets up the active NIF.
 * @param nodeId
 */
static void SetupActiveNIF(void)
{
  if(0 == ZAF_GetNodeID() || (SECURITY_KEY_NONE == GetHighestSecureLevel(ZAF_GetSecurityKeys())) )
  {
    m_AppInfo.activeNonsecureList = m_AppInfo.u.pCmdLists->unsecList;
  }
  else
  {
    m_AppInfo.activeNonsecureList = m_AppInfo.u.pCmdLists->sec.unsecList;
  }
}
