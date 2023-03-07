# What is autobot

The Open IoT SDK is shipped with several components pinned to a particular revision. These components need updating regularly so we can catch any integration issues as early as possible. The "autobot" bot aids quick integration with the latest revisions of components by creating Merge Requests (MRs) for each component.

# How does autobot work

The SDK primarily fetches libraries using two CMake functions:

* FetchContent_Declare
* ExternalProject_Add

The function calls are of the structure

```
FetchContent_Declare(
    <component_label>
    GIT_REPOSITORY  <component_git_url>
    GIT_TAG         <component_revision>
)
```

For example,

```
FetchContent_Declare(
    azure-sdk
    GIT_REPOSITORY  https://github.com/Azure/azure-iot-sdk-c
    GIT_TAG         LTS_01_2022_Ref01
    GIT_SHALLOW     ON
    GIT_PROGRESS    ON
)
```

To update the SDK repository to contain the latest revision of the component, logic used by autobot is:

* Parse the SDK repository and use regular expressions to look for components with in `.cmake` files.
* For every component:
    * If an MR is not open for the component:
        * If a new git revision of the component is available, autobot creates a new git commit in a new branch and creates an MR for the change.
        * If a new git revision of the component is not available, component is already up-to-date. autobot has nothing to do.
    * If an MR is open:
        * If a new git revision of the component is available, autobot updates the existing MR.
        * If a new git revision of the component is not available, the existing MR is already updating to the latest component revision. autobot does not update the MR.

:information_source: autobot adds a string in MR description BEGIN:<GIT_REPOSITORY>:END to mark which MR belong to which component.
:information_source: autobot runs in a scheduled pipeline which can be accessed from GitLab CI.
