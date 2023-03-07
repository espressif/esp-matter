/******************************************************************************
*
*   filebrowser.js - CC31xx/CC32xx SimpleLink Image Creator
*
*   Copyright (C) 2016-2019 Texas Instruments Incorporated
*
*   All rights reserved. Property of Texas Instruments Incorporated.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*
*   The program may not be used without the written permission of
*   Texas Instruments Incorporated or against the terms and conditions
*   stipulated in the agreement under which this program has been supplied,
*   and under no circumstances can it be used with non-TI connectivity device.
*
******************************************************************************/
var fileBrowser = (function($) {

//var PemMessage = "This file is a PEM formatted certificate file. The SimpleLink Wi-Fi device accepts only PEM formatted certificate files that their line ending is in UNIX format (LF only instead of CRLF). Would you like to replace the line ending format of this file to UNIX format? Notice: This change will not change the original file"
var PemMessage = "CRLF will be replaced by LF at the end of this file. The original file will not be changed."
var sfDeviceMCUImgName = "mcuflashimg.bin";
var srDeviceMCUImgName = "mcuimg.bin";
var checkedClass = "fa fa-check-square-o"; //"fa fa-check-circle-o";
var uncheckedClass = "fa fa-square-o"; //"fa fa-circle-o";
var checkedClass1 = "fa-check-square-o"; //"fa-check-circle-o";
var uncheckedClass1 = "fa-square-o"; //"fa-circle-o";
var gHttpKeyFileName    = "";
var gHttpCertFileName   = "";
var gHttpCaCertFileName = "";
var callbackClose = undefined;
var callbackSave = undefined;
var callbackDeleteHttpCert = undefined;
var callbackDeleteHttpKey = undefined;
var callbackDeleteHttpCaCert = undefined;

var folderDiv = (
"<div class='fb-nesting'>" +
"<span class='fb-text'>" +
"<span id='check' class='fb-text " + uncheckedClass + "'></span>" +
"<span id='icon' class='fb-ti-red fa fa-folder-open'></span>" +
"<span id='actions' class='fb-invisible fb-text'>" +
"<span id='rename' class='fa fa-edit fb-actions' title='Rename'></span>" +

//"<span id='add' class='fa fa-plus-circle fb-actions' title='New Folder'></span>" +
"<span id='add' class='fa fa-folder-o fb-actions' title='New Folder'></span>" +

//"<span id='upload' class='fa fa-upload fb-actions' title='Upload'></span>" +
"<span id='upload' class='fa fa-file-text-o fb-actions' title='Add File'></span>" +

"<span id='del' class='fa fa-close fb-actions' title='Delete'></span>" +
"</span>" + 
"<span id='text' class='fb-unsel fb-text2'></span>" +
"</span>" + 
"<div id='content'></div>" +
"</div>"
);

function getPemMessage()
{
    return PemMessage;
}
function updateHttpKeyFileName(name) {

    gHttpKeyFileName = name;
}

function updateHttpCertFileName(name) {

    gHttpCertFileName = name;
}

function updateHttpCaCertFileName(name) {

    gHttpCaCertFileName = name;
}

function Folder(name, parent) {
    if (name) {
        this.name = name.toLowerCase();
    }
    
    this.folders = [];
    this.files = [];
    this.parent = parent;
    this.node = null;
    
}

Folder.prototype.render = function(args) {
    if (!this.node) {
        this.node = $(folderDiv)
        //this.node.find("#text").html(this.name);
        
        if (args["check"] === false) {
            this.node.find("#check").first().hide();
        } else {
            this.node.find("#check").on("click", {that: this}, this.toggleCheck);
        }
        
        this.node.find("#icon").on("click", {node: this.node}, this.toggle);
        if (args["rename"] === false) {
            this.node.find("#rename").hide();
        } else {
            this.node.find("#rename").on("click", {that: this}, this.propsWithDialog);
        }
        
        this.node.find("#add").on("click", {that: this}, this.addFolderWithDialog);
        this.node.find("#upload").on("click", {that: this}, uploadFileWithBrowse);
        this.node.find("#del").on("click", {that: this}, this.removeWithDialog);
        
        this.node.unbind("dragenter").unbind("dragover").unbind("drop").unbind("dragleave");
        this.node.find("#content").unbind("dragenter").unbind("dragover").unbind("drop").unbind("dragleave");
        
        
        this.node.on("dragenter", {that: this}, this.dragenter);
        this.node.on("dragleave", {that: this}, this.dragleave);
        this.node.on("dragover", this.dragover);
        this.node.on("drop", {that: this}, this.drop);
        
        
        //this.node.find("#content").on("dragenter", {that: this}, this.dragenter);
        //this.node.find("#content").on("dragleave", {that: this}, this.dragleave);
        this.node.find("#content").on("dragover", this.dragover);
        this.node.find("#content").on("drop", {that: this}, this.drop);
        
        
    }
    
    if (args) {
        this.args = args
    } else {
        args = this.args;
    }
    
    this.node.find("#text").first().text(this.name);
    var content = this.node.find("#content").first();
    
    $.each(this.folders, function(i, folder) {
        content.append(folder.render(args));
    });
        
    $.each(this.files, function(i, file) {
        content.append(file.render(args));
    });
        
    return this.node;
}

Folder.prototype.dump = function() {
    function replacer(k, v) {
        if (k == "node") {
            return undefined;
        } else if (k == "parent") {
            return undefined;
        } else if (k == "args") {
            return undefined;
        }
        
        return v;
    }
    
    return JSON.stringify(this, replacer, 4);
}

Folder.prototype.addFolder = function(name) {
    var folder = new Folder(name, this);
    this.folders.push(folder);
    return folder;
}

Folder.prototype.addFile = function(name, attrs) {
    var file = new File(name, attrs, this);
    this.files.push(file);
    //return this;
    return file;
}

Folder.prototype.toggle = function(event) {
    var node = event.data.node;
    
    /*
    node.find("#content").slideToggle("fast", function() {
        var icon = node.find("#icon").first();
        if ($(this).is(":visible")) {
            icon.addClass("fa fa-folder").addClass("fa fa-folder-open");
        } else {
            icon.removeClass("fa fa-folder-open").addClass("fa fa-folder");
        }
    });
    */
        
    var content = node.find("#content")
    var icon = node.find("#icon").first();
    
    content.toggle()
    
    if (content.is(":visible")) {
        icon.removeClass("fa fa-folder").addClass("fa fa-folder-open");
    } else {
        icon.removeClass("fa fa-folder-open").addClass("fa fa-folder");
    }
}

Folder.prototype.remove = function(event) {
    var that = event.data.that;
    that.node.remove();
    
    var index = that.parent.folders.indexOf(that);
    if (index < 0) {
        throw "Folder does not exist in parent folder"
    }
        
    that.parent.folders.splice(index, 1);
}

Folder.prototype.addFolderWithDialog = function(event) {
    var that = event.data.that;
    
    showInputDialog("New folder name:", true,
        {text: "Create", func: function(event) {
            var name = event.data.value.val().toLowerCase();
            var error = event.data.error;
            
            var ret = checkValidName(name, joinFullPath(that.fullPath(), name, true) + "/");
            if (ret.err) {
                error.html(ret.err);
                return;
        
            } else if (name) {
                var found = false;
                $.each(that.folders, function(i, folder) {
                    if (folder.name == name) {
                        found = true;
                        return;
                    }
                });
                if (found) {
                    error.html("Error: folder with the same name already exists");
                    return false;
                }

                that.addFolder(name);
                    that.render();
                
                gBrowserActions.addFolder(joinFullPath(that.fullPath(), name, true), function(data) {
                    closeInputDialog();
                },
                function(data) {
                    error.html("Error: " + data.err);
                });
                
                return false;
            } else {
                error.html("Error: Invalid name")
                return false;
            }
            return true;
        }},
        {text: "Cancel", func: null});
}

Folder.prototype.removeWithOverlay = function(event) {
   $(".cool-browser-overlay").css("z-index", 100);
   var that = event.data.that;
   that.node.css("z-index", 101);
   actions = that.node.find("#actions").first();
   
}

Folder.prototype.removeWithDialog = function(event) {
    var that = event.data.that;
    
    if (!$.isEmptyObject(that.folders) || !$.isEmptyObject(that.files)) {
        if (gBrowserActions instanceof BrowserActionsOnline) {
            showInputDialog("Cannot delete non empty folder", false, 
                null,
                {text: "Close"});
                
            return;
        }
    }
    
    var that = event.data.that;
    var fullPath = that.fullPath();
    
    showInputDialog('Delete folder: ' + fullPath + " ?", false,
        {text: "Delete", func: function(event) {
            var error = event.data.error;
            var value = event.data.value;
            
            /*
            deleteFile(that.fullPath(), value.val(),
                function(ok) {
                    that.remove({data: {that: that}});
                    that.parent.render();
                    closeInputDialog();
                },
                function(err) {
                    error.html(err);
                });
            */
            gBrowserActions.deleteFolder(fullPath, 
                function(data) {
                    if (data && data.ok) {
                        that.remove({data: {that: that}});
                        that.render();
                        closeInputDialog();
                        /*
                        gBrowserActions.removeSelected(function() {
                            closeInputDialog();
                        },
                        function(data) {
                            error.html(data);
                        });
                        */
                    } else {
                        error.html(data);
                    }
                },
                function(data) {
                    error.html(data.err);
                });
            
            return false;
        }},
        {text: "Cancel", func: null});
}

Folder.prototype.deletable = function(x) {
    node = this.node.find("#del");
    if (x === undefined) {
        return node.is(":visible");
    } else if (x === true) {
        node.show();
    } else {
        node.hide();
    }   
}

Folder.prototype.editable = function(x) {
    node = this.node.find("#rename");
    if (x === undefined) {
        return node.is(":visible");
    } else if (x === true) {
        node.show();
    } else {
        node.hide();
    }   
}

Folder.prototype.propsWithDialog = function(event) {
    var that = event.data.that;
    
    showInputDialog("Rename folder:", true,
        {text: "Rename", func: function(event) {
            var name = event.data.value.val().toLowerCase();
            var error = event.data.error;
            
            var maxPathLen = maxFullPathLen;
            var orgName = that.name;
            
            try {
                that.name = name
                maxPathLen = findMaxPathLen(that);
            } finally {
                that.name = orgName;
            }
            
            if (maxPathLen > maxFullPathLen) {
                error.html("New name is too long in regards to inner folder/files (maximum of 179 characters per full path");
                return;
            }
               
            var ret = checkValidName(name, joinFullPath(that.parent.fullPath(), name, true));
            if (ret.err) {
                error.html(ret.err);
                return;
                
            } else if (name) {
                var found = false;
                $.each(that.parent.folders, function(i, folder) {
                    if (folder.name == name) {
                        found = true;
                        return;
                    }
                });
                if (found) {
                    error.html("Error: folder with the same name already exists");
                    return false;
                }
                
                gBrowserActions.renameFolder(that, name, 
                    function() {
                        that.name = name;
                        that.render();
                    },
                    function(err) {
                        error.html("Error: " + err);
                    });
            } else {
                error.html("Error: Invalid name")
                return false;
            }
            return true;
        }},
        {text: "Cancel", func: null},
        that.name);
}

Folder.prototype.toggleCheck = function(event) {
    var that = event.data.that;
    var check = that.node.find("#check").first();
        
    if (check.hasClass(uncheckedClass)) {
        check.removeClass(uncheckedClass1).addClass(checkedClass1);
    } else {
        check.removeClass(checkedClass1).addClass(uncheckedClass1);
    }
}    

Folder.prototype.fullPath = function() {
    if (!this.parent && this.name == "/")
        return "";
    
    var parent = this.parent;
    var path = [this.name];
    
    while (parent) {
        path.push(parent.name);
        parent = parent.parent;
    }
    
    path.pop();
    
    return "/" + path.reverse().join("/").toLowerCase();
}

Folder.prototype.findFolder = function(name) {
    var found = null;
    
    $.each(this.folders, function(i, folder) {
        if (folder.name == name) {
            found = folder;
        }
    });
    
    return found;
}

Folder.prototype.findFile = function(name) {
    var found = null;
    
    $.each(this.files, function(i, file) {
        if (file.name == name) {
            found = file;
        }
    });
    
    return found;
}


Folder.prototype.root = function() {
    var obj = this;
    
    while (obj.parent) {
        obj = obj.parent;
    }
    
    return obj;
}

Folder.prototype.checked = function() {
    var check = this.node.find("#check").first();
    return check.hasClass(checkedClass);
}

Folder.prototype.dragenter = function(e) {
    var that = e.data.that;
    var folder = that.fullPath();
    gInfoPane.text("Add file into: " + (folder == "" ? "/" : folder));

    e.preventDefault();
	e.stopPropagation();
}

Folder.prototype.dragleave = function(e) {
    var that = e.data.that;

    e.preventDefault();
	e.stopPropagation();
}

Folder.prototype.dragover = function(e) {
    e.preventDefault();
	e.stopPropagation();
}

Folder.prototype.drop = function(e) {
    var that = e.data.that;
    
    gInfoPane.text("");
    that.uploadFile(e.originalEvent.dataTransfer.files[0]);
    
    e.preventDefault();
	e.stopPropagation();
}

var fileDiv = (
"<div class='fb-nesting'>" +
"<span class='fb-text'>" +
"<span id='check' class='fb-text " + uncheckedClass + "'></span>" +
//"<span id='icon' class='fa-stack'><i class='fa fa-file-o fa-stack-1x'></i><i class='fa fa-lock fa-stack-1x'></i></span>" +
"<span id='icon' class='fa fa-file'></span>" +
"<span id='actions' class='fb-invisible fb-text'>" +
"<span id='props' class='fa fa-edit fb-actions' title='Properties'></span>" +

//"<span id='download' class='fa fa-download fb-actions' title='Download'></span>" +
"<span id='download' class='fa fa-download fb-actions' title='Get File'></span>" +
"<span id='overwrite' class='fa fa-pencil fb-actions' title='Overwrite File'></span>" +
"<span id='del' class='fa fa-close fb-actions' title='Delete'></span>" +
"</span>" +
//"<span id='text' class='unselectable fb-text2'></span>" +
"<span id='text' class='unselectable fb-text3'></span>" +
"<span class='fb-attr'>" +
"<span id='sec' class='unselectable fb-text fa fa-lock' data-title='Secure'></span>" +
"<span id='failsafe' class='unselectable fb-text fa fa-save' data-title='Fail Safe'></span>" +
"<span id='size' class='unselectable fb-text fb-size'>0</span>" +
"</span>" +
"</span>" +
"</div>"
);

function File(name, attrs, parent) {
    if (name) {
        this.name = name.toLowerCase();
    }
    
    this.parent = parent;
    $.extend(this, attrs);
}

File.prototype.render = function(args) {
    if (!this.node) {
        this.node = $(fileDiv)
        //this.node.find("#text").html(this.name);
        
        if (args["check"] === false) {
            this.node.find("#check").first().hide();
        } else {
            this.node.find("#check").on("click", {that: this}, this.toggleCheck);
        }

        if (args["props"] === false) {
            this.node.find("#props").hide();
        } else {
            this.node.find("#props").on("click", {that: this}, this.propsWithDialog);
        }
        
        
        if (args["download"] === false) {
            this.node.find("#download").hide();
        } else {
            this.node.find("#download").on("click", {that: this}, this.downloadWithDialog);
        }
        
        if (this.flagSystemFile && !this.flagSystemFileHostAccess) {
            this.node.find("#del").removeClass("fb-actions")
                                  .addClass("fb-actions-disabled")
                                  .prop("title", "Unavailable for System Files");
        } else {
            this.node.find("#del").on("click", {that: this}, this.removeWithDialog);
        }
        
        /*
        this.node.unbind("dragenter").unbind("dragover").unbind("drop").unbind("dragleave");
        this.node.on("dragenter").on("dragenter", this.dragover);
        this.node.on("dragover").on("dragenter", this.dragover);
        this.node.on("drageleave").on("dragenter", this.dragover);
        this.node.on("drop").on("dragenter", this.dragover);
        */
        
        this.node.find("#overwrite").on("click", {that: this.parent, newName: this.name, oldFile: this}, uploadFileWithBrowse);
    
        this.node.on("dragenter", {that: this}, this.dragenter);
        this.node.on("dragleave", {that: this}, this.dragleave);
        this.node.on("dragover", this.dragover);
        this.node.on("drop", {that: this}, this.drop);

        
    }
    this.node.find("#text").first().html(this.name);
    
    var sizeInKb = (this.maxFileSize / 1024).toFixed(1);
    var size = this.node.find("#size").first();
    size.html(sizeInKb + "KB");
    size.prop("title", "Size: " + this.maxFileSize + " Bytes");
    
    var sec = this.node.find("#sec").first();
    if (!this.flagSecure) {
        sec.css("opacity", 0).removeAttr("title");
    } else {
        sec.css("opacity", 1).attr("title", sec.attr("data-title"));
    }
    
    var failSafe = this.node.find("#failsafe").first();
    if (!this.flagFailSafe) {
        failSafe.css("opacity", 0).removeAttr("title");
    } else {
        failSafe.css("opacity", 1).attr("title", failSafe.attr("data-title"));
    }
        
    if (this.flagNoValidCopy) {
        this.node.find("#text").first().css("text-decoration", "line-through")
            .addClass("fb-ti-red")
            .prop("title", "File wasn't closed properly");
        
        this.node.find("#download").removeClass("fb-actions")
          .addClass("fb-actions-disabled")
          .prop("title", "Invalid file");
    }

    
    /*
    this.node.find("#overwrite").on("click", {that: this.parent, newName: this.name, oldFile: this}, uploadFileWithBrowse);
    
    
    this.node.on("dragenter", {that: this}, this.dragenter);
    this.node.on("dragleave", {that: this}, this.dragleave);
    this.node.on("dragover", this.dragover);
    this.node.on("drop", {that: this}, this.drop);
    */
    
    
    return this.node;
}

File.prototype.remove = function(event) {
    var that = event.data.that;
    that.node.remove();
    
    var index = that.parent.files.indexOf(that);
    if (index < 0) {
        throw "File does not exist in parent folder"
    }

    that.parent.files.splice(index, 1)
}

File.prototype.removeWithOverlay = function(event) {
   $(".fb-overlay").css("z-index", 100);
   var that = event.data.that;
   that.node.css("z-index", 101);
   actions = that.node.find("#actions").first();
   
}

File.prototype.removeWithDialog = function(event) {
    var that = event.data.that;
    var fullPath = that.fullPath();

    var bhttpcert   = (fullPath == gHttpCertFileName)
    var bhttpkey    = (fullPath == gHttpKeyFileName)
    var bhttpcacert = (fullPath == gHttpCaCertFileName)
 
    if (that.flagSecure && (gBrowserActions instanceof BrowserActionsOnline) && !that.flagNoValidCopy) {
        promptText = 'Delete file: ' + fullPath + ' ?<br>Token:';
        hasInput = true;
    } else {
        promptText = 'Delete file: ' + fullPath + ' ?';
        hasInput = false;
    }   
 
    if (bhttpcert == true) {
        promptText = promptText + '<br> This file is used as <b>http certificate</b> and will be deleted from <b>HTTP Server</b> too.'
    }
    if (bhttpkey == true) {
        promptText = promptText + '<br> This file is used as <b>http key</b> and will be deleted from <b>HTTP Server</b> too.'
    }
    if (bhttpcacert == true) {
        promptText = promptText + '<br> This file is used as <b>http ca certificate</b> and will be deleted from <b>HTTP Server</b> too.'
    }

    showInputDialog(promptText, hasInput,
        {text: "Delete", func: function(event) {
            var error = event.data.error;
            var value = event.data.value;
            
            gBrowserActions.deleteFile(that.fullPath(), value.val(),
                function(ok) {
                    that.remove({data: {that: that}});
                    that.parent.render();
                    //closeInputDialog();
                    closeInputDialogHttp(bhttpcert, bhttpkey, bhttpcacert);
                },
                function(err) {
                    error.html(err);
                });
                    
            return false;
        }},
        {text: "Cancel", func: null});
}

File.prototype.propsWithDialog = function(event) {
    //alert("Properties dialog for file:" + event.data.that.name);
    var that = event.data.that;
    
    if (gBrowserActions instanceof BrowserActionsOnline) {
        disableAll = true;
    } else {
        disableAll = false;
    }
    
    pd = new PropsDialog(disableAll);
    pd.props(that);
    
    if (event.data.err)
        pd.showError(event.data.err);
    
    //pd.open(that, function() {
    var save = function() {
        var fullPath = that.fullPath();
        
        var signFileObj = fileInputSign[0].files[0];
        if (dropSignFileObj) {
            signFileObj = dropSignFileObj;
        }
        
        gBrowserActions.changeProps(that, fullPath, pd, signFileObj, 
            function(newProps) {
                if (!newProps) {
                    newProps = pd.props();
                }
                //$.extend(that, pd.props());
                $.extend(that, newProps);
                that.render();

                //pd.close()
            }, 
            function(err) {
                pd.showError(err);
                pd.open(that, save, "Update");
            });
    }//);
    pd.open(that, save, "Update");
}

File.prototype.toggleCheck = function(event) {
    var that = event.data.that;
    var check = that.node.find("#check").first();
        
    if (check.hasClass(uncheckedClass)) {
        check.removeClass(uncheckedClass1).addClass(checkedClass1);
    } else {
        check.removeClass(checkedClass1).addClass(uncheckedClass1);
    }
}    

File.prototype.fullPath = function(folder) {
    if (folder === undefined) {
        var parent = this.parent;
    } else {
        var parent = folder;
    }
        
    if (!parent.parent && parent.name == "/")
        return this.name.toLowerCase();
    
    var path = [this.name];
    
    while (parent && parent.name != "/") {
        path.push(parent.name);
        parent = parent.parent;
    }
    
    return "/" + path.reverse().join("/").toLowerCase();
}

File.prototype.downloadWithDialog = function(event) {
    var that = event.data.that;
    fullPath = that.fullPath();
    
    if (that.flagSecure && (gBrowserActions instanceof BrowserActionsOnline) && !that.flagPublicRead) {
        promptText = 'Get File: ' + fullPath + ' ?<br>Token:';
        hasInput = true;
    } else {
        promptText = 'Get File: ' + fullPath + ' ?';
        hasInput = false;
    }   

    showInputDialog(promptText , hasInput,
        {text: "Get", func: function(event) {
            var error = event.data.error;
            var value = event.data.value;
            
            gBrowserActions.downloadFile(that.fullPath(), value.val());
                    
            return false;
        }},
        {text: "Cancel", func: null});
}

File.prototype.root = function() {
    var obj = this;
    
    while (obj.parent) {
        obj = obj.parent;
    }
    
    return obj;
}

File.prototype.editable = function(enable) {
    node = this.node.find("#props");
    if (x === undefined) {
        return node.is(":visible");
    } else if (x === true) {
        node.show();
    } else {
        node.hide();
    }   
}

File.prototype.checked = function() {
    var check = this.node.find("#check").first();
    return check.hasClass(checkedClass);
}


File.prototype.dragenter = function(e) {
    var that = e.data.that;
    gInfoPane.text("Overwrite file: " + that.fullPath());
    
    e.preventDefault();
	e.stopPropagation();
}

File.prototype.dragleave = function(e) {
    var that = e.data.that;
        
    e.preventDefault();
	e.stopPropagation();
}

File.prototype.dragover = function(e) {
    e.preventDefault();
	e.stopPropagation();
}

File.prototype.drop = function(e) {
    var that = e.data.that;
    
    gInfoPane.text("");
    that.parent.uploadFile(e.originalEvent.dataTransfer.files[0], that.name, that);
    
    e.preventDefault();
	e.stopPropagation();
}

File.prototype.updateProps = function(props) {
    $.extend(this, props);
}


var browserDiv = (
"<div class='fb-title'>" +
"<span class='fb-text'>File</span>" +
"<span class='fb-attr-header'>" +
"<span>Properties</span>" +
"</div>" +
"</span>"
);


var gBrowserActions = null;
var gProjectName = null;

function Browser(div, mode, projectName) {
    gProjectName = projectName;
    
    if (!fileInput) {
        fileInput = $("<input type='file' hidden>");
        $("body").append(fileInput);
    }
    
    if (!fileInputSign) {
        fileInputSign = $("<input type='file' hidden>");
        $("body").append(fileInputSign);
    }
    if (!fileLink) {
        //fileLink = $("<a id='malink' href='/api/1/flash/devModeDownloadFileGet' hidden></a>");
        fileLink = $("<a hidden></a>");
        $("body").append(fileLink);
    }

    this.root = new Folder("/");
    
    if (div) {
        this.main = div;
        //this.main = div.clone();
        //div.append(this.main);
        
        //this.overlay = this.main.clone().addClass("cool-browser-in cool-browser-overlay").appendTo(this.main);
    
        //this.inner = this.main.clone().addClass("cool-browser-in").appendTo(this.main);
        this.inner = this.main.clone().addClass("fb-in")
        this.main.html(browserDiv).append(this.inner);	
        this.inner.append(this.root.node);
        this.inner.on("scroll", this.scroll);
        
        this.main.off("dragenter").off("dragover").off("drop");
        this.main.on("dragenter", this.drag);
        this.main.on("dragleave", this.drag);
        this.main.on("dragover", this.drag);
        this.main.on("drop", this.drag);
        
        this.inner.off("dragenter").off("dragover").off("drop");
        this.inner.on("dragenter", this.drag);
        this.main.on("dragleave", this.drag);
        this.inner.on("dragover", this.drag);
        this.inner.on("drop", this.drag);
        
        this.main.parent().append($("<div style='height: 30px;'>"));
        this.info = $("<div id='info' class='fb-info'></div>");
        this.main.parent().append(this.info);
        
        gInfoPane = this.info;
                
        document.addEventListener("dragover", function( event ) {
            gInfoPane.text("");
            event.preventDefault();
            event.stopPropagation();
        }, false);

        
    }
    
    if (mode === undefined) {
        throw "File Browser requires mode"
    }
    
    if (mode == "offline") {
        if (projectName === undefined) {
            throw "Offline file Browsesr requires project"
        }
        gBrowserActions = new BrowserActionsOffline(this);
    } else {
        gBrowserActions = new BrowserActionsOnline(this);
    }
}

Browser.prototype.render = function() {
    $("<div class='fb-attrs'></div>").appendTo(this.inner);
    
    /* "cert" directory is unsufficient for chanined certificates and therefor deprecated in favor of just using the root folder
    if (!this.root.findFolder("cert")) {
        this.root.addFolder("cert");
    }
    */
    
    var args = {};
    if (gBrowserActions instanceof BrowserActionsOnline) {
        args.check = false;
        //args.props = false;
        args.rename = false;
    } else {
        //args.download = false // re-enable download for offline browser;
    }
      
    this.inner.append(this.root.render(args));
    
    
    this.root.deletable(false);
    this.root.editable(false);
    this.root.node.find("#actions").removeClass("fb-invisible");
    
    this.scroll({currentTarget: this.inner[0]});    
}

Browser.prototype.load = function(data) {
    this.inner.empty();
    
    if (typeof(data) == "string") {
        data = JSON.parse(data);
    }
        
    this.root = $.extend(true, new Folder(), data);
    
    function load(root) {
        $.each(root.folders, function(i, folder) {
            folder = $.extend(new Folder(), folder);
            folder.parent = root;
            root.folders[i] = folder;
            load(folder);
        });
        root.folders.sort(function(a, b) {return a.name > b.name});
        
        $.each(root.files, function(i, file) {
            file = $.extend(new File(), file);
            file.parent = root;
            root.files[i] = file;
        });
        root.files.sort(function(a, b) {return a.name > b.name});
    }

    load(this.root);

}

Browser.prototype.dump = function(data) {
    return this.root.dump();
}

Browser.prototype.scroll = function(event) {
	$(".fb-attr").css("right", -event.currentTarget.scrollLeft);
	$(".fb-attrs").css("right", -event.currentTarget.scrollLeft);
	$(".fb-attrs").css("top", event.currentTarget.scrollTop);
	//var w = $(".fb-attr").first().width();
    var w = $(".fb-attrs").first().width();
	$(".fb-text2").css("margin-right", w + "px");
    
    // Safari issue with fb-text2
    $(".fb-text3").css("margin-right", w + "px");
}


Browser.prototype.uploadMCUImage = function(fDevice) {
    gBrowserActions.uploadMCUImage(fDevice);
}

Browser.prototype.uploadMCUImageSimple = function (fDevice, data, f_close, f_save) {
    callbackClose = f_close
    callbackSave  = f_save
    gBrowserActions.uploadMCUImageSimple(fDevice, data);
}

Browser.prototype.removeSelected = function() {
    var that = this;
    var folders = [];
    var files = [];
    
    function remove(root) {        
        $.each(root.folders, function(i, folder) {
            if (folder.checked()) {
                folders.push(folder.fullPath());
            } else {
                remove(folder);
            }
        });
    
        $.each(root.files, function(i, file) {
            if (file.checked()) {
                files.push(file.fullPath());
            }
        });
    }
    
    remove(this.root)
    gBrowserActions.removeSelected(folders, files,
        function() {
            that.refresh();
        },
        function() {
            that.refresh();
        });

}

Browser.prototype.selectAll = function() {
    //this.root.node.find("*#check").removeClass(uncheckedClass).addClass(checkedClass);
    check = this.root.node.find("*#check")
    if (check.hasClass(uncheckedClass)) {
        check.removeClass(uncheckedClass1).addClass(checkedClass1);
    }
}

Browser.prototype.unselectAll = function() {
    //this.root.node.find("*#check").removeClass(checkedClass).addClass(uncheckedClass);
    check = this.root.node.find("*#check")
    if (check.hasClass(checkedClass)) {
        check.removeClass(checkedClass1).addClass(uncheckedClass1);
    }
}

Browser.prototype.drag = function(e) {
    gInfoPane.text("");
    e.preventDefault();
	e.stopPropagation();
}

Browser.prototype.refresh = function() {
    var that = this;
    
    $.post("/api/1/flash/loadUserFiles", {"name": gProjectName})
        .done(function(data) {
            if (data.ok) {
                that.load(data.ok);
            } else {
                if (data.err) {
                    alert("error loading user files");
                }
            }
            
            that.render();
        })
        .fail(function(data) {
            alert("error loading user files");
        });
}

var inputDialog = (
'<div class="white-popup-small">' +
'	<div class="wrapPopup">' +
'		<div id="prompt" class="filePathN center-block" tabIndex=0 style="outline: none"></div>' +
'		<div class="wrapDRS center-block">' +
'			<input type="text" id="inputValue" value="">' +
'           <div id="error" class="fb-ti-red"></div>' +
'		</div>' +
'		<div class="clear" />' +
'		<div class="wrapButtons">' +
'			<div id="ok", class="regButton floatLeft" ></div>' +
//'			<div id="ok", class="regButton floatLeft" style="width:60px; text-align: center"></div>' +
//'			<div id="cancel" class="regButton floatRight" style="width:60px; text-align: center"</div>' +
'			<div id="cancel" class="regButton floatRight"</div>' +
'		</div>' +
'	</div>' +
'	<div class="clear" />' +
'</div>');
	
function showInputDialog(prompt, hasInput, ok, cancel, defaultText) {
    var div = $(inputDialog);
    
    var okBtn = div.find("#ok");
    var cancelBtn = div.find("#cancel");
    var input = div.find("#inputValue");
    var error = div.find("#error");
    
    if (!hasInput) {
        input.hide();
    } else if (defaultText) {
        input.val(defaultText);
    }

    if (ok) {
        okBtn.html(ok.text);
        if (ok.func) {
            okBtn.on("click", {value: input, error: error}, function okFunc(event) {
                if (ok.func(event)) {
                    $.magnificPopup.close();
                }
            });
        } else {
            
        }
        //div.find("#ok").on("click", $.magnificPopup.close);
    } else {
        //okBtn.addClass("btnDisabled");
        okBtn.remove();
        //cancelBtn.removeClass("mLeftS").addClass("mleft50");
        cancelBtn.removeClass("mLeftS").addClass("mright60");
    }
    
    if (cancel) {
        cancelBtn.html(cancel.text);
        if (cancel.func) {
            cancelBtn.on("click", cancel.func);
        }
        cancelBtn.on("click", $.magnificPopup.close);
    } else {
        cancelBtn.addClass("btnDisabled");
    }
    
    div.find("div#prompt").html(prompt);
    
        
    input.keydown(function(e) {
        if (e.keyCode == 13) {
            okBtn.click();
        } else if (e.keyCode == 27) {
            cancelBtn.click();
        }           
    });
    
    div.find("div#prompt").keydown(function(e) {
        if (e.keyCode == 13) {
            okBtn.click();
        } else if (e.keyCode == 27) {
            cancelBtn.click();
        }           
    });
    
	$.magnificPopup.open({
		items: {
			src: div,
			type: 'inline',
		},
        focus: (hasInput) ? 'input#inputValue' : 'div#prompt',
        modal: true
	});
}

function closeInputDialog() {
    $.magnificPopup.close();
}

function closeInputDialogHttp(delete_http_cert, delete_http_key, delete_http_cacert) {
    $.magnificPopup.close();
    if (delete_http_cert == true)    {
        dialogDeleteHttpCertCallback();
    }
    if (delete_http_key == true) {
        dialogDeleteHttpKeyCallback();
    }
    if (delete_http_cacert == true) {
        dialogDeleteHttpCaCertCallback();
    }
}

function uploadFileWithBrowse(event) {
    var that = event.data.that;
    var newName = event.data.newName;
    var oldFile = event.data.oldFile;
    
    fileInput.off("change").val("");
    
    fileInput.on("change", function _handler () {
        try {
            input_file = fileInput[0].files[0]
            if (/\.pem$/i.test(input_file.name.toLowerCase()) === false) {
                that.uploadFile(input_file, newName, oldFile, false);

            } else {
                fileBrowser.showInputDialog(PemMessage,
                                   false,
                                   {
                                       text: "Allow", func: function () {

                                           that.uploadFile(input_file, newName, oldFile, true);
                                       }
                                   },
                                   {
                                       text: "&#8196;Deny", func: function () {
                                       }
                                   });

            }//pem
            
        } finally {
            fileInput.off("change").val("", _handler);
        }
    });
    
    //fileInput.val("");
    fileInput.click();
}
    
Folder.prototype.uploadFile = function(fileObj, newName, oldFile, pem_replace) {
    name = fileObj.name;
    
    var pd = new PropsDialog();
    var forOverwrite = false;
    
    if (!oldFile) {
        var props = pd.props();
        props.maxFileSize = fileObj.size;
    } else {
        pd.props(oldFile);
        var props = pd.props();
        
        if (gBrowserActions instanceof BrowserActionsOnline) {
            forOverwrite = true;
            pd.disableAll(forOverwrite);
        } else {
            props.signatureFileNameSelect = "1";
            props.signatureFileName = "";
            pd.props(props);
            pd.disableRename();
        }
    }
    
    //props.filename = new File(fileInput[0].files[0].name).fullPath(that);
    if (!newName) {
        props.name = new File(fileObj.name).name;
    } else {
        props.name = newName.toLowerCase();
    }
    

    //props.maxFileSize = fileObj.size;

    
    props.origFileSize = fileObj.size;
    props.origFileName = fileObj.name;
    
    var fullPath = new File(props.name).fullPath(this)
    if (fullPath == getMCUImgNameAndPath(true).fullPath) {
        //props.maxFileSize += 20
        var defaultMaxSize = 1024 * 1024;
        if (props.maxFileSize + 20 > defaultMaxSize) {
            displayError("MCU Image file exceeds the maximum size for this device");
            return;
        } else {
            props.maxFileSize = defaultMaxSize;
        }
        
        props.flagFailSafe = true;
        props.flagSecure = true;
        props.flagPublicWrite = true;
    }
    
    if (fullPath == getMCUImgNameAndPath().fullPath) {
        props.flagFailSafe = true;
        
        if (project && project.header.IsTheDeviceSecure) {
            props.flagSecure = true;
            props.flagPublicWrite = true;
        }
        
        var defaultMaxSize = 256 * 1024;
        if (props.maxFileSize > defaultMaxSize) {
            displayError("MCU Image file exceeds the maximum size for this device");
            return;
        } else {
            props.maxFileSize = defaultMaxSize;
        }
    }
    
    pd.props(props);
    
    var that = this;
    
    //var save = function() {
    pd.open(that, function save() {
        var props = pd.props();
        if (that.findFile(props.name) && !oldFile) {
            pd.showError("File with the same name already exists in this folder");
            return;
        }
        /*if (props.name == gHttpKeyFileName      ) {
            pd.showError("File with the same name already exists in http key folder");
            return;
        }

        if (props.name == gHttpCertFileName     ) {
            pd.showError("File with the same name already exists in http cert folder");
            return;
        }

        if (props.name == gHttpCaCertFileName   ) {
            pd.showError("File with the same name already exists in http ca cert folder");
            return;
        }*/

        var ret = checkValidName(props.name, joinFullPath(that.fullPath(), props.name));
        if (ret.err) {
            pd.showError(ret.err);
            return;
        }
        
        if (props.flagSecure && (props.flagVendor || (oldFile && gBrowserActions.requireToken(props)))) {
            if (!$.isNumeric(props.fileToken)) {
                //pd.showError("Token has to be an integer");
                pd.div.find("span#tokenError").text("Token has to be an integer");
                return;
            }
            
            if (props.fileToken > 0xFFFFFFFF || props.fileToken < 0) {
                //pd.showError("Token cannot be smaller than zero or bigger than 4294967295");
                pd.div.find("span#tokenError").text("Token cannot be smaller than zero or bigger than 4294967295");
                return;
            }
        }
        
        var fileSize = fileObj.size;
        if (fullPath == getMCUImgNameAndPath(true).fullPath) {
            fileSize += 20
        }
        
        if (Number(props.maxFileSize ) < fileSize)
        {
            pd.showError("Max Size is smaller than file's original size: " + fileSize + " bytes");
            return;
        }
    
        var fullPath = new File(props.name).fullPath(that);
        
        var signFileObj = fileInputSign[0].files[0];
        if (dropSignFileObj) {
            signFileObj = dropSignFileObj;
        }
        
        gBrowserActions.uploadFile(fullPath, props, fileObj, signFileObj, pem_replace,
            function(ok) {
                try {                    
                    if (!oldFile) {
                        var file = that.addFile(props.name, props);
                    } else {
                        var file = oldFile;
                        file.updateProps(props);
                        file.render();
                    }
                    
                    if (ok && ok.ok) {
                        if (file.flagSecure && ok.ok.indexOf("File Token:") != -1) {
                            file.fileToken = ok.ok.split(":")[1].trim();
                        }
                    }
                    
                    //file.render();
                    that.render();
                    
                    if (signFileObj && signFileObj.name) {
                        gBrowserActions.uploadSignFile(fullPath, props, fileObj, signFileObj, 
                            function(ok) {
                                if (file.signatureFileNameSelect == "1") {
                                    file.signatureFileNameSelect = "0";
                                    file.signatureFileName = "generated_signature.bin";
                                }
                                
                                gBrowserActions.uploadDone(props);
                            },
                            function(err) {
                                file.signatureFileName = "";
                                file.propsWithDialog({data: {that: file, err: err}});
                            });
                    } else {
                        gBrowserActions.uploadDone(props);
                    }
                } finally {
                    //fileInput.val("");
                    //fileInputSign.val("");
                }
            },
            function(err) {
                pd.showError(err);
                
                if (err.indexOf("Invalid Signature") !== -1 || err.indexOf("Invalid private key") !== -1) {
                    props.signatureFileName = "";
                    pd.props(props);
                }
                pd.open(that, save, (!oldFile) ? "Write" : "Overwrite");
            }
        );
    }, (!oldFile) ? "Write" : "Overwrite");
    //pd.open(that, save, (!oldFile) ? "Write" : "Overwrite");
}

Folder.prototype.uploadFileSimple = function (fileObj, newName, oldFile) { //no gui
    name = fileObj.name;

    var pd = new PropsDialog();
    var forOverwrite = false;
 /*   
    if (!oldFile) {
        var props = pd.props();
        props.maxFileSize = fileObj.size;
    } else {
        pd.props(oldFile);
        var props = pd.props();

        if (gBrowserActions instanceof BrowserActionsOnline) {
            forOverwrite = true;
            pd.disableAll(forOverwrite);
        } else {
            props.signatureFileNameSelect = "1";
            props.signatureFileName = "";
            pd.props(props);
            pd.disableRename();
        }
    }
*/

    var props = pd.props();
    props.maxFileSize = fileObj.size;

    //props.filename = new File(fileInput[0].files[0].name).fullPath(that);
    if (!newName) {
        props.name = new File(fileObj.name).name;
    } else {
        props.name = newName.toLowerCase();
    }


    //props.maxFileSize = fileObj.size;


    props.origFileSize = fileObj.size;
    props.origFileName = fileObj.name;

    var fullPath = new File(props.name).fullPath(this)
    if (fullPath == getMCUImgNameAndPath(true).fullPath) {
        //props.maxFileSize += 20
        var defaultMaxSize = 1024 * 1024;
        if (props.maxFileSize + 20 > defaultMaxSize) {
            displayError("MCU Image file exceeds the maximum size for this device");
            return;
        } else {
            props.maxFileSize = defaultMaxSize;
        }

        props.flagFailSafe = true;
        props.flagSecure = true;
        props.flagPublicWrite = true;
    }

    if (fullPath == getMCUImgNameAndPath().fullPath) {
        props.flagFailSafe = true;

        if (project && project.header.IsTheDeviceSecure) {
            props.flagSecure = true;
            props.flagPublicWrite = true;
        }

        var defaultMaxSize = 256 * 1024;
        if (props.maxFileSize > defaultMaxSize) {
            displayError("MCU Image file exceeds the maximum size for this device");
            return;
        } else {
            props.maxFileSize = defaultMaxSize;
        }
    }

    pd.props(props);

    var that = this;

    //var save = function() {
    pd.open(that, function save() {
        var props = pd.props();
        if (that.findFile(props.name)) {
            pd.showError("File with the same name already exists in this folder");
            return;
        }
        /*if (props.name == gHttpKeyFileName) {
            pd.showError("File with the same name already exists in http key folder");
            return;
        }

        if (props.name == gHttpCertFileName) {
            pd.showError("File with the same name already exists in http cert folder");
            return;
        }

        if (props.name == gHttpCaCertFileName) {
            pd.showError("File with the same name already exists in http ca cert folder");
            return;
        }*/

        var ret = checkValidName(props.name, joinFullPath(that.fullPath(), props.name));
        if (ret.err) {
            pd.showError(ret.err);
            return;
        }

        if (props.flagSecure && (props.flagVendor || (oldFile && gBrowserActions.requireToken(props)))) {
            if (!$.isNumeric(props.fileToken)) {
                //pd.showError("Token has to be an integer");
                pd.div.find("span#tokenError").text("Token has to be an integer");
                return;
            }

            if (props.fileToken > 0xFFFFFFFF || props.fileToken < 0) {
                //pd.showError("Token cannot be smaller than zero or bigger than 4294967295");
                pd.div.find("span#tokenError").text("Token cannot be smaller than zero or bigger than 4294967295");
                return;
            }
        }

        var fileSize = fileObj.size;
        if (fullPath == getMCUImgNameAndPath(true).fullPath) {
            fileSize += 20
        }

        if (Number(props.maxFileSize) < fileSize) {
            pd.showError("Max Size is smaller than file's original size: " + fileSize + " bytes");
            return;
        }

        var fullPath = new File(props.name).fullPath(that);

        var signFileObj = fileInputSign[0].files[0];
        if (dropSignFileObj) {
            signFileObj = dropSignFileObj;
        }

        gBrowserActions.uploadFile(fullPath, props, fileObj, signFileObj,
            function (ok) {
                try {
                    /*
                    if (!oldFile) {
                        var file = that.addFile(props.name, props);
                    } else {
                        var file = oldFile;
                        file.updateProps(props);
                        //file.render();
                    }
                    */

                    var file = that.addFile(props.name, props);

                    if (ok && ok.ok) {
                        if (file.flagSecure && ok.ok.indexOf("File Token:") != -1) {
                            file.fileToken = ok.ok.split(":")[1].trim();
                        }
                    }

                    //file.render();
                    //that.render();

                    if (signFileObj && signFileObj.name) {
                        gBrowserActions.uploadSignFile(fullPath, props, fileObj, signFileObj,
                            function (ok) {
                                if (file.signatureFileNameSelect == "1") {
                                    file.signatureFileNameSelect = "0";
                                    file.signatureFileName = "generated_signature.bin";
                                }

                                gBrowserActions.uploadDone(props);
                            },
                            function (err) {
                                file.signatureFileName = "";
                                file.propsWithDialog({ data: { that: file, err: err } });
                            });
                    } else {
                        gBrowserActions.uploadDone(props);
                    }
                } finally {
                    //fileInput.val("");
                    //fileInputSign.val("");
                }
            },
            function (err) {
                pd.showError(err);

                if (err.indexOf("Invalid Signature") !== -1 || err.indexOf("Invalid private key") !== -1) {
                    props.signatureFileName = "";
                    pd.props(props);
                }
                //pd.open(that, save, (!oldFile) ? "Write" : "Overwrite");
                pd.open(that, save, "Write");
            }
        );
   // }, (!oldFile) ? "Write" : "Overwrite");
    }, "Write");
    //pd.open(that, save, (!oldFile) ? "Write" : "Overwrite");
}

Folder.prototype.uploadFileSimpleNoBr = function (fileObj, newName, oldFile) { //no gui
    name = fileObj.name;

    var pd = new PropsDialog();
    var forOverwrite = false;

    if (!oldFile) {
        var props = pd.props();
        props.maxFileSize = fileObj.size;
    } else {
        pd.props(oldFile);
        var props = pd.props();

        if (gBrowserActions instanceof BrowserActionsOnline) {
            forOverwrite = true;
            pd.disableAll(forOverwrite);
        } else {
            props.signatureFileNameSelect = "1";
            props.signatureFileName = "";
            pd.props(props);
            pd.disableRename();
        }
    }

    //props.filename = new File(fileInput[0].files[0].name).fullPath(that);
    if (!newName) {
        props.name = new File(fileObj.name).name;
    } else {
        props.name = newName.toLowerCase();
    }

    props.origFileSize = fileObj.size;
    props.origFileName = fileObj.name;

    var fullPath = new File(props.name).fullPath(this)
    
    if (fullPath == getMCUImgNameAndPath(true).fullPath) {
        //props.maxFileSize += 20
        var defaultMaxSize = 1024 * 1024;
        if (props.maxFileSize + 20 > defaultMaxSize) {
            displayError("MCU Image file exceeds the maximum size for this device");
            return;
        } else {
            props.maxFileSize = defaultMaxSize;
        }

        props.flagFailSafe = true;
        props.flagSecure = true;
        props.flagPublicWrite = true;
    }

    if (fullPath == getMCUImgNameAndPath().fullPath) {
        props.flagFailSafe = true;

        if (project && project.header.IsTheDeviceSecure) {
            props.flagSecure = true;
            props.flagPublicWrite = true;
        }

        var defaultMaxSize = 256 * 1024;
        if (props.maxFileSize > defaultMaxSize) {
            displayError("MCU Image file exceeds the maximum size for this device");
            return;
        } else {
            props.maxFileSize = defaultMaxSize;
        }
    }
    
    pd.props(props);

    var that = this;

    //var save = function() {
   // pd.open(that, function save() {
    var props = pd.props();
   /* 
        if (that.findFile(props.name) && !oldFile) {
            displayError("File with the same name already exists in this folder");
            return;
        }*
        if (props.name == gHttpKeyFileName) {
            pd.showError("File with the same name already exists in http key folder");
            return;
        }

        if (props.name == gHttpCertFileName) {
            pd.showError("File with the same name already exists in http cert folder");
            return;
        }

        if (props.name == gHttpCaCertFileName) {
            pd.showError("File with the same name already exists in http ca cert folder");
            return;
        }
        */
        var ret = checkValidName(props.name, joinFullPath(that.fullPath(), props.name));
        if (ret.err) {
            pd.showError(ret.err);
            return;
        }

        if (props.flagSecure && (props.flagVendor || (oldFile && gBrowserActions.requireToken(props)))) {
            if (!$.isNumeric(props.fileToken)) {
                //pd.showError("Token has to be an integer");
                pd.div.find("span#tokenError").text("Token has to be an integer");
                return;
            }

            if (props.fileToken > 0xFFFFFFFF || props.fileToken < 0) {
                //pd.showError("Token cannot be smaller than zero or bigger than 4294967295");
                pd.div.find("span#tokenError").text("Token cannot be smaller than zero or bigger than 4294967295");
                return;
            }
        }

        var fileSize = fileObj.size;
        if (fullPath == getMCUImgNameAndPath(true).fullPath) {
            fileSize += 20
        }

        if (Number(props.maxFileSize) < fileSize) {
            pd.showError("Max Size is smaller than file's original size: " + fileSize + " bytes");
            return;
        }

        var fullPath = new File(props.name).fullPath(that);

        var signFileObj = fileInputSign[0].files[0];
        if (dropSignFileObj) {
            signFileObj = dropSignFileObj;
        }

        gBrowserActions.uploadMCUFileSimple(fullPath, props, fileObj, signFileObj,
            function (ok) {
                try {
                    /*
                    if (!oldFile) {
                        var file = that.addFile(props.name, props);
                    } else {
                        var file = oldFile;
                        file.updateProps(props);
                        //file.render();
                    }
                    */
                    var file = that.addFile(props.name, props);
                    dialogSavedCallback();
                    /*gBrowserActions.uploadMCUKeySignFile(fullPath, props, fileObj, signFileObj,
                           function (ok) {
                               if (file.signatureFileNameSelect == "1") {
                                   file.signatureFileNameSelect = "0";
                                   file.signatureFileName = "generated_signature.bin";
                               }

                               gBrowserActions.uploadDone(props);
                           },
                           function (err) {
                               file.signatureFileName = "";
                               file.propsWithDialog({ data: { that: file, err: err } });
                           });*/
                    if (ok && ok.ok) {
                        if (file.flagSecure && ok.ok.indexOf("File Token:") != -1) {
                            file.fileToken = ok.ok.split(":")[1].trim();
                        }
                    }

                    //file.render();
                    //that.render();
                    /*
                    if (signFileObj && signFileObj.name) {
                        gBrowserActions.uploadSignFile(fullPath, props, fileObj, signFileObj,
                            function (ok) {
                                if (file.signatureFileNameSelect == "1") {
                                    file.signatureFileNameSelect = "0";
                                    file.signatureFileName = "generated_signature.bin";
                                }

                                gBrowserActions.uploadDone(props);
                            },
                            function (err) {
                                file.signatureFileName = "";
                                file.propsWithDialog({ data: { that: file, err: err } });
                            });
                    } else {
                        gBrowserActions.uploadDone(props);
                    }*/
                } finally {
                    //fileInput.val("");
                    //fileInputSign.val("");
                }
            },
            function (err) {
                pd.showError(err);
                dialogClosedCallback();
                /*
                if (err.indexOf("Invalid Signature") !== -1 || err.indexOf("Invalid private key") !== -1) {
                    props.signatureFileName = "";
                    pd.props(props);
                }
                pd.open(that, save, (!oldFile) ? "Write" : "Overwrite");*/
            }
        );
    //}, (!oldFile) ? "Write" : "Overwrite");
}

function BrowserActionsOnline(browser) {
    this.browser = browser;
}

BrowserActionsOnline.prototype.downloadFile = function(fullPath, token) {
    progressModalOpen("Reading File", true);
    
    $.post("/api/1/flash/devModeDownloadFile", {name: fullPath, token: token})
        .done(function(data) {
            if (data.ok) {
                getProgressStatus(100, 
                    function(data) {
                        fileLink[0].href = '/api/1/flash/devModeDownloadFileGet?x=' + new Date().getTime() + '&r=' + Math.random();
                        fileLink[0].click();
                    }, false, 
                    function(data) { 
                        if (data.info) {
                            updateFsUsage(data.info);
                            updateAlerts(data.info);
                        }
                    });
            } else {
                if (data.info) {
                    updateFsUsage(data.info);
                    updateAlerts(data.info);
                }
                progressModalError("Downloading, err:" + data.err);
            }
        })
        .fail(function() {
            progressModalError("Downloading");
        });
}

BrowserActionsOnline.prototype.deleteFile = function(fullPath, token, done, fail) {
    progressModalOpen("Deleting File: " + fullPath);
    
    $.post("/api/1/flash/devModeDeleteFile", {name: fullPath, token: token})
        .done(function(data) {
            if (data && data.ok) {
                progressModalClose();
                if (data.info) {
                    updateFsUsage(data.info);
                }
                done(data.ok);
            } else if (data && data.err) {
                if (data.info) {
                    updateFsUsage(data.info);
                    updateAlerts(data.info);
                }
                progressModalError("Deleting File: " + fullPath + " err: " + data.err,
                   function() {
                        fail(data.err);
                    });
            } else {
                progressModalError("Deleting File: " + fullPath,
                   function() {
                        fail(data.err);
                   });
            }
        })
        .fail(function() {
            progressModalError("Deleting File: " + fullPath + " err: " + data.err, fail);
            //fail();
        });
}

BrowserActionsOnline.prototype.uploadFile = function(fullPath, props, fileObj, signFileObj, pem_replace, done, fail ) {
    var xhr = new XMLHttpRequest();
    var formData = new FormData();
    
    formData.append("fullPath", fullPath);
    formData.append("props", JSON.stringify(props));
    formData.append("source", fileObj);
    formData.append("name", gProjectName);
    formData.append("pem_replace", pem_replace);
    if (signFileObj) {
        formData.append("sign_file", signFileObj);
    }
    
    var that = this;
    xhr.onreadystatechange = function() {
        progressModalOpen("Writing File", true);
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                getProgressStatus(100, 
                    function(data) {
                        if (data.info) {
                            updateFsUsage(data.info);
                        }
                        done(data);
                    }, false, 
                    function(data) { 
                        if (data.info) {
                            updateAlerts(data.info);
                        }
                        that.refresh.call(that)
                    });
            }
            else if (data && data.err) {
               if (data.info) {
                    updateFsUsage(data.info);
                    updateAlerts(data.info);
                }
                progressModalError("Uploading, err:" + data.err);
                fail(data.err);
            }
            else {
                progressModalError("Uploading");
                fail(data.err);
            }
        } else if (xhr.status > 200) {
            progressModalError("Uploading");
            fail();
        }
    }
    
    //progressModalOpen("Writing File");
    xhr.open("POST", "/api/1/flash/devModeUploadFile", true);
    xhr.send(formData);
}

