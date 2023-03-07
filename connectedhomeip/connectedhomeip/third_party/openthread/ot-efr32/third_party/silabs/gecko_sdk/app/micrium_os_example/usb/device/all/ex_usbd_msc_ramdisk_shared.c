/***************************************************************************//**
 * @file
 * @brief USB Device Example - USB MSC Logical Unit Add - File System
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

#if (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL) && defined(RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>
#include  <usb/include/device/usbd_core.h>
#include  <usb/include/device/usbd_msc.h>
#include  <usb/include/device/usbd_scsi_storage_blk_dev.h>
#include  <fs/include/fs_core.h>
#include  <fs/include/fs_core_cache.h>
#include  <fs/include/fs_core_partition.h>
#include  <fs/include/fs_core_vol.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_ramdisk.h>
#include  <fs/include/fs_fat.h>
#include  <fs/include/fs_storage.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_USBD_MSC_STARTUP_PRIO                         21u
#define  EX_USBD_MSC_RAMDISK_SEC_SIZE                    512u
#define  EX_USBD_MSC_RAMDISK_SEC_NBR                      60u
#define  EX_USBD_MSC_RAMDISK_LU_QTY_UNLIMITED              0u
#define  EX_USBD_MSC_TASK_STK_SIZE                      1024u
#define  EX_USBD_MSC_TASK_PRIO                            25u
#define  EX_USBD_NBR_ITER                                 10u

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_USBD_MSC_Task(void *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT08U Ex_USBD_RAM_Disk[EX_USBD_MSC_RAMDISK_SEC_SIZE * EX_USBD_MSC_RAMDISK_SEC_NBR];
static CPU_STK    Ex_USBD_MSC_TaskStk[EX_USBD_MSC_TASK_STK_SIZE];
static OS_TCB     Ex_USBD_MSC_TaskTCB;
static CPU_INT08U Ex_USBD_MSC_ClassNbr;
static CPU_INT08U Ex_USBD_MSC_LU_Nbr;
static CPU_CHAR   *Ex_USBD_MSC_MediaNamePtr;
static FS_CACHE   *Ex_USBD_MSC_CachePtr;
static CPU_CHAR   Ex_USBD_MSC_FileWrStr[] = "\r\nIteration number    ";

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Ex_USBD_MSC_Init()
 *
 * @brief  Adds an MSC interface to the device and adds a logical unit to it. The logical unit uses
 *         the MSC Ramdisk storage driver. This example requires uC-RTOS-FS to be part of the project.
 *
 * @param  dev_nbr        Device number.
 *
 * @param  config_nbr_fs  Full-Speed configuration number.
 *
 * @param  config_nbr_hs  High-Speed configuration number.
 *
 * @note   (1) Using the refresh task is recommended when using uC/RTOS-FS storage driver. It is
 *             especially recommended when dealing with removable medias such as an SD Card. In case
 *             of a non-removable media (such as NAND device), the refresh task can be disabled, but
 *             it is recommended to do a low-level mount of the device before starting the RTOS.
 *             This can be done by calling FSMedia_Open() on the device and closing it after by
 *             calling FSMedia_Close() with the "low-level unmount" flag set to DEF_NO.
 *******************************************************************************************************/
