.. _module-pw_log_tokenized:

----------------
pw_log_tokenized
----------------
The ``pw_log_tokenized`` module contains utilities for tokenized logging. It
connects ``pw_log`` to ``pw_tokenizer``.

C++ backend
===========
``pw_log_tokenized`` provides a backend for ``pw_log`` that tokenizes log
messages with the ``pw_tokenizer`` module. By default, log messages are
tokenized with the ``PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD`` macro.
The log level, 16-bit tokenized module name, and flags bits are passed through
the payload argument. The macro eventually passes logs to the
``pw_tokenizer_HandleEncodedMessageWithPayload`` function, which must be
implemented by the application.

Example implementation:

.. code-block:: cpp

   extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
       pw_tokenizer_Payload payload, const uint8_t message[], size_t size) {
     // The metadata object provides the log level, module token, and flags.
     // These values can be recorded and used for runtime filtering.
     pw::log_tokenized::Metadata metadata(payload);

     if (metadata.level() < current_log_level) {
       return;
     }

     if (metadata.flags() & HIGH_PRIORITY_LOG != 0) {
       EmitHighPriorityLog(metadata.module(), message, size);
     } else {
       EmitLowPriorityLog(metadata.module(), message, size);
     }
   }

See the documentation for :ref:`module-pw_tokenizer` for further details.

Metadata in the format string
-----------------------------
With tokenized logging, the log format string is converted to a 32-bit token.
Regardless of how long the format string is, it's always represented by a 32-bit
token. Because of this, metadata can be packed into the tokenized string with
no cost.

``pw_log_tokenized`` uses a simple key-value format to encode metadata in a
format string. Each field starts with the ``■`` (U+25A0 "Black Square")
character, followed by the key name, the ``♦`` (U+2666 "Black Diamond Suit")
character, and then the value. The string is encoded as UTF-8. Key names are
comprised of alphanumeric ASCII characters and underscore and start with a
letter.

.. code-block::

  "■key1♦contents1■key2♦contents2■key3♦contents3"

This format makes the message easily machine parseable and human readable. It is
extremely unlikely to conflict with log message contents due to the characters
used.

``pw_log_tokenized`` uses three fields: ``msg``, ``module``, and ``file``.
Implementations may add other fields, but they will be ignored by the
``pw_log_tokenized`` tooling.

.. code-block::

  "■msg♦Hyperdrive %d set to %f■module♦engine■file♦propulsion/hyper.cc"

Using key-value pairs allows placing the fields in any order.
``pw_log_tokenized`` places the message first. This is prefered when tokenizing
C code because the tokenizer only hashes a fixed number of characters. If the
file were first, the long path might take most of the hashed characters,
increasing the odds of a collision with other strings in that file. In C++, all
characters in the string are hashed, so the order is not important.

Metadata in the tokenizer payload argument
-------------------------------------------
``pw_log_tokenized`` packs runtime-accessible metadata into a 32-bit integer
which is passed as the "payload" argument for ``pw_log_tokenizer``'s global
handler with payload facade. Packing this metadata into a single word rather
than separate arguments reduces the code size significantly.

Four items are packed into the payload argument:

- Log level -- Used for runtime log filtering by level.
- Line number -- Used to track where a log message originated.
- Log flags -- Implementation-defined log flags.
- Tokenized :c:macro:`PW_LOG_MODULE_NAME` -- Used for runtime log filtering by
  module.

Configuring metadata bit fields
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The number of bits to use for each metadata field is configurable through macros
in ``pw_log/config.h``. The field widths must sum to 32 bits. A field with zero
bits allocated is excluded from the log metadata.

.. c:macro:: PW_LOG_TOKENIZED_LEVEL_BITS

  Bits to allocate for the log level. Defaults to :c:macro:`PW_LOG_LEVEL_BITS`
  (3).

