# Custom ZCL Extensions

- [Custom ZCL Extensions](#Custom-ZCL-Extensions)
  - [Introduction](#Introduction)
  - [Custom Entities Overiew](#Custom-Entities-Overiew)
  - [Managing Custom Entities](#Managing-Custom-Entities)
  - [Seamless Functionality](#Seamless-Functionality)
  - [ZAP File Format](#ZAP-File-Format)
  - [Restoring information about custom ZCL entities](#Restoring-information-about-custom-ZCL-entities)
  - [User interface functionality](#User-interface-functionality)

## Introduction

The ZCL data model provides for the ability to extend the standard library with custom ZCL entities.

This allows for different manufacturers and users to create their own specific functionality that interacts with the ZCL data model.

e.g. if a user wants to provision additional functionality to the lighting cluster, then a custom extension of the lighting cluster is possible.

Therefore, ZAP must provide a way for users to be able to add custom entities to the set of ZCL entities used within their projects.

## Custom Entities Overiew

At its core, we can divide these custom ZCL Entities into the following categories:

- Standard Cluster Extensions
- Custom Clusters

Both of these entities may be composed of attributes and commands, which themselves are composed of references to sub-entities such as enums, command_args, bitmaps, etc.

In terms of file parsing, there is no difference between a file containing custom ZCL entities and a file that contains standard ZCL entities.

A user may wish to interchange and use different sets of XMLs for different projects due to reasons of wanting to integrate an external vendor's/user's ZCL functionality for specific projects.

Therefore, ZAP must have a way to manage these custom extensions on a per-project basis.

## Managing Custom Entities

The XML file format for custom ZCL extensions should still be the same format as the standard library.

The way to differentiate different sets of ZCL entities in the ZAP database is via associating each of those entities with a specific package, where each package corresponds to either one or more XML files.

The way we associate these packages with a specific project in the database is via the `SESSION_PACKAGE` table, which ties a package to a specific session.

To support custom XMLs, `SESSION_PACKAGE` contains a key that specifies the optionality of a package with respect to a session. [More particularly, custom XMLs should be marked as optional, but the core standard XML content should be marked as non-optional]

Question:

1. Do we want to require that a custom extension can only rely on the main non-optional XML?
2. If we have a situation where a custom extension can rely on other custom extensions, how do we want to maintain consistency when the prerequisite is deleted? Do we also want to delete the additional files? How do we want to warn the user? Do we need to introduce priorities for loading when we have chained reliance?

## Seamless Functionality

In order for the data to be used seamlessly, following needs to be assured:

- whenever session based queries are issued from the UI, joined across ZCL entities, an entire list of package IDs for the session needs to be considered.
- whenever generation helpers are querying the database to generate ZCL entities, an entire list of package IDs needs to be considered.

Apart from making sure that the queries all consider custom package IDs when querying for its package IDs, there should be nothing else special happening with these packages, they are just regular ZCL entities.

ZAP should provide some validation though, since there are specific ZCL rules around what IDs the custom entities can take. If there is a conflict between two XML files [possibly due to trying to overload], then ZAP should also point this out to the user, and ask/tell the user about what the desired resolution should be.

Essentially, once these XMLs are loaded, the ZCL entities between custom and standard entities should be treated identically, as long as queries respect the session's list of packageIds.

## ZAP File Format

When custom XML package is used for user configuration, the `*.zap` file will carry that information inside it, inline with the `package` values.

The so the description of this `package` key in the `*.zap` file would include:

```
{
    pathRelativity: "Type of path relationship"
    path: "Path relationship based on type of path relationship"
    version: "Version of the XML"
    type: "Type of ZCL package"
    optionality: "Is this package the main/required package"
    crc: "CRC for checking"
  }
```

The CRC here is the CRC associated w/ the `PACKAGE` table, and is used when loading a `*.zap` file.

## Restoring information about custom ZCL entities

When a `*.zap` file is loaded, that contains information about a custom XML file, the following process is handled:

- using path, version, and CRC the correct supporting package has to be located and loaded into the database if it hasn't yet been loaded
- user has to be given choice what to do if required custom XML file is not found.
- if the CRC that was recorded in the `*.zap` file is different from the one located in the ZAP file, then the UI should throw a warning about the data being different than expected, with options to confirm or replace the file.

## User interface functionality

Following are the UI functions that will have to be performed by ZAP to support this functionality:

- global UI support to locate and load a custom XML file.
- global UI support to unload custom XML file and delete the data about it from the database.
- session-specific UI support to add or remove the use of a specific custom XML file for a given session.

In addition and command-line argument to point to custom XML files should be supported when executing zap. This UI was specified by a design firm.
