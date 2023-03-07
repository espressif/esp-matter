/**
 * @file cmds_security.h
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef APPS_SERIALAPI_CMDS_SECURITY_H_
#define APPS_SERIALAPI_CMDS_SECURITY_H_

#include <stdint.h>

void func_id_zw_security_setup(uint8_t inputLength,
                               const uint8_t *pInputBuffer,
                               uint8_t *pOutputBuffer,
                               uint8_t *pOutputLength);

#endif /* APPS_SERIALAPI_CMDS_SECURITY_H_ */
