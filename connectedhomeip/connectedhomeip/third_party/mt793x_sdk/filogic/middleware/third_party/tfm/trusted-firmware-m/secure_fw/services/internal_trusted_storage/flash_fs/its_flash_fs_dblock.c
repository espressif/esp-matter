/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_flash_fs_dblock.h"

#include "secure_fw/services/internal_trusted_storage/flash/its_flash.h"

/**
 * \brief Converts logical data block number to physical number.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     lblock  Logical block number
 *
 * \return Return physical block number.
 */
static uint32_t its_dblock_lo_to_phy(struct its_flash_fs_ctx_t *fs_ctx,
                                     uint32_t lblock)
{
    struct its_block_meta_t block_meta;
    psa_status_t err;

    err = its_flash_fs_mblock_read_block_metadata(fs_ctx, lblock, &block_meta);
    if (err != PSA_SUCCESS) {
        return ITS_BLOCK_INVALID_ID;
    }

    return block_meta.phy_id;
}

psa_status_t its_flash_fs_dblock_compact_block(
                                              struct its_flash_fs_ctx_t *fs_ctx,
                                              uint32_t lblock,
                                              size_t free_size,
                                              size_t src_offset,
                                              size_t dst_offset,
                                              size_t size)
{
    struct its_block_meta_t block_meta;
    psa_status_t err;
    uint32_t scratch_id = 0;

    /* Read current block meta */
    err = its_flash_fs_mblock_read_block_metadata(fs_ctx, lblock, &block_meta);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Release data from block meta */
    block_meta.free_size += free_size;

    /* Save scratch data block physical IDs */
    scratch_id = its_flash_fs_mblock_cur_data_scratch_id(fs_ctx, lblock);

    /* Check if there are bytes to be compacted */
    if (size > 0) {
        /* Move data from source offset in current data block to scratch block
         * destination offset.
         */
        err = its_flash_block_to_block_move(fs_ctx->flash_info, scratch_id,
                                            dst_offset, block_meta.phy_id,
                                            src_offset, size);
        if (err != PSA_SUCCESS) {
            return PSA_ERROR_GENERIC_ERROR;
        }
    }

    if (dst_offset > block_meta.data_start) {
        /* Copy data from the beginning of data block until
         * the position where the data will be reallocated later
         */
        err = its_flash_block_to_block_move(fs_ctx->flash_info, scratch_id,
                                            block_meta.data_start,
                                            block_meta.phy_id,
                                            block_meta.data_start,
                                            (dst_offset-block_meta.data_start));
        if (err != PSA_SUCCESS) {
            return PSA_ERROR_GENERIC_ERROR;
        }
    }

    /* Swap the scratch and current data blocks. Must swap even with nothing
     * to compact so that deleted file is left in scratch and erased as part
     * of finalization.
     */
    its_flash_fs_mblock_set_data_scratch(fs_ctx, block_meta.phy_id, lblock);

    /* Set scratch block ID as the one which contains the new data block */
    block_meta.phy_id = scratch_id;

    /* Update block metadata in scratch metadata block */
    err = its_flash_fs_mblock_update_scratch_block_meta(fs_ctx, lblock,
                                                        &block_meta);
    if (err != PSA_SUCCESS) {
        /* Swap back the data block as there was an issue in the process */
        its_flash_fs_mblock_set_data_scratch(fs_ctx, scratch_id, lblock);
        return err;
    }

    /* Commit data block modifications to flash, unless the data is in logical
     * data block 0, in which case it will be flushed at the end of the metadata
     * block update.
     */
    if (lblock != ITS_LOGICAL_DBLOCK0) {
        err = fs_ctx->flash_info->flush(fs_ctx->flash_info);
    }

    return err;
}

psa_status_t its_flash_fs_dblock_read_file(
                                        struct its_flash_fs_ctx_t *fs_ctx,
                                        const struct its_file_meta_t *file_meta,
                                        size_t offset,
                                        size_t size,
                                        uint8_t *buf)
{
    uint32_t phys_block;
    size_t pos;

    phys_block = its_dblock_lo_to_phy(fs_ctx, file_meta->lblock);
    if (phys_block == ITS_BLOCK_INVALID_ID) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    pos = (file_meta->data_idx + offset);

    return fs_ctx->flash_info->read(fs_ctx->flash_info, phys_block, buf, pos,
                                    size);
}

psa_status_t its_flash_fs_dblock_write_file(
                                      struct its_flash_fs_ctx_t *fs_ctx,
                                      const struct its_block_meta_t *block_meta,
                                      const struct its_file_meta_t *file_meta,
                                      size_t offset,
                                      size_t size,
                                      const uint8_t *data)
{
    psa_status_t err;
    uint32_t scratch_id;
    size_t pos;
    size_t num_bytes;

    scratch_id = its_flash_fs_mblock_cur_data_scratch_id(fs_ctx,
                                                         file_meta->lblock);

    /* Calculate the position of the new file data in the block */
    pos = file_meta->data_idx + offset;

    /* Move data up to the new file data position */
    err = its_flash_block_to_block_move(fs_ctx->flash_info,
                                        scratch_id,
                                        block_meta->data_start,
                                        block_meta->phy_id,
                                        block_meta->data_start,
                                        pos - block_meta->data_start);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Write the new file data */
    err = fs_ctx->flash_info->write(fs_ctx->flash_info, scratch_id, data, pos,
                                    size);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Calculate the position of the end of the file */
    pos = file_meta->data_idx + file_meta->max_size;

    /* Calculate the size of the data in the block after the end of the file */
    num_bytes = (fs_ctx->flash_info->block_size - block_meta->free_size) - pos;

    /* Move data between the end of the file and the end of the block data */
    err = its_flash_block_to_block_move(fs_ctx->flash_info, scratch_id, pos,
                                        block_meta->phy_id, pos, num_bytes);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Commit data block modifications to flash, unless the data is in logical
     * data block 0, in which case it will be flushed at the end of the metadata
     * block update.
     */
    if (file_meta->lblock != ITS_LOGICAL_DBLOCK0) {
        err = fs_ctx->flash_info->flush(fs_ctx->flash_info);
    }

    return err;
}
