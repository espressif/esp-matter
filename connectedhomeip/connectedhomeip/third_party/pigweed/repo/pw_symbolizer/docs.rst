.. _module-pw_symbolizer:

=============
pw_symbolizer
=============

.. warning::
  This module is under construction and may not be ready for use.

pw_symbolizer provides python-based tooling for symbolizing addresses emitted by
on-device firmware.

-----
Usage
-----
Symbolizer
==========
The ``Symbolizer`` abstract base class is an interface for translating addresses
to human-readable source locations. Different architectures and operating
systems can require vastly different implementations, so this interface is
provided to allow Pigweed tooling to symbolize addresses without requiring
Pigweed to provide explicit support for all possible implementations.

``Symbolizer`` Also provides a helper function for producing nicely formatted
stack trace style dumps.

.. code:: py

  import pw_symbolizer

  symbolizer = pw_symbolizer.LlvmSymbolizer(Path('device_fw.elf'))
  print(symbolizer.dump_stack_trace(backtrace_addresses))

Which produces output like this:

.. code-block:: none

  Stack Trace (most recent call first):
     1: at device::system::logging_thread_context (0x08004BE0)
        in threads.cc:0
     2: at device::system::logging_thread (0x0800B508)
        in ??:?
     3: at device::system::logging_thread_context (0x08004CB8)
        in threads.cc:0
     4: at device::system::logging_thread (0x0800B3C0)
        in ??:?
     5: at device::system::logging_thread (0x0800B508)
        in ??:?
     6: at (0x0800BAF7)
        in ??:?
     7: at common::log::LoggingThread::Run() (0x0800BAD1)
        in out/common/log/logging_thread.cc:26
     8: at pw::thread::threadx::Context::ThreadEntryPoint(unsigned long) (0x0800539D)
        in out/pigweed/pw_thread_threadx/thread.cc:41
     9: at device::system::logging_thread_context (0x08004CB8)
        in threads.cc:0
    10: at device::system::logging_thread_context (0x08004BE0)
        in threads.cc:0

FakeSymbolizer
==============
The ``FakeSymbolizer`` is utility class that implements the ``Symbolizer``
interface with a fixed database of address to ``Symbol`` mappings. This is
useful for testing, or as a no-op ``Symbolizer``.

.. code:: py

  import pw_symbolizer

  known_symbols = (
      pw_symbolizer.Symbol(0x0800A200, 'foo()', 'src/foo.c', 41),
      pw_symbolizer.Symbol(0x08000004, 'boot_entry()', 'src/vector_table.c', 5),
  )
  symbolizer = pw_symbolizer.FakeSymbolizer(known_symbols)
  sym = symbolizer.symbolize(0x0800A200)
  print(f'This fake symbolizer knows about: {sym}')

LlvmSymbolizer
==============
The ``LlvmSymbolizer`` is a python layer that wraps ``llvm-symbolizer`` to
produce symbols from provided addresses. This module will only work if
``llvm-symbolizer`` is available on the system ``PATH``.

.. code:: py

  import pw_symbolizer

  symbolizer = pw_symbolizer.LlvmSymbolizer(Path('device_fw.elf'))
  sym = symbolizer.symbolize(0x2000ac21)
  print(f'You have a bug here: {sym}')
