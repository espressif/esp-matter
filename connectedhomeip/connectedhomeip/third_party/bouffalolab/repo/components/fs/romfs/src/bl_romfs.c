/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <string.h>
#include <vfs.h>
#include <vfs_inode.h>
#include <vfs_register.h>
#include <fs/vfs_romfs.h>
#include <aos/kernel.h>

#ifdef ROMFS_STATIC_ROOTADDR
#else
#include <bl_mtd.h>
#endif
#include <bl_romfs.h>

#include <utils_log.h>

#define ROMFS_DUBUG(...)
#define ROMFS_ASSERT(EXPR)

#define ROMFS_MOUNTPOINT        "/romfs"       /* must '/' */

#define ROMFS_MAX_NAME_LEN      (64)

#define ROMFH_HRD       0
#define ROMFH_DIR       1
#define ROMFH_REG       2
#define ROMFH_UNKNOW    3

#define HEAD_MAGIC_LEN  (16)    /*romfs header magic num size*/

struct romfh {
    int32_t nextfh;
    int32_t spec;
    int32_t size;
    int32_t checksum;
} romfh_t;

typedef struct _rom_dir_t
{
    aos_dir_t dir;
    char *dir_start_addr;
    char *dir_end_addr;
    char *dir_cur_addr;
    aos_dirent_t cur_dirent;
} romfs_dir_t;

static char *romfs_root = NULL;         /* The mount point of the physical addr */
#ifdef ROMFS_STATIC_ROOTADDR
#else
static bl_mtd_handle_t handle_romfs;
#endif

static int is_path_ch(char ch)
{
    if (((ch >= 'a') && (ch <= 'z')) ||
            ((ch >= 'A') && (ch <= 'Z')) ||
            ((ch >= '0') && (ch <= '9')) ||
            (ch == '/') ||
            (ch == '.') ||
            (ch == '_') ||
            (ch == '-')
            ) {
        return 1;
    }
    return 0;
}

static int filter_format(const char *path, uint32_t size)
{
    int res;
    int i;
    int i_old;

    /* sure mountpoint */
    res = strncmp(path, ROMFS_MOUNTPOINT, strlen(ROMFS_MOUNTPOINT));
    if (res) {
        log_error("format is error.\r\n");
        return -1;
    }

    /* sure '/' format, sure every ch */
    for (i = 0; i < size; i++) {
        /* sure every ch */
        if (0 == is_path_ch(path[i])) {
            log_error("is_path_ch. i = %d\r\n", i);
            return -2;
        }

        /* sure '/' */
        if ('/' != path[i]) {
            continue;
        }
        if (i != 0) {
            if(i == i_old) {
                log_error("format error.\r\n");
                return -3;
            } else {
                i_old = i;
            }
        } else {
            i_old = i;
        }
    }

    return 0;
}

static int romfs_mount(void)
{
#ifdef ROMFS_STATIC_ROOTADDR
    romfs_root = (char *)ROMFS_STATIC_ROOTADDR;
#else
    int ret;
    bl_mtd_info_t info;

    if ((ret = bl_mtd_open(BL_MTD_PARTITION_NAME_ROMFS, &handle_romfs, BL_MTD_OPEN_FLAG_BUSADDR))) {
        log_error("[EF] [PART] [XIP] error when get romfs partition %d\r\n", ret);
        return -1;
    }
    memset(&info, 0, sizeof(info));
    bl_mtd_info(handle_romfs, &info);
    
    if (0 == info.xip_addr) {
        log_error("romfs has no XIP-Addr\r\n");
        return -1;
    }

    if (0 != memcmp((char *)info.xip_addr, "-rom1fs-", strlen("-rom1fs-"))) {
        log_error("romfs magic is NOT correct\r\n");
        return -1;
    }

    romfs_root = (char *)info.xip_addr;
    ROMFS_DUBUG("xip addr = %p\r\n", romfs_root);
    log_buf(romfs_root, 64);
#endif

    return 0;
}

