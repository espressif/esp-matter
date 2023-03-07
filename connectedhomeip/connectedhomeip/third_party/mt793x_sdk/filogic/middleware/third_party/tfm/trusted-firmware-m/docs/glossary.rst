###################################
Glossary of terms and abbreviations
###################################

************
TF-M related
************
.. glossary::

    TFM
    TF-M
    Trusted Firmware - M
    Trusted Firmware for M-class
        ARM TF-M provides a reference implementation of secure world software for ARMv8-M.

    SPE : TF-M related
    Secure Processing Environment
        PSA term. In TF-M this means the secure domain protected by TF-M.

    NSPE : TF-M related
    Non Secure Processing Enviroment
        PSA term. In TF-M this means non secure domain typically running an OS using services provided by TF-M.

    SS : TF-M related
    Secure Service
        A component within the TEE that is atomic from a security/trust point of view, i.e. which is viewed as a single entity from a TF-M point of view.

    SP : TF-M related
    Secure Partition
        A logical container for secure services.

    SPM : TF-M related
    Secure Partition Manager
        The TF-M component responsible for enumeration, management and isolation of multiple Secure Partitions within the TEE.

    SPRT : TF-M related
    Secure Partition Runtime
        The TF-M component responsible for Secure Partition runtime functionalities.

    SPRTL : TF-M related
    Secure Partition Runtime Library
        A library contains the SPRT code and data.

    SFN : TF-M related
    Secure Function
        The function entry to a secure service. Multiple SFN per SS are permitted.

    SST : TF-M related
    Secure Storage Service
        Secure storage service provided by TF-M.

****************
SSE-200 platform
****************
.. glossary::

    MPC : SSE-200 platform
    Memory Protection Controller
        Bus slave-side security controller for memory regions.

    PPC : SSE-200 platform
    Peripheral Protection Controller
        Bus slave-side security controller for peripheral access.

************
v8M-specific
************
.. glossary::

    S/NS : v8M-specific
    Secure/Non-secure
        The separation provided by TrustZone hardware components in the system.

    SAU : v8M-specific
    Secure Attribution Unit
        Hardware component providing isolation between Secure, Non-secure Callable and Non-secure addresses.


***************
M-class Generic
***************
.. glossary::

    AAPCS
    ARM Architecture Procedure Call Standard
        The AAPCS defines how subroutines can be separately written, separately compiled, and separately assembled to work together. It describes a contract between a calling routine and a called routine

    SVC
    SuperVisor Call
        ARMv7M assembly instruction to call a privileged handler function

    MPU : M-class Generic
    Memory Protection Unit
        Hardware component providing privilege control.

--------------

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*
