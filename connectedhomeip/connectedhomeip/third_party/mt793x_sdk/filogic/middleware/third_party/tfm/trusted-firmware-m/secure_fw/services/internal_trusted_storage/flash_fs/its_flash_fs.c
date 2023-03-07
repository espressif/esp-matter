/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_flash_fs.h"

#include "its_flash_fs_dblock.h"
#include "tfm_memory_utils.h"
#include "secure_fw/services/internal_trusted_storage/its_utils.h"

#define ITS_FLASH_FS_INIT_FILE 0

static psa_status_t its_flash_fs_file_write_aligned_data(
                                      struct its_flash_fs_ctx_t *fs_ctx,
                                      const struct its_block_meta_t *block_meta,
                                      const struct its_file_meta_t *file_meta,
                                      size_t offset,
                                      size_t size,
                                      const uint8_t *data)
{
#if (ITS_FLASH_MAX_ALIGNMENT != 1)
    /* Check that the offset is aligned with the flash program unit */
    if (!ITS_UTILS_IS_ALIGNED(offset, fs_ctx->flash_info->program_unit)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Set the size to be aligned with the flash program unit */
    size = ITS_UTILS_ALIGN(size, fs_ctx->flash_info->program_unit);
#endif

    /* It is not permitted to create gaps in the file */
    if (offset > file_meta->cur_size) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check that the new data is contained within the file's max size */
    if (its_utils_check_contained_in(file_meta->max_size, offset, size)
        != PSA_SUCCESS) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return its_flash_fs_dblock_write_file(fs_ctx, block_meta, file_meta, offset,
                                          size, data);
}

psa_status_t its_flash_fs_prepare(struct its_flash_fs_ctx_t *fs_ctx,
                                  const struct its_flash_info_t *flash_info)
{
    /* Associate the flash device info with the context */
    fs_ctx->flash_info = flash_info;

    /* Initialize metadata block with the valid/active metablock */
    return its_flash_fs_mblock_init(fs_ctx);
}

psa_status_t its_flash_fs_wipe_all(struct its_flash_fs_ctx_t *fs_ctx)
{
    /* Clean and initialize the metadata block */
    return its_flash_fs_mblock_reset_metablock(fs_ctx);
}

psa_status_t its_flash_fs_file_exist(struct its_flash_fs_ctx_t *fs_ctx,
                                     const uint8_t *fid)
{
    psa_status_t err;
    uint32_t idx;

    err = its_flash_fs_mblock_get_file_idx(fs_ctx, fid, &idx);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    return PSA_SUCCESS;
}

psa_status_t its_flash_fs_file_create(struct its_flash_fs_ctx_t *fs_ctx,
                                      const uint8_t *fid,
                                      size_t max_size,
                                      size_t data_size,
                                      uint32_t flags,
                                      const uint8_t *data)
{
    struct its_block_meta_t block_meta;
    uint32_t cur_phys_block;
    psa_status_t err;
    uint32_t idx;
    struct its_file_meta_t file_meta;

#if (ITS_FLASH_MAX_ALIGNMENT != 1)
    /* Set the max_size to be aligned with the flash program unit */
    max_size = ITS_UTILS_ALIGN(max_size, fs_ctx->flash_info->program_unit);
#endif

    /* Check that the file's maximum size is valid */
    if (max_size > fs_ctx->flash_info->max_file_size) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check if file already exists */
    err = its_flash_fs_mblock_get_file_idx(fs_ctx, fid, &idx);
    if (err == PSA_SUCCESS) {
        /* If it exits return an error as needs to be removed first */
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Try to reserve an file based on the input parameters */
    err = its_flash_fs_mblock_reserve_file(fs_ctx, fid, max_size, flags, &idx,
                                           &file_meta, &block_meta);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Check if data needs to be stored in the new file */
    if (data_size != 0) {
        /* Write the content into scratch data block */
        err = its_flash_fs_file_write_aligned_data(fs_ctx, &block_meta,
                                                   &file_meta,
                                                   ITS_FLASH_FS_INIT_FILE,
                                                   data_size,
                                                   data);
        if (err != PSA_SUCCESS) {
            return PSA_ERROR_GENERIC_ERROR;
        }

        /* Add current size to the file metadata */
        file_meta.cur_size = data_size;

        cur_phys_block = block_meta.phy_id;

        /* Cur scratch block become the active datablock */
        block_meta.phy_id =
            its_flash_fs_mblock_cur_data_scratch_id(fs_ctx, file_meta.lblock);

        /* Swap the scratch data block */
        its_flash_fs_mblock_set_data_scratch(fs_ctx, cur_phys_block,
                                             file_meta.lblock);
    }

    /* Update metadata block information */
    err = its_flash_fs_mblock_update_scratch_block_meta(fs_ctx,
                                                        file_meta.lblock,
                                                        &block_meta);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Add file metadata in the metadata block */
    err = its_flash_fs_mblock_update_scratch_file_meta(fs_ctx, idx, &file_meta);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Copy rest of the file metadata entries */
    err = its_flash_fs_mblock_cp_remaining_file_meta(fs_ctx, idx);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* The file data in the logical block 0 is stored in same physical block
     * where the metadata is stored. A change in the metadata requires a
     * swap of physical blocks. So, the file data stored in the current
     * metadata block needs to be copied in the scratch block, if the data
     * of the file processed is not located in the logical block 0. When an
     * file data is located in the logical block 0, that copy has been done
     * while processing the file data.
     */
    if ((file_meta.lblock != ITS_LOGICAL_DBLOCK0) || (data_size == 0)) {
        err = its_flash_fs_mblock_migrate_lb0_data_to_scratch(fs_ctx);
        if (err != PSA_SUCCESS) {
            return PSA_ERROR_GENERIC_ERROR;
        }
    }

    /* Write metadata header, swap metadata blocks and erase scratch blocks */
    return its_flash_fs_mblock_meta_update_finalize(fs_ctx);
}

psa_status_t its_flash_fs_file_get_info(struct its_flash_fs_ctx_t *fs_ctx,
                                        const uint8_t *fid,
                                        struct its_file_info_t *info)
{
    psa_status_t err;
    uint32_t idx;
    struct its_file_meta_t tmp_metadata;

    /* Get the meta data index */
    err = its_flash_fs_mblock_get_file_idx(fs_ctx, fid, &idx);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Read file metadata */
    err = its_flash_fs_mblock_read_file_meta(fs_ctx, idx, &tmp_metadata);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Check if index is still referring to same file */
    if (tfm_memcmp(fid, tmp_metadata.id, ITS_FILE_ID_SIZE)) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    info->size_max = tmp_metadata.max_size;
    info->size_current = tmp_metadata.cur_size;
    info->flags = tmp_metadata.flags;

    return PSA_SUCCESS;
}

psa_status_t its_flash_fs_file_write(struct its_flash_fs_ctx_t *fs_ctx,
                                     const uint8_t *fid,
                                     size_t size,
                                     size_t offset,
                                     const uint8_t *data)
{
    struct its_block_meta_t block_meta;
    uint32_t cur_phys_block;
    psa_status_t err;
    uint32_t idx;
    struct its_file_meta_t file_meta;

    /* Get the file index */
    err = its_flash_fs_mblock_get_file_idx(fs_ctx, fid, &idx);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Read file metadata */
    err = its_flash_fs_mblock_read_file_meta(fs_ctx, idx, &file_meta);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Read block metadata */
    err = its_flash_fs_mblock_read_block_metadata(fs_ctx, file_meta.lblock,
                                                  &block_meta);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Write the content into scratch data block */
    err = its_flash_fs_file_write_aligned_data(fs_ctx, &block_meta, &file_meta,
                                               offset, size, data);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Update the file's current size if required */
    if (offset + size > file_meta.cur_size) {
        /* Update the file metadata */
        file_meta.cur_size = offset + size;
    }

    cur_phys_block = block_meta.phy_id;

    /* Cur scratch block become the active datablock */
    block_meta.phy_id =
        its_flash_fs_mblock_cur_data_scratch_id(fs_ctx, file_meta.lblock);

    /* Swap the scratch data block */
    its_flash_fs_mblock_set_data_scratch(fs_ctx, cur_phys_block,
                                         file_meta.lblock);

    /* Update block metadata in scratch metadata block */
    err = its_flash_fs_mblock_update_scratch_block_meta(fs_ctx,
                                                        file_meta.lblock,
                                                        &block_meta);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Update file metadata to reflect new attributes */
    err = its_flash_fs_mblock_update_scratch_file_meta(fs_ctx, idx, &file_meta);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Copy rest of the file metadata entries */
    err = its_flash_fs_mblock_cp_remaining_file_meta(fs_ctx, idx);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* The file data in the logical block 0 is stored in same physical block
     * where the metadata is stored. A change in the metadata requires a
     * swap of physical blocks. So, the file data stored in the current
     * metadata block needs to be copied in the scratch block, if the data
     * of the file processed is not located in the logical block 0. When an
     * file data is located in the logical block 0, that copy has been done
     * while processing the file data.
     */
    if (file_meta.lblock != ITS_LOGICAL_DBLOCK0) {
        err = its_flash_fs_mblock_migrate_lb0_data_to_scratch(fs_ctx);
        if (err != PSA_SUCCESS) {
            return PSA_ERROR_GENERIC_ERROR;
        }
    }

    /* Update the metablock header, swap scratch and active blocks,
     * erase scratch blocks.
     */
    return its_flash_fs_mblock_meta_update_finalize(fs_ctx);
}

psa_status_t its_flash_fs_file_delete(struct its_flash_fs_ctx_t *fs_ctx,
                                      const uint8_t *fid)
{
    size_t del_file_data_idx;
    uint32_t del_file_lblock;
    uint32_t del_file_idx;
    size_t del_file_max_size;
    psa_status_t err;
    size_t src_offset = fs_ctx->flash_info->block_size;
    size_t nbr_bytes_to_move = 0;
    uint32_t idx;
    struct its_file_meta_t file_meta;

    /* Get the file index */
    err = its_flash_fs_mblock_get_file_idx(fs_ctx, fid, &del_file_idx);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    err = its_flash_fs_mblock_read_file_meta(fs_ctx, del_file_idx, &file_meta);
    if (err != PSA_SUCCESS) {
        return err;
    }

    if (its_utils_validate_fid(file_meta.id) != PSA_SUCCESS) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Save logical block, data_index and max_size to be used later on */
    del_file_lblock = file_meta.lblock;
    del_file_data_idx = file_meta.data_idx;
    del_file_max_size = file_meta.max_size;

    /* Remove file metadata */
    file_meta = (struct its_file_meta_t){0};

    /* Update file metadata in to the scratch block */
    err = its_flash_fs_mblock_update_scratch_file_meta(fs_ctx, del_file_idx,
                                                       &file_meta);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Read all file metadata */
    for (idx = 0; idx < fs_ctx->flash_info->max_num_files; idx++) {
        if (idx == del_file_idx) {
            /* Skip deleted file */
            continue;
        }

        /* Read file meta for the given file index */
        err = its_flash_fs_mblock_read_file_meta(fs_ctx, idx, &file_meta);
        if (err != PSA_SUCCESS) {
            return err;
        }

        /* Check if the file is located in the same logical block and has a
         * valid FID.
         */
        if ((file_meta.lblock == del_file_lblock) &&
            (its_utils_validate_fid(file_meta.id) == PSA_SUCCESS)) {
            /* If a file is located after the data to delete, this
             * needs to be moved.
             */
            if (file_meta.data_idx > del_file_data_idx) {
                /* Check if this is the position after the deleted
                 * data. This will be the first file data to move.
                 */
                if (src_offset > file_meta.data_idx) {
                    src_offset = file_meta.data_idx;
                }

                /* Set the new file data index location in the
                 * data block.
                 */
                file_meta.data_idx -= del_file_max_size;

                /* Increase number of bytes to move */
                nbr_bytes_to_move += file_meta.max_size;
            }
        }
        /* Update file metadata in to the scratch block */
        err = its_flash_fs_mblock_update_scratch_file_meta(fs_ctx, idx,
                                                           &file_meta);
        if (err != PSA_SUCCESS) {
            return err;
        }
    }

    /* Compact data block */
    err = its_flash_fs_dblock_compact_block(fs_ctx, del_file_lblock,
                                            del_file_max_size,
                                            src_offset, del_file_data_idx,
                                            nbr_bytes_to_move);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* The file data in the logical block 0 is stored in same physical block
     * where the metadata is stored. A change in the metadata requires a
     * swap of physical blocks. So, the file data stored in the current
     * metadata block needs to be copied in the scratch block, if the data
     * of the file processed is not located in the logical block 0. When an
     * file data is located in the logical block 0, that copy has been done
     * while processing the file data.
     */
    if (del_file_lblock != ITS_LOGICAL_DBLOCK0) {
        err = its_flash_fs_mblock_migrate_lb0_data_to_scratch(fs_ctx);
        if (err != PSA_SUCCESS) {
            return PSA_ERROR_GENERIC_ERROR;
        }
    }

    /* Update the metablock header, swap scratch and active blocks,
     * erase scratch blocks.
     */
    return its_flash_fs_mblock_meta_update_finalize(fs_ctx);
}

psa_status_t its_flash_fs_file_read(struct its_flash_fs_ctx_t *fs_ctx,
                                    const uint8_t *fid,
                                    size_t size,
                                    size_t offset,
                                    uint8_t *data)
{
    psa_status_t err;
    uint32_t idx;
    struct its_file_meta_t tmp_metadata;

    /* Get the file index */
    err = its_flash_fs_mblock_get_file_idx(fs_ctx, fid, &idx);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Read file metadata */
    err = its_flash_fs_mblock_read_file_meta(fs_ctx, idx, &tmp_metadata);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Check if index is still referring to same file */
    if (tfm_memcmp(fid, tmp_metadata.id, ITS_FILE_ID_SIZE)) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Boundary check the incoming request */
    err = its_utils_check_contained_in(tmp_metadata.cur_size, offset, size);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Read the file from flash */
    err = its_flash_fs_dblock_read_file(fs_ctx, &tmp_metadata, offset, size,
                                        data);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    return PSA_SUCCESS;
}
