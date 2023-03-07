/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef COMM_ROUTINES_H
#define COMM_ROUTINES_H
#include <stdint.h>
/** Saves string describing data types and number of data elements */
typedef struct {
  char* header;
  uint8_t instances;
}HeaderStruct_t;

/** Helper function for outputting a string */
uint8_t OutputString(uint8_t* buffer);

/** Hardware init of serial port */
void CSLIB_commInit(void);

/** Outputs | between columns when needed in ASCII serial protocol */
void outputBreak(void);

/** Outputs Profiler header with column information */
void outputBeginHeader(void);

/** Carriage return function */
void outputNewLine(void);

/** Causes serial interface to block until transfer completes */
void BlockWhileTX(void);

/** Top level function for serial interface tick, called in main() */
void CSLIB_commUpdate(void);

/** Prints header line in interface for one data type */
void outputHeaderCount(HeaderStruct_t headerEntry);

#endif // COMM_ROUTINES_H
