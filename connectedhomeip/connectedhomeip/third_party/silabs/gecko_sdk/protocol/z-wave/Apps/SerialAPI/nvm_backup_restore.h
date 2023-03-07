/**
 * @file nvm_backup_restore.h
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef APPS_SERIALAPI_NVM_BACKUP_RESTROE_H_
#define APPS_SERIALAPI_NVM_BACKUP_RESTROE_H_

#include <stdint.h>

/**
 * Must be called upon receiving a "Serial API NVM Backup/restore commands".
 * @param inputLength Length of data in input buffer.
 * @param pInputBuffer Input buffer
 * @param pOutputBuffer Output buffer
 * @param pOutputLength Length of data in output buffer.
 */
void func_id_serial_api_nvm_backup_restore(uint8_t   inputLength,
                                           uint8_t* pInputBuffer,
                                           uint8_t* pOutputBuffer,
                                           uint8_t* pOutputLength);

#endif /* APPS_SERIALAPI_NVM_BACKUP_RESTROE_H_ */
