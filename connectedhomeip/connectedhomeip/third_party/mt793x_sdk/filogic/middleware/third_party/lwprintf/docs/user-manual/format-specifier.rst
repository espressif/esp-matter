.. _um_format_specifier:

Format specifier
================

Syntax
******

Full syntax for format specifier is
``%[flags][width][.precision][length]type``

Flags
*****

*Flags* field may have zero or more characters, and in any order. List of supported flags:

.. rst-class:: table-nowrap

+------------------+--------------------------------------------------------------------------+
| Character        | Description                                                              |
+==================+==========================================================================+
| minus ``-``      | Left-align the output of this placeholder.                               |
|                  | The default is to right-align the output                                 |
+------------------+--------------------------------------------------------------------------+
| plus ``+``       | Prepends a plus for positive signed-numeric types.                       |
|                  | positive = ``+``, negative = ``-``                                       |
+------------------+--------------------------------------------------------------------------+
| space `` ``      | Prepends a space for positive signed-numeric types.                      |
|                  | positive = `` ``, negative = ``-``.                                      |
|                  | This flag is ignored if the + flag exists                                |
+------------------+--------------------------------------------------------------------------+
| zero ``0``       | When the *width* option is specified, prepends zeros for numeric types.  |
|                  | The default prepends spaces, if this flag is not set                     |
+------------------+--------------------------------------------------------------------------+
| apostrophe ``'`` | The integer or exponent of a decimal has the thousands                   |
|                  | grouping separator applied.                                              |
+------------------+--------------------------------------------------------------------------+
| has ``#``        | Alternate form:                                                          |
|                  | For ``g`` and ``G`` types, trailing zeros are not removed.               |
|                  | For ``f``, ``F``, ``e``, ``E``, ``g``, ``G`` types, the output always    |
|                  | contains a decimal point.                                                |
|                  | For ``o``, ``x``, ``X`` types, the text ``0``, ``0x``, ``0X``,           |
|                  | respectively, is prepended to non-zero numbers.                          |
+------------------+--------------------------------------------------------------------------+

Width
*****

*Width* field specifies a *minimum* number of characters to output, and is typically used to pad fixed-width
fields in tabulated output, where fields would otherwise be smaller.
Please keep in mind that this parameter does not truncate output is input is longer than *width* field value.

Concerning *width* field, you may:

  - Ignore it completely, output does not rely on *width* field by any means
  - Write a fixed value as part of format specifier. Number must be an integer value
  - Use asterisk ``*`` char and pass number as part of parameter.
    ``printf("%3d", 6)`` or ``printf("%*d", 3, 6)`` will generate the same output.

.. tip::
    When fixed value is used to set width field, leading zero is not counted as part of *width* field,
    but as flag instead, indicating prepend number with leading zeros

Precision
*********

*Precision* field usually specifies a maximum limit on the output, depending on the particular formatting type. For floating point numeric types, it specifies the number of digits to the right of the decimal point that the output should be rounded. For the string type, it limits the number of characters that should be output, after which the string is truncated.

*Precision* field may be omitted, or a numeric integer value, or a dynamic value when passed as another argument when indicated by an asterisk ``*``. For example, ``printf("%.*s", 3, "abcdef")`` will result in ``abc`` being printed.

Length
******

*Length* field may be ignored or one of the below:

.. rst-class:: table-nowrap

+-----------+------------------------------------------------------------------------------------------------------------------+
| Character | Description                                                                                                      |
+===========+==================================================================================================================+
| ``hh``    | For integer types, causes ``printf`` to expect an ``int``-sized integer argument which was promoted from a char  |
+-----------+------------------------------------------------------------------------------------------------------------------+
| ``h``     | For integer types, causes ``printf`` to expect an ``int``-sized integer argument which was promoted from a short |
+-----------+------------------------------------------------------------------------------------------------------------------+
| ``l``     | For integer types, causes ``printf`` to expect a ``long``-sized integer argument.                                |
|           | For floating point types, this has no effect                                                                     |
+-----------+------------------------------------------------------------------------------------------------------------------+
| ``ll``    | For integer types, causes ``printf`` to expect a ``long long``-sized integer argument                            |
+-----------+------------------------------------------------------------------------------------------------------------------+
| ``L``     | For floating point types, causes ``printf`` to expect a ``long double`` argument                                 |
+-----------+------------------------------------------------------------------------------------------------------------------+
| ``z``     | For integer types, causes ``printf`` to expect a ``size_t``-sized integer argument                               |
+-----------+------------------------------------------------------------------------------------------------------------------+
| ``j``     | For integer types, causes ``printf`` to expect a ``intmax_t``-sized integer argument                             |
+-----------+------------------------------------------------------------------------------------------------------------------+
| ``t``     | For integer types, causes ``printf`` to expect a ``ptrdiff_t``-sized integer argument                            |
+-----------+------------------------------------------------------------------------------------------------------------------+

Specifier types
***************

