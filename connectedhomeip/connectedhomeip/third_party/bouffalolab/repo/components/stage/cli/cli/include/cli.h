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

#ifndef AOS_CLI_H
#define AOS_CLI_H

#define CONFIG_AOS_CLI
#define BUILD_BIN 1

#ifndef AOS_CLI_MINI_SIZE
#define AOS_CLI_MINI_SIZE       0
#endif

#if(AOS_CLI_MINI_SIZE > 0)

/*can config to cut mem size*/
#define INBUF_SIZE   64
#define OUTBUF_SIZE  32    /*not use now*/
#define MAX_DYNAMIC_COMMANDS 10

#define CLI_MAX_ARG_NUM    8
#define CLI_MAX_ONCECMD_NUM    1

#else

/*can config to cut mem size*/
#define INBUF_SIZE   256
#define OUTBUF_SIZE  512
#define MAX_DYNAMIC_COMMANDS 8

#define CLI_MAX_ARG_NUM    32
#define CLI_MAX_ONCECMD_NUM    4

#endif


#ifndef CONFIG_AOS_CLI_STACK_SIZE
#define CONFIG_AOS_CLI_STACK_SIZE 2048
#endif


#ifndef FUNCPTR
typedef void (*FUNCPTR)(void);
#endif

#define STATIC_CLI_CMD_ATTRIBUTE __attribute__((used, section(".static_cli_cmds")))

/* Structure for registering CLI commands */
struct cli_command {
    const char *name;
    const char *help;

    void (*function)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
};

struct cli_st {
    int initialized;
    int echo_disabled;

    const struct cli_command *static_cmds;
    const struct cli_command *dynamic_cmds[MAX_DYNAMIC_COMMANDS]; // dynamic commands

    unsigned int num_static_cmds;
    unsigned int num_commands; // static + dynamic commands
    unsigned int bp; /* buffer pointer */

    char inbuf[INBUF_SIZE];
    char *outbuf;

#if(AOS_CLI_MINI_SIZE <= 0)
    int his_idx;
    int his_cur;
    char history[INBUF_SIZE];
#endif
};

#define CLI_ARGS char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv

#ifdef CONFIG_AOS_CLI

#define cmd_printf(...)                                            \
    do {                                                           \
        if (xWriteBufferLen > 0) {                                 \
            snprintf(pcWriteBuffer, xWriteBufferLen, __VA_ARGS__); \
            xWriteBufferLen-= os_strlen(pcWriteBuffer);            \
            pcWriteBuffer+= os_strlen(pcWriteBuffer);              \
        }                                                          \
    } while(0)


/**
 * This function registers a command with the command-line interface.
 *
 * @param[in]  command  The structure to register one CLI command
 *
 * @return  0 on success, error code otherwise.
 */
int aos_cli_register_command(const struct cli_command *command);

/**
 * This function unregisters a command from the command-line interface.
 *
 * @param[in]  command  The structure to unregister one CLI command
 *
 * @return  0 on success,  error code otherwise.
 */
int aos_cli_unregister_command(const struct cli_command *command);

/**
 * Register a batch of CLI commands
 * Often, a module will want to register several commands.
 *
 * @param[in]  commands      Pointer to an array of commands.
 * @param[in]  num_commands  Number of commands in the array.
 *
 * @return  0 on success， error code otherwise.
 */
int aos_cli_register_commands(const struct cli_command *commands, int num_commands);

/**
 * Unregister a batch of CLI commands
 *
 * @param[in]  commands      Pointer to an array of commands.
 * @param[in]  num_commands  Number of commands in the array.
 *
 * @return  0 on success, error code otherwise.
 */
int aos_cli_unregister_commands(const struct cli_command *commands, int num_commands);

/**
 * Print CLI msg
 *
 * @param[in]  buff  Pointer to a char * buffer.
 *
 * @return  0  on success, error code otherwise.
 */
#if defined BUILD_BIN || defined BUILD_KERNEL
/* SINGLEBIN or KERNEL */
int aos_cli_printf(const char *buff, ...);
#else
/* FRAMWORK or APP */
#define aos_cli_printf(fmt, ...) csp_printf("%s" fmt, aos_cli_get_tag(), ##__VA_ARGS__)
#endif

/**
 * CLI initial function
 *
 * @return  0 on success, error code otherwise
 */
int aos_cli_init(int use_thread);

/**
 * CLI callback function for read
 *
 * @return  function of read
 */
void *aos_cli_event_cb_read_get();

/**
 * Input data directly to CLI logic
 *
 * @return  none
 */
void aos_cli_input_direct(char *buffer, int count);

/**
 * CLI callback function for write
 *
 * @return  function of write
 */
void *aos_cli_event_cb_write_get();

/**
 * Stop the CLI thread and carry out the cleanup
 *
 * @return  0 on success, error code otherwise.
 *
 */
int aos_cli_stop(void);

/**
 * CLI get tag string
 *
 * @return cli tag storing buffer
 */
const char *aos_cli_get_tag(void);

/**
 * get CLI task handle
 *
 * @return task handle
 */
void *aos_cli_task_get(void);

/**
 * create CLI task
 *
 * @return 0 success, others not success
 */
int aos_cli_task_create(void);

/**
 * get cli device fd
 *
 * @return cli device fd
 */
int aos_cli_device_fd_get(void);

#else /* CONFIG_AOS_CLI */

#define cmd_printf(...) do {} while(0)

RHINO_INLINE int aos_cli_register_command(const struct cli_command *command)
{
    return 0;
}

RHINO_INLINE int aos_cli_unregister_command(const struct cli_command *command)
{
    return 0;
}

RHINO_INLINE int aos_cli_register_commands(const struct cli_command *commands,
                                           int num_commands)
{
    return 0;
}

RHINO_INLINE int aos_cli_unregister_commands(const struct cli_command *commands,
                                             int num_commands)
{
    return 0;
}

#define aos_cli_printf csp_printf

RHINO_INLINE int aos_cli_init(void)
{
    return 0;
}

RHINO_INLINE int aos_cli_stop(void)
{
    return 0;
}

RHINO_INLINE void *aos_cli_task_get(void)
{
    return NULL;
}

RHINO_INLINE int aos_cli_task_create(void)
{
    return 0;
}

#endif

#endif /* AOS_CLI_H */

