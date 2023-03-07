.. _seed-0001:

======================
0001: The SEED Process
======================

.. card::
   :fas:`seedling` SEED-0001: :ref:`The SEED Process<seed-0001>`

   :octicon:`comment-discussion` Status:
   :bdg-primary:`Open for Comments`
   :octicon:`chevron-right`
   :bdg-secondary-line:`Last Call`
   :octicon:`chevron-right`
   :bdg-secondary-line:`Accepted`
   :octicon:`kebab-horizontal`
   :bdg-secondary-line:`Rejected`

   :octicon:`calendar` Proposal Date: 2022-10-31

   :octicon:`code-review` CL: `pwrev/116577 <https://pigweed-review.googlesource.com/c/pigweed/pigweed/+/116577>`_

-------
Summary
-------
SEEDs are the process through which substantial changes to Pigweed are proposed,
reviewed, and decided by community stakeholders to collaboratively drive the
project in a favorable direction.

This document outlines the SEED process at a high level. Details about how SEEDs
should be written and structured are described in :ref:`seed-0002`.

----------
Motivation
----------
As Pigweed and its community grow, it becomes important to ensure that the
Pigweed team, Pigweed users, and other community stakeholders align on the
future of the project. To date, development of Pigweed has primarily been
driven by the core team, and community feedback has been mostly informal and
undocumented.

SEEDs are a formalized process for authoring, reviewing, and ratifying proposed
changes to Pigweed.

The SEED process has several goals.

- Increase the visibility of proposed changes to Pigweed early on and allow
  interested parties to engage in their design.

- Maintain a public record of design decisions rendered during Pigweed's
  development and the rationales behind them.

- Ensure consistent API design across Pigweed modules through a formal team-wide
  review process.

Active SEEDs are discussed by the community through Gerrit code review comments
as well as a SEEDs chatroom in Pigweed's Discord server. Decisions are
ultimately rendered by a review committee of Pigweed team members.

------------------------
When is a SEED required?
------------------------
SEEDs should be written by any developer wishing to make a "substantial" change
to Pigweed. Whether or not a change is considered "substantial" varies depending
on what parts of Pigweed it touches and may change over time as the project
evolves. Some general guidelines are established below.

Examples of changes considered "substantial" include, but are not limited to:

- Adding a new top-level module.

- Modifying a widely-used public API.

- A breaking update to typical Pigweed user workflows (bootstrap, build setup,
  ``pw`` commands, etc.).

- Changing any data interchange or storage protocol or format (e.g. transport
  protocols, flash layout), unless the change is small and backwards compatible
  (e.g. adding a field to an exchanged Protobuf message).

- Changes to Pigweed's code policy, style guides, or other project-level
  guidelines.

- Whenever a Pigweed team member asks you to write a SEED.

Conversely, the following changes would likely not require a SEED:

- Fixing typos.

- Refactoring internal module code without public API changes.

- Adding minor parallel operations to existing APIs (e.g.
  ``Read(ConstByteSpan)`` vs ``Read(const byte*, size_t)``).

If you're unsure whether a change you wish to make requires a SEED, it's worth
asking the Pigweed team in our Discord server prior to writing any code.

-------
Process
-------
Suppose you'd like to propose a new Pigweed RPC Over Smoke Signals protocol.

#. If you haven't already, clone the Pigweed repository and set it up locally,
   following the :ref:`docs-getting-started` guide.

#. Copy the `SEED template <0002-template>`_ to create the RST file for your
   SEED. As you don't yet have a SEED number, use XXXX as a placeholder,
   followed by the lowercase title of the proposal, with words separated by
   hyphens.

   .. code-block:: sh

      cp seed/0002-template.rst seed/XXXX-pw_rpc-over-smoke-signals.rst

#. Push up the template to Gerrit, marking it as a Work-In-Progress change.
   From here, you may fill the template out with the content of your proposal
   at your convenience.

#. At any point, you may claim a SEED number by opening the
   `SEED index <0000-index>`_ and taking the next available number by inserting
   a row into the ``toctree`` table. Link the entry to the WIP change for your
   SEED.

   .. code-block:: rst

      .. toctree::

         0001-the-seed-process
         ...
         5308-some-other-seed
         5309: pw_rpc Over Smoke Signals<https://pigweed-review.googlesource.com/c/pigweed/pigweed/+/116577>

#. Commit your change to the index (and nothing else) with the commit message
   ``SEED-xxxx: Claim SEED number``.

   .. code-block:: sh

      git add seed/0000-index.rst
      git commit -m "SEED-5309: Claim SEED number"

