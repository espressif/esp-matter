# Copyright 2020 The Pigweed Authors
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
"""Decodes arguments and formats tokenized messages.

The decode(format_string, encoded_arguments) function provides a simple way to
format a string with encoded arguments. The FormatString class may also be used.

Missing, truncated, or otherwise corrupted arguments are handled and displayed
in the resulting string with an error message.
"""

from datetime import datetime
import re
import struct
from typing import Iterable, List, NamedTuple, Match, Optional, Sequence, Tuple


def zigzag_decode(value: int) -> int:
    """ZigZag decode function from protobuf's wire_format module."""
    if not value & 0x1:
        return value >> 1
    return (value >> 1) ^ (~0)


class FormatSpec:
    """Represents a format specifier parsed from a printf-style string."""

    # Regular expression for finding format specifiers.
    FORMAT_SPEC = re.compile(
        r'%(?:(?P<flags>[+\- #0]*\d*(?:\.\d+)?)'
        r'(?P<length>hh|h|ll|l|j|z|t|L)?'
        r'(?P<type>[csdioxXufFeEaAgGnp])|%)'
    )

    # Conversions to make format strings Python compatible.
    _UNSUPPORTED_LENGTH = frozenset(['hh', 'll', 'j', 'z', 't'])
    _REMAP_TYPE = {'a': 'f', 'A': 'F'}

    # Conversion specifiers by type; n is not supported.
    _SIGNED_INT = 'di'
    _UNSIGNED_INT = frozenset('oxXup')
    _FLOATING_POINT = frozenset('fFeEaAgG')

    _PACKED_FLOAT = struct.Struct('<f')

    @classmethod
    def from_string(cls, format_specifier: str):
        """Creates a FormatSpec from a str with a single format specifier."""
        match = cls.FORMAT_SPEC.fullmatch(format_specifier)

        if not match:
            raise ValueError(
                '{!r} is not a valid single format specifier'.format(
                    format_specifier
                )
            )

        return cls(match)

    def __init__(self, re_match: Match):
        """Constructs a FormatSpec from an re.Match object for FORMAT_SPEC."""
        self.match = re_match
        self.specifier: str = self.match.group()

        self.flags: str = self.match.group('flags') or ''
        self.length: str = self.match.group('length') or ''

        # If there is no type, the format spec is %%.
        self.type: str = self.match.group('type') or '%'

        # %p prints as 0xFEEDBEEF; other specs may need length/type switched
        if self.type == 'p':
            self.compatible = '0x%08X'
        else:
            self.compatible = ''.join(
                [
                    '%',
                    self.flags,
                    '' if self.length in self._UNSUPPORTED_LENGTH else '',
                    self._REMAP_TYPE.get(self.type, self.type),
                ]
            )

    def decode(self, encoded_arg: bytes) -> 'DecodedArg':
        """Decodes the provided data according to this format specifier."""
        if self.type == '%':  # literal %
            return DecodedArg(
                self, (), b''
            )  # Use () as the value for % formatting.

        if self.type == 's':  # string
            return self._decode_string(encoded_arg)

        if self.type == 'c':  # character
            return self._decode_char(encoded_arg)

        if self.type in self._SIGNED_INT:
            return self._decode_signed_integer(encoded_arg)

        if self.type in self._UNSIGNED_INT:
            return self._decode_unsigned_integer(encoded_arg)

        if self.type in self._FLOATING_POINT:
            return self._decode_float(encoded_arg)

        # Unsupported specifier (e.g. %n)
        return DecodedArg(
            self,
            None,
            b'',
            DecodedArg.DECODE_ERROR,
            'Unsupported conversion specifier "{}"'.format(self.type),
        )

    def _decode_signed_integer(self, encoded: bytes) -> 'DecodedArg':
        """Decodes a signed variable-length integer."""
        if not encoded:
            return DecodedArg.missing(self)

        count = 0
        result = 0
        shift = 0

        for byte in encoded:
            count += 1
            result |= (byte & 0x7F) << shift

            if not byte & 0x80:
                return DecodedArg(self, zigzag_decode(result), encoded[:count])

            shift += 7
            if shift >= 64:
                break

        return DecodedArg(
            self,
            None,
            encoded[:count],
            DecodedArg.DECODE_ERROR,
            'Unterminated variable-length integer',
        )

    def _decode_unsigned_integer(self, encoded: bytes) -> 'DecodedArg':
        arg = self._decode_signed_integer(encoded)

        # Since ZigZag encoding is used, unsigned integers must be masked off to
        # their original bit length.
        if arg.value is not None:
            arg.value &= (1 << self.size_bits()) - 1

        return arg

    def _decode_float(self, encoded: bytes) -> 'DecodedArg':
        if len(encoded) < 4:
            return DecodedArg.missing(self)

        return DecodedArg(
            self, self._PACKED_FLOAT.unpack_from(encoded)[0], encoded[:4]
        )

    def _decode_string(self, encoded: bytes) -> 'DecodedArg':
        """Reads a unicode string from the encoded data."""
        if not encoded:
            return DecodedArg.missing(self)

        size_and_status = encoded[0]
        status = DecodedArg.OK

        if size_and_status & 0x80:
            status |= DecodedArg.TRUNCATED
            size_and_status &= 0x7F

        raw_data = encoded[0 : size_and_status + 1]
        data = raw_data[1:]

        if len(data) < size_and_status:
            status |= DecodedArg.DECODE_ERROR

        try:
            decoded = data.decode()
        except UnicodeDecodeError as err:
            return DecodedArg(
                self,
                repr(bytes(data)).lstrip('b'),
                raw_data,
                status | DecodedArg.DECODE_ERROR,
                err,
            )

        return DecodedArg(self, decoded, raw_data, status)

    def _decode_char(self, encoded: bytes) -> 'DecodedArg':
        """Reads an integer from the data, then converts it to a string."""
        arg = self._decode_signed_integer(encoded)

        if arg.ok():
            try:
                arg.value = chr(arg.value)
            except (OverflowError, ValueError) as err:
                arg.error = err
                arg.status |= DecodedArg.DECODE_ERROR

        return arg

    def size_bits(self) -> int:
        """Size of the argument in bits; 0 for strings."""
        if self.type == 's':
            return 0

        # TODO(hepler): 64-bit targets likely have 64-bit l, j, z, and t.
        return 64 if self.length in ['ll', 'j'] else 32

    def __str__(self) -> str:
        return self.specifier


