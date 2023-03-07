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

#include  "ex_fs_file_multi_desc.h"
#include  "ex_fs_utils.h"
#include  "ex_fs.h"

#include  <common/include/rtos_utils.h>
#include  <kernel/include/os.h>

#include  <fs/include/fs_core_file.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_FS_TASK_RD_PRIO                     10u
#define  EX_FS_TASK_RD_STK_SIZE                 768u
#define  EX_FS_FILE_MULTI_DESC_LB_QTY_MIN       20u
#define  EX_FS_FILE_MULTI_DESC_LB_QTY_MAX       50u

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

typedef  struct  ex_fs_task_info {
  CPU_CHAR      *FilePathPtr;
  FS_VOL_HANDLE VolHandle;
  CPU_INT32U    DevLbSize;
  CPU_INT32U    LbCntMax;
  OS_SEM        *ReaderSemPtr;
} EX_FS_TASK_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static OS_TCB  Ex_FS_TaskRdTCB;
static CPU_STK Ex_FS_TaskRdStk[EX_FS_TASK_RD_STK_SIZE];

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_FS_FileMultiDesc_Exec(FS_VOL_HANDLE vol_handle,
                                     CPU_CHAR      *p_file_name,
                                     RTOS_ERR      *p_err);

static void Ex_FS_TaskRd(void *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                     LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_TASK_DEL_EN != DEF_ENABLED)
#error "File multi-descriptor example requires OS_CFG_TASK_DEL_EN set to DEF_ENABLED in os_cfg.h."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_FS_FileMultiDesc()
 *
 * @brief  Perform file multi-descriptor example.
 *******************************************************************************************************/
