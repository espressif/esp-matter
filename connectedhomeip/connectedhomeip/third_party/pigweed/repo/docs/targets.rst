.. _docs-targets:

=======
Targets
=======
Pigweed is designed to be portable to many different hardware platforms.
Pigweed's GN build comes with an extensible target system that allows it to be
configured to build for any number of platforms, which all build simultaneously.

Defining a target
=================
Each Pigweed target built by a project is defined within the GN build as a
toolchain providing the target's build parameters.

In Pigweed, these target toolchains are defined as GN scopes, which are fed into
a ``generate_toolchain`` template to create the complete GN toolchain.

Hierarchical target structure
-----------------------------
The rationale for scope-based toolchains is to make Pigweed targets extensible.
Variables from a toolchain can be forwarded into new scopes and then extended
or overriden. This facilitates the sharing of common configuration options
between toolchains, and allows for hierarchical structures. Upstream Pigweed
makes use of this heavily; it defines basic compiler-only configurations, uses
these as a base for board-specific toolchains, then creates its final targets on
top of those.

.. mermaid::

  graph TD
    arm_gcc --> arm_gcc_cortex_m4
    arm_gcc --> arm_gcc_cortex_m4f
    arm_gcc_cortex_m4f --> arm_gcc_cortex_m4f_debug
    arm_gcc_cortex_m4f --> arm_gcc_cortex_m4f_size_optimized
    arm_gcc_cortex_m4f_debug --> stm32f429i_disc1_debug

Toolchain target variables
--------------------------
The core of a toolchain is defining the tools it uses. This is done by setting
the variables ``ar``, ``cc``, and ``cxx`` to the appropriate compilers. Pigweed
provides many commonly used compiler configurations in the ``pw_toolchain``
module.

The rest of a Pigweed target's configuration is listed within a ``defaults``
scope in its toolchain. Every variable in this scope is an override of a GN
build argument defined in Pigweed. Some notable arguments include:

* ``default_configs``: A list of GN configs to apply to every ``pw_*`` GN
  template. This is typically used to set compiler flags, optimization levels,
  global #defines, etc.
* ``default_public_deps``: List of GN targets which are added as a dependency
  to all ``pw_*`` GN targets. This is used to add global module dependencies;
  for example, in upstream, ``pw_polyfill`` is added here to provide C++17
  features in C++14 code.
* Facade backends: Pigweed defines facades to provide a common interface for
  core system features such as logging without assuming an implementation.
  When building a Pigweed target, the implementations for each of these must be
  chosen. The ``*_BACKEND`` build args that Pigweed defines are used to set
  these.

There are many other build arguments that can be set, some of which are
module-specific. A full list can be seen by running ``gn args --list out``,
and further documentation can be found within their respective modules.

Example Pigweed target
======================
The code below demonstrates how a project might configure one of its Pigweed
targets.

.. code-block::

  import("//build_overrides/pigweed.gni")

  import("$dir_pw_toolchain/arm_gcc/toolchains.gni")
  import("$dir_pw_toolchain/generate_toolchain.gni")

  my_target_scope = {
    # Use Pigweed's Cortex M4 toolchain as a base.
    _toolchain_base = pw_toolchain_arm_gcc.cortex_m4f_debug

    # Forward everything except the defaults scope from that toolchain.
    forward_variables_from(_toolchain_base, "*", [ "defaults" ])

    defaults = {
      # Forward everything from the base toolchain's defaults.
      forward_variables_from(_toolchain_base.defaults, "*")

      # Extend with custom build arguments for the target.
      pw_log_BACKEND = dir_pw_log_tokenized
    }
  }

  # Create the actual GN toolchain from the scope.
  generate_toolchain("my_target") {
    forward_variables_from(my_target_scope, "*")
  }

Upstream targets
================
The following is a list of targets used for upstream Pigweed development.

.. toctree::
  :maxdepth: 1
  :glob:

  targets/*/target_docs
