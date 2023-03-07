/***************************************************************************//**
 * @file
 * @brief File System - Core File Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_file_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/core/fs_core_op_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>
#include  <fs/source/core/fs_core_working_dir_priv.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>
#include  <common/source/collections/slist_priv.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                              (FS, CORE, FILE)
#define  RTOS_MODULE_CUR                          RTOS_CFG_MODULE_FS

//                                                                 Buffer status.
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
#define  FS_FILE_BUF_STATUS_NONE                  0u
#define  FS_FILE_BUF_STATUS_EMPTY                 1u
#define  FS_FILE_BUF_STATUS_NONEMPTY_RD           2u
#define  FS_FILE_BUF_STATUS_NONEMPTY_WR           3u
#endif

//                                                                 IO state flags.
#define  FS_FILE_IO_STATE_NONE                    DEF_BIT_NONE
#define  FS_FILE_IO_STATE_RD                      DEF_BIT_00
#define  FS_FILE_IO_STATE_WR                      DEF_BIT_01

#define  FS_FILE_COPY_LB_TBL_MAX_SIZE             4u            // Maximum nbr of lb's gathered up front while copying.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_FILE_HANDLE FSFile_NullHandle = { 0 };

static SLIST_MEMBER *FSFile_OpenDescListHeadPtr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ REFERENCE ACQUIRE/RELEASE -------------
#define  FS_FILE_WITH(file_handle, p_err)          FS_OBJ_WITH(file_handle, p_err, FSFile_Desc, RTOS_ERR_ENTRY_CLOSED)

#define  FS_FILE_WITH_NO_IO(file_handle, p_err)    FS_OBJ_WITH_NO_IO(file_handle, p_err, FSFile_Desc, RTOS_ERR_ENTRY_CLOSED)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------ BUFFER ACCESS -------------------
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
static void FSFile_BufRd(FS_FILE_DESC *p_desc,
                         void         *p_dest,
                         FS_FILE_SIZE file_pos,
                         CPU_SIZE_T   size,
                         RTOS_ERR     *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_SIZE_T FSFile_BufWr(FS_FILE_DESC *p_desc,
                               void         *p_src,
                               CPU_SIZE_T   size,
                               RTOS_ERR     *p_err);
#endif

static void FSFile_BufEmpty(FS_FILE_DESC *p_desc,
                            RTOS_ERR     *p_err);
#endif

//                                                                 -------------------- FILE WRITE --------------------
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_SIZE_T FSFile_WrInternal(FS_FILE_DESC *p_desc,
                                    CPU_INT08U   *p_src,
                                    FS_FILE_SIZE wr_size,
                                    RTOS_ERR     *p_err);

#if (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED)
static void FSFile_IntraCacheCopy(FS_CACHE   *p_cache,
                                  FS_VOL     *p_src_vol,
                                  FS_VOL     *p_dest_vol,
                                  FS_SYS_POS src_entry_pos,
                                  FS_SYS_POS dest_entry_pos,
                                  CPU_SIZE_T src_file_size,
                                  RTOS_ERR   *p_err);

static void FSFile_InterCacheCopy(FS_VOL     *p_src_vol,
                                  FS_VOL     *p_dest_vol,
                                  FS_SYS_POS src_entry_pos,
                                  FS_SYS_POS dest_entry_pos,
                                  CPU_SIZE_T src_file_size,
                                  RTOS_ERR   *p_err);
#endif
#endif

//                                                                 --------------------- FILE RD ----------------------
static CPU_SIZE_T FSFile_RdInternal(FS_FILE_DESC *p_desc,
                                    CPU_INT08U   *p_dest,
                                    FS_FILE_SIZE pos,
                                    FS_FILE_SIZE rd_size,
                                    RTOS_ERR     *p_err);

//                                                                 --------------------- ARG CHK ----------------------
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static CPU_BOOLEAN FSFile_AccessModeIsValid(FS_FLAGS mode);
#endif

//                                                                 --------------- ON NULL REF CNT CB'S ---------------
static void FSFile_Desc_OnNullRefCnt(FS_OBJ *p_obj);

static void FSFile_Node_OnNullRefCnt(FS_OBJ *p_obj);

static FS_OBJ_HANDLE FSFile_Desc_ToObjHandle(FS_FILE_HANDLE file_handle);

static FS_OBJ *FSFile_Desc_ToObj(FS_FILE_DESC *p_file_desc);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSFile_Open()
 *
 * @brief    Open a file.
 *
 * @param    wrk_dir_handle  Handle to a working directory (see Note #1).
 *
 * @param    path            File path relative to the given working directory.
 *
 * @param    mode            File access mode; valid bit-wise OR of one or more of the following
 *                           (see Note #2):
 *                               - FS_FILE_ACCESS_MODE_RD          File opened for reads.
 *                               - FS_FILE_ACCESS_MODE_WR          File opened for writes.
 *                               - FS_FILE_ACCESS_MODE_CREATE      File will be created, if necessary.
 *                               - FS_FILE_ACCESS_MODE_TRUNCATE    File length will be truncated to 0.
 *                               - FS_FILE_ACCESS_MODE_APPEND      All writes will be performed at EOF.
 *                               - FS_FILE_ACCESS_MODE_EXCL        File will be opened if & only if it
 *                                                                 does not already exist.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_ALREADY_EXISTS
 *                               - RTOS_ERR_TYPE_INVALID
 *                               - RTOS_ERR_NAME_INVALID
 *                               - RTOS_ERR_PARENT_NOT_DIR
 *                               - RTOS_ERR_MAX_DEPTH_EXCEEDED
 *                               - RTOS_ERR_DIR_FULL
 *                               - RTOS_ERR_WRK_DIR_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_FULL
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @return   Handle to the opened file descriptor.
 *
 * @note     (1) If no working directory has been opened with FSWrkDir_Open(), you can pass a NULL
 *               working directory handle using the #define FS_WRK_DIR_NULL.
 *
 * @note     (2) Micrium OS File System native file access modes along with equivalent standard
 *               'fopen()' access modes.
 *               Combinations not listed below will generate an error. The FS_FILE_ACCESS_MODE_EXCL
 *               flag is allowed wherever FS_FILE_ACCESS_MODE_CREATE is allowed.
 *               @verbatim
 *               ---------------------------------------------------------------
 *               |         NATIVE ACCESS MODES          |  POSIX ACCESS MODES  |
 *               ---------------------------------------|-----------------------
 *               | RD                                   | "r"  / "rb"          |
 *               |      WR                              |                      |
 *               |      WR                     | APPEND |                      |
 *               |      WR          | TRUNCATE          |                      |
 *               |      WR          | TRUNCATE | APPEND |                      |
 *               |      WR | CREATE                     |                      |
 *               |      WR | CREATE            | APPEND | "a"  / "ab"          |
 *               |      WR | CREATE | TRUNCATE          | "w"  / "wb"          |
 *               |      WR | CREATE | TRUNCATE | APPEND |                      |
 *               | RD | WR                              | "r+" / "rb+" / "r+b" |
 *               | RD | WR                     | APPEND |                      |
 *               | RD | WR          | TRUNCATE          |                      |
 *               | RD | WR          | TRUNCATE | APPEND |                      |
 *               | RD | WR | CREATE | TRUNCATE          | "w+" / "wb+" / "w+b" |
 *               | RD | WR | CREATE            | APPEND | "a+" / "ab+" / "a+b" |
 *               | RD | WR            TRUNCATE | APPEND |                      |
 *               | RD | WR | CREATE | TRUNCATE | APPEND |                      |
 *               ---------------------------------------------------------------
 *               @endverbatim
 *
 * @note     (3) The depth of the directory tree may NOT exceed 255, counting from the virtual file
 *               system root directory. The 'RTOS_ERR_WOULD_OVF' error is returned whenever this limit
 *               is exceeded. This can only happen when the 'FS_FILE_ACCESS_MODE_CREATE' access mode
 *               is used.
 *
 * @note     (4) If the entry located at the specified path is a directory, the 'RTOS_ERR_INVALID_TYPE'
 *               is returned.
 *******************************************************************************************************/
FS_FILE_HANDLE FSFile_Open(FS_WRK_DIR_HANDLE wrk_dir_handle,
                           const CPU_CHAR    *path,
                           FS_FLAGS          mode,
                           RTOS_ERR          *p_err)
{
  FS_FILE_DESC         *p_file_desc = DEF_NULL;
  FS_FILE_NODE         *p_file_node;
  FS_VOL               *p_vol = DEF_NULL;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  FS_FILE_HANDLE       file_handle;
  CORE_DECLARE_IRQ_STATE;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, FSFile_NullHandle);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    //                                                             Check that Core layer is initialized.
    CORE_ENTER_ATOMIC();
    if (!FSCore_Data.IsInit) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      CORE_EXIT_ATOMIC();
      break;
    }
    CORE_EXIT_ATOMIC();

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
    RTOS_ASSERT_DBG_ERR_SET(FSFile_AccessModeIsValid(mode), *p_err,
                            RTOS_ERR_INVALID_ARG, FSFile_NullHandle);
#endif

    file_handle = FSFile_NullHandle;

    BREAK_ON_ERR(entry_node_handle = FSEntry_NodeOpen(wrk_dir_handle,
                                                      (CPU_CHAR *)path,
                                                      mode,
                                                      FS_ENTRY_TYPE_FILE,
                                                      p_err));
    ON_BREAK {
      RTOS_ERR err_tmp;
      FSEntry_NodeClose(entry_node_handle, FSFile_Node_OnNullRefCnt, &err_tmp);
      break;
    } WITH {
      p_file_node = (FS_FILE_NODE *)entry_node_handle.EntryNodePtr;

      BREAK_ON_ERR(FS_VOL_WITH) (p_file_node->VolHandle, p_err) {
        p_vol = p_file_node->VolHandle.VolPtr;

        ASSERT_BREAK(!((p_vol->AccessMode == FS_VOL_STATUS_ACCESS_MODE_RD_ONLY)
                       && (DEF_BIT_IS_SET(mode, FS_FILE_ACCESS_MODE_WR) == 1u)),
                     RTOS_ERR_VOL_RD_ONLY);

        //                                                         ---------------- CREATE ENTRY DESC -----------------
        //                                                         See Note #1e.
        BREAK_ON_ERR(p_file_desc = p_file_node->SysPtr->FileDescAlloc(p_err));

        ON_BREAK {
          p_file_node->SysPtr->FileDescFree(p_file_desc);
          break;
        } WITH {
          //                                                       -------------------- DESC INIT ---------------------
          p_file_desc->Pos = 0u;
          p_file_desc->RdOnly = DEF_BIT_IS_CLR(mode, FS_FILE_ACCESS_MODE_WR);
          p_file_desc->Append = DEF_BIT_IS_SET(mode, FS_FILE_ACCESS_MODE_APPEND);
          p_file_desc->IsEOF = DEF_NO;
          p_file_desc->IsErr = DEF_NO;
          p_file_desc->IOStateFlags = FS_FILE_IO_STATE_NONE;
          p_file_desc->Id = FSObj_CurId++;
          p_file_desc->RefCnt = 1u;
          p_file_desc->IOCnt = 1u;
          p_file_desc->IsClosing = DEF_NO;
          p_file_desc->FileNodePtr = p_file_node;
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
          p_file_desc->BufModeFlags = FS_FILE_BUF_MODE_NONE;
          p_file_desc->BufStatus = FS_FILE_BUF_STATUS_NONE;
          p_file_desc->BufData.BufPtr = DEF_NULL;
          p_file_desc->BufData.WrSeqNo = 0u;
          p_file_desc->BufData.Pos = 0u;
          p_file_desc->BufData.Size = 0u;
#endif

          FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR) {
            SList_Push(&FSFile_OpenDescListHeadPtr, &p_file_desc->OpenListMember);
          }

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
          {
            KAL_LOCK_EXT_CFG lock_cfg;
            lock_cfg.Opt = KAL_OPT_CREATE_REENTRANT;
            BREAK_ON_ERR(p_file_desc->FileLockHandle = KAL_LockCreate("FS file lock",
                                                                      &lock_cfg,
                                                                      p_err));
          }
          ON_BREAK {
            KAL_LockDel(p_file_desc->FileLockHandle);
            break;
          } WITH
#endif
          {
            BREAK_ON_ERR(p_file_node->SysPtr->FileDescInit(p_file_desc, p_err));

            //                                                     --------------------- TRUNCATE ---------------------
            if ((p_file_node->Size != 0u)
                && DEF_BIT_IS_SET(mode, FS_FILE_ACCESS_MODE_TRUNCATE)) {
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
              BREAK_ON_ERR(p_vol->SysPtr->FileTruncate(p_vol,
                                                       p_file_node,
                                                       p_file_node->Pos,
                                                       0u,
                                                       p_err));
              p_file_node->Size = 0u;
#else
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, FSFile_NullHandle);
#endif
            }
          }
        }
      }
    }

    file_handle.FileDescPtr = p_file_desc;
    file_handle.FileDescId = p_file_desc->Id;

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
    if ((p_vol != DEF_NULL) && p_vol->AutoSync) {
      BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));
    }
