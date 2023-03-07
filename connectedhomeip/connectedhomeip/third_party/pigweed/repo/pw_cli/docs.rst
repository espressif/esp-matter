.. _module-pw_cli:

------
pw_cli
------
This directory contains the ``pw`` command line interface (CLI) that facilitates
working with Pigweed. The CLI module adds several subcommands prefixed with
``pw``, and provides a mechanism for other Pigweed modules to behave as
"plugins" and register themselves as ``pw`` commands as well. After activating
the Pigweed environment, these commands will be available for use.

``pw`` includes the following commands by default:

.. code-block:: text

  doctor   Check that the environment is set up correctly for Pigweed.
  format   Check and fix formatting for source files.
  help     Display detailed information about pw commands.
  ide      Configure editors and IDEs to work best with Pigweed.
  logdemo  Show how logs look at various levels.
  module   Utilities for managing modules.
  test     Run Pigweed unit tests built using GN.
  watch    Watch files for changes and rebuild.

To see an up-to-date list of ``pw`` subcommands, run ``pw --help``.

Invoking  ``pw``
================
``pw`` subcommands are invoked by providing the command name. Arguments prior to
the command are interpreted by ``pw`` itself; all arguments after the command
name are interpreted by the command.

Here are some example invocations of ``pw``:

.. code-block:: text

  # Run the doctor command
  $ pw doctor

  # Run format --fix with debug-level logs
  $ pw --loglevel debug format --fix

  # Display help for the pw command
  $ pw -h watch

  # Display help for the watch command
  $ pw watch -h

Registering ``pw`` plugins
==========================
Projects can register their own Python scripts as ``pw`` commands. ``pw``
plugins are registered by providing the command name, module, and function in a
``PW_PLUGINS`` file. ``PW_PLUGINS`` files can add new commands or override
built-in commands. Since they are accessed by module name, plugins must be
defined in Python packages that are installed in the Pigweed virtual
environment.

Plugin registrations in a ``PW_PLUGINS`` file apply to the their directory and
all subdirectories, similarly to configuration files like ``.clang-format``.
Registered plugins appear as commands in the ``pw`` tool when ``pw`` is run from
those directories.

Projects that wish to register commands might place a ``PW_PLUGINS`` file in the
root of their repo. Multiple ``PW_PLUGINS`` files may be applied, but the ``pw``
tool gives precedence to a ``PW_PLUGINS`` file in the current working directory
or the nearest parent directory.

PW_PLUGINS file format
----------------------
``PW_PLUGINS`` contains one plugin entry per line in the following format:

.. code-block:: python

  # Lines that start with a # are ignored.
  <command name> <Python module> <function>

The following example registers three commands:

.. code-block:: python

  # Register the presubmit script as pw presubmit
  presubmit my_cool_project.tools run_presubmit

  # Override the pw test command with a custom version
  test my_cool_project.testing run_test

  # Add a custom command
  flash my_cool_project.flash main

Defining a plugin function
--------------------------
Any function without required arguments may be used as a plugin function. The
function should return an int, which the ``pw`` uses as the exit code. The
``pw`` tool uses the function docstring as the help string for the command.

Typically, ``pw`` commands parse their arguments with the ``argparse`` module.
``pw`` sets ``sys.argv`` so it contains only the arguments for the plugin,
so plugins can behave the same whether they are executed independently or
through ``pw``.

Example
^^^^^^^
This example shows a function that is registered as a ``pw`` plugin.

.. code-block:: python

  # my_package/my_module.py

  def _do_something(device):
      ...

  def main() -> int:
      """Do something to a connected device."""

      parser = argparse.ArgumentParser(description=__doc__)
      parser.add_argument('--device', help='Set which device to target')
      return _do_something(**vars(parser.parse_args()))


  if __name__ == '__main__':
      logging.basicConfig(format='%(message)s', level=logging.INFO)
      sys.exit(main())

This plugin is registered in a ``PW_PLUGINS`` file in the current working
directory or a parent of it.

.. code-block:: python

  # Register my_commmand
  my_command my_package.my_module main

The function is now available through the ``pw`` command, and will be listed in
``pw``'s help. Arguments after the command name are passed to the plugin.

