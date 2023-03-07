#include "blprog_ctrl.h"
#include "bl_gpio.h"

#if defined(BL602)
#include "bl602_glb.h"
#endif

#if defined(BL702)
#include "bl702_glb.h"
#endif


static uint8_t blprog_ctrl_boot_pin = 0;
static uint8_t blprog_ctrl_reset_pin = 0;
static uint8_t blprog_ctrl_inited = 0;


int blprog_ctrl_init(uint8_t boot_pin, uint8_t reset_pin)
{
    bl_gpio_enable_output(boot_pin, 0, 0);
    bl_gpio_enable_output(reset_pin, 0, 0);
    
    blprog_ctrl_boot_pin = boot_pin;
    blprog_ctrl_reset_pin = reset_pin;
    blprog_ctrl_inited = 1;
    
    return 0;
}

int blprog_ctrl_set_uart_boot(void)
{
    if(blprog_ctrl_inited == 0){
        printf("Unable to control boot pin!\r\n");
        return -1;
    }
    
    bl_gpio_output_set(blprog_ctrl_boot_pin, 1);
    
    return 0;
}

int blprog_ctrl_set_flash_boot(void)
{
    if(blprog_ctrl_inited == 0){
        printf("Unable to control boot pin!\r\n");
        return -1;
    }
    
    bl_gpio_output_set(blprog_ctrl_boot_pin, 0);
    
    return 0;
}

int blprog_ctrl_reset_chip(void)
{
    if(blprog_ctrl_inited == 0){
        printf("Unable to control reset pin!\r\n");
        return -1;
    }
    
    bl_gpio_output_set(blprog_ctrl_reset_pin, 0);
#if defined(BL602)
    BL602_Delay_MS(5);
#else
    arch_delay_ms(5);
#endif
    bl_gpio_output_set(blprog_ctrl_reset_pin, 1);
    
    return 0;
}