void Ex_FS_FileMultiDesc(void)
{
  FS_MEDIA_HANDLE media_handle;
  FS_VOL_HANDLE   vol_handle;
  RTOS_ERR        err;

  media_handle = FSMedia_Get(EX_CFG_FS_ACTIVE_MEDIA_NAME);      // Get media handle.
  APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle),; );
  EX_TRACE("FS Example: File multi-descriptor on '%s'...", EX_CFG_FS_ACTIVE_MEDIA_NAME);

  //                                                               Open device and volume.
  vol_handle = Ex_FS_VolOpen(media_handle,
                             "vol2",                            // Unique volume name.
                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               Execute file multi-descriptor example.
  Ex_FS_FileMultiDesc_Exec(vol_handle,
                           "file-multi-desc.txt",
                           &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Ex_FS_VolClose(vol_handle, &err);                             // Close volume and device.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("OK\n\r");
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Ex_FS_FileMultiDesc_Exec()
 *
 * @brief  This example will do the following main steps:
 *           - (a) Create a Reader task.
 *           - (b) Writer task writes N logical blocks to the specified file. The Writer task is
 *                 represented by the function Ex_FS_FileMultiDesc() called from the application
 *                 Start task.
 *           - (c) Reader task reads N logical blocks from the specified file and verifies data
 *                 read.
 *         @n
 *         Writer and Reader tasks will open the specified file with their own file descriptor.
 *
 * @param  vol_handle   Volume handle.
 *
 * @param  p_file_name  Pointer to file name.
 *
 * @param  p_err        Pointer to variable that will receive the return error code from this
 *                      function.
 *
 * @note   (1) If the media type is a RAM disk, the maximum number of logical block counts used
 *             to write a file is kept to a minimum. Other media such as NAND, NOR and SD can
 *             handle a bigger file size.
 *******************************************************************************************************/
void Ex_FS_FileMultiDesc_Exec(FS_VOL_HANDLE vol_handle,
                              CPU_CHAR      *p_file_name,
                              RTOS_ERR      *p_err)
{
  CPU_INT08U        *p_tx_buf;
  OS_SEM            *p_sem_sync;
  CPU_CHAR          *p_file_path_buf;
  FS_BLK_DEV_HANDLE blk_dev_handle;
  FS_MEDIA_HANDLE   media_handle;
  FS_MEDIA_TYPE     media_type;
  FS_LB_SIZE        dev_lb_size;
  FS_FILE_HANDLE    file_wr_handle;
  EX_FS_TASK_INFO   task_info;
  CPU_SIZE_T        expected_path_len;
  CPU_INT08U        pattern_start = 0u;
  CPU_INT32U        lb_cnt = 0u;
  CPU_INT32U        lb_cnt_max;
  MEM_SEG_INFO      seg_info;
  CPU_SIZE_T        seg_rem_size;
  RTOS_ERR          local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  //                                                               ------------------ INITIALIZATION ------------------
  blk_dev_handle = FSVol_BlkDevGet(vol_handle);                 // Get block device handle from volume handle.
  APP_RTOS_ASSERT_CRITICAL((FS_BLK_DEV_HANDLE_IS_NULL(blk_dev_handle) == 0),; );

  dev_lb_size = FSBlkDev_LbSizeGet(blk_dev_handle, p_err);      // Get block device logical block size.
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error getting device logical block size w/ err %d\r\n", p_err->Code);
    return;
  }
  //                                                               Ensure enough space to allocate internal resources.
  seg_rem_size = Mem_SegRemSizeGet(&Ex_FS_MemSeg, sizeof(CPU_ALIGN), &seg_info, &local_err);
  APP_RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE),; );
  APP_RTOS_ASSERT_CRITICAL((seg_rem_size > (dev_lb_size * 2 + sizeof(OS_SEM) + EX_FS_MAX_PATH_NAME_LEN)),; );

  //                                                               Set max logical count depending of media type...
  //                                                               ...(see Note #1).
  media_handle = FSBlkDev_MediaGet(blk_dev_handle);
  media_type = FSMedia_TypeGet(media_handle, p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error getting media type w/ err %d\r\n", p_err->Code);
    return;
  }
  if (media_type == FS_MEDIA_TYPE_RAM_DISK) {
    lb_cnt_max = EX_FS_FILE_MULTI_DESC_LB_QTY_MIN;
  } else {
    lb_cnt_max = EX_FS_FILE_MULTI_DESC_LB_QTY_MAX;
  }

  //                                                               Allocate Tx buffer.
  p_tx_buf = (CPU_INT08U *)Mem_SegAlloc("Ex - FS file multi-desc tx buf",
                                        &Ex_FS_MemSeg,
                                        dev_lb_size,
                                        p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error allocating Tx buf w/ err %d\r\n", p_err->Code);
    return;
  }
  //                                                               Allocate buffer for file path.
  p_file_path_buf = (CPU_CHAR *)Mem_SegAlloc("Ex - FS file multi-desc path buf",
                                             &Ex_FS_MemSeg,
                                             EX_FS_MAX_PATH_NAME_LEN,
                                             p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileRdWr(): Error allocating file path buf w/ err %d", p_err->Code);
    goto end_mem_seg;
  }

  Mem_Clr((void *)p_file_path_buf, EX_FS_MAX_PATH_NAME_LEN);
  //                                                               Get volume name associated to volume handle.
  FSVol_NameGet(vol_handle,
                p_file_path_buf,
                EX_FS_MAX_PATH_NAME_LEN,
                p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileRdWr(): Error getting volume name w/ err %d", p_err->Code);
    goto end_mem_seg;
  }
  //                                                               Build full file path.
  expected_path_len = Str_Len(p_file_path_buf)
                      + Str_Len(p_file_name)
                      + 1u;
  APP_RTOS_ASSERT_CRITICAL((expected_path_len < EX_FS_MAX_PATH_NAME_LEN),; );
  Str_Cat(p_file_path_buf, "/");
  Str_Cat(p_file_path_buf, p_file_name);
  //                                                               Open file in write only mode.
  file_wr_handle = FSFile_Open(FS_WRK_DIR_NULL,                 // NULL working directory means absolute path used.
                               p_file_path_buf,                 // Buffer containing full path to file.
                               FS_FILE_ACCESS_MODE_CREATE       // File created if it does not exist.
                               | FS_FILE_ACCESS_MODE_WR         // File can be read or written.
                               | FS_FILE_ACCESS_MODE_TRUNCATE,  // If file exists, its size is truncated to 0 bytes.
                               p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error opening file w/ err %d\r\n", p_err->Code);
    goto end_mem_seg;
  }
  //                                                               ----------- TASKS AND SERVICES CREATION ------------
  //                                                               Sem to sync this task (Writer) and Reader task.
  p_sem_sync = (OS_SEM *)Mem_SegAlloc("Ex - FS file multi-desc sync sem",
                                      &Ex_FS_MemSeg,
                                      sizeof(OS_SEM),
                                      p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error allocating sync sem w/ err %d\r\n", p_err->Code);
    goto end_file;
  }
  OSSemCreate(p_sem_sync, DEF_NULL, 0, p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error creating sync sem w/ err %d\r\n", p_err->Code);
    goto end_file;
  }
  //                                                               Create Reader task passing all necessary info as arg.
  task_info.FilePathPtr = p_file_path_buf;
  task_info.VolHandle = vol_handle;
  task_info.DevLbSize = dev_lb_size;
  task_info.LbCntMax = lb_cnt_max;
  task_info.ReaderSemPtr = p_sem_sync;

  OSTaskCreate(&Ex_FS_TaskRdTCB,
               "Task Reader",
               Ex_FS_TaskRd,
               &task_info,
               EX_FS_TASK_RD_PRIO,
               &Ex_FS_TaskRdStk[0],
               (EX_FS_TASK_RD_STK_SIZE / 10u),
               EX_FS_TASK_RD_STK_SIZE,
               0,
               0,
               DEF_NULL,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error creating Reader task w/ err %d\r\n", p_err->Code);
    goto end_sem;
  }
  //                                                               ---------------- EXAMPLE EXECUTION -----------------
  while (lb_cnt < lb_cnt_max) {
    //                                                             Write buffer with known pattern.
    Ex_FS_BufFill(p_tx_buf, dev_lb_size, pattern_start);
    //                                                             Write 1 block of data to file.
    FSFile_Wr(file_wr_handle,
              (void *)p_tx_buf,                                 // Buffer containing data to write.
              dev_lb_size,                                      // Size of buffer in bytes.
              p_err);
    if (p_err->Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error writing file w/ err %d\r\n", p_err->Code);
      goto end_task;
    }

    lb_cnt++;                                                   // Increase logical block count.
    if (lb_cnt == (lb_cnt_max / 4)) {                           // If enough data written, start Reader task.
      OSTaskSemPost(&Ex_FS_TaskRdTCB,
                    OS_OPT_POST_NONE,
                    p_err);
      if (p_err->Code != RTOS_ERR_NONE) {
        EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error posting semaphore w/ err %d\r\n", p_err->Code);
        goto end_task;
      }
    }

    pattern_start++;

    OSTimeDly(15u, OS_OPT_TIME_DLY, &local_err);
  }
  //                                                               Wait for Reader task completion.
  OSSemPend(p_sem_sync,
            0,
            OS_OPT_PEND_BLOCKING,
            DEF_NULL,
            p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileMultiDesc_Exec(): Error waiting Reader task completion w/ err %d\r\n", p_err->Code);
  }

  //                                                               --------------------- CLOSING ----------------------