.. code-block:: text

  $ pw

   ▒█████▄   █▓  ▄███▒  ▒█    ▒█ ░▓████▒ ░▓████▒ ▒▓████▄
    ▒█░  █░ ░█▒ ██▒ ▀█▒ ▒█░ █ ▒█  ▒█   ▀  ▒█   ▀  ▒█  ▀█▌
    ▒█▄▄▄█░ ░█▒ █▓░ ▄▄░ ▒█░ █ ▒█  ▒███    ▒███    ░█   █▌
    ▒█▀     ░█░ ▓█   █▓ ░█░ █ ▒█  ▒█   ▄  ▒█   ▄  ░█  ▄█▌
    ▒█      ░█░ ░▓███▀   ▒█▓▀▓█░ ░▓████▒ ░▓████▒ ▒▓████▀

  usage: pw [-h] [-C DIRECTORY] [-l LOGLEVEL] [--no-banner] [command] ...

  The Pigweed command line interface (CLI).

  ...

  supported commands:
    doctor        Check that the environment is set up correctly for Pigweed.
    format        Check and fix formatting for source files.
    help          Display detailed information about pw commands.
    ...
    my_command    Do something to a connected device.

  $ pw my_command -h

   ▒█████▄   █▓  ▄███▒  ▒█    ▒█ ░▓████▒ ░▓████▒ ▒▓████▄
    ▒█░  █░ ░█▒ ██▒ ▀█▒ ▒█░ █ ▒█  ▒█   ▀  ▒█   ▀  ▒█  ▀█▌
    ▒█▄▄▄█░ ░█▒ █▓░ ▄▄░ ▒█░ █ ▒█  ▒███    ▒███    ░█   █▌
    ▒█▀     ░█░ ▓█   █▓ ░█░ █ ▒█  ▒█   ▄  ▒█   ▄  ░█  ▄█▌
    ▒█      ░█░ ░▓███▀   ▒█▓▀▓█░ ░▓████▒ ░▓████▒ ▒▓████▀

  usage: pw my_command [-h] [--device DEVICE]

  Do something to a connected device.

  optional arguments:
    -h, --help       show this help message and exit
    --device DEVICE  Set which device to target

Branding Pigweed's tooling
==========================
An important part of starting a new project is picking a name, and in the case
of Pigweed, designing a banner for the project. Pigweed supports configuring
the banners by setting environment variables:

* ``PW_BRANDING_BANNER`` - Absolute path to a filename containing a banner to
  display when running the ``pw`` commands. See the example below.
* ``PW_BRANDING_BANNER_COLOR`` - Color of the banner. Possible values include:
  ``red``, ``bold_red``, ``yellow``, ``bold_yellow``, ``green``,
  ``bold_green``, ``blue``, ``cyan``, ``magenta``, ``bold_white``,
  ``black_on_white``. See ``pw_cli.colors`` for details.

The below example shows how to manually change the branding at the command
line. However, these environment variables should be set in the project root's
``bootstrap.sh`` before delegating to Pigweed's upstream ``bootstrap.sh``.

.. code-block:: text

  $ cat foo-banner.txt

   ▒██████  ░▓██▓░  ░▓██▓░
    ▒█░    ▒█   ▒█ ▒█   ▒█
    ▒█▄▄▄▄ ▒█ █ ▒█ ▒█ █ ▒█
    ▒█▀    ▒█   ▒█ ▒█   ▒█
    ▒█      ░▓██▓░  ░▓██▓░

  $ export PW_BRANDING_BANNER="$(pwd)/foo-banner.txt"
  $ export PW_BRANDING_BANNER_COLOR="bold_red"
  $ pw logdemo

   ▒██████  ░▓██▓░  ░▓██▓░
    ▒█░    ▒█   ▒█ ▒█   ▒█
    ▒█▄▄▄▄ ▒█ █ ▒█ ▒█ █ ▒█
    ▒█▀    ▒█   ▒█ ▒█   ▒█
    ▒█      ░▓██▓░  ░▓██▓░

  20200610 12:03:44 CRT This is a critical message
  20200610 12:03:44 ERR There was an error on our last operation
  20200610 12:03:44 WRN Looks like something is amiss; consider investigating
  20200610 12:03:44 INF The operation went as expected
  20200610 12:03:44 OUT Standard output of subprocess

The branding is not purely visual; it serves to make it clear which project an
engineer is working with.

