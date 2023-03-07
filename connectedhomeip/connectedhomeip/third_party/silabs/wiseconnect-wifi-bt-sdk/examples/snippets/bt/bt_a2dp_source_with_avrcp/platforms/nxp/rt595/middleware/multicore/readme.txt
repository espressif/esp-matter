Multicore SDK
=============

This is the top level Multicore SDK directory. This directory contains source code
for NXP's multicore support.


Prerequisites
-------------

There are several packages required to build multicore source code.

erpcgen prerequisites:
  - flex
  - bison
  - boost

Documentation
-------------

Multicore SDK documents can be found in the top level KSDK doc directory:
    "KSDK_ROOT/doc/multicore".

Directory Structure
-------------------

erpc - Embedded Remote Procedure Call. An infrastructure to abstract multicore
communication to fully user-definable sets of function calls. A host tool,
erpcgen, is provided to generate code to implement the multicore communication
layer for the defined function calls. Erpcgen is built for Linux, Windows, and
OS X.

mcmgr - Multicore Manager. Provides control of second core startup and shutdown.

rdmgr - Resource Domain Manager. Configures the XRDC periphal, which controls
access rights to peripherals and memory blocks.

rpmsg_lite - Remote Processor Messaging Lite software component is a lightweight
implementation of the RPMsg protocol. Compared to the OpenAMP implementation,
the RPMsg-Lite offers a code size reduction, API simplification and improved
modularity.

tools - Folder contains tools and utilities for multicore like a Peripheral and
Memory Manager (PAM) GUI configuration tool for easy XRDC settings.
