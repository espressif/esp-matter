#!/usr/bin/env python
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
"""Contains the Python decoder tests and generates C++ decoder tests."""

import queue
from typing import Iterator, List, NamedTuple, Optional, Tuple, Union
import unittest

from pw_build.generated_tests import Context, PyTest, TestGenerator, GroupOrTest
from pw_build.generated_tests import parse_test_generation_args
from pw_hdlc.decode import (
    Frame,
    FrameDecoder,
    FrameAndNonFrameDecoder,
    FrameStatus,
    NO_ADDRESS,
)
from pw_hdlc.protocol import frame_check_sequence as fcs
from pw_hdlc.protocol import encode_address


def _encode(address: int, control: int, data: bytes) -> bytes:
    frame = encode_address(address) + bytes([control]) + data
    frame += fcs(frame)
    frame = frame.replace(b'}', b'}\x5d')
    frame = frame.replace(b'~', b'}\x5e')
    return b''.join([b'~', frame, b'~'])


class Expected(NamedTuple):
    address: int
    control: bytes
    data: bytes
    status: FrameStatus = FrameStatus.OK

    @classmethod
    def error(cls, status: FrameStatus):
        assert status is not FrameStatus.OK
        return cls(NO_ADDRESS, b'', b'', status)

    def __eq__(self, other) -> bool:
        """Define == so an Expected and a Frame can be compared."""
        return (
            self.address == other.address
            and self.control == other.control
            and self.data == other.data
            and self.status is other.status
        )


class ExpectedRaw(NamedTuple):
    raw_encoded: bytes
    status: FrameStatus

    def __eq__(self, other) -> bool:
        """Define == so an ExpectedRaw and a Frame can be compared."""
        return (
            self.raw_encoded == other.raw_encoded
            and self.status is other.status
        )


class TestCase(NamedTuple):
    data: bytes
    frames: List[Union[Expected, ExpectedRaw]]
    raw_data: bytes


def case(data: bytes, frames: list, raw: Optional[bytes] = None) -> TestCase:
    """Creates a TestCase, filling in the default value for the raw bytes."""
    if raw is not None:
        return TestCase(data, frames, raw)
    if not frames or all(f.status is not FrameStatus.OK for f in frames):
        return TestCase(data, frames, data)
    if all(f.status is FrameStatus.OK for f in frames):
        return TestCase(data, frames, b'')
    raise AssertionError(
        f'Must specify expected non-frame data for this test case ({data=})!'
    )


_PARTIAL = fcs(b'\x0ACmsg\x5e')
_ESCAPED_FLAG_TEST_CASE = case(
    b'~\x0ACmsg}~' + _PARTIAL + b'~',
    [
        Expected.error(FrameStatus.FRAMING_ERROR),
        Expected.error(FrameStatus.FRAMING_ERROR),
    ],
)