.. c:macro:: PW_LOG_TOKENIZED_LINE_BITS

  Bits to allocate for the line number. Defaults to 11 (up to line 2047). If the
  line number is too large to be represented by this field, line is reported as
  0.

  Including the line number can slightly increase code size. Without the line
  number, the log metadata argument is the same for all logs with the same level
  and flags. With the line number, each metadata value is unique and must be
  encoded as a separate word in the binary. Systems with extreme space
  constraints may exclude line numbers by setting this macro to 0.

  It is possible to include line numbers in tokenized log format strings, but
  that is discouraged because line numbers change whenever a file is edited.
  Passing the line number with the metadata is a lightweight way to include it.

.. c:macro:: PW_LOG_TOKENIZED_FLAG_BITS

  Bits to use for implementation-defined flags. Defaults to 2.

.. c:macro:: PW_LOG_TOKENIZED_MODULE_BITS

  Bits to use for the tokenized version of :c:macro:`PW_LOG_MODULE_NAME`.
  Defaults to 16, which gives a ~1% probability of a collision with 37 module
  names.

Creating and reading Metadata payloads
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
A C++ class is provided to facilitate the creation and interpretation of packed
log metadata payloads. The ``GenericMetadata`` class allows flags, log level,
line number, and a module identifier to be packed into bit fields of
configurable size. The ``Metadata`` class simplifies the bit field width
templatization of ``GenericMetadata`` by pulling from this module's
configuration options. In most cases, it's recommended to use ``Metadata`` to
create or read metadata payloads.

A ``Metadata`` object can be created from a ``pw_tokenizer_Payload`` allowing
various peices of metadata to be read from the payload as seen below:

.. code-block:: cpp

  extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
      pw_tokenizer_Payload payload,
      const uint8_t message[],
      size_t size_bytes) {
    pw::log_tokenized::Metadata metadata = payload;
    // Check the log level to see if this log is a crash.
    if (metadata.level() == PW_LOG_LEVEL_FATAL) {
      HandleCrash(metadata, pw::ConstByteSpan(
          reinterpret_cast<const std::byte*>(message), size_bytes));
      PW_UNREACHABLE;
    }
    // ...
  }

It's also possible to construct a ``pw_tokenizer_Payload`` using the
``Metadata`` class:

.. code-block:: cpp

  // Logs an explicitly created string token.
  void LogToken(uint32_t token, int level, int line_number, int module) {
    const pw_tokenizer_Payload payload =
        log_tokenized::Metadata(
            level, module, PW_LOG_FLAGS, line_number)
            .value();
    std::array<std::byte, sizeof(token)> token_buffer =
        pw::bytes::CopyInOrder(endian::little, token);

    pw_tokenizer_HandleEncodedMessageWithPayload(
        payload,
        reinterpret_cast<const uint8_t*>(token_buffer.data()),
        token_buffer.size());
  }

Using a custom macro
--------------------
Applications may use their own macro instead of
``PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD`` by setting the
``PW_LOG_TOKENIZED_ENCODE_MESSAGE`` config macro. This macro should take
arguments equivalent to ``PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD``:

.. c:macro:: PW_LOG_TOKENIZED_ENCODE_MESSAGE(log_metadata, message, ...)

  :param log_metadata:

    Packed metadata for the log message. See the Metadata_ class for how to
    unpack the details.

  :type log_metadata: pw_tokenizer_Payload

  :param message: The log message format string (untokenized)
  :type message: :c:texpr:`const char*`

  .. _Metadata: https://cs.pigweed.dev/pigweed/+/HEAD:pw_log_tokenized/public/pw_log_tokenized/log_tokenized.h;l=113

For instructions on how to implement a custom tokenization macro, see
:ref:`module-pw_tokenizer-custom-macro`.

Build targets
-------------
The GN build for ``pw_log_tokenized`` has two targets: ``pw_log_tokenized`` and
``log_backend``. The ``pw_log_tokenized`` target provides the
``pw_log_tokenized/log_tokenized.h`` header. The ``log_backend`` target
implements the backend for the ``pw_log`` facade. ``pw_log_tokenized`` invokes
the ``pw_tokenizer:global_handler_with_payload`` facade, which must be
implemented by the user of ``pw_log_tokenized``.

Python package
==============
``pw_log_tokenized`` includes a Python package for decoding tokenized logs.

pw_log_tokenized
----------------
.. automodule:: pw_log_tokenized
  :members:
  :undoc-members:
