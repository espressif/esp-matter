#include <stdio.h>
#include <stdint.h>
#include <string.h>

//#ifdef CONF_ENABLE_STACK_OVERFLOW_CHECK
#include <FreeRTOS.h>
#include <task.h>
//#endif

#include "panic.h"

#define STR(R)  	#R
#define DEF2STR(R)  STR(R)

#define VALID_IROM_START_XIP  (0x01000000)
#define VALID_FP_START_XIP    (0x02000000)
#define VALID_PC_START_XIP    (0x03000000)


int backtrace_now(int (*print_func)(const char *fmt, ...), uintptr_t *regs) __attribute__ ((weak, alias ("backtrace_riscv")));

#ifdef CONF_ENABLE_FRAME_PTR

/** WARNING: to use call trace facilities, enable
 *  compiler's frame pointer feature:
 *  -fno-omit-frame-pointer
 */


/* function call stack graph
 *
 *                                   low addr
 *                                  |___..___|<--- current sp
 *                                  |   ..   |
 *                                  |___..___|
 *                               $$$|___fp___|<------ previous fp
 * func_A frame start -->        $  |___ra___|
 *                               $  |        |<--- current fp
 *                               $  |   ..   |
 *                               $  |________|
 *                             ++$++|___fp___|
 * func_B stack start -->      + $  |___ra___|
 *                             + $$>|        |
 *                             +    |        |
 *                             +    |___..___|
 *                           ##+####|___fp___|
 * func_C stack start -->    # +    |___ra___|
 *                           # ++++>|        |
 *                           #       high addr
 *                           ######>
 *
 *                   func_C () {
 *                      ...
 *                      func_B();
 *                      ...
 *                   }
 *
 *                   func_B() {
 *                      ...
 *                      func_A();
 *                      ...
 *                   }
 */

static void backtrace_stack(int (*print_func)(const char *fmt, ...),
                            uintptr_t *fp, uintptr_t *regs)
{
    uintptr_t *ra;
    uint32_t i = 0; 

    while (1) {
        ra = (uintptr_t *)*(unsigned long *)(fp - 1);

        if (ra == 0) {
            print_func("backtrace: INVALID!!!\r\n");
            break;
        }

        print_func("backtrace: %p\r\n", ra);
        if (1 == i)
            print_func("backtrace: %p   <--- TRAP\r\n", regs[0]);

        fp = (uintptr_t *)*(fp - 2);

        i ++;
    }
}

int backtrace_riscv(int (*print_func)(const char *fmt, ...), uintptr_t *regs)
{
    static int     processing_backtrace = 0;
    uintptr_t *fp;

    if (processing_backtrace == 0) {
        processing_backtrace = 1;
    } else {
        print_func("backtrace nested...\r\n");
        return -1;
    }

    __asm__("add %0, x0, fp" : "=r"(fp));

    print_func("=== backtrace start ===\r\n");

    backtrace_stack(print_func, fp, regs);

    print_func("=== backtrace end ===\r\n\r\n");

    processing_backtrace = 0;

    return 0;
}

static inline void backtrace_stack_app(int (*print_func)(const char *fmt, ...), unsigned long *fp, int depth) {
  uintptr_t *pc;

  while (depth--) {
    if ((((uintptr_t)fp & 0xff000000ul) != VALID_PC_START_XIP) && (((uintptr_t)fp & 0xff000000ul) != VALID_FP_START_XIP)) {
      print_func("!!\r\n");
      return;
    }

    pc = (uintptr_t *)fp[-1];

    if ((((uintptr_t)pc & 0xff000000ul) != VALID_PC_START_XIP) && (((uintptr_t)pc & 0xff000000ul) != VALID_FP_START_XIP)) {
      print_func("!!\r\n");
      return;
    }

    if ((unsigned long)pc > VALID_FP_START_XIP) {
      /* there is a function that does not saved ra,
      * skip!
      * this value is the next fp
      */
      fp = (unsigned long *)pc;
    } else if ((uintptr_t)pc > VALID_PC_START_XIP) {
      print_func("backtrace: %p\r\n", pc);
      fp = (unsigned long *)fp[-2];

      if (pc == (uintptr_t *)0) {
        break;
        print_func("\r\n");
      }
    }
  }
}

int backtrace_now_app(int (*print_func)(const char *fmt, ...)) {
  static int processing_backtrace = 0;
  unsigned long *fp;

  if (processing_backtrace == 0) {
    processing_backtrace = 1;
  } else {
    print_func("backtrace nested...\r\n");
    return 0;
  }

#if defined(__GNUC__)
  __asm__("add %0, x0, fp"
	  : "=r"(fp));
#else
#error "Compiler is not gcc!"
#endif

  print_func(">> ");
  backtrace_stack_app(print_func, fp, 256);
  print_func(" <<\r\n");

  processing_backtrace = 0;

  return 0;
}

#else
int backtrace_riscv(int (*print_func)(const char *fmt, ...), uintptr_t *regs)
{
    return -1;
}
#endif

