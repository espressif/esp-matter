/**
 * @file nvm_backup_restore.c
 * @copyright 2022 Silicon Laboratories Inc.
 */

#include <string.h>
#include <serialappl.h>
#include <nvm_backup_restore.h>
#include <utils.h>
#include <ZW_controller_api.h>
#include <serialapi_file.h>
#include <ZAF_Common_interface.h>
#include <zpal_watchdog.h>
#include <zpal_nvm.h>

//#define DEBUGPRINT
#include "DebugPrint.h"

/*WARNING: The backup/restore feature is based on the thesis that the NVM area is one continuous block even if it consist of two blocks,
 A protocol and an application block. These blocks are defined in the linker script. The blocks are addressed using the data structure below.
 The definition of the NVM blocks should not be changed, changing the definition will result in breaking the backup/restore feature*/

#define WORK_BUFFER_SIZE    64

/* HOST->ZW:
operation          [open=0|read=1|write=2|close=3]
length             desired length of read/write operation
offset(MSB)        pointer to NVM memory
offset(LSB)
buffer[]           buffer only sent for operation=write
*/
/* ZW->HOST:
retVal             [OK=0|error=1|EOF=-1]
length             actual length of read/written data
offset(MSB)        pointer to NVM memory (EOF ptr for operation=open)
offset(LSB)
buffer[]           buffer only returned for operation=read
*/

static eNVMBackupRestoreOperation NVMBackupRestoreOperationInProgress = NVMBackupRestoreOperationClose;

/**
 * Must be called to open the backup restore feature
 * The function will shut down the RF, Z-Wave timer system , close the NVM system, and disable the watchdog timer
 *
 * @return true if backup/resotre is opened else false
 */
static bool NvmBackupOpen(void)
{
  SZwaveCommandPackage nvmOpen = {
       .eCommandType = EZWAVECOMMANDTYPE_NVM_BACKUP_OPEN,
       .uCommandParams.NvmBackupRestore.offset = 0,
       .uCommandParams.NvmBackupRestore.length = 0,
  };
  zpal_enable_watchdog(false);
  uint8_t bReturn = QueueProtocolCommand((uint8_t*)&nvmOpen);
  if (EQUEUENOTIFYING_STATUS_SUCCESS == bReturn)
  {
    SZwaveCommandStatusPackage cmdStatus;
    if (GetCommandResponse(&cmdStatus, EZWAVECOMMANDSTATUS_NVM_BACKUP_RESTORE))
    {
      if (cmdStatus.Content.NvmBackupRestoreStatus.status)
      {
        return true;
      }
    }
  }
  return false;
}

/**
 * Read data from the NVM area
 *
 * @param offset[in] The offset of the NVM area to read from
 * @param length[in] the length of the NVM area to read
 * @param pNvmData[out] the data read from the NVM area
 *
 * @return true if data is read else false
 */
static uint8_t NvmBackupRead( uint32_t offset, uint8_t length, uint8_t* pNvmData)
{
  SZwaveCommandPackage nvmRead = {
       .eCommandType = EZWAVECOMMANDTYPE_NVM_BACKUP_READ,
       .uCommandParams.NvmBackupRestore.offset = offset,
       .uCommandParams.NvmBackupRestore.length = length,
       .uCommandParams.NvmBackupRestore.nvmData = pNvmData
  };
  DPRINTF("NVM_Read_ 0x%08x, 0x%08x, 0x%08x\r\n",offset, length, (uint32_t)pNvmData);
  uint8_t bReturn = QueueProtocolCommand((uint8_t*)&nvmRead);
  if (EQUEUENOTIFYING_STATUS_SUCCESS == bReturn)
  {
    SZwaveCommandStatusPackage cmdStatus;
    if (GetCommandResponse(&cmdStatus, EZWAVECOMMANDSTATUS_NVM_BACKUP_RESTORE))
    {
      if (cmdStatus.Content.NvmBackupRestoreStatus.status)
      {
    	DPRINT("NVM_READ_OK\r\n");
        return true;
      }
    }
  }
  DPRINT("NVM_READ_ERR\r\n");
  return false;
}