# Test cases are a tuple with the following elements:
#
#   - raw data stream
#   - expected valid & invalid frames
#   - [optional] expected raw, non-HDLC data; defaults to the full raw data
#     stream if no valid frames are expected, or b'' if only valid frames are
#     expected
#
# These tests are executed twice: once for the standard HDLC decoder, and a
# second time for the FrameAndNonFrameDecoder. The FrameAndNonFrameDecoder tests
# flush the non-frame data to simulate a timeout or MTU overflow, so the
# expected raw data includes all bytes not in an HDLC frame.
TEST_CASES: Tuple[GroupOrTest[TestCase], ...] = (
    'Empty payload',
    case(_encode(0, 0, b''), [Expected(0, b'\0', b'')]),
    case(_encode(55, 0x99, b''), [Expected(55, b'\x99', b'')]),
    case(_encode(55, 0x99, b'') * 3, [Expected(55, b'\x99', b'')] * 3),
    'Simple one-byte payload',
    case(_encode(0, 0, b'\0'), [Expected(0, b'\0', b'\0')]),
    case(_encode(123, 0, b'A'), [Expected(123, b'\0', b'A')]),
    'Simple multi-byte payload',
    case(
        _encode(0, 0, b'Hello, world!'), [Expected(0, b'\0', b'Hello, world!')]
    ),
    case(_encode(123, 0, b'\0\0\1\0\0'), [Expected(123, b'\0', b'\0\0\1\0\0')]),
    'Escaped one-byte payload',
    case(_encode(1, 2, b'~'), [Expected(1, b'\2', b'~')]),
    case(_encode(1, 2, b'}'), [Expected(1, b'\2', b'}')]),
    case(
        _encode(1, 2, b'~') + _encode(1, 2, b'}'),
        [Expected(1, b'\2', b'~'), Expected(1, b'\2', b'}')],
    ),
    'Escaped address',
    case(_encode(0x7E, 0, b'A'), [Expected(0x7E, b'\0', b'A')]),
    case(_encode(0x7D, 0, b'B'), [Expected(0x7D, b'\0', b'B')]),
    'Escaped control',
    case(_encode(0, 0x7E, b'C'), [Expected(0, b'~', b'C')]),
    case(_encode(0, 0x7D, b'D'), [Expected(0, b'}', b'D')]),
    'Escaped address and control',
    case(_encode(0x7E, 0x7D, b'E'), [Expected(0x7E, b'}', b'E')]),
    case(_encode(0x7D, 0x7E, b'F'), [Expected(0x7D, b'~', b'F')]),
    case(_encode(0x7E, 0x7E, b'~'), [Expected(0x7E, b'~', b'~')]),
    'Multibyte address',
    case(
        _encode(128, 0, b'big address'), [Expected(128, b'\0', b'big address')]
    ),
    case(
        _encode(0xFFFFFFFF, 0, b'\0\0\1\0\0'),
        [Expected(0xFFFFFFFF, b'\0', b'\0\0\1\0\0')],
    ),
    'Multiple frames separated by single flag',
    case(
        _encode(0, 0, b'A')[:-1] + _encode(1, 2, b'123'),
        [Expected(0, b'\0', b'A'), Expected(1, b'\2', b'123')],
    ),
    case(
        _encode(0xFF, 0, b'Yo')[:-1] * 3 + b'~',
        [Expected(0xFF, b'\0', b'Yo')] * 3,
    ),
    'Empty frames produce framing errors with raw data',
    case(b'~~', [ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR)], b'~~'),
    case(
        b'~' * 10,
        [
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
        ],
    ),
    case(
        b'~~' + _encode(1, 2, b'3') + b'~' * 5,
        [
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            Expected(1, b'\2', b'3'),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            # One flag byte remains in the decoding state machine.
        ],
        b'~~~~~~~',
    ),
    case(
        b'~' * 10 + _encode(1, 2, b':O') + b'~' * 3 + _encode(3, 4, b':P'),
        [
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            Expected(1, b'\2', b':O'),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            Expected(3, b'\4', b':P'),
        ],
        b'~' * 13,
    ),
    'Cannot escape flag',
    case(
        b'~\xAA}~\xab\x00Hello' + fcs(b'\xab\0Hello') + b'~',
        [
            Expected.error(FrameStatus.FRAMING_ERROR),
            Expected(0x55, b'\0', b'Hello'),
        ],
        b'~\xAA}',
    ),
    _ESCAPED_FLAG_TEST_CASE,
    'Frame too short',
    case(b'~1~', [Expected.error(FrameStatus.FRAMING_ERROR)]),
    case(b'~12~', [Expected.error(FrameStatus.FRAMING_ERROR)]),
    case(b'~12345~', [Expected.error(FrameStatus.FRAMING_ERROR)]),
    'Multibyte address too long',
    case(
        _encode(2**100, 0, b'too long'),
        [Expected.error(FrameStatus.BAD_ADDRESS)],
    ),
    'Incorrect frame check sequence',
    case(b'~123456~', [Expected.error(FrameStatus.FCS_MISMATCH)]),
    case(
        b'~\1\2msg\xff\xff\xff\xff~', [Expected.error(FrameStatus.FCS_MISMATCH)]
    ),
    case(
        _encode(0xA, 0xB, b'???')[:-2] + _encode(1, 2, b'def'),
        [
            Expected.error(FrameStatus.FCS_MISMATCH),
            Expected(1, b'\2', b'def'),
        ],
        _encode(0xA, 0xB, b'???')[:-2],
    ),
    'Invalid escape in address',
    case(
        b'~}}\0' + fcs(b'\x5d\0') + b'~',
        [Expected.error(FrameStatus.FRAMING_ERROR)],
    ),
    'Invalid escape in control',
    case(
        b'~\0}}' + fcs(b'\0\x5d') + b'~',
        [Expected.error(FrameStatus.FRAMING_ERROR)],
    ),
    'Invalid escape in data',
    case(
        b'~\0\1}}' + fcs(b'\0\1\x5d') + b'~',
        [Expected.error(FrameStatus.FRAMING_ERROR)],
    ),
    'Frame ends with escape',
    case(b'~}~', [Expected.error(FrameStatus.FRAMING_ERROR)]),
    case(b'~\1}~', [Expected.error(FrameStatus.FRAMING_ERROR)]),
    case(b'~\1\2abc}~', [Expected.error(FrameStatus.FRAMING_ERROR)]),
    case(b'~\1\2abcd}~', [Expected.error(FrameStatus.FRAMING_ERROR)]),
    case(b'~\1\2abcd1234}~', [Expected.error(FrameStatus.FRAMING_ERROR)]),
    'Inter-frame data is only escapes',
    case(
        b'~}~}~',
        [
            Expected.error(FrameStatus.FRAMING_ERROR),
            Expected.error(FrameStatus.FRAMING_ERROR),
        ],
    ),
    case(
        b'~}}~}}~',
        [
            Expected.error(FrameStatus.FRAMING_ERROR),
            Expected.error(FrameStatus.FRAMING_ERROR),
        ],
    ),
    'Data before first flag',
    case(b'\0\1' + fcs(b'\0\1'), []),
    case(
        b'\0\1' + fcs(b'\0\1') + b'~',
        [Expected.error(FrameStatus.FRAMING_ERROR)],
    ),
    'No frames emitted until flag',
    case(_encode(1, 2, b'3')[:-1], []),
    case(b'~' + _encode(1, 2, b'3')[1:-1] * 2, []),
    'Only flag and escape characters can be escaped',
    case(
        b'~}\0' + _encode(1, 2, b'3'),
        [Expected.error(FrameStatus.FRAMING_ERROR), Expected(1, b'\2', b'3')],
        b'~}\0',
    ),
    case(
        b'~1234}a' + _encode(1, 2, b'3'),
        [Expected.error(FrameStatus.FRAMING_ERROR), Expected(1, b'\2', b'3')],
        b'~1234}a',
    ),
    'Invalid frame records raw data',
    case(b'Hello?~', [ExpectedRaw(b'Hello?~', FrameStatus.FRAMING_ERROR)]),
    case(
        b'~~Hel}}lo~',
        [
            Expected.error(FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'Hel}}lo~', FrameStatus.FRAMING_ERROR),
        ],
    ),
    case(
        b'Hello?~~~~~',
        [
            ExpectedRaw(b'Hello?~', FrameStatus.FRAMING_ERROR),
            Expected.error(FrameStatus.FRAMING_ERROR),
            Expected.error(FrameStatus.FRAMING_ERROR),
        ],
    ),
    case(
        b'~~~~Hello?~~~~~',
        [
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'Hello?~', FrameStatus.FCS_MISMATCH),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~~', FrameStatus.FRAMING_ERROR),
        ],
    ),
    case(
        b'Hello?~~Goodbye~',
        [
            ExpectedRaw(b'Hello?~', FrameStatus.FRAMING_ERROR),
            ExpectedRaw(b'~Goodbye~', FrameStatus.FCS_MISMATCH),
        ],
    ),
    'Valid data followed by frame followed by invalid',
    case(
        b'Hi~ this is a log message\r\n'
        + _encode(0, 0, b'')
        + b'More log messages!\r\n',
        [
            Expected.error(FrameStatus.FRAMING_ERROR),
            Expected.error(FrameStatus.FCS_MISMATCH),
            Expected(0, b'\0', b''),
        ],
        b'Hi~ this is a log message\r\nMore log messages!\r\n',
    ),
    case(
        b'Hi~ this is a log message\r\n',
        [Expected.error(FrameStatus.FRAMING_ERROR)],
    ),
    case(
        b'~Hi~' + _encode(1, 2, b'def') + b' How are you?',
        [Expected.error(FrameStatus.FRAMING_ERROR), Expected(1, b'\2', b'def')],
        b'~Hi~ How are you?',
    ),
)
# Formatting for the above tuple is very slow, so disable yapf. Manually enable
# it as needed to format the test cases.

