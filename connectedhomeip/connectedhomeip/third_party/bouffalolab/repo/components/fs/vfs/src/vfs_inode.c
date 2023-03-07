/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <utils_rbtree.h>
#include <vfs_inode.h>

#define VFS_NULL_PARA_CHK(para)     do { if (!(para)) return -EINVAL; } while(0)
struct rb_tree *inode_rb_tree = NULL;

static int inode_cmp_cb (struct rb_tree *self, struct rb_node *node_a, struct rb_node *node_b)
{
    int ret = 0;
    inode_t *a = (inode_t *) node_a->value;
    inode_t *b = (inode_t *) node_b->value;
    if (INODE_IS_TYPE(a, VFS_TYPE_FS_DEV) ^ INODE_IS_TYPE(b, VFS_TYPE_FS_DEV)) {
        ret = strncmp(a->i_name, b->i_name, INODE_IS_TYPE(a, VFS_TYPE_FS_DEV) ? strlen(a->i_name) : strlen(b->i_name));
        if (!ret && strlen(a->i_name) != strlen(b->i_name)) {
            ret = INODE_IS_TYPE(a, VFS_TYPE_FS_DEV) ? strncmp(b->i_name + strlen(a->i_name), "/", 1) : strncmp(a->i_name + strlen(b->i_name), "/", 1);
            goto exit; 
        }
        goto exit;
    }
    ret = strcmp(a->i_name, b->i_name);
exit:
    return ret;
}

int inode_init()
{
    int ret = 0;
    if (NULL == inode_rb_tree) {
        inode_rb_tree = rb_tree_create(inode_cmp_cb);
        if (NULL == inode_rb_tree) {
            ret = -1;
        }
    }
    return ret;
}

int inode_alloc(inode_t **node)
{
    *node = pvPortMalloc(sizeof(inode_t));

    VFS_ASSERT(NULL != *node);

    memset(*node, 0, sizeof(inode_t));

    return 0;
}

int inode_del(inode_t *node)
{
    if (node->refs > 0) {
        return -EBUSY;
    }

    if (node->refs == 0) {
        rb_tree_remove(inode_rb_tree, node);
        if (node->i_name != NULL) {
            vPortFree(node->i_name);
        }
        vPortFree(node);
    }

    return VFS_SUCCESS;
}

inode_t *inode_open(const char *path)
{
    inode_t node;
    memset(&node, 0, sizeof(inode_t));
    node.i_name = (char *)path;
    return (inode_t *)rb_tree_find(inode_rb_tree, &node);
}

int inode_forearch_name(int (*cb)(void *arg, inode_t *node), void *arg)
{
    int i;
    size_t node_num;
    inode_t *node;
    struct rb_iter *iter;
    node_num = rb_tree_size(inode_rb_tree);
    
    if (0 == node_num) {
        goto exit;
    }

    iter = rb_iter_create();
    
    if (NULL == iter) {
        node_num = 0;
        goto exit;
    }

    node = rb_iter_first(iter, inode_rb_tree);
    
    if (NULL == node) {
        rb_iter_dealloc(iter);
        node_num = 0;
        goto exit;
    }

    for (i = 0; i < node_num; i++) {
        if (NULL != node->i_name) {
            if (cb(arg, node)) {
                break;
            }
        }
        node = rb_iter_next(iter);
        if (NULL == node) {
            break;
        }
    }

    rb_iter_dealloc(iter);
exit:
    return node_num;
}

int inode_ptr_get(int fd, inode_t **node)
{
    /*this API is useless if use rb_tree*/
#if 0    
    if (fd < 0 || fd >= AOS_CONFIG_VFS_DEV_NODES) {
        return -EINVAL;
    }

    *node = &g_vfs_dev_nodes[fd];
#endif
    return VFS_SUCCESS;
}

void inode_ref(inode_t *node)
{
    node->refs++;
}

void inode_unref(inode_t *node)
{
    if (node->refs > 0) {
        node->refs--;
    }
}

int inode_busy(inode_t *node)
{
    return node->refs > 0;
}

int inode_avail_count(void)
{
    return AOS_CONFIG_VFS_DEV_NODES - rb_tree_size(inode_rb_tree);
}

static int inode_set_name(const char *path, inode_t **inode)
{
    size_t len;
    void  *mem;

    len = strlen(path);
    mem = (void *)pvPortMalloc(len + 1);

    VFS_ASSERT(NULL != mem);

    memcpy(mem, (const void *)path, len);
    (*inode)->i_name = (char *)mem;
    (*inode)->i_name[len] = '\0';

    return VFS_SUCCESS;
}

int inode_reserve(const char *path, inode_t **inode)
{
    int ret;
    inode_t *node = NULL;

    VFS_NULL_PARA_CHK(path != NULL && inode != NULL);
    *inode = NULL;

    /* Handle paths that are interpreted as the root directory */
#ifdef _WIN32
    if (path[0] == '\0' || path[1] != ':') {
#else
    if (path[0] == '\0' || path[0] != '/') {
#endif
        return -EINVAL;
    }
    
    if (NULL == inode_rb_tree || AOS_CONFIG_VFS_DEV_NODES <= rb_tree_size(inode_rb_tree)) {
        printf("inode_rb_tree is NULL when inode alloc\r\n");
        return -EINVAL;
    }

    ret = inode_alloc(&node);
    if (ret < 0) {
        return ret;
    }

    ret = inode_set_name(path, &node);
    if (ret < 0) {
        vPortFree(node);
        return ret;
    }

    if (!(rb_tree_insert(inode_rb_tree, node))) {
        vPortFree(node->i_name);
        vPortFree(node);
        return -ENOMEM;
    }

    *inode = node;
    return VFS_SUCCESS;
}

int inode_release(const char *path)
{
    int ret;
    inode_t *node;

    VFS_NULL_PARA_CHK(path != NULL);

    node = inode_open(path);
    if (node == NULL) {
        return -ENODEV;
    }

    ret = inode_del(node);
    if (ret < 0) {
        return ret;
    }

    return VFS_SUCCESS;
}