class DecodedArg:
    """Represents a decoded argument that is ready to be formatted."""

    # Status flags for a decoded argument. These values should match the
    # DecodingStatus enum in pw_tokenizer/internal/decode.h.
    OK = 0  # decoding was successful
    MISSING = 1  # the argument was not present in the data
    TRUNCATED = 2  # the argument was truncated during encoding
    DECODE_ERROR = 4  # an error occurred while decoding the argument
    SKIPPED = 8  # argument was skipped due to a previous error

    @classmethod
    def missing(cls, specifier: FormatSpec):
        return cls(specifier, None, b'', cls.MISSING)

    def __init__(
        self,
        specifier: FormatSpec,
        value,
        raw_data: bytes,
        status: int = OK,
        error=None,
    ):
        self.specifier = specifier  # FormatSpec (e.g. to represent "%0.2f")
        self.value = value  # the decoded value, or None if decoding failed
        self.raw_data = bytes(
            raw_data
        )  # the exact bytes used to decode this arg
        self._status = status
        self.error = error

    def ok(self) -> bool:
        """The argument was decoded without errors."""
        return self.status == self.OK or self.status == self.TRUNCATED

    @property
    def status(self) -> int:
        return self._status

    @status.setter
    def status(self, status: int):
        # The %% specifier is always OK and should always be printed normally.
        self._status = status if self.specifier.type != '%' else self.OK

    def format(self) -> str:
        """Returns formatted version of this argument, with error handling."""
        if self.status == self.TRUNCATED:
            return self.specifier.compatible % (self.value + '[...]')

        if self.ok():
            try:
                return self.specifier.compatible % self.value
            except (OverflowError, TypeError, ValueError) as err:
                self.status |= self.DECODE_ERROR
                self.error = err

        if self.status & self.SKIPPED:
            message = '{} SKIPPED'.format(self.specifier)
        elif self.status == self.MISSING:
            message = '{} MISSING'.format(self.specifier)
        elif self.status & self.DECODE_ERROR:
            message = '{} ERROR'.format(self.specifier)
        else:
            raise AssertionError(
                'Unhandled DecodedArg status {:x}!'.format(self.status)
            )

        if self.value is None or not str(self.value):
            return '<[{}]>'.format(message)

        return '<[{} ({})]>'.format(message, self.value)

    def __str__(self) -> str:
        return self.format()

    def __repr__(self) -> str:
        return f'DecodedArg({self})'