Making the ASCII / ANSI art
---------------------------
The most direct way to make the ASCII art is to create it with a text editor.
However, there are some tools to make the process faster and easier.

* `Patorjk's ASCII art generator <http://patorjk.com/software/taag/>`_ - A
  great starting place, since you can copy and paste straight from the browser
  into a file, and then point ``PW_BRANDING_BANNER`` at it.  Most of the fonts
  use normal ASCII characters; and fonts with extended ASCII characters use the
  Unicode versions of them (needed for modern terminals).

There are other options, but these require additional work to put into Pigweed
since they only export in the traditional ANS or ICE formats. The old ANS
formats do not have a converter (contributions welcome!). Here are some of the
options as of mid-2020:

* `Playscii <http://vectorpoem.com/playscii/>`_ - Actively maintained.
* `Moebius <https://github.com/blocktronics/moebius>`_ - Actively maintained.
* `SyncDraw <http://syncdraw.bbsdev.net/>`_ - Actively maintained, in 2020, in
  a CVS repository.
* `PabloDraw <http://picoe.ca/products/pablodraw/>`_ - Works on most desktop
  machines thanks to being written in .NET. Not maintained, but works well. Has
  an impresive brush system for organic style drawing.
* `TheDraw <https://en.wikipedia.org/wiki/TheDraw>`_ - One of the most popular
  ANSI art editors back in the 90s. Requires DOSBox to run on modern machines,
  but otherwise works. It has some of the most impressive capabilities,
  including supporting full-color multi-character fonts.

Future branding improvements
----------------------------
Branding the ``pw`` tool is a great start, but more changes are planned:

- Supporting branding the ``bootstrap/activate`` banner, which for technical
  reasons is not the same code as the banner printing from the Python tooling.
  These will use the same ``PW_BRANDING_BANNER`` and
  ``PW_BRANDING_BANNER_COLOR`` environment variables.
- Supporting renaming the ``pw`` command to something project specific, like
  ``foo`` in this case.
- Re-coloring the log headers from the ``pw`` tool.

pw_cli Python package
=====================
The ``pw_cli`` Pigweed module includes the ``pw_cli`` Python package, which
provides utilities for creating command line tools with Pigweed.

pw_cli.log
----------
.. automodule:: pw_cli.log
  :members:

pw_cli.plugins
--------------
:py:mod:`pw_cli.plugins` provides general purpose plugin functionality. The
module can be used to create plugins for command line tools, interactive
consoles, or anything else. Pigweed's ``pw`` command uses this module for its
plugins.

To use plugins, create a :py:class:`pw_cli.plugins.Registry`. The registry may
have an optional validator function that checks plugins before they are
registered (see :py:meth:`pw_cli.plugins.Registry.__init__`).

Plugins may be registered in a few different ways.

 * **Direct function call.** Register plugins by calling
   :py:meth:`pw_cli.plugins.Registry.register` or
   :py:meth:`pw_cli.plugins.Registry.register_by_name`.

   .. code-block:: python

     registry = pw_cli.plugins.Registry()

     registry.register('plugin_name', my_plugin)
     registry.register_by_name('plugin_name', 'module_name', 'function_name')

 * **Decorator.** Register using the :py:meth:`pw_cli.plugins.Registry.plugin`
   decorator.

   .. code-block:: python

     _REGISTRY = pw_cli.plugins.Registry()

     # This function is registered as the "my_plugin" plugin.
     @_REGISTRY.plugin
     def my_plugin():
         pass

     # This function is registered as the "input" plugin.
     @_REGISTRY.plugin(name='input')
     def read_something():
         pass

   The decorator may be aliased to give a cleaner syntax (e.g. ``register =
   my_registry.plugin``).

 * **Plugins files.** Plugins files use a simple format:

   .. code-block::

     # Comments start with "#". Blank lines are ignored.
     name_of_the_plugin module.name module_member

     another_plugin some_module some_function

   These files are placed in the file system and apply similarly to Git's
   ``.gitignore`` files. From Python, these files are registered using
   :py:meth:`pw_cli.plugins.Registry.register_file` and
   :py:meth:`pw_cli.plugins.Registry.register_directory`.

pw_cli.plugins module reference
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. automodule:: pw_cli.plugins
  :members:
