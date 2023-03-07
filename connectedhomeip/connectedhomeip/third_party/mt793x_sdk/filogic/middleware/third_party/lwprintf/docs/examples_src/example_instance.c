#include "lwprintf/lwprintf.h"

/* Define application custom instance */
lwprintf_t custom_instance;

/* Define custom output function for print */
int
custom_out(int ch, lwprintf_t* p) {
    /* Do whatever with this character */
    if (ch == '\0') {
        /* This is end of string in current formatting */
        /* Maybe time to start DMA transfer? */
    } else {
        /* Print or send character */
    }

    /* Return character to proceed */
    return ch;
}

/* Define output function for default instance */
int
default_out(int ch, lwprintf_t* p) {
    /* Print function for default instance */

    /* See custom_out function for implementation details */
}

int
main(void) {
    /* Initialize default lwprintf instance with output function */
    lwprintf_init(default_out);
    /* Initialize custom lwprintf instance with output function */
    lwprintf_init_ex(&custom_instance, custom_out);

    /* Print first text over default output */
    lwprintf_printf("Text: %d", 10);
    /* Print text over custom instance */
    lwprintf_printf_ex(&custom_instance, "Custom: %f", 3.2f);
}
