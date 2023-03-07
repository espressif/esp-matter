#include "common.h"

#include <bl_sec.h>

#if defined(MBEDTLS_ENTROPY_C)

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    bl_rand_stream( output, len );
    *olen = len;

    return( 0 );
}
#endif

#endif /* MBEDTLS_ENTROPY_C */
