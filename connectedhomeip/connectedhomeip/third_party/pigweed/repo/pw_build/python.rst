.. _module-pw_build-python:

===================
Python GN Templates
===================
The Python build is implemented with GN templates defined in
``pw_build/python.gni``. See the .gni file for complete usage documentation.

.. seealso::

   - :bdg-ref-primary-line:`docs-python-build` for an overview on how Python in
     GN is built.
   - The :bdg-ref-primary-line:`module-pw_build` docs for other GN templates
     available within Pigweed.

.. _module-pw_build-python-base-templates:

---------------------
Python Base Templates
---------------------
The core subset of templates where you can create Python packages, actions,
scripts and group them together are listed below.

- :ref:`module-pw_build-pw_python_package`
- :ref:`module-pw_build-pw_python_action`
- :ref:`module-pw_build-pw_python_script`
- :ref:`module-pw_build-pw_python_group`

.. _module-pw_build-pw_python_package:

pw_python_package
=================
The main Python template is ``pw_python_package``. Each ``pw_python_package``
target represents a Python package. As described in
:ref:`module-pw_build-python-target`, each ``pw_python_package`` expands to
several subtargets. In summary, these are:

- ``<name>`` - Represents the files themselves
- ``<name>.lint`` - Runs static analysis
- ``<name>.tests`` - Runs all tests for this package
- ``<name>.install`` - Installs the package
- ``<name>.wheel`` - Builds a Python wheel

