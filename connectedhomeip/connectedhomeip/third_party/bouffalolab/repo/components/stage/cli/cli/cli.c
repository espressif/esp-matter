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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cli.h>
#include <stdio.h>
#include <aos/kernel.h>
#include <hal/soc/soc.h>
#include <hal/soc/uart.h>
#include <vfs.h>
#include <vfs_inode.h>
#include <fs/vfs_romfs.h>
#include <FreeRTOS.h>
#include <task.h>
#include <bl_chip.h>
#include <hal_sys.h>
#include <utils_hexdump.h>

#include "cli_internal.h"


#define RET_CHAR '\n'
#define END_CHAR '\r'
#define PROMPT   "# "
#define EXIT_MSG "exit"

#define csp_printf(...) printf(__VA_ARGS__)
#define fflush(...) do {} while (0)

#if (AOS_CLI_MINI_SIZE > 0)
char *cli_mini_support_cmds[] = { "netmgr", "help", "sysver",
                                  "reboot", "time", "ota" };
#endif

static struct cli_st *cli     = NULL;
static int volatile cliexit   = 0;
char              esc_tag[64] = { 0 };
static uint8_t    esc_tag_len = 0;
static aos_task_t cli_task;
static int fd_console;

extern void log_cli_init(void);

#ifdef CONFIG_AOS_CLI_BOARD
extern int board_cli_init(void);
#endif

#ifdef OSAL_RHINO
extern uint32_t krhino_version_get(void);
#endif

int cli_getchar(char *inbuf);
int cli_putstr(char *msg);

static const struct cli_command *cli_command_get(int idx, int *is_static_cmd)
{
    if (!(idx >= 0 && idx < cli->num_static_cmds + MAX_DYNAMIC_COMMANDS)) {
        return (struct cli_command *)-1;
    }
    if (idx < cli->num_static_cmds) {
        if (NULL != is_static_cmd) {
            *is_static_cmd = 1;
        }
        return &cli->static_cmds[idx];
    }
    if (NULL != is_static_cmd) {
        *is_static_cmd = 0;
    }
    return cli->dynamic_cmds[idx - cli->num_static_cmds];
}

/* Find the command 'name' in the cli commands table.
 * If len is 0 then full match will be performed else upto len bytes.
 * Returns: a pointer to the corresponding cli_command struct or NULL.
 */
static const struct cli_command *lookup_command(char *name, int len)
{
    int i = 0;
    int n = 0;

    while (i < cli->num_static_cmds + MAX_DYNAMIC_COMMANDS && n < cli->num_commands) {
        const struct cli_command *cmd = cli_command_get(i, NULL);
        if (cmd->name == NULL) {
            i++;
            continue;
        }
        /* See if partial or full match is expected */
        if (len != 0) {
            if (!strncmp(cmd->name, name, len)) {
                return cmd;
            }
        } else {
            if (!strcmp(cmd->name, name)) {
                return cmd;
            }
        }

        i++;
        n++;
    }

    return NULL;
}


/*proc one cli cmd and to run the according funtion
* Returns: 0 on success:
           1 fail
*/
int proc_onecmd(int argc, char *argv[])
{
    int                       i = 0;
    const char               *p;
    const struct cli_command *command = NULL;

    if (argc < 1) {
        return 0;
    }

    if (!cli->echo_disabled) {
        csp_printf("\r\n");
        fflush(stdout);
    }

    /*
     * Some comamands can allow extensions like foo.a, foo.b and hence
     * compare commands before first dot.
     */
    i = ((p = strchr(argv[0], '.')) == NULL) ? 0 : (p - argv[0]);

    command = lookup_command(argv[0], i);
    if (command == NULL) {
        return 1;
    }

    cli->outbuf = aos_malloc(OUTBUF_SIZE);
    if (NULL == cli->outbuf) {
        aos_cli_printf("Error! cli alloc mem fail!\r\n");
        return 1;
    }
    memset(cli->outbuf, 0, OUTBUF_SIZE);

    command->function(cli->outbuf, OUTBUF_SIZE, argc, argv);
    aos_cli_printf("%s", cli->outbuf);

    aos_free(cli->outbuf);
    cli->outbuf = NULL;
    return 0;
}


/* Parse input line and locate arguments (if any), keeping count of the number
 * of arguments and their locations.  Look up and call the corresponding cli
 * function if one is found and pass it the argv array.
 *
 * Returns: 0 on success: the input line contained at least a function name and
 *          that function exists and was called.
 *          1 on lookup failure: there is no corresponding function for the
 *          input line.
 *          2 on invalid syntax: the arguments list couldn't be parsed
 */