#endif
  } WITH_SCOPE_END

  return (file_handle);
}

/****************************************************************************************************//**
 *                                               FSFile_Close()
 *
 * @brief    Close a file.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @internal
 * @note     (1) [INTERNAL] This function decreases the file descriptor reference count by one. It does
 *               NOT flush the associated file buffer, update the on-disk metadata and free the related
 *               data structures. These operations are deferred until the null reference count callback
 *               is called (see FSFile_OnNullDescRefCnt()).
 * @endinternal
 *******************************************************************************************************/
void FSFile_Close(FS_FILE_HANDLE file_handle,
                  RTOS_ERR       *p_err)
{
  FS_FILE_DESC *p_desc = DEF_NULL;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_FILE_WITH_NO_IO(file_handle, p_err) {
      p_desc = file_handle.FileDescPtr;
      ASSERT_BREAK(!FSObj_IsClosingTestSet(FSFile_Desc_ToObj(p_desc)),
                   RTOS_ERR_ENTRY_CLOSED);
    }

    FSObj_IOEndWait(FSFile_Desc_ToObj(p_desc));

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
    FS_VOL_WITH(p_desc->FileNodePtr->VolHandle, p_err) {
      if (p_desc->BufData.BufPtr != DEF_NULL) {
        FSFile_BufEmpty(p_desc, p_err);
      }
    }
#endif

    FSObj_RefRelease(FSFile_Desc_ToObj(p_desc), FSFile_Desc_OnNullRefCnt);
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSFile_PosGet()
 *
 * @brief    Get file position indicator.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *
 * @return   The current file position measured in bytes from the beginning of the file.
 *******************************************************************************************************/
FS_FILE_SIZE FSFile_PosGet(FS_FILE_HANDLE file_handle,
                           RTOS_ERR       *p_err)
{
  FS_FILE_SIZE pos;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  pos = 0u;
  FS_FILE_WITH(file_handle, p_err) {
    CORE_ENTER_ATOMIC();
    pos = file_handle.FileDescPtr->Pos;
    CORE_EXIT_ATOMIC();
  }

  return (pos);
}

/****************************************************************************************************//**
 *                                               FSFile_PosSet()
 *
 * @brief    Set file position indicator.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    offset          Offset from the file position specified by 'origin'.
 *
 * @param    origin          Reference position for offset :
 *                               - FS_FILE_ORIGIN_START    Offset is from the beginning of the file.
 *                               - FS_FILE_ORIGIN_CUR      Offset is from current file position.
 *                               - FS_FILE_ORIGIN_END      Offset is from the end of the file.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                           @n
 *                           If a file buffer in write mode is in use, the following error codes may be
 *                           triggered as well:
 *                               - RTOS_ERR_VOL_FULL
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) The new file position, measured in bytes from the beginning of the file is
 *               obtained by adding 'offset' to...
 *               - (a) ...0 (the beginning of the file) if 'origin' is FS_FILE_ORIGIN_START,
 *               - (b) ...the current file position     if 'origin' is FS_FILE_ORIGIN_CUR.
 *               - (c) ...the file size                 if 'origin' is FS_FILE_ORIGIN_END.
 *               The end-of-file indicator is cleared.
 *
 * @note     (2) If the file position indicator is set beyond the file's current data...
 *               - (a) ...and data is later written to that point, reads from the gap will read 0.
 *               - (b) ...the file MUST be opened in write or read/write mode.
 *
 * @note     (3) Locking is needed to protect the file buffer from being emptied and/or the current
 *               position from being changed while a write operation is being performed.
 *******************************************************************************************************/
void FSFile_PosSet(FS_FILE_HANDLE file_handle,
                   FS_FILE_OFFSET offset,
                   FS_FLAGS       origin,
                   RTOS_ERR       *p_err)
{
  FS_FILE_DESC   *p_desc;
  FS_FILE_NODE   *p_node;
  FS_VOL         *p_vol;
  FS_FILE_OFFSET chk_pos;
  FS_FILE_SIZE   start_pos;
  FS_FILE_SIZE   cur_pos;
  FS_FILE_SIZE   cur_size;
  CPU_BOOLEAN    is_rd_only;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET((origin == FS_FILE_ORIGIN_START)
                            || (origin == FS_FILE_ORIGIN_CUR)
                            || (origin == FS_FILE_ORIGIN_END),
                            *p_err, RTOS_ERR_INVALID_ARG,; );

    FS_FILE_WITH(file_handle, p_err) {
      p_desc = file_handle.FileDescPtr;
      p_node = p_desc->FileNodePtr;
      p_vol = p_node->VolHandle.VolPtr;

      PP_UNUSED_PARAM(p_vol);                                   // Removed GCC warning (Variable set but not used).

      FS_VOL_WITH(p_node->VolHandle, p_err) {
        //                                                         See Note #3.
        FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_FILE_WR) {
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
          if (p_desc->BufData.BufPtr != DEF_NULL) {             // Flush file buf before pos set if any.
            BREAK_ON_ERR(FSFile_BufEmpty(p_desc, p_err));
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
            BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));
#endif
          }
#endif

          FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
            if ((offset != 0u) || (origin != FS_FILE_ORIGIN_CUR)) {
              //                                                   ------------------- CALC NEW POS -------------------
              is_rd_only = p_desc->RdOnly;
              cur_pos = p_desc->Pos;
              cur_size = p_desc->FileNodePtr->Size;

              //                                                   Start (ref) pos for offset (see Note #1).
              start_pos = origin == FS_FILE_ORIGIN_CUR ? cur_pos
                          : origin == FS_FILE_ORIGIN_END ? cur_size : 0u;

              if (offset < 0) {                                 // Neg offset ... chk for neg ovf.
                if (offset == FS_FILE_OFFSET_MIN) {
                  if ((FS_FILE_SIZE)FS_FILE_OFFSET_MAX + 1u > start_pos) {
                    RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_ARG,; );
                  }
                } else {
                  chk_pos = (FS_FILE_OFFSET)start_pos + offset;
                  if (chk_pos < 0) {
                    RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_ARG,; );
                  }
                }
              } else {
                if (offset > 0) {                               // Pos offset ... chk for pos ovf.
                  if (FS_FILE_SIZE_MAX - (FS_FILE_SIZE)offset < start_pos) {
                    RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_ARG,; );
                  }
                  //                                               Chk file mode (see Note #2b).
                  if ((FS_FILE_SIZE)offset + start_pos > cur_size) {
                    if (is_rd_only) {
                      RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_ARG,; );
                    }
                  }
                }
              }

              p_desc->Pos = start_pos + offset;
            }

            p_desc->IsEOF = DEF_NO;                             // See Note #1.
            p_desc->IOStateFlags = FS_FILE_IO_STATE_NONE;
          }
        }
      }
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSFile_IsEOF()
 *
 * @brief    Test EOF indicator on a file.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *
 * @return   DEF_YES, if EOF indicator is set.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) FSFile_ErrClr() can be used to clear the EOF indicator.
 *******************************************************************************************************/
CPU_BOOLEAN FSFile_IsEOF(FS_FILE_HANDLE file_handle,
                         RTOS_ERR       *p_err)
{
  CPU_BOOLEAN is_eof;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  is_eof = DEF_NO;
  FS_FILE_WITH(file_handle, p_err) {
    CORE_ENTER_ATOMIC();
    is_eof = file_handle.FileDescPtr->IsEOF;
    CORE_EXIT_ATOMIC();
  }

  return (is_eof);
}

/****************************************************************************************************//**
 *                                               FSFile_Query()
 *
 * @brief    Get information about a file.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_entry_info    Pointer to structure that will receive the file information.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @internal
 * @note     (1) [INTERNAL] Access to an entry's metadata is presumed atomic with respect to other
 *               entry metadata accesses. It is the file system driver responsibility to fulfill this
 *               requirement.
 *
 * @note     (2) [INTERNAL] An opened entry may not be deleted. The entry is thus guaranteed to remain
 *               on disk while the query is being performed.
 * @endinternal
 *******************************************************************************************************/
void FSFile_Query(FS_FILE_HANDLE file_handle,
                  FS_ENTRY_INFO  *p_entry_info,
                  RTOS_ERR       *p_err)

