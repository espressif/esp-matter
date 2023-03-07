/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

/*
 * Pin configuration
 */
typedef enum {
    /*GPIO MODE*/
    GPIO_CONFIG_MODE_AF = 0x1,
    GPIO_CONFIG_MODE_OUTPUT = 0x2,
    GPIO_CONFIG_MODE_INPUT = 0x3,

    /*GPIO PULL MODE*/
    GPIO_CONFIG_PULL_NONE = 0x10,
    GPIO_CONFIG_PULL_UP = 0x20,
    GPIO_CONFIG_PULL_DOWN = 0x30,
} gpio_config_t;

/*
 * GPIO dev struct
 */
typedef struct {
    uint8_t       port;    /* gpio port */
    gpio_config_t config;  /* gpio config */
    void         *priv;    /* priv data */
    uint8_t       level;   /* record latest level */
} gpio_dev_t;

/*
 * GPIO interrupt trigger
 */
typedef enum {
    IRQ_TRIGGER_RISING_EDGE  = 0x1, /* Interrupt triggered at input signal's rising edge  */
    IRQ_TRIGGER_FALLING_EDGE = 0x2, /* Interrupt triggered at input signal's falling edge */
    IRQ_TRIGGER_BOTH_EDGES   = IRQ_TRIGGER_RISING_EDGE | IRQ_TRIGGER_FALLING_EDGE,
} gpio_irq_trigger_t;

/*
 * GPIO interrupt callback handler
 */
typedef void (*gpio_irq_handler_t)(void *arg);

/**
 * Initialises a GPIO pin
 *
 * @note  Prepares a GPIO pin for use.
 *
 * @param[in]  gpio           the gpio pin which should be initialised
 * @param[in]  configuration  A structure containing the required gpio configuration
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_init(gpio_dev_t *gpio);

/**
 * Sets an output GPIO pin high
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set high
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_high(gpio_dev_t *gpio);

/**
 * Sets an output GPIO pin low
 *
 * @note  Using this function on a gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set low
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_low(gpio_dev_t *gpio);

/**
 * Trigger an output GPIO pin's output. Using this function on a
 * gpio pin which is set to input mode is undefined.
 *
 * @param[in]  gpio  the gpio pin which should be set low
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_output_toggle(gpio_dev_t *gpio);

/**
 * Get the state of an input GPIO pin. Using this function on a
 * gpio pin which is set to output mode will return an undefined value.
 *
 * @param[in]  gpio   the gpio pin which should be read
 * @param[in]  value  gpio value
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint8_t *value);

/**
 * Set the GPIO pulltype 
 *
 * @param[in]  gpio the gpio pin which should be set
 * @param[in]  pulltype the gpio pulltype value
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hal_gpio_pulltype_set(gpio_dev_t *gpio, gpio_config_t conf);

/**
 * Enables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which is set to
 * output mode is undefined.
 *
 * @param[in]  gpio     the gpio pin which will provide the interrupt trigger
 * @param[in]  trigger  the type of trigger (rising/falling edge)
 * @param[in]  handler  a function pointer to the interrupt handler
 * @param[in]  arg      an argument that will be passed to the interrupt handler
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg);

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref hal_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_disable_irq(gpio_dev_t *gpio);

/**
 * Disables an interrupt trigger for an input GPIO pin.
 * Using this function on a gpio pin which has not been set up
 * using @ref hal_gpio_input_irq_enable is undefined.
 *
 * @param[in]  gpio  the gpio pin which provided the interrupt trigger
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_clear_irq(gpio_dev_t *gpio);

/**
 * Set a GPIO pin in default state.
 *
 * @param[in]  gpio  the gpio pin which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_gpio_finalize(gpio_dev_t *gpio);

#endif /* HAL_GPIO_H */

