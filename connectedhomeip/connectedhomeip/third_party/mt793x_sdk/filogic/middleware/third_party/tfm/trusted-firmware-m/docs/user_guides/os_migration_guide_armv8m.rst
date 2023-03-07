#########################################################
Generic OS migration from Armv7-M to Armv8-M architecture
#########################################################
The purpose of this document is to list a set of requirements needed for
migrating a generic OS kernel running on Armv7-M to the Armv8-M architecture.

********************
List of requirements
********************
- If the same OS codebase is used for both Secure and Non Secure builds, it is
  suggested to put specific code targeted to the Non Secure build under a
  compile time switch, e.g. ``#if (DOMAIN_NS == 1U)``. The OS build system in
  this case needs to be amended accordingly to support this new switch.
- If the OS implements stack limit checking, the ``PSPLIM`` register
  needs to be initialized and properly handled during thread context switch
  operations.
- If the OS manipulates directly the Link Register, the default Link Register
  value used in Handler mode transitions needs to be differentiated between
  Secure and Non Secure builds, i.e. ``0xFD`` and ``0xBC``, respectively.
- If the OS supports the Thread Context Management for Armv8-M TrustZone APIs,
  as described
  `here <https://www.keil.com/pack/doc/CMSIS/Core/html/group__context__trustzone__functions.html>`__
  , and would like to use the non-secure client identification feature of TF-M,
  then it also have to use the
  ``enum tfm_status_e tfm_register_client_id (int32_t ns_client_id)``
  API function provided by TF-M, as described in
  :doc:`NS client identification documentation <tfm_ns_client_identification>`.
- if the OS doesn't support the API mentioned above, it should set
  ``TFM_NS_CLIENT_IDENTIFICATION`` to ``OFF`` in the cmake system.
- .. Note::

    This is NOT REQUIRED when the Non Secure OS build is meant
    to be integrated with TF-M running in Secure world.
  
  If generic function calls into Secure world have to be supported in Non Secure
  builds, integrate an API for secure stack memory management (e.g. the
  TrustZone API for secure stack memory management described in
  ``tz_context.h``).

--------------

*Copyright (c) 2018-2019, Arm Limited. All rights reserved.*
