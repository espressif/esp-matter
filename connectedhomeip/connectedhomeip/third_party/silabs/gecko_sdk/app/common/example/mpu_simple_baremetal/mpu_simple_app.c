/***************************************************************************//**
 * @file
 * @brief MPU Simple example functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "em_ramfunc.h" //Needed for SL_RAMFUNC_DECLARATOR macro
#include "sl_iostream.h"
#include "sl_iostream_init_instances.h"
#include "sl_iostream_handles.h"
#include "sl_mpu.h"

#define THUMB_BIT 0x00000001

/*
 * ARM functions are always word aligned and Thumb functions are always at least
 * half-word aligned. That means the THUMB_BIT of the address of a function in memory
 * is always 0. This bit is used by the CPU and the compiler to keep track of
 * whether the function is compiled in ARM or Thumb mode. When using function
 * pointers, the compiler will use X variant (BX, BLX, etc) to branch to the
 * function in order know in which mode it has to switch prior to executing the
 * function (basically, the THUMB_BIT of the address is copied in the T bit of the
 * CPU Control Register).
 */
#define IS_THUMB_FUNCTION(function) (((uint32_t) &function) & THUMB_BIT)

/*
 * When retrieving the address of a thumb function with the & operator, the
 * compiler returns its address with the THUMB_BIT set to 1. In order to use the &
 * operator for other thing than branching, like memcpy'ing the code, this
 * macro clears the THUMB_BIT
 */
#define THUMB_FUNC_PTR_TO_ADDR(function) ((void*) (((uint32_t) &function) & ~THUMB_BIT))

#define ADDR_TO_THUMB_FUNC_PTR(address) (((uint32_t)address) | THUMB_BIT)

typedef void (*exploit_payload_t)(void);

// This ram buffer will be used to simulate a code injection attack.
// Explicit 32bit alignment to support ARM and Thumb function.
__ALIGNED(4) uint8_t exploit_buffer[256];

/**
 * @brief Function executing from flash.
 *
 * This is an ordinary function executing from flash to demonstrate that
 * functions declared as such will execute normally without triggering MPU
 * fault when it is activated.
 */
static void function_executing_from_flash(void)
{
  printf("  Normal function executing from flash.\r\n");
}

/**
 * @brief Function executing from ram.
 *
 * For this example demonstration, we mark this function with the special
 * macro SL_RAMFUNC_DECLARATOR. It tells the compiler to put the code of this
 * function in the .ram section, which in turn, is placed in the RAM address
 * space (see auto-generated linkerfile.ld linker script). Functions placed in
 * the appropriate (.ram) section won't be denied execution from the MPU.
 */
SL_RAMFUNC_DECLARATOR static void function_executing_from_ram(void)
{
  printf("  Normal function executing from appropriate section in ram.\r\n");
}

/**
 * @brief Malicious function meant to be executed from unauthorized ram.
 *
 * This function is declared with necessary attribute in order to prevent the
 * compiler from potentially inserting a prolog/epilog. Because the Cortex Ms
 * do not have MMUs, compiled code is highly position dependent. This
 * function, written as it is, will be placed in flash. If the code is copied
 * elsewhere (like ram, in our example), then the compiled instruction's relative branching
 * will break and executing this function will eventually trigger and undefined instruction
 * exception. The following assembly snippet is a workaround for that.
 */
#if defined(__GNUC__)
  #define ASM_FUNCTION __attribute__((naked))
#elif defined(__ICCARM__)
  #define ASM_FUNCTION __stackless
#else
  #error "unsupported compiler"
#endif

ASM_FUNCTION static void exploit_payload(void)
{
  static const char* malicious_string = "\r\n  *** Malicious function executing from ram outside RAMFUNC. ***\r\n";

  __ASM volatile ("mov r0, %0\n" // Store the address of the malicious string in r0.
                                 // r0 is where the first argument to a function is placed

                  "mov pc, %1\n" // Instead of using branch instructions (B,BL,etc) which
                                 // relies on relative addresses from PC, we call printf
                                 // manually with its absolute address by placing it in the
                                 // program counter. This allows this function to be moved anywhere.

                  :              // No output
                  : "r" (malicious_string), "r" (printf)
                  :);

  // Because no branching with linking (BL) occurred when calling printf,
  // returning from printf will directly return from this function too.
}

exploit_payload_t inject_malicious_code_in_ram()
{
  void*      exploit_payload_address; // Real address of the malicious function
  exploit_payload_t exploit_function;           // Function pointer to the exploit buffer

  // Check in which mode is the function compiled.
  if (IS_THUMB_FUNCTION(exploit_payload)) {
    exploit_payload_address = THUMB_FUNC_PTR_TO_ADDR(exploit_payload);

    exploit_function = (exploit_payload_t) ADDR_TO_THUMB_FUNC_PTR(exploit_buffer);
  } else {
    exploit_payload_address = (void*) &exploit_payload;

    exploit_function = (exploit_payload_t) exploit_buffer;
  }

  // Copy the code of the malicious function in the exploit buffer.
  // In order not to alter the generated linker script to be able to
  // know the exact function code size, this lazy approach copies a way bigger
  // amount of data than the function code size (which is 16 bytes).
  memcpy(exploit_buffer, exploit_payload_address, sizeof(exploit_buffer));

  return exploit_function;
}

void mpu_simple_init(void)
{
  exploit_payload_t exploit_function;

  // Prevent buffering of output/input.
#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
  setvbuf(stdout, NULL, _IONBF, 0);   // Set unbuffered mode for stdout (newlib)
  setvbuf(stdin, NULL, _IONBF, 0);    // Set unbuffered mode for stdin (newlib)
#endif

  exploit_function = inject_malicious_code_in_ram();

  // By including the 'mpu' module in this project, the function
  // 'sl_mpu_disable_execute_from_ram()' was automatically called in
  // the auto-generated 'sl_service_init()' function. The MPU regions
  // are already properly configured and the MPU Enabled. For the sake
  // of demonstration, it is manually disabled here and re-enabled later
  // (its configuration stays untouched).
  ARM_MPU_Disable();

  printf("The MPU is not enabled, code execution is allowed everywhere\r\n");

  function_executing_from_flash();

  function_executing_from_ram();

  exploit_function();

  // Enable MPU with default background region
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

  printf("\r\nThe MPU is enabled, code execution is not authorized from ram outside .ram section.\r\n");

  function_executing_from_flash();

  function_executing_from_ram();

  // This call is supposed to trigger an MPU fault
  exploit_function();
}

/**
 * @brief Process action.
 *
 * This example is meant to conclude inside the abort handler
 * of the MPU fault. There is no action to process in the super loop.
 */
void mpu_simple_process_action(void)
{
}

/**
 * @brief The handler called when an MPU fault occurs.
 *
 * The MemManage_Handler exception handler executes when an MPU fault occurs.
 * It is defined in sl_mpu.c and the only thing it does is calling
 * mpu_fault_handler(void). The later function is defined as a weak symbol in
 * the same source file and it only performs a system reset; this redefinition
 * will have precedence over the __weakly__ defined one.
 *
 */
void mpu_fault_handler(void)
{
  printf("\r\n/!\\ The MPU has triggered a fault /!\\\r\n");

  while (1) {
  }
}
