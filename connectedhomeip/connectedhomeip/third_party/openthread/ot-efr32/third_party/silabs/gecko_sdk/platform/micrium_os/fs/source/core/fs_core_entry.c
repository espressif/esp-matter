/***************************************************************************//**
 * @file
 * @brief File System - Core Directory Operations
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
#include  <fs/include/fs_core_file.h>
#include  <fs/source/core/fs_core_entry_priv.h>
#include  <fs/source/core/fs_core_op_priv.h>
#include  <fs/source/shared/fs_obj_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_file_priv.h>
#include  <fs/source/core/fs_core_working_dir_priv.h>
#include  <fs/source/core/fs_core_dir_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

// ----------------------- EXT ------------------------
#include  <common/include/lib_utils.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

#include  <sl_sleeptimer.h>
#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    RTOS_MODULE_CUR    RTOS_CFG_MODULE_FS
#define    LOG_DFLT_CH        (FS, CORE, ENTRY)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSEntry_DelLockedAcquired(FS_ENTRY_LOC_DATA *p_entry_loc_data,
                                      FS_FLAGS          entry_type,
                                      RTOS_ERR          *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_ENTRY_NODE_HANDLE FSEntry_NodeNullHandle = { 0 };

SLIST_MEMBER *FSEntry_OpenListHeadPtr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSEntry_Create()
 *
 * @brief    Create a file or directory.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @param    p_path          Entry path relative to the given working directory.
 *
 * @param    entry_type      Indicates whether the new entry is a directory / a file :
 *                               - FS_ENTRY_TYPE_DIR,  if the entry can be a directory.
 *                               - FS_ENTRY_TYPE_FILE, if the entry can be a file.
 *
 * @param    excl            Indicates whether the creation of new entry shall be exclusive :
 *                           DEF_YES, if the entry shall be created ONLY if entry does not exist.
 *                           DEF_NO,  if the entry shall be created even if entry does exist.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ALREADY_EXISTS
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_NAME_INVALID
 *                               - RTOS_ERR_ENTRY_OPENED
 *                               - RTOS_ERR_ENTRY_ROOT_DIR
 *                               - RTOS_ERR_ENTRY_PARENT_NOT_DIR
 *                               - RTOS_ERR_WRK_DIR_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) The function behavior is resumed below.
 *           @verbatim
 *               Entry type      |   'excl'   |      Entry       |  Entry type   |       Behavior
 *               argument        |  argument  | already exists?  |    on disk    |
 *           --------------------+------------+------------------+---------------+-----------------------
 *                   X           |     X      |       no         |     ---       |  Create entry
 *           FS_ENTRY_TYPE_FILE  |  DEF_NO    |       yes        |     file      |  Truncate file
 *           FS_ENTRY_TYPE_FILE  |  DEF_YES   |       yes        |     file      |  Error: already exists
 *           FS_ENTRY_TYPE_FILE  |     X      |       yes        |     dir       |  Error: type mismatch
 *           FS_ENTRY_TYPE_DIR   |     X      |       yes        |     file      |  Error: type mismatch
 *           FS_ENTRY_TYPE_DIR   |  DEF_NO    |       yes        |     dir       |  Nothing is done
 *           FS_ENTRY_TYPE_DIR   |  DEF_YES   |       yes        |     dir       |  Error: already exists
 *
 *           "X"   means "don't care".
 *           "---" means "not relevant".
 *
 *           @endverbatim
 *
 * @note     (2) The root directory may NOT be created.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSEntry_Create(FS_WRK_DIR_HANDLE wrk_dir_handle,
                    const CPU_CHAR    *p_path,
                    FS_FLAGS          entry_type,
                    CPU_BOOLEAN       excl,
                    RTOS_ERR          *p_err)
{
  FS_VOL               *p_vol;
  CPU_CHAR             *p_entry_name;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  FS_WRK_DIR_HANDLE    open_wrk_dir_handle;
  FS_SYS_POS           entry_pos_data;
  FS_SYS_POS           parent_pos_data;
  FS_ENTRY_LOC_DATA    entry_loc_data;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             --------------------- ARG CHK ----------------------
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET((entry_type == FS_ENTRY_TYPE_FILE)
                            || (entry_type == FS_ENTRY_TYPE_DIR),
                            *p_err, RTOS_ERR_INVALID_ARG,; );

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(wrk_dir_handle,
                                            (CPU_CHAR *)p_path,
                                            &entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);

    FS_OP_LOCK_WITH(&entry_loc_data.VolHandle.VolPtr->OpLock, FS_VOL_OP_ENTRY_CREATE_DEL) {
      BREAK_ON_ERR(FS_VOL_WITH) (entry_loc_data.VolHandle, p_err) {
        BREAK_ON_ERR((void)FSEntry_Lookup(&entry_loc_data,
                                          &parent_pos_data,
                                          &entry_pos_data,
                                          DEF_NULL,
                                          p_err));

        p_vol = entry_loc_data.VolHandle.VolPtr;

        ASSERT_BREAK(p_vol->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                     RTOS_ERR_VOL_RD_ONLY);

        ASSERT_BREAK_LOG(parent_pos_data != p_vol->SysPtr->VoidEntryPos,
                         RTOS_ERR_NOT_FOUND, ("Parent entry not found."));

        ASSERT_BREAK_LOG(entry_pos_data != p_vol->SysPtr->RootDirPosData,
                         RTOS_ERR_ENTRY_ROOT_DIR, ("Cannot create root directory."));

        //                                                         ------------------ EXISTING ENTRY ------------------
        if (entry_pos_data != p_vol->SysPtr->VoidEntryPos) {
          entry_node_handle = FSEntry_OpenFind(p_vol, entry_pos_data);
          ASSERT_BREAK_LOG(FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle), RTOS_ERR_ENTRY_OPENED,
                           ("Cannot create entry: entry exists and is opened."));

          open_wrk_dir_handle = FSWrkDir_OpenFind(p_vol, entry_pos_data);
          ASSERT_BREAK_LOG(FS_WRK_DIR_HANDLE_IS_NULL(open_wrk_dir_handle), RTOS_ERR_ENTRY_OPENED,
                           ("Cannot create entry: entry exists and is opened."));

          //                                                       -------------------- EXCL FLAG ---------------------
          if (!excl) {
            if (entry_type == FS_ENTRY_TYPE_FILE) {
              BREAK_ON_ERR(p_vol->SysPtr->FileTruncate(p_vol,
                                                       DEF_NULL,
                                                       entry_pos_data,
                                                       0u,
                                                       p_err));
            }
          } else {
            LOG_ERR(("\"", (s)p_path, "\" path already exists."));
            BREAK_ERR_SET(RTOS_ERR_ALREADY_EXISTS);
          }
        } else {                                                // ----------------- NEW ENTRY CREATE -----------------
          p_entry_name = FSCore_PathLastSegGet((CPU_CHAR *)p_path);
          BREAK_ON_ERR(entry_pos_data = p_vol->SysPtr->EntryCreate(p_vol,
                                                                   parent_pos_data,
                                                                   p_entry_name,
                                                                   entry_type == FS_ENTRY_TYPE_DIR,
                                                                   p_err));
        }

        //                                                         -------------------- AUTO SYNC ---------------------
        if (p_vol->AutoSync) {
          BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));
        }
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSEntry_Del()
 *
 * @brief    Delete a file or directory.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @param    p_path          Entry path relative to the given working directory.
 *
 * @param    entry_type      Indicates whether the entry to delete is a directory or a file :
 *                               - FS_ENTRY_TYPE_DIR,  if the entry is a directory.
 *                               - FS_ENTRY_TYPE_FILE, if the entry is a file.
 *                               - FS_ENTRY_TYPE_ANY,  if the entry is any type.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_ENTRY_OPENED
 *                               - RTOS_ERR_ENTRY_ROOT_DIR
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_NAME_INVALID
 *                               - RTOS_ERR_DIR_NOT_EMPTY
 *                               - RTOS_ERR_WRK_DIR_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) When a file is removed, the space occupied by the file is freed and is no longer
 *               accessible.
 *
 * @note     (2) The root directory cannot be deleted.
 *
 * @note     (3) A directory can be removed only if it is an empty directory.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSEntry_Del(FS_WRK_DIR_HANDLE wrk_dir_handle,
                 const CPU_CHAR    *p_path,
                 FS_FLAGS          entry_type,
                 RTOS_ERR          *p_err)
{
  FS_VOL            *p_vol;
  FS_ENTRY_LOC_DATA entry_loc_data;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(((entry_type &  FS_ENTRY_TYPE_ANY) != 0)
                            && ((entry_type & ~FS_ENTRY_TYPE_ANY) == 0),
                            *p_err, RTOS_ERR_INVALID_TYPE,; );

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(wrk_dir_handle,
                                            (CPU_CHAR *)p_path,
                                            &entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);
    FS_OP_LOCK_WITH(&entry_loc_data.VolHandle.VolPtr->OpLock, FS_VOL_OP_ENTRY_CREATE_DEL) {
      ASSERT_BREAK(entry_loc_data.VolHandle.VolPtr->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                   RTOS_ERR_VOL_RD_ONLY);

      BREAK_ON_ERR(FS_VOL_WITH) (entry_loc_data.VolHandle, p_err) {
        //                                                         ---------------------- DELETE ----------------------
        BREAK_ON_ERR(FSEntry_DelLockedAcquired(&entry_loc_data,
                                               entry_type,
                                               p_err));

        //                                                         -------------------- AUTO SYNC ---------------------
        p_vol = entry_loc_data.VolHandle.VolPtr;
        if (p_vol->AutoSync) {
          BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));
        }
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSEntry_Rename()
 *
 * @brief    Rename a file or directory.
 *
 * @param    src_wrk_dir_handle      Handle to the source working directory.
 *
 * @param    p_src_path              Source entry path relative to the given source working directory.
 *
 * @param    dest_wrk_dir_handle     Handle to the destination working directory.
 *
 * @param    p_dest_path             Destination entry path relative to the given source working directory.
 *
 * @param    excl                    Indicates whether creation of new entry should be exclusive
 *                                   (see Note #3):
 *                                   DEF_YES, if the entry shall be renamed only if the destination
 *                                   entry does not exist.
 *                                   DEF_NO,  if the entry shall be renamed even if the destination
 *                                   entry does exist.
 *
 * @param    p_err                   Pointer to variable that will receive the return error code(s) from
 *                                   this function:
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_NOT_FOUND
 *                                       - RTOS_ERR_ENTRY_ROOT_DIR
 *                                       - RTOS_ERR_ENTRY_PARENT_NOT_DIR
 *                                       - RTOS_ERR_ENTRY_OPENED
 *                                       - RTOS_ERR_ALREADY_EXISTS
 *                                       - RTOS_ERR_NAME_INVALID
 *                                       - RTOS_ERR_INVALID_TYPE
 *                                       - RTOS_ERR_DIR_NOT_EMPTY
 *                                       - RTOS_ERR_WRK_DIR_CLOSED
 *                                       - RTOS_ERR_VOL_CLOSED
 *                                       - RTOS_ERR_VOL_CORRUPTED
 *                                       - RTOS_ERR_BLK_DEV_CLOSED
 *                                       - RTOS_ERR_BLK_DEV_CORRUPTED
 *                                       - RTOS_ERR_IO
 *
 * @note     (1) If source and destination entries reside on different volumes, then the source entry
 *               must be a file. If the source entry is a directory, an error will be returned.
 *
 * @note     (2) Depending on source entry:
 *           - (a) If the source and destination entries are the same entry, the volume will not be
 *                   modified and no error will be returned.
 *           - (b) If the source entry is a file:
 *               - (1) ... the destination entry must NOT be a directory.
 *               - (2) ... if 'excl' is DEF_NO and the destination entry is a file, the destination
 *                       entry will be deleted.
 *           - (c) If the source entry is a directory:
 *               - (1) ... the destination entry must NOT be a file.
 *               - (2) ... if 'excl' is DEF_NO and the destination entry is a directory, the
 *                       destination entry MUST be empty; if so, it will be deleted.
 *
 * @note     (3) If 'excl' is DEF_NO, the destination entry must not exist.
 *
 * @note     (4) The root directory may NOT be renamed.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSEntry_Rename(FS_WRK_DIR_HANDLE src_wrk_dir_handle,
                    const CPU_CHAR    *p_src_path,
                    FS_WRK_DIR_HANDLE dest_wrk_dir_handle,
                    const CPU_CHAR    *p_dest_path,
                    CPU_BOOLEAN       excl,
                    RTOS_ERR          *p_err)
{
  FS_VOL               *p_src_vol;
  FS_VOL               *p_dest_vol;
  CPU_CHAR             *p_dest_entry_name;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  FS_SYS_POS           src_entry_pos_data;
  FS_SYS_POS           dest_entry_pos_data;
  FS_SYS_POS           dest_parent_pos_data;
  FS_ENTRY_INFO        dest_entry_info;
  FS_ENTRY_INFO        src_entry_info;
  CPU_BOOLEAN          dest_entry_exists;
  FS_ENTRY_LOC_DATA    src_entry_loc_data;
  FS_ENTRY_LOC_DATA    dest_entry_loc_data;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(src_wrk_dir_handle,
                                            (CPU_CHAR *)p_src_path,
                                            &src_entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(src_entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(dest_wrk_dir_handle,
                                            (CPU_CHAR *)p_dest_path,
                                            &dest_entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(dest_entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);

    BREAK_ON_ERR(FS_VOL_WITH) (src_entry_loc_data.VolHandle, p_err) {
      BREAK_ON_ERR(FS_VOL_WITH) (dest_entry_loc_data.VolHandle, p_err) {
        p_src_vol = src_entry_loc_data.VolHandle.VolPtr;
        p_dest_vol = dest_entry_loc_data.VolHandle.VolPtr;

        ASSERT_BREAK(p_src_vol->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                     RTOS_ERR_VOL_RD_ONLY);

        ASSERT_BREAK(p_dest_vol->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                     RTOS_ERR_VOL_RD_ONLY);

        if (p_src_vol != p_dest_vol) {                          // -------------------- CROSS VOL ---------------------
#if (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED)
          FS_OP_LOCK_WITH(&p_dest_vol->OpLock, FS_VOL_OP_FILE_WR) {
            FS_OP_LOCK_WITH(&p_src_vol->OpLock, FS_VOL_OP_FILE_RD) {
              BREAK_ON_ERR(FSFile_CopyLockedAcquired(&src_entry_loc_data,
                                                     &dest_entry_loc_data,
                                                     excl,
                                                     p_err));

              BREAK_ON_ERR(FSEntry_DelLockedAcquired(&src_entry_loc_data,
                                                     FS_ENTRY_TYPE_ANY,
                                                     p_err));
            }
          }
#else
          BREAK_ERR_SET(RTOS_ERR_NOT_AVAIL);
#endif
        } else {                                                // -------------------- WITHIN VOL --------------------
          BREAK_ON_ERR(FS_OP_LOCK_WITH) (&p_src_vol->OpLock, FS_VOL_OP_FILE_WR) {
            //                                                     Find src.
            BREAK_ON_ERR((void)FSEntry_Lookup(&src_entry_loc_data,
                                              DEF_NULL,
                                              &src_entry_pos_data,
                                              DEF_NULL,
                                              p_err));

            //                                                     Source must exist.
            ASSERT_BREAK_LOG(src_entry_pos_data != p_src_vol->SysPtr->VoidEntryPos,
                             RTOS_ERR_NOT_FOUND, ("Source file not found."));

            //                                                     Cannot rename root dir.
            ASSERT_BREAK_LOG(src_entry_pos_data != p_src_vol->SysPtr->RootDirPosData,
                             RTOS_ERR_ENTRY_ROOT_DIR, ("Cannot rename root directory."));

            //                                                     Cannot rename opened entry.
            entry_node_handle = FSEntry_OpenFind(p_src_vol, src_entry_pos_data);
            ASSERT_BREAK_LOG(FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle), RTOS_ERR_ENTRY_OPENED,
                             ("Cannot rename an opened entry."));

            BREAK_ON_ERR(p_src_vol->SysPtr->EntryQuery(p_src_vol,
                                                       src_entry_pos_data,
                                                       &src_entry_info,
                                                       DEF_NULL,
                                                       DEF_NULL,
                                                       0u,
                                                       p_err));
#if (FS_CORE_CFG_DIR_EN != DEF_ENABLED)
            if (src_entry_info.Attrib.IsDir) {                  // If src is dir, dir support must be enabled.
              BREAK_ERR_SET(RTOS_ERR_NOT_AVAIL);
            }
#endif

            //                                                     Find dest's parent.
            BREAK_ON_ERR((void)FSEntry_Lookup(&dest_entry_loc_data,
                                              &dest_parent_pos_data,
                                              &dest_entry_pos_data,
                                              DEF_NULL,
                                              p_err));

            ASSERT_BREAK_LOG(dest_parent_pos_data != p_dest_vol->SysPtr->VoidEntryPos,
                             RTOS_ERR_NOT_FOUND, ("Destination parent entry not found."));

            BREAK_ON_ERR(p_dest_vol->SysPtr->EntryQuery(p_dest_vol,
                                                        dest_parent_pos_data,
                                                        &dest_entry_info,
                                                        DEF_NULL,
                                                        DEF_NULL,
                                                        0u,
                                                        p_err));

            //                                                     Dest's parent must be a dir.
            ASSERT_BREAK(dest_entry_info.Attrib.IsDir == DEF_YES, RTOS_ERR_ENTRY_PARENT_NOT_DIR);

            //                                                     If excl mode, dest entry must not exist.
            dest_entry_exists = (dest_entry_pos_data != p_dest_vol->SysPtr->VoidEntryPos);
            ASSERT_BREAK(!dest_entry_exists || !excl, RTOS_ERR_ALREADY_EXISTS);

            if (dest_entry_exists) {
              //                                                   If src & dest entry are the same...
              //                                                   ...return without doing anything.
              if (dest_entry_pos_data == src_entry_pos_data) {
                goto end_scope;
              }

              BREAK_ON_ERR(p_src_vol->SysPtr->EntryQuery(p_src_vol,
                                                         dest_entry_pos_data,
                                                         &dest_entry_info,
                                                         DEF_NULL,
                                                         DEF_NULL,
                                                         0u,
                                                         p_err));

              //                                                   Entry of one type may not replace ...
              //                                                   ... entry of another type.
              ASSERT_BREAK(dest_entry_info.Attrib.IsDir == src_entry_info.Attrib.IsDir,
                           RTOS_ERR_INVALID_TYPE);

              //                                                   Cannot rename to root dir.
              ASSERT_BREAK(!dest_entry_info.Attrib.IsRootDir, RTOS_ERR_ENTRY_ROOT_DIR);

              if (dest_entry_info.Attrib.IsDir) {               // If dest is dir, chk if empty.
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
                CPU_BOOLEAN is_empty;

                BREAK_ON_ERR(is_empty = p_src_vol->SysPtr->DirChkEmpty(p_src_vol,
                                                                       &dest_entry_pos_data,
                                                                       p_err));
                ASSERT_BREAK(is_empty, RTOS_ERR_DIR_NOT_EMPTY);
#else
                //                                                 If dest is dir, dir support must be enabled.
                BREAK_ERR_SET(RTOS_ERR_NOT_AVAIL);
#endif
              }
            }

            p_dest_entry_name = FSCore_PathLastSegGet((CPU_CHAR *)p_dest_path);
            //                                                     Perform entry rename according to file system type.
            BREAK_ON_ERR(p_src_vol->SysPtr->EntryRename(p_src_vol,
                                                        src_entry_pos_data,
                                                        dest_entry_pos_data,
                                                        dest_parent_pos_data,
                                                        p_dest_entry_name,
                                                        dest_entry_exists,
                                                        p_err));
end_scope:  ;
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

/****************************************************************************************************//**
 *                                               FSEntry_Query()
 *
 * @brief    Get information about a file or directory.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @param    p_path          Entry path relative to the given working directory.
 *
 * @param    p_entry_info    Pointer to structure that will receive the entry information.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_WRK_DIR_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/
void FSEntry_Query(FS_WRK_DIR_HANDLE wrk_dir_handle,
                   const CPU_CHAR    *p_path,
                   FS_ENTRY_INFO     *p_entry_info,
                   RTOS_ERR          *p_err)

{
  FS_VOL            *p_vol;
  FS_SYS_POS        entry_pos_data;
  FS_ENTRY_LOC_DATA entry_loc_data;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ASSERT_DBG_ERR_SET(p_entry_info != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(wrk_dir_handle,
                                            (CPU_CHAR *)p_path,
                                            &entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);

    BREAK_ON_ERR(FS_VOL_WITH) (entry_loc_data.VolHandle, p_err) {
      p_vol = entry_loc_data.VolHandle.VolPtr;

      BREAK_ON_ERR(FS_OP_LOCK_WITH) (&p_vol->OpLock, FS_VOL_OP_ENTRY_QUERY) {
        BREAK_ON_ERR((void)FSEntry_Lookup(&entry_loc_data,
                                          DEF_NULL,
                                          &entry_pos_data,
                                          DEF_NULL,
                                          p_err));

        ASSERT_BREAK_LOG(entry_pos_data != p_vol->SysPtr->VoidEntryPos,
                         RTOS_ERR_NOT_FOUND, ("Entry not found."));

        BREAK_ON_ERR(p_vol->SysPtr->EntryQuery(p_vol,
                                               entry_pos_data,
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
 *                                           FSEntry_AttribSet()
 *
 * @brief    Set a file or directory's attributes.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @param    p_path          Entry path relative to the given working directory.
 *
 * @param    attrib          Entry attributes to set (see Note #2).
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_ENTRY_OPENED
 *                               - RTOS_ERR_ENTRY_ROOT_DIR
 *                               - RTOS_ERR_WRK_DIR_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) If the entry does not exist, an error is returned.
 *
 * @note     (2) Three entry attributes may be modified by this function :
 *               - FS_ENTRY_ATTRIB_RD         Entry is readable.
 *               - FS_ENTRY_ATTRIB_WR         Entry is writable.
 *               - FS_ENTRY_ATTRIB_HIDDEN     Entry is hidden from user-level processes.
 *
 *           - (a) An attribute will be cleared if its flag is not OR'd into 'attrib'.
 *                 The attribute will be set otherwise.
 *           - (b) If another flag besides these is set, then an error will be returned.
 *
 * @note     (3) The attributes of the root directory may NOT be set.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSEntry_AttribSet(FS_WRK_DIR_HANDLE wrk_dir_handle,
                       const CPU_CHAR    *p_path,
                       FS_FLAGS          attrib,
                       RTOS_ERR          *p_err)
{
  FS_VOL               *p_vol;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  FS_SYS_POS           entry_pos;
  FS_ENTRY_LOC_DATA    entry_loc_data;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ASSERT_DBG_ERR_SET(DEF_BIT_IS_SET_ANY(attrib,
                                               (FS_ENTRY_ATTRIB_RD
                                                | FS_ENTRY_ATTRIB_WR
                                                | FS_ENTRY_ATTRIB_HIDDEN)),
                            *p_err, RTOS_ERR_INVALID_ARG,; );

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(wrk_dir_handle,
                                            (CPU_CHAR *)p_path,
                                            &entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);

    BREAK_ON_ERR(FS_VOL_WITH) (entry_loc_data.VolHandle, p_err) {
      //                                                           ------------------- ENTRY LOOKUP -------------------
      BREAK_ON_ERR((void)FSEntry_Lookup(&entry_loc_data,
                                        DEF_NULL,
                                        &entry_pos,
                                        DEF_NULL,
                                        p_err));

      p_vol = entry_loc_data.VolHandle.VolPtr;

      ASSERT_BREAK(p_vol->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                   RTOS_ERR_VOL_RD_ONLY);

      ASSERT_BREAK_LOG(entry_pos != p_vol->SysPtr->VoidEntryPos,
                       RTOS_ERR_NOT_FOUND, ("File not found"));

      //                                                           See Note #3.
      ASSERT_BREAK_LOG(entry_pos != p_vol->SysPtr->RootDirPosData,
                       RTOS_ERR_ENTRY_ROOT_DIR, ("Cannot set root directory attributes."));

      //                                                           ---------------- CHK IF ENTRY OPEN -----------------
      entry_node_handle = FSEntry_OpenFind(p_vol, entry_pos);
      ASSERT_BREAK_LOG(FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle), RTOS_ERR_ENTRY_OPENED,
                       ("Cannot set attributes of opened entry."));

      //                                                           -------------------- ATTRIB SET --------------------
      BREAK_ON_ERR(p_vol->SysPtr->EntryAttribSet(p_vol,
                                                 entry_pos,
                                                 attrib,
                                                 p_err));

      //                                                           -------------------- AUTO SYNC ---------------------
      if (p_vol->AutoSync) {
        BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));
      }
    }
  } WITH_SCOPE_END
}
#endif

/*****************************************************************************************************//**
 *                                            sl_fs_entry_time_set()
 *
 * @brief    Set a file or directory's date/time.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @param    p_path          Entry path relative to the given working directory.
 *
 * @param    p_time          Pointer to date/time.
 *
 * @param    time_type       Flag to indicate which Date/Time should be set
 *                           FS_DATE_TIME_CREATE
 *                           FS_DATE_TIME_MODIFY
 *                           FS_DATE_TIME_ACCESS
 *                           FS_DATE_TIME_ALL
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_ENTRY_OPENED
 *                               - RTOS_ERR_ENTRY_ROOT_DIR
 *                               - RTOS_ERR_WRK_DIR_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) The date/time of the root directory may NOT be set.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void sl_fs_entry_time_set(FS_WRK_DIR_HANDLE    wrk_dir_handle,
                          const CPU_CHAR       *p_path,
                          sl_sleeptimer_date_t *p_date,
                          CPU_INT08U           time_type,
                          RTOS_ERR             *p_err)
{
  FS_VOL               *p_vol;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  FS_SYS_POS           entry_pos_data;
  FS_ENTRY_INFO        entry_info;
  FS_ENTRY_LOC_DATA    entry_loc_data;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ASSERT_DBG_ERR_SET(p_date != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET(FS_IS_VALID_DATE_TIME_TYPE(time_type), *p_err, RTOS_ERR_INVALID_ARG,; );

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(wrk_dir_handle,
                                            (CPU_CHAR *)p_path,
                                            &entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);

    BREAK_ON_ERR(FS_VOL_WITH) (entry_loc_data.VolHandle, p_err) {
      p_vol = entry_loc_data.VolHandle.VolPtr;

      ASSERT_BREAK(p_vol->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                   RTOS_ERR_VOL_RD_ONLY);

      BREAK_ON_ERR(FS_OP_LOCK_WITH) (&p_vol->OpLock, FS_VOL_OP_FILE_WR) {
        BREAK_ON_ERR((void)FSEntry_Lookup(&entry_loc_data,
                                          DEF_NULL,
                                          &entry_pos_data,
                                          DEF_NULL,
                                          p_err));

        ASSERT_BREAK_LOG(entry_pos_data != p_vol->SysPtr->VoidEntryPos,
                         RTOS_ERR_NOT_FOUND, ("Entry not found."));

        //                                                         Cannot change root dir time.
        ASSERT_BREAK_LOG(entry_pos_data != p_vol->SysPtr->RootDirPosData,
                         RTOS_ERR_ENTRY_ROOT_DIR, ("Cannot change root directory time."));

        entry_node_handle = FSEntry_OpenFind(p_vol, entry_pos_data);
        ASSERT_BREAK_LOG(FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle), RTOS_ERR_ENTRY_OPENED,
                         ("Cannot change opened entry time."));

        BREAK_ON_ERR(p_vol->SysPtr->EntryQuery(p_vol,
                                               entry_pos_data,
                                               &entry_info,
                                               DEF_NULL,
                                               DEF_NULL,
                                               0u,
                                               p_err));

        ASSERT_BREAK_LOG(!entry_info.Attrib.IsRootDir,          // Cannot change root dir time (see Note #1).
                         RTOS_ERR_ENTRY_ROOT_DIR, ("Cannot change root directory time."));

        BREAK_ON_ERR(p_vol->SysPtr->EntryTimeSet(p_vol,
                                                 entry_pos_data,
                                                 p_date,
                                                 time_type,
                                                 p_err));
      }

      //                                                           -------------------- AUTO SYNC ---------------------
      if (p_vol->AutoSync) {
        BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));
      }
    }
  } WITH_SCOPE_END
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSEntry_IsOpen()
 *
 * @brief    Check whether an entry is opened.
 *
 * @param    working_dir_handle  Handle to a working directory.
 *
 * @param    p_path              Entry path relative to the given working directory.
 *
 * @param    p_err               Pointer to variable that will receive the return error code(s) from this
 *                               function:
 *                                   - RTOS_ERR_WRK_DIR_CLOSED
 *                                   - RTOS_ERR_VOL_CLOSED
 *                                   - RTOS_ERR_VOL_CORRUPTED
 *                                   - RTOS_ERR_BLK_DEV_CLOSED
 *                                   - RTOS_ERR_BLK_DEV_CORRUPTED
 *                                   - RTOS_ERR_IO
 *
 * @return   DEF_YES, if entry is opened.
 *           DEF_NO', otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN FSEntry_IsOpen(FS_WRK_DIR_HANDLE wrk_dir_handle,
                           CPU_CHAR          *p_path,
                           RTOS_ERR          *p_err)
{
  FS_VOL               *p_vol;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  FS_SYS_POS           entry_pos_data;
  FS_ENTRY_LOC_DATA    entry_loc_data;
  CPU_BOOLEAN          is_closed;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    is_closed = DEF_NO;

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(wrk_dir_handle,
                                            p_path,
                                            &entry_loc_data,
                                            p_err));

    ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(entry_loc_data.VolHandle), RTOS_ERR_INVALID_ARG);

    BREAK_ON_ERR(FS_VOL_WITH) (entry_loc_data.VolHandle, p_err) {
      BREAK_ON_ERR((void)FSEntry_Lookup(&entry_loc_data,
                                        DEF_NULL,
                                        &entry_pos_data,
                                        DEF_NULL,
                                        p_err));

      p_vol = entry_loc_data.VolHandle.VolPtr;

      if (entry_pos_data == p_vol->SysPtr->VoidEntryPos) {
        is_closed = DEF_YES;
      } else {
        entry_node_handle = FSEntry_OpenFind(p_vol, entry_pos_data);
        is_closed = FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle);
      }
    }
  } WITH_SCOPE_END

  return (is_closed);
}

/****************************************************************************************************//**
 *                                           FSEntry_PathGetInternal()
 *
 * @brief    Get entry path by performing on-disk reverse lookup.
 *
 * @param    p_entry_node    Pointer to an entry node.
 *
 * @param    entry_type      Entry type:
 *                               - FS_ENTRY_TYPE_FILE
 *                               - FS_ENTRY_TYPE_DIR
 *
 * @param    p_path          Pointer to a buffer that will receive the path.
 *
 * @param    path_buf_len    Size of the provided path buffer.
 *
 * @param    p_err           Error pointer.
 *
 * @return   The total number of characters in the path.
 *******************************************************************************************************/