/**
 * Close the open/restore feature
 *
 * @return true if backup/retore feature is closed else false
 */

static uint8_t NvmBackupClose(void)
{
  SZwaveCommandPackage nvmClose = {
       .eCommandType = EZWAVECOMMANDTYPE_NVM_BACKUP_CLOSE
  };
  uint8_t bReturn = QueueProtocolCommand((uint8_t*)&nvmClose);

  return ((EQUEUENOTIFYING_STATUS_SUCCESS == bReturn)? true: false);
}

/**
 * Restore the NVM data
 *
 * @param offset[in] The offset of the NVM area to wite backup data to
 * @param length[in] the length of the backup data
 * @param pNvmData[out] the data to be written to the NVM
 *
 * @return true if data is written else false
 */
static uint8_t NvmBackupRestore( uint32_t offset, uint8_t length, uint8_t* pNvmData)
{
  SZwaveCommandPackage nvmWrite = {
       .eCommandType = EZWAVECOMMANDTYPE_NVM_BACKUP_WRITE,
       .uCommandParams.NvmBackupRestore.offset = offset,
       .uCommandParams.NvmBackupRestore.length = length,
       .uCommandParams.NvmBackupRestore.nvmData = pNvmData
  };
  uint8_t bReturn = QueueProtocolCommand((uint8_t*)&nvmWrite);
  if (EQUEUENOTIFYING_STATUS_SUCCESS == bReturn)
  {
    SZwaveCommandStatusPackage cmdStatus;
    if (GetCommandResponse(&cmdStatus, EZWAVECOMMANDSTATUS_NVM_BACKUP_RESTORE))
    {
      if (cmdStatus.Content.NvmBackupRestoreStatus.status)
      {
    	DPRINT("NVM_WRITE_OK\r\n");
        return true;
      }
    }
  }
  DPRINT("NVM_WRITE_ERR\r\n");
  return false;
}

