##################
Trusted Firmware M
##################
Trusted Firmware M provides a reference implementation of secure world software
for Arm M-profile architecture.

.. Note::
    The software implementation contained in this project is designed to be a
    reference implementation of the Platform Security Architecture (PSA).

Terms ``TFM`` and ``TF-M`` are commonly used in documents and code and both
refer to ``Trusted Firmware M.`` :doc:`Glossary </docs/glossary>` has the list
of terms and abbreviations.

#######
License
#######
The software is provided under a BSD-3-Clause :doc:`License </docs/lic>`.
Contributions to this project are accepted under the same license with developer
sign-off as described in the :doc:`Contributing Guidelines </docs/processes/contributing>`.

This project contains code from other projects as listed below. The code from
external projects is limited to ``app``, ``bl2``, ``lib`` and ``platform``
folders. The original license text is included in those source files.

- The ``app`` folder contains files imported from CMSIS_5 project and the files
  have Apache 2.0 license.
- The ``bl2`` folder contains files imported from MCUBoot project and the files
  have Apache 2.0 license.
- The ``lib`` folder may contain 3rd party files with diverse licenses.
- The ``platform`` folder currently contains platforms support imported from
  the external project and the files may have different licenses.

.. Note::
    Any folder that contains or directly imports 3rd party code is kept in a
    specific subfolder named ``ext`` so that it can be isolated if required.
    The 'ext' folder can be seen in the folders above mentioned.

#############
Release Notes
#############
The :doc:`Change Log & Release Notes </docs/changelog>` provides details of
major features of the release and platforms supported.

###############
Getting Started
###############

************
Prerequisite
************
Trusted Firmware M provides a reference implementation of PSA specifications.
It is assumed that the reader is familiar with PSA concepts and terms. PSA
specifications can be found at
`Platform Security Architecture Resources <https://developer.arm.com/architectures/security-architectures/platform-security-architecture>`__.

The current TF-M implementation specifically targets TrustZone for ARMv8-M so a
good understanding of the v8-M architecture is also necessary. A good place to
get started with ARMv8-M is
`developer.arm.com <https://developer.arm.com/technologies/trustzone>`__.

**********************
Really getting started
**********************
Trusted Firmware M source code is available on
`git.trustedfirmware.org <https://git.trustedfirmware.org/trusted-firmware-m.git/>`__.

To build & run TF-M:

    - Follow the :doc:`SW requirements guide </docs/user_guides/tfm_sw_requirement>`
      to set up your environment.
    - Follow the
      :doc:`Build instructions </docs/user_guides/tfm_build_instruction>` to compile
      and build the TF-M source.
    - Follow the :doc:`User guide </docs/user_guides/tfm_user_guide>` for information
      on running the example.

To port TF-M to a another system or OS, follow the
:doc:`OS Integration Guide </docs/user_guides/tfm_integration_guide>`

Please also see the :doc:`glossary </docs/glossary>` of terms used in the project.

:doc:`Contributing Guidelines </docs/processes/contributing>` contains guidance on how to
contribute to this project.

Further documents can be found in the ``docs`` folder.


####################
Feedback and support
####################
For this release, feedback is requested via email to
`tf-m@lists.trustedfirmware.org <tf-m@lists.trustedfirmware.org>`__.

###############
Version history
###############
+-------------+--------------+--------------------+---------------+
| Version     | Date         | Description        | PSA-arch tag  |
+=============+==============+====================+===============+
| v1.0-beta   | 2019-02-15   | 1.0-beta release   |       -       |
+-------------+--------------+--------------------+---------------+
| v1.0-RC1    | 2019-05-31   | 1.0-RC1 release    | v19.06_API0.9 |
+-------------+--------------+--------------------+---------------+
| v1.0-RC2    | 2019-10-09   | 1.0-RC2 release    | v19.06_API0.9 |
+-------------+--------------+--------------------+---------------+
| v1.0-RC3    | 2019-11-29   | 1.0-RC3 release    | v19.06_API0.9 |
+-------------+--------------+--------------------+---------------+
| v1.0        | 2020-03-27   | 1.0 release        | v20.03_API1.0 |
+-------------+--------------+--------------------+---------------+

--------------

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*
