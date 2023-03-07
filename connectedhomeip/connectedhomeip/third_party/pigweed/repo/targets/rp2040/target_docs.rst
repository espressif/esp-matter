.. _target-raspberry-pi-pico:

-----------------
Raspberry Pi Pico
-----------------
.. warning::
  This target is in an early state and is under active development. Usability
  is not very polished, and many features/configuration options that work in
  upstream Pi Pico CMake build have not yet been ported to the GN build.

Setup
=====
To use this target, Pigweed must be set up to build against the Raspberry Pi
Pico SDK. This can be downloaded via ``pw package``, and then the build must be
manually configured to point to the location of the downloaded SDK.

.. code:: sh

  pw package install pico_sdk

  gn args out
    # Add this line.
    PICO_SRC_DIR = getenv("PW_PACKAGE_ROOT") + "/pico_sdk"

Linux
-----
On linux, you may need to update your udev rules at
``/etc/udev/rules.d/49-pico.rules`` to include the following:

.. code:: none

  SUBSYSTEMS=="usb", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0004", MODE:="0666"
  KERNEL=="ttyACM*", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0004", MODE:="0666"
  SUBSYSTEMS=="usb", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0003", MODE:="0666"
  KERNEL=="ttyACM*", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0003", MODE:="0666"
  SUBSYSTEMS=="usb", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="000a", MODE:="0666"
  KERNEL=="ttyACM*", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="000a", MODE:="0666"

Usage
=====
The Pi Pico is configured to output logs and test results over USB serial at a
baud rate of 115200.

Once the pico SDK is configured, the Pi Pico will build as part of the default
GN build:

.. code:: sh

  ninja -C out

Pigweed's build will produce ELF and UF2 files for each unit test built for the
Pi Pico.

Flashing
========
Flashing the Pi Pico is two easy steps:

#. While holding the button on the Pi Pico, connect the Pico to your computer
   via the micro USB port.
#. Copy the desired UF2 firmware image to the RPI-RP2 volume that enumerated
   when you connected the Pico.

Testing
=======
Unlike some other targets, the RP2040 does not automatically run tests on boot.
To run a test, flash it to the RP2040 and connect to the serial port and then
press the spacebar to start the test:

.. code:: none

  $ python -m serial.tools.miniterm --raw /dev/ttyACM0 115200
  --- Miniterm on /dev/cu.usbmodem142401  115200,8,N,1 ---
  --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---INF  [==========] Running all tests.
  INF  [ RUN      ] Status.Default
  INF  [       OK ] Status.Default
  INF  [ RUN      ] Status.ConstructWithStatusCode
  INF  [       OK ] Status.ConstructWithStatusCode
  INF  [ RUN      ] Status.AssignFromStatusCode
  INF  [       OK ] Status.AssignFromStatusCode
  INF  [ RUN      ] Status.Ok_OkIsTrue
  INF  [       OK ] Status.Ok_OkIsTrue
  INF  [ RUN      ] Status.NotOk_OkIsFalse
  INF  [       OK ] Status.NotOk_OkIsFalse
  INF  [ RUN      ] Status.Code
  INF  [       OK ] Status.Code
  INF  [ RUN      ] Status.EqualCodes
  INF  [       OK ] Status.EqualCodes
  INF  [ RUN      ] Status.IsError
  INF  [       OK ] Status.IsError
  INF  [ RUN      ] Status.IsNotError
  INF  [       OK ] Status.IsNotError
  INF  [ RUN      ] Status.Strings
  INF  [       OK ] Status.Strings
  INF  [ RUN      ] Status.UnknownString
  INF  [       OK ] Status.UnknownString
  INF  [ RUN      ] Status.Update
  INF  [       OK ] Status.Update
  INF  [ RUN      ] StatusCLinkage.CallCFunctionWithStatus
  INF  [       OK ] StatusCLinkage.CallCFunctionWithStatus
  INF  [ RUN      ] StatusCLinkage.TestStatusFromC
  INF  [       OK ] StatusCLinkage.TestStatusFromC
  INF  [ RUN      ] StatusCLinkage.TestStatusStringsFromC
  INF  [       OK ] StatusCLinkage.TestStatusStringsFromC
  INF  [==========] Done running all tests.
  INF  [  PASSED  ] 15 test(s).

This is done because the serial port enumerated by the Pi Pico goes away on
reboot, so it's not safe to run tests until the port has fully enumerated and
a terminal has connected. To avoid races, the Pico will just wait until it
receives the space character (0x20) as a signal to start running the tests.

The RP2040 does not yet provide an automated test runner with build system
integration.
