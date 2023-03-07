#include <string.h>

void *memmove(void *dest, void const *src, size_t n)
{
    register char *dp = dest;
    register char const *sp = src;
    if (dp < sp) {
        while(n-- > 0) {
            *(dp++) = *(sp++);
        }
    } else {
        dp += n;
        sp += n;
        while(n-- > 0) {
            *(--dp) = *(--sp);
        }
    }

    return dest;
}
