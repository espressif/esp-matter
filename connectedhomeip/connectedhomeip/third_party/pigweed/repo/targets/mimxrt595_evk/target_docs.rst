.. _target-mimxrt595-evk:

-------------
mimxrt595-evk
-------------
The NXP MIMXRT595-EVK_ evaluation board is a demonstration target for on-device
Pigweed development

.. _MIMXRT595-EVK: https://www.nxp.com/design/development-boards/i-mx-evaluation-and-development-boards/i-mx-rt595-evaluation-kit:MIMXRT595-EVK

Configuring
===========
Step 1: Download SDK
--------------------
To configure this Pigweed target you will first need to download an NXP
`MCUXpresso SDK`_ for your device and unpack it within your project source tree.

.. _MCUXpresso SDK: https://mcuxpresso.nxp.com/en/welcome

Step 2: Create SDK source set
-----------------------------
You'll next need to create a source set based on the downloaded SDK, and set
the ``pw_third_party_mcuxpresso_SDK`` build arg to the name of the source set
you create. See :ref:`module-pw_build_mcuxpresso` for more details.

Alternatively to get started you can start with the basic project template by
setting the ``pw_target_mimxrt595_evk_MANIFEST`` build arg to the location of
the manifest file within the unpacked SDK, and then setting the
``pw_third_party_mcuxpresso_SDK`` to the ``sample_sdk`` source set within the
Pigweed target directory.

.. code:: sh

  $ gn args out
  # Modify and save the args file to use the sample SDK.
  pw_target_mimxrt595_evk_MANIFEST = "//third_party/mcuxpresso/sdk/EVK-MIMXRT595_manifest_v3_8.xml"
  pw_third_party_mcuxpresso_SDK = "//targets/mimxrt595_evk:sample_sdk"

Building
========
Once configured, to build for this Pigweed target, simply build the top-level
"mimxrt595" Ninja target.

.. code:: sh

  $ ninja -C out mimxrt595

Running and Debugging
=====================
First Time Setup
----------------
The MIMXRT595-EVK comes with an integrated Link2 debug probe that can operate in
either CMSIS-DAP or SEGGER J-Link mode. CMSIS-DAP is how the board will likely
come by default, but J-Link is the mode that is currently known to work, so
you'll need to flash the Link2 with the correct firmware.

1. Download and install the LPCScrypt_ utility from the NXP website.

2. Place a jumper over **JP1** (not **J1**). If you're having trouble locating
   this, it's in the top-right of the board in a block of four jumpers closest
   to the USB ports.

3. Connect a USB cable into the top-right USB port (**J40**) and your computer.

4. Run ``scripts/boot_lpcscrypt`` from the LPCScrypt installation.

5. Run ``scripts/program_JLINK`` from the LPCScrypt installation, press the
   *SPACE* key to update the firmware.

6. Unplug the USB cable and remove the **JP1** jumper.

Now is also a good time to download and install the J-Link_ package from the
SEGGER website.

.. _LPCScrypt: https://www.nxp.com/design/microcontrollers-developer-resources/lpcscrypt-v2-1-2:LPCSCRYPT
.. _J-Link: https://www.segger.com/downloads/jlink/

General Setup
-------------
Each time you prepare the MIMXRT595-EVK for use, you'll need to do a few steps.
You don't need to repeat these if you leave everything setup and don't
disconnect or reboot.

1. Ensure the **SW7** DIP switches are set to Off-Off-On (boot from QSPI Flash).

2. Connect a USB cable into the top-right USB port (**J40**) and your computer.

3. Start the J-Link GDB Server and leave this running:

   .. code-block:: sh

     JLinkGDBServer -select USB -device MIMXRT595S -endian little -if SWD -speed 4000 -noir

On Linux, you may need to install the `libncurses5` library to use the tools:

.. code-block:: sh

  sudo apt install libncurses5

Running and Debugging
---------------------
Use ``arm-none-eabi-gdb`` to load an executable into the target, debug, and run
it.

.. code-block::
  :emphasize-lines: 1,6,10,12,20

  (gdb) target remote :2331
  Remote debugging using :2331
  warning: No executable has been specified and target does not support
  determining executable automatically.  Try using the "file" command.
  0x08000000 in ?? ()
  (gdb) file out/mimxrt595_evk_debug/obj/pw_status/test/status_test.elf
  A program is being debugged already.
  Are you sure you want to change the file? (y or n) y
  Reading symbols from out/mimxrt595_evk_debug/obj/pw_status/test/status_test.elf...
  (gdb) monitor reset
  Resetting target
  (gdb) load
  Loading section .flash_config, size 0x200 lma 0x8000400
  Loading section .vector_table, size 0x168 lma 0x8001000
  Loading section .code, size 0xb34c lma 0x8001180
  Loading section .ARM, size 0x8 lma 0x800c4d0
  Loading section .static_init_ram, size 0x3c8 lma 0x800c4d8
  Start address 0x080048d0, load size 47748
  Transfer rate: 15542 KB/sec, 6821 bytes/write.
  (gdb) monitor reset
  Resetting target

You can now set any breakpoints you wish, and ``continue`` to run the
executable.

To reset the target use ``monitor reset``.

To load an updated version of the same file, after resetting the target,
use ``load`` and a second ``monitor reset`` as shown above.

To debug a new file, use ``file`` before ``load``.

Debug console is available on the USB serial port, e.g. ``/dev/ttyACM0``
(Linux) or ``/dev/tty.usbmodem*`` (Mac).

