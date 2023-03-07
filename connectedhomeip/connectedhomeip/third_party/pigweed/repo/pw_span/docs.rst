.. _module-pw_span:

=======
pw_span
=======
The ``pw_span`` module provides :cpp:class:`pw::span`, an implementation of
C++20's `std::span <https://en.cppreference.com/w/cpp/container/span>`_.
``std::span`` is a non-owning view of an array of values. The intent is for
:cpp:class:`pw::span` is to match the C++20 standard as closely as possible.

If C++20's ``std::span`` is available, :cpp:class:`pw::span` is simply an alias
of it.

--------------
Using pw::span
--------------
:cpp:class:`pw::span` is a convenient abstraction that wraps a pointer and a
size. :cpp:class:`pw::span` is especially useful in APIs. Spans support implicit
conversions from C arrays, ``std::array``, or any STL-style container, such as
``std::string_view``.

Functions operating on an array of bytes typically accept pointer and size
arguments:

.. code-block:: cpp

  bool ProcessBuffer(char* buffer, size_t buffer_size);

  bool DoStuff() {
    ProcessBuffer(c_array, sizeof(c_array));
    ProcessBuffer(array_object.data(), array_object.size());
    ProcessBuffer(data_pointer, data_size);
  }

Pointer and size arguments can be replaced with a :cpp:class:`pw::span`:

.. code-block:: cpp

  #include <span>

  // With pw::span, the buffer is passed as a single argument.
  bool ProcessBuffer(pw::span<uint8_t> buffer);

  bool DoStuff() {
    ProcessBuffer(c_array);
    ProcessBuffer(array_object);
    ProcessBuffer(pw::span(data_pointer, data_size));
  }

.. tip::

  Use ``pw::span<std::byte>`` or ``pw::span<const std::byte>`` to represent
  spans of binary data. Use ``pw::as_bytes`` or ``pw::as_writeable_bytes`` to
  convert any span to a byte span.

  .. code-block:: cpp

    void ProcessData(pw::span<const std::byte> data);

    void DoStuff() {
      std::array<AnyType, 7> data = { ... };
      ProcessData(pw::as_bytes(pw::span(data)));
    }

  ``pw_bytes/span.h`` provides ``ByteSpan`` and ``ConstByteSpan`` aliases for
  these types.

----------------------------
Module Configuration Options
----------------------------
The following configurations can be adjusted via compile-time configuration of
this module, see the
:ref:`module documentation <module-structure-compile-time-configuration>` for
more details.

.. c:macro:: PW_SPAN_ENABLE_ASSERTS

  PW_SPAN_ENABLE_ASSERTS controls whether pw_span's implementation includes
  asserts for detecting disallowed span operations at runtime. For C++20 and
  later, this replaces std::span with the custom implementation in pw_span to
  ensure bounds-checking asserts have been enabled.

  This defaults to disabled because of the significant increase in code size
  caused by enabling this feature. It's strongly recommended to enable this
  in debug and testing builds. This can be done by setting
  ``pw_span_ENABLE_ASSERTS`` to ``true`` in the GN build.

-------------
Compatibility
-------------
Works with C++14, but some features require C++17. In C++20, use ``std::span``
instead.

------
Zephyr
------
To enable ``pw_span`` for Zephyr add ``CONFIG_PIGWEED_SPAN=y`` to the project's
configuration.
