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
"""Wrapers for pyserial classes to log read and write data."""

from contextvars import ContextVar
import logging
import textwrap

import serial  # type: ignore

from pw_console.widgets.event_count_history import EventCountHistory

_LOG = logging.getLogger('pw_console.serial_debug_logger')


def _log_hex_strings(data: bytes, prefix=''):
    """Create alinged hex number and character view log messages."""
    # Make a list of 2 character hex number strings.
    hex_numbers = textwrap.wrap(data.hex(), 2)

    hex_chars = [
        ('<' + str(b.to_bytes(1, byteorder='big')) + '>')
        .replace("<b'\\x", '', 1)  # Remove b'\x from the beginning
        .replace("<b'", '', 1)  # Remove b' from the beginning
        .replace("'>", '', 1)  # Remove ' from the end
        .rjust(2)
        for b in data
    ]

    # Replace non-printable bytes with dots.
    for i, num in enumerate(hex_numbers):
        if num == hex_chars[i]:
            hex_chars[i] = '..'

    hex_numbers_msg = ' '.join(hex_numbers)
    hex_chars_msg = ' '.join(hex_chars)

    _LOG.debug(
        '%s%s',
        prefix,
        hex_numbers_msg,
        extra=dict(
            extra_metadata_fields={
                'msg': hex_numbers_msg,
                'view': 'hex',
            }
        ),
    )
    _LOG.debug(
        '%s%s',
        prefix,
        hex_chars_msg,
        extra=dict(
            extra_metadata_fields={
                'msg': hex_chars_msg,
                'view': 'chars',
            }
        ),
    )


BANDWIDTH_HISTORY_CONTEXTVAR = ContextVar(
    'pw_console_bandwidth_history',
    default={
        'total': EventCountHistory(interval=3),
        'read': EventCountHistory(interval=3),
        'write': EventCountHistory(interval=3),
    },
)


class SerialWithLogging(serial.Serial):  # pylint: disable=too-many-ancestors
    """pyserial with read and write wrappers for logging."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.pw_bps_history = BANDWIDTH_HISTORY_CONTEXTVAR.get()

    def read(self, *args, **kwargs):
        data = super().read(*args, **kwargs)
        self.pw_bps_history['read'].log(len(data))
        self.pw_bps_history['total'].log(len(data))

        if len(data) > 0:
            prefix = 'Read %2d B: ' % len(data)
            _LOG.debug(
                '%s%s',
                prefix,
                data,
                extra=dict(
                    extra_metadata_fields={
                        'mode': 'Read',
                        'bytes': len(data),
                        'view': 'bytes',
                        'msg': str(data),
                    }
                ),
            )
            _log_hex_strings(data, prefix=prefix)

            # Print individual lines
            for line in data.decode(
                encoding='utf-8', errors='ignore'
            ).splitlines():
                _LOG.debug(
                    '%s',
                    line,
                    extra=dict(
                        extra_metadata_fields={
                            'msg': line,
                            'view': 'lines',
                        }
                    ),
                )

        return data

    def write(self, data: bytes, *args, **kwargs):
        self.pw_bps_history['write'].log(len(data))
        self.pw_bps_history['total'].log(len(data))

        if len(data) > 0:
            prefix = 'Write %2d B: ' % len(data)
            _LOG.debug(
                '%s%s',
                prefix,
                data,
                extra=dict(
                    extra_metadata_fields={
                        'mode': 'Write',
                        'bytes': len(data),
                        'view': 'bytes',
                        'msg': str(data),
                    }
                ),
            )
            _log_hex_strings(data, prefix=prefix)

            # Print individual lines
            for line in data.decode(
                encoding='utf-8', errors='ignore'
            ).splitlines():
                _LOG.debug(
                    '%s',
                    line,
                    extra=dict(
                        extra_metadata_fields={
                            'msg': line,
                            'view': 'lines',
                        }
                    ),
                )

        super().write(data, *args, **kwargs)
