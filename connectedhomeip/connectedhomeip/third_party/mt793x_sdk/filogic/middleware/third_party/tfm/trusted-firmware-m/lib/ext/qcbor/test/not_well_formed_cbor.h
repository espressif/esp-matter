/*==============================================================================
 not_well_formed_cbor.h -- vectors to test for handling of not-well-formed CBOR

 This is part of QCBOR -- https://github.com/laurencelundblade/QCBOR

 Copyright (c) 2019, Laurence Lundblade. All rights reserved.

 SPDX-License-Identifier: BSD-3-Clause

 See BSD-3-Clause license in README.md

 Created on 7/27/19
 ==============================================================================*/

#ifndef not_well_formed_cbor_h
#define not_well_formed_cbor_h

#include <stdint.h> // for size_t and uint8_t


struct someBinaryBytes {
    const uint8_t *p; // Pointer to the bytes
    size_t         n; // Length of the bytes
};


static const struct someBinaryBytes paNotWellFormedCBOR[] = {

    // Indefinite length strings must be closed off

    // An indefinite length byte string not closed off
    {(uint8_t[]){0x5f, 0x41, 0x00}, 3},
    // An indefinite length text string not closed off
    {(uint8_t[]){0x7f, 0x61, 0x00}, 3},


    // All the chunks in an indefinite length string must be of the
    // type of indefinite length string and indefinite

    // indefinite length byte string with text string chunk
    {(uint8_t[]){0x5f, 0x61, 0x00, 0xff}, 4},
    // indefinite length text string with a byte string chunk
    {(uint8_t[]){0x7f, 0x41, 0x00, 0xff}, 4},
    // indefinite length byte string with an positive integer chunk
    {(uint8_t[]){0x5f, 0x00, 0xff}, 3},
    // indefinite length byte string with an negative integer chunk
    {(uint8_t[]){0x5f, 0x21, 0xff}, 3},
    // indefinite length byte string with an array chunk
    {(uint8_t[]){0x5f, 0x80, 0xff}, 3},
    // indefinite length byte string with an map chunk
    {(uint8_t[]){0x5f, 0xa0, 0xff}, 3},
    // indefinite length byte string with tagged integer chunk
    {(uint8_t[]){0x5f, 0xc0, 0x00, 0xff}, 4},
    // indefinite length byte string with an simple type chunk
    {(uint8_t[]){0x5f, 0xe0, 0xff}, 3},
    // indefinite length byte string with indefinite string inside
    {(uint8_t[]){0x5f, 0x5f, 0x41, 0x00, 0xff, 0xff}, 6},
    // indefinite length text string with indefinite string inside
    {(uint8_t[]){0x7f, 0x7f, 0x61, 0x00, 0xff, 0xff}, 6},

    // Definte length maps and arrays must be closed by having the
    // right number of items

    // A definte length array that is supposed to have 1 item, but has none
    {(uint8_t[]){0x81}, 1},
    // A definte length array that is supposed to have 2 items, but has only 1
    {(uint8_t[]){0x82, 0x00}, 2},
    // A definte length array that is supposed to have 511 items, but has only 1
    {(uint8_t[]){0x9a, 0x01, 0xff, 0x00}, 4},
    // A definte length map that is supposed to have 1 item, but has none
    {(uint8_t[]){0xa1}, 1},
    // A definte length map that is supposed to have s item, but has only 1
    {(uint8_t[]){0xa2, 0x01, 0x02}, 3},


    // Indefinte length maps and arrays must be ended by a break

    // Indefinite length array with zero items and no break
    {(uint8_t[]){0x9f}, 1},
    // Indefinite length array with two items and no break
    {(uint8_t[]){0x9f, 0x01, 0x02}, 3},
    // Indefinite length map with zero items and no break
    {(uint8_t[]){0xbf}, 1},
    // Indefinite length map with two items and no break
    {(uint8_t[]){0xbf, 0x01, 0x02, 0x01, 0x02}, 5},


    // Some extra test vectors for unclosed arrays and maps

    // Unclosed indefinite array containing a close definite array
    {(uint8_t[]){0x9f, 0x80, 0x00}, 3},
    // Definite length array containing an unclosed indefinite array
    {(uint8_t[]){0x81, 0x9f}, 2},
    // Deeply nested definite length arrays with deepest one unclosed
    {(uint8_t[]){0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81}, 9},
    // Deeply nested indefinite length arrays with deepest one unclosed
    {(uint8_t[]){0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xff, 0xff, 0xff}, 9},
    // Mixed nesting with indefinite unclosed
    {(uint8_t[]){0x9f, 0x81, 0x9f, 0x81, 0x9f, 0x9f, 0xff, 0xff, 0xff}, 9},
    // Mixed nesting with definite unclosed
    {(uint8_t[]){0x9f, 0x82, 0x9f, 0x81, 0x9f, 0x9f, 0xff, 0xff, 0xff, 0xff}, 10},


    // The "argument" for the data item is missing bytes

    // Positive integer missing 1 byte argument
    {(uint8_t[]){0x18}, 1},
    // Positive integer missing 2 byte argument
    {(uint8_t[]){0x19}, 1},
    // Positive integer missing 4 byte argument
    {(uint8_t[]){0x1a}, 1},
    // Positive integer missing 8 byte argument
    {(uint8_t[]){0x1b}, 1},
    // Positive integer missing 1 byte of 2 byte argument
    {(uint8_t[]){0x19, 0x01}, 2},
    // Positive integer missing 2 bytes of 4 byte argument
    {(uint8_t[]){0x1a, 0x01, 0x02}, 3},
    // Positive integer missing 1 bytes of 7 byte argument
    {(uint8_t[]){0x1b, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}, 8},
    // Negative integer missing 1 byte argument
    {(uint8_t[]){0x38}, 1},
    // Binary string missing 1 byte argument
    {(uint8_t[]){0x58}, 1},
    // Text string missing 1 byte argument
    {(uint8_t[]){0x78}, 1},
    // Array missing 1 byte argument
    {(uint8_t[]){0x98}, 1},
    // Map missing 1 byte argument
    {(uint8_t[]){0xb8}, 1},
    // Tag missing 1 byte argument
    {(uint8_t[]){0xd8}, 1},
    // Simple missing 1 byte argument
    {(uint8_t[]){0xf8}, 1},
    // Half-precision missing 1 byte
    {(uint8_t[]){0xf9, 0x00}, 2},
    // Float missing 2 bytes
    {(uint8_t[]){0xfa, 0x00, 0x00}, 3},
    // Double missing 5 bytes
    {(uint8_t[]){0xfb, 0x00, 0x00, 0x00}, 4},

    // Breaks must not occur in definite length arrays and maps

    // Array of length 1 with sole member replaced by a break
    {(uint8_t[]){0x81, 0xff}, 2},
    // Array of length 2 with 2nd member replaced by a break
    {(uint8_t[]){0x82, 0x00, 0xff}, 3},
    // Map of length 1 with sole member label replaced by a break
    {(uint8_t[]){0xa1, 0xff}, 2},
    // Map of length 1 with sole member label replaced by break
    // Alternate representation that some decoders handle difference
    {(uint8_t[]){0xa1, 0xff, 0x00}, 3},
    // Array of length 1 with 2nd member value replaced by a break
    {(uint8_t[]){0xa1, 0x00, 0xff}, 3},
    // Map of length 2 with 2nd member replaced by a break
    {(uint8_t[]){0xa2, 0x00, 0x00, 0xff}, 4},


    // Breaks must not occur on their own out of an indefinite length
    // data item

    // A bare break is not well formed
    {(uint8_t[]){0xff}, 1},
    // A bare break after a zero length definite length array
    {(uint8_t[]){0x80, 0xff}, 2},
    // A bare break after a zero length indefinite length map
    {(uint8_t[]){0x9f, 0xff, 0xff}, 3},


    // Forbidden two-byte encodings of simple types

    // Must use 0xe0 instead
    {(uint8_t[]){0xf8, 0x00}, 2},
    // Should use 0xe1 instead
    {(uint8_t[]){0xf8, 0x01}, 2},
    // Should use 0xe2 instead
    {(uint8_t[]){0xf8, 0x02}, 2},
    // Should use 0xe3 instead
    {(uint8_t[]){0xf8, 0x03}, 2},
    // Should use 0xe4 instead
    {(uint8_t[]){0xf8, 0x04}, 2},
    // Should use 0xe5 instead
    {(uint8_t[]){0xf8, 0x05}, 2},
    // Should use 0xe6 instead
    {(uint8_t[]){0xf8, 0x06}, 2},
    // Should use 0xe7 instead
    {(uint8_t[]){0xf8, 0x07}, 2},
    // Should use 0xe8 instead
    {(uint8_t[]){0xf8, 0x08}, 2},
    // Should use 0xe9 instead
    {(uint8_t[]){0xf8, 0x09}, 2},
    // Should use 0xea instead
    {(uint8_t[]){0xf8, 0x0a}, 2},
    // Should use 0xeb instead
    {(uint8_t[]){0xf8, 0x0b}, 2},
    // Should use 0xec instead
    {(uint8_t[]){0xf8, 0x0c}, 2},
    // Should use 0xed instead
    {(uint8_t[]){0xf8, 0x0d}, 2},
    // Should use 0xee instead
    {(uint8_t[]){0xf8, 0x0e}, 2},
    // Should use 0xef instead
    {(uint8_t[]){0xf8, 0x0f}, 2},
    // Should use 0xf0 instead
    {(uint8_t[]){0xf8, 0x10}, 2},
    // Should use 0xf1 instead
    {(uint8_t[]){0xf8, 0x11}, 2},
    // Should use 0xf2 instead
    {(uint8_t[]){0xf8, 0x12}, 2},
    // Must use 0xf3 instead
    {(uint8_t[]){0xf8, 0x13}, 2},
    // Must use 0xf4 instead
    {(uint8_t[]){0xf8, 0x14}, 2},
    // Must use 0xf5 instead
    {(uint8_t[]){0xf8, 0x15}, 2},
    // Must use 0xf6 instead
    {(uint8_t[]){0xf8, 0x16}, 2},
    // Must use 0xf7 instead
    {(uint8_t[]){0xf8, 0x17}, 2},
    // Reserved (as defined in RFC 8126), considered not-well-formed
    {(uint8_t[]){0xf8, 0x18}, 2},
    // Reserved (as defined in RFC 8126), considered not-well-formed
    {(uint8_t[]){0xf8, 0x19}, 2},
    // Reserved (as defined in RFC 8126), considered not-well-formed
    {(uint8_t[]){0xf8, 0x1a}, 2},
    // Reserved (as defined in RFC 8126), considered not-well-formed
    {(uint8_t[]){0xf8, 0x1b}, 2},
    // Reserved (as defined in RFC 8126), considered not-well-formed
    {(uint8_t[]){0xf8, 0x1c}, 2},
    // Reserved (as defined in RFC 8126), considered not-well-formed
    {(uint8_t[]){0xf8, 0x1d}, 2},
    // Reserved (as defined in RFC 8126), considered not-well-formed
    {(uint8_t[]){0xf8, 0x1e}, 2},
    // Reserved (as defined in RFC 8126), considered not-well-formed
    {(uint8_t[]){0xf8, 0x1f}, 2},

    // Integers with "argument" equal to an indefinite length

    // Positive integer with "argument" an indefinite length
    {(uint8_t[]){0x1f}, 1},
    // Negative integer with "argument" an indefinite length
    {(uint8_t[]){0x3f}, 1},
    // CBOR tag with "argument" an indefinite length
    {(uint8_t[]){0xdf, 0x00}, 2},
    // CBOR tag with "argument" an indefinite length alternate vector
    {(uint8_t[]){0xdf}, 1},


    // Missing content bytes from a definite length string

    // A byte string is of length 1 without the 1 byte
    {(uint8_t[]){0x41}, 1},
    // A text string is of length 1 without the 1 byte
    {(uint8_t[]){0x61}, 1},
    // Byte string should have 2^32-15 bytes, but has one
    {(uint8_t[]){0x5a, 0xff, 0xff, 0xff, 0xf0, 0x00}, 6},
    // Byte string should have 2^32-15 bytes, but has one
    {(uint8_t[]){0x7a, 0xff, 0xff, 0xff, 0xf0, 0x00}, 6},


    // Use of unassigned additional information values

    // Major type positive integer with reserved value 28
    {(uint8_t[]){0x1c}, 1},
    // Major type positive integer with reserved value 29
    {(uint8_t[]){0x1d}, 1},
    // Major type positive integer with reserved value 30
    {(uint8_t[]){0x1e}, 1},
    // Major type negative integer with reserved value 28
    {(uint8_t[]){0x3c}, 1},
    // Major type negative integer with reserved value 29
    {(uint8_t[]){0x3d}, 1},
    // Major type negative integer with reserved value 30
    {(uint8_t[]){0x3e}, 1},
    // Major type byte string with reserved value 28 length
    {(uint8_t[]){0x5c}, 1},
    // Major type byte string with reserved value 29 length
    {(uint8_t[]){0x5d}, 1},
    // Major type byte string with reserved value 30 length
    {(uint8_t[]){0x5e}, 1},
    // Major type text string with reserved value 28 length
    {(uint8_t[]){0x7c}, 1},
    // Major type text string with reserved value 29 length
    {(uint8_t[]){0x7d}, 1},
    // Major type text string with reserved value 30 length
    {(uint8_t[]){0x7e}, 1},
    // Major type array with reserved value 28 length
    {(uint8_t[]){0x9c}, 1},
    // Major type array with reserved value 29 length
    {(uint8_t[]){0x9d}, 1},
    // Major type array with reserved value 30 length
    {(uint8_t[]){0x9e}, 1},
    // Major type map with reserved value 28 length
    {(uint8_t[]){0xbc}, 1},
    // Major type map with reserved value 29 length
    {(uint8_t[]){0xbd}, 1},
    // Major type map with reserved value 30 length
    {(uint8_t[]){0xbe}, 1},
    // Major type tag with reserved value 28 length
    {(uint8_t[]){0xdc}, 1},
    // Major type tag with reserved value 29 length
    {(uint8_t[]){0xdd}, 1},
    // Major type tag with reserved value 30 length
    {(uint8_t[]){0xde}, 1},
    // Major type simple with reserved value 28 length
    {(uint8_t[]){0xfc}, 1},
    // Major type simple with reserved value 29 length
    {(uint8_t[]){0xfd}, 1},
    // Major type simple with reserved value 30 length
    {(uint8_t[]){0xfe}, 1},


    // Maps must have an even number of data items (key & value)

    // Map with 1 item when it should have 2
    {(uint8_t[]){0xa1, 0x00}, 2},
    // Map with 3 item when it should have 4
    {(uint8_t[]){0xa2, 0x00, 0x00, 0x00}, 2},
    // Map with 1 item when it should have 2
    {(uint8_t[]){0xbf, 0x00, 0xff}, 3},
    // Map with 3 item when it should have 4
    {(uint8_t[]){0xbf, 0x00, 0x00, 0x00, 0xff}, 5},

};

#endif /* not_well_formed_cbor_h */
