# Prerequisites

## Git

[Git](https://git-scm.com/downloads) is required for acquiring the source code.

## Developer-tools

Our tools configuration are part of [developer tools](https://gitlab.arm.com/iot/open-iot-sdk/tools/developer-tools).

Use `git clone --recurse-submodules` to fetch also submodules like developer-tools.

## Toolchains

A toolchain is required for building the examples, tests, and other code. There are two choices for the toolchain.

| Toolchain | Download | C Compiler |
|---------|----------|----------------|
| Arm Compiler for Embedded 6.18 | - [standalone](https://developer.arm.com/downloads/-/arm-compiler-for-embedded) <br /> - [Keil MDK 5.37](https://www.keil.com/update/relnotes/MDK537.htm) | armclang |
| GNU Arm Embedded version 10.3 (10.3-2021.10) | - [standalone](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads) | arm-none-eabi-gcc |

The open source GNU Arm Embedded Toolchain is recommended for quick start.

Whichever toolchain is chosen, it should be verified that the toolchain is in the system path to allow it to be located. For the Arm Compiler, `armclang --version` or with the GNU Arm Embedded Toolchain, `arm-none-eabi-gcc --version` can be used to verify this.

## CMake

Use [CMake](https://cmake.org/download/) v3.21.0 or newer for the build process.

## Build system

For this project, either *GNU Make* or *Ninja* is required as the CMake build generator. GNU Make is usually present on Linux distributions, but [Ninja](https://ninja-build.org/) is strongly preferred for its fast build speed. Neither comes preinstalled on Windows and macOS, so you need to install one of them.

Whichever build generator is chosen, it should be verified that the build generator is in the system path to allow it to be located. For Ninja, `ninja --version` or with GNU Make, `make --version` can be used to verify this.

**Note**: On Windows, CMake's default build generator *Visual Studio* does *not* work with this project. You must override the build generator as described below.

If you are using a generator that is not the default for your platform, CMake must be informed either using the `CMAKE_GENERATOR` environment variable or by passing `-G <generator-name>` in the CMake command line. See [CMake options](https://cmake.org/cmake/help/latest/manual/cmake.1.html) and [CMake generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) for more information.

For example, if you want to use Ninja for a build, pass `-GNinja` when configuring the build. To avoid needing `-GNinja` every time and make it the default, you can add `export CMAKE_GENERATOR=Ninja` to your shell's profile, or (if on Windows) add an entry `CMAKE_GENERATOR` with value `Ninja` to the *Environment Variables* system setting.

## Arm Corstone-300 MPS3 Fixed Virtual Platform (FVP)

For a guide on setting up and running software on the Corstone-300 FVP, please
see [Running apps on virtual
hardware](https://gitlab.arm.com/iot/open-iot-sdk/mcu-driver-hal/mcu-driver-reference-platforms-for-arm/-/blob/main/docs/fvp/corstone-300/installation.md)

## Greentea tests

To run the Greentea tests for included examples, there are some additional prerequistes.

### Python

[Python](https://www.python.org/downloads/) v3.8+ is recommended. Pip (included with all recent versions of Python) is required to install packages.

### htrun
Tests require the `htrun` command provided by the
[greentea-host](https://pypi.org/project/greentea-host/) python package.

Once Python is installed, it can be installed with pip with this command:

```
$ pip3 install greentea-host --upgrade
```

It should be verified that `htrun` is in the system path. This can be done by running `htrun --version`.

For testing on Corstone-300 FVP, the FastModel agent https://github.com/ARMmbed/mbed-fastmodel-agent.git needs to be fetched, with fm_agent/settings.json edited to point to your local installation of the FVP for the model binary as well as the path to Iris. Alternatively, you can keep the default values in the file and symlink your installation to there. Then do `pip3 install <path/to/mbed-fastmodel-agent>` to install it.

## Trusted Firmware-M (TF-M)

### Python packages

To build TF-M, a few Python packages need to be installed after configuration (i.e. the step with `--toolchain`) and before building (i.e. the step with `--build`):

```
$ pip3 install -r https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/plain/tools/requirements.txt?h=TF-Mv1.6.0
```

This only needs to be done once unless the revision of TF-M has changed.

### Objcopy and SRecord

To convert and combine signed TF-M images to the ELF format required by the FVP, you need
* *either* `arm-none-eabi-objcopy` from the GNU Arm Embedded Toolchain (described above) *or* `objcopy` from GNU Binutils
* `srec_cat` from SRecord

If you want to use the Arm Compiler only without setting up the GNU Arm Embedded Toolchain, you can get the `objcopy` command by installing the `binutils` package provided by your operating system's package manager, or, for Windows, get [the pre-built Binutils](https://osdn.net/projects/mingw/releases/p15608) and add the extracted `bin` subdirectory to `PATH` in your environment.

You can get `srec_cat` by installing the `srecord` package provided by your operating system's package manager, or, for Windows, get [the pre-built SRecord](https://sourceforge.net/projects/srecord/files/srecord-win32/) and add the extracted directory to `PATH` in your environment.
