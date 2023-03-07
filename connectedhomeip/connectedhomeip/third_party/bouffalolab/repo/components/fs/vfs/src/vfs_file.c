/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <FreeRTOS.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_inode.h>
#include <stdio.h>
#include <string.h>
#include <utils_rbtree.h>
#include <vfs_file.h>

typedef struct files_handle {
    struct rb_tree *file_rb_tree;
    uint32_t fd_bitmap[FILE_BITMAP_NUM];
} files_handle_t;

static files_handle_t *files_handle_dev = NULL;

static int file_cmp_cb(struct rb_tree *self, struct rb_node *node_a, struct rb_node *node_b)
{
    file_t *a = (file_t *) node_a->value;
    file_t *b = (file_t *) node_b->value;
    return (a->fd > b->fd) - (a->fd < b->fd);
}

int file_init(void)
{
    int ret = 0;
    if (NULL == files_handle_dev) {
        files_handle_dev = pvPortMalloc(sizeof(files_handle_t));
        if (NULL == files_handle_dev) {
            ret = -1;
            goto exit;
        }
        memset(files_handle_dev, 0, sizeof(files_handle_t));
        files_handle_dev->file_rb_tree = rb_tree_create(file_cmp_cb);
        if (NULL == files_handle_dev->file_rb_tree) {
            vPortFree(files_handle_dev);
            files_handle_dev = NULL;
            ret = -1;
            goto exit;
        }
    }
exit:
    return ret;
}

file_t *new_file(inode_t *node)
{
    file_t *f = NULL;
    int fd = -1, i;

    if (NULL == files_handle_dev || MAX_FILE_NUM <= rb_tree_size(files_handle_dev->file_rb_tree)) {
        goto exit;
    }

    for (i = 0; i < MAX_FILE_NUM; i++) {
        if ((files_handle_dev->fd_bitmap[i / 32] & (1 << (i % 32))) == 0) {
            files_handle_dev->fd_bitmap[i / 32] |= (1 << (i % 32));
            fd = i;
            break;
        }
    }

    if (fd < 0) {
        goto exit;
    }

    f = pvPortMalloc(sizeof(file_t));

    VFS_ASSERT(f != NULL);

    f->node = node;
    f->f_arg = NULL;
    f->offset = 0;
    f->fd = fd + AOS_CONFIG_VFS_FD_OFFSET;

    if (!(rb_tree_insert(files_handle_dev->file_rb_tree, f))) {
        files_handle_dev->fd_bitmap[fd / 32] &= ~(1 << (fd % 32));
        vPortFree(f);
        f = NULL;
        goto exit;
    }
    inode_ref(node);
exit:
    return f;
}

void del_file(file_t *file)
{
    int fd;
    inode_unref(file->node);
    file->node = NULL;
    fd = file->fd - AOS_CONFIG_VFS_FD_OFFSET;
    files_handle_dev->fd_bitmap[fd / 32] &= ~(1 << (fd % 32));
    rb_tree_remove(files_handle_dev->file_rb_tree, file);
    vPortFree(file);
}

int get_fd(file_t *file)
{
    return file->fd;
}

file_t *get_file(int fd)
{
    file_t f;
    if (fd < 0) {
        return NULL;
    }
    f.fd = fd;

    return (file_t *)rb_tree_find(files_handle_dev->file_rb_tree, &f);
}

size_t get_all_file(file_t **file, int size)
{
    size_t file_num;
    struct rb_iter *iter;
    file_num = rb_tree_size(files_handle_dev->file_rb_tree);

    if (0 == file_num) {
        goto exit;
    }

    file_num = size > file_num ? file_num : size;
    iter = rb_iter_create();

    if (NULL == iter) {
        file_num = 0;
        goto exit;
    }

    file[0] = rb_iter_first(iter, files_handle_dev->file_rb_tree);
    
    if (NULL == file[0]) {
        rb_iter_dealloc(iter);
        file_num = 0;
        goto exit;
    }

    for (int i = 1; i < file_num; i++) {
        file[i] = rb_iter_next(iter);
        if (NULL == file[i]) {
            file_num = i;
            break;
        }
    }

    rb_iter_dealloc(iter);
exit:
    return file_num;
}
