/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef VFS_ERRNO_H
#define VFS_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>

#define    VFS_SUCCESS               0u

#define VFS_ASSERT(cond)                                    \
    do {                                                    \
        if (!(cond)) {                                      \
            puts("VFS_ASSERT:" #cond);                      \
            printf(" %s:%d\r\n", __FILENAME__, __LINE__);   \
            while(1);                                       \
        }                                                   \
    } while(0)

#ifdef __cplusplus
}
#endif

#endif /* VFS_ERRNO_H */

