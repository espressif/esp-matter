.. _module-pw_package:

==========
pw_package
==========
The package module provides a mechanism to install additional tools used by
Pigweed. Most Pigweed dependencies should be installed using
:ref:`module-pw_env_setup`. Examples of reasons packages should be managed using
this module instead are listed below.

* The dependency is extremely large and not commonly used.
* The dependency has a number of compatible versions and we want to allow
  downstream projects to pick a version rather than being forced to use ours.
* The dependency has license issues that make it complicated for Google to
  include it directly as a submodule or distribute it as a CIPD package.
* The dependency needs to be "installed" into the system in some manner beyond
  just extraction and thus isn't a good match for distribution with CIPD.

Pigweed itself includes a number of packages that simply clone git repositories.
In general, these should not be used by projects using Pigweed. Pigweed uses
these packages to avoid using submodules so downstream projects don't have
multiple copies of a given repository in their source tree. Projects using
Pigweed should use submodules instead of packages because submodules are
supported by much more mature tooling: git.

-----
Usage
-----
The package module can be accessed through the ``pw package`` command. This
has several subcommands.

``pw package list``
  Lists all the packages installed followed by all the packages available.

``pw package install <package-name>``
  Installs ``<package-name>``. Exactly how this works is package-dependent,
  and packages can decide to do nothing because the package is current, do an
  incremental update, or delete the current version and install anew. Use
  ``--force`` to remove the package before installing.

``pw package status <package-name>``
  Indicates whether ``<package-name>`` is installed.

``pw package remove <package-name>``
  Removes ``<package-name>``.

By default ``pw package`` operates on the directory referenced by
``PW_PACKAGE_ROOT``.

-----------
Configuring
-----------

Compatibility
~~~~~~~~~~~~~
Python 3

Adding a New Package
~~~~~~~~~~~~~~~~~~~~
To add a new package create a class that subclasses ``Package`` from
``pw_package/package_manager.py``.

.. code-block:: python

  class Package:
      """Package to be installed.

      Subclass this to implement installation of a specific package.
      """
      def __init__(self, name):
          self._name = name

      @property
      def name(self):
          return self._name

      def install(self, path: pathlib.Path) -> None:
          """Install the package at path.

          Install the package in path. Cannot assume this directory is empty—it
          may need to be deleted or updated.
          """

      def remove(self, path: pathlib.Path) -> None:
          """Remove the package from path.

          Removes the directory containing the package. For most packages this
          should be sufficient to remove the package, and subclasses should not
          need to override this package.
          """
          if os.path.exists(path):
              shutil.rmtree(path)

      def status(self, path: pathlib.Path) -> bool:
          """Returns if package is installed at path and current.

          This method will be skipped if the directory does not exist.
          """

There's also a helper class for retrieving specific revisions of Git
repositories in ``pw_package/git_repo.py``.

Then call ``pw_package.package_manager.register(PackageClass)`` to register
the class with the package manager.

Setting up a Project
~~~~~~~~~~~~~~~~~~~~
To set up the package manager for a new project create a file like below and
add it to the ``PW_PLUGINS`` file (see :ref:`module-pw_cli` for details). This
file is based off of ``pw_package/pigweed_packages.py``.

.. code-block:: python

  from pw_package import package_manager
  # These modules register themselves so must be imported despite appearing
  # unused.
  from pw_package.packages import nanopb

  def main(argv=None) -> int:
      return package_manager.run(**vars(package_manager.parse_args(argv)))
