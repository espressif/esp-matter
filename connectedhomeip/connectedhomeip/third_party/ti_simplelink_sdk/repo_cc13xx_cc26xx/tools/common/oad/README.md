# OAD Image Tool

## Table of Contents

* [Introduction](#Introduction)
* [Software Prerequisites](#Software Prerequisites)
* [Usage](#Usage)

## <a name="Introduction"></a>Introduction

The oad\_image\_tool adds OAD metadata to an image to prepare it for over the
transfer. The OAD metadata is a collection of information about the image
including length, start address, and crc that will be used throughout the OAD.
Process.

This tool is intended to run as a post build step. Please refer to the
OAD enabled sample applications to for a reference invocation of the tool.

Further documentation can be found the SDK's User Guide. There is a chapter
dedicated to OAD which will cover the tool.

## <a name="Software Prerequisites"></a>Software Prerequisites

The OAD image tool is shipped in both source and binary form. It is recommended
to run the tool in binary form unless source changes are absolutely required.


The following is required to run the binary image:

**Linux:**
 - The binary is 64-bit, will run without modification on 64-bit systems
 - 32-bit distributions are not supported at this time

**Windows**
 - The binary is 32-bit, but is supported on 32-bit or 64-bit versions of
   Windows.

**MacOS**
 - WARNING: MacOS support is experimental.
 - The binary is 64-bit, as of MacOS Sierra this is the only supported ArchType.

In order to run the tool from source, the following dependencies are needed:

 - Python 2.x
 - The Python packages listed in the requirements.txt in this folder. These can
   be installed via pip.

## <a name="Usage"></a>Usage

More information on the tool can be found in the SDK User's Guide in the OAD
section. Invoke the tool with the `-h` option to display the help menu and to
see required and optional arguments.
