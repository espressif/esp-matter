.. _lwprintf_instances:

LwPRINTF instances
==================

LwPRINTF is very flexible and allows multiple instances for output print functions.

.. note::
    Multiple instances with LwPRINTF are useful only with direct
    print functions, suchs as :c:macro:`lwprintf_printf`.
    If application uses only format functions which write to input buffer,
    it may always use default LwPRINTF instance which is
    created by the library itself

Use of different instances is useful if application needs different print
configurations. Each instance has its own ``print_output`` function,
allowing application to use multiple debug configurations (as an example)

.. tip::
    Use functions with ``_ex`` suffix to direcly work with custom instances.
    Functions without ``_ex`` suffix use default LwPRINTF instance

.. literalinclude:: ../examples_src/example_instance.c
    :language: c
    :linenos:
    :caption: Custom LwPRINTF instance for output

.. note::
    It is perfectly valid to use single output function for all application instances.
    Use check against input parameter for :c:type:`lwprintf_t` if it matches your custom LwPRINTF instance memory address

.. literalinclude:: ../examples_src/example_instance_single_func.c
    :language: c
    :linenos:
    :caption: Single output function for all LwPRINTF instances

.. toctree::
    :maxdepth: 2