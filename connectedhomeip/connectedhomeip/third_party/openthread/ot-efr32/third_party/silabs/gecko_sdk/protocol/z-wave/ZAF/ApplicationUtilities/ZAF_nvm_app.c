/**
 * @file
 * @brief ZWave FileSystem Application module
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */
#include "ZAF_nvm_app.h"


static zpal_nvm_handle_t handle;

bool ApplicationFileSystemInit(zpal_nvm_handle_t* pFileSystemApplication)
{
  handle = zpal_nvm_init(ZPAL_NVM_AREA_APPLICATION);

  if (handle)
  {
    *pFileSystemApplication = handle;
    return true;
  }

  return false;
}

zpal_nvm_handle_t ZAF_GetFileSystemHandle(void)
{
  return handle;
}
