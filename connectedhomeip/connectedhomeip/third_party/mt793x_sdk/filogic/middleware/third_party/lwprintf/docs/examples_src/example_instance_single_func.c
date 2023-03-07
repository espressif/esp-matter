#include "lwprintf/lwprintf.h"

/* Define application custom instance */
lwprintf_t custom_instance1;
lwprintf_t custom_instance2;

/* Define custom output function for print */
int
my_out(int ch, lwprintf_t* p) {
    if (p == &custom_instance1) {
        /* This is custom instance 1 */
    } else if (p == &custom_instance2) {
        /* This is custom instance 2 */
    } else {
        /* This is default instance */
    }
    return ch;
}

int
main(void) {
    /* Initialize default lwprintf instance with output function */
    lwprintf_init(my_out);
    lwprintf_init_ex(&custom_instance1, my_out);
    lwprintf_init_ex(&custom_instance2, my_out);

    /* Use print functions ... */
}