_TESTS = TestGenerator(TEST_CASES)


def _expected(frames: List[Frame]) -> Iterator[str]:
    for i, frame in enumerate(frames, 1):
        if frame.ok():
            yield f'      Frame::Parse(kDecodedFrame{i:02}).value(),'
        elif frame.status is FrameStatus.BAD_ADDRESS:
            yield f'      Frame::Parse(kDecodedFrame{i:02}).status(),'
        else:
            yield f'      Status::DataLoss(),  // Frame {i}'


_CPP_HEADER = """\
#include "pw_hdlc/decoder.h"

#include <array>
#include <cstddef>
#include <variant>

#include "gtest/gtest.h"
#include "pw_bytes/array.h"

namespace pw::hdlc {
namespace {
"""

_CPP_FOOTER = """\
}  // namespace
}  // namespace pw::hdlc"""

_TS_HEADER = """\
import 'jasmine';

import {Buffer} from 'buffer';

import {Decoder, FrameStatus} from './decoder'
import * as protocol from './protocol'
import * as util from './util'

class Expected {
  address: number
  control: Uint8Array
  data: Uint8Array
  status: FrameStatus

  constructor(
      address: number,
      control: Uint8Array,
      data: Uint8Array,
      status: FrameStatus) {
    this.address = address;
    this.control = control;
    this.data = data;
    this.status = status;
  }
}

class ExpectedRaw {
  raw: Uint8Array
  status: FrameStatus

  constructor(raw: Uint8Array, status: FrameStatus) {
    this.status = status;
    this.raw = raw;
  }
}

describe('Decoder', () => {
  let decoder: Decoder;
  let textEncoder: TextEncoder;

  beforeEach(() => {
    decoder = new Decoder();
    textEncoder = new TextEncoder();
  });

"""
_TS_FOOTER = """\
});
"""


