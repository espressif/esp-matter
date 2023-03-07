/***************************************************************************//**
 * @brief Zigbee - simple stack test application - main.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_component_catalog.h"
#include "sl_system_init.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_system_kernel.h"
#else
#include "sl_system_process_action.h"
#endif  // SL_CATALOG_KERNEL_PRESENT

#include PLATFORM_HEADER
#include "ember.h"

#ifdef EMBER_TEST
#include "serial/serial.h"
#endif

#ifdef UNIX_HOST
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
extern void app_process_args(int argc, char *argv[]);
#ifdef __linux__
#include <sys/prctl.h>
#endif

jmp_buf gResetJump;

void __gcov_flush();
#endif //UNIX_HOST

// From platform/base/phy/simulation/security.h:
extern uint8_t simulatorEncryptionType;

void app_init(void)
{
#if defined(EMBER_TEST)
  // In simulation we still rely on the ember serial driver.
  assert(emberSerialInit((uint8_t)APP_SERIAL,
                         (SerialBaudRate)APP_BAUD_RATE,
                         (SerialParity)PARITY_NONE,
                         1) == EMBER_SUCCESS);
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_init();
#endif

#if defined(UNIX_HOST)
  simulatorEncryptionType = 0;   // USE_REAL_ENCRYPTION
#endif
}

void app_process_action(void)
{
}

#ifdef UNIX_HOST
int main(int argc, char *argv[])
#elif EMBER_TEST
int nodeMain(void)
#else
int main(void)
#endif
{
#ifdef UNIX_HOST
#if defined(SL_CATALOG_ZIGBEE_MSLA_CHECK_PRESENT)
  if ( access("/accept_silabs_msla", F_OK) != 0 ) {
    fprintf(stderr, "In order to run this software, you must accept the Silicon Labs MSLA found at https://www.silabs.com/about-us/legal/master-software-license-agreement by creating a file called /accept_silabs_msla.\n");
    return -1;
  }
#endif
  fprintf(stdout, "By using this software, you are agreeing to the Silicon Labs MSLA found at https://www.silabs.com/about-us/legal/master-software-license-agreement.\n");

  app_process_args(argc, argv);

  if (setjmp(gResetJump)) {
    fprintf(stderr, "Restarting\n");
    execvp(argv[0], argv);
  }
#endif //UNIX_HOST

  // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
  // Note that if the kernel is present, processing task(s) will be created by
  // this call.
  sl_system_init();

  // Initialize the application. For example, create periodic timer(s) or
  // task(s) if the kernel is present.
  app_init();

#if defined(SL_CATALOG_KERNEL_PRESENT)
  // Start the kernel. Task(s) created in app_init() will start running.
  sl_system_kernel_start();
#else // SL_CATALOG_KERNEL_PRESENT
  while (1) {
    // Do not remove this call: Silicon Labs components process action routine
    // must be called from the super loop.
    sl_system_process_action();

    // Application process.
    app_process_action();

    // Let the CPU go to sleep if the system allow it.
    #if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    sl_power_manager_sleep();
    #endif
  }
#endif // SL_CATALOG_KERNEL_PRESENT

  return 0;
}
