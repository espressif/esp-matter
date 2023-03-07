.. _docs-faq:

--------------------------
Frequently Asked Questions
--------------------------

Is Pigweed a framework?
-----------------------
There are three core components of Pigweed's offering:

1. The environment setup system (bootstrap and activate)
2. The GN-based build and test system
3. The individual modules (mostly C++ code)

We consider #1 and #2 combined as the "Pigweed Monolith" - since it has an
integrated environment setup and build system. However, this part of the system
is entirely optional; it is not only possible, but encouraged to take
individual modules (like for example ``pw_tokenizer`` or ``pw_ring_buffer``)
and integrate them into your existing environment setup approach and build
system.

So, to answer the question:

1. **Is Pigweed's environment setup system a framework?** No. This component
   solves (a) downloading compilers and (b) setting up a Python virtual
   environment; what you do with the environment is up to you.
2. **Is Pigweed's GN-based build & test setup system a framework?** In short,
   yes. This is the most framework-like part of Pigweed, since you build your
   code using Pigweed's GN-based primitives. However, it is optional.
3. **Are Pigweed's C++ modules a framework?** No. They are libraries you can
   independently take into your project and use like any other C++ library.

Isn't C++ bloated and slow?
---------------------------
In general, no, but it is important to follow some guidelines as discussed in
the :ref:`Embedded C++ Guide <docs-embedded-cpp>`.

At Google, we have made some quantitative analysis of various common embedded
patterns in C++ to evaluate the cost of various constructs. We will open source
this work at some point.

How do I setup Pigweed for my project?
--------------------------------------

.. attention::

  This FAQ entry should be an entire article, but isn't one yet; sorry!

To use Pigweed, you must decide the answer to one question: Do you wish to use
the Pigweed integrated environment and build, or just use individual modules?

A la carte: Individual modules only
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This is the best option if you have an existing project, with pre-existing build
in place.

To use the libraries, submodule or copy the relevant Pigweed modules into your
project, and use them like any other C++ library. You can reference the
existing GN files or CMake files when doing this. In the case of CMake, you can
directly import Pigweed's build from your project with CMake's external project
system, or just use a CMake include statement.

Monolith: Using the integrated system
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This may be a good option if you are starting a new project. However,
there is a gap in our documentation and examples at the moment; sorry about
that! You may need to ask us for help; try the mailing list or chat room.

Note: Pigweed isn't quite ready for masses of projects to use the whole system
just yet. We will be offering examples and material for this eventually.

The summary is:

- Create a new repository
- Add Pigweed as a submodule; we suggest under ``third_party/pigweed`` or
  ``vendor/pigweed``. It is also possible to use Android's repo tool, but we
  suggest submodules.
- Create your own ``BUILD.gn`` and ``BUILDCONFIG.gn``; we suggest starting by
  copying the ones from Pigweed. You'll need to create your own toplevel
  targets like those in Pigweed's root ``BUILD.gn``.
- Create a bootstrap script in your project root that optionally does some
  project specific setup, then invokes the Pigweed upstream bootstrap (or in
  the other order).
- If you have custom hardware, you will want to create a *target*. See the
  `targets/` directory for examples like the STM32F429i-Discovery.

.. attention::

  This section is incomplete; if you need help please reach out in chat or on
  the mailing list. We know this part of Pigweed is incomplete and will help
  those who are interested in giving Pigweed a try.

Why doesn't Pigweed allow shell scripting?
------------------------------------------
Pigweed supports multiple platforms. The native shells on these differ and
additionally "compatible" shells often have sububle differences in behavior.
Pigweed uses Python instead shell wherever practical and changes to Pigweed that
include shell scripting will likely be rejected. Users of Pigweed may use shell
scripts in their own code and we have included support for
`Shellcheck <https://www.shellcheck.net/>`_ during presubmit checks that is
automatically enabled if ``shellcheck`` found in the path.

What development hosts are supported?
-------------------------------------
We support the following platforms:

+-------------------------------+---------------------------------------+
| **Development host**          | **Comments**                          |
+-------------------------------+---------------------------------------+
| Linux on x86-64               | Most recent Linux distributions       |
|                               | will work.                            |
+-------------------------------+---------------------------------------+
| macOS on x86-64               | Mojave or newer should work.          |
+-------------------------------+---------------------------------------+
| Windows 10 on x86-64          | Native Windows only; WSL1 or 2 not    |
|                               | supported.                            |
+-------------------------------+---------------------------------------+
| Docker on x86-64              | Containers based on Ubuntu 18.04      |
|                               | and newer.                            |
+-------------------------------+---------------------------------------+

.. attention::

  In all of the above supported platforms, the support is contingent on using
  Pigweed's bootstrap (env setup) system. While it is possible to use Pigweed
  without bootstrap, it is unsupported and undocumented at this time.

Partially supported host platform: Chrome OS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Chromebooks are able to run some of the Pigweed components; notably all the
"host" target builds and runs. However, due to USB access issues, it is not
possible to flash and run on real hardware (like for example the STM32F429i
Discovery).

To run on ChromeOS:

1. `Enable the Linux shell <https://support.google.com/chromebook/answer/9145439>`_
2. ``sudo apt-install build-essential``
3. Go through Pigweed setup.

What about other host platforms?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
There are two key issues that make running the host tooling on other platforms
challenging:

1. Availability of up-to-date compilers
2. Availability of up-to-date Python

For both of these, Pigweed relies on Google-maintained binaries packaged in
CIPD, which includes daily builds of LLVM, and recent versions of the ARM GCC
toolchains. Platforms other than Mac/Windows/Linux running on x86-64 will need
to provide equivalent binaries, which is some effort.

Host platforms that we are likely to support in the future
..........................................................

- **Mac on ARM (M1)** - This is currently experimentally supported through
  Rosetta, and this support is enabled by default. To explicitly choose to use
  or not use Rosetta set add ``"rosetta": "force"`` to your environment setup
  config file. Other possible values are ``"never"`` and ``"allow"``. For now,
  ``"allow"`` means ``"force"`` but at some point in the future it will change
  to ``"never"``.
- **Linux on ARM** - At time of writing (mid 2020), we do not support ARM-based
  host platforms.  However, we would like to support this eventually.
- **Windows on WSL2 x86-64** - There are some minor issues preventing WSL2 on
  Windows from being a smooth experience, but we are working on them.

Platforms that we are unlikely to support
.........................................

- **Anything on x86-32** - While it's possible 32-bit x86 could be made to
  work, we don't have enough users to make this worthwhile. If this is
  something you are interested in and would be willing to support, let us know.

Why name the project Pigweed?
-----------------------------
Pigweed, also known as amaranth, is a nutritious grain and leafy salad green
that is also a rapidly growing weed. When developing the project that
eventually became Pigweed, we wanted to find a name that was fun, playful, and
reflective of how we saw Pigweed growing. Teams would start out using one
module that catches their eye, and after that goes well, they’d quickly start
using more.

So far, so good 😁