static int dirent_type(void *addr)
{
    if (0 == ((U32HTONL(*((uint32_t *)addr))) & 0x00000007)) {
        return ROMFH_HRD;
    } else if (2 == ((U32HTONL(*((uint32_t *)addr))) & 0x00000007)) {
        return ROMFH_REG;
    } else if (1 == ((U32HTONL(*((uint32_t *)addr))) & 0x00000007)) {
        return ROMFH_DIR;
    }

    return ROMFH_UNKNOW;
}

static uint32_t romfs_endaddr(void)
{
    return ((uint32_t)romfs_root + U32HTONL(*((uint32_t *)romfs_root + 2)));
}

static uint32_t dirent_hardfh(void *addr)
{
    return U32HTONL(*((uint32_t *)addr)) & 0xFFFFFFF0;
}

static uint32_t dirent_childaddr(void *addr)
{
    return U32HTONL(*((uint32_t *)addr + 1)) & 0xFFFFFFF0;
}

static uint32_t dirent_size(void *addr)
{
    return U32HTONL(*((uint32_t *)addr + 2));
}

static int file_info(char *path, char **p_addr_start_input, char **p_addr_end_input)
{
    char *addr_start = *p_addr_start_input;
    char *addr_end = *p_addr_end_input;
    ROMFS_ASSERT(path && addr_start_input && addr_end_input);

    ROMFS_DUBUG("file info path = %s\r\n", path);

    /* check arg */
    if (ROMFS_MAX_NAME_LEN < strlen(path)) {
        return -1;
    }

    /* /romfs */
    ROMFS_DUBUG("addr_start = %p\r\n", addr_start);
    if (addr_start == romfs_root) {
        /* point first dot file*/
        addr_start = (char *)(romfs_root + ALIGNUP16(strlen(romfs_root + HEAD_MAGIC_LEN) + 1) + HEAD_MAGIC_LEN);
    }

    ROMFS_DUBUG("addr_start = %p, addr_end = %p, path = %s\r\n", addr_start, addr_end, path);
    while (1) {
        if (ROMFH_DIR == dirent_type(addr_start)) {
            if (0 == memcmp(path, addr_start + 16, strlen(path))) {
                if (addr_start[16 + strlen(path)] == 0) {
                    if (0 == dirent_hardfh(addr_start)) {
                        break;// the dir is the last dirent
                    }
                    addr_end = romfs_root + dirent_hardfh(addr_start);
                    ROMFS_DUBUG("update addr_end = %p\r\n", addr_end);
                    break;
                }
            }
        } else if (ROMFH_REG == dirent_type(addr_start)) {
            if (0 == memcmp(path, addr_start + 16, strlen(path))) {
                if (addr_start[16 + strlen(path)] == 0) {
                    addr_end = romfs_root + dirent_hardfh(addr_start);
                    break;
                }
            }
        } else if (ROMFH_HRD != dirent_type(addr_start)) {
            log_error("addr_start = %p, dirent_type(addr_start) = %d\r\n", addr_start, dirent_type(addr_start));
            log_buf(addr_start, 8);
            log_error("unknow the dirent_type.\r\n");
            return -1;
        }

        ROMFS_DUBUG("addr_start = %p, off = 0x%08lx\r\n", addr_start, dirent_hardfh(addr_start));
        addr_start = romfs_root + dirent_hardfh(addr_start);
        if (addr_start >= addr_end) {
            log_warn("start >= end, not found path = %s, addr_start = %p, addr_end = %p\r\n", path, addr_start, addr_end);
            return -1;
        }
    }

    ROMFS_DUBUG("update addr_start = %p, addr_end = %p\r\n", addr_start, addr_end);
    /* update out */
    *p_addr_start_input = addr_start;
    *p_addr_end_input = addr_end;
    return 0;
}

/*
 * input : path
 * output: p_addr_start_input, p_addr_end_input
 * return: 0 success, other error
 */
