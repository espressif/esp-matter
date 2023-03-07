.. _module-pw_software_update:

-------------------
pw_software_update
-------------------

This module provides the following building blocks of a high assurance software
update system:

1. A `TUF <https://theupdateframework.io>`_-based security framework.
2. A `protocol buffer <https://developers.google.com/protocol-buffers>`_ based
   software update "bundle" format.
3. An update bundle decoder and verification stack.
4. An extensible software update RPC service.

High assurance software update
==============================

On a high-level, a high-assurance software update system should make users feel
safe to use and be technologically worthy of user's trust over time. In
particular it should demonstrate the following security and privacy properties.

1. The update packages are generic, sufficiently qualified, and officially
   signed with strong insider attack guardrails.
2. The update packages are delivered over secure channels.
3. Update checking, changelist, and installation are done with strong user
   authorization and awareness.
4. Incoming update packages are strongly authenticated by the client.
5. Software update requires and builds on top of verified boot.

Life of a software update
=========================

The following describes a typical software update sequence of events. The focus
is not to prescribe implementation details but to raise awareness in subtle
security and privacy considerations.

Stage 0: Product makers create and publish updates
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
A (system) software update is essentially a new version of the on-device
software stack. Product makers create, qualify and publish new software updates
to deliver new experiences or bug fixes to users.

While not visible to end users, the product maker team is expected to follow
widely agreed security and release engineering best practices before signing and
publishing a new software update. A new software update should be generic for
all devices, rather than targeting specific devices.

Stage 1: Users check for updates
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
For most consumer products, software updates are "opt-in", which means users
either manually check for new updates or opt-in for the device itself to check
(and/or install) updates automatically on the user's behalf. The opt-in may be
blanket or conditioned on the nature of the updates.

If users have authorized automatic updates, update checking also happens on a
regular schedule and at every reboot.

.. important::
   As a critical security recovery mechanism, checking and installing software
   updates ideally should happen early in boot, where the software stack has
   been freshly verified by verified boot and minimum mutable input is taken
   into account in update checking and installation.

   In other words, the longer the system has been running (up), the greater
   the chance that system has been taken control by an attacker. So it is
   a good idea to remind users to reboot when the system has been running for
   "too long".

Stage 2: Users install updates
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Once a new update has been determined to be available for the device, users will
be prompted to authorize downloading and installing the update. Users can also
opt-in to automatic downloading and installing.

.. important::
   If feasible, rechecking, downloading and installing an update should be
   carried out early in a reboot -- to recover from potential temporary attacker
   control.

To improve reliability and reduce disruption, modern system updates typically
employ an A/B update mechanism, where the incoming update is installed into
a backup boot slot, and only enacted and locked down (anti-rollback) after
the new slot has passed boot verification and fully booted into a good state.

.. important::
   While a system update is usually carried out by a user space update client,
   an incoming update may contain more than just the user space. It could
   contain firmware for the bootloader, trusted execution environment, DSP,
   sensor cores etc. which could be important components of a device's TCB (
   trusted compute base, where critical device security policies are enforced).
   When updating these components across different domains, it is best to let
   each component carry out the actual updating, some of which may require
   stronger user authorization (e.g. a test of physical presence, explicit
   authorization with an admin passcode etc.)

Lastly, updates should be checked again in case there are newer updates
available.


Command-line Interface
======================

You can access the software update CLI by running ``pw update`` in the pigweed environment.


.. code-block:: bash

  ~$ cd pigweed
  ~/pigweed$ source ./activate.sh
  ~/pigweed$ pw update

  usage: pw update [-h] <command>

  Software update related operations.

  positional arguments:
      generate-key
      create-root-metadata
      sign-root-metadata
      inspect-root-metadata
      create-empty-bundle
      add-root-metadata-to-bundle
      add-file-to-bundle
      sign-bundle
      inspect-bundle
      verify-bundle

  optional arguments:
    -h, --help            show this help message and exit

  Learn more at: pigweed.dev/pw_software_update


generate-key
^^^^^^^^^^^^

The ``generate-key`` subcommmand generates an ECDSA SHA-256 public + private keypair.

