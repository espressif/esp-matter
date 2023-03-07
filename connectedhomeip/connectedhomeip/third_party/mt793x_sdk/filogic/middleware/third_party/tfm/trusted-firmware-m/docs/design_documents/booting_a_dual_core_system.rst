##########################
Booting a Dual-Core System
##########################

:Authors: Chris Brand
:Organization: Cypress Semiconductor Corporation
:Contact: chris.brand@cypress.com
:Status: Accepted

*******************
System Architecture
*******************
There are many possibly ways to design a dual core system. Some important
considerations from a boot perspective are:

- Which core has access to which areas of Flash?

    - It is possible that the secure core has no access to the Flash from which
      the non-secure core will boot, in which case the non-secure core will
      presumably have a separate root of trust and perform its own integrity
      checks on boot.

- How does the non-secure core behave on power-up? Is it held in reset,
  does it jump to a set address, …?

- What are the performance characteristics of the two core?

    - There could be a great disparity in performance

**********************
TF-M Twin Core Booting
**********************
In an effort to make the problem manageable, as well as to provide a system
with good performance, that is flexible enough to work for a variety of dual
core systems, the following design decisions have been made:

- TF-M will (for now) only support systems where the secure core has full
  access to the Flash that the non-secure core will boot from

    - This keeps the boot flow as close as possible to the single core design,
      with the secure core responsible for maintaining the chain of trust for
      the entire system, and for upgrade of the entire system

- The secure code will make a platform-specific call immediately after setting
  up hardware protection to (potentially) start the non-secure core running

    - This is the earliest point at which it is safe to allow the non-secure
      code to start running, so starting it here ensures system integrity while
      also giving the non-secure code the maximum amount of time to perform its
      initialization

    - Note that this is after the bootloader has validated the non-secure image,
      which is the other key part to maintain security

    - This also means that only tfm_s and tfm_ns have to change, and not mcuboot

- Both the secure and non-secure code will make platform-specific calls to
  establish a synchronization point. This will be after both sides have done
  any initialization that is required, including setting up inter-core
  communications. On a single core system, this would be the point at which the
  secure code jumps to the non-secure code, and at the very start of the
  non-secure code.

- After completing initialization on the secure core (at the point where on a
  single core system, it would jump to the non-secure code), the main thread on
  the secure core will be allowed to die

    - The scheduler has been started at this point, and an idle thread exists.
      Any additional work that is only required in the dual core case will be
      interrupt-driven.

- Because both cores may be booting in parallel, executing different
  initialization code, at different speeds, the design must be resilient if
  either core attempts to communicate with the other before the latter is ready.
  For example, the client (non-secure) side of the IPC mechanism must be able
  to handle the situation where it has to wait for the server (secure) side to
  finish setting up the IPC mechanism.

    - This relates to the synchronization calls mentioned above. It means that
      those calls cannot utilise the IPC mechanism, but must instead use some
      platform-specific mechanism to establish this synchronization. This could
      be as simple as setting aside a small area of shared memory and having
      both sides set a “ready” flag, but may well also involve the use of
      interrupts.

    - This also means that the synchronization call must take place after the
      IPC mechanism has been set up but before any attempt (by either side) to
      use it.

*************
API Additions
*************
Three new HAL functions are required:

.. code-block:: c

    void tfm_spm_hal_boot_ns_cpu(uintptr_t start_addr);

- Called on the secure core from ``tfm_core_init()`` after hardware protections
  have been configured.

- Performs the necessary actions to start the non-secure core running the code
  at the specified address.

.. code-block:: c

    void tfm_spm_hal_wait_for_ns_cpu_ready(void);

- Called on the secure core from the end of ``tfm_core_init()`` where on a
  single core system the secure code calls into the non-secure code.

- Flags that the secure core has completed its initialization, including setting
  up the IPC mechanism.

- Waits, if necessary, for the non-secure core to flag that it has completed its
  initialisation

.. code-block:: c

    void tfm_ns_wait_for_s_cpu_ready(void);

- Called on the non-secure core from ``main()`` after the dual-core-specific
  initialization (on a single core system, this would be the start of the
  non-secure code), before the first use of the IPC mechanism.

- Flags that the non-secure side has completed its initialization.

- Waits, if necessary, for the secure core to flag that it has completed its
  initialization.

For all three, an empty implementation will be provided with a weak symbol so
that platforms only have to provide the new functions if they are required.

---------------

Copyright (c) 2019 Cypress Semiconductor Corporation
