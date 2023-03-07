#include "pet_decode.h"
#include "main_entry.h"

bool fuzz_one_input(const uint8_t *data, size_t size)
{
    uint32_t payload_len_out = 0;
    struct Pet result;
    bool ret = cbor_decode_Pet(data, size,
                               &result,
                               &payload_len_out);
    return ret;
}