GN permits using abbreviated labels when the target name matches the directory
name (e.g. ``//foo`` for ``//foo:foo``). For consistency with this, Python
package subtargets are aliased to the directory when the target name is the
same as the directory. For example, these two labels are equivalent:

.. code-block::

   //path/to/my_python_package:my_python_package.tests
   //path/to/my_python_package:tests

The actions in a ``pw_python_package`` (e.g. installing packages and running
Pylint) are done within a single GN toolchain to avoid duplication in
multi-toolchain builds. This toolchain can be set with the
``pw_build_PYTHON_TOOLCHAIN`` GN arg, which defaults to
``$dir_pw_build/python_toolchain:python``.

Arguments
---------
- ``setup`` - List of setup file paths (setup.py or pyproject.toml & setup.cfg),
  which must all be in the same directory.
- ``generate_setup``: As an alternative to ``setup``, generate setup files with
  the keywords in this scope. ``name`` is required. This follows the same
  structure as a ``setup.cfg`` file's `declarative config
  <https://setuptools.readthedocs.io/en/latest/userguide/declarative_config.html>`_
  For example:

  .. code-block::

     generate_setup = {
       metadata = {
         name = "a_nifty_package"
         version = "1.2a"
       }
       options = {
         install_requires = [ "a_pip_package" ]
       }
     }

- ``sources`` - Python sources files in the package.
- ``tests`` - Test files for this Python package.
- ``python_deps`` - Dependencies on other pw_python_packages in the GN build.
- ``python_test_deps`` - Test-only pw_python_package dependencies.
- ``other_deps`` - Dependencies on GN targets that are not pw_python_packages.
- ``inputs`` - Other files to track, such as package_data.
- ``proto_library`` - A pw_proto_library target to embed in this Python package.
  ``generate_setup`` is required in place of setup if proto_library is used. See
  :ref:`module-pw_protobuf_compiler-add-to-python-package`.
- ``static_analysis`` List of static analysis tools to run; ``"*"`` (default)
  runs all tools. The supported tools are ``"mypy"`` and ``"pylint"``.
- ``pylintrc`` - Optional path to a pylintrc configuration file to use. If not
  provided, Pylint's default rcfile search is used. Pylint is executed
  from the package's setup directory, so pylintrc files in that directory
  will take precedence over others.
- ``mypy_ini`` - Optional path to a mypy configuration file to use. If not
  provided, mypy's default configuration file search is used. mypy is
  executed from the package's setup directory, so mypy.ini files in that
  directory will take precedence over others.

Example
-------
This is an example Python package declaration for a ``pw_my_module`` module.

.. code-block::

   import("//build_overrides/pigweed.gni")

   import("$dir_pw_build/python.gni")

   pw_python_package("py") {
     setup = [
       "pyproject.toml",
       "setup.cfg",
       "setup.py",
     ]
     sources = [
       "pw_my_module/__init__.py",
       "pw_my_module/alfa.py",
       "pw_my_module/bravo.py",
       "pw_my_module/charlie.py",
     ]
     tests = [
       "alfa_test.py",
       "charlie_test.py",
     ]
     python_deps = [
       "$dir_pw_status/py",
       ":some_protos.python",
     ]
     python_test_deps = [ "$dir_pw_build/py" ]
     pylintrc = "$dir_pigweed/.pylintrc"
   }

.. _module-pw_build-pw_python_action:

pw_python_action
================
The ``pw_python_action`` template is a convenience wrapper around GN's `action
function <https://gn.googlesource.com/gn/+/main/docs/reference.md#func_action>`_
for running Python scripts. See
:bdg-ref-primary-line:`module-pw_build-python-action` in the ``pw_build``
documentation for usage.

.. _module-pw_build-pw_python_script:

pw_python_script
================
A ``pw_python_script`` represents a set of standalone Python scripts and/or
tests. These files support all of the arguments of ``pw_python_package`` except
those ``setup``. These targets can be installed, but this only installs their
dependencies.

``pw_python_script`` allows creating a
:ref:`pw_python_action <module-pw_build-python-action>` associated with the
script. To create an action, pass an ``action`` scope to ``pw_python_script``.
If there is only a single source file, it serves as the action's ``script`` by
default.

An action in ``pw_python_script`` can always be replaced with a standalone
``pw_python_action``, but using the embedded action has some advantages:

- The embedded action target bridges the gap between actions and Python targets.
  A Python script can be expressed in a single, concise GN target, rather than
  in two overlapping, dependent targets.
- The action automatically depends on the ``pw_python_script``. This ensures
  that the script's dependencies are installed and the action automatically
  reruns when the script's sources change, without needing to specify a
  dependency, a step which is easy to forget.
- Using a ``pw_python_script`` with an embedded action is a simple way to check
  an existing action's script with Pylint or Mypy or to add tests.

.. _module-pw_build-pw_python_group:

pw_python_group
===============
Represents a group of ``pw_python_package`` and ``pw_python_script`` targets.
These targets do not add any files. Their subtargets simply forward to those of
their dependencies.

.. code-block::

   pw_python_group("solar_system_python_packages") {
     python_deps = [
       "//planets/mercury/py",
       "//planets/venus/py",
       "//planets/earth/py",
       "//planets/mars/py",
       "//planets/jupiter/py",
       "//planets/saturn/py",
       "//planets/uranus/py",
       "//planets/neptune/py",
       "//planetoids/ceres/py",
       "//planetoids/pluto/py",
     ]
   }

----------------------------
Python Environment Templates
----------------------------
Templates that manage the Python build and bootstrap environment are listed
here.

- :ref:`module-pw_build-pw_python_venv`
- :ref:`module-pw_build-pw_python_pip_install`

.. _module-pw_build-pw_python_venv:

pw_python_venv
==============
Defines and creates a Python virtualenv. This template is used by Pigweed in
https://cs.pigweed.dev/pigweed/+/main:pw_env_setup/BUILD.gn to create a
virtualenv for use within the GN build that all Python actions will run in.

Example
-------
.. code-block::
   :caption: Example of a typical Python venv definition in a top level
             :octicon:`file;1em` ``BUILD.gn``

   declare_args() {
     pw_build_PYTHON_BUILD_VENV = "//:my_build_venv"
   }

   pw_python_group("my_product_packages") {
     python_deps = [
       "//product_dev_tools/py",
       "//product_release_tools/py",
     ]
   }

   pw_python_venv("my_build_venv") {
     path = "$root_build_dir/python-build-venv"
     constraints = [ "//tools/constraints.list" ]
     requirements = [ "//tools/requirements.txt" ]
     source_packages = [
       "$dir_pw_env_setup:core_pigweed_python_packages",
       "//tools:another_pw_python_package",
       "//:my_product_packages",
     ]
   }

Arguments
---------
- ``path``: The directory where the virtualenv will be created. This is relative
  to the GN root and must begin with "$root_build_dir/" if it lives in the
  output directory or "//" if it lives in elsewhere.

- ``constraints``: A list of constraint files used when performing pip install
  into this virtualenv. By default this is set to the
  ``pw_build_PIP_CONSTRAINTS`` GN arg.

- ``requirements``: A list of requirements files to install into this virtualenv
  on creation. By default this is set to the ``pw_build_PIP_REQUIREMENTS`` GN
  arg.

  .. seealso::

     For more info on the ``pw_build_PIP_CONSTRAINTS`` and
     ``pw_build_PIP_REQUIREMENTS`` GN args see:
     :ref:`docs-python-build-python-gn-requirements-files`

- ``source_packages``: A list of in-tree
  :ref:`module-pw_build-pw_python_package` or targets that will be checked for
  external third_party pip dependencies to install into this
  virtualenv. Note this list of targets isn't actually installed into the
  virtualenv. Only packages defined inside the [options] install_requires
  section of each pw_python_package's setup.cfg will be pip installed.

  .. seealso::

     For an example ``setup.cfg`` file see: `Configuring setuptools using
     setup.cfg files
     <https://setuptools.pypa.io/en/latest/userguide/declarative_config.html>`_


.. _module-pw_build-pw_python_pip_install:

pw_python_pip_install
=====================
This will pip install ``pw_python_package`` targets into the bootstrapped
developer environment.

Example
-------
.. code-block::
   :caption: Example of a typical Python venv definition in a top level
             :octicon:`file;1em` ``BUILD.gn``

   pw_python_pip_install("pip_install_my_product_packages") {
     packages = [
       "//product_dev_tools/py",
       "//product_release_tools/py",
     ]
   }

Arguments
---------

- ``packages``: A list of :ref:`module-pw_build-pw_python_package` targets to be
  pip installed.  All packages specified will be installed using a single ``pip
  install`` command with a ``--constraint`` argument for each constraint file in
  the ``pw_build_PIP_CONSTRAINTS`` GN arg.

- ``editable``: If true, --editable is passed to the pip install command.

- ``force_reinstall``: If true, ``--force-reinstall`` is passed to the pip
  install command.

.. _module-pw_build-python-dist:

------------------------------
Python Distributable Templates
------------------------------
Pigweed also provides some templates to make it easier to bundle Python packages
for deployment. These templates are found in ``pw_build/python_dist.gni``.

- :ref:`module-pw_build-pw_python_wheels`
- :ref:`module-pw_build-pw_python_zip_with_setup`
- :ref:`module-pw_build-pw_python_distribution`

.. _module-pw_build-pw_python_wheels:

pw_python_wheels
================
Collects Python wheels for one or more ``pw_python_package`` targets, plus any
additional ``pw_python_package`` targets they depend on, directly or indirectly.
Note that this does not include Python dependencies that come from outside the
GN build, like packages from PyPI, for example. Those should still be declared
in the package's ``setup.py`` file as usual.

Arguments
---------
- ``packages`` - List of ``pw_python_package`` targets whose wheels should be
  included; their dependencies will be pulled in as wheels also.
- ``directory`` - Output directory for the collected wheels. Defaults to
  ``$target_out_dir/$target_name``.

Wheel collection under the hood
-------------------------------
The ``.wheel`` subtarget of every ``pw_python_package`` generates a wheel
(``.whl``) for the Python package. The ``pw_python_wheels`` template figures
out which wheels to collect by traversing the ``pw_python_package_wheels``
`GN metadata
<https://gn.googlesource.com/gn/+/HEAD/docs/reference.md#var_metadata>`_ key,
which lists the output directory for each wheel.

.. _module-pw_build-pw_python_zip_with_setup:

pw_python_zip_with_setup
========================
Generates a ``.zip`` archive suitable for deployment outside of the project's
developer environment. The generated ``.zip`` contains Python wheels
(``.whl`` files) for one or more ``pw_python_package`` targets, plus wheels for
any additional ``pw_python_package`` targets in the GN build they depend on,
directly or indirectly. Dependencies from outside the GN build, such as packages
from PyPI, must be listed in packages' ``setup.py`` or ``setup.cfg`` files as
usual.

The ``.zip`` also includes simple setup scripts for Linux,
MacOS, and Windows. The setup scripts automatically create a Python virtual
environment and install the whole collection of wheels into it using ``pip``.

Optionally, additional files and directories can be included in the archive.
One common example of an additional file to include is a README file with setup
and usage instructions for the distributable. A simple ready-to-use README file
is available at ``pw_build/py_dist/README.md``.

Arguments
---------
- ``packages`` - A list of `pw_python_package` targets whose wheels should be
  included; their dependencies will be pulled in as wheels also.
- ``inputs`` - An optional list of extra files to include in the generated
  ``.zip``, formatted the same way as the ``inputs`` argument to ``pw_zip``
  targets.
- ``dirs`` - An optional list of directories to include in the generated
  ``.zip``, formatted the same was as the ``dirs`` argument to ``pw_zip``
  targets.

Example
-------

.. code-block::

   import("//build_overrides/pigweed.gni")

   import("$dir_pw_build/python_dist.gni")

   pw_python_zip_with_setup("my_tools") {
     packages = [ ":some_python_package" ]
     inputs = [ "$dir_pw_build/python_dist/README.md > /${target_name}/" ]
   }

.. _module-pw_build-pw_python_distribution:

pw_python_distribution
======================
Generates a directory of Python packages from source files suitable for
deployment outside of the project developer environment. The resulting directory
contains only files mentioned in each package's ``setup.cfg`` file. This is
useful for bundling multiple Python packages up into a single package for
distribution to other locations like `<http://pypi.org>`_.

Arguments
---------

- ``packages`` - A list of :ref:`module-pw_build-pw_python_package` targets to be installed into
  the build directory. Their dependencies will be pulled in as wheels also.

- ``include_tests`` - If true, copy Python package tests to a ``tests`` subdir.

- ``extra_files`` - A list of extra files that should be included in the output.
  The format of each item in this list follows this convention:

  .. code-block:: text

     //some/nested/source_file > nested/destination_file

  - Source and destination file should be separated by ``>``.

  - The source file should be a GN target label (starting with ``//``).

  - The destination file will be relative to the generated output
    directory. Parent directories are automatically created for each file. If a
    file would be overwritten an error is raised.

- ``generate_setup_cfg`` - If included, create a merged ``setup.cfg`` for all
  python Packages using either a ``common_config_file`` as a base or ``name``
  and ``version`` strings. The ``common_config_file`` should contain the
  required fields in the ``metadata`` and ``options`` sections as shown in
  `Configuring setup() using setup.cfg files <https://setuptools.pypa.io/en/latest/userguide/declarative_config.html>`_.
  ``append_git_sha_to_version = true`` and ``append_date_to_version = true``
  will optionally append the current git SHA or date to the package version
  string after a ``+`` sign. You can also opt to include a generated
  ``pyproject.toml`` file by setting ``include_default_pyproject_file = true``.

  .. code-block::
     :caption: :octicon:`file;1em` Example using a common setup.cfg and
               pyproject.toml files.

     generate_setup_cfg = {
       common_config_file = "pypi_common_setup.cfg"
       append_date_to_version = true
     }
     extra_files = [
       "//source/pyproject.toml > pyproject.toml"
     ]

  .. code-block::
     :caption: :octicon:`file;1em` Example using name and version strings and a
               default pyproject.toml file.

     generate_setup_cfg = {
       name = "awesome"
       version = "1.0.0"
       include_default_pyproject_file = true
       append_date_to_version = true
     }

Using this template will create an additional target for and building a Python
wheel. For example if you define ``pw_python_distribution("awesome")`` the
resulting targets that get created will be:

- ``awesome`` - This will create the merged package with all source files in
  place in the out directory under ``out/obj/awesome/``.
- ``awesome.wheel`` - This builds a Python wheel from the above source files
  under ``out/obj/awesome._build_wheel/awesome*.whl``.

Example
-------

.. code-block::
   :caption: :octicon:`file;1em` ./pw_env_setup/BUILD.gn

   import("//build_overrides/pigweed.gni")

   import("$dir_pw_build/python_dist.gni")

   pw_python_distribution("build_python_source_tree") {
     packages = [
       ":some_python_package",
       ":another_python_package",
     ]
     include_tests = true
     extra_files = [
       "//README.md > ./README.md",
       "//some_python_package/py/BUILD.bazel > some_python_package/BUILD.bazel",
       "//another_python_package/py/BUILD.bazel > another_python_package/BUILD.bazel",
     ]
     generate_setup_cfg = {
       common_config_file = "pypi_common_setup.cfg"
       append_git_sha_to_version = true
       append_date_to_version = true
     }
   }


.. code-block:: text
   :caption: :octicon:`file-directory;1em`
             ./out/obj/pw_env_setup/build_python_source_tree/

   $ tree ./out/obj/pw_env_setup/build_python_source_tree/
   ├── README.md
   ├── setup.cfg
   ├── some_python_package
   │   ├── BUILD.bazel
   │   ├── __init__.py
   │   ├── py.typed
   │   ├── some_source_file.py
   │   └── tests
   │       └── some_source_test.py
   └── another_python_package
       ├── BUILD.bazel
       ├── __init__.py
       ├── another_source_file.py
       ├── py.typed
       └── tests
           └── another_source_test.py