BrowserActionsOnline.prototype.renameFolder = function(folder, newName, done, fail) {
    done();
}


BrowserActionsOnline.prototype.refresh = function() {
    var that = this;
    
    $.post("/api/1/flash/getFileList")
        .done(function(data) {
            if (data.ok) {
                that.browser.load(data.ok);
                that.browser.render();
                if (data.info) {
                    updateFsUsage(data.info);
                }
            } else if (data.err) {
                showInputDialog("Error refreshing: " + data.err, false, 
                    null,
                    {text: "Close"});
            } else {
                showInputDialog("Error refreshing: " + data.err, false,
                    null,
                    {text: "Close"});
            }
        })
        .fail(function() {
            showInputDialog("Error refreshing", false,
                null,
                {text: "Close"});
        });
}

BrowserActionsOnline.prototype.refreshFile = function(file) {
    $.post("/api/1/flash/getFileList")
        .done(function(data) {
            if (data.ok) {
                var fb = new Browser(null, "online");
                fb.load(data.ok);

                path = file.fullPath.split("/");
                
                
            } else if (data.err) {
                showInputDialog("Error refreshing: " + data.err, false,
                    null,
                    {text: "Close"});
            } else {
                showInputDialog("Error refreshing: " + data.err, false,
                    null,
                    {text: "Close"});
            }
        })
        .fail(function() {
            showInputDialog("Error refreshing", false,
                null,
                {text: "Close"});
        });
}

