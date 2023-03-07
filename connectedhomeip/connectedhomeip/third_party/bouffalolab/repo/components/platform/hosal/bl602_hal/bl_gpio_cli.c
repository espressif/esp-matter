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
#include <cli.h>

#include "bl_gpio.h"

#include <blog.h>
#define USER_UNUSED(a) ((void)(a))

static void cmd_gpio_func(char *buf, int len, int argc, char **argv) 
{
    int ionum = -1, inputmode = -1, pullup = -1, pulldown = -1;

    if (5 != argc) {
        blog_info("Usage: %s 24 1 1 0\r\n  set GPIO24 to input with pullup\r\n",
                argv[0]
        );
        return;
    }
    ionum = atoi(argv[1]);
    inputmode = atoi(argv[2]);
    pullup = atoi(argv[3]);
    pulldown = atoi(argv[4]);
    if (ionum < 0 || inputmode < 0 || pullup < 0 || pulldown < 0) {
        blog_error("Illegal arg\r\n");
        return;
    }
    blog_info("GPIO%d is set %s with %s pullup %s pulldown\r\n",
            ionum,
            inputmode ? "input" : "output",
            pullup ? "Active" : "null",
            pulldown ? "Active" : "null"
    );
    if (inputmode) {
        bl_gpio_enable_input(ionum, pullup ? 1 : 0, pulldown ? 1 : 0);
    } else {
        bl_gpio_enable_output(ionum, pullup ? 1 : 0, pulldown ? 1 : 0);
    }
}

static void cmd_gpio_set(char *buf, int len, int argc, char **argv) 
{
    int ionum = -1, val = -1;

    if  (3 != argc) {
        blog_info("Usage: %s 24 1\r\n  set GPIO24 output to high\r\n",
                argv[0]
        );
        return;
    }
    ionum = atoi(argv[1]);
    val = atoi(argv[2]);
    if (ionum < 0 || val < 0) {
        blog_error("Illegal arg\r\n");
        return;
    }
    blog_info("GPIO%d is set to %s\r\n",
        ionum,
        val ? "high" : "lo"
    );
    bl_gpio_output_set(ionum, val ? 1 : 0);
}

static void cmd_gpio_get(char *buf, int len, int argc, char **argv) 
{
    int ionum = -1, ret;
    uint8_t val;

    USER_UNUSED(ret);
    if  (2 != argc) {
        blog_info("Usage: %s 24\r\n  get GPIO24 value\r\n",
                argv[0]
        );
        return;
    }
    ionum = atoi(argv[1]);
    if (ionum < 0) {
        puts("Illegal arg\r\n");
        return;
    }
    ret = bl_gpio_input_get(ionum, &val);
    blog_info("GPIO%d val is %s\r\n",
        ionum,
        0 == ret ? (val ? "high" : "low") : "Err"
    );
}

// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"gpio-func", "gpio-func pinnum 0/1[0:output, 1:input] 0/1[pullup] 0/1[pulldown]", cmd_gpio_func},
    {"gpio-set", "gpio-set pinnum 0/1", cmd_gpio_set},
    {"gpio-get", "gpio-get pinnum", cmd_gpio_get},
};                                                                                   

int bl_gpio_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
