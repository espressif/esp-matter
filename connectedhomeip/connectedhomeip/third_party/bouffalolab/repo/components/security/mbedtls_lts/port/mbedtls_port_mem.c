#include "mbedtls_port_platform.h"

#include <string.h>
#include <FreeRTOS.h>

void *mbedtls_port_calloc( size_t nmemb, size_t size )
{
    const size_t sz = nmemb * size;
    void *m = pvPortMalloc( sz );
    if ( m )
        memset( m, 0, sz );
    return( m );
}