#. Push up a changelist (CL) to Gerrit following the :ref:`docs-contributing`
   guide and add GWSQ as a reviewer. Set ``Pigweed-Auto-Submit`` to +1.

   .. image:: 0001-the-seed-process/seed-index-gerrit.png

#. Once your CL has been reviewed and submitted, the SEED number belongs to you.
   Update your document's template and filename with this number.

#. When you feel you have enough substantive content in your proposal to be
   reviewed, push it up to Gerrit and switch the change from WIP to Active.
   This will begin the open comments period.

#. Engage with reviewers to iterate on your proposal through its comment period.

#. When a tentative decision has been reached, a Pigweed team member will
   comment on your proposal with a summary of the discussion and reasoning,
   moving it into its Last Call phase (as described in the :ref:`Lifecycle
   <seed-0001-lifecycle>` section).

#. Following the conclusion of the Last Call period, a Pigweed team member will
   sign off on the CL with a +2 vote, allowing it to be submitted. Update the
   reference in the SEED index with the link to your document and submit the CL.

   .. code-block:: rst

      .. toctree::

         0001-the-seed-process
         ...
         5308-some-other-seed
         5309-pw_rpc-over-smoke-signals

--------------
SEED documents
--------------
SEEDs are written as ReST documents integrated with the rest of Pigweed's
documentation. They live directly within the core Pigweed repository, under a
top-level ``seed/`` subdirectory.

The structure of SEED documents themselves, their format, required sections, and
other considerations are outlined in :ref:`seed-0002`.

The first 100 SEEDs (0000-0100) are *Meta-SEEDs*. These are reserved for
internal Pigweed usage and generally detail SEED-related processes. Unlike
regular SEEDs, Meta-SEEDs are living documents which may be revised over time.

.. _seed-0001-lifecycle:

-----------------------
The lifecycle of a SEED
-----------------------
A SEED proposal undergoes several phases between first being published and a
final decision.

:bdg-primary-line:`Draft` **The SEED is a work-in-progress and not yet ready
for comments.**

- The SEED exists in Gerrit as a Work-In-Progress (WIP) change.
- Has an assigned SEED number and exists in the index.
- Not yet ready to receive feedback.

:bdg-primary:`Open for Comments` **The SEED is soliciting feedback.**

- The SEED has sufficient substance to be reviewed, as determined by its
  author.
- A thread for the SEED is created in Discord to promote the proposal and open
  discussion.
- Interested parties comment on the SEED to evaluate the proposal, raise
  questions and concerns, and express support or opposition.
- Back and forth discussion between the author and reviewers, resulting in
  modifications to the document.
- The SEED remains open for as long as necessary. Internally, Pigweed's review
  committee will regularly meet to consider active SEEDs and determine when to
  advance to them the next stage.

:bdg-warning:`Last Call` **A tentative decision has been reached, but
commenters may raise final objections.**

- A tentative decision on the SEED has been made. The decision is issued at the
  best judgement of Pigweed's review committee when they feel there has been
  sufficient discussion on the tradeoffs of the proposal to do so.
- Transition is triggered manually by a member of the Pigweed team, with a
  comment on the likely outcome of the SEED (acceptance / rejection).
- On entering Last Call, the visibility of the SEED is widely boosted through
  Pigweed's communication channels (Discord, mailing list, Pigweed Live, etc.)
  to solicit any strong objections from stakeholders.
- Typically, Last Call lasts for a set period of 7 calendar days, after which
  the final decision is formalized.
- If any substantial new arguments are raised during Last Call, the review
  committee may decide to re-open the discussion, returning the SEED to a
  commenting phase.

:bdg-success:`Accepted` **The proposal is ratified and ready for
implementation.**

- The SEED is submitted into the Pigweed repository.
- A tracking bug is created for the implementation, if applicable.
- The SEED may no longer be modified (except minor changes such as typos).
  Follow-up discussions on the same topic require a new SEED.

:bdg-danger:`Rejected` **The proposal has been turned down.**

- The SEED is submitted into the Pigweed repository to provide a permanent
  record of the considerations made for future reference.
- The SEED may no longer be modified.

:bdg-secondary:`Deprecated` **The proposal was originally accepted and
implemented but later removed.**

- The proposal was once implemented but later undone.
- The SEED's changelog contains justification for the deprecation.

