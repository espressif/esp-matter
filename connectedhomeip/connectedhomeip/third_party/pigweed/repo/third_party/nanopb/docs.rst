.. _module-pw_third_party_nanopb:

======
Nanopb
======

The ``$dir_pw_third_party/nanopb/`` module contains Nanopb, a tiny protobuf
library. It is used by :ref:`module-pw_protobuf_compiler`.

----------------
GN Build Support
----------------
This module provides support to compile Nanopb with GN.

Follow the documentation on :ref:`module-pw_protobuf_compiler` for general
help on how to use this.

Enabling ``PB_NO_ERRMSG=1``
---------------------------

In your toolchain configuration, you can use the following:

.. code-block::

  pw_third_party_nanopb_CONFIG = "$dir_pw_third_party/nanopb:disable_error_messages"


This will add ``-DPB_NO_ERRMSG=1`` to the build, which disables error messages
as strings and may save some code space at the expense of ease of debugging.
