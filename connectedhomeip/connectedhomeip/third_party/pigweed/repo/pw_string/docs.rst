.. _module-pw_string:

=========
pw_string
=========
String manipulation is a very common operation, but the standard C and C++
string libraries have drawbacks. The C++ functions are easy-to-use and powerful,
but require too much flash and memory for many embedded projects. The C string
functions are lighter weight, but can be difficult to use correctly. Mishandling
of null terminators or buffer sizes can result in serious bugs.

The ``pw_string`` module provides the flexibility, ease-of-use, and safety of
C++-style string manipulation, but with no dynamic memory allocation and a much
smaller binary size impact. Using ``pw_string`` in place of the standard C
functions eliminates issues related to buffer overflow or missing null
terminators.

-------------
Compatibility
-------------
C++17, C++14 (:cpp:type:`pw::InlineString`)

--------
Features
--------

pw::InlineString
================
:cpp:class:`pw::InlineBasicString` and :cpp:type:`pw::InlineString` are
C++14-compatible, fixed-capacity, null-terminated string classes. They are
equivalent to ``std::basic_string<T>`` and ``std::string``, but store the string
contents inline and use no dynamic memory.

:cpp:type:`pw::InlineString` takes the fixed capacity as a template argument,
but may be used generically without specifying the capacity. The capacity value
is stored in a member variable, which the generic ``pw::InlineString<>`` /
``pw::InlineBasicString<T>`` specialization uses in place of the template
parameter.

:cpp:type:`pw::InlineString` is efficient and compact. The current size and
capacity are stored in a single word. Accessing the contents of a
:cpp:type:`pw::InlineString` is a simple array access within the object, with no
pointer indirection, even when working from a generic ``pw::InlineString<>``
reference.

Key differences from ``std::string``
------------------------------------
- **Fixed capacity** -- Operations that add characters to the string beyond its
  capacity are an error. These trigger a ``PW_ASSERT`` at runtime. When
  detectable, these situations trigger a ``static_assert`` at compile time.
- **Minimal overhead** -- :cpp:type:`pw::InlineString` operations never
  allocate. Reading the contents of the string is a direct memory access within
  the string object, without pointer indirection.
- **Constexpr support** -- :cpp:type:`pw::InlineString` works in ``constexpr``
  contexts, which is not supported by ``std::string`` until C++20.

API reference
-------------
.. cpp:class:: template <typename T, unsigned short kCapacity> pw::InlineBasicString

   Represents a fixed-capacity string of a generic character type. Equivalent to
   ``std::basic_string<T>``. Always null (``T()``) terminated.

.. cpp:type:: template <unsigned short kCapacity> pw::InlineString = pw::InlineBasicString<char, kCapacity>

   Represents a fixed-capacity string of ``char`` characters. Equivalent to
   ``std::string``. Always null (``'\0'``) terminated.

:cpp:type:`pw::InlineString` / :cpp:class:`pw::InlineBasicString` follows the
``std::string`` / ``std::basic_string<T>`` API, with a few variations:

- :cpp:type:`pw::InlineString` provides overloads specific to character arrays.
  These perform compile-time capacity checks and are used for class template
  argument deduction. Like ``std::string``, character arrays are treated as
  null-terminated strings.
- :cpp:type:`pw::InlineString` allows implicit conversions from
  ``std::string_view``. Specifying the capacity parameter is cumbersome, so
  implicit conversions are helpful. Also, implicitly creating a
  :cpp:type:`pw::InlineString` is less costly than creating a ``std::string``.
  As with ``std::string``, explicit conversions are required from types that
  convert to ``std::string_view``.
- Functions related to dynamic memory allocation are not present (``reserve()``,
  ``shrink_to_fit()``, ``get_allocator()``).
- ``resize_and_overwrite()`` only takes the ``Operation`` argument, since the
  underlying string buffer cannot be resized.

See the `std::string documentation
<https://en.cppreference.com/w/cpp/string/basic_string>`_ for full details.

Usage
-----
:cpp:type:`pw::InlineString` objects must be constructed by specifying a fixed
capacity for the string.

