/***************************************************************************//**
 * @file
 * @brief File System Example
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

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  "ex_fs.h"
#include  "ex_fs_utils.h"
#include  "ex_fs_media_poll.h"

#include  <common/include/rtos_utils.h>
#include  <kernel/include/os.h>

#if (defined(RTOS_MODULE_FS_STORAGE_SCSI_AVAIL))
#include  <fs/include/fs_scsi.h>
#endif
#if (defined(RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL) || defined(RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL))
#include  <fs/include/fs_sd.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_FS_MEDIA_POLL_TASK_Q_LEN                        10u
#define  EX_FS_MEDIA_POLL_TASK_PRIO                         22u
#define  EX_FS_MEDIA_POLL_TASK_STK_SIZE                     640u

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)              printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  ex_fs_media_poll_data {
  MEM_DYN_POOL HandlePool;
} EX_FS_MEDIA_POLL_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static EX_FS_MEDIA_POLL_DATA *Ex_FS_MediaPollDataPtr;           // Pointer to private data.
                                                                // Stack area for Media Poll application task.
static CPU_STK Ex_FS_MediaPollTaskStk[EX_FS_MEDIA_POLL_TASK_STK_SIZE];
static OS_TCB  Ex_FS_MediaPollTaskTCB;                          // Media Poll task control block.

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_FS_MediaPollTaskHandler(void *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_FS_MediaPollInit()
 *
 * @brief  Initialize and create resources needed by the Media Poll example.
 *******************************************************************************************************/
