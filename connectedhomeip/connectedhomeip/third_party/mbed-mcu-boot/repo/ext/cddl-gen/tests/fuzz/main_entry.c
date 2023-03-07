
#include "main_entry.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#ifdef LIBFUZZER
    int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
        (void) fuzz_one_input(data, size);
        return 0;
    }
#else

    #ifndef AFL_MAX_SIZE
        #define AFL_MAX_SIZE 8192
    #endif

    static uint8_t tmp_buffer[AFL_MAX_SIZE];

    static size_t afl_read(uint8_t **data) {

        ssize_t size = read(0, tmp_buffer, AFL_MAX_SIZE);
        if (size <= 0) {
            return 0;
        }
        *data = (uint8_t *) malloc(size);
        if (NULL == *data) {
            return 0;
        }
        memcpy(*data, tmp_buffer, size);
        return size;
    }

    #define AFL_READ_AND_EXECUTE                \
        do {                                    \
            size = afl_read(&data);             \
            if (NULL == data || 0 == size) {    \
                return 0;                       \
            }                                   \
            ret = fuzz_one_input(data, size);   \
            free(data);                         \
        } while (0)

    int main(void) {

        size_t size;
        uint8_t *data = NULL;
        bool ret;

        #ifdef __AFL_LOOP
        while (__AFL_LOOP(1000)) {
            AFL_READ_AND_EXECUTE;
        }
        return 0;
        #else
            AFL_READ_AND_EXECUTE;
            return ret ? 0 : -1;
        #endif
    }
#endif
