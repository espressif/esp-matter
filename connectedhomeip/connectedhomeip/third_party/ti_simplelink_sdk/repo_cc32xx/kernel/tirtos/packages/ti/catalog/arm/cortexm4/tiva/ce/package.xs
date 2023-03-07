/*
 *  ======== package.xs ========
 *
 *! Revision History
 *! ================
 *! 02-Feb-2011 sasha	added init()
 *! 17-Apr-2009 agd     created
 */

/*
 *  ======== Package.init ========
 */
function init()
{
    /* Nothing to do, unless we are in Config Object Model */
    if (xdc.om.$name != "cfg") {
        return;
    }

    /* If Program.bld.rtsName is not 'null', this package will depend on
     * xdc.runtime and we want to establish that dependency early so that the
     * close() functions are invoked in the right order.
     */
    if (Program.build.rtsName !== null) {
    	xdc.loadPackage("xdc.runtime");
    }
}

/*
 *  ======== Package.getLibs ========
 */
function getLibs(prog)
{
    var libs = "lib/";
    var Boot = xdc.module('ti.catalog.arm.cortexm4.tiva.ce.Boot');

    /* return nothing if C files were provided to a custom library build */
    if (Boot.$private.cFilesProvided == true) {
        return (null);
    }

    if (Program.build.cfgScript == null || !Program.build.cfgScript.match(/\.tcf$/)) {
	libs += "Boot.a" + Program.build.target.suffix;
    }

    return (libs);
}
