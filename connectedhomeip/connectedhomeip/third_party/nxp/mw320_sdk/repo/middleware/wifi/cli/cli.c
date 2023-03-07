/** @file cli.c
 *
 *  @brief This file provides  CLI: command-line interface
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

/* cli.c: CLI: command-line interface
 *
 */

#include <string.h>
#include <ctype.h>

#include <cli.h>
#include <cli_utils.h>
#include <wm_os.h>
#include <fsl_debug_console.h>

#include "cli_mem.h"

#define END_CHAR      '\r'
#define PROMPT        "\r\n# "
#define HALT_MSG      "CLI_HALT"
#define NUM_BUFFERS   1
#define MAX_COMMANDS  50
#define IN_QUEUE_SIZE 4

#define RX_WAIT   OS_WAIT_FOREVER
#define SEND_WAIT OS_WAIT_FOREVER

#define CONFIG_CLI_STACK_SIZE 4096

os_mutex_t cli_mutex;
static os_queue_pool_define(queue_data, IN_QUEUE_SIZE);
static struct
{
    int input_enabled;
    int initialized;

    unsigned int bp; /* buffer pointer */
    char *inbuf;

    const struct cli_command *commands[MAX_COMMANDS];
    unsigned int num_commands;
    bool echo_disabled;

    os_queue_t input_queue;
    os_queue_pool_t in_queue_data;

} cli;

static os_thread_t cli_main_thread;
static os_thread_stack_define(cli_stack, CONFIG_CLI_STACK_SIZE);

/* Find the command 'name' in the cli commands table.
 * If len is 0 then full match will be performed else upto len bytes.
 * Returns: a pointer to the corresponding cli_command struct or NULL.
 */
