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
#pragma once

#include <cstdint>
#include <cstring>

#include "pw_bytes/span.h"
#include "pw_random/random.h"
#include "pw_span/span.h"
#include "pw_status/status_with_size.h"

namespace pw::random {

// This is the "xorshift*" algorithm which is a bit stronger than plain XOR
// shift thanks to the nonlinear transformation at the end (multiplication).
//
// See: https://en.wikipedia.org/wiki/Xorshift
//
// This random generator is NOT cryptographically secure, and incorporates
// pseudo-random generation to extrapolate any true injected entropy. The
// distribution is not guaranteed to be uniform.
class XorShiftStarRng64 : public RandomGenerator {
 public:
  XorShiftStarRng64(uint64_t initial_seed) : state_(initial_seed) {}

  // This generator uses entropy-seeded PRNG to never exhaust its random number
  // pool.
  void Get(ByteSpan dest) final {
    while (!dest.empty()) {
      uint64_t random = Regenerate();
      size_t copy_size = std::min(dest.size_bytes(), sizeof(state_));
      std::memcpy(dest.data(), &random, copy_size);
      dest = dest.subspan(copy_size);
    }
  }

  // Entropy is injected by rotating the state by the number of entropy bits
  // before xoring the entropy with the current state. This ensures seeding
  // the random value with single bits will progressively fill the state with
  // more entropy.
  void InjectEntropyBits(uint32_t data, uint_fast8_t num_bits) final {
    if (num_bits == 0) {
      return;
    } else if (num_bits > 32) {
      num_bits = 32;
    }

    // Rotate state.
    uint64_t untouched_state = state_ >> (kNumStateBits - num_bits);
    state_ = untouched_state | (state_ << num_bits);
    // Zero-out all irrelevant bits, then XOR entropy into state.
    uint32_t mask = (static_cast<uint64_t>(1) << num_bits) - 1;
    state_ ^= (data & mask);
  }

 private:
  // Calculate and return the next value based on the "xorshift*" algorithm
  uint64_t Regenerate() {
    // State must be nonzero, or the algorithm will get stuck and always return
    // zero.
    if (state_ == 0) {
      state_--;
    }
    state_ ^= state_ >> 12;
    state_ ^= state_ << 25;
    state_ ^= state_ >> 27;
    return state_ * kMultConst;
  }
  uint64_t state_;
  static constexpr uint8_t kNumStateBits = sizeof(state_) * 8;

  // For information on why this constant was selected, see:
  // https://www.jstatsoft.org/article/view/v008i14
  // http://vigna.di.unimi.it/ftp/papers/xorshift.pdf
  static constexpr uint64_t kMultConst = 0x2545F4914F6CDD1D;
};

}  // namespace pw::random
