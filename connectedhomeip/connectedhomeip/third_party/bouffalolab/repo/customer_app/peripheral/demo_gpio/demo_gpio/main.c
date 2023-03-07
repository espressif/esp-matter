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

static void cmd_hosal_gpio_ledon(char *buf, int len, int argc, char **argv)
{
    extern void gpio_set_led_on(void);
    gpio_set_led_on();
}

static void cmd_hosal_gpio_ledoff(char *buf, int len, int argc, char **argv)
{
    extern void gpio_set_led_off(void);
    gpio_set_led_off();
}

static void cmd_hosal_gpio_irq_test(char *buf, int len, int argc, char **argv)
{
    extern void gpio_irq_test(void);
    gpio_irq_test();
}

static void cmd_hosal_gpio_irq_mask(char *buf, int len, int argc, char **argv)
{
    extern void key1_irq_mask_test(uint8_t mask);
    if (argc != 2) {
        printf("demo_gpio_irq1_mask used wrong!, should demo_gpio_irq1_mask 1 or 0\r\n");
        return;
    }
    uint8_t ma = atoi(argv[1]);
    key1_irq_mask_test(ma);
}

static void cmd_hosal_gpio_finalize(char *buf, int len, int argc, char **argv)
{
    extern void gpio_finalize_test(void);
    gpio_finalize_test();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_gpio_led_on", "demo_gpio_led_on", cmd_hosal_gpio_ledon},
    {"demo_gpio_led_off", "demo_gpio_led_off", cmd_hosal_gpio_ledoff},
    {"demo_gpio_irq_test", "demo_gpio_irq_test", cmd_hosal_gpio_irq_test},
    {"demo_gpio_irq1_mask", "demo_gpio_irq_mask", cmd_hosal_gpio_irq_mask},
    {"demo_gpio_finalize", "demo_gpio_finalize", cmd_hosal_gpio_finalize},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal gpio!\r\n");
    return 0;
}
