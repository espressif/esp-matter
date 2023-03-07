.. _module-pw_ide:

------
pw_ide
------
This module provides tools for supporting code editor and IDE features for
Pigweed projects.

Usage
=====

Setup
-----
Most of the time, ``pw ide setup`` is all you need to get started.

Configuration
-------------
``pw_ide`` has a built-in default configuration. You can create a configuration
file if you need to override those defaults.

A project configuration can be defined in ``.pw_ide.yaml`` in the project root.
This configuration will be checked into source control and apply to all
developers of the project. Each user can also create a ``.pw_ide.user.yaml``
file that overrides both the default and project settings, is not checked into
source control, and applies only to that checkout of the project. All of these
files have the same schema, in which these options can be configured:

.. autoproperty:: pw_ide.settings.PigweedIdeSettings.working_dir
.. autoproperty:: pw_ide.settings.PigweedIdeSettings.build_dir
.. autoproperty:: pw_ide.settings.PigweedIdeSettings.compdb_paths
.. autoproperty:: pw_ide.settings.PigweedIdeSettings.targets
.. autoproperty:: pw_ide.settings.PigweedIdeSettings.target_inference
.. autoproperty:: pw_ide.settings.PigweedIdeSettings.default_target
.. autoproperty:: pw_ide.settings.PigweedIdeSettings.setup
.. autoproperty:: pw_ide.settings.PigweedIdeSettings.clangd_additional_query_drivers
.. autoproperty:: pw_ide.settings.PigweedIdeSettings.editors

C++ Code Intelligence
---------------------
`clangd <https://clangd.llvm.org/>`_ is a language server that provides C/C++
code intelligence features to any editor that supports the language server
protocol (LSP). It uses a
`compilation database <https://clang.llvm.org/docs/JSONCompilationDatabase.html>`_,
a JSON file containing the compile commands for the project. Projects that have
multiple targets and/or use multiple toolchains need separate compilation
databases for each target/toolchain. ``pw_ide`` provides tools for managing
those databases.

Assuming you have a compilation database output from a build system, start with:

.. code-block:: bash

   pw ide cpp --process <path or glob to your compile_commands.json file(s)>

The ``pw_ide`` working directory will now contain one or more compilation
database files, each for a separate target among the targets defined in
``.pw_ide.yaml``. If you're using GN, you can generate the initial compilation
database and process it in a single command by adding the ``--gn`` flag.

List the available targets with:

.. code-block:: bash

   pw ide cpp --list

Then set the target that ``clangd`` should use with:

.. code-block:: bash

   pw ide cpp --set <selected target name>

``clangd`` can now be configured to point to the ``compile_commands.json`` file
in the ``pw_ide`` working directory and provide code intelligence for the
selected target. If you select a new target, ``clangd`` *does not* need to be
reconfigured to look at a new file (in other words, ``clangd`` can always be
pointed at the same, stable ``compile_commands.json`` file). However,
``clangd`` may need to be restarted when the target changes.

``clangd`` must be run within the activated Pigweed environment in order for
``clangd.sh`` instead of directly using the ``clangd`` binary.

``clangd`` must be run with arguments that provide the Pigweed environment paths
to the correct toolchains and sysroots. One way to do this is to launch your
editor from the terminal in an activated environment (e.g. running ``vim`` from
the terminal), in which case nothing special needs to be done as long as your
toolchains are in the Pigweed environment or ``$PATH``. But if you launch your
editor outside of the activated environment (e.g. launching Visual Studio Code
from your GUI shell's launcher), you can generate the command that invokes
``clangd`` with the right arguments with:

.. code-block:: bash

   pw ide cpp --clangd-command

Python Code Intelligence
------------------------
Any Python language server should work well with Pigweed projects as long as
it's configured to use the Pigweed virtual environment. You can output the path
to the virtual environment on your system with:

.. code-block:: bash

  pw ide python --venv

Command-Line Interface Reference
--------------------------------
.. argparse::
   :module: pw_ide.cli
   :func: _build_argument_parser
   :prog: pw ide

Design
======

Supporting ``clangd`` for Embedded Projects
-------------------------------------------
There are three main challenges that often prevent ``clangd`` from working
out-of-the-box with embedded projects:

#. Embedded projects cross-compile using alternative toolchains, rather than
   using the system toolchain. ``clangd`` doesn't know about those toolchains
   by default.

#. Embedded projects (particularly Pigweed project) often have *multiple*
   targets that use *multiple* toolchains. Most build systems that generate
   compilation databases put all compile commands in a single database, meaning
   a single file can have multiple, conflicting compile commands. ``clangd``
   will typically use the first one it finds, which may not be the one you want.

#. Pigweed projects have build steps that use languages other than C/C++. These
   steps are not relevant to ``clangd`` but many build systems will include them
   in the compilation database anyway.

To deal with these challenges, ``pw_ide`` processes the compilation database you
provide, yielding one or more compilation databases that are valid, consistent,
and specific to a particular build target. This enables code intelligence and
navigation features that reflect that build.

After processing a compilation database, ``pw_ide`` knows what targets are
available and provides tools for selecting which target is active. These tools
can be integrated into code editors, but are ultimately CLI-driven and
editor-agnostic. Enabling code intelligence in your editor may be as simple as
configuring its language server protocol client to use the ``clangd`` command
that ``pw_ide`` can generate for you.

When to provide additional configuration to support your use cases
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The default configuration for ``clangd`` in ``pw_ide`` should work without
additional configuration as long as you're using only toolchains provided by
Pigweed and your native host toolchain. If you're using other toolchains, keep
reading.

``clangd`` needs two pieces of information to use a toolchain:

#. A path to the compiler, which will be taken from the compile command.

#. The same compiler to be reflected in the
   `query driver <https://releases.llvm.org/10.0.0/tools/clang/tools/extra/docs/clangd/Configuration.html>`_
   argument provided when running ``clangd``.

When using ``pw_ide`` with external toolchains, you only need to add a path to
the compiler to ``clangd_additional_query_drivers`` in your project's
``pw_ide.yaml`` file. When processing a compilation database, ``pw_ide`` will
use the query driver globs to find your compiler and configure ``clangd`` to
use it.

Selected API Reference
^^^^^^^^^^^^^^^^^^^^^^
.. automodule:: pw_ide.cpp
   :members: CppCompileCommand, CppCompilationDatabase, CppCompilationDatabasesMap, CppIdeFeaturesState, path_to_executable, target_is_enabled, ClangdSettings

Automated Support for Code Editors & IDEs
-----------------------------------------
``pw_ide`` provides a consistent framework for automatically applying settings
for code editors, where default settings can be defined within ``pw_ide``,
which can be overridden by project settings, which in turn can be overridden
by individual user settings.

Selected API Reference
^^^^^^^^^^^^^^^^^^^^^^
.. automodule:: pw_ide.editors
   :members: EditorSettingsDefinition, EditorSettingsFile, EditorSettingsManager

.. automodule:: pw_ide.vscode
   :members: VscSettingsType, VscSettingsManager