{
  FS_FILE_NODE *p_file_node;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; )
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_entry_info != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );

    FS_FILE_WITH(file_handle, p_err) {
      p_file_node = file_handle.FileDescPtr->FileNodePtr;
      FS_VOL_WITH(p_file_node->VolHandle, p_err) {
        //                                                         No need for locking here: see Note #1 & 2.
        BREAK_ON_ERR(p_file_node->SysPtr->EntryQuery(p_file_node->VolHandle.VolPtr,
                                                     p_file_node->Pos,
                                                     p_entry_info,
                                                     DEF_NULL,
                                                     DEF_NULL,
                                                     0u,
                                                     p_err));
      }
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSFile_Rd()
 *
 * @brief    Read from a file.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_dest          Pointer to destination buffer.
 *
 * @param    size            Number of octets to read.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_FILE_ERR_STATE
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @return   Number of bytes read (may be smaller than 'size' if the end of file is encountered).
 *
 * @note     (1) All accesses to the file descriptor are atomically performed before the actual I/O's
 *               occur. Since FSFile_Rd()'s may execute concurrently, this means that there is no
 *               (guaranteed) relation between the order in which FSFile_Rd() calls return and the order
 *               of the returned data chunks inside the file.
 *
 * @note     (2) If an error occurred in the previous file access, the error indicator must be
 *               cleared (with FSFile_ErrClr()) before another access will be allowed. Otherwise,
 *               the 'RTOS_ERR_FILE_ERR_STATE' error is set and the function returns.
 *
 * @note     (3) If the file buffer is in used and in write mode, the file buffer must be flushed.
 *******************************************************************************************************/
CPU_SIZE_T FSFile_Rd(FS_FILE_HANDLE file_handle,
                     void           *p_dest,
                     CPU_SIZE_T     size,
                     RTOS_ERR       *p_err)
{
  FS_FILE_DESC *p_desc;
  FS_FILE_NODE *p_node;
  FS_FILE_SIZE rem_size;
  FS_FILE_SIZE file_pos = 0u;
  CPU_SIZE_T   rd_size;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_dest != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR, 0u);

    if (size == 0u) {
      return (0u);
    }

    rd_size = 0u;
    FS_FILE_WITH(file_handle, p_err) {
      p_desc = file_handle.FileDescPtr;
      p_node = p_desc->FileNodePtr;

      FS_VOL_WITH(p_node->VolHandle, p_err) {
        //                                                         See Note #1.
        FS_OP_LOCK_WITH(&p_node->VolHandle.VolPtr->OpLock, FS_VOL_OP_FILE_RD) {
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
          //                                                       See Note #3.
          if (p_desc->BufStatus == FS_FILE_BUF_STATUS_NONEMPTY_WR) {
            BREAK_ON_ERR(FSFile_BufEmpty(p_desc, p_err));

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
            {
              FS_VOL *p_vol = p_node->VolHandle.VolPtr;

              BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));
            }
#endif
          }
#endif

          FS_OP_LOCK_WITH(&p_node->VolHandle.VolPtr->OpLock, FS_VOL_OP_DATA_ACCESS) {
            //                                                     -------------------- STATE CHK ---------------------
            //                                                     See Note #2.
            ASSERT_BREAK(!p_desc->IsErr, RTOS_ERR_FILE_ERR_STATE);

            //                                                     ---------------- UPDATE DESCRIPTOR -----------------
            file_pos = p_desc->Pos;

            //                                                     Handle file pos possibly beyond file end.
            rem_size = (file_pos < p_node->Size) ? p_node->Size - file_pos : 0u;
            rd_size = DEF_MIN(size, rem_size);                  // Read no more than the file contains.

            p_desc->Pos += rd_size;
            p_desc->IOStateFlags = FS_FILE_IO_STATE_RD;
            p_desc->IsEOF = (rd_size < size);
          }

          ON_BREAK {
            p_desc->IsErr = DEF_YES;
            break;
          } WITH {
            //                                                     ----------------- RD FROM FILE BUF -----------------
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
            if (DEF_BIT_IS_SET(p_desc->BufModeFlags, FS_FILE_BUF_MODE_RD)) {
              BREAK_ON_ERR(FSFile_BufRd(p_desc,
                                        p_dest,
                                        file_pos,
                                        rd_size,
                                        p_err));
            } else
#endif
            {                                                   // ------------------- RD FROM DISK -------------------
              BREAK_ON_ERR((void)FSFile_RdInternal(p_desc,
                                                   (CPU_INT08U *)p_dest,
                                                   file_pos,
                                                   rd_size,
                                                   p_err));
            }
          }
        }
      }
    }
  } WITH_SCOPE_END

  return (rd_size);
}

/****************************************************************************************************//**
 *                                               FSFile_Wr()
 *
 * @brief    Write to a file.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_src           Pointer to source buffer.
 *
 * @param    size            Number of octets to write.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_FILE_ERR_STATE
 *                               - RTOS_ERR_FILE_ACCESS_MODE_INVALID
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_FULL
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @return   Number of octets written.
 *
 * @note     (1) The file MUST have been opened in write or update (read/write) mode.
 *
 * @note     (2) If the file was opened in append mode, all writes are forced to the EOF.
 *
 * @note     (3) If an error occurred in the previous file access, the error indicator must be
 *               cleared (with FSFile_ErrClr()) before another access will be allowed. Otherwise,
 *               the 'RTOS_ERR_FILE_ERR_STATE' error will be set and the function will return.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
CPU_SIZE_T FSFile_Wr(FS_FILE_HANDLE file_handle,
                     const void     *p_src,
                     CPU_SIZE_T     size,
                     RTOS_ERR       *p_err)
{
  FS_FILE_DESC *p_desc;
  FS_FILE_NODE *p_file_node;
  CPU_SIZE_T   wr_size = 0u;

  if (size == 0u) {
    return (0u);
  }

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_CRITICAL_ERR_SET(p_src != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR, 0u);

    wr_size = 0u;
    FS_FILE_WITH(file_handle, p_err) {
      p_desc = file_handle.FileDescPtr;
      p_file_node = p_desc->FileNodePtr;

      FS_VOL_WITH(p_file_node->VolHandle, p_err) {
        BREAK_ON_ERR(FS_OP_LOCK_WITH) (&p_file_node->VolHandle.VolPtr->OpLock, FS_VOL_OP_FILE_WR) {
          ON_BREAK {
            p_desc->IsErr = DEF_YES;
            break;
          } WITH {
            BREAK_ON_ERR(FS_OP_LOCK_WITH) (&p_file_node->VolHandle.VolPtr->OpLock, FS_VOL_OP_DATA_ACCESS) {
              ASSERT_BREAK(!p_desc->IsErr, RTOS_ERR_FILE_ERR_STATE);
              ASSERT_BREAK(!p_desc->RdOnly, RTOS_ERR_FILE_ACCESS_MODE_INVALID);

              p_desc->IsEOF = DEF_NO;
              p_desc->IOStateFlags = FS_FILE_IO_STATE_WR;
            }

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
            //                                                     ---------------- WRITE TO FILE BUF -----------------
            if (DEF_BIT_IS_SET(p_desc->BufModeFlags, FS_FILE_BUF_MODE_WR)) {
              BREAK_ON_ERR(wr_size = FSFile_BufWr(p_desc,
                                                  (void *)p_src,
                                                  size,
                                                  p_err));
            } else
#endif
            {                                                   // ------------------ WRITE TO DISK -------------------
              BREAK_ON_ERR(wr_size = FSFile_WrInternal(p_desc,
                                                       (CPU_INT08U *)p_src,
                                                       size,
                                                       p_err));
            }
          }
        }

        //                                                         -------------------- AUTO SYNC ---------------------
        if (p_file_node->VolHandle.VolPtr->AutoSync) {
          BREAK_ON_ERR(p_file_node->SysPtr->FileSync(p_file_node, p_err));
        }
      }
    }
  } WITH_SCOPE_END;

  return (wr_size);
}
#endif

/****************************************************************************************************//**
 *                                               FSFile_Truncate()
 *
 * @brief    Truncate or extend a file to a specified length.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    size            Size of file after truncation or extension.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_FILE_ERR_STATE
 *                               - RTOS_ERR_FILE_ACCESS_MODE_INVALID
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_FULL
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSFile_Truncate(FS_FILE_HANDLE file_handle,
                     FS_FILE_SIZE   size,
                     RTOS_ERR       *p_err)
{
  FS_FILE_DESC *p_desc;
  FS_FILE_NODE *p_node;
  FS_VOL       *p_vol;
  FS_SYS_API   *p_sys_api;
  FS_FILE_SIZE file_size;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_FILE_WITH(file_handle, p_err) {
      p_desc = file_handle.FileDescPtr;
      p_node = p_desc->FileNodePtr;
      p_vol = p_node->VolHandle.VolPtr;
      p_sys_api = p_vol->SysPtr;

      ASSERT_BREAK(p_vol->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                   RTOS_ERR_VOL_RD_ONLY);

      ASSERT_BREAK(!p_desc->IsErr, RTOS_ERR_FILE_ERR_STATE);

      FS_VOL_WITH(p_node->VolHandle, p_err) {
        FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_FILE_WR) {
          //                                                       Chk if target file is opened for writing.
          ASSERT_BREAK(!p_desc->RdOnly, RTOS_ERR_FILE_ACCESS_MODE_INVALID);

          ON_BREAK {
            p_desc->IsErr = DEF_YES;
            break;
          } WITH {
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
            if (p_desc->BufData.BufPtr != DEF_NULL) {
              BREAK_ON_ERR(FSFile_BufEmpty(p_desc, p_err));
              BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));
            }
#endif

            FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
              file_size = p_node->Size;
            }

            if (size != file_size) {
              FS_SYS_POS entry_pos_data;
              entry_pos_data = p_node->Pos;

              if (size > file_size) {
                //                                                 ---------------------- EXTEND ----------------------
                BREAK_ON_ERR(p_sys_api->FileWr(p_desc,
                                               file_size,
                                               DEF_NULL,
                                               size - file_size,
                                               p_err));
              } else {
                //                                                 --------------------- SHORTEN ----------------------
                BREAK_ON_ERR(p_sys_api->FileTruncate(p_vol,
                                                     p_node,
                                                     entry_pos_data,
                                                     size,
                                                     p_err));
              }

              FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
                p_node->Size = size;
                p_desc->IsEOF = DEF_NO;
              }
            }
          }
        }
      }
      //                                                           -------------------- AUTO SYNC ---------------------
      p_vol = p_node->VolHandle.VolPtr;
      if (p_vol->AutoSync) {
        BREAK_ON_ERR(p_vol->SysPtr->FileSync(p_node, p_err));
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSFile_ErrClr()
 *
 * @brief    Clear file descriptor error state.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *******************************************************************************************************/
