/* List of specifiers added in the library which are not available in standard printf implementation */

#include "lwprintf/lwprintf.h"

/**
 * \brief           List of additional specifiers to print
 */
void
additional_format_specifiers(void) {
    unsigned char my_array[] = { 0x01, 0x02, 0xA4, 0xB5, 0xC6 };

    /* Binary output */

    /* Prints number 8 in binary format, so "1000" */
    lwprintf_printf("%b\r\n", 8U);
    /* Prints number 16 in binary format with 10 places, so "     10000" */
    lwprintf_printf("%10b\r\n", 16U);
    /* Prints number 16 in binary format with 10 places, leading zeros, so "0000010000" */
    lwprintf_printf("%010b\r\n", 16U);

    /* Array outputs */

    /* Fixed length with uppercase hex numbers, outputs "0102A4B5C6" */
    lwprintf_printf("%5K\r\n", my_array);
    /* Fixed length with lowercase hex numbers, outputs "0102a4b5c6" */
    lwprintf_printf("%5k\r\n", my_array);
    /* Variable length with uppercase letters, outputs "0102A4B5C6" */
    lwprintf_printf("%*K\r\n", (int)LWPRINTF_ARRAYSIZE(my_array), my_array);
    /* Variable length with lowercase letters, outputs "0102a4b5c6" */
    lwprintf_printf("%*k\r\n", (int)LWPRINTF_ARRAYSIZE(my_array), my_array);
    /* Variable length with uppercase letters and spaces, outputs "01 02 A4 B5 C6" */
    lwprintf_printf("% *K\r\n", (int)LWPRINTF_ARRAYSIZE(my_array), my_array);
    /* Variable length with uppercase letters and spaces, outputs "01 02 a4 b5 c6" */
    lwprintf_printf("% *k\r\n", (int)LWPRINTF_ARRAYSIZE(my_array), my_array);
}