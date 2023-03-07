.. _docs-concepts:

=============
About Pigweed
=============

Why Build Pigweed?
==================
Our goal is to make embedded software development efficient, robust, and
heck, even delightful, for projects ranging from weekend Arduino experiements
to commercial products selling in the millions.

Embedded software development is notoriously arcane. Developers often have to
track down vendor toolchains specific to the hardware they're targeting, write
their code against hardware-specfic SDKs/HALs, and limit themselves to a small
subset of C. Project teams are on their own to figure out how to set up a build
system, automated testing, serial communication, and many other embedded
project fundamentals. This is error prone and takes effort away from developing
the actual product!

There are solutions on the market that promise to solve all of these problems
with a monolithic framework—just write your code against the framework and use
hardware the framework supports, and you get an efficient embedded development
environment. But this approach doesn't work well for existing projects that
weren't built on the framework from the beginning or for projects that have
specific needs the framework wasn't designed for. We know from experience that
this approach alone doesn't meet our goal.

So we have set out to build a platform that supports successful embedded
developers at every scale by allowing them to adopt as much or as little of
what Pigweed provides as they need, in the way that works best for their
project.

How Pigweed Works
=================
Pigweed provides four foundational pillars to support your embedded development:

* :ref:`A comprehensive set of libraries for embedded development<docs-concepts-embedded-development-libraries>`
* :ref:`A hermetic and replicable development environment<docs-concepts-development-environment>`
* :ref:`A system for building, testing, and linting your project<docs-concepts-build-system>`
* :ref:`A full framework for new projects that want a turn-key solution<docs-concepts-full-framework>`

.. _docs-concepts-embedded-development-libraries:

Embedded Development Libraries
------------------------------
Pigweed enables you to use modern C++ and software development best practices in
your embedded project without compromising performance or increasing memory use
compared to conventional embedded C.

We provide libraries (modules) for :ref:`strings<module-pw_string>`,
:ref:`time<module-pw_chrono>`, :ref:`assertions<module-pw_assert>`,
:ref:`logging<module-pw_log>`, :ref:`serial communication<module-pw_spi>`,
:ref:`remote procedure calls (RPC)<module-pw_rpc>`, and
:ref:`much more<docs-module-guides>`.

These modules are designed to work both on your host machine and on a wide
variety of target devices. We achieve this by writing them in an inherently
portable way, or through the facade/backend pattern. As a result, you can write
most or all of your code to run transparently on your host machine and targets.

.. _docs-concepts-development-environment:

Development Environment
-----------------------
Managing toolchains, build systems, and other software needed for a project is
complex. Pigweed provides all of this out of the box for Linux, Mac, and
Windows systems in a sealed environment that leaves the rest of your system
untouched. Getting new developers started is as simple as cloning your project
repository and activating the Pigweed environment.

.. _docs-concepts-build-system:

Build System
------------
Pigweed modules are built to integrate seamlessly into projects using GN. We
are rapidly expanding our good support for CMake and nascent support for Bazel
so you can use your build system of choice. For new projects, Pigweed provides a
build system you can integrate your own code into that works out of the box.

.. _docs-concepts-full-framework:

Full Framework (coming in 2022)
-------------------------------
For those who want a fully-integrated solution that provides everything Pigweed
has to offer with an opinionated project structure, we are working diligently
on a :ref:`Pigweed framework<module-pw_system>`. Stay tuned for more news to
come! In the meantime, we invite you to discuss this and collaborate with us
on `Discord <https://discord.gg/M9NSeTA>`_.

.. _docs-concepts-right-for-my-project:

Is Pigweed Right for My Project?
================================
Pigweed is still in its early stages, and while we have ambitious plans for it,
Pigweed might not be the right fit for your project today. Here are some things
to keep in mind:

* Many individual modules are stable and are running on shipped devices today.
  If any of those modules meet your needs, you should feel safe bringing them
  into your project.

* Some modules are in very early and active stages of development. They likely
  have unstable APIs and may not work on all supported targets. If this is the
  case, it will be indicated in the module's documentation. If you're interested
  in contributing to the development of one of these modules, we encourage you
  to experiment with them. Otherwise they aren't ready for use in most projects.

* Setting up new projects to use Pigweed is currently not very easy, but we are
  working to address that. In the meantime, join the Pigweed community on
  `Discord <https://discord.gg/M9NSeTA>`_ to get help.

Supported language versions
===========================

C++
---
Most Pigweed code requires C++17, but a few modules, such as
:ref:`module-pw_kvs` and :ref:`module-pw_tokenizer`, work with C++14. All
Pigweed code is compatible with C++20. Pigweed defines toolchains for testing
with C++14 and C++20; see :ref:`target-host` target documentation for more
information.

.. _docs-concepts-python-version:

Python
------
Pigweed officially supports Python 3.8, 3.9, and 3.10.