static int handle_input(char *inbuf)
{
    struct
    {
        unsigned inArg : 1;
        unsigned inQuote : 1;
        unsigned done : 1;
    } stat;
    static char *argvall[CLI_MAX_ONCECMD_NUM][CLI_MAX_ARG_NUM];
    int          argcall[CLI_MAX_ONCECMD_NUM] = { 0 };
    /*
    static char *argv[CLI_MAX_ONCECMD_NUM][CLI_MAX_ARG_NUM];
    int argc = 0;*/
    int  cmdnum = 0;
    int *pargc  = &argcall[0];
    int  i      = 0;
    int  ret    = 0;

    memset((void *)&argvall, 0, sizeof(argvall));
    memset((void *)&argcall, 0, sizeof(argcall));
    memset(&stat, 0, sizeof(stat));

    do {
        switch (inbuf[i]) {
            case '\0':
                if (stat.inQuote) {
                    return 2;
                }
                stat.done = 1;
                break;

            case '"':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i], strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (!stat.inQuote && stat.inArg) {
                    break;
                }
                if (stat.inQuote && !stat.inArg) {
                    return 2;
                }

                if (!stat.inQuote && !stat.inArg) {
                    stat.inArg   = 1;
                    stat.inQuote = 1;
                    (*pargc)++;
                    argvall[cmdnum][(*pargc) - 1] = &inbuf[i + 1];
                } else if (stat.inQuote && stat.inArg) {
                    stat.inArg   = 0;
                    stat.inQuote = 0;
                    inbuf[i]     = '\0';
                }
                break;

            case ' ':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i], strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (!stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    inbuf[i]   = '\0';
                }
                break;

            case ';':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i], strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (stat.inQuote) {
                    return 2;
                }
                if (!stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    inbuf[i]   = '\0';

                    if (*pargc) {
                        if (++cmdnum < CLI_MAX_ONCECMD_NUM) {
                            pargc = &argcall[cmdnum];
                        }
                    }
                }

                break;

            default:
                if (!stat.inArg) {
                    stat.inArg = 1;
                    (*pargc)++;
                    argvall[cmdnum][(*pargc) - 1] = &inbuf[i];
                }
                break;
        }
    } while (!stat.done && ++i < INBUF_SIZE && cmdnum < CLI_MAX_ONCECMD_NUM &&
             (*pargc) < CLI_MAX_ARG_NUM);

    if (stat.inQuote) {
        return 2;
    }

    for (i = 0; i <= cmdnum && i < CLI_MAX_ONCECMD_NUM; i++) {
        ret |= proc_onecmd(argcall[i], argvall[i]);
    }

    return ret;
}

__attribute__((weak)) void *fhost_cmd_tab_complete(char *inbuf, unsigned int *bp, int cli_cmd, int *fhost_cmd)
{
    return NULL;
}

/* Perform basic tab-completion on the input buffer by string-matching the
 * current input line against the cli functions table.  The current input line
 * is assumed to be NULL-terminated.
 */
static void tab_complete(char *inbuf, unsigned int *bp)
{
    int         i, n, m, fhost_cmd = 0;
    const char *fm = NULL, *fhost_fm = NULL;

    aos_cli_printf("\r\n");

    /* show matching commands */
    for (i = 0, n = 0, m = 0; i < cli->num_static_cmds + MAX_DYNAMIC_COMMANDS
            && n < cli->num_commands; i++) {
        const struct cli_command *cmd = cli_command_get(i, NULL);
        if (cmd->name != NULL) {
            if (!strncmp(inbuf, cmd->name, *bp)) {
                m++;
                if (m == 1) {
                    fm = cmd->name;
                } else if (m == 2)
                    aos_cli_printf("%s %s ", fm, cmd->name);
                else
                    aos_cli_printf("%s ", cmd->name);
            }
            n++;
        }
    }

    fhost_fm = fhost_cmd_tab_complete(inbuf, bp, m, &fhost_cmd);

    /* there's only one match, so complete the line */
    if ((m == 1 && fm) && fhost_cmd == 0) {
        n = strlen(fm) - *bp;
        if (*bp + n < INBUF_SIZE) {
            memcpy(inbuf + *bp, fm + *bp, n);
            *bp += n;
            inbuf[(*bp)++] = ' ';
            inbuf[*bp]     = '\0';
        }
    } else if (m == 0 && (fhost_cmd == 1 && fhost_fm)) {
        n = strlen(fhost_fm) - *bp;
        if (*bp + n < INBUF_SIZE) {
            memcpy(inbuf + *bp, fhost_fm + *bp, n);
            *bp += n;
            inbuf[(*bp)++] = ' ';
            inbuf[*bp]     = '\0';
        }
    } else if (m == 1 && fhost_cmd != 0) {
        aos_cli_printf("%s ", fm);
    }

    if ((m + fhost_cmd) >= 2) {
        aos_cli_printf("\r\n");
    }

    /* just redraw input line */
    aos_cli_printf("%s%s", PROMPT, inbuf);
}


#if (AOS_CLI_MINI_SIZE <= 0)

static void cli_history_input(void)
{
    char *inbuf   = cli->inbuf;
    int   charnum = strlen(cli->inbuf) + 1;

    int  his_cur  = cli->his_cur;
    int  left_num = INBUF_SIZE - his_cur;
    char lastchar;
    int  tmp_idx;

    cli->his_idx = his_cur;

    if (left_num >= charnum) {
        tmp_idx  = his_cur + charnum - 1;
        lastchar = cli->history[tmp_idx];
        strncpy(&(cli->history[his_cur]), inbuf, charnum);

    } else {
        tmp_idx  = (his_cur + charnum - 1) % INBUF_SIZE;
        lastchar = cli->history[tmp_idx];
        strncpy(&(cli->history[his_cur]), inbuf, left_num);
        strncpy(&(cli->history[0]), inbuf + left_num, charnum - left_num);
    }
    tmp_idx      = (tmp_idx + 1) % INBUF_SIZE;
    cli->his_cur = tmp_idx;

    /*overwrite*/
    if ('\0' != lastchar) {

        while (cli->history[tmp_idx] != '\0') {
            cli->history[tmp_idx] = '\0';
            tmp_idx               = (tmp_idx + 1) % INBUF_SIZE;
        }
    }
}


