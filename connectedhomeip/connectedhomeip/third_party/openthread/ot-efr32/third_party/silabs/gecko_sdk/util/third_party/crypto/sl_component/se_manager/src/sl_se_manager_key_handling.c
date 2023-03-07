/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager key handling.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include <string.h>

#include "sl_se_manager.h"
#include "sli_se_manager_internal.h"
#include "sl_se_manager_key_handling.h"

/***************************************************************************//**
 * \addtogroup sl_se_key SE key handling API
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Defines

#define KEYSPEC_TYPE_MASK                   0xf0000000
#define KEYSPEC_TYPE_OFFSET                 28
#define KEYSPEC_TYPE_RAW                    (uint32_t)(0x0UL << KEYSPEC_TYPE_OFFSET)

#define KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME  (uint32_t)(0x8UL << KEYSPEC_TYPE_OFFSET)

#define KEYSPEC_MODE_MASK                   0x0c000000
#define KEYSPEC_MODE_OFFSET                 26
#define KEYSPEC_MODE_UNPROTECTED            (0UL << KEYSPEC_MODE_OFFSET)
#define KEYSPEC_MODE_VOLATILE               (1UL << KEYSPEC_MODE_OFFSET)

#define KEYSPEC_RESTRICTION_MASK            0x03000000U
#define KEYSPEC_RESTRICTION_OFFSET          24U

#define KEYSPEC_RESTRICTION_UNLOCKED        (0U << KEYSPEC_RESTRICTION_OFFSET)

#define KEYSPEC_INDEX_MASK                  0x00ff0000U
#define KEYSPEC_INDEX_OFFSET                16U

#define KEYSPEC_TRANSFER_MODE_MASK          0x00000300U
#define KEYSPEC_TRANSFER_MODE_OFFSET        8U
#define KEYSPEC_TRANSFER_INDEX_MASK         0x000000ffU
#define KEYSPEC_TRANSFER_INDEX_OFFSET       0U
#define KEYSPEC_TRANSFER_PROT_BIT_MASK      0xffe00000U
#define KEYSPEC_TRANSFER_PROT_BIT_OFFSET    21U

#define KEYSPEC_NOPROT_MASK                 0x00008000U
#define KEYSPEC_NOPROT_OFFSET               15U

#define KEYSPEC_ATTRIBUTES_MASK             0x00007fffU
#define KEYSPEC_ATTRIBUTES_OFFSET           0U

#define KEYSPEC_ATTRIBUTES_ECC_PRIVATE_MASK (1U << 14)
#define KEYSPEC_ATTRIBUTES_ECC_PUBLIC_MASK  (1U << 13)
#define KEYSPEC_ATTRIBUTES_ECC_DOMAIN       (1U << 12)
#define KEYSPEC_ATTRIBUTES_ECC_SIGN         (1U << 10)
#define KEYSPEC_ATTRIBUTES_ECC_SIZE_MASK    0x0000007fU

#define KEYSPEC_TYPE_ECC_EDWARDS    ((uint32_t)(0xaUL << KEYSPEC_TYPE_OFFSET))
#define KEYSPEC_TYPE_ECC_MONTGOMERY ((uint32_t)(0xbUL << KEYSPEC_TYPE_OFFSET))
#define KEYSPEC_TYPE_ECC_EDDSA      ((uint32_t)(0xcUL << KEYSPEC_TYPE_OFFSET))

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

  #define KEYSPEC_MODE_WRAPPED \
  (2UL << KEYSPEC_MODE_OFFSET)

  #define KEYSPEC_ECC_WEIERSTRASS_PRIME_A_IS_ZERO \
  (1U << 9)
  #define KEYSPEC_ECC_WEIERSTRASS_PRIME_A_IS_MINUS_THREE \
  (1U << 8)

#endif

#define KEYSPEC_RESTRICTION_LOCKED          (1UL << KEYSPEC_RESTRICTION_OFFSET)
#define KEYSPEC_RESTRICTION_INTERNAL        (2UL << KEYSPEC_RESTRICTION_OFFSET)
#define KEYSPEC_RESTRICTION_RESTRICTED      (3UL << KEYSPEC_RESTRICTION_OFFSET)

// -----------------------------------------------------------------------------
// Local Functions

sl_status_t sli_key_get_storage_size(const sl_se_key_descriptor_t* key,
                                     uint32_t *storage_size)
{
  if (key == NULL || storage_size == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint32_t key_size = 0;
  sl_status_t status = sli_key_get_size(key, &key_size);
  if (status != SL_STATUS_OK) {
    return status;
  }

  uint32_t key_type = (key->type & KEYSPEC_TYPE_MASK);

  #if defined(SLI_SE_KEY_PADDING_REQUIRED)
  // Round up to word length
  key_size = (key_size + 3U) & ~0x03U;
  #endif

  if (key_type == KEYSPEC_TYPE_RAW) {
    *storage_size = key_size;
    return SL_STATUS_OK;
  }

  bool has_private_key =
    ((key->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY) != 0);
  bool has_public_key =
    ((key->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY) != 0);

  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  bool has_custom_curve =
    ((key->flags & SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN) != 0);
  #else
  bool has_custom_curve = false;
  #endif

  if ((key_type == KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME)) {
    *storage_size = key_size * (1 * has_private_key + 2 * has_public_key + 6 * has_custom_curve);
  } else if (key_type == KEYSPEC_TYPE_ECC_EDWARDS) {
    *storage_size = key_size * (has_private_key + has_public_key + 5 * has_custom_curve);
  } else if ((key_type == KEYSPEC_TYPE_ECC_MONTGOMERY)
             || (key_type == KEYSPEC_TYPE_ECC_EDDSA)) {
    *storage_size = key_size * (has_private_key + has_public_key);
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

#if defined(SLI_SE_KEY_PADDING_REQUIRED)
/***************************************************************************//**
 * @brief
 *   Clear the additional bytes of a key that is not word-aligned.
 *   Does nothing for aligned or asymmetric keys, or non-plaintext keys.
 *
 * @param key
 *   Key descriptor for the key.
 *
 * @returns
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
static sl_status_t clear_padding(const sl_se_key_descriptor_t *key)
{
  if (key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT
      && (key->type & KEYSPEC_TYPE_MASK) == KEYSPEC_TYPE_RAW) {
    uint32_t key_size = 0;
    if (sli_key_get_size(key, &key_size) != SL_STATUS_OK) {
      return SL_STATUS_INVALID_PARAMETER;
    }
    if (key_size & 0x03) {
      if (key_size > key->storage.location.buffer.size) {
        return SL_STATUS_WOULD_OVERFLOW;
      }
      uint8_t *padding_ptr = key->storage.location.buffer.pointer + key_size;
      uint32_t padding_size = (key->storage.location.buffer.size - key_size) & 0x3;
      memset(padding_ptr, 0, padding_size);
    }
  }
  return SL_STATUS_OK;
}
#endif

// -----------------------------------------------------------------------------
// Global Functions

sl_status_t sli_key_get_size(const sl_se_key_descriptor_t *key, uint32_t *size)
{
  if (key == NULL || size == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Find size from key_type
  uint32_t key_type = (key->type & KEYSPEC_TYPE_MASK);
  if (key_type == KEYSPEC_TYPE_RAW) {
    *size = (key->type & KEYSPEC_ATTRIBUTES_MASK);
  } else if ((key_type == KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME)
             || (key_type == KEYSPEC_TYPE_ECC_EDWARDS)
             || (key_type == KEYSPEC_TYPE_ECC_MONTGOMERY)
             || (key_type == KEYSPEC_TYPE_ECC_EDDSA)) {
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    bool has_custom_curve =
      ((key->flags & SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN) != 0);
#endif

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    if (has_custom_curve) {
      if (key->domain == NULL) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      if (key->type & SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
        sl_se_custom_weierstrass_prime_domain_t *domain = (sl_se_custom_weierstrass_prime_domain_t*)key->domain;
        *size = domain->size;
      } else {
        return SL_STATUS_NOT_SUPPORTED;
      }
    } else
#endif
    {
      *size = (key->type & KEYSPEC_ATTRIBUTES_ECC_SIZE_MASK);
    }
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

// If the key type didn't contain a size, use the one in the dedicated size
// field
  if (*size == 0) {
    if (key->size == 0) {
      return SL_STATUS_INVALID_PARAMETER;
    }
    *size = key->size;
  }

  return SL_STATUS_OK;
}

sl_status_t sli_key_check_equivalent(const sl_se_key_descriptor_t *key_1,
                                     const sl_se_key_descriptor_t *key_2,
                                     bool check_key_flag,
                                     bool public_export)
{
  sl_status_t status;

  if (key_1 == NULL || key_2 == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (key_1->type != key_2->type) {
    // The type fields are different, but this may be due to one of the keys
    // not containing the size in the type, but rather in the size field.
    if (((key_1->type & SL_SE_KEY_TYPE_ALGORITHM_MASK)
         == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM)
        || ((key_1->type & SL_SE_KEY_TYPE_ALGORITHM_MASK)
            == SL_SE_KEY_TYPE_ECC_EDDSA)) {
      if ((key_1->type & SL_SE_KEY_TYPE_ALGORITHM_MASK)
          == (key_2->type & SL_SE_KEY_TYPE_ALGORITHM_MASK)) {
        // Assume that the sizes are equal for now (this will be checked later)
        uint32_t key_size = 0;
        if (sli_key_get_size(key_1, &key_size) == SL_STATUS_OK) {
          // Add the key sizes to both key types, and check for equality then
          sl_se_key_type_t type_1 = key_1->type | (SL_SE_KEY_TYPE_ATTRIBUTES_MASK & key_size);
          sl_se_key_type_t type_2 = key_2->type | (SL_SE_KEY_TYPE_ATTRIBUTES_MASK & key_size);
          if (type_1 != type_2) {
            return SL_STATUS_INVALID_PARAMETER;
          }
        } else {
          return SL_STATUS_INVALID_PARAMETER;
        }
      }
    } else {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Verify asymmetry flags
  if ((key_1->type & SL_SE_KEY_TYPE_ALGORITHM_MASK)
      >= KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME) {
    uint32_t consistent_flags =
      (public_export) ? SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN
      : (SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
         | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
         | SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN);
    if (check_key_flag && (key_1->flags & consistent_flags) ^ (key_2->flags & consistent_flags)) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    if (public_export
        && (!(key_2->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY))) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Verify key sizes
  uint32_t key_1_size = 0;
  status = sli_key_get_size(key_1, &key_1_size);
  if (status != SL_STATUS_OK) {
    return status;
  }
  uint32_t key_2_size = 0;
  status = sli_key_get_size(key_2, &key_2_size);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if (key_1_size != key_2_size) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Convert key descriptor struct to a 32-bit keyspec value to pass into an SE
 *   command.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sli_se_key_to_keyspec(const sl_se_key_descriptor_t *key,
                                  uint32_t *keyspec)
{
  if (key == NULL || keyspec == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Clear output
  *keyspec = 0;

  // ---------------------
  // Key type [31-28]

  *keyspec = (key->type & KEYSPEC_TYPE_MASK);

  // ---------------------
  // Key mode [27-26]

  switch (key->storage.method) {
    case SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT:
      *keyspec |= KEYSPEC_MODE_UNPROTECTED;
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED:
      *keyspec |= KEYSPEC_MODE_WRAPPED;
      break;

    case SL_SE_KEY_STORAGE_INTERNAL_VOLATILE:
      *keyspec |= KEYSPEC_MODE_VOLATILE;
      break;
#endif

    case SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE:
      *keyspec |= KEYSPEC_MODE_VOLATILE;
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  // ---------------------
  // Key restriction [25-24]

  uint32_t keyspec_restriction = 0;
  if (key->flags & SL_SE_KEY_FLAG_NON_EXPORTABLE) {
    keyspec_restriction = KEYSPEC_RESTRICTION_LOCKED;
  }
  if (key->flags & SL_SE_KEY_FLAG_IS_DEVICE_GENERATED) {
    keyspec_restriction = KEYSPEC_RESTRICTION_INTERNAL;
  }
  if ((key->flags & SL_SE_KEY_FLAG_IS_RESTRICTED) == SL_SE_KEY_FLAG_IS_RESTRICTED) {
    keyspec_restriction = KEYSPEC_RESTRICTION_RESTRICTED;
  }
  // Key restrictions are only applicable to volatile and wrapped keys
  if (key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT) {
    if (keyspec_restriction != 0) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  *keyspec = (*keyspec & ~KEYSPEC_RESTRICTION_MASK)
             | (keyspec_restriction & KEYSPEC_RESTRICTION_MASK);

  // ---------------------
  // Key index [23-16]

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  if (key->storage.method == SL_SE_KEY_STORAGE_INTERNAL_VOLATILE) {
    if (key->storage.location.slot > SL_SE_KEY_SLOT_VOLATILE_3) {
      return SL_STATUS_INVALID_PARAMETER;
    }
    *keyspec = (*keyspec & ~KEYSPEC_INDEX_MASK)
               | ((key->storage.location.slot << KEYSPEC_INDEX_OFFSET)
                  & KEYSPEC_INDEX_MASK);
  }
#endif

  if (key->storage.method == SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE) {
    if (key->storage.location.slot
        < SL_SE_KEY_SLOT_INTERNAL_MIN) {
      return SL_STATUS_INVALID_PARAMETER;
    }
    *keyspec = (*keyspec & ~KEYSPEC_INDEX_MASK)
               | ((key->storage.location.slot << KEYSPEC_INDEX_OFFSET)
                  & KEYSPEC_INDEX_MASK);
  }

  // ---------------------
  // Key NoProt [15]

  if (key->flags & SL_SE_KEY_FLAG_ALLOW_ANY_ACCESS) {
    *keyspec = (*keyspec & ~KEYSPEC_NOPROT_MASK) | (1 << KEYSPEC_NOPROT_OFFSET);
  }

  // ---------------------
  // Key attributes [14-0]

  // Set general properties from key->type
  if (key->storage.method != SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE) {
    *keyspec = (*keyspec & ~KEYSPEC_ATTRIBUTES_MASK)
               | (key->type & KEYSPEC_ATTRIBUTES_MASK);
  }

  // Set public/private flags
  bool has_private_key =
    ((key->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY) != 0);
  bool has_public_key =
    ((key->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY) != 0);
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  bool has_custom_curve =
    ((key->flags & SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN) != 0);
#endif
  bool signing_only =
    ((key->flags & SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY) != 0);

  if ((key->type & KEYSPEC_TYPE_MASK) >= KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME) {
    if (has_private_key) {
      *keyspec |= KEYSPEC_ATTRIBUTES_ECC_PRIVATE_MASK;
    }
    if (has_public_key) {
      *keyspec |= KEYSPEC_ATTRIBUTES_ECC_PUBLIC_MASK;
    }
  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    if (has_custom_curve) {
      *keyspec |= KEYSPEC_ATTRIBUTES_ECC_DOMAIN;
    }
  #endif
    if (!(has_private_key || has_public_key)) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  if ((key->type & KEYSPEC_TYPE_MASK) >= KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME
      && (key->type & KEYSPEC_TYPE_MASK) < KEYSPEC_TYPE_ECC_EDDSA) {
    if (signing_only) {
      *keyspec |= KEYSPEC_ATTRIBUTES_ECC_SIGN;
    }
  }

  // Ensure that symmetric keys don't have asymmetric flags
  if ((key->type & KEYSPEC_TYPE_MASK) < KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME) {
    if (has_private_key || has_public_key || signing_only) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Set or adjust the key attributes for different key types
  uint32_t size = 0;
  sl_status_t status;
  status = sli_key_get_size(key, &size);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Symmetric and raw keys
  uint32_t key_type = (key->type & KEYSPEC_TYPE_MASK);
  if (key_type == KEYSPEC_TYPE_RAW) {
    *keyspec = (*keyspec & ~KEYSPEC_ATTRIBUTES_MASK)
               | (size & KEYSPEC_ATTRIBUTES_MASK);
  } else if ((key_type == KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME)
             || (key_type == KEYSPEC_TYPE_ECC_EDWARDS)
             || (key_type == KEYSPEC_TYPE_ECC_MONTGOMERY)
             || (key_type == KEYSPEC_TYPE_ECC_EDDSA)) {
    *keyspec = (*keyspec & ~KEYSPEC_ATTRIBUTES_ECC_SIZE_MASK)
               | ((size - 1) & KEYSPEC_ATTRIBUTES_ECC_SIZE_MASK);
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Custom domain
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  if (has_custom_curve) {
    if (key->domain == NULL) {
      return SL_STATUS_INVALID_PARAMETER;
    }
    if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
      sl_se_custom_weierstrass_prime_domain_t *domain = (sl_se_custom_weierstrass_prime_domain_t*)key->domain;
      if (domain->a_is_zero && signing_only) {
        *keyspec |= KEYSPEC_ECC_WEIERSTRASS_PRIME_A_IS_ZERO;
      }
      if (domain->a_is_minus_three && signing_only) {
        *keyspec |= KEYSPEC_ECC_WEIERSTRASS_PRIME_A_IS_MINUS_THREE;
      }
    } else {
      return SL_STATUS_NOT_SUPPORTED;
    }
  }
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Convert a 32-bit SE compatible keyspec value to a key descriptor struct.
 ******************************************************************************/