uint32_t dirent_file(char *path, void **p_addr_start_input, void **p_addr_end_input)
{
    char *addr_start;
    char *addr_end;

    char *p_name = NULL;
    char name[ROMFS_MAX_NAME_LEN + 1];
    char *p_ret = NULL;
    char need_enter_child = 0;

    ROMFS_ASSERT(path && addr_start_input && addr_end_input);

    /* check arg */
    if (strlen(path) < strlen(ROMFS_MOUNTPOINT)) {
       return -1;
    }

    ROMFS_DUBUG("dirent_file path = %s\r\n", path);

    /* rm root_mountpoint and'/', /romfs/ */
    if (0 != memcmp(path, ROMFS_MOUNTPOINT, strlen(ROMFS_MOUNTPOINT))) {
        log_error("not support path.\r\n");
        return -1;
    }
    p_name = path + strlen(ROMFS_MOUNTPOINT);
    if ((*p_name != '/') && (*p_name != '\0')) {
        log_error("not support path.\r\n");
        return -1;
    }
    if (*p_name == '/') {
        p_name += 1;
    }

    /* search every one */
    addr_start = romfs_root;
    addr_end = (char *)romfs_endaddr();
    ROMFS_DUBUG("romfs start_addr:%p, end_addr:%p, p_name = %s\r\n", addr_start, addr_end, p_name);

    while (1) {
        if (0 == *p_name) {
            break;
        }
        p_ret = strchr(p_name, '/');

        if (1 == need_enter_child) {
            if (addr_start == (romfs_root + dirent_childaddr(addr_start))) {
                return -2;
            }
            addr_start = romfs_root + dirent_childaddr(addr_start);
            need_enter_child = 0;
        }

        if (NULL == p_ret) {
            /* last name, use it find, update addr_start_end and return */
            ROMFS_DUBUG("last name.\r\n");
            if (strlen(p_name) > ROMFS_MAX_NAME_LEN) {
                log_error("name too long!\r\n");
                return -1;
            }
            if (0 != file_info(p_name, (char **)&addr_start, (char **)&addr_end)) {
                log_warn("file info error, p_name = %s, addr_start = %p, addr_end = %p\r\n", p_name, addr_start, addr_end);
                return -1;
            }

            break;
        } else {
            memset(name, 0, sizeof(name));
            memcpy(name, p_name, (p_ret - p_name));
            ROMFS_DUBUG("mid name.\r\n");
            /* mid name, use it find, update addr_start_end and continue */
            if (0 != file_info(name, (char **)&addr_start, (char **)&addr_end)) {
                log_error("file info error.\r\n");
                return -1;
            }

            need_enter_child = 1;
            p_name = p_ret + 1;
            continue;
        }
    }

    ROMFS_DUBUG("dirent_file start = %p, end = %p\r\n", addr_start, addr_end);
    /* update out arg, and return */
    *p_addr_start_input = addr_start;
    *p_addr_end_input = addr_end;

    return 0;
}

static int romfs_open(file_t *fp, const char *path, int flags)
{
    char *start_addr;
    char *end_addr;

    ROMFS_DUBUG("romfs open.\r\n");

    /* sure romfs_root is valid */
    if (romfs_root == NULL) {
        log_error("romfs_root is null.\r\n");
        return -1;
    }

    /* sure format is valid */
    if (0 != filter_format(path, strlen(path))) {
        log_error("path format is error.\r\n");
        return -1;
    }

    /* jump to the back of volume name, get addr_max */
    if (0 != dirent_file((char *)path, (void **)&start_addr, (void **)&end_addr)) {
        return -2;
    }

    fp->f_arg = start_addr;
    fp->offset = 0;

    return 0;
}

static int romfs_close(file_t *fp)
{
    ROMFS_DUBUG("romfs close.\r\n");
    /* update file_t *fp */
    fp->f_arg = NULL;
    fp->offset = 0;
    return -1;
}

