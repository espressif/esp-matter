projectList = function($) {

var row = (
"<div class='recentFile fb-text'>" +
"<span>" +
"<span class='fa fa-folder-open leftIcon'></span>" +
"<span id='text' class='inTitle fb-text'><span>" +
"<div>"
)

function ProjectListItem(parent, name) {
    this.div = $(row);
    this.parent = parent;
    parent.div.append(this.div);
    
    this.div.on("click", {that: this}, function(e) {
        var that = e.data.that;
        that.parent.div.children().removeClass("selectedRow");
        that.div.addClass("selectedRow");
    });
    
    this.div.on("dblclick", {that: this}, function(e) {
        openProject(e.data.that._name);
    });
    
    this._name = name;
    this.name(name)
}

ProjectListItem.prototype.name = function(name) {
    if (name === undefined) {
        return this._name
    }
    
    this.div.find("span#text").text(name);
}

var list = (
//"<div id='recentProjectsDiv' class='projScrollBox'>"
//"<div id='recentProjectsDiv' class='fb-in12'>"
"<div id='recentProjectsDiv' class='fb-in'>"
)

function ProjectList(parent) {
    this.div = $(list);
    this.div.appendTo(parent);
    
    this.projects = [];
    $("#pmiconopen").on("click", {that: this}, this.openProject);
    $("#pmicondelete").on("click", {that: this}, this.deleteProject);
    $("#pmiconexport").on("click", {that: this}, this.exportProject);
    $("#pmimportproject").off("change").on("change", {that: this}, this.importProject);
    $("#pmiconimport").off("click").on("click", function() {
        $("#pmimportproject").val(""); 
        $("#pmimportproject").click();
    });
    $("#pmiconrename").on("click", {that: this}, this.displayProjectName);
}

ProjectList.prototype.add = function(name) {
    var item = new ProjectListItem(this, name);
    this.projects.push(item);
}

ProjectList.prototype.load = function(block) {
    var that = this;
    
    if (block) {
        progressModalOpen("Refreshing project list, please wait");
    }
    
    $.post("/api/1/flash/getProjectList", {})
        .done(function(data) {
            if (data && data.ok) {
                that.projects = [];
                that.div.empty();
                $.each(data.ok, function(i, name) {
                    that.add(name);
                });
                if (that.projects) {
                    that.projects[0].div.click();
                }
            } else if (data && data.err) {
                progressModalError("Error refreshing project list: " + data.err);
            } else {
                progressModalError("Error refreshing project list");
            }
        })
        .fail(function(data) {
            progressModalError("Error refreshing project list");
        })
    
    if (block) {
        setTimeout(progressModalClose, 1000);
    }
}

ProjectList.prototype.selected = function() {
    var found = null;
    
    $.each(this.projects, function(i, project) {
        if (found) {
            return;
        }
        
        if (project.div.hasClass("selectedRow")) {
            found = project;
        }
    });
    
    return found;
}

ProjectList.prototype.openProject = function(e) {
    var that = e.data.that;
    var selected = that.selected();
    if (!selected)
        return;

    openProject(selected.name())
}

ProjectList.prototype.deleteProject = function(e) {
    var that = e.data.that;
    var selected = that.selected();
    if (!selected)
        return;
    
    progressModalOpen("Deleting project, please wait");
    
    $.post("/api/1/flash/deleteProject",   {'name': selected._name})
        .done(function(data,status) {
            if (data && data.ok) {
                that.load();
                setTimeout(progressModalClose, 1000);
            } else if (data.err){
                progressModalError("Error deleting project: " + data.err);
            } else {
                progressModalError("Error deleting project");
            }
                
        })
        .fail(function(data) { progressModalError("Error deleting project"); });
}

ProjectList.prototype.exportProject = function(e) {
    var that = e.data.that;
    var selected = that.selected();
    if (!selected)
        return;

    progressModalOpen("Exporting project, please wait");
    
    $.post("/api/1/flash/exportProject", {name: selected._name})
        .done(function(data) {
            if (data && data.ok) {
                $("#pmExportLink").html("<a id='pmExportLinkPath' href='/api/1/flash/devModeDownloadFileGet'</a>");
                $("#pmExportLinkPath")[0].click();
                setTimeout(progressModalClose, 1000);
            } else if (data && data.err) {
                progressModalError("Error exporting project: " + data.err);
            } else {
                progressModalError("Error exporting project");
            }
        })
        .fail(function(data) {
            progressModalError("Error exporting project");
        });
}

ProjectList.prototype.importProject = function(e) {
    var source = $('#pmimportproject')[0].files[0];  
    if (!source) {
        return;
    }
    
    var that = e.data.that;
    
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                //update GUI
                that.load();
                setTimeout(progressModalClose, 1000);
            }
            else if (data && data.err) {
                progressModalError("Error importing project: " + data.err);
            }
            else {
                progressModalError("Error importing project");
            }
        }
    }
    
    var formData = new FormData();
    formData.append("source", source);
    
    xhr.open("POST", "/api/1/flash/importProject", true);
    
    progressModalOpen("Importing project, please wait");
    xhr.send(formData);
}

ProjectList.prototype.renameProject = function(that, oldProjectName, newProjectName) {
    var ret = verifyProjectName(newProjectName);
    if (ret.err) {
        $("span#renameProjectError").text(ret.err).css("color", "red");
        return;
    }
    progressModalOpen("Renaming project, please wait");
    
    $.post("/api/1/flash/renameProject",   {'oldName': oldProjectName, 'newName': newProjectName})
        .done(function(data,status) {
            if (data && data.ok) {
                //update GUI
                that.load();
                setTimeout(progressModalClose, 1000);
            } else if (data.err) {
                progressModalError("Error renaming project: " + data.err);
            } else {
                progressModalError("Error renaming project");
            }
        })
        .fail(function(data) { progressModalError("Error renaming project: " + data.err); });
}

var projectNameRename=('<div class="white-popup sysPopup">'+
	'	<div class="">'+
    '		<div class="">'+
	'			<b>Input new project name:</b> '+
	'			<span>'+
	'				<input class="txtPopup mtop30" type="text" id="textNewProjectName">'+
    '                <span id="renameProjectError"></span>' +
	'			</span>'+
    '   <div class="clearM" />' +
    '   <div class="clearM" />' +
    '	<div class="regButton  mtop50 mleft90"'+
	'		id="renameProjectOkButton">'+
    '		Rename'+
	'	</div>'+
    '   <div class="regButton  mtop50  mright10 mleft90"' +
    '		id="renameProjectCancelButton">'+
    '		&#8196;Cancel&#8196;'+
	'		</div>'+
    '   </div>' +
	'	</div>'+
	'</div>'
    );
  
ProjectList.prototype.displayProjectName = function(e) {
    var that = e.data.that;
    var selected = that.selected();
    if (!selected)
        return;

    var oldName = selected._name;
    
    var div = $(projectNameRename);
    
    var renameBtn = div.find("#renameProjectOkButton");
    var cancelBtn = div.find("#renameProjectCancelButton");
    var input     = div.find("#textNewProjectName");

    $(input).val(oldName);
    
    $(renameBtn).on("click",  function(e) {
        that.renameProject(that, oldName, $(input).val());
    });
    
    cancelBtn.on("click", $.magnificPopup.close);
    
	$.magnificPopup.open({
		items: {
			src     : div,
			type    : 'inline'
		},
        modal: true        
	});   
}

return {
    ProjectList: ProjectList
}

}(jQuery);
