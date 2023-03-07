.. _module-pw_tokenizer:

============
pw_tokenizer
============
:bdg-primary:`host`
:bdg-primary:`device`
:bdg-secondary:`Python`
:bdg-secondary:`C++`
:bdg-secondary:`TypeScript`
:bdg-success:`stable`

Logging is critical, but developers are often forced to choose between
additional logging or saving crucial flash space. The ``pw_tokenizer`` module
helps address this by replacing printf-style strings with binary tokens during
compilation. This enables extensive logging with substantially less memory
usage.

.. note::
  This usage of the term "tokenizer" is not related to parsing! The
  module is called tokenizer because it replaces a whole string literal with an
  integer token. It does not parse strings into separate tokens.

The most common application of ``pw_tokenizer`` is binary logging, and it is
designed to integrate easily into existing logging systems. However, the
tokenizer is general purpose and can be used to tokenize any strings, with or
without printf-style arguments.

**Why tokenize strings?**

* Dramatically reduce binary size by removing string literals from binaries.
* Reduce I/O traffic, RAM, and flash usage by sending and storing compact tokens
  instead of strings. We've seen over 50% reduction in encoded log contents.
* Reduce CPU usage by replacing snprintf calls with simple tokenization code.
* Remove potentially sensitive log, assert, and other strings from binaries.

--------------
Basic overview
--------------
There are two sides to ``pw_tokenizer``, which we call tokenization and
detokenization.

* **Tokenization** converts string literals in the source code to binary tokens
  at compile time. If the string has printf-style arguments, these are encoded
  to compact binary form at runtime.
* **Detokenization** converts tokenized strings back to the original
  human-readable strings.

Here's an overview of what happens when ``pw_tokenizer`` is used:

1. During compilation, the ``pw_tokenizer`` module hashes string literals to
   generate stable 32-bit tokens.
2. The tokenization macro removes these strings by declaring them in an ELF
   section that is excluded from the final binary.
3. After compilation, strings are extracted from the ELF to build a database of
   tokenized strings for use by the detokenizer. The ELF file may also be used
   directly.
4. During operation, the device encodes the string token and its arguments, if
   any.
5. The encoded tokenized strings are sent off-device or stored.
6. Off-device, the detokenizer tools use the token database to decode the
   strings to human-readable form.

Example: tokenized logging
==========================
This example demonstrates using ``pw_tokenizer`` for logging. In this example,
tokenized logging saves ~90% in binary size (41 → 4 bytes) and 70% in encoded
size (49 → 15 bytes).

**Before**: plain text logging

