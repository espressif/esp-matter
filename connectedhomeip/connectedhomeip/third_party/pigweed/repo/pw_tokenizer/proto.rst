.. _module-pw_tokenizer-proto:

------------------------------------
Tokenized fields in protocol buffers
------------------------------------
Text may be represented in a few different ways:

- Plain ASCII or UTF-8 text (``This is plain text``)
- Base64-encoded tokenized message (``$ibafcA==``)
- Binary-encoded tokenized message (``89 b6 9f 70``)
- Little-endian 32-bit integer token (``0x709fb689``)

``pw_tokenizer`` provides tools for working with protobuf fields that may
contain tokenized text.

Tokenized field protobuf option
===============================
``pw_tokenizer`` provides the ``pw.tokenizer.format`` protobuf field option.
This option may be applied to a protobuf field to indicate that it may contain a
tokenized string. A string that is optionally tokenized is represented with a
single ``bytes`` field annotated with ``(pw.tokenizer.format) =
TOKENIZATION_OPTIONAL``.

For example, the following protobuf has one field that may contain a tokenized
string.

.. code-block:: protobuf

  message MessageWithOptionallyTokenizedField {
    bytes just_bytes = 1;
    bytes maybe_tokenized = 2 [(pw.tokenizer.format) = TOKENIZATION_OPTIONAL];
    string just_text = 3;
  }

Decoding optionally tokenized strings
=====================================
The encoding used for an optionally tokenized field is not recorded in the
protobuf. Despite this, the text can reliably be decoded. This is accomplished
by attempting to decode the field as binary or Base64 tokenized data before
treating it like plain text.

The following diagram describes the decoding process for optionally tokenized
fields in detail.

.. mermaid::

  flowchart TD
     start([Received bytes]) --> binary

     binary[Decode as<br>binary tokenized] --> binary_ok
     binary_ok{Detokenizes<br>successfully?} -->|no| utf8
     binary_ok -->|yes| done_binary([Display decoded binary])

     utf8[Decode as UTF-8] --> utf8_ok
     utf8_ok{Valid UTF-8?} -->|no| base64_encode
     utf8_ok -->|yes| base64

     base64_encode[Encode as<br>tokenized Base64] --> display
     display([Display encoded Base64])

     base64[Decode as<br>Base64 tokenized] --> base64_ok

     base64_ok{Fully<br>or partially<br>detokenized?} -->|no| is_plain_text
     base64_ok -->|yes| base64_results

     is_plain_text{Text is<br>printable?} -->|no| base64_encode
     is_plain_text-->|yes| plain_text

     base64_results([Display decoded Base64])
     plain_text([Display text])

Potential decoding problems
---------------------------
The decoding process for optionally tokenized fields will yield correct results
in almost every situation. In rare circumstances, it is possible for it to fail,
but these can be avoided with a low-overhead mitigation if desired.

There are two ways in which the decoding process may fail.

Accidentally interpreting plain text as tokenized binary
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
If a plain-text string happens to decode as a binary tokenized message, the
incorrect message could be displayed. This is very unlikely to occur. While many
tokens will incidentally end up being valid UTF-8 strings, it is highly unlikely
that a device will happen to log one of these strings as plain text. The
overwhelming majority of these strings will be nonsense.

If an implementation wishes to guard against this extremely improbable
situation, it is possible to prevent it. This situation is prevented by
appending 0xFF (or another byte never valid in UTF-8) to binary tokenized data
that happens to be valid UTF-8 (or all binary tokenized messages, if desired).
When decoding, if there is an extra 0xFF byte, it is discarded.

Displaying undecoded binary as plain text instead of Base64
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
If a message fails to decode as binary tokenized and it is not valid UTF-8, it
is displayed as tokenized Base64. This makes it easily recognizable as a
tokenized message and makes it simple to decode later from the text output (for
example, with an updated token database).

A binary message for which the token is not known may coincidentally be valid
UTF-8 or ASCII. 6.25% of 4-byte sequences are composed only of ASCII characters.
When decoding with an out-of-date token database, it is possible that some
binary tokenized messages will be displayed as plain text rather than tokenized
Base64.

This situation is likely to occur, but should be infrequent. Even if it does
happen, it is not a serious issue. A very small number of strings will be
displayed incorrectly, but these strings cannot be decoded anyway. One nonsense
string (e.g. ``a-D1``) would be displayed instead of another (``$YS1EMQ==``).
Updating the token database would resolve the issue, though the non-Base64 logs
would be difficult decode later from a log file.

This situation can be avoided with the same approach described in
`Accidentally interpreting plain text as tokenized binary`_. Appending
an invalid UTF-8 character prevents the undecoded binary message from being
interpreted as plain text.

Python library
==============
The ``pw_tokenizer.proto`` module defines functions that may be used to
detokenize protobuf objects in Python. The function
:func:`pw_tokenizer.proto.detokenize_fields` detokenizes all fields annotated as
tokenized, replacing them with their detokenized version. For example:

.. code-block:: python

  my_detokenizer = pw_tokenizer.Detokenizer(some_database)

  my_message = SomeMessage(tokenized_field=b'$YS1EMQ==')
  pw_tokenizer.proto.detokenize_fields(my_detokenizer, my_message)

  assert my_message.tokenized_field == b'The detokenized string! Cool!'

pw_tokenizer.proto
------------------
.. automodule:: pw_tokenizer.proto
  :members:
