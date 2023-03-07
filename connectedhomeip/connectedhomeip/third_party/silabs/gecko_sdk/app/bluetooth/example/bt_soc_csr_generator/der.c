/***************************************************************************//**
 * @file
 * @brief Certificate signing request structure generation.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "psa/crypto.h"
#include "psa/crypto_values.h"

#include "sl_status.h"
#include "der.h"
#include "app_log.h"

/*
 * This file implements an incomplete DER encoder which provides
 * just enough functionality to construct a CBP certificate signing
 * request on device, using PSA ctypto API only for key generation,
 * SHA-1 calculation, and ECDSA signing.
 *
 * In general, see X.690 for reference on the encoding syntax; and see
 * RFCs 2986, 5280, 5430, and 5758 for particular details on values and
 * encodings to use.
 */

#define X520_DN_MAX_ATTRIBUTES (8) /**< Artificial upper limit to DN components */

#define DER_INTEGER_LEN (4) /**< Internal limit */

enum der_version {
  der_version_1 = 0,
};

enum der_type {
  DER_PRIMITIVE_BOOLEAN = 0x01,
  DER_PRIMITIVE_INTEGER = 0x02,
  DER_PRIMITIVE_BIT_STRING = 0x03,
  DER_PRIMITIVE_OCTET_STRING = 0x04,
  DER_PRIMITIVE_OID = 0x06,
  DER_PRIMITIVE_UTF8_STRING = 0x0c,
  DER_PRIMITIVE_PRINTABLE_STRING = 0x13,
  DER_PRIMITIVE_IA5_STRING = 0x16,
  DER_CONSTRUCT_SEQUENCE = 0x30,
  DER_CONSTRUCT_SET = 0x31,
  DER_CONSTRUCT_CSR_ATTRIBUTES = 0xa0,
  DER_PRIMITIVE_BIG_INTEGER = 0x102, // Same as DER_PRIMITIVE_INTEGER but represented as a byte array
};

typedef int32_t der_int_t;

/*
 * A single DER value that gets encoded as tag-length-contents;
 * for constructed values the contents will be recursively written out
 */
struct der_value {
  enum der_type type;
  union {
    bool boolean;
    der_int_t integer;
    struct {
      size_t len;
      uint8_t *ptr;
    } big_integer;
    const uint8_t *oid;
    struct {
      size_t bit_len;
      uint8_t *ptr;
    } bit_string;
    struct {
      size_t len;
      uint8_t *ptr;
    } octet_string;
    struct {
      size_t len;
      uint8_t *ptr;
    } utf8_string;
    struct {
      size_t len;
      uint8_t *ptr;
    } printable_string;
    struct {
      size_t len;
      uint8_t *ptr;
    } ia5_string;
    struct {
      size_t count;
      struct der_value *ptr;
    } collection;
  } data;
};

struct x520_dn_component {
  const uint8_t *oid;
  struct der_value value;
};

struct x520_dn {
  size_t count;
  struct x520_dn_component *component;
};

/*
 * OIDs are represented merely as strings prefixed with type and length bytes;
 * the string length is given in the 2nd byte for each.
 */

// X.509 ext components are under 2.5.29
#define X509_EXT_OID_PREFIX "\x06\x03\x55\x1d"
#define X509_EXT_OID(o) ((const uint8_t *)(X509_EXT_OID_PREFIX o))
#define X509_EXT_OID_SUBJECT_KEY_IDENTIFIER X509_EXT_OID("\x0e")
#define X509_EXT_OID_KEY_USAGE X509_EXT_OID("\x0f")
#define X509_EXT_OID_BASIC_CONSTRAINT X509_EXT_OID("\x13")

// X.520 DN components are under 2.5.4
#define X520_DN_OID_PREFIX "\x06\x03\x55\x04"
#define X520_DN_OID(o) ((const uint8_t *)(X520_DN_OID_PREFIX o))
#define X520_DN_OID_CN X520_DN_OID("\x03")
#define X520_DN_OID_COUNTRY X520_DN_OID("\x06")
#define X520_DN_OID_LOCALITY X520_DN_OID("\x07")
#define X520_DN_OID_STATE_OR_PROVINCE X520_DN_OID("\x08")
#define X520_DN_OID_ORGANIZATION X520_DN_OID("\x0a")
#define X520_DN_OID_ORGANIZATIONAL_UNIT X520_DN_OID("\x0b")