def parse_format_specifiers(format_string: str) -> Iterable[FormatSpec]:
    for spec in FormatSpec.FORMAT_SPEC.finditer(format_string):
        yield FormatSpec(spec)


class FormattedString(NamedTuple):
    value: str
    args: Sequence[DecodedArg]
    remaining: bytes

    def ok(self) -> bool:
        """Arg data decoded successfully and all expected args were found."""
        return all(arg.ok() for arg in self.args) and not self.remaining

    def score(self, date_removed: Optional[datetime] = None) -> tuple:
        """Returns a key for sorting by how successful a decode was.

        Decoded strings are sorted by whether they

          1. decoded all bytes for all arguments without errors,
          2. decoded all data,
          3. have the fewest decoding errors,
          4. decoded the most arguments successfully, or
          5. have the most recent removal date, if they were removed.

        This must match the collision resolution logic in detokenize.cc.

        To format a list of FormattedStrings from most to least successful,
        use sort(key=FormattedString.score, reverse=True).
        """
        return (
            self.ok(),  # decocoded all data and all expected args were found
            not self.remaining,  # decoded all data
            -sum(not arg.ok() for arg in self.args),  # fewest errors
            len(self.args),  # decoded the most arguments
            date_removed or datetime.max,
        )  # most recently present


class FormatString:
    """Represents a printf-style format string."""

    def __init__(self, format_string: str):
        """Parses format specifiers in the format string."""
        self.format_string = format_string
        self.specifiers = tuple(parse_format_specifiers(self.format_string))

        # List of non-specifier string pieces with room for formatted arguments.
        self._segments = self._parse_string_segments()

    def _parse_string_segments(self) -> List:
        """Splits the format string by format specifiers."""
        if not self.specifiers:
            return [self.format_string]

        spec_spans = [spec.match.span() for spec in self.specifiers]

        # Start with the part of the format string up to the first specifier.
        string_pieces = [self.format_string[: spec_spans[0][0]]]

        for ((_, end1), (start2, _)) in zip(spec_spans[:-1], spec_spans[1:]):
            string_pieces.append(self.format_string[end1:start2])

        # Append the format string segment after the last format specifier.
        string_pieces.append(self.format_string[spec_spans[-1][1] :])

        # Make a list with spots for the replacements between the string pieces.
        segments: List = [None] * (len(string_pieces) + len(self.specifiers))
        segments[::2] = string_pieces

        return segments

    def decode(self, encoded: bytes) -> Tuple[Sequence[DecodedArg], bytes]:
        """Decodes arguments according to the format string.

        Args:
          encoded: bytes; the encoded arguments

        Returns:
          tuple with the decoded arguments and any unparsed data
        """
        decoded_args = []

        fatal_error = False
        index = 0

        for spec in self.specifiers:
            arg = spec.decode(encoded[index:])

            if fatal_error:
                # After an error is encountered, continue to attempt to parse
                # arguments, but mark them all as SKIPPED. If an error occurs,
                # it's impossible to know if subsequent arguments are valid.
                arg.status |= DecodedArg.SKIPPED
            elif not arg.ok():
                fatal_error = True

            decoded_args.append(arg)
            index += len(arg.raw_data)

        return tuple(decoded_args), encoded[index:]

    def format(
        self, encoded_args: bytes, show_errors: bool = False
    ) -> FormattedString:
        """Decodes arguments and formats the string with them.

        Args:
          encoded_args: the arguments to decode and format the string with
          show_errors: if True, an error message is used in place of the %
              conversion specifier when an argument fails to decode

        Returns:
          tuple with the formatted string, decoded arguments, and remaining data
        """
        # Insert formatted arguments in place of each format specifier.
        args, remaining = self.decode(encoded_args)

        if show_errors:
            self._segments[1::2] = (arg.format() for arg in args)
        else:
            self._segments[1::2] = (
                arg.format() if arg.ok() else arg.specifier.specifier
                for arg in args
            )

        return FormattedString(''.join(self._segments), args, remaining)


def decode(
    format_string: str, encoded_arguments: bytes, show_errors: bool = False
) -> str:
    """Decodes arguments and formats them with the provided format string.

    Args:
      format_string: the printf-style format string
      encoded_arguments: encoded arguments with which to format
          format_string; must exclude the 4-byte string token
      show_errors: if True, an error message is used in place of the %
          conversion specifier when an argument fails to decode

    Returns:
      the printf-style formatted string
    """
    return (
        FormatString(format_string).format(encoded_arguments, show_errors).value
    )
