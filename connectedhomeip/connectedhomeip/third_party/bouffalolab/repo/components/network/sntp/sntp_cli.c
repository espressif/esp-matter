#include <stdio.h>

#include <lwip/tcpip.h>
#include <sntp.h>
#include <cli.h>
#include <utils_time.h>

void _startup_sntp(void *arg)
{
    puts("--------------------------------------- Start NTP now\r\n");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "0.asia.pool.ntp.org");
    sntp_init();
    puts("--------------------------------------- Start NTP Done\r\n");
}

static void cmd_sntp_start(char *buf, int len, int argc, char **argv)
{
    tcpip_callback(_startup_sntp, NULL);
}

static void cmd_sntp_time(char *buf, int len, int argc, char **argv)
{
    uint32_t seconds = 0, frags = 0;

    sntp_get_time(&seconds, &frags);
    printf("[NTP] time is %lu:%lu\r\n", seconds, frags);
}

static void cmd_sntp_date(char *buf, int len, int argc, char **argv)
{
    uint32_t seconds = 0, frags = 0;
    utils_time_date_t date;

    puts("test epoch from 1581863713 --->>>\r\n");
    puts("Should be Sunday, February 16, 2020 2:35:13 PM\r\n");
    utils_time_date_from_epoch(1581863713, &date);
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

    puts("SNTP GMT time is\r\n");
    sntp_get_time(&seconds, &frags);
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

// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "sntp_start", "sntp start", cmd_sntp_start},
        { "sntp_time", "sntp time", cmd_sntp_time},
        { "sntp_date", "sntp date", cmd_sntp_date},
};                                                                                   

int sntp_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