sl_status_t sli_se_keyspec_to_key(const uint32_t keyspec,
                                  sl_se_key_descriptor_t* key)
{
  if (key == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Clear output
  key->type = 0;
  key->storage.method = 0;
  key->storage.location.slot = 0;
  key->size = 0;
  key->flags = 0;

  // ---------------------
  // Key type [31-28]
  key->type = keyspec & KEYSPEC_TYPE_MASK;

  // ---------------------
  // Key mode [27-26] and key index [23-16]

  if ((keyspec & KEYSPEC_MODE_MASK) == KEYSPEC_MODE_UNPROTECTED) {
    key->storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
  }
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  else if ((keyspec & KEYSPEC_MODE_MASK) == KEYSPEC_MODE_WRAPPED) {
    key->storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED;
  }
#endif
  else if ((keyspec & KEYSPEC_MODE_MASK) == KEYSPEC_MODE_VOLATILE) {
    // Volatile can mean either internal-volatile or internal-immutable
    // Check which is which based on key index
    uint32_t key_index = (keyspec & KEYSPEC_INDEX_MASK) >> KEYSPEC_INDEX_OFFSET;
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    if (key_index <= SL_SE_KEY_SLOT_VOLATILE_3) {
      key->storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE;
    } else
#endif
    if ((key_index <= SL_SE_KEY_SLOT_APPLICATION_ATTESTATION_KEY)
        && (key_index >= SL_SE_KEY_SLOT_INTERNAL_MIN)) {
      key->storage.method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE;
    } else {
      return SL_STATUS_INVALID_PARAMETER;
    }
    key->storage.location.slot = key_index;
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // ---------------------
  // Key restriction [25-24]

  uint32_t keyspec_restriction = (keyspec & KEYSPEC_RESTRICTION_MASK);
  if (keyspec_restriction == KEYSPEC_RESTRICTION_LOCKED) {
    key->flags |= SL_SE_KEY_FLAG_NON_EXPORTABLE;
  } else if (keyspec_restriction == KEYSPEC_RESTRICTION_UNLOCKED) {
    // no-op
  }
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  else if (keyspec_restriction == KEYSPEC_RESTRICTION_INTERNAL) {
    key->flags |= SL_SE_KEY_FLAG_IS_DEVICE_GENERATED;
  } else if (keyspec_restriction == KEYSPEC_RESTRICTION_RESTRICTED) {
    key->flags |= SL_SE_KEY_FLAG_IS_RESTRICTED;
  }
#endif
  else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Key restrictions are only applicable to volatile and wrapped keys
  if (key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT) {
    if (keyspec_restriction != 0) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  // ---------------------
  // Key NoProt [15]

  if ((keyspec & KEYSPEC_NOPROT_MASK) == (1 << KEYSPEC_NOPROT_OFFSET)) {
    key->flags |= SL_SE_KEY_FLAG_ALLOW_ANY_ACCESS;
  }

  // ---------------------
  // Key attributes [14-0]

  // Set public/private flags
  bool has_private_key = ((keyspec & KEYSPEC_ATTRIBUTES_ECC_PRIVATE_MASK) != 0);
  bool has_public_key = ((keyspec & KEYSPEC_ATTRIBUTES_ECC_PUBLIC_MASK) != 0);
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  bool has_custom_curve = ((keyspec & KEYSPEC_ATTRIBUTES_ECC_DOMAIN) != 0);
#endif
  bool signing_only = ((keyspec & KEYSPEC_ATTRIBUTES_ECC_SIGN) != 0);

  if ((keyspec & KEYSPEC_TYPE_MASK) >= KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME) {
    if (has_private_key) {
      key->flags |= SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY;
    }
    if (has_public_key) {
      key->flags |= SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY;
    }
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    if (has_custom_curve) {
      key->flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;

      // TODO: custom curve parameter injection?
      return SL_STATUS_NOT_SUPPORTED;
    }
#endif
    if (!(has_private_key || has_public_key)) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    // For ECC keys, their length is encoded in the type
    key->type = (key->type & ~SL_SE_KEY_TYPE_ATTRIBUTES_MASK) | ((keyspec & KEYSPEC_ATTRIBUTES_ECC_SIZE_MASK) + 1);
  }

  if ((keyspec & KEYSPEC_TYPE_MASK) >= KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME
      && (keyspec & KEYSPEC_TYPE_MASK) < KEYSPEC_TYPE_ECC_EDDSA) {
    if (signing_only) {
      key->flags |= SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY;
    }
  }

  // Ensure that symmetric keys don't have asymmetric flags
  if ((keyspec & KEYSPEC_TYPE_MASK) < KEYSPEC_TYPE_ECC_WEIERSTRASS_PRIME) {
    if (has_private_key || has_public_key || signing_only) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    key->size = keyspec & KEYSPEC_ATTRIBUTES_MASK;

    switch ((keyspec & KEYSPEC_ATTRIBUTES_MASK)) {
      case SL_SE_KEY_TYPE_AES_128:
      case SL_SE_KEY_TYPE_AES_192:
      case SL_SE_KEY_TYPE_AES_256:
        key->type = keyspec & (KEYSPEC_TYPE_MASK | KEYSPEC_ATTRIBUTES_MASK);
        break;
      default:
        break;
    }
  }

  return SL_STATUS_OK;
}

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
static const uint32_t default_auth_data[2] = { 0 };
#endif

sl_status_t sli_se_get_auth_buffer(const sl_se_key_descriptor_t *key,
                                   SE_DataTransfer_t *auth_buffer)
{
  if (key == NULL || auth_buffer == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  auth_buffer->next = (void*)SE_DATATRANSFER_STOP;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  if ((key->storage.method == SL_SE_KEY_STORAGE_INTERNAL_VOLATILE)
      || (key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED)) {
    if (key->password) {
      auth_buffer->data = key->password;
      auth_buffer->length = sizeof(default_auth_data) | SE_DATATRANSFER_REALIGN;
    } else {
      auth_buffer->data = (void*)default_auth_data;
      auth_buffer->length = sizeof(default_auth_data) | SE_DATATRANSFER_REALIGN;
    }
  } else {
    auth_buffer->length = 0 | SE_DATATRANSFER_REALIGN;
  }
#else
  (void)key;
  auth_buffer->length = 0 | SE_DATATRANSFER_REALIGN;
#endif

  return SL_STATUS_OK;
}

sl_status_t sli_se_get_key_input_output(const sl_se_key_descriptor_t *key,
                                        SE_DataTransfer_t *buffer)
{
  if (key == NULL || buffer == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  buffer->next = (void*)SE_DATATRANSFER_STOP;

  if ((key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      || (key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED)
#endif
      ) {
    // Set or adjust the key attributes for different key types
    uint32_t storage_size = 0;
    sl_status_t status = sli_key_get_storage_size(key, &storage_size);
    if (status != SL_STATUS_OK) {
      return status;
    }

    uint32_t total_storage_size = 0;
    if (key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT) {
      total_storage_size = storage_size;
    }
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    else if (key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED) {
      total_storage_size = storage_size + SLI_SE_WRAPPED_KEY_OVERHEAD;
    }
#endif

    if (total_storage_size > key->storage.location.buffer.size) {
      return SL_STATUS_WOULD_OVERFLOW;
    }

    buffer->data = key->storage.location.buffer.pointer;
    buffer->length = total_storage_size | SE_DATATRANSFER_REALIGN;
  } else if ((key->storage.method == SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
             || (key->storage.method == SL_SE_KEY_STORAGE_INTERNAL_VOLATILE)
#endif
             ) {
    buffer->length = 0;
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Validate key descriptor.
 ******************************************************************************/
sl_status_t sl_se_validate_key(const sl_se_key_descriptor_t *key)
{
  sl_status_t status = SL_STATUS_OK;
  uint32_t keyspec = 0;

  if (key == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  status = sli_se_key_to_keyspec(key, &keyspec);
  if (status != SL_STATUS_OK) {
    return status;
  }

  SE_DataTransfer_t auth_buffer;
  status = sli_se_get_auth_buffer(key, &auth_buffer);
  if (status != SL_STATUS_OK) {
    return status;
  }

  SE_DataTransfer_t key_buffer;
  status = sli_se_get_key_input_output(key, &key_buffer);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return status;
}

/***************************************************************************//**
 * Generate a random key adhering to the given key descriptor
 ******************************************************************************/
sl_status_t sl_se_generate_key(sl_se_command_context_t *cmd_ctx,
                               const sl_se_key_descriptor_t *key_out)
{
  sl_status_t status;

  if (cmd_ctx == NULL || key_out == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize command
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_CREATE_KEY);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key_out, status);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  SE_Command_t *se_cmd = &cmd_ctx->command;
  // Custom curve domain
  SE_DataTransfer_t domain_p_buffer;
  SE_DataTransfer_t domain_N_buffer;
  SE_DataTransfer_t domain_Gx_buffer;
  SE_DataTransfer_t domain_Gy_buffer;
  SE_DataTransfer_t domain_a_buffer;
  SE_DataTransfer_t domain_b_buffer;

  if (key_out->flags & SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN) {
    if (key_out->type & SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
      sl_se_custom_weierstrass_prime_domain_t *domain = (sl_se_custom_weierstrass_prime_domain_t*)key_out->domain;
      uint32_t domain_size = domain->size;

      domain_p_buffer.next = (void*)SE_DATATRANSFER_STOP;
      domain_p_buffer.data = (void*)domain->p;
      domain_p_buffer.length = domain_size;
      SE_addDataInput(se_cmd, (SE_DataTransfer_t*)&domain_p_buffer);

      domain_N_buffer.next = (void*)SE_DATATRANSFER_STOP;
      domain_N_buffer.data = (void*)domain->N;
      domain_N_buffer.length = domain_size;
      SE_addDataInput(se_cmd, (SE_DataTransfer_t*)&domain_N_buffer);

      domain_Gx_buffer.next = (void*)SE_DATATRANSFER_STOP;
      domain_Gx_buffer.data = (void*)domain->Gx;
      domain_Gx_buffer.length = domain_size;
      SE_addDataInput(se_cmd, (SE_DataTransfer_t*)&domain_Gx_buffer);

      domain_Gy_buffer.next = (void*)SE_DATATRANSFER_STOP;
      domain_Gy_buffer.data = (void*)domain->Gy;
      domain_Gy_buffer.length = domain_size;
      SE_addDataInput(se_cmd, (SE_DataTransfer_t*)&domain_Gy_buffer);

      domain_a_buffer.next = (void*)SE_DATATRANSFER_STOP;
      domain_a_buffer.data = (void*)domain->a;
      domain_a_buffer.length = domain_size;
      SE_addDataInput(se_cmd, (SE_DataTransfer_t*)&domain_a_buffer);

      domain_b_buffer.next = (void*)SE_DATATRANSFER_STOP;
      domain_b_buffer.data = (void*)domain->b;
      domain_b_buffer.length = domain_size;
      SE_addDataInput(se_cmd, (SE_DataTransfer_t*)&domain_b_buffer);
    } else {
      return SL_STATUS_NOT_SUPPORTED;
    }
  }
#endif

  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key_out, status);
  // Add key output block to command
  sli_add_key_output(cmd_ctx, key_out, status);

  // Execute command
  status = sli_se_execute_and_wait(cmd_ctx);

  #if defined(SLI_SE_KEY_PADDING_REQUIRED)
  // Clear padding for plaintext keys upon success
  if (status == SL_STATUS_OK) {
    status = clear_padding(key_out);
  }
  #endif

  return status;
}

/***************************************************************************//**
 * Export the public key of an ECC keypair
 ******************************************************************************/
sl_status_t sl_se_export_public_key(sl_se_command_context_t *cmd_ctx,
                                    const sl_se_key_descriptor_t *key_in,
                                    const sl_se_key_descriptor_t *key_out)
{
  if (cmd_ctx == NULL || key_in == NULL || key_out == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;

  // Check input/output key type and size relationship
  status = sli_key_check_equivalent(key_in, key_out, true, true);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Check that the input key has a private or public key
  if (!(key_in->flags & (SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
                         | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (!(key_out->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY)
      || (key_out->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize command
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_READPUB_KEY);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key_in, status);
  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key_in, status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key_in, status);

  // Pubkey buffer
  // Check for correct storage and flags
  if ((key_out->storage.method != SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT)
      || (key_out->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY)
      || !(key_out->flags & SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint32_t required_storage_size;
  status = sli_key_get_storage_size(key_out, &required_storage_size);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if (required_storage_size > key_out->storage.location.buffer.size) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  SE_DataTransfer_t pubkey_buffer = SE_DATATRANSFER_DEFAULT(
    key_out->storage.location.buffer.pointer, required_storage_size);
  SE_addDataOutput(se_cmd, &pubkey_buffer);

  // Execute command.
  // The retries are necessary in order to reduce the risk of random failures
  // in the accelerated point multiplication. This mainly affects very small or
  // large scalars, which in this case would be the private key.
  for (size_t i = 0; i < SLI_SE_MAX_POINT_MULT_RETRIES; ++i) {
    status = sli_se_execute_and_wait(cmd_ctx);
    if (status != SL_STATUS_FAIL) {
      break;
    }
  }
  return status;
}

/***************************************************************************//**
 * Returns the required storage size for the given key
 ******************************************************************************/
sl_status_t sl_se_get_storage_size(const sl_se_key_descriptor_t *key, uint32_t *storage_size)
{
  if (key == NULL || storage_size == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      || (key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED)
#endif
      ) {
    // Set or adjust the key attributes for different key types
    sl_status_t status = sli_key_get_storage_size(key, storage_size);
    if (status != SL_STATUS_OK) {
      return status;
    }
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    if (key->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED) {
      *storage_size = *storage_size + SLI_SE_WRAPPED_KEY_OVERHEAD;
    }
#endif
  } else if ((key->storage.method == SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
             || (key->storage.method == SL_SE_KEY_STORAGE_INTERNAL_VOLATILE)
#endif
             ) {
    *storage_size = 0;
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Protect a plaintext key using the SE
 ******************************************************************************/
sl_status_t sl_se_import_key(sl_se_command_context_t *cmd_ctx,
                             const sl_se_key_descriptor_t *key_in,
                             const sl_se_key_descriptor_t *key_out)
{
  sl_status_t status;

  if (cmd_ctx == NULL || key_in == NULL || key_out == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check input/output key type and size relationship
  status = sli_key_check_equivalent(key_in, key_out, true, false);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if ((key_in->storage.method != SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT)
      || ((key_out->storage.method != SL_SE_KEY_STORAGE_INTERNAL_VOLATILE)
          && (key_out->storage.method != SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize command
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_WRAP_KEY);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key_out, status);

  // Add key input block to command
  sli_add_key_input(cmd_ctx, key_in, status);

  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key_out, status);

  // Add key output block to command
  sli_add_key_output(cmd_ctx, key_out, status);

  status = sli_se_execute_and_wait(cmd_ctx);
  return status;
}

/***************************************************************************//**
 * Export a volatile or wrapped key back to plaintext if allowed
 ******************************************************************************/
sl_status_t sl_se_export_key(sl_se_command_context_t *cmd_ctx,
                             const sl_se_key_descriptor_t *key_in,
                             const sl_se_key_descriptor_t *key_out)
{
  sl_status_t status;

  if (cmd_ctx == NULL || key_in == NULL || key_out == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check input/output key type and size relationship
  status = sli_key_check_equivalent(key_in, key_out, true, false);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if ((key_out->storage.method != SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT)
      || ((key_in->storage.method != SL_SE_KEY_STORAGE_INTERNAL_VOLATILE)
          && (key_in->storage.method != SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize command
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_UNWRAP_KEY);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key_in, status);

  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key_in, status);

  // Add key input block to command
  sli_add_key_input(cmd_ctx, key_in, status);

  // Add key output block to command
  sli_add_key_output(cmd_ctx, key_out, status);

  status = sli_se_execute_and_wait(cmd_ctx);

  #if defined(SLI_SE_KEY_PADDING_REQUIRED)
  // The SE will only output word-aligned data. Clear the extra padding before
  // returning
  if (status == SL_STATUS_OK) {
    status = clear_padding(key_out);
  }
  #endif

  return status;
}

/***************************************************************************//**
 * Transfer a protected (volatile/wrapped) key
 ******************************************************************************/
sl_status_t sl_se_transfer_key(sl_se_command_context_t *cmd_ctx,
                               const sl_se_key_descriptor_t *key_in,
                               const sl_se_key_descriptor_t *key_out)
{
  if (cmd_ctx == NULL || key_in == NULL || key_out == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;
  SE_DataTransfer_t auth_buffer_out;
  uint32_t key_update_index;
  uint32_t key_update_mode;

  // Check input/output key type and size relationship
  status = sli_key_check_equivalent(key_in, key_out, true, false);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if ((key_in->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT)
      || (key_in->storage.method == SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE)
      || (key_out->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT)
      || (key_out->storage.method == SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Create command
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_TRANSFER_KEY);

  // Add key input parameters to command
  sli_add_key_parameters(cmd_ctx, key_in, status);

  // Add key update parameters to command
  uint32_t keyspec_out;
  status = sli_se_key_to_keyspec(key_out, &keyspec_out);
  if (status != SL_STATUS_OK) {
    return status;
  }
  key_update_index = ((keyspec_out & KEYSPEC_INDEX_MASK) >> KEYSPEC_INDEX_OFFSET);
  key_update_mode = ((keyspec_out & KEYSPEC_MODE_MASK) >> KEYSPEC_MODE_OFFSET);
  keyspec_out = (keyspec_out & ~KEYSPEC_TRANSFER_INDEX_MASK)
                | ((key_update_index << KEYSPEC_TRANSFER_INDEX_OFFSET)
                   & KEYSPEC_TRANSFER_INDEX_MASK);
  keyspec_out = (keyspec_out & ~KEYSPEC_TRANSFER_MODE_MASK)
                | ((key_update_mode << KEYSPEC_TRANSFER_MODE_OFFSET)
                   & KEYSPEC_TRANSFER_MODE_MASK);
  keyspec_out = (keyspec_out & ~KEYSPEC_TRANSFER_PROT_BIT_MASK);
  SE_addParameter(se_cmd, keyspec_out);

  // Add key input metadata block to command
  sli_add_key_metadata(cmd_ctx, key_in, status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key_in, status);

  // Add key output metadata block to command
  status = sli_se_get_auth_buffer(key_out, &auth_buffer_out);
  if (status != SL_STATUS_OK) {
    return status;
  }
  SE_addDataInput(se_cmd, &auth_buffer_out);

  // Add key output block to command
  sli_add_key_output(cmd_ctx, key_out, status);

  status = sli_se_execute_and_wait(cmd_ctx);

  #if defined(SLI_SE_KEY_PADDING_REQUIRED)
  // Clear padding bytes for plaintext keys upon success
  if (status == SL_STATUS_OK) {
    status = clear_padding(key_out);
  }
  #endif

  return status;
}

/***************************************************************************//**
 * Delete a key a volatile SE storage slot
 ******************************************************************************/
sl_status_t sl_se_delete_key(sl_se_command_context_t *cmd_ctx,
                             const sl_se_key_descriptor_t *key)
{
  sl_status_t status;

  if (cmd_ctx == NULL || key == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize command
  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_DELETE_KEY);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key, status);
  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key, status);

  // Execute command
  status = sli_se_execute_and_wait(cmd_ctx);
  return status;
}
#endif

/** @} (end addtogroup sl_se_key) */

#endif // defined(SEMAILBOX_PRESENT)