void FSFile_ErrClr(FS_FILE_HANDLE file_handle,
                   RTOS_ERR       *p_err)
{
  FS_FILE_DESC *p_file_desc;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_FILE_WITH(file_handle, p_err) {
    p_file_desc = file_handle.FileDescPtr;
    CORE_ENTER_ATOMIC();
    p_file_desc->IsErr = DEF_NO;
    p_file_desc->IsEOF = DEF_NO;
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//**
 *                                               FSFile_IsErr()
 *
 * @brief    Check whether a file descriptor is in error state.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *
 * @return   DEF_YES, if the files descriptor is in error state.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN FSFile_IsErr(FS_FILE_HANDLE file_handle,
                         RTOS_ERR       *p_err)
{
  FS_FILE_DESC *p_file_desc;
  CPU_BOOLEAN  is_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  is_err = DEF_YES;
  FS_FILE_WITH(file_handle, p_err) {
    p_file_desc = file_handle.FileDescPtr;
    CORE_ENTER_ATOMIC();
    is_err = p_file_desc->IsErr;
    CORE_EXIT_ATOMIC();
  }

  return (is_err);
}

/****************************************************************************************************//**
 *                                               FSFile_Copy()
 *
 * @brief    Copy a file.
 *
 * @param    src_wrk_dir_handle      Handle to source working directory.
 *
 * @param    src_path                Source file path.
 *
 * @param    dest_wrk_dir_handle     Handle to destination working directory.
 *
 * @param    dest_path               Destination file path.
 *
 * @param    excl                    Indicates whether creation of new entry should be exclusive :
 *                                   DEF_YES, if the entry will be copied ONLY if destination entry
 *                                   does not exist.
 *                                   DEF_NO,  if the entry will be copied even if destination entry
 *                                   does exist.
 *
 * @param    p_err                   Pointer to variable that will receive the return error code(s) from
 *                                   this function:
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ENTRY_OPENED
 *                                       - RTOS_ERR_ALREADY_EXISTS
 *                                       - RTOS_ERR_NOT_FOUND
 *                                       - RTOS_ERR_VOL_CLOSED
 *                                       - RTOS_ERR_VOL_FULL
 *                                       - RTOS_ERR_VOL_CORRUPTED
 *                                       - RTOS_ERR_BLK_DEV_CLOSED
 *                                       - RTOS_ERR_BLK_DEV_CORRUPTED
 *                                       - RTOS_ERR_IO
 *
 * @note     (1) The source file MUST exist, otherwise an error is returned.
 *
 * @note     (2) If 'excl' is DEF_NO, the destination entry must either not exist or be an existing
 *               file; it may not be an existing directory.  If 'excl' is DEF_YES, the destination
 *               entry must not exist.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSFile_Copy(FS_WRK_DIR_HANDLE src_wrk_dir_handle,
                 const CPU_CHAR    *src_path,
                 FS_WRK_DIR_HANDLE dest_wrk_dir_handle,
                 const CPU_CHAR    *dest_path,
                 CPU_BOOLEAN       excl,
                 RTOS_ERR          *p_err)
{
  FS_VOL            *p_src_vol;
  FS_VOL            *p_dest_vol;
  FS_ENTRY_LOC_DATA src_entry_loc_data;
  FS_ENTRY_LOC_DATA dest_entry_loc_data;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; )
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(src_wrk_dir_handle,
                                            (CPU_CHAR *)src_path,
                                            &src_entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(src_entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(dest_wrk_dir_handle,
                                            (CPU_CHAR *)dest_path,
                                            &dest_entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(dest_entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);

    BREAK_ON_ERR(FS_VOL_WITH) (src_entry_loc_data.VolHandle, p_err) {
      BREAK_ON_ERR(FS_VOL_WITH) (dest_entry_loc_data.VolHandle, p_err) {
        p_src_vol = src_entry_loc_data.VolHandle.VolPtr;
        p_dest_vol = dest_entry_loc_data.VolHandle.VolPtr;

        ASSERT_BREAK(p_dest_vol->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                     RTOS_ERR_VOL_RD_ONLY);

        FS_OP_LOCK_WITH(&p_dest_vol->OpLock, FS_VOL_OP_FILE_WR) {
          if (p_src_vol != p_dest_vol) {
            FS_OP_LOCK_ACQUIRE(&p_src_vol->OpLock, FS_VOL_OP_FILE_RD);
          }
          BREAK_ON_ERR(ON_EXIT) {
            if (p_src_vol != p_dest_vol) {
              FS_OP_LOCK_RELEASE(&p_src_vol->OpLock, FS_VOL_OP_FILE_RD);
            }
          } WITH {
            BREAK_ON_ERR(FSFile_CopyLockedAcquired(&src_entry_loc_data,
                                                   &dest_entry_loc_data,
                                                   excl,
                                                   p_err));
          }
        }

        //                                                         -------------------- AUTO SYNC ---------------------
        if (p_dest_vol->AutoSync) {
          BREAK_ON_ERR(p_dest_vol->SysPtr->VolSync(p_dest_vol, p_err));
        }
      }
    }
  } WITH_SCOPE_END
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FSFile_TryLock()
 *
 * @brief   Acquire task ownership of a file (if available).
 *
 * @param   file_handle     Handle to a file.
 *
 * @param   p_err           Pointer to variable that will receive the return error code(s) from this
 *                          function:
 *                              - RTOS_ERR_NONE
 *                              - RTOS_ERR_ENTRY_CLOSED
 *                              - RTOS_ERR_WOULD_BLOCK
 *
 *
 * @note     (1) FSFile_TryLock() is the non-blocking version of FSFile_Lock(). If the lock
 *           is not available, the function returns an error.
 *
 * @note     (2) File locks may be nested.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
void FSFile_TryLock(FS_FILE_HANDLE file_handle,
                    RTOS_ERR       *p_err)

{
  FS_FILE_DESC *p_file_desc;
  RTOS_ERR     lock_err;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_FILE_WITH_NO_IO(file_handle, p_err) {
      p_file_desc = file_handle.FileDescPtr;
      KAL_LockAcquire(p_file_desc->FileLockHandle,
                      KAL_OPT_PEND_NON_BLOCKING,
                      0u,
                      &lock_err);

      ASSERT_BREAK((RTOS_ERR_CODE_GET(lock_err) == RTOS_ERR_NONE)
                   || (RTOS_ERR_CODE_GET(lock_err) == RTOS_ERR_WOULD_BLOCK),
                   RTOS_ERR_CODE_GET(lock_err));

      //                                                           The file descriptor must be acquired so that ...
      //                                                           ... it is not freed while holding the lock.
      if (RTOS_ERR_CODE_GET(lock_err) == RTOS_ERR_NONE) {
        (void)FSObj_RefAcquire(FSFile_Desc_ToObjHandle(file_handle));
      }
    }

    *p_err = lock_err;
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSFile_Lock()
 *
 * @brief    Acquire task ownership of a file.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *
 * @note     (1) File locks may be nested.
 *
 * @note     (2) The file descriptor must be acquired so that it is not freed while holding the lock.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
void FSFile_Lock(FS_FILE_HANDLE file_handle,
                 RTOS_ERR       *p_err)
{
  FS_FILE_DESC *p_file_desc;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_FILE_WITH_NO_IO(file_handle, p_err) {
      p_file_desc = file_handle.FileDescPtr;
      BREAK_ON_ERR(KAL_LockAcquire(p_file_desc->FileLockHandle,
                                   KAL_OPT_NONE,
                                   0u,
                                   p_err));

      //                                                           See Note #2.
      (void)FSObj_RefAcquire(FSFile_Desc_ToObjHandle(file_handle));
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSFile_Unlock()
 *
 * @brief    Release task ownership of a file.
 *
 * @param    file_handle     Handle to a file.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
void FSFile_Unlock(FS_FILE_HANDLE file_handle)

{
  FS_FILE_DESC *p_file_desc;
  RTOS_ERR     err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  p_file_desc = file_handle.FileDescPtr;
  KAL_LockRelease(p_file_desc->FileLockHandle, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE),
                       RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  FSObj_RefRelease(FSFile_Desc_ToObj(p_file_desc), FSFile_Desc_OnNullRefCnt);
}
#endif

/****************************************************************************************************//**
 *                                           FSFile_BufAssign()
 *
 * @brief    Assign buffer to a file.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    p_buf           Pointer to buffer.
 *
 * @param    mode            Buffer mode :
 *                               - FS_FILE_BUF_MODE_RD       Data buffered for reads.
 *                               - FS_FILE_BUF_MODE_WR       Data buffered for writes.
 *                               - FS_FILE_BUF_MODE_RD_WR    Data buffered for reads & writes.
 *
 * @param    size            Size of the given buffer, in octets.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_FILE_ACCESS_MODE_INVALID
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *
 * @note     (1) Once a buffer is assigned to a file, a new buffer may not be assigned nor may the
 *               assigned buffer be removed.  To change the buffer, the file should be closed and
 *               re-opened.
 *
 * @note     (2) 'size' MUST be more than or equal to the size of one sector; it will be rounded DOWN
 *               to the nearest size of a multiple of full sectors.
 *
 * @note     (3) Upon power loss, any data stored in file buffers will be lost.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
void FSFile_BufAssign(FS_FILE_HANDLE file_handle,
                      void           *p_buf,
                      FS_FLAGS       mode,
                      CPU_SIZE_T     size,
                      RTOS_ERR       *p_err)
{
  FS_FILE_DESC *p_file_desc;
  FS_FILE_NODE *p_file_node;
  FS_VOL       *p_vol;
  CPU_SIZE_T   lb_size;
  CPU_SIZE_T   size_rem;
  CPU_INT08U   lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_buf != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET((mode == FS_FILE_BUF_MODE_RD)
                            || (mode == FS_FILE_BUF_MODE_WR)
                            || (mode == FS_FILE_BUF_MODE_RD_WR),
                            *p_err, RTOS_ERR_INVALID_ARG,; )

    FS_FILE_WITH(file_handle, p_err) {
      p_file_desc = file_handle.FileDescPtr;
      p_file_node = p_file_desc->FileNodePtr;
      p_vol = p_file_node->VolHandle.VolPtr;

      FS_VOL_WITH(p_file_node->VolHandle, p_err) {
        FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_FILE_WR) {
          //                                                       ------------------- VALIDATE BUF -------------------
          //                                                       If buf has already been assigned (see Note #1).
          RTOS_ASSERT_DBG_ERR_SET(p_file_desc->BufData.BufPtr == DEF_NULL,
                                  *p_err, RTOS_ERR_INVALID_STATE,; );

          BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->BlkDevHandle, p_err));

          lb_size = FS_UTIL_PWR2(lb_size_log2);
          ASSERT_BREAK(size >= lb_size, RTOS_ERR_SIZE_INVALID);

          //                                                       If buf mode does not match access mode rtn err.
          if (DEF_BIT_IS_SET(mode, FS_FILE_BUF_MODE_WR) && p_file_desc->RdOnly) {
            BREAK_ERR_SET(RTOS_ERR_FILE_ACCESS_MODE_INVALID);
          }

          //                                                       -------------------- ASSIGN BUF --------------------
          //                                                       Rnd size DOWN to nearest sec size mult (see Note #2).
          size_rem = size % lb_size;
          size -= size_rem;

          p_file_desc->BufModeFlags = mode;
          p_file_desc->BufStatus = FS_FILE_BUF_STATUS_EMPTY;
          p_file_desc->BufData.Pos = 0u;
          p_file_desc->BufData.Size = size;
          p_file_desc->BufData.WrSeqNo = 0u;
          p_file_desc->BufData.BufPtr = p_buf;
        }
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSFile_BufFlush()
 *
 * @brief    Flush buffer contents to file.
 *
 * @param    file_handle    Handle to a file.
 *
 * @param    p_err          Pointer to variable that will receive the return error code(s) from this
 *                          function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_FULL
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) If the most recent operation is output (write), all unwritten data is written
 *               to the file.
 *               - (a) If the most recent operation is input (read), all buffered data is cleared.
 *               - (b) If a read or write error occurs, the error indicator is set.
 *
 * @note     (2) If an error occurred in the previous file access, the error indicator must be
 *               cleared (with FSFile_ErrClr()) before another access will be allowed.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
void FSFile_BufFlush(FS_FILE_HANDLE file_handle,
                     RTOS_ERR       *p_err)
{
  FS_FILE_DESC *p_desc;
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  FS_FILE_NODE *p_node;
  FS_VOL       *p_vol;
#endif

  p_desc = file_handle.FileDescPtr;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_FILE_WITH(file_handle, p_err) {
      p_desc = file_handle.FileDescPtr;
      FS_VOL_WITH(p_desc->FileNodePtr->VolHandle, p_err) {
        BREAK_ON_ERR(FS_OP_LOCK_WITH) (&p_desc->FileNodePtr->VolHandle.VolPtr->OpLock, FS_VOL_OP_FILE_WR) {
          if (p_desc->BufData.BufPtr != DEF_NULL) {
            ON_BREAK {
              p_desc->IsErr = DEF_YES;
              break;
            } WITH {
              BREAK_ON_ERR(FSFile_BufEmpty(p_desc, p_err));
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
              p_node = p_desc->FileNodePtr;
              p_vol = p_node->VolHandle.VolPtr;
              BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));
#endif
              p_desc->IOStateFlags = FS_FILE_IO_STATE_NONE;
            }
          }
        }
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSFile_PathGet()
 *
 * @brief    Get absolute path to a opened file.
 *
 * @param    file_handle     Handle to a file.
 *
 * @param    buf             Pointer to a buffer that will receive the file path.
 *
 * @param    buf_size        Size of the provided buffer.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/
void FSFile_PathGet(FS_FILE_HANDLE file_handle,
                    CPU_CHAR       *p_buf,
                    CPU_SIZE_T     buf_size,
                    RTOS_ERR       *p_err)
{
  FS_ENTRY_NODE *p_entry_node;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET(p_buf != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; )

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_FILE_WITH(file_handle, p_err) {
    p_entry_node = (FS_ENTRY_NODE *)file_handle.FileDescPtr->FileNodePtr;
    FSEntry_PathGetInternal(p_entry_node,
                            FS_ENTRY_TYPE_FILE,
                            p_buf,
                            buf_size,
                            p_err);
  }
}

/****************************************************************************************************//**
 *                                               FSFile_VolGet()
 *
 * @brief    Get handle to the volume in which the file is.
 *
 * @param    file_handle     Handle to a file.
 *
 * @return   Handle to the volume or NULL handle if the file is closed.
 *******************************************************************************************************/
FS_VOL_HANDLE FSFile_VolGet(FS_FILE_HANDLE file_handle)
{
  FS_VOL_HANDLE vol_handle;
  RTOS_ERR      err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  vol_handle = FSVol_NullHandle;

  FS_FILE_WITH(file_handle, &err) {
    vol_handle = file_handle.FileDescPtr->FileNodePtr->VolHandle;
  }

  PP_UNUSED_PARAM(err);

  return (vol_handle);
}

/****************************************************************************************************//**
 *                                           FSFile_FirstAcquire()
 *
 * @brief    Acquire the first opened file.
 *
 * @return   Handle to the first opened file Handle
 *           'FS_FILE_NULL' if no file is opened.
 *******************************************************************************************************/
FS_FILE_HANDLE FSFile_FirstAcquire(void)
{
  FS_OBJ_HANDLE  obj_handle;
  FS_FILE_HANDLE file_handle;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD) {
    obj_handle = FSObj_FirstRefAcquireLocked(FSFile_OpenDescListHeadPtr);
  }

  file_handle.FileDescPtr = (FS_FILE_DESC *)obj_handle.ObjPtr;
  file_handle.FileDescId = obj_handle.ObjId;

  return (file_handle);
}

/****************************************************************************************************//**
 *                                           FSFile_NextAcquire()
 *
 * @brief    Acquire the next file.
 *
 * @param    file_handle     Handle to the current file.
 *
 * @return   Handle to the next file
 *           'FS_FILE_NULL' if no file is opened.
 *******************************************************************************************************/
FS_FILE_HANDLE FSFile_NextAcquire(FS_FILE_HANDLE file_handle)
{
  FS_OBJ_HANDLE  cur_obj_handle;
  FS_OBJ_HANDLE  next_obj_handle;
  FS_FILE_HANDLE next_file_handle;

  cur_obj_handle.ObjPtr = (FS_OBJ *)file_handle.FileDescPtr;
  cur_obj_handle.ObjId = file_handle.FileDescId;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD) {
    next_obj_handle = FSObj_NextRefAcquireLocked(cur_obj_handle);
  }

  FSObj_RefRelease(FSFile_Desc_ToObj(file_handle.FileDescPtr), FSFile_Desc_OnNullRefCnt);

  next_file_handle.FileDescPtr = (FS_FILE_DESC *)next_obj_handle.ObjPtr;
  next_file_handle.FileDescId = next_obj_handle.ObjId;

  return (next_file_handle);
}