static void cli_up_history(char *inaddr)
{
    int index;
    int lastindex = 0;

    lastindex = cli->his_idx;
    index     = (cli->his_idx - 1 + INBUF_SIZE) % INBUF_SIZE;

    while ((cli->history[index] == '\0') && (index != cli->his_idx)) {
        index = (index - 1 + INBUF_SIZE) % INBUF_SIZE;
    }
    if (index != cli->his_idx) {
        while (cli->history[index] != '\0') {
            index = (index - 1 + INBUF_SIZE) % INBUF_SIZE;
        }
        index = (index + 1) % INBUF_SIZE;
    }
    cli->his_idx = index;

    while (cli->history[lastindex] != '\0') {

        *inaddr++ = cli->history[lastindex];
        lastindex = (lastindex + 1) % INBUF_SIZE;
    }
    *inaddr = '\0';

    return;
}

static void cli_down_history(char *inaddr)
{
    int index;
    int lastindex = 0;

    lastindex = cli->his_idx;
    index     = cli->his_idx;

    while ((cli->history[index] != '\0')) {
        index = (index + 1) % INBUF_SIZE;
    }
    if (index != cli->his_idx) {
        while (cli->history[index] == '\0') {
            index = (index + 1) % INBUF_SIZE;
        }
    }
    cli->his_idx = index;

    while (cli->history[lastindex] != '\0') {
        *inaddr++ = cli->history[lastindex];
        lastindex = (lastindex + 1) % INBUF_SIZE;
    }

    *inaddr = '\0';

    return;
}
#endif

/* Get an input line.
 *
 * Returns: 1 if there is input, 0 if the line should be ignored.
 */
static int get_input(char *inbuf, unsigned int *bp, char *buffer_cb, int count)
{
    char c;
    int  pos = 0;
    static int8_t  esc = 0, key1 = -1, key2 = -1;
    if (inbuf == NULL) {
        aos_cli_printf("inbuf_null\r\n");
        return 0;
    }

    /*return data from buffer_cb or get data from cli_getchar*/
    while (1 == (buffer_cb ? ((pos < count) ? (c = buffer_cb[pos], pos++, 1) : 0) : (cli_getchar(&c)))) {
        if (c == RET_CHAR || c == END_CHAR) { /* end of input line */
            inbuf[*bp] = '\0';
            *bp        = 0;
            return 1;
        }

        if (c == 0x1b) { /* escape sequence */
            esc  = 1;
            key1 = -1;
            key2 = -1;
            continue;
        }

        if (esc) {
            if (key1 < 0) {
                key1 = c;
                if (key1 != 0x5b) {
                    /* not '[' */
                    inbuf[(*bp)] = 0x1b;
                    (*bp)++;
                    inbuf[*bp] = key1;
                    (*bp)++;
                    if (!cli->echo_disabled) {
                        csp_printf("\x1b%c", key1);
                        fflush(stdout);
                    }
                    esc = 0; /* quit escape sequence */
                }
                continue;
            }

            if (key2 < 0) {
                key2 = c;
                if (key2 == 't') {
                    esc_tag[0]  = 0x1b;
                    esc_tag[1]  = key1;
                    esc_tag_len = 2;
                }
            }

            if (key2 != 0x41 && key2 != 0x42 && key2 != 't') {
                /*unsupported esc sequence*/
                inbuf[(*bp)] = 0x1b;
                (*bp)++;
                inbuf[*bp] = key1;
                (*bp)++;
                inbuf[*bp] = key2;
                (*bp)++;
                if (!cli->echo_disabled) {
                    csp_printf("\x1b%c%c", key1, key2);
                    fflush(stdout);
                }
                esc_tag[0]  = '\x0';
                esc_tag_len = 0;
                esc         = 0; /* quit escape sequence */
                continue;
            }

#if (AOS_CLI_MINI_SIZE > 0)
            if (key2 == 0x41 || key2 == 0x42) {
                csp_printf(
                  "\r\n" PROMPT
                  "Warning! mini cli mode do not support history cmds!");
            }

#else
            if (key2 == 0x41) { /* UP */
                cli_up_history(inbuf);
                csp_printf("\r\n" PROMPT "%s", inbuf);
                *bp         = strlen(inbuf);
                esc_tag[0]  = '\x0';
                esc_tag_len = 0;
                esc         = 0; /* quit escape sequence */
                continue;
            }

            if (key2 == 0x42) { /* DOWN */
                cli_down_history(inbuf);
                csp_printf("\r\n" PROMPT "%s", inbuf);
                *bp         = strlen(inbuf);
                esc_tag[0]  = '\x0';
                esc_tag_len = 0;
                esc         = 0; /* quit escape sequence */
                continue;
            }
#endif

            /* ESC_TAG */
            if (esc_tag_len >= sizeof(esc_tag)) {
                esc_tag[0]  = '\x0';
                esc_tag_len = 0;
                esc         = 0; /* quit escape sequence */
                csp_printf("Error: esc_tag buffer overflow\r\n");
                fflush(stdout);
                continue;
            }
            esc_tag[esc_tag_len++] = c;
            if (c == 'm') {
                esc_tag[esc_tag_len++] = '\x0';
                if (!cli->echo_disabled) {
                    csp_printf("%s", esc_tag);
                    fflush(stdout);
                }
                esc = 0; /* quit escape sequence */
            }
            continue;
        }

        inbuf[*bp] = c;
        if ((c == 0x08) || /* backspace */
            (c == 0x7f)) { /* DEL */
            if (*bp > 0) {
                (*bp)--;
                if (!cli->echo_disabled) {
                    csp_printf("%c %c", 0x08, 0x08);
                    fflush(stdout);
                }
            }
            continue;
        }

        if (c == '\t') {
            inbuf[*bp] = '\0';
            tab_complete(inbuf, bp);
            continue;
        }

        if (!cli->echo_disabled) {
            csp_printf("%c", c);
            fflush(stdout);
        }

        (*bp)++;
        if (*bp >= INBUF_SIZE) {
            aos_cli_printf("Error: input buffer overflow\r\n");
            aos_cli_printf(PROMPT);
            *bp = 0;
            return 0;
        }
    }

    return 0;
}

