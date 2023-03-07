#include <stdio.h>
#include <cli.h>
#include <bl_romfs.h>
#include "demo.h"
#include <vfs.h>
#include <blog.h>
#include <fs/vfs_romfs.h>
#include <fastlz.h>
#include <fastlz_wrapper.h>

void log_step(const char *step[2])
{
    printf("%s   %s\r\r\n", step[0], step[1]);
}

void cmd_romfs(char *buf, int len, int argc, char **argv)
{
    test1_romfs();
}

void cmd_fastlz(char *buf, int len, int argc, char **argv)
{
    int fd = -1;
    int err;
    romfs_filebuf_t filebuf;
    char *outbuf = NULL;
    char *buf2 = NULL;
    size_t outlen;
    fd = aos_open("/romfs/test1.txt", 0);
    blog_info("fd = %d\r\n", fd);
    if (fd < 0) {
        blog_error("open error.\r\n");
        return;
    }
    aos_ioctl(fd, IOCTL_ROMFS_GET_FILEBUF, (long unsigned int)&filebuf);
    blog_info("filebuf.buf = %p\r\r\n", filebuf.buf);
    blog_info("filebuf.bufsize = %lu\r\n", filebuf.bufsize);
    outbuf = pvPortMalloc(filebuf.bufsize + FASTLZ_BUFFER_PADDING(filebuf.bufsize));
    buf2  = pvPortMalloc(filebuf.bufsize);
    if (!outbuf || !buf2) {
        blog_error("malloc failed\r\n");
        return;
    }
    outlen = fastlz_compress(filebuf.buf, filebuf.bufsize, outbuf);
    blog_info("after compress len %d\r\n", outlen);
    printf("************after compress ***********\r\n");
    for(int i = 0; i < outlen; i++) {
        printf("%c", outbuf[i]);
    }
    printf("\r\n");
    err = fastlz_decompress(outbuf, outlen, buf2, filebuf.bufsize);
    if (err == 0) {
        printf("uncompression of failed: %d\r\n", err);
        return;
    }
    printf("************after decompress ***********\r\n");
    for (int i = 0; i <filebuf.bufsize; i++) {
        printf("%c", buf2[i]);
    }
    if (memcmp(buf2, filebuf.buf, filebuf.bufsize)) {
        printf("final comparision of failed \r\n");
    } else {
        printf("\r\n %s test OK!\r\n");
    }

}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"romfs", "romfs test", cmd_romfs}, 
    {"fastlz", "zip test", cmd_fastlz}, 
};

int romfs_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}