BrowserActionsOnline.prototype.removeSelected = function(done, fail) {
    if (done) {
        done();
    }
}

BrowserActionsOnline.prototype.addFolder = function(name, done, fail) {
    if (done) {
        done();
    }
}

BrowserActionsOnline.prototype.deleteFolder = function(fullPath, done, fail) {
    if (done) {
        done({ok: true});
    }
}

BrowserActionsOnline.prototype.uploadSignFile = function(fullPath, props, fileObj, signFileObj, done, fail) {
    if (done) {
        done({ok: true});
    }
}

BrowserActionsOnline.prototype.uploadMCUKeySignFile = function (fullPath, props, fileObj, signFileObj, done, fail) {
    if (done) {
        done({ ok: true });
    }
}

BrowserActionsOnline.prototype.uploadDone = function(props) {
    if (!props.flagSecure) {
        progressModalClose();
    }
}

BrowserActionsOnline.prototype.requireToken = function(props) {
    if (props.flagSecure && !props.flagPublicWrite) {
        return true;
    }
}

BrowserActionsOnline.prototype.showWarnings = function(div, props) {

}


function BrowserActionsOffline(browser) {
    this.browser = browser;
}

BrowserActionsOffline.prototype.uploadFile = function(fullPath, props, fileObj, signFileObj, pem_replace, done, fail ) {
    var xhr = new XMLHttpRequest();
    var formData = new FormData();
    
    formData.append("dest", fullPath);
    formData.append("props", JSON.stringify(props));
    formData.append("source", fileObj);
    //if (signFileObj) {
    //    formData.append("sign_file", signFileObj);
    //}
    formData.append("name", gProjectName);
    formData.append("pem_replace", pem_replace);
    
    
    var that = this;
    xhr.onreadystatechange = function() {
        progressModalOpen("Writing File");
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                /*
                if (signFileObj && signFileObj.name) {
                    that.uploadSignFile(fullPath, props, fileObj, signFileObj, 
                        function(ok) {
                            done();
                        },
                        function(err) {
                            props.signatureFileName = "";
                            done(props);
                            fail(err);
                        });
                } else {
                    done();
                    progressModalClose();
                }
                */
                done();
            } else if (data && data.err) {
                progressModalError("Uploading, err:" + data.err);
                fail(data.err);
            } else {
                progressModalError("Uploading");
                fail(data.err);
            }
        } else if (xhr.status > 200) {
            progressModalError("Uploading");
            fail();
        }
    }
    
    //progressModalOpen("Writing File");
    xhr.open("POST", "/api/1/flash/uploadProjectFsFile", true);
    xhr.send(formData);
}

