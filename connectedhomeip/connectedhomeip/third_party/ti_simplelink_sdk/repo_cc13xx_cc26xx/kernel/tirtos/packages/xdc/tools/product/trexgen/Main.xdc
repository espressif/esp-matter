/*
 *  Copyright 2019 by Texas Instruments Incorporated.
 *
 */

/*!
 *  ======== Main ========
 *  Command-line tool for generating projectspec files for RTSC content
 *
 *  The `trexgen` tool allows RTSC content producers to
 *  create projectspec files that allow their content to be
 *  integrated in the CCS Resource Explorer environment. As input,
 *  the tool requires
 *  @p(blist)
 *      - a RTSC module implementing {@link xdc.tools.product.IProduct}
 *  @p
 *  Refer to {@link ./doc-files/ExampleProduct.xdc ExampleProduct}
 *  for an implementation of {@link xdc.tools.product.IProduct IProduct}.
 *
 *  The tool generates starter examples that will show up
 *  in the Resource explorer. This is accomplished by implementing
 *  {@link xdc.tools.product.IProductTemplate IProductTemplate} and setting
 *  the {@link xdc.tools.product.IProduct#templateModule templateModule}
 *  configuration parameter of the {@link xdc.tools.product.IProduct IProduct}
 *  implementation to the name of the implementation module.
 *  Refer to {@link ./doc-files/Examples.xdc Examples}
 *  for a sample implementation of {@link
 *  xdc.tools.product.IProductTemplate IProductTemplate}.
 *
 *  The tool operates on a product that is either specified by the `-p`
 *  option or is present in the folder from which the tool is executed.
 *  The tool will create the resources sub-folder in the output directory
 *  specified with `-o` option. If the `-o`
 *  option is not specified the resources is created in the product root
 *  directory.
 *
 *  The tool will search the repositories specified in the module
 *  implementing {@link xdc.tools.product.IProduct} for RTSC platforms.
 *  This search can be disabled by specifying the`--disable_repo_search`
 *  option. The user needs to ensure that the repositories are installed
 *  in the product root directory before using the tool.
 *
 *  @a(Example)
 *  @p(code)
 *      xs xdc.tools.product.trexgen
 *                      -p exampleprod_1_0_0_00
 *                      -m xdc.tools.product.plugingen.examples.ExampleProduct
 *  @p
 */
metaonly module Main inherits xdc.tools.ICmd
{
    override config String usage[] = [
        '[-p product_root_directory]',
        ' -m module',
        '[-o outdir]'
    ];

instance:

    /*!
     *  ======== productDir ========
     *  Product root directory
     *
     *  This option names the product root directory that is used by the
     *  tool to generate the projectspec files.  For example, it's declared
     *  repositories are added to the package path and are searched for
     *  contributed platforms.
     *
     *  If the '-o' option is not specified, the tool generates the resources
     *  directory in the product root directory.
     *
     *  If this option is not specified, the product root directory is
     *  assumed to be `./`.
     *
     */
    @CommandOption("p")
    config String productDir = "./";

    /*!
     *  ======== productModule ========
     *  Input description of the plug-in to be specified
     *
     *  This required option names a module that implements
     *  `{@link xdc.tools.product.IProduct}`.
     */
    @CommandOption("m")
    config String productModule;

    /*!
     *  ======== outputDir ========
     *  Output directory in which the plugin will be generated.
     *
     *  This option names the directory in which the project spec
     *  will be generated in a sub-directory named `eclipse`.
     *  If this option is not specified the tool will generate
     *  the plugin in the product root directory specified with
     *  the '-p' option.
     */
    @CommandOption("o")
    config String outputDir = null;

    /*!
     *  ======== generationFormat ========
     *  Plugin generation format
     */
    @CommandOption("generation_format")
    config String generationFormat = null;

    /*!
     *  ======== run ========
     */
    override Any run(xdc.tools.Cmdr.Instance cmdr, String args[]);
}
/*
 *  @(#) xdc.tools.product.trexgen; 1, 0, 0,3; 2-18-2019 11:04:07; /db/ztree/library/trees/xdctools/xdctools-h03/src/
 */

