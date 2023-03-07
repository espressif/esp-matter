/*
 * Copyright (c) 2015-2017 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== Settings.xs ========
 *
 */

/*
 *  ======== module$use ========
 */
function module$use()
{
    /*  This module is deprecated; it has been replaced by
     *  ti.posix.tirtos.Settings. Search the package path
     *  for the new module.
     */
    var pname = xdc.findFile("ti/posix/tirtos/Settings.xdc");

    if (pname == null) {
        /*  The module is not on the package path. Log an
         *  error and terminate the configuration phase.
         */
        this.$logError("This module is deprecated, it has been replaced "
            + "by ti.posix.tirtos.Settings. Please update your config "
            + "script and add the package ti.posix.tirtos to your package "
            + "path.", this);
    }

    /*  Found the module on the package path. Check if the user
     *  already added it to the config.
     */
    else if ("ti.posix.tirtos.Settings" in xdc.om) {
        /*  User added the new module but forgot to remove this old one.
         *  Log a warning to inform the user but don't configure the new
         *  module. Assume that much has been done in the config script.
         */
        this.$logWarning("This module is deprecated, it has been replaced "
            + "by ti.posix.tirtos.Settings, which has already been added "
            + "to the configuration. Please remove this module from your "
            + "config script.", this);
    }
    else {
        /*  New module not in config. Load and configure the new module
         *  on behalf of this module. Log a warning to inform the user.
         */
        var Settings = xdc.useModule('ti.posix.tirtos.Settings');
        Settings.enableMutexPriority = this.supportsMutexPriority;
        Settings.debug = this.debug;

        this.$logWarning("This module is deprecated, it has been replaced "
            + "by ti.posix.tirtos.Settings. Please update your config "
            + "script. Loading and configuring the new module on behalf "
            + "of this module.", this);
    }
}