def _py_only_frame(frame: Frame) -> bool:
    """Returns true for frames only returned by the Python library"""
    return (
        frame.status is FrameStatus.FRAMING_ERROR and frame.raw_encoded == b'~~'
    )


def _cpp_test(ctx: Context) -> Iterator[str]:
    """Generates a C++ test for the provided test data."""
    data, _, _ = ctx.test_case
    frames = [
        f for f in list(FrameDecoder().process(data)) if not _py_only_frame(f)
    ]
    data_bytes = ''.join(rf'\x{byte:02x}' for byte in data)

    yield f'TEST(Decoder, {ctx.cc_name()}) {{'
    yield f'  static constexpr auto kData = bytes::String("{data_bytes}");\n'

    for i, frame in enumerate(frames, 1):
        if frame.ok() or frame.status is FrameStatus.BAD_ADDRESS:
            frame_bytes = ''.join(
                rf'\x{byte:02x}' for byte in frame.raw_decoded
            )
            yield (
                f'  static constexpr auto kDecodedFrame{i:02} = '
                f'bytes::String("{frame_bytes}");'
            )
        else:
            yield f'  // Frame {i}: {frame.status.value}'

    yield ''

    expected = '\n'.join(_expected(frames)) or '      // No frames'
    decoder_size = max(len(data), 8)  # Make sure large enough for a frame

    yield f"""\
  DecoderBuffer<{decoder_size}> decoder;

  static std::array<std::variant<Frame, Status>, {len(frames)}> kExpected = {{
{expected}
  }};

  size_t decoded_frames = 0;

  decoder.Process(kData, [&](const Result<Frame>& result) {{
    ASSERT_LT(decoded_frames++, kExpected.size());
    auto& expected = kExpected[decoded_frames - 1];

    if (std::holds_alternative<Status>(expected)) {{
      EXPECT_EQ(Status::DataLoss(), result.status());
    }} else {{
      ASSERT_EQ(OkStatus(), result.status());

      const Frame& decoded_frame = result.value();
      const Frame& expected_frame = std::get<Frame>(expected);
      EXPECT_EQ(expected_frame.address(), decoded_frame.address());
      EXPECT_EQ(expected_frame.control(), decoded_frame.control());
      ASSERT_EQ(expected_frame.data().size(), decoded_frame.data().size());
      EXPECT_EQ(std::memcmp(expected_frame.data().data(),
                            decoded_frame.data().data(),
                            expected_frame.data().size()),
                0);
    }}
  }});

  EXPECT_EQ(decoded_frames, kExpected.size());
}}"""


