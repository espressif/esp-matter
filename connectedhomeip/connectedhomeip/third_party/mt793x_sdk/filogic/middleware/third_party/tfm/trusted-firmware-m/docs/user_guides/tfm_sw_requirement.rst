#####################
Software requirements
#####################

.. |DS5_VERSION| replace:: v5.29.1
.. |KEIL_VERSION| replace:: v5.25.2
.. |DEV_STUDIO_VERSION| replace:: 2018.0

A quick reference table is included in the
:ref:`docs/user_guides/tfm_sw_requirement:Tool & Dependency overview` section
of this document.

****************************
Supported build environments
****************************

TF-M officially supports a limited set of build environments and setups. In
this context, official support means that the environments listed below
are actively used by team members and active developers hence users should
be able to recreate the same configurations by following the instructions
described below. In case of problems, the TF-M team provides support
only for these environments, but building in other environments can still be
possible.

The following environments are supported:

    - Ubuntu 16.04 x64
    - Ubuntu 18.04 x64
    - Windows 10 x64 + git-bash (MinGW) + gnumake from DS-5 or msys2.
    - Windows 10 x64 + Cygwin x64 (example configuration is provided for
      this Windows setup only).

.. note::
    Some tools (i.e. python3 and CMake) must NOT be installed from
    Cygwin and instead a native windows version is needed. Please see the
    chapter `Windows + Cygwin setup`_ below.

*********************
Supported C compilers
*********************

To compile TF-M code, at least one of the supported compiler toolchains have to
be available in the build environment. The currently supported compiler
versions are:

    - Arm Compiler v6.10
    - Arm Compiler v6.11
    - Arm Compiler v6.12
    - Arm Compiler v6.13
    - GNU Arm compiler v6.3.1
    - GNU Arm compiler v7.3

.. Note::
    - The Arm compilers above are provided via Keil uVision |KEIL_VERSION|
      or greater, DS-5 |DS5_VERSION| or greater, and Development Studio
      |DEV_STUDIO_VERSION| or greater, or they can be downloaded as standalone
      packages from
      `here <https://developer.arm.com/products/software-development-tools/compilers/arm-compiler/downloads/version-6>`__.

    - Arm compiler specific environment variable may need updating based
      on specific products and licenses as explained in
      `product-and-toolkit-configuration <https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration>`__.

    - The GNU Arm compiler can be downloaded from
      `here <https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads>`__.
      On the page select *GNU Arm Embedded Toolchain: 6-2017-q1-update*
      or *GNU Arm Embedded Toolchain: 7-2018-q2-update*

************************
Supported CMake versions
************************

The build-system is CMake based and supports the following versions:

    - 3.7
    - 3.10
    - 3.11
    - 3.12
    - 3.13
    - 3.14

.. Note::
    - Please use the latest build version available (i.e. 3.7.2 instead of
      3.7.0).
      While it is preferable to use the newest version this is not required
      and any version from the above list should work.
    - Recent versions of CMake can be downloaded from
      https://cmake.org/download/, and older releases are available from
      https://cmake.org/files.
    - For Cygwin users, please use a native windows CMake version
      instead of the version installed with Cygwin.

***************************
Supported GNU make versions
***************************

The TF-M team builds using the "Unix Makefiles" generator of CMake and
thus GNU make is needed for the build. On Linux please use the version
available from the official repository of your distribution.

On Windows the following binaries are supported:

    - GNU make v4.2.1 executable from Cygwin
    - GNU make v4.2.1 executable from msys2
    - GNU make v4.2 executable from DS5 |DS5_VERSION| (see <DS5 directory>/bin)

CMake is quiet tolerant to GNU make versions and basically any
"reasonably recent" GNU make version shall work.

CMake generators other than "Unix Makefiles" may work, but are not
officially supported.

*********************
External dependencies
*********************

In order to build TF-M the following external projects are required:

    - `Mbed Crypto library <https://github.com/ARMmbed/mbed-crypto>`__ v3.0.1
      is used as crypto library on the secure side
    - `CMSIS Version 5 <https://github.com/ARM-software/CMSIS_5>`__ v5.5.0
      is used to import RTX for the example non-secure app

Each of the listed dependencies should be placed in a common root directory
with trustedfirmware-m

.. code-block:: bash

    .
    ├── CMSIS_5
    ├── mbed-crypto
    └── trusted-firmware-m

.. Note::
   `CMSIS_5` v5.5.0 has a known issue, affecting compilation using
   Arm Compiler v6.10. Please refer to
   `CMSIS release notes <https://github.com/ARM-software/CMSIS_5/releases>`__.
   for guidance.

