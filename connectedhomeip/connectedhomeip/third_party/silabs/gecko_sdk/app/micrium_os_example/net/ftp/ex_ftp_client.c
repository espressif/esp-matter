/***************************************************************************//**
 * @file
 * @brief FTP Example Code
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_FTP_CLIENT_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  <common/include/rtos_path.h>
#include  <common/include/rtos_utils.h>

#include  <net/include/net_app.h>
#include  <net/include/ftp_client.h>

#include  "ex_ftp_client_files.h"

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
#include  <fs/include/fs_core.h>
#include  <fs/include/fs_core_cache.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_core_partition.h>
#include  <fs/include/fs_core_vol.h>
#include  <fs/include/fs_fat.h>
#include  <fs/include/fs_ramdisk.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
#ifndef  EX_FTP_CLIENT_SEND_FILE_LOCAL_PATH
#define  EX_FTP_CLIENT_SEND_FILE_LOCAL_PATH     "local_file.txt"    // TODO Modify to specify the file path to upload.
#endif

#ifndef  EX_FTP_CLIENT_SEND_FILE_REMOTE_NAME                        // TODO Modify to specify the remote file path.
#define  EX_FTP_CLIENT_SEND_FILE_REMOTE_NAME    "upload/remote_file.txt"
#endif

#ifndef  EX_FTP_CLIENT_RECEIVE_FILE_LOCAL_PATH
#define  EX_FTP_CLIENT_RECEIVE_FILE_LOCAL_PATH  "file_1KB.zip"      // TODO Modify to specify the download file path.
#endif
#endif

#ifndef  EX_FTP_CLIENT_RAMDISK_SEC_SIZE
#define  EX_FTP_CLIENT_RAMDISK_SEC_SIZE          512u
#endif

#ifndef  EX_FTP_CLIENT_RAMDISK_SEC_NBR
#define  EX_FTP_CLIENT_RAMDISK_SEC_NBR            70u
#endif

#ifndef  EX_FTP_CLIENT_FILE_RAM_MEDIA_NAME
#define  EX_FTP_CLIENT_FILE_RAM_MEDIA_NAME      "ram_ftpc"
#endif

#ifndef  EX_FTP_CLIENT_FILE_VOL_NAME
#define  EX_FTP_CLIENT_FILE_VOL_NAME            "ram_ftpc"
#endif

#ifndef  EX_FTP_CLIENT_WRK_DIR
#define  EX_FTP_CLIENT_WRK_DIR                   EX_FTP_CLIENT_FILE_VOL_NAME
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
static CPU_INT08U  Ex_FTP_Client_RAMDisk[EX_FTP_CLIENT_RAMDISK_SEC_SIZE * EX_FTP_CLIENT_RAMDISK_SEC_NBR];
static FS_CACHE    *Ex_FTP_Client_CachePtr;
static CPU_BOOLEAN FTP_FS_Media_Prepared = DEF_FALSE;
#endif

static CPU_CHAR Ex_FTP_ClientData[] = "This is the data to transmit to the Server";

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
static void Ex_FTP_Client_FS_MediaPrepare(void);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Ex_FTP_ClientSendData()
 *
 * @brief  Send data to a file to the FTP server.
 *******************************************************************************************************/
