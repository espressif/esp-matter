// Copyright 2020 The Pigweed Authors
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

// This file provides the Detokenizer class, which is used to decode tokenized
// strings.  To use a Detokenizer, load a binary format token database into
// memory, construct a TokenDatabase, and pass it to a Detokenizer:
//
//   std::vector data = ReadFile("my_tokenized_strings.db");
//   Detokenizer detok(TokenDatabase::Create(data));
//
//   DetokenizedString result = detok.Detokenize(my_data);
//   std::cout << result.BestString() << '\n';
//
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "pw_span/span.h"
#include "pw_tokenizer/internal/decode.h"
#include "pw_tokenizer/token_database.h"

namespace pw::tokenizer {

using TokenizedStringEntry = std::pair<FormatString, uint32_t /*date removed*/>;

// A string that has been detokenized. This class tracks all possible results if
// there are token collisions.
class DetokenizedString {
 public:
  DetokenizedString(uint32_t token,
                    const span<const TokenizedStringEntry>& entries,
                    const span<const uint8_t>& arguments);

  DetokenizedString() : has_token_(false) {}

  // True if there was only one valid match and it decoded successfully.
  bool ok() const { return matches_.size() == 1 && matches_[0].ok(); }

  // Returns the strings that matched the token, with the best matches first.
  const std::vector<DecodedFormatString>& matches() const { return matches_; }

  const uint32_t& token() const { return token_; }

  // Returns the detokenized string or an empty string if there were no matches.
  // If there are multiple possible results, the DetokenizedString returns the
  // first match.
  std::string BestString() const;

  // Returns the best match, with error messages inserted for arguments that
  // failed to parse.
  std::string BestStringWithErrors() const;

 private:
  uint32_t token_;
  bool has_token_;
  std::vector<DecodedFormatString> matches_;
};

// Decodes and detokenizes strings from a TokenDatabase. This class builds a
// hash table from the TokenDatabase to give O(1) token lookups.
class Detokenizer {
 public:
  // Constructs a detokenizer from a TokenDatabase. The TokenDatabase is not
  // referenced by the Detokenizer after construction; its memory can be freed.
  Detokenizer(const TokenDatabase& database);

  // Decodes and detokenizes the encoded message. Returns a DetokenizedString
  // that stores all possible detokenized string results.
  DetokenizedString Detokenize(const span<const uint8_t>& encoded) const;

  DetokenizedString Detokenize(const std::string_view& encoded) const {
    return Detokenize(encoded.data(), encoded.size());
  }

  DetokenizedString Detokenize(const void* encoded, size_t size_bytes) const {
    return Detokenize(span(static_cast<const uint8_t*>(encoded), size_bytes));
  }

 private:
  std::unordered_map<uint32_t, std::vector<TokenizedStringEntry>> database_;
};

}  // namespace pw::tokenizer
