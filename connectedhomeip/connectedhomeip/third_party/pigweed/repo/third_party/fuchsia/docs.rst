.. _module-pw_third_party_fuchsia:

=================
Fuchsia libraries
=================
`Fuchsia <https://fuchsia.dev/>`_ is a modern open source operating system
developed by Google.

Pigweed does not use the Fuchsia operating system itself, but uses some
low-level libraries developed for it.

--------
Features
--------
Parts of two Fuchsia libraries are used in Pigweed:

- `FIT <https://cs.opensource.google/fuchsia/fuchsia/+/main:sdk/lib/fit/>`_ --
  Portable library of low-level C++ features.
- `stdcompat <https://cs.opensource.google/fuchsia/fuchsia/+/main:sdk/lib/stdcompat/>`_ --
  Implements newer C++ features for older standards.

--------------------
Code synchronization
--------------------
Unlike other third party libraries used by Pigweed, some Fuchsia source code is
included in tree. A few factors drove this decision:

- Core Pigweed features like :cpp:type:`pw::Function` depend on these Fuchsia
  libraries. Including the source in-tree avoids having Pigweed require an
  an external repository.
- The Fuchsia repository is too large to require downstream projects to clone.

If Fuchsia moves ``stdcompat`` and ``fit`` to separate repositories, the
decision to include Fuchsia code in tree may be reconsidered.

Files are synced from Fuchsia repository to the ``third_party/fuchsia/repo``
directory in Pigweed. The files maintain their original paths under that
directory. The Copybara script applies patches to adapt the sources for use in
Pigweed. For example, ``__builtin_abort`` is replaced with ``PW_ASSERT``.

Process
=======
Code is synchronized between the `Fuchsia repository
<https://fuchsia.googlesource.com/fuchsia>`_ and the `Pigweed repository
<https://pigweed.googlesource.com/pigweed/pigweed>`_ using the
`third_party/fuchsia/copy.bara.sky
<https://cs.opensource.google/pigweed/pigweed/+/main:third_party/fuchsia/copy.bara.sky>`_
`Copybara <https://github.com/google/copybara>`_ script.

To synchronize with the Fuchsia repository, run the ``copybara`` tool with the
script:

.. code-block:: bash

   copybara third_party/fuchsia/copy.bara.sky

That creates a Gerrit change with updates from the Fuchsia repo, if any.

If the ``copybara`` command fails, the Copybara script or patch file may need to
be updated. Try the following:

- Ensure that the source files in ``copy.bara.sky`` are up-to-date. Fix the list
  if any files were renamed in Fuchsia.
- Update the patch file Copybara applies by running ``python
  third_party/fuchsia/generate_fuchsia_patch.py``.