+------------------+-------------------------------------------+---------------+
| Location         | Logging Content                           | Size in bytes |
+==================+===========================================+===============+
| Source contains  | ``LOG("Battery state: %s; battery         |               |
|                  | voltage: %d mV", state, voltage);``       |               |
+------------------+-------------------------------------------+---------------+
| Binary contains  | ``"Battery state: %s; battery             | 41            |
|                  | voltage: %d mV"``                         |               |
+------------------+-------------------------------------------+---------------+
|                  | (log statement is called with             |               |
|                  | ``"CHARGING"`` and ``3989`` as arguments) |               |
+------------------+-------------------------------------------+---------------+
| Device transmits | ``"Battery state: CHARGING; battery       | 49            |
|                  | voltage: 3989 mV"``                       |               |
+------------------+-------------------------------------------+---------------+
| When viewed      | ``"Battery state: CHARGING; battery       |               |
|                  | voltage: 3989 mV"``                       |               |
+------------------+-------------------------------------------+---------------+

**After**: tokenized logging

+------------------+-----------------------------------------------------------+---------+
| Location         | Logging Content                                           | Size in |
|                  |                                                           | bytes   |
+==================+===========================================================+=========+
| Source contains  | ``LOG("Battery state: %s; battery                         |         |
|                  | voltage: %d mV", state, voltage);``                       |         |
+------------------+-----------------------------------------------------------+---------+
| Binary contains  | ``d9 28 47 8e`` (0x8e4728d9)                              | 4       |
+------------------+-----------------------------------------------------------+---------+
|                  | (log statement is called with                             |         |
|                  | ``"CHARGING"`` and ``3989`` as arguments)                 |         |
+------------------+-----------------------------------------------------------+---------+
| Device transmits | =============== ============================== ========== | 15      |
|                  | ``d9 28 47 8e`` ``08 43 48 41 52 47 49 4E 47`` ``aa 3e``  |         |
|                  | --------------- ------------------------------ ---------- |         |
|                  | Token           ``"CHARGING"`` argument        ``3989``,  |         |
|                  |                                                as         |         |
|                  |                                                varint     |         |
|                  | =============== ============================== ========== |         |
+------------------+-----------------------------------------------------------+---------+
| When viewed      | ``"Battery state: CHARGING; battery voltage: 3989 mV"``   |         |
+------------------+-----------------------------------------------------------+---------+

---------------
Getting started
---------------
Integrating ``pw_tokenizer`` requires a few steps beyond building the code. This
section describes one way ``pw_tokenizer`` might be integrated with a project.
These steps can be adapted as needed.

1. Add ``pw_tokenizer`` to your build. Build files for GN, CMake, and Bazel are
   provided. For Make or other build systems, add the files specified in the
   BUILD.gn's ``pw_tokenizer`` target to the build.
2. Use the tokenization macros in your code. See `Tokenization`_.
3. Add the contents of ``pw_tokenizer_linker_sections.ld`` to your project's
   linker script. In GN and CMake, this step is done automatically.
4. Compile your code to produce an ELF file.
5. Run ``database.py create`` on the ELF file to generate a CSV token
   database. See `Managing token databases`_.
6. Commit the token database to your repository. See notes in `Database
   management`_.
7. Integrate a ``database.py add`` command to your build to automatically update
   the committed token database. In GN, use the ``pw_tokenizer_database``
   template to do this. See `Update a database`_.
8. Integrate ``detokenize.py`` or the C++ detokenization library with your tools
   to decode tokenized logs. See `Detokenization`_.

Using with Zephyr
=================
When building ``pw_tokenizer`` with Zephyr, 3 Kconfigs can be used currently:

* ``CONFIG_PIGWEED_TOKENIZER`` will automatically link ``pw_tokenizer`` as well
  as any dependencies.
* ``CONFIG_PIGWEED_TOKENIZER_BASE64`` will automatically link
  ``pw_tokenizer.base64`` as well as any dependencies.
* ``CONFIG_PIGWEED_DETOKENIZER`` will automatically link
  ``pw_tokenizer.decoder`` as well as any dependencies.

Once enabled, the tokenizer headers can be included like any Zephyr headers:

.. code-block:: cpp

   #include <pw_tokenizer/tokenize.h>

------------
Tokenization
------------
Tokenization converts a string literal to a token. If it's a printf-style
string, its arguments are encoded along with it. The results of tokenization can
be sent off device or stored in place of a full string.

Tokenization macros
===================
Adding tokenization to a project is simple. To tokenize a string, include
``pw_tokenizer/tokenize.h`` and invoke one of the ``PW_TOKENIZE_`` macros.

Tokenize a string literal
-------------------------
The ``PW_TOKENIZE_STRING`` macro converts a string literal to a ``uint32_t``
token.

.. code-block:: cpp

   constexpr uint32_t token = PW_TOKENIZE_STRING("Any string literal!");

.. admonition:: When to use this macro

   Use ``PW_TOKENIZE_STRING`` to tokenize string literals that do not have
   %-style arguments.

Tokenize to a handler function
------------------------------
``PW_TOKENIZE_TO_GLOBAL_HANDLER`` is the most efficient tokenization function,
since it takes the fewest arguments. It encodes a tokenized string to a
buffer on the stack. The size of the buffer is set with
``PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES``.

This macro is provided by the ``pw_tokenizer:global_handler`` facade. The
backend for this facade must define the ``pw_tokenizer_HandleEncodedMessage``
C-linkage function.

.. code-block:: cpp

   PW_TOKENIZE_TO_GLOBAL_HANDLER(format_string_literal, arguments...);

   void pw_tokenizer_HandleEncodedMessage(const uint8_t encoded_message[],
                                          size_t size_bytes);

``PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD`` is similar, but passes a
``uintptr_t`` argument to the global handler function. Values like a log level
can be packed into the ``uintptr_t``.

This macro is provided by the ``pw_tokenizer:global_handler_with_payload``
facade. The backend for this facade must define the
``pw_tokenizer_HandleEncodedMessageWithPayload`` C-linkage function.

.. code-block:: cpp

   PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(payload,
                                              format_string_literal,
                                              arguments...);

   void pw_tokenizer_HandleEncodedMessageWithPayload(
       uintptr_t payload, const uint8_t encoded_message[], size_t size_bytes);

.. admonition:: When to use these macros

   Use anytime a global handler is sufficient, particularly for widely expanded
   macros, like a logging macro. ``PW_TOKENIZE_TO_GLOBAL_HANDLER`` or
   ``PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD`` are the most efficient macros
   for tokenizing printf-style strings.

Tokenize to a callback
----------------------
``PW_TOKENIZE_TO_CALLBACK`` tokenizes to a buffer on the stack and calls a
``void(const uint8_t* buffer, size_t buffer_size)`` callback that is provided at
the call site. The size of the buffer is set with
``PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES``.

.. code-block:: cpp

   PW_TOKENIZE_TO_CALLBACK(HandlerFunction, "Format string: %x", arguments...);

.. admonition:: When to use this macro

   Use ``PW_TOKENIZE_TO_CALLBACK`` if the global handler version is already in
   use for another purpose or more flexibility is needed.

Tokenize to a buffer
--------------------
The most flexible tokenization macro is ``PW_TOKENIZE_TO_BUFFER``, which encodes
to a caller-provided buffer.

.. code-block:: cpp

   uint8_t buffer[BUFFER_SIZE];
   size_t size_bytes = sizeof(buffer);
   PW_TOKENIZE_TO_BUFFER(buffer, &size_bytes, format_string_literal, arguments...);

While ``PW_TOKENIZE_TO_BUFFER`` is maximally flexible, it takes more arguments
than the other macros, so its per-use code size overhead is larger.

.. admonition:: When to use this macro

   Use ``PW_TOKENIZE_TO_BUFFER`` to encode to a custom-sized buffer or if the
   other macros are insufficient. Avoid using ``PW_TOKENIZE_TO_BUFFER`` in
   widely expanded macros, such as a logging macro, because it will result in
   larger code size than its alternatives.

.. _module-pw_tokenizer-custom-macro:

Tokenize with a custom macro
----------------------------
Projects may need more flexbility than the standard ``pw_tokenizer`` macros
provide. To support this, projects may define custom tokenization macros. This
requires the use of two low-level ``pw_tokenizer`` macros:

.. c:macro:: PW_TOKENIZE_FORMAT_STRING(domain, mask, format, ...)

   Tokenizes a format string and sets the ``_pw_tokenizer_token`` variable to the
   token. Must be used in its own scope, since the same variable is used in every
   invocation.

   The tokenized string uses the specified :ref:`tokenization domain
   <module-pw_tokenizer-domains>`.  Use ``PW_TOKENIZER_DEFAULT_DOMAIN`` for the
   default. The token also may be masked; use ``UINT32_MAX`` to keep all bits.

.. c:macro:: PW_TOKENIZER_ARG_TYPES(...)

   Converts a series of arguments to a compact format that replaces the format
   string literal.

Use these two macros within the custom tokenization macro to call a function
that does the encoding. The following example implements a custom tokenization
macro for use with :ref:`module-pw_log_tokenized`.

.. code-block:: cpp

   #include "pw_tokenizer/tokenize.h"

   #ifndef __cplusplus
   extern "C" {
   #endif

   void EncodeTokenizedMessage(pw_tokenizer_Payload metadata,
                               pw_tokenizer_Token token,
                               pw_tokenizer_ArgTypes types,
                               ...);

   #ifndef __cplusplus
   }  // extern "C"
   #endif

   #define PW_LOG_TOKENIZED_ENCODE_MESSAGE(metadata, format, ...)         \
     do {                                                                 \
       PW_TOKENIZE_FORMAT_STRING(                                         \
           PW_TOKENIZER_DEFAULT_DOMAIN, UINT32_MAX, format, __VA_ARGS__); \
       EncodeTokenizedMessage(payload,                                    \
                              _pw_tokenizer_token,                        \
                              PW_TOKENIZER_ARG_TYPES(__VA_ARGS__)         \
                                  PW_COMMA_ARGS(__VA_ARGS__));            \
     } while (0)

In this example, the ``EncodeTokenizedMessage`` function would handle encoding
and processing the message. Encoding is done by the
``pw::tokenizer::EncodedMessage`` class or ``pw::tokenizer::EncodeArgs``
function from ``pw_tokenizer/encode_args.h``. The encoded message can then be
transmitted or stored as needed.

.. code-block:: cpp

   #include "pw_log_tokenized/log_tokenized.h"
   #include "pw_tokenizer/encode_args.h"

   void HandleTokenizedMessage(pw::log_tokenized::Metadata metadata,
                               pw::span<std::byte> message);

   extern "C" void EncodeTokenizedMessage(const pw_tokenizer_Payload metadata,
                                          const pw_tokenizer_Token token,
                                          const pw_tokenizer_ArgTypes types,
                                          ...) {
     va_list args;
     va_start(args, types);
     pw::tokenizer::EncodedMessage encoded_message(token, types, args);
     va_end(args);

     HandleTokenizedMessage(metadata, encoded_message);
   }

.. admonition:: When to use a custom macro

   Use existing tokenization macros whenever possible. A custom macro may be
   needed to support use cases like the following:

   * Variations of ``PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD`` that take
     different arguments.
   * Supporting global handler macros that use different handler functions.

Binary logging with pw_tokenizer
================================
String tokenization is perfect for logging. Consider the following log macro,
which gathers the file, line number, and log message. It calls the ``RecordLog``
function, which formats the log string, collects a timestamp, and transmits the
result.

.. code-block:: cpp

   #define LOG_INFO(format, ...) \
       RecordLog(LogLevel_INFO, __FILE_NAME__, __LINE__, format, ##__VA_ARGS__)

   void RecordLog(LogLevel level, const char* file, int line, const char* format,
                  ...) {
     if (level < current_log_level) {
       return;
     }

     int bytes = snprintf(buffer, sizeof(buffer), "%s:%d ", file, line);

     va_list args;
     va_start(args, format);
     bytes += vsnprintf(&buffer[bytes], sizeof(buffer) - bytes, format, args);
     va_end(args);

     TransmitLog(TimeSinceBootMillis(), buffer, size);
   }

It is trivial to convert this to a binary log using the tokenizer. The
``RecordLog`` call is replaced with a
``PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD`` invocation. The
``pw_tokenizer_HandleEncodedMessageWithPayload`` implementation collects the
timestamp and transmits the message with ``TransmitLog``.

.. code-block:: cpp

   #define LOG_INFO(format, ...)                   \
       PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD( \
           (pw_tokenizer_Payload)LogLevel_INFO,    \
           __FILE_NAME__ ":%d " format,            \
           __LINE__,                               \
           __VA_ARGS__);                           \

   extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
       uintptr_t level, const uint8_t encoded_message[], size_t size_bytes) {
     if (static_cast<LogLevel>(level) >= current_log_level) {
       TransmitLog(TimeSinceBootMillis(), encoded_message, size_bytes);
     }
   }

Note that the ``__FILE_NAME__`` string is directly included in the log format
string. Since the string is tokenized, this has no effect on binary size. A
``%d`` for the line number is added to the format string, so that changing the
line of the log message does not generate a new token. There is no overhead for
additional tokens, but it may not be desirable to fill a token database with
duplicate log lines.

Tokenizing function names
=========================
The string literal tokenization functions support tokenizing string literals or
constexpr character arrays (``constexpr const char[]``). In GCC and Clang, the
special ``__func__`` variable and ``__PRETTY_FUNCTION__`` extension are declared
as ``static constexpr char[]`` in C++ instead of the standard ``static const
char[]``. This means that ``__func__`` and ``__PRETTY_FUNCTION__`` can be
tokenized while compiling C++ with GCC or Clang.

.. code-block:: cpp

   // Tokenize the special function name variables.
   constexpr uint32_t function = PW_TOKENIZE_STRING(__func__);
   constexpr uint32_t pretty_function = PW_TOKENIZE_STRING(__PRETTY_FUNCTION__);

   // Tokenize the function name variables to a handler function.
   PW_TOKENIZE_TO_GLOBAL_HANDLER(__func__)
   PW_TOKENIZE_TO_GLOBAL_HANDLER(__PRETTY_FUNCTION__)

Note that ``__func__`` and ``__PRETTY_FUNCTION__`` are not string literals.
They are defined as static character arrays, so they cannot be implicitly
concatentated with string literals. For example, ``printf(__func__ ": %d",
123);`` will not compile.

Tokenization in Python
======================
The Python ``pw_tokenizer.encode`` module has limited support for encoding
tokenized messages with the ``encode_token_and_args`` function.

.. autofunction:: pw_tokenizer.encode.encode_token_and_args

This function requires a string's token is already calculated. Typically these
tokens are provided by a database, but they can be manually created using the
tokenizer hash.

.. autofunction:: pw_tokenizer.tokens.pw_tokenizer_65599_hash

This is particularly useful for offline token database generation in cases where
tokenized strings in a binary cannot be embedded as parsable pw_tokenizer
entries.

.. note::
   In C, the hash length of a string has a fixed limit controlled by
   ``PW_TOKENIZER_CFG_C_HASH_LENGTH``. To match tokens produced by C (as opposed
   to C++) code, ``pw_tokenizer_65599_hash()`` should be called with a matching
   hash length limit. When creating an offline database, it's a good idea to
   generate tokens for both, and merge the databases.

Encoding
========
The token is a 32-bit hash calculated during compilation. The string is encoded
little-endian with the token followed by arguments, if any. For example, the
31-byte string ``You can go about your business.`` hashes to 0xdac9a244.
This is encoded as 4 bytes: ``44 a2 c9 da``.

Arguments are encoded as follows:

* **Integers**  (1--10 bytes) --
  `ZagZag and varint encoded <https://developers.google.com/protocol-buffers/docs/encoding#signed-integers>`_,
  similarly to Protocol Buffers. Smaller values take fewer bytes.
* **Floating point numbers** (4 bytes) -- Single precision floating point.
* **Strings** (1--128 bytes) -- Length byte followed by the string contents.
  The top bit of the length whether the string was truncated or not. The
  remaining 7 bits encode the string length, with a maximum of 127 bytes.

.. TODO(hepler): insert diagram here!

.. tip::
   ``%s`` arguments can quickly fill a tokenization buffer. Keep ``%s``
   arguments short or avoid encoding them as strings (e.g. encode an enum as an
   integer instead of a string). See also `Tokenized strings as %s arguments`_.

Encoding command line utility
-----------------------------
The ``pw_tokenizer.encode`` command line tool can be used to encode tokenized
strings.

.. code-block:: bash

  python -m pw_tokenizer.encode [-h] FORMAT_STRING [ARG ...]

Example:

.. code-block:: text

  $ python -m pw_tokenizer.encode "There's... %d many of %s!" 2 them
        Raw input: "There's... %d many of %s!" % (2, 'them')
  Formatted input: There's... 2 many of them!
            Token: 0xb6ef8b2d
          Encoded: b'-\x8b\xef\xb6\x04\x04them' (2d 8b ef b6 04 04 74 68 65 6d) [10 bytes]
  Prefixed Base64: $LYvvtgQEdGhlbQ==

See ``--help`` for full usage details.

Token generation: fixed length hashing at compile time
======================================================
String tokens are generated using a modified version of the x65599 hash used by
the SDBM project. All hashing is done at compile time.

In C code, strings are hashed with a preprocessor macro. For compatibility with
macros, the hash must be limited to a fixed maximum number of characters. This
value is set by ``PW_TOKENIZER_CFG_C_HASH_LENGTH``. Increasing
``PW_TOKENIZER_CFG_C_HASH_LENGTH`` increases the compilation time for C due to
the complexity of the hashing macros.

C++ macros use a constexpr function instead of a macro. This function works with
any length of string and has lower compilation time impact than the C macros.
For consistency, C++ tokenization uses the same hash algorithm, but the
calculated values will differ between C and C++ for strings longer than
``PW_TOKENIZER_CFG_C_HASH_LENGTH`` characters.

.. _module-pw_tokenizer-domains:

Tokenization domains
====================
``pw_tokenizer`` supports having multiple tokenization domains. Domains are a
string label associated with each tokenized string. This allows projects to keep
tokens from different sources separate. Potential use cases include the
following:

* Keep large sets of tokenized strings separate to avoid collisions.
* Create a separate database for a small number of strings that use truncated
  tokens, for example only 10 or 16 bits instead of the full 32 bits.

If no domain is specified, the domain is empty (``""``). For many projects, this
default domain is sufficient, so no additional configuration is required.

.. code-block:: cpp

   // Tokenizes this string to the default ("") domain.
   PW_TOKENIZE_STRING("Hello, world!");

   // Tokenizes this string to the "my_custom_domain" domain.
   PW_TOKENIZE_STRING_DOMAIN("my_custom_domain", "Hello, world!");

The database and detokenization command line tools default to reading from the
default domain. The domain may be specified for ELF files by appending
``#DOMAIN_NAME`` to the file path. Use ``#.*`` to read from all domains. For
example, the following reads strings in ``some_domain`` from ``my_image.elf``.

.. code-block:: sh

   ./database.py create --database my_db.csv path/to/my_image.elf#some_domain

See `Managing token databases`_ for information about the ``database.py``
command line tool.

Smaller tokens with masking
===========================
``pw_tokenizer`` uses 32-bit tokens. On 32-bit or 64-bit architectures, using
fewer than 32 bits does not improve runtime or code size efficiency. However,
when tokens are packed into data structures or stored in arrays, the size of the
token directly affects memory usage. In those cases, every bit counts, and it
may be desireable to use fewer bits for the token.

``pw_tokenizer`` allows users to provide a mask to apply to the token. This
masked token is used in both the token database and the code. The masked token
is not a masked version of the full 32-bit token, the masked token is the token.
This makes it trivial to decode tokens that use fewer than 32 bits.

Masking functionality is provided through the ``*_MASK`` versions of the macros.
For example, the following generates 16-bit tokens and packs them into an
existing value.

.. code-block:: cpp

   constexpr uint32_t token = PW_TOKENIZE_STRING_MASK("domain", 0xFFFF, "Pigweed!");
   uint32_t packed_word = (other_bits << 16) | token;

Tokens are hashes, so tokens of any size have a collision risk. The fewer bits
used for tokens, the more likely two strings are to hash to the same token. See
`token collisions`_.

Masked tokens without arguments may be encoded in fewer bytes. For example, the
16-bit token ``0x1234`` may be encoded as two little-endian bytes (``34 12``)
rather than four (``34 12 00 00``). The detokenizer tools zero-pad data smaller
than four bytes. Tokens with arguments must always be encoded as four bytes.

Token collisions
================
Tokens are calculated with a hash function. It is possible for different
strings to hash to the same token. When this happens, multiple strings will have
the same token in the database, and it may not be possible to unambiguously
decode a token.

The detokenization tools attempt to resolve collisions automatically. Collisions
are resolved based on two things:

- whether the tokenized data matches the strings arguments' (if any), and
- if / when the string was marked as having been removed from the database.

Working with collisions
-----------------------
Collisions may occur occasionally. Run the command
``python -m pw_tokenizer.database report <database>`` to see information about a
token database, including any collisions.

If there are collisions, take the following steps to resolve them.

- Change one of the colliding strings slightly to give it a new token.
- In C (not C++), artificial collisions may occur if strings longer than
  ``PW_TOKENIZER_CFG_C_HASH_LENGTH`` are hashed. If this is happening, consider
  setting ``PW_TOKENIZER_CFG_C_HASH_LENGTH`` to a larger value.  See
  ``pw_tokenizer/public/pw_tokenizer/config.h``.
- Run the ``mark_removed`` command with the latest version of the build
  artifacts to mark missing strings as removed. This deprioritizes them in
  collision resolution.

  .. code-block:: sh

     python -m pw_tokenizer.database mark_removed --database <database> <ELF files>

  The ``purge`` command may be used to delete these tokens from the database.

Probability of collisions
-------------------------
Hashes of any size have a collision risk. The probability of one at least
one collision occurring for a given number of strings is unintuitively high
(this is known as the `birthday problem
<https://en.wikipedia.org/wiki/Birthday_problem>`_). If fewer than 32 bits are
used for tokens, the probability of collisions increases substantially.

This table shows the approximate number of strings that can be hashed to have a
1% or 50% probability of at least one collision (assuming a uniform, random
hash).

+-------+---------------------------------------+
| Token | Collision probability by string count |
| bits  +--------------------+------------------+
|       |         50%        |          1%      |
+=======+====================+==================+
|   32  |       77000        |        9300      |
+-------+--------------------+------------------+
|   31  |       54000        |        6600      |
+-------+--------------------+------------------+
|   24  |        4800        |         580      |
+-------+--------------------+------------------+
|   16  |         300        |          36      |
+-------+--------------------+------------------+
|    8  |          19        |           3      |
+-------+--------------------+------------------+

Keep this table in mind when masking tokens (see `Smaller tokens with
masking`_). 16 bits might be acceptable when tokenizing a small set of strings,
such as module names, but won't be suitable for large sets of strings, like log
messages.

Using tokenization in expressions
=================================
The tokenization macros above cannot be used inside expressions. For example,
the following code snippet will fail to compile:

.. code-block:: cpp

   DoSomething(PW_TOKENIZE_STRING("Fail"));

An alternate set of macros are provided for use inside expressions. These make
use of lambda functions, so while they can be used inside expressions, they
cannot be assigned to constexpr variables or be used with special function
variables like ``__func__``.

The following tokenization macros may be used inside epxressions:

* ``PW_TOKENIZE_STRING_EXPR``
* ``PW_TOKENIZE_STRING_DOMAIN_EXPR``
* ``PW_TOKENIZE_STRING_MASK_EXPR``

For example, the following code snippet will work:

.. code-block:: cpp

   DoSomething(PW_TOKENIZE_STRING_EXPR("Succeed"));

---------------
Token databases
---------------
Token databases store a mapping of tokens to the strings they represent. An ELF
file can be used as a token database, but it only contains the strings for its
exact build. A token database file aggregates tokens from multiple ELF files, so
that a single database can decode tokenized strings from any known ELF.

Token databases contain the token, removal date (if any), and string for each
tokenized string.

Token database formats
======================
Three token database formats are supported: CSV, binary, and directory. Tokens
may also be read from ELF files or ``.a`` archives, but cannot be written to
these formats.

CSV database format
-------------------
The CSV database format has three columns: the token in hexadecimal, the removal
date (if any) in year-month-day format, and the string literal, surrounded by
quotes. Quote characters within the string are represented as two quote
characters.

This example database contains six strings, three of which have removal dates.

.. code-block::

   141c35d5,          ,"The answer: ""%s"""
   2e668cd6,2019-12-25,"Jello, world!"
   7b940e2a,          ,"Hello %s! %hd %e"
   851beeb6,          ,"%u %d"
   881436a0,2020-01-01,"The answer is: %s"
   e13b0f94,2020-04-01,"%llu"

Binary database format
----------------------
The binary database format is comprised of a 16-byte header followed by a series
of 8-byte entries. Each entry stores the token and the removal date, which is
0xFFFFFFFF if there is none. The string literals are stored next in the same
order as the entries. Strings are stored with null terminators. See
`token_database.h <https://pigweed.googlesource.com/pigweed/pigweed/+/HEAD/pw_tokenizer/public/pw_tokenizer/token_database.h>`_
for full details.

The binary form of the CSV database is shown below. It contains the same
information, but in a more compact and easily processed form. It takes 141 B
compared with the CSV database's 211 B.

.. code-block:: text

   [header]
   0x00: 454b4f54 0000534e  TOKENS..
   0x08: 00000006 00000000  ........

   [entries]
   0x10: 141c35d5 ffffffff  .5......
   0x18: 2e668cd6 07e30c19  ..f.....
   0x20: 7b940e2a ffffffff  *..{....
   0x28: 851beeb6 ffffffff  ........
   0x30: 881436a0 07e40101  .6......
   0x38: e13b0f94 07e40401  ..;.....

   [string table]
   0x40: 54 68 65 20 61 6e 73 77 65 72 3a 20 22 25 73 22  The answer: "%s"
   0x50: 00 4a 65 6c 6c 6f 2c 20 77 6f 72 6c 64 21 00 48  .Jello, world!.H
   0x60: 65 6c 6c 6f 20 25 73 21 20 25 68 64 20 25 65 00  ello %s! %hd %e.
   0x70: 25 75 20 25 64 00 54 68 65 20 61 6e 73 77 65 72  %u %d.The answer
   0x80: 20 69 73 3a 20 25 73 00 25 6c 6c 75 00            is: %s.%llu.

Directory database format
-------------------------
The directory format is a collection of CSV format databases in a directory.
This format is optimized for storage in a Git repository alongside source code.
The token database commands randomly generate unique file names for the CSVs in
the database to prevent merge conflicts. Running ``mark_removed`` or ``purge``
commands in the database CLI consolidates the files to a single CSV.

The database command line tool supports a ``--discard-temporary
<upstream_commit>`` option for ``add``. In this mode, the tool attempts to
discard temporary tokens. It identifies the latest CSV not present in the
provided ``<upstream_commit>``, and tokens present that CSV that are not in the
newly added tokens are discarded. This helps keep temporary tokens (e.g from
debug logs) out of the database.

JSON support
============
While pw_tokenizer doesn't specify a JSON database format, a token database can
be created from a JSON formatted array of strings. This is useful for side-band
token database generation for strings that are not embedded as parsable tokens
in compiled binaries. See :ref:`module-pw_tokenizer-database-creation` for
instructions on generating a token database from a JSON file.

Managing token databases
========================
Token databases are managed with the ``database.py`` script. This script can be
used to extract tokens from compilation artifacts and manage database files.
Invoke ``database.py`` with ``-h`` for full usage information.

An example ELF file with tokenized logs is provided at
``pw_tokenizer/py/example_binary_with_tokenized_strings.elf``. You can use that
file to experiment with the ``database.py`` commands.

.. _module-pw_tokenizer-database-creation:

Create a database
-----------------
The ``create`` command makes a new token database from ELF files (.elf, .o, .so,
etc.), archives (.a), existing token databases (CSV or binary), or a JSON file
containing an array of strings.

.. code-block:: sh

   ./database.py create --database DATABASE_NAME ELF_OR_DATABASE_FILE...

Two database output formats are supported: CSV and binary. Provide
``--type binary`` to ``create`` to generate a binary database instead of the
default CSV. CSV databases are great for checking into a source control or for
human review. Binary databases are more compact and simpler to parse. The C++
detokenizer library only supports binary databases currently.

Update a database
-----------------
As new tokenized strings are added, update the database with the ``add``
command.

.. code-block:: sh

   ./database.py add --database DATABASE_NAME ELF_OR_DATABASE_FILE...

This command adds new tokens from ELF files or other databases to the database.
Adding tokens already present in the database updates the date removed, if any,
to the latest.

A CSV token database can be checked into a source repository and updated as code
changes are made. The build system can invoke ``database.py`` to update the
database after each build.

GN integration
--------------
Token databases may be updated or created as part of a GN build. The
``pw_tokenizer_database`` template provided by
``$dir_pw_tokenizer/database.gni`` automatically updates an in-source tokenized
strings database or creates a new database with artifacts from one or more GN
targets or other database files.

To create a new database, set the ``create`` variable to the desired database
type (``"csv"`` or ``"binary"``). The database will be created in the output
directory. To update an existing database, provide the path to the database with
the ``database`` variable.

.. code-block::

   import("//build_overrides/pigweed.gni")

   import("$dir_pw_tokenizer/database.gni")

   pw_tokenizer_database("my_database") {
     database = "database_in_the_source_tree.csv"
     targets = [ "//firmware/image:foo(//targets/my_board:some_toolchain)" ]
     input_databases = [ "other_database.csv" ]
   }

Instead of specifying GN targets, paths or globs to output files may be provided
with the ``paths`` option.

.. code-block::

   pw_tokenizer_database("my_database") {
     database = "database_in_the_source_tree.csv"
     deps = [ ":apps" ]
     optional_paths = [ "$root_build_dir/**/*.elf" ]
   }

.. note::

   The ``paths`` and ``optional_targets`` arguments do not add anything to
   ``deps``, so there is no guarantee that the referenced artifacts will exist
   when the database is updated. Provide ``targets`` or ``deps`` or build other
   GN targets first if this is a concern.

--------------
Detokenization
--------------
Detokenization is the process of expanding a token to the string it represents
and decoding its arguments. This module provides Python, C++ and TypeScript
detokenization libraries.

**Example: decoding tokenized logs**

A project might tokenize its log messages with the `Base64 format`_. Consider
the following log file, which has four tokenized logs and one plain text log:

.. code-block:: text

   20200229 14:38:58 INF $HL2VHA==
   20200229 14:39:00 DBG $5IhTKg==
   20200229 14:39:20 DBG Crunching numbers to calculate probability of success
   20200229 14:39:21 INF $EgFj8lVVAUI=
   20200229 14:39:23 ERR $DFRDNwlOT1RfUkVBRFk=

The project's log strings are stored in a database like the following:

.. code-block::

   1c95bd1c,          ,"Initiating retrieval process for recovery object"
   2a5388e4,          ,"Determining optimal approach and coordinating vectors"
   3743540c,          ,"Recovery object retrieval failed with status %s"
   f2630112,          ,"Calculated acceptable probability of success (%.2f%%)"

Using the detokenizing tools with the database, the logs can be decoded:

.. code-block:: text

   20200229 14:38:58 INF Initiating retrieval process for recovery object
   20200229 14:39:00 DBG Determining optimal algorithm and coordinating approach vectors
   20200229 14:39:20 DBG Crunching numbers to calculate probability of success
   20200229 14:39:21 INF Calculated acceptable probability of success (32.33%)
   20200229 14:39:23 ERR Recovery object retrieval failed with status NOT_READY

.. note::

   This example uses the `Base64 format`_, which occupies about 4/3 (133%) as
   much space as the default binary format when encoded. For projects that wish
   to interleave tokenized with plain text, using Base64 is a worthwhile
   tradeoff.

Python
======
To detokenize in Python, import ``Detokenizer`` from the ``pw_tokenizer``
package, and instantiate it with paths to token databases or ELF files.

.. code-block:: python

   import pw_tokenizer

   detokenizer = pw_tokenizer.Detokenizer('path/to/database.csv', 'other/path.elf')

   def process_log_message(log_message):
       result = detokenizer.detokenize(log_message.payload)
       self._log(str(result))

The ``pw_tokenizer`` package also provides the ``AutoUpdatingDetokenizer``
class, which can be used in place of the standard ``Detokenizer``. This class
monitors database files for changes and automatically reloads them when they
change. This is helpful for long-running tools that use detokenization. The
class also supports token domains for the given database files in the
``<path>#<domain>`` format.

For messages that are optionally tokenized and may be encoded as binary,
Base64, or plaintext UTF-8, use
:func:`pw_tokenizer.proto.decode_optionally_tokenized`. This will attempt to
determine the correct method to detokenize and always provide a printable
string. For more information on this feature, see
:ref:`module-pw_tokenizer-proto`.

C++
===
The C++ detokenization libraries can be used in C++ or any language that can
call into C++ with a C-linkage wrapper, such as Java or Rust. A reference
Java Native Interface (JNI) implementation is provided.

The C++ detokenization library uses binary-format token databases (created with
``database.py create --type binary``). Read a binary format database from a
file or include it in the source code. Pass the database array to
``TokenDatabase::Create``, and construct a detokenizer.

.. code-block:: cpp

   Detokenizer detokenizer(TokenDatabase::Create(token_database_array));

   std::string ProcessLog(span<uint8_t> log_data) {
     return detokenizer.Detokenize(log_data).BestString();
   }

The ``TokenDatabase`` class verifies that its data is valid before using it. If
it is invalid, the ``TokenDatabase::Create`` returns an empty database for which
``ok()`` returns false. If the token database is included in the source code,
this check can be done at compile time.

.. code-block:: cpp

   // This line fails to compile with a static_assert if the database is invalid.
   constexpr TokenDatabase kDefaultDatabase =  TokenDatabase::Create<kData>();

   Detokenizer OpenDatabase(std::string_view path) {
     std::vector<uint8_t> data = ReadWholeFile(path);

     TokenDatabase database = TokenDatabase::Create(data);

     // This checks if the file contained a valid database. It is safe to use a
     // TokenDatabase that failed to load (it will be empty), but it may be
     // desirable to provide a default database or otherwise handle the error.
     if (database.ok()) {
       return Detokenizer(database);
     }
     return Detokenizer(kDefaultDatabase);
   }


TypeScript
==========
To detokenize in TypeScript, import ``Detokenizer`` from the ``pigweedjs``
package, and instantiate it with a CSV token database.

.. code-block:: typescript

   import { pw_tokenizer, pw_hdlc } from 'pigweedjs';
   const { Detokenizer } = pw_tokenizer;
   const { Frame } = pw_hdlc;

   const detokenizer = new Detokenizer(String(tokenCsv));

   function processLog(frame: Frame){
     const result = detokenizer.detokenize(frame);
     console.log(result);
   }

For messages that are encoded in Base64, use ``Detokenizer::detokenizeBase64``.
`detokenizeBase64` will also attempt to detokenize nested Base64 tokens. There
is also `detokenizeUint8Array` that works just like `detokenize` but expects
`Uint8Array` instead of a `Frame` argument.

Protocol buffers
================
``pw_tokenizer`` provides utilities for handling tokenized fields in protobufs.
See :ref:`module-pw_tokenizer-proto` for details.

.. toctree::
   :hidden:

   proto.rst

-------------
Base64 format
-------------
The tokenizer encodes messages to a compact binary representation. Applications
may desire a textual representation of tokenized strings. This makes it easy to
use tokenized messages alongside plain text messages, but comes at a small
efficiency cost: encoded Base64 messages occupy about 4/3 (133%) as much memory
as binary messages.

The Base64 format is comprised of a ``$`` character followed by the
Base64-encoded contents of the tokenized message. For example, consider
tokenizing the string ``This is an example: %d!`` with the argument -1. The
string's token is 0x4b016e66.

.. code-block:: text

   Source code: PW_TOKENIZE_TO_GLOBAL_HANDLER("This is an example: %d!", -1);

    Plain text: This is an example: -1! [23 bytes]

        Binary: 66 6e 01 4b 01          [ 5 bytes]

        Base64: $Zm4BSwE=               [ 9 bytes]

Encoding
========
To encode with the Base64 format, add a call to
``pw::tokenizer::PrefixedBase64Encode`` or ``pw_tokenizer_PrefixedBase64Encode``
in the tokenizer handler function. For example,

.. code-block:: cpp

   void pw_tokenizer_HandleEncodedMessage(const uint8_t encoded_message[],
                                          size_t size_bytes) {
     pw::InlineBasicString base64 = pw::tokenizer::PrefixedBase64Encode(
         pw::span(encoded_message, size_bytes));

     TransmitLogMessage(base64.data(), base64.size());
   }

Decoding
========
The Python ``Detokenizer`` class supprts decoding and detokenizing prefixed
Base64 messages with ``detokenize_base64`` and related methods.

.. tip::
   The Python detokenization tools support recursive detokenization for prefixed
   Base64 text. Tokenized strings found in detokenized text are detokenized, so
   prefixed Base64 messages can be passed as ``%s`` arguments.

   For example, the tokenized string for "Wow!" is ``$RhYjmQ==``. This could be
   passed as an argument to the printf-style string ``Nested message: %s``, which
   encodes to ``$pEVTYQkkUmhZam1RPT0=``. The detokenizer would decode the message
   as follows:

   ::

     "$pEVTYQkkUmhZam1RPT0=" → "Nested message: $RhYjmQ==" → "Nested message: Wow!"

Base64 decoding is supported in C++ or C with the
``pw::tokenizer::PrefixedBase64Decode`` or ``pw_tokenizer_PrefixedBase64Decode``
functions.

Investigating undecoded messages
================================
Tokenized messages cannot be decoded if the token is not recognized. The Python
package includes the ``parse_message`` tool, which parses tokenized Base64
messages without looking up the token in a database. This tool attempts to guess
the types of the arguments and displays potential ways to decode them.

This tool can be used to extract argument information from an otherwise unusable
message. It could help identify which statement in the code produced the
message. This tool is not particularly helpful for tokenized messages without
arguments, since all it can do is show the value of the unknown token.

The tool is executed by passing Base64 tokenized messages, with or without the
``$`` prefix, to ``pw_tokenizer.parse_message``. Pass ``-h`` or ``--help`` to
see full usage information.

Example
-------
.. code-block::

   $ python -m pw_tokenizer.parse_message '$329JMwA=' koSl524TRkFJTEVEX1BSRUNPTkRJVElPTgJPSw== --specs %s %d

   INF Decoding arguments for '$329JMwA='
   INF Binary: b'\xdfoI3\x00' [df 6f 49 33 00] (5 bytes)
   INF Token:  0x33496fdf
   INF Args:   b'\x00' [00] (1 bytes)
   INF Decoding with up to 8 %s or %d arguments
   INF   Attempt 1: [%s]
   INF   Attempt 2: [%d] 0

   INF Decoding arguments for '$koSl524TRkFJTEVEX1BSRUNPTkRJVElPTgJPSw=='
   INF Binary: b'\x92\x84\xa5\xe7n\x13FAILED_PRECONDITION\x02OK' [92 84 a5 e7 6e 13 46 41 49 4c 45 44 5f 50 52 45 43 4f 4e 44 49 54 49 4f 4e 02 4f 4b] (28 bytes)
   INF Token:  0xe7a58492
   INF Args:   b'n\x13FAILED_PRECONDITION\x02OK' [6e 13 46 41 49 4c 45 44 5f 50 52 45 43 4f 4e 44 49 54 49 4f 4e 02 4f 4b] (24 bytes)
   INF Decoding with up to 8 %s or %d arguments
   INF   Attempt 1: [%d %s %d %d %d] 55 FAILED_PRECONDITION 1 -40 -38
   INF   Attempt 2: [%d %s %s] 55 FAILED_PRECONDITION OK

Command line utilities
----------------------
``pw_tokenizer`` provides two standalone command line utilities for detokenizing
Base64-encoded tokenized strings.

* ``detokenize.py`` -- Detokenizes Base64-encoded strings in files or from
  stdin.
* ``serial_detokenizer.py`` -- Detokenizes Base64-encoded strings from a
  connected serial device.

If the ``pw_tokenizer`` Python package is installed, these tools may be executed
as runnable modules. For example:

.. code-block::

   # Detokenize Base64-encoded strings in a file
   python -m pw_tokenizer.detokenize -i input_file.txt

   # Detokenize Base64-encoded strings in output from a serial device
   python -m pw_tokenizer.serial_detokenizer --device /dev/ttyACM0

See the ``--help`` options for these tools for full usage information.

--------------------
Deployment war story
--------------------
The tokenizer module was developed to bring tokenized logging to an
in-development product. The product already had an established text-based
logging system. Deploying tokenization was straightforward and had substantial
benefits.

Results
=======
* Log contents shrunk by over 50%, even with Base64 encoding.

  * Significant size savings for encoded logs, even using the less-efficient
    Base64 encoding required for compatibility with the existing log system.
  * Freed valuable communication bandwidth.
  * Allowed storing many more logs in crash dumps.

* Substantial flash savings.

  * Reduced the size firmware images by up to 18%.

* Simpler logging code.

  * Removed CPU-heavy ``snprintf`` calls.
  * Removed complex code for forwarding log arguments to a low-priority task.

This section describes the tokenizer deployment process and highlights key
insights.

Firmware deployment
===================
* In the project's logging macro, calls to the underlying logging function were
  replaced with a ``PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD`` invocation.
* The log level was passed as the payload argument to facilitate runtime log
  level control.
* For this project, it was necessary to encode the log messages as text. In
  ``pw_tokenizer_HandleEncodedMessageWithPayload``, the log messages were
  encoded in the $-prefixed `Base64 format`_, then dispatched as normal log
  messages.
* Asserts were tokenized using ``PW_TOKENIZE_TO_CALLBACK``.

.. attention::
  Do not encode line numbers in tokenized strings. This results in a huge
  number of lines being added to the database, since every time code moves,
  new strings are tokenized. If :ref:`module-pw_log_tokenized` is used, line
  numbers are encoded in the log metadata. Line numbers may also be included by
  by adding ``"%d"`` to the format string and passing ``__LINE__``.

Database management
===================
* The token database was stored as a CSV file in the project's Git repo.
* The token database was automatically updated as part of the build, and
  developers were expected to check in the database changes alongside their code
  changes.
* A presubmit check verified that all strings added by a change were added to
  the token database.
* The token database included logs and asserts for all firmware images in the
  project.
* No strings were purged from the token database.

.. tip::
   Merge conflicts may be a frequent occurrence with an in-source CSV database.
   Use the `Directory database format`_ instead.

Decoding tooling deployment
===========================
* The Python detokenizer in ``pw_tokenizer`` was deployed to two places:

  * Product-specific Python command line tools, using
    ``pw_tokenizer.Detokenizer``.
  * Standalone script for decoding prefixed Base64 tokens in files or
    live output (e.g. from ``adb``), using ``detokenize.py``'s command line
    interface.

* The C++ detokenizer library was deployed to two Android apps with a Java
  Native Interface (JNI) layer.

  * The binary token database was included as a raw resource in the APK.
  * In one app, the built-in token database could be overridden by copying a
    file to the phone.

.. tip::
   Make the tokenized logging tools simple to use for your project.

   * Provide simple wrapper shell scripts that fill in arguments for the
     project. For example, point ``detokenize.py`` to the project's token
     databases.
   * Use ``pw_tokenizer.AutoUpdatingDetokenizer`` to decode in
     continuously-running tools, so that users don't have to restart the tool
     when the token database updates.
   * Integrate detokenization everywhere it is needed. Integrating the tools
     takes just a few lines of code, and token databases can be embedded in APKs
     or binaries.

---------------------------
Limitations and future work
---------------------------

GCC bug: tokenization in template functions
===========================================
GCC incorrectly ignores the section attribute for template `functions
<https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70435>`_ and `variables
<https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88061>`_. For example, the
following won't work when compiling with GCC and tokenized logging:

.. code-block:: cpp

   template <...>
   void DoThings() {
     int value = GetValue();
     // This log won't work with tokenized logs due to the templated context.
     PW_LOG_INFO("Got value: %d", value);
     ...
   }

The bug causes tokenized strings in template functions to be emitted into
``.rodata`` instead of the special tokenized string section. This causes two
problems:

1. Tokenized strings will not be discovered by the token database tools.
2. Tokenized strings may not be removed from the final binary.

There are two workarounds.

#. **Use Clang.** Clang puts the string data in the requested section, as
   expected. No extra steps are required.

#. **Move tokenization calls to a non-templated context.** Creating a separate
   non-templated function and invoking it from the template resolves the issue.
   This enables tokenizing in most cases encountered in practice with
   templates.

   .. code-block:: cpp

      // In .h file:
      void LogThings(value);

      template <...>
      void DoThings() {
        int value = GetValue();
        // This log will work: calls non-templated helper.
        LogThings(value);
        ...
      }

      // In .cc file:
      void LogThings(int value) {
        // Tokenized logging works as expected in this non-templated context.
        PW_LOG_INFO("Got value %d", value);
      }

There is a third option, which isn't implemented yet, which is to compile the
binary twice: once to extract the tokens, and once for the production binary
(without tokens). If this is interesting to you please get in touch.

64-bit tokenization
===================
The Python and C++ detokenizing libraries currently assume that strings were
tokenized on a system with 32-bit ``long``, ``size_t``, ``intptr_t``, and
``ptrdiff_t``. Decoding may not work correctly for these types if a 64-bit
device performed the tokenization.

Supporting detokenization of strings tokenized on 64-bit targets would be
simple. This could be done by adding an option to switch the 32-bit types to
64-bit. The tokenizer stores the sizes of these types in the
``.pw_tokenizer.info`` ELF section, so the sizes of these types can be verified
by checking the ELF file, if necessary.

Tokenization in headers
=======================
Tokenizing code in header files (inline functions or templates) may trigger
warnings such as ``-Wlto-type-mismatch`` under certain conditions. That
is because tokenization requires declaring a character array for each tokenized
string. If the tokenized string includes macros that change value, the size of
this character array changes, which means the same static variable is defined
with different sizes. It should be safe to suppress these warnings, but, when
possible, code that tokenizes strings with macros that can change value should
be moved to source files rather than headers.

Tokenized strings as ``%s`` arguments
=====================================
Encoding ``%s`` string arguments is inefficient, since ``%s`` strings are
encoded 1:1, with no tokenization. It would be better to send a tokenized string
literal as an integer instead of a string argument, but this is not yet
supported.

A string token could be sent by marking an integer % argument in a way
recognized by the detokenization tools. The detokenizer would expand the
argument to the string represented by the integer.

.. code-block:: cpp

   #define PW_TOKEN_ARG PRIx32 "<PW_TOKEN]"

   constexpr uint32_t answer_token = PW_TOKENIZE_STRING("Uh, who is there");

   PW_TOKENIZE_TO_GLOBAL_HANDLER("Knock knock: %" PW_TOKEN_ARG "?", answer_token);

Strings with arguments could be encoded to a buffer, but since printf strings
are null-terminated, a binary encoding would not work. These strings can be
prefixed Base64-encoded and sent as ``%s`` instead. See `Base64 format`_.

Another possibility: encode strings with arguments to a ``uint64_t`` and send
them as an integer. This would be efficient and simple, but only support a small
number of arguments.

-------------
Compatibility
-------------
* C11
* C++14
* Python 3

------------
Dependencies
------------
* ``pw_varint`` module
* ``pw_preprocessor`` module
* ``pw_span`` module
