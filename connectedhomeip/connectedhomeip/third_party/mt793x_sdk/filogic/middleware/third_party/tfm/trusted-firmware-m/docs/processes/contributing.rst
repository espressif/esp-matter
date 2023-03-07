Contributing
============

Contributions to the TF-M project need to follow the process below.

.. Note::

   Please contact :doc:`maintainers </docs/about/maintainers>` for any
   questions.

- Subscribe to `TF-M development
  <https://lists.trustedfirmware.org/mailman/listinfo/tf-m>`_ if not subscribed
  already.
- Refer to the `Roadmap
  <https://developer.trustedfirmware.org/w/tf_m/planning>`_ or send a mail to
  the tf-m@lists.trustedfirmware.org to check if this work is already
  planned/progresses elsewhere.
- Create a task in `Phabricator
  <https://developer.trustedfirmware.org/maniphest>`_, put as many details as
  possible in the description. Add 'Trusted Firmware M' in the 'Tags' field.
- For non-trivial changes, need to follow the design proposal process
  :doc:`Design Proposal Process </docs/processes/tfm_design_proposal_process>`
  for the TF-M project.
- After the design has been accepted by the maintainer(s), a corresponding
  patch should be posted; follow guidelines below:

  - Clone the TF-M code on your own machine from `TF-M git repository
    <http://git.trustedfirmware.org/trusted-firmware-m.git>`_.
  - Follow the :doc:`SW Requirements </docs/user_guides/tfm_sw_requirement>`,
    :doc:`Build Instructions </docs/user_guides/tfm_build_instruction>` and
    :doc:`Coding Guide </docs/about/coding_guide>` for the TF-M project.
  - Make your changes in logical chunks to help reviewers. Each commit should
    be a separate review and either work properly or be squashed after the
    review and before merging.
  - Update documentation in docs/ folder if needed.
  - Test your changes and add details to the commit description.
  - The code is accepted under :doc:`DCO </docs/about/dco>`, Developer
    Certificate of Origin, so you must add following fields to your
    commit description:

    .. code-block:: text

       Author: Full Name <email address>
       Signed-off-by: Full Name <email address>

    .. Note::

       Sign off authority needs to adhere to the [DCO](./dco.txt) rules.

  - You may add other fields in the commit message. Need to add the Task ID link
    in the comment for the commit.
  - Submit your patch for review.

    .. code-block:: shell

       git push ssh://review.trustedfirmware.org:29418/trusted-firmware-m.git HEAD:refs/for/master

- Add relevant :doc:`maintainers </docs/about/maintainers>` for reviewing
  the patch.
- You may be asked to provide further details or make additional changes.
- You can discuss further with maintainer(s) by directly over email if
  necessary.
- Once the change is approved by maintainers, the patch will be merged by the
  maintainer.
- Mark the task as 'resolved' after patch is merged.

--------------

*Copyright (c) 2017-2019, Arm Limited. All rights reserved.*
