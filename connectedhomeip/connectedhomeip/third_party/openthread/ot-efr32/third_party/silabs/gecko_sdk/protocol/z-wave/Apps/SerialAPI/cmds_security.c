/**
 * @file cmds_security.c
 * @copyright 2022 Silicon Laboratories Inc.
 */

#include <cmds_security.h>
#include <slave_supported_func.h>
#include <stdbool.h>
#include <ZAF_Common_interface.h>
#include <MfgTokens.h>

//#define DEBUGPRINT
#include <DebugPrint.h>

#define SUPPORT_ZW_GET_SECURITY_S2_PUBLIC_DSK 1

/* FUNC_ID_ZW_SECURITY_SETUP command definitions. */
typedef enum
{
  E_SECURITY_SETUP_CMD_ZW_GET_SECURITY_KEYS = 0,
  E_SECURITY_SETUP_CMD_ZW_SET_SECURITY_S0_NETWORK_KEY, // OBSOLETE
  E_SECURITY_SETUP_CMD_ZW_GET_SECURITY_S2_PUBLIC_DSK,
  E_SECURITY_SETUP_CMD_ZW_SET_SECURITY_S2_CRITICAL_NODE_ID, // OBSOLETE
  E_SECURITY_SETUP_CMD_ZW_SET_SECURITY_S2_INCLUSION_PUBLIC_DSK_CSA, // OBSOLETE
  E_SECURITY_SETUP_CMD_SET_SECURITY_INCLUSION_REQUESTED_KEYS,
  E_SECURITY_SETUP_CMD_SET_SECURITY_INCLUSION_REQUESTED_AUTHENTICATION, // OBSOLETE
  E_SECURITY_SETUP_CMD_GET_SECURITY_CAPABILITIES = 0xFE,
  E_SECURITY_SETUP_CMD_UNKNOWN = 0xFF
} eSecuritySetupCmd_t;

/* FUNC_ID_ZW_SECURITY_SETUP command supported definitions. */
/* Used together with the Security Setup command GetSecurityCapabilities */
/* returned supported command bitmask to determine if a specific command are supported */
typedef enum
{
  E_SECURITY_SETUP_SUPPORT_CMD_ZW_GET_SECURITY_KEYS                            = (1<<E_SECURITY_SETUP_CMD_ZW_GET_SECURITY_KEYS),
  E_SECURITY_SETUP_SUPPORT_CMD_ZW_SET_SECURITY_S0_NETWORK_KEY                  = (1<<E_SECURITY_SETUP_CMD_ZW_SET_SECURITY_S0_NETWORK_KEY), // OBSOLETE
  E_SECURITY_SETUP_SUPPORT_CMD_ZW_GET_SECURITY_S2_PUBLIC_DSK                   = (1<<E_SECURITY_SETUP_CMD_ZW_GET_SECURITY_S2_PUBLIC_DSK),
  E_SECURITY_SETUP_SUPPORT_CMD_ZW_SET_SECURITY_S2_CRITICAL_NODE_ID             = (1<<E_SECURITY_SETUP_CMD_ZW_SET_SECURITY_S2_CRITICAL_NODE_ID), // OBSOLETE
  E_SECURITY_SETUP_SUPPORT_CMD_SET_SECURITY_INCLUSION_REQUESTED_KEYS           = (1<<E_SECURITY_SETUP_CMD_SET_SECURITY_INCLUSION_REQUESTED_KEYS),
  E_SECURITY_SETUP_SUPPORT_CMD_SET_SECURITY_INCLUSION_REQUESTED_AUTHENTICATION = (1<<E_SECURITY_SETUP_CMD_SET_SECURITY_INCLUSION_REQUESTED_AUTHENTICATION) // OBSOLETE
} eSecuritySetupSupportCmd_t;