/****************************************************************************************************//**
 *                                               FSFile_Release()
 *
 * @brief    Release a file.
 *
 * @param    file_handle     Handle to a file.
 *******************************************************************************************************/
void FSFile_Release(FS_FILE_HANDLE file_handle)
{
  FSObj_RefRelease((FS_OBJ *)file_handle.FileDescPtr, FSFile_Desc_OnNullRefCnt);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FSFile_CopyLockedAcquired()
 *
 * @brief    Copy a file.
 *
 * @param    p_src_entry_loc_data    Pointer to the source entry location data.
 *
 * @param    p_dest_entry_loc_data   Pointer to the destination entry location data.
 *
 * @param    excl                    Indicates whether creation of new entry should be exclusive.
 *
 * @param    p_err                   Error pointer.
 *******************************************************************************************************/

#if ((FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED) \
  && (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED))
void FSFile_CopyLockedAcquired(FS_ENTRY_LOC_DATA *p_src_entry_loc_data,
                               FS_ENTRY_LOC_DATA *p_dest_entry_loc_data,
                               CPU_BOOLEAN       excl,
                               RTOS_ERR          *p_err)
{
  FS_VOL               *p_src_vol;
  FS_VOL               *p_dest_vol;
  FS_CACHE             *p_src_cache;
  FS_CACHE             *p_dest_cache;
  CPU_CHAR             *p_dest_entry_name;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  FS_ENTRY_INFO        src_file_info;
  FS_SYS_POS           src_entry_pos;
  FS_SYS_POS           dest_entry_pos;
  FS_SYS_POS           dest_parent_pos;

  WITH_SCOPE_BEGIN(p_err) {
    p_src_vol = p_src_entry_loc_data->VolHandle.VolPtr;
    p_dest_vol = p_dest_entry_loc_data->VolHandle.VolPtr;
    //                                                             Search entry in src vol.
    BREAK_ON_ERR(FSEntry_Lookup(p_src_entry_loc_data,
                                DEF_NULL,
                                &src_entry_pos,
                                DEF_NULL,
                                p_err));

    ASSERT_BREAK_LOG(src_entry_pos != p_src_vol->SysPtr->VoidEntryPos,
                     RTOS_ERR_NOT_FOUND, ("Entry not found."));
    //                                                             Retrieve info about src entry.
    BREAK_ON_ERR(p_src_vol->SysPtr->EntryQuery(p_src_vol,
                                               src_entry_pos,
                                               &src_file_info,
                                               DEF_NULL,
                                               DEF_NULL,
                                               0u,
                                               p_err));
    //                                                             Chk if src entry is dir.
    ASSERT_BREAK_LOG(!src_file_info.Attrib.IsDir, RTOS_ERR_INVALID_TYPE,
                     ("Cannot copy a directory."));
    //                                                             Open src entry to get a node handle.
    entry_node_handle = FSEntry_OpenFind(p_src_vol, src_entry_pos);
    ASSERT_BREAK_LOG(FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle), RTOS_ERR_ENTRY_OPENED,
                     ("Cannot copy an opened file."));

    //                                                             Search entry in destination vol.
    BREAK_ON_ERR(FSEntry_Lookup(p_dest_entry_loc_data,
                                &dest_parent_pos,
                                &dest_entry_pos,
                                DEF_NULL,
                                p_err));

    ASSERT_BREAK_LOG(dest_parent_pos != p_dest_vol->SysPtr->VoidEntryPos,
                     RTOS_ERR_NOT_FOUND, ("Destination parent entry not found."));
    //                                                             Del existing destination file entry if required.
    if (dest_entry_pos != p_dest_vol->SysPtr->VoidEntryPos) {
      ASSERT_BREAK_LOG(!excl, RTOS_ERR_ALREADY_EXISTS, ("Entry already exists."));
      //                                                           Open destination entry to get a node handle.
      entry_node_handle = FSEntry_OpenFind(p_dest_vol, dest_entry_pos);
      ASSERT_BREAK_LOG(FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle), RTOS_ERR_ENTRY_OPENED,
                       ("Cannot copy to an opened file."));
      //                                                           If destination file exists, del file entry in dir tbl
      BREAK_ON_ERR(p_dest_vol->SysPtr->EntryDel(p_dest_vol,
                                                dest_entry_pos,
                                                p_err));
    }
    //                                                             Create destination file entry in dir tbl.
    p_dest_entry_name = FSCore_PathLastSegGet(p_dest_entry_loc_data->RelPathPtr);
    BREAK_ON_ERR(dest_entry_pos = p_dest_vol->SysPtr->EntryCreate(p_dest_vol,
                                                                  dest_parent_pos,
                                                                  p_dest_entry_name,
                                                                  DEF_NO,
                                                                  p_err));
    //                                                             Alloc cluster(s) for destination file.
    BREAK_ON_ERR(p_dest_vol->SysPtr->FileExtend(p_dest_vol,
                                                dest_entry_pos,
                                                src_file_info.Size,
                                                p_err));

    p_src_cache = FSCache_Get(p_src_vol->BlkDevHandle);         // Get cache instance associated to src vol.
    p_dest_cache = FSCache_Get(p_dest_vol->BlkDevHandle);       // Get cache instance associated to destination vol.

    //                                                             ------------------- INTRA CACHE --------------------
    if (p_src_cache == p_dest_cache) {
      BREAK_ON_ERR(FSFile_IntraCacheCopy(p_dest_cache,
                                         p_src_vol,
                                         p_dest_vol,
                                         src_entry_pos,
                                         dest_entry_pos,
                                         src_file_info.Size,
                                         p_err));
    } else {                                                    // ------------------- INTER CACHE --------------------
      BREAK_ON_ERR(FSFile_InterCacheCopy(p_src_vol,
                                         p_dest_vol,
                                         src_entry_pos,
                                         dest_entry_pos,
                                         src_file_info.Size,
                                         p_err));
    }
  } WITH_SCOPE_END
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSFile_OpenModeChk()
 *
 * @brief    Check whether access mode flags combination is valid.
 *
 * @param    mode    Access mode flags.
 *
 * @return   DEF_YES, if flag combination is valid.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static CPU_BOOLEAN FSFile_AccessModeIsValid(FS_FLAGS mode)
{
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_ENABLED)
  if (DEF_BIT_IS_SET_ANY(mode, FS_FILE_ACCESS_MODE_WR
                         | FS_FILE_ACCESS_MODE_CREATE
                         | FS_FILE_ACCESS_MODE_EXCL
                         | FS_FILE_ACCESS_MODE_APPEND
                         | FS_FILE_ACCESS_MODE_TRUNCATE)) {
    return (DEF_NO);
  }
#endif

  if (DEF_BIT_IS_CLR(mode, FS_FILE_ACCESS_MODE_RD | FS_FILE_ACCESS_MODE_WR) == DEF_YES) {
    return (DEF_NO);
  }

  if (DEF_BIT_IS_SET(mode, FS_FILE_ACCESS_MODE_TRUNCATE)
      && DEF_BIT_IS_CLR(mode, FS_FILE_ACCESS_MODE_WR)) {
    return (DEF_NO);
  }

  if (DEF_BIT_IS_SET(mode, FS_FILE_ACCESS_MODE_EXCL)
      && DEF_BIT_IS_CLR(mode, FS_FILE_ACCESS_MODE_CREATE)) {
    return (DEF_NO);
  }

  if (DEF_BIT_IS_SET(mode, FS_FILE_ACCESS_MODE_APPEND)
      && DEF_BIT_IS_CLR(mode, FS_FILE_ACCESS_MODE_WR)) {
    return (DEF_NO);
  }

  if ((mode & FS_FILE_ACCESS_MODE_ALL) != mode) {
    return (DEF_NO);
  }

  if (DEF_BIT_IS_SET(mode, FS_FILE_ACCESS_MODE_RD)
      && DEF_BIT_IS_SET(mode, FS_FILE_ACCESS_MODE_CREATE)
      && DEF_BIT_IS_CLR(mode, (FS_FILE_ACCESS_MODE_EXCL
                               | FS_FILE_ACCESS_MODE_WR))) {
    return (DEF_NO);
  }

  return (DEF_YES);
}
#endif

