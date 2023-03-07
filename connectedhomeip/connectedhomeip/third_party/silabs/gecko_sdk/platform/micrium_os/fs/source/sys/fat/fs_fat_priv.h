/***************************************************************************//**
 * @file
 * @brief File System - Fat Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_FAT_PRIV_H_
#define  FS_FAT_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- CORE -----------------------
#include  <fs_core_cfg.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <common/include/rtos_err.h>
#include  <fs/source/core/fs_core_cache_priv.h>

//                                                                 ----------------------- FAT ------------------------
#include  <fs/source/sys/fat/fs_fat_vol_priv.h>
#include  <fs/source/sys/fat/fs_fat_type_priv.h>
#include  <fs/include/fs_fat.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>

#include  "sl_sleeptimer.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_FAT_LB_TYPE_DIRENT        100u
#define  FS_FAT_LB_TYPE_FAT           101u
#define  FS_FAT_LB_TYPE_DATA          102u
#define  FS_FAT_LB_TYPE_JNL_DIRENT    103u
#define  FS_FAT_LB_TYPE_JNL_DATA      104u
#define  FS_FAT_LB_TYPE_RSVD_AREA     105u

#define  FS_FAT_MIN_CLUS_NBR          2u

#define  FS_FAT_VOID_DATA_LB_NBR      0u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_fat_data {
  CPU_BOOLEAN IsInit;
} FS_FAT_DATA;

typedef struct fs_fat_clus_chain_ends {
  FS_FAT_CLUS_NBR FirstClusNbr;
  FS_FAT_CLUS_NBR LastClusNbr;
} FS_FAT_CLUS_CHAIN_ENDS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const FS_SYS_API          FS_FAT_API;
extern const FS_FAT_SEC_BYTE_POS FS_FAT_VoidSecBytePos;
extern const FS_FAT_SEC_BYTE_POS FS_FAT_NullSecBytePos;
extern const FS_SYS_POS          FS_FAT_VoidEntrySysPos;

extern FS_FAT_DATA FS_FAT_Data;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_FAT_SEC_BYTE_POS_IS_VOID(sec_byte_pos)     (((sec_byte_pos).SecNbr == (FS_FAT_SEC_NBR)-1) \
                                                        && ((sec_byte_pos).SecOffset == (FS_FAT_SEC_SIZE)-1))

#define  FS_FAT_SEC_BYTE_POS_ARE_EQUAL(pos_a, pos_b)   (((pos_a).SecNbr == (pos_b).SecNbr) \
                                                        && ((pos_a).SecOffset == (pos_b).SecOffset))

#define  FS_FAT_SEC_TO_CLUS(p_fat_vol, sec_nbr)       ((FS_FAT_CLUS_NBR)FS_UTIL_DIV_PWR2(((sec_nbr) - (p_fat_vol)->DataAreaStart), (p_fat_vol)->SecPerClusLog2) + 2u)

#define  FS_FAT_CLUS_TO_SEC(p_fat_vol, clus_nbr)      ((p_fat_vol)->DataAreaStart + (FS_FAT_SEC_NBR)FS_UTIL_MULT_PWR2(((clus_nbr) - FS_FAT_MIN_CLUS_NBR), (p_fat_vol)->SecPerClusLog2))

#define  FS_FAT_CLUS_SEC_REM(p_fat_vol, sec_nbr)      ((FS_FAT_SEC_NBR)FS_UTIL_PWR2((p_fat_vol)->SecPerClusLog2) - (((sec_nbr) - (p_fat_vol)->DataAreaStart) & ((FS_FAT_SEC_NBR)FS_UTIL_PWR2((p_fat_vol)->SecPerClusLog2) - 1u)))

#define  FS_FAT_IS_VALID_CLUS(p_fat_vol, clus_nbr)    (((clus_nbr) < (p_fat_vol)->ClusCnt + FS_FAT_MIN_CLUS_NBR) \
                                                       && ((clus_nbr) >= FS_FAT_MIN_CLUS_NBR))

#define  FS_FAT_IS_VALID_SEC(p_fat_vol, sec_nbr)     (((sec_nbr) >= (p_fat_vol)->RootDirStart) \
                                                      && ((sec_nbr) <= (p_fat_vol)->RootDirStart + (p_fat_vol)->RootDirSize + (p_fat_vol)->DataAreaSize))

/********************************************************************************************************
 *                                       FS_FAT_IS_LEGAL_xxx_CHAR()
 *
 * Description : Determine whether character is ...
 *
 *               (a) ... legal Short File Name character ('FS_FAT_IS_LEGAL_SFN_CHAR()') (see Note #1).
 *
 *               (b) ... legal Long  File Name character ('FS_FAT_IS_LEGAL_LFN_CHAR()') (see Note #2).
 *
 * Argument(s) : c           Character.
 *
 * Return(s)   : DEF_YES, if character is   legal.
 *
 *               DEF_NO,  if character is illegal.
 *
 * Note(s) : (1) (a) According to Microsoft's "FAT: General Overview of On-DiSk Format", the legal characters
 *                   for a short name are :
 *
 *                   (1) Letters, digits & characters with code values greater than 127.
 *                   (2) The special characters
 *
 *                       !  #  $  %  &  '  (  )  -  @  ^  _  `  {  }  ~
 *
 *                   Taken together, this means that the set of legal character codes is the union of the
 *                   following character code ranges :
 *
 *                   (1) [0x80, 0xFF]    characters with code values above 127
 *                   (2) [0x7D, 0x7E]    }  ~
 *                   (3) [0x5E, 0x7B]    ^  _  `  lower-case letters  {
 *                   (4) [0x40, 0x5A]    @  upper-case letters
 *                   (5) [0x30, 0x39]    digits
 *                   (6) [0x2D, 0x2D]    -
 *                   (7) [0x23, 0x29]    #  $  %  &  '  (  )
 *                   (8) [0x21, 0x21]    !
 *
 *               (b) The legal characters for a long name are :
 *
 *                   (1) The legal SFN characters.
 *                   (2) Periods.
 *                   (3) Spaces.
 *                   (4) The special characters
 *
 *                       +  ,  ;  =  [  ]
 *
 *                   Taken together, this means that the set of legal character codes is the union of the
 *                   following character code ranges :
 *
 *                   (1) [0x80, 0xFF]    characters with code values above 127
 *                   (2) [0x7D, 0x7E]    }  ~
 *                   (3) [0x5D, 0x7B]    ]  ^  _  `  lower-case letters  {
 *                   (4) [0x40, 0x5B]    @  upper-case letters  [
 *                   (5) [0x3D, 0x3D]    =
 *                   (6) [0x3B, 0x3B]    ;
 *                   (7) [0x30, 0x39]    digits
 *                   (8) [0x2B, 0x2E]    +  ,  -  .
 *                   (9) [0x23, 0x29]    #  $  %  &  '  (  )
 *                   (10) [0x20, 0x21]    space  !
 *******************************************************************************************************/

#define  FS_FAT_IS_LEGAL_SFN_CHAR(c)           (((((c) >= (CPU_CHAR)ASCII_CHAR_CIRCUMFLEX_ACCENT)       \
                                                  && ((c) != (CPU_CHAR)ASCII_CHAR_VERTICAL_LINE)        \
                                                  && ((c) != (CPU_CHAR)ASCII_CHAR_DELETE))              \
                                                 || (((c) >= (CPU_CHAR)ASCII_CHAR_COMMERCIAL_AT)        \
                                                     && ((c) <= (CPU_CHAR)ASCII_CHAR_LATIN_UPPER_Z))    \
                                                 || (((c) >= (CPU_CHAR)ASCII_CHAR_DIGIT_ZERO)           \
                                                     && ((c) <= (CPU_CHAR)ASCII_CHAR_DIGIT_NINE))       \
                                                 || (((c) == (CPU_CHAR)ASCII_CHAR_HYPHEN_MINUS))        \
                                                 || (((c) >= (CPU_CHAR)ASCII_CHAR_EXCLAMATION_MARK)     \
                                                     && ((c) <= (CPU_CHAR)ASCII_CHAR_RIGHT_PARENTHESIS) \
                                                     && ((c) != (CPU_CHAR)ASCII_CHAR_QUOTATION_MARK))) ? (DEF_YES) : (DEF_NO))

