.. _thread_safety:

Thread safety
=============

LwPRINTF uses re-entrant functions, especially the one that format string to user application buffer.
It is fully allowed to access to the same LwPRINTF instance from multiple operating-system threads.

However, when it comes to direct print functions, such as :cpp:func:`lwprintf_printf_ex` (or any other similar),
calling those functions from multiple threads may introduce mixed output stream of data.

This is due to the fact that direct printing functions use same output function
to print single character. When called from multiple threads, one thread
may preempt another, causing strange output string.

.. literalinclude:: ../examples_src/example_multi_thread_corrupted_text.c
    :language: c
    :linenos:
    :caption: Multiple threads printing at the same time without thread-safety enabled

LwPRINTF therefore comes with a solution that introduces mutexes to lock print functions
when in use from within single thread context.

.. note::
    If application does not have any issues concerning mixed output,
    it is safe to disable OS support in OS environment.
    This will not have any negative effect on performance or memory corruption.

.. tip::
    To enable thread-safety support, parameter ``LWPRINTF_CFG_OS`` must be set to ``1``.
    Please check :ref:`api_lwprintf_opt` for more information about other options.

After thread-safety features has been enabled, it is necessary to implement
``4`` low-level system functions.

.. tip::
    System function template example is available in ``lwprintf/src/system/`` folder.

Example code for ``CMSIS-OS V2``

.. note::
    Check :ref:`api_lwprintf_sys` section for function description

.. literalinclude:: ../../lwprintf/src/system/lwprintf_sys_cmsis_os.c
    :language: c
    :linenos:
    :caption: System function implementation for CMSIS-OS based operating systems

.. toctree::
    :maxdepth: 2