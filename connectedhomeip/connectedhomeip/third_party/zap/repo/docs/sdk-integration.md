# SDK integration with zap tool

The purpose of this document is to provide SDK developers with the instructions how to integrate their and their customers' workflow with the zap tool.

The SDK provides the embedded code and the implementation of the actual ZCL embedded layer. It may be written in C++, C or any other language. For the purpose of this document the SDK is essentialy a body of code that implements the ZCL concepts, such as clusters, attributes, commands, etc.

This body of code can then use the ZAP tool to gain:

- The generation of static content, such as constant ID values, enums, etc.
- The ability for the end-users to use the UI to configure their application, and then generate application-specific content for their application itself.
- The ability to feed extensions into the zap tool, to gain additional UI elements and other options that tailor the zap tool for a specific SDK.

To gain these abilities, the SDK should provide data to zap. The data required is:

- **ZCL metafiles**: these are the metafiles that provide information about the ZCL specification to zap tool. Examples of this (and currently supported) are Silicon Labs XML ZCL metadata or the Zigbee official XML format of the ZCL metafiles. Examples of these metafiles are available from the Zigbee website (http://zigbee.org).

  The ZAP project itself provides examples as well, see the `zcl-builtin/` directory. These are used internally for unit testing, but they are also packaged with the application as examples. They can be a good starting point for SDK developers, but ultimately it's the responsibility of SDK developers to provide the accurate XML packages for their SDK, and let ZAP know where they can be loaded from on the local hard drive.

- **generation templates and extensions**: these are actual ZAP templates (`*.zapt` files), which correspond to the files that need to be generated. The entry point is a JSON formatted file, usually named `gen-templates.json` which lists all the individual template files and additional extension options. This document will deal with the `gen-templates.json` file itself, however for development of each individual template, you can follow the [template tutorial](template-tutorial.md)

These two categories are decoupled and loaded separately, mostly because their maintenace processes are different. The ZCL metafiles themselves follow the ZCL specs. If the ZCL spec does not change, then there is no need to change ZCL metafiles.

The second category (generation templates and extesions), however, follow the SDK itself. The generation templates will change, if the embedded code that they are targetting changes. Hence the breakup into these two categories.

- **sdk.json**: This json file is a shortcut to the previous two meta-data elements. It allows for a single file to be used as an interface to one or more ZCL metafiles and generation templates. It can also list `*.zap` files which can be used to target automatic regeneration. See example under `test/resource/meta/sdk.json`.

## ZCL metafiles

The ZAP tool loads the ZCL metadata starting from the top-level ZCL metafile.
This file does not contain the ZCL cluster/attributes itself, but mostly provides a top-level linking, pointing to individual XML files that contain individual clusters and such.

Currently supported formats for this file are:

| Type           | Example                                | Description                                                                                                                                                                                                                                                                                                                                               |
| -------------- | -------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| zcl.json       | zcl-builtin/silabs/zcl.json            | Native zap file, JSON format.                                                                                                                                                                                                                                                                                                                             |
| library.xml    | zcl-builtin/dotdot/library.xml         | Format of the aggregation file as currently planned for Zigbee official ZCL metadata. XML format.                                                                                                                                                                                                                                                         |
| zcl.properties | zcl-builtin/silabs/zcl-test.properties | Legacy file format, used by Silicon Labs in older ZCL products, such as appbuilder and Simplicity Studio. Should not be used for new work, as this format has some deficiencies that are not going to be fixed, and in there is no plan to keep adding future-proofing features to this file format. File format is a format of the java properties file. |

All these aggregation files, are refering to the XML files that contain the ZCL metadata. The format of these xml files are of 2 different types:

| Type             | Example                        | Description                                                                                                                                                                                                                     |
| ---------------- | ------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Silicon Labs XML | zcl-builtin/silabs/general.xml | Silicon Labs format for the ZCL metadata, in existence since about 2016. Well tested format, however it does contain an occasional reference and data point that is not ZCL specific, but Silicon Labs implementation specific. |
| Zigbee XML       | zcl-builtin/dotdot/Basic.xml   | Zigbee supported format. At the time of writing this format is still a work in progress, but it is expected to become the official format of the ZCL metadata.                                                                  |

If the aggregation metafile is of a type `zcl.json` or `zcl.properties`, then the XML files it refers to, are assumed to be of the Silicon Labs XML format.
If the aggregation metafile is using `library.xml` type, then the XML files provided by it are assumed to be of the Zigbee XML format.

## Generation templates and extensions

Generation templates and extensions are provided by the SDK. They are the input to the ZAP tool. They control generation and tailor the ZAP tool specifically to a given SDK, by providing the correct details of the implementation that ZAP cares about.

Neither the templates or extensions are mandatory. The ZAP tool can operate without them, simply as an editor of zap files, however there will be little use of the tool, if a developer can't generate useful output.

The entry point to ZAP reading the generation templates from the SDK is a file typically called `gen-templates.json`. This file provides the ability for the SDK to configure ZAP to do the generation work it requires.

The file is a JSON-formatted JS object, with the following structure:

| Key       | Value type                                                              | Value meaning                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| --------- | ----------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| name      | String                                                                  | Name of the template. This will be shown in UI for selection                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| version   | String                                                                  | Version of the template. This will be shown in the UI for selection                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| options   | JS object, containing keys that can map to further key/value maps       | This is a mechanism that allows the adding of additional category/key/value triplets to the package options. If the SDK wants to provide an additional generic categorized key/value properties, then you can provide an object here. Top-level keys map to either an Object itself, or a String, which will be interpreted as a path to a JSON serialized object. Objects should contain Strings as values of the elements. This data is loaded into the OPTIONS table. The toplevel key is used as OPTION\*CATEGORY column, intermediate key is used as OPTION_CODE column, and the final String value of the sub-key is used as OPTION_LABEL column. These can be accessed from templates via `template_options` iterator, so you can iterate over a given category. They can also be accessed by the UI in certain cases. |
| zcl       | Object, keyed by ZCL entitites, containing arrays of ZCL modifications. | This is a mechanism that allows the SDK to attach custom configurable data points to the ZCL entities. This could be used, for example, to configure special SDK-specific per-cluster properties. It is a way to associate the cluster with the SDK directories for generation of build files, or any similar SDK customization that is based on ZCL entities.                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| templates | Array of JS objects containing keys: \_path\*, _name_, _output_         | Lists the individual files. Path should be relative against the location of the `gen-template.json` itself, output is the name of the generated file, and name is a human readable name that might show in the UI.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| helpers   | Array of strings, representing relative paths.                          | This mechanism is how the SDK can create their own helpers in JavaScript, should this be needed. It is vastly prefered to stick to provided helpers. If you need additional helpers it is also prefered to reach to ZAP developers to have your required feature added. This will ensure backwards compatibility and proper future-proofing and so on. However, as a last resort, you can add your own helpers using this mechanism. The files listed will be treated as node.js modules, and any exported symbol will be registered as a helper.                                                                                                                                                                                                                                                                             |
| override  | Single relative path.                                                   | Specifies the override JS file, which is a file containing functions that provide overrides for the overridable functions. This is an advanced mechanism how an SDK can provide override functions for some core functionality, such as mapping atomic types to the native types, and similar.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |

The following sections describe the details specific for use in certain more complex areas of the `gen-template.json`.

## Templates key: options

Options are loaded into the database, keyed to the given generation package. While you can always use them in templates via the `template_options` key, certain keys have special meanings.
The following is the list of special meanings:

| Category          | Meaning                                                                                                                                                                                                     |
| ----------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| manufacturerCodes | This category backs a code/label map of valid manufacturer codes. They can be used in UI when selecting manufacturers.                                                                                      |
| types             | This category backs a code/label map of type mappings. For a code, you can use any of the atomic or other ZCL types, and label is an override for a generator, detemining what type it shuld translate into |
| generator         | This category contains generation specific options. See section bellow.                                                                                                                                     |

### Generator options

Following table lists generator options supported by the template generator category.

| Key                        | Value                                                                                                                                                                                                                                                                  |
| -------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| postProcessMulti           | a string, representing command to execute, with the list of generated files appended in a single command line. This would be typically used by zap to execute commands like `uncrustify`, or similar post-processing, which can take multiple file names as arguments. |
| postProcessSingle          | a string, representing a command to execute for each individual file appended in a single command line. This would be typically used by commands or scripts that clean up the generated files, and can only take one file name at a time as an argument.               |
| postProcessConditionalFile | a path, representing a file. If this file does not exist, then a specified post-process action will not get executed.                                                                                                                                                  |
| routeErrToOut              | a boolean flag. If it's set to true, then any stderr from the post-process will be routed to stdout. This is due that a driving process sometimes has trouble dealing with both stdout and stderr, so this flag might help you preserve the logs.                      |

## Template key: override

This key specifies tha path of the "override functions file".
Override functions file is a javascript file that will be loaded by the generation engine to override any of the "Overridable" function.

The function provided as an override will be executed FIRST, if it returns a value, that value will be used and original function will not be called at all. However, if the override function throws an exception, then it will be ignored and original overridable function, provided by the stock zap application, will be called instead.

There are specific overridable functions that can be overriden and the following table lists them.

| Function        | Arguments                                | Functionality                                                                                                                                                                                                                                             |
| --------------- | ---------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| atomicType()    | object containing keys `name` and `size` | Function returns the native type, that represents a given atomic type. Default version provided by the zap tool lists the mappings as are used by the Silicon Labs Gecko SDK, but an SDK developer can override this by providing this override function. |
| nonAtomicType() | object containing key `name`             | Function returns the native type, that represents a type passed to any of the type functions that doesn't resolve into atomic or any other ZCL types (bitmaps, enums, etc.)                                                                               |

## Template key: zcl

ZCL customization are provided in a form of entity-based customization records. Let's learn by example:

```
{
  "zcl": {
    "cluster": {
      "toggleLedOnAttributeChange": {
        "type": "boolean",
        "configurability": "editable",
        "label": "Cluster will toggle LED when one of its attributes is changed.",
        "globalDefault": "false"
      },
      "sdkPath": {
        "type": "path",
        "configurability": "hidden",
        "defaults": "clusterPaths.json"
      }
    },
    "command": {
      "lcdMessage": {
        "type": "text",
        "configurability": "editable",
        "label": "LCD message to display when command received.",
        "defaults": [
          {
            "clusterCode": 0x0003,
            "commandCode": 0x00,
            "source": "client",
            "value": "Identifying!"
          }
        ]
      }
    }
  }
}
```

This example shows two configuration, provided by the SDK. It adds the following functionality:

- It adds to each cluster a new of property, called `toggleLedOnAttributeChange`. That property is of a type boolean and can be editable by the end user. It is set to `false` by default. When ZAP UI shows the configuration of a cluster, it will show an additional checkbox, labeled "Cluster will toggle LED when one of its attributes is changed." The end user will have ability to turn that on or off, and that value will be recorded in the `*.zap` file. The recorded value will also be available in generation templates, so that the SDK developer can inject the proper generation of the code for the handling of this custom property.
- It adds a new property of type `path` to each cluster, called "sdkPath". It's configurability is `hidden` which means that the UI will not show this property. However, generation templates will have access to it. Default values are not stored directly here, but in an external file called `clusterPaths.json`, as specified by the key `"defaults": "clusterPaths.json"`. This can, for example, be used when you are using templates to generate a build file or some other compile-time artifact that needs to know directories or filenames of the cluster implementation code.
- It adds a property `lcdMessage` to each ZCL command. The SDK implementation could (as an example), show the given messages on the LCD that the device has built-in. Configurability is `editable`, which means that the UI will show a text field for end-users to configure this value. Global defaults are missing, so it is `null`, but there is a provided default value for the ZCL `Identify` command, and it is configured so that it shows the text `Identifying!` on the LCD screen.

Obviously, all of the custom properties need to have an actual implementation on the SDK backing them up, ZAP is just a mechanism for configuring them, and passing them through to the generation templates, and neither knows, nor assumes what a real meaning of these properties may be.

Following are descriptions of some keys that you can configure:

| Key                                       | Possible Values                                                                      | Description                                                                                                                                                                                                                                         |
| ----------------------------------------- | ------------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| top-level key under `zcl`                 | `cluster`<br>`command`<br>`attribute`<br>`deviceType`<br>`event`<br>`attributeTypes` | Specifies what kind of ZCL entity this customization applies to.                                                                                                                                                                                    |
| `type` key under individual customization | `boolean`<br>`integer`<br>`text`<br>`path`                                           | Specifies what type this particular customization option is. This mostly affects the UI.                                                                                                                                                            |
| `globalDefault`                           | anything                                                                             | Specifies the default value for all entities, except the ones defined in the `defaults` array.                                                                                                                                                      |
| `configurability`                         | `hidden`<br>`visible`<br>`editable`                                                  | Specifies how zap UI will treat this value. If it's `hidden` it will not show it at all, if it's `visible` it will be shown, but users won't be able to change it, and if it's `editable`, then users can provide their own values for each entity. |
