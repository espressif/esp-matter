.. _module-pw_log-protobuf:

-------------------
The pw_log protobuf
-------------------
``pw_log`` defines a protocol buffer for storing and transmitting log messages.
The protobuf is optimized to be maximally efficient.

Fields
======
The ``pw_log`` protobuf is defined in ``log.proto``.

.. literalinclude:: log.proto
  :language: protobuf
  :lines: 14-

Timestamps
----------
Timestamps are specified in implementation-defined ticks. Ticks could be
milliseconds, microsends, or any arbitrary duration derived from the device’s
clock.

For many applications, the timestamp field can be interpreted based on the prior
knowledge of the system. For example, ``timestamp`` might be known to be
milliseconds since boot for one core or microseconds since boot for another.

If desired, a project could collect information about clock parameters
separately from ``pw_log``, and use that to interpret log timestamps. For
example, they may call an RPC that returns a ``pw_chrono`` ``ClockParamters``
protobuf. The values from that could be used to interpret timestamp from that
device.

The ``pw_log`` proto contains two timestamp fields, only one of which may be set
at a time:

- ``timestamp`` – Absolute time when message was logged.
- ``time_since_last_entry`` – When the message was logged, relative
  to the previously encoded log message. This is used when multiple log entries
  are sent in a single ``LogEntries`` proto. The previous log entry must use the
  same time source. If logs with multiple time sources are intermingled in a
  single ``LogEntries`` proto, they must use an absolute timestamp each time the
  time source changes.

Optionally tokenized text fields
--------------------------------
Several fields in the ``pw_log`` proto store text. Examples include ``message``,
``module``, and ``thread``. These fields may contain either plain or tokenized
text, either of which is represented as a single bytes field. These fields are
marked with a protocol buffer option so the ``pw_tokenizer.proto`` module can
detect and detokenize tokenized fields as appropriate.

See :ref:`module-pw_tokenizer-proto` for details.

Packing and unpacking line_level
--------------------------------
As a way to minimize on-the-wire log message size, the log level and the line
number of a given log statement are packed into a single proto field. There are
helpers in ``pw_log/proto_utils.h`` for properly packing and unpacking this
field.

.. code-block:: cpp

   #include "pw_bytes/span.h"
   #include "pw_log/levels.h"
   #include "pw_log/proto_utils.h"
   #include "pw_protobuf/decoder.h"

  bool FilterLog(pw::ConstByteSpan serialized_log) {
    pw::protobuf::Decoder log_decoder(serialized_log);
    while (log_decoder.Next().ok()) {
      if (log_decoder.FieldNumber() == 2) {
        uint32_t line_and_level;
        entry_decoder.ReadUint32(&line_and_level);
        PW_DCHECK(entry_decoder.ok());

        uint8_t level = std::get<1>(pw::log::UnpackLineLevel(line_and_level));
        if (level < PW_LOG_LEVEL_INFO) {
          return false;
        }
      }
    }

    return true;
  }

Log encoding helpers
--------------------
Encoding logs to the ``log.proto`` format can be performed using the helpers
provided in the ``pw_log/proto_utils.h`` header. Separate helpers are provided
for encoding tokenized logs and string-based logs.

.. code-block:: cpp

   #include "pw_log/proto_utils.h"

   extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
       pw_tokenizer_Payload payload, const uint8_t data[], size_t size) {
     pw::log_tokenized::Metadata metadata(payload);
     std::byte log_buffer[kLogBufferSize];

     Result<ConstByteSpan> result = EncodeTokenizedLog(
         metadata,
         pw::as_bytes(pw::span(data, size)),
         log_buffer);
     if (result.ok()) {
       // This makes use of the encoded log proto and is custom per-product.
       // It should be implemented by the caller and is not in Pigweed.
       EmitProtoLogEntry(result.value());
     }
   }