static const struct cli_command *lookup_command(char *name, int len)
{
    unsigned int i = 0;
    unsigned int n = 0;

    while (i < MAX_COMMANDS && n < cli.num_commands)
    {
        if (cli.commands[i]->name == NULL)
        {
            i++;
            continue;
        }
        /* See if partial or full match is expected */
        if (len != 0)
        {
            if (!strncmp(cli.commands[i]->name, name, len))
                return cli.commands[i];
        }
        else
        {
            if (!strcmp(cli.commands[i]->name, name))
                return cli.commands[i];
        }

        i++;
        n++;
    }

    return NULL;
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
    static char *argv[16];
    int argc                          = 0;
    int i                             = 0;
    int j                             = 0;
    const struct cli_command *command = NULL;
    const char *p;

    memset((void *)&argv, 0, sizeof(argv));
    memset(&stat, 0, sizeof(stat));

    /*
     * Some terminals add CRLF to the input buffer.
     * Sometimes the CR and LF characters maybe misplaced (it maybe added at the
     * start or at the end of the buffer). Therefore, strip all CRLF (0x0d, 0x0a).
     */
    for (j = 0; j < INBUF_SIZE; j++)
    {
        if (inbuf[j] == 0x0D || inbuf[j] == 0x0A)
        {
            if (j < (INBUF_SIZE - 1))
                memmove((inbuf + j), inbuf + j + 1, (INBUF_SIZE - i));
            inbuf[INBUF_SIZE] = 0x00;
        }
    }

    do
    {
        switch (inbuf[i])
        {
            case '\0':
                if (stat.inQuote)
                    return 2;
                stat.done = 1;
                break;

            case '"':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg)
                {
                    memcpy(&inbuf[i - 1], &inbuf[i], strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (!stat.inQuote && stat.inArg)
                    break;
                if (stat.inQuote && !stat.inArg)
                    return 2;

                if (!stat.inQuote && !stat.inArg)
                {
                    stat.inArg   = 1;
                    stat.inQuote = 1;
                    argc++;
                    argv[argc - 1] = &inbuf[i + 1];
                }
                else if (stat.inQuote && stat.inArg)
                {
                    stat.inArg   = 0;
                    stat.inQuote = 0;
                    inbuf[i]     = '\0';
                }
                break;

            case ' ':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg)
                {
                    memcpy(&inbuf[i - 1], &inbuf[i], strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (!stat.inQuote && stat.inArg)
                {
                    stat.inArg = 0;
                    inbuf[i]   = '\0';
                }
                break;

            default:
                if (!stat.inArg)
                {
                    stat.inArg = 1;
                    argc++;
                    argv[argc - 1] = &inbuf[i];
                }
                break;
        }
    } while (!stat.done && ++i < INBUF_SIZE);

    if (stat.inQuote)
        return 2;

    if (argc < 1)
        return 0;

    if (!cli.echo_disabled)
        PRINTF("\r\n");

    /*
     * Some comamands can allow extensions like foo.a, foo.b and hence
     * compare commands before first dot.
     */
    i       = ((p = strchr(argv[0], '.')) == NULL) ? 0 : (p - argv[0]);
    command = lookup_command(argv[0], i);
    if (command == NULL)
        return 1;

    command->function(argc, argv);

    return 0;
}

/* Perform basic tab-completion on the input buffer by string-matching the
 * current input line against the cli functions table.  The current input line
 * is assumed to be NULL-terminated. */
static void tab_complete(char *inbuf, unsigned int *bp)
{
    unsigned int i, n, m;
    const char *fm = NULL;

    PRINTF("\r\n");

    /* show matching commands */
    for (i = 0, n = 0, m = 0; i < MAX_COMMANDS && n < cli.num_commands; i++)
    {
        if (cli.commands[i]->name != NULL)
        {
            if (!strncmp(inbuf, cli.commands[i]->name, *bp))
            {
                m++;
                if (m == 1)
                    fm = cli.commands[i]->name;
                else if (m == 2)
                    PRINTF("%s %s ", fm, cli.commands[i]->name);
                else
                    PRINTF("%s ", cli.commands[i]->name);
            }
            n++;
        }
    }

    /* there's only one match, so complete the line */
    if (m == 1 && fm)
    {
        n = strlen(fm) - *bp;
        if (*bp + n < INBUF_SIZE)
        {
            memcpy(inbuf + *bp, fm + *bp, n);
            *bp += n;
            inbuf[(*bp)++] = ' ';
            inbuf[*bp]     = '\0';
        }
    }

    /* just redraw input line */
    PRINTF("%s%s", PROMPT, inbuf);
}

enum
{
    BASIC_KEY,
    EXT_KEY_FIRST_SYMBOL,
    EXT_KEY_SECOND_SYMBOL,
};

/* Get an input line.
 *
 * Returns: 1 if there is input, 0 if the line should be ignored. */
static int get_input(char *inbuf, unsigned int *bp)
{
    static int state = BASIC_KEY;
    static char second_char;

    if (inbuf == NULL)
    {
        return 0;
    }

    // wmstdio_flush();

    while (1)
    {
        inbuf[*bp] = GETCHAR();
        if (state == EXT_KEY_SECOND_SYMBOL)
        {
            if (second_char == 0x4F)
            {
                if (inbuf[*bp] == 0x4D)
                {
                    /* Num. keypad ENTER */
                    inbuf[*bp] = '\0';
                    *bp        = 0;
                    state      = BASIC_KEY;
                    return 1;
                }
            }
        }

        if (state == EXT_KEY_FIRST_SYMBOL)
        {
            second_char = inbuf[*bp];
            if (inbuf[*bp] == 0x4F)
            {
                state = EXT_KEY_SECOND_SYMBOL;
                continue;
            }
            if (inbuf[*bp] == 0x5B)
            {
                state = EXT_KEY_SECOND_SYMBOL;
                continue;
            }
        }
        if (inbuf[*bp] == 0x1B)
        {
            /* We may be seeing a first character from a
               extended key */
            state = EXT_KEY_FIRST_SYMBOL;
            continue;
        }
        state = BASIC_KEY;

        if (inbuf[*bp] == END_CHAR)
        { /* end of input line */
            inbuf[*bp] = '\0';
            *bp        = 0;
            return 1;
        }

        if ((inbuf[*bp] == 0x08) || /* backspace */
            (inbuf[*bp] == 0x7f))
        { /* DEL */
            if (*bp > 0)
            {
                (*bp)--;
                if (!cli.echo_disabled)
                    PRINTF("%c %c", 0x08, 0x08);
            }
            continue;
        }

        if (inbuf[*bp] == '\t')
        {
            inbuf[*bp] = '\0';
            tab_complete(inbuf, bp);
            continue;
        }

        if (!cli.echo_disabled)
            PRINTF("%c", inbuf[*bp]);

        (*bp)++;
        if (*bp >= INBUF_SIZE)
        {
            PRINTF("Error: input buffer overflow\r\n");
            PRINTF(PROMPT);
            *bp = 0;
            return 0;
        }
    }
}

/* Print out a bad command string, including a hex
 * representation of non-printable characters.
 * Non-printable characters show as "\0xXX".
 */
static void print_bad_command(char *cmd_string)
{
    if (cmd_string != NULL)
    {
        unsigned char *c = (unsigned char *)cmd_string;
        PRINTF("command '");
        while (*c != '\0')
        {
            if (isprint(*c))
            {
                PRINTF("%c", *c);
            }
            else
            {
                PRINTF("\\0x%x", *c);
            }
            ++c;
        }
        PRINTF("' not found\r\n");
    }
}

/* Ticker function for polling the UART for character input. */
static void console_tick(void)
{
    int ret;

    if (cli.inbuf == NULL)
    {
        ret = cli_get_cmd_buffer(&cli.inbuf);
        if (ret != WM_SUCCESS)
        {
            return;
        }
        cli.bp = 0;
    }

    if (cli.input_enabled && get_input(cli.inbuf, &cli.bp))
    {
        cli.input_enabled = 0;
        ret               = cli_submit_cmd_buffer(&cli.inbuf);
        cli.inbuf         = NULL;
        if (ret != WM_SUCCESS)
        {
            PRINTF(
                "Error: problem sending cli message"
                "\r\n");
        }
        cli.input_enabled = 1;
    }
}

/* Main CLI processing thread
 *
 * Waits to receive a command buffer pointer from an input collector, and
 * then processes.  Note that it must cleanup the buffer when done with it.
 *
 * Input collectors handle their own lexical analysis and must pass complete
 * command lines to CLI.
 */
static void cli_main(os_thread_arg_t data)
{
    os_mutex_get(&cli_mutex, OS_WAIT_FOREVER);
    while (1)
    {
        int ret;
        char *msg;

        msg = NULL;
        ret = os_queue_recv(&cli.input_queue, &msg, RX_WAIT);
        if (ret != WM_SUCCESS)
        {
            if (ret == WM_E_BADF)
            {
                PRINTF(
                    "Error: CLI fatal queue error."
                    "\r\n");
                /* Special case fatal errors.  Shouldn't happen. If it does
                 * it means CLI is fatally corrupted, so end the thread.
                 */
                return;
            }
            /* A number of other non-fatal conditions can cause us to get here]
             * without a message to process, if so, just go back and wait.
             */
            continue;
        }

        /* HALT message indicates that this thread will be deleted
         * shortly. Hence this function need to do necessary actions
         * required before getting deleted.
         * HALT message is not dynamically allocated,
         * hence msg doesn't need to be freed up in that case.
         */
        if (msg != NULL)
        {
            if (strcmp(msg, HALT_MSG) == 0)
                break;
            ret = handle_input(msg);
            if (ret == 1)
                print_bad_command(msg);
            else if (ret == 2)
                PRINTF("syntax error\r\n");
            PRINTF(PROMPT);
            /* done with it, clean up the message (we own it) */
            cli_mem_free(&msg);
        }
    }
    os_mutex_put(&cli_mutex);
    os_thread_self_complete(NULL);
}
/* Automatically bind an input processor to the console */
int cli_install_UART_Tick(void)
{
    return os_setup_idle_function(console_tick);
}

int cli_remove_UART_Tick(void)
{
    return os_remove_idle_function(console_tick);
}

/* Internal cleanup function. */
static int __cli_cleanup(void)
{
    int ret, final = WM_SUCCESS;
    char *halt_msg = HALT_MSG;

    if (cli_remove_UART_Tick() != WM_SUCCESS)
    {
        PRINTF(
            "Error: could not remove UART Tick function."
            "\r\n");
        final = -WM_FAIL;
    }

    ret = cli_submit_cmd_buffer(&halt_msg);
    if (ret != WM_SUCCESS)
    {
        PRINTF(
            "Error: problem sending cli message"
            "\r\n");
    }
    os_mutex_get(&cli_mutex, OS_WAIT_FOREVER);
    ret = os_queue_delete(&cli.input_queue);
    if (ret != WM_SUCCESS)
    {
        PRINTF("Warning: failed to delete queue.\r\n");
        final = -WM_FAIL;
    }

    if (cli.inbuf)
        cli_mem_free(&cli.inbuf);

    ret = cli_mem_cleanup();
    if (ret != WM_SUCCESS)
    {
        final = -WM_FAIL;
    }

    ret = os_thread_delete(&cli_main_thread);
    if (ret != WM_SUCCESS)
    {
        PRINTF("Warning: failed to delete thread.\r\n");
        final = -WM_FAIL;
    }
    os_mutex_put(&cli_mutex);
    os_mutex_delete(&cli_mutex);
    cli.initialized = false;
    return final;
}

/* Initialize and start the main thread */
int cli_start(void)
{
    int ret;

    if (cli.initialized == true)
        return WM_SUCCESS;

    ret = os_mutex_create(&cli_mutex, "cli", OS_MUTEX_INHERIT);
    if (ret != WM_SUCCESS)
    {
        return -WM_FAIL;
    }

    ret = os_thread_create(&cli_main_thread, "cli", cli_main, 0, &cli_stack, OS_PRIO_3);
    if (ret != WM_SUCCESS)
    {
        PRINTF("Error: Failed to create cli thread: %d\r\n", ret);
        return -WM_FAIL;
    }

    ret = cli_mem_init();

    if (ret != WM_SUCCESS)
        return -WM_FAIL;

    ret = os_queue_create(&cli.input_queue, "cli_queue", sizeof(void *), &cli.in_queue_data);
    if (ret != WM_SUCCESS)
    {
        PRINTF("Error: Failed to create cli queue: %d\r\n", ret);
        return -WM_FAIL;
    }
    cli.initialized = true;

    return WM_SUCCESS;
}

/* Stop the thread and cleanup */
int cli_stop(void)
{
    if (cli.initialized == false)
        return -WM_FAIL;

    return __cli_cleanup();
}

/*
 *  Command buffer API
 */

/* Get a command buffer */
int cli_get_cmd_buffer(char **buff)
{
    *buff = cli_mem_malloc(INBUF_SIZE);
    if (*buff == NULL)
    {
        return -WM_FAIL;
    }

    return WM_SUCCESS;
}

/* Submit a command buffer to the main thread for processing */
int cli_submit_cmd_buffer(char **buff)
{
    int ret   = -WM_FAIL;
    int final = WM_SUCCESS;

    if (buff == NULL)
    {
        PRINTF("Error: release_cmd_buffer given NULL buff\r\n");
        return -WM_FAIL;
    }

    if (cli.initialized)
    {
        ret = os_queue_send(&cli.input_queue, (void *)buff, SEND_WAIT);
    }

    if (ret != WM_SUCCESS)
    {
        final = -WM_FAIL;
    }
    return final;
}

/* Built-in "help" command: prints all registered commands and their help
 * text string, if any. */
void help_command(int argc, char **argv)
{
    unsigned int i, n;

    PRINTF("\r\n");
    for (i = 0, n = 0; i < MAX_COMMANDS && n < cli.num_commands; i++)
    {
        if (cli.commands[i]->name)
        {
            PRINTF("%s %s\r\n", cli.commands[i]->name, cli.commands[i]->help ? cli.commands[i]->help : "");
            n++;
        }
    }
}

#if 0
static void echo_cmd_handler(int argc, char **argv)
{
	if (argc == 1) {
		PRINTF("Usage: echo on/off. Echo is currently %s\r\n",
			 cli.echo_disabled ? "Disabled" : "Enabled");
		return;
	}

	if (!strcasecmp(argv[1], "on")) {
		PRINTF("Enable echo\r\n");
		cli.echo_disabled = false;
	} else if (!strcasecmp(argv[1], "off")) {
		PRINTF("Disable echo\r\n");
		cli.echo_disabled = true;
	} else
		PRINTF("Usage: echo on/off. Echo is currently %s\r\n",
			 cli.echo_disabled ? "Disabled" : "Enabled");
}

bool cli_get_echo_mode()
{
	return !cli.echo_disabled;
}

void cli_set_echo_mode(bool enabled)
{
	if (enabled)
		cli.echo_disabled = false;
	else
		cli.echo_disabled = true;
}
#endif

static struct cli_command built_ins[] = {
    {"help", NULL, help_command},
};

/*
 * NXP Test Framework API
 */

int cli_register_command(const struct cli_command *command)
{
    unsigned int i;
    if (!command->name || !command->function)
        return 1;

    if (cli.num_commands < MAX_COMMANDS)
    {
        /* Check if the command has already been registered.
         * Return 0, if it has been registered.
         */
        for (i = 0; i < cli.num_commands; i++)
        {
            if (cli.commands[i] == command)
                return 0;
        }
        cli.commands[cli.num_commands++] = command;
        return 0;
    }

    return 1;
}

int cli_unregister_command(const struct cli_command *command)
{
    unsigned int i;
    if (!command->name || !command->function)
        return 1;

    for (i = 0; i < cli.num_commands; i++)
    {
        if (cli.commands[i] == command)
        {
            cli.num_commands--;
            int remaining_cmds = cli.num_commands - i;
            if (remaining_cmds > 0)
            {
                memmove(&cli.commands[i], &cli.commands[i + 1], (remaining_cmds * sizeof(struct cli_command *)));
            }
            cli.commands[cli.num_commands] = NULL;
            return 0;
        }
    }

    return 1;
}

int cli_register_commands(const struct cli_command *commands, int num_commands)
{
    int i;
    for (i = 0; i < num_commands; i++)
        if (cli_register_command(commands++))
            return 1;
    return 0;
}

int cli_unregister_commands(const struct cli_command *commands, int num_commands)
{
    int i;
    for (i = 0; i < num_commands; i++)
        if (cli_unregister_command(commands++))
            return 1;

    return 0;
}

int cli_init(void)
{
    static bool cli_init_done;
    if (cli_init_done)
        return WM_SUCCESS;

    memset((void *)&cli, 0, sizeof(cli));
    cli.input_enabled = 1;
    cli.in_queue_data = queue_data;

    /* add our built-in commands */
    if (cli_register_commands(&built_ins[0], sizeof(built_ins) / sizeof(struct cli_command)))
        return -WM_FAIL;

    if (cli_install_UART_Tick() != WM_SUCCESS)
    {
        PRINTF(
            "Error: could not install UART Tick function."
            "\r\n");
        return -WM_FAIL;
    }

    int ret = cli_start();
    if (ret == WM_SUCCESS)
        cli_init_done = true;
    return ret;
}
