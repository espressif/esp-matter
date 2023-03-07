#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <openthread/platform/entropy.h>
#include <openthread/platform/radio.h>
#include <bl_sec.h>

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength) 
{
    bl_rand_stream(aOutput, aOutputLength);
    return OT_ERROR_NONE;
}