def _define_py_decoder_test(ctx: Context) -> PyTest:
    data, expected_frames, _ = ctx.test_case

    def test(self) -> None:
        self.maxDiff = None
        # Decode in one call
        self.assertEqual(
            expected_frames,
            list(FrameDecoder().process(data)),
            msg=f'{ctx.group}: {data!r}',
        )
        # Decode byte-by-byte
        decoder = FrameDecoder()
        decoded_frames: List[Frame] = []
        for i in range(len(data)):
            decoded_frames += decoder.process(data[i : i + 1])

        self.assertEqual(
            expected_frames,
            decoded_frames,
            msg=f'{ctx.group} (byte-by-byte): {data!r}',
        )

    return test


def _define_raw_decoder_py_test(ctx: Context) -> PyTest:
    raw_data, expected_frames, expected_non_frame_data = ctx.test_case

    # The non-frame data decoder only yields valid frames.
    expected_frames = [f for f in expected_frames if f.status is FrameStatus.OK]

    def test(self) -> None:
        self.maxDiff = None

        non_frame_data = bytearray()

        # Decode in one call
        decoder = FrameAndNonFrameDecoder(
            non_frame_data_handler=non_frame_data.extend
        )

        self.assertEqual(
            expected_frames,
            list(decoder.process(raw_data)),
            msg=f'{ctx.group}: {raw_data!r}',
        )

        decoder.flush_non_frame_data()
        self.assertEqual(expected_non_frame_data, bytes(non_frame_data))

        # Decode byte-by-byte
        non_frame_data.clear()
        decoder = FrameAndNonFrameDecoder(
            non_frame_data_handler=non_frame_data.extend
        )
        decoded_frames: List[Frame] = []
        for i in range(len(raw_data)):
            decoded_frames += decoder.process(raw_data[i : i + 1])

        self.assertEqual(
            expected_frames,
            decoded_frames,
            msg=f'{ctx.group} (byte-by-byte): {raw_data!r}',
        )
        decoder.flush_non_frame_data()
        self.assertEqual(expected_non_frame_data, bytes(non_frame_data))

    return test


def _ts_byte_array(data: bytes) -> str:
    return '[' + ', '.join(rf'0x{byte:02x}' for byte in data) + ']'


def _ts_test(ctx: Context) -> Iterator[str]:
    """Generates a TS test for the provided test data."""
    data, _, _ = ctx.test_case
    frames = [
        f for f in list(FrameDecoder().process(data)) if not _py_only_frame(f)
    ]
    data_bytes = _ts_byte_array(data)

    yield f'  it(\'{ctx.ts_name()}\', () => {{'
    yield f'    const data = new Uint8Array({data_bytes});'

    yield '    const expectedFrames = ['
    for frame in frames:
        control_bytes = _ts_byte_array(frame.control)
        frame_bytes = _ts_byte_array(frame.data)

        if frame is Expected:
            yield (
                f'      new Expected({frame.address}, '
                f'new Uint8Array({control_bytes}), '
                f'new Uint8Array({frame_bytes}), {frame.status}),'
            )
        else:
            raw = _ts_byte_array(frame.raw_encoded)
            yield (
                f'      new ExpectedRaw(new Uint8Array({raw}), {frame.status}),'
            )

    yield '    ].values();\n'

    yield """\
    const result = decoder.process(data);

    while (true) {
      const expectedFrame = expectedFrames.next();
      const actualFrame = result.next();
      if (expectedFrame.done && actualFrame.done) {
        break;
      }
      expect(expectedFrame.done).toBeFalse();
      expect(actualFrame.done).toBeFalse();

      const expected = expectedFrame.value;
      const actual = actualFrame.value;
      if (expected instanceof Expected) {
        expect(actual.address).toEqual(expected.address);
        expect(actual.control).toEqual(expected.control);
        expect(actual.data).toEqual(expected.data);
        expect(actual.status).toEqual(expected.status);
      } else {
        // Expected Raw
        expect(actual.rawEncoded).toEqual(expected.raw);
        expect(actual.status).toEqual(expected.status);
      }
    }
  });
"""


