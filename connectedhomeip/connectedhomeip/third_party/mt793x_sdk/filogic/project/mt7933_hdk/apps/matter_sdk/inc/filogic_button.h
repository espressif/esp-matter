

#ifndef __FILOGIC_BUTTON_H__
#define __FILOGIC_BUTTON_H__


#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Prerequisites
 ******************************************************************************/


#ifndef HAL_GPIO_MODULE_ENABLED
#error "HAL_GPIO_MODULE_ENABLED is needed by BUTTON"
#endif

#ifndef HAL_NVIC_MODULE_ENABLED
#error "HAL_NVIC_MODULE_ENABLED is needed by BUTTON"
#endif

#ifndef HAL_EINT_MODULE_ENABLED
#error "HAL_EINT_MODULE_ENABLED is needed by BUTTON"
#endif


/*****************************************************************************
 * Enums
 *****************************************************************************/


typedef enum filogic_button_id_e
{
    FILOGIC_BUTTON_0, /* SW5 on SAC board, download mode */
    FILOGIC_BUTTON_MAX
} filogic_button_id_t;


/*****************************************************************************
 * Structures
 *****************************************************************************/


typedef struct filogic_button_s
{
    filogic_button_id_t     id;
    bool                    press; /* true if down, false if up */
} filogic_button_t;


/*****************************************************************************
 * Functions
 *****************************************************************************/


typedef void (*filogic_button_event_callback)(const filogic_button_t *event);


bool filogic_button_init(void);


bool filogic_button_set_callback(filogic_button_event_callback callback);


#ifdef __cplusplus
}
#endif


#endif /* __FILOGIC_BUTTON_H__ */