BrowserActionsOffline.prototype.uploadMCUFileSimple = function (fullPath, props, fileObj, signFileObj, done, fail) {
    var xhr = new XMLHttpRequest();
    var formData = new FormData();

    formData.append("dest", fullPath);
    formData.append("props", JSON.stringify(props));
    formData.append("source", fileObj);
    //if (signFileObj) {
    //    formData.append("sign_file", signFileObj);
    //}
    formData.append("name", gProjectName);


    var that = this;
    xhr.onreadystatechange = function () {
        progressModalOpen("Writing File");
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                /*
                if (signFileObj && signFileObj.name) {
                    that.uploadSignFile(fullPath, props, fileObj, signFileObj, 
                        function(ok) {
                            done();
                        },
                        function(err) {
                            props.signatureFileName = "";
                            done(props);
                            fail(err);
                        });
                } else {
                    done();
                    progressModalClose();
                }
                */
                done();
                progressModalClose();
            } else if (data && data.err) {
                progressModalError("Uploading, err:" + data.err);
                fail(data.err);
            } else {
                progressModalError("Uploading");
                fail(data.err);
            }
        } else if (xhr.status > 200) {
            progressModalError("Uploading");
            fail();
        }
    }

    //progressModalOpen("Writing File");
    xhr.open("POST", "/api/1/flash/uploadMCUSimpleMode", true);
    xhr.send(formData);
}
BrowserActionsOffline.prototype.deleteFile = function(fullPath, token, done, fail) {
    var that = this;
    
    progressModalOpen("Deleting File: " + fullPath);
    
    $.post("/api/1/flash/deleteProjectFsFile", {name: gProjectName, source: fullPath})
        .done(function(data) {
            if (data && data.ok) {
                done(data.ok);
                //saveUserFilesAPI(that.browser.dump(), function() {
                
                //});
                setTimeout(progressModalClose, 1000);
            } else if (data && data.err) {
                progressModalError("Deleting File: " + fullPath + " err: " + data.err,
                    function() {
                        fail(data.err);
                    });
                return;
            } else {
                progressModalError("Deleting File: " + fullPath, fail);
                
                return;
            }
            
            //done();

        })
        .fail(function() {
            fail();
        });
}