void Ex_FS_MediaPollInit(void)
{
  RTOS_ERR err;

  //                                                               Allocate media poll private data.
  Ex_FS_MediaPollDataPtr = (EX_FS_MEDIA_POLL_DATA *) Mem_SegAlloc("Ex - FS Media Poll Data",
                                                                  DEF_NULL,
                                                                  sizeof(EX_FS_MEDIA_POLL_DATA),
                                                                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               Allocate media handles pool.
  Mem_DynPoolCreate("FS - handles pool",
                    &Ex_FS_MediaPollDataPtr->HandlePool,
                    DEF_NULL,
                    sizeof(FS_MEDIA_HANDLE),
                    sizeof(CPU_ALIGN),
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,                  // Unlimited pool.
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               Create the media poll application task.
  OSTaskCreate(&Ex_FS_MediaPollTaskTCB,
               "Ex - FS media poll task",
               Ex_FS_MediaPollTaskHandler,
               DEF_NULL,
               EX_FS_MEDIA_POLL_TASK_PRIO,
               Ex_FS_MediaPollTaskStk,
               EX_FS_MEDIA_POLL_TASK_STK_SIZE / 10u,
               EX_FS_MEDIA_POLL_TASK_STK_SIZE,
               EX_FS_MEDIA_POLL_TASK_Q_LEN,                     // Built-in task queue used for media handles.
               0u,
               DEF_NULL,
               OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                           Ex_FS_MediaOnConn()
 *
 * @brief  Callback called by the file system upon connection of a removable media.
 *
 * @param  media_handle  Handle to a media.
 *******************************************************************************************************/
void Ex_FS_MediaPollOnConn(FS_MEDIA_HANDLE media_handle)
{
  FS_MEDIA_HANDLE *p_media_handle;
  CPU_CHAR        media_name[20u];
  FS_MEDIA_TYPE   media_type;
#if defined(RTOS_MODULE_FS_STORAGE_SCSI_AVAIL)
  FS_SCSI_HANDLE  scsi_handle;
  FS_SCSI_LU_INFO lu_info;
#endif
#if defined(RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL) \
  || defined(RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL)
  FS_SD_HANDLE sd_handle;
  FS_SD_INFO   sd_info;
#endif
  CPU_BOOLEAN notify = DEF_NO;
  RTOS_ERR    err;

  //                                                               Get media name associated to handle.
  FSMedia_NameGet(media_handle,
                  media_name,
                  20u,                                          // Media name buffer size.
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("\r\n>> Ex_FS_MediaOnConn(): Media '%s' connected!\r\n\r\n", media_name);

  media_type = FSMedia_TypeGet(media_handle, &err);             // Retrieve media type.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               Display info about connected media.
  switch (media_type) {
#if defined(RTOS_MODULE_FS_STORAGE_SCSI_AVAIL)
    case FS_MEDIA_TYPE_SCSI:
      scsi_handle = FS_SCSI_Open(media_handle, &err);           // Open a SCSI device.
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      //                                                           Get information about the SCSI device.
      FS_SCSI_LU_InfoGet(scsi_handle,
                         &lu_info,                              // Structure to receive device information.
                         &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      //                                                           Display specific information about SCSI device.
      EX_TRACE("LU info: \r\n");
      EX_TRACE("- Sector size   : %d bytes\r\n", lu_info.SecDfltSize);
      EX_TRACE("- Nbr of sectors: %d\r\n", lu_info.SecCnt);
      EX_TRACE("- Vendor        : %s\r\n", lu_info.VendorID_StrTbl);
      EX_TRACE("- Product       : %s\r\n", lu_info.ProductID_StrTbl);

      FS_SCSI_Close(scsi_handle, &err);                         // Close block device.
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

      notify = DEF_YES;
      break;
#endif

#if defined(RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL) \
    || defined(RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL)
    case FS_MEDIA_TYPE_SD_CARD:
    case FS_MEDIA_TYPE_SD_SPI:
      sd_handle = FS_SD_Open(media_handle, &err);               // Open a SD device.
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      //                                                           Get information about the SD device.
      FS_SD_InfoGet(sd_handle,
                    &sd_info,                                   // Structure to receive device information.
                    &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      //                                                           Display specific information about SD device.
      EX_TRACE("SD info: \r\n");
      EX_TRACE("- Sector size       : %d bytes\r\n", sd_info.BlkSize);
      EX_TRACE("- Nbr of sectors    : %d\r\n", sd_info.NbrBlks);
      EX_TRACE("- Max clk freq      : %d Hz\r\n", sd_info.ClkFreq);
      EX_TRACE("- Comm timeout      : %d cycles\r\n", sd_info.Timeout);
      EX_TRACE("- Card Type         : %d\r\n", sd_info.CardType);
      EX_TRACE("- High Capacity     : %d\r\n", sd_info.HighCapacity);
      EX_TRACE("- ManufID           : 0x%X\r\n", sd_info.ManufID);
      EX_TRACE("- OEM_ID            : 0x%X\r\n", sd_info.OEM_ID);
      EX_TRACE("- Product serial nbr: %d\r\n", sd_info.ProdSN);
      EX_TRACE("- Product name      : %s\r\n", sd_info.ProdName);

      FS_SD_Close(sd_handle, &err);                             // Close SD device.
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

      notify = DEF_YES;
      break;
#endif

    default:
      break;
  }

  if (notify) {                                                 // Notify app media poll task about connected media.
                                                                // Get a media handle place holder.
    p_media_handle = (FS_MEDIA_HANDLE *)Mem_DynPoolBlkGet(&Ex_FS_MediaPollDataPtr->HandlePool, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    *p_media_handle = media_handle;                             // Copy media handle in place holder.
                                                                // Post media handle in task's queue.
    OSTaskQPost(&Ex_FS_MediaPollTaskTCB,
                (void *)p_media_handle,
                sizeof(FS_MEDIA_HANDLE *),
                OS_OPT_NONE,
                &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  }
}

/****************************************************************************************************//**
 *                                         Ex_FS_MediaOnDisconn()
 *
 * @brief  Callback called by the file system upon disconnection of a removable media.
 *
 * @param  media_handle  Handle to a media.
 *******************************************************************************************************/
void Ex_FS_MediaPollOnDisconn(FS_MEDIA_HANDLE media_handle)
{
  CPU_CHAR media_name[20u];
  RTOS_ERR err;

  //                                                               Get media name associated to handle.
  FSMedia_NameGet(media_handle,
                  media_name,
                  20u,                                          // Media name buffer size.
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("\r\n>> Ex_FS_MediaOnDisconn(): Media '%s' disconnected!\r\n\r\n", media_name);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      Ex_FS_MediaPollTaskHandler()
 *
 * @brief  Media Poll task handler.
 *
 * @param  p_arg  Pointer to task's argument.
 *******************************************************************************************************/
static void Ex_FS_MediaPollTaskHandler(void *p_arg)
{
  FS_MEDIA_HANDLE *p_media_handle;
  CPU_CHAR        media_name[20u];
  OS_MSG_SIZE     msg_size;
  FS_VOL_HANDLE   vol_handle;
  RTOS_ERR        err;

  PP_UNUSED_PARAM(p_arg);

  while (DEF_ON) {
    //                                                             Wait for handle of a newly connected media.
    p_media_handle = (FS_MEDIA_HANDLE *)OSTaskQPend(0u,
                                                    OS_OPT_NONE,
                                                    &msg_size,
                                                    DEF_NULL,
                                                    &err);
    if (err.Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_MediaPollTaskHandler(): Error receiving media handle from queue.\r\n");
      continue;
    }

    //                                                             Open device and volume.
    vol_handle = Ex_FS_VolOpen(*p_media_handle,
                               "vol10",                         // Unique volume name.
                               &err);
    if (err.Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_MediaPollTaskHandler(): Error opening volume.\r\n");
      continue;
    }

    //                                                             Get media name associated to handle.
    FSMedia_NameGet(*p_media_handle,
                    media_name,
                    20u,                                        // Media name buffer size.
                    &err);
    if (err.Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_MediaPollTaskHandler(): Error getting media name.\r\n");
      continue;
    }
    EX_TRACE("FS Example: File rd/wr on '%s' (removable media)...", media_name);
    //                                                             Execute file read/write example.
    Ex_FS_FileRdWr_Exec(vol_handle,
                        "file-rdwr-mediapoll.txt",
                        5454u,                                  // File size.
                        &err);
    if (err.Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_MediaPollTaskHandler(): Error executing file read-write.\r\n");
    } else {
      EX_TRACE("OK\n\r");
    }

    Ex_FS_VolClose(vol_handle, &err);                           // Close volume and device.
    if (err.Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_MediaPollTaskHandler(): Error closing volume.\r\n");
      continue;
    }
    //                                                             Free media handle place holder for next media.
    Mem_DynPoolBlkFree(&Ex_FS_MediaPollDataPtr->HandlePool,
                       (void *)p_media_handle,
                       &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