# Class that tests all cases in TEST_CASES.
DecoderTest = _TESTS.python_tests('DecoderTest', _define_py_decoder_test)
NonFrameDecoderTest = _TESTS.python_tests(
    'NonFrameDecoderTest', _define_raw_decoder_py_test
)


class AdditionalNonFrameDecoderTests(unittest.TestCase):
    """Additional tests for the non-frame decoder."""

    def test_shared_flags_waits_for_tilde_to_emit_data(self) -> None:
        non_frame_data = bytearray()
        decoder = FrameAndNonFrameDecoder(non_frame_data.extend)

        self.assertEqual(
            [Expected(0, b'\0', b'')], list(decoder.process(_encode(0, 0, b'')))
        )
        self.assertEqual(non_frame_data, b'')

        self.assertEqual([], list(decoder.process(b'uh oh, no tilde!')))
        self.assertEqual(non_frame_data, b'')

        self.assertEqual([], list(decoder.process(b'~')))
        self.assertEqual(non_frame_data, b'uh oh, no tilde!')

    def test_no_shared_flags_immediately_emits_data(self) -> None:
        non_frame_data = bytearray()
        decoder = FrameAndNonFrameDecoder(
            non_frame_data.extend, handle_shared_flags=False
        )

        self.assertEqual(
            [Expected(0, b'\0', b'')], list(decoder.process(_encode(0, 0, b'')))
        )
        self.assertEqual(non_frame_data, b'')

        self.assertEqual([], list(decoder.process(b'uh oh, no tilde!')))
        self.assertEqual(non_frame_data, b'uh oh, no tilde!')

    def test_emits_data_if_mtu_is_exceeded(self) -> None:
        frame_start = b'~this looks like a real frame'

        non_frame_data = bytearray()
        decoder = FrameAndNonFrameDecoder(
            non_frame_data.extend, mtu=len(frame_start)
        )

        self.assertEqual([], list(decoder.process(frame_start)))
        self.assertEqual(non_frame_data, b'')

        self.assertEqual([], list(decoder.process(b'!')))
        self.assertEqual(non_frame_data, frame_start + b'!')

    def test_emits_data_if_timeout_expires(self) -> None:
        frame_start = b'~this looks like a real frame'

        non_frame_data: 'queue.Queue[bytes]' = queue.Queue()
        decoder = FrameAndNonFrameDecoder(non_frame_data.put, timeout_s=0.001)

        self.assertEqual([], list(decoder.process(frame_start)))
        self.assertEqual(non_frame_data.get(timeout=2), frame_start)

    def test_emits_raw_data_and_valid_frame_if_flushed_partway(self) -> None:
        payload = b'Do you wanna ride in my blimp?'
        frame = _encode(1, 2, payload)

        non_frame_data = bytearray()
        decoder = FrameAndNonFrameDecoder(non_frame_data.extend)

        self.assertEqual([], list(decoder.process(frame[:5])))
        decoder.flush_non_frame_data()

        self.assertEqual(
            [Expected(1, b'\2', payload)], list(decoder.process(frame[5:]))
        )


if __name__ == '__main__':
    args = parse_test_generation_args()
    if args.generate_cc_test:
        _TESTS.cc_tests(
            args.generate_cc_test, _cpp_test, _CPP_HEADER, _CPP_FOOTER
        )
    elif args.generate_ts_test:
        _TESTS.ts_tests(args.generate_ts_test, _ts_test, _TS_HEADER, _TS_FOOTER)
    else:
        unittest.main()