__attribute__((weak)) int fhost_ipc_help(void)
{
    return 0;
}

__attribute__((weak)) int _extra_command(char *cmd_string)
{
    return -1;
}

/* Print out a bad command string, including a hex
 * representation of non-printable characters.
 * Non-printable characters show as "\0xXX".
 */
static void print_bad_command(char *cmd_string)
{
    if (cmd_string != NULL) {
        aos_cli_printf("command '%s' not found\r\n", cmd_string);
    }
}

static void cli_main_input(char *buffer, int count)
{
    int   ret;
    char *msg = NULL;

    if (get_input(cli->inbuf, &cli->bp, buffer, count)) {
        msg = cli->inbuf;
#if 0
        if (strcmp(msg, EXIT_MSG) == 0) {
            break;
        }
#endif
#if (AOS_CLI_MINI_SIZE <= 0)
        if (strlen(cli->inbuf) > 0) {
            cli_history_input();
        }
#endif
        ret = _extra_command(msg);
        if (ret != 0) {
            ret = handle_input(msg);
            if (ret == 1) {
                print_bad_command(msg);
            } else if (ret == 2) {
                aos_cli_printf("syntax error\r\n");
            }
        }

        aos_cli_printf("\r\n");
        esc_tag[0]  = '\x0';
        esc_tag_len = 0;
        aos_cli_printf(PROMPT);
    }
}

static void help_cmd(char *buf, int len, int argc, char **argv);
static void version_cmd(char *buf, int len, int argc, char **argv);
#if (AOS_CLI_MINI_SIZE <= 0)

static void echo_cmd(char *buf, int len, int argc, char **argv);
static void exit_cmd(char *buf, int len, int argc, char **argv);
static void devname_cmd(char *buf, int len, int argc, char **argv);
static void pmem_cmd(char *buf, int len, int argc, char **argv);
static void mmem_cmd(char *buf, int len, int argc, char **argv);

#endif
static void reboot_cmd(char *buf, int len, int argc, char **argv);
static void poweroff_cmd(char *buf, int len, int argc, char **argv);
static void reset_cmd(char *buf, int len, int argc, char **argv);
static void uptime_cmd(char *buf, int len, int argc, char **argv);
static void ota_cmd(char *buf, int len, int argc, char **argv);
static void ps_cmd(char *buf, int len, int argc, char **argv);
static void ls_cmd(char *buf, int len, int argc, char **argv);
static void hexdump_cmd(char *buf, int len, int argc, char **argv);
static void cat_cmd(char *buf, int len, int argc, char **argv);

const struct cli_command built_ins[] STATIC_CLI_CMD_ATTRIBUTE = {
    /*cli self*/
    { "help", "print this", help_cmd },

#if (AOS_CLI_MINI_SIZE <= 0)

    { "p", "print memory", pmem_cmd },
    { "m", "modify memory", mmem_cmd },
    { "echo", "echo for command", echo_cmd },
    { "exit", "close CLI", exit_cmd },
    { "devname", "print device name", devname_cmd },
#endif

    /*rhino*/
    { "sysver", "system version", version_cmd },
    { "reboot", "reboot system", reboot_cmd },
    { "poweroff", "poweroff system", poweroff_cmd },
    { "reset", "system reset", reset_cmd },

    /*aos_rhino*/
    { "time", "system time", uptime_cmd },
    { "ota", "system ota", ota_cmd },
    { "ps", "thread dump", ps_cmd },
    { "ls", "file list", ls_cmd },
    { "hexdump", "dump file", hexdump_cmd },
    { "cat", "cat file", cat_cmd },
};

/* Built-in "help" command: prints all registered commands and their help
 * text string, if any.
 */
static void help_cmd(char *buf, int len, int argc, char **argv)
{
    int      i, n;
    uint32_t build_in_count = sizeof(built_ins) / sizeof(built_ins[0]);

    fhost_ipc_help();

    aos_cli_printf("====Build-in Commands====\r\n");
    aos_cli_printf("====Support %d cmds once, seperate by ; ====\r\n",
                   CLI_MAX_ONCECMD_NUM);

    for (i = 0; i < build_in_count; i++) {
        const struct cli_command *cmd = &built_ins[i];
        if (cmd->name) {
            aos_cli_printf("%-25s: %s\r\n", cmd->name,
                           cmd->help ? cmd->help : "");
        }
    }
    aos_cli_printf("\r\n");
    aos_cli_printf("====User Commands====\r\n");
    for (i = 0, n = build_in_count; i < cli->num_static_cmds + MAX_DYNAMIC_COMMANDS && n < cli->num_commands; i++) {
        const struct cli_command *cmd = cli_command_get(i, NULL);
        if (cmd >= built_ins && cmd < built_ins + build_in_count) {
            continue;
        }
        if (cmd->name) {
            aos_cli_printf("%-25s: %s\r\n", cmd->name,
                           cmd->help ? cmd->help : "");
            n++;
        }
    }
}


static void version_cmd(char *buf, int len, int argc, char **argv)
{
    int num = 4, i;
    unsigned int addr[4], size[4];
    char desc[4][6];

    aos_cli_printf("kernel version :posix\r\n");
    aos_cli_printf(BL_SDK_VER);
    aos_cli_printf("\r\n");
    if (0 == bl_chip_memory_ram(&num, addr, size, desc)) {
        aos_cli_printf("Memory Configuration on %d banks:\r\n", num);
        for (i = 0; i < num; i++) {
            aos_cli_printf("    [%d]%6s %6u Kbytes @ %p\r\n",
                i,
                &(desc[i]),
                size[i] >> 10,
                addr[i]
            );
        }
    }
    aos_cli_printf("Heap left: %d Bytes\r\n", xPortGetFreeHeapSize());
}


#if (AOS_CLI_MINI_SIZE <= 0)

static void echo_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc == 1) {
        aos_cli_printf("Usage: echo on/off. Echo is currently %s\r\n",
                       cli->echo_disabled ? "Disabled" : "Enabled");
        return;
    }

    if (!strcmp(argv[1], "on")) {
        aos_cli_printf("Enable echo\r\n");
        cli->echo_disabled = 0;
    } else if (!strcmp(argv[1], "off")) {
        aos_cli_printf("Disable echo\r\n");
        cli->echo_disabled = 1;
    }
}

static void exit_cmd(char *buf, int len, int argc, char **argv)
{
    cliexit = 1;
    return;
}

static void devname_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("device name: %s\r\n", SYSINFO_DEVICE_NAME);
}

static void pmem_cmd(char *buf, int len, int argc, char **argv)
{
    int   i;
    char *pos    = NULL;
    char *addr   = NULL;
    int   nunits = 16;
    int   width  = 4;

    switch (argc) {
        case 4:
            width = strtol(argv[3], NULL, 0);
            __attribute__ ((fallthrough));
        case 3:
            nunits = strtol(argv[2], NULL, 0);
            nunits = nunits > 0x400 ? 0x400 : nunits;
            __attribute__ ((fallthrough));
        case 2:
            addr = (char *)strtol(argv[1], &pos, 0);
            break;
        default:
            break;
    }

    if (pos == NULL || pos == argv[1]) {
        aos_cli_printf("p <addr> <nunits> <width>\r\n"
                       "addr  : address to display\r\n"
                       "nunits: number of units to display (default is 16)\r\n"
                       "width : width of unit, 1/2/4 (default is 4)\r\n");
        return;
    }

    switch (width) {
        case 1:
            for (i = 0; i < nunits; i++) {
                if (i % 16 == 0) {
                    aos_cli_printf("0x%08x:", (unsigned int)addr);
                }
                aos_cli_printf(" %02x", *(unsigned char *)addr);
                addr += 1;
                if (i % 16 == 15) {
                    aos_cli_printf("\r\n");
                }
            }
            break;
        case 2:
            for (i = 0; i < nunits; i++) {
                if (i % 8 == 0) {
                    aos_cli_printf("0x%08x:", (unsigned int)addr);
                }
                aos_cli_printf(" %04x", *(unsigned short *)addr);
                addr += 2;
                if (i % 8 == 7) {
                    aos_cli_printf("\r\n");
                }
            }
            break;
        default:
            for (i = 0; i < nunits; i++) {
                if (i % 4 == 0) {
                    aos_cli_printf("0x%08x:", (unsigned int)addr);
                }
                aos_cli_printf(" %08x", *(unsigned int *)addr);
                addr += 4;
                if (i % 4 == 3) {
                    aos_cli_printf("\r\n");
                }
            }
            break;
    }
}

