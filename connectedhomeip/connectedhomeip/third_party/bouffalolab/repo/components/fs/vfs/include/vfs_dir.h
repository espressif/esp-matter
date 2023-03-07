#ifndef __VFS_DIR_H__
#define __VFS_DIR_H__

struct statfs {
    long f_type;    /* fs type */
    long f_bsize;   /* optimized transport block size */
    long f_blocks;  /* total blocks */
    long f_bfree;   /* available blocks */
    long f_bavail;  /* number of blocks that non-super users can acquire */
    long f_files;   /* total number of file nodes */
    long f_ffree;   /* available file nodes */
    long f_fsid;    /* fs id */
    long f_namelen; /* max file name length */
};

typedef struct {
    int     d_ino;    /* file number */
    uint8_t d_type;   /* type of file */
    char    d_name[]; /* file name */
} aos_dirent_t;

typedef struct {
    int dd_vfs_fd;
    int dd_rsv;
} aos_dir_t;

#endif