/****************************************************************************************************//**
 *                                               FSFile_BufEmpty()
 *
 * @brief    Empty a file's buffer.
 *
 * @param    p_desc  Pointer to a file descriptor.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
static void FSFile_BufEmpty(FS_FILE_DESC *p_desc,
                            RTOS_ERR     *p_err)
{
  WITH_SCOPE_BEGIN(p_err) {
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
    if (p_desc->BufStatus == FS_FILE_BUF_STATUS_NONEMPTY_WR) {
      BREAK_ON_ERR((void)FSFile_WrInternal(p_desc,
                                           (CPU_INT08U *)p_desc->BufData.BufPtr,
                                           p_desc->BufData.Pos,
                                           p_err));
    }
#endif

    FS_OP_LOCK_WITH(&p_desc->FileNodePtr->VolHandle.VolPtr->OpLock, FS_VOL_OP_DATA_ACCESS) {
      p_desc->BufData.Pos = 0u;
      p_desc->BufStatus = FS_FILE_BUF_STATUS_EMPTY;
    }

#if (FS_CORE_CFG_DBG_MEM_CLR_EN == DEF_ENABLED)
    Mem_Clr(p_desc->BufData.BufPtr, p_desc->BufData.Size);
#endif
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSFile_BufRd()
 *
 * @brief    Read from a file (through file buffer).
 *
 * @param    p_desc      Pointer to a file descriptor.
 *
 * @param    p_dest      Pointer to buffer that will hold data read by this file.
 *
 * @param    file_pos    Current read position in the file.
 *
 * @param    size        Number of octets to read.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Number of octets read.
 *
 * @note     (1) Data is first copied from the buffer, up to the length of the request OR the
 *               amount stored in the buffer (whichever is less).
 *               @n
 *               Any large remaining read (longer than the file buffer) is satisfied by
 *               reading directly into the destination buffer.
 *               @n
 *               Otherwise, the buffer is filled (if possible) and data copied into the user
 *               buffer, up to the length of the remaining request OR the amount stored in the
 *               buffer (whichever is less).
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
static void FSFile_BufRd(FS_FILE_DESC *p_desc,
                         void         *p_dest,
                         FS_FILE_SIZE file_pos,
                         CPU_SIZE_T   size,
                         RTOS_ERR     *p_err)
{
  FS_FILE_NODE          *p_node;
  FS_VOL                *p_vol;
  FS_FILE_DESC_BUF_DATA *p_buf_data;
  void                  *p_file_buf;
  CPU_INT08U            *p_dest_08;
  FS_FILE_SIZE          cur_file_pos;
  CPU_SIZE_T            buf_pos;
  CPU_SIZE_T            size_rd;
  CPU_SIZE_T            disk_rd_size;
  CPU_SIZE_T            size_rem;
  CPU_INT16U            node_wr_seq;
  CPU_INT16U            desc_wr_seq;
  FS_STATE              buf_status;
  CPU_SIZE_T            buf_size;
  CPU_SIZE_T            size_copy;
  CPU_SIZE_T            rd_thresh;
  CPU_INT08U            lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ---------- RD FROM FILE BUF INTO USER BUF ----------
    size_rem = size;
    cur_file_pos = file_pos;

    p_node = p_desc->FileNodePtr;
    p_vol = p_node->VolHandle.VolPtr;
    p_dest_08 = (CPU_INT08U *)p_dest;

    FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
      p_buf_data = &p_desc->BufData;
      buf_pos = p_buf_data->Pos;
      buf_status = p_desc->BufStatus;
      buf_size = p_buf_data->Size;
      p_file_buf = (CPU_INT08U *)p_buf_data->BufPtr;
    }

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->BlkDevHandle, p_err));

    rd_thresh = cur_file_pos - buf_pos + buf_size;
    rd_thresh -= FS_UTIL_MODULO_PWR2(rd_thresh, lb_size_log2);
    rd_thresh -= (cur_file_pos - buf_pos);

    //                                                             --------------- HANDLE NON EMPTY BUF ---------------
    if (buf_status == FS_FILE_BUF_STATUS_NONEMPTY_RD) {
      FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
        node_wr_seq = p_node->BufData.WrSeqNo;
        desc_wr_seq = p_desc->BufData.WrSeqNo;
      }

      if (node_wr_seq != desc_wr_seq) {
        buf_pos = 0u;
      } else {                                                  // Read rem data contained in file buf.
        size_copy = DEF_MIN(size_rem, rd_thresh - buf_pos);
        Mem_Copy(p_dest_08, (CPU_INT08U *)p_file_buf + buf_pos, size_copy);
        size_rem -= size_copy;
        cur_file_pos += size_copy;
        buf_pos += size_copy;
        p_dest_08 += size_copy;
      }
    }

    //                                                             ----------- HANDLE DATA OUTSIDE FILE BUF -----------
    if (size_rem > 0u) {
      disk_rd_size = (size_rem / buf_size) * buf_size;
      buf_pos = 0u;
      size_rd = 0u;
      if (disk_rd_size != 0u) {                                 // Rd into dest buf portion of rem data which size ...
                                                                // ... is the biggest integer multiple of the buf size.
        BREAK_ON_ERR(size_rd = FSFile_RdInternal(p_desc,
                                                 p_dest_08,
                                                 cur_file_pos,
                                                 disk_rd_size,
                                                 p_err));
        size_rem -= size_rd;
        p_dest_08 += size_rd;
        cur_file_pos += size_rd;
      }
      //                                                           Rd into file buf one more 'buf_size' chunk ...
      //                                                           ... to complete the rd.
      if ((size_rem > 0u) && (size_rd == disk_rd_size)) {
        BREAK_ON_ERR(size_rd = FSFile_RdInternal(p_desc,
                                                 (CPU_INT08U *)p_file_buf,
                                                 cur_file_pos,
                                                 buf_size,
                                                 p_err));
        size_copy = DEF_MIN(size_rd, size_rem);
        Mem_Copy((CPU_INT08U *)p_dest_08, p_file_buf, size_copy);
        buf_pos = size_copy;
        size_rem -= size_copy;
        p_desc->BufStatus = FS_FILE_BUF_STATUS_NONEMPTY_RD;     /* Make the buffer status non empty.                    */
      }
    }

    FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {    // Update buf data.
      p_buf_data->Pos = buf_pos;
      p_buf_data->WrSeqNo = p_node->BufData.WrSeqNo;
    }
  } WITH_SCOPE_END

  //                                                               ------------------- ASSIGN & RTN -------------------
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    if (size_rem > 0u) {                                        // If less data rd than rem ...
      p_desc->IsEOF = DEF_YES;                                  // ... file at EOF.
    } else {                                                    // If all  data rd that rem'd ...
      p_desc->IsEOF = DEF_NO;                                   // ... file NOT at EOF.
    }
  } else {
    p_desc->IsEOF = DEF_NO;
  }
}
#endif

/****************************************************************************************************//**
 *                                               FSFile_BufWr()
 *
 * @brief    Write to a file (through file buffer).
 *
 * @param    p_desc      Pointer to a file descriptor.
 *
 * @param    p_src       Pointer to buffer that contains data to write in this file.
 *
 * @param    size        Number of octets to write.
 *
 * @param    p_err       Error pointer.
 *
 * @note         (1) The flush threshold is used to determine the maximum amount of data that may be
 *                   accumulated in the buffer such that, after flushing, the file buffer will
 *                   be aligned on logical block boundaries (see figure below).
 *                       @verbatim
 *                                           current buffer         flush
 *                                           byte offset         threshold
 *                                               |                   |
 *                                               v                   v
 *                                           +------+------+------+------+
 *                           FILE BUF        |////  |      |      |      |
 *                                           +------+------+------+------+
 *
 *                                   ---+------+------+------+------+---
 *                       ON-DISK FILE     |      |      |      |      |
 *                                   ---+------+------+------+------+---
 *                                           ^
 *                                           |
 *                                       current
 *                                   file position
 *
 *                   The flush threshold is given by
 *
 *                       flush thresh = buf size - (cur file pos % sec size).
 *                       @endverbatim
 * @note         (2) Overlapping source and destination is allowed as long as the source is located at a
 *                   higher address than the destination, which is always the case here. See Mem_Copy()
 *                   notes.
 *
 * @note         (3) Data written in a single buffered write operation is guaranteed end up in a
 *                   monolithic region on the media. To achieve this guarantee in append mode, the source
 *                   buffer must either be completely written to the media or not written at all (i.e.
 *                   accumulated in the file buffer).
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_SIZE_T FSFile_BufWr(FS_FILE_DESC *p_desc,
                               void         *p_src,
                               CPU_SIZE_T   size,
                               RTOS_ERR     *p_err)
{
  FS_VOL                *p_vol;
  CPU_INT08U            *p_src_08;
  void                  *p_file_buf;
  FS_FILE_DESC_BUF_DATA *p_buf_data;
  FS_FILE_NODE          *p_node;
  CPU_SIZE_T            wr_size;
  CPU_SIZE_T            size_rem = 0u;
  CPU_SIZE_T            flush_thresh;
  CPU_SIZE_T            buf_pos;
  CPU_SIZE_T            file_pos;
  CPU_SIZE_T            buf_rem;
  CPU_SIZE_T            buf_size;
  CPU_INT08U            lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    if (size == 0u) {
      return (0u);
    }

    p_node = p_desc->FileNodePtr;
    p_vol = p_node->VolHandle.VolPtr;

    if (p_desc->BufStatus == FS_FILE_BUF_STATUS_NONEMPTY_RD) {
      BREAK_ON_ERR(FSFile_BufEmpty(p_desc, p_err));
    }

    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_vol->BlkDevHandle, p_err));

    p_buf_data = &p_desc->BufData;
    p_src_08 = (CPU_INT08U *)p_src;
    size_rem = size;
    buf_size = p_buf_data->Size;

    FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
      file_pos = p_desc->Pos;
      buf_pos = p_buf_data->Pos;
      p_file_buf = p_buf_data->BufPtr;
    }

    buf_rem = buf_size - buf_pos;
    if (size_rem < buf_rem) {
      Mem_Copy((CPU_INT08U *)p_file_buf + buf_pos, p_src_08, size_rem);
      buf_pos += size_rem;
      size_rem = 0u;
    } else {
      //                                                           See Note #1.
      flush_thresh = buf_size - FS_UTIL_MODULO_PWR2(file_pos, lb_size_log2);

      if (buf_pos >= flush_thresh) {                            // If file buf is fill'd up to thresh or beyond ...
                                                                // ... dump to disk up to thresh.
        BREAK_ON_ERR((void)FSFile_WrInternal(p_desc,
                                             (CPU_INT08U *)p_file_buf,
                                             flush_thresh,
                                             p_err));
        wr_size = buf_pos - flush_thresh;
        if (wr_size > 0u) {                                     // If valid data exists beyond flush thresh ...
          Mem_Copy(p_file_buf,                                  // ... move it to file buf start (see Note #2).
                   (void *)((CPU_INT08U *)p_file_buf + flush_thresh),
                   wr_size);
        }
        buf_pos = wr_size;
        flush_thresh = buf_size;
      }

      if (buf_pos > 0u) {                                       // If file buf contains valid data ...
                                                                // ... but not up to flush thresh ...
        wr_size = DEF_MIN(flush_thresh - buf_pos, size_rem);
        Mem_Copy((CPU_INT08U *)p_file_buf + buf_pos,            // ... fill buf up to flush thresh ...
                 p_src_08,                                      // ... or until user data is exhausted.
                 wr_size);

        p_src_08 += wr_size;
        size_rem -= wr_size;
        buf_pos += wr_size;
        //                                                         If flush thresh reached or file descriptor ...
        //                                                         ... is in append mode, flush file buf (see Note #3).
        if (p_desc->Append || (buf_pos == flush_thresh)) {
          BREAK_ON_ERR((void)FSFile_WrInternal(p_desc,
                                               (CPU_INT08U *)p_file_buf,
                                               buf_pos,
                                               p_err));
          buf_pos = 0u;
        }
      }

      if (size_rem > 0u) {
        //                                                         See Note #3.
        if (p_desc->Append) {
          BREAK_ON_ERR((void)FSFile_WrInternal(p_desc,
                                               p_src_08,
                                               size_rem,
                                               p_err));
          size_rem = 0u;
        } else {
          wr_size = (size_rem / buf_size) * buf_size;
          if (wr_size != 0u) {
            BREAK_ON_ERR((void)FSFile_WrInternal(p_desc,
                                                 p_src_08,
                                                 wr_size,
                                                 p_err));
            size_rem -= wr_size;
            p_src_08 += wr_size;
          }

          if (size_rem > 0u) {
            Mem_Copy(p_file_buf, (void *)p_src_08, size_rem);
            buf_pos = size_rem;
            size_rem = 0u;
          }
        }
      }
    }

    FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
      p_desc->BufStatus = (buf_pos == 0u) ? FS_FILE_BUF_STATUS_EMPTY
                          : FS_FILE_BUF_STATUS_NONEMPTY_WR;
      p_buf_data->Pos = buf_pos;
    }
  } WITH_SCOPE_END

  return (size - size_rem);
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FSFile_RdInternal()
 *
 * @brief    Read from media.
 *
 * @param    p_desc      Pointer to a file descriptor.
 *
 * @param    p_dest      Pointer the user provided destination buffer.
 *
 * @param    pos         Position in the file (in bytes) to read from.
 *
 * @param    rd_size     Size of the data (in bytes) to be read.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Size of the read data.
 *
 * @note     (1) This function does not update the descriptor's current position. It is the caller
 *               responsibility to perform the position update.
 *******************************************************************************************************/
