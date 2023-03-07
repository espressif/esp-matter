.. _module-pw_sys_io_mcuxpresso:

====================
pw_sys_io_mcuxpresso
====================
``pw_sys_io_mcuxpresso`` implements the ``pw_sys_io`` facade using the
NXP MCUXpresso SDK.

The implementation is based on the debug console component.

Setup
=====
This module requires a little setup:

 1. Use ``pw_build_mcuxpresso`` to create a ``pw_source_set`` for an
    MCUXpresso SDK.
 2. Include the debug console component in this SDK definition.
 3. Specify the ``pw_third_party_mcuxpresso_SDK`` GN global variable to specify
    the name of this source set.
 4. Use a target that calls ``pw_sys_io_mcuxpresso_Init`` in
    ``pw_boot_PreMainInit`` or similar.

The name of the SDK source set must be set in the
"pw_third_party_mcuxpresso_SDK" GN arg

Configuration
=============
The configuration of the module can be adjusted via compile-time configuration
of the MCUXpresso source set, see the
:ref:`documentation <module-pw_build_mcuxpresso>` for more details.

.. c:macro:: DEBUG_CONSOLE_TRANSFER_NON_BLOCKING

  Whether the MCUXpresso debug console supports non-blocking transfers. The
  default will depend on your SDK configuration.

  Enabling this adds support for ``pw::sys_io::TryReadByte``.