.. code-block:: bash

  $ pw update generate-key -h

  usage: pw update generate-key [-h] pathname

  Generates an ecdsa-sha2-nistp256 signing key pair (private + public)

  positional arguments:
    pathname    Path to generated key pair

  optional arguments:
    -h, --help  show this help message and exit


+------------+------------+----------------+
| positional argument                      |
+============+============+================+
|``pathname``|path to the generated keypair|
+------------+------------+----------------+

create-root-metadata
^^^^^^^^^^^^^^^^^^^^

The ``create-root-metadata`` subcommand creates a root metadata.

.. code-block:: bash

  $ pw update create-root-metadata -h

  usage: pw update create-root-metadata [-h] [--version VERSION] --append-root-key ROOT_KEY
         --append-targets-key TARGETS_KEY -o/--out OUT

  Creation of root metadata

  optional arguments:
    -h, --help                        show this help message and exit
    --version VERSION                 Canonical version number for rollback checks;
                                      Defaults to 1

  required arguments:
    --append-root-key ROOT_KEY        Path to root key
    --append-targets-key TARGETS_KEY  Path to targets key
    -o OUT, --out OUT                 Path to output file



+--------------------------+-------------------------------------------+
| required arguments                                                   |
+==========================+===========================================+
|``--append-root-key``     | path to desired root key                  |
+--------------------------+-------------------------------------------+
|``--append-targets-key``  | path to desired target key                |
+--------------------------+-------------------------------------------+
|``--out``                 | output path of newly created root metadata|
+--------------------------+-------------------------------------------+


+-------------+------------+------------------------------+
| optional argument                                       |
+=============+============+==============================+
|``--version``| Rollback version number(default set to 1) |
+-------------+------------+------------------------------+

sign-root-metadata
^^^^^^^^^^^^^^^^^^

The ``sign-root-metadata`` subcommand signs a given root metadata.

.. code-block:: bash

  usage: pw update sign-root-metadata [-h] --root-metadata ROOT_METADATA --root-key ROOT_KEY

  Signing of root metadata

  optional arguments:
    -h, --help                     show this help message and exit

  required arguments:
    --root-metadata ROOT_METADATA  Root metadata to be signed
    --root-key ROOT_KEY            Root signing key



+--------------------------+-------------------------------------------+
| required arguments                                                   |
+==========================+===========================================+
|``--root-metadata``       | Path of root metadata to be signed        |
+--------------------------+-------------------------------------------+
|``--root-key``            | Path to root signing key                  |
+--------------------------+-------------------------------------------+

inspect-root-metadata
^^^^^^^^^^^^^^^^^^^^^

The ``inspect-root-metadata`` subcommand prints the contents of a given root metadata.

.. code-block:: bash

  $ pw update inspect-root-metadata -h

  usage: pw update inspect-root-metadata [-h] pathname

  Outputs contents of root metadata

  positional arguments:
    pathname    Path to root metadata

  optional arguments:
    -h, --help  show this help message and exit


+--------------------------+-------------------------------------------+
| positional argument                                                  |
+==========================+===========================================+
|``pathname``              | Path to root metadata                     |
+--------------------------+-------------------------------------------+


create-empty-bundle
^^^^^^^^^^^^^^^^^^^

The ``create-empty-bundle`` subcommand creates an empty update bundle.

.. code-block:: bash

  $ pw update create-empty-bundle -h

  usage: pw update create-empty-bundle [-h] [--target-metadata-version VERSION] pathname

  Creation of an empty bundle

  positional arguments:
    pathname                           Path to newly created empty bundle

  optional arguments:
    -h, --help                         show this help message and exit
    --target-metadata-version VERSION  Version number for targets metadata;
                                       Defaults to 1

+--------------------------+-------------------------------------------+
| positional argument                                                  |
+==========================+===========================================+
|``pathname``              | Path to newly created empty bundle        |
+--------------------------+-------------------------------------------+

+------------------------------+--------------------------------------+
| optional arguments                                                  |
+==============================+======================================+
|``--target-metadata-version`` | Version number for targets metadata; |
|                              |         Defaults to 1                |
+------------------------------+--------------------------------------+

