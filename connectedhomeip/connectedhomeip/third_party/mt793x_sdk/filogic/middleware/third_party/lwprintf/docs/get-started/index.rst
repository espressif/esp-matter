.. _getting_started:

Getting started
===============

Getting started may be the most challenging part of every new library.
This guide is describing how to start with the library quickly and effectively

.. _download_library:

Download library
^^^^^^^^^^^^^^^^

Library is primarly hosted on `Github <https://github.com/MaJerle/lwprintf>`_.

You can get it with:

* Downloading latest release from `releases area <https://github.com/MaJerle/lwprintf/releases>`_ on Github
* Cloning ``master`` branch for latest stable version
* Cloning ``develop`` branch for latest development

Download from releases
**********************

All releases are available on Github `releases area <https://github.com/MaJerle/lwprintf/releases>`_.

Clone from Github
*****************

First-time clone
""""""""""""""""

This is used when you do not have yet local copy on your machine.

* Make sure ``git`` is installed.
* Open console and navigate to path in the system to clone repository to. Use command ``cd your_path``
* Clone repository with one of available ``3`` options

  * Run ``git clone --recurse-submodules https://github.com/MaJerle/lwprintf`` command to clone entire repository, including submodules
  * Run ``git clone --recurse-submodules --branch develop https://github.com/MaJerle/lwprintf`` to clone `development` branch, including submodules
  * Run ``git clone --recurse-submodules --branch master https://github.com/MaJerle/lwprintf`` to clone `latest stable` branch, including submodules

* Navigate to ``examples`` directory and run favourite example

Update cloned to latest version
"""""""""""""""""""""""""""""""

* Open console and navigate to path in the system where your resources repository is. Use command ``cd your_path``
* Run ``git pull origin master --recurse-submodules`` command to pull latest changes and to fetch latest changes from submodules on ``master`` branch
* Run ``git pull origin develop --recurse-submodules`` command to pull latest changes and to fetch latest changes from submodules on ``develop`` branch
* Run ``git submodule foreach git pull origin master`` to update & merge all submodules

.. note::
  This is preferred option to use when you want to evaluate library and run prepared examples.
  Repository consists of multiple submodules which can be automatically downloaded when cloning and pulling changes from root repository.

Add library to project
^^^^^^^^^^^^^^^^^^^^^^

At this point it is assumed that you have successfully download library, either cloned it or from releases page.
Next step is to add the library to the project, by means of source files to compiler inputs and header files in search path

* Copy ``lwprintf`` folder to your project, it contains library files
* Add ``lwprintf/src/include`` folder to `include path` of your toolchain. This is where `C/C++` compiler can find the files during compilation process. Usually using ``-I`` flag
* Add source files from ``lwprintf/src/`` folder to toolchain build. These files are built by `C/C++` compiler
* Copy ``lwprintf/src/include/lwprintf/lwprintf_opts_template.h`` to project folder and rename it to ``lwprintf_opts.h``
* Build the project

Configuration file
^^^^^^^^^^^^^^^^^^

Configuration file is used to overwrite default settings defined for the essential use case.
Library comes with template config file, which can be modified according to needs.
and it should be copied (or simply renamed in-place) and named ``lwprintf_opts.h``

.. note::
    Default configuration template file location: ``lwprintf/src/include/lwprintf/lwprintf_opts_template.h``.
    File must be renamed to ``lwprintf_opts.h`` first and then copied to the project directory where compiler
    include paths have access to it by using ``#include "lwprintf_opts.h"``.

List of configuration options are available in the :ref:`api_lwprintf_opt` section.
If any option is about to be modified, it should be done in configuration file

.. literalinclude:: ../../lwprintf/src/include/lwprintf/lwprintf_opts_template.h
    :language: c
    :linenos:
    :caption: Template configuration file

.. note::
    If you prefer to avoid using configuration file, application must define
    a global symbol ``LWPRINTF_IGNORE_USER_OPTS``, visible across entire application.
    This can be achieved with ``-D`` compiler option.

Minimal example code
^^^^^^^^^^^^^^^^^^^^

To verify proper library setup, minimal example has been prepared.
Run it in your main application file to verify its proper execution

.. literalinclude:: ../examples_src/example_minimal.c
    :language: c
    :linenos:
    :caption: Absolute minimum example