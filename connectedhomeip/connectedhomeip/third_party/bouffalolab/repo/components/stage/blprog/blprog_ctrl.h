#ifndef __BLPROG_CTRL_H__
#define __BLPROG_CTRL_H__


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int blprog_ctrl_init(uint8_t boot_pin, uint8_t reset_pin);
int blprog_ctrl_set_uart_boot(void);
int blprog_ctrl_set_flash_boot(void);
int blprog_ctrl_reset_chip(void);


#endif