.. code-block:: c++

  // Initialize from a C string.
  pw::InlineString<32> inline_string = "Literally";
  inline_string.append('?', 3);   // contains "Literally???"

  // Supports copying into known-capacity strings.
  pw::InlineString<64> other = inline_string;

  // Supports various helpful std::string functions
  if (inline_string.starts_with("Lit") || inline_string == "not\0literally"sv) {
    other += inline_string;
  }

  // Like std::string, InlineString is always null terminated when accessed
  // through c_str(). InlineString can be used to null-terminate
  // length-delimited strings for APIs that expect null-terminated strings.
  std::string_view file(".gif");
  if (std::fopen(pw::InlineString<kMaxNameLen>(file).c_str(), "r") == nullptr) {
    return;
  }

  // pw::InlineString integrates well with std::string_view. It supports
  // implicit conversions to and from std::string_view.
  inline_string = std::string_view("not\0literally", 12);

  FunctionThatTakesAStringView(inline_string);

  FunctionThatTakesAnInlineString(std::string_view("1234", 4));

All :cpp:type:`pw::InlineString` operations may be performed on strings without
specifying their capacity.

.. code-block:: c++

  void RemoveSuffix(pw::InlineString<>& string, std::string_view suffix) {
    if (string.ends_with(suffix)) {
       string.resize(string.size() - suffix.size());
    }
  }

  void DoStuff() {
    pw::InlineString<32> str1 = "Good morning!";
    RemoveSuffix(str1, " morning!");

    pw::InlineString<40> str2 = "Good";
    RemoveSuffix(str2, " morning!");

    PW_ASSERT(str1 == str2);
  }

:cpp:type:`pw::InlineString` operations on known-size strings may be used in
``constexpr`` expressions.

.. code-block:: c++

   static constexpr pw::InlineString<64> kMyString = [] {
     pw::InlineString<64> string;

     for (int i = 0; i < 10; ++i) {
       string += "Hello";
     }

     return string;
   }();

:cpp:type:`pw::InlineBasicString` supports class template argument deduction
(CTAD) in C++17 and newer. Since :cpp:type:`pw::InlineString` is an alias, CTAD
is not supported until C++20.

.. code-block:: c++

   // Deduces a capacity of 5 characters to match the 5-character string literal
   // (not counting the null terminator).
   pw::InlineBasicString inline_string = "12345";

   // In C++20, CTAD may be used with the pw::InlineString alias.
   pw::InlineString my_other_string("123456789");

Exceeding the capacity
----------------------
Any :cpp:type:`pw::InlineString` operations that exceed the string's capacity
fail an assertion, resulting in a crash. Heplers are provided in
``pw_string/util.h`` that return ``pw::Status::ResourceExhausted()`` instead of
failing an assert when the capacity would be exceeded.

.. cpp:function:: pw::Status pw::string::Assign(pw::InlineString<>& string, const std::string_view& view)

   Assigns ``view`` to ``string``. Truncates and returns ``RESOURCE_EXHAUSTED``
   if ``view`` is too large for ``string``.

.. cpp:function:: pw::Status pw::string::Append(pw::InlineString<>& string, const std::string_view& view)

   Appends ``view`` to ``string``. Truncates and returns ``RESOURCE_EXHAUSTED``
   if ``view`` does not fit within the remaining capacity of ``string``.

Use with :cpp:class:`pw::StringBuilder`
---------------------------------------
:cpp:class:`pw::StringBuilder` may be used to build a string in a
:cpp:type:`pw::InlineString`.

pw::string::Format
==================
The ``pw::string::Format`` and ``pw::string::FormatVaList`` functions provide
safer alternatives to ``std::snprintf`` and ``std::vsnprintf``. The snprintf
return value is awkward to interpret, and misinterpreting it can lead to serious
bugs.

Size report: replacing snprintf with pw::string::Format
-------------------------------------------------------
The ``Format`` functions have a small, fixed code size cost. However, relative
to equivalent ``std::snprintf`` calls, there is no incremental code size cost to
using ``Format``.

.. include:: format_size_report

