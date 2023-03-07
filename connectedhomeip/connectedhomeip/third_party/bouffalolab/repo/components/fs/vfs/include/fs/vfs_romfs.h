#ifndef AOS_VFS_ROMFS_H
#define AOS_VFS_ROMFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_inode.h"

/* romfs ioctl struct */
typedef struct _romfs_file_buf {
    char *buf;
    uint32_t bufsize;
} romfs_filebuf_t;

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_ROMFS_H */