static ssize_t romfs_read(file_t *fp, char *buf, size_t length)
{
    char *payload_buf;
    uint32_t payload_size;
    int len;

    /* init payload_buf and payload_size */
    payload_buf  = ((char *)fp->f_arg) + ALIGNUP16(strlen(((char *)fp->f_arg) + 16) + 1) + 16;
    payload_size = dirent_size(fp->f_arg);

    /* check arg */
    if (fp->offset >= payload_size) {
        //log_warn("offset >= payload_size\r\n");
        return 0;
    }

    /* memcpy data */
    if ((fp->offset + length) < payload_size) {
        len = length;
        memcpy(buf, payload_buf + fp->offset, len);
        fp->offset += len;
    } else {
        len = payload_size - fp->offset;
        memcpy(buf, payload_buf + fp->offset, len);
        fp->offset = payload_size;
    }

    return len;
}

int romfs_ioctl(file_t *fp, int cmd, unsigned long arg)
{
    int ret = -1;
    romfs_filebuf_t *file_buf = (romfs_filebuf_t *)arg;

    if ((NULL == fp) || (NULL == file_buf)) {
        return -2;
    }
    switch(cmd) {
        case (IOCTL_ROMFS_GET_FILEBUF):
        {
            ROMFS_DUBUG("IOCTL_ROMFS_GET_FILEBUF.\r\n");
            file_buf->buf= ((char *)fp->f_arg) + ALIGNUP16(strlen(((char *)fp->f_arg) + 16) + 1) + 16;
            file_buf->bufsize = dirent_size(fp->f_arg);
            return 0;
        }
        break;
        default:
        {
            ret =  -3;
        }
    }

    return ret;
}

static off_t romfs_lseek(file_t *fp, off_t off, int whence)
{
    uint32_t payload_size;
    off_t tmp;

    if (NULL == fp) {
        return -1;
    }

    payload_size = dirent_size(fp->f_arg);

    if (whence == SEEK_SET) {
        if (off < 0) {
            printf("not support whence.\r\n");
            return -2;
        }
        tmp = off;
    } else if (whence == SEEK_END) {
        if (off > 0) {
            printf("not support whence.\r\n");
            return -3;
        }
        tmp = off + payload_size;
    } else if (whence == SEEK_CUR) {
        tmp = off + fp->offset;
    } else {
        printf("not support whence.\r\n");
        return -4;
    }

    if ((tmp < 0) || (tmp > payload_size)) {
        printf("not support whence.\r\n");
        return -5;
    }

    fp->offset = tmp;

    return fp->offset;
}

static int romfs_stat(file_t *fp, const char *path, struct stat *st)
{
    char *start_addr = 0;
    char *end_addr = 0;
    int res;

    ROMFS_DUBUG("romfs_stat path = %s\r\n", path);
    res = dirent_file((char *)path, (void **)&start_addr, (void **)&end_addr);

    if (res != 0) {
        log_warn("dirent_file res = %d\r\n", res);
        return -1;
    }

    if (start_addr == romfs_root) {
        st->st_size = 0;
    } else {
        if (ROMFH_DIR == dirent_type(start_addr)) {
            st->st_size = 0;
            st->st_mode = S_IFDIR;
            ROMFS_DUBUG("st_size set 0");
        } else if (ROMFH_REG == dirent_type(start_addr)) {
            st->st_size = dirent_size(start_addr);
            ROMFS_DUBUG("st_size set %ld\r\n", st->st_size);
            st->st_mode = S_IFREG;
        } else {
            log_warn("dirent_type err.\r\n");
            return -2;
        }
    }

    return 0;
}

