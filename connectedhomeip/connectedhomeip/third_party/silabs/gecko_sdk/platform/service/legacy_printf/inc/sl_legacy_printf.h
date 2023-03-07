/***************************************************************************//**
 * @file
 * @brief Legacy printf Layer.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#define COM_Port_t uint8_t
#define Ecode_t EmberStatus

/** @name Printf Prototypes
 * These prototypes are for the internal printf implementation, in case
 * it is desired to use it elsewhere. See the code for emberSerialPrintf()
 * for an example of printf usage.
 *@{
 */
uint8_t *emWriteHexInternal(uint8_t *charBuffer, uint16_t value, uint8_t charCount);

/** @brief Typedefine to cast a function into the appropriate format
 * to be used inside the \c emPrintfInternal function below, for performing the
 * actual flushing of a formatted string to a destination such as a serial port.
 *
 * @param flushVar: The destination of the flush, most commonly a serial port
 * number (0 or 1).
 *
 * @param contents  A pointer to the string to flush.
 *
 * @param length  The number of bytes to flush.
 *
 * @return The EmberStatus value of the typedefined function.
 */
typedef Ecode_t (emPrintfFlushHandler)(COM_Port_t flushVar,
                                       uint8_t *contents,
                                       uint8_t length);

/** @brief The internal printf function, which scans the string for the
 * format specifiers and appropriately implants the passed data into the string.
 *
 * @param flushHandler: The name of an internal function, which has
 * parameters matching the function \c emPrintfFlushHandler above, responsible
 * for flushing a string formatted by this function, \c emPrintfInternal, to
 * the appropriate buffer or function that performs the actual transmission.
 *
 * @param port  The destination of the flush performed above, most commonly
 * serial port number (0 or 1).
 *
 * @param string  The string to print.
 *
 * @param args  The list of arguments for the format specifiers.
 *
 * @return The number of characters written.
 */
uint8_t emPrintfInternal(emPrintfFlushHandler flushHandler,
                         COM_Port_t port,
                         PGM_P string,
                         va_list args);

/** @} END Printf Prototypes */

EmberStatus emberSerialWriteString(uint8_t port, PGM_P string);
Ecode_t COM_WriteHex(COM_Port_t port, uint8_t dataByte);
Ecode_t COM_PrintfVarArg(COM_Port_t port, PGM_P formatString, va_list ap);
