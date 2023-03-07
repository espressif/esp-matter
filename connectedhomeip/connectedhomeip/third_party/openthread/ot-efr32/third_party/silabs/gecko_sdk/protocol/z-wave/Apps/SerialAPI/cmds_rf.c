/**************************************************************************//**
 * @file cmds_rf.c
 * @brief The source file for command handling of RF related serialAPI
 * commands
 * @copyright 2022 Silicon Laboratories Inc.
 *****************************************************************************/

#include <cmds_rf.h>
#include <serialappl.h>
#include <ZW_application_transport_interface.h>
#include <utils.h>

#ifdef SUPPORT_ZW_SET_LISTEN_BEFORE_TALK_THRESHOLD
void func_id_set_listen_before_talk(uint8_t inputLength,
                                    const uint8_t *pInputBuffer,
                                    uint8_t *pOutputBuffer,
                                    uint8_t *pOutputLength)
{
  UNUSED(inputLength);
  uint8_t bReturn;
  SZwaveCommandPackage setLBTMode = {
       .eCommandType = EZWAVECOMMANDTYPE_ZW_SET_LBT_THRESHOLD,
       .uCommandParams.SetLBTThreshold.channel = pInputBuffer[0],
       .uCommandParams.SetLBTThreshold.level = (int8_t)pInputBuffer[1]
  };

  bReturn = QueueProtocolCommand((uint8_t*)&setLBTMode);

  pOutputBuffer[0] = ((EQUEUENOTIFYING_STATUS_SUCCESS == bReturn) ? true : false);
  *pOutputLength = 1;
}
#endif

