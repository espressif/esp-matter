/**
*
* @file
* @brief functionality to check if a cmdClass is present in NodeInfo.
*
* @copyright 2018 Silicon Laboratories Inc.
*
*/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZAF_command_class_utils.h>
#include <ZW_classcmd.h>
#include <ZW_TransportSecProtocol.h>
#include <ZAF_Common_interface.h>
#include <string.h>
//#define DEBUGPRINT
#include "DebugPrint.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

bool
CmdClassSupported(security_key_t eKey,
                  uint8_t commandClass,
                  uint8_t command,
                  uint8_t* pSecurelist,
                  uint8_t securelistLen,
                  uint8_t* pNonSecurelist,
                  uint8_t nonSecurelistLen)
{
  security_key_t device_higest_secure_level = GetHighestSecureLevel(ZAF_GetSecurityKeys());
  const SProtocolConfig_t* app_protocol_config = ZAF_getAppProtocolConfig();
  uint8_t app_secure_keys_requested = *(app_protocol_config->pSecureKeysRequested);
  /*Check commandClass is in non-secure list*/
  if(NON_NULL( pNonSecurelist ))
  {
    if (NULL != memchr(pNonSecurelist, commandClass, nonSecurelistLen))
    {
      return true;
    }
  }

  /* First make sure we comply with requirement DT:00.21.000C.1 (Document SDS14224 "Z-Wave Plus v2 Device Type Specification") */
  if ( (app_secure_keys_requested & SECURITY_KEY_S2_ACCESS_BIT) && // If the App requests S2 Access Control
       (SECURITY_KEY_S2_ACCESS != device_higest_secure_level)   &&
               (SECURITY_KEY_S0 != device_higest_secure_level) )   // And the device has neither S2 Access Control or S0 as highest
  {
    return false;                                                  // Then Command Class is NOT supported regardless of type
  }

  /*Only allow secure command list if node is securely included.*/
  /*Check if cmd Class are supported in current mode (non-secure or secure)*/
  if((SECURITY_KEY_NONE != device_higest_secure_level) && (eKey == device_higest_secure_level) && NON_NULL( pSecurelist ))
  {
    if((NULL != memchr(pSecurelist, commandClass, securelistLen)) ||
       (COMMAND_CLASS_BASIC == commandClass))
    {
      return true; /*cmd is supported!*/
    }
  }

  if ((SECURITY_KEY_NONE == device_higest_secure_level) && (COMMAND_CLASS_BASIC == commandClass))
  {
    /* Non-secure node always support CC Basic. */
    return true;
  }

  if ((SECURITY_KEY_S0 == device_higest_secure_level) &&
      (COMMAND_CLASS_MANUFACTURER_SPECIFIC == commandClass) &&
      (MANUFACTURER_SPECIFIC_GET == command) &&
      (SECURITY_KEY_NONE == eKey))
  {
    // This covers CC:0072.01.00.41.004
    return true;
  }

  // Command is not supported.
  return false;
}
