#include "stdio.h"
#include "stdint.h"
#include "stdarg.h"
#include "string.h"
#include "platform_gpio.h"

__WEAK
void bflb_platform_init_uart_debug_gpio()
{
    GLB_GPIO_Cfg_Type cfg;

    cfg.drive=0;
    cfg.smtCtrl=1;

    /*set GPIO as UART TX */
    cfg.gpioPin=BFLB_UART_DBG_TX_GPIO;
    cfg.gpioFun=BFLB_UART_DBG_TX_NORMAL_FUN;
    cfg.gpioMode=GPIO_MODE_OUTPUT;
    cfg.pullType=GPIO_PULL_UP;
    GLB_GPIO_Init(&cfg);
#if ((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
    GLB_UART_Sig_Swap_Set(UART_SIG_SWAP_GPIO8_GPIO15);
#endif
    /* select uart gpio function */
    GLB_UART_Fun_Sel(BFLB_UART_DBG_TX_SIG,BFLB_UART_DBG_TX_SIG_FUN);

#if !((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
    /*set GPIO as UART RX */
    cfg.gpioPin=BFLB_UART_DBG_RX_GPIO;
    cfg.gpioFun=BFLB_UART_DBG_RX_NORMAL_FUN;
    cfg.gpioMode=GPIO_MODE_INPUT;
    cfg.pullType=GPIO_PULL_UP;
    GLB_GPIO_Init(&cfg);
    /* select uart gpio function */
    GLB_UART_Fun_Sel(BFLB_UART_DBG_RX_SIG,BFLB_UART_DBG_RX_SIG_FUN);
#endif
}

__WEAK
void bflb_platform_deinit_uart_debug_gpio()
{
    GLB_GPIO_Cfg_Type cfg;

    cfg.gpioMode=GPIO_MODE_INPUT;
    cfg.pullType=GPIO_PULL_NONE;
    cfg.drive=0;
    cfg.smtCtrl=1;

    /*set GPIO as input */
    cfg.gpioPin=BFLB_UART_DBG_TX_GPIO;
    cfg.gpioFun=BFLB_UART_DBG_TX_INPUT_FUN;
    GLB_GPIO_Init(&cfg);

    GLB_UART_Sig_Swap_Set(UART_SIG_SWAP_NONE);

    /* restore uart function sel*/
    GLB_UART_Fun_Sel(BFLB_UART_DBG_TX_SIG,(GLB_UART_SIG_FUN_Type)(BFLB_UART_DBG_TX_SIG));

#if !((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
    /*set GPIO as input */
    cfg.gpioPin=BFLB_UART_DBG_RX_GPIO;
    cfg.gpioFun=BFLB_UART_DBG_RX_INPUT_FUN;
    GLB_GPIO_Init(&cfg);

    /* restore uart function sel*/
    GLB_UART_Fun_Sel(BFLB_UART_DBG_RX_SIG,(GLB_UART_SIG_FUN_Type)(BFLB_UART_DBG_RX_SIG));
#endif
}
