# Copyright 2022 The Pigweed Authors
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
"""Parses the arguments in a Base64-encoded tokenized message.

This is useful for attempting to decode tokenized messages with arguments for
which the token is not recognized.
"""

import argparse
import base64
from dataclasses import dataclass
import logging
import sys
from typing import Collection, Iterable, Iterator, Sequence

import pw_cli.log
from pw_tokenizer.decode import FormatString, FormattedString

_LOG: logging.Logger = logging.getLogger('pw_tokenizer')

DEFAULT_FORMAT_SPECS = (
    '%s',
    '%d',
    '%f',
)

DEFAULT_MAX_ARGS = 8
PREFIX = '$'


def attempt_to_decode(
    arg_data: bytes,
    format_specs: Collection[str] = DEFAULT_FORMAT_SPECS,
    max_args: int = DEFAULT_MAX_ARGS,
    yield_failures: bool = False,
) -> Iterator[FormattedString]:
    """Attempts to decode arguments using the provided format specifiers."""
    format_strings = [(0, '')]  # (argument count, format string)

    # Each argument requires at least 1 byte.
    max_args = min(max_args, len(arg_data))

    while format_strings:
        arg_count, string = format_strings.pop(0)
        decode_attempt = FormatString(string).format(arg_data)

        if yield_failures or decode_attempt.ok():
            yield decode_attempt

        if arg_count < max_args:
            format_strings.extend(
                (arg_count + 1, string + spec) for spec in format_specs
            )


@dataclass(frozen=True)
class TokenizedMessage:
    string: str
    binary: bytes

    @property
    def token(self) -> int:
        return int.from_bytes(self.binary[:4], 'little')

    @property
    def binary_args(self) -> bytes:
        return self.binary[4:]

    @classmethod
    def parse(cls, message: str, prefix: str = '$') -> 'TokenizedMessage':
        if not message.startswith(prefix):
            raise ValueError(
                f'{message} does not start with {prefix!r} as expected'
            )

        binary = base64.b64decode(message[1:])

        if len(binary) < 4:
            raise ValueError(
                f'{message} is only {len(binary)} bytes; '
                'tokenized messages must be at least 4 bytes'
            )

        return cls(message, binary)


def _read_stdin():
    try:
        while True:
            yield input()
    except KeyboardInterrupt:
        return


def _text_list(items: Sequence, conjunction: str = 'or') -> str:
    if len(items) == 1:
        return str(items[0])

    return f'{", ".join(str(i) for i in items[:-1])} {conjunction} {items[-1]}'


def main(
    messages: Iterable[str],
    max_args: int,
    specs: Sequence[str],
    show_failures: bool,
) -> int:
    """Parses the arguments for a series of tokenized messages."""
    exit_code = 0

    for message in iter(messages) if messages else _read_stdin():
        if not message:
            continue

        if not message.startswith(PREFIX):
            message = PREFIX + message

        _LOG.info('Decoding arguments for %r', message)
        try:
            parsed = TokenizedMessage.parse(message)
        except ValueError as exc:
            _LOG.error('%s', exc)
            exit_code = 2
            continue

        _LOG.info(
            'Binary: %r [%s] (%d bytes)',
            parsed.binary,
            parsed.binary.hex(' ', 1),
            len(parsed.binary),
        )
        _LOG.info('Token:  0x%08x', parsed.token)
        _LOG.info(
            'Args:   %r [%s] (%d bytes)',
            parsed.binary_args,
            parsed.binary_args.hex(' ', 1),
            len(parsed.binary_args),
        )
        _LOG.info(
            'Decoding with up to %d %s arguments', max_args, _text_list(specs)
        )

        results = sorted(
            attempt_to_decode(
                parsed.binary_args, specs, max_args, show_failures
            ),
            key=FormattedString.score,
            reverse=True,
        )

        if not any(result.ok() for result in results):
            _LOG.warning(
                '  No combinations of up to %d %s arguments decoded '
                'successfully',
                max_args,
                _text_list(specs),
            )
            exit_code = 1

        for i, result in enumerate(results, 1):
            _LOG.info(  # pylint: disable=logging-fstring-interpolation
                f'  Attempt %{len(str(len(results)))}d: [%s] %s',
                i,
                ' '.join(str(a.specifier) for a in result.args),
                ' '.join(str(a) for a in result.args),
            )
        print()

    return exit_code


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        '--max-args',
        default=DEFAULT_MAX_ARGS,
        type=int,
        help='Maximum number of printf-style arguments',
    )
    parser.add_argument(
        '--specs',
        nargs='*',
        default=DEFAULT_FORMAT_SPECS,
        help='Which printf-style format specifiers to check',
    )
    parser.add_argument(
        '--show-failures',
        action='store_true',
        help='Show argument combintations that fail to decode',
    )
    parser.add_argument(
        'messages',
        nargs='*',
        help=(
            'Base64-encoded tokenized messages to decode; omit to read from '
            'stdin'
        ),
    )
    return parser.parse_args()


if __name__ == '__main__':
    pw_cli.log.install()
    sys.exit(main(**vars(_parse_args())))