This is a list of standard specifiers for outputting the data to the stream.
Column *Supported* gives an overview which specifiers are actually supported by the library.

.. rst-class:: table-nowrap

+-------------+-----------+--------------------------------------------------------------------------+
| Specifier   | Supported | Description                                                              |
+=============+===========+==========================================================================+
| ``%``       | Yes       | Prints literal ``%`` character                                           |
+-------------+-----------+--------------------------------------------------------------------------+
| ``d`` ``i`` | Yes       | Prints ``signed int``. No difference between either of them              |
+-------------+-----------+--------------------------------------------------------------------------+
| ``u``       | Yes       | Prints ``unsigned int``                                                  |
+-------------+-----------+--------------------------------------------------------------------------+
| ``f`` ``F`` | Yes       | Prints ``double`` in normal fixed-point notation.                        |
|             |           | ``f`` and ``F`` only differs in how the strings for an infinite number   |
|             |           | or NaN are printed                                                       |
|             |           | (``inf``, ``infinity`` and ``nan`` for ``f``;                            |
|             |           | ``INF``, ``INFINITY`` and ``NAN`` for ``F``).                            |
+-------------+-----------+--------------------------------------------------------------------------+
| ``e`` ``E`` | Yes       | Prints ``double`` in standard form ``[-]d.ddd e[+-]ddd``.                |
|             |           | ``e`` uses lower-case and                                                |
|             |           | ``E`` uses upper-case letter for exponent annotation.                    |
+-------------+-----------+--------------------------------------------------------------------------+
| ``g`` ``G`` | Yes       | Prints ``double`` in either normal or exponential notation,              |
|             |           | whichever is more appropriate for its magnitude.                         |
|             |           | ``g`` uses lower-case letters, ``G`` uses upper-case letters.            |
|             |           | This type differs slightly from fixed-point notation                     |
|             |           | in that insignificant zeroes to the right of the decimal point are       |
|             |           | not included. Also, the decimal point is not included on whole numbers.  |
+-------------+-----------+--------------------------------------------------------------------------+
| ``x`` ``X`` | Yes       | Prints ``unsigned int`` as a hexadecimal number. ``x`` uses lower-case   |
|             |           | and ``X`` uses upper-case letters                                        |
+-------------+-----------+--------------------------------------------------------------------------+
| ``o``       | Yes       | Prints ``unsigned int`` in octal format                                  |
+-------------+-----------+--------------------------------------------------------------------------+
| ``s``       | Yes       | Prints null terminated string                                            |
+-------------+-----------+--------------------------------------------------------------------------+
| ``c``       | Yes       | Prints ``char`` type                                                     |
+-------------+-----------+--------------------------------------------------------------------------+
| ``p``       | Yes       | Prints ``void *`` in an hex-based format.                                |
|             |           | Reads input as ``unsigned int`` by default.                              |
+-------------+-----------+--------------------------------------------------------------------------+
| ``a`` ``A`` | Not yet   | Prints ``double`` in hexadecimal notation.                               |
|             |           | Currently it will print ``NaN`` when used                                |
+-------------+-----------+--------------------------------------------------------------------------+
| ``n``       | Yes       | Prints nothing but writes the number of characters successfully          |
|             |           | written so far into an integer pointer parameter                         |
+-------------+-----------+--------------------------------------------------------------------------+

Notes about float types
^^^^^^^^^^^^^^^^^^^^^^^

It is important to understand how library works under the hood to understand limitations on floating-point numbers.
When it comes to level of precision, maximum number of digits is linked to support ``long`` or ``long long`` integer types.

.. note::
    When ``long long`` type is supported by the compiler (usually part of C99 or later),
    maximum number of valid digits is ``18``, or ``9`` digits if system supports only ``long`` data types.

If application tries to use more precision digits than maximum, remaining digits are automatically printed as all ``0``.
As a consequence, output using LwPRINTF library may be different in comparison to other ``printf`` implementations.

.. tip::
    Float data type supports up to ``7`` and double up to ``15``.

Additional specifier types
**************************

LwPRINTF implementation supports some specifiers that are usually not available in standard implementation.
Those are more targeting embedded systems although they may be used in any general-purpose application

.. rst-class:: table-nowrap

+-------------+--------------------------------------------------------------------------+
| Specifier   | Description                                                              |
+=============+==========================================================================+
| ``B`` ``b`` | Prints ``unsigned int`` data as binary representation.                   |
+-------------+--------------------------------------------------------------------------+
| ``K`` ``k`` | Prints ``unsigned char`` based data array as sequence of hex numbers.    |
|             | Use *width* field to specify length of input array.                      |
|             | Use ``K`` for upper-case hex letters, ``k`` for lower-case.              |
+-------------+--------------------------------------------------------------------------+

.. literalinclude:: ../../examples/additional_format_specifiers.c
    :language: c
    :linenos:
    :caption: Additional format specifiers

.. toctree::
    :maxdepth: 2