static aos_dir_t *romfs_opendir(file_t *fp, const char *path)
{
    romfs_dir_t *dp      = NULL;
    char *start_addr;
    char *end_addr;
    int res;

    ROMFS_DUBUG("path = %s\r\n", path);

    /* sure romfs_root is valid */
    if (romfs_root == NULL) {
        log_error("romfs_root is null.\r\n");
        return NULL;
    }

    dp = (romfs_dir_t *)aos_malloc(sizeof(romfs_dir_t) + ROMFS_MAX_NAME_LEN + 1);
    if (NULL == dp) {
        return NULL;
    }
    memset(dp, 0, sizeof(romfs_dir_t) + ROMFS_MAX_NAME_LEN + 1);

    res = dirent_file((char *)path, (void **)&start_addr, (void **)&end_addr);
    ROMFS_DUBUG("open dir path = %s, start = %p, end = %p\r\n", path, start_addr, end_addr);
    if (0 == res) {
        /* need add update dir_addr and current_addr */
        if (start_addr == romfs_root) {
            /* point first dot file*/
            dp->dir_start_addr = (char *)(romfs_root + ALIGNUP16(strlen(romfs_root + HEAD_MAGIC_LEN) + 1) + HEAD_MAGIC_LEN);
        } else {
            if (0 == dirent_childaddr(start_addr)) {
                return NULL;
            } else {
                dp->dir_start_addr = (char *)(romfs_root + dirent_childaddr(start_addr));
            }
        }
        dp->dir_end_addr = end_addr;
        dp->dir_cur_addr = NULL;
        return (aos_dir_t *)dp;
    }

    /* open err */
    aos_free(dp);
    return NULL;
}

static aos_dirent_t *romfs_readdir(file_t *fp, aos_dir_t *dir)
{
    romfs_dir_t    *dp = (romfs_dir_t *)dir;

    if (!dp) {
        return NULL;
    }

    while (1) {
        /* current is NULL */
        if (NULL == dp->dir_cur_addr) {
            dp->dir_cur_addr = dp->dir_start_addr;
        } else {
            if (dp->dir_cur_addr >= dp->dir_end_addr) {
                return NULL;
            } else {
                while (1) {
                    if ((dp->dir_cur_addr >= dp->dir_end_addr) || (dp->dir_cur_addr < dp->dir_start_addr)){
                        ROMFS_DUBUG("cur_addr = %p\r\n", dp->dir_cur_addr);
                        return NULL;
                    }
                    //log_error("cur_addr = %p\r\n", dp->dir_cur_addr);
                    if (0 == dirent_hardfh(dp->dir_cur_addr)) {
                        ROMFS_DUBUG("cur_addr = %p\r\n", dp->dir_cur_addr);
                        break;
                    }
                    ROMFS_DUBUG("cur_addr = %p\r\n", dp->dir_cur_addr);
                    if (NULL == dp->dir_cur_addr) {
                        return NULL;
                    }
                    if ((ROMFH_DIR == dirent_type(dp->dir_cur_addr)) ||
                        (ROMFH_REG == dirent_type(dp->dir_cur_addr))) {
                        ROMFS_DUBUG("cur_addr = %p\r\n", dp->dir_cur_addr);
                        break;
                    }
                    dp->dir_cur_addr = romfs_root + dirent_hardfh(dp->dir_cur_addr);
                }
            }
        }

        ROMFS_DUBUG("name = %s\r\n", (char *)(dp->dir_cur_addr + 16));
        strncpy(dp->cur_dirent.d_name, dp->dir_cur_addr + 16, ROMFS_MAX_NAME_LEN);
        dp->cur_dirent.d_name[ROMFS_MAX_NAME_LEN] = '\0';
        ROMFS_DUBUG("name = %s\r\n", dp->cur_dirent.d_name);

        if (0 == dirent_hardfh(dp->dir_cur_addr)) {
            dp->dir_cur_addr = dp->dir_end_addr;
        } else {
            dp->dir_cur_addr = romfs_root + dirent_hardfh(dp->dir_cur_addr);
        }

        /* rm . and .. dir */
        if ( ((dp->cur_dirent.d_name[0] == '.') && (dp->cur_dirent.d_name[1] == '.') && (dp->cur_dirent.d_name[2] == '\0')) ||
             ((dp->cur_dirent.d_name[0] == '.') && (dp->cur_dirent.d_name[1] == '\0')) ) {
            ROMFS_DUBUG("......name = %s\r\n", dp->cur_dirent.d_name);
            continue;
        } else {
            break;
        }
    }

    return &(dp->cur_dirent);
}

static int romfs_closedir(file_t *fp, aos_dir_t *dir)
{
    romfs_dir_t *dp  = (romfs_dir_t *)dir;

    if (!dp) {
        return -1;
    }

    aos_free(dp);
    return 0;
}

