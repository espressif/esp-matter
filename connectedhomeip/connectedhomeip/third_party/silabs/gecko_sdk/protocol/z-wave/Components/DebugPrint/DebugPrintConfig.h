/**
* @file
* DebugPrint Configuration.
*
* Seperate header file for configuring DebugPrint. 
* Using a seperate header file avoid exposing configuration to all users of
* debugprint.
*
* @copyright 2018 Silicon Laboratories Inc.
*/

#ifndef _DEBUGPRINTCONFIG_H_
#define _DEBUGPRINTCONFIG_H_

#include <stdint.h>


/**
* Function pointer type for the DebugPrint printing.
* @param[in]  p_data       Pointer to data to be printed.
* @param[in]  data_length  Length of data to be printed.
*/
typedef void(*DebugPrintPrinter)(const uint8_t* p_data, uint32_t data_length);

/**
* Configures DebugPrint.
* 
* Nothing is printed until DebugPrintConfig has been called and argument
* Printer has been set. Printer may be null, causing no printing.
*
* @param[in]  pBuffer       Pointer to buffer array where DebugPrint can build strings.
* @param[in]  iBufferSize   Size of array pointed to by pBuffer.
* @param[in]  Printer       DebugPrintPrinter function pointer.
*                           Function pointer is used by DebugPrint as printer.
*                           Arg0 is pointer to data to be printed.
*                           Arg1 is length of data to be printed.
*                           Function bound is expected to do the printing.
*                           If Printer is NULL, nothing will be printed.
*/
void DebugPrintConfig(uint8_t* pBuffer, uint16_t iBufferSize, DebugPrintPrinter Printer);


#endif	// _DEBUGPRINTCONFIG_H_