add-root-metadata-to-bundle
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``add-root-metadata-to-bundle`` subcommand adds a root metadata to a bundle.

.. code-block:: bash

  $ pw update add-root-metadata-to-bundle -h

  usage: pw update add-root-metadata-to-bundle [-h] --append-root-metadata ROOT_METADATA
         --bundle BUNDLE

  Add root metadata to a bundle

  optional arguments:
    -h, --help                            show this help message and exit

  required arguments:
    --append-root-metadata ROOT_METADATA  Path to root metadata
    --bundle BUNDLE                       Path to bundle

+--------------------------+-------------------------------------------+
| required arguments                                                   |
+==========================+===========================================+
|``--append-root-metadata``| Path to root metadata                     |
+--------------------------+-------------------------------------------+
|``--bundle``              | Path to bundle                            |
+--------------------------+-------------------------------------------+


add-file-to-bundle
^^^^^^^^^^^^^^^^^^

The ``add-file-to-bundle`` subcommand adds a target file to an existing bundle.

.. code-block:: bash

  $ pw update add-file-to-bundle -h

  usage: pw update add-file-to-bundle [-h] [--new-name NEW_NAME] --bundle BUNDLE
         --file FILE_PATH

  Add a file to an existing bundle

  optional arguments:
    -h, --help           show this help message and exit
    --new-name NEW_NAME  Optional new name for target

  required arguments:
    --bundle BUNDLE      Path to an existing bundle
    --file FILE_PATH     Path to a target file

+--------------------------+-------------------------------------------+
| required arguments                                                   |
+==========================+===========================================+
|``--file``                | Path to a target file                     |
+--------------------------+-------------------------------------------+
|``--bundle``              | Path to bundle                            |
+--------------------------+-------------------------------------------+

+--------------------------+-------------------------------------------+
| optional argument                                                    |
+==========================+===========================================+
|``--new-name``            | Optional new name for target              |
+--------------------------+-------------------------------------------+

sign-bundle
^^^^^^^^^^^

The ``sign-bundle`` subcommand signs an existing bundle with a dev key.

.. code-block:: bash

  $ pw update sign-bundle -h

  usage: pw update sign-bundle [-h] --bundle BUNDLE --key KEY

  Sign an existing bundle using a development key

  optional arguments:
    -h, --help       show this help message and exit

  required arguments:
    --bundle BUNDLE  Bundle to be signed
    --key KEY        Bundle signing key

+--------------------------+-------------------------------------------+
| required arguments                                                   |
+==========================+===========================================+
|``--key``                 | Key to sign bundle                        |
+--------------------------+-------------------------------------------+
|``--bundle``              | Path to bundle                            |
+--------------------------+-------------------------------------------+

inspect-bundle
^^^^^^^^^^^^^^

The ``inspect-bundle`` subcommand prints the contents of a given bundle.

.. code-block:: bash

  $ pw update inspect-bundle -h

  usage: pw update inspect-bundle [-h] pathname

  Outputs contents of bundle

  positional arguments:
    pathname    Path to bundle

  optional arguments:
    -h, --help  show this help message and exit


+------------+------------+----------------+
| positional argument                      |
+============+============+================+
|``pathname``|Path to bundle               |
+------------+------------+----------------+

verify-bundle
^^^^^^^^^^^^^

The ``verify-bundle`` subcommand performs verification of an existing bundle.

.. code-block:: bash

  $ pw update verify-bundle -h

  usage: pw update verify-bundle [-h] --bundle BUNDLE
         --trusted-root-metadata ROOT_METADATA

  Verify a bundle

  optional arguments:
    -h, --help                             show this help message and exit

  required arguments:
    --bundle BUNDLE                        Bundle to be verified
    --trusted-root-metadata ROOT_METADATA  Trusted root metadata

+---------------------------+-------------------------------------------+
| required arguments                                                    |
+===========================+===========================================+
|``--trusted-root-metadata``| Trusted root metadata(anchor)             |
+---------------------------+-------------------------------------------+
|``--bundle``               | Path of bundle to be verified             |
+---------------------------+-------------------------------------------+

Getting started with bundles (coming soon)
==========================================
