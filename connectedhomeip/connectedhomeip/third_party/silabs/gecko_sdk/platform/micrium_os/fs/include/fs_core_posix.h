/***************************************************************************//**
 * @file
 * @brief File System - Core Posix Operations
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

/****************************************************************************************************//**
 * @defgroup FS_POSIX File System POSIX API
 * @ingroup  FS
 * @brief      File System POSIX API
 *
 * @addtogroup FS_POSIX
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_POSIX_H_
#define  FS_CORE_POSIX_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 *
 * Note(s) : (1) When using ARM GCC compiler, the function clearerr(), feof() and ferror() provokes a
 *               warning indicating that these functions are already defined in stdio.h provided by ARM
 *               GCC compiler. Defining _REENT_SMALL allows to consider only clearerr(), feof() and
 *               ferror() defined in the file fs_core_posix.h.
 *
 *           (2) Including fs_core_posix.h besides stdio.h may cause some compilation errors/warnings
 *               with some toolchains (e.g. IAR ARM, ARM GCC). stdio.h is included here to properly handle
 *               its inclusion in your application file. If you need to use stdio.h in addition to
 *               fs_core_posix.h, you shall follow these two requirements:
 *
 *               (a) Include only fs_core_posix.h in your source file. stdio.h must be omitted.
 *
 *               (b) Place fs_core_posix.h as the first file in your inclusion block to ensure that other
 *                   header files do not accidently include stdio.h which could cancel the stdio.h
 *                   handling centralized in fs_core_posix.h.
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs_core_cfg.h>

#if (FS_CORE_CFG_POSIX_EN == DEF_ENABLED)

#include  <common/include/toolchains.h>
#if (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_GNU)
#define _REENT_SMALL                                            // See Note #1.
#endif
#include  <stdio.h>                                             // See Note #2.

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
#include  <fs/include/fs_core_dir.h>
#endif
#include  <fs/include/fs_core_file.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef FS_CORE_CFG_POSIX_PUTCHAR
int _fs_errno(void);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 *
 * Note(s) : (1) IEEE Std 1003.1, 2004 Edition, Section 'stdio.h() : DESCRIPTION' states that fpos_t
 *               should be A non-array type containing all information needed to specify uniquely every
 *               position within a file."
 *
 *           (2) IEEE Std 1003.1, 2004 Edition, Section 'sys/types.h() : DESCRIPTION' states that
 *
 *               (a) off_t is "Used for file sizes" and, additionally, that it "shall be [a] signed
 *                   integer [type]".
 *
 *               (a) size_t is "Used for sizes of objects" and, additionally, that it "shall be an
 *                   unsigned integer type".
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT32U fs_fpos_t;

typedef CPU_INT32U fs_off_t;

typedef CPU_SIZE_T fs_size_t;

typedef CPU_INT32U fs_dev_t;

typedef CPU_INT32U fs_ino_t;

typedef CPU_INT32U fs_mode_t;

typedef CPU_INT32U fs_nlink_t;

typedef CPU_INT32U fs_uid_t;

typedef CPU_INT32U fs_gid_t;

typedef CPU_INT32U fs_time_t;

typedef CPU_INT32U fs_blksize_t;

typedef CPU_INT32U fs_blkcnt_t;

typedef FS_FILE_HANDLE FS_FILE;

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
typedef FS_DIR_HANDLE FS_DIR;
#endif

/*
 ********************************************************************************************************
 *                                           FILE STATS DATA TYPE
 *******************************************************************************************************/

struct fs_stat {
  fs_dev_t     st_dev;                                          ///< Device ID of device containing file.
  fs_ino_t     st_ino;                                          ///< File serial number.
  fs_mode_t    st_mode;                                         ///< Mode of file.
  fs_nlink_t   st_nlink;                                        ///< Number of hard links to the file.
  fs_uid_t     st_uid;                                          ///< User ID of file.
  fs_gid_t     st_gid;                                          ///< Group ID of file.
  fs_off_t     st_size;                                         ///< File size in bytes.
  fs_time_t    st_atime;                                        ///< Time of last access.
  fs_time_t    st_mtime;                                        ///< Time of last data modification.
  fs_time_t    st_ctime;                                        ///< Time of last status change.
  fs_blksize_t st_blksize;                                      ///< Preferred I/O block size for file.
  fs_blkcnt_t  st_blocks;                                       ///< Number of blocks allocated for file.
};

/*
 ********************************************************************************************************
 *                                           TIME DATA TYPE
 *******************************************************************************************************/

