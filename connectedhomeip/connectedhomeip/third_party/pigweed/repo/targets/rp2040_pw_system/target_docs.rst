.. _target-raspberry-pi-pico-pw-system:

================================
Raspberry Pi Pico with pw_system
================================
.. warning::

  This target is in a very preliminary state and is under active development.
  This demo gives a preview of the direction we are heading with
  :ref:`pw_system<module-pw_system>`, but it is not yet ready for production
  use.

This target configuration uses :ref:`pw_system<module-pw_system>` on top of
FreeRTOS and the `Raspberry Pi Pico SDK
<https://github.com/raspberrypi/pico-sdk>`_ HAL rather than a from-the-ground-up
baremetal approach.

-----
Setup
-----
To use this target, Pigweed must be set up to use FreeRTOS and the STM32Cube HAL
for the STM32F4 series. The supported repositories can be downloaded via
``pw package``, and then the build must be manually configured to point to the
locations the repositories were downloaded to.

.. code:: sh

   pw package install nanopb
   pw package install freertos
   pw package install pico_sdk

   gn gen out --export-compile-commands --args="
     dir_pw_third_party_nanopb=\"//environment/packages/nanopb\"
     dir_pw_third_party_freertos=\"//environment/packages/freertos\"
     PICO_SRC_DIR=\"//environment/packages/pico_sdk\"
   "

.. tip::

   Instead of the ``gn gen out`` with args set on the command line above you can
   run:

   .. code:: sh

      gn args out

   Then add the following lines to that text file:

   .. code::

      dir_pw_third_party_nanopb = pw_env_setup_PACKAGE_ROOT + "/nanopb"
      dir_pw_third_party_freertos = pw_env_setup_PACKAGE_ROOT + "/freertos"
      PICO_SRC_DIR = pw_env_setup_PACKAGE_ROOT + "/pico_sdk"

-----------------------------
Building and Running the Demo
-----------------------------
This target has an associated demo application that can be built and then
flashed to a device with the following commands:

**Build**

.. code:: sh

   ninja -C out pw_system_demo

**Flash**

- Using a uf2 file:

  Copy to ``out/rp2040_pw_system.size_optimized/obj/pw_system/system_example.uf2``
  your Pico when it is in USB bootloader mode. Hold down the BOOTSEL button when
  plugging in the pico and it will appear as a mass storage device.

- Using a Pico Probe and openocd:

  This requires installing the Raspberry Pi foundation's OpenOCD fork for the
  Pico probe. More details including how to connect the two Pico boards is
  available in ``Appendix A: Using Picoprobe`` of the `Getting started with
  Raspberry Pi Pico
  <https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf>`_ guide.

  **Install RaspberryPi's OpenOCD Fork:**

  .. code:: sh

     git clone https://github.com/raspberrypi/openocd.git \
       --branch picoprobe \
       --depth=1 \
       --no-single-branch \
       openocd-picoprobe

     cd openocd-picoprobe

     ./bootstrap
     ./configure --enable-picoprobe --prefix=$HOME/apps/openocd --disable-werror
     make -j2
     make install

  **Setup udev rules (Linux only):**

  .. code:: sh

     cat <<EOF > 49-picoprobe.rules
     SUBSYSTEMS=="usb", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="000[43a]", MODE:="0666"
     KERNEL=="ttyACM*", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="000[43a]", MODE:="0666"
     EOF
     sudo cp 49-picoprobe.rules /usr/lib/udev/rules.d/49-picoprobe.rules
     sudo udevadm control --reload-rules

  **Flash the Pico:**

  .. code:: sh

     ~/apps/openocd/bin/openocd -f ~/apps/openocd/share/openocd/scripts/interface/picoprobe.cfg -f ~/apps/openocd/share/openocd/scripts/target/rp2040.cfg -c 'program out/rp2040_pw_system.size_optimized/obj/pw_system/bin/system_example.elf verify reset exit'

**Connect with pw_console**

Once the board has been flashed, you can connect to it and send RPC commands
via the Pigweed console:

.. code:: sh

   pw-system-console -d /dev/{ttyX} -b 115200 \
     --proto-globs pw_rpc/echo.proto \
     --token-databases \
       out/rp2040_pw_system.size_optimized/obj/pw_system/bin/system_example.elf

Replace ``{ttyX}`` with the appropriate device on your machine. On Linux this
may look like ``ttyACM0``, and on a Mac it may look like ``cu.usbmodem***``.

When the console opens, try sending an Echo RPC request. You should get back
the same message you sent to the device.

.. code:: pycon

   >>> device.rpcs.pw.rpc.EchoService.Echo(msg="Hello, Pigweed!")
   (Status.OK, pw.rpc.EchoMessage(msg='Hello, Pigweed!'))

You can also try out our thread snapshot RPC service, which should return a
stack usage overview of all running threads on the device in Host Logs.

.. code:: pycon

   >>> device.snapshot_peak_stack_usage()

Example output:

.. code::

   20220826 09:47:22  INF  PendingRpc(channel=1, method=pw.thread.ThreadSnapshotService.GetPeakStackUsage) completed: Status.OK
   20220826 09:47:22  INF  Thread State
   20220826 09:47:22  INF    5 threads running.
   20220826 09:47:22  INF
   20220826 09:47:22  INF  Thread (UNKNOWN): IDLE
   20220826 09:47:22  INF  Est CPU usage: unknown
   20220826 09:47:22  INF  Stack info
   20220826 09:47:22  INF    Current usage:   0x20002da0 - 0x???????? (size unknown)
   20220826 09:47:22  INF    Est peak usage:  390 bytes, 76.77%
   20220826 09:47:22  INF    Stack limits:    0x20002da0 - 0x20002ba4 (508 bytes)
   20220826 09:47:22  INF
   20220826 09:47:22  INF  ...

You are now up and running!

.. seealso::

   The :ref:`module-pw_console`
   :bdg-ref-primary-line:`module-pw_console-user_guide` for more info on using
   the the pw_console UI.