void Ex_FTP_ClientSendData(void)
{
  FTPc_CONN ftp_conn;
  RTOS_ERR  err;

  FTPc_Open(&ftp_conn,
            DEF_NULL,
            DEF_NULL,
            "speedtest.tele2.net",
            0,
            DEF_NULL,
            DEF_NULL,
            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FTPc_SendBuf(&ftp_conn,
               "upload/datamicriumos.bin",
               (CPU_INT08U *)Ex_FTP_ClientData,
               sizeof(Ex_FTP_ClientData),
               DEF_NO,
               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FTPc_Close(&ftp_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                         Ex_FTP_ClientSendFile()
 *
 * @brief  Send a file to the FTP server.
 *******************************************************************************************************/
#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
void Ex_FTP_ClientSendFile(void)
{
  FTPc_CONN ftp_conn;
  RTOS_ERR  err;

  if (FTP_FS_Media_Prepared == DEF_FALSE) {
    Ex_FTP_Client_FS_MediaPrepare();
  }

  FTPc_Open(&ftp_conn,
            DEF_NULL,
            DEF_NULL,
            "speedtest.tele2.net",
            0,
            DEF_NULL,
            DEF_NULL,
            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FTPc_SendFile(&ftp_conn, EX_FTP_CLIENT_SEND_FILE_REMOTE_NAME, EX_FTP_CLIENT_SEND_FILE_LOCAL_PATH, DEF_NO, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FTPc_Close(&ftp_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}
#endif

/****************************************************************************************************//**
 *                                       Ex_FTP_ClientReceiveData()
 *
 * @brief  Download a file in a local buffer.
 *******************************************************************************************************/
void Ex_FTP_ClientReceiveData(void)
{
  CPU_INT08U buf[1024];
  FTPc_CONN  ftp_conn;
  CPU_INT32U file_size;
  RTOS_ERR   err;

  FTPc_Open(&ftp_conn,
            DEF_NULL,
            DEF_NULL,
            "speedtest.tele2.net",
            0,
            DEF_NULL,
            DEF_NULL,
            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FTPc_RecvBuf(&ftp_conn, "1KB.zip", buf, sizeof(buf), &file_size, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FTPc_Close(&ftp_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                       Ex_FTP_ClientReceiveFile()
 *
 * @brief  Download a file.
 *******************************************************************************************************/
#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
void Ex_FTP_ClientReceiveFile(void)
{
  FTPc_CONN ftp_conn;
  RTOS_ERR  err;

  if (FTP_FS_Media_Prepared == DEF_FALSE) {
    Ex_FTP_Client_FS_MediaPrepare();
  }

  FTPc_Open(&ftp_conn,
            DEF_NULL,
            DEF_NULL,
            "speedtest.tele2.net",
            0,
            DEF_NULL,
            DEF_NULL,
            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FTPc_RecvFile(&ftp_conn, "1KB.zip", EX_FTP_CLIENT_RECEIVE_FILE_LOCAL_PATH, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FTPc_Close(&ftp_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                     Ex_FTP_Client_FS_MediaPrepare()
 *
 * @brief  Example of File System media preparation to contain FTP Client example files.
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
static void Ex_FTP_Client_FS_MediaPrepare(void)
{
  FS_RAM_DISK_CFG   ramdisk_cfg;
  FS_BLK_DEV_HANDLE dev_handle;
  FS_MEDIA_HANDLE   media_handle;
  FS_WRK_DIR_HANDLE wrk_dir_handle;
  FS_CACHE_CFG      cache_cfg;
  FS_FILE_HANDLE    file_handle;
  CPU_SIZE_T        size_wr;
  CPU_SIZE_T        rem_size = STATIC_INDEX_HTML_LEN;
  RTOS_ERR          err;

  ramdisk_cfg.DiskPtr = Ex_FTP_Client_RAMDisk;                 // Create a RAMDisk that will contain the files.
  ramdisk_cfg.LbCnt = EX_FTP_CLIENT_RAMDISK_SEC_NBR;
  ramdisk_cfg.LbSize = EX_FTP_CLIENT_RAMDISK_SEC_SIZE;

  FS_RAM_Disk_Add(EX_FTP_CLIENT_FILE_RAM_MEDIA_NAME,
                  &ramdisk_cfg,
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  cache_cfg.Align = sizeof(CPU_ALIGN);                          // Create cache for RAMDisk block device.
  cache_cfg.BlkMemSegPtr = DEF_NULL;
  cache_cfg.MaxLbSize = 512u;
  cache_cfg.MinLbSize = 512u;
  cache_cfg.MinBlkCnt = 1u;

  Ex_FTP_Client_CachePtr = FSCache_Create(&cache_cfg,
                                          &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Open block device.
  media_handle = FSMedia_Get(EX_FTP_CLIENT_FILE_RAM_MEDIA_NAME);
  APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle),; );

  dev_handle = FSBlkDev_Open(media_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FSCache_Assign(dev_handle,                                    // Assign cache to block device.
                 Ex_FTP_Client_CachePtr,
                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FS_FAT_Fmt(dev_handle,                                        // Format RAMDisk in FAT format.
             FS_PARTITION_NBR_VOID,
             DEF_NULL,
             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  (void)FSVol_Open(dev_handle,                                  // Open volume and call it "ram_ftpc".
                   1u,
                   EX_FTP_CLIENT_FILE_VOL_NAME,
                   FS_VOL_OPT_DFLT,
                   &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  wrk_dir_handle = FSWrkDir_Open(FSWrkDir_NullHandle,           // Create and open a working directory at the root of vol.
                                 EX_FTP_CLIENT_WRK_DIR,
                                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FSWrkDir_TaskBind(FS_WRK_DIR_NULL, EX_FTP_CLIENT_WRK_DIR, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  file_handle = FSFile_Open(wrk_dir_handle,
                            EX_FTP_CLIENT_SEND_FILE_LOCAL_PATH,
                            (FS_FILE_ACCESS_MODE_CREATE | FS_FILE_ACCESS_MODE_WR),
                            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  while (rem_size > 0 && err.Code == RTOS_ERR_NONE) {
    size_wr = FSFile_Wr(file_handle,
                        (CPU_CHAR *)Ex_FTP_Client_IndexHtml,
                        STATIC_INDEX_HTML_LEN,
                        &err);

    rem_size -= size_wr;
  }

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FSFile_Close(file_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FSWrkDir_Close(wrk_dir_handle, &err);                         // Close working directory.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FTP_FS_Media_Prepared = DEF_TRUE;
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_FTP_CLIENT_AVAIL