end_task:
  OSTaskDel(&Ex_FS_TaskRdTCB, &local_err);                    // Delete Reader task.
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
end_sem:
  OSSemDel(p_sem_sync, OS_OPT_DEL_ALWAYS, &local_err);          // Delete synchronization semaphore.
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
end_file:
  FSFile_Close(file_wr_handle, &local_err);                     // Close file.
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
end_mem_seg:
  Mem_SegClr(&Ex_FS_MemSeg, &local_err);                        // Clear memory segment for next example.
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                             Ex_FS_TaskRd()
 *
 * @brief  Task that reads some data from a specified file and verify the data read. This task works
 *         with a Writer task represented by the function Ex_FS_FileMultiDesc_Exec().
 *
 * @param  p_arg  Pointer to argument passed to the task upon its creation.
 *******************************************************************************************************/
static void Ex_FS_TaskRd(void *p_arg)
{
  EX_FS_TASK_INFO *p_task_info = (EX_FS_TASK_INFO *)p_arg;
  CPU_INT08U      *p_rx_buf;
  FS_FILE_HANDLE  file_rd_handle;
  CPU_INT08U      pattern_start = 0u;
  CPU_INT32U      lb_cnt = 0u;
  CPU_SIZE_T      data_rd_bytes;
  CPU_BOOLEAN     ok;
  CPU_BOOLEAN     eof;
  CPU_INT32U      eof_cnt;
  RTOS_ERR        err;

  //                                                               Allocate a read buffer.
  p_rx_buf = (CPU_INT08U *)Mem_SegAlloc("Ex - FS file multi-desc rx buf",
                                        &Ex_FS_MemSeg,
                                        p_task_info->DevLbSize,
                                        &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               Open file in read mode.
  file_rd_handle = FSFile_Open(FS_WRK_DIR_NULL,                 // NULL working directory means absolute path used.
                               p_task_info->FilePathPtr,        // Absolute path to file to create.
                               FS_FILE_ACCESS_MODE_RD,          // File can be only read.
                               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Wait enough data written to file by Writer task.
  OSTaskSemPend(0u,
                OS_OPT_PEND_BLOCKING,
                DEF_NULL,
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  while (lb_cnt < p_task_info->LbCntMax) {
    //                                                             Read 1 block of data from file.
    data_rd_bytes = FSFile_Rd(file_rd_handle,
                              (void *)p_rx_buf,                 // Buffer receiving data from file.
                              p_task_info->DevLbSize,           // Size of buffer in bytes.
                              &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
    if (data_rd_bytes == p_task_info->DevLbSize) {
      //                                                           Verify data.
      ok = Ex_FS_BufValidate(p_rx_buf, p_task_info->DevLbSize, pattern_start);
      APP_RTOS_ASSERT_CRITICAL(ok,; );

      lb_cnt++;
      pattern_start++;
    } else {
      eof = FSFile_IsEOF(file_rd_handle, &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      if (eof) {                                                // Increment this local counter if end of file reached.
        eof_cnt++;
      }
    }

    OSTimeDly(5u, OS_OPT_TIME_DLY, &err);
  }

  FSFile_Close(file_rd_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               Inform Writer task about completion.
  OSSemPost(p_task_info->ReaderSemPtr,
            OS_OPT_POST_1,
            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Reader task deleted by Writer task.
  OSTaskSemPend(0u,                                             // This semaphore pend is forever until task deleted.
                OS_OPT_PEND_BLOCKING,
                DEF_NULL,
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