// PKCS#9 email address is under 1.2.840.113549.1.9
#define PKCS9_OID_PREFIX "\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x09"
#define PKCS9_OID(o) ((const uint8_t *)(PKCS9_OID_PREFIX o))
#define PKCS9_OID_EMAIL_ADDRESS PKCS9_OID("\x01")
#define PKCS9_OID_EXTENSION_REQUEST PKCS9_OID("\x0e")

// ECC algorithm identifiers are under 1.2.840.10045.2
#define ECC_ALG_OID_PREFIX "\x06\x07\x2a\x86\x48\xce\x3d\x02"
#define ECC_ALG_OID(o) ((const uint8_t *)(ECC_ALG_OID_PREFIX o))
#define ECC_ALG_OID_EC_PUBLIC_KEY ECC_ALG_OID("\x01")

// ECC named curves are under 1.2.840.10045.3.1
#define ECC_CURVE_OID_PREFIX "\x06\x08\x2a\x86\x48\xce\x3d\x03\x01"
#define ECC_CURVE_OID(o) ((const uint8_t *)(ECC_CURVE_OID_PREFIX o))
#define ECC_CURVE_OID_PRIME256V1 ECC_CURVE_OID("\x07")

// ECC signatures are under 1.2.840.10045.4.3
#define ECC_SIGNATURE_OID_PREFIX "\x06\x08\x2a\x86\x48\xce\x3d\x04\x03"
#define ECC_SIGNATURE_OID(o) ((const uint8_t *)(ECC_SIGNATURE_OID_PREFIX o))
#define ECC_SIGNATURE_OID_ECDSA_WITH_SHA256 ECC_SIGNATURE_OID("\x02")

// Mesh static authentication data OID is self-allocated UUID under 2.25
#define MESH_OID_STATIC_AUTH_DATA ((const uint8_t *)("\x06\x14\x69\x82\xE1\x9D\xE4\x91\xEA\xC0\xC2\x83\x99\x9C\xAA\x83\xFD\x8C\xC3\xD0\xD3\x67"))

static sl_status_t der_encode_oid(const uint8_t *oid,
                                  uint8_t *buffer,
                                  size_t buffer_len,
                                  size_t *write_len);

static sl_status_t der_encode_string(enum der_type type,
                                     size_t len,
                                     const uint8_t *ptr,
                                     uint8_t *buffer,
                                     size_t buffer_len,
                                     size_t *write_len);

static sl_status_t der_encode_bit_string(enum der_type type,
                                         size_t bit_len,
                                         const uint8_t *ptr,
                                         uint8_t *buffer,
                                         size_t buffer_len,
                                         size_t *write_len);

static sl_status_t der_encode_collection(enum der_type type,
                                         const struct der_value *value,
                                         size_t value_count,
                                         uint8_t *buffer,
                                         size_t buffer_len,
                                         size_t *write_len);

static size_t der_int_length(der_int_t i)
{
  der_int_t a = abs(i);
  uint8_t tmp[DER_INTEGER_LEN];
  size_t p;

  for (p = DER_INTEGER_LEN; p; p--) {
    tmp[p - 1] = a & 0xff;
    a >>= 8;
  }

  for (p = 0; p < DER_INTEGER_LEN - 1; p++) {
    if (tmp[p] != 0x00 || (tmp[p + 1] & 0x80)) {
      return DER_INTEGER_LEN - p;
    }
  }
  return 1;
}

/*
 * All der_encode functions work with and without buffer; without buffer they merely return
 * the length of the would-be-constructed encoding. This is needed so that constructed values
 * can be recursively encoded.
 */
