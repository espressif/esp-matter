# 1 Introduction #

## 1.1 Purpose of this document ##

The purpose of this document is to describe the git organization and branching convention used by the Micrium OS software team. Since only a single repo will be used for all the Micrium OS stacks, having clear branches, commits and commit messages is important, this document describes how these should be handled.

## 1.2 Latest version of this document ##

The latest version of this document can always be found in Stash:

[https://stash.silabs.com/projects/MICRIUMOS/repos/micrium_os/browse/readme.md]
[convention]

[convention]:
https://stash.silabs.com/projects/MICRIUMOS/repos/micrium_os/browse/readme.md

**Note:** The reason why this document is in git is so that we can use pull-requests for changes and make it easier for people to track changes to the standard.

## 1.3 Changing this document ##

To make changes to this document you must create a branch. On that branch you should create the changes you wish to propose. Then create a pull request. That pull request needs to be approved by the people listed as reviewers in the next section.
Make sure that your commit has a good commit message.

## 1.4 Micrium OS Reviewers ##
* Alexandre Autotte (alautott)
* Olivier Belisle (olbelisl)
* Olivier Deschambault (oldescha)
* Jean-Francois Deschenes (jedesche)
* Cedric Migliorini (cemiglio)
* Marylise Monchalin (mamoncha)
* Ugo Pellerin (ugpeller)
* Yan Perron (yaperron)


## 1.4 References ##
This model must comply with Silicon Labs' standard for naming branches: https://confluence.silabs.com/display/PIT/Silabs+SDK+Branch+Standards.
This model has been discussed and agreed upon here: https://confluence.silabs.com/display/SOFTWAREPLATFORM/2018+Q3+Platform+Integration+Plan and should apply to all of the Software Platform team.


*****


# 2 Summary #
The branching model used is roughly described by this simplified model
![alt text](http://www.bitsnbites.eu/wp-content/uploads/2016/12/a-stable-mainline-branching-model-for-git.svg "Branching model").


*****


# 3 Branches #
We will have only `master`, `feature/*`, `bugfix/*`, `experimental/*` and `release/*` branches. No `hotfixes/*` branch is allowed at the moment.
We will manage how we provide new content to the rest of Silicon Labs by deciding when we advance super's submodule pointers for our repos.

## 3.1 Branches description ##

### 3.1.1 `master` branch ###
The latest commit in the `master` branch always contains the latest developments on the code. `feature/*` branches can be merged in `master` if we know this is going to be part of the next release.

### 3.1.2 `feature/*` branch ###
`feature/` branches should be used when creating new features or improvements. These branches should always branch off of `master`, from their release's `release/*` branch or from another `feature/*` branch and be merged back in the branch from which they originated from. Typically, epic-related branches will branch off `master` or `release/*` and story-related branches will branch off a `feature/*` epic-related branch.


### 3.1.3 `bugfix/*` branch ###
`bugfix/` branches should be used when fixing a bug. These branches should always branch off of `master`, from their release's `release/*` branch and be merged back in the branch they originated from.


### 3.1.4 `experimental/*` branch ###
`experimental/` branches should be used when taking a first look at something still in development or not destined to be released. These branches should always branch off of `master`, from their release's `release/*` branch and be merged back in the branch they originated from.


### 3.1.5 `release/*` branch ###
The `release/*` branches will contain the work for a given release of Gecko SDK. Each of these branches will always point to the latest commit associated to this particular release and should be working properly (building, running), even if not all the features for a given release are present. They will serve as the main branch in which to merge completed `bugfix/*` or `feature/*` branches for a given version. Once the release is completed, they will be merged back in `master` and any other subsequent `release/*` branches that were created since.


## 3.2 Branches naming convention ##

### 3.2.1 `master` branch ###
The `master` branch will be named `master`.

### 3.2.2 `release/*` branches ###
These branches will be named `release/gecko_sdk_<version>`, where:
* `<version>` is the version of the Gecko SDK (for example 2.4).


### 3.2.3 Other branches ###
These branches will follow this format: `<type>/<module>/<supertopic>/<JIRATICKET>-<short-description>`, where:
* `<type>` can be `feature`, `bugfix` or `experimental`
* `<module>` can be one of the folder names under 'rtos', in lower-case ('can', 'canopen', 'common', 'cpu', 'fs', 'io', 'kernel', 'net' or 'usb'). This can be omitted if a branch applies to several or all modules
* `<supertopic>` is an optional item that might apply to long running branches, often not used in `bugfix` branches
* `<JIRATICKET>` is the ticket number, for example MICRIUM-123
* `<short-description>` is a couple of words to know at a glance what the branch is about, separated by dashes `-` if needed


*****


# 4 Merges and Pull Requests #
Most of the merge operations will be done via pull request.
Before creating the pull request, to avoid any potential merge conflicts when doing the pull request and the following merge, the `<destination>` branch should be merged back in the `<source>` branch, where:
* `<destination>` is the branch in which `<source>` will be merged via the pull request
* `<source>`is the branch that will be merged in `<destination>` via the pull request

For example, merging `feature/kernel/foo` in `release/gecko_sdk_2.4` would require `release/gecko_sdk_2.4` to be merged back in `feature/kernel/foo` before creating the pull request to merge `feature/kernel/foo` in `release/gecko_sdk_2.4`.


Pull-requests and merges **MUST**  be done with the `no-ff` flag, in order to keep a trace of the branch that was merged, even though it will be deleted afterwards.
## 4.1 Merging `feature/*` in `feature/*` ##
Typically, this would happen for a story-related branch merging back in an epic-related branch. This can be done via pull request or via a simple merge. It is the developer's choice and the scope of the branch merged back might affect the choice. In any case, the code should be completely reviewed before being merged back in the `master` or `release/*` branch. If the merges were not done via pull request, a code review using another tool like Crucible must be performed. For complex reviews, use of Crucible is preferred over a simple pull request.
Once the pull request/merge operation is done, the remote `feature/*` branch **MUST** be deleted.
The commit message for the pull request/merge **MUST** contain the JIRA key associated to the issue solved, in the <key>-\### format, where:
* `<key>` is the JIRA Project key referencing the issue (MICRIUM most of the time)
* `###` is the JIRA issue number that this branch is fixing


## 4.2 Merging any branch except `release/*` in `release/*` or `master` ##
Necessarily via pull request. The pull request must be reviewed and approved by two persons, with at least one of them being on the Micrium OS Reviewers list (section 1.4 of this document) before being merged.
Once the pull request and the merge operations are done, the remote `feature/*` or `bugfix/*` branch **MUST** be deleted.
The commit message for the pull request/merge **MUST** contain the JIRA key associated to the issue solved, in the <key>-\### format, where:
* `<key>` is the JIRA Project key referencing the issue (MICRIUM most of the time)
* `###` is the JIRA issue number that this branch is fixing


## 4.3 Merging `release/*` in `master` ##
Necessarily via pull request or by one of the Micrium OS admins. If by pull request, it must be reviewed and approved by two persons, with at least one of them being on the Micrium OS Reviewers list (section 1.4 of this document) before being merged.
The commit message for the pull request/merge **MUST** contain the JIRA key associated to the issue solved, in the <key>-\### format, where:
* `<key>` is the JIRA Project key referencing the issue (MICRIUM most of the time)
* `###` is the JIRA issue number that this branch is fixing


# 5 Commits #
## 5.1 Commit Messages ##
Any commit message should reflect what has been done in that commit and be detailed enough. It can but is not required to contain the JIRA key.


## 5.2 Squashing Commits ##
If several commits are doing back and forth or if they should be grouped together, they need to be squashed together. Ideally, do this before pushing those commits. If it's too late, do it before anybody else pulls those commits, to avoid trouble. At any rate, it must be done before the merge. Squashing can easily be done with an interactive rebase.
The following is an excerpt from: https://www.devroom.io/2011/07/05/git-squash-your-latests-commits-into-one/
***
The command to accomplish that is:
`git rebase -i HEAD~3`
This will open up your editor with the following:
```
pick f392171 Added new feature X
pick ba9dd9a Added new elements to page design
pick df71a27 Updated CSS for new elements
```
Now you can tell git what to do with each commit. Let's keep the commit f392171, the one where we added our feature. We'll squash the following two commits into the first one - leaving us with one clean commit with features X in it, including the added element and CSS.
Change your file to this:
```
pick f392171 Added new feature X
squash ba9dd9a Added new elements to page design
squash df71a27 Updated CSS for new elements
```
When done, save and quit your editor. Git will now squash the commits into one. All done!
***
**This is the only case where re-writing history can be used.**
