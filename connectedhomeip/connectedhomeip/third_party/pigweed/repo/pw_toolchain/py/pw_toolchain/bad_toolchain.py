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
"""Emits an error telling the user not to use the default toolchain."""

import sys

_ERROR_MESSAGE = """
Error: trying to build a target with the default toolchain

  This occurs when a GN target is listed as a dependency outside of a toolchain
  group (such as host_clang or stm32f429i) in the root BUILD.gn file.

  Make sure that your top-level targets are always instantiated with a toolchain
  and that no dependencies are pulled in through the default toolchain.

  group("my_target_wrapper") {
    deps = [ ":my_target(//path/to/my/toolchain)" ]
  }

  group("my_target") {
    deps = []
    if (current_toolchain != default_toolchain) {
      deps += [ "//my_application:image" ]
    }
  }

  If you are developing in Pigweed itself, list your build target under one of
  the predefined groups in //BUILD.gn. For example,

    # apps is an existing group intended for building application images.
    group("apps") {
      deps = [
        ...
        "your_target:here",
      ]
    }

  Other predefined groups include host_tools, pw_modules, and pw_module_tests.

  If you want to add a custom group instead of using an existing one, it must be
  defined alongside the predefined groups, within the toolchain condition block:

    if (current_toolchain != default_toolchain) {
      group("apps") {
        ...
      }

      # Other predefined groups...

      group("my_custom_group") {
        deps = [ "//path/to:my_target" ]
      }
    }

  To include your custom group in the build, add it to the pigweed_default group
  to have it compile for every supported Pigweed target.

    group("pigweed_default") {
      deps = []

      if (current_toolchain != default_toolchain) {
        # Standard Pigweed dependencies...

        # Add your group here.
        deps += [ ":my_custom_group" ]
      }
    }

  For more details on the Pigweed build structure and how to configure custom
  build targets or toolchains, please refer to "Build system" in the Pigweed
  documentation.
"""


def main() -> int:
    print(_ERROR_MESSAGE, file=sys.stderr)
    return 1


if __name__ == '__main__':
    sys.exit(main())
