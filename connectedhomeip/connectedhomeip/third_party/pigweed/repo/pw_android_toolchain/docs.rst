.. _module-pw_android_toolchain:

--------------------
pw_android_toolchain
--------------------
Android toolchains differ from ``pw_toolchain`` in that the latter defines the
tool names and paths at the lowest level, with customisation added at higher
levels, while in ``pw_android_toolchain`` the tool names and paths are derived
from build args and defaults so are defined last by calling
``pw_generate_android_toolchain``.

Setup
=====
You must first download and unpack a copy of the `Android NDK`_ and let Pigweed
know where that is located using the ``pw_android_toolchain_NDK_PATH`` build
arg.

.. _Android NDK: https://developer.android.com/ndk

You can set Pigweed build options using ``gn args out``.

Toolchains
==========
``pw_android_toolchain`` provides GN toolchains that may be used to build
Pigweed against an Android NDK. The following toolchains are defined:

 - arm_android_debug
 - arm_android_size_optimized
 - arm_android_speed_optimized
 - arm64_android_debug
 - arm64_android_size_optimized
 - arm64_android_speed_optimized
 - x64_android_debug
 - x64_android_size_optimized
 - x64_android_speed_optimized
 - x86_android_debug
 - x86_android_size_optimized
 - x86_android_speed_optimized

.. note::
  The documentation for this module is currently incomplete.

Defining Toolchains
===================
Defining Android NDK toolchains is similar to ``pw_toolchain`` except that
instead of using ``generate_toolchain`` use ``pw_generate_android_toolchain``,
and ensure that ``current_cpu`` is set in the toolchain ``defaults``.

For example:

.. code::

  import("//build_overrides/pigweed.gni")

  import("$dir_pw_android_toolchain/toolchains.gni")
  import("$dir_pw_android_toolchain/generate_toolchain.gni")

  my_target_scope = {
    # Use Pigweed's Android toolchain as a base.
    _toolchain_base = pw_toolchain_android.debug

    # Forward everything except the defaults scope from that toolchain.
    forward_variables_from(_toolchain_base, "*", [ "defaults" ])

    defaults = {
      # Forward everything from the base toolchain's defaults.
      forward_variables_from(_toolchain_base.defaults, "*")

      # Build for 64-bit AArch64 Android devices.
      current_cpu = "arm64"

      # Extend with custom build arguments for the target.
      pw_log_BACKEND = dir_pw_log_tokenized
    }
  }

  # Create the actual GN toolchain from the scope.
  pw_generate_android_toolchain("my_target") {
    forward_variables_from(my_target_scope, "*")
  }

Since Android NDKs contain toolchains for all supported targets, as a
convenience, ``pw_generate_android_toolchains`` does not require that
``current_cpu`` is set. If any toolchain scope in the list does not set it, a
toolchain for each supported target will be generated.

.. code::

  # Generate arm_*, arm64_*, x64_*, and x86_* for each scope in the list.
  pw_generate_android_toolchains("target_toolchains) {
    toolchains = pw_toolchain_android_list
  }

Customization
-------------
The Android SDK target version defaults to the value of the
``pw_android_toolchain_API_LEVEL`` build arg. You can override this on global
level, or on a per-toolchain level by setting ``api_level`` in the toolchain
defaults.
