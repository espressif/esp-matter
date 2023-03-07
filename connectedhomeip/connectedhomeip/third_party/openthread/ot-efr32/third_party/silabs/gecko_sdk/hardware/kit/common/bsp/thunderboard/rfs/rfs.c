/***************************************************************************//**
 * @file
 * @brief ROM File System Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "thunderboard/rfs/rfs.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#define debug_printf(x, ...)
/** @endcond */

/***************************************************************************//**
 * @cond DOXYGEN_INCLUDE_RFS
 * @defgroup RFS RFS - Read Only File System
 * @{
 * @brief Driver for ROM File System
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
extern const uint32_t RFS_fileCount;      /**< Number of the files in the file system   */
extern const uint8_t *RFS_fileNames[];    /**< Array to store the file names            */
extern const uint32_t RFS_fileLength[];   /**< Array to store the lengths of the files  */
extern const uint8_t *RFS_fileData[];     /**< Array to store pointers to raw file data */
/** @endcond */

/***************************************************************************//**
 * @brief
 *    Gets the number of files stored in the file system
 *
 * @return
 *    Returns the file count
 ******************************************************************************/
uint32_t RFS_getFileCount(void)
{
  return RFS_fileCount;
}

/***************************************************************************//**
 * @brief
 *    Gets the length of a file specified by the file index
 *
 * @param[in] index
 *    File index
 *
 * @return
 *    Returns the file length. If the index is greater than the number of files
 *    in the file system, returns -1.
 ******************************************************************************/
int32_t RFS_getFileLengthByIndex(uint32_t index)
{
  if ( index >= RFS_fileCount ) {
    return -1;
  } else {
    return RFS_fileLength[index];
  }
}

/***************************************************************************//**
 * @brief
 *    Gets the length of a file specified by the file handle
 *
 * @param[in] fileHandle
 *    RFS file handle
 *
 * @return
 *    Returns the file length
 ******************************************************************************/
int32_t RFS_getFileLength(RFS_FileHandle *fileHandle)
{
  return RFS_getFileLengthByIndex(fileHandle->fileIndex);
}

/***************************************************************************//**
 * @brief
 *    Gets the name of a file specified by the file index
 *
 * @param[in] index
 *    File index
 *
 * @return
 *    Returns pointer to an uint8_t type array, which contains the name of the
 *    file. If the index is greater than the number of files in the file system,
 *    returns NULL pointer.
 ******************************************************************************/
uint8_t *RFS_getFileNameByIndex(uint32_t index)
{
  if ( index >= RFS_fileCount ) {
    return NULL;
  } else {
    return (uint8_t *) RFS_fileNames[index];
  }
}

/***************************************************************************//**
 * @brief
 *    Gets the name of a file specified by the file handle
 *
 * @param[in] fileHandle
 *    RFS file handle
 *
 * @return
 *    Returns pointer to an uint8_t type array, which contains the name of the
 *    file.
 ******************************************************************************/
uint8_t *RFS_getFileName(RFS_FileHandle *fileHandle)
{
  return RFS_getFileNameByIndex(fileHandle->fileIndex);
}

/***************************************************************************//**
 * @brief
 *    Gets the index of a file specified by the name
 *
 * @param[in] name
 *    Pointer to an uint8_t array, which contains the file name
 *
 * @return
 *    Returns the file index. If there is no such file with the specified name
 *    returns -1.
 ******************************************************************************/
int16_t RFS_getFileIndex(uint8_t name[])
{
  int i;
  int ret;

  ret = -1;
  for ( i = 0; i < RFS_getFileCount(); i++ ) {
    if ( strncmp( (char const*) name, (char const *) RFS_getFileNameByIndex(i), RFS_MAX_FILE_NAME_SIZE) == 0 ) {
      ret = i;
      break;
    }
  }

  return ret;
}

/***************************************************************************//**
 * @brief
 *    Opens a file specified by the name
 *
 * @param fileHandle
 *    RFS file handle
 *
 * @param[in] name
 *    Pointer to an uint8_t array, which contains the file name
 *
 * @return
 *    Returns 1 on success, error code otherwise
 ******************************************************************************/
int32_t RFS_fileOpen(RFS_FileHandle *fileHandle, uint8_t name[])
{
  int index;

  index = RFS_getFileIndex(name);
  if ( index < 0 ) {
    return index;
  }

  fileHandle->fileIndex = index;
  fileHandle->currentIndex = 0;

  return 1;
}

