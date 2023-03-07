.. _module-pw_system:

=========
pw_system
=========
.. warning::
  This module is an early work-in-progress towards an opinionated framework for
  new projects built on Pigweed. It is under active development, so stay tuned!

pw_system is quite different from typical Pigweed modules. Rather than providing
a single slice of vertical functionality, pw_system pulls together many modules
across Pigweed to construct a working system with RPC, Logging, an OS
Abstraction layer, and more. pw_system exists to greatly simplify the process
of starting a new project using Pigweed by drastically reducing the required
configuration space required to go from first signs of on-device life to a more
sophisticated production-ready system.

Trying out pw_system
====================
If you'd like to give pw_system a spin and have a STM32F429I Discovery board,
refer to the board's
:ref:`target documentation<target-stm32f429i-disc1-stm32cube>` for instructions
on how to build the demo and try things out

If you don't have a discovery board, there's a simulated device variation that
you can run on your local machine with no additional hardware. Check out the
steps for trying this out :ref:`here<target-host-device-simulator>`.

Target Bringup
==============
Bringing up a new device is as easy as 1-2-3! (Kidding, this is a work in
progress)

#. **Create a ``pw_system_target`` in your GN build.**
   This is what will control the configuration of your target from a build
   system level. This includes which compiler will be used, what architecture
   flags will be used, which backends will be used, and more. A large quantity
   of configuration will be pre-set to work with pw_system after you select the
   CPU and scheduler your target will use, but your target will likely need to
   set a few other things to get to a fully working state.
#. **Write target-specific initialization.**
   Most embedded devices require a linker script, manual initialization of
   memory, and some clock initialization. pw_system leaves this to users to
   implement as the exact initialization sequence can be very project-specific.
   All that's required is that after early memory initialization and clock
   configuration is complete, your target initialization should call
   ``pw::system::Init()`` and then start the RTOS scheduler (e.g.
   ``vTaskStartScheduler()``).
#. **Implement ``pw::system::UserAppInit()`` in your application.**
   This is where most of your project's application-specific logic goes. This
   could be starting threads, registering RPC services, turning on Bluetooth,
   or more. In ``UserAppInit()``, the RTOS will be running so you're free to use
   OS primitives and use features that rely on threading (e.g. RPC, logging).

Pigweed's ``stm32f429i_disc1_stm32cube`` target demonstrates what's required by
the first two steps. The third step is where you get to decide how to turn your
new platform into a project that does something cool! It might be as simple as
a blinking LED, or something more complex like a Bluetooth device that brews you
a cup of coffee whenever ``pw watch`` kicks off a new build.

.. note::
  Because of the nature of the hard-coded conditions in ``pw_system_target``,
  you may find that some options are missing for various RTOSes and
  architectures. The design of the GN integration is still a work-in-progress
  to improve the scalability of this, but in the meantime the Pigweed team
  welcomes contributions to expand the breadth of RTOSes and architectures
  supported as ``pw_system_target``\s.

GN Target Toolchain Template
============================
This module includes a target toolchain template called ``pw_system_target``
that reduces the amount of work required to declare a target toolchain with
pre-selected backends for pw_log, pw_assert, pw_malloc, pw_thread, and more.
The configurability and extensibility of this template is relatively limited,
as this template serves as a "one-size-fits-all" starting point rather than
being foundational infrastructure.

