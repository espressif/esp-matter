#ifndef __PLATFORM_GPIO_H__
#define __PLATFORM_GPIO_H__


#include "bl602_uart.h"
#include "bl602_glb.h"
#include "bl602_gpio.h"

#if !((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
/*UART  dbug option */
#define BFLB_UART_DBG_RX_GPIO          GLB_GPIO_PIN_7
#define BFLB_UART_DBG_RX_INPUT_FUN     GPIO7_FUN_SWGPIO_7
#define BFLB_UART_DBG_RX_NORMAL_FUN    GPIO7_FUN_UART_SIG7
#define BFLB_UART_DBG_RX_SIG           GLB_UART_SIG_7
#define BFLB_UART_DBG_RX_SIG_FUN       GLB_UART_SIG_FUN_UART0_RXD

#define BFLB_UART_DBG_TX_GPIO          GLB_GPIO_PIN_16
#define BFLB_UART_DBG_TX_INPUT_FUN     GPIO16_FUN_SWGPIO_16
#define BFLB_UART_DBG_TX_NORMAL_FUN    GPIO16_FUN_UART_SIG0
#define BFLB_UART_DBG_TX_SIG           GLB_UART_SIG_0
#define BFLB_UART_DBG_TX_SIG_FUN       GLB_UART_SIG_FUN_UART0_TXD
#else
#define BFLB_UART_DBG_TX_GPIO           GLB_GPIO_PIN_8
#define BFLB_UART_DBG_TX_INPUT_FUN      GPIO8_FUN_SWGPIO_8
#define BFLB_UART_DBG_TX_NORMAL_FUN     GPIO8_FUN_UART_SIG0
/*Sig swapped*/
#define BFLB_UART_DBG_TX_SIG            GLB_UART_SIG_4
#define BFLB_UART_DBG_TX_SIG_FUN        GLB_UART_SIG_FUN_UART1_TXD

#endif

void bflb_platform_init_uart_debug_gpio(void);
void bflb_platform_deinit_uart_debug_gpio(void);


#endif