CPU_SIZE_T FSEntry_PathGetInternal(FS_ENTRY_NODE *p_entry_node,
                                   CPU_INT08U    entry_type,
                                   CPU_CHAR      *p_path,
                                   CPU_SIZE_T    path_buf_len,
                                   RTOS_ERR      *p_err)
{
  FS_VOL            *p_vol;
  CPU_CHAR          *p_cur_char;
  CPU_SIZE_T        path_len = 0u;
  CPU_SIZE_T        vol_name_len;
  FS_FILE_NAME_LEN  path_component_len;
  FS_SYS_POS        entry_pos_data;
  CPU_BOOLEAN       first_iter;
  FS_ENTRY_LOC_DATA entry_loc_data;

  WITH_SCOPE_BEGIN(p_err) {
    p_vol = p_entry_node->VolHandle.VolPtr;
    entry_pos_data = p_entry_node->Pos;

    FS_OP_LOCK_WITH(&p_vol->OpLock, FS_VOL_OP_DATA_ACCESS) {
      FS_VOL_WITH(p_entry_node->VolHandle, p_err) {
        //                                                         -------------------- BUILD PATH --------------------
        //                                                         From last to first path segment.
        //                                                         Start with terminating null character.
        if (p_path != DEF_NULL) {
          p_cur_char = &p_path[path_buf_len - 1u];
          *p_cur_char = '\0';
        } else {
          p_cur_char = DEF_NULL;
        }

        path_len = 0u;
        first_iter = DEF_YES;
        BREAK_ON_ERR(while) (entry_pos_data != p_vol->SysPtr->RootDirPosData) {
          BREAK_ON_ERR(p_vol->SysPtr->EntryQuery(p_vol,
                                                 entry_pos_data,
                                                 DEF_NULL,
                                                 p_path,
                                                 &path_component_len,
                                                 p_cur_char - p_path,
                                                 p_err));
          path_len += path_component_len + 1u;                  // +1 to account for path sep.

          if (p_path != DEF_NULL) {
            p_cur_char -= path_component_len;
            ASSERT_BREAK(p_path <= p_cur_char, RTOS_ERR_WOULD_OVF);
            if (p_path != p_cur_char) {
              Mem_Move(p_cur_char, p_path, path_component_len);
            }

            p_cur_char -= 1u;
            *p_cur_char = FS_CHAR_PATH_SEP;
          }

          if (first_iter && (entry_type == FS_ENTRY_TYPE_FILE)) {
            entry_pos_data = ((FS_FILE_NODE *)p_entry_node)->ParentPosData;
          } else {
            entry_loc_data.VolHandle.VolPtr = p_vol;
            entry_loc_data.VolHandle.VolId = p_vol->Id;
            entry_loc_data.RelPathPtr = "..";
            entry_loc_data.StartPos = entry_pos_data;
            entry_loc_data.StartDepth = 0u;                     // Don't care.
            BREAK_ON_ERR((void)FSEntry_Lookup(&entry_loc_data,
                                              DEF_NULL,
                                              &entry_pos_data,
                                              DEF_NULL,
                                              p_err));

            ASSERT_BREAK(entry_pos_data != p_vol->SysPtr->VoidEntryPos,
                         RTOS_ERR_VOL_CORRUPTED);
          }

          first_iter = DEF_NO;
        }

        vol_name_len = Str_Len_N(p_vol->Name, FS_CORE_CFG_MAX_VOL_NAME_LEN);
        path_len += vol_name_len + 1u;                          // +1 to account for leading path sep.

        if (p_path != DEF_NULL) {
          ASSERT_BREAK((CPU_SIZE_T)(p_cur_char - p_path) >= vol_name_len + 1u, RTOS_ERR_WOULD_OVF);
          p_path[0] = '/';
          Mem_Copy(&p_path[1], p_vol->Name, vol_name_len);
          Str_Copy_N(&p_path[vol_name_len + 1u], p_cur_char, path_buf_len);
        }
      }
    }
  } WITH_SCOPE_END

  return (path_len);
}