static void mmem_cmd(char *buf, int len, int argc, char **argv)
{
    void        *addr  = NULL;
    int          width = 4;
    unsigned int value = 0;
    unsigned int old_value;
    unsigned int new_value;

    switch (argc) {
        case 4:
            width = strtol(argv[3], NULL, 0);
            __attribute__ ((fallthrough));
        case 3:
            value = strtol(argv[2], NULL, 0);
            __attribute__ ((fallthrough));
        case 2:
            addr = (void *)strtol(argv[1], NULL, 0);
            break;
        default:
            addr = NULL;
            break;
    }

    if (addr == NULL) {
        aos_cli_printf("m <addr> <value> <width>\r\n"
                       "addr  : address to modify\r\n"
                       "value : new value (default is 0)\r\n"
                       "width : width of unit, 1/2/4 (default is 4)\r\n");
        return;
    }

    switch (width) {
        case 1:
            old_value = (unsigned int)(*(unsigned char volatile *)addr);
            *(unsigned char volatile *)addr = (unsigned char)value;
            new_value = (unsigned int)(*(unsigned char volatile *)addr);
            break;
        case 2:
            old_value = (unsigned int)(*(unsigned short volatile *)addr);
            *(unsigned short volatile *)addr = (unsigned short)value;
            new_value = (unsigned int)(*(unsigned short volatile *)addr);
            break;
        case 4:
        default:
            old_value                      = *(unsigned int volatile *)addr;
            *(unsigned int volatile *)addr = (unsigned int)value;
            new_value                      = *(unsigned int volatile *)addr;
            break;
    }
    aos_cli_printf("value on 0x%x change from 0x%x to 0x%x.\r\n", (unsigned int)addr,
                   old_value, new_value);
}

#endif

static void reboot_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("reboot\r\n");

    hal_reboot();
}

static void poweroff_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("poweroff\r\n");

    hal_poweroff();
}

static void reset_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("system reset\r\n");

    hal_sys_reset();
}

static void uptime_cmd(char *buf, int len, int argc, char **argv)
{
    long long ms;
    long long days;
    long long hours;
    long long minutes;
    long long seconds;

    ms = aos_now_ms();
    aos_cli_printf("UP time in ms %llu\r\n", ms);
    seconds = ms / 1000;
    minutes = seconds / 60;
    hours = minutes / 60;
    days = hours / 24;
    aos_cli_printf("UP time in %llu days, %llu hours, %llu minutes, %llu seconds\r\n",
            days,
            hours % 24,
            minutes % 60,
            seconds % 60
    );
}

void tftp_ota_thread(void *arg)
{
    aos_task_exit(0);
}

static void ota_cmd(char *buf, int len, int argc, char **argv)
{
    aos_task_new("LOCAL OTA", tftp_ota_thread, 0, 4096);
}

static void ps_cmd(char *buf, int len, int argc, char **argv)
{
    char *pcWriteBuffer, *info;
    const char *const pcHeader = "State   Priority  Stack    #          Base\r\n********************************************************\r\n";
    BaseType_t xSpacePadding;
 
    info = pvPortMalloc(1536);
    if (NULL == info) {
        return;
    }
    pcWriteBuffer = info;

    /* Generate a table of task stats. */
    strcpy(pcWriteBuffer, "Task" );
    pcWriteBuffer += strlen(pcWriteBuffer );
 
    /* Minus three for the null terminator and half the number of characters in
    "Task" so the column lines up with the centre of the heading. */
    for ( xSpacePadding = strlen( "Task" ); xSpacePadding < ( configMAX_TASK_NAME_LEN - 3 ); xSpacePadding++ ) {                                 
        /* Add a space to align columns after the task's name. */
        *pcWriteBuffer = ' ';         
        pcWriteBuffer++;              
    
        /* Ensure always terminated. */
        *pcWriteBuffer = 0x00;        
    }                                 
    strcpy(pcWriteBuffer, pcHeader );
    vTaskList(pcWriteBuffer + strlen(pcHeader));
    cli_putstr(info);

    vPortFree(info);
}

static int cb_idnoe(void *arg, inode_t *node)
{
    int *env = (int*)arg;

    if (0 == (*env)) {
        (*env) = 1;
        printf("\tSize\t\t\t\t\t Name\t\t\tFiletype\r\n");
        printf("----------------------------------------------------------------------------------\r\n");
    }
    printf("%10d\t\t%30s\t\t\t%s\r\n",
            (int)(INODE_IS_CHAR(node) ? sizeof(struct file_ops) : (INODE_IS_BLOCK(node) ? sizeof(struct file_ops) : (INODE_IS_FS(node) ? sizeof(struct fs_ops) : 0))),
            node->i_name,
            INODE_IS_CHAR(node) ? "Char Device" : (INODE_IS_BLOCK(node) ? "Block Device" : (INODE_IS_FS(node) ? "File System" : "Unknown"))
    );
    return 0;
}

