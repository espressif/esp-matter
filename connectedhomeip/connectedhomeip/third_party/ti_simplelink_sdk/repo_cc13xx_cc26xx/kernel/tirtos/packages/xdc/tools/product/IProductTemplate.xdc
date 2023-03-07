/* 
 *  Copyright (c) 2009 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * */
package xdc.tools.product;

/*!
 *  ======== IProductTemplate ========
 *  Interface to provide examples for a product
 *
 *  This interface allows products to contribute examples to
 *  development environments like IDE's. This is an experimental
 *  interface and is subject to change.
 */
metaonly interface IProductTemplate
{
    /*!
     *  ======== FileDesc ========
     *  Structure defining properties of a file included in an example
     *
     *  @field(path)                    Path to file or folder relative to the
     *                                  package containing the implementation
     *                                  of `IProductTemplate`.
     *                                  If the path points to a file then the
     *                                  file is copied or linked into the
     *                                  project.
     *                                  If the path points to a directory then
     *                                  the entire directory and contents are
     *                                  copied into the project.
     *  @field(excludeFromBuild)        Flag indicating whether file should be
     *                                  excluded from build inside an IDE
     *                                  project
     *  @field(openOnCreation)          Flag indicating whether the file should
     *                                  be opened when a project containing the
     *                                  example file is created by the IDE.
     *  @field(copyIntoConfiguration)   Flag indicating whether the file should
     *                                  be copied into RTSC configuration. This
     *                                  flag applies only when the `isHybrid`
     *                                  flag of the
     *                                  {@link #TemplateInfo template} is set
     *                                  to `false`.
     *
     *  @field(action)                  Flag indicating whether the file
     *                                  should be copied into the project or
     *                                  linked in.  For example, the
     *                                  `{@link xdc.tools.product.trexgen trexgen}`
     *                                  tool recognizes the following flags:
     *                                  "COPY" - copy the file into the project
     *                                  "LINK" - link the file into the project
     *
     *  @field(targetDirectory)         The file or folder from the `path` field
     *                                  will be copied into this target
     *                                  directory within the project folder.
     */
    struct FileDesc {
        String path;                    /*! path to file or folder */
        Bool   excludeFromBuild;        /*! exclude from build */
        Bool   openOnCreation;          /*! open on project creation */
        Bool   copyIntoConfiguration;   /*! copy into RTSC configuration */
        String action;                  /*! Flag indicating whether the file
                                         *  should be copied into the project
                                         *  or linked to.
                                         */
        String   targetDirectory;       /*! directory into which the path
                                         *  file/folder should be copied into
                                         */
    };

    /*!
     *  ======== Board ========
     *  Structure defining properties of a board used in the example
     *
     *  @field(tirexName)               TIREX on Cloud recognizes board under
     *                                  its previously defined name. We have to
     *                                  use these names to get the tree
     *                                  structure that uses the names.
     */
    struct Board {
        String tiCloudName;               /*! name used in TI Cloud TIREX */
    };

    /*!
     *  ======== Filter ========
     *  Structure defining filter properties for an example
     *
     *  This structure allows products to define constraints for their
     *  examples. IDEs use the `Filter` properties of an example
     *  to decide whether the example should be presented to the user.
     *
     *  The filter is evaluated by performing an 'AND' operation on its
     *  individual elements. In other words, all the defined elements must
     *  evaluate to `true` for the filter to evaluate to `true`.
     *
     *  Each example typically defines an array of more than one filter;
     *  see {@link #TemplateInfo TemplateInfo.filterArr}. This filter array
     *  is evaluated using the 'OR' operation.  In other words, the example
     *  is presented to the user when any one of the filters evaluate
     *  to `true`.
     *
     *  Each filter property can be an arbitrary Java regular expression.
     *  In addition, each allows the user to define the NOT property
     *  by inserting a "~" character at the start of the string. For
     *  example, if the deviceFamily field is set to "~MSP430", the example
     *  will be displayed for all device families except "MSP430".
     *
     *  @field(deviceFamily)   String indicating the device family
     *                         eg. "MSP430","C6000"
     *  @field(deviceVariant)  String indicating device variant
     *                         eg. "C674X", "CortexA8"
     *  @field(deviceId)       String indicating the device part number
     *                         eg. "TMS320C6747"
     *  @field(endianness)     String indicating the device endianness
     *                         eg. "little", "big"
     *  @field(toolChain)      String indicating the tool chain
     *                         eg. "TI", "GNU"
     *  @field(outputFormat)   String indicating the object file format
     *                         eg. "COFF", "ELF"
     */
    struct Filter {
        String deviceFamily; 
        String deviceVariant;
        String deviceId;
        String endianness;
        String toolChain;
        String outputFormat;
    };

    /*!
     *  ======== TemplateInfo ========
     *  TemplateInfo structure
     *
     *  @field(title)             String containing the title of the template
     *  @field(name)              String containing the name of the project
     *                            created from this template
     *  @field(fileList)          Array of {@link #FileDesc} defining the
     *                            properties of the files contributed by this
     *                            example.
     *  @field(description)       String containing  description of example
     *  @field(target)            String containing RTSC target
     *  @field(platform)          String containing RTSC platform
     *  @field(buildProfile)      String containing RTSC build profile
     *  @field(linkerCommandFile) Linker command file for the example. If
     *                            this is set to the empty string then no
     *                            linker command file is added by the IDE
     *                            to the example. If this is not defined,
     *                            then the wizard picks the default
     *                            linker command file for the selected device.
     *  @field(compilerBuildOptions) Special compiler options required to
     *                               build template. For example the template
     *                               may need special -I and -D options
     *                               to build and these may be specified
     *                               here.
     *  @field(linkerBuildOptions)   Special linker options to build template
     *  @field(endianness)        String containing the endianness of the
     *                            project (little|big|be8|be32).
     *  @field(outputFormat)      String indicating object file format (COFF, ELF)
     *  @field(requiredProducts)  Products required to build this
     *                            example. Products are  identified by
     *                            their globally unique
     *                            {@link xdc.tools.product.IProduct#id}
     *                            eg. 'com.ti.bios'. Dependency on a
     *                            minimum version of a product may be
     *                            defined in the following manner
     *                            : <product-id>:<min-version>
     *  @field(xdcToolsVersion)   String containing XDCTools the exact version
     *                            required eg. '3.24.6.63'.  Note that this
     *                            number must be an 'eclipse' version number:
     *                            no leading 0's in the first three segments
     *                            and the forth is treated as an arbitrary
     *                            string.
     *
     *                            If this string is `undefined`, any
     *                            version of XDCtools will be allowed.
     *
     *                            If the exact version doesn't exist for some
     *                            reason, the project will still be
     *                            created, but the project will give a 'this
     *                            version of XDCtools doesn't exist' warning.
     *                            The user can then select a version that is
     *                            hopefully compatible.
     *
     *  @field(groups)            Array of strings referring example groups
     *                            that a particular example may
     *                            belong. Products may
     *                            provide examples that are part of an
     *                            existing example group eg."Empty Projects"
     *                            that are defined elsewhere. The
     *                            groups are identified by an unique id.
     *  @field(configOnly)        Flag indicating to the IDE
     *                            whether example contributes only
     *                            to a RTSC configuration project.
     *  @field(configuroOptions)  This string contains options that are passed
     *                            to `xdc.tools.configuro`.  You must be
     *                            careful to quote embedded special characters
     *                            in this string in such a way that they can
     *                            be directly embedded in an XML file.  For
     *                            example, to pass '-foo "bar"' to `configuro`
     *                            you must use the string
     *                            '-foo &quot;bar&quot;'.
     *
     *  @field(isHybrid)          Flag indicating to the IDE whether example
     *                            contains RTSC configuration and target
     *                            content files in one project.
     *                            If this field is set to `true`, then the IDE
     *                            consuming this example will create one project
     *                            with all the files. Otherwise, multiple
     *                            projects will be created - one containing the
     *                            target content and the other containing the
     *                            RTSC configuration files. This flag applies
     *                            only when `configOnly` flag is set to `false`.
     *
     *  @field(filterArr)         Array of {@link #Filter}. Used to specify
     *                            the constraints for a particular
     *                            example. The filter array is evaluated using
     *                            an OR operation on the individual array
     *                            elements. Note that individual elements
     *                            within a {@link #Filter filter} is evaluated
     *                            with the 'AND' operation.
     *
     *  @field(ignoreDefaults)    Property {@link #Filter filter} commonly
     *                            specifies a device. Each device has an XML
     *                            file with default settings, which are applied
     *                            to a project, in addition to properties
     *                            specified here. Whe this parameter is set to
     *                            'true', these default options are ignored.
     *
     *  @field(options)           Comma separated list of options used to
     *                            specify various configurable items for this
     *                            template.  For example, the
     *                            `{@link xdc.tools.product.trexgen trexgen}`
     *                            tool recognizes the following flags:
     *                                "NPW" - display this example in the New
     *                                        Project Wizard, and
     *                                "TREX" - display this example in the
     *                                         Resource Explorer
     *
     *  @field(references)        Comma separated list of referenced project
     *                            names
     *
     *  @field(buildCommandFlags) Comma separated list of build-command flags.
     *
     *  @field(launchWizard)      Flag indicating to launch the New Project
     *                            wizard allowing the user to adjust the
     *                            details.
     *
     *  @field(preBuildStep)      Shell cmd to run before the build. Cmd is
     *                            run within the debug/release directory
     *
     *  @field(postBuildStep)     Shell cmd to run after the build. Cmd is run
     *                            within the debug/release directory
     */
    struct TemplateInfo {
        String   title;              /*! Title of this example */
        String   name;               /*! Name of the project imported from
                                      *  this template
                                      */
        FileDesc fileList[];         /*! List of files along with properties
                                      *  for this example
                                      */
        String   description;        /*! Description of this example */
        String   target;             /*! RTSC target */
        String   platform;           /*! RTSC platform */
        Board    board;              /*! Properties of the board */
        String   buildProfile;       /*! RTSC build profile */
        String   linkerCommandFile;  /*! Linker file for this example */
        String   compilerBuildOptions; /*! Special compiler options */
        String   linkerBuildOptions; /*! Special linker options */
        String   endianness;         /*! endian setting (little|big|be8|be32) */
        String   outputFormat;       /*! format of object file (COFF or ELF) */
        String   requiredProducts[]; /*! List of products required to build
                                      *  this example
                                      */
        String   xdcToolsVersion;    /*! Version of xdctools this template
                                      *  requires eg. '3.24.6.63' */
        String   groups[];           /*! Array of group ids for groups
                                      * containing this example */
        Bool     legacyTcf;          /* obsolete option */
        String   configuroOptions;   /*! configuro options */
        Bool     configOnly;         /*! Indicates whether only a RTSC
                                      *  configuration project should
                                      *  be created for this example.
                                      */
        Bool     isHybrid;           /*! Indicates whether application and
                                      * configuration content exists in
                                      * one project
                                      */
        Bool     isFragment;         /*! Indicates whether this template
                                      *  can be applied "incrementally"
                                      *  to an already existing project
                                      */
        Filter   filterArr[];        /*! Array of filters for this example */
        Bool     ignoreDefaults;     /*! Indicates if settings from the
                                      * device XML file are ignored.
                                      */
        String   options;            /*! Comma separated attributes */
        String   references;         /*! Comma separated list of referenced
                                      * project names */
        String   buildCommandFlags;  /*! Comma seperated list of build-command
                                      * flags */
        Bool     launchWizard;       /*! Flag indicating to launch the New
                                      * Project Wizard allowing the user to
                                      * adjust the details */
        String   preBuildStep;       /*! Shell cmd to run before the build.
                                      * Cmd is run within the debug/release
                                      * directory */
        String   postBuildStep;      /*! Shell cmd to run after the build. Cmd
                                      * is run within the debug/release
                                      * directory */
        String   projectType;        /*! type of project.  If this field is
                                      *  not set, a default will be used that
                                      *  is based on other settings.  If it is
                                      *  set to "ccs", the project will be a
                                      *  "Code Composer Studio" project.
                                      */
        String   outputType;         /*! type of output produced by this
                                      *  project: "executable" or
                                      *  "staticLibrary", or "any".  The "any"
                                      *  type indicates that this project
                                      *  template applies to type of output,
                                      *  executable, library, etc.
                                      *
                                      *  If this field is not set "any" is
                                      *  used.
                                      */
    };

    /*!
     *  ======== TemplateGroup ========
     *  TemplateGroup structure
     *
     *  This structure may be used to define a hierarchy of examples for the
     *  product. Examples may be logically organized into groups with a unique
     *  `id` and may specify membership of other groups by referring to their
     *  ids in the `groups` array. In this manner, the example producer
     *  can define a tree topology of examples for their product. Once the
     *  template groups are defined, the individual examples may specify their
     *  membership within a template group in the `groups` array of
     *  {@link #TemplateInfo}. The example provider may specify all the defined
     *  groups for their product in {@link #templateGroupArr}.
     *
     *  @field(id) Unique id for the template group
     *  @field(name) Name of the group
     *  @field(description) Description of the group
     *  @field(groups) Array of group ids used to specify
     *                 membership of other groups.
     */
    struct TemplateGroup {
        String id;
        String name;
        String description;
        String groups[];
    };

    /*!
     *  ======== templateArr ========
     *  Examples contained in the product
     */
    config TemplateInfo templateArr[];

    /*!
     *  ======== templateGroupArr ========
     *  Array of template group ids
     *
     *  This array may be optionally specified by Products that
     *  organize their examples into groups.
     */
    config TemplateGroup templateGroupArr[] = [];
}
/*
 *  @(#) xdc.tools.product; 1, 0, 0,3; 2-18-2019 11:04:00; /db/ztree/library/trees/xdctools/xdctools-h03/src/
 */