static const fs_ops_t romfs_ops = {
    .open      = &romfs_open,
    .close     = &romfs_close,
    .read      = &romfs_read,
    .write     = NULL,
    .access    = NULL,
    .lseek     = &romfs_lseek,
    .stat      = &romfs_stat,
    .unlink    = NULL,
    .opendir   = &romfs_opendir,
    .readdir   = &romfs_readdir,
    .closedir  = &romfs_closedir,
    .telldir   = NULL,
    .seekdir   = NULL,
    .ioctl     = &romfs_ioctl
};

int romfs_register(void)
{
    if (0 != romfs_mount()) {
        return -1;
    }
    return aos_register_fs(ROMFS_MOUNTPOINT, &romfs_ops, NULL);
}

void test1_romfs(void)
{
    int fd = -1;
    int len;
    char buf[50];
    romfs_filebuf_t filebuf;
    off_t length;

    memset(buf, 0, sizeof(buf));

    fd = aos_open("/romfs/child/aa.bin", 0);
    log_info("fd = %d\r\n", fd);
    if (fd < 0) {
        log_error("open error.\r\n");
        return;
    }

    len = aos_read(fd, buf, 1);
    log_info("case1:len = %d\r\n", len);
    log_buf(buf, 1);

    aos_lseek(fd, 1, SEEK_CUR);
    memset(buf, 0, sizeof(buf));
    len = aos_read(fd, buf, 1);
    log_info("case2:len = %d\r\n", len);
    log_buf(buf, 1);

    len = aos_read(fd, buf, sizeof(buf));
    log_info("case3:len = %d\r\n", len);
    log_buf(buf, sizeof(buf));

    aos_lseek(fd, 0, SEEK_SET);
    memset(buf, 0, sizeof(buf));
    len = aos_read(fd, buf, sizeof(buf));
    log_info("case4:len = %d\r\n", len);
    log_info("buf:\r\n");
    log_buf(buf, sizeof(buf));

    aos_ioctl(fd, IOCTL_ROMFS_GET_FILEBUF, (long unsigned int)&filebuf);
    log_info("filebuf.buf = %p\r\n", filebuf.buf);
    log_info("filebuf.bufsize = %lu\r\n", filebuf.bufsize);

    length = aos_lseek(fd, 0, SEEK_END);
    log_info("aos_lseek(end) = 0x%08lx\r\n", length);

    memset(buf, 0, sizeof(buf));
    len = aos_read(fd, buf, sizeof(buf));
    log_info("case5:len = %d\r\n", len);
    log_buf(buf, sizeof(buf));

    aos_close(fd);
}

void test2_romfs(void)
{
    int i;
    int fd = -1;
    char buffer[128];
    int read_len = 0;

    char *test[7] = {
        "/romfs/child_dir1/aa.bin",
        "/romfs/child_dir2/son_dir/EE.bin",
        "/romfs/child_dir2/bb.bin",
        "/romfs/child_dir3/cc.bin",
        "/romfs/11.bin",
        "/romfs/22.bin",
        "/romfs/33.bin"
    };

    for (i = 0; i < 7; i++) {
        log_warn("test[%d] %s \r\n", i, test[i]);

        /* open */
        fd = aos_open(test[i], 0);
        if (fd < 0) {
            log_error("test[%d] open %s error, fd = %d\r\n", i, test[i], fd);
            continue;
        }
        log_info("test[%d] open fd = %d\r\n", i, fd);

        /* read */
        while ((read_len = aos_read(fd, buffer, sizeof(buffer))) > 0) {
            log_info("test[%d] buf:\r\n", i);
            log_buf(buffer, read_len);
        }

        aos_close(fd);
    }
}

