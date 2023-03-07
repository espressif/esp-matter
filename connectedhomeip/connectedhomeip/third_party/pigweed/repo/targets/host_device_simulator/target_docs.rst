.. _target-host-device-simulator:

=====================
Host Device Simulator
=====================
This Pigweed target simulates the behavior of an embedded device, spawning
threads for facilities like RPC and logging. Executables built by this target
will perpetually run until they crash or are explicitly terminated. All
communications with the process are over the RPC server hosted on a local
socket rather than by directly interacting with the terminal via standard I/O.

-----
Setup
-----
To use this target, Pigweed must be set up to use nanopb and FreeRTOS. The
required source repositories can be downloaded via ``pw package``, and then the
build must be manually configured to point to the location the repository was
downloaded to using gn args. Optionally you can include the ``stm32cube_f4``
package to build for the
:bdg-ref-primary-line:`target-stm32f429i-disc1-stm32cube` target at the same
time.

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
run with the following commands:

.. code:: sh

   ninja -C out pw_system_demo

.. code:: sh

   ./out/host_device_simulator.speed_optimized/obj/pw_system/bin/system_example

To communicate with the launched process run this in a separate shell:

.. code:: sh

   pw-system-console -s default --proto-globs pw_rpc/echo.proto \
     --token-databases out/host_device_simulator.speed_optimized/obj/pw_system/bin/system_example

.. tip::

   Alternatively you can run the system_example app in the background, then
   launch the console on the same line with:

   .. code:: sh

      ./out/host_device_simulator.speed_optimized/obj/pw_system/bin/system_example
        & \
        pw-system-console -s default --proto-globs pw_rpc/echo.proto \
        --token-databases \
          out/host_device_simulator.speed_optimized/obj/pw_system/bin/system_example

   Exit the console via the menu or pressing :kbd:`Ctrl-d` twice. Then stop the
   system_example app with:

   .. code:: sh

      killall system_example

In the bottom-most pane labeled ``Python Repl`` you should be able to send RPC
commands to the simulated device process. For example, you can send an RPC
message that will be echoed back:

.. code:: pycon

   >>> device.rpcs.pw.rpc.EchoService.Echo(msg='Hello, world!')
   (Status.OK, pw.rpc.EchoMessage(msg='Hello, world!'))

Or run unit tests included on the simulated device:

.. code:: pycon

   >>> device.run_tests()
   True

You are now up and running!

.. seealso::

   The :ref:`module-pw_console`
   :bdg-ref-primary-line:`module-pw_console-user_guide` for more info on using
   the the pw_console UI.
