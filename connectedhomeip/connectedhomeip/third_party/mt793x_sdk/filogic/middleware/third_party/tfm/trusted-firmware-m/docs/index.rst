:Page authors: Gyorgy Szing <gyorgy.szing@arm.com>

..
   The build-system will copy all documents into a temporary directory tree
   before the documentation is built.
   This file will be copied to the top level and thus please use relative paths
   as if this file would be in <TFM_ROOT_DIR>.

   The values between @ characters will be filled in by CMake.


Trusted Firmware-M Documentation
================================

.. toctree::
    :maxdepth: 2
    :hidden:

    Home<docs/readme>
    docs/about/index
    docs/changelog
    docs/design_documents/index
    docs/user_guides/index
    docs/processes/index
    docs/glossary
    docs/lic

.. toctree::
    :caption: Target platforms
    :maxdepth: 2
    :glob:
    :hidden:

    platform/**

.. toctree::
    :caption: Tools
    :maxdepth: 2
    :glob:
    :hidden:

    tools/iat-verifier/*

.. include:: docs/readme.rst

--------------

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*