static void ls_cmd(char *buf, int len, int argc, char **argv)
{
    char path_name[128];
    aos_dir_t * dp;
    aos_dirent_t *out_dirent;
    struct stat *st;
    int env = 0;

    st = pvPortMalloc(sizeof(struct stat));
    memset(st, 0, sizeof(struct stat));
    memset(path_name, 0, sizeof(path_name));

    if (argc == 2) {
        if (!strcmp(argv[1], "/sdcard")) {
            dp = (aos_dir_t *)aos_opendir("/sdcard");
            if (dp) {
                printf("\tSize\t\t\t\t\t Name\t\t\tFiletype\r\n");
                printf("----------------------------------------------------------------------------------\r\n");
                while(1) {
                    out_dirent = (aos_dirent_t *)aos_readdir(dp);
                    if (out_dirent == NULL) {
                        break;
                    }
                    snprintf(path_name, sizeof(path_name) - 1, "/sdcard/%s", out_dirent->d_name);
                    if (0 == aos_stat(path_name, st)) {
                        if (S_IFDIR & st->st_mode) {
                            printf("%10ld\t\t%30s\t\t\tDirectory\r\n", st->st_size, out_dirent->d_name);
                        } else {
                            printf("%10ld\t\t%30s\t\t\tFile\r\n", st->st_size, out_dirent->d_name);
                        }
                    }
                }
                aos_closedir(dp);
            } else {
                printf("can not open sdcard\r\n");
            }
        } else if (!memcmp(argv[1], "/romfs", strlen("/romfs"))) {
            dp = (aos_dir_t *)aos_opendir(argv[1]);
            if (dp) {
                printf("\tSize\t\t\t\t\t Name\t\t\tFiletype\r\n");
                printf("----------------------------------------------------------------------------------\r\n");
                while(1) {
                    out_dirent = (aos_dirent_t *)aos_readdir(dp);
                    if (out_dirent == NULL) {
                        break;
                    }
                    //log_info("path_name = %s\r\n", out_dirent->d_name);

                    memset(path_name, 0, sizeof(path_name));
                    snprintf(path_name, sizeof(path_name) - 1, "%s", argv[1]);
                    if (path_name[strlen(path_name) - 1] != '/') {
                        path_name[strlen(path_name)] = '/';
                    }
                    snprintf(path_name + strlen(path_name), sizeof(path_name)- strlen(path_name) - 1, "%s", out_dirent->d_name);

                    //log_info("path_name = %s\r\n", path_name);
                    if (0 == aos_stat(path_name, st)) {
                        if (S_IFDIR & st->st_mode) {
                            printf("%10ld\t\t%30s\t\t\tDirectory\r\n", st->st_size, out_dirent->d_name);
                        } else {
                            printf("%10ld\t\t%30s\t\t\tFile\r\n", st->st_size, out_dirent->d_name);
                        }
                    }
                }
                aos_closedir(dp);
            } else {
                printf("can not open romfs\r\n");
            }
        } else if (!strcmp(argv[1], "/")) {
            inode_forearch_name(cb_idnoe, &env);
        } else {
            aos_cli_printf("un-supported direcotry!\r\n");
        }
    } else {
        if (argc == 1) {
            inode_forearch_name(cb_idnoe, &env);
        } else {
            aos_cli_printf("invalid parameter!\r\n");
        }        
    }
    vPortFree(st);
}

static void hexdump_cmd(char *buf, int len, int argc, char **argv)
{
    int fd;
    romfs_filebuf_t filebuf;

    if (2 != argc) {
        printf("invalid parameter!\r\n");
    }

    fd = aos_open(argv[1], 0);
    if (fd < 0) {
        printf("open %s failed!\r\n", argv[1]);
        return;
    }

    memset(&filebuf, 0, sizeof(filebuf));
    aos_ioctl(fd, IOCTL_ROMFS_GET_FILEBUF, (long unsigned int)&filebuf);
    printf("Found file %s. XIP Addr %p, len %lu\r\n",
            argv[1],
            filebuf.buf,
            (unsigned long)filebuf.bufsize
    );
    utils_hexdump(filebuf.buf, filebuf.bufsize);
    aos_close(fd);
}

static void cat_cmd(char *buf, int len, int argc, char **argv)
{
    int fd;
    char ch;

    if (2 != argc) {
        printf("invalid parameter!\r\n");
    }

    fd = open(argv[1], 0);
    if (fd < 0) {
        printf("open %s failed!\r\n", argv[1]);
        return;
    }

    while (1) {
        if(1 != read(fd, &ch, 1)) {
            break;
        }
        printf("%c", ch);
    }
    printf("\r\n");
    close(fd);
}
/* ------------------------------------------------------------------------- */

int aos_cli_register_command(const struct cli_command *cmd)
{
    int i;

    if (!cli) {
        return EPERM;
    }

    if (!cmd->name || !cmd->function) {
        return EINVAL;
    }

    if (cli->num_commands >= cli->num_static_cmds + MAX_DYNAMIC_COMMANDS) {
        return ENOMEM;
    }

    /* Check if the command has already been registered.
     * Return 0, if it has been registered.
     */
    for (i = 0; i < cli->num_commands; i++) {
        if (cli_command_get(i, NULL) == cmd) {
            return 0;
        }
    }

#if (AOS_CLI_MINI_SIZE > 0)
    for (i = 0; i < sizeof(cli_mini_support_cmds) / sizeof(char *); i++) {
        if (strcmp(cmd->name, cli_mini_support_cmds[i]) == 0) {
            break;
        }
    }
    if (i == sizeof(cli_mini_support_cmds) / sizeof(char *)) {
        aos_cli_printf("Warning! mini cli mode do not support cmd:%s\r\n",
                       cmd->name);
        return 0;
    }
#endif

    cli->dynamic_cmds[cli->num_commands++ - cli->num_static_cmds] = cmd;

    return 0;
}