Safe Length Checking
====================
This module provides two safer alternatives to ``std::strlen`` in case the
string is extremely long and/or potentially not null-terminated.

First, a constexpr alternative to C11's ``strnlen_s`` is offerred through
:cpp:func:`pw::string::ClampedCString`. This does not return a length by
design and instead returns a string_view which does not require
null-termination.

Second, a constexpr specialized form is offered where null termination is
required through :cpp:func:`pw::string::NullTerminatedLength`. This will only
return a length if the string is null-terminated.

.. cpp:function:: constexpr std::string_view pw::string::ClampedCString(span<const char> str)
.. cpp:function:: constexpr std::string_view pw::string::ClampedCString(const char* str, size_t max_len)

   Safe alternative to the string_view constructor to avoid the risk of an
   unbounded implicit or explicit use of strlen.

   This is strongly recommended over using something like C11's strnlen_s as
   a string_view does not require null-termination.

.. cpp:function:: constexpr pw::Result<size_t> pw::string::NullTerminatedLength(span<const char> str)
.. cpp:function:: pw::Result<size_t> pw::string::NullTerminatedLength(const char* str, size_t max_len)

   Safe alternative to strlen to calculate the null-terminated length of the
   string within the specified span, excluding the null terminator. Like C11's
   strnlen_s, the scan for the null-terminator is bounded.

   Returns:
     null-terminated length of the string excluding the null terminator.
     OutOfRange - if the string is not null-terminated.

   Precondition: The string shall be at a valid pointer.

pw::string::Copy
================
The ``pw::string::Copy`` functions provide a safer alternative to
``std::strncpy`` as it always null-terminates whenever the destination
buffer has a non-zero size.

.. cpp:function:: StatusWithSize Copy(const std::string_view& source, span<char> dest)
.. cpp:function:: StatusWithSize Copy(const char* source, span<char> dest)
.. cpp:function:: StatusWithSize Copy(const char* source, char* dest, size_t num)
.. cpp:function:: StatusWithSize Copy(const pw::Vector<char>& source, span<char> dest)

   Copies the source string to the dest, truncating if the full string does not
   fit. Always null terminates if dest.size() or num > 0.

   Returns the number of characters written, excluding the null terminator. If
   the string is truncated, the status is ResourceExhausted.

   Precondition: The destination and source shall not overlap.
   Precondition: The source shall be a valid pointer.

It also has variants that provide a destination of ``pw::Vector<char>``
(see :ref:`module-pw_containers` for details) that do not store the null
terminator in the vector.

.. cpp:function:: StatusWithSize Copy(const std::string_view& source, pw::Vector<char>& dest)
.. cpp:function:: StatusWithSize Copy(const char* source, pw::Vector<char>& dest)


pw::string::PrintableCopy
=========================
The ``pw::string::PrintableCopy`` function provides a safe printable copy of a
string. It functions with the same safety of ``pw::string::Copy`` while also
converting any non-printable characters to a ``.`` char.

.. cpp:function:: StatusWithSize PrintableCopy(const std::string_view& source, span<char> dest)

