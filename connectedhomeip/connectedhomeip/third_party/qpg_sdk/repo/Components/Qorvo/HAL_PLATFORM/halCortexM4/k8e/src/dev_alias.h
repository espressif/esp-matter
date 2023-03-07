#ifndef _DEV_ALIAS_H_
#define _DEV_ALIAS_H_

#if defined(__GNUC__)
#define WEAK_ALIAS(aliasSymbol) __attribute__((weak, alias(aliasSymbol)))
#elif defined(__IAR_SYSTEMS_ICC__)
#define WEAK_ALIAS(aliasSymbol)
#endif

void adcif_handler_impl(void)    WEAK_ALIAS("unexpected_int");
void asp_handler_impl(void)      WEAK_ALIAS("unexpected_int");
void bbpll_handler_impl(void)    WEAK_ALIAS("unexpected_int");
void dmas_handler_impl(void)     WEAK_ALIAS("unexpected_int");
void es_handler_impl(void)       WEAK_ALIAS("unexpected_int");
void gpio_handler_impl(void)     WEAK_ALIAS("unexpected_int");
void gpmicro_handler_impl(void)  WEAK_ALIAS("unexpected_int");
void i2cm_handler_impl(void)     WEAK_ALIAS("unexpected_int");
void i2csl_handler_impl(void)    WEAK_ALIAS("unexpected_int");
void i2sm_handler_impl(void)     WEAK_ALIAS("unexpected_int");
void ipcext2x_handler_impl(void) WEAK_ALIAS("unexpected_int");
void ipcgpm2x_handler_impl(void) WEAK_ALIAS("unexpected_int");
void ipcx2int_handler_impl(void) WEAK_ALIAS("unexpected_int");
void ir_handler_impl(void)       WEAK_ALIAS("unexpected_int");
void keypad_handler_impl(void)   WEAK_ALIAS("unexpected_int");
void mri_handler_impl(void)      WEAK_ALIAS("unexpected_int");
void parble_handler_impl(void)   WEAK_ALIAS("unexpected_int");
void parfcs_handler_impl(void)   WEAK_ALIAS("unexpected_int");
void phy_handler_impl(void)      WEAK_ALIAS("unexpected_int");
void pwms_handler_impl(void)     WEAK_ALIAS("unexpected_int");
void qta_handler_impl(void)      WEAK_ALIAS("unexpected_int");
void rci_handler_impl(void)      WEAK_ALIAS("unexpected_int");
void rpa_handler_impl(void)      WEAK_ALIAS("unexpected_int");
void secproc_handler_impl(void)  WEAK_ALIAS("unexpected_int");
void spim_handler_impl(void)     WEAK_ALIAS("unexpected_int");
void spisl_handler_impl(void)    WEAK_ALIAS("unexpected_int");
void ssp_handler_impl(void)      WEAK_ALIAS("unexpected_int");
void stbc_handler_impl(void)     WEAK_ALIAS("unexpected_int");
void timer_handler_impl(void)    WEAK_ALIAS("unexpected_int");
void trc_handler_impl(void)      WEAK_ALIAS("unexpected_int");
void uart0_handler_impl(void)    WEAK_ALIAS("unexpected_int");
void uart1_handler_impl(void)    WEAK_ALIAS("unexpected_int");
void uart2_handler_impl(void)    WEAK_ALIAS("unexpected_int");
void watchdog_handler_impl(void) WEAK_ALIAS("unexpected_int");

void pendsv_handler_impl(void)   WEAK_ALIAS("unexpected_int");
void systick_handler_impl(void)  WEAK_ALIAS("unexpected_int");

#if defined(__IAR_SYSTEMS_ICC__)
// IAR ... no known way to define this like we do 'WEAK_ALIAS' in GCC.
#pragma weak adcif_handler_impl=unexpected_int
#pragma weak asp_handler_impl=unexpected_int
#pragma weak bbpll_handler_impl=unexpected_int
#pragma weak dmas_handler_impl=unexpected_int
#pragma weak es_handler_impl=unexpected_int
#pragma weak gpio_handler_impl=unexpected_int
#pragma weak gpmicro_handler_impl=unexpected_int
#pragma weak i2cm_handler_impl=unexpected_int
#pragma weak i2csl_handler_impl=unexpected_int
#pragma weak i2sm_handler_impl=unexpected_int
#pragma weak ipcext2x_handler_impl=unexpected_int
#pragma weak ipcgpm2x_handler_impl=unexpected_int
#pragma weak ipcx2int_handler_impl=unexpected_int
#pragma weak ir_handler_impl=unexpected_int
#pragma weak keypad_handler_impl=unexpected_int
#pragma weak mri_handler_impl=unexpected_int
#pragma weak parble_handler_impl=unexpected_int
#pragma weak parfcs_handler_impl=unexpected_int
#pragma weak phy_handler_impl=unexpected_int
#pragma weak pwms_handler_impl=unexpected_int
#pragma weak qta_handler_impl=unexpected_int
#pragma weak rci_handler_impl=unexpected_int
#pragma weak rpa_handler_impl=unexpected_int
#pragma weak secproc_handler_impl=unexpected_int
#pragma weak spim_handler_impl=unexpected_int
#pragma weak spisl_handler_impl=unexpected_int
#pragma weak ssp_handler_impl=unexpected_int
#pragma weak stbc_handler_impl=unexpected_int
#pragma weak timer_handler_impl=unexpected_int
#pragma weak trc_handler_impl=unexpected_int
#pragma weak uart0_handler_impl=unexpected_int
#pragma weak uart1_handler_impl=unexpected_int
#pragma weak uart2_handler_impl=unexpected_int
#pragma weak watchdog_handler_impl=unexpected_int

#pragma weak pendsv_handler_impl=unexpected_int
#pragma weak systick_handler_impl=unexpected_int
#endif
#endif // _DEV_ALIAS_H
