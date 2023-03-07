#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cli.h>

#define BASE_PRIORITY       (0)
#define TASK_NUMBER         (32)
#define TASK_RUNTIME        (10)
static uint32_t result;

void test_task(void *arg)
{
    int i = (int)arg, run_num = 0;
    while (1) {
        if (run_num >= TASK_RUNTIME) {
            result++;
            printf("task:%d end, result:%lu\r\n", i, result);

            if (TASK_NUMBER == result) {
                result = 0;
            }
            vTaskDelete(NULL);
        }
        run_num++;
        printf("task:%d, Number of runs：%d\r\n", i, run_num);
        vTaskDelay(100 * (i + BASE_PRIORITY));
    }
}

void cmd_test_task(char *buf, int len, int argc, char **argv)
{
    int i;
    uint32_t priority = BASE_PRIORITY;
    char task_name[16];

    if (argc != 2) {
        printf("Please Input Parameter!\r\n");
        return;
    }

    if (0 == strcmp(argv[1], "32")) {
        for (i = 0; i < atoi(argv[1]); i++) 
        {
            sprintf(task_name, "task_%d", i);
            xTaskCreate(test_task, task_name, 512, (void *)i, priority++, NULL);
        }
    } else {
        printf("argv not match!\r\n");
    }
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "task", "test task schedule", cmd_test_task},
};
int task_test_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}
