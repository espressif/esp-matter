/**
 *
 * @file
 * @brief ADC utility functions
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */


#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

/**
 * Return the supply voltage level (AVDD)
 *
 * @return AVDD in millivolts
 */
uint32_t ADC_Measure_VSupply(void);

/**
 * Initialize and enable the ADC
 */
void ADC_Enable(void);

/**
 * Disable the ADC
 */
void ADC_Disable(void);

#endif /* _ADC_H_ */
