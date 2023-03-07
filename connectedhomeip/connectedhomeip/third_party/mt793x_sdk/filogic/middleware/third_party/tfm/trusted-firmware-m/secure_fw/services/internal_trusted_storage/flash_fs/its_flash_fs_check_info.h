/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file its_flash_fs_check_info.h
 *
 * \brief Checks at compile time that the flash device configuration is valid.
 *        Should be included after defining the flash info parameters for a
 *        flash device.
 */

#ifndef __ITS_FLASH_FS_CHECK_INFO_H__
#define __ITS_FLASH_FS_CHECK_INFO_H__

#include "its_flash_fs_mblock.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ITS_BLOCK_META_HEADER_SIZE  sizeof(struct its_metadata_block_header_t)
#define ITS_BLOCK_METADATA_SIZE     sizeof(struct its_block_meta_t)
#define ITS_FILE_METADATA_SIZE      sizeof(struct its_file_meta_t)

#if ((FLASH_INFO_NUM_BLOCKS < 2) || (FLASH_INFO_NUM_BLOCKS == 3))
  /* The minimum number of blocks is 2. In this case, metadata and data are
   * stored in the same physical block, and the other block is required for
   * power failure safe operation.
   * If at least 1 data block is available, 1 data scratch block is required for
   * power failure safe operation. So, in this case, the minimum number of
   * blocks is 4 (2 metadata block + 2 data blocks).
   */
  #error "Total number of blocks should be 2 or bigger than 3"
#endif

/* The numbers in the defines are physical block indexes, starting from 0,
 * except for ITS_NUM_DEDICATED_DBLOCKS.
 */
#if (FLASH_INFO_NUM_BLOCKS == 2)
  /* Metadata and data are stored in the same physical block, and the other
   * block is required for power failure safe operation.
   */

  /* Initial position of scratch block is the scratch metadata block */
  #define ITS_INIT_SCRATCH_DBLOCK 1

  /* Metadata and data are stored in the same block */
  #define ITS_INIT_DBLOCK_START 0

  /* There are no dedicated data blocks when only two blocks are available */
  #define ITS_NUM_DEDICATED_DBLOCKS 0

#else

  /* Initial position of scratch block is immediately after metadata blocks */
  #define ITS_INIT_SCRATCH_DBLOCK 2

  /* One metadata block and two scratch blocks are reserved. One scratch block
   * for metadata operations and the other for files data operations.
   */
  #define ITS_INIT_DBLOCK_START 3

  /* Number of blocks dedicated just for data is the number of blocks available
   * beyond the initial datablock start index.
   */
  #define ITS_NUM_DEDICATED_DBLOCKS (FLASH_INFO_NUM_BLOCKS - \
                                     ITS_INIT_DBLOCK_START)
#endif /* FLASH_INFO_NUM_BLOCKS == 2 */

/* Total number of datablocks is the number of dedicated datablocks plus
 * logical datablock 0 stored in the metadata block.
 */
#define ITS_NUM_ACTIVE_DBLOCKS (ITS_NUM_DEDICATED_DBLOCKS + 1)

#define ITS_ALL_METADATA_SIZE \
    (ITS_BLOCK_META_HEADER_SIZE \
     + (ITS_NUM_ACTIVE_DBLOCKS * ITS_BLOCK_METADATA_SIZE) \
     + (FLASH_INFO_MAX_NUM_FILES * ITS_FILE_METADATA_SIZE))

/* It is not required that all files fit in ITS flash area at the same time.
 * So, it is possible that a create action fails because flash is full.
 * However, the larger file must have enough space in the ITS flash area to be
 * created, at least, when the ITS flash area is empty.
 */
/* Checks at compile time if the largest file fits in the data area */
ITS_UTILS_BOUND_CHECK(LARGEST_ITS_FILE_NOT_FIT_IN_DATA_BLOCK,
                      FLASH_INFO_MAX_FILE_SIZE, FLASH_INFO_BLOCK_SIZE);

#if (FLASH_INFO_NUM_BLOCKS == 2)
ITS_UTILS_BOUND_CHECK(ITS_FILE_NOT_FIT_IN_DATA_AREA, FLASH_INFO_MAX_FILE_SIZE,
                      (FLASH_INFO_BLOCK_SIZE - ITS_ALL_METADATA_SIZE));
#endif

/* Checks at compile time if the metadata fits in a flash block */
ITS_UTILS_BOUND_CHECK(ITS_METADATA_NOT_FIT_IN_METADATA_BLOCK,
                      ITS_ALL_METADATA_SIZE, FLASH_INFO_BLOCK_SIZE);

#ifdef __cplusplus
}
#endif

#endif /* __ITS_FLASH_FS_CHECK_INFO_H__ */
