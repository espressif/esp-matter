
    .syntax unified
    .thumb

    .weak hardfault_handler, memmanage_handler, busfault_handler, usagefault_handler, svcall_handler, debugmon_handler, pendsv_handler_impl, systick_handler_impl

    .align 1
    .text

    .section .text.unexpected_fault, "ax", %progbits
    .thumb_func
    .type   unexpected_fault, %function
hardfault_handler:
memmanage_handler:
busfault_handler:
usagefault_handler:
svcall_handler:
debugmon_handler:
pendsv_handler_impl:
systick_handler_impl:
unexpected_fault:
    mov         r0, sp
    push        {r4-r11}
    mov         r1, sp
    b           unexpected_fault_validsp
