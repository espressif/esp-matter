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

#include "pw_tokenizer/detokenize.h"

#include <algorithm>
#include <cstring>

#include "pw_bytes/bit.h"
#include "pw_bytes/endian.h"
#include "pw_tokenizer/internal/decode.h"

namespace pw::tokenizer {
namespace {

std::string UnknownTokenMessage(uint32_t value) {
  std::string output(PW_TOKENIZER_ARG_DECODING_ERROR_PREFIX "unknown token ");

  // Output a hexadecimal version of the token.
  for (int shift = 28; shift >= 0; shift -= 4) {
    output.push_back("0123456789abcdef"[(value >> shift) & 0xF]);
  }

  output.append(PW_TOKENIZER_ARG_DECODING_ERROR_SUFFIX);
  return output;
}

// Decoding result with the date removed, for sorting.
using DecodingResult = std::pair<DecodedFormatString, uint32_t>;

// Determines if one result is better than the other if collisions occurred.
// Returns true if lhs is preferred over rhs. This logic should match the
// collision resolution logic in detokenize.py.
bool IsBetterResult(const DecodingResult& lhs, const DecodingResult& rhs) {
  // Favor the result for which decoding succeeded.
  if (lhs.first.ok() != rhs.first.ok()) {
    return lhs.first.ok();
  }

  // Favor the result for which all bytes were decoded.
  if ((lhs.first.remaining_bytes() == 0u) !=
      (rhs.first.remaining_bytes() == 0u)) {
    return lhs.first.remaining_bytes() == 0u;
  }

  // Favor the result with fewer decoding errors.
  if (lhs.first.decoding_errors() != rhs.first.decoding_errors()) {
    return lhs.first.decoding_errors() < rhs.first.decoding_errors();
  }

  // Favor the result that successfully decoded the most arguments.
  if (lhs.first.argument_count() != rhs.first.argument_count()) {
    return lhs.first.argument_count() > rhs.first.argument_count();
  }

  // Favor the result that was removed from the database most recently.
  return lhs.second > rhs.second;
}

}  // namespace

DetokenizedString::DetokenizedString(
    uint32_t token,
    const span<const TokenizedStringEntry>& entries,
    const span<const uint8_t>& arguments)
    : token_(token), has_token_(true) {
  std::vector<DecodingResult> results;

  for (const auto& [format, date_removed] : entries) {
    results.push_back(DecodingResult{format.Format(arguments), date_removed});
  }

  std::sort(results.begin(), results.end(), IsBetterResult);

  for (auto& result : results) {
    matches_.push_back(std::move(result.first));
  }
}

std::string DetokenizedString::BestString() const {
  return matches_.empty() ? std::string() : matches_[0].value();
}

std::string DetokenizedString::BestStringWithErrors() const {
  if (matches_.empty()) {
    return has_token_ ? UnknownTokenMessage(token_)
                      : PW_TOKENIZER_ARG_DECODING_ERROR("missing token");
  }
  return matches_[0].value_with_errors();
}

Detokenizer::Detokenizer(const TokenDatabase& database) {
  for (const auto& entry : database) {
    database_[entry.token].emplace_back(entry.string, entry.date_removed);
  }
}

DetokenizedString Detokenizer::Detokenize(
    const span<const uint8_t>& encoded) const {
  // The token is missing from the encoded data; there is nothing to do.
  if (encoded.empty()) {
    return DetokenizedString();
  }

  uint32_t token = bytes::ReadInOrder<uint32_t>(
      endian::little, encoded.data(), encoded.size());

  const auto result = database_.find(token);

  return DetokenizedString(
      token,
      result == database_.end() ? span<TokenizedStringEntry>()
                                : span(result->second),
      encoded.size() < sizeof(token) ? span<const uint8_t>()
                                     : encoded.subspan(sizeof(token)));
}

}  // namespace pw::tokenizer