void test3_romfs(void)
{
    char *start_addr = 0;
    char *end_addr = 0;
    int res;
    struct stat st;

    log_warn("test /romfs .\r\n");
    log_info("addr = %ld\r\n", dirent_file("/romfs", (void **)&start_addr, (void **)&end_addr));
    log_info("test start_addr:%p, end_addr:%p\r\n", start_addr, end_addr);
    res = romfs_stat(NULL, "/romfs", &st);
    if (res == 0) {
        log_info("st.st_size = %ld\r\n", st.st_size);
    }

    log_warn("test /romfs/ .\r\n");
    log_info("addr = %ld\r\n", dirent_file("/romfs/", (void **)&start_addr, (void **)&end_addr));
    log_info("test start_addr:%p, end_addr:%p\r\n", start_addr, end_addr);
    res = romfs_stat(NULL, "/romfs", &st);
    if (res == 0) {
        log_info("st.st_size = %ld\r\n", st.st_size);
    }

    log_warn("test /romf .\r\n");
    log_info("addr = %ld\r\n", dirent_file("/romf", (void **)&start_addr, (void **)&end_addr));
    log_info("test start_addr:%p, end_addr:%p\r\n", start_addr, end_addr);
    res = romfs_stat(NULL, "/romf", &st);
    if (res == 0) {
        log_info("st.st_size = %ld\r\n", st.st_size);
    }

    /* child_dir1 */
    log_warn("test /romfs/child_dir1/\r\n");
    log_info("addr = %ld\r\n", dirent_file("/romfs/child_dir1/", (void **)&start_addr, (void **)&end_addr));
    log_info("test start_addr:%p, end_addr:%p\r\n", start_addr, end_addr);
    res = romfs_stat(NULL, "/romfs/child_dir1/", &st);
    if (res == 0) {
        log_info("st.st_size = %ld\r\n", st.st_size);
    }

    log_warn("test /romfs/child_dir1/aa.bin\r\n");
    log_info("addr = %ld\r\n", dirent_file("/romfs/child_dir1/aa.bin", (void **)&start_addr, (void **)&end_addr));
    log_info("test start_addr:%p, end_addr:%p\r\n", start_addr, end_addr);
    res = romfs_stat(NULL, "/romfs/child_dir1/aa.bin", &st);
    if (res == 0) {
        log_info("st.st_size = %ld\r\n", st.st_size);
    }

    /* child_dir2 */
    log_warn("test /romfs/child_dir2\r\n");
    log_info("addr = %ld\r\n", dirent_file("/romfs/child_dir2", (void **)&start_addr, (void **)&end_addr));
    log_info("test start_addr:%p, end_addr:%p\r\n", start_addr, end_addr);
    res = romfs_stat(NULL, "/romfs/child_dir2", &st);
    if (res == 0) {
        log_info("st.st_size = %ld\r\n", st.st_size);
    }

    log_warn("test /romfs/child_dir2/bb.bin\r\n");
    log_info("addr = %ld\r\n", dirent_file("/romfs/child_dir2/bb.bin", (void **)&start_addr, (void **)&end_addr));
    log_info("test start_addr:%p, end_addr:%p\r\n", start_addr, end_addr);
    res = romfs_stat(NULL, "/romfs/child_dir2/bb.bin", &st);
    if (res == 0) {
        log_info("st.st_size = %ld\r\n", st.st_size);
    }

    log_warn("test /romfs/child_dir2/son_dir\r\n");
    log_info("addr = %ld\r\n", dirent_file("/romfs/child_dir2/son_dir", (void **)&start_addr, (void **)&end_addr));
    log_info("test start_addr:%p, end_addr:%p\r\n", start_addr, end_addr);
    res = romfs_stat(NULL, "/romfs/child_dir2/son_dir", &st);
    if (res == 0) {
        log_info("st.st_size = %ld\r\n", st.st_size);
    }

    log_warn("test /romfs/child_dir2/son_dir/EE.bin\r\n");
    log_info("addr = %ld\r\n", dirent_file("/romfs/child_dir2/son_dir/EE.bin", (void **)&start_addr, (void **)&end_addr));
    log_info("test start_addr:%p, end_addr:%p\r\n", start_addr, end_addr);
    res = romfs_stat(NULL, "/romfs/child_dir2/son_dir/EE.bin", &st);
    if (res == 0) {
        log_info("st.st_size = %ld\r\n", st.st_size);
    }
}
