# Silicon Labs Gecko SDK (GSDK)

The Gecko SDK (GSDK) combines Silicon Labs wireless software development kits (SDKs) and Gecko Platform into a single, integrated package. The GSDK is your primary tool for developing in the Silicon Labs IoT Software ecosystem. All of Silicon Labs' stacks are written in-house to provide a seamless experience from silicon to tools, allowing you to unlock powerful features with ease, including:

- Abstraction of complex requirements like multiprotocol and pre-certification
- Industry-leading ability to support a large number of nodes
- Ultra-low power consumption
- Strong network reliability

Silicon Labs also helps future-proof your devices with over-the-air software and security updates, helping to minimize maintenance cost and improve your end user product experience!

## Licensing

Gecko SDK is covered by one of several different licenses.
The default license is the [Master Software License Agreement (MSLA)](https://www.silabs.com/about-us/legal/master-software-license-agreement), which applies unless otherwise noted. Refer to [License.txt](./License.txt) for full details.

## Documentation

An archive of all documents associated with the latest release of GSDK can be found in [Releases](https://github.com/SiliconLabs/gecko_sdk/releases). Documentation for some of the protocol SDKs and software components is available on https://docs.silabs.com.

## Getting Started with GSDK

The recommended method to get started with the GSDK is to first install [Simplicity Studio 5](https://www.silabs.com/developers/simplicity-studio), which will setup your development environment and walk you through the installation of GSDK. Alternatively, GSDK and other required tools may be installed manually.

### Installing through Simplicity Studio

Simplicity Studio 5 includes everything needed for IoT product development with Silicon Labs devices, including:

- Detection and recognition of evaluation and development kits,
- Resource navigator with kit and device-specific content,
- Software project generator for multiple IDEs and GNU Make targets,
- Software and hardware configuration tools,
- Eclipse CDT-based IDE,
- GNU toolchain,
- Advanced network analysis tools,
- Code-correlated energy profiling,
- And a variety of other tools.

Start by downloading the [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio) package for your operating system from https://www.silabs.com/simplicity. Step-by-step installation instructions are provided in the online [Simplicity Studio 5 User’s Guide](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/install-ss-5-and-software).

### Installing GSDK and Tools Manually

A GSDK development environment may also be installed and set up manually.

#### GSDK

GSDK releases are packaged as an archive that may be downloaded from the [Releases](https://github.com/SiliconLabs/gecko_sdk/releases) page on GitHub. You may also clone this repo using a [git client](https://github.com/git-guides/install-git). Note that an installation of [git lfs](https://git-lfs.github.com/) is also required. If you are installing git lfs only to use with the GSDK, you do not need to specify file types. 

#### Toolchain

This release of GSDK supports the Arm Embedded Toolchain version 10.2 (10-2020-q4-major) and IAR Embedded Workbench v8.50.9. One of these toolchains must be installed.

- [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) - Download and install v10.2 for your host operating system
- [IAR Embedded Workbench for Arm](https://www.iar.com/downloads) - Only the latest version is available in Downloads; for older versions you must log-in to your "My Pages".

#### Silicon Labs Configurator (SLC) Command-line Interface (CLI)

GSDK contains software that follows the [SLC specification](https://siliconlabs.github.io/slc-specification/). Software is grouped into components (defined by .slcc files) that may provide features and/or require features provided by other components. Example projects (.slcp) describe a single software application (usually made up of multiple components plus application code) that can be used to generate an IDE project.

Simplicity Studio 5 includes a pre-integrated version of SLC tooling. The SLC-CLI tool provides a command-line alternative that, among other things, will resolve project and component dependencies and generate a project for a specified embedded target and build system (for example, IAR Embedded Workbench or GNU tools via a Makefile).  

See the [SLC specification](https://siliconlabs.github.io/slc-specification/) for details about SLC. For information on installing and using the SLC-CLI, see [UG520: Software Project Generation and Configuration with SLC-CLI](https://www.silabs.com/documents/public/user-guides/ug520-software-project-generation-configuration-with-slc-cli.pdf). 
