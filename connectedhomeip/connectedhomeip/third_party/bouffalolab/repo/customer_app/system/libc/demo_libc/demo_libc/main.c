/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <FreeRTOS.h>
#include <task.h>

#include <aos/kernel.h>
#include <aos/yloop.h>
#include <cli.h>
#include <blog.h>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

static void merror(const char *msg)
{
    printf("Error: %s\n\r", msg);
}

static const char *format[] = {
  "%",
  "%0.",
  "%.0",
  "%+0.",
  "%+.0",
  "%.5",
  "%+.5",
  "%2.5",
  "%22.5",
  "%022.5",
  "%#022.5",
  "%-#022.5",
  "%+#022.5",
  "%-22.5",
  "%+22.5",
  "%--22.5",
  "%++22.5",
  "%+-22.5",
  "%-+22.5",
  "%-#022.5",
  "%-#22.5",
  "%-2.22",
  "%+2.22",
  "%-#02.22",
  "%-#2.22",
  "%-1.5",
  "%1.5",
  "%-#01.5",
  "%-#1.5",
  "%-#.5",
  "%-#1.",
  "%-#.",
  NULL
};

static void cmd_cli(char *buf, int len, int argc, char **argv)
{
    printf("hello world\r\r\n");
}

static void intchk (const char *fmt)
{
  (void) printf("%15s :, \"", fmt);
  (void) printf(fmt, 0);
  (void) printf("\", \"");
  (void) printf(fmt, 123);
  (void) printf("\", \"");
  (void) printf(fmt, -18);
  (void) printf("\"\r\n");
}

static void fltchk (const char *fmt)
{
  (void) printf("%15s :, \"", fmt);
  (void) printf(fmt, 0.0);
  (void) printf("\", \"");
  (void) printf(fmt, 123.0001);
  (void) printf("\", \"");
  (void) printf(fmt, -18.0002301);
  (void) printf("\"\r\n");
}