#define  FS_FAT_IS_LEGAL_LFN_CHAR(c)           (((((c) >= (CPU_CHAR)ASCII_CHAR_COMMERCIAL_AT)        \
                                                  && ((c) != (CPU_CHAR)ASCII_CHAR_REVERSE_SOLIDUS)   \
                                                  && ((c) != (CPU_CHAR)ASCII_CHAR_VERTICAL_LINE)     \
                                                  && ((c) != (CPU_CHAR)ASCII_CHAR_DELETE))           \
                                                 || ((c) == (CPU_CHAR)ASCII_CHAR_EQUALS_SIGN)        \
                                                 || ((c) == (CPU_CHAR)ASCII_CHAR_SEMICOLON)          \
                                                 || (((c) >= (CPU_CHAR)ASCII_CHAR_DIGIT_ZERO)        \
                                                     && ((c) <= (CPU_CHAR)ASCII_CHAR_DIGIT_NINE))    \
                                                 || (((c) >= (CPU_CHAR)ASCII_CHAR_SPACE)             \
                                                     && ((c) <= (CPU_CHAR)ASCII_CHAR_FULL_STOP)      \
                                                     && ((c) != (CPU_CHAR)ASCII_CHAR_QUOTATION_MARK) \
                                                     && ((c) != (CPU_CHAR)ASCII_CHAR_ASTERISK)         ))  ? (DEF_YES) : (DEF_NO))

#define  FS_FAT_IS_LEGAL_VOL_LABEL_CHAR(c)     ((((c) != ASCII_CHAR_QUOTATION_MARK)          \
                                                 && ((c) != ASCII_CHAR_AMPERSAND)            \
                                                 && ((c) != ASCII_CHAR_ASTERISK)             \
                                                 && ((c) != ASCII_CHAR_PLUS_SIGN)            \
                                                 && ((c) != ASCII_CHAR_HYPHEN_MINUS)         \
                                                 && ((c) != ASCII_CHAR_COMMA)                \
                                                 && ((c) != ASCII_CHAR_FULL_STOP)            \
                                                 && ((c) != ASCII_CHAR_SOLIDUS)              \
                                                 && ((c) != ASCII_CHAR_COLON)                \
                                                 && ((c) != ASCII_CHAR_SEMICOLON)            \
                                                 && ((c) != ASCII_CHAR_LESS_THAN_SIGN)       \
                                                 && ((c) != ASCII_CHAR_EQUALS_SIGN)          \
                                                 && ((c) != ASCII_CHAR_GREATER_THAN_SIGN)    \
                                                 && ((c) != ASCII_CHAR_QUESTION_MARK)        \
                                                 && ((c) != ASCII_CHAR_LEFT_SQUARE_BRACKET)  \
                                                 && ((c) != ASCII_CHAR_RIGHT_SQUARE_BRACKET) \
                                                 && ((c) != ASCII_CHAR_REVERSE_SOLIDUS))  ? (DEF_YES) : (DEF_NO))

