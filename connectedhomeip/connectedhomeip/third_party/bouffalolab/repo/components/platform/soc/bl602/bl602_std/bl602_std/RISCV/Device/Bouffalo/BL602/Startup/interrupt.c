#include <stdint.h>
#include <clic.h>
#include <cmsis_compatible_gcc.h>

typedef void( *pFunc )( void );
extern void Trap_Handler(void);
extern void Interrupt_Handler(void);
void Interrupt_Handler_Stub(void);
#ifndef BL602_USE_HAL_DRIVER
void clic_msip_handler_Wrapper                  (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void clic_mtimer_handler_Wrapper                (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void clic_mext_handler_Wrapper                  (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void clic_csoft_handler_Wrapper                 (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void BMX_ERR_IRQHandler_Wrapper                 (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void BMX_TO_IRQHandler_Wrapper                  (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void L1C_BMX_ERR_IRQHandler_Wrapper             (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void L1C_BMX_TO_IRQHandler_Wrapper              (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SEC_BMX_ERR_IRQHandler_Wrapper             (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void RF_TOP_INT0_IRQHandler_Wrapper             (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void RF_TOP_INT1_IRQHandler_Wrapper             (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SDIO_IRQHandler_Wrapper                    (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void DMA_BMX_ERR_IRQHandler_Wrapper             (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SEC_GMAC_IRQHandler_Wrapper                (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SEC_CDET_IRQHandler_Wrapper                (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SEC_PKA_IRQHandler_Wrapper                 (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SEC_TRNG_IRQHandler_Wrapper                (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SEC_AES_IRQHandler_Wrapper                 (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SEC_SHA_IRQHandler_Wrapper                 (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void DMA_ALL_IRQHandler_Wrapper                 (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void IRTX_IRQHandler_Wrapper                    (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void IRRX_IRQHandler_Wrapper                    (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SF_CTRL_IRQHandler_Wrapper                 (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void GPADC_DMA_IRQHandler_Wrapper               (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void EFUSE_IRQHandler_Wrapper                   (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void SPI_IRQHandler_Wrapper                     (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void UART0_IRQHandler_Wrapper                   (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void UART1_IRQHandler_Wrapper                   (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void I2C_IRQHandler_Wrapper                     (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void PWM_IRQHandler_Wrapper                     (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void TIMER_CH0_IRQHandler_Wrapper               (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void TIMER_CH1_IRQHandler_Wrapper               (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void TIMER_WDT_IRQHandler_Wrapper               (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void GPIO_INT0_IRQHandler_Wrapper               (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void PDS_WAKEUP_IRQHandler_Wrapper              (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void HBN_OUT0_IRQHandler_Wrapper                (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void HBN_OUT1_IRQHandler_Wrapper                (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void BOR_IRQHandler_Wrapper                     (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void WIFI_IRQHandler_Wrapper                    (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void BZ_PHY_IRQHandler_Wrapper                  (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void BLE_IRQHandler_Wrapper                     (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void MAC_TXRX_TIMER_IRQHandler_Wrapper          (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void MAC_TXRX_MISC_IRQHandler_Wrapper           (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void MAC_RX_TRG_IRQHandler_Wrapper              (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void MAC_TX_TRG_IRQHandler_Wrapper              (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void MAC_GEN_IRQHandler_Wrapper                 (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void MAC_PORT_TRG_IRQHandler_Wrapper            (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));
void WIFI_IPC_PUBLIC_IRQHandler_Wrapper         (void) __attribute__ ((weak, alias("Interrupt_Handler_Stub")));

const pFunc __Vectors[] __attribute__ ((section(".init"),aligned(64))) = {
    0,                                       /*         */
    0,                                       /*         */
    0,                                       /*         */
    clic_msip_handler_Wrapper,               /*         */
    0,                                       /*         */
    0,                                       /*         */
    0,                                       /*         */
    clic_mtimer_handler_Wrapper,             /*         */
    (pFunc)0x00000004,                       /*         */
    (pFunc)0x00001007,                       /*         */
    (pFunc)0x00010002,                       /*         */
    clic_mext_handler_Wrapper,               /*         */
    clic_csoft_handler_Wrapper,              /*         */
    (pFunc)2000000,                          /*         */
    0,                                       /*         */
    0,                                       /*         */
    BMX_ERR_IRQHandler_Wrapper,              /* 16 +  0 */
    BMX_TO_IRQHandler_Wrapper,               /* 16 +  1 */
    L1C_BMX_ERR_IRQHandler_Wrapper,          /* 16 +  2 */
    L1C_BMX_TO_IRQHandler_Wrapper,           /* 16 +  3 */
    SEC_BMX_ERR_IRQHandler_Wrapper,          /* 16 +  4 */
    RF_TOP_INT0_IRQHandler_Wrapper,          /* 16 +  5 */
    RF_TOP_INT1_IRQHandler_Wrapper,          /* 16 +  6 */
    SDIO_IRQHandler_Wrapper,                 /* 16 +  7 */
    DMA_BMX_ERR_IRQHandler_Wrapper,          /* 16 +  8 */
    SEC_GMAC_IRQHandler_Wrapper,             /* 16 +  9 */
    SEC_CDET_IRQHandler_Wrapper,             /* 16 + 10 */
    SEC_PKA_IRQHandler_Wrapper,              /* 16 + 11 */
    SEC_TRNG_IRQHandler_Wrapper,             /* 16 + 12 */
    SEC_AES_IRQHandler_Wrapper,              /* 16 + 13 */
    SEC_SHA_IRQHandler_Wrapper,              /* 16 + 14 */
    DMA_ALL_IRQHandler_Wrapper,              /* 16 + 15 */
    0,                                       /* 16 + 16 */
    0,                                       /* 16 + 17 */
    0,                                       /* 16 + 18 */
    IRTX_IRQHandler_Wrapper,                 /* 16 + 19 */
    IRRX_IRQHandler_Wrapper,                 /* 16 + 20 */
    0,                                       /* 16 + 21 */
    0,                                       /* 16 + 22 */
    SF_CTRL_IRQHandler_Wrapper,              /* 16 + 23 */
    0,                                       /* 16 + 24 */
    GPADC_DMA_IRQHandler_Wrapper,            /* 16 + 25 */
    EFUSE_IRQHandler_Wrapper,                /* 16 + 26 */
    SPI_IRQHandler_Wrapper,                  /* 16 + 27 */
    0,                                       /* 16 + 28 */
    UART0_IRQHandler_Wrapper,                /* 16 + 29 */
    UART1_IRQHandler_Wrapper,                /* 16 + 30 */
    0,                                       /* 16 + 31 */
    I2C_IRQHandler_Wrapper,                  /* 16 + 32 */
    0,                                       /* 16 + 33 */
    PWM_IRQHandler_Wrapper,                  /* 16 + 34 */
    0,                                       /* 16 + 35 */
    TIMER_CH0_IRQHandler_Wrapper,            /* 16 + 36 */
    TIMER_CH1_IRQHandler_Wrapper,            /* 16 + 37 */
    TIMER_WDT_IRQHandler_Wrapper,            /* 16 + 38 */
    0,                                       /* 16 + 39 */
    0,                                       /* 16 + 40 */
    0,                                       /* 16 + 41 */
    0,                                       /* 16 + 42 */
    0,                                       /* 16 + 43 */
    GPIO_INT0_IRQHandler_Wrapper,            /* 16 + 44 */
    0,                                       /* 16 + 45 */
    0,                                       /* 16 + 46 */
    0,                                       /* 16 + 47 */
    0,                                       /* 16 + 48 */
    0,                                       /* 16 + 49 */
    PDS_WAKEUP_IRQHandler_Wrapper,           /* 16 + 50 */
    HBN_OUT0_IRQHandler_Wrapper,             /* 16 + 51 */
    HBN_OUT1_IRQHandler_Wrapper,             /* 16 + 52 */
    BOR_IRQHandler_Wrapper,                  /* 16 + 53 */
    WIFI_IRQHandler_Wrapper,                 /* 16 + 54 */
    BZ_PHY_IRQHandler_Wrapper,               /* 16 + 55 */
    BLE_IRQHandler_Wrapper,                  /* 16 + 56 */
    MAC_TXRX_TIMER_IRQHandler_Wrapper,       /* 16 + 57 */
    MAC_TXRX_MISC_IRQHandler_Wrapper,        /* 16 + 58 */
    MAC_RX_TRG_IRQHandler_Wrapper,           /* 16 + 59 */
    MAC_TX_TRG_IRQHandler_Wrapper,           /* 16 + 60 */
    MAC_GEN_IRQHandler_Wrapper,              /* 16 + 61 */
    MAC_PORT_TRG_IRQHandler_Wrapper,         /* 16 + 62 */
    WIFI_IPC_PUBLIC_IRQHandler_Wrapper,      /* 16 + 63 */
};

void __IRQ_ALIGN64 Trap_Handler_Stub(void){
    Trap_Handler();
}

void __IRQ Interrupt_Handler_Stub(void){
    Interrupt_Handler();
}
#endif
void clic_enable_interrupt (uint32_t source) {
    *(volatile uint8_t*)(CLIC_HART0_ADDR+CLIC_INTIE+source) = 1;
}

void clic_disable_interrupt ( uint32_t source){
  *(volatile uint8_t*)(CLIC_HART0_ADDR+CLIC_INTIE+source) = 0;
}

void clic_set_pending(uint32_t source){
  *(volatile uint8_t*)(CLIC_HART0_ADDR+CLIC_INTIP+source) = 1;
}

void clic_clear_pending(uint32_t source){
  *(volatile uint8_t*)(CLIC_HART0_ADDR+CLIC_INTIP+source) = 0;
}

void clic_set_intcfg (uint32_t source, uint32_t intcfg){
  *(volatile uint8_t*)(CLIC_HART0_ADDR+CLIC_INTCFG+source) = intcfg;
}

uint8_t clic_get_intcfg  ( uint32_t source){
  return *(volatile uint8_t*)(CLIC_HART0_ADDR+CLIC_INTCFG+source);
}

void clic_set_cliccfg ( uint32_t cfg){
  *(volatile uint8_t*)(CLIC_HART0_ADDR+CLIC_CFG) = cfg;
}

uint8_t clic_get_cliccfg  (void){
  return *(volatile uint8_t*)(CLIC_HART0_ADDR+CLIC_CFG);
}