static CPU_SIZE_T FSFile_RdInternal(FS_FILE_DESC *p_desc,
                                    CPU_INT08U   *p_dest,
                                    FS_FILE_SIZE pos,
                                    FS_FILE_SIZE rd_size,
                                    RTOS_ERR     *p_err)
{
  FS_FILE_SIZE size_rd;

  size_rd = p_desc->FileNodePtr->SysPtr->FileRd(p_desc,
                                                pos,
                                                p_dest,
                                                rd_size,
                                                p_err);
  return (size_rd);
}

/****************************************************************************************************//**
 *                                           FSFile_WrInternal()
 *
 * @brief    Write data to the media and update file descriptor's current position and file node's
 *           file size. Increment the write sequence number.
 *
 * @param    p_desc      Pointer to a file descriptor.
 *
 * @param    p_src       Pointer to the user provided source buffer.
 *
 * @param    wr_size     Size of the given buffer.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Size of the written data.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_SIZE_T FSFile_WrInternal(FS_FILE_DESC *p_desc,
                                    CPU_INT08U   *p_src,
                                    FS_FILE_SIZE wr_size,
                                    RTOS_ERR     *p_err)
{
  FS_FILE_NODE *p_node;
  FS_VOL       *p_vol;
  FS_FILE_SIZE size_wr;

  WITH_SCOPE_BEGIN(p_err) {
    p_node = p_desc->FileNodePtr;
    p_vol = p_node->VolHandle.VolPtr;

    //                                                             Wr at the end if append mode is set.
    FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
      if (p_desc->Append) {
        p_desc->Pos = p_node->Size;
      }
    }

    BREAK_ON_ERR(size_wr = p_vol->SysPtr->FileWr(p_desc,
                                                 p_desc->Pos,
                                                 p_src,
                                                 wr_size,
                                                 p_err));

    //                                                             Inc file size if needed.
    FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
      p_desc->Pos += size_wr;
      if (p_desc->Pos > p_node->Size) {
        p_node->Size = p_desc->Pos;
      }
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
      p_node->BufData.WrSeqNo++;
#endif
    }
  } WITH_SCOPE_END

  return (size_wr);
}
#endif

/****************************************************************************************************//**
 *                                           FSFile_IntraCacheCopy()
 *
 * @brief    Copy file where both source and destination device share the same cache instance.
 *
 * @param    p_cache         Pointer to the shared cache instance.
 *
 * @param    p_src_vol       Pointer to the source volume.
 *
 * @param    p_dest_vol      Pointer to the destination volume.
 *
 * @param    src_entry_pos   Source entry position.
 *
 * @param    dest_entry_pos  Destination entry position.
 *
 * @param    src_file_size   Source file size.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSFile_IntraCacheCopy(FS_CACHE   *p_cache,
                                  FS_VOL     *p_src_vol,
                                  FS_VOL     *p_dest_vol,
                                  FS_SYS_POS src_entry_pos,
                                  FS_SYS_POS dest_entry_pos,
                                  CPU_SIZE_T src_file_size,
                                  RTOS_ERR   *p_err)
{
  FS_VOL     *p_small_vol;
  FS_VOL     *p_big_vol;
  CPU_INT08U *p_buf;
  FS_LB_NBR  src_lb_nbr;
  FS_LB_NBR  dest_lb_nbr;
  FS_LB_NBR  small_lb_nbr;
  FS_LB_NBR  big_lb_nbr;
  FS_LB_NBR  prev_big_lb_nbr;
  FS_LB_NBR  small_lb_tbl[FS_FILE_COPY_LB_TBL_MAX_SIZE];
  CPU_INT08U sub_blks_per_blk_log2;
  CPU_INT08U sub_blks_per_blk;
  CPU_INT08U sub_blks_per_blk_ix;
  CPU_INT08U small_lb_cnt;
  CPU_SIZE_T small_lb_rd_wr_cnt;
  CPU_SIZE_T small_lb_rd_wr_size;
  CPU_INT08U small_lb_offset;
  CPU_INT08U small_lb_tbl_ix;
  CPU_INT08U src_lb_size_log2;
  CPU_INT08U dest_lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(src_lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_src_vol->BlkDevHandle, p_err));
    BREAK_ON_ERR(dest_lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_dest_vol->BlkDevHandle, p_err));

    BREAK_ON_ERR(src_lb_nbr = p_src_vol->SysPtr->EntryFirstLbGet(p_src_vol,
                                                                 src_entry_pos,
                                                                 p_err));

    BREAK_ON_ERR(dest_lb_nbr = p_dest_vol->SysPtr->EntryFirstLbGet(p_dest_vol,
                                                                   dest_entry_pos,
                                                                   p_err));

    if (src_lb_size_log2 > dest_lb_size_log2) {
      sub_blks_per_blk_log2 = src_lb_size_log2 - dest_lb_size_log2;
      p_big_vol = p_src_vol;
      p_small_vol = p_dest_vol;
      big_lb_nbr = src_lb_nbr;
      small_lb_nbr = dest_lb_nbr;
    } else {
      sub_blks_per_blk_log2 = dest_lb_size_log2 - src_lb_size_log2;
      p_big_vol = p_dest_vol;
      p_small_vol = p_src_vol;
      big_lb_nbr = dest_lb_nbr;
      small_lb_nbr = src_lb_nbr;
    }

    prev_big_lb_nbr = 0u;
    small_lb_rd_wr_cnt = 0u;
    small_lb_rd_wr_size = 0u;
    small_lb_offset = 0u;
    sub_blks_per_blk = FS_UTIL_PWR2(sub_blks_per_blk_log2);
    sub_blks_per_blk_ix = 0u;

    BREAK_ON_ERR(while) ((big_lb_nbr != p_big_vol->SysPtr->VoidDataLbNbr)
                         && (small_lb_nbr != p_small_vol->SysPtr->VoidDataLbNbr)
                         && (src_file_size > small_lb_rd_wr_size)) {
      small_lb_cnt = DEF_MIN(sub_blks_per_blk, FS_FILE_COPY_LB_TBL_MAX_SIZE);
      //                                                           Gather upfront all small log blks to rd/wr from/to...
      //                                                           ...It allows to use one cache buffer during cpy op.
      BREAK_ON_ERR(for) (small_lb_tbl_ix = 0u; small_lb_tbl_ix < small_lb_cnt; small_lb_tbl_ix++) {
        small_lb_tbl[small_lb_tbl_ix] = small_lb_nbr;
        BREAK_ON_ERR(small_lb_nbr = p_small_vol->SysPtr->NextLbGet(p_small_vol,
                                                                   small_lb_nbr,
                                                                   p_err));
        if (small_lb_nbr == p_small_vol->SysPtr->VoidDataLbNbr) {
          break;
        }
      }
      //                                                           Src log blk bigger than dest log blk.
      if (src_lb_size_log2 > dest_lb_size_log2) {
        BREAK_ON_ERR(FS_CACHE_LOCK_WITH) (p_cache) {
          BREAK_ON_ERR(p_buf = FSCache_BlkAlloc(p_cache,
                                                src_lb_size_log2,
                                                p_err));

          //                                                       Rd 1 big blk from src media.
          BREAK_ON_ERR(FSCache_LbInvalidate(p_cache,
                                            p_src_vol->BlkDevHandle,
                                            p_src_vol->PartitionStart + big_lb_nbr,
                                            p_err));

          BREAK_ON_ERR(FSBlkDev_Rd(p_src_vol->BlkDevHandle,
                                   p_buf,
                                   p_src_vol->PartitionStart + big_lb_nbr,
                                   1u,
                                   p_err));

          //                                                       Wr N small blks to dest media.
          BREAK_ON_ERR(for) (small_lb_tbl_ix = 0u; small_lb_tbl_ix < small_lb_cnt; small_lb_tbl_ix++) {
            BREAK_ON_ERR(FSCache_LbInvalidate(p_cache,
                                              p_dest_vol->BlkDevHandle,
                                              p_dest_vol->PartitionStart + small_lb_tbl[small_lb_tbl_ix],
                                              p_err));

            BREAK_ON_ERR(FSBlkDev_Wr(p_dest_vol->BlkDevHandle,
                                     p_buf + FS_UTIL_MULT_PWR2(sub_blks_per_blk_ix, dest_lb_size_log2),
                                     p_dest_vol->PartitionStart + small_lb_tbl[small_lb_tbl_ix],
                                     1u,
                                     p_err));

            small_lb_rd_wr_size += FS_UTIL_PWR2(dest_lb_size_log2);
            if (small_lb_rd_wr_size > src_file_size) {
              small_lb_cnt = small_lb_tbl_ix + 1u;
              break;                                            // Quit main loop because src file entirely copied.
            }
            sub_blks_per_blk_ix++;
          }

          FSCache_BlkFree(p_cache, p_buf);
        }
      } else {                                                  // Dest log blk bigger than src log blk.
        BREAK_ON_ERR(FS_CACHE_LOCK_WITH) (p_cache) {
          BREAK_ON_ERR(p_buf = FSCache_BlkAlloc(p_cache,
                                                dest_lb_size_log2,
                                                p_err));

          if (big_lb_nbr == prev_big_lb_nbr) {                  // Verify if partial big blk wr in progress.
                                                                // If yes, need to rd big blk previously partially wr.
            BREAK_ON_ERR(FSCache_LbInvalidate(p_cache,
                                              p_dest_vol->BlkDevHandle,
                                              p_dest_vol->PartitionStart + big_lb_nbr,
                                              p_err));

            BREAK_ON_ERR(FSBlkDev_Rd(p_dest_vol->BlkDevHandle,
                                     p_buf,
                                     p_dest_vol->PartitionStart + big_lb_nbr,
                                     1u,
                                     p_err));
          }
          //                                                       Rd N small blks from src media.
          BREAK_ON_ERR(for) (small_lb_tbl_ix = 0u; small_lb_tbl_ix < small_lb_cnt; small_lb_tbl_ix++) {
            BREAK_ON_ERR(FSCache_LbInvalidate(p_cache,
                                              p_src_vol->BlkDevHandle,
                                              p_src_vol->PartitionStart + small_lb_tbl[small_lb_tbl_ix],
                                              p_err));

            BREAK_ON_ERR(FSBlkDev_Rd(p_src_vol->BlkDevHandle,
                                     p_buf + FS_UTIL_MULT_PWR2(sub_blks_per_blk_ix, src_lb_size_log2),
                                     p_src_vol->PartitionStart + small_lb_tbl[small_lb_tbl_ix],
                                     1u,
                                     p_err));

            small_lb_rd_wr_size += FS_UTIL_PWR2(src_lb_size_log2);
            if (small_lb_rd_wr_size > src_file_size) {
              small_lb_cnt = small_lb_tbl_ix + 1u;
              break;                                            // Avoid reading extra src blks because whole src read.
            }
            sub_blks_per_blk_ix++;
          }
          //                                                       Wr 1 big blk to dest media.
          BREAK_ON_ERR(FSCache_LbInvalidate(p_cache,
                                            p_dest_vol->BlkDevHandle,
                                            p_dest_vol->PartitionStart + big_lb_nbr,
                                            p_err));

          BREAK_ON_ERR(FSBlkDev_Wr(p_dest_vol->BlkDevHandle,
                                   p_buf,
                                   p_dest_vol->PartitionStart + big_lb_nbr,
                                   1u,
                                   p_err));

          FSCache_BlkFree(p_cache, p_buf);
        }
      }

      prev_big_lb_nbr = big_lb_nbr;
      small_lb_rd_wr_cnt += small_lb_cnt;
      small_lb_offset = FS_UTIL_MODULO_PWR2(small_lb_rd_wr_cnt, sub_blks_per_blk_log2);

      if (small_lb_offset == 0u) {
        //                                                         Retrieve nxt big log blk nbr.
        BREAK_ON_ERR(big_lb_nbr = p_big_vol->SysPtr->NextLbGet(p_big_vol,
                                                               big_lb_nbr,
                                                               p_err));
        sub_blks_per_blk_ix = 0u;                               // Reset nxt big blk ix.
      }
    }
  } WITH_SCOPE_END
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FSFile_InterCacheCopy()
 *
 * @brief    Copy file where source and destination devices have different caches.
 *
 * @param    p_src_vol       Pointer to source volume.
 *
 * @param    p_dest_vol      Pointer to destination volume.
 *
 * @param    src_entry_pos   Source entry position.
 *
 * @param    dest_entry_pos  Destination entry position.
 *
 * @param    src_file_size   Source file size.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) The file copy uses the internal FS cache. The destination cache block will be written
 *               to the destination media during a flush point. The macro FS_MEDIA_CACHE_BLK_ADD_WR()
 *               is a flush point. Thus each time a new destination cache block is needed, the
 *               previous one will be flushed (i.e. written) to the destination media.
 *               When the source file has been completely copied, the last destination cache block may
 *               not have been written to the media upon exit of FSFile_InterCacheCopy(). The
 *               destination cache block will be written during the next flush point, that is during
 *               a volume auto-sync or during an explicit media sync.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSFile_InterCacheCopy(FS_VOL     *p_src_vol,
                                  FS_VOL     *p_dest_vol,
                                  FS_SYS_POS src_entry_pos,
                                  FS_SYS_POS dest_entry_pos,
                                  CPU_SIZE_T src_file_size,
                                  RTOS_ERR   *p_err)
{
  CPU_INT08U *p_src_buf;
  CPU_INT08U *p_dest_buf;
  FS_LB_NBR  src_lb_nbr;
  FS_LB_NBR  dest_lb_nbr;
  FS_LB_NBR  lb_cnt;
  FS_LB_NBR  lb_ix;
  CPU_SIZE_T src_file_size_copied = 0u;
  CPU_INT08U src_lb_size_log2;
  CPU_INT08U dest_lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(src_lb_nbr = p_src_vol->SysPtr->EntryFirstLbGet(p_src_vol,
                                                                 src_entry_pos,
                                                                 p_err));

    BREAK_ON_ERR(dest_lb_nbr = p_dest_vol->SysPtr->EntryFirstLbGet(p_dest_vol,
                                                                   dest_entry_pos,
                                                                   p_err));

    //                                                             Src log blk bigger than dest log blk.
    BREAK_ON_ERR(src_lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_src_vol->BlkDevHandle, p_err));
    BREAK_ON_ERR(dest_lb_size_log2 = FSBlkDev_LbSizeLog2Get(p_dest_vol->BlkDevHandle, p_err));
    if (src_lb_size_log2 > dest_lb_size_log2) {
      BREAK_ON_ERR(while) (src_lb_nbr != p_src_vol->SysPtr->VoidDataLbNbr
                           && src_file_size > src_file_size_copied) {
        //                                                         Rd 1 log blk from src media into cache blk.
        BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(p_src_vol,
                                          src_lb_nbr,
                                          FS_LB_TYPE_DATA,
                                          &p_src_buf,
                                          p_err) {
          lb_cnt = FS_UTIL_PWR2(src_lb_size_log2 - dest_lb_size_log2);
          //                                                       Wr N blks to dest media.
          BREAK_ON_ERR(for) (lb_ix = 0; lb_ix < lb_cnt; lb_ix++){
            //                                                     Get 1 src cache blk (flush 1 blk if necessary).
            BREAK_ON_ERR(FS_VOL_CACHE_BLK_WR)(p_dest_vol,
                                              dest_lb_nbr,
                                              FS_LB_TYPE_DATA,
                                              FSCache_VoidWrJobHandle,
                                              &p_dest_buf,
                                              DEF_NULL,
                                              p_err) {
              //                                                   Copy src cache blk in dest cache blk (see Note #1).
              Mem_Copy(p_dest_buf,
                       p_src_buf + FS_UTIL_MULT_PWR2(lb_ix, dest_lb_size_log2),
                       FS_UTIL_PWR2(dest_lb_size_log2));

              src_file_size_copied += dest_lb_size_log2;
              if (src_file_size_copied > src_file_size) {
                lb_ix = lb_cnt;                                 // Allows to quit FOR loop.
                break;                                          // Quit main loop because src file entirely copied.
              }
            }
            BREAK_ON_ERR(dest_lb_nbr = p_dest_vol->SysPtr->NextLbGet(p_dest_vol,
                                                                     dest_lb_nbr,
                                                                     p_err));
          }
        }
        BREAK_ON_ERR(src_lb_nbr = p_src_vol->SysPtr->NextLbGet(p_src_vol,
                                                               src_lb_nbr,
                                                               p_err));
      }
    } else {                                                    // Dest log blk bigger than src log blk.
      BREAK_ON_ERR(while) (dest_lb_nbr != p_dest_vol->SysPtr->VoidDataLbNbr
                           && src_file_size > src_file_size_copied) {
        //                                                         Get 1 dest cache blk (flush 1 blk if necessary).
        BREAK_ON_ERR(FS_VOL_CACHE_BLK_WR)(p_dest_vol,
                                          dest_lb_nbr,
                                          FS_LB_TYPE_DATA,
                                          FSCache_VoidWrJobHandle,
                                          &p_dest_buf,
                                          DEF_NULL,
                                          p_err) {
          lb_cnt = FS_UTIL_PWR2(dest_lb_size_log2 - src_lb_size_log2);
          //                                                       Rd N blks from src media and copy in dest cache blk.
          BREAK_ON_ERR(for) (lb_ix = 0; lb_ix < lb_cnt; lb_ix++){
            //                                                     Rd 1 log blk from src media into cache blk.
            BREAK_ON_ERR(FS_VOL_CACHE_BLK_RD)(p_src_vol,
                                              src_lb_nbr,
                                              FS_LB_TYPE_DATA,
                                              &p_src_buf,
                                              p_err) {
              //                                                   Copy src cache blk in dest cache blk (see Note #1).
              Mem_Copy(p_dest_buf + FS_UTIL_MULT_PWR2(lb_ix, src_lb_size_log2),
                       p_src_buf,
                       FS_UTIL_PWR2(src_lb_size_log2));

              src_file_size_copied += FS_UTIL_PWR2(src_lb_size_log2);
              if (src_file_size_copied > src_file_size) {
                lb_ix = lb_cnt;                                 // Allows to quit FOR loop.
                break;                                          // Quit main loop because src file entirely copied.
              }
            }
            BREAK_ON_ERR(src_lb_nbr = p_dest_vol->SysPtr->NextLbGet(p_src_vol,
                                                                    src_lb_nbr,
                                                                    p_err));
          }
        }
        BREAK_ON_ERR(dest_lb_nbr = p_dest_vol->SysPtr->NextLbGet(p_dest_vol,
                                                                 dest_lb_nbr,
                                                                 p_err));
      }
    }
  } WITH_SCOPE_END
}
#endif
#endif

/****************************************************************************************************//**
 *                                           FSFile_OnNullDescRefCnt()
 *
 * @brief    On file descriptor null reference count callback.
 *
 * @param    p_obj   Pointer to a file descriptor.
 *
 * @note     (1) The file descriptor will be freed regardless of possible errors occurring while:
 *               - (a) flushing the file buffer;
 *               - (b) updating the time stamps;
 *               - (c) performing any operation related to detaching from a parent object.
 *******************************************************************************************************/
