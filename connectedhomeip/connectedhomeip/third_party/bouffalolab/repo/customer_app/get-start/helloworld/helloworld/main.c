#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <bl_uart.h>

#define CI_CASE_TABLE_STEP1 {"[helloworld]", "start"}
#define CI_CASE_TABLE_STEP2 {"[helloworld]", "helloworld"}
#define CI_CASE_TABLE_STEP3 {"[helloworld]", "end"}

static const char *ci_table_step_init[] = CI_CASE_TABLE_STEP1;
static const char *ci_table_step_log[] = CI_CASE_TABLE_STEP2;
static const char *ci_table_step_end[] = CI_CASE_TABLE_STEP3;

void log_step(const char *step[2])
{
    printf("%s   %s\r\n", step[0], step[1]);
}

void helloworld(void)
{
    log_step(ci_table_step_init);
    log_step(ci_table_step_log);
    log_step(ci_table_step_end);
}

void main(void)
{
    helloworld();
}

