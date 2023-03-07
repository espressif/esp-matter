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
#include  <fs/include/fs_core.h>
#include  <usb/include/device/usbd_scsi_storage_blk_dev.h>
#include  <fs/include/fs_ramdisk.h>
#include  <fs/include/fs_storage.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_USBD_MSC_STARTUP_PRIO                         21u
#define  EX_USBD_MSC_RAMDISK_SEC_SIZE                    512u
#define  EX_USBD_MSC_RAMDISK_SEC_NBR                      44u
#define  EX_USBD_MSC_RAMDISK_LU_QTY_UNLIMITED              0u

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_USBD_MSC_Ramdisk_LUN_HostEjectEvent(CPU_INT08U lu_nbr);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT08U Ex_USBD_MSC_RAMDisk[EX_USBD_MSC_RAMDISK_SEC_SIZE * EX_USBD_MSC_RAMDISK_SEC_NBR];

/********************************************************************************************************
 *                                  MSC LOGICAL UNIT CALLBACK STRUCT
 *
 * Note(s) : (1) MSC offers a callback to notify the application when the logical unit is ejected from the
 *               host (with a right-clik -> Eject on the drive icon from Computer on a Windows PC, for
 *               example)
 *******************************************************************************************************/

USBD_SCSI_LU_FNCTS Ex_USBD_MSC_Ramdisk_Lun_Fncts = {
  .HostEjectEvent = Ex_USBD_MSC_Ramdisk_LUN_HostEjectEvent
};

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
  USBD_MSC_QTY_CFG  msc_quantity_conf;
  FS_RAM_DISK_CFG   ramdisk_cfg;

  ramdisk_cfg.DiskPtr = Ex_USBD_MSC_RAMDisk;
  ramdisk_cfg.LbCnt = EX_USBD_MSC_RAMDISK_SEC_NBR;
  ramdisk_cfg.LbSize = EX_USBD_MSC_RAMDISK_SEC_SIZE;

  //                                                               Add RAM Disk instance to file system.
  FS_RAM_Disk_Add("ramusb",                                     // String identifying this RAM Disk instance.
                  &ramdisk_cfg,                                 // Configuration describing RAM area.
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  msc_quantity_conf.ClassInstanceQty = 1u;
  msc_quantity_conf.ConfigQty = 2u;
  msc_quantity_conf.LUN_Qty = 1u;

  USBD_MSC_Init(&msc_quantity_conf,                             // Pointer to the MSC configuration structure.
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----------- CREATE AN MSC CLASS INSTANCE -----------
  //                                                               Each MSC class instance requires a task.
  //                                                               TODO: Adapt task configuration to your need.
  task_cfg.Prio = EX_USBD_MSC_STARTUP_PRIO;
  task_cfg.StkPtr = DEF_NULL;
  task_cfg.StkSizeElements = 512u;

  msc_nbr = USBD_MSC_Add(&task_cfg,                             // Pointer to the configuration structure
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
                                //                                 Callback functions. Optional, can be DEF_NULL.
                                &Ex_USBD_MSC_Ramdisk_Lun_Fncts,
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
  //                                                               ----- ATTACH A STORAGE MEDIA TO THE GIVEN LUN. -----
  USBD_MSC_SCSI_LunAttach(msc_nbr,                              // MSC instance number.
                          lu_nbr,                               // Logical unit number.
                          "ramusb",                             // Name of the storage media.
                          &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                Ex_USBD_MSC_Ramdisk_LUN_HostEjectEvent()
 *
 * @brief  Logical unit host eject event callback.
 *
 * @param  lu_nbr  Logical unit number.
 *
 * @note   (1) This function will be called when an eject operation is performed from the host.
 *******************************************************************************************************/
static void Ex_USBD_MSC_Ramdisk_LUN_HostEjectEvent(CPU_INT08U lu_nbr)
{
  PP_UNUSED_PARAM(lu_nbr);

  // TODO Host has eject the device, you can now call a USBD_MSC_SCSI_LunDetach().
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_DEV_MSC_AVAIL && RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