static void cmd_printf_test(char *pbuf, int len, int argc, char **argv)
{
    char buf[256];
    int i;

    printf("%s\r\n\r\n", "# vim:syntax=off:");
    
    /* integers */
    for (i = 0; format[i]; i++)
    {
        strcpy(buf, format[i]);
        strcat(buf, "d");
        intchk(buf);
    }

    /* floats */
    for (i = 0; format[i]; i++)
    {
        strcpy(buf, format[i]);
        strcat(buf, "f");
        fltchk(buf);
    }

    /* hexa */
    for (i = 0; format[i]; i++)
    {
        strcpy(buf, format[i]);
        strcat(buf, "x");
        intchk(buf);
    }

    printf("#%.4x %4x#\r\n", 4, 88);
    printf("#%4x#\r\n", 4);
    printf("#%#22.8x#\r\n", 1234567);

    printf("#%+2i#\r\n", 18);
    printf("#%i#\r\n", 18);
    printf("#%llu#\r\n", 4294967297ULL);
    printf("#%#x#\r\n", 44444);
    printf("#%-8i#\r\n", 33);
    printf("#%i#\r\n", 18);
    printf("#%d#\r\n", 18);
    printf("#%u#\r\n", 18);
    printf("#%lu#\r\n", 18);
    printf("#%li#\r\n", 18);
    printf("#%-+#06d#\r\n", -123);
    printf("#%-+#6d#\r\n", -123);
    printf("#%+#06d#\r\n", -123);
    printf("#%06d#\r\n", -123);
    printf("#%+15s#\r\n", "ABCDEF");
    /* from ncurses make_keys */
    printf("{ %4d, %-*.*s },\t/* %s */\r\n", 139, 16, 16, "KEY_A1", "key_a1");
    printf("{ %4d, %-*.*s },\t/* %s */\r\n", 139, 16, 2, "KEY_A1", "key_a1");
    printf("{ %4d, %-*.*s },\t/* %s */\r\n", 139, 2, 16, "KEY_A1", "key_a1");
    printf("{ %4d, %-*.*s },\t/* %s */\r\n", 139, 16, 0, "KEY_A1", "key_a1");
    printf("{ %4d, %-*.*s },\t/* %s */\r\n", 139, 0, 16, "KEY_A1", "key_a1");
    printf("{ %4d, %-*.*s },\t/* %s */\r\n", 139, 0, 0, "KEY_A1", "key_a1");
    printf("{ %4d, %*.*s },\t/* %s */\r\n", 139, 16, 16, "KEY_A1", "key_a1");
    printf("{ %4d, %*.*s },\t/* %s */\r\n", 139, 16, 2, "KEY_A1", "key_a1");
    printf("{ %4d, %*.*s },\t/* %s */\r\n", 139, 2, 16, "KEY_A1", "key_a1");
    printf("{ %4d, %*.*s },\t/* %s */\r\n", 139, 16, 0, "KEY_A1", "key_a1");
    printf("{ %4d, %*.*s },\t/* %s */\r\n", 139, 0, 16, "KEY_A1", "key_a1");
    printf("{ %4d, %*.*s },\t/* %s */\r\n", 139, 0, 0, "KEY_A1", "key_a1");
    printf("%*.*f\r\n", 0, 16, 0.0);
    printf("%*.*f\r\n", 16, 16, 0.0);
    printf("%*.*f\r\n", 2, 2, -0.0);
    printf("%*.*f\r\n", 20, 0, -123.123);
    printf("%*.*f\r\n", 10, 0, +123.123);

    i = printf("\"%s\"\r\n", "A");
    printf("%i\r\n", i);
    /* from glibc's tst-printf.c */

    {
        char buf[20];
        char buf2[512];
        int i;

        printf("snprintf (\"%%30s\", \"foo\") == %d, \"%.*s\"\r\n",
               snprintf(buf, sizeof(buf), "%30s", "foo"), (int)sizeof(buf),
               buf);
        memset(buf2, 0, sizeof(buf));
        i = snprintf(buf2, 256, "%.9999u", 10);
        printf("%i %i\r\n", i, strlen(buf2));

        printf("snprintf (\"%%.999999u\", 10) == %d\r\n",
               snprintf(buf2, sizeof(buf2), "%.999999u", 10));
    }
}

static void cmd_abort(char *buf, int len, int argc, char **argv)
{
    printf("abort!!\r\r\n");
    configASSERT(0);
}

static void cmd_mem(char *buf, int len, int argc, char **argv)
{
    void *p;

    p = malloc(10);
    if (p == NULL)
        merror("malloc (10) failed.");

    /* realloc (p, 0) == free (p).  */
    p = realloc(p, 0);
    if (p != NULL)
        merror("realloc (p, 0) failed.");
}

static void cmd_mkdir(char *buf, int len, int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: mkdir [OPTION] DIRECTORY\n");
        printf("Create the DIRECTORY, if they do not already exist.\n");
    }

    mkdir(argv[1], 0);
}

static void cmd_write(char *buf, int len, int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: write DATA to /dev/ttyS0\r\n");
        return;
    }

    int fd = open("/dev/ttyS0", 0);
    if (fd)
    {
        write(fd, argv[1], strlen(argv[1]));

        fsync(fd);

        close(fd);
    }
}

static void cmd_random(char *buf, int len, int argc, char **argv)
{
    long result = 0;

    result = random();

    printf("\r\n**********random test rand[%08x]**************\r\n", result);
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"test", "cli test", cmd_cli},
    {"mem_test", "test mem control", cmd_mem},
    {"printf_test", "libc printf test", cmd_printf_test},
    {"abort_test", "libc abort test", cmd_abort},
    {"mkdir", "libc mkdir", cmd_mkdir}, /* romfs don't support */
    {"write", "libc write to /dev/ttyS0", cmd_write},
    {"random", "libc random", cmd_random},
};


void vAssertCalled(void)
{
    taskDISABLE_INTERRUPTS();

    abort();
}

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo libc!\r\n");
    return 0;
}
