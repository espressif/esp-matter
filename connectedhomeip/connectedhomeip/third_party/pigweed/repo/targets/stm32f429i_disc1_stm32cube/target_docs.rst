.. _target-stm32f429i-disc1-stm32cube:

===========================
stm32f429i-disc1: STM32Cube
===========================

.. warning::

  This target is in a very preliminary state and is under active development.
  This demo gives a preview of the direction we are heading with
  :ref:`pw_system<module-pw_system>`, but it is not yet ready for production
  use.

The STMicroelectronics STM32F429I-DISC1 development board is currently Pigweed's
primary target for on-device testing and development. This target configuration
uses :ref:`pw_system<module-pw_system>` on top of FreeRTOS and the STM32Cube HAL
rather than a from-the-ground-up baremetal approach.

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
   pw package install stm32cube_f4

   gn gen out --export-compile-commands --args="
     dir_pw_third_party_nanopb=\"$PW_PROJECT_ROOT/environment/packages/nanopb\"
     dir_pw_third_party_freertos=\"$PW_PROJECT_ROOT/environment/packages/freertos\"
     dir_pw_third_party_stm32cube_f4=\"$PW_PROJECT_ROOT/environment/packages/stm32cube_f4\"
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
      dir_pw_third_party_stm32cube_f4 = pw_env_setup_PACKAGE_ROOT + "/stm32cube_f4"

-----------------------------
Building and Running the Demo
-----------------------------
This target has an associated demo application that can be built and then
flashed to a device with the following commands:

.. code:: sh

   ninja -C out pw_system_demo

.. code:: sh

   openocd -f targets/stm32f429i_disc1/py/stm32f429i_disc1_utils/openocd_stm32f4xx.cfg \
     -c "program out/stm32f429i_disc1_stm32cube.size_optimized/obj/pw_system/bin/system_example.elf reset exit"

Once the board has been flashed, you can connect to it and send RPC commands
via the Pigweed console:

.. code:: sh

   pw-system-console -d /dev/{ttyX} -b 115200 \
     --proto-globs pw_rpc/echo.proto \
     --token-databases \
       out/stm32f429i_disc1_stm32cube.size_optimized/obj/pw_system/bin/system_example.elf

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
