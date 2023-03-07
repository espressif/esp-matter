// Copyright 2022 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

/** Decodes arguments and formats them with the provided format string. */

const SPECIFIER_REGEX = /%(\.([0-9]+))?([%csdioxXufFeEaAgGnp])/g;

// Conversion specifiers by type; n is not supported.
const SIGNED_INT = 'di'.split('');
const UNSIGNED_INT = 'oxXup'.split('');
const FLOATING_POINT = 'fFeEaAgG'.split('');

enum DecodedStatusFlags {
  // Status flags for a decoded argument. These values should match the
  // DecodingStatus enum in pw_tokenizer/internal/decode.h.
  OK = 0, // decoding was successful
  MISSING = 1, // the argument was not present in the data
  TRUNCATED = 2, // the argument was truncated during encoding
  DECODE_ERROR = 4, // an error occurred while decoding the argument
  SKIPPED = 8, // argument was skipped due to a previous error
}

interface DecodedArg {
  size: number;
  value: string | number | null;
}

// ZigZag decode function from protobuf's wire_format module.
function zigzagDecode(value: number): number {
  if (!(value & 0x1)) return value >> 1;
  return (value >> 1) ^ ~0;
}

export class PrintfDecoder {
  // Reads a unicode string from the encoded data.
  private decodeString(args: Uint8Array): DecodedArg {
    if (args.length === 0) return {size: 0, value: null};
    let sizeAndStatus = args[0];
    let status = DecodedStatusFlags.OK;

    if (sizeAndStatus & 0x80) {
      status |= DecodedStatusFlags.TRUNCATED;
      sizeAndStatus &= 0x7f;
    }

    const rawData = args.slice(0, sizeAndStatus + 1);
    const data = rawData.slice(1);
    if (data.length < sizeAndStatus) {
      status |= DecodedStatusFlags.DECODE_ERROR;
    }

    const decoded = new TextDecoder().decode(data);
    return {size: rawData.length, value: decoded};
  }

  private decodeSignedInt(args: Uint8Array): DecodedArg {
    if (args.length === 0) return {size: 0, value: null};

    let count = 0;
    let result = 0;
    let shift = 0;
    for (count = 0; count < args.length; count++) {
      const byte = args[count];
      result |= (byte & 0x7f) << shift;
      if (!(byte & 0x80)) {
        return {value: zigzagDecode(result), size: count + 1};
      }
      shift += 7;
      if (shift >= 64) break;
    }

    return {size: 0, value: null};
  }

  private decodeUnsignedInt(args: Uint8Array): DecodedArg {
    const arg = this.decodeSignedInt(args);

    // Since ZigZag encoding is used, unsigned integers must be masked off to
    // their original bit length.
    if (arg.value !== null) {
      let num = arg.value as number;
      num = num >>> 0;
      arg.value = num;
    }
    return arg;
  }

  private decodeChar(args: Uint8Array): DecodedArg {
    const arg = this.decodeSignedInt(args);

    if (arg.value !== null) {
      const num = arg.value as number;
      arg.value = String.fromCharCode(num);
    }
    return arg;
  }

  private decodeFloat(args: Uint8Array, precision: string): DecodedArg {
    if (args.length < 4) return {size: 0, value: ''};
    const floatValue = new DataView(args.buffer, args.byteOffset, 4).getFloat32(
      0,
      true
    );
    if (precision) return {size: 4, value: floatValue.toFixed(parseInt(precision))}
    return {size: 4, value: floatValue};
  }

  private format(specifierType: string, args: Uint8Array, precision: string): DecodedArg {
    if (specifierType == '%') return {size: 0, value: '%'}; // literal %
    if (specifierType === 's') {
      return this.decodeString(args);
    }
    if (specifierType === 'c') {
      return this.decodeChar(args);
    }
    if (SIGNED_INT.indexOf(specifierType) !== -1) {
      return this.decodeSignedInt(args);
    }
    if (UNSIGNED_INT.indexOf(specifierType) !== -1) {
      return this.decodeUnsignedInt(args);
    }
    if (FLOATING_POINT.indexOf(specifierType) !== -1) {
      return this.decodeFloat(args, precision);
    }

    // Unsupported specifier, return as-is
    return {size: 0, value: '%' + specifierType};
  }

  decode(formatString: string, args: Uint8Array): string {
    return formatString.replace(
      SPECIFIER_REGEX,
      (_specifier, _precisionFull, precision, specifierType) => {
        const decodedArg = this.format(specifierType, args, precision);
      args = args.slice(decodedArg.size);
      if (decodedArg === null) return '';
      return String(decodedArg.value);
    });
  }
}
