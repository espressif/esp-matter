/**************************************************************************//**
 * @file cmds_rf.h
 * @brief The header file for command handling of RF related serialAPI
 * commands
 * @copyright 2022 Silicon Laboratories Inc.
 *****************************************************************************/

#ifndef APPS_SERIALAPI_CMD_RF_H_
#define APPS_SERIALAPI_CMD_RF_H_

#include <stdint.h>

void func_id_set_listen_before_talk(uint8_t inputLength,
                                    const uint8_t *pInputBuffer,
                                    uint8_t *pOutputBuffer,
                                    uint8_t *pOutputLength);
void func_id_set_powerlevel(uint8_t inputLength,
                            uint8_t *pInputBuffer,
                            uint8_t *pOutputBuffer,
                            uint8_t *pOutputLength);

#endif /* APPS_SERIALAPI_CMD_RF_H_ */