int aos_cli_unregister_command(const struct cli_command *cmd)
{
    int i;
    int remaining_cmds;

    if (!cmd->name || !cmd->function) {
        return EINVAL;
    }

    for (i = 0; i < cli->num_commands; i++) {
        int is_static_cmd = 0;
        const struct cli_command *c = cli_command_get(i, &is_static_cmd);
        if (c == cmd) {
            // unregister a static command is not allowed
            if (is_static_cmd) {
                return EPERM;
            }
            cli->num_commands--;
            remaining_cmds = cli->num_commands - i;
            if (remaining_cmds > 0) {
                memmove(&cli->dynamic_cmds[i - cli->num_static_cmds], &cli->dynamic_cmds[i - cli->num_static_cmds + 1],
                        (remaining_cmds * sizeof(struct cli_command *)));
            }
            cli->dynamic_cmds[cli->num_commands - cli->num_static_cmds] = NULL;
            return 0;
        }
    }

    return -ENOMEM;
}

int aos_cli_register_commands(const struct cli_command *cmds, int num_cmds)
{
    int i;
    int err;

    if (!cli) {
        return EPERM;
    }
    for (i = 0; i < num_cmds; i++) {
        if ((err = aos_cli_register_command(cmds++)) != 0) {
            return err;
        }
    }

    return 0;
}

int aos_cli_unregister_commands(const struct cli_command *cmds, int num_cmds)
{
    int i;
    int err;

    for (i = 0; i < num_cmds; i++) {
        if ((err = aos_cli_unregister_command(cmds++)) != 0) {
            return err;
        }
    }

    return 0;
}

int aos_cli_stop(void)
{
    cliexit = 1;

    return 0;
}


void *aos_cli_task_get(void)
{
    return cli_task.hdl;
}

int aos_cli_init(int use_thread)
{
    extern char _ld_bl_static_cli_cmds_start, _ld_bl_static_cli_cmds_end;
#if 0
    int ret;
#endif

    cli = (struct cli_st *)aos_malloc(sizeof(struct cli_st));
    if (cli == NULL) {
        return ENOMEM;
    }

    memset((void *)cli, 0, sizeof(struct cli_st));

#if 0
    /* add our built-in commands */
    if ((ret = aos_cli_register_commands(
           &built_ins[0], sizeof(built_ins) / sizeof(built_ins[0]))) != 0) {
        goto init_general_err;
    }
#endif
    cli->static_cmds = (struct cli_command *)&_ld_bl_static_cli_cmds_start;
    cli->num_static_cmds = (struct cli_command *)&_ld_bl_static_cli_cmds_end -
            (struct cli_command *)&_ld_bl_static_cli_cmds_start;
    cli->num_commands = cli->num_static_cmds;

    cli->initialized   = 1;
    cli->echo_disabled = 0;

#ifdef CONFIG_AOS_CLI_BOARD
    board_cli_init();
#endif

#if 0
    log_cli_init();
#endif

    return 0;

#if 0
init_general_err:
    if (cli) {
        aos_free(cli);
        cli = NULL;
    }

    return ret;
#endif 
}

static void console_cb_read(int fd, void *param)
{
    char buffer[64];  /* adapt to usb cdc since usb fifo is 64 bytes */
    int ret;

    ret = aos_read(fd, buffer, sizeof(buffer));
    if (ret > 0) {
        if (ret <= sizeof(buffer)) {
            fd_console = fd;
            cli_main_input(buffer, ret);
        } else {
            printf("-------------BUG from aos_read for ret\r\n");
        }
    }
}                                                                                 

static void console_cb_write(int fd, void *param)
{
    printf("Empty cb\r\n");
}                                                                                 

void *aos_cli_event_cb_read_get()
{
    return console_cb_read;
}

void *aos_cli_event_cb_write_get()
{
    return console_cb_write;
}

const char *aos_cli_get_tag(void)
{
    return esc_tag;
}

void aos_cli_input_direct(char *buffer, int count)
{
    cli_main_input(buffer, count);
}

#if defined BUILD_BIN || defined BUILD_KERNEL
int                              aos_cli_printf(const char *msg, ...)
{
    va_list ap;

    char *pos, message[256];
    int   sz;
    int   len;

    memset(message, 0, 256);

    sz = 0;
    if (esc_tag_len) {
        strcpy(message, esc_tag);
        sz = strlen(esc_tag);
    }
    pos = message + sz;

    va_start(ap, msg);
    len = vsnprintf(pos, 256 - sz, msg, ap);
    va_end(ap);

    if (len <= 0) {
        return 0;
    }

    cli_putstr(message);

    return 0;
}
#endif

int cli_putstr(char *msg)
{
    int len, pos, ret;

    len = strlen(msg);
    pos = 0;
    while (pos < len) {
        if ((ret = aos_write(fd_console, msg + pos, len - pos)) >= 0) {
            pos += ret;//move to next data block
            continue;
        }
        break;
    }

    return 0;
}

int cli_getchar(char *inbuf)
{
    int        ret       = -1;
    extern hosal_uart_dev_t uart_stdio;

    ret = hosal_uart_receive(&uart_stdio, inbuf, 1);

    if (ret == 1) {
        return 1;
    } else {
        return 0;
    }
}

int aos_cli_device_fd_get(void)
{
    return fd_console;
}