.. code-block::

  # Declare a toolchain with suggested, compiler, compiler flags, and default
  # backends.
  pw_system_target("stm32f429i_disc1_stm32cube_size_optimized") {
    # These options drive the logic for automatic configuration by this
    # template.
    cpu = PW_SYSTEM_CPU.CORTEX_M4F
    scheduler = PW_SYSTEM_SCHEDULER.FREERTOS

    # Optionally, override pw_system's defaults to build with clang.
    system_toolchain = pw_toolchain_arm_clang

    # The pre_init source set provides things like the interrupt vector table,
    # pre-main init, and provision of FreeRTOS hooks.
    link_deps = [ "$dir_pigweed/targets/stm32f429i_disc1_stm32cube:pre_init" ]

    # These are hardware-specific options that set up this particular board.
    # These are declared in ``declare_args()`` blocks throughout Pigweed. Any
    # build arguments set by the user will be overridden by these settings.
    build_args = {
      pw_third_party_freertos_CONFIG = "$dir_pigweed/targets/stm32f429i_disc1_stm32cube:stm32f4xx_freertos_config"
      pw_third_party_freertos_PORT = "$dir_pw_third_party/freertos:arm_cm4f"
      pw_sys_io_BACKEND = dir_pw_sys_io_stm32cube
      dir_pw_third_party_stm32cube = dir_pw_third_party_stm32cube_f4
      pw_third_party_stm32cube_PRODUCT = "STM32F429xx"
      pw_third_party_stm32cube_CONFIG =
          "//targets/stm32f429i_disc1_stm32cube:stm32f4xx_hal_config"
      pw_third_party_stm32cube_CORE_INIT = ""
      pw_boot_cortex_m_LINK_CONFIG_DEFINES = [
        "PW_BOOT_FLASH_BEGIN=0x08000200",
        "PW_BOOT_FLASH_SIZE=2048K",
        "PW_BOOT_HEAP_SIZE=7K",
        "PW_BOOT_MIN_STACK_SIZE=1K",
        "PW_BOOT_RAM_BEGIN=0x20000000",
        "PW_BOOT_RAM_SIZE=192K",
        "PW_BOOT_VECTOR_TABLE_BEGIN=0x08000000",
        "PW_BOOT_VECTOR_TABLE_SIZE=512",
      ]
    }
  }

  # Example for the Emcraft SmartFusion2 system-on-module
  pw_system_target("emcraft_sf2_som_size_optimized") {
    cpu = PW_SYSTEM_CPU.CORTEX_M3
    scheduler = PW_SYSTEM_SCHEDULER.FREERTOS

    link_deps = [ "$dir_pigweed/targets/emcraft_sf2_som:pre_init" ]
    build_args = {
      pw_log_BACKEND = dir_pw_log_basic #dir_pw_log_tokenized
      pw_tokenizer_GLOBAL_HANDLER_WITH_PAYLOAD_BACKEND = "//pw_system:log"
      pw_third_party_freertos_CONFIG = "$dir_pigweed/targets/emcraft_sf2_som:sf2_freertos_config"
      pw_third_party_freertos_PORT = "$dir_pw_third_party/freertos:arm_cm3"
      pw_sys_io_BACKEND = dir_pw_sys_io_emcraft_sf2
      dir_pw_third_party_smartfusion_mss = dir_pw_third_party_smartfusion_mss_exported
      pw_third_party_stm32cube_CONFIG =
          "//targets/emcraft_sf2_som:sf2_mss_hal_config"
      pw_third_party_stm32cube_CORE_INIT = ""
      pw_boot_cortex_m_LINK_CONFIG_DEFINES = [
        "PW_BOOT_FLASH_BEGIN=0x00000200",
        "PW_BOOT_FLASH_SIZE=200K",

        # TODO(b/235348465): Currently "pw_tokenizer/detokenize_test" requires at
        # least 6K bytes in heap when using pw_malloc_freelist. The heap size
        # required for tests should be investigated.
        "PW_BOOT_HEAP_SIZE=7K",
        "PW_BOOT_MIN_STACK_SIZE=1K",
        "PW_BOOT_RAM_BEGIN=0x20000000",
        "PW_BOOT_RAM_SIZE=64K",
        "PW_BOOT_VECTOR_TABLE_BEGIN=0x00000000",
        "PW_BOOT_VECTOR_TABLE_SIZE=512",
      ]
    }
  }


Metrics
=======
The log backend is tracking metrics to illustrate how to use pw_metric and
retrieve them using `Device.get_and_log_metrics()`.