pw::StringBuilder
=================
.. cpp:class:: pw::StringBuilder

  ``pw::StringBuilder`` facilitates creating formatted strings in a fixed-sized
  buffer or :cpp:type:`pw::InlineString`. It is designed to give the flexibility
  of ``std::ostringstream``, but with a small footprint.

  ``pw::StringBuilder`` supports C++ ``<<``-style output, printf formatting, and
  a few ``std::string`` functions (``append()``, ``push_back()``,
  ``pop_back()``.

The following shows basic use of a :cpp:class:`pw::StringBuilder`.

.. code-block:: cpp

  #include "pw_log/log.h"
  #include "pw_string/string_builder.h"

  pw::Status LogProducedData(std::string_view func_name,
                             span<const std::byte> data) {
    // pw::StringBuffer allocates a pw::StringBuilder with a built-in buffer.
    pw::StringBuffer<42> sb;

    // Append a std::string_view to the buffer.
    sb << func_name;

    // Append a format string to the buffer.
    sb.Format(" produced %d bytes of data: ", static_cast<int>(data.data()));

    // Append bytes as hex to the buffer.
    sb << data;

    // Log the final string.
    PW_LOG_DEBUG("%s", sb.c_str());

    // Errors encountered while mutating the string builder are tracked.
    return sb.status();
  }

Supporting custom types with StringBuilder
------------------------------------------
As with ``std::ostream``, StringBuilder supports printing custom types by
overriding the ``<<`` operator. This is is done by defining ``operator<<`` in
the same namespace as the custom type. For example:

.. code-block:: cpp

  namespace my_project {

  struct MyType {
    int foo;
    const char* bar;
  };

  pw::StringBuilder& operator<<(pw::StringBuilder& sb, const MyType& value) {
    return sb << "MyType(" << value.foo << ", " << value.bar << ')';
  }

  }  // namespace my_project

Internally, ``StringBuilder`` uses the ``ToString`` function to print. The
``ToString`` template function can be specialized to support custom types with
``StringBuilder``, though it is recommended to overload ``operator<<`` instead.
This example shows how to specialize ``pw::ToString``:

.. code-block:: cpp

  #include "pw_string/to_string.h"

  namespace pw {

  template <>
  StatusWithSize ToString<MyStatus>(MyStatus value, span<char> buffer) {
    return Copy(MyStatusString(value), buffer);
  }

  }  // namespace pw

Choosing between InlineString and StringBuilder
-----------------------------------------------
:cpp:type:`pw::InlineString` is comparable to ``std::string``, while
:cpp:class:`pw::StringBuilder` is comparable to ``std::ostringstream``.
Because :cpp:class:`pw::StringBuilder` provides high-level stream functionality,
it has more overhead than :cpp:type:`pw::InlineString`.

Use :cpp:type:`pw::InlineString` unless :cpp:class:`pw::StringBuilder`'s
capabilities are needed. Features unique to :cpp:class:`pw::StringBuilder`
include:

* Polymorphic C++ stream-style output, potentially supporting custom types.
* Non-fatal handling of failed append/format operations.
* Tracking the status of a series of operations.
* Building a string in an external buffer.

If those features are not required, use :cpp:type:`pw::InlineString`. A common
example of when to prefer :cpp:type:`pw::InlineString` is wrapping a
length-delimited string (e.g. ``std::string_view``) for APIs that require null
termination.

.. code-block:: cpp

  void ProcessName(std::string_view name) {
    PW_LOG_DEBUG("The name is %s", pw::InlineString<kMaxNameLen>(name).c_str());

Size report: replacing snprintf with pw::StringBuilder
------------------------------------------------------
StringBuilder is safe, flexible, and results in much smaller code size than
using ``std::ostringstream``. However, applications sensitive to code size
should use StringBuilder with care.

The fixed code size cost of StringBuilder is significant, though smaller than
``std::snprintf``. Using StringBuilder's << and append methods exclusively in
place of ``snprintf`` reduces code size, but ``snprintf`` may be difficult to
avoid.

The incremental code size cost of StringBuilder is comparable to ``snprintf`` if
errors are handled. Each argument to StringBuilder's ``<<`` expands to a
function call, but one or two StringBuilder appends may have a smaller code size
impact than a single ``snprintf`` call.

.. include:: string_builder_size_report

Module Configuration Options
============================
The following configuration options can be adjusted via compile-time
configuration of this module.

.. c:macro:: PW_STRING_ENABLE_DECIMAL_FLOAT_EXPANSION

   Setting this to a non-zero value will result in the ``ToString`` function
   outputting string representations of floating-point values with a decimal
   expansion after the point, by using the ``Format`` function. The default
   value of this configuration option is zero, which will result in floating
   point values being rounded to the nearest integer in their string
   representation.

   Using a non-zero value for this configuration option may incur a code size
   cost due to the dependency on ``Format``.

-----------
Future work
-----------
* StringBuilder's fixed size cost can be dramatically reduced by limiting
  support for 64-bit integers.
* Consider integrating with the tokenizer module.

Zephyr
======
To enable ``pw_string`` for Zephyr add ``CONFIG_PIGWEED_STRING=y`` to the
project's configuration.