:bdg-info:`Superseded` **The proposal was originally accepted and implemented
but significant portions were later overruled by a different SEED.**

- A newer SEED proposal revisits the same topic and proposal and redesigns
  significant parts of the original.
- The SEED is marked as superseded with a reference to the newer proposal.

---------
Rationale
---------

Document format
---------------
Three different documentation formats are considered for SEEDs:

- **ReST:** Used for Pigweed's existing documentation, making it a natural
  option.
- **Google Docs:** The traditional way of writing SEED-like investigation and
  design documents.
- **Markdown:** Ubiquitous across open-source projects, with extensive tooling
  available.

Summary
^^^^^^^
Based on the evaluated criteria, ReST documents provide the best overall SEED
experience. The primary issues with ReST exist around contributor tooling, which
may be mitigated with additional investment from the Pigweed team.

The table below details the main criteria evaluated for each format, with more
detailed explanations following.

.. list-table::
   :widths: 55 15 15 15
   :header-rows: 1

   * - Criterion
     - ReST
     - Markdown
     - Google Docs
   * - Straightforward integration with existing docs
     - ✅
     - ❌
     - ❌
   * - Indexable on `pigweed.dev <https://pigweed.dev>`_
     - ✅
     - ✅
     - ❌
   * - Auditable through source control
     - ✅
     - ✅
     - ❌
   * - Archive of review comments and changes
     - ✅
     - ✅
     - ❌
   * - Accessible to contributors
     - ❌
     - ✅
     - ✅
   * - Extensive styling and formatting options
     - ✅
     - ❌
     - ✅
   * - Easy sharing between Google and external contributors
     - ✅
     - ✅
     - ❌

Integration
^^^^^^^^^^^
.. admonition:: Goal

   SEED documents should seamlessly integrate with the rest of Pigweed's docs.

As all of Pigweed's documentation is written using ReST, it becomes a natural
choice for SEEDs. The use of other formats requires additional scaffolding and
may not provide as seamless of an experience.

Indexability
^^^^^^^^^^^^
.. admonition:: Goal

   Design decisions in SEEDs should be readily available for Pigweed users.

`pigweed.dev <https://pigweed.dev>`_ has a search function allowing users to
search the site for Pigweed-related keywords. As SEEDs contain design discussion
and rationales, having them appear in these searches offers useful information
to users.

The search function is provided by Pigweed's Sphinx build, so only documents
which exist as part of that (ReST / Markdown) are indexed.

Auditability
^^^^^^^^^^^^
.. admonition:: Goal

   Changes to SEED documents should be reviewed and recorded.

ReST and Markdown documents exist directly within Pigweed's source repository
after being submitted, requiring any further changes to go through a code
review process.

Conversely, Google Docs may be edited by anyone with access, making them prone
to unintentional modification.

Archive of discussions
^^^^^^^^^^^^^^^^^^^^^^
.. admonition:: Goal

   Discussions during the review of a SEED should be well-archived for
   future reference.

ReST and Markdown documentation are submitted through Gerrit and follow the
standard code review process. Review comments on the changes are saved in
Gerrit and are easily revisited. Incremental updates to the SEED during the
review process are saved as patch sets.

Comments in Google Docs are more difficult to find once they are resolved, and
document changes do not exist as clearly-defined snapshots, making the history
of a SEED harder to follow.

Accessibility
^^^^^^^^^^^^^
.. admonition:: Goal

   SEEDs should be easy for contributors to write.

Both Markdown and Google Docs are easy to write, familiar to many, and have
extensive tooling available. SEED documents can be written outside of the
Pigweed ecosystem using authors' preferred tools.

ReST, on the other hand, is an unfamiliar and occasionally strange format, and
its usage for SEEDs is heavily tied to Pigweed's documentation build. Authors
are required to set up and constantly re-run this build, slowing iteration.

Format and styling
^^^^^^^^^^^^^^^^^^
.. admonition:: Goal

   SEED authors should have options for formatting various kinds of information
   and data in their proposals.

Markdown intentionally only offers limited control over document formatting,
whereas ReST has a wide selection of directives and Google Docs functions as a
traditional WYSIWYG editor, making them far more flexible.

Sharing between Google and non-Google
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. admonition:: Goal

   Both Google and non-Google contributors should easily be able to write and
   review SEEDs.

Due to security and legal concerns, managing ownership of Google Docs between
internal and external contributors is nontrivial.

Text documentation formats like Markdown and ReST live within the Pigweed
repository, and as such follow the standard code contribution process.
