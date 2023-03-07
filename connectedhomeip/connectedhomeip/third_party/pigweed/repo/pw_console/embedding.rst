.. _module-pw_console-embedding:

===============
Embedding Guide
===============

-------------
Using embed()
-------------
``pw console`` is invoked by calling ``PwConsoleEmbed().embed()`` in your
own Python script. For a complete example of an embedded device console script see
:bdg-link-primary-line:`pw_system/py/pw_system/console.py <https://cs.pigweed.dev/pigweed/+/main:pw_system/py/pw_system/console.py>`.

.. automodule:: pw_console.embed
    :members: PwConsoleEmbed
    :undoc-members:
    :show-inheritance:

.. _module-pw_console-embedding-logstore:

.. autoclass:: pw_console.log_store.LogStore
    :members: __init__
    :undoc-members:
    :show-inheritance:

.. _module-pw_console-embedding-plugins:

Adding Plugins
==============
User plugin instances are created before starting-up and passed to the Pigweed
Console embed instance. Typically, a console is started by creating a
``PwConsoleEmbed()`` instance, calling customization functions, then calling
``.embed()`` as shown in `Using embed()`_. Adding plugins functions similarly by
calling ``add_top_toolbar``, ``add_bottom_toolbar``,
``add_floating_window_plugin`` or ``add_window_plugin``. For example:

.. code-block:: python

   # Create plugin instances
   user_toolbar1 = DeviceStatusToolbar(device=client.client.channel(1))
   user_toolbar2 = BandwithToolbar()
   user_device_window = CustomWindowPlugin()

   console = PwConsoleEmbed(
       global_vars=local_variables,
       loggers={
           'Device Logs': [logging.getLogger('rpc_device')],
           'Host Logs': [logging.getLogger()],
       },
       ...
   )

   # Add toolbar plugins
   console.add_top_toolbar(user_toolbar1)
   console.add_bottom_toolbar(user_toolbar2)

   # Add Window plugins
   console.add_window_plugin(user_device_window)

   # Start the console
   console.embed()

-------------------
Adding Log Metadata
-------------------
``pw_console`` can display log messages in a table with justified columns for
metadata fields provided by :ref:`module-pw_log_tokenized`.

It is also possible to manually add values that should be displayed in columns
using the ``extra`` keyword argument when logging from Python. See the `Python's
logging documentation`_ for how ``extra`` works. A dict of name, value pairs can
be passed in as the ``extra_metadata_fields`` variable. For example, the
following code will create a log message with two custom columns titled
``module`` and ``timestamp``.

.. code-block:: python

  import logging

  LOG = logging.getLogger('log_source_1')

  LOG.info(
      'Hello there!',
      extra={
          'extra_metadata_fields': {
              'module': 'cool',
              'timestamp': 1.2345,
          }
      }
  )


---------------------
Debugging Serial Data
---------------------
``pw_console`` is often used to communicate with devices using `pySerial
<https://pythonhosted.org/pyserial/>`_ and it may be necessary to monitor the
raw data flowing over the wire to help with debugging. ``pw_console`` provides a
simple wrapper for a pySerial instances that log data for each read and write
call.

.. code-block:: python

   # Instead of 'import serial' use this import:
   from pw_console.pyserial_wrapper import SerialWithLogging

   serial_device = SerialWithLogging('/dev/ttyUSB0', 115200, timeout=1)

With the above example each ``serial_device.read`` and ``write`` call will
create a log message to the ``pw_console.serial_debug_logger`` Python
logger. This logger can then be included as a log window pane in the
``PwConsoleEmbed()`` call.

.. code-block:: python

   import logging
   from pw_console import PwConsoleEmbed

   console = PwConsoleEmbed(
       global_vars=globals(),
       local_vars=locals(),
       loggers={
           'Host Logs': [
               # Root Python logger
               logging.getLogger(''),
               # Your current Python package logger.
               logging.getLogger(__package__)
           ],
           'Device Logs': [
               logging.getLogger('usb_gadget')
           ],
           'Serial Debug': [
               # New log window to display serial read and writes
               logging.getLogger('pw_console.serial_debug_logger')
           ],
       },
       app_title='CoolConsole',
   )
   # Then run the console with:
   console.embed()

.. figure:: images/serial_debug.svg
  :alt: Serial debug pw_console screenshot.

  Screenshot of issuing an Echo RPC with serial debug logging.


.. _Python's logging documentation: https://docs.python.org/3/library/logging.html#logging.Logger.debug