BrowserActionsOffline.prototype.changeProps = function(file, fullPath, pd, signFileObj, done, fail) {
    var that = this;

    //if (file.parent.findFile(pd.props().name)) {
    //    pd.showError("File with the same name already exists in this folder");
    //    return;
    //}

    var ret = checkValidName(pd.props().name, joinFullPath(file.parent.fullPath(), pd.props().name));
    if (ret.err) {
        fail(ret.err);
        return;
    }

    var props = pd.props();
    if (props.flagSecure && props.flagVendor) {
        if (!$.isNumeric(props.fileToken)) {
            pd.showError("Token has to be an integer");
            return;
        }
        
        if (props.fileToken > 0xFFFFFFFF || props.fileToken < 0) {
            pd.showError("Token cannot be smaller than zero or bigger than 4294967295");
            return;
        }
    }

    if (Number(props.maxFileSize ) < Number(file.origFileSize))
    {
        pd.showError("Max Size is smaller than file's original size: " + file.origFileSize + " bytes");
        return;
    }

    //$.extend(file, pd.props());   // don't update before completion
    $.post("/api/1/flash/renameProjectFsFile", {name: gProjectName, source: fullPath, dest: pd.props().name})
        .done(function(data) {
            if (data && data.ok) {
                //saveUserFilesAPI(that.browser.dump(), function() {
                file.name = pd.props().name;
                var newFullPath = data.ok
                $.post("/api/1/flash/changeProjectFsFileProps", {name: gProjectName, source: newFullPath, props: JSON.stringify(pd.props())})
                    .done(function(data) {
                        if (data && data.ok) {
                            //$.extend(file, pd.props());
                            //var newProps = pd.props();
                            //newProps = file.signatureFileName
                            //done();
                            
                            if (signFileObj && signFileObj.name) {
                                //var fullPath = file.fullPath();
                                //gBrowserActions.uploadSignFile(fullPath, pd.props(), {}, signFileObj, function() { 
                                gBrowserActions.uploadSignFile(newFullPath, pd.props(), {}, signFileObj, 
                                    function() { 
                                        var props = pd.props();
                                        if (props.signatureFileNameSelect == "1") {
                                            props.signatureFileNameSelect = "0";
                                            props.signatureFileName = "generated_signature.bin";
                                            pd.props(props)
                                        }

                                        done(); 
                                        pd.close(); 
                                    }, 
                                    function() {
                                        props.signatureFileName = ""
                                        pd.props(props);
                                        fail();
                                    });
                            } else if (file.signatureFileName !== "" && pd.props().signatureFileName === "") {
                                $.post("/api/1/flash/deleteProjectSigFile", {name: gProjectName, dest: newFullPath})
                                    .done(function(data) { 
                                        if (data && data.ok) {
                                            pd.props({signatureFileNameSelect: "0"});
                                            done(); 
                                            pd.close(); 
                                        } else {
                                            fail(data.err);
                                        }
                                    })
                                    .fail(fail);
                            } else {
                                done();
                                pd.close();
                            }
                        } else {
                            fail(data.err);
                        }
                    })
                    .fail(function(data) {fail});
            } else {
                fail(data.err);
            }
        })
        .fail(function() {
            fail("Error");
        });
    
    //that.uploadSignFile(fullPath, props, fileObj, signFileObj, done, fail);
    
}