static void FSFile_Desc_OnNullRefCnt(FS_OBJ *p_obj)
{
  FS_FILE_DESC *p_desc;
  FS_FILE_NODE *p_node;

  p_desc = (FS_FILE_DESC *)p_obj;
  p_node = p_desc->FileNodePtr;

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
  KAL_LockDel(p_desc->FileLockHandle);
#endif

  //                                                               See Note #1c.
  FSObj_RefRelease((FS_OBJ *)p_desc->FileNodePtr, FSFile_Node_OnNullRefCnt);

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR) {
    SList_Rem(&FSFile_OpenDescListHeadPtr, &p_desc->OpenListMember);
  }

  p_node->SysPtr->FileDescFree(p_desc);
}

/****************************************************************************************************//**
 *                                           FSFile_OnNullNodeRefCnt()
 *
 * @brief    On file node null reference count callback.
 *
 * @param    p_obj   Pointer to a file node.
 *******************************************************************************************************/
static void FSFile_Node_OnNullRefCnt(FS_OBJ *p_obj)
{
  FS_FILE_NODE *p_node;
  RTOS_ERR     err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  p_node = (FS_FILE_NODE *)p_obj;

  FS_VOL_WITH(p_node->VolHandle, &err) {
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
    if (p_node->VolHandle.VolPtr->AutoSync) {
      p_node->SysPtr->FileSync(p_node, &err);
    }
#endif
    FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR) {
      SList_Rem(&FSEntry_OpenListHeadPtr, &p_node->OpenListMember);
    }
  }

  p_node->SysPtr->FileNodeFree(p_node);
  PP_UNUSED_PARAM(err);
}

/****************************************************************************************************//**
 *                                           FSFile_Desc_ToObjHandle()
 *
 * @brief    Convert file handle to generic object handle.
 *
 * @param    file_handle     Handle to a file.
 *
 * @return   Handle to generic object.
 *******************************************************************************************************/
static FS_OBJ_HANDLE FSFile_Desc_ToObjHandle(FS_FILE_HANDLE file_handle)
{
  FS_OBJ_HANDLE obj_handle;

  obj_handle.ObjPtr = (FS_OBJ *)file_handle.FileDescPtr;
  obj_handle.ObjId = file_handle.FileDescId;

  return (obj_handle);
}

/****************************************************************************************************//**
 *                                           FSFile_Desc_ToObj()
 *
 * @brief    Convert file descriptor to associated structure object.
 *
 * @param    p_file_desc     Pointer to file descriptor.
 *
 * @return   Pointer to associated structure object.
 *******************************************************************************************************/
static FS_OBJ *FSFile_Desc_ToObj(FS_FILE_DESC *p_file_desc)
{
  return ((FS_OBJ *)p_file_desc);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
