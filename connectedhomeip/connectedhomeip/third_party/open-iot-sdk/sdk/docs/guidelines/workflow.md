# Open IoT SDK Workflow

## Branching naming convention
Namespace your branches, using / as the delimiter

```
<scope>/<username>/<branch-name>
```

### scope

Examples could be:

* fix - bugfixes
* dev - development

### username

This should be your full Gitlab user name (e.g. myname.mylastname).

### branch-name

The branch name should give more details of the purpose for the submission and should be dash seperated words.
E.g. config-bugfix-#531

### Examples

```
fix/myname.mylastname/config-bugfix-#531
dev/myname.mylastname/add-ticker-measurement-in-seconds
```

## Git Commit guidelines

For git commit messages, we follow Chris Beams' [7 rules](https://cbea.ms/git-commit/#seven-rules) with the following exceptions:

### Exceptions:

* Commit message subject is allowed to be 72 characters wide.
* Subject line must include a prefix that identifies the subsystem being changed. For example:
```
  tests: Remove unused definitions from the threading test
```
  If the file belongs to no subsystem, i.e. files at the root of the repo, then use the file name as prefix.
* Each commit should have a Message Body which concisely describes what is being changed, how and why. Unless a short subject line suffices.
* Each commit must contain a Signed-off-by line. Reference: https://gitlab.arm.com/iot/open-iot-sdk/sdk/-/blob/main/CONTRIBUTING.md#developers-certificate-of-origin

In addition:

* When fixing bugs caused by an older commit (not at the HEAD), please mention the issue's origin by providing the merge request number or commit SHA of the original change in the commit message.
* Each commit should build cleanly when applied on top of one another. This ensures that git bisect is not broken.
* Each commit should address a single identifiable issue and every attempt should be made to ensure that unrelated changes don't seep in.
* In general, commit diffs should be small and self contained unless new functionality is being added.
* Git commit guidelines are checked using GitLint

## Merge request workflow

Introduce your proposed changes in the Merge request description section with as much detail as you can. It should contain:
* What change you are adding/issue you are fixing
* How did you test it
* Anything reviewers can help with (outstanding issues to be addressed or multiple solutions available to the problem)
* Please ensure you have considered any security implications of your change by checking the security.md file. In your MR check the corresponding checkbox to confirm this.
* Please create separate merge requests for each topic; each merge request needs a clear unity of purpose.
  In particular, separate code formatting and style changes from functional changes. This makes each merge request’s true contribution clearer, so
  review is quicker and easier.
* Update any documentation (doxygen in header files or updating files in docs folder) related to your code changes and make it part of the merge request.
* Whenever you make changes to a file, please make sure to update the copyright year in the license header to the current gregorian calendar year.
* Ensure that all changes conform to the [Coding Style](#coding_style.md) and all commit messages conform to our commit guidelines (referenced above) before opening a merge request.
* Every new addition to an already opened merge request should be commented to inform the reviewers on what has changed.
* Keep a linear git history: no merge commits are allowed, always rebase your branch.
