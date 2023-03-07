/*
 *
 * Copyright 2022 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#define SE05X_GPIO_PIN_PATH "/sys/class/gpio/gpio4/value"

int se05x_check_and_enable_cl_ct(void)
{
#ifdef CHIP_RPI_SE05X_T4T_DEMO
    // Add callback for gpio
    return 0;
#else
    printf("SIMW : Nothing to do in se05x_check_and_enable_cl_ct \n");
    return 0;
#endif
}

int se05x_enable_contactless_interface(void)
{
#ifdef CHIP_RPI_SE05X_T4T_DEMO
    FILE *se05x_gpio_pin_file = NULL;

    se05x_gpio_pin_file = fopen(SE05X_GPIO_PIN_PATH, "w");
    if (se05x_gpio_pin_file == NULL){
        printf("Could not open gpio file \n");
        return 1;
    }

    fwrite("0", sizeof(char), 2, se05x_gpio_pin_file);
    fclose(se05x_gpio_pin_file);
    printf("se05x_gpio_pin is set to LOW ! \n");

    return 0;
#else
    printf("SIMW : Nothing to do in enable_se05x_contactless_interface \n");
    return 0;
#endif
}

#ifdef __cplusplus
extern "C" {
#endif

int se05x_enable_contact_interface(void)
{
#ifdef CHIP_RPI_SE05X_T4T_DEMO
    FILE *se05x_gpio_pin_file = NULL;

    se05x_gpio_pin_file = fopen(SE05X_GPIO_PIN_PATH, "w");
    if (se05x_gpio_pin_file == NULL)
    {
        printf("Could not open gpio file \n");
        return 1;
    }

    fwrite("1", sizeof(char), 2, se05x_gpio_pin_file);
    fclose(se05x_gpio_pin_file);
    printf("se05x_gpio_pin is set to HIGH ! \n");
    return 0;
#else
    printf("SIMW : Nothing to do in enable_se05x_contact_interface \n");
    return 0;
#endif
}

#ifdef __cplusplus
}
#endif