/***************************************************************************//**
 * @brief
 *    Changes the position of the index pointer in the file
 *
 * @param fileHandle
 *    RFS file handle
 *
 * @param[in] offset
 *    Pointer offset (positive or negative)
 *
 * @param[in] whence
 *    The base pointer position in the file. The index pointer will be moved
 *    to the whence + offset position in the file. The possible values of the
 *    whence parameter are:
 *    RFS_SEEK_SET: the beginning of the file
 *    RFS_SEEK_CUR: the current position of the index pointer
 *    RFS_SEEK_END: the end of file
 *
 * @return
 *    Returns the new index position in case of success, -1 otherwise
 ******************************************************************************/
int32_t RFS_fileSeek(RFS_FileHandle *fileHandle, int32_t offset, uint32_t whence)
{
  /* Check file handle state */

  if ( whence == RFS_SEEK_SET ) {
    fileHandle->currentIndex = offset;
  } else if ( whence == RFS_SEEK_CUR ) {
    fileHandle->currentIndex += offset;
  } else if ( whence == RFS_SEEK_END ) {
    fileHandle->currentIndex = (RFS_getFileLengthByIndex(fileHandle->fileIndex) + offset);
  } else {
    return -1;
  }

  /* Check for out of bounds error on index before returning */

  return fileHandle->currentIndex;
}

/***************************************************************************//**
 * @brief
 *    Reads size*nmemb number of bytes from the file to the output buffer
 *
 * @param[in] buf
 *    Pointer to the output buffer
 *
 * @param[in] size
 *    Size of each element
 *
 * @param[in] nmemb
 *    Number of elements
 *
 * @param fileHandle
 *    RFS file handle
 *
 * @return
 *    Returns 1 on success, error code otherwise
 ******************************************************************************/
int32_t RFS_fileRead(uint8_t *buf, uint32_t size, uint32_t nmemb, RFS_FileHandle *fileHandle)
{
  int32_t i;
  int32_t fileIndex = -1;
  uint32_t byteIndex;
  uint32_t totalByteCount;
  uint8_t *src = (uint8_t *) -1, *dst = (uint8_t *) -1, *end = (uint8_t *) -1;

  /* Get file index */
  fileIndex = fileHandle->fileIndex;
  debug_printf("RFS_fileRead(): fileIndex = %d \n", fileIndex);

  if ( fileIndex < 0 ) {
    return fileIndex;
  }

  if ( fileIndex >= RFS_getFileCount() ) {
    return -1;
  }

  /* Find data index */
  byteIndex = fileHandle->currentIndex;

  src = (unsigned char *) RFS_fileData[fileIndex];
  src += byteIndex;

  debug_printf(" endindex = %d\n", RFS_getFileLengthByIndex(fileIndex) );

  end = (unsigned char *) RFS_fileData[fileIndex];
  end += RFS_getFileLengthByIndex(fileIndex) - 1;

  dst = (uint8_t *) buf;
  debug_printf("RFS_fileRead():  byteIndex = %d    src = %08Xh    end = %08Xh    dst = %08Xh   \n",
               byteIndex, src, end, dst);

  totalByteCount = size * nmemb;

  for ( i = 0; (i < totalByteCount) && (src <= end); i++ ) {
    *dst++ = *src++;
  }

  debug_printf("RFS_fileRead(): Bytes copied: %d [%d]\n", i, totalByteCount);

  fileHandle->currentIndex += i;

  return 1;
}

/***************************************************************************//**
 * @brief
 *    Gets the raw data from a file specified by the file handle
 *
 * @param[in] fileHandle
 *    RFS file handle
 *
 * @return
 *    Returns a pointer to the raw data on success, 0 otherwise
 *
 ******************************************************************************/
uint8_t *RFS_fileGetRawData(RFS_FileHandle *fileHandle)
{
  int32_t fileIndex = -1;

  /* Get file index */
  fileIndex = fileHandle->fileIndex;
  if ( fileIndex < 0 ) {
    return 0;
  }
  if ( fileIndex >= RFS_getFileCount() ) {
    return 0;
  }

  return (unsigned char *) RFS_fileData[fileIndex];
}

/** @} */

/** @endcond */