/********************************************************************************************************
 ********************************************************************************************************
 *                                       INTERNAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FS_FAT_Init(RTOS_ERR *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
FS_FAT_DATE FS_FAT_DateFmt(sl_sleeptimer_date_t *date_time);

FS_FAT_TIME FS_FAT_TimeFmt(sl_sleeptimer_date_t *date_time);
#endif

void FS_FAT_DateTimeParse(sl_sleeptimer_timestamp_t  *p_ts,
                          FS_FAT_DATE                 date,
                          FS_FAT_TIME                 time);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)

FS_FAT_CLUS_NBR FS_FAT_ClusChainAlloc(FS_FAT_VOL             *p_vol,
                                      FS_FAT_CLUS_NBR        start_clus,
                                      FS_FAT_CLUS_NBR        *p_last_clus,
                                      FS_FAT_CLUS_NBR        nbr_clus,
                                      CPU_BOOLEAN            clr,
                                      FS_CACHE_WR_JOB_HANDLE *p_wr_job_handle,
                                      CPU_INT08U             fat_lb_type,
                                      RTOS_ERR               *p_err);

FS_FAT_CLUS_NBR FS_FAT_ClusChainDel(FS_FAT_VOL      *p_vol,
                                    FS_FAT_CLUS_NBR start_clus,
                                    CPU_BOOLEAN     del_first,
                                    RTOS_ERR        *p_err);

void FS_FAT_ClusChainReverseDel(FS_FAT_VOL      *p_vol,
                                FS_FAT_CLUS_NBR start_clus,
                                CPU_BOOLEAN     del_first,
                                RTOS_ERR        *p_err);
#endif

FS_FAT_CLUS_NBR FS_FAT_ClusChainFollow(FS_FAT_VOL      *p_fat_vol,
                                       FS_FAT_CLUS_NBR start_clus,
                                       FS_FAT_CLUS_NBR len,
                                       FS_FAT_CLUS_NBR *p_last_clus_tbl,
                                       RTOS_ERR        *p_err);

FS_FAT_CLUS_NBR FS_FAT_ClusChainEndFind(FS_FAT_VOL      *p_fat_vol,
                                        FS_FAT_CLUS_NBR start_clus,
                                        FS_FAT_CLUS_NBR *p_last_clus_tbl,
                                        RTOS_ERR        *p_err);

FS_FAT_CLUS_NBR FS_FAT_ClusChainReverseFollow(FS_FAT_VOL      *p_vol,
                                              FS_FAT_CLUS_NBR start_clus,
                                              FS_FAT_CLUS_NBR stop_clus,
                                              RTOS_ERR        *p_err);

FS_FAT_SEC_NBR FS_FAT_SecNextGet(FS_FAT_VOL     *p_vol,
                                 FS_FAT_SEC_NBR start_sec,
                                 RTOS_ERR       *p_err);

CPU_BOOLEAN FS_FAT_BootSecChk(FS_BLK_DEV_HANDLE blk_dev_handle,
                              FS_PARTITION_INFO *p_partition_info,
                              FS_LB_NBR         mbr_sec,
                              RTOS_ERR          *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_CFG_FAT_EN
#error  "FS_CORE_CFG_FAT_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  (FS_CORE_CFG_FAT_EN != DEF_ENABLED)
#error  "FS_CORE_CFG_FAT_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED."
#endif

#ifndef  FS_FAT_CFG_LFN_EN
#error  "FS_FAT_CFG_LFN_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_FAT_CFG_LFN_EN != DEF_DISABLED) \
  && (FS_FAT_CFG_LFN_EN != DEF_ENABLED))
#error  "FS_FAT_CFG_LFN_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_FAT_CFG_FAT12_EN
#error  "FS_FAT_CFG_FAT12_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_FAT_CFG_FAT12_EN != DEF_DISABLED) \
  && (FS_FAT_CFG_FAT12_EN != DEF_ENABLED))
#error  "FS_FAT_CFG_FAT12_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_FAT_CFG_FAT16_EN
#error  "FS_FAT_CFG_FAT16_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_FAT_CFG_FAT16_EN != DEF_DISABLED) \
  && (FS_FAT_CFG_FAT16_EN != DEF_ENABLED))
#error  "FS_FAT_CFG_FAT16_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_FAT_CFG_FAT32_EN
#error  "FS_FAT_CFG_FAT32_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_FAT_CFG_FAT32_EN != DEF_DISABLED) \
  && (FS_FAT_CFG_FAT32_EN != DEF_ENABLED))
#error  "FS_FAT_CFG_FAT32_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#if    ((FS_FAT_CFG_FAT12_EN != DEF_ENABLED) \
  && (FS_FAT_CFG_FAT16_EN != DEF_ENABLED)    \
  && (FS_FAT_CFG_FAT32_EN != DEF_ENABLED))
#error  "Invalid FAT CONFIG in 'fs_core_cfg.h' At least one of (FS_FAT_CFG_FAT12_EN, FS_FAT_CFG_FAT16_EN, FS_FAT_CFG_FAT32_EN) must be DEF_ENABLED."
#endif

#ifndef  FS_FAT_CFG_JOURNAL_EN
#error  "FS_FAT_CFG_JOURNAL_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."

#elif  ((FS_FAT_CFG_JOURNAL_EN != DEF_DISABLED) \
  && (FS_FAT_CFG_JOURNAL_EN != DEF_ENABLED))
#error  "FS_FAT_CFG_JOURNAL_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#if    ((FS_FAT_CFG_JOURNAL_EN == DEF_ENABLED) \
  && (FS_CORE_CFG_RD_ONLY_EN == DEF_ENABLED))
#error  "Invalid configuration in 'fs_core_cfg.h': journaling is useless when FS_CORE_CFG_RD_ONLY_EN is DEF_ENABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
