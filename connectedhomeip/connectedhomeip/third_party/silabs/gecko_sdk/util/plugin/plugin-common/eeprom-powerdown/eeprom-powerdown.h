/******************************************************************************
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 * Header file for eeprom-powerdown plugin API.
 *
 *****************************************************************************/

#ifndef EEPROM_POWERDOWN_H
#define EEPROM_POWERDOWN_H

/**
 * Initialize the EEPROM powerdown functionality.
 *
 * If a user needs to disable the EEPROM to save power, the following plugin
 * should be used and initialized, which automatically places the MX25 EEPROM in
 * a power-saving mode without requiring a bootloader.
 */
void emberEepromPowerDownInitCallback(void);

#endif // EEPROM_POWERDOWN_H
