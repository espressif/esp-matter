# Copyright 2021 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Utilities for working with tokenized fields in protobufs."""

from typing import Iterator

from google.protobuf.descriptor import FieldDescriptor
from google.protobuf.message import Message

from pw_tokenizer.proto import options_pb2
from pw_tokenizer import detokenize, encode


def _tokenized_fields(proto: Message) -> Iterator[FieldDescriptor]:
    for field in proto.DESCRIPTOR.fields:
        extensions = field.GetOptions().Extensions
        if (
            options_pb2.format in extensions
            and extensions[options_pb2.format]
            == options_pb2.TOKENIZATION_OPTIONAL
        ):
            yield field


def decode_optionally_tokenized(
    detokenizer: detokenize.Detokenizer,
    data: bytes,
    prefix: str = encode.BASE64_PREFIX,
) -> str:
    """Decodes data that may be plain text or binary / Base64 tokenized text."""
    # Try detokenizing as binary.
    result = detokenizer.detokenize(data)
    if result.best_result() is not None:
        # Rather than just returning the detokenized string, continue
        # detokenization in case recursive Base64 detokenization is needed.
        data = str(result).encode()

    # Attempt to decode as UTF-8.
    try:
        text = data.decode()
    except UnicodeDecodeError:
        # Not UTF-8. Assume the token is unknown or the data is corrupt.
        return encode.prefixed_base64(data, prefix)

    # See if the string is prefixed Base64 or contains prefixed Base64.
    detokenized = detokenize.detokenize_base64(detokenizer, data, prefix)
    if detokenized != data:  # If anything detokenized successfully, use that.
        return detokenized.decode()

    # Attempt to determine whether this is an unknown token or plain text.
    # Any string with only printable or whitespace characters is plain text.
    if ''.join(text.split()).isprintable():
        return text

    # Assume this field is tokenized data that could not be decoded.
    return encode.prefixed_base64(data, prefix)


def detokenize_fields(
    detokenizer: detokenize.Detokenizer,
    proto: Message,
    prefix: str = encode.BASE64_PREFIX,
) -> None:
    """Detokenizes fields annotated as tokenized in the given proto.

    The fields are replaced with their detokenized version in the proto.
    Tokenized fields are bytes fields, so the detokenized string is stored as
    bytes. Call .decode() to convert the detokenized string from bytes to str.
    """
    for field in _tokenized_fields(proto):
        decoded = decode_optionally_tokenized(
            detokenizer, getattr(proto, field.name), prefix
        )
        setattr(proto, field.name, decoded.encode())
