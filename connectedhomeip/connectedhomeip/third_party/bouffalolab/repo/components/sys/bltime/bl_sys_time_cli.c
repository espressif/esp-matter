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
#include <stdlib.h>
#include <bl_sys_time.h>

#include <cli.h>
#include <utils_time.h>
#include <blog.h>

static void cmd_sys_time_now(char *buf, int len, int argc, char **argv)
{
    uint64_t epoch_ms = 0;
    uint32_t seconds;
    utils_time_date_t date;

    puts("BL SYS TIME time is\r\n");
    bl_sys_time_get(&epoch_ms);
    seconds = epoch_ms / 1000;
    utils_time_date_from_epoch(seconds, &date);
    printf("Date & time is: %u-%02u-%02u %02u:%02u:%02u (Day %u of week, Day %u of Year)\r\n",
        date.ntp_year,
        date.ntp_month,
        date.ntp_date,
        date.ntp_hour,
        date.ntp_minute,
        date.ntp_second,
        date.ntp_week_day,
        date.day_of_year
    );
}

static void cmd_sys_time_date(char *buf, int len, int argc, char **argv)
{
    uint32_t seconds;
    utils_time_date_t date;

    if (2 != argc) {
        return;
    }

    seconds = atoi(argv[1]);
    printf("Epoch is %lu\r\n", seconds);
    utils_time_date_from_epoch(seconds, &date);
    printf("Date & time is: %u-%02u-%02u %02u:%02u:%02u (Day %u of week, Day %u of Year, leap days %u, leap year status %u)\r\n",
        date.ntp_year,
        date.ntp_month,
        date.ntp_date,
        date.ntp_hour,
        date.ntp_minute,
        date.ntp_second,
        date.ntp_week_day,
        date.day_of_year,
        date.leap_days,
        date.leap_year_ind
    );
}

// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "bl_sys_time_now", "sys time now", cmd_sys_time_now},
        { "bl_sys_time_date", "sys time now", cmd_sys_time_date},
};                                                                                   

int bl_sys_time_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
