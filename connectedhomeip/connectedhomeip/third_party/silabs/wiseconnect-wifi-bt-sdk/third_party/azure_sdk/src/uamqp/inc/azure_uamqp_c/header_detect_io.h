// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef HEADER_DETECT_IO_H
#define HEADER_DETECT_IO_H

#include "azure_c_shared_utility/xio.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/xio.h"

    typedef struct AMQP_HEADER_TAG
    {
        const unsigned char* header_bytes;
        size_t header_size;
    } AMQP_HEADER;

    typedef struct HEADER_DETECT_ENTRY_TAG
    {
        AMQP_HEADER header;
        const IO_INTERFACE_DESCRIPTION* io_interface_description;
    } HEADER_DETECT_ENTRY;

    typedef struct HEADER_DETECT_IO_CONFIG_TAG
    {
        XIO_HANDLE underlying_io;
        HEADER_DETECT_ENTRY* header_detect_entries;
        size_t header_detect_entry_count;
    } HEADER_DETECT_IO_CONFIG;

    MOCKABLE_FUNCTION(, AMQP_HEADER, header_detect_io_get_amqp_header);
    MOCKABLE_FUNCTION(, AMQP_HEADER, header_detect_io_get_sasl_amqp_header);
    MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, header_detect_io_get_interface_description);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HEADER_DETECT_IO_H */
