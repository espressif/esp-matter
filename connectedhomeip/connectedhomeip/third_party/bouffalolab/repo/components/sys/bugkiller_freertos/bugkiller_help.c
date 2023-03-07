/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>

struct bugkiller_command {
    char *name;
    char *help;
    char *example;
};
struct bugkiller_command __attribute__((used, section(".static_bugkiller_command"))) bg_help[] = {
 {
             "task_dump",
	     "list all task",
	     "call bugkiller_task_dump ()",
 },
 {
             "uart_dump",
	     "dump uart info, (need remap uart coredump section to another address)",
	     "call bugkiller_uart_dump ()",
 },
 {
             "switch_task",
	     "switch regs to interrupted time point, specify $sp of interrupted task",
	     "call bugkiller_switch_task (pxCurrentTCB->pxTopOfStack)",
 },
 {
             "sem_dump",
	     "dump semaphore",
	     "call bugkiller_sem_dump (Queue_t *res)",
 },
 {
             "softtimer_dump",
	     "dump softtimer",
	     "call bugkiller_softtimer_dump (Timer_t *res)",
 },
 {
             "streambuffer_dump",
	     "dump streambuffer",
	     "call bugkiller_streambuffer_dump (StreamBufferHandle_t res)",
 },
 {
             "eventgroup_dump",
	     "dump eventgroup",
	     "call bugkiller_eventgroup_dump (EventGroup_t *res)",
 },
 {
             "heap_dump",
	     "dump heap",
	     "call bugkiller_heap_dump ()",
 },
 {
             "sockets_dump",
	     "dump sockets",
	     "call bugkiller_sockets_dump ()",
 },
 {
             "help",
	     "list all command usage",
	     "call bugkiller_help ()",
 },
};
void bugkiller_help(void)
{
    extern char __bugkiller_command_start, __bugkiller_command_end;
    struct bugkiller_command *start = &__bugkiller_command_start;
    struct bugkiller_command *end = &__bugkiller_command_end;
    while(start < end) {
	printf("name: %s\r\nhelp: %s\r\nexample: %s\r\n\r\n", start->name, start->help, start->example);
	start++;
    }
}