/****************************************************************************************************//**
 *                                           FSEntry_NodeOpen()
 *
 * @brief    Open a file or directory node
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @param    p_path          Relative path to the target entry, starting from the given working
 *                           directory.
 *
 * @param    mode            Entry access mode.
 *
 * @param    entry_type      Entry type:
 *                               - FS_ENTRY_TYPE_FILE
 *                               - FS_ENTRY_TYPE_DIRECTORY
 *
 * @param    p_err           Error pointer.
 *
 * @return   A handle to the opened entry.
 *
 * @note     (1) The opening process is as follows:
 *           - (a) Perform on-disk lookup to find the location of the target entry (if any) and
 *                 target entry's parent. The parent must exist in all scenarios, otherwise an
 *                 error is returned.
 *           - (b) Create the entry if it does not already exist and the FS_ENTRY_ACCESS_MODE_CREATE
 *                 flag as been set.
 *           - (c) If the entry already exists, check if the on-disk attributes are compatible
 *                 with the specified flag parameter (i.e. read-only access mode vs on-disk attribute).
 *                 Also check if the found entry's type is compatible with the type parameter.
 *           - (d) If the entry is not already opened, create initialize and attach to the file system
 *                 tree a new entry node.
 *******************************************************************************************************/
FS_ENTRY_NODE_HANDLE FSEntry_NodeOpen(FS_WRK_DIR_HANDLE wrk_dir_handle,
                                      CPU_CHAR          *p_path,
                                      FS_FLAGS          mode,
                                      FS_FLAGS          entry_type,
                                      RTOS_ERR          *p_err)
{
  FS_VOL               *p_vol;
  FS_ENTRY_NODE        *p_node = DEF_NULL;
  FS_FILE_NODE         *p_file_node;
  FS_SYS_API           *p_sys_api;
  FS_ENTRY_INFO        entry_info;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  CPU_BOOLEAN          entry_exist;
  CPU_BOOLEAN          is_open;
  FS_SYS_POS           entry_pos_data;
  FS_SYS_POS           parent_pos_data;
  FS_ENTRY_LOC_DATA    entry_loc_data;
  CPU_INT16S           rel_depth;
  CPU_INT32U           entry_size;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FSEntry_VirtualPathResolve(wrk_dir_handle,
                                            p_path,
                                            &entry_loc_data,
                                            p_err));

    if (FS_VOL_HANDLE_IS_NULL(entry_loc_data.VolHandle)) {
      ASSERT_BREAK(entry_type == FS_ENTRY_TYPE_DIR, RTOS_ERR_INVALID_ARG);
      entry_node_handle = FSEntry_NodeNullHandle;
    } else {
      BREAK_ON_ERR(FS_VOL_WITH) (entry_loc_data.VolHandle, p_err) {
        p_vol = entry_loc_data.VolHandle.VolPtr;
        p_sys_api = p_vol->SysPtr;

        BREAK_ON_ERR(FS_OP_LOCK_WITH) (&p_vol->OpLock, FS_VOL_OP_ENTRY_OPEN) {
          //                                                       ------------------ ON-DISK LOOKUP ------------------
          //                                                       See note #1a.
          BREAK_ON_ERR((void)FSEntry_Lookup(&entry_loc_data,
                                            &parent_pos_data,
                                            &entry_pos_data,
                                            &rel_depth,
                                            p_err));

          ASSERT_BREAK(parent_pos_data != p_vol->SysPtr->VoidEntryPos,
                       RTOS_ERR_NOT_FOUND);

          ASSERT_BREAK(entry_loc_data.StartDepth + rel_depth <= 255,
                       RTOS_ERR_WOULD_OVF);

          entry_exist = (entry_pos_data != p_vol->SysPtr->VoidEntryPos);

          ASSERT_BREAK_LOG(entry_exist || DEF_BIT_IS_SET(mode, FS_ENTRY_ACCESS_MODE_CREATE),
                           RTOS_ERR_NOT_FOUND,
                           ((s)entry_type == FS_ENTRY_TYPE_FILE ? "File" : "Directory", " not found."));

          ASSERT_BREAK_LOG(!entry_exist || (DEF_BIT_IS_CLR(mode, FS_ENTRY_ACCESS_MODE_EXCL)),
                           RTOS_ERR_ALREADY_EXISTS,
                           ((s)entry_type == FS_ENTRY_TYPE_FILE ? "File" : "Directory", " already exists."));

          //                                                       ------------------- ENTRY CREATE -------------------
          //                                                       See Note #1b.
          if (!entry_exist) {
            ASSERT_BREAK(p_vol->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                         RTOS_ERR_VOL_RD_ONLY);
            entry_size = 0u;
            is_open = DEF_NO;
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
            CPU_CHAR *p_entry_name = FSCore_PathLastSegGet(p_path);
            BREAK_ON_ERR(entry_pos_data = p_sys_api->EntryCreate(p_vol,
                                                                 parent_pos_data,
                                                                 p_entry_name,
                                                                 entry_type == FS_ENTRY_TYPE_DIR,
                                                                 p_err));
#else
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, FSEntry_NullHandle);
#endif
          } else {
            //                                                     ------------------ EXISTING ENTRY ------------------
            //                                                     See Note #1c.
            BREAK_ON_ERR(p_sys_api->EntryQuery(p_vol,
                                               entry_pos_data,
                                               &entry_info,
                                               DEF_NULL,
                                               DEF_NULL,
                                               0u,
                                               p_err));

            if (((entry_info.Attrib.IsDir == DEF_NO)  && (entry_type == FS_ENTRY_TYPE_DIR))
                || ((entry_info.Attrib.IsDir == DEF_YES) && (entry_type == FS_ENTRY_TYPE_FILE))) {
              BREAK_ERR_SET_LOG(RTOS_ERR_INVALID_TYPE, ("Entry already exists with different type."));
            }

            if (entry_type == FS_ENTRY_TYPE_FILE) {
              ASSERT_BREAK_LOG(entry_info.Attrib.Wr || DEF_BIT_IS_CLR(mode, FS_FILE_ACCESS_MODE_WR),
                               RTOS_ERR_FILE_ACCESS_MODE_INVALID, ("File already exists with read-only attribute."));
            }

            entry_size = entry_info.Size;

            //                                                     ----------- CHK IF ENTRY ALREADY OPENED ------------
            //                                                     See Note #1d.
            entry_node_handle = FSEntry_OpenFind(p_vol, entry_pos_data);
            if (FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle)) {
              is_open = DEF_NO;
              p_node = DEF_NULL;
            } else {
              FS_OBJ_HANDLE obj_handle;
              obj_handle.ObjPtr = (FS_OBJ *)entry_node_handle.EntryNodePtr;
              obj_handle.ObjId = entry_node_handle.EntryNodeId;
              is_open = FSObj_RefAcquire(obj_handle);
              p_node = entry_node_handle.EntryNodePtr;
            }
          }

          ON_BREAK {
            if (!entry_exist) {
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
              RTOS_ERR err_tmp = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
              p_sys_api->EntryDel(p_vol, entry_pos_data, &err_tmp);
#else
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, FSEntry_NullHandle);
#endif
            }
            break;
          } WITH {
            //                                                     ---------------- CREATE ENTRY NODE -----------------
            if (!is_open) {                                     // See Note #1d.
              if (entry_type == FS_ENTRY_TYPE_FILE) {
                BREAK_ON_ERR(p_node = (FS_ENTRY_NODE *)p_sys_api->FileNodeAlloc(p_err));
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
              } else {
                BREAK_ON_ERR(p_node = (FS_ENTRY_NODE *)p_sys_api->DirNodeAlloc(p_err));
#endif
              }
              ON_BREAK {
                if (entry_type == FS_ENTRY_TYPE_FILE) {
                  p_sys_api->FileNodeFree((FS_FILE_NODE *)p_node);
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
                } else {
                  p_sys_api->DirNodeFree((FS_DIR_NODE *)p_node);
#endif
                }
                break;
              } WITH {
                FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR) {
                  SList_Push(&FSEntry_OpenListHeadPtr, &p_node->OpenListMember);
                }

                p_node->Id = FSObj_CurId++;
                p_node->RefCnt = 1u;
                p_node->VolHandle.VolPtr = p_vol;
                p_node->VolHandle.VolId = p_vol->Id;
                p_node->Pos = entry_pos_data;
                p_node->SysPtr = p_vol->SysPtr;
                p_node->IsClosing = DEF_NO;

                if (entry_type == FS_ENTRY_TYPE_FILE) {
                  p_file_node = (FS_FILE_NODE *)p_node;
                  p_file_node->ParentPosData = parent_pos_data;
                  p_file_node->Size = entry_size;
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
                  p_file_node->BufData.WrSeqNo = 0u;
#endif
                  BREAK_ON_ERR(p_sys_api->FileNodeInit(p_file_node,
                                                       entry_pos_data,
                                                       p_err));
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
                } else {
                  FS_DIR_NODE *p_dir_node;

                  p_dir_node = (FS_DIR_NODE *)p_node;
                  BREAK_ON_ERR(p_sys_api->DirNodeInit(p_dir_node,
                                                      entry_pos_data,
                                                      p_err));
#endif
                }
              }
            }
          }

          entry_node_handle.EntryNodePtr = p_node;
          entry_node_handle.EntryNodeId = p_node->Id;
        }
      }
    }
  } WITH_SCOPE_END

  return (entry_node_handle);
}

