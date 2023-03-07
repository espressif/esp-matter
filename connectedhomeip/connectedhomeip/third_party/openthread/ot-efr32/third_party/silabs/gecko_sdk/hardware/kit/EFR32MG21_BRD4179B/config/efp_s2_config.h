#ifndef EFP_S2_CONFIG_H
#define EFP_S2_CONFIG_H

#include "sl_efp.h"

#ifdef __cplusplus
extern "C" {
#endif

// <<< Use Configuration Wizard in Context Menu >>>

// <h>EFP driver configuration

// <o EFP_S2_EM_CTRL_MODE> Selects method of controlling EFP Energy Mode (EM) transitions.
// <efp_em_transition_mode_gpio_bitbang=> GPIO driven direct mode EM transitions
// <efp_em_transition_mode_i2c=> I2C transfers control EM transitions
// <efp_em_transition_mode_emu=> Builtin EMU controlled direct mode EM transitions
// <i> Default: efp_em_transition_mode_gpio_bitbang
#define EFP_S2_EM_CTRL_MODE       efp_em_transition_mode_gpio_bitbang

// <q EFP_S2_ENABLE_IRQ> Enable GPIO as interrupt input line from EFP.
// <i> Default: 1
#define EFP_S2_ENABLE_IRQ         1

// <q EFP_S2_POWERS_HOST> This EFP powers host SoC.
// <i> Default: 1
#define EFP_S2_POWERS_HOST        1

// </h> end EFP configuration

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <gpio optional> EFP_S2_IRQ
// $[GPIO_EFP_S2_IRQ]
#define EFP_S2_IRQ_PORT           gpioPortA
#define EFP_S2_IRQ_PIN            0
// [GPIO_EFP_S2_IRQ]$

// <i2c signal=SDA,SCL> EFP_S2_I2C
// $[I2C_EFP_S2_I2C]
#define EFP_S2_I2C_PERIPHERAL     I2C0
#define EFP_S2_I2C_PERIPHERAL_NO  0

#define EFP_S2_I2C_SDA_PORT       gpioPortC
#define EFP_S2_I2C_SDA_PIN        1

#define EFP_S2_I2C_SCL_PORT       gpioPortC
#define EFP_S2_I2C_SCL_PIN        0
// [I2C_EFP_S2_I2C]$

// <<< sl:end pin_tool >>>

#define SL_EFP_S2_INIT {                                                    \
    .config_size = 0,                           /* No initial config */     \
    .config_data = NULL,                        /* No config data */        \
    .is_host_efp = EFP_S2_POWERS_HOST,          /* This EFP powers host */  \
    .em_transition_mode = EFP_S2_EM_CTRL_MODE,  /* EFP EM transition mode*/ \
    .enable_irq_pin = EFP_S2_ENABLE_IRQ,        /* Init GPIO as EFP IRQ */  \
    .irq_port = EFP_S2_IRQ_PORT,                /* EFP IRQ port */          \
    .irq_pin = EFP_S2_IRQ_PIN,                  /* EFP IRQ pin */           \
    .i2c_peripheral = EFP_S2_I2C_PERIPHERAL,    /* I2C port instance */     \
    .i2c_scl_port = EFP_S2_I2C_SCL_PORT,        /* SCL port */              \
    .i2c_scl_pin = EFP_S2_I2C_SCL_PIN,          /* SCL pin */               \
    .i2c_sda_port = EFP_S2_I2C_SDA_PORT,        /* SDA port */              \
    .i2c_sda_pin = EFP_S2_I2C_SDA_PIN,          /* SDA pin */               \
}

#ifdef __cplusplus
}
#endif

#endif /* EFP_S2_CONFIG_H */