BrowserActionsOffline.prototype.uploadSignFile = function(fullPath, props, fileObj, signFileObj, done, fail) {
var xhr = new XMLHttpRequest();
    var formData = new FormData();
    
    formData.append("dest", fullPath);
    formData.append("source", signFileObj);
    formData.append("name", gProjectName);

    if (props.signatureFileNameSelect == "1") {
        formData.append("is_pk", true)
    }
    
    var that = this;
        
    xhr.onreadystatechange = function() {
        progressModalOpen("Writing File");
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                //getProgressStatus(100, null);
                //don't call done();
                done();
                
                //saveUserFilesAPI(that.browser.dump(), function() {
                //    progressModalClose(true);
                //}, fail);
                
            }
            else if (data && data.err) {
                progressModalError("Uploading, err: " + data.err);
                fail(data.err);
            }
            else {
                progressModalError("Uploading");
                fail(data.err);
            }
        } else if (xhr.status > 200) {
            progressModalError("Uploading");
            fail();
        }
    }
    
    //progressModalOpen("Writing File");
    xhr.open("POST", "/api/1/flash/uploadProjectSigFile", true);
    xhr.send(formData); 
 
}

BrowserActionsOffline.prototype.uploadMCUKeySignFile = function (fullPath, props, fileObj, signFileObj, done, fail) {
    var xhr = new XMLHttpRequest();
    var formData = new FormData();

    formData.append("dest", fullPath);
    //formData.append("source", signFileObj);
    formData.append("name", gProjectName);

    if (props.signatureFileNameSelect == "1") {
        formData.append("is_pk", true)
    }

    var that = this;

    xhr.onreadystatechange = function () {
        progressModalOpen("Writing File");
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                //getProgressStatus(100, null);
                //don't call done();
                done();

                //saveUserFilesAPI(that.browser.dump(), function() {
                //    progressModalClose(true);
                //}, fail);

            }
            else if (data && data.err) {
                progressModalError("Uploading, err: " + data.err);
                fail(data.err);
            }
            else {
                progressModalError("Uploading");
                fail(data.err);
            }
        } else if (xhr.status > 200) {
            progressModalError("Uploading");
            fail();
        }
    }

    //progressModalOpen("Writing File");
    xhr.open("POST", "/api/1/flash/uploadMCUKeySimpleMode", true);
    xhr.send(formData);

}

BrowserActionsOffline.prototype.renameFolder = function(folder, newName, done, fail) {
    var that = this;
    $.post("/api/1/flash/renameProjectFsDir", {"name": gProjectName, "source": folder.fullPath(), "dest": newName})
        .done(function(data) {
            if (data && data.ok) {
                done(data.ok);
                //saveUserFilesAPI(that.browser.dump(), done, fail);
            } else {
                fail(data.err);
            }
        })
        .fail(function() {
            fail("Error");  
        });
}

BrowserActionsOffline.prototype.uploadMCUImage = function(fDevice) {
    //var mcuImgName = gMCUImgName;
    //if(fDevice & is_pk)
        //mcuImgName= 'mcuflashimg.bin';
    //gFdevice = fDevice;
    var folder = this.browser.root.findFolder("sys");
    if (!folder) {
        folder = this.browser.root.addFolder("sys");
        folder.parent.render();
    }
    
    //if (fDevice) {
    //    mcuImgName = fDeviceMCUImgName
    //}
    
    mcuImgName = getMCUImgNameAndPath(fDevice).name;
    
    var event = {data: {that: folder, newName: mcuImgName}};
    uploadFileWithBrowse(event);
}
/*
BrowserActionsOffline.prototype.uploadMCUImageSimple = function (fDevice, data) {

    var folder = this.browser.root.findFolder("sys");
    if (!folder) {
        folder = this.browser.root.addFolder("sys");
        //folder.parent.render();
    }

    mcuImgName = getMCUImgNameAndPath(fDevice).name;

    folder.uploadFileSimpleNoBr(data, mcuImgName);
}
*/
BrowserActionsOffline.prototype.uploadFileSimpleNoBr = function (data, name) {

    var folder = this.browser.root;

    folder.uploadFileSimpleNoBr(data, name);

}

BrowserActionsOffline.prototype.removeSelected = function(folders, files, done, fail) {
    var that = this;
    var fileIndex = 0;
    var folderIndex = 0;
    
    function deleteFolders() {
        if (folders.length == 0) {
            deleteFiles();
            return;
        }
        
        that.deleteFolder(folders[folderIndex],
            function() {
                folderIndex++;
                if (folderIndex == folders.length) {
                    deleteFiles();
                } else {
                    deleteFolders();
                }
            },
            fail
        );
    }
            
    function deleteFiles() {
        if (files.length == 0) {
            done();
            return;
        }
        
        that.deleteFile(files[fileIndex], 0, 
            function() {
                fileIndex++;
                if (fileIndex == files.length) {
                    done();
                } else {
                    deleteFiles();
                }
            },
            fail
        );
    }
    
    deleteFolders();
}

BrowserActionsOffline.prototype.downloadFile = function(fullPath, token) {
    progressModalOpen("Reading File");
    
    $.post("/api/1/flash/downloadProjectFile", {name: gProjectName, fullpath: fullPath})
        .done(function(data) {
            if (data.ok) {
                getProgressStatus(100, function(data) { 
                    fileLink[0].href = '/api/1/flash/devModeDownloadFileGet?t=' + new Date().getTime() + '&r=' + Math.random();
                    fileLink[0].click() 
                });
            } else {
                progressModalError("Downloading, err:" + data.err);
            }
        })
        .fail(function() {
            progressModalError("Downloading");
        });
}

BrowserActionsOffline.prototype.addFolder = function(name, done, fail) {
    $.post("/api/1/flash/addProjectFsDir", {"name": gProjectName, "dirname": name})
        .done(function(data) {
            if (data && data.ok)
                done(data);
            else
                fail(data);
            
        })
        .fail(function(data) {
            fail(data);
        });
        
    //saveUserFilesAPI(this.browser.dump(), done, fail);
}

