.. _module-pw_assert_basic:

===============
pw_assert_basic
===============

--------
Overview
--------
This is a simple assert backend to implement the ``pw_assert`` facade which
relies on a single function ``pw_assert_basic_HandleFailure`` handler facade
which defaults to the ``basic_handler`` backend. Users may be interested in
overriding this default in case they need to do things like transition to
crash time logging or implementing application specific reset and/or hang
behavior.

.. attention::

  This log backend comes with a very large ROM and potentially RAM cost. It is
  intended mostly for ease of initial bringup. We encourage teams to use
  tokenized asserts since they are much smaller both in terms of ROM and RAM.

.. _module-pw_assert_basic-custom_handler:

Custom handler backend example
------------------------------
Here is a typical usage example implementing a simple handler backend which uses
a UART backed sys_io implementation to print during crash time and then reboots.
Note that this example uses CMSIS and a psuedo STM HAL, as a backend implementer
you are responsible for using whatever APIs make sense for your use case(s).

.. code-block:: cpp

  #include "cmsis.h"
  #include "hal.h"
  #include "pw_string/string_builder.h"

  using pw::sys_io::WriteLine;

  extern "C" void pw_assert_basic_HandleFailure(
      [[maybe_unused]] const char* file_name,
      [[maybe_unused]] int line_number,
      [[maybe_unused]] const char* function_name,
      const char* message,
      ...) {
    // Global interrupt disable for a single core microcontroller.
    __disable_irq();

    // Re-initialize the UART to ensure it's safe to use at crash time.
    HAL_UART_DeInit(sys_io_uart);
    HAL_UART_Init(sys_io_uart);

    WriteLine(
        "  Welp, that didn't go as planned. "
        "It seems we crashed. Terribly sorry! Assert reason:");
    {
      pw::StringBuffer<150> buffer;
      buffer << "     ";
      va_list args;
      va_start(args, format);
      buffer.FormatVaList(format, args);
      va_end(args);
      WriteLine(buffer.view());
    }

    // Reboot the microcontroller.
    HAL_NVIC_SystemReset();
  }