#ifdef CONF_ENABLE_STACK_OVERFLOW_CHECK
extern StackType_t *xTaskGetStackBase(TaskHandle_t xTask);
extern uint32_t hal_boot2_get_flash_addr(void);
void __attribute__((no_instrument_function)) __cyg_profile_func_enter(void *this_fn, void *call_site) {
  register uintptr_t *sp;
  uintptr_t *stack_base;
  
  // this function may called before .data section initialized, skip it
  if (this_fn == hal_boot2_get_flash_addr) {
    return;
  }

  // inside these functions MUST NOT call other functions
  if (this_fn == xTaskGetSchedulerState|| this_fn == xTaskGetStackBase
      || this_fn == xPortIsInsideInterrupt) {
    return;
  }

  // if schedule not start or in the interrupt
  if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING || xPortIsInsideInterrupt() == 1) {
    return;
  } 

  __asm__("add %0, x0, sp" : "=r"(sp));
  stack_base = (uintptr_t *)xTaskGetStackBase(NULL);
  if (stack_base != NULL && sp <= stack_base){
    portDISABLE_INTERRUPTS();
    while(1);
  }
  return;
}

void __attribute__((no_instrument_function)) __cyg_profile_func_exit(void *this_fn, void *call_site) {
  return;
}
#endif

extern BaseType_t TrapNetCounter;
BaseType_t xPortIsInsideInterrupt( void )
{
    return TrapNetCounter ? 1 : 0;
}


#ifdef CONF_ENABLE_FUNC_BACKTRACE_ELF

#define portasmADDITIONAL_CONTEXT_SIZE 34

static void backtrace_task(int (*print_func)(const char *s), char ptAddr[][12], uintptr_t *fp, int depth)
{
	uintptr_t *ra;
	uintptr_t *cur_fp = fp;
	int pt_num = 0;

	while (depth && (((uintptr_t) cur_fp & 0xf000000ul) == VALID_FP_START_XIP)) {
		ra = (uintptr_t*) cur_fp[-1];
		if ((((uintptr_t) ra & 0x0f000000ul) != VALID_PC_START_XIP)
			&& (((uintptr_t) ra & 0x0f000000ul) != VALID_FP_START_XIP)
			&& (((uintptr_t) ra & 0x0f000000ul) != VALID_IROM_START_XIP)) {
			break;
		}

		snprintf(&ptAddr[pt_num][0], sizeof(ptAddr[0]), "%p", ra);
		print_func("    ");
		print_func((char *)&ptAddr[pt_num++][0]);
		print_func("\r\n");

		cur_fp = (uintptr_t*) cur_fp[-2];
		depth--;
	}
}



int backtrace_now_task(int (*print_func)(const char *s), uintptr_t *regs)
{
	TaskHandle_t pxFirstTCB, pxNextTCB;
	List_t *pxAllList;
	StackType_t *pxTopOfStack;
	uintptr_t *fp, *reg;
	char ptAddr[16][12];
	int pt_num = 0;
	static char log[]=	"\r\n==========\r\n"
						"ELF File: "DEF2STR(CONF_BUILD_PATH)"\r\n"
						"Visit https://dev.bouffalolab.com/media/doc/backtrace/html/QA/backtrace/backtrace.html for more detail\r\n";

	print_func(log);

	taskENTER_CRITICAL();
	/* Get num of task */
	xAddTasksToAllList();
	pxAllList = pxTaskGetAllList();

	if (pxAllList->uxNumberOfItems > (UBaseType_t) 0)
	{
		listGET_OWNER_OF_NEXT_ENTRY(pxFirstTCB, pxAllList);
		do
		{
			listGET_OWNER_OF_NEXT_ENTRY(pxNextTCB, pxAllList);
			pxTopOfStack = (StackType_t*) (*(StackType_t*) pxNextTCB);

			/* Clear buf */
			pt_num = 0;
			memset(ptAddr, 0xA5, sizeof(ptAddr));

			print_func("==========\r\nTask name:");
			print_func(pcTaskGetName(pxNextTCB));
			print_func("\r\nBacktrace:\r\n");

			/* Get fp */
			fp = (uintptr_t*) (pxTopOfStack + portasmADDITIONAL_CONTEXT_SIZE + 5);
			fp = (uintptr_t*) *fp;

			/* Get pc reg */
			reg = (uintptr_t*) (*(StackType_t*) pxTopOfStack);
			snprintf(ptAddr[pt_num], sizeof(ptAddr[0]), "%p", reg);
			print_func("    ");
			print_func(ptAddr[pt_num++]);
			print_func("\r\n");

			/* Get ra reg */
			reg = (uintptr_t*) pxTopOfStack + portasmADDITIONAL_CONTEXT_SIZE + 1;
			reg = (uintptr_t*) *reg;
			snprintf(ptAddr[pt_num], sizeof(ptAddr[0]), "%p", reg);
			print_func("    ");
			print_func(ptAddr[pt_num++]);
			print_func("\r\n");

			/* Find task call information */
			backtrace_task(print_func, &ptAddr[pt_num], fp, 14);

          char proj_elf[] = DEF2STR(CONF_ENABLE_FUNC_BACKTRACE_ELF);
          print_func("CMD: riscv64-unknown-elf-addr2line -e ");
          print_func(proj_elf);
          print_func(" -a -f ");
          for (int i = 0; ptAddr[i][1] == 'x'; i++)
          {
              print_func(ptAddr[i]);
              print_func(" ");
          }
          print_func("\r\n");

		} while (pxNextTCB != pxFirstTCB);
	}
	
	return 0;
}
#else
int backtrace_now_task(int (*print_func)(const char *s), uintptr_t *regs)
{
	return -1;
}
#endif
