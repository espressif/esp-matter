// Copyright 2021 The Pigweed Authors
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

package dev.pigweed.pw_rpc;

import static com.google.common.truth.Truth.assertThat;

import org.junit.Test;

public final class IdsTest {
  @Test
  public void calculate_emptyString() {
    assertThat(Ids.calculate("")).isEqualTo(0);
  }

  @Test
  public void calculate_singleCharacter() {
    assertThat(Ids.calculate("")).isEqualTo(0x00000000);
    assertThat(Ids.calculate("\0")).isEqualTo(0x00000001);
    assertThat(Ids.calculate("\1")).isEqualTo(65600);
    assertThat(Ids.calculate("?")).isEqualTo(4132738);
  }

  @Test
  public void calculate_multipleCharacters() {
    assertThat(Ids.calculate("Pigweed?")).isEqualTo(0x63D43D8C);
    assertThat(Ids.calculate("\0\0\0\1\1\1\1")).isEqualTo(0xD3556087);
    assertThat(Ids.calculate("Pigweed!Pigweed!Pigweed!Pigweed!Pigweed!Pigweed!"))
        .isEqualTo(0x79AB6494);
  }
}