/****************************************************************************************************//**
 *                                           FSEntry_NodeClose()
 *
 * @brief    Close an entry node.
 *
 * @param    entry_node_handle   Handle to entry node.
 *
 * @param    on_null_ref_cnt     Pointer to callback called when entry reference count has reached
 *                               zero.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) The first release accounts for the reference acquired upon entry opening (see FSEntry_Open())
 *               while the second release accounts for the reference that has just been acquired.
 *******************************************************************************************************/
void FSEntry_NodeClose(FS_ENTRY_NODE_HANDLE      entry_node_handle,
                       FS_OBJ_ON_NULL_REF_CNT_CB on_null_ref_cnt,
                       RTOS_ERR                  *p_err)
{
  FS_ENTRY_NODE *p_entry_node;
  FS_OBJ_HANDLE obj_handle;
  CPU_BOOLEAN   acquired;
  CORE_DECLARE_IRQ_STATE;

  WITH_SCOPE_BEGIN(p_err) {
    obj_handle.ObjPtr = (FS_OBJ *)entry_node_handle.EntryNodePtr;
    obj_handle.ObjId = entry_node_handle.EntryNodeId;

    acquired = FSObj_RefAcquire(obj_handle);
    ASSERT_BREAK(acquired, RTOS_ERR_ENTRY_CLOSED);
    ON_EXIT {
      FSObj_RefRelease((FS_OBJ *)p_entry_node, on_null_ref_cnt);
    } WITH {
      p_entry_node = entry_node_handle.EntryNodePtr;
      CORE_ENTER_ATOMIC();
      ON_EXIT {
        CORE_EXIT_ATOMIC();
      } WITH {
        ASSERT_BREAK(!p_entry_node->IsClosing, RTOS_ERR_ENTRY_CLOSED);
        p_entry_node->IsClosing = DEF_YES;
      }
    }

    FSObj_RefRelease((FS_OBJ *)entry_node_handle.EntryNodePtr, on_null_ref_cnt);
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FSEntry_VirtualPathResolve()
 *
 * @brief    Resolve given relative path to determine some useful information.
 *
 * @param    wrk_dir_handle      Handle to a working directory.
 *
 * @param    p_path              Entry path relative to the given working directory.
 *
 * @param    p_entry_loc_data    Pointer to entry location determined from resolved path.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
void FSEntry_VirtualPathResolve(FS_WRK_DIR_HANDLE wrk_dir_handle,
                                CPU_CHAR          *p_path,
                                FS_ENTRY_LOC_DATA *p_entry_loc_data,
                                RTOS_ERR          *p_err)
{
  CPU_CHAR      *p_cur_path_seg;
  CPU_CHAR      *p_resolved_path;
  FS_VOL_HANDLE resolved_vol_handle;
  CPU_BOOLEAN   is_dot_dot;
  CPU_BOOLEAN   has_vol_chng;
  CPU_INT16U    cur_depth;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             Get current wrk dir depth.
    cur_depth = FS_WRK_DIR_HANDLE_IS_NULL(wrk_dir_handle) ? 0u
                : wrk_dir_handle.WrkDirPtr->Depth;

    //                                                             Remove initial reducible path components.
    p_cur_path_seg = FSCore_PathReduce(p_path);

    has_vol_chng = DEF_NO;

    if (cur_depth == 0u) {
      resolved_vol_handle = FSVol_NullHandle;
    } else {
      resolved_vol_handle = wrk_dir_handle.WrkDirPtr->VolHandle;
    }

    p_resolved_path = DEF_NULL;
    BREAK_ON_ERR(while) (p_cur_path_seg != DEF_NULL) {
      RTOS_ASSERT_CRITICAL((!FSCore_PathSegIsDot(p_cur_path_seg)), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      is_dot_dot = FSCore_PathSegIsDotDot(p_cur_path_seg);
      if (is_dot_dot) {
        if (cur_depth > 0u) {
          cur_depth--;
        }
        if (cur_depth == 0u) {
          resolved_vol_handle = FSVol_NullHandle;
        }
      } else {
        if (cur_depth == 0u) {
          BREAK_ON_ERR(FS_OP_LOCK_WITH) (&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_RD) {
            resolved_vol_handle = FSVol_GetLocked(p_cur_path_seg);
            ASSERT_BREAK(!FS_VOL_HANDLE_IS_NULL(resolved_vol_handle), RTOS_ERR_NOT_FOUND);
          }
        }
        cur_depth++;
      }

      p_cur_path_seg = FS_PathNextSegGet(p_cur_path_seg);
      p_cur_path_seg = FSCore_PathReduce(p_cur_path_seg);

      if ((is_dot_dot && (cur_depth == 0u))
          || (!is_dot_dot && (cur_depth == 1u))) {
        has_vol_chng = DEF_YES;
        p_resolved_path = p_cur_path_seg;
      }
    }

    ASSERT_BREAK(cur_depth <= 255u, RTOS_ERR_ENTRY_MAX_DEPTH_EXCEEDED);

    RTOS_ASSERT_CRITICAL((!FS_VOL_HANDLE_IS_NULL(resolved_vol_handle)
                          || (p_resolved_path == DEF_NULL)), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    if (has_vol_chng) {
      p_entry_loc_data->VolHandle = resolved_vol_handle;
      p_entry_loc_data->RelPathPtr = p_resolved_path;
      if (FS_VOL_HANDLE_IS_NULL(resolved_vol_handle)) {
        p_entry_loc_data->StartPos = 0u;                        // Don't care.
        p_entry_loc_data->StartDepth = 0u;
      } else {
        p_entry_loc_data->StartPos = resolved_vol_handle.VolPtr->SysPtr->RootDirPosData;
        p_entry_loc_data->StartDepth = 1u;
      }
    } else {
      p_entry_loc_data->RelPathPtr = FSCore_PathReduce(p_path);
      if (FS_WRK_DIR_HANDLE_IS_NULL(wrk_dir_handle)) {
        p_entry_loc_data->VolHandle = FSVol_NullHandle;
        p_entry_loc_data->StartPos = 0u;                        // Don't care.
        p_entry_loc_data->StartDepth = 0u;
      } else {
        p_entry_loc_data->VolHandle = wrk_dir_handle.WrkDirPtr->VolHandle;

        p_entry_loc_data->StartPos = wrk_dir_handle.WrkDirPtr->Pos;
        p_entry_loc_data->StartDepth = wrk_dir_handle.WrkDirPtr->Depth;
      }
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSEntry_Lookup()
 *
 * @brief    Look for an entry.
 *
 * @param    p_entry_loc_data    Pointer to entry location information.
 *
 * @param    p_parent_pos_data   Pointer to parent entry location.
 *
 * @param    p_entry_pos_data    Pointer to found encoded entry position.
 *
 * @param    p_rel_depth         Pointer to found entry path.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Pointer to entry path, if NO error(s).
 *           NULL pointer,          otherwise.
 *
 * @note     (1) Possible outcome:
 *               - (a) Entry found: returned vol handle set to containing vol handle, returned entry
 *                     position is set to found entry position, returned parent position is set to
 *                     found entry's parent position if found entry is not the volume root. Otherwise the
 *                     returned parent position should be discarded. Returned path is set to null.
 *               - (b) FS Root found: returned vol_handle is set to null. Returned path is set to null.
 *                     Returned entry and parent position should be discarded.
 *               - (c) FS Root is encountered: returned vol handle is set to null. Returned path is set
 *                     to the remaining portion of the path. Returned entry and parent position should be
 *                     discarded.
 *               - (c) Entry not found: the returned vol handle is set to the containing volume. Entry
 *                     position is set to void. Parent position is set to the found parent if any.
 *                     Otherwise it is set to void. The returned path is set to the portion of the path
 *                     that remains.
 *******************************************************************************************************/
CPU_CHAR *FSEntry_Lookup(FS_ENTRY_LOC_DATA *p_entry_loc_data,
                         FS_SYS_POS        *p_parent_pos_data,
                         FS_SYS_POS        *p_entry_pos_data,
                         CPU_INT16S        *p_rel_depth,
                         RTOS_ERR          *p_err)
{
  FS_VOL     *p_vol;
  CPU_CHAR   *p_cur_component;
  CPU_CHAR   *p_path_not_found;
  CPU_CHAR   *p_next_component;
  CPU_CHAR   *p_prev_component;
  CPU_CHAR   *p_rel_path;
  FS_SYS_POS prev_pos_data;
  FS_SYS_POS cur_pos_data;
  CPU_INT16S cur_rel_depth;

  WITH_SCOPE_BEGIN(p_err) {
    p_vol = p_entry_loc_data->VolHandle.VolPtr;
    p_rel_path = p_entry_loc_data->RelPathPtr;
    cur_pos_data = p_entry_loc_data->StartPos;
    prev_pos_data = p_vol->SysPtr->VoidEntryPos;

    p_cur_component = FSCore_PathReduce(p_rel_path);
    p_prev_component = DEF_NULL;
    p_path_not_found = DEF_NULL;

    cur_rel_depth = 0u;
    //                                                             ------------------- BROWSE PATH --------------------
    //                                                             Browse until path component not found or path end.
    BREAK_ON_ERR(while) ((p_cur_component != DEF_NULL)
                         && (cur_pos_data != p_vol->SysPtr->VoidEntryPos)) {
      RTOS_ASSERT_CRITICAL((!FSCore_PathSegIsDot(p_cur_component)), RTOS_ERR_ASSERT_CRITICAL_FAIL, p_cur_component);
      if (FSCore_PathSegIsDotDot(p_cur_component)) {
        cur_rel_depth -= 1u;
      } else {
        cur_rel_depth += 1u;
      }

      prev_pos_data = cur_pos_data;
      BREAK_ON_ERR(cur_pos_data = p_vol->SysPtr->EntryLookup(p_vol,
                                                             cur_pos_data,
                                                             p_cur_component,
                                                             p_err));

      if (cur_pos_data != p_vol->SysPtr->VoidEntryPos) {
        p_cur_component = FS_PathNextSegGet(p_cur_component);
        p_cur_component = FSCore_PathReduce(p_cur_component);
      }
    }

    //                                                             ------------------- SET RTN VAL --------------------
    //                                                             Set parent and entry pos based on lookup outcome.
    if (p_rel_depth != DEF_NULL) {
      *p_rel_depth = cur_rel_depth;
    }

    *p_entry_pos_data = cur_pos_data;

    if (cur_pos_data == p_vol->SysPtr->VoidEntryPos) {
      if (p_parent_pos_data != DEF_NULL) {
        p_next_component = FS_PathNextSegGet(p_cur_component);
        p_next_component = FSCore_PathReduce(p_next_component);
        if (p_next_component == DEF_NULL) {
          *p_parent_pos_data = prev_pos_data;
        } else {
          *p_parent_pos_data = p_vol->SysPtr->VoidEntryPos;
        }
      }
    } else {
      if (p_parent_pos_data != DEF_NULL) {
        if (((p_prev_component != DEF_NULL) && FSCore_PathSegIsDot(p_prev_component))
            || (prev_pos_data == p_vol->SysPtr->VoidEntryPos)) {
          BREAK_ON_ERR(cur_pos_data = p_vol->SysPtr->EntryLookup(p_vol,
                                                                 cur_pos_data,
                                                                 "..",
                                                                 p_err));

          RTOS_ASSERT_CRITICAL((cur_pos_data != p_vol->SysPtr->VoidEntryPos), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
          *p_parent_pos_data = cur_pos_data;
        } else {
          *p_parent_pos_data = prev_pos_data;
        }
      }
    }
  } WITH_SCOPE_END

  return (p_path_not_found);
}

/****************************************************************************************************//**
 *                                           FSEntry_OpenEntryFind()
 *
 * @brief    Find an open entry based on its on-disk position and some volume information.
 *
 * @param    p_vol               Pointer to a volume.
 *
 * @param    p_entry_pos_data    Pointer to the entry's position data.
 *
 * @return   Pointer to the entry node object, if the entry is opened.
 *           Null pointer,                     otherwise.
 *******************************************************************************************************/
FS_ENTRY_NODE_HANDLE FSEntry_OpenFind(FS_VOL     *p_vol,
                                      FS_SYS_POS entry_pos)
{
  FS_ENTRY_NODE        *p_cur_entry_node;
  FS_ENTRY_NODE_HANDLE found_entry_node_handle = FSEntry_NodeNullHandle;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD) {
    SLIST_FOR_EACH_ENTRY(FSEntry_OpenListHeadPtr, p_cur_entry_node, FS_ENTRY_NODE, OpenListMember) {
      if ((p_cur_entry_node->Pos == entry_pos)
          && (FS_VOL_HANDLE_IS_VALID(p_cur_entry_node->VolHandle))
          && (p_cur_entry_node->VolHandle.VolPtr == p_vol)) {
        found_entry_node_handle.EntryNodePtr = p_cur_entry_node;
        found_entry_node_handle.EntryNodeId = p_cur_entry_node->Id;
        break;
      }
    }
  }

  return (found_entry_node_handle);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       FSEntry_DelLockedAcquired()
 *
 * @brief    Delete an entry.
 *
 * @param    p_entry_loc_data    Pointer to entry location information.
 *
 * @param    entry_type          Entry type to be deleted:
 *                                   - FS_ENTRY_TYPE_FILE
 *                                   - FS_ENTRY_TYPE_DIR
 *                                   - FS_ENTRY_TYPE_ANY
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSEntry_DelLockedAcquired(FS_ENTRY_LOC_DATA *p_entry_loc_data,
                                      FS_FLAGS          entry_type,
                                      RTOS_ERR          *p_err)
{
  FS_VOL               *p_vol;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  FS_WRK_DIR_HANDLE    found_wrk_dir_handle;
  FS_ENTRY_INFO        entry_info;
  FS_SYS_POS           entry_pos;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR((void)FSEntry_Lookup(p_entry_loc_data,
                                      DEF_NULL,
                                      &entry_pos,
                                      DEF_NULL,
                                      p_err));

    p_vol = p_entry_loc_data->VolHandle.VolPtr;

    //                                                             Entry must exist.
    ASSERT_BREAK_LOG(entry_pos != p_vol->SysPtr->VoidEntryPos,
                     RTOS_ERR_NOT_FOUND, ("Entry not found"));

    //                                                             Cannot del root dir (see Note #2a).
    ASSERT_BREAK_LOG(entry_pos != p_vol->SysPtr->RootDirPosData,
                     RTOS_ERR_ENTRY_ROOT_DIR, ("Cannot delete root directory."));

    //                                                             ---------------- CHK IF ENTRY OPEN -----------------
    entry_node_handle = FSEntry_OpenFind(p_vol, entry_pos);
    ASSERT_BREAK_LOG(FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle),
                     RTOS_ERR_ENTRY_OPENED, ("Cannot delete opened entry."));

    found_wrk_dir_handle = FSWrkDir_OpenFind(p_vol, entry_pos);
    ASSERT_BREAK_LOG(FS_WRK_DIR_HANDLE_IS_NULL(found_wrk_dir_handle),
                     RTOS_ERR_ENTRY_OPENED, ("Cannot delete opened entry."));

    BREAK_ON_ERR(p_vol->SysPtr->EntryQuery(p_vol,
                                           entry_pos,
                                           &entry_info,
                                           DEF_NULL,
                                           DEF_NULL,
                                           0u,
                                           p_err));

    //                                                             Chk entry type against allowed type.
    ASSERT_BREAK_LOG(!entry_info.Attrib.IsDir || DEF_BIT_IS_SET(entry_type, FS_ENTRY_TYPE_DIR),
                     RTOS_ERR_INVALID_TYPE, ("Entry found but type is invalid."));

    ASSERT_BREAK_LOG(entry_info.Attrib.IsDir || DEF_BIT_IS_SET(entry_type, FS_ENTRY_TYPE_FILE),
                     RTOS_ERR_INVALID_TYPE, ("Entry found but type is invalid."));

    if (entry_info.Attrib.IsDir) {                              // Chk if dir empty (see Note #2b).
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
      CPU_BOOLEAN dir_empty;
      BREAK_ON_ERR(dir_empty = p_vol->SysPtr->DirChkEmpty(p_vol,
                                                          &entry_pos,
                                                          p_err));

      ASSERT_BREAK_LOG(dir_empty, RTOS_ERR_DIR_NOT_EMPTY,
                       ("Cannot delete non-empty directory."));
#else
      BREAK_ERR_SET(RTOS_ERR_NOT_AVAIL);
#endif
    }

    //                                                             Delete entry.
    BREAK_ON_ERR(p_vol->SysPtr->EntryDel(p_vol,
                                         entry_pos,
                                         p_err));
  } WITH_SCOPE_END
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
