LwPRINTF |version| documentation
================================

Welcome to the documentation for version |version|.

LwPRINTF is lightweight stdio manager optimized for embedded systems.
It includes implementation of standard output functions such as ``printf``, ``vprintf``, ``snprintf``, ``sprintf`` and ``vsnprintf`` in an embedded-systems optimized way.

.. image:: static/images/logo.svg
    :align: center

.. rst-class:: center
.. rst-class:: index_links

	:ref:`download_library` :ref:`getting_started` `Open Github <https://github.com/MaJerle/lwprintf>`_ `Donate <https://paypal.me/tilz0R>`_

Features
^^^^^^^^

* Written in ANSI C99, compatible with ``size_t`` and ``uintmax_t`` types for some specifiers
* Implements output functions compatible with ``printf``, ``vprintf``, ``snprintf``, ``sprintf`` and ``vsnprintf``
* Low-memory footprint, suitable for embedded systems
* Reentrant access to all API functions
* Operating-system ready
* Requires single output function to be implemented by user for ``printf``-like API calls
* With optional functions for operating systems to protect multiple threads printing to the same output stream
* Allows multiple output stream functions (unlike standard ``printf`` which supports only one) to separate parts of application
* Added additional specifiers vs original features
* User friendly MIT license

Requirements
^^^^^^^^^^^^

* C compiler
* Few kB of non-volatile memory

Contribute
^^^^^^^^^^

Fresh contributions are always welcome. Simple instructions to proceed:

#. Fork Github repository
#. Respect `C style & coding rules <https://github.com/MaJerle/c-code-style>`_ used by the library
#. Create a pull request to ``develop`` branch with new features or bug fixes

Alternatively you may:

#. Report a bug
#. Ask for a feature request

License
^^^^^^^

.. literalinclude:: ../LICENSE

Table of contents
^^^^^^^^^^^^^^^^^

.. toctree::
    :maxdepth: 2

    self
    get-started/index
    user-manual/index
    api-reference/index
    test-results/index
    examples/index