**************
Example setups
**************

This section lists dependencies and some exact and tested steps to set-up a
TF-M-m build environment under various OSes.

Ubuntu setup
============

Install the following tools:

- DS-5 |DS5_VERSION|.
- Git tools v2.10.0
- Git Large File Storage (git-lfs)
- CMake (see the "Supported CMake versions" chapter)
- GNU Make (see the "Supported make versions" chapter)
- Python3 and the pip package manager (from Python 3.4 it's included)
- Python3 packages: *cryptography, pyasn1, yaml, jinja2 v2.10, cbor v1.0.0*

  .. code-block:: bash

    pip3 install --user cryptography pyasn1 pyyaml jinja2 cbor

- SRecord v1.58 (for Musca test chip boards)

Setup a shell to enable compiler toolchain and CMake after installation.
------------------------------------------------------------------------

To import Arm Compiler v6.10 in your bash shell console:

.. Warning::
    Arm compiler specific environment variable may need updating based on
    specific products and licenses as explained in
    `product-and-toolkit-configuration <https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration>`__.

.. code-block:: bash

    export PATH=<DS-5_PATH>/sw/ARMCompiler6.10/bin:$PATH
    export ARM_TOOL_VARIANT=ult
    export ARM_PRODUCT_PATH=<DS-5_PATH>/sw/mappings
    export ARMLMD_LICENSE_FILE=<LICENSE_FILE_PATH>

To import CMake in your bash shell console:

.. code-block:: bash

    export PATH=/bin:$PATH

To import GNU Arm in your bash shell console:

.. code-block:: bash

    export PATH=/bin:$PATH

Windows + Cygwin setup
======================

Install the following tools:

- uVision |KEIL_VERSION| or DS-5 |DS5_VERSION| (DS-5 Ultimate Edition) which
  provides the Arm Compiler v6.10 compiler or GNU Arm compiler v6.3.1.
- Git client latest version (https://git-scm.com/download/win)
- Git Large File Storage (`native Windows version <https://git-lfs.github.com/>`__,
- CMake (`native Windows version <https://cmake.org/download/>`__,
  see the `Supported CMake versions`_ chapter)
- `Cygwin <https://www.cygwin.com/>`__. Tests done with version 2.877
  (64 bits)
- GNU make should be installed by selecting appropriate package during
  cygwin
  installation.
- Python3 `(native Windows version) <https://www.python.org/downloads/>`__ and
  the pip package manager (from Python 3.4 it's included)
- Python3 packages: *cryptography, pyasn1, yaml, jinja2 v2.10, cbor v1.0.0*

  .. code-block:: bash

    pip3 install --user cryptography pyasn1 pyyaml jinja2 cbor

- `SRecord v1.63 <https://sourceforge.net/projects/srecord/>`__ (for Musca test
  chip boards)

Setup Cygwin to enable a compiler toolchain and CMake after installation.
-------------------------------------------------------------------------

If applicable, import Arm Compiler v6.10 in your shell console. To make this
change permanent, add the command line into ~/.bashrc

Armclang + DS-5
^^^^^^^^^^^^^^^
.. Note::

    - Arm compiler specific environment variable may need updating based on
      specific products and licenses as explained in
      `product-and-toolkit-configuration <https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration>`__.
    - Arm licensing related environment variables must use Windows paths, and not
      the Cygwin specific one relative to */cygrive*.

.. code-block:: bash

    export PATH=/cygdrive/c/<DS-5_PATH>/sw/ARMCompiler6.10/bin:$PATH
    export ARM_PRODUCT_PATH=C:/<DS-5_PATH>/sw/mappings
    export ARM_TOOL_VARIANT=ult
    export ARMLMD_LICENSE_FILE=<LICENSE_FILE_PATH>

Armclang + Keil MDK Arm
^^^^^^^^^^^^^^^^^^^^^^^

.. Note::

    - Arm compiler specific environment variable may need updating based
      on specific products and licenses as explained in
      `product-and-toolkit-configuration <https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration>`__.

.. code-block:: bash

    export PATH=/cygdrive/c/<uVision path>/ARM/ARMCLANG/bin:$PATH

GNU Arm
^^^^^^^

If applicable, import GNU Arm compiler v6.3.1 in your shell console. To make
this change permanent, add the command line into ~/.bashrc

.. code-block:: bash

    export PATH=<GNU Arm path>/bin:$PATH

CMake
^^^^^

To import CMake in your bash shell console:

.. code-block:: bash

    export PATH=/cygdrive/c/<CMake path>/bin:$PATH

Building the documentation
==========================

The build system is prepared to support generation of two documents.
The Reference Manual which is Doxygen based, and the User Guide which is
Sphinx based. Both document can be generated in HTML and PDF format.

.. Note::

    Support for document generation in the build environment is not mandatory.
    Missing document generation tools will not block building the TF-M
    firmware.

To compile the TF-M Reference Manual
------------------------------------

The following additional tools are needed:

    - Doxygen v1.8.0 or later
    - Graphviz dot v2.38.0 or later
    - PlantUML v1.2018.11 or later
    - Java runtime environment 1.8 or later (for running PlantUML)

For PDF generation the following tools are needed in addition to the
above list:

    - LaTeX
    - PdfLaTeX

Set-up the needed tools
^^^^^^^^^^^^^^^^^^^^^^^

Linux
"""""
.. code-block:: bash

    sudo apt-get install -y doxygen graphviz default-jre
    mkdir ~/plantuml
    curl -L http://sourceforge.net/projects/plantuml/files/plantuml.jar/download --output ~/plantuml/plantuml.jar

For PDF generation:

.. code-block:: bash

    sudo apt-get install -y doxygen-latex

Windows + Cygwin
""""""""""""""""

Download and install the following tools:
    - `Doxygen
      1.8.8 <https://sourceforge.net/projects/doxygen/files/snapshots/doxygen-1.8-svn/windows/doxygenw20140924_1_8_8.zip/download>`__
    - `Graphviz
      2.38 <https://graphviz.gitlab.io/_pages/Download/windows/graphviz-2.38.msi>`__
    - The Java runtime is part of the DS5 installation or can be
      `downloaded from here <https://www.java.com/en/download/>`__
    - `PlantUML <http://sourceforge.net/projects/plantuml/files/plantuml.jar/download>`__

For PDF generation:

    -  `MikTeX <https://miktex.org/download>`__

    .. Note::
        When building the documentation the first time, MikTeX might prompt for
        installing missing LaTeX components. Please allow the MikTeX package
        manager to set-up these.

Configure the shell
^^^^^^^^^^^^^^^^^^^

Linux
"""""

::

    export PLANTUML_JAR_PATH=~/plantuml/plantuml.jar

Windows + Cygwin
""""""""""""""""

Assumptions for the settings below:

    - plantuml.jar is available at c:\\plantuml\\plantuml.jar
    - doxygen, dot, and MikTeX binaries are available on the PATH.
    - Java JVM is used from DS5 installation.

::

    export PLANTUML_JAR_PATH=c:/plantuml/plantuml.jar
    export PATH=$PATH:/cygdrive/c/<DS-5 path>/sw/java/bin

To compile the TF-M User Guide
------------------------------

The following additional tools are needed:

    - Python3 and the following modules:
    - Sphinx v1.7.9
    - m2r v0.2.0
    - sphinxcontrib-plantuml
    - sphinx-rtd-theme
    - Graphviz dot v2.38.0 or later
    - PlantUML v1.2018.11 or later
    - Java runtime environment 1.8 or later (for running PlantUML)

For PDF generation the following tools are needed in addition to the
above list:

    - LaTeX
    - PdfLaTeX

Set-up the needed tools
^^^^^^^^^^^^^^^^^^^^^^^

Linux
"""""

.. code-block:: bash

    sudo apt-get install -y python3 graphviz default-jre
    pip --user install m2r Sphinx sphinx-rtd-theme sphinxcontrib-plantuml
    mkdir ~/plantuml
    curl -L http://sourceforge.net/projects/plantuml/files/plantuml.jar/download --output ~/plantuml/plantuml.jar

For PDF generation:

.. code-block:: bash

    sudo apt-get install -y doxygen-latex

Windows + Cygwin
""""""""""""""""
Download and install the following tools:
    - Python3 `(native Windows version) <https://www.python.org/downloads/>`__
    - Pip packages *m2r, Sphinx, sphinx-rtd-theme sphinxcontrib-plantuml*

      .. code-block:: bash

        pip --user install m2r Sphinx sphinx-rtd-theme sphinxcontrib-plantuml

    - `Graphviz 2.38 <https://graphviz.gitlab.io/_pages/Download/windows/graphviz-2.38.msi>`__
    - The Java runtime is part of the DS5 installation or can be
      `downloaded from here <https://www.java.com/en/download/>`__
    - `PlantUML <http://sourceforge.net/projects/plantuml/files/plantuml.jar/download>`__

For PDF generation:

-  `MikTeX <https://miktex.org/download>`__

.. Note::
     When building the documentation the first time, MikTeX might
     prompt for installing missing LaTeX components. Please allow the MikTeX
     package manager to set-up these.

Configure the shell
^^^^^^^^^^^^^^^^^^^

Linux
"""""
.. code-block:: bash

    export PLANTUML_JAR_PATH=~/plantuml/plantuml.jar

Windows + Cygwin
""""""""""""""""

Assumptions for the settings below:

    - plantuml.jar is available at c:\\plantuml\\plantuml.jar
    - doxygen, dot, and MikTeX binaries are available on the PATH.
    - Java JVM is used from DS5 installation.

.. code-block:: bash

    export PLANTUML_JAR_PATH=c:/plantuml/plantuml.jar
    export PATH=$PATH:/cygdrive/c/<DS-5 path>/sw/java/bin

**************************
Tool & Dependency overview
**************************

To build the TF-M firmware the following tools are needed:

.. csv-table:: Tool dependencies
   :header: "Name", "Version", "Component"

   "C compiler",See `Supported C compilers`_,"Firmware"
   "CMake",See `Supported CMake versions`_,
   "GNU Make",See `Supported GNU make versions`_,
   "CMSIS 5",See `External dependencies`_,
   "mbed-crypto",See `External dependencies`_,
   "Python",3.x,"Firmware, User Guide"
   "yaml",,"Firmware"
   "pyasn1",,"Firmware"
   "jinja2",,"Firmware"
   "cryptography",,"Firmware"
   "cbor",,"Firmware"
   "Doxygen",">1.8","Reference manual"
   "Sphinx",">1.4","User Guide"
   "sphinxcontrib-plantuml",,"User Guide"
   "sphinx-trd-theme",,"User Guide"
   "Git",,
   "PlantUML",">v1.2018.11","Reference Manual, User Guide"
   "Graphviz dot",">v2.38.0","Reference manual"
   "Java runtime environment (JRE)",">1.8","Reference Manual, User Guide"
   "LaTex",,"pdf version of Reference Manual and User Guide"
   "PdfLaTex",,"pdf version of Reference Manual and User Guide"

Dependency chain:

.. uml::

   @startuml
    skinparam state {
      BackgroundColor #92AEE0
      FontColor black
      FontSize 16
      AttributeFontColor black
      AttributeFontSize 16
      BackgroundColor<<pdf>> #A293E2
      BackgroundColor<<doc>> #90DED6
    }
    state fw as "Firmware" : TF-M binary
    state c_comp as "C Compiler" : C99
    state gmake as "GNU make"
    state u_guide as "User Guide" <<doc>>
    state refman as "Reference Manual" <<doc>>
    state rtd_theme as "sphinx-rtd-theme" <<doc>>
    state sphnix_puml as "sphinxcontrib-plantuml" <<doc>>
    state JRE as "JRE" <<doc>> : Java Runtime Environment
    state gwiz as "Graphwiz dot" <<doc>>
    state Sphinx as "Sphinx" <<doc>>
    state m2r as "m2r" <<doc>>
    state PlantUML as "PlantUML" <<doc>>
    state LaTex as "LaTex" <<pdf>>
    state PdfLaTex as "PdfLaTex" <<<<pdf>>>>
    state Doxygen as "Doxygen" <<doc>>

    [*] --> fw
    fw --> c_comp
    fw --> CMake
    CMake --> gmake
    fw --> cryptography
    fw --> pyasn1
    fw --> yaml
    fw --> jinja2
    fw --> cbor
    cryptography --> Python3
    pyasn1 --> Python3
    yaml --> Python3
    jinja2 --> Python3
    cbor --> Python3

    [*] --> u_guide
    u_guide --> Sphinx
    Sphinx --> m2r
    Sphinx --> rtd_theme
    Sphinx --> sphnix_puml
    m2r --> Python3
    rtd_theme --> Python3
    sphnix_puml --> Python3
    Sphinx --> PlantUML
    PlantUML --> JRE
    PlantUML --> gwiz
    Sphinx --> LaTex
    LaTex --> PdfLaTex

    [*] --> refman
    refman --> Doxygen
    Doxygen --> PlantUML
    Doxygen --> LaTex
    state Legend {
      state x as "For PDF generation only" <<pdf>>
      state y as "For document generation only" <<doc>>
      state z as "Mandatory"
    }

   @enduml

--------------

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*
