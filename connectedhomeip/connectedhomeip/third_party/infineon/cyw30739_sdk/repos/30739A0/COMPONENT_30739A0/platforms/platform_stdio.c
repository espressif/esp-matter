/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/**
 * @file
 * @brief Implement platform functions required by standard buffered input/output functions
 */
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wiced.h>

#include <wiced_hal_duart.h>

int _close(int __fildes)
{
    UNUSED_VARIABLE(__fildes);

    errno = ENOSYS;
    return -1;
}

int _isatty(int __fildes)
{
    UNUSED_VARIABLE(__fildes);

    errno = ENOSYS;
    return 0;
}

_off_t _lseek(int __fildes, _off_t __offset, int __whence)
{
    UNUSED_VARIABLE(__fildes);
    UNUSED_VARIABLE(__offset);
    UNUSED_VARIABLE(__whence);

    errno = ENOSYS;
    return -1;
}

int _read(int __fd, void *__buf, size_t __nbyte)
{
    UNUSED_VARIABLE(__fd);
    UNUSED_VARIABLE(__buf);
    UNUSED_VARIABLE(__nbyte);

    errno = ENOSYS;
    return -1;
}


int _write(int __fd, const void *__buf, size_t __nbyte)
{
    UNUSED_VARIABLE(__fd);

    wiced_hal_duart_write((uint8_t *) __buf, __nbyte);

    return __nbyte;
}

int	_fstat(int __fd, struct stat *__sbuf)
{
    UNUSED_VARIABLE(__fd);
    UNUSED_VARIABLE(__sbuf);

    errno = ENOSYS;
    return -1;
}