void func_id_zw_security_setup(uint8_t inputLength,
                               const uint8_t *pInputBuffer,
                               uint8_t *pOutputBuffer,
                               uint8_t *pOutputLength)
{
  UNUSED(inputLength);
  {
    /* HOST->ZW: securityFuncID [| bDataLen | abData[bDataLen]] */
    /* ZW->HOST: securityFuncID | bretValLen | retVal[bretValLen] */
    *pOutputBuffer = *pInputBuffer;
    switch ((eSecuritySetupCmd_t)*pInputBuffer)
    {
#if SUPPORT_ZW_GET_SECURITY_KEYS
      case E_SECURITY_SETUP_CMD_ZW_GET_SECURITY_KEYS:
      {
        /* HOST->ZW: */
        /* ZW->HOST: securityKeys_bitmaskLen(1) | securityKeys_bitmask[securityKeys_bitmaskLen] */
        *(pOutputBuffer + 1) = 1;
        *(pOutputBuffer + 2) = ZAF_GetSecurityKeys();
        *pOutputLength = 3;
      }
      break;

      case E_SECURITY_SETUP_CMD_SET_SECURITY_INCLUSION_REQUESTED_KEYS:
      {
        /* HOST->ZW: registeredSecurityKeysLen(1) | registeredSecurityKeys */
        /* ZW->HOST: retValLen(1) | retVal[retValLen] */
        bool retVal = false;
        if (1 == *(pInputBuffer + 1))
        {
          /* Set the requestedSecurityKeysBits requested by protocol when doing S2 inclusion */
          //SecureKeysRequested = serial_frame->payload[2];
          SZwaveCommandPackage Package;
          Package.eCommandType = EZWAVECOMMANDTYPE_SET_SECURITY_KEYS;
          Package.uCommandParams.SetSecurityKeys.keys = *(pInputBuffer + 2);
          QueueNotifyingSendToBack(ZAF_getAppHandle()->pZwCommandQueue, (uint8_t *)&Package, 0);
          /* sRequestedSecuritySettings.requestedSecurityKeysBits are either the initialized */
          /* define value (REQUESTED_SECURITY_KEYS) or the value set through the FUNC_ID_ZW_SECURITY_SETUP */
          /* function E_SECURITY_SETUP_CMD_SET_SECURITY_INCLUSION_REQUESTED_AUTHENTICATION */
          // FIXME insert check of if protocol has been started yet.
          // if it has - return false.
          retVal = true;
        }
        *(pOutputBuffer + 1) = 1;
        *(pOutputBuffer + 2) = retVal;
        *pOutputLength = 3;
      }
      break;
#endif
#if SUPPORT_ZW_GET_SECURITY_S2_PUBLIC_DSK
      case E_SECURITY_SETUP_CMD_ZW_GET_SECURITY_S2_PUBLIC_DSK:
      {
        /* HOST->ZW: */
        /* ZW->HOST: retValLen(SECURITY_KEY_S2_PUBLIC_DSK_LENGTH) | retVal[retValLen] */
        *(pOutputBuffer + 1) = SECURITY_KEY_S2_PUBLIC_DSK_LENGTH;
        ZW_GetMfgTokenData(pOutputBuffer + 2, TOKEN_MFG_ZW_PUK_ID, SECURITY_KEY_S2_PUBLIC_DSK_LENGTH);
        *pOutputLength = 2 + SECURITY_KEY_S2_PUBLIC_DSK_LENGTH;
      }
      break;
#endif

      case E_SECURITY_SETUP_CMD_GET_SECURITY_CAPABILITIES:
      {
        /* HOST->ZW: */
        /* ZW->HOST: securitySetup_bitmaskLen | securityKeys_bitmask[securitySetup_bitmaskLen] */
        *(pOutputBuffer + 1) = 1;
        /* LSB first if more than one byte in securityKeys_bitmask[] */
        *(pOutputBuffer + 2) = 0
#if SUPPORT_ZW_GET_SECURITY_KEYS
            | E_SECURITY_SETUP_SUPPORT_CMD_ZW_GET_SECURITY_KEYS
            | E_SECURITY_SETUP_SUPPORT_CMD_SET_SECURITY_INCLUSION_REQUESTED_KEYS
#endif
#if SUPPORT_ZW_GET_SECURITY_S2_PUBLIC_DSK
            | E_SECURITY_SETUP_SUPPORT_CMD_ZW_GET_SECURITY_S2_PUBLIC_DSK
#endif
            ;
        *pOutputLength = 3;
      }
      break;

      default:
      {
        /* ZW->HOST: E_SECURITY_SETUP_UNKNOWN_COMMAND | retValLen | securityFuncID(called)[retValLen] */
        *(pOutputBuffer + 0) = E_SECURITY_SETUP_CMD_UNKNOWN;
        *(pOutputBuffer + 1) = 1;
        /* Return the called Unknown FUNC_ID_ZW_SECURITY_SETUP Command */
        *(pOutputBuffer + 2) = *pInputBuffer;
        *pOutputLength = 3;
      }
      break;
    }
  }
}
