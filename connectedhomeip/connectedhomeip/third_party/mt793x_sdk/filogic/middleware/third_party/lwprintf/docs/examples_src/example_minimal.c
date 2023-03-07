#include "lwprintf/lwprintf.h"

/* Called for every character to be printed */
int
lwprintf_out(int ch, lwprintf_t* lwp) {
    /* May use printf to output it for test */
    if (ch != '\0') {
    	printf("%c", (char)ch);
    }

    return ch;
}

int
main(void) {
    /* Initialize default lwprintf instance with output function */
    lwprintf_init(lwprintf_out);

    /* Print first text */
    lwprintf_printf("Text: %d", 10);
}