BrowserActionsOffline.prototype.deleteFolder = function(fullPath, done, fail) {
    $.post("/api/1/flash/deleteProjectFsDir", {"name": gProjectName, "source": fullPath})
    .done(function(data) {
        if (data && data.ok)
            done(data);
        else
            fail(data);
        
    })
    .fail(function(data) {
        fail(data);
    });
}

BrowserActionsOffline.prototype.uploadDone = function(props) {
    //progressModalClose();
    setTimeout(progressModalClose, 1000);
}

BrowserActionsOffline.prototype.requireToken = function(props) {
    return false;
}

BrowserActionsOffline.prototype.showWarnings = function(div, props) {
    if (props.flagSecure && (!props.flagVendor && !props.flagPublicWrite)) {
        div.find("span#tokenError").text("Warning: the file will not be overwriteable without a known token (Vendor flag) or public write access (Public Write flag)");
    } else {
        div.find("span#tokenError").text("");
    }
}

var propsDiv = (
'<div class="white-popup">' +
'	<div class="wrapPopup">' +
'		<div class="filePathNN">' +
'			<span>' +
'			<div class="fieldName" id="fileName">File Name:</div>' +
'               <span id="textFolderName"></span>' +
'				<input type="text" value="" id="textFileName">' +
'               <span id="fileError" class="uRed"></span>' +
'			</span>' +
'		</div>' +

'		<div class="wrapDRS floatRight mright15 ">' +
'			<span>' +
'			<div class="fieldName" style="color:#cc0000; font-weight:bold" data-tooltip="Max size should take into consideration future file updates">Max File Size: <span style="color:#555;">(actual size: <span id="origFileSize"></span><span> bytes)</span></span></div>' +
'			<input id="fileSize" type="number" class="sMargin" min="0">' +
'			</span>' +
'		</div>' +
//'		<div class="clearM  mbot25"></div>' +

'		<div class="mbot25"></div>' +
'		<div class="WrapChkBx">' +
'			<div class="floatLeft">' +
'				<div class="chkBx">' +
'					<input type="checkbox" id="flagFailSafe">' +
'					Failsafe' +
'				</div>' +
'				<div class="chkBx">' +
'					<input type="checkbox" id="flagSecure">' +
'					Secure' +
'				</div>' +
'				<div class="chkBx mleft20">' +
'					<input type="checkbox" id="flagStatic">' +
'					Static' +
'				</div>' +
'				<div class="chkBx mleft20">' +
'					<input type="checkbox" id="flagVendor">' +
'					Vendor' +
'				</div>' +
'			</div>' +
'		</div>' +
'		<div class="mbot25 clearS"></div>' +
'		<div class="wrapDRS floatLeft mleft20">' +
'			<div class="fieldName">File Token:</div>' +
'			<input type="text" value="" id="fileToken">' +
'           <br><span id="tokenError" class="uRed"></span>' +
'		</div>' +
'		<div class="mbot25 clearS"></div>' +
'		<div class="WrapChkBx">' +
'           <div class="floatLeft mleft20">' +
'				<div class="chkBx">' +
'					<input type="checkbox" id="flagPublicWrite">' +
'					Public Write' +
'				</div>' +
'				<div class="chkBx">' +
'					<input type="checkbox" id="flagPublicRead">' +
'					Public Read' +
'				</div>' +
'             <div class="chkBx">' +
'					<input type="checkbox" id="flagNoSignatureTest">' +
'					No Signature Test' +
'				</div>' +
'			</div>' +
'		</div>' +  
'		<div class="clearM mbot25"></div>' +          
'			<fieldset class = "wrapSectionF mleft20" >' +
//'			<div class="fieldName ">Signature File Name:</div>' +
'			<div class="wrapDRS">' +             
//'			<div class="" data-tooltip="In Private key option you should increase you MAX SIZE on 20 bytes">'+
'			<div class="">'+
'                <select id="userSigFileSelect">' +
'				 <option value="0">Signature File Name:</option>' +
'				 <option value="1">Private Key File Name:</option>' +
'			     </select>' +
'           </div>' +
'           <div class="clearS mbot25"></div>' +
'				<div class="floatLeft">' +
'					<input type="text" id="userSigFileInput">' +
'				</div>' +
'				<div id="browseUserSigFile" class="regButtonBr mtop0 mLeftS">' +
//'					<id="browseUserSigFile">' +
'					Browse' +
'				</div>' +
'				<div id="clearUserSigFile" class="regButton mtop0 mLeftS">' +
//'					<id="clearUserSigFile">' +
'					Clear' +
'				</div>' +
'				<div class="hideFile">' +
'					<input type="file" id="USER_SIG_FILE">' +
'				</div>' +
'			</div>' +
'			</fieldset>' + 
'		<div class="clearM  mbot25"></div>' +
'		<div class="wrapDRS floatLeft mleft20">' +
'			<div class="fieldName">Certification File Name:</div>' +
'			<select id="fileCertification">' +
'				<option value="">None</option>' +
'			</select>' +
'		</div>' +
/*'		<div class="wrapDRS floatRight noMargin">' +
'			<div class="fieldName">Max Size:</div>' +
'			<input id="fileSize" type="number" class="sMargin" min="0">' +
'		</div>' +*/
'		<div class="clear"></div>' +
'		<div id="saveBtn" class="regButton">Save</div>&nbsp;&nbsp;' +
'		<div id="cancelBtn" class="regButton" style="margin-left: 20px;">Cancel</div>' +
'		<div class="clearM  mbot25"></div>' +
'       <div id="error" hidden></div>' +
'	</div>' +
'</div>'
);

function PropsDialog(disableAll) {
    this.div = $(propsDiv);
    
    this.toDisableAll = disableAll;
    
    dropSignFileObj = null;
    fileInput.val("");
    fileInputSign.val("");
    
    this._props = {};
    
    if (!disableAll) {
        var that = this;
        this.div.find("#saveBtn").on("click", {that: that}, this.save);
        //this.div.find("#saveBtn").on("click", { that: that }, this.saveDialog);
        
        var div = this.div
        fileInputSign.on("change", function(event) {
            dropSignFileObj = null;
            div.find("#userSigFileInput").val(fileInputSign.prop("files")[0].name);
        });
        
        this.div.find("#flagSecure").on("click", {that: this}, this.change);
        this.div.find("#flagVendor").on("click", {that: this}, this.change);
        this.div.find("#flagPublicWrite").on("click", {that: this}, this.change);
        this.div.find("#flagNoSignatureTest").on("click", {that: this}, this.change);
        
        this.div.find("#userSigFileInput").on("dragenter", {that: this}, this.drag);
        this.div.find("#userSigFileInput").on("dragleave", {that: this}, this.drag);
        this.div.find("#userSigFileInput").on("dragover", {that: this}, this.drag);
        this.div.find("#userSigFileInput").on("drop", {that: this}, this.dropSignFile);
    } else {
        this.disableAll();
    }
    
    this.div.find("#cancelBtn").on("click", $.magnificPopup.close);
    //this.div.find("#cancelBtn").on("click", {}, this.closeDialog);
    this.div.find("#userSigFileSelect").val("1");
}

function dialogClosedCallback() {
    if (callbackClose != undefined) {
        callbackClose.call();
    }
}

function dialogSavedCallback() {
    if (callbackSave != undefined) {
        callbackSave.call();
    }
}


function dialogDeleteHttpCertCallback() {
    if (callbackDeleteHttpCert != undefined) {
        callbackDeleteHttpCert.call();
    }
}

function dialogDeleteHttpKeyCallback() {
    if (callbackDeleteHttpKey != undefined) {
        callbackDeleteHttpKey.call();
    }
}
function dialogDeleteHttpCaCertCallback() {
    if (callbackDeleteHttpCaCert != undefined) {
        callbackDeleteHttpCaCert.call();
    }
}

function setHttpCallBack(cert_cb, key_cb, ca_cert_cb)
{
    callbackDeleteHttpCert = cert_cb;
    callbackDeleteHttpKey = key_cb;
    callbackDeleteHttpCaCert = ca_cert_cb;
}

PropsDialog.prototype.closeDialog = function () {
    $.magnificPopup.close();
    dialogClosedCallback();
}
/*
PropsDialog.prototype.saveDialog = function () {
    dialogSavedCallback();
    this.save();
}
*/
PropsDialog.prototype.saveDialog = function (event) {
    
    var that = event.data.that;

    if (that.saveFunc) {
        that.saveFunc(that);
    }
    dialogSavedCallback();
}

PropsDialog.prototype.open = function(obj, saveFunc, okBtnText) {     
    var opts = this.div.find("#fileCertification");
    opts = opts.empty()[0];
    opts.options.add(new Option(""));
    
    /* "cert" directory is unsufficient for chanined certificates and therefor deprecated in favor of just using the root folder
    var cert = folder.root().findFolder("cert");
    if (cert && opts) {
        $.each(cert.files, function(i, file) {
            opts.options.add(new Option(file.fullPath()));
        });
    }
    */
    
    if (opts) {
        $.each(obj.root().files, function(i, file) {
            opts.options.add(new Option(file.fullPath()));
        });
    }
    
    if (obj.certificationFileName) {
        $(opts).val(obj.certificationFileName);
    }
    
    this.saveFunc = saveFunc;
    
    this.change({data: {that: this}});
    if (this.toDisableAll) {
        this.disableAll();
    } else if (this.forOverwrite) {
        this.disableAll(this.forOverwrite);
    }
    
    fileInputSign.val("");
    
    
    if (okBtnText)
        this.div.find("#saveBtn").html(okBtnText);
    
    $.magnificPopup.open({
        items: {
            src: this.div,
            type: 'inline'
        },
        modal: true
    });
}

PropsDialog.prototype.showError = function(text) {    
    this.div.find("#fileError").html(text).show();
}

PropsDialog.prototype.hideError = function(text) {
    this.div.find("#fileError").html("").hide();
}

PropsDialog.prototype.props = function(obj) {
    if (obj === undefined) {
        obj = {};
        obj.name = this.div.find("#textFileName").val().toLowerCase();
        obj.maxFileSize = this.div.find("#fileSize").val();
        obj.origFileSize = this.div.find("#origFileSize").text();
        obj.flagFailSafe = this.div.find("#flagFailSafe")[0].checked
        obj.flagSecure = this.div.find("#flagSecure")[0].checked
        obj.flagVendor = this.div.find("#flagVendor")[0].checked
        obj.flagStatic = this.div.find("#flagStatic")[0].checked
        obj.flagNoSignatureTest = this.div.find("#flagNoSignatureTest")[0].checked
        obj.flagPublicWrite = this.div.find("#flagPublicWrite")[0].checked
        obj.flagPublicRead = this.div.find("#flagPublicRead")[0].checked
        obj.fileToken = this.div.find("#fileToken").val();
        obj.certificationFileName = this.div.find("#fileCertification").val();
        obj.signatureFileName = this.div.find("#userSigFileInput").val();
        obj.signatureFileNameSelect = this.div.find("#userSigFileSelect").val();
        //obj.origFileName = this.div.find("#origFileName").text();
        obj.origFileName = this._props.origFileName;
    } else {
        if (obj.name) {
            this.div.find("#textFileName").val(obj.name);
        }

        if (obj.folderName) {
            this.div.find("#textFolderName").text(obj.folderName);
        }
        
        if (obj.maxFileSize) {
            this.div.find("#fileSize").val(obj.maxFileSize);
        }
        
        if (obj.origFileSize) {
            //this.div.find("#origFileSize").html('(original size is ' + obj.origFileSize + ')') ;
            this.div.find("#origFileSize").text(obj.origFileSize) ;
        }
        
        if (obj.flagFailSafe) {
            this.div.find("#flagFailSafe")[0].checked = obj.flagFailSafe;
        }
        
        if (obj.flagSecure) {
            this.div.find("#flagSecure")[0].checked = obj.flagSecure;
        }
        
        if (obj.flagVendor) {
            this.div.find("#flagVendor")[0].checked = obj.flagVendor;
        }
        
        if (obj.flagStatic) {
            this.div.find("#flagStatic")[0].checked = obj.flagStatic;
        }
        
        if (obj.flagNoSignatureTest) {
            this.div.find("#flagNoSignatureTest")[0].checked = obj.flagNoSignatureTest;
        }
        
        if (obj.flagPublicWrite) {
            this.div.find("#flagPublicWrite")[0].checked = obj.flagPublicWrite;
        }
        
        if (obj.flagPublicRead) {
            this.div.find("#flagPublicRead")[0].checked = obj.flagPublicRead;
        }
        
        if (obj.fileToken) {
            this.div.find("#fileToken").val(obj.fileToken);
        }
        
        if (obj.certificationFileName) {
            this.div.find("#fileCertification").val(obj.certificationFileName);
        }
        
        //if (obj.signatureFileName) {
        if (obj.signatureFileName !== undefined) {
            this.div.find("#userSigFileInput").val(obj.signatureFileName);
        }
        
        if (obj.signatureFileNameSelect) {
            this.div.find("#userSigFileSelect").val(obj.signatureFileNameSelect);
        }
        
        if (obj.origFileName) {
            this._props.origFileName = obj.origFileName;
            var text = this.div.find("#fileName").text();
            // ToDo: don't display yet, find the right place to put this
            //this.div.find("#fileName").text(text + " (original name: " + obj.origFileName +")");
        }
    }
    
    return obj;
}

