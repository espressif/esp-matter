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

#include "pw_intrusive_ptr/internal/ref_counted_base.h"

#include <atomic>
#include <cstdint>

#include "pw_assert/check.h"

namespace pw::internal {

RefCountedBase::~RefCountedBase() {
  // Reset the ref-count back to the pre-adopt sentinel value so that we
  // have the best chance of catching a use-after-free situation.
  //
  // The value is chosen specifically to be negative when stored as an int32_t,
  // and as far away from becoming positive (via either addition or subtraction)
  // as possible.
  ref_count_.store(static_cast<int32_t>(0xC0000000), std::memory_order_release);
}

void RefCountedBase::AddRef() const {
  const auto refs = ref_count_.fetch_add(1, std::memory_order_relaxed);

  // This assertion will fire if someone calls AddRef() on a ref-counted object
  // that has reached ref_count_ == 0 but has not been destroyed yet. This could
  // happen by manually calling AddRef(), or re-wrapping such a pointer with
  // RefPtr<T>(T*) (which calls AddRef()).
  PW_DCHECK(refs >= 0);
}

bool RefCountedBase::ReleaseRef() const {
  // We don't follow the boost::intrusive_ptr/fit::RefPtr approach here with a
  // release fetch_sub and acquire fence afterwards due to TSAN not supporting
  // fences (see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97868).
  //
  // This approach is a bit less performant - it does the acquiring on each
  // release, not only for the last ref - but otherwise works the same.
  const auto refs = ref_count_.fetch_sub(1, std::memory_order_acq_rel);

  // This assertion will fire if someone manually calls ReleaseRef()
  // on a ref-counted object too many times, or if ReleaseRef is called
  // before an object has been wrapped with RefPtr.
  PW_DCHECK(refs >= 1);

  return refs == 1;
}

}  // namespace pw::internal
