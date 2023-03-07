/***************************************************************************//**
 * @file
 * @brief File System - Core Operations
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
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_PRIV_H
#define  FS_CORE_PRIV_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <common/include/rtos_path.h>
#include  <fs_core_cfg.h>
#include  <fs_storage_cfg.h>
#include  <fs/include/fs_core.h>
#include  <fs/include/fs_core_cache.h>
#include  <fs/include/fs_blk_dev.h>
#include  <fs/source/shared/fs_obj_priv.h>
#include  <fs/source/core/fs_core_unicode_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/kal.h>
#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/include/lib_ascii.h>
#include  <common/include/rtos_err.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_LB_TYPE_INVALID                       ((CPU_INT08U)-1)
#define  FS_LB_TYPE_DATA                           0
#define  FS_LB_TYPE_MBR                            1

#define  FS_CHAR_PATH_SEP                          ((CPU_CHAR)ASCII_CHAR_SOLIDUS)
#define  FS_CHAR_PATH_SEP_ALT                      ((CPU_CHAR)ASCII_CHAR_REVERSE_SOLIDUS)

#define  FS_CHAR_IS_PATH_SEP(path_char)            ((path_char == FS_CHAR_PATH_SEP) \
                                                    || (path_char == FS_CHAR_PATH_SEP_ALT))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_core_data {
  CPU_BOOLEAN  IsInit;
  OP_LOCK      OpLock;
  SLIST_MEMBER *CacheBlkDevDataListHeadPtr;
  MEM_DYN_POOL CacheBlkDevDataPool;
} FS_CORE_DATA;

typedef struct fs_cache_blk_dev_data {
  FS_CACHE          *CachePtr;
  FS_BLK_DEV_HANDLE BlkDevHandle;
  SLIST_MEMBER      ListMember;
  CPU_BOOLEAN       IsUnassigning;
} FS_CACHE_BLK_DEV_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_ENABLED)
extern const FS_CORE_INIT_CFG FSCore_InitCfg;
#else
extern FS_CORE_INIT_CFG FSCore_InitCfg;
#endif

extern FS_CORE_DATA FSCore_Data;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_CTR_STAT_EN == DEF_ENABLED)                    // ----------------- STAT CTR MACRO'S -----------------
#define  FS_CTR_STAT_INC(stat_ctr)                           FS_CTR_INC(stat_ctr)
#define  FS_CTR_STAT_ADD(stat_ctr, val)                      FS_CTR_ADD((stat_ctr), (val))
#else
#define  FS_CTR_STAT_INC(stat_ctr)
#define  FS_CTR_STAT_ADD(stat_ctr, val)
#endif

#if (FS_CORE_CFG_CTR_ERR_EN == DEF_ENABLED)                     // ----------------- ERR CTR MACRO'S ------------------
#define  FS_CTR_ERR_INC(err_ctr)                              FS_CTR_INC(err_ctr)
#else
#define  FS_CTR_ERR_INC(err_ctr)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

FS_FILE_NAME_LEN FSCore_PathSegLenGet(CPU_CHAR *p_path_component);

CPU_BOOLEAN FSCore_PathSegIsDot(CPU_CHAR *name);

CPU_BOOLEAN FSCore_PathSegIsDotDot(CPU_CHAR *name);

CPU_CHAR *FSCore_PathReduce(CPU_CHAR *p_path);

CPU_CHAR *FS_PathNextSegGet(CPU_CHAR *p_cur_component);

CPU_CHAR *FSCore_PathLastSegGet(CPU_CHAR *p_cur_component);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_CFG_CTR_STAT_EN
#error  "FS_CORE_CFG_CTR_STAT_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_CTR_STAT_EN != DEF_DISABLED) \
  && (FS_CORE_CFG_CTR_STAT_EN != DEF_ENABLED))
#error  "FS_CORE_CFG_CTR_STAT_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_CORE_CFG_CTR_ERR_EN
#error  "FS_CORE_CFG_CTR_ERR_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_CTR_ERR_EN != DEF_DISABLED) \
  && (FS_CORE_CFG_CTR_ERR_EN != DEF_ENABLED))
#error  "FS_CORE_CFG_CTR_ERR_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_CORE_CFG_DBG_MEM_CLR_EN
#error  "FS_CORE_CFG_DBG_MEM_CLR_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_DBG_MEM_CLR_EN != DEF_DISABLED) \
  && (FS_CORE_CFG_DBG_MEM_CLR_EN != DEF_ENABLED))
#error  "FS_CORE_CFG_DBG_MEM_CLR_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_CORE_CFG_UTF8_EN
#error  "FS_CORE_CFG_UTF8_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_UTF8_EN != DEF_ENABLED) \
  && (FS_CORE_CFG_UTF8_EN != DEF_DISABLED))
#error  "FS_CORE_CFG_UTF8_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_CORE_CFG_RD_ONLY_EN
#error  "FS_CORE_CFG_RD_ONLY_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_RD_ONLY_EN != DEF_ENABLED) \
  && (FS_CORE_CFG_RD_ONLY_EN != DEF_DISABLED))
#error  "FS_CORE_CFG_RD_ONLY_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#if ((FS_CORE_CFG_RD_ONLY_EN) == (!FS_STORAGE_CFG_RD_ONLY_EN))
#error "FS_STORAGE_CFG_RD_ONLY_EN must be set to DEF_DISABLED if FS_CORE_CFG_RD_ONLY_EN is DEF_DISABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