PropsDialog.prototype.save = function(event) {
    var that = event.data.that;
    
    if (that.saveFunc) {
        that.saveFunc(that);
    }
}

PropsDialog.prototype.close = function() {
    $.magnificPopup.close();
}

PropsDialog.prototype.change = function(event) {
    var that = event.data.that;
    var flagSecure = that.div.find("#flagSecure")[0];
    var flagNoSigTest =  that.div.find("#flagNoSignatureTest")[0];
    var flagVendor =  that.div.find("#flagVendor")[0];
    var flagStatic = that.div.find("#flagStatic")[0];
    var flagPublicRead = that.div.find("#flagPublicRead")[0];
    var flagPublicWrite = that.div.find("#flagPublicWrite")[0];
    var fileToken = that.div.find("#fileToken")[0];
    var fileSign = that.div.find("#userSigFileInput")[0];
    var fileSignSelect = that.div.find("#userSigFileSelect")[0];
    var fileCert = that.div.find("#fileCertification")[0];
    var textFileName = that.div.find("#textFileName")[0];
    
    //textFileName.disabled = ($(textFileName).val() == gMCUImgName);
    textFileName.disabled = ($(textFileName).val() == getMCUImgNameAndPath().name || $(textFileName).val() == getMCUImgNameAndPath(true).name) || textFileName.disabled;
    if (flagSecure.checked) {
        flagNoSigTest.disabled = false;
        flagVendor.disabled = false;
        flagStatic.disabled = false;
        flagPublicRead.disabled = false;
        flagPublicWrite.disabled = false;
        fileToken.disabled = !flagVendor.checked;
        fileSign.disabled = flagNoSigTest.checked;
        fileSignSelect.disabled = flagNoSigTest.checked;
        fileCert.disabled = flagNoSigTest.checked;
        if (!flagNoSigTest.checked) {
            that.div.find("#browseUserSigFile")
                .removeClass("btnDisabled")
                .off("click")
                .on("click", function() { 
                    fileInputSign.val("");
                    fileInputSign.click();
                });

            that.div.find("#clearUserSigFile")
                .removeClass("btnDisabled")
                .off("click")
                .on("click", function(event) {
                    that.div.find("#userSigFileInput").val("");
                    fileInputSign.val("");
                    dropSignFileObj = null;
                });
         
        } else {
            that.div.find("#browseUserSigFile")
                .addClass("btnDisabled")
                .off("click");

            that.div.find("#clearUserSigFile")
                .addClass("btnDisabled")
                .off("click");
        }
    } else {
        flagNoSigTest.disabled = true;
        flagVendor.disabled = true;
        flagStatic.disabled = true;
        flagPublicRead.disabled = true;
        flagPublicWrite.disabled = true;
        fileToken.disabled = true;
        fileSign.disabled = true;
        fileSignSelect.disabled = true;
        fileCert.disabled = true;

        that.div.find("#browseUserSigFile")
            .addClass("btnDisabled")
            .off("click");

        that.div.find("#clearUserSigFile")
            .addClass("btnDisabled")
            .off("click");   
    }
    
    gBrowserActions.showWarnings(that.div, that.props());
}

PropsDialog.prototype.disableAll = function(forOverwrite) {
    this.forOverwrite = forOverwrite;
    
    var that = this;
    var flagSecure = that.div.find("#flagSecure")[0];
    var flagNoSigTest =  that.div.find("#flagNoSignatureTest")[0];
    var flagVendor =  that.div.find("#flagVendor")[0];
    var flagStatic = that.div.find("#flagStatic")[0];
    var flagPublicRead = that.div.find("#flagPublicRead")[0];
    var flagPublicWrite = that.div.find("#flagPublicWrite")[0];
    var fileToken = that.div.find("#fileToken")[0];
    var fileSign = that.div.find("#userSigFileInput")[0];
    var fileSignSelect = that.div.find("#userSigFileSelect")[0];
    var fileCert = that.div.find("#fileCertification")[0];
    var flagFailSafe = that.div.find("#flagFailSafe")[0];
    
    var textFileName = that.div.find("#textFileName")[0];
    var maxFileSize = this.div.find("#fileSize")[0];
    var origFileSize = this.div.find("#origFileSize")[0];
    var saveBtn = this.div.find("#saveBtn");
    
    
    flagNoSigTest.disabled = true;
    flagVendor.disabled = true;
    flagStatic.disabled = true;
    flagPublicRead.disabled = true;
    flagPublicWrite.disabled = true;
    fileToken.disabled = true;
    //fileSign.disabled = true;
    //fileSignSelect.disabled = true;
    //fileCert.disabled = true;
    flagSecure.disabled = true;
    flagFailSafe.disabled = true;
    
    textFileName.disabled = true;
    maxFileSize.disabled = true;
    
    if (forOverwrite) {
        fileToken.disabled = !gBrowserActions.requireToken(this.props());
        return;
    }

    fileSign.disabled = true;
    fileSignSelect.disabled = true;
    fileCert.disabled = true;


    
    that.div.find("#browseUserSigFile")
        .addClass("btnDisabled")
        .off("click");

    that.div.find("#clearUserSigFile")
        .addClass("btnDisabled")
        .off("click");
        
    saveBtn.addClass("btnDisabled").off("click");

    that.div.find("#fileCertification").addClass("fb-cert-disabled");
    $(that.div.find("#fileCertification")[0].options[0]).addClass("fb-cert-disabled");
}

PropsDialog.prototype.drag = function(e) {
    e.preventDefault();
	e.stopPropagation();
}

PropsDialog.prototype.dropSignFile = function(e) {
    var that = e.data.that;
    
    e.preventDefault();
	e.stopPropagation();
    
    dropSignFileObj = e.originalEvent.dataTransfer.files[0];
    that.div.find("#userSigFileInput").val(dropSignFileObj.name);
}

PropsDialog.prototype.disableRename = function() {
    var textFileName = this.div.find("#textFileName")[0];
    textFileName.disabled = true;
}


function joinFullPath(folderPath, fileName, isFolder) {
    if ((!folderPath || folderPath == "/") && !isFolder) {
        return fileName;
    }
    
    return folderPath + "/" + fileName;
}

function findMaxPathLen(root, maxPathLen) {
    if (maxPathLen === undefined) {
        maxPathLen = root.fullPath().length + 1;
    }
    
    $.each(root.folders, function(i, folder) {
        var fullPathLen = folder.fullPath().length + 1;
        maxPathLen = Math.max(maxPathLen, fullPathLen);
        
        maxPathLen = findMaxPathLen(folder, maxPathLen);
    });
    
    $.each(root.files, function(i, file) {
        var fullPathLen = file.fullPath().length;
        maxPathLen = Math.max(maxPathLen, fullPathLen);
    });
    
    return maxPathLen;
}
    


var overwriteDiv = (
'		<div class="clearM mbot25"></div>' +
'			<div class="wrapDRS">' + 
'			<div class="fieldName">File Name:</div>' +
'				<div class="floatLeft">' +
'					<input type="text" id="userFileInput">' +
'				</div>' +
'				<div id="browseUserFile" class="regButton mtop0 mLeftS">' +
'					Browse' +
'				</div>' +
'				<div id="clearUserFile" class="regButton mtop0 mLeftS">' +
'					Clear' +
'				</div>' +
'			</div>'
);


function overwriteDialog() {
    this.div = $(overwriteDiv);
    
}

overwriteDialog.prototype.open = function() {
    $.magnificPopup.open({
        items: {
            src: this.div,
            type: 'inline'
        },
        modal: true
    });
}

var invalidCharsPatt = new RegExp("[\\<\\>:\"/\\\\|\\?\\*]");
var maxFullPathLen = 179; //240;
//var maxPathLen = 190 // windows limit

function checkValidName(name, fullPath) {
    if (name.trim().toLowerCase() == "con") {
        return {err: '"con" is a reserved file name'};
    }
    
    //if (name.trim() === "" || name.startsWith(" ") || name.endsWith(" ")) { // startsWith/endsWith not supported in ie11 ...
    if (name.trim() === "" || name.indexOf(" ") == 0 || name.lastIndexOf(" ") == name.length - 1) {
        return {err: "Name cannot be: empty, made out of spaces only, start with space/end with space"};
    }
    
    if (invalidCharsPatt.test(name)) {
        return {err: "Invalid characters in name: < > \ / ? * :"};
    }

    //if (name.length > maxPathLen) {
    //    return {err: "File/folder name is too long (>" + maxPathLen + ")"};
    //}
    
    if (fullPath.length > maxFullPathLen) {
        return {err: "Full path is too long (" + fullPath.length + ">" + maxFullPathLen + ")"};
    }
    
    return {ok: true};
}

function getMCUImgNameAndPath(isFDevice) {
    if (isFDevice) {
        return {name: sfDeviceMCUImgName, fullPath: "/sys/" + sfDeviceMCUImgName};
    }
    
    return {name: srDeviceMCUImgName, fullPath: "/sys/" + srDeviceMCUImgName};
}

function updateFsUsage(info) {
    //$("span#alertsId").html(info.alerts[0] + " / " + info.alerts[1]);
    $("span#storageSizeId").html(info.storage_size + "KB")
    $("span#actualUserFilesSizeId").html(info.available_user_files_size + "KB")
}

function updateAlerts(info) {
    //$("span#alertsId").html(info.alerts[0] + " / " + info.alerts[1]).addClass("fb-ti-red").css("font-weight", "bold");
    $("span#alertsId").html(info.alerts[0] + " / " + info.alerts[1]).css("color", "red").css("font-weight", "bold");
}

var fileInput = null;
var fileInputSign = null;
var fileLink = null;

var dropSignFileObj = null;

var propsDialog;

var gInfoPane;

return {Folder: Folder,
        File: File,
        Browser: Browser,
        PropsDialog: PropsDialog,
        showInputDialog: showInputDialog,
        updateHttpKeyFileName: updateHttpKeyFileName,
        updateHttpCertFileName: updateHttpCertFileName,
        updateHttpCaCertFileName: updateHttpCaCertFileName,
        dialogClosedCallback:dialogClosedCallback,
        dialogSavedCallback: dialogSavedCallback,
        setHttpCallBack: setHttpCallBack,
        getPemMessage: getPemMessage}
                
})(jQuery);
