.. _module-pw_sys_io_arduino:

-----------------
pw_sys_io_arduino
-----------------

``pw_sys_io_arduino`` implements the ``pw_sys_io`` facade over
`Arduino's Serial interface <https://www.arduino.cc/reference/en/language/functions/communication/serial/>`_.

On initialization it runs Arduino's first ``Serial`` interface at a 115200 baud
rate:

.. code-block:: cpp

  Serial.begin(115200);

  // Wait for serial port to be available
  while (!Serial) {
  }

After ``Serial.begin(115200)`` it will busy wait until a host connects to the
serial port.

.. seealso::
   - :ref:`target-arduino` target documentation for a list of working hardware.
   - :ref:`module-pw_arduino_build` for caveats when running Pigweed on top of
     the Arduino API.
