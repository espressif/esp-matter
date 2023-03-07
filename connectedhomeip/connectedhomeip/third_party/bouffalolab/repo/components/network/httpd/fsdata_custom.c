
/**
 * @file
 * HTTPD custom file system example
 *
 * This file demonstrates how to add support for an external file system to httpd.
 * It provides access to the specified root directory and uses stdio.h file functions
 * to read files.
 *
 * ATTENTION: This implementation is *not* secure: no checks are added to ensure
 * files are only read below the specified root directory!
 */
 
 /*
 * Copyright (c) 2017 Simon Goldschmidt
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Simon Goldschmidt <goldsimon@gmx.de>
 *
 */
#include "httpd_cfg.h"

#include "lwip/opt.h"
#include "fsdata_custom.h"

#include "fs.h"
#include "lwip/def.h"
#include "lwip/mem.h"

#include <vfs.h>
#include <vfs_inode.h>
#include "fs/vfs_romfs.h"

#include <stdio.h>
#include <string.h>
#include <utils_log.h>
/** define LWIP_HTTPD_EXAMPLE_CUSTOMFILES to 1 to enable this file system */
#ifndef LWIP_HTTPD_EXAMPLE_CUSTOMFILES
#define LWIP_HTTPD_EXAMPLE_CUSTOMFILES 1
#endif

/** define LWIP_HTTPD_EXAMPLE_CUSTOMFILES_LIMIT_READ to the number of bytes
 * to read to emulate limited transfer buffers and don't read whole files in
 * one chunk.
 * WARNING: lowering this slows down the connection!
 */
#ifndef LWIP_HTTPD_EXAMPLE_CUSTOMFILES_LIMIT_READ
#define LWIP_HTTPD_EXAMPLE_CUSTOMFILES_LIMIT_READ 0
#endif

#if LWIP_HTTPD_EXAMPLE_CUSTOMFILES

#if !LWIP_HTTPD_CUSTOM_FILES
#error This needs LWIP_HTTPD_CUSTOM_FILES
#endif
#if !LWIP_HTTPD_DYNAMIC_FILE_READ
#error This wants to demonstrate read-after-open, so LWIP_HTTPD_DYNAMIC_FILE_READ is required!
#endif
#ifndef LWIP_HTTPD_FSROOT_DIR
#error This needs LWIP_HTTPD_FSROOT_DIR
#endif

int fs_open_custom(struct fs_file *file, const char *name)
{
    char full_filename[256];
    int fd;
    romfs_filebuf_t file_buf;

    snprintf(full_filename, 255, "%s%s", LWIP_HTTPD_FSROOT_DIR, name);
    log_info("name = %s\r\n", name);
    full_filename[255] = 0;

    fd = aos_open(full_filename, 0);
    if (fd >= 0) {
        memset(file, 0, sizeof(struct fs_file));
        aos_ioctl(fd, IOCTL_ROMFS_GET_FILEBUF, (unsigned long)&file_buf); 
        file->len = file_buf.bufsize;
        file->flags = FS_FILE_FLAGS_HEADER_PERSISTENT;
        file->pextension = (fs_file_extension *)fd;
        return 1;
    }
    return 0;
}

void fs_close_custom(struct fs_file *file)
{
    int fd;

    if (file && file->pextension) {
        fd = (int)file->pextension;
        aos_close(fd);
    }
}

int fs_read_custom(struct fs_file *file, char *buffer, int count)
{
  int fd = (int)file->pextension;
  int len;
  int read_count = count;
  LWIP_ASSERT("data not set", fd >= 0);

#if LWIP_HTTPD_EXAMPLE_CUSTOMFILES_LIMIT_READ
  read_count = LWIP_MIN(read_count, LWIP_HTTPD_EXAMPLE_CUSTOMFILES_LIMIT_READ);
#endif

  len = aos_read(fd, buffer, read_count);

  file->index += len;

  /* Return FS_READ_EOF if all bytes have been read */
  return len;
}

#endif /* LWIP_HTTPD_EXAMPLE_CUSTOMFILES */