void Ex_USBD_MSC_Init(CPU_INT08U dev_nbr,
                      CPU_INT08U config_nbr_fs,
                      CPU_INT08U config_nbr_hs)
{
  CPU_INT08U        lu_nbr;
  CPU_INT08U        msc_nbr;
  RTOS_ERR          err;
  RTOS_TASK_CFG     task_cfg;
  USBD_SCSI_LU_INFO logical_unit_info;
  USBD_MSC_QTY_CFG  msc_qty_conf;
  FS_CACHE_CFG      cache_cfg;
  FS_BLK_DEV_HANDLE dev_handle;
  FS_MEDIA_HANDLE   media_handle;
  FS_RAM_DISK_CFG   ramdisk_cfg;

  ramdisk_cfg.DiskPtr = Ex_USBD_RAM_Disk;
  ramdisk_cfg.LbCnt = EX_USBD_MSC_RAMDISK_SEC_NBR;
  ramdisk_cfg.LbSize = EX_USBD_MSC_RAMDISK_SEC_SIZE;

  //                                                               Add RAM Disk instance to file system.
  FS_RAM_Disk_Add("ramusb",                                     // String identifying this RAM Disk instance.
                  &ramdisk_cfg,                                 // Configuration describing RAM area.
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  cache_cfg.Align = sizeof(CPU_ALIGN);
  cache_cfg.BlkMemSegPtr = DEF_NULL;
  cache_cfg.MaxLbSize = 512u;
  cache_cfg.MinLbSize = 512u;
  cache_cfg.MinBlkCnt = 4u;
  //                                                               ----------- FORMAT PARTITION ON RAMDISK ------------
  Ex_USBD_MSC_CachePtr = FSCache_Create(&cache_cfg,             // Pointer to a cache configuration structure.
                                        &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  media_handle = FSMedia_Get("ramusb");
  APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle),; );

  dev_handle = FSBlkDev_Open(media_handle,
                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FSCache_Assign(dev_handle,
                 Ex_USBD_MSC_CachePtr,
                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FS_FAT_Fmt(dev_handle,                                        // Device handle.
             FS_PARTITION_NBR_VOID,                             // Number of partition to be formated.
             DEF_NULL,                                          // Pointer to a FAT volume configuration structure.
             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FSBlkDev_Close(dev_handle,
                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------------- INITIALIZE MSC CLASS ---------------

  msc_qty_conf.ClassInstanceQty = 1u;
  msc_qty_conf.ConfigQty = 2u;
  msc_qty_conf.LUN_Qty = 1u;

  USBD_MSC_Init(&msc_qty_conf,                                  // Pointer to the MSC configuration structure.
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----------- CREATE AN MSC CLASS INSTANCE -----------
  //                                                               Each MSC class instance requires a task.
  //                                                               TODO: Adapt task configuration to your need.
  task_cfg.Prio = EX_USBD_MSC_STARTUP_PRIO;
  task_cfg.StkPtr = DEF_NULL;
  task_cfg.StkSizeElements = 512u;

  msc_nbr = USBD_MSC_Add(&task_cfg,                             // Pointer to the configuration structure.
                         &err);                                 // Create an MSC instance. Retrieve class instance nbr.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------------- ADD FS LOGICAL UNIT ----------------

  //                                                               Prepare logical unit information...
  logical_unit_info.VendIdPtr = "Micrium";
  logical_unit_info.ProdIdPtr = "USB Ram example";
  logical_unit_info.ProdRevLevel = 0x1000u;
  logical_unit_info.IsRdOnly = DEF_NO;                          // Media is not read only.
                                                                // Specify FS driver and configurations.
  logical_unit_info.SCSI_StorageAPI_Ptr = &USBD_SCSI_StorageBlkDevAPI;

  //                                                               Add logical unit to MSC class instance...
  lu_nbr = USBD_MSC_SCSI_LunAdd(msc_nbr,
                                &logical_unit_info,
                                DEF_NULL,
                                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---- ADD MSC CLASS INSTANCE TO CONFIGURATION(S) ----
  (void)USBD_MSC_ConfigAdd(msc_nbr,                             // MSC instance number.
                           dev_nbr,                             // Device number.
                           config_nbr_fs,                       // Index to which to add the existing MSC interface.
                           &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  if (config_nbr_hs != USBD_CONFIG_NBR_NONE) {                  // Add instance to HS config if available.
    (void)USBD_MSC_ConfigAdd(msc_nbr,                           // MSC instance number.
                             dev_nbr,                           // Device number.
                             config_nbr_hs,                     // Index to which to add the existing MSC interface.
                             &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  }

  Ex_USBD_MSC_LU_Nbr = lu_nbr;
  Ex_USBD_MSC_ClassNbr = msc_nbr;

  OSTaskCreate(&Ex_USBD_MSC_TaskTCB,                            // Create USBD Task.
               "Ex USBD Ramdisk Task",
               Ex_USBD_MSC_Task,
               0,
               EX_USBD_MSC_TASK_PRIO,
               &Ex_USBD_MSC_TaskStk[0],
               (EX_USBD_MSC_TASK_STK_SIZE / 10u),
               EX_USBD_MSC_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        Ex_USBDTask()
 *
 * @brief  This task perform attach/detach operation and write on a file
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_USBD_MSC_Task(void *p_arg)
{
  RTOS_ERR          err;
  CPU_INT08U        i;
  FS_FILE_HANDLE    file_handle;
  FS_VOL_HANDLE     vol_handle;
  CPU_SIZE_T        str_len;
  FS_BLK_DEV_HANDLE dev_handle;
  FS_MEDIA_HANDLE   media_handle;

  PP_UNUSED_PARAM(p_arg);

  for (i = 0; i < EX_USBD_NBR_ITER; i++) {
    media_handle = FSMedia_Get("ramusb");

    dev_handle = FSBlkDev_Open(media_handle,                    // Media handle
                               &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    FSCache_Assign(dev_handle,                                  // Device handle
                   Ex_USBD_MSC_CachePtr,
                   &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    vol_handle = FSVol_Open(dev_handle,                         // Device handle.
                            1u,                                 // Number of partition to be opened.
                            "0",                                // Volume name.
                            FS_VOL_OPT_DFLT,                    // Open option.
                            &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    file_handle = FSFile_Open(FSWrkDir_NullHandle,              // Handle to a working directory.
                              "0/Embedded.txt",                 // File path relative to the given working directory.
                              FS_FILE_ACCESS_MODE_CREATE        // File opening mode.
                              | FS_FILE_ACCESS_MODE_WR
                              | FS_FILE_ACCESS_MODE_APPEND,
                              &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    (void)Str_FmtNbr_Int32U(i,                                  // Number to format.
                            2u,                                 // Number of digit to format.
                            DEF_NBR_BASE_DEC,                   // Base of the number to format.
                            '\0',                               // Prepend leading character.
                            DEF_NO,                             // Format alphabetic characters.
                            DEF_YES,                            // Append terminating NULL-character.
                                                                // Pointer to the character array.
                            (CPU_CHAR *)&Ex_USBD_MSC_FileWrStr[19]);

    str_len = Str_Len((CPU_CHAR *)Ex_USBD_MSC_FileWrStr);

    (void)FSFile_Wr(file_handle,                                // File handle.
                    (CPU_CHAR *)Ex_USBD_MSC_FileWrStr,          // String to be written to the file.
                    str_len,                                    // String size.
                    &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    FSFile_Close(file_handle,                                   // File handle.
                 &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    FSVol_Close(vol_handle,                                     // Volume handle.
                &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    FSBlkDev_Close(dev_handle,
                   &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    USBD_MSC_SCSI_LunAttach(Ex_USBD_MSC_ClassNbr,               // MSC instance number.
                            Ex_USBD_MSC_LU_Nbr,                 // Logical unit number.
                            "ramusb",                           // Name of the storage media.
                            &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    OSTimeDlyHMSM(0u, 0u, 20u, 0u,                              // Delay of 20 seconds.
                  OS_OPT_TIME_HMSM_NON_STRICT,
                  &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    USBD_MSC_SCSI_LunDetach(Ex_USBD_MSC_ClassNbr,               // MSC instance number.
                            Ex_USBD_MSC_LU_Nbr,                 // Logical unit number.
                            &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    OSTimeDlyHMSM(0u, 0u, 5u, 0u,                               // Delay of 5 seconds.
                  OS_OPT_TIME_HMSM_NON_STRICT,
                  &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  }

  USBD_MSC_SCSI_LunAttach(Ex_USBD_MSC_ClassNbr,                 // MSC instance number.
                          Ex_USBD_MSC_LU_Nbr,                   // Logical unit number.
                          Ex_USBD_MSC_MediaNamePtr,             // Name of the storage media.
                          &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  OSTaskDel(DEF_NULL,                                           // Task kills itself.
            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_DEV_MSC_AVAIL && RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