void func_id_serial_api_nvm_backup_restore(uint8_t inputLength, uint8_t *pInputBuffer, uint8_t *pOutputBuffer, uint8_t *pOutputLength)
{
  UNUSED(inputLength);
  uint32_t NVM_WorkPtr;
  uint8_t dataLength;
  const uint32_t nvm_storage_size = zpal_nvm_backup_get_size();

  dataLength = 0;                                   /* Assume nothing is read or written */
  pOutputBuffer[0] = NVMBackupRestoreReturnValueOK; /* Assume not at EOF and no ERROR */
  pOutputBuffer[1] = 0;                             /* Assume no data */
  pOutputBuffer[2] = 0;
  pOutputBuffer[3] = 0;
  switch (pInputBuffer[0]) /* operation */
  {
    case NVMBackupRestoreOperationOpen: /* open */
    {
      if (NVMBackupRestoreOperationClose == NVMBackupRestoreOperationInProgress)
      {
      /* Lock everyone else out from making changes to the NVM content */
      /* Remember to have some kind of dead-mans-pedal to release lock again. */
      /* TODO */
      // here we have to  shut down RF and disable power management and close NVM subsystem
        if (NvmBackupOpen())
        {
          NVMBackupRestoreOperationInProgress = NVMBackupRestoreOperationOpen;
          NVM_WorkPtr = 0;

      /* Set the size of the backup/restore. (Number of bytes in flash used for file systems) */
      /* Please note that the special case where nvm_storage_size == 0x10000 is indicated by 0x00 0x00 */
          pOutputBuffer[2] = (uint8_t)(nvm_storage_size >> 8);
          pOutputBuffer[3] = (uint8_t)nvm_storage_size;
        }
        else
        {
          pOutputBuffer[0] = NVMBackupRestoreReturnValueError; /*Report error we can't open backup restore feature*/
        }
      }
    }
    break;

    case NVMBackupRestoreOperationRead: /* read */
    {
      /* Validate input */
      DPRINT("NVM_Read_ \r\n");
      if ((NVMBackupRestoreOperationInProgress != NVMBackupRestoreOperationRead) &&
          (NVMBackupRestoreOperationInProgress != NVMBackupRestoreOperationOpen))
      {
    	DPRINT("NVM_Read_Mis \r\n");
        pOutputBuffer[0] = NVMBackupRestoreReturnValueOperationMismatch;
        break;
      }
      NVMBackupRestoreOperationInProgress = NVMBackupRestoreOperationRead;
      dataLength = pInputBuffer[1]; /* Requested dataLength */
      NVM_WorkPtr = (((uint32_t)pInputBuffer[2] << 8) + pInputBuffer[3]);
      /* Make sure that length isn't larger than the available buffer size */
      if (dataLength > WORK_BUFFER_SIZE)
      {
        dataLength = WORK_BUFFER_SIZE;
      }
      /* Make sure that we don't go beyond valid NVM content */
      if ((NVM_WorkPtr + dataLength) >= nvm_storage_size)
      {
    	DPRINT("NVM_Read_EOF \r\n");
        dataLength = (uint8_t)(nvm_storage_size - NVM_WorkPtr);
        pOutputBuffer[0] = (uint8_t)NVMBackupRestoreReturnValueEOF; /* Indicate at EOF */
      }
      pOutputBuffer[1] = dataLength;
      pOutputBuffer[2] = pInputBuffer[2];
      pOutputBuffer[3] = pInputBuffer[3];
      NvmBackupRead(NVM_WorkPtr, dataLength, &pOutputBuffer[4]);
    }
    break;

    case NVMBackupRestoreOperationWrite: /* write */
    {
      /* Validate input */
      if ((NVMBackupRestoreOperationInProgress != NVMBackupRestoreOperationWrite) &&
          (NVMBackupRestoreOperationInProgress != NVMBackupRestoreOperationOpen))
      {
        DPRINT("NVM_Write_mis \r\n");
        pOutputBuffer[0] = NVMBackupRestoreReturnValueOperationMismatch;
        break;
      }
      NVMBackupRestoreOperationInProgress = NVMBackupRestoreOperationWrite;
      dataLength = pInputBuffer[1]; /* Requested dataLength */
      NVM_WorkPtr = (uint32_t)(((uint16_t)pInputBuffer[2] << 8) + pInputBuffer[3]);
      if (dataLength > WORK_BUFFER_SIZE)
      {
        DPRINT("NVM_Write_buff_err \r\n");
        pOutputBuffer[0] = NVMBackupRestoreReturnValueError; /* ERROR: ignore request if length is larger than available buffer */
      }
      else
      {
        /* Make sure that we don't go beyond valid NVM content */
        uint8_t tmp_buf[WORK_BUFFER_SIZE];
        if ((NVM_WorkPtr + dataLength) >= nvm_storage_size)
        {
          DPRINT("NVM_Write_EOF \r\n");
          dataLength = (uint8_t)(nvm_storage_size - NVM_WorkPtr);
          pOutputBuffer[0] = (uint8_t)NVMBackupRestoreReturnValueEOF; /* Indicate at EOF */
        }
        memcpy(tmp_buf, (uint8_t*)&pInputBuffer[4], dataLength);
        NvmBackupRestore(NVM_WorkPtr , dataLength, tmp_buf);
        pOutputBuffer[1] = dataLength; /* Data written */
        pOutputBuffer[2] = pInputBuffer[2];
        pOutputBuffer[3] = pInputBuffer[3];

      }
      dataLength = 0;
    }
    break;

    case NVMBackupRestoreOperationClose: /* close */
    {
      /* Unlock NVM content, so everyone else can make changes again */
      // here we have to  shut down RF and disable power management
      /* TODO */
      if (NVMBackupRestoreOperationInProgress == NVMBackupRestoreOperationClose)
      {
        break;
      }
      if (NvmBackupClose())
      {
        NVMBackupRestoreOperationInProgress = NVMBackupRestoreOperationClose;
      }
      else
      {
        pOutputBuffer[0] = NVMBackupRestoreReturnValueError; /*repoert error we canot close backup restore feature*/
      }
    }
    break;

    default:
    break;
  }
  *pOutputLength = dataLength + 4;

}