static sl_status_t der_encode_header(enum der_type type,
                                     size_t length,
                                     uint8_t *buffer,
                                     size_t buffer_len,
                                     size_t *write_len)
{
  size_t header_len;

  if (length < 0x80) { // may use short form
    header_len = 2;
  } else if (length < 0x100) { // definite long form with 2 octets
    header_len = 3;
  } else if (length < 0x10000) { // definite long form with 3 octets
    header_len = 4;
  } else { // too long for practical purposes, not supported
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (buffer_len < header_len) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  if (buffer) {
    buffer[0] = type;
    if (length < 0x80) {
      buffer[1] = length;
    } else if (length < 0x100) {
      buffer[1] = 0x81;
      buffer[2] = length & 0xff;
    } else {
      buffer[1] = 0x82;
      buffer[2] = (length >> 8) & 0xff;
      buffer[3] = length & 0xff;
    }
  }

  *write_len = header_len;
  return SL_STATUS_OK;
}

static sl_status_t der_encode(const struct der_value *value,
                              uint8_t *buffer,
                              size_t buffer_len,
                              size_t *write_len)
{
  size_t len;

  switch (value->type) {
    case DER_PRIMITIVE_BOOLEAN:
      if (buffer_len < 3) {
        return SL_STATUS_WOULD_OVERFLOW;
      }
      if (buffer) {
        buffer[0] = DER_PRIMITIVE_BOOLEAN;
        buffer[1] = 1;
        buffer[2] = value->data.boolean;
      }
      *write_len = 3;
      return SL_STATUS_OK;

    case DER_PRIMITIVE_INTEGER:
      len = der_int_length(value->data.integer);
      if (len > DER_INTEGER_LEN) {
        return SL_STATUS_NOT_SUPPORTED;
      }
      if (buffer_len < 2 + len) {
        return SL_STATUS_WOULD_OVERFLOW;
      }
      if (buffer) {
        size_t pos;
        buffer[0] = DER_PRIMITIVE_INTEGER;
        buffer[1] = len;
        for (pos = 0; pos < len; pos++) {
          uint8_t byte = (value->data.integer >> (8 * (DER_INTEGER_LEN - 1 - pos))) & 0xff;
          buffer[2 + pos] = byte;
        }
      }
      *write_len = 2 + len;
      return SL_STATUS_OK;

    case DER_PRIMITIVE_BIG_INTEGER:
      len = value->data.big_integer.len;
      if (value->data.big_integer.ptr[0] > 0x7f) {
        len++;
      }
      if (buffer_len < 2 + len) {
        return SL_STATUS_WOULD_OVERFLOW;
      }
      if (buffer) {
        size_t pos = 0;
        buffer[pos++] = DER_PRIMITIVE_INTEGER;
        buffer[pos++] = len;
        if (value->data.big_integer.ptr[0] > 0x7f) {
          buffer[pos++] = 0x00;
        }
        memcpy(buffer + pos,
               value->data.big_integer.ptr,
               value->data.big_integer.len);
      }
      *write_len = 2 + len;
      return SL_STATUS_OK;

    case DER_PRIMITIVE_OID:
      return der_encode_oid(value->data.oid,
                            buffer,
                            buffer_len,
                            write_len);

    case DER_PRIMITIVE_BIT_STRING:
      return der_encode_bit_string(value->type,
                                   value->data.bit_string.bit_len,
                                   value->data.bit_string.ptr,
                                   buffer,
                                   buffer_len,
                                   write_len);

    case DER_PRIMITIVE_OCTET_STRING:
      return der_encode_string(value->type,
                               value->data.octet_string.len,
                               value->data.octet_string.ptr,
                               buffer,
                               buffer_len,
                               write_len);

    case DER_PRIMITIVE_UTF8_STRING:
      return der_encode_string(value->type,
                               value->data.utf8_string.len,
                               value->data.utf8_string.ptr,
                               buffer,
                               buffer_len,
                               write_len);

    case DER_PRIMITIVE_PRINTABLE_STRING:
      return der_encode_string(value->type,
                               value->data.printable_string.len,
                               value->data.printable_string.ptr,
                               buffer,
                               buffer_len,
                               write_len);

    case DER_PRIMITIVE_IA5_STRING:
      return der_encode_string(value->type,
                               value->data.ia5_string.len,
                               value->data.ia5_string.ptr,
                               buffer,
                               buffer_len,
                               write_len);

    case DER_CONSTRUCT_SEQUENCE:
    case DER_CONSTRUCT_SET:
    case DER_CONSTRUCT_CSR_ATTRIBUTES:
      return der_encode_collection(value->type,
                                   value->data.collection.ptr,
                                   value->data.collection.count,
                                   buffer,
                                   buffer_len,
                                   write_len);

    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

static sl_status_t der_encode_oid(const uint8_t *oid,
                                  uint8_t *buffer,
                                  size_t buffer_len,
                                  size_t *write_len)
{
  // This is ugly; but we know that it works as all OIDs we use are
  // encoded using a single octet length
  size_t len = 2 + oid[1];

  if (buffer_len < len) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  if (buffer) {
    memcpy(buffer, oid, len);
  }

  *write_len = len;
  return SL_STATUS_OK;
}

static sl_status_t der_encode_bit_string(enum der_type type,
                                         size_t bit_len,
                                         const uint8_t *ptr,
                                         uint8_t *buffer,
                                         size_t buffer_len,
                                         size_t *write_len)
{
  size_t header_len, len;
  sl_status_t e;

  len = (bit_len + 7) / 8;

  e = der_encode_header(type, 1 + len, NULL, buffer_len, &header_len);
  if (e != SL_STATUS_OK) {
    return e;
  }

  if (buffer_len < header_len + 1 + len) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  if (buffer) {
    e = der_encode_header(type, 1 + len, buffer, buffer_len, &header_len);
    if (e != SL_STATUS_OK) {
      return e;
    }
    buffer[header_len] = bit_len % 8;
    memcpy(buffer + header_len + 1, ptr, len);
  }

  *write_len = header_len + 1 + len;
  return SL_STATUS_OK;
}

static sl_status_t der_encode_string(enum der_type type,
                                     size_t len,
                                     const uint8_t *ptr,
                                     uint8_t *buffer,
                                     size_t buffer_len,
                                     size_t *write_len)
{
  size_t header_len;
  sl_status_t e;

  e = der_encode_header(type, len, NULL, buffer_len, &header_len);
  if (e != SL_STATUS_OK) {
    return e;
  }

  if (buffer_len < header_len + len) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  if (buffer) {
    e = der_encode_header(type, len, buffer, buffer_len, &header_len);
    if (e != SL_STATUS_OK) {
      return e;
    }

    memcpy(buffer + header_len, ptr, len);
  }

  *write_len = header_len + len;
  return SL_STATUS_OK;
}

static sl_status_t der_encode_collection(enum der_type type,
                                         const struct der_value *value,
                                         size_t value_count,
                                         uint8_t *buffer,
                                         size_t buffer_len,
                                         size_t *write_len)
{
  size_t write_pos, header_len, i;
  sl_status_t e;

  // First, figure out the length needed for encoding the contents
  for (write_pos = 0, i = 0; i < value_count; i++) {
    size_t chunk;
    e = der_encode(&value[i], NULL, ~0, &chunk);
    if (e != SL_STATUS_OK) {
      return e;
    }
    write_pos += chunk;
  }

  // Check header encoding as well
  e = der_encode_header(type, write_pos, NULL, buffer_len, &header_len);
  if (e != SL_STATUS_OK) {
    return e;
  }

  if (buffer_len < header_len + write_pos) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  if (buffer) {
    e = der_encode_header(type, write_pos, buffer, buffer_len, &header_len);
    if (e != SL_STATUS_OK) {
      return e;
    }

    for (write_pos = 0, i = 0; i < value_count; i++) {
      size_t chunk;
      e = der_encode(&value[i], buffer + header_len + write_pos, buffer_len - header_len - write_pos, &chunk);
      if (e != SL_STATUS_OK) {
        return e;
      }
      write_pos += chunk;
    }
  }

  *write_len = header_len + write_pos;
  return SL_STATUS_OK;
}

static sl_status_t der_construct_and_sign(enum der_version ver,
                                          const struct x520_dn *dn,
                                          mbedtls_svc_key_id_t device_key,
                                          uint8_t *public_key_ptr,
                                          size_t public_key_bit_len,
                                          uint8_t *subject_key_identifier_ptr,
                                          size_t subject_key_identifier_len,
                                          uint8_t *static_auth_ptr,
                                          size_t static_auth_len,
                                          uint8_t *buffer,
                                          size_t buffer_len,
                                          size_t *write_len)
{
  uint8_t signature_buffer[64] = { 0 };
  size_t signature_len = 0;
  uint8_t sig_buf[72] = { 0 };
  size_t sig_len = 0;
  psa_algorithm_t signature_alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
  uint8_t tbs[512] = { 0 };
  size_t tbs_len = 0;
  uint8_t ski_buf[2 + CRYPTO_SHA_1_LEN] = { 0 };
  size_t ski_len = 0;
  uint8_t auth_buf[2 + CRYPTO_AUTH_256_LEN] = { 0 };
  size_t auth_len = 0;
  struct der_value attribute[X520_DN_MAX_ATTRIBUTES];
  struct der_value attribute_pair[X520_DN_MAX_ATTRIBUTES][2];
  struct der_value set[X520_DN_MAX_ATTRIBUTES];
  struct der_value dn_sequence;
  struct der_value info_sequence[4];
  sl_status_t e;
  size_t s = 0;
  size_t i;

  memset(set, 0, sizeof(set));
  memset(attribute, 0, sizeof(attribute));
  memset(attribute_pair, 0, sizeof(attribute_pair));

  if (public_key_bit_len != 520) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (subject_key_identifier_len != CRYPTO_SHA_1_LEN) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (static_auth_ptr) {
    if (static_auth_len != CRYPTO_AUTH_256_LEN) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  if (dn->count > X520_DN_MAX_ATTRIBUTES) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // 1)
  // version is just an integer

  struct der_value version = {
    .type = DER_PRIMITIVE_INTEGER,
    .data.integer = ver,
  };

  info_sequence[s++] = version;

  // 2)
  // Name in RFC 5280 is a SEQUENCE of RDNs; each RDN is a set; each
  // set is a sequence of type and value

  // First create an attribute for each DN component and fill
  // the set with the attributes. Note: does not check that
  // there are no duplicate entries
  for (i = 0; i < dn->count; i++) {
    attribute_pair[i][0].type = DER_PRIMITIVE_OID;
    attribute_pair[i][0].data.oid = dn->component[i].oid;
    attribute_pair[i][1].type = dn->component[i].value.type;
    attribute_pair[i][1].data = dn->component[i].value.data;
    attribute[i].type = DER_CONSTRUCT_SEQUENCE;
    attribute[i].data.collection.count = 2;
    attribute[i].data.collection.ptr = attribute_pair[i];
    set[i].type = DER_CONSTRUCT_SET;
    set[i].data.collection.count = 1;
    set[i].data.collection.ptr = &attribute[i];
  }

  // Then create the overall sequence
  dn_sequence.type = DER_CONSTRUCT_SEQUENCE;
  dn_sequence.data.collection.count = dn->count;
  dn_sequence.data.collection.ptr = set;

  info_sequence[s++] = dn_sequence;

  // 3)
  // Add the public key; ECC named curve and bit string

  struct der_value algorithm_contents[2] = {
    {
      .type = DER_PRIMITIVE_OID,
      .data = {
        .oid = ECC_ALG_OID_EC_PUBLIC_KEY
      }
    }, {
      .type = DER_PRIMITIVE_OID,
      .data = {
        .oid = ECC_CURVE_OID_PRIME256V1
      }
    }
  };

  struct der_value spki_contents[2] = {
    {
      .type = DER_CONSTRUCT_SEQUENCE,
      .data = {
        .collection = {
          .count = 2,
          .ptr = algorithm_contents
        }
      }
    }, {
      .type = DER_PRIMITIVE_BIT_STRING,
      .data = {
        .bit_string = {
          .bit_len = public_key_bit_len,
          .ptr = public_key_ptr,
        }
      }
    }
  };

  struct der_value spki_sequence = {
    .type = DER_CONSTRUCT_SEQUENCE,
    .data = {
      .collection = {
        .count = 2,
        .ptr = spki_contents
      }
    }
  };

  info_sequence[s++] = spki_sequence;

  // 4) Add requested extensions

  // The level of complication here is just lovely. Each extension
  // is a pairing of OID and an octet string, which contains the
  // DER-encoded object instead of the plain object. Even if the
  // value would be an octet string, it still needs to be encoded
  // into a wrapping octet string ...

  // 4.1) Add CA=FALSE basic constraint
  // CA=FALSE is the default so the parameter sequence can be empty

  uint8_t empty_sequence[2] = { 0x30, 0x00 };

  struct der_value ext_basic_constraints_contents[2] = {
    {
      .type = DER_PRIMITIVE_OID,
      .data = {
        .oid = X509_EXT_OID_BASIC_CONSTRAINT,
      }
    }, {
      .type = DER_PRIMITIVE_OCTET_STRING,
      .data = {
        .octet_string = {
          .len = sizeof(empty_sequence),
          .ptr = empty_sequence,
        }
      }
    }
  };

  // 4.2) Add key agreement key usage

  uint8_t key_agreement_bit_string[4] = { 0x03, 0x02, 0x03, 0x08 };

  struct der_value ext_key_usage_contents[2] = {
    {
      .type = DER_PRIMITIVE_OID,
      .data = {
        .oid = X509_EXT_OID_KEY_USAGE,
      }
    }, {
      .type = DER_PRIMITIVE_OCTET_STRING,
      .data = {
        .octet_string = {
          .len = sizeof(key_agreement_bit_string),
          .ptr = key_agreement_bit_string,
        }
      }
    }
  };

  // 4.3) Add subject key identifier (?)

  struct der_value subject_key_identifier = {
    .type = DER_PRIMITIVE_OCTET_STRING,
    .data = {
      .octet_string = {
        .len = subject_key_identifier_len,
        .ptr = subject_key_identifier_ptr,
      }
    }
  };

  e = der_encode(&subject_key_identifier, ski_buf, sizeof(ski_buf), &ski_len);
  if (e != SL_STATUS_OK) {
    return e;
  }

  struct der_value ext_subject_key_identifier_contents[2] = {
    {
      .type = DER_PRIMITIVE_OID,
      .data = {
        .oid = X509_EXT_OID_SUBJECT_KEY_IDENTIFIER,
      }
    }, {
      .type = DER_PRIMITIVE_OCTET_STRING,
      .data = {
        .octet_string = {
          .len = ski_len,
          .ptr = ski_buf,
        }
      }
    }
  };

  // 4.4) Add static auth data if any

  struct der_value static_auth_data = {
    .type = DER_PRIMITIVE_OCTET_STRING,
    .data = {
      .octet_string = {
        .len = static_auth_len,
        .ptr = static_auth_ptr,
      }
    }
  };

  e = der_encode(&static_auth_data, auth_buf, sizeof(auth_buf), &auth_len);
  if (e != SL_STATUS_OK) {
    return e;
  }

  struct der_value ext_static_auth_data_contents[2] = {
    {
      .type = DER_PRIMITIVE_OID,
      .data = {
        .oid = MESH_OID_STATIC_AUTH_DATA,
      }
    }, {
      .type = DER_PRIMITIVE_OCTET_STRING,
      .data = {
        .octet_string = {
          .len = auth_len,
          .ptr = auth_buf,
        }
      }
    }
  };

  struct der_value ext_sequence_contents[4] = {
    {
      .type = DER_CONSTRUCT_SEQUENCE,
      .data = {
        .collection = {
          .count = 2,
          .ptr = ext_basic_constraints_contents,
        }
      }
    }, {
      .type = DER_CONSTRUCT_SEQUENCE,
      .data = {
        .collection = {
          .count = 2,
          .ptr = ext_key_usage_contents,
        }
      }
    }, {
      .type = DER_CONSTRUCT_SEQUENCE,
      .data = {
        .collection = {
          .count = 2,
          .ptr = ext_subject_key_identifier_contents,
        }
      }
    }, {
      .type = DER_CONSTRUCT_SEQUENCE,
      .data = {
        .collection = {
          .count = 2,
          .ptr = ext_static_auth_data_contents,
        }
      }
    }
  };

  struct der_value ext_sequence = {
    .type = DER_CONSTRUCT_SEQUENCE,
    .data = {
      .collection = {
        .count = static_auth_ptr ? 4 : 3,
        .ptr = ext_sequence_contents
      }
    }
  };

  struct der_value ext_contents[2] = {
    {
      .type = DER_PRIMITIVE_OID,
      .data = {
        .oid = PKCS9_OID_EXTENSION_REQUEST,
      }
    }, {
      .type = DER_CONSTRUCT_SET,
      .data = {
        .collection = {
          .count = 1,
          .ptr = &ext_sequence,
        }
      }
    }
  };

  struct der_value ext = {
    .type = DER_CONSTRUCT_SEQUENCE,
    .data = {
      .collection = {
        .count = 2,
        .ptr = ext_contents,
      }
    }
  };

  struct der_value attributes = {
    .type = DER_CONSTRUCT_CSR_ATTRIBUTES,
    .data = {
      .collection = {
        .count = 1,
        .ptr = &ext,
      }
    }
  };

  info_sequence[s++] = attributes;

  // 5) Encode request information into temporary buffer

  struct der_value info_envelope = {
    .type = DER_CONSTRUCT_SEQUENCE,
    .data = {
      .collection = {
        .count = s,
        .ptr = info_sequence,
      }
    }
  };

  e = der_encode(&info_envelope, tbs, sizeof(tbs), &tbs_len);
  if (e != SL_STATUS_OK) {
    app_log("Failed to DER-encode certificate request information\n");
    return e;
  }

  // 6) Self-sign the request information
  e = psa_status_to_sl_status(psa_sign_message(device_key,
                                               signature_alg,
                                               tbs,
                                               tbs_len,
                                               signature_buffer,
                                               sizeof(signature_buffer),
                                               &signature_len));
  if (e != SL_STATUS_OK) {
    app_log("Failed to sign certificate request information\n");
    return e;
  }

  // 7) Construct DER sequence for the signature

  // No parameters for the signature algorithm

  struct der_value signature_algorithm = {
    .type = DER_PRIMITIVE_OID,
    .data = {
      .oid = ECC_SIGNATURE_OID_ECDSA_WITH_SHA256,
    }
  };

  struct der_value signature_algorithm_envelope = {
    .type = DER_CONSTRUCT_SEQUENCE,
    .data = {
      .collection = {
        .count = 1,
        .ptr = &signature_algorithm,
      }
    }
  };

  // Signature itself is represented as a pair of numbers (r,s)
  // which will yet again be encoded into a wrapping construct,
  // this time bit string

  struct der_value signature_pair[2] = {
    {
      .type = DER_PRIMITIVE_BIG_INTEGER,
      .data = {
        .big_integer = {
          .len = signature_len / 2,
          .ptr = signature_buffer,
        }
      }
    }, {
      .type = DER_PRIMITIVE_BIG_INTEGER,
      .data = {
        .big_integer = {
          .len = signature_len / 2,
          .ptr = signature_buffer + signature_len / 2,
        }
      }
    }
  };

  struct der_value signature_envelope = {
    .type = DER_CONSTRUCT_SEQUENCE,
    .data = {
      .collection = {
        .count = 2,
        .ptr = signature_pair,
      }
    }
  };

  e = der_encode(&signature_envelope, sig_buf, sizeof(sig_buf), &sig_len);
  if (e != SL_STATUS_OK) {
    return e;
  }

  struct der_value signature_bitstring = {
    .type = DER_PRIMITIVE_BIT_STRING,
    .data = {
      .bit_string = {
        .bit_len = 8 * sig_len,
        .ptr = sig_buf,
      }
    }
  };

  // 8) Construct the final request

  struct der_value csr_sequence[3] = {
    info_envelope,
    signature_algorithm_envelope,
    signature_bitstring
  };

  struct der_value csr = {
    .type = DER_CONSTRUCT_SEQUENCE,
    .data = {
      .collection = {
        .count = 3,
        .ptr = csr_sequence,
      }
    }
  };

  e = der_encode(&csr, buffer, buffer_len, write_len);
  if (e != SL_STATUS_OK) {
    app_log("Failed to construct certificate request\n");
    return e;
  }

  return SL_STATUS_OK;
}

sl_status_t der_encode_csr(const struct subject_name_field *subject_name_ptr,
                           size_t subject_name_len,
                           mbedtls_svc_key_id_t device_key,
                           bool use_static_auth,
                           uint8_t *buffer,
                           size_t buffer_len,
                           size_t *write_len)
{
  uint8_t public_key_buffer[1 + CRYPTO_EC_PUBLIC_KEY_LEN] = { 0 };
  uint8_t public_key_digest[CRYPTO_SHA_1_LEN] = { 0 };
  uint8_t static_auth_buffer[CRYPTO_AUTH_256_LEN] = { 0 };
  struct x520_dn_component dn_component[X520_DN_MAX_ATTRIBUTES] = { 0 };
  struct x520_dn dn = {
    subject_name_len,
    dn_component,
  };
  size_t public_key_len = 0, static_auth_len = 0, i;
  sl_status_t e = SL_STATUS_OK;

  memset(dn_component, 0, sizeof(dn_component));

  // Prepare for encoding Certificate Request Info:
  // * Construct subject name ASN.1 structures
  // * Export public key data into octet array
  // * Construct subject public key identifier
  // * Export static auth data, if any

  if (subject_name_len > X520_DN_MAX_ATTRIBUTES) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  // Limited support for DN components; add more as necessary
  for (i = 0; i < subject_name_len; i++) {
    if (subject_name_ptr[i].name_len == 1 && strncmp(subject_name_ptr[i].name, "C", 1) == 0) {
      dn_component[i].oid = X520_DN_OID_COUNTRY;
    } else if (subject_name_ptr[i].name_len == 2 && strncmp(subject_name_ptr[i].name, "ST", 2) == 0) {
      dn_component[i].oid = X520_DN_OID_STATE_OR_PROVINCE;
    } else if (subject_name_ptr[i].name_len == 1 && strncmp(subject_name_ptr[i].name, "L", 1) == 0) {
      dn_component[i].oid = X520_DN_OID_LOCALITY;
    } else if (subject_name_ptr[i].name_len == 1 && strncmp(subject_name_ptr[i].name, "O", 1) == 0) {
      dn_component[i].oid = X520_DN_OID_ORGANIZATION;
    } else if (subject_name_ptr[i].name_len == 2 && strncmp(subject_name_ptr[i].name, "OU", 2) == 0) {
      dn_component[i].oid = X520_DN_OID_ORGANIZATIONAL_UNIT;
    } else if (subject_name_ptr[i].name_len == 2 && strncmp(subject_name_ptr[i].name, "CN", 2) == 0) {
      dn_component[i].oid = X520_DN_OID_CN;
    } else {
      return SL_STATUS_NOT_SUPPORTED;
    }
    dn_component[i].value.type = DER_PRIMITIVE_UTF8_STRING;
    dn_component[i].value.data.utf8_string.len = subject_name_ptr[i].value_len;
    dn_component[i].value.data.utf8_string.ptr = (uint8_t *)subject_name_ptr[i].value;
  }

  e = export_public_key(device_key, public_key_buffer, sizeof(public_key_buffer), &public_key_len);
  if (e != SL_STATUS_OK) {
    app_log("Failed to export public key\n");
    return e;
  }

  // Skip 0x04 header byte, calculate digest only the the key data
  e = calculate_sha_1(public_key_buffer + 1, CRYPTO_EC_PUBLIC_KEY_LEN, public_key_digest);
  if (e != SL_STATUS_OK) {
    app_log("Failed to calculate subject key identifier\n");
    return e;
  }

  if (use_static_auth) {
    e = export_static_auth_data(static_auth_buffer, sizeof(static_auth_buffer), &static_auth_len);
    if (e != SL_STATUS_OK) {
      app_log("Failed to export static auth data\n");
      return e;
    }
  }

  e = der_construct_and_sign(der_version_1,
                             &dn,
                             device_key,
                             public_key_buffer,
                             8 * public_key_len,
                             public_key_digest,
                             sizeof(public_key_digest),
                             use_static_auth ? static_auth_buffer : NULL,
                             use_static_auth ? static_auth_len : 0,
                             buffer,
                             buffer_len,
                             write_len);
  if (e != SL_STATUS_OK) {
    app_log("Failed to construct DER\n");
    return e;
  }

  return SL_STATUS_OK;
}