struct fs_tm {
  int tm_sec;                                                   ///< Seconds [0,60].
  int tm_min;                                                   ///< Minutes [0,59].
  int tm_hour;                                                  ///< Hour [0,23].
  int tm_mday;                                                  ///< Day of month [1,31].
  int tm_mon;                                                   ///< Month of year [0,11].
  int tm_year;                                                  ///< Years since 1900.
  int tm_wday;                                                  ///< Day of week [0,6] (Sunday = 0).
  int tm_yday;                                                  ///< Day of year [1,366].
  int tm_isdst;                                                 ///< Daylight Savings flag.
};

/*
 ********************************************************************************************************
 *                                       DIRECTORY ENTRY DATA TYPE
 *******************************************************************************************************/

struct fs_dirent {
  fs_ino_t d_ino;                                               ///< File serial number.
  char     d_name[256];                                         ///< Buffer receiving directory entry name.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  BUFSIZ
#undef  BUFSIZ
#endif
#define BUFSIZ                              4096u

#ifdef  EOF
#undef  EOF
#endif
#define EOF                                 (-1)

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
#ifdef  _IOFBF
#undef  _IOFBF
#endif
#define _IOFBF                              FS_FILE_BUF_MODE_RD_WR

#ifdef  _IONBR
#undef  _IONBR
#endif
#define _IONBR                              FS_FILE_BUF_MODE_NONE
#endif

#ifdef  FILENAME_MAX
#undef  FILENAME_MAX
#endif
#define FILENAME_MAX                        256u

#ifdef  SEEK_SET
#undef  SEEK_SET
#endif
#define SEEK_SET                            FS_FILE_ORIGIN_START

#ifdef  SEEK_CUR
#undef  SEEK_CUR
#endif
#define SEEK_CUR                            FS_FILE_ORIGIN_CUR

#ifdef  SEEK_END
#undef  SEEK_END
#endif
#define SEEK_END                            FS_FILE_ORIGIN_END

#ifdef  NULL
#undef  NULL
#endif
#define NULL                                DEF_NULL

#ifdef  STDIO_ERR
#undef  STDIO_ERR
#endif
#define STDIO_ERR                           (-1)

/********************************************************************************************************
 *                                               ERROR CODES
 *******************************************************************************************************/

#ifdef   EACCES
#undef   EACCES
#endif
#define  EACCES                          1u                     ///< "Permission denied."

#ifdef   EAGAIN
#undef   EAGAIN
#endif
#define  EAGAIN                          2u                     ///< "Resource temporarily unavailable."

#ifdef   EBADF
#undef   EBADF
#endif
#define  EBADF                           3u                     ///< "Bad file descriptor."

#ifdef   EBUSY
#undef   EBUSY
#endif
#define  EBUSY                           4u                     ///< "Resource busy."

#ifdef   EEXIST
#undef   EEXIST
#endif
#define  EEXIST                          5u                     ///< "File exists."

#ifdef   EFBIG
#undef   EFBIG
#endif
#define  EFBIG                           6u                     ///< "File too large."

#ifdef   EINVAL
#undef   EINVAL
#endif
#define  EINVAL                          7u                     ///< "Invalid argument."

#ifdef   EIO
#undef   EIO
#endif
#define  EIO                             8u                     ///< "Input/output error."

#ifdef   EISDIR
#undef   EISDIR
#endif
#define  EISDIR                          9u                     ///< "Is a directory."

#ifdef   EMFILE
#undef   EMFILE
#endif
#define  EMFILE                         10u                     ///< "Too many files open in system."

#ifdef   ENAMETOOLONG
#undef   ENAMETOOLONG
#endif
#define  ENAMETOOLONG                   11u                     ///< "Filename too long."

#ifdef   EBFILE
#undef   EBFILE
#endif
#define  EBFILE                         12u                     ///< "Too many open files."

#ifdef   ENOENT
#undef   ENOENT
#endif
#define  ENOENT                         13u                     ///< "No such file or directory."

#ifdef   ENOMEM
#undef   ENOMEM
#endif
#define  ENOMEM                         14u                     ///< "Not enough space."

#ifdef   ENOSPC
#undef   ENOSPC
#endif
#define  ENOSPC                         15u                     ///< "No space left on device."

#ifdef   ENOTDIR
#undef   ENOTDIR
#endif
#define  ENOTDIR                        16u                     ///< "Not a directory."

#ifdef   ENOTEMPTY
#undef   ENOTEMPTY
#endif
#define  ENOTEMPTY                      17u                     ///< "Directory not empty."

#ifdef   EOVERFLOW
#undef   EOVERFLOW
#endif
#define  EOVERFLOW                      18u                     ///< "Value too large to be stored in date type."

#ifdef   ERANGE
#undef   ERANGE
#endif
#define  ERANGE                         19u                     ///< "Result too large or too small."

#ifdef   EROFS
#undef   EROFS
#endif
#define  EROFS                          20u                     ///< "Read-only file system."

#ifdef   EXDEV
#undef   EXDEV
#endif
#define  EXDEV                          21u                     ///< "Improper link."

#ifdef   EPERM
#undef   EPERM
#endif
#define  EPERM                          22u                     ///< "Operation not permitted."

#ifdef   ENOTDEF
#undef   ENOTDEF
#endif
#define  ENOTDEF                        23u                     ///< Error not mapped to a POSIX error code.

/********************************************************************************************************
 *                                               MODE VALUES
 *******************************************************************************************************/

#ifdef   S_IFMT
#undef   S_IFMT
#endif
#define  S_IFMT                         0x7000u

#ifdef   S_IFBLK
#undef   S_IFBLK
#endif
#define  S_IFBLK                        0x1000u

#ifdef   S_IFCHR
#undef   S_IFCHR
#endif
#define  S_IFCHR                        0x2000u

#ifdef   S_IFIFO
#undef   S_IFIFO
#endif
#define  S_IFIFO                        0x3000u

#ifdef   S_IFREG
#undef   S_IFREG
#endif
#define  S_IFREG                        0x4000u

#ifdef   S_IFDIR
#undef   S_IFDIR
#endif
#define  S_IFDIR                        0x5000u

#ifdef   S_IFLNK
#undef   S_IFLNK
#endif
#define  S_IFLNK                        0x6000u

#ifdef   S_IFSOCK
#undef   S_IFSOCK
#endif
#define  S_IFSOCK                       0x7000u

#ifdef   S_IRUSR
#undef   S_IRUSR
#endif
#define  S_IRUSR                        0x0400u

#ifdef   S_IWUSR
#undef   S_IWUSR
#endif
#define  S_IWUSR                        0x0200u

#ifdef   S_IXUSR
#undef   S_IXUSR
#endif
#define  S_IXUSR                        0x0100u

#ifdef   S_IRWXU
#undef   S_IRWXU
#endif
#define  S_IRWXU                        (S_IRUSR | S_IWUSR | S_IXUSR)

#ifdef   S_IRGRP
#undef   S_IRGRP
#endif
#define  S_IRGRP                        0x0040u

#ifdef   S_IWGRP
#undef   S_IWGRP
#endif
#define  S_IWGRP                        0x0020u

#ifdef   S_IXGRP
#undef   S_IXGRP
#endif
#define  S_IXGRP                        0x0010u

#ifdef   S_IRWXG
#undef   S_IRWXG
#endif
#define  S_IRWXG                        (S_IRGRP | S_IWGRP | S_IXGRP)

#ifdef   S_IROTH
#undef   S_IROTH
#endif
#define  S_IROTH                        0x0004u

#ifdef   S_IWOTH
#undef   S_IWOTH
#endif
#define  S_IWOTH                        0x0002u

#ifdef   S_IXOTH
#undef   S_IXOTH
#endif
#define  S_IXOTH                        0x0001u

#ifdef   S_IRWXO
#undef   S_IRWXO
#endif
#define  S_IRWXO                        (S_IROTH | S_IWOTH | S_IXOTH)

/********************************************************************************************************
 *                                       STANDARD POSIX FUNCTIONS
 *******************************************************************************************************/

#define  p_error       fs_perror

#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
#define  chdir         fs_chdir
#define  getcwd        fs_getcwd
#endif

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
#define  closedir      fs_closedir
#define  opendir       fs_opendir
#define  readdir       fs_readdir
#endif

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
#define  mkdir         fs_mkdir
#define  remove        fs_remove
#define  rmdir         fs_rmdir
#endif

#define  stat          fs_stat
#define  clearerr      fs_clearerr
#define  fclose        fs_fclose
#define  feof          fs_feof
#define  ferror        fs_ferror

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
#define  setbuf        fs_setbuf
#define  setvbuf       fs_setvbuf
#define  fflush        fs_fflush
#endif

#define  fgetpos       fs_fgetpos
#define  fopen         fs_fopen
#define  fread         fs_fread
#define  fseek         fs_fseek
#define  rewind        fs_rewind
#define  fsetpos       fs_fsetpos
#define  fileno        fs_fileno
#define  fstat         fs_fstat
#define  ftell         fs_ftell

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
#define  fwrite        fs_fwrite
#define  ftruncate     fs_ftruncate
#endif

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
#define  flockfile     fs_flockfile
#define  ftrylockfile  fs_ftrylockfile
#define  funlockfile   fs_funlockfile
#endif

#define  asctime_r     fs_asctime_r
#define  ctime_r       fs_ctime_r
#define  localtime_r   fs_localtime_r
#define  mktime        fs_mktime

/********************************************************************************************************
 *                                           STANDARD POSIX TYPES
 *******************************************************************************************************/

#define  fpos_t        fs_fpos_t
#define  off_t         fs_off_t
#define  size_t        fs_size_t
#define  dev_t         fs_dev_t
#define  ino_t         fs_ino_t
#define  mode_t        fs_mode_t
#define  nlink_t       fs_nlink_t
#define  uid_t         fs_uid_t
#define  gid_t         fs_gid_t
#define  time_t        fs_time_t
#define  blksize_t     fs_blksize_t
#define  blkcnt_t      fs_blkcnt_t
#define  FILE          FS_FILE
#define  DIR           FS_DIR

#define  stat          fs_stat
#define  dirent        fs_dirent
#define  tm            fs_tm

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef   S_ISREG
#undef   S_ISREG
#endif
#define  S_ISREG(m)                  ((((m) & S_IFMT) == S_IFREG) ? 1 : 0)

#ifdef   S_ISDIR
#undef   S_ISDIR
#endif
#define  S_ISDIR(m)                  ((((m) & S_IFMT) == S_IFDIR) ? 1 : 0)

/********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *
 * Note(s) : (1) (a) Equivalents of all standard POSIX 'stdio.h' functions are provided EXCEPT :
 *
 *                   (1) The following file operation functions :
 *                       (A) tmpfile()
 *                       (B) tmpnam()
 *                   (2) The following file access functions :
 *                       (A) freopen()
 *                   (3) ALL formatted input/output functions.
 *                   (4) ALL character input/output functions.
 *                   (5) The following error-handling functions:
 *                       (A) perror()
 *
 *               (b) The original function name, besides the prefixed 'fs_', is preserved.
 *
 *               (c) The types of arguments & return values are transformed for the file system suite
 *                   environment :
 *
 *                   (a) 'FILE *' --> 'FS_FILE *'.
 *                   (b) 'fpos_t' --> 'fs_fpos_t'.
 *                   (c) 'size_t' --> 'fs_size_t'.
 *
 *           (2) (a) Equivalents of several POSIX 'dirent.h' functions are provided :
 *
 *                   (1) closedir()
 *                   (2) opendir()
 *                   (3) readdir()
 *
 *               (b) The original function name, besides the prefixed 'fs_', is preserved.
 *
 *               (c) The types of arguments & return values were transformed for the file system suite
 *                   environment :
 *
 *                   (1) 'DIR *'           --> 'FS_DIR *'.
 *                   (2) 'struct dirent *' --> 'struct fs_dirent *'
 *
 *           (3) (a) Equivalents of several POSIX 'sys/stat.h' functions are provided :
 *
 *                   (1) fstat()
 *                   (2) mkdir()
 *                   (3) stat()
 *
 *               (b) The original function name, besides the prefixed 'fs_', is preserved.
 *
 *           (4) (a) Equivalents of several POSIX 'time.h' function are provided :
 *
 *                   (1) asctime_r()
 *                   (2) ctime_r()
 *                   (3) localtime_r()
 *                   (4) mktime()
 *
 *               (b) The original function name, besides the prefixed 'fs_', is preserved.
 *
 *           (5) (a) Equivalents of several POSIX 'unistd.h' functions are provided :
 *
 *                   (1) chdir()
 *                   (2) ftruncate()
 *                   (3) getcwd()
 *                   (4) rmdir()
 *
 *               (b) The original function name, besides the prefixed 'fs_', is preserved.
 *
 *               (c) The types of arguments & return values were transformed for the file system suite
 *                   environment :
 *
 *                   (1) 'int'     --> 'FS_FILE *'. (file descriptor)
 *                   (2) 'off_t'   --> 'fs_off_t'.
 *                   (3) 'size_t'  --> 'fs_size_t'.
 *******************************************************************************************************/

/********************************************************************************************************
 *                               UNPREFIXED WRAPPER FUNCTION PROTOTYPES
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  FS_CORE_CFG_POSIX_PUTCHAR
void fs_perror(const char *err_desc);
#endif

//                                                                 ----------- WORKING DIRECTORY FUNCTIONS ------------
#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
int fs_chdir(const char *path_dir);                             // Set the working dir for current task.

char *fs_getcwd(char      *path_dir,                            // Get the working dir for current task.
                fs_size_t size);
#endif

//                                                                 --------------- DIRECTORY FUNCTIONS ----------------
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
FS_DIR *fs_opendir(const char *name_full);                      // Open a directory.

int fs_closedir(FS_DIR *p_dir);                                 // Close & free a directory.

int fs_readdir_r(FS_DIR           *p_dir,                       // Read a directory entry from a directory
                 struct fs_dirent *p_dir_entry,
                 struct fs_dirent **pp_result);
#endif

//                                                                 ----------------- ENTRY FUNCTIONS ------------------
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
int fs_mkdir(const char *name_full);                            // Create a directory.

int fs_remove(const char *name_full);                           // Delete a file or directory.

int fs_rename(const char *name_full_old,                        // Rename a file or directory.
              const char *name_full_new);

int fs_rmdir(const char *name_full);                            // Delete a directory.
#endif

int fs_stat(const char     *name_full,                          // Get information about a file or dir.
            struct fs_stat *p_info);

//                                                                 ------------------ FILE FUNCTIONS ------------------
FS_FILE *fs_fopen(const char *name_full,                        // Open a file.
                  const char *str_mode);

int fs_fclose(FS_FILE *p_file);                                 // Close & free a file.

fs_size_t fs_fread(void      *p_dest,                           // Read from a file.
                   fs_size_t size,
                   fs_size_t nitems,
                   FS_FILE   *p_file);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
fs_size_t fs_fwrite(const void *p_src,                          // Write to a file.
                    fs_size_t  size,
                    fs_size_t  nitems,
                    FS_FILE    *p_file);

int fs_ftruncate(int      file_desc,                            // Truncate a file.
                 fs_off_t size);
#endif

int fs_feof(FS_FILE *p_file);                                   // Test EOF indicator on a file.

int fs_ferror(FS_FILE *p_file);                                 // Test error indicator on a file.

void fs_clearerr(FS_FILE *p_file);                              // Clear EOF & error indicators on a file.

int fs_fgetpos(FS_FILE   *p_file,                               // Get file position indicator.
               fs_fpos_t *p_pos);

int fs_fsetpos(FS_FILE         *p_file,                         // Set file position indicator.
               const fs_fpos_t *p_pos);

int fs_fseek(FS_FILE  *p_file,                                  // Set file position indicator.
             long int offset,
             int      origin);

long int fs_ftell(FS_FILE *p_file);                             // Get file position indicator.

void fs_rewind(FS_FILE *p_file);                                // Reset file position indicator of a file

int fs_fileno(FS_FILE *p_file);                                 // Get file desc int associated to file.

int fs_fstat(int            file_desc,                          // Get information about a file.
             struct fs_stat *p_info);

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
void fs_flockfile(FS_FILE *p_file);                             // Acquire task ownership of a file.

int fs_ftrylockfile(FS_FILE *p_file);                           // Acquire task ownership of file, if avail

void fs_funlockfile(FS_FILE *p_file);                           // Release task ownership of a file.
#endif

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
void fs_setbuf(FS_FILE *p_file,                                 // Assign buffer to a file.
               char    *p_buf);

int fs_setvbuf(FS_FILE   *p_file,                               // Assign buffer to a file.
               char      *p_buf,
               int       mode,
               fs_size_t size);

int fs_fflush(FS_FILE *p_file);                                 // Flush buffer contents to file.
#endif

//                                                                 ------------------ TIME FUNCTIONS ------------------
char *fs_asctime_r(const struct fs_tm *p_time,                  // Convert date/time to string.
                   char               *str_time);

char *fs_ctime_r(const fs_time_t *p_ts,                         // Convert timestamp to string.
                 char            *str_time);

struct fs_tm *fs_localtime_r(const fs_time_t *p_ts,             // Convert timestamp to date/time.
                             struct fs_tm    *p_time);

fs_time_t fs_mktime(struct fs_tm *p_time);                      // Convert date/time to timestamp.

#ifdef __cplusplus
}
#endif

#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
