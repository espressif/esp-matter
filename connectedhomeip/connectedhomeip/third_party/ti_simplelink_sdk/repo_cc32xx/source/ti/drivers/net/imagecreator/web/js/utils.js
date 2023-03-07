var projectName;
var project;
var folderstatus={};
var pageLoadFunction;
var g_tools_source_file;
var g_tools_key_file;
var g_tools_img_dec_key_file;
var g_open_project_image_file;
var g_open_project_image_key_file;
var gLoadedPage;
var gMainMenu;
var gMainMenuShort;
var gb_ShortMenu;
var gFileBrowser;

function getParameterByName(name) {
    name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
    var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(location.search);
    return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}

function beep() {
    var snd = new Audio("data:audio/wav;base64,//uQRAAAAWMSLwUIYAAsYkXgoQwAEaYLWfkWgAI0wWs/ItAAAGDgYtAgAyN+QWaAAihwMWm4G8QQRDiMcCBcH3Cc+CDv/7xA4Tvh9Rz/y8QADBwMWgQAZG/ILNAARQ4GLTcDeIIIhxGOBAuD7hOfBB3/94gcJ3w+o5/5eIAIAAAVwWgQAVQ2ORaIQwEMAJiDg95G4nQL7mQVWI6GwRcfsZAcsKkJvxgxEjzFUgfHoSQ9Qq7KNwqHwuB13MA4a1q/DmBrHgPcmjiGoh//EwC5nGPEmS4RcfkVKOhJf+WOgoxJclFz3kgn//dBA+ya1GhurNn8zb//9NNutNuhz31f////9vt///z+IdAEAAAK4LQIAKobHItEIYCGAExBwe8jcToF9zIKrEdDYIuP2MgOWFSE34wYiR5iqQPj0JIeoVdlG4VD4XA67mAcNa1fhzA1jwHuTRxDUQ//iYBczjHiTJcIuPyKlHQkv/LHQUYkuSi57yQT//uggfZNajQ3Vmz+Zt//+mm3Wm3Q576v////+32///5/EOgAAADVghQAAAAA//uQZAUAB1WI0PZugAAAAAoQwAAAEk3nRd2qAAAAACiDgAAAAAAABCqEEQRLCgwpBGMlJkIz8jKhGvj4k6jzRnqasNKIeoh5gI7BJaC1A1AoNBjJgbyApVS4IDlZgDU5WUAxEKDNmmALHzZp0Fkz1FMTmGFl1FMEyodIavcCAUHDWrKAIA4aa2oCgILEBupZgHvAhEBcZ6joQBxS76AgccrFlczBvKLC0QI2cBoCFvfTDAo7eoOQInqDPBtvrDEZBNYN5xwNwxQRfw8ZQ5wQVLvO8OYU+mHvFLlDh05Mdg7BT6YrRPpCBznMB2r//xKJjyyOh+cImr2/4doscwD6neZjuZR4AgAABYAAAABy1xcdQtxYBYYZdifkUDgzzXaXn98Z0oi9ILU5mBjFANmRwlVJ3/6jYDAmxaiDG3/6xjQQCCKkRb/6kg/wW+kSJ5//rLobkLSiKmqP/0ikJuDaSaSf/6JiLYLEYnW/+kXg1WRVJL/9EmQ1YZIsv/6Qzwy5qk7/+tEU0nkls3/zIUMPKNX/6yZLf+kFgAfgGyLFAUwY//uQZAUABcd5UiNPVXAAAApAAAAAE0VZQKw9ISAAACgAAAAAVQIygIElVrFkBS+Jhi+EAuu+lKAkYUEIsmEAEoMeDmCETMvfSHTGkF5RWH7kz/ESHWPAq/kcCRhqBtMdokPdM7vil7RG98A2sc7zO6ZvTdM7pmOUAZTnJW+NXxqmd41dqJ6mLTXxrPpnV8avaIf5SvL7pndPvPpndJR9Kuu8fePvuiuhorgWjp7Mf/PRjxcFCPDkW31srioCExivv9lcwKEaHsf/7ow2Fl1T/9RkXgEhYElAoCLFtMArxwivDJJ+bR1HTKJdlEoTELCIqgEwVGSQ+hIm0NbK8WXcTEI0UPoa2NbG4y2K00JEWbZavJXkYaqo9CRHS55FcZTjKEk3NKoCYUnSQ0rWxrZbFKbKIhOKPZe1cJKzZSaQrIyULHDZmV5K4xySsDRKWOruanGtjLJXFEmwaIbDLX0hIPBUQPVFVkQkDoUNfSoDgQGKPekoxeGzA4DUvnn4bxzcZrtJyipKfPNy5w+9lnXwgqsiyHNeSVpemw4bWb9psYeq//uQZBoABQt4yMVxYAIAAAkQoAAAHvYpL5m6AAgAACXDAAAAD59jblTirQe9upFsmZbpMudy7Lz1X1DYsxOOSWpfPqNX2WqktK0DMvuGwlbNj44TleLPQ+Gsfb+GOWOKJoIrWb3cIMeeON6lz2umTqMXV8Mj30yWPpjoSa9ujK8SyeJP5y5mOW1D6hvLepeveEAEDo0mgCRClOEgANv3B9a6fikgUSu/DmAMATrGx7nng5p5iimPNZsfQLYB2sDLIkzRKZOHGAaUyDcpFBSLG9MCQALgAIgQs2YunOszLSAyQYPVC2YdGGeHD2dTdJk1pAHGAWDjnkcLKFymS3RQZTInzySoBwMG0QueC3gMsCEYxUqlrcxK6k1LQQcsmyYeQPdC2YfuGPASCBkcVMQQqpVJshui1tkXQJQV0OXGAZMXSOEEBRirXbVRQW7ugq7IM7rPWSZyDlM3IuNEkxzCOJ0ny2ThNkyRai1b6ev//3dzNGzNb//4uAvHT5sURcZCFcuKLhOFs8mLAAEAt4UWAAIABAAAAAB4qbHo0tIjVkUU//uQZAwABfSFz3ZqQAAAAAngwAAAE1HjMp2qAAAAACZDgAAAD5UkTE1UgZEUExqYynN1qZvqIOREEFmBcJQkwdxiFtw0qEOkGYfRDifBui9MQg4QAHAqWtAWHoCxu1Yf4VfWLPIM2mHDFsbQEVGwyqQoQcwnfHeIkNt9YnkiaS1oizycqJrx4KOQjahZxWbcZgztj2c49nKmkId44S71j0c8eV9yDK6uPRzx5X18eDvjvQ6yKo9ZSS6l//8elePK/Lf//IInrOF/FvDoADYAGBMGb7FtErm5MXMlmPAJQVgWta7Zx2go+8xJ0UiCb8LHHdftWyLJE0QIAIsI+UbXu67dZMjmgDGCGl1H+vpF4NSDckSIkk7Vd+sxEhBQMRU8j/12UIRhzSaUdQ+rQU5kGeFxm+hb1oh6pWWmv3uvmReDl0UnvtapVaIzo1jZbf/pD6ElLqSX+rUmOQNpJFa/r+sa4e/pBlAABoAAAAA3CUgShLdGIxsY7AUABPRrgCABdDuQ5GC7DqPQCgbbJUAoRSUj+NIEig0YfyWUho1VBBBA//uQZB4ABZx5zfMakeAAAAmwAAAAF5F3P0w9GtAAACfAAAAAwLhMDmAYWMgVEG1U0FIGCBgXBXAtfMH10000EEEEEECUBYln03TTTdNBDZopopYvrTTdNa325mImNg3TTPV9q3pmY0xoO6bv3r00y+IDGid/9aaaZTGMuj9mpu9Mpio1dXrr5HERTZSmqU36A3CumzN/9Robv/Xx4v9ijkSRSNLQhAWumap82WRSBUqXStV/YcS+XVLnSS+WLDroqArFkMEsAS+eWmrUzrO0oEmE40RlMZ5+ODIkAyKAGUwZ3mVKmcamcJnMW26MRPgUw6j+LkhyHGVGYjSUUKNpuJUQoOIAyDvEyG8S5yfK6dhZc0Tx1KI/gviKL6qvvFs1+bWtaz58uUNnryq6kt5RzOCkPWlVqVX2a/EEBUdU1KrXLf40GoiiFXK///qpoiDXrOgqDR38JB0bw7SoL+ZB9o1RCkQjQ2CBYZKd/+VJxZRRZlqSkKiws0WFxUyCwsKiMy7hUVFhIaCrNQsKkTIsLivwKKigsj8XYlwt/WKi2N4d//uQRCSAAjURNIHpMZBGYiaQPSYyAAABLAAAAAAAACWAAAAApUF/Mg+0aohSIRobBAsMlO//Kk4soosy1JSFRYWaLC4qZBYWFRGZdwqKiwkNBVmoWFSJkWFxX4FFRQWR+LsS4W/rFRb/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////VEFHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAU291bmRib3kuZGUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMjAwNGh0dHA6Ly93d3cuc291bmRib3kuZGUAAAAAAAAAACU=");  
    snd.play();
}

function displayMainContent(page) {
    var pageexists=false;
    $.each(pages.page, function( index, _page ) {
        if(_page.id==page){ 
        //page found
            $('#mainContentData').empty();
            pageexists=true;
            //now build the page
            //set the title
            $('h1.mainTitle').html(_page.title).css("color", "");
            // display a different title in development mode...
            if (project && project.header.Mode == "development") {
                $('h1.mainTitle').html("Development Mode - " + _page.title).css("color", "red");
            }
            //build the objects
            buildDisplayElements(_page.objects ,'#mainContentData');
            
            if(_page.load){
				//save it to run after getting the project from the server
				pageLoadFunction = new Function (_page.load);
                
                if (_page.id == "newproject") {
                    pageLoadFunction();
                    return;
                }
                //special edit for openProjImg
                if (_page.id == "openProjImg") {
                    pageLoadFunction();
                    return;
                }
                
                if (_page.id == "projectmanagement") {
                    pageLoadFunction();
                    return;
                }

                if (_page.id == "toolssignfile" && !projectName) {
                    loadToolsWelcome();
                    return;
                }
                if (_page.id == "regulatorydomain") {
                    regulatoryDomainLoad();
                    return;
                }
                if (_page.id == "regulatorydomain24") {
                    regulatoryDomain24Load();
                    return;
                }
			}
			//add function to submit
			if(_page.submit){
				var pageSubmit= new Function(_page.submit);
				$( "#form" ).submit(function( event ) {
					return pageSubmit();
				});
			}
			pageLoadFunction();
            gLoadedPage = page;
        }
    });
    if(!pageexists){
		//page not in the system
        gLoadedPage = "";
		$('h1.mainTitle').html("Page not found");
	}
}

function buildMenu(_menu, i) {
    //first  check if should display
    var visibleFunction = new Function(_menu.visibleFunction);
    if (!visibleFunction()) {
        return "";
    }
    //if visisble start building
    var menu = "<li id=\"" + _menu.id + "\">";

    var target = _menu.page.split("?")[1].split("=")[1];
    menu += "<a href='javascript:void(0);' id=\"" + _menu.id + i +"\" onclick='displayMainContent(\"" + target + "\");'>" + _menu.name + "</a>";
   // menu += "<a href='javascript:void(0);' id=\"" + _menu.id + i + "\" onclick='test123(\"" + target +"\" ,"+_menu.id + i + " );'>" + _menu.name + "</a>";

    //check if need status
    var checkFunction = new Function(_menu.checkFunction);
    if (checkFunction()) {
        menu += "<i class=\"fa fa-exclamation-circle\"></i>";
    }

    menu += "</li>";
    return menu;
}
/*
function test123(target, id) {

    $(id).click(function () {
        $(id).css('color', 'black');
        $(this).css('color', 'green');
    });

    displayMainContent(target);
}
*/
//Menu builder
function buildChildMenu(_menu)
{
	//first  check if should display
	var visibleFunction= new Function(_menu.visibleFunction);
	if(!visibleFunction()){
		return "";
	}
	//if visisble start building
	var menu="<li id=\""+_menu.id+"\">";
	var child="";
	//start getting childs
	if(_menu.child){
		//build child menus in recursion
		for (var i=0;i<_menu.child.length;i++){
			child += buildChildMenu( _menu.child[i]);
		}
	}
	if(child.length >1) {
		menu +="<i class=\"fa-li fa fa-minus-square-o\" onclick=\"collapse('#"+_menu.id+"');\"></i>";
	}
	else{
		menu +="<i class=\"fa-li\"></i>";
	
	}
	////menu +="<a href=\""+_menu.page+"\">"+ _menu.name+ "</a>";
    var target = _menu.page.split("?")[1].split("=")[1];
    menu +="<a href='javascript:void(0);' id=\"" + _menu.id + i + "\" onclick='displayMainContent(\"" + target + "\");'>"+ _menu.name+ "</a>";
    
    
	//check if need status
	var checkFunction= new Function(_menu.checkFunction);
	if(checkFunction()){
		menu += "<i class=\"fa fa-exclamation-circle\"></i>";
	}
	//add childs
	if(child.length >1){
		menu+="<ul class=\"fa-ul\">" +child +"</ul>";
	}

	menu+="</li>";
	return menu;
};

//menu collapse
function collapse(element)
{
	$(element).find('ul').first().toggle('show');
	if($(element).find('i').first().hasClass('fa-minus-square-o')){
		$(element).find('i').first().removeClass('fa-minus-square-o');
		$(element).find('i').first().addClass('fa-plus-square-o');
	}else{
		$(element).find('i').first().removeClass('fa-plus-square-o');
		$(element).find('i').first().addClass('fa-minus-square-o');
	}
}

//build display elements
function buildDisplayElements(elements, parent){
	$.each(elements, function( index, _object ) {
		var input='';
		switch(_object.type.toLowerCase()){
			case 'div':
				input+="<div ";
				
				if(_object.id){
					input +="id='"+_object.id+"'";
				}
				if(_object.class){
					input+=	" class='"+_object.class +"' ";
				}
				if (_object.style) {
				    input += " style='" + _object.style + "' ";
				}
				if(_object.tooltip){
					if(!(_object.class && _object.class.match('fieldName'))){
						input+= "  data-tooltip='"+_object.tooltip+"'";
					}
				}
				
				input+=" >";
				
                if(_object.iclass){
                    input+= "<i ";
					input+=	" class='"+_object.iclass +"' >";
                    input+="</i>";
                }
				if(_object.title){
					input+=	_object.title;
				}
				if(_object.class && _object.class.match('fieldName') && _object.tooltip){
					input+= "&nbsp;&nbsp;<span data-tooltip='"+_object.tooltip+"'><i class='fa fa-question-circle' ></i></span>";
				}
				input+="</div>";
			break;
		case 'fieldset':
				input+="<fieldset ";
				if(_object.id){
					input +="id='"+_object.id+"'";
				}
				if(_object.id){
					input +="id='"+_object.id+"'";
				}
				if(_object.class){
					input+=	" class='"+_object.class+"' ";
				}
				if(_object.tooltip){
					input+= "  data-tooltip='"+_object.tooltip+"'";	
				}
				input+=" >";
				if(_object.title){
					input+="<legend>"+	_object.title+"</legend>";
				}
				input+="</fieldset>";
				break;
		    case 'scheckbox':
			case 'checkbox':
			case 'radio':
				$(_object.values).each(function(index,value){
					tooltip=false;
					input+="<input type='"+_object.type+"' id='"+_object.id+"_"+value.value+"' name='"+_object.id+"' value='"+value.value+"'";
					if(_object.tooltip){
						input+= "  data-tooltip='"+_object.tooltip+"'";
					}
				
					if(_object.checked && _object.checked==value.value){
						input+="  checked='checked'";
					}
					if(_object.class){
						input+=	" class='"+_object.class+"' ";
					}
					input+=">";
					if(value.text ){
						input+=value.text;
					}
					if (_object.label1)
					{
					    //<label for="checkbox">Click me</label>
					    //input+= "<label for='"+_object.id+" '><'" + value.value +" '<label>'"
					    input += '<label for="+_object.id+">' + _object.label1 + '</label>';
					}
					input+="<br>";
				});
				break;
			case 'select':
				input+="<select id='"+_object.id+"' name='"+_object.id+"'";
				if(_object.tooltip){
						input+= "  data-tooltip='"+_object.tooltip+"'";
				}
				if(_object.required){
						input+=" "+_object.required;
					}
					if(_object.multiple){
						input+=" "+_object.multiple;
					}
					if(_object.class){
						input+=	" class='"+_object.class+"' ";
					}
					input+=">";
					//for each option
					$(_object.values).each(function(index,value){
						input+="<option value='"+value.value+"' ";
						if(_object.value && _object.value==value.value){
							input+="selected";
						}
						input+=">"+value.text+"</option>";
					});
					input+="</select>";
					break;
			case 'textarea':
				input+="<textarea id='"+_object.id+"' name='"+_object.id+"'";
				if(_object.placeholder){
					input+=" placeholder='"+_object.placeholder +"'";
				}
				if(_object.tooltip){
					input+= "  data-tooltip='"+_object.tooltip+"'";
				}
				if(_object.class){
					input+=	" class='"+_object.class+"' ";
				}
				if(_object.rows){
					input+=" rows='"+_object.rows +"'";
				}
				if(_object.cols){
					input+=" cols='"+_object.cols +"'";
				}
				if(_object.required){
					input+=" "+_object.required;
				}
				input+=">";
				if(_object.value){
					input+=_object.value;
				}	
				input+="</textarea>";
				break;
			case 'justtext':
				if(_object.title){
					input+=	_object.title;
				}
				break;
			case 'br':
				input+=	"<br />";
				break;
			default:
				input+="<input type='"+_object.type+"'  id='"+_object.id+"' name='"+_object.id+"'";
				if(_object.tooltip){
					input+= "  data-tooltip='"+_object.tooltip+"'";
				}
				if(_object.value!=undefined){
					input+=" value='"+_object.value +"'";
				}
				if(_object.class){
					input+=	" class='"+_object.class+"' ";
				}
				if(_object.placeholder){
					input+=" placeholder='"+_object.placeholder +"'";
				}	
				if(_object.max!=undefined){
					input+=" max='"+_object.max +"'";
				}
				if(_object.min!=undefined){
					input+=" min='"+_object.min +"'";
				}
				if(_object.required){
					input+=" "+_object.required;
				}
				input+=">";
		}
		$(parent).append(input);
		//now check if have sons
		if(_object.objects)
		{
			buildDisplayElements(_object.objects, "#"+_object.id);
		}
		//Add events    
		if(_object.change){
			var objectChange= new Function(_object.change);
			$("#"+_object.id ).change(function( event ) {
				objectChange();
			});
		}
		if(_object.select){
			var objectSelect= new Function(_object.select);
			$("#"+_object.id ).select(function( event ) {
				objectSelect();
			});
		}
		if(_object.click){
			var objectClick= new Function(_object.click);
			$("#"+_object.id ).click(function( event ) {
				objectClick();
			});
		}
        if(_object.drop){
			var objectDrop = new Function(_object.drop);
			$("#"+_object.id ).on("drop", function( event ) {
				objectDrop();
			});
		}
		if(_object.focus){
			var objectFocus= new Function(_object.focus);
			$("#"+_object.id ).focusin(function( event ) {
				objectFocus();
			});
		}
		if(_object.keydown){
			var objectKeydown= new Function(_object.keydown);
			$("#"+_object.id ).keydown(function( event ) {
				objectKeydown();
			});
		}
		if(_object.mouseenter){
			var objectMouseEnter= new Function(_object.mouseenter);
			$("#"+_object.id ).mouseenter(function( event ) {
				objectMouseEnter();
			});
		}
		if(_object.mouseout){
			var objectMouseOut= new Function(_object.mouseout);
			$("#"+_object.id ).mouseout(function( event ) {
				objectMouseOut();
			});
		}
	});
}

//run this on load
$(function() {
	//get the page name
    var page = getParameterByName('page');
    //var is_project_loaded = localStorage.getItem('projectLoaded') == 'true';
    //var page = 'simplemode';
	//read project Name from localstorage
	projectName= localStorage.getItem('projectName');
    
	if (projectName) {

		try{
			//load project info from API
			$.post("/api/1/flash/openProject",   {'name':projectName},
				function(data,status){
				//if return success and has data
					if(status=='success' && data.ok){
						//save on localStorage project name
						project=data.ok.metadata;
						displayMainContent(page);
					}
					else if(data.err){
						alert(data.err);
					    //go to new project
						window.location.href = '/index.htm?page=newproject';
					}
					//pageLoadFunction();
			});
		}
		catch(e){}
        
	    //openProject(projectName, true);
    }

    
		//Build main menu
    //var mainmenu='';
	gMainMenuShort = '';
	gMainMenu = '';
	for (var i = 0; i < menu.length; i++) {
	    gMainMenuShort += buildMenu(menu[i], i);
	    gMainMenu += buildChildMenu(menu[i], i);
	    //mainmenu += buildChildMenu(menu[i]);
	   
	}
    
    //display main menu
	jQuery('ul.fa-ul.leftContentTree').html(gMainMenuShort);
	gb_ShortMenu = true;
	

    if (!projectName) {
        displayMainContent(page);
    }

	//start the tooltip
    try {
        $('.tooltip').tooltipster(
        {
            animation: 'fade',
            delay: 200,
            theme: 'tooltipster-default',
            touchDevices: false,
            trigger: 'click'
        });
    } catch (e) {}

});

//unload event
$( window ).unload(function() {
	//save the project to localStorage
	localStorage.setItem('project', JSON.stringify(project));
    localStorage.setItem('isconnected' , 'false');
});


//Event handler
function addEventHandler(obj, evt, handler) {
    if (obj.addEventListener) {
        // W3C method
        obj.addEventListener(evt, handler, false);
    } else if (obj.attachEvent) {
        // IE method.
        obj.attachEvent('on' + evt, handler);
    } else {
        // Old school method.
        obj['on' + evt] = handler;
    }
}

function saveImage(event)
{
    var kind = event.data.kind
    //'^5':'<a id="lastImageLink" href="/api/1/flash/last_image"></a>',
    $('#lastImageLink').html('<a id="lastImageLinkPath" href="/api/1/flash/lastImage?name=' + projectName + '&kind=' + kind + '&x=' + new Date().getTime() + '&r=' + Math.random() + '"></a>');
    document.getElementById('lastImageLinkPath').click();
}

function verifyDeviceSettings(callOnDone, noClose)
{
    if (project.header.Mode != $('#currentModeId').html())
    {
        fileBrowser.showInputDialog('You try to program image with mode mismach', 
        false, 
        {
            text: "&#8196;Ok&nbsp;&nbsp&nbsp;&nbsp", func: function () {
            createImageBase(callOnDone, noClose);
        }},
        {text: "Close"});
    }
    
}
function createImageBase(callOnDone, noClose)
{
    saveProjectAPI(function(){
        progressModalOpen("Creating image, please wait");
        $.post("/api/1/flash/createImageFromProject",   {name: projectName, key: null})
            .done(function(data) {
                if (data && data.ok) {
                    getProgressStatus(100, callOnDone, noClose);
                    
                } else {
                    $('#progStatusImg').hide();
                    $('#progStatusBtn').text("Close");
                    $('#progStatus').text("Error: " + ((data && data.err) ? data.err : "unknown"));
                }
            })
            .fail( function(xhr, textStatus, errorThrown) {
                $('#progStatusImg').hide();
                $('#progStatusBtn').text("Close");
                $('#progStatus').text("Error: disconnected");
            });
    });
}
function createImage(callOnDone, noClose)
{ 
    var isconnected = localStorage.getItem('isconnected') == 'true';
    
    if(project.systemFiles.CONFIG_TYPE_MODE.PHY_CAL_MODE == "2")//Onetime
    {
        var is_sta_high =  (parseInt(project.systemFiles.CONFIG_TYPE_MODE.STA_TX_POWER_LEVEL) < 4);
        var is_ap_high  =  (parseInt(project.systemFiles.CONFIG_TYPE_MODE.AP_TX_POWER_LEVEL)  < 4);
 
        if(is_sta_high != is_ap_high)//one is high and another is low
        {
            fileBrowser.showInputDialog('STA and AP TX power level at OneTime calibration should be the same level mode (High: 0-3, Low:4-15 )', 
            false, 
            null,
            {text: "Close"});
                return;
        }
    }

    if (project.header.UseDefaultCertStore == false) {

        if(!project.systemFiles.FILES.CS_FILE_NAME.trim() || !project.systemFiles.FILES.CSS_FILE_NAME.trim()) {

            project.header.UseDefaultCertStore = true;
            saveProjectAPI();

            fileBrowser.showInputDialog('Certificate store was not provided, using defaults',
            false,
            null,
            { text: "Close" });
            return;
        }
    }
    if (project.systemFiles.CONFIG_TYPE_HTTP_SRV && project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_SECURE) {
        if ((project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME == "") || (project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME == "")) {
            fileBrowser.showInputDialog('HTTPS key or certificate was not provided',
            false,
            null,
            { text: "Close" });
            return;
        }
    }

    if('development' == project.header.Mode)
    {
        /*no development macaddress*/
        if(!project.header.DEV_MAC_ADDR || project.header.DEV_MAC_ADDR==undefined || project.header.DEV_MAC_ADDR.length<1)
        {
            if(isconnected)
            {
                project.header.DEV_MAC_ADDR  = $('#devMacAddr').html();
                 
                createImageBase(callOnDone, noClose)
                //verifyDeviceSettings(callOnDone, noClose)
                
            }
            else
            {
                fileBrowser.showInputDialog('Original mac address was not provided. This parameter is mandatory in development mode', 
                        false, 
                        null,
                        {text: "Close"});
                return;

            }
        }
        else
        {
            createImageBase(callOnDone, noClose)
        }
        
    }
    else if ('production' == project.header.Mode)
    {
        //createImageBase(callOnDone, noClose)
        if(isconnected)
        {
            //verifyDeviceSettings(callOnDone, noClose)
            createImageBase(callOnDone, noClose)
        }
        else
        {
            createImageBase(callOnDone, noClose)
        }
    }
    
    
    
}
//files Div go button clicked
function filesDivAction()
{
	//action is delete
	if ($('#filesDivActionSelect').val()=='Delete')
	{
		var toDelete=$('.fsLine>.fs1>i.fa-check-square-o');
		for (var i=0;i<toDelete.length;i++){
			var folderarray=toDelete[i].id.replace("//FileSelect", "").split("//").reverse();
			//get the first element
			var elementToDelete=folderarray.pop();
			var folderPath=findFolder(folderarray,project.userFiles);
			//now go to all object untill find and delete;
			if( folderPath && folderPath.object){
				for(var j=0;j<folderPath.object.length;j++){
					if(folderPath.object[j].id==elementToDelete){
						//remove the items
						folderPath.object.splice(j,1);
					}
				}
			}
		}
		//erase elements on display
		$('#fileSysDiplayDiv .fsLine').remove();
		//rebuild
		//rebuild
		build_files(project.userFiles,'#fileSysDiplayDiv','','');
	}
	
}

//change the slider Development Mode
function toogleSlider()
{
	if($('#DevelopmentModeToggle .slider').hasClass('sliderRight')){
		$('#DevelopmentModeToggle .slider').removeClass('sliderRight');
		$('#DevelopmentModeToggle .slider').addClass('sliderLeft');
	}
	else{
		$('#DevelopmentModeToggle .slider').removeClass('sliderLeft');
		$('#DevelopmentModeToggle .slider').addClass('sliderRight');
	}
}

//load Filesuserfiles page function 
function loadFilesuserfiles(){
    
   $('#fileActionSelectAction').addClass('btnDisabled').off('click');
    
   var fb = new fileBrowser.Browser($("#fileSysDiv"), "offline", projectName);
   gFileBrowser = fb;
    if (projectName) {
        progressModalOpen("Loading user files, please wait...")
        $.post("/api/1/flash/loadUserFiles", {"name": projectName})
            .done(function(data) {
                if (data && data.ok) {
                    fb.load(data.ok);
                    setTimeout(progressModalClose, 500);
                } else if (data && data.err) {
                    progressModalError("Error loading users files: " + data.err)
                    
                } else {
                    progressModalError("Error loading users files")
                }
                
                fb.render();
            })
            .fail(function(data) {
                progressModalError("Error loading users files")
            });
    }
    
    if(project.header.DeviceType.indexOf('CC31') != -1) /*3100*/
    {
        $($('#fileActionSelect')[0].options[2]).hide();
    }
    else
    {
        $($('#fileActionSelect')[0].options[2]).show();
    }

    $("#checkUnchekDiv").children().eq(0).on("click", function() { fb.selectAll.call(fb) })
    $("#checkUnchekDiv").children().eq(1).on("click", function () { fb.unselectAll.call(fb) })

    fileBrowser.updateHttpCertFileName(project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME);
    fileBrowser.updateHttpKeyFileName(project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME);
    fileBrowser.updateHttpCaCertFileName(project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME);

    //fileBrowser.dialogDeleteHttpCertCallback = onDialogDeleteHttpCert
    fileBrowser.setHttpCallBack(onDialogDeleteHttpCert, onDialogDeleteHttpKey, onDialogDeleteHttpCaCert);
    
    return;
}

//load Filesuserfiles page function 
function loadFilesuserfilesOnline(){
    var fb = new fileBrowser.Browser($("#fileSysDiv"), "online", projectName);
    //fb1 = fb;
    progressModalOpen("Loading file list, please wait");
    
    $.post("/api/1/flash/getFileList")
        .done(function(data) {
            if (data.ok) {
                fb.load(data.ok);
                fb.render();
                progressModalClose();
            } else if (data.err) {
                fileBrowser.showInputDialog("Error: " + data.err, false, null, {text: "Close"});
            } else {
                fileBrowser.showInputDialog("Error: " + data.err, false, null, {text: "Close"});
            }
        })
        .fail(function() {
            fileBrowser.showInputDialog("Error", false, null, {text: "Close"});
        });
}

function changeAction()
{
    if($('#fileActionSelect').val() != undefined)
    {
        switch($('#fileActionSelect').val())
        {
            case 'MCUImg'       :
            case 'Upload'       : $('#fileActionSelectAction').html('Browse');
                                  $('#fileActionSelectAction').removeClass('btnDisabled');
                                  $('#fileActionSelectAction').off("click").on('click', filesDivPageAction);
                                    break;
            case 'Program'      : $('#fileActionSelectAction').html('Execute');
                                    break;

            case 'NewFolder'    : $('#fileActionSelectAction').html('Create');
                                    break;
            case 'Rename'       :
            case 'Remove'       : $('#fileActionSelectAction').html('&nbsp;&nbsp;Apply');
                                  $('#fileActionSelectAction').removeClass('btnDisabled');
                                  $('#fileActionSelectAction').off("click").on('click', filesDivPageAction);
                                    break;
            case 'Properties'   : $('#fileActionSelectAction').html('Display');
                                    break;
             default:    $('#fileActionSelectAction').html('Execute');
                         $('#fileActionSelectAction').addClass('btnDisabled').off('click');
        }
    }
}

//files page drop down action
function filesDivPageAction(){
    if ($('#fileActionSelect').val() == 'MCUImg') {
        if (gFileBrowser) {
            gFileBrowser.uploadMCUImage(project.header.DeviceType == 'CC3220SF' || project.header.DeviceType == 'CC3235SF' || project.header.DeviceType == 'CC3230SF');
        }
    } else if ($('#fileActionSelect').val() == 'Remove') {
        gFileBrowser.removeSelected();
    }
    
    return;
    /*
	//action is delete
	if ($('#fileActionSelect').val()=='Remove')
	{
		var toDelete=$('#fileSysDiplayPageDiv .fsLine>.fs1>i.fa-check-square-o');
		for (var i=0;i<toDelete.length;i++){
			var folderarray=toDelete[i].id.replace("//FileSelect", "").replace(/---/g, ' ').split("//").reverse();
			//get the first element
			var elementToDelete=folderarray.pop();
			if(folderarray.length===0){ //is root
				if (project.userFiles )
				{
					for(var j=0;j<project.userFiles.length;j++){
						if(project.userFiles[j].id==elementToDelete){
							if(project.userFiles[j].type=="file"){ //if it a file
								//remove the items
								project.userFiles.splice(j,1);
								var objectToSend={
									'name':projectName,
									'source':"/"+elementToDelete
								};
								//delete from API
								$.post("/api/1/flash/deleteProjectFsFile", 
									objectToSend,
									function(data,status){
										if(status=='success' && data.ok){
											//save project
											saveProjectAPI();
										}
										else if(data.err){
											alert("Error deleteing the file"+data.err);
										}
									}
								);								
							}
							else{ //is a folder delete for Project only
								//remove the items
								project.userFiles.splice(j,1);								
							}
						}
					}
				}
			}
			else{
				var folderPath=findFolder(folderarray,project.userFiles);
				//now go to all object until find and delete;
				if( folderPath && folderPath.object){
					for(var j=0;j<folderPath.object.length;j++){
						if(folderPath.object[j].id==elementToDelete){
							if(folderPath.object[j].type=="file"){ //if it a file
								//remove the items
								folderPath.object.splice(j,1);
											
								var objectToSend={
									'name':projectName,
									'source':"/"+folderarray.join("/")+"/"+elementToDelete
								};
								//delete from API
								$.post("/api/1/flash/deleteProjectFsFile", 
									objectToSend,
									function(data,status){
										if(status=='success' && data.ok){
											//save project
											saveProjectAPI();
										}
										else if(data.err){
											alert("Error delete the file"+data.err);
										}
									}
								);
							}
							else{
								//remove the items
								folderPath.object.splice(j,1);
							}
						}
					}
				}
			}
		}
		//erase elements on display
		$('#fileSysDiplayPageDiv').html("");
		//redo menus and files
		build_files(project.userFiles,'#fileSysDiplayPageDiv','','');
        
        saveProjectAPI();
	}
	else if($('#fileActionSelect').val()=='NewFolder'){
		var toAction=$('#fileSysDiplayPageDiv .fsLine>.fs1>i.fa-check-square-o');
		if(toAction.length===0){
			folderCreate(toAction);
			
			
		}
		else{
			//check if there is a file selected
			for (var i=0;i<toAction.length;i++){
				var type=$(toAction[i]).attr("data-type");
				if(type!="folder"){
						alert ("Can't create a folder in a a file")
						return;
				}				
			}
			folderCreate(toAction);
		}
	}
	else if($('#fileActionSelect').val()=='Upload'){
		var toAction=$('#fileSysDiplayPageDiv .fsLine>.fs1>i.fa-check-square-o');
		if(toAction.length>1){
			alert ("Please select only one folder!");
			return;

		}
		else{
			//check if there is a file selected
			for (var i=0;i<toAction.length;i++){
				var type=$(toAction[i]).attr("data-type");
				if(type!="folder"){
					alert ("File can not be uploaded into a file!");
					return;
				}				
			}
		}
		//if here display the  browser
        $('#fileElement').click();
	}
	else if($('#fileActionSelect').val()=='Rename'){
		var toAction=$('#fileSysDiplayPageDiv .fsLine>.fs1>i.fa-check-square-o');
		if(toAction.length>1){
			alert ("Please select only one folder!");
			return;

		}
		else{
			//check if there is a file selected
			for (var i=0;i<toAction.length;i++){
				var type=$(toAction[i]).attr("data-type");
				if(type!="folder"){
					alert ("File can not be uploaded into a file!");
					return;
				}				
			}
		}
		//rename
		var toAction=$('#fileSysDiplayPageDiv .fsLine>.fs1>i.fa-check-square-o');
		if(toAction.length>0)
		{
			foldername=$(toAction[0]).attr("id").replace('//FileSelect','').replace(/---/g, ' ');
			//rename
			folderRename(foldername);
		}
		
	}
    else if($('#fileActionSelect').val()=='MCUImg'){ //Select MCU Img
            $('#MCUImg').val("");
            $('#MCUImg').click();
            //displayFileProperties('/sys/mcuimg.bin');
    }
	else if($('#fileActionSelect').val()=='Properties'){ //files properties
		var toAction=$('#fileSysDiplayPageDiv .fsLine>.fs1>i.fa-check-square-o');
		if(toAction.length>1 ){
			alert ("Please select only one file!");
			return;

		}
		else if(toAction.length===0){
			alert ("Please select a file!");
			return;	
		}
		else{
			//check if there is a file selected
			for (var i=0;i<toAction.length;i++){
				var type=$(toAction[i]).attr("data-type");
				if(type=="folder"){
					alert ("Please select files only!");
					return;
				}				
			}
		}
		displayFileProperties($(toAction[0]).attr("id").replace("//FileSelect","").split("//").reverse().join("/"));
		
	}
    */
}

function fileSecureGUI()
{
    var disabled = !$('#fileSecure').is(':checked');
    $('#fileNoSignature'    ).prop("disabled", disabled) ;
    $('#fileStatic'         ).prop("disabled", disabled) ;
    $('#fileVendor'         ).prop("disabled", disabled) ;
    $('#filePlublicWrite'   ).prop("disabled", disabled) ;
    
}

function fileNoSignatureGUI()
{
    var disabled = $('#fileNoSignature').is(':checked');
    $('#fileCertification'    ).prop("disabled", disabled) ;
    $('#USER_SIG_FILE'        ).prop("disabled", disabled) ;
     
}

function fileVendorGUI()
{
     document.getElementById("fileToken").disabled = !document.getElementById("fileVendor").checked;
}

//upload/delete user signature  file
function uploadSignatureBase(filePath, upload)
{  
    var apiFuncPath = '';
    if($('#USER_SIG_FILE'))
    {
        var spf = document.getElementById('USER_SIG_FILE');
        
        if(upload)
        {
            $('#userSigFileInput').val(spf.value.replace(/^.*[\\\/]/, ''));
            apiFuncPath= "/api/1/flash/uploadProjectSigFile";
        }
        else//clear
        {
            $('#userSigFileInput').val('');
            apiFuncPath = "/api/1/flash/deleteProjectSigFile";
        }
    
        uploadSignature(spf.files, filePath, apiFuncPath);
    }
}

//common part of upload/delete signature file
function uploadSignature (files,filePath, apiFuncPath ){
	for (var i = 0; i < files.length; i++) {
		var file = files[i];
      
        //Send file to API
        
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
            if (xhr.status == 200 && xhr.readyState == 4) {
                var data = JSON.parse(xhr.responseText);
                if (data && data.ok) {
        
                }
                else if (data && data.err) {
                    alert(data.err);
                }
                else {
                    alert("error upload/delete file");
                }
            }
        }
            
        var formData = new FormData();
        formData.append("name", projectName);
        formData.append("source", file);
        formData.append("dest", "/"+filePath);
        
        xhr.open("post", apiFuncPath, true);
        xhr.send(formData);
	}	
}

Function.prototype.bindToEventHandler = function bindToEventHandler() {
		var handler = this;
		var boundParameters = Array.prototype.slice.call(arguments);
		//create closure
		return function (e) {
			e = e || window.event; // get window.event if e argument missing (in IE)   
			boundParameters.unshift(e);
			handler.apply(this, boundParameters);
		}
	};

function addProfileClicked ()
{
	if(!project.STAProfiles){
		project.STAProfiles=[];
	}
	//get data and add to json
	project.STAProfiles.push(
		{
			'id':makeid(),
			'SSID':$('#SSIDText').val(),
			'SecurityKey':$('#SecurityText').val(),
			'SecurityType':$('#SecurityTypeSelect').val(),
			'ProfilePriority':$('#ProfilePriorityText').val()
		}
	);
	//reload the list
	loadDeviceRoleSettingsSTA();
	//save project
	saveProjectAPI();
}
//build the profile list
function loadDeviceRoleSettingsSTA ()
{
	//erase all the profiles
	$('#ProfileListDetailsDiv div.pLwrap').remove();
	
	//is has profiles
	if(project.STAProfiles && project.STAProfiles.length>0)
	{
		//erase the no profile message
		$('#listNotes').hide();
		for (var i=0;i<project.STAProfiles.length;i++){
			var html='<div class="pLwrap"><div class="lPname">'+project.STAProfiles[i].SSID+'</div><div class="lRbin" onclick="removeProfile(\''+project.STAProfiles[i].id+'\')"><i class="fa fa-trash-o"></i></div>'
			$('#ProfileListDetailsDiv').append(html);	
		}
    }
	else{
		$('#listNotes').show();
	}
}
//erase the profile
function removeProfile(profileID){
	if(project.STAProfiles && project.STAProfiles.length>0)
	{
		for (var i=0;i<project.STAProfiles.length;i++){
			if(project.STAProfiles[i].id==profileID)
			{
				project.STAProfiles.splice(i,1);
				//reload the list
				loadDeviceRoleSettingsSTA();
				//save project
				saveProjectAPI();
				return;
			}
		}
    }
}

//make a random id
function makeid()
{
    var text = "";
    var possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for( var i=0; i < 5; i++ )
        text += possible.charAt(Math.floor(Math.random() * possible.length));

    return text;
}
// Deny Address page
function loadDeviceRoleSettingsAP (){
	//erase all the Address
	$('#denyListDetailsDiv div.pLwrap').remove();
	//is has Address
	if(project.denyAddresses && project.denyAddresses.length>0)
	{
		//erase the no Address message
		$('#listNotes').hide();
		for (var i=0;i<project.denyAddresses.length;i++){
			var html='<div class="pLwrap"><div class="lPname">'+project.denyAddresses[i].address+'</div><div class="lRbin" onclick="removeAddresses(\''+project.denyAddresses[i].id+'\')"><i class="fa fa-trash-o"></i></div>'
			$('#denyListDetailsDiv').append(html);	
		}
    }
	else{
		$('#listNotes').show();
	}
}
//erase address
function removeAddresses(profileID){
	if(project.denyAddresses && project.denyAddresses.length>0)
	{
		for (var i=0;i<project.denyAddresses.length;i++){
			if(project.denyAddresses[i].id==profileID)
			{
				project.denyAddresses.splice(i,1);
				//reload the list
				loadDeviceRoleSettingsAP();
				//save project
				saveProjectAPI();
				return;
			}
		}
    }
}
//add address
function addAddressClicked(){
	if(!project.denyAddresses){
		project.denyAddresses=[];
	}
	//get data and add to json
	project.denyAddresses.push(
		{
			'id':makeid(),
			'address':$('#addressText').val()
		}
	);
	//reload the list
	loadDeviceRoleSettingsAP();
	//save project
	saveProjectAPI();
}

function setProjImageClicked()
{
    //var sliFile = $('#IMG_FILE').prop("files")[0];
    var sliFile = $('#IMG_FILE')[0];
    
    if (sliFile.files.length) {
        sliFile = sliFile.files[0];
    } else if (g_open_project_image_file) {
        sliFile = g_open_project_image_file;
    } else {
        fileBrowser.showInputDialog('No sli file selected', 
                    false, 
                    null,
                    {text: "Close"});
            return;
    }
    
    var keyFile = $('#PROJ_IMAGE_KEY_FILE_NAME');

    var use2btldr   = $('#PROJ_IMAGE_USE_SEC_BTLDR_1' ).is(':checked');
    var use_ota     = $('#PROJ_IMAGE_USE_OTP_1'       ).is(':checked');
    programImageFromSLI(sliFile, keyFile ? keyFile.prop("files")[0] : undefined, use2btldr, use_ota);
}
//New Project page functions
//load
function newprojectLoad(){
    toggleFullOverlay($("#mainContent"), true);
    
	if(!project || !project.header){
		return;
	}
	//set name
	if(project.header.Name){
		$('#projectNameText').val(project.header.Name);
	}
	//description
	if(project.header.Description){
		$('#projectDescriptionText').val(project.header.Description);
	}
	//device type
	if(project.header.DeviceType){
		$('#deviceTypeSelect').val(project.header.DeviceType);
	}
	//device capacity
	if(project.header.StorageCapacityBytes){
		$('#capacitySelect').val(project.header.StorageCapacityBytes);
	}
	//set mode
	if(project.header.Mode){
		if(project.header.Mode=='production'){
			$('#DevelopmentModeToggle .slider').removeClass('sliderRight');
			$('#DevelopmentModeToggle .slider').addClass('sliderLeft');
		}
		else{
			$('#DevelopmentModeToggle .slider').removeClass('sliderLeft');
			$('#DevelopmentModeToggle .slider').addClass('sliderRight');
        }
	}
}
//check Device Size Clicked
function checkDeviceSizeClicked(){
	//save the project in the API
	try{
		//load project info from API
		$.post("/api/1/flash/getDeviceCapacityAndDestroyFS",
			function(data,status){
				//if return success and has data
				if(status=='success' && data.ok){
					//set capacity
					$('#capacitySelect').val(data.ok);
				}

				if(data.err){
					alert(data.err);
				}
			});
	}
	catch(e){
		alert("Error Creating File");		
	}			
}
//Create Project Clicked
function createProjectClicked(){
	//save data
	if(!project){
		project={};
	}
	if(!project.header){
		project.header={};
	}
	//set name
    var projectName = $('#projectNameText').val();

    var ret = verifyProjectName(projectName);
    if (ret.err) {
        $('#projectNameTextError').text(ret.err).css("color", "red");
        return;
    }
    
    $.post("/api/1/flash/getProjectList", {})
        .done(function(data) {
            if (data) {
                if (data.ok && data.ok.indexOf(projectName) != -1) {
                    $('#projectNameTextError').text("Error: a project with the same name already exists").css("color", "red");
                } else {
                    project.header.Name=$('#projectNameText').val();
                    
                    
                    //and save name in the local storage
                    localStorage.setItem('projectName',  project.header.Name);
                    
                    localStorage.setItem('isconnected', 'false');
                    //description
                    project.header.Description=$('#projectDescriptionText').val();
                    //device type
                    project.header.DeviceType=$('#deviceTypeSelect').val();
                    //device capacity
                    project.header.StorageCapacityBytes=$('#capacitySelect').val();
                    //set mode
                    if($('#DevelopmentModeToggle .slider').hasClass('sliderRight')){
                        project.header.Mode='development';
                    }
                    else{
                        project.header.Mode='production';
                    }

                    project.header.Is5GSupport = false;
                    if (project.header.DeviceType.indexOf('5') != -1)//5G
                    {
                        project.header.Is5GSupport = true;
                    }

                    if (project.header.DeviceType.indexOf('323') == -1 && project.header.DeviceType.indexOf('313') == -1)//gen2
                    {
                        project.header.IsGen3 = false;
                        //project.header.IsModule = false;
                    }
                    else
                    {
                        project.header.IsGen3 = true;
                        //project.header.IsModule = $('#deviceSubType_0').is(':checked');
                    }



                    //if (project.header.DeviceType.indexOf('S') != -1)//Secured
                    if (project.header.DeviceType == "CC3220R")//Secured
                    {
                        project.header.IsTheDeviceSecure = false;
                    }
                    else
                    {
                        project.header.IsTheDeviceSecure = true;
                    }
                    
                    $("select#3200Select").prop("disabled", true);

                    try {
                        //load project info from API
                        $.post("/api/1/flash/createDefaultProjectValues", {
                            'name': project.header.Name,
                            'desc': project.header.Description,
                            'mode': project.header.Mode,
                            'type': project.header.DeviceType,
                            'gen3': project.header.IsGen3,
                            'fiveg_support': project.header.Is5GSupport/*,
                            'is_module':project.header.IsModule*/
                        },
                            function (data, status) {
                                //if return success and has data
                                if (data.err) {

                                    alert(data.err);
                                }
                                else if (status == 'success' && data.ok) {
                                    project = data.ok.metadata;
                                    //project.systemFiles.CONFIG_TYPE_MODE.Is5GEnabled = project.header.Is5GSupport;
                                    saveProjectAPI(function () { window.location.href = "/index.htm?page=simplemode" });
                                    toggleFullOverlay($("#mainContent"), false);
                                }
                            });
                    }
                    catch (e) {
                        alert("Error Saving Project");
                    }
                    //save the project in the API
                    //saveProjectAPI(function() {window.location.href = "/index.htm?page=general"});

                    //toggleFullOverlay($("#mainContent"), false);
                }
            }
        })
        .fail(function(data) {
            $('#projectNameTextError').text("Error: unknown");
        });
}

//end of New Project page functions
//Device Role Page function
function LoadDeviceMode(){
	//load info from object
	if(!project || !project.general_settings){
		return;
	}
	//deviceModeSelect
	if(project.general_settings.deviceMode){
		$('#deviceModeSelect').val(project.general_settings.deviceMode);
	}
	//rolePreferenceSelect
	if(project.general_settings.rolePreference){
		$('#rolePreferenceSelect').val(project.general_settings.rolePreference);
	}
	//channelSelect
	if(project.general_settings.channel){
		$('#channelSelect').val(project.general_settings.channel);
	}
	//deviceNameText
	if(project.general_settings.deviceName){
		$(deviceNameText).val(project.general_settings.deviceName);
	}
}
//save function
function DeviceModeSaveClick(){
	//save data to object
	if(!project){
		project={};
	}
	if(!project.general_settings){
		project.general_settings={};
	}
	//deviceModeSelect
	project.general_settings.deviceMode=$('#deviceModeSelect').val();
	//rolePreferenceSelect
	project.general_settings.rolePreference=$('#rolePreferenceSelect').val();
	//channelSelect
	project.general_settings.channel=$('#channelSelect').val();
	//deviceNameText
	project.general_settings.deviceName=$('#deviceNameText').val();
	//save project
	saveProjectAPI();
}
//end of Device Role Page function
//Api Save Project
function saveProjectAPI(successFunc){
	try{
		//load project info from API
	    $.post("/api/1/flash/saveProject", { 'name': project.header.Name, 'data': JSON.stringify(project, null, 4) },
			function(data,status){
			//if return success and has data
				if(data.err){
                    
				    //alert(data.err);
				    fileBrowser.showInputDialog("Error (111) Saving Project:" + data.err,
                                            false,
                                             null,
                                            { text: "Close" });
				}
                else if(status=='success' && data.ok){
                    if(successFunc){
                        successFunc();
                    }
                }
			});
	}
	catch(e){	
	    fileBrowser.showInputDialog("Error (112) Saving Project:" + e.message ,
                                            false,
                                             null,
                                            { text: "Close" });
	}
}

/*----------------------------------------Simple mode    --------------------------*/
function displayProjectNameMenu() {
    //display project name in menu
    var menuProjectName = projectName;
    if (projectName.length > 32) {
        menuProjectName = menuProjectName.slice(0, 29) + "..."
    }
    $("span#menuProjectName").text(menuProjectName).attr("title", projectName);

}

function pressAdvanced() {

    if (gb_ShortMenu) {
        jQuery('ul.fa-ul.leftContentTree').html(gMainMenu);
        displayMainContent('generalsettings');
        displayProjectNameMenu();
        gb_ShortMenu = false;
        $('#idmenugeneral1').css('color', 'black');
        $('#idmenuadvanced3').css('color', '#4CAF50');
        $('#SaveProjectBtn').show()
        $('#BrowseBtn').show()
        $('#ToolsBtn').show()
        ShowDice();

    } else {
        displayMainContent('simplemode'); //calls to loadSimpleMode()
    }
}

function changeRoleSimple() {
    project.systemFiles.CONFIG_TYPE_MODE.START_ROLE = $('#StartRoleSelectSimple').val();
    saveProjectAPI();
}

function loadSimpleMode() {

    jQuery('ul.fa-ul.leftContentTree').html(gMainMenuShort);
    gb_ShortMenu = true;

    $('#idmenugeneral0').css('color', '#4CAF50');
    $('#idmenuadvanced1').css('color', 'black');
    $('#SaveProjectBtn').hide()
    $('#BrowseBtn').hide()
    $('#ToolsBtn').hide()

    $('#Name_SUM_Simple').html(project.header.Name);
    $('#DeviceType_SUM_Simple').html(project.header.DeviceType);


    $('#mcuFileInputSimple').prop('readonly', true);
    //$('#keyCertMCUFileInputSimple').prop('readonly', true);

    $('#StartRoleSelectSimple').val(project.systemFiles.CONFIG_TYPE_MODE.START_ROLE)
    $('#COUNTRY_CODE_SUM_SIMPLE').append(project.systemFiles.CONFIG_TYPE_MODE.COUNTRY_CODE)
    
    if (project.header.DeviceType == 'CC3135R' || project.header.DeviceType == 'CC3120R' || project.header.DeviceType == 'CC3130R') //3100
    {
        $('#fieldsetMCUImgSimple').hide();
    }
    else {
        $('#fieldsetMCUImgSimple').show();
        if (!gFileBrowser) {
            gFileBrowser = new fileBrowser.Browser($("#fileSysDiv"), "offline", projectName);
        }
        //var fb = new fileBrowser.Browser($("#fileSysDiv"), "offline", projectName);
        progressModalOpen("Verifying user files, please wait...")
        if (projectName) {
            $.post("/api/1/flash/loadUserFiles", { "name": projectName })
                .done(function (data) {
                    if (data && data.ok) {
                        gFileBrowser.load(data.ok);

                        $.each(gFileBrowser.root.folders, function (i, folder) {
                            if (folder.name == 'sys') {

                                ff = folder
                                $.each(ff.files, function (i, file) {
                                    if ((file.name == 'mcuimg.bin') && (project.header.DeviceType != 'CC3220SF' && project.header.DeviceType != 'CC3235SF' && project.header.DeviceType != 'CC3230SF')) {
                                        $('#mcuFileInputSimple').val('mcuimg.bin');
                                    }
                                    if ((file.name == 'mcuflashimg.bin') && (project.header.DeviceType == 'CC3220SF' || project.header.DeviceType == 'CC3235SF' || project.header.DeviceType == 'CC3230SF')) {
                                        $('#mcuFileInputSimple').val('mcuflashimg.bin');
                                    }
                                });

                            }
                        });

                        if (!project.systemFiles.FILES.SM_MCU_CERT_NAME || 0 === project.systemFiles.FILES.SM_MCU_CERT_NAME.length) {
                            //do nothing
                        }
                        else {
                            $.each(gFileBrowser.root.files, function (i, file) {
                                if (file.name == project.systemFiles.FILES.SM_MCU_CERT_NAME) {
                                    $('#keyCertMCUFileInputSimple').val(project.systemFiles.FILES.SM_MCU_CERT_NAME);
                                }
                            });
                        }

                        setTimeout(progressModalClose, 100);

                    } else if (data && data.err) {
                        progressModalError("Error loading users files: " + data.err)

                    } else {
                        progressModalError("Error loading users files")
                    }
                })
                .fail(function (data) {
                    progressModalError("Error loading users files")
                });
        }
    }   
}

function postMCUSimpleBase(data) {
    var fullPath = "/sys/mcuimg.bin"
    var defaultMaxSize = 256 * 1024;
    var origFileSize = data.size;
    var sfheader = 0;
    if (project.header.DeviceType == 'CC3220SF' || project.header.DeviceType == 'CC3235SF' || project.header.DeviceType == 'CC3230SF') {
        fullPath = "/sys/mcuflashimg.bin"
        defaultMaxSize = 1024 * 1024;
        sfheader = 20;
        if(project.header.IsGen3)
            sfheader = 32;
    }

    if (origFileSize + sfheader > defaultMaxSize)
    {
        progressModalError("MCU Image file exceeds the maximum size for this device")
        return;
    }

    var props = {}
    props.name = fullPath.substr(5, fullPath.length)
    props.maxFileSize = defaultMaxSize
    props.origFileSize = origFileSize
    props.fileToken = ''
    props.flagFailSafe = true
    props.flagNoSignatureTest = false
    props.flagPublicRead = false
    props.flagPublicWrite = true
    props.flagSecure = true
    props.flagStatic = false
    props.flagVendor = false
    props.signatureFileName = 'dummy-root-ca-cert-key'
    props.signatureFileNameSelect = '1'
    props.certificationFileName = 'dummy-root-ca-cert'



    var xhr = new XMLHttpRequest();
    var formData = new FormData();

    formData.append("name", project.header.Name);
    formData.append("source", data);
    formData.append("dest", fullPath);
    
    formData.append("props", JSON.stringify(props));

    var that = this;
    xhr.onreadystatechange = function () {
        progressModalOpen("Writing File");
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                smdone(fullPath.substr(5, fullPath.length));
            } else if (data && data.err) {
                smfail(data.err);
            } else {
                smfail("data error");
            }
        } else if (xhr.status > 200) {
            smfail("common error");
        }
    }

    xhr.open("POST", "/api/1/flash/uploadMCUSimpleMode", true);
    //xhr.open("POST", "/api/1/flash/uploadProjectFsFile", true);
    
    xhr.send(formData);
}

function smdone(mcu_name) {
    progressModalClose();

    $('#mcuFileInputSimple').val(mcu_name);
    project.systemFiles.FILES.SM_MCU_NAME = mcu_name;
    saveProjectAPI();

    //Update relevant certificate store
    project.header.UseDefaultCertStore = false;
    project.header.UseDefaultPlayGroundCatalog = true;
    project.systemFiles.FILES.CS_FILE_NAME = "/files/certcatalogPlayGround.lst"

    if (project.header.IsGen3) {
        project.systemFiles.FILES.CSS_FILE_NAME = "/files/certcatalogPlayGround.lst.signed_gen3.bin"
    }
    else {
        project.systemFiles.FILES.CSS_FILE_NAME = "/files/certcatalogPlayGround.lst.signed_gen2.bin"
    }
}

function smfail(err) {
    
    progressModalError("Upload Simple Mode error: " + err);
}
/*
function postMCUSimpleBaseOld(data) {
    if (!gFileBrowser) {
        gFileBrowser = new fileBrowser.Browser($("#fileSysDiv"), "offline", projectName);
    }
    gFileBrowser.dialogClosedCallback = onDialogClose

    gFileBrowser.dialogSavedCallback = onDialogSave

    gFileBrowser.uploadMCUImageSimple(project.header.DeviceType == 'CC3220SF' || project.header.DeviceType == 'CC3235SF',data , onDialogClose, onDialogSave);
    $('#MCU_FILE_SIMPLE').val('');

    //Update relevant certificate store
    project.header.UseDefaultCertStore = false;
    project.header.UseDefultPlayGroundCatalog = true;
    project.systemFiles.FILES.CS_FILE_NAME = "/files/certcatalogPlayGround.lst"

    if (project.header.IsGen3) {
        project.systemFiles.FILES.CSS_FILE_NAME = "/files/certcatalogPlayGround.lst.signed_gen3.bin"
    }
    else {
        project.systemFiles.FILES.CSS_FILE_NAME = "/files/certcatalogPlayGround.lst.signed_gen2.bin"
    }
}
*/

function onDialogClose() {
    console.log('Callback executed');
    //$('#MCU_FILE_SIMPLE').val('');
}

function onDialogSave() {
    //$('#MCU_FILE_SIMPLE').val('');
    $('#mcuFileInputSimple').val(mcuImgName);
    project.systemFiles.FILES.SM_MCU_NAME = mcuImgName;
    saveProjectAPI();
}

//TBD - remove cert?     
function clearUserFileSimpleMode() {

    var fullPath = "";

    if (project.header.DeviceType == 'CC3220SF' || project.header.DeviceType == 'CC3235SF' || project.header.DeviceType == 'CC3230SF') {
        fullPath = "/sys/mcuflashimg.bin"
    }
    else {
        fullPath = "/sys/mcuimg.bin"
    }

    var obj = $('#mcuFileInputSimple');
    var hide_obj = $('#MCU_FILE_SIMPLE');

    var name = obj.val()
    if (!name || name.length == 0) {
        $('#MCU_FILE_SIMPLE').val('');
        $('#mcuFileInputSimple').val('');
    }
    else {
        //$.post("/api/1/flash/deleteProjectFsFile", { name: projectName, source: fullPath })
        $.post("/api/1/flash/deleteMCUSimpleMode", { name: projectName, source: fullPath })
           .done(function (data) {
               if (data && data.ok) {
                   //done(data.ok);
                   obj.val('');
                   //hide_obj.val('');
                   setTimeout(progressModalClose, 50);
               } else if (data && data.err) {
                   progressModalError("Deleting File: " + fullPath + " err: " + data.err,
                       function () {
                           //fail(data.err);
                       });
                   return;
               } else {
                   progressModalError("Deleting File: " + fullPath, fail);
                   return;
               }

               //done();

           })
           .fail(function () {
               // fail();
               obj.val('');
               //hide_obj.val('');
               progressModalError("Deleting File: " + fullPath, fail);
               return;
           });
    }
}

/*---------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
function changeSecBtldr() {
    project.header.UseSecBtldr = $('#USE_SEC_BTLDR_1').is(':checked')

    if (project.header.UseSecBtldr)
    {
        $('#USE_DEF_CERT_STORE_1').prop('checked', false);
        $('#fieldsetOTPOut').show();
    }
    else{
        $('#USE_DEF_CERT_STORE_1').prop('disabled', project.header.UseSecBtldr);
        $('#fieldsetOTPOut').hide();
    }
    

    saveProjectAPI();
    changeCertStoreDefault();
}


function changeCertStoreDefault()
{
    if (project.header.UseSecBtldr) {
        $('#USE_DEF_CERT_STORE_1').prop('checked', false);
        project.header.UseDefaultCertStore = false;
    }
    else
    {
        project.header.UseDefaultCertStore = $('#USE_DEF_CERT_STORE_1').is(':checked')
    }
   

    saveProjectAPI();
    loadCertStore();
}

function loadCertStore() 
{
    $('#USE_SEC_BTLDR_1').prop('checked', project.header.UseSecBtldr);

    $('#USE_OTP_1').prop('checked', project.header.UseOtp);

    $('#otpFileInput').val(project.systemFiles.FILES.OTP_NAME.replace(/^.*[\\\/]/, ''));

    if (project.header.UseSecBtldr) {
        $('#USE_DEF_CERT_STORE_1').prop('checked', false);
        $('#fieldsetOTPOut').show();
    }
    else
    {
        $('#USE_DEF_CERT_STORE_1').prop('checked', project.header.UseDefaultCertStore);
        $('#fieldsetOTPOut').hide();
    }
   
    $('#keyCertStoreFileInput'      ).prop('readonly', true);
    
    if (project.header.UseDefaultCertStore && !project.header.UseSecBtldr)
    {
        $('#keyCertStoreFileInput').val("/files/certstore.lst");
        $('div#browseCertStoreFile').addClass("btnDisabled");

        var cert_sig_name = "/files/certstore.lst.signed";
        if (project.header.IsGen3) {

            cert_sig_name = "/files/certstore.lst.g3.signed";
        }
                
        $('#keyCertStoreSignFileInput').val(cert_sig_name);
        $('div#browseCertStoreSignFile').addClass("btnDisabled");
        
    }
    else
    {
        if (project.header.UseDefultPlayGroundCatalog) {

        } else {
            $('#keyCertStoreFileInput').val(project.systemFiles.FILES.CS_FILE_NAME.replace(/^.*[\\\/]/, ''));
            $('#keyCertStoreSignFileInput').val(project.systemFiles.FILES.CSS_FILE_NAME.replace(/^.*[\\\/]/, ''));
        }

        $('div#browseCertStoreFile').removeClass("btnDisabled");
        $('div#browseCertStoreSignFile').removeClass("btnDisabled");
    }
    
    $('#CERT_STORE_FILE'          ).prop("disabled", project.header.UseDefaultCertStore);
    $('#CERT_STORE_SIGNATURE_FILE').prop("disabled", project.header.UseDefaultCertStore);

    $('#USE_DEF_CERT_STORE_1').unbind('change');
    $('#USE_DEF_CERT_STORE_1').bind('change', changeCertStoreDefault);

    $('#USE_SEC_BTLDR_1').unbind('change');
    $('#USE_SEC_BTLDR_1').bind('change', changeSecBtldr);

    $('#USE_OTP_1').unbind('change');
    $('#USE_OTP_1').bind('change', changeOtpFile);
    
    UpdateOTPFileGui(project.header.UseOtp)
}

function changeOtpFile(enable)
{
    project.header.UseOtp = $('#USE_OTP_1').is(':checked')

    saveProjectAPI();
    UpdateOTPFileGui(project.header.UseOtp);
}

function UpdateOTPFileGui(enable)
{
    if (enable) {
        $('#fieldsetOTPInn').show();
    }
    else {
        $('#fieldsetOTPInn').hide();
    }
}

function clearOtpFile() {

    if ($('#otpFileInput').val().trim() != '') {

        clearProjectFile("/api/1/flash/deleteOtpFile", $('#otpFileInput'), 13);
        $('#OTP_FILE').val('');
        //$('#otpFileInput').val('');
        //project.systemFiles.FILES.OTP_NAME = "";

        //saveProjectAPI();
    }
}

function dropFileExt(eName)
{
    var eName = $(eName);
    
    event.preventDefault();
	event.stopPropagation();
                
    if(event.dataTransfer.files.length == 1) {
        
        switch(eName.selector)
        {
            case '#toolSignFileSource'  :   g_tools_source_file = event.dataTransfer.files[0];
                                            $('#toolSignFileSource').val(event.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                            break;
                                          
            case '#toolSignFileKey'     :   g_tools_key_file = event.dataTransfer.files[0];
                                            $('#toolSignFileKey').val(event.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                            break;
                                          
            case '#toolsImgKeyFileInput':   g_tools_img_dec_key_file = event.dataTransfer.files[0];
                                            $('#toolsImgKeyFileInput').val(event.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                            $('#TOOLS_PROJ_IMAGE_KEY_FILE_NAME').val("");
                                            break;
            case '#toolsImgKeyFileInput':   g_tools_csr_file = event.dataTransfer.files[0];
                                            $('#toolsCSRFileInput').val(event.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                            $('#TOOLS_SIGNED_CSR_FILE_NAME').val("");
                                            break;
                                          
            case '#keyImgFileInput'     :   g_open_project_image_file = event.dataTransfer.files[0];
                                            $('#keyImgFileInput').val(event.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                            $('#IMG_FILE').val("");
                                            enableSetProjImage(true);
                                            break;
            case '#keyImgKeyFileInput'  :   g_open_project_image_key_file = event.dataTransfer.files[0];
                                            $('#keyImgKeyFileInput').val(event.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                            $('#PROJ_IMAGE_KEY_FILE_NAME').val("");
                                            enableSetProjImage(true);
                                            break;
                                                
        }
    }
}

function dropFile(eName)
{
    var eName = $(eName);
        eName.unbind('dragover');
		eName.unbind('dragenter');
		eName.unbind('drop');
		// Tells the browser that we *can* drop on this target
		eName.bind('dragover',   function(e) {
				e.preventDefault();
				e.stopPropagation();
		});
        
		eName.bind( 'dragenter',   function(e) {
                e.preventDefault();
				e.stopPropagation();   
		});
    
            
        eName.bind('drop',   function(e) {
                e.preventDefault();
				e.stopPropagation();
                if(e.originalEvent.dataTransfer.files.length == 1) {
						
                        var json_num;
                        var path ;
                        switch(eName.selector)
                        {
                            /*case '#SDK_DICEFolderInput':            json_num = "30";
                                                                    path = "/api/1/flash/uploadSDKFile";
                                                                    $('#SDK_DICEFolderInput').val(e.originalEvent.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                                                    $('#SDK_DICE_FILE').val("");
                                                                    break;*/

                            case '#keyFileInput':                   json_num = "3";
                                                                    path = "/api/1/flash/uploadProjectKSFile";
                                                                    break;

                            case '#spFileInputSimple':              json_num = "10";
                                                                    path = "/api/1/flash/uploadProjectSPFile";
                                                                    break;
                            case '#spFileInput':                    json_num = "0";
                                                                    path = "/api/1/flash/uploadProjectSPFile";
                                                                    break;
                            case '#keyCertStoreFileInputSimple':    json_num = "11";
                                                                    path = "/api/1/flash/uploadProjectCertStoreFile";
                                                                    break;
                            case '#keyCertStoreFileInput':          json_num = "1";
                                                                    path = "/api/1/flash/uploadProjectCertStoreFile";
                                                                    break;
                            case '#otpFileInput':                   json_num = "13";
                                                                    path = "/api/1/flash/uploadOtpFile";
                                                                    break;
                            case '#keyCertStoreSignFileInputSimple':json_num = "12";
                                                                    path = "/api/1/flash/uploadProjectCertStoreSigFile";
                                                                    break;
                            case '#keyCertStoreSignFileInput':      json_num = "2";
                                                                    path = "/api/1/flash/uploadProjectCertStoreSigFile";
                                                                    break;
                                                                    
                            case '#toolSignFileSource':             json_num = "4";
                                                                    g_tools_source_file = e.originalEvent.dataTransfer.files[0];
                                                                    $('#toolSignFileSource').val(e.originalEvent.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                                                    break;
                                                                    
                            case '#toolSignFileKey':                json_num = "5";
                                                                    g_tools_key_file = e.originalEvent.dataTransfer.files[0];
                                                                    $('#toolSignFileKey').val(e.originalEvent.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                                                    break;
                                                                    
                            case '#toolsImgKeyFileInput':           json_num = "6";
                                                                    g_tools_img_dec_key_file = e.originalEvent.dataTransfer.files[0];
                                                                    $('#toolsImgKeyFileInput').val(e.originalEvent.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                                                    $('#TOOLS_PROJ_IMAGE_KEY_FILE_NAME').val("");
                                                                    break;

                            case '#toolsCSRFileInput':              json_num = "16";
                                                                    g_tools_csr_file = e.originalEvent.dataTransfer.files[0];
                                                                    $('#toolsCSRFileInput').val(e.originalEvent.dataTransfer.files[0].name.replace(/^.*[\\\/]/, ''));
                                                                    $('#TOOLS_SIGNED_CSR_FILE_NAME').val("");
                                                                    break;

                            case '#keyHttpPrimFileInput':           json_num = "7";
                                                                    //path = "/api/1/flash/uploadHttpKeyFile";
                                                                    path = "";
                                                                    break;

                            case '#certHttpPrimFileInput':          json_num = "8";

                                                                    //path = "/api/1/flash/uploadHttpCertFile";
                                                                    path = "";
                                                                    break;
                            case '#cacertHttpPrimFileInput':        json_num = "9";
                                                                    path = "/api/1/flash/uploadHttpCaCertFile";
                                                                    break;

                            case '#mcuFileInputSimple': postMCUSimpleBase(e.originalEvent.dataTransfer.files[0]);
                                                                    break;
                        }
                        
                            if(((json_num == "1") || (json_num == "2")) && (project.header.UseDefaultCertStore))
                            {
                                alert("Operation not allowed!");
                            }
                            else if (json_num == "4" || json_num == "5" || json_num == "6" || json_num == "16")
                            {
                                //updateFiles (json_num   , path);
                            }
                            
                            else if (json_num == "7" || json_num == "8" || json_num == "9") {

                                updateFiles(json_num, e.originalEvent.dataTransfer.files[0].name);


                                if (/\.pem$/i.test(spf.files[0].name.toLowerCase()) === false) {
                                    postHttpFileBase(spf.files[0], funcname, json_num);

                                } else {
                                    //fileBrowser.showInputDialog("The file is detected as PEM certificate. Specification requires a certificate where the last line ends the UNIX end line format; any other symbol may cause unexpected behavior. All line ends will be replaced to the Unix line format.",
                                    fileBrowser.showInputDialog(fileBrowser.getPemMessage(),
                                                       false,
                                                       {
                                                           text: "Allow", func: function () {

                                                               postHttpFileBase(spf.files[0], funcname, json_num);
                                                           }
                                                       },
                                                       {
                                                           text: "&#8196;Deny", func: function () {

                                                           }
                                                       });

                                }//pem

                                //postHttpFileBase(e.originalEvent.dataTransfer.files[0], path, json_num);


                                loadRootUserFiles();
                                
                            }
                            else
                            {
                                updateFiles (json_num                             , e.originalEvent.dataTransfer.files[0].name);
                                postFileBase(e.originalEvent.dataTransfer.files[0], path                                      );
                            }
				}   

		});
}

function postFileBase(source, path, done, fail)
{
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                done(data)
            }
            else if (data && data.err) {
                //alert(data.err);
                fail(data)
            }
            else {
                //alert("error upload file");
                fail()
            }
        }
    }
    
    var formData = new FormData();
    formData.append("name", projectName);
    formData.append("source", source);
    
    xhr.open("POST", path, true);
    xhr.send(formData);
}

function postFile(id, funcname, json_num)
{
    var spf = document.getElementById(id);

    if (spf) {
        var name = spf.files[0].name;

        progressModalOpen("Updating file, please wait...")

        postFileBase(spf.files[0], "/api/1/flash/".concat(funcname),
            function (data) {
                updateFiles(json_num, name);
                setTimeout(progressModalClose, 1000);
            },
            function (data) {
                if (data.err) {
                    progressModalError("Error updating file: " + data.err);
                } else {
                    progressModalError("Error updating file");
                }
            }
        );
    }
    
}

function updateFiles(json_num, name)
{
    switch(json_num)
    {
        case "10":
        case "0":   project.systemFiles.FILES.SP_FILE_NAME = name;
                    project.header.ServicePackFileLocation = name;
                    $('#spFileInput').val(name.replace(/^.*[\\\/]/, ''));
                    $('#spFileInputSimple').val(name.replace(/^.*[\\\/]/, ''));
                    break;
        case "11":
        case "1":   project.systemFiles.FILES.CS_FILE_NAME = name;
                    $('#keyCertStoreFileInput').val(name.replace(/^.*[\\\/]/, ''));
                    //$('#keyCertStoreFileInputSimple').val(name.replace(/^.*[\\\/]/, ''));
                    project.header.UseDefaultPlayGroundCatalog = false;
                    break;
        case "12":
        case "2":   project.systemFiles.FILES.CSS_FILE_NAME = name;
                    $('#keyCertStoreSignFileInput').val(name.replace(/^.*[\\\/]/, ''));
                    project.header.UseDefaultPlayGroundCatalog = false;
                    //$('#keyCertStoreSignFileInputSimple').val(name.replace(/^.*[\\\/]/, ''));
                    break;
                
        case "3":   project.header.KeyFileLocation = name;
                    $('#keyFileInput').val(name.replace(/^.*[\\\/]/, ''));
                    break;
                
        case "4":   
                    source_file_name = document.getElementById(name).files[0].name
                    $('#toolSignFileSource').val(source_file_name.replace(/^.*[\\\/]/, ''));
                    break;
        case "5":   key_file_name = document.getElementById(name).files[0].name
                    $('#toolSignFileKey').val(key_file_name.replace(/^.*[\\\/]/, ''));
                    break;
                    
        case "6":   tools_key_file_name = document.getElementById(name).files[0].name
                    $('#toolsImgKeyFileInput').val(tools_key_file_name.replace(/^.*[\\\/]/, ''));
                    break;
        case "16":  tools_csr_file_name = document.getElementById(name).files[0].name
                    $('#toolsCSRFileInput').val(tools_csr_file_name.replace(/^.*[\\\/]/, ''));
                    break;

        case "7":   project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME = name;
                     $('#keyHttpPrimFileInput').val(name.replace(/^.*[\\\/]/, ''));
                    fileBrowser.updateHttpKeyFileName(name.replace(/^.*[\\\/]/, ''));
                    break;

        case "8":   project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME = name;
                    $('#certHttpPrimFileInput').val(name.replace(/^.*[\\\/]/, ''));
                    fileBrowser.updateHttpCertFileName(name.replace(/^.*[\\\/]/, ''));
                    break;

        case "9":   project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME = name;
                    $('#cacertHttpPrimFileInput').val(name.replace(/^.*[\\\/]/, ''));
                    fileBrowser.updateHttpCaCertFileName(name.replace(/^.*[\\\/]/, ''));
                    break;
        case "13":  project.systemFiles.FILES.OTP_NAME = name;
                    $('#otpFileInput').val(name.replace(/^.*[\\\/]/, ''));
                    break;
        case "23": 
                    $('#prImgotpFileInput').val(name.replace(/^.*[\\\/]/, ''));
                    break;
        /*case "30": $('#SDK_DICEFolderInput').val(name.replace(/^.*[\\\/]/, ''));
                   $('#SDK_DICE_FILE').val("");
                   break;*/

    }

    if (projectName) {
        saveProjectAPI();
    }
}

function signFile()
{
    signFileBase(false)
}

function signFile64()
{
    signFileBase(true)
}

function signFileBase(is64bit)
{
    var sourceFile = $('#TOOLS_SIGN_SOURCE_FILE');
    var pKeyFile   = $('#TOOLS_SIGN_KEY_FILE');
    
    var xhr = new XMLHttpRequest();
    var formData = new FormData();
    
    var sourceFileData;
    var pKeyFileData;
    
    if (sourceFile[0].files[0] != undefined){
        sourceFileData = sourceFile[0].files[0]
    }else{
        sourceFileData = g_tools_source_file
    }
    
    if (pKeyFile[0].files[0] != undefined){
        pKeyFileData = pKeyFile[0].files[0]
    }else{
        pKeyFileData = g_tools_key_file
    }
    
    if((sourceFileData!= undefined)&&(pKeyFileData!= undefined)){
        formData.append("source", sourceFileData);
        formData.append("key"   , pKeyFileData);
        
        progressModalOpen("Creating, please wait");
       
        xhr.onreadystatechange = function() 
        {
            progressModalOpen("Signing File");
            if (xhr.status == 200 && xhr.readyState == 4) {
                var data = JSON.parse(xhr.responseText);
                if (data && data.ok) {

                    progressModalClose(false);
                    //$('#toolSignBinaryLinkPath').remove()
                    if(is64bit){
                        $('#toolSignBinaryLink').html('<a id="toolSignBinaryLinkPath64" href="/api/1/flash/getSignature64?source_name=' + sourceFileData.name + '&x=' + new Date().getTime() + '&r=' + Math.random() + '"></a>');
                        //$('#toolSignBinaryLinkPath64').click();
                        document.getElementById('toolSignBinaryLinkPath64').click();
                    }
                    else{
                        $('#toolSignBinaryLink').html('<a id="toolSignBinaryLinkPath" href="/api/1/flash/getSignature?source_name=' + sourceFileData.name + '&x=' + new Date().getTime() + '&r=' + Math.random() + '"></a>');
                        //$('#toolSignBinaryLinkPath').click();
                        document.getElementById('toolSignBinaryLinkPath').click();
                    }
                    
                }
                else if (data && data.err) {
                    progressModalError("Signing, err:" + data.err);
                    fail(data.err);
                }
                else {
                    progressModalError("Signing");
                    fail(data.err);
                }
            } else if (xhr.status > 200) {
                progressModalError("Signing");
                fail();
            }
        }
        
        if(is64bit){
            xhr.open("POST", "/api/1/flash/signFile64", true);
        }else{
            xhr.open("POST", "/api/1/flash/signFile", true);
        }
        xhr.send(formData);
    }
    else
    {
        fileBrowser.showInputDialog('Wrong file input', 
                    false, 
                    null,
                    {text: "Close"});
            return;
    }
    

    
}
/*
function getSDKPath() {
    
    var input = document.getElementById('SDK_DICE_FILE');

    var reader = new FileReader();
    reader.onload = function () {
        //var text = reader.result;
        //console.log(reader.result.substring(0, 200));
        //alert(text)
        project.header.SDK_PATH = reader.result;
    };
    reader.readAsText(input.files[0]);
}*/
/*-------------------------------Mac Address---TBD--------------------------------------------------*/
function addMacDefault(e)
{
    var use_default = document.getElementById('USE_DEF_MAC_1').checked;
    project.systemFiles.CONFIG_TYPE_MAC.USE_DEFAULT = use_default ? 1 : 0;
    
        //save project
    saveProjectAPI();
    
    document.getElementById('MA1').disabled = use_default;
    
    if(use_default)
    {
        if(localStorage.getItem('isconnected') == 'true')//connected
        {
            document.getElementById('MA1').value = document.getElementById('devMacAddr').innerHTML;
        }
        else
        {
            document.getElementById('MA1').value = "";
        }
    }
    else
    {
        document.getElementById('MA1').value = project.systemFiles.CONFIG_TYPE_MAC.MAC_ADDR;
    }
}

function addMacAddr(e)
{
    if(verifyMacAddr(e, project.systemFiles.CONFIG_TYPE_MAC.MAC_ADDR))
    {
        project.systemFiles.CONFIG_TYPE_MAC.MAC_ADDR = document.getElementById('USE_DEF_MAC_1').checked ? "" : document.getElementById('MA1').value;
            //save project
        saveProjectAPI();
    }
}

function loadDevelopeMacAddress()
{
    var isconnected = localStorage.getItem('isconnected') == 'true';
    if(isconnected)
    {
        $('#DevMA1').val($('#devMacAddr').html());
        changeDevelopeMacAddress();
    }
    else
    {
        $('#DevMA1').val(project.header.DEV_MAC_ADDR);
    }
    
}

function changeDevelopeMacAddress(save)
{
    //project.systemFiles.CONFIG_TYPE_MAC.DEV_MAC_ADDR = $('#DevMA1').val();
    project.header.DEV_MAC_ADDR = $('#DevMA1').val();
    if (save !== false) {
        saveProjectAPI();
    }
}

function loadMacAddress(){
    var isconnected = localStorage.getItem('isconnected') == 'true';
    
    if(project.systemFiles.CONFIG_TYPE_MAC.USE_DEFAULT == 1)
    {
        if($('#USE_DEF_MAC_1').val() != undefined)
        {
            $("#USE_DEF_MAC_1").attr('checked', true);
            if(isconnected)
            {
                $('#MA1').val($('#devMacAddr').html());
            }
            else
            {
                $('#MA1').val('');
            }
            $('#MA1').prop("disabled", true);
        }        
    }
    else
    {
        $("#USE_DEF_MAC_1").attr('checked', false);
        $('#MA1').val(project.systemFiles.CONFIG_TYPE_MAC.MAC_ADDR);
        $('#MA1').prop("disabled", false);
    }
    
    /*--------------------------------*/
    if($('#USE_DEF_MAC_1').val() != undefined)
    {
        document.getElementById('USE_DEF_MAC_1').addEventListener('change', addMacDefault, false);
        document.getElementById('MA1'          ).addEventListener('change', addMacAddr, false);
    }

}
/*-------------------------------Calibrations----------------------------------------------------*/
function add2_4gCal_Mode() {
    if (project.header.IsGen3 == true)
    {
        if (project.systemFiles.CONFIG_TYPE_MODE.Is5GEnabled == false) {
            $('<option>').val('2').text('Onetime').appendTo('#PHY_CAL_MODE');
        }
        else {
            $("#PHY_CAL_MODE option[value='2']").remove();
        }
    }
    else
    {
        if (($("#PHY_CAL_MODE option[value='2']").length == 0))
                $('<option>').val('2').text('Onetime').appendTo('#PHY_CAL_MODE');
    }
    

}

/*------------------coexistence-----------------------*/
function displayCoex() {

    if ($('#COEX_MODE').val() == 0) {//disable


        $('#coexInputPadDiv').hide();
        $('#coexOutputPadDiv').hide();

    }
    else if ($('#COEX_MODE').val() == 2) //dual 
    {
        $('#coexInputPadDiv').show();
        $('#coexOutputPadDiv').hide();
    }
    else {
        $('#coexInputPadDiv').show();
        $('#coexOutputPadDiv').show();
    }
}

function changeCoex() {

    project.systemFiles.CONFIG_TYPE_MODE.CO_EX_MODE = $('#COEX_MODE').val();
    displayCoex();
}

function loadCoex() {

    if (project.header.IsGen3 == true)
    {
        var input_arr;
        var output_arr;
        if ((project.header.DeviceType == 'CC3135R') ||  (project.header.DeviceType == 'CC3130R')){
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#INPUT_PAD');//input default 3135
            /*$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#INPUT_PAD');*/
            /*$('<option>').val(03).text('   PAD03(03)(58)').appendTo('#INPUT_PAD');*/
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#INPUT_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#INPUT_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#INPUT_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#INPUT_PAD');
            
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#INPUT_PAD');
            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#INPUT_PAD');//output default 3135
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#INPUT_PAD');
            /*$('<option>').val(23).text('   PAD23(23)(16)').appendTo('#INPUT_PAD');
            $('<option>').val(24).text('   PAD24(24)(17)').appendTo('#INPUT_PAD');*/
            /*$('<option>').val(26).text('   PAD26(26)(29)').appendTo('#INPUT_PAD');
            //$('<option>').val(27).text('   PAD27(27)(30)').appendTo('#INPUT_PAD');Removed gen3*/
            /*$('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#INPUT_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#INPUT_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#INPUT_PAD');


            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#OUTPUT_PAD');//output default 3135
            /*$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#OUTPUT_PAD');*/
            /*$('<option>').val(03).text('   PAD03(03)(58)').appendTo('#OUTPUT_PAD');*/
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#OUTPUT_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#OUTPUT_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#OUTPUT_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#OUTPUT_PAD');
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#OUTPUT_PAD');
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#OUTPUT_PAD');
            /*$('<option>').val(23).text('   PAD23(23)(16)').appendTo('#OUTPUT_PAD');
            $('<option>').val(24).text('   PAD24(24)(17)').appendTo('#OUTPUT_PAD');*/
            $('<option>').val(25).text('   PAD25(25)(21)').appendTo('#OUTPUT_PAD');
            /*$('<option>').val(26).text('   PAD26(26)(29)').appendTo('#OUTPUT_PAD');
            $('<option>').val(27).text('   PAD27(27)(30)').appendTo('#OUTPUT_PAD');removed gen3*/
            /*$('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#OUTPUT_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#OUTPUT_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#OUTPUT_PAD');

            input_arr = [   [09, '   PAD09(09)(64)'],//input default 3135
                            /*[00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],*/
                            [04, '   PAD04(04)(59)'],
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],   
                            [10, '   PAD10(10)(01)'],
                            [12, '   PAD12(12)(03)'],//output default 3135
                            [13, '   PAD13(13)(04)'],
                            /*[23, '   PAD23(23)(16)'],
                            [24, '   PAD24(24)(17)'],*/
                            //[26, '   PAD26(26)(29)'],
                            //[27, '   PAD27(27)(30)'],
                            [28, '   PAD40(28)(18)'],
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash; check val!!!*/
                            [32, 'SH_PAD01(32)(52)']/*!!! - check val*/

                            
                        ]
            output_arr = [  [12, '   PAD12(12)(03)'],//output default 3135
                           /* [00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],*/
                            [04, '   PAD04(04)(59)'],
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [10, '   PAD10(10)(01)'],
                            [13, '   PAD13(13)(04)'],
                            /*[23, '   PAD23(23)(16)'],
                            [24, '   PAD24(24)(17)'],*/
                            [25, '   PAD25(25)(21)'],/*Output only*/
                            //[26, '   PAD26(26)(29)'],
                            //[27, '   PAD27(27)(30)'],
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash; check val!!!*/
                            [32, 'SH_PAD01(32)(52)'],/*!!! - check val*/
                            [28, '   PAD40(28)(18)']//input default 3135
                         ]

        }
        else {
            $('<option>').val(00).text('   PAD00(00)(50)').appendTo('#INPUT_PAD');//default input 3235
            $('<option>').val(03).text('   PAD03(03)(58)').appendTo('#INPUT_PAD');//default output3235
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#INPUT_PAD');
            //$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#INPUT_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#INPUT_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#INPUT_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#INPUT_PAD');
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#INPUT_PAD');
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#INPUT_PAD');
            $('<option>').val(11).text('   PAD11(11)(02)').appendTo('#INPUT_PAD');
            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#INPUT_PAD');
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#INPUT_PAD');
            $('<option>').val(14).text('   PAD14(14)(05)').appendTo('#INPUT_PAD');
            $('<option>').val(15).text('   PAD15(15)(06)').appendTo('#INPUT_PAD');
            $('<option>').val(16).text('   PAD16(16)(07)').appendTo('#INPUT_PAD');
            $('<option>').val(17).text('   PAD17(17)(08)').appendTo('#INPUT_PAD');
            $('<option>').val(22).text('   PAD22(22)(15)').appendTo('#INPUT_PAD');
            /*$('<option>').val(26).text('   PAD26(26)(29)').appendTo('#INPUT_PAD');
            $('<option>').val(27).text('   PAD27(27)(30)').appendTo('#INPUT_PAD');removed gen3*/
            /*if (project.header.DeviceType == 'CC3235S')
                $('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#INPUT_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#INPUT_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#INPUT_PAD');

            $('<option>').val(03).text('   PAD03(03)(58)').appendTo('#OUTPUT_PAD');//default output 3235
            $('<option>').val(00).text('   PAD00(00)(50)').appendTo('#OUTPUT_PAD');//default input 3235
            //$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#OUTPUT_PAD');
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#OUTPUT_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#OUTPUT_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#OUTPUT_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#OUTPUT_PAD');
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#OUTPUT_PAD');
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#OUTPUT_PAD');
            $('<option>').val(11).text('   PAD11(11)(02)').appendTo('#OUTPUT_PAD');
            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#OUTPUT_PAD');
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#OUTPUT_PAD');
            $('<option>').val(14).text('   PAD14(14)(05)').appendTo('#OUTPUT_PAD');
            $('<option>').val(15).text('   PAD15(15)(06)').appendTo('#OUTPUT_PAD');
            $('<option>').val(16).text('   PAD16(16)(07)').appendTo('#OUTPUT_PAD');
            $('<option>').val(17).text('   PAD17(17)(08)').appendTo('#OUTPUT_PAD');
            $('<option>').val(22).text('   PAD22(22)(15)').appendTo('#OUTPUT_PAD');
            $('<option>').val(25).text('   PAD25(25)(21)').appendTo('#OUTPUT_PAD');
            /*$('<option>').val(26).text('   PAD26(26)(29)').appendTo('#OUTPUT_PAD');
            $('<option>').val(27).text('   PAD27(27)(30)').appendTo('#OUTPUT_PAD');removed gen3*/
            /*if (project.header.DeviceType == 'CC3235S')
                $('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#OUTPUT_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#OUTPUT_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#OUTPUT_PAD');

            input_arr = [   [00, '   PAD00(00)(50)'],/*input default*/
                            [03, '   PAD03(03)(58)'],/*output default*/
                            [04, '   PAD04(04)(59)'],
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [10, '   PAD10(10)(01)'],
                            [11, '   PAD11(11)(02)'],
                            [12, '   PAD12(12)(03)'],
                            [13, '   PAD13(13)(04)'],
                            [14, '   PAD14(14)(05)'],
                            [15, '   PAD15(15)(06)'],
                            [16, '   PAD16(16)(07)'],
                            [17, '   PAD17(17)(08)'],
                            [22, '   PAD22(22)(15)'],
                            /*[26, '   PAD26(26)(29)'],
                            [27, '   PAD27(27)(30)'],*/
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash; check val!!!*/
                            [32, 'SH_PAD01(32)(52)'],/*!!! - check val*/
                            [28, '   PAD40(28)(18)']
                         ]
            output_arr = [  [03, '   PAD03(03)(58)'],/*output default*/
                            [00, '   PAD00(00)(50)'],/*input default*/
                            [04, '   PAD04(04)(59)'],
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [10, '   PAD10(10)(01)'],
                            [11, '   PAD11(11)(02)'],
                            [12, '   PAD12(12)(03)'],
                            [13, '   PAD13(13)(04)'],
                            [14, '   PAD14(14)(05)'],
                            [15, '   PAD15(15)(06)'],
                            [16, '   PAD16(16)(07)'],
                            [17, '   PAD17(17)(08)'],
                            [22, '   PAD22(22)(15)'],
                            [25, '   PAD25(25)(21)'],/*Output only*/
                            /*[26, '   PAD26(26)(29)'],
                            [27, '   PAD27(27)(30)'],*/
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash; check val!!!*/
                            [32, 'SH_PAD01(32)(52)'],/*!!! - check val*/
                            [28, '   PAD40(28)(18)']
                         ]
        }

       

        $('#COEX_MODE').val(project.systemFiles.CONFIG_TYPE_MODE.CO_EX_MODE);
        $('#OUTPUT_PAD').val(project.systemFiles.CONFIG_TYPE_MODE.CO_EX_OUTPUT);
        $('#INPUT_PAD').val(project.systemFiles.CONFIG_TYPE_MODE.CO_EX_INPUT);

        //remove input/output onload
        var coex_input = Number(project.systemFiles.CONFIG_TYPE_MODE.CO_EX_INPUT);
        var coex_output = Number(project.systemFiles.CONFIG_TYPE_MODE.CO_EX_OUTPUT);
        updateCoexListManual(coex_output, input_arr[coex_output], '#INPUT_PAD', input_arr);
        updateCoexListManual(coex_input, output_arr[coex_input], '#OUTPUT_PAD', output_arr);

        $(document).on('change', '#INPUT_PAD' , function (e) { updateCoexListAdvanced(e, '#OUTPUT_PAD', output_arr); project.systemFiles.CONFIG_TYPE_MODE.CO_EX_INPUT = $('#INPUT_PAD').val(); saveProjectAPI(); });
        $(document).on('change', '#OUTPUT_PAD', function (e) { updateCoexListAdvanced(e, '#INPUT_PAD', input_arr); project.systemFiles.CONFIG_TYPE_MODE.CO_EX_OUTPUT = $('#OUTPUT_PAD').val(); saveProjectAPI(); });



        displayCoex();
    }
    else
    {
        $('#fieldsetDeviceAdvancedRadioSettings').hide();
        $('#fieldsetDeviceCoExist').hide();
    }
}

function updateCoexListManual(value, text, select_str, select_arr) {

    var found = false;

    $(select_str).find('option').not(':selected').remove(); //remove all except selected
    var sel_val = $(select_str).val()

    for (var i = 0, len = select_arr.length; i < len; i++) {

        if ((value == select_arr[i][0]) || (sel_val == select_arr[i][0]))//element is valid 
            found = true
        else //add
        {
            if (project.header.DeviceType == 'CC3235SF') {

                if ((select_arr[i][0] == 31) || (select_arr[i][0] == '31'))
                    found = true;
                else
                    $('<option>').val(select_arr[i][0]).text(select_arr[i][1]).appendTo(select_str);
            }

            else
                $('<option>').val(select_arr[i][0]).text(select_arr[i][1]).appendTo(select_str);
        }

    }

}

function updateCoexListAdvanced(e, select_str, select_arr) {

    var value = e.currentTarget.options[e.currentTarget.options["selectedIndex"]].value;
    var text = e.currentTarget.options[e.currentTarget.options["selectedIndex"]].text;
    
   
    updateCoexListManual(value, text, select_str, select_arr);
}

/*------------------ant selection---------------------*/
function displayAntSel() {
    if ($('#ANT_SEL_MODE').val() == 0) {//disable


        $('#antselAnt1PadDiv').hide();
        $('#antselAnt2PadDiv').hide();

    }/*
    else if ($('#ANT_SEL_MODE').val() == 1) //ANT1 
    {
        $('#antselAnt1PadDiv').show();
        $('#antselAnt2PadDiv').show();
    }
    else if ($('#ANT_SEL_MODE').val() == 2) //ANT2
    {
        $('#antselAnt1PadDiv').show();
        $('#antselAnt2PadDiv').show();
    }*/
    else {
        $('#antselAnt1PadDiv').show();
        $('#antselAnt2PadDiv').show();
    }
}

function changeAntSel() {

    project.systemFiles.CONFIG_TYPE_MODE.ANTSEL_MODE = $('#ANT_SEL_MODE').val();
    displayAntSel();
}

function loadAntSel() {
    if (project.header.IsGen3 == true) {
        var input_arr;
        var output_arr;
        if (project.header.DeviceType == 'CC3135R') {
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#ANT1_PAD');/*Ant1 default*/
            /*$('<option>').val(26).text('   PAD26(26)(29)').appendTo('#ANT1_PAD');*/
            /*$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#ANT1_PAD');*/
            /*$('<option>').val(03).text('   PAD03(03)(58)').appendTo('#ANT1_PAD');*/
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#ANT1_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#ANT1_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#ANT1_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#ANT1_PAD');
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#ANT1_PAD');
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#ANT1_PAD');/*Ant2 default*/
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#ANT1_PAD');
            /*$('<option>').val(23).text('   PAD23(23)(16)').appendTo('#ANT1_PAD');
            $('<option>').val(24).text('   PAD24(24)(17)').appendTo('#ANT1_PAD');*/
            /*$('<option>').val(27).text('   PAD27(27)(30)').appendTo('#ANT1_PAD');default Ant2*/
            /*$('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#ANT1_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#ANT1_PAD');
            

            /*$('<option>').val(27).text('   PAD27(27)(30)').appendTo('#ANT2_PAD');*/
            /*$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#ANT2_PAD');*/
            /*$('<option>').val(03).text('   PAD03(03)(58)').appendTo('#ANT2_PAD');*/
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#ANT2_PAD');/*Ant2 default*/
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#ANT2_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#ANT2_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#ANT2_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#ANT2_PAD');
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#ANT2_PAD');
            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#ANT2_PAD');
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#ANT2_PAD');
            /*$('<option>').val(23).text('   PAD23(23)(16)').appendTo('#ANT2_PAD');
            $('<option>').val(24).text('   PAD24(24)(17)').appendTo('#ANT2_PAD');*/
            $('<option>').val(25).text('   PAD25(25)(21)').appendTo('#ANT2_PAD');
            /*$('<option>').val(26).text('   PAD26(26)(29)').appendTo('#ANT2_PAD'); default Ant1*/
            /*$('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#ANT2_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#ANT2_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#ANT2_PAD');/*Ant1 default*/

            input_arr = [   [28, '   PAD40(28)(18)'],/*default ANT1*/
                            [04, '   PAD04(04)(59)'],
                            /*[26, '   PAD26(26)(29)'],
                            [00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],*/
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [10, '   PAD10(10)(01)'],/*default ANT2*/
                            [13, '   PAD13(13)(04)'],
                            /*[23, '   PAD23(23)(16)'],
                            [24, '   PAD24(24)(17)'],
                            [27, '   PAD27(27)(30)'],
                            [31, 'SH_PAD04(31)(45)'],/*Non - Flash; check val!!!*/
                            [32, 'SH_PAD01(32)(52)']
                            
            ]
            output_arr = [[10, '   PAD10(10)(01)'],/*Default ANT2*/
                            /*[27, '   PAD27(27)(30)'],*/
                            [12, '   PAD12(12)(03)'],
                            /*[00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],*/
                            [04, '   PAD04(04)(59)'],
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [13, '   PAD13(13)(04)'],
                            /*[23, '   PAD23(23)(16)'],
                            [24, '   PAD24(24)(17)'],*/
                            [25, '   PAD25(25)(21)'],/*Output only*/
                            /*[26, '   PAD26(26)(29)'],*/
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash*/
                            [32, 'SH_PAD01(32)(52)'],
                            [28, '   PAD40(28)(18)'],/*default ANT1*/
            ]

        }
        else if (project.header.DeviceType == 'CC3130R') {
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#ANT1_PAD');/*Ant1 default*/
            $('<option>').val(26).text('   PAD26(26)(29)').appendTo('#ANT1_PAD');
            /*$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#ANT1_PAD');*/
            /*$('<option>').val(03).text('   PAD03(03)(58)').appendTo('#ANT1_PAD');*/
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#ANT1_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#ANT1_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#ANT1_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#ANT1_PAD');
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#ANT1_PAD');
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#ANT1_PAD');/*Ant2 default*/
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#ANT1_PAD');
            /*$('<option>').val(23).text('   PAD23(23)(16)').appendTo('#ANT1_PAD');
            $('<option>').val(24).text('   PAD24(24)(17)').appendTo('#ANT1_PAD');*/
            $('<option>').val(27).text('   PAD27(27)(30)').appendTo('#ANT1_PAD');
            /*$('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#ANT1_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#ANT1_PAD');


            
            /*$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#ANT2_PAD');*/
            /*$('<option>').val(03).text('   PAD03(03)(58)').appendTo('#ANT2_PAD');*/
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#ANT2_PAD');/*Ant2 default*/
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#ANT2_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#ANT2_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#ANT2_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#ANT2_PAD');
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#ANT2_PAD');
            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#ANT2_PAD');
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#ANT2_PAD');
            /*$('<option>').val(23).text('   PAD23(23)(16)').appendTo('#ANT2_PAD');
            $('<option>').val(24).text('   PAD24(24)(17)').appendTo('#ANT2_PAD');*/
            $('<option>').val(25).text('   PAD25(25)(21)').appendTo('#ANT2_PAD');
            $('<option>').val(26).text('   PAD26(26)(29)').appendTo('#ANT2_PAD');
            $('<option>').val(27).text('   PAD27(27)(30)').appendTo('#ANT2_PAD');
            /*$('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#ANT2_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#ANT2_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#ANT2_PAD');/*Ant1 default*/

            input_arr = [[28, '   PAD40(28)(18)'],/*default ANT1*/
                            [04, '   PAD04(04)(59)'],
                            [26, '   PAD26(26)(29)'],
                            /*[00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],*/
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [10, '   PAD10(10)(01)'],/*default ANT2*/
                            [13, '   PAD13(13)(04)'],
                            /*[23, '   PAD23(23)(16)'],
                            [24, '   PAD24(24)(17)'],*/
                            [27, '   PAD27(27)(30)'],
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash; check val!!!*/
                            [32, 'SH_PAD01(32)(52)']

            ]
            output_arr = [[10, '   PAD10(10)(01)'],/*Default ANT2*/
                            [12, '   PAD12(12)(03)'],
                            /*[00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],*/
                            [04, '   PAD04(04)(59)'],
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [13, '   PAD13(13)(04)'],
                            /*[23, '   PAD23(23)(16)'],
                            [24, '   PAD24(24)(17)'],*/
                            [25, '   PAD25(25)(21)'],/*Output only*/
                            [26, '   PAD26(26)(29)'],
                            [27, '   PAD27(27)(30)'],
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash*/
                            [32, 'SH_PAD01(32)(52)'],
                            [28, '   PAD40(28)(18)'],/*default ANT1*/
            ]
        }
        else if ((project.header.DeviceType == 'CC3230S') || (project.header.DeviceType == 'CC3230SF')) {
            //$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#ANT1_PAD');
            $('<option>').val(03).text('   PAD03(03)(58)').appendTo('#ANT1_PAD');
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#ANT1_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#ANT1_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#ANT1_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#ANT1_PAD');/*default Ant1*/
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#ANT1_PAD');/*default Ant2*/
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#ANT1_PAD');
            $('<option>').val(11).text('   PAD11(11)(02)').appendTo('#ANT1_PAD');
            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#ANT1_PAD');
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#ANT1_PAD');
            $('<option>').val(14).text('   PAD14(14)(05)').appendTo('#ANT1_PAD');
            $('<option>').val(15).text('   PAD15(15)(06)').appendTo('#ANT1_PAD');
            $('<option>').val(16).text('   PAD16(16)(07)').appendTo('#ANT1_PAD');
            $('<option>').val(17).text('   PAD17(17)(08)').appendTo('#ANT1_PAD');
            $('<option>').val(22).text('   PAD22(22)(15)').appendTo('#ANT1_PAD');
            $('<option>').val(26).text('   PAD26(26)(29)').appendTo('#ANT1_PAD');
            $('<option>').val(27).text('   PAD27(27)(30)').appendTo('#ANT1_PAD');
            /*if (project.header.DeviceType == 'CC3235S')
                $('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#ANT1_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#ANT1_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#ANT1_PAD');

            
            //$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#ANT2_PAD');
            $('<option>').val(03).text('   PAD03(03)(58)').appendTo('#ANT2_PAD');
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#ANT2_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#ANT2_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#ANT2_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#ANT2_PAD');/*default Ant1*/
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#ANT2_PAD');/*default Ant2*/
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#ANT2_PAD');
            $('<option>').val(11).text('   PAD11(11)(02)').appendTo('#ANT2_PAD');
            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#ANT2_PAD');
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#ANT2_PAD');
            $('<option>').val(14).text('   PAD14(14)(05)').appendTo('#ANT2_PAD');
            $('<option>').val(15).text('   PAD15(15)(06)').appendTo('#ANT2_PAD');
            $('<option>').val(16).text('   PAD16(16)(07)').appendTo('#ANT2_PAD');
            $('<option>').val(17).text('   PAD17(17)(08)').appendTo('#ANT2_PAD');
            $('<option>').val(22).text('   PAD22(22)(15)').appendTo('#ANT2_PAD');
            $('<option>').val(25).text('   PAD25(25)(21)').appendTo('#ANT2_PAD');
            $('<option>').val(26).text('   PAD26(26)(29)').appendTo('#ANT2_PAD');
            $('<option>').val(27).text('   PAD27(27)(30)').appendTo('#ANT2_PAD'); 
            /*if (project.header.DeviceType == 'CC3235S')
                $('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#ANT2_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#ANT2_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#ANT2_PAD');

            input_arr = [[04, '   PAD04(04)(59)'],
                            //[00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [10, '   PAD10(10)(01)'],
                            [11, '   PAD11(11)(02)'],
                            [12, '   PAD12(12)(03)'],
                            [13, '   PAD13(13)(04)'],
                            [14, '   PAD14(14)(05)'],
                            [15, '   PAD15(15)(06)'],
                            [16, '   PAD16(16)(07)'],
                            [17, '   PAD17(17)(08)'],
                            [22, '   PAD22(22)(15)'],
                            [26, '   PAD26(26)(29)'],
                            [27, '   PAD27(27)(30)'], 
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash!*/
                            [32, 'SH_PAD01(32)(52)']/*,
                            [28, '   PAD40(28)(18)']*/
            ]
            output_arr = [[05, '   PAD05(05)(60)'],
                            //[00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],
                            [04, '   PAD04(04)(59)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [10, '   PAD10(10)(01)'],
                            [11, '   PAD11(11)(02)'],
                            [12, '   PAD12(12)(03)'],
                            [13, '   PAD13(13)(04)'],
                            [14, '   PAD14(14)(05)'],
                            [15, '   PAD15(15)(06)'],
                            [16, '   PAD16(16)(07)'],
                            [17, '   PAD17(17)(08)'],
                            [22, '   PAD22(22)(15)'],
                            [25, '   PAD25(25)(21)'],/*Output only*/
                            [26, '   PAD26(26)(29)'],
                            [27, '   PAD27(27)(30)'],
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash*/
                            [32, 'SH_PAD01(32)(52)'],
                            [28, '   PAD40(28)(18)']
            ]
        }
        else  { //CC3235S/CC3235SF 5GHz support
            /*$('<option>').val(26).text('   PAD26(26)(29)').appendTo('#ANT1_PAD');removed from gen3*/
            //$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#ANT1_PAD');
            $('<option>').val(03).text('   PAD03(03)(58)').appendTo('#ANT1_PAD');
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#ANT1_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#ANT1_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#ANT1_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#ANT1_PAD');/*default Ant1*/
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#ANT1_PAD');/*default Ant2*/
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#ANT1_PAD');
            $('<option>').val(11).text('   PAD11(11)(02)').appendTo('#ANT1_PAD');
            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#ANT1_PAD');
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#ANT1_PAD');
            $('<option>').val(14).text('   PAD14(14)(05)').appendTo('#ANT1_PAD');
            $('<option>').val(15).text('   PAD15(15)(06)').appendTo('#ANT1_PAD');
            $('<option>').val(16).text('   PAD16(16)(07)').appendTo('#ANT1_PAD');
            $('<option>').val(17).text('   PAD17(17)(08)').appendTo('#ANT1_PAD');
            $('<option>').val(22).text('   PAD22(22)(15)').appendTo('#ANT1_PAD');
            /*$('<option>').val(27).text('   PAD27(27)(30)').appendTo('#ANT1_PAD'); removed from gen3*/
            /*if (project.header.DeviceType == 'CC3235S')
                $('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#ANT1_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#ANT1_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#ANT1_PAD');

            //$('<option>').val(27).text('   PAD27(27)(30)').appendTo('#ANT2_PAD'); removed from gen3
            //$('<option>').val(00).text('SH_PAD02(00)(50)').appendTo('#ANT2_PAD');
            $('<option>').val(03).text('   PAD03(03)(58)').appendTo('#ANT2_PAD');
            $('<option>').val(04).text('   PAD04(04)(59)').appendTo('#ANT2_PAD');
            $('<option>').val(05).text('   PAD05(05)(60)').appendTo('#ANT2_PAD');
            $('<option>').val(06).text('   PAD06(06)(61)').appendTo('#ANT2_PAD');
            $('<option>').val(08).text('   PAD08(08)(63)').appendTo('#ANT2_PAD');/*default Ant1*/
            $('<option>').val(09).text('   PAD09(09)(64)').appendTo('#ANT2_PAD');/*default Ant2*/
            $('<option>').val(10).text('   PAD10(10)(01)').appendTo('#ANT2_PAD');
            $('<option>').val(11).text('   PAD11(11)(02)').appendTo('#ANT2_PAD');
            $('<option>').val(12).text('   PAD12(12)(03)').appendTo('#ANT2_PAD');
            $('<option>').val(13).text('   PAD13(13)(04)').appendTo('#ANT2_PAD');
            $('<option>').val(14).text('   PAD14(14)(05)').appendTo('#ANT2_PAD');
            $('<option>').val(15).text('   PAD15(15)(06)').appendTo('#ANT2_PAD');
            $('<option>').val(16).text('   PAD16(16)(07)').appendTo('#ANT2_PAD');
            $('<option>').val(17).text('   PAD17(17)(08)').appendTo('#ANT2_PAD');
            $('<option>').val(22).text('   PAD22(22)(15)').appendTo('#ANT2_PAD');
            $('<option>').val(25).text('   PAD25(25)(21)').appendTo('#ANT2_PAD');
            /*$('<option>').val(26).text('   PAD26(26)(29)').appendTo('#OUTPUT_PAD'); Removed from gen3*/

            /*if (project.header.DeviceType == 'CC3235S')
                $('<option>').val(31).text('SH_PAD04(31)(45)').appendTo('#ANT2_PAD');/*Non Flash versions only*/
            $('<option>').val(32).text('SH_PAD01(32)(52)').appendTo('#ANT2_PAD');
            $('<option>').val(28).text('   PAD40(28)(18)').appendTo('#ANT2_PAD');

            input_arr = [[04, '   PAD04(04)(59)'],
                            //[26, '   PAD26(26)(29)'],
                            //[00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],
                            [05, '   PAD05(05)(60)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [10, '   PAD10(10)(01)'],
                            [11, '   PAD11(11)(02)'],
                            [12, '   PAD12(12)(03)'],
                            [13, '   PAD13(13)(04)'],
                            [14, '   PAD14(14)(05)'],
                            [15, '   PAD15(15)(06)'],
                            [16, '   PAD16(16)(07)'],
                            [17, '   PAD17(17)(08)'],
                            [22, '   PAD22(22)(15)'],
                            /*[27, '   PAD27(27)(30)'], removed from gen3*/
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash!*/
                            [32, 'SH_PAD01(32)(52)']/*,
                            [28, '   PAD40(28)(18)']*/
            ]
            output_arr = [[05, '   PAD05(05)(60)'],
                            //[27, '   PAD27(27)(30)'],
                            //[00, 'SH_PAD02(00)(50)'],
                            [03, '   PAD03(03)(58)'],
                            [04, '   PAD04(04)(59)'],
                            [06, '   PAD06(06)(61)'],
                            [08, '   PAD08(08)(63)'],
                            [09, '   PAD09(09)(64)'],
                            [10, '   PAD10(10)(01)'],
                            [11, '   PAD11(11)(02)'],
                            [12, '   PAD12(12)(03)'],
                            [13, '   PAD13(13)(04)'],
                            [14, '   PAD14(14)(05)'],
                            [15, '   PAD15(15)(06)'],
                            [16, '   PAD16(16)(07)'],
                            [17, '   PAD17(17)(08)'],
                            [22, '   PAD22(22)(15)'],
                            [25, '   PAD25(25)(21)'],/*Output only*/
                            //[26, '   PAD26(26)(29)'],
                            /*[31, 'SH_PAD04(31)(45)'],/*Non - Flash*/
                            [32, 'SH_PAD01(32)(52)'],
                            [28, '   PAD40(28)(18)']
            ]
        }

        $('#ANT_SEL_MODE').val(project.systemFiles.CONFIG_TYPE_MODE.ANTSEL_MODE);
        $('#ANT2_PAD').val(project.systemFiles.CONFIG_TYPE_MODE.ANTSEL_ANT2);
        $('#ANT1_PAD').val(project.systemFiles.CONFIG_TYPE_MODE.ANTSEL_ANT1);

        //remove input/output onload
        var ant1 = Number(project.systemFiles.CONFIG_TYPE_MODE.ANTSEL_ANT1);
        var ant2 = Number(project.systemFiles.CONFIG_TYPE_MODE.ANTSEL_ANT2);
        updateCoexListManual(ant2, input_arr[ant2], '#ANT1_PAD', input_arr);
        updateCoexListManual(ant1, output_arr[ant1], '#ANT2_PAD', output_arr);

        $(document).on('change', '#ANT1_PAD', function (e) { updateCoexListAdvanced(e, '#ANT2_PAD', output_arr); project.systemFiles.CONFIG_TYPE_MODE.ANTSEL_ANT1 = $('#ANT1_PAD').val(); saveProjectAPI(); });
        $(document).on('change', '#ANT2_PAD', function (e) { updateCoexListAdvanced(e, '#ANT1_PAD', input_arr); project.systemFiles.CONFIG_TYPE_MODE.ANTSEL_ANT2 = $('#ANT2_PAD').val(); saveProjectAPI(); });



        displayAntSel();
    }
    else {
                $('#fieldsetDeviceAntSelect').hide();
         }
}
/*----------------------------------------------------*/
function loadRadioSettings() {
    $('#STA_TX_PL'                  ).val(project.systemFiles.CONFIG_TYPE_MODE.STA_TX_POWER_LEVEL   );
    $('#AP_TX_PL'                   ).val(project.systemFiles.CONFIG_TYPE_MODE.AP_TX_POWER_LEVEL    );
    $('#PHY_CAL_MODE'               ).val(project.systemFiles.CONFIG_TYPE_MODE.PHY_CAL_MODE         );
    $('#PHY_DEVICE_HIGH_TX_POWER').val(project.systemFiles.CONFIG_TYPE_MODE.PHY_STA_HIGH_TX_POWER);
    $('#Is5GEnabledSelect').val(String(project.systemFiles.CONFIG_TYPE_MODE.Is5GEnabled));

    updateRF5GGUI();
    
    if ((project.header.IsGen3 == true)&&(project.header.Is5GSupport == true)){
    //if (project.header.Is5GSupport == true) {
        $('#fieldsetDeviceRadioSettingsF').show();
        $('#fieldsetDeviceAdvancedRadio24Settings').show();
        //$('#Is5GEnabled').show();
    }
    else {
        //$('#Is5GEnabled').hide();
        $('#fieldsetDeviceRadioSettingsF').hide();
        $('#fieldsetDeviceAdvancedRadio24Settings').hide();
    }

    $(document).on('change', '#Is5GEnabledSelect', updateRF5GGUI);


    $(document).on('change', '#STA_TX_PL'               , function (e) { if (verifyNumLimits(e, 0, 15, project.systemFiles.CONFIG_TYPE_MODE.STA_TX_POWER_LEVEL)) { changeStationTxPowerLevel(); } });
    $(document).on('change', '#AP_TX_PL'                , function (e) { if (verifyNumLimits(e, 0, 15, project.systemFiles.CONFIG_TYPE_MODE.AP_TX_POWER_LEVEL)) { changeAPTxPowerLevel(); } });
    $(document).on('change', '#PHY_CAL_MODE', changePHYCalMode);
    $(document).on('change', '#PHY_DEVICE_HIGH_TX_POWER', function (e) { project.systemFiles.CONFIG_TYPE_MODE.PHY_STA_HIGH_TX_POWER = $('#PHY_DEVICE_HIGH_TX_POWER').val();
                                                                        project.systemFiles.CONFIG_TYPE_MODE.PHY_AP_HIGH_TX_POWER = $('#PHY_DEVICE_HIGH_TX_POWER').val();
                                                                            saveProjectAPI();});
}
function changePHYCalMode() {

    if ("2" == $('#PHY_CAL_MODE').val()) {

        fileBrowser.showInputDialog('For One-Time calibration mode, the calibration is made once on the first power/hibernate cycle after the device programming; &nbsp;&#8196;One-time should be used only when the system power source is not able to handle the peak calibration current. For example Any OTA that contain RF changes in this mode will result in failure.',
            false,
            {
                text: "&#8196;Ok&nbsp;&nbsp&nbsp;&nbsp", func: function () {
                    project.systemFiles.CONFIG_TYPE_MODE.PHY_CAL_MODE = $('#PHY_CAL_MODE').val();
                    saveProjectAPI();
                    return true;
                }
            },
            {
                text: "Close", func: function () {
                    $('#PHY_CAL_MODE').val(project.systemFiles.CONFIG_TYPE_MODE.PHY_CAL_MODE);
                    saveProjectAPI();
                    return true;
                }
            });
    }
    else {
        project.systemFiles.CONFIG_TYPE_MODE.PHY_CAL_MODE = $('#PHY_CAL_MODE').val();
        saveProjectAPI();
    }
}

function updateRF5GGUI() {
    project.systemFiles.CONFIG_TYPE_MODE.Is5GEnabled = $('#Is5GEnabledSelect').val() === "false" ? false : true;

    if (project.systemFiles.CONFIG_TYPE_MODE.Is5GEnabled == false) {
        $("#PHY_5G_CAL_MODE").prop("disabled", true);
        $('#btnRegDomainOpen').addClass('btnDisabled').off('click');
        if (35 < parseInt(project.systemFiles.CONFIG_TYPE_AP.CHANNEL)) /*channel is A - band, need to set it back to B,G - band*/ {
            fileBrowser.showInputDialog(' AP Channel ' + project.systemFiles.CONFIG_TYPE_AP.CHANNEL + ' is illegal for 2.4 GHz band. It will be change to 1',
            false,
            {
                text: "&#8196;Ok&nbsp;&nbsp&nbsp;&nbsp", func: function () {
                    project.systemFiles.CONFIG_TYPE_AP.CHANNEL = "1"; /*default for B,G band*/
                    return true;
                }
            },
            {
                text: "Close", func: function () {
                    project.systemFiles.CONFIG_TYPE_MODE.Is5GEnabled = true; /* cancel from disable operation*/
                    $('#Is5GEnabledSelect').val(String(project.systemFiles.CONFIG_TYPE_MODE.Is5GEnabled));
                    return true;
                }
            });
        }
    }
    else {
        $("#PHY_5G_CAL_MODE").prop("disabled", false);
        $('#btnRegDomainOpen').removeClass('btnDisabled');
        $('#btnRegDomainOpen').off("click").on('click', btnRegDomainOpen);
    }
    saveProjectAPI();
}

function changeStationTxPowerLevel()
{
    project.systemFiles.CONFIG_TYPE_MODE.STA_TX_POWER_LEVEL = document.getElementById('STA_TX_PL'     ).value;
        
    //save project
    saveProjectAPI();
}

function changeAPTxPowerLevel()
{
    project.systemFiles.CONFIG_TYPE_MODE.AP_TX_POWER_LEVEL = document.getElementById('AP_TX_PL'     ).value;
        
    //save project
    saveProjectAPI();
}

/*---------------------------------Device Role Settings --> General Settings--------------------------------------------------------------*/
function changeAutoProvisioningSelect()
{
    document.getElementById('AutoProvExternalConfSelect'    ).disabled = document.getElementById('conPolAutoProvisioningSelect'  ).value == '0';
}

function changeRoleSelect() {
    project.systemFiles.CONFIG_TYPE_MODE.START_ROLE = $('#StartRoleSelect').val();
    saveProjectAPI();
}
function loadDeviceRoleGeneralSettings(){
    //document.getElementById('StartRoleSelect').value = project.systemFiles.CONFIG_TYPE_MODE.START_ROLE;
    toggleFullOverlay($("#mainContent").css("width", "500px"), false);
    $('#StartRoleSelect').val(project.systemFiles.CONFIG_TYPE_MODE.START_ROLE);

    var val;/*backward compatibility vs gen2 old*/
    switch (project.systemFiles.CONFIG_TYPE_MODE.COUNTRY_CODE) {
        case "0":
        case 0: val = 'EU';
            break;
        case "1":
        case 1: val = 'US';
            break;
        case "2":
        case 2: val = 'JP';
            break;
        default: val = project.systemFiles.CONFIG_TYPE_MODE.COUNTRY_CODE;
            break;
    }
    //document.getElementById('CountryCodeSelect').value = project.systemFiles.CONFIG_TYPE_MODE.COUNTRY_CODE;
    $('#CountryCodeSelect').val(val);
    $('#CountryCodeInput').val(val);
    
    document.getElementById('deviceNameText'            ).value = project.systemFiles.CONFIG_TYPE_DEVICE_NAME.DEVICE_URN;
    
    document.getElementById('conPolAutoStartSelect'         ).value = project.systemFiles.CONFIG_TYPE_STA_CONFIG.AUTOSTART        ;
    document.getElementById('conPolFastConnectSelect'       ).value = project.systemFiles.CONFIG_TYPE_STA_CONFIG.USEFASTCONNECT   ;
    document.getElementById('conAnyWFDirectSelect'          ).value = project.systemFiles.CONFIG_TYPE_STA_CONFIG.CONNECTTOANYP2P  ;
    document.getElementById('conPolAutoProvisioningSelect'  ).value = project.systemFiles.CONFIG_TYPE_STA_CONFIG.AUTOPROVISIONING ;
    
    changeAutoProvisioningSelect();
    document.getElementById('AutoProvExternalConfSelect').value = project.systemFiles.CONFIG_TYPE_MODE.AUTO_PROV_EXTERNAL_CONFIRMATION;
    
    //document.getElementById('StartRoleSelect'           ).addEventListener("change", function(e) { project.systemFiles.CONFIG_TYPE_MODE.START_ROLE   = document.getElementById('StartRoleSelect'  ).value; saveProjectAPI();});


    
    document.getElementById('CountryCodeSelect').addEventListener("change", function (e) {

        document.getElementById('CountryCodeSelect').nextElementSibling.value = document.getElementById('CountryCodeSelect').value
        project.systemFiles.CONFIG_TYPE_MODE.COUNTRY_CODE = document.getElementById('CountryCodeSelect').value;
        saveProjectAPI();
    });

    document.getElementById('CountryCodeInput').addEventListener("change", function (e) {
        if (verifyStringLength(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.COUNTRY_CODE, 2)) {
            project.systemFiles.CONFIG_TYPE_MODE.COUNTRY_CODE = $("#CountryCodeInput").val().toUpperCase();
            saveProjectAPI();
        }
    });

    document.getElementById('deviceNameText'            ).addEventListener("change", function(e) {
        if (verifyDeviceName(e, project.systemFiles.CONFIG_TYPE_DEVICE_NAME.DEVICE_URN)) {
            project.systemFiles.CONFIG_TYPE_DEVICE_NAME.DEVICE_URN  = document.getElementById('deviceNameText').value; 
            saveProjectAPI();
        }
    });
        
    document.getElementById('conPolAutoStartSelect'         ).addEventListener("change", function(e) { project.systemFiles.CONFIG_TYPE_STA_CONFIG.AUTOSTART        = document.getElementById('conPolAutoStartSelect'         ).value; saveProjectAPI();});
    document.getElementById('conPolFastConnectSelect'       ).addEventListener("change", function(e) { project.systemFiles.CONFIG_TYPE_STA_CONFIG.USEFASTCONNECT   = document.getElementById('conPolFastConnectSelect'       ).value; saveProjectAPI();});
    document.getElementById('conAnyWFDirectSelect'          ).addEventListener("change", function(e) { project.systemFiles.CONFIG_TYPE_STA_CONFIG.CONNECTTOANYP2P  = document.getElementById('conAnyWFDirectSelect'          ).value; saveProjectAPI();});
    document.getElementById('conPolAutoProvisioningSelect'  ).addEventListener("change", function(e) { project.systemFiles.CONFIG_TYPE_STA_CONFIG.AUTOPROVISIONING = document.getElementById('conPolAutoProvisioningSelect'  ).value; saveProjectAPI();});
    
    document.getElementById('AutoProvExternalConfSelect').addEventListener("change", function(e) { project.systemFiles.CONFIG_TYPE_MODE.AUTO_PROV_EXTERNAL_CONFIRMATION = document.getElementById('AutoProvExternalConfSelect').value; saveProjectAPI();});
    
    //TBD
}
/*---------------------------------Device Role Settings --> Station & Wi-Fi Direct Client Configuration -- > Wlan Settings----------------*/
function loadSTAWlanSettings()
{
    /*
    document.getElementById('staNoPspollEnable_1').value = project.systemFiles.CONFIG_TYPE_MODE.NO_PSPOLL_MODE;

    document.getElementById('staNoPspollEnable_1').addEventListener("change", function (e) {
        project.systemFiles.CONFIG_TYPE_MODE.NO_PSPOLL_MODE = $('#staNoPspollEnable_1').val(); saveProjectAPI();
    });*/

    $('#staNoPspollEnable_1').prop("checked", project.systemFiles.CONFIG_TYPE_MODE.NO_PSPOLL_MODE);
    document.getElementById('staNoPspollEnable_1').addEventListener("change", function (e) {
        project.systemFiles.CONFIG_TYPE_MODE.NO_PSPOLL_MODE = $('#staNoPspollEnable_1').is(':checked'); saveProjectAPI();
    });
}
/*---------------------------------Device Role Settings --> Station & Wi-Fi Direct Client Configuration -- > Network Settings----------------*/
function enableDisableSTAP2PBase()
{
    var enabled = document.getElementById('stap2pDHCPEnable_1').checked; 
    
    project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_IP_MODE = enabled? "True" : "False";
    saveProjectAPI();
    
    document.getElementById('staipAddrText'    ).disabled = enabled;
    document.getElementById('stasubnetMaskText').disabled = enabled;
    document.getElementById('stadgwText'       ).disabled = enabled;
    document.getElementById('stadnsText'       ).disabled = enabled;   
}

function enableDisableSTAP2P()
{
    enableDisableSTAP2PBase();
    document.getElementById('stap2pDHCPEnable_1').addEventListener('change', enableDisableSTAP2PBase, false);
}

function loadSTANWSettings(){
    if(project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_IP_MODE == "True")
    {
        document.getElementById('stap2pDHCPEnable_1').checked = 1;
    }
    else
    {
        document.getElementById('stap2pDHCPEnable_1').checked = 0;
    }
    
    document.getElementById('staipAddrText'     ).value = project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_STATIC_IP;
    document.getElementById('stasubnetMaskText' ).value = project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_SUBNET_MASK;
    document.getElementById('stadgwText'        ).value = project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_DEFAULT_GATEWAY;
    document.getElementById('stadnsText'        ).value = project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_IPV4_DNS_SERVER;
    
    
    document.getElementById('staipAddrText'     ).addEventListener("change", function(e) { if (validateIPaddress(e, project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_STATIC_IP         )){ project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_STATIC_IP       = document.getElementById('staipAddrText'     ).value; saveProjectAPI();}});
    document.getElementById('stasubnetMaskText' ).addEventListener("change", function(e) { if (validateIPaddress(e, project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_SUBNET_MASK       )){ project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_SUBNET_MASK     = document.getElementById('stasubnetMaskText' ).value; saveProjectAPI();}});
    document.getElementById('stadgwText'        ).addEventListener("change", function(e) { if (validateIPaddress(e, project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_DEFAULT_GATEWAY   )){ project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_DEFAULT_GATEWAY = document.getElementById('stadgwText'        ).value; saveProjectAPI();}});
    document.getElementById('stadnsText'        ).addEventListener("change", function(e) { if (validateIPaddress(e, project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_IPV4_DNS_SERVER   )){ project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_IPV4_DNS_SERVER = document.getElementById('stadnsText'        ).value; saveProjectAPI();}});
    
}
/*---------------------------------Device Role Settings --> AP & Wi-Fi Direct Client Configuration -- > WLan Settings------------------------*/
function add5gChannels2App() {
    
    //if (project.header.Is5GSupport == true) {
    if (project.systemFiles.CONFIG_TYPE_MODE.Is5GEnabled == true) {
        $('<option>').val('36').text('36').appendTo('#apChannelNum');
        $('<option>').val('40').text('40').appendTo('#apChannelNum');
        $('<option>').val('44').text('44').appendTo('#apChannelNum');
        $('<option>').val('48').text('48').appendTo('#apChannelNum');

       //if (project.systemFiles.CONFIG_TYPE_MODE.COUNTRY_CODE == "US") {
       //    $('<option>').val('149').text('149').appendTo('#apChannelNum');
       //    $('<option>').val('153').text('153').appendTo('#apChannelNum');
       //    $('<option>').val('157').text('157').appendTo('#apChannelNum');
       //    $('<option>').val('161').text('161').appendTo('#apChannelNum');
       //    $('<option>').val('165').text('165').appendTo('#apChannelNum');
       //}
       //else
       //{
       //    $("#apChannelNum option[value='149']").remove();
       //    $("#apChannelNum option[value='153']").remove();
       //    $("#apChannelNum option[value='157']").remove();
       //    $("#apChannelNum option[value='161']").remove();
       //    $("#apChannelNum option[value='165']").remove();
       //}
    }
}

function loadAPWLanSettings()
{
    add5gChannels2App();

    document.getElementById('apSSIDText'        ).value = project.systemFiles.CONFIG_TYPE_AP.SSID       ;
    document.getElementById('apMaxStaNum'       ).value = project.systemFiles.CONFIG_TYPE_AP.MAX_STT    ;
    document.getElementById('hiddenSSIDSelect'  ).value = project.systemFiles.CONFIG_TYPE_AP.HIDDEN_SSID;
    document.getElementById('apSecuritySelect'  ).value = project.systemFiles.CONFIG_TYPE_AP.SECURITY   ;
    if (project.systemFiles.CONFIG_TYPE_AP.SECURITY == '0') {//Open
        $('#apPWText').val('');
        $('#apPWText').prop("disabled", true);
    }
    else {
        $('#apPWText').val(project.systemFiles.CONFIG_TYPE_AP.PASSWORD);
        $('#apPWText').prop("disabled", false);
    }

    document.getElementById('apChannelNum'      ).value = project.systemFiles.CONFIG_TYPE_AP.CHANNEL    ;
    
    
    document.getElementById('apMaxStaNum'     ).addEventListener("change", function(e) { if (verifyNumLimits(e, 1 , 4 , project.systemFiles.CONFIG_TYPE_AP.MAX_STT )){ changeApMaxStationsNum();}});
    //document.getElementById('apChannelNum'    ).addEventListener("change", function(e) { if (verifyNumLimits(e, 1 , 14, project.systemFiles.CONFIG_TYPE_AP.CHANNEL )){ changeApChannelNum();}});
    //document.getElementById('apChannelNum').addEventListener("change", changeApChannelNum());
    document.getElementById('hiddenSSIDSelect').addEventListener("change", function(e) { 
                project.systemFiles.CONFIG_TYPE_AP.HIDDEN_SSID = $('#hiddenSSIDSelect').val(); saveProjectAPI();});
    document.getElementById('apSecuritySelect').addEventListener("change", changeApSecuritySelect);
                
    document.getElementById('apSSIDText'      ).addEventListener("change", function(e) { 
                if (verifyText(e, project.systemFiles.CONFIG_TYPE_AP.SSID, 0, 33)){project.systemFiles.CONFIG_TYPE_AP.SSID  = $('#apSSIDText').val(); saveProjectAPI();}});
    document.getElementById('apPWText'      ).addEventListener("change", function(e) { if (verifyPassword(e, project.systemFiles.CONFIG_TYPE_AP.PASSWORD)){project.systemFiles.CONFIG_TYPE_AP.PASSWORD  = $('#apPWText').val(); saveProjectAPI();}});
                
}
function changeApSecuritySelect()
{
    project.systemFiles.CONFIG_TYPE_AP.SECURITY    = $('#apSecuritySelect').val();

    if(project.systemFiles.CONFIG_TYPE_AP.SECURITY == '0')//Open
    {
        $('#apPWText').val('');
        $('#apPWText').prop("disabled", true);
    }
    else if (project.systemFiles.CONFIG_TYPE_AP.SECURITY == '1')
    {
        $('#apPWText').prop("disabled", false);
        project.systemFiles.CONFIG_TYPE_AP.PASSWORD  = '1234567890123';
        $('#apPWText').val(project.systemFiles.CONFIG_TYPE_AP.PASSWORD);

    }
    else if (project.systemFiles.CONFIG_TYPE_AP.SECURITY == '2')
    {
        $('#apPWText').prop("disabled", false);
        project.systemFiles.CONFIG_TYPE_AP.PASSWORD  = '12345678';
        $('#apPWText').val(project.systemFiles.CONFIG_TYPE_AP.PASSWORD);

    }
    
    saveProjectAPI();
}
function changeApMaxStationsNum()
{
    project.systemFiles.CONFIG_TYPE_AP.MAX_STT = $('#apMaxStaNum').val();
        
    //save project
    saveProjectAPI();
}
function verifyApChannelNum() {

    if (35 < parseInt(project.systemFiles.CONFIG_TYPE_AP.CHANNEL)) /*channel is A - band, need to set it back to B,G - band*/ {

        displayError("  AP Channel " + project.systemFiles.CONFIG_TYPE_AP.CHANNEL + " is illegal for 2.4 GHz band. Was changed to 1");
        project.systemFiles.CONFIG_TYPE_AP.CHANNEL = "1"; /*default for B,G band*/
    }
}

function changeApChannelNum()
{
    project.systemFiles.CONFIG_TYPE_AP.CHANNEL = $('#apChannelNum').val();
        
    //save project
    saveProjectAPI();
}
/*---------------------------------Device Role Settings --> AP & Wi-Fi Direct Client Configuration -- > Network Settings---------------------*/

function enableDisableAPP2P()
{
    //enableDisableSTAP2PBase();
    //document.getElementById('stap2pDHCPEnable_1').addEventListener('change', enableDisableSTAP2PBase, false);
}

function loadAPNWSettings()
{
    toggleFullOverlay($("#mainContent").css("width", "611px"), false);
    document.getElementById('apipAddrText'  ).value = project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_STATIC_IP;
    document.getElementById('apdgText'      ).value = project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_DEFAULT_GATEWAY;
    document.getElementById('apDNSText'     ).value = project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_IPV4_DNS_SERVER;

    document.getElementById('apDhcpStartAddressText'  ).value = project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_START_IP_ADDRESS;
    document.getElementById('apDhcpLastAddressText'   ).value = project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_LAST_IP_ADDRESS;
    document.getElementById('apDhcpLeaseTimeText'     ).value = project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_LEASE_TIME;

    document.getElementById('apipAddrText'          ).addEventListener("change", function(e) { if (validateIPaddress(e, project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_STATIC_IP                   )){ project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_STATIC_IP                  = document.getElementById('apipAddrText'          ).value; saveProjectAPI();}});
    document.getElementById('apdgText'              ).addEventListener("change", function(e) { if (validateIPaddress(e, project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_DEFAULT_GATEWAY             )){ project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_DEFAULT_GATEWAY            = document.getElementById('apdgText'              ).value; saveProjectAPI();}});
    document.getElementById('apDNSText'             ).addEventListener("change", function(e) { if (validateIPaddress(e, project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_IPV4_DNS_SERVER             )){ project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_IPV4_DNS_SERVER            = document.getElementById('apDNSText'             ).value; saveProjectAPI();}});
    document.getElementById('apDhcpStartAddressText').addEventListener("change", function(e) { if (validateIPaddress(e, project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_START_IP_ADDRESS )){ project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_START_IP_ADDRESS= document.getElementById('apDhcpStartAddressText').value; saveProjectAPI();}});
    document.getElementById('apDhcpLastAddressText' ).addEventListener("change", function(e) { if (validateIPaddress(e, project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_LAST_IP_ADDRESS  )){ project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_LAST_IP_ADDRESS = document.getElementById('apDhcpLastAddressText' ).value; saveProjectAPI();}});
    document.getElementById('apDhcpLeaseTimeText'   ).addEventListener("change", function(e) { if (verifyNumLimits  (e, 0, 24, project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_LEASE_TIME)){ project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_LEASE_TIME      = document.getElementById('apDhcpLeaseTimeText'   ).value; saveProjectAPI();}});

}
/*---------------------------------Device Role Settings --> Httpd Server ------------------------------------------------------------*/
function loadHttpServer() {
    toggleFullOverlay($("#mainContent").css("width", "500px"), false);

    $('#keyHttpPrimFileInput'   ).val(project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME     );
    $('#certHttpPrimFileInput'  ).val(project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME    );
    $('#cacertHttpPrimFileInput').val(project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME);

    fileBrowser.updateHttpCertFileName(project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME);
    fileBrowser.updateHttpKeyFileName(project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME);
    fileBrowser.updateHttpCaCertFileName(project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME);
   
    $('#SEC_PORT_ENABLE_0').prop("checked", project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_ENABLE);
    changeHttpSecondaryPortEnable();
    $('#ACCESS_ROM_1').prop("checked", project.systemFiles.CONFIG_TYPE_HTTP_SRV.ACCESS_ROM);

    $('#PRIM_PORT_SECURE_0').prop("checked", project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_SECURE);
    $('#ENABLE_CA_CERT_1'  ).prop("checked", project.systemFiles.CONFIG_TYPE_HTTP_SRV.ACCESS_CA_CERT);
    $('#PRIM_PORT_VAL'     ).val(project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL)
    $('#SEC_PORT_VAL'      ).val(project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_VAL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
    document.getElementById('PRIM_PORT_SECURE_0'  ).addEventListener('change', changeHttpPrimaryPort, false);
    document.getElementById('ENABLE_CA_CERT_1'    ).addEventListener('change', changeHttpCaCert, false);
    document.getElementById('SEC_PORT_ENABLE_0'   ).addEventListener('change', changeHttpSecondaryPortEnable, false);
    
    document.getElementById('ACCESS_ROM_1').addEventListener('change', function (e) { project.systemFiles.CONFIG_TYPE_HTTP_SRV.ACCESS_ROM = $('#ACCESS_ROM_1').is(':checked'); saveProjectAPI(); }, false);
    document.getElementById('PRIM_PORT_VAL').addEventListener("change", function (e) { if (verifyNumLimits(e, 0, 65535, project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL)) { project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL = $('#PRIM_PORT_VAL').val(); saveProjectAPI(); } });
    document.getElementById('SEC_PORT_VAL').addEventListener("change", function (e) { if (verifyNumLimits(e, 0, 65535, project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_VAL)) { project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_VAL = $('#SEC_PORT_VAL').val(); saveProjectAPI(); } });

    updateHttpPrimaryPortGui();

    loadRootUserFiles();
    //loadRootUserFilesKey();

    $("#httpCertSelect").val('-');
    $("#httpKeySelect").val('-');
    $("#httpCaCertSelect").val('-');
 /*
    $(document).on('change', '#httpCertSelect', function (e) {
        $('#certHttpPrimFileInput').val($("#httpCertSelect").val());
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME = $('#certHttpPrimFileInput').val();
        saveProjectAPI();
    });
    $(document).on('change', '#httpKeySelect', function (e) {
        $('#keyHttpPrimFileInput').val($("#httpKeySelect").val());
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME = $('#keyHttpPrimFileInput').val();
        saveProjectAPI();
    });
   $(document).off('change').on('change', '#httpCaCertSelect', function (e) {
        $('#cacertHttpPrimFileInput').val($("#httpCaCertSelect").val());
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME = $('#cacertHttpPrimFileInput').val();
        saveProjectAPI();
    });*/
    $('#httpCertSelect').off('change').on('change', function (e) {
        $('#certHttpPrimFileInput').val($("#httpCertSelect").val());
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME = $('#certHttpPrimFileInput').val();
        fileBrowser.updateHttpCertFileName(project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME);
        saveProjectAPI();
    });
    $('#httpKeySelect').off('change').on('change', function (e) {
        $('#keyHttpPrimFileInput').val($("#httpKeySelect").val());
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME = $('#keyHttpPrimFileInput').val();
        fileBrowser.updateHttpKeyFileName(project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME);
        saveProjectAPI();
    });
    $('#httpCaCertSelect').off('change').on('change', function (e) {
        $('#cacertHttpPrimFileInput').val($("#httpCaCertSelect").val());
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME = $('#cacertHttpPrimFileInput').val();
        fileBrowser.updateHttpCaCertFileName(project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME);
        saveProjectAPI();
    });
}
function onDialogDeleteHttpCert() {

    console.log('Callback HttpCert executed');
    $('#HTTP_PRIM_CERT_FILE').val('');
    $('#certHttpPrimFileInput').val('');
    fileBrowser.updateHttpCertFileName('');
    project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME = "";
    saveProjectAPI();
}

function onDialogDeleteHttpKey() {

    console.log('Callback HttpKey executed');
    $('#keyHttpPrimFileInput').val('');
    project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME = "";
    $('#HTTP_PRIM_KEY_FILE').val('');
    fileBrowser.updateHttpKeyFileName('');

    saveProjectAPI();
}
function onDialogDeleteHttpCaCert() {

    console.log('Callback HttpCaCert executed');
    $('#HTTP_CA_CERT_FILE').val('');
    $('#cacertHttpPrimFileInput').val('');
    fileBrowser.updateHttpCaCertFileName('');

    project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME = "";
    saveProjectAPI();
}
function loadRootUserFiles() {
    //var opts = this.div.find("#httpCertSelect");
    var copts = $("#httpCertSelect");
    
    var kopts = $("#httpKeySelect");

    var cacopts = $('#httpCaCertSelect');

    copts = copts.empty()[0];
    copts.options.add(new Option(""));

    kopts = kopts.empty()[0];
    kopts.options.add(new Option(""));

    cacopts = cacopts.empty()[0];
    cacopts.options.add(new Option(""));

    

    if (copts || kopts || cacopts)
    {
        if (!gFileBrowser) {
            gFileBrowser = new fileBrowser.Browser($("#fileSysDiv"), "offline", projectName);
        }

        if (projectName) {
            $.post("/api/1/flash/loadUserFiles", { "name": projectName })
                .done(function (data) {
                    if (data && data.ok) {
                        gFileBrowser.load(data.ok);


                        if (copts) {
                            $.each(gFileBrowser.root.files, function (i, file) {
                                copts.options.add(new Option(file.fullPath()));
                            });
                        }
                        if (kopts) {
                            $.each(gFileBrowser.root.files, function (i, file) {
                                kopts.options.add(new Option(file.fullPath()));
                            });
                        }

                        if (cacopts) {
                            $.each(gFileBrowser.root.files, function (i, file) {
                                cacopts.options.add(new Option(file.fullPath()));
                            });
                        }


                        if (gFileBrowser.certificationFileName) {
                            $(copts).val(gFileBrowser.certificationFileName);
                            $(kopts).val(gFileBrowser.certificationFileName);
                            $(cacopts).val(gFileBrowser.certificationFileName);
                        }
                    }
                });
            }
    }
   
}


function postHttpFile(id, funcname, json_num) {
    var spf = document.getElementById(id);
    if (spf) {
        if (/\.pem$/i.test(spf.files[0].name.toLowerCase()) === false)
        {
            postHttpFileBase(spf.files[0], funcname, json_num);

        } else {
            //fileBrowser.showInputDialog("The file is detected as PEM certificate. Specification requires a certificate where the last line ends the UNIX end line format; any other symbol may cause unexpected behavior. All line ends will be replaced to the Unix line format.",
            fileBrowser.showInputDialog(fileBrowser.getPemMessage(),
                               false,
                               {
                                   text: "Allow", func: function () {

                                       postHttpFileBase(spf.files[0], funcname, json_num);
                                   }
                               },
                               {
                                   text: "&#8196;Deny", func: function () {

                                   }
                               });

        }//pem
   }//spf
}

function postHttpFileBase(source, funcname, json_num) {


    var name = source.name;
    var found_same_name = false;

    if (!gFileBrowser) {
        gFileBrowser = new fileBrowser.Browser($("#fileSysDiv"), "offline", projectName);
    }
    progressModalOpen("Verifying user files, please wait...")
    if (projectName) {
        $.post("/api/1/flash/loadUserFiles", { "name": projectName })
            .done(function (data) {
                if (data && data.ok) {
                    gFileBrowser.load(data.ok);
                    
                    $.each(gFileBrowser.root.files, function (i, file) {
                        if (file.name == name) {
                            found_same_name = true;
                            
                            fileBrowser.showInputDialog("File with the same name already exists in the userfiles root folder",
                                            false,
                                             null,
                                            { text: "Close" });
                        }
                    });

                    if (!found_same_name) {
                        setTimeout(progressModalClose, 500);
                            /*
                            postUserFile(source,
                                () => { updateFiles(json_num, name); loadRootUserFiles(); },
                                            function (data) {
                                                if (data.err) {
                                                    progressModalError("Error updating file: " + data.err);
                                                } else {
                                                    progressModalError("Error updating file");
                                                }
                                            });
                            */

                        postUserFile(source,
                                function () { updateFiles(json_num, name); loadRootUserFiles(); },
                                            function (data) {
                                                if (data.err) {
                                                    progressModalError("Error updating file: " + data.err);
                                                } else {
                                                    progressModalError("Error updating file");
                                                }
                                            });


                        /*}
                        else {
                            postFileBase(source, "/api/1/flash/".concat(funcname),
                            function (data) {
                                updateFiles(json_num, name);
                                setTimeout(progressModalClose, 1000);
                            },
                            function (data) {
                                if (data.err) {
                                    progressModalError("Error updating file: " + data.err);
                                } else {
                                    progressModalError("Error updating file");
                                }
                            });
                        }*/
                    }
                } else if (data && data.err) {
                    progressModalError("Error loading users files: " + data.err)

                } else {
                    progressModalError("Error loading users files")
                }
            })
            .fail(function (data) {
                progressModalError("Error loading users files")
            });
    }
}

function postUserFile(source, done, fail) {

    //var origFileSize = data.size;
    //var defaultMaxSize = 256 * 1024;
    var name = source.name
    var size = source.size
    var props = {}
    props.name = name
    props.maxFileSize = size
    props.origFileSize = size
    props.fileToken = ''
    props.flagFailSafe = false
    props.flagNoSignatureTest = false
    props.flagPublicRead = false
    props.flagPublicWrite = false
    props.flagSecure = false
    props.flagStatic = false
    props.flagVendor = false
    props.signatureFileName = ''
    props.signatureFileNameSelect = '1'
    props.certificationFileName = ''

    var xhr = new XMLHttpRequest();
    var formData = new FormData();

    formData.append("name", project.header.Name);
    formData.append("source", source);
    formData.append("dest", source.name);

    formData.append("props", JSON.stringify(props));

    var that = this;
    xhr.onreadystatechange = function () {
        progressModalOpen("Writing File");
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                done();
            } else if (data && data.err) {
                fail(data.err);
            } else {
                fail("data error");
            }
        } else if (xhr.status > 200) {
            fail("common error");
        }
    }

    //xhr.open("POST", "/api/1/flash/uploadMCUSimpleMode", true);
    xhr.open("POST", "/api/1/flash/uploadProjectFsFile", true);

    xhr.send(formData);
}

function changeHttpSecondaryPortEnable() {
    project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_ENABLE = $('#SEC_PORT_ENABLE_0').is(':checked');
    if (!project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_ENABLE) {
        $("#SEC_PORT_VAL").prop("disabled", true);
    }
    else {
        $("#SEC_PORT_VAL").prop("disabled", false);
    }
    saveProjectAPI();
}

function updateHttpPrimaryPortGui()
{
    if (project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_SECURE) {
        /*
        if (!project.systemFiles.CONFIG_TYPE_HTTP_SRV.DEFAULT_CHANGED) {
            project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL = '443'
            project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_VAL = '80'
            //project.systemFiles.CONFIG_TYPE_HTTP_SRV.DEFAULT_CHANGED = true;
        }
          */
        $('#PRIM_PORT_VAL').val(project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL)
        $('#SEC_PORT_VAL').val(project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_VAL)
        
        $('#keyHttpPrimFileInput').prop("disabled", false);
        $('#httpKeySelect').prop("disabled", false);
        $('#browseHttpPrimKeyFile').prop("disabled", false).removeClass("btnDisabled");
        //$('#clearHttpPrimKeyFile').prop("disabled", false).removeClass("btnDisabled");
        $('#HTTP_PRIM_KEY_FILE').prop("disabled", false);
        
        $('#certHttpPrimFileInput').prop("disabled", false);
        $('#httpCertSelect').prop("disabled", false);
        $('#browseHttpPrimCertFile').prop("disabled", false).removeClass("btnDisabled");
        //$('#clearHttpPrimCertFile').prop("disabled", false).removeClass("btnDisabled");
        $('#HTTP_PRIM_CERT_FILE').prop("disabled", false);

        $('#ENABLE_CA_CERT_1').prop("disabled", false);
        $('#SEC_PORT_ENABLE_0').prop("disabled", false);

    }
    else {
        /*
        if (!project.systemFiles.CONFIG_TYPE_HTTP_SRV.DEFAULT_CHANGED)
        {
            project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL = '80'
            project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_VAL = '8080'

            project.systemFiles.CONFIG_TYPE_HTTP_SRV.DEFAULT_CHANGED = true;
        }*/

        $('#PRIM_PORT_VAL').val(project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL)
        $('#SEC_PORT_VAL').val(project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_VAL)
        $('#keyHttpPrimFileInput').prop("disabled", true);
        $('#httpKeySelect').prop("disabled", true);
        $('#browseHttpPrimKeyFile').prop("disabled", true).addClass("btnDisabled");
        //$('#clearHttpPrimKeyFile').prop("disabled", true).addClass("btnDisabled")/*.off('click')*/;
        $('#HTTP_PRIM_KEY_FILE').prop("disabled", true);

        $('#certHttpPrimFileInput').prop("disabled", true);
        $('#httpCertSelect').prop("disabled", true);
        $('#browseHttpPrimCertFile').prop("disabled", true).addClass("btnDisabled");
        //$('#clearHttpPrimCertFile').prop("disabled", true).addClass("btnDisabled")/*.off('click')*/;
        $('#HTTP_PRIM_CERT_FILE').prop("disabled", true);
        $('#ENABLE_CA_CERT_1').prop("disabled", true);
        $('#SEC_PORT_ENABLE_0').prop("disabled", true);
    }
    changeHttpCaCertGui();
    saveProjectAPI();
}
/*
function clearHttpKeyFile() {

    if (project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_SECURE & ($('#keyHttpPrimFileInput').val().trim()!= '')) {

        //clearProjectFile("/api/1/flash/deleteHttpKeyFile", $('#keyHttpPrimFileInput'), 0);
        $('#keyHttpPrimFileInput').val('');
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME = "";
        $('#HTTP_PRIM_KEY_FILE').val('');
        fileBrowser.updateHttpKeyFileName('');

        saveProjectAPI();
    }
}

function clearHttpCertFile() {

    if (project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_SECURE & $('#certHttpPrimFileInput').val().trim() != '') {

        //clearProjectFile("/api/1/flash/deleteHttpCertFile", $('#certHttpPrimFileInput'), 1);

        $('#HTTP_PRIM_CERT_FILE').val('');
        $('#certHttpPrimFileInput').val('');
        fileBrowser.updateHttpCertFileName('');
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME = "";
        saveProjectAPI();
    }
}

function clearHttpCAFile() {

    if (project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_SECURE & project.systemFiles.CONFIG_TYPE_HTTP_SRV.ACCESS_CA_CERT & $('#cacertHttpPrimFileInput').val().trim() != '') {


        //clearProjectFile("/api/1/flash/deleteHttpCAFile", $('#cacertHttpPrimFileInput'), 2);

        $('#HTTP_CA_CERT_FILE').val('');
        $('#cacertHttpPrimFileInput').val('');
        fileBrowser.updateHttpCaCertFileName('');

        project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME = "";
        saveProjectAPI();

    }
}
*/
function changeHttpPrimaryPort() {
    project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_SECURE = $('#PRIM_PORT_SECURE_0').is(':checked');

    if (project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_SECURE)
    {
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL = '443'
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_VAL = '80'
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_ENABLE = true
    }
    else
    {
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL = '80'
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_VAL = '8080'
        project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_ENABLE = false
    }

    $('#SEC_PORT_ENABLE_0').prop("checked", project.systemFiles.CONFIG_TYPE_HTTP_SRV.SEC_PORT_ENABLE);
    changeHttpSecondaryPortEnable();

    updateHttpPrimaryPortGui();

    project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_VAL = $('#PRIM_PORT_VAL').val();
    saveProjectAPI();
}

function changeHttpCaCertGui() {

    if (project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIM_PORT_SECURE & project.systemFiles.CONFIG_TYPE_HTTP_SRV.ACCESS_CA_CERT) {
        $('#cacertHttpPrimFileInput').prop("disabled", false);
        $('#httpCaCertSelect').prop("disabled", false);
        $('#browseHttpCaCertFile').prop("disabled", false).removeClass("btnDisabled");
       // $('#clearHttpCaCertFile').prop("disabled", false).removeClass("btnDisabled")/*.off('click').on('click', clearProjectFile("/api/1/flash/deleteHttpCAFile", $('#cacertHttpPrimFileInput')))*/;
        $('#HTTP_CA_CERT_FILE').prop("disabled", false);
    }
    else {
        $('#cacertHttpPrimFileInput').prop("disabled", true);
        $('#httpCaCertSelect').prop("disabled", true);
        $('#browseHttpCaCertFile').prop("disabled", true).addClass("btnDisabled");
       // $('#clearHttpCaCertFile').prop("disabled", true).addClass("btnDisabled")/*.off('click')*/;
        $('#HTTP_CA_CERT_FILE').prop("disabled", true);
    }
}

function changeHttpCaCert() {
    project.systemFiles.CONFIG_TYPE_HTTP_SRV.ACCESS_CA_CERT = $('#ENABLE_CA_CERT_1').is(':checked');

    changeHttpCaCertGui();

    saveProjectAPI();
}
/*---------------------------------Device Role Settings --> Network Applications ------------------------------------------------------------*/

function loadNetworkApp()
{
    toggleFullOverlay($("#mainContent").css("width", "611px"), false);
    $('#STA_HTTP_SID_1').prop("checked", project.systemFiles.CONFIG_TYPE_MODE.STA_START_APPS & 1);
    $('#STA_MDNS_ID_4' ).prop("checked", project.systemFiles.CONFIG_TYPE_MODE.STA_START_APPS & 4 );
    
    $('#AP_HTTP_SID_1').prop("checked", project.systemFiles.CONFIG_TYPE_MODE.AP_START_APPS & 1  );
    $('#AP_DHCP_SID_2').prop("checked", project.systemFiles.CONFIG_TYPE_MODE.AP_START_APPS & 2  );
    $('#AP_MDNS_ID_4' ).prop("checked", project.systemFiles.CONFIG_TYPE_MODE.AP_START_APPS & 4  );
    $('#AP_DNS_SID_8' ).prop("checked", project.systemFiles.CONFIG_TYPE_MODE.AP_START_APPS & 8  );
    
    $('#CLS_HTTP_SID_1').prop("checked", project.systemFiles.CONFIG_TYPE_MODE.P2P_CLS_START_APPS & 1 );
    $('#CLS_MDNS_ID_4' ).prop("checked", project.systemFiles.CONFIG_TYPE_MODE.P2P_CLS_START_APPS & 4 );
    
    $('#GO_HTTP_SID_1').prop("checked", project.systemFiles.CONFIG_TYPE_MODE.P2P_GO_START_APPS & 1 );
    $('#GO_DHCP_SID_2').prop("checked", project.systemFiles.CONFIG_TYPE_MODE.P2P_GO_START_APPS & 2 );
    $('#GO_MDNS_ID_4' ).prop("checked", project.systemFiles.CONFIG_TYPE_MODE.P2P_GO_START_APPS & 4 );
    $('#GO_DNS_SID_8' ).prop("checked", project.systemFiles.CONFIG_TYPE_MODE.P2P_GO_START_APPS & 8 );
    
    
    
    document.getElementById('STA_HTTP_SID_1').addEventListener('change', updateSTANetApp, false);
    //document.getElementById('STA_DHCP_SID_2').addEventListener('change', updateSTANetApp, false);
    document.getElementById('STA_MDNS_ID_4' ).addEventListener('change', updateSTANetApp, false);
    //document.getElementById('STA_DNS_SID_8' ).addEventListener('change', updateSTANetApp, false);
    //document.getElementById('STA_DC_ID_16'  ).addEventListener('change', updateSTANetApp, false);    
    
    document.getElementById('AP_HTTP_SID_1').addEventListener('change', updateAPNetApp, false);
    document.getElementById('AP_DHCP_SID_2').addEventListener('change', updateAPNetApp, false);
    document.getElementById('AP_MDNS_ID_4' ).addEventListener('change', updateAPNetApp, false);
    document.getElementById('AP_DNS_SID_8' ).addEventListener('change', updateAPNetApp, false);
    //document.getElementById('AP_DC_ID_16'  ).addEventListener('change', updateAPNetApp, false);    
    
    document.getElementById('CLS_HTTP_SID_1').addEventListener('change', updateCLSNetApp, false);
    //document.getElementById('CLS_DHCP_SID_2').addEventListener('change', updateCLSNetApp, false);
    document.getElementById('CLS_MDNS_ID_4' ).addEventListener('change', updateCLSNetApp, false);
    //document.getElementById('CLS_DNS_SID_8' ).addEventListener('change', updateCLSNetApp, false);
    //document.getElementById('CLS_DC_ID_16'  ).addEventListener('change', updateCLSNetApp, false);
    
    document.getElementById('GO_HTTP_SID_1').addEventListener('change', updateGONetApp, false);
    document.getElementById('GO_DHCP_SID_2').addEventListener('change', updateGONetApp, false);
    document.getElementById('GO_MDNS_ID_4' ).addEventListener('change', updateGONetApp, false);
    document.getElementById('GO_DNS_SID_8' ).addEventListener('change', updateGONetApp, false);
    //document.getElementById('GO_DC_ID_16'  ).addEventListener('change', updateGONetApp, false);
  
}

function updateSTANetApp()
{
    project.systemFiles.CONFIG_TYPE_MODE.STA_START_APPS =      (($('#STA_HTTP_SID_1').is(':checked') ? $('#STA_HTTP_SID_1').val() : 0) |
                                                                ($('#STA_DHCP_SID_2').is(':checked') ? $('#STA_DHCP_SID_2').val() : 0) |
                                                                ($('#STA_MDNS_ID_4' ).is(':checked') ? $('#STA_MDNS_ID_4').val() : 0) |
                                                                ($('#STA_DNS_SID_8' ).is(':checked') ? $('#STA_DNS_SID_8').val() : 0) |
                                                                ($('#STA_DC_ID_16'  ).is(':checked') ? $('#STA_DC_ID_16').val() : 0));

    //save project
	saveProjectAPI();
}

function updateAPNetApp()
{

    project.systemFiles.CONFIG_TYPE_MODE.AP_START_APPS =       (($('#AP_HTTP_SID_1').is(':checked') ? $('#AP_HTTP_SID_1').val() : 0) |
                                                                ($('#AP_DHCP_SID_2').is(':checked') ? $('#AP_DHCP_SID_2').val() : 0) |
                                                                ($('#AP_MDNS_ID_4' ).is(':checked') ? $('#AP_MDNS_ID_4').val() : 0) |
                                                                ($('#AP_DNS_SID_8' ).is(':checked') ? $('#AP_DNS_SID_8').val() : 0) |
                                                                ($('#AP_DC_ID_16'  ).is(':checked') ? $('#AP_DC_ID_16').val() : 0));
    //save project
	saveProjectAPI();

}

function updateCLSNetApp()
{

    project.systemFiles.CONFIG_TYPE_MODE.P2P_CLS_START_APPS = ( (document.getElementById('CLS_HTTP_SID_1').checked? document.getElementById('CLS_HTTP_SID_1').value :0) | 
                                                                (document.getElementById('CLS_MDNS_ID_4' ).checked? document.getElementById('CLS_MDNS_ID_4' ).value :0) );
    //save project
	saveProjectAPI();
}

function updateGONetApp()
{

    project.systemFiles.CONFIG_TYPE_MODE.P2P_GO_START_APPS  = ( (document.getElementById('GO_HTTP_SID_1').checked? document.getElementById('GO_HTTP_SID_1').value :0) | 
                                                                (document.getElementById('GO_DHCP_SID_2').checked? document.getElementById('GO_DHCP_SID_2').value :0) | 
                                                                (document.getElementById('GO_MDNS_ID_4' ).checked? document.getElementById('GO_MDNS_ID_4' ).value :0) | 
                                                                (document.getElementById('GO_DNS_SID_8' ).checked? document.getElementById('GO_DNS_SID_8' ).value :0) );
    //save project
	saveProjectAPI();
}

/*-------------------------------------------------------------------------------------------------------------------------------------------*/
function loadDevice()
{
    document.getElementById('STA_TX_PL_SUM').innerHTML  = project.systemFiles.CONFIG_TYPE_MODE.STA_TX_POWER_LEVEL ;
    document.getElementById('AP_TX_PL_SUM').innerHTML  = project.systemFiles.CONFIG_TYPE_MODE.AP_TX_POWER_LEVEL   ;
    
    var phy_mode;
    switch (project.systemFiles.CONFIG_TYPE_MODE.PHY_CAL_MODE)
    {
        case 0:
        case "0":    phy_mode = 'Normal';
                break;
        case 1:
        case "1":    phy_mode = 'Trigger';
                break;
        case 2:
        case "2":    phy_mode = 'Onetime';
                break;

    }
    
    document.getElementById('PHY_CAL_MODE_SUM').innerHTML  =    phy_mode;
    
    var dev_high_tx_pow;
    switch (project.systemFiles.CONFIG_TYPE_MODE.PHY_STA_HIGH_TX_POWER)
    {
        case 0:
        case "0":    dev_high_tx_pow = 'Normal';
                break;
        case 1:
        case "1":    dev_high_tx_pow = 'High';
                break;
    }
    
    document.getElementById('PHY_DEVICE_HIGH_TX_POWER_SUM').innerHTML  =    dev_high_tx_pow;
}

function loadDeviceRoleSettingsSum()
{
    var role_settings;
    switch (project.systemFiles.CONFIG_TYPE_MODE.START_ROLE)
    {
        case "2":
        case 2  :    role_settings = 'Access Point';
                    break;
        case "3":            
        case 3:    role_settings = 'P2P';
                break;
        case "0":
        case 0:    role_settings = 'Station';
                break;

    }
    
    document.getElementById('START_ROLE_SELECT_SUM').innerHTML  =    role_settings;
    var val;/*backward compatibility vs gen2 old*/
    switch (project.systemFiles.CONFIG_TYPE_MODE.COUNTRY_CODE) {
        case "0":
        case 0: val = 'EU';
            break;
        case "1":
        case 1: val = 'US';
            break;
        case "2":
        case 2: val = 'JP';
            break;
        default: val = project.systemFiles.CONFIG_TYPE_MODE.COUNTRY_CODE;
            break;
    }
    document.getElementById('COUNTRY_CODE_SUM').innerHTML = val;

}

function loadSTADirectDeviceSum()
{
   
    document.getElementById('DHCP_CLIENT_ENABLE_SUM').innerHTML  =   project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_IP_MODE          ;
    document.getElementById('STA_IP_ADDRESS_SUM'    ).innerHTML  =   project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_STATIC_IP        ;
    document.getElementById('STA_SUBNET_MASK_SUM'   ).innerHTML  =   project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_SUBNET_MASK      ;
    document.getElementById('STA_DEFAULT_GW_SUM'    ).innerHTML  =   project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_DEFAULT_GATEWAY  ;
    document.getElementById('STA_DNS_SRVR_SUM'      ).innerHTML  =   project.systemFiles.CONFIG_TYPE_IP_CONFIG.STA_IPV4_DNS_SERVER  ;
    
}

function loadAPDirectDeviceSum()
{

    document.getElementById('AP_IP_ADDRESS_SUM'         ).innerHTML  =   project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_STATIC_IP                     ;
    document.getElementById('AP_DEFAULT_GW_SUM'         ).innerHTML  =   project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_DEFAULT_GATEWAY               ;
    document.getElementById('AP_DNS_SRVR_SUM'           ).innerHTML  =   project.systemFiles.CONFIG_TYPE_IP_CONFIG.AP_IPV4_DNS_SERVER               ;
    document.getElementById('apDhcpStartAddressTextSum' ).innerHTML  =   project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_START_IP_ADDRESS   ;
    document.getElementById('apDhcpLastAddressTextSum'  ).innerHTML  =   project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_LAST_IP_ADDRESS    ;
    document.getElementById('apDhcpLeaseTimeSum'        ).innerHTML  =   project.systemFiles.CONFIG_TYPE_DHCP_SRV.AP_DHCP_SERVER_LEASE_TIME         ;
}

function loadFilesSP()
{
    toggleFullOverlay($("#mainContent").css("width", "611px"), false);
    $('#spFileInput').prop('readonly', true);
    $('#spFileInput').val(project.systemFiles.FILES.SP_FILE_NAME.replace(/^.*[\\\/]/, ''));

    $('#spFileInputSimple').prop('readonly', true);
    $('#spFileInputSimple').val(project.systemFiles.FILES.SP_FILE_NAME.replace(/^.*[\\\/]/, ''));
}

function loadFilesSum()
{
    document.getElementById('SP_FILE_SUM'         ).innerHTML = project.systemFiles.FILES.SP_FILE_NAME.replace(/^.*[\\\/]/, '');
    
    //if(project.systemFiles.FILES.DEFAULT_CERTIFICATES)
    if(project.header.UseDefaultCertStore)
    {
        document.getElementById('CS_FILE_SUM').innerHTML = '/files/certstore.lst';

        var cert_sig_name = '/files/certstore.lst.signed';
        if (project.header.IsGen3) {

            cert_sig_name = '/files/certstore.lst.g3.signed';
        }

        document.getElementById('CSS_FILE_SUM'          ).innerHTML = cert_sig_name;
    }
    else
    {
        document.getElementById('CS_FILE_SUM'          ).innerHTML = project.systemFiles.FILES.CS_FILE_NAME.replace(/^.*[\\\/]/, '') ;
        document.getElementById('CSS_FILE_SUM'         ).innerHTML = project.systemFiles.FILES.CSS_FILE_NAME.replace(/^.*[\\\/]/, '');
    }
}

function changeProductionDevice(save) {   
    if($('#ProductionNWP').val() != undefined)
    {
        project.header.ProductionNWP = $('#ProductionNWP').val();
        
        if (save !== false) {
            //save project
            saveProjectAPI();
        }
    }
}

function loadProductionNWP()
{
    document.getElementById('ProductionNWP').value = project.header.ProductionNWP;
}

function changeFactoryConfig(save)
{
    if($('#FACTORY').val() != undefined)
    {
        project.header.EnableReturnToFactory = $('#FACTORY').val();
        
        if (save !== false) {
            //save project
            saveProjectAPI();
        }
    }
}

function loadFactoryConfig()
{
    document.getElementById('FACTORY').value = project.header.EnableReturnToFactory;
    
    /*if(localStorage.getItem('isconnected') == 'true')
    {
        $('select#FACTORY').prop("disabled", true);
    }
    else
    {
        $('select#FACTORY').prop("disabled", false);
    }*/
}

function changeFactorySOP(save)
{  
    if($('#IGNORE_FORCE_AP').val() != undefined)
    {
        project.systemFiles.CONFIG_TYPE_MODE.IGNORE_FORCE_AP = $('#IGNORE_FORCE_AP').val().toString();
        project.header.ReturnToFactoryGPIO = project.systemFiles.CONFIG_TYPE_MODE.IGNORE_FORCE_AP.toString();/*Katjas' requirment*/
        
        if(project.systemFiles.CONFIG_TYPE_MODE.IGNORE_FORCE_AP == "1")
        {
            document.getElementById('FACTORY').options[1].style.display = 'block';
        }
        else
        {
           document.getElementById('FACTORY').options[1].style.display = 'none';
        }

        if($("#FACTORY").val() =='none')
        {
              $("#FACTORY").val('defaults_and_image') ;
               project.header.EnableReturnToFactory = $('#FACTORY').val();
        }
        
        if (save !== false) {
            //save project
            saveProjectAPI();
        }
    }
}

function loadFactorySOP()
{
    document.getElementById('IGNORE_FORCE_AP').value = project.systemFiles.CONFIG_TYPE_MODE.IGNORE_FORCE_AP;
    
        if(project.systemFiles.CONFIG_TYPE_MODE.IGNORE_FORCE_AP == "1")
        {
            document.getElementById('FACTORY').options[1].style.display = 'block';
 
        }
        else
        {
           document.getElementById('FACTORY').options[1].style.display = 'none';
        }
        
        
    /*if(localStorage.getItem('isconnected') == 'true')
    {
        $('#IGNORE_FORCE_AP').prop("disabled", true);
    }
    else
    {
        $('#IGNORE_FORCE_AP').prop("disabled", false);
    }*/
    
}

function loadGeneralSettings()
{
    loadModuleType();
    loadMode();
    loadCapacity();
    
    loadSecureDevice();
    loadProductionNWP();
    loadFactoryConfig();
    loadFactorySOP();
    loadMacAddress();
    loadEncryptionKey();
    changeEncryptionKey();
    
    if(project.header.Mode == 'development')
    {
        loadDevelopeMacAddress();
    }
    
    if(localStorage.getItem('isconnected') == 'true')
    {
        $('#connectSpan').text("Disconnect");
    }
    else
    {
        $('#connectSpan').text("Connect");
    }
    
    $("#encrKeyFileNote").css("font-weight", "bold");

    
    $('#generalSettingsProductionDeviceDiv').hide();
    window.addEventListener('keypress', keypressenter, false);

    document.getElementById('USE_KEY_1').addEventListener('change', changeEncryptionKey, false);

}
function keypressenter(e)
{
    if (e.keyCode == 112) {
        $('#generalSettingsProductionDeviceDiv').show();
    }
    else { $('#generalSettingsProductionDeviceDiv').hide(); }
}
function changeEncryptionKey()
{
    if (project.header.UseKey) {
        $('#fieldsetKeySourceFileBrowse').show();
    } else {
        $('#fieldsetKeySourceFileBrowse').hide();
    }
}

function addEncryptionKey()
{
     project.header.UseKey = document.getElementById('USE_KEY_1').checked;
     
    //save project
	saveProjectAPI();
}

function changeSecureDevice(save)
{
    if($('#3200Select').val() != undefined)
    {
        project.header.IsTheDeviceSecure = $('#3200Select').val() == '1' ? false : true;

        updateEncryptionFileGui (project.header.IsTheDeviceSecure);
        project.header.KeyFileLocation = $('#keyFileInput').val();

        if (save !== false) {
            //save project
            saveProjectAPI();
        }

        $('#3200Select').prop("disabled", true);
    }

   
}

function updateEncryptionFileGui (enable)
{
    $('input#USE_KEY_1'         ).prop("disabled", !enable);
    $('input#KEY_FILE'          ).prop("disabled", !enable);
    $('input#keyFileInput'      ).prop("disabled", !enable);   
    
    if(!enable)
    {
        $('div#browseKeyFile').addClass("btnDisabled");
        $('#USE_KEY_1').prop("checked", project.header.UseKey);
        addEncryptionKey();
    }
    else
    {
        $('div#browseKeyFile').removeClass("btnDisabled");
    }
    
    if(project.header.IsTheDeviceSecure == true)
    {
        $('#fieldsetKeySourceFile').show();
    }
    else
    {
        $('#fieldsetKeySourceFile').hide();
    }
}

function loadSecureDevice()
{
    $('#3200Select').val(project.header.IsTheDeviceSecure == true ? '2' : '1');
    var kfl = '';
    if(project.header.KeyFileLocation)
        {
            kfl = project.header.KeyFileLocation.replace(/^.*[\\\/]/, '');
        }
    $('#keyFileInput').val(kfl);
    
    updateEncryptionFileGui(project.header.IsTheDeviceSecure);

    $('#3200Select').prop("disabled", true);
}

function loadEncryptionKey()
{
    if($('#keyFileInput').val() != undefined)
    {
        document.getElementById('USE_KEY_1').addEventListener('change', addEncryptionKey, false);
        $('#keyFileInput').prop('readonly', true);
        
        $('#USE_KEY_1').prop("checked", project.header.UseKey);

    }
}

function loadModuleType() {

    if (project.header.IsGen3) {
        $('#fieldsetGeneralSettingsModuleType').show();

        //$('select#ModuleTypeSelect').prop('disabled', true);

        $('#ModuleTypeSelect').val(project.header.IsModule)
    }
    else {

        $('#fieldsetGeneralSettingsModuleType').hide();
    }
   
}

function loadMode()
{
    if(project.header.Mode == 'development')
    {
        document.getElementById('ModeSelect').value = 2;
        document.getElementById('generalSettingsDevelopMacAddressText').style.display = 'block';
    }
    else
    {
        document.getElementById('ModeSelect').value = 1;
        document.getElementById('generalSettingsDevelopMacAddressText').style.display = 'none';
    }
    
    if(localStorage.getItem('isconnected') == 'true')
    {
        document.getElementById('ModeSelect').disabled = true;
        document.getElementById('generalSettingsDevelopMacAddressText').disabled = true;
    }
    else
    {
        document.getElementById('ModeSelect').disabled = false;
        document.getElementById('generalSettingsDevelopMacAddressText').disabled = false;
    }
    
}
function changeModuleSelect() {

    project.header.IsModule = $("#ModuleTypeSelect").val();
    saveProjectAPI();
}

function changeMode(save)
{
    var strText = $("#ModeSelect option:selected").text().toLowerCase();
    
    if((strText == 'development') || (strText == 'production'))
    {
        project.header.Mode = strText;
    }
    
    if(project.header.Mode == 'development')
    {
        $('#generalSettingsDevelopMacAddressText').show();
        
        //$('#BrowseBtn').removeClass('btnDisabled');
        //$('#BrowseBtn').off("click").on('click', browse);
    }
    else
    {
        $('#generalSettingsDevelopMacAddressText').hide();
        
        $('#BrowseBtn').addClass('btnDisabled').off('click');
    }
    
   if (save !== false) {
        //save project
        saveProjectAPI(function() { displayMainContent("generalsettings"); });
   }
}

function changeCapacity(save)
{
    if($('#capacitySelect').val() != undefined)
    {
        project.header.StorageCapacityBytes = $('#capacitySelect').val();
        
        if (save !== false) {
            //save project
            saveProjectAPI();
        }
    }

}

function loadCapacity()
{
    document.getElementById('capacitySelect').value = project.header.StorageCapacityBytes;
    
    /*if(localStorage.getItem('isconnected') == 'true')
    {
        document.getElementById('capacitySelect').disabled = true;
    }
    else
    {
        document.getElementById('capacitySelect').disabled = false;
    }*/
}

function loadGenerateImage()
{
var pname = localStorage.getItem('projectName');
$.post("/api/1/flash/getImageFileExist",   {'name':pname},
				function(data,status){
                    //if sli file exists
					if(status=='success' && data.ok){
                        //enable save button
						        $('#saveImageButton').removeClass('btnDisabled');
                                $('#saveImageButton').off('click').on('click', {kind: "sli"}, saveImage);
                                
                                $('#saveUCFButton').removeClass('btnDisabled');
                                $('#saveUCFButton').off('click').on('click', {kind: "ucf"}, saveImage);
                                
                                $('#saveBINButton').removeClass('btnDisabled');
                                $('#saveBINButton').off('click').on('click', {kind: "bin"}, saveImage);
                                
                                $('#saveHEXButton').removeClass('btnDisabled');
                                $('#saveHEXButton').off('click').on('click', {kind: "hex"}, saveImage);
					}
					else if(data.err){
                        console.log(data.err);
						//disable save button
					$('#saveImageButton').addClass('btnDisabled').off('click');
					$('#saveUCFButton').addClass('btnDisabled').off('click');
					$('#saveBINButton').addClass('btnDisabled').off('click');
					$('#saveHEXButton').addClass('btnDisabled').off('click');
					}

                });
    updateBrowseBtn();
}

function clearServicePackFile()
{
    project.systemFiles.FILES.SP_FILE_NAME = '';
    project.header.ServicePackFileLocation = '';
    $('#spFileInput').val('');
    $('#spFileInputSimple').val('');
    saveProjectAPI();
}

function checkDeviceType()
{
    return true;
}

function loadGeneralSum()
{
    $('#Name_SUM').html(project.header.Name);   
    $('#Mode_SUM').html(project.header.Mode);   
    
    $('#DeviceType_SUM').html(project.header.DeviceType);
    
    switch(project.header.StorageCapacityBytes)
    {
        
        case '1048576' : $('#Capacity_SUM').html('1M Byte'); 
                        break;
        case '2097152' : $('#Capacity_SUM').html('2M Byte'); 
                        break;
        case '4194304' : $('#Capacity_SUM').html('4M Byte'); 
                        break;
        case '8388608' : $('#Capacity_SUM').html('8M Byte'); 
                        break;
        case '16777216': $('#Capacity_SUM').html('16M Byte'); 
                        break;
         
         default:    $('#Capacity_SUM').html('1M Byte');           
    }
    
    updateBrowseBtn();
}

function verifyText(e, oldVal, min, max)
{
    var str = e.currentTarget.value;
    if((str.length > min) && (str.length < max))
    {
        return true;
    }
    else
    {
        fileBrowser.showInputDialog('String length should be between ' +  Number(min + 1)  + ' - ' + Number(max - 1) + '!', 
                        false, 
                        null,
                        {text: "Close"});
        e.currentTarget.value = oldVal;
        return false;
    }
}

function verifyPassword(e, oldVal)
{
    var str = e.currentTarget.value;
    
    if($('#apSecuritySelect').val() == '1')/*WEP*/
    {
        /*check length 13 or 5*/
        if((str.length == 13) || (str.length == 5))
        {
            /*check ASCII input*/
            if(/^[\x00-\x7F]*$/.test(str))/*ASCII*/
            {
                return true;
            }
            else
            {
                fileBrowser.showInputDialog('Wrong ASCII input!', 
                        false, 
                        null,
                        {text: "Close"});
            }
        }
        else
        {
            fileBrowser.showInputDialog('Wrong password length! Should be 13 or 5!', 
                        false, 
                        null,
                        {text: "Close"});

        }
    }
    else if ($('#apSecuritySelect').val() == '2') /*WPA*/
    {
        if((str.length < 8)||(str.length > 63))
        {
            fileBrowser.showInputDialog('Wrong password length! Should be 8-63!', 
                        false, 
                        null,
                        {text: "Close"});
        }
        else
        {
            return true;
        }
    }
    else/*Open*/
    {
        return true;
    }
    
    e.currentTarget.value = oldVal;
    return false;
}

function filterFloat(str)
{
    if (/^-*([0-9]{0,2}[\.0-9]{0,4})$/
         .test(str))
        return Number(str);
    return NaN;
}

function verifyNumLimitsTT(e, min, max, oldVal) {
    
    var n = filterFloat(e.currentTarget.value);
    if (isNaN(n))
    {
        fileBrowser.showInputDialog('Wrong input, max 2 digits before and 3 digits after floating point',
                        false,
                        null,
                        { text: "Close" });

        e.currentTarget.value = oldVal;

        return false;
    }

    if (n >= min && n <= max) {
        return true;
    }
    else {
        fileBrowser.showInputDialog('Value should be between ' + min + ' - ' + max + '!',
                        false,
                        null,
                        { text: "Close" });
        e.currentTarget.value = oldVal;
        return false;
    }
}

function verifyNumLimits(e, min, max, oldVal)
{
    var str = e.currentTarget.value;
    var n = Number(str);
    if(String(n) === str && n >= min && n <= max)
    {
        return true;
    }
    else
    {
        fileBrowser.showInputDialog('Value should be between ' +  min + ' - ' + max + '!', 
                        false, 
                        null,
                        {text: "Close"});
        e.currentTarget.value = oldVal;
        return false;
    }
}

function validateIPaddress(e, oldIP)   
{  
    var IPRegex=new RegExp("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    
    if(!IPRegex.test(e.currentTarget.value))
    {
        fileBrowser.showInputDialog('You have entered an invalid IP address!', 
                        false, 
                        null,
                        {text: "Close"});
        e.currentTarget.value = oldIP;
        return false;
    }
    
    return true;
}  

function verifyMacAddr(e, oldVal)
{
    
    var MACAddress = e.currentTarget.value;
    var MACRegex=new RegExp("^(?!(?:ff:ff:ff:ff:ff:ff|00:00:00:00:00:00))([0-9a-fA-F][0-9a-fA-F]:){5}([0-9a-fA-F][0-9a-fA-F])$");
    if(!MACRegex.test(MACAddress))
    {
        fileBrowser.showInputDialog('Wrong Mac Address format. Should be "01:23:45:67:89:AB"\n "00:00:00:00:00:00" and "ff:ff:ff:ff:ff:ff" are not required', 
                        false, 
                        null,
                        {text: "Close"});
        e.currentTarget.value = oldVal;
        return false;
    }
    return true;
}

function displayError(text){
	var html='<div class="white-popup-small">'
	html +='	<div class="wrapPopup">';
	html +='		<div class="filePathN">Error:<span>'+text+'</span></div>';
	html +='		<div class="mbot25"></div>';
	html +='		<div class="wrapDRS floatLeft">';
	html +='			<div class="fieldName"></div>';
	html +='		</div>';	
    html +='		<div class="clear" />';
	html +='		<div class="wrapButtons">';
	html +='			<div class="regButton mCenterM" onClick="$.magnificPopup.close();">Ok</div>';
	html +='		</div>';
	html +='	</div>';
	html +='	<div class="clear" />';
	html +='</div>';
	
	
	$.magnificPopup.open({
		items: {
			src:html,
			type: 'inline'
		}
	});
}

function updateConnectionStatus(data) {
    //connect status
    $('#deviceStatusBox').empty();
    $('#deviceStatusBox').append('<div class="uStatus"><div class="uText jslink"><div class="uIcon"><i class="fa fa-link"></i></div>Connected: <span id = "devConnect">On</span></div></div>');
    //device
    var securedText = "";
    var secured = false;
    if (data.ok.chip_rev < 0x20 && data.ok.device == "CC3200") {
        securedText = "Preproduction Secured";
        secured = true;
    }else if (data.ok.secure) {
        securedText = "Secure";
        secured = data.ok.secure;
    } else {
        securedText = "Non-Secure";
        secured = false;
    }
    
    if (data.ok.dev_mode) {
        currentMode = "Development"
    } else {
        currentMode = "Production"
    }
    
    if (data.ok.dev_not_formatted) {
        currentMode = "No file system"
    }
    
    if (data.ok.fs_locked) {
        currentMode = "File System Locked"
    }


    
   // $('#deviceStatusBox').append('<div class="uStatus">');
    $('#deviceStatusBox').append('<div class="uText"><div class="uIcon"><i class="fa fa-star" ></i></div>Device Type: <span class="mf20">'+ data.ok.device+ ', ' + securedText + '</span>');
    $('#deviceStatusBox').append('</div>');
    //MAC Address
    $('#deviceStatusBox').append('<div class="uStatus">');
    $('#deviceStatusBox').append('<div class="uText"><div class="uIcon"><i class="fa fa-arrow-circle-o-right"></i></div>MAC Address: <span id = "devMacAddr">'+data.ok.mac_address+'</span></div>');
    $('#deviceStatusBox').append('</div>');
    //hardware
    $('#deviceStatusBox').append('<div class="uStatus">');
    $('#deviceStatusBox').append('<div class="uText"><div class="uIcon"><i class="fa fa-briefcase"></i></div>HW Version: <span>'+ data.ok.chip_rev+'</span></div>');
    $('#deviceStatusBox').append('</div>');
    //programing
    $('#deviceStatusBox').append('<div class="uStatus">');
    if(data.ok.secure && data.ok.secure === true){
        $('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-code"></i></i></div>Programming Status: <span>On</span></div>');
    }
    else{
        $('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-code"></i></i></div>Programming Status: <span>On</span></div>');
    }
    $('#deviceStatusBox').append('</br>');
    $('#deviceStatusBox').append('</div>');
    
    
    $('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-code"></i></i></div>Current Mode: <span id = currentModeId>' + currentMode + '</span></div>');
    
    $('#deviceStatusBox').append('</br>');
    
    var realCapacity = (data.ok.capacity) ? (data.ok.capacity * 1024) + "KB" : "N/A";
    $('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-database"></i></i></div>Storage Capacity: <span id = realStorageSizeId>' + realCapacity + '</span></div>');
    
    $('#deviceStatusBox').append('</br>');
    var formattedCapacity = (data.ok.storage_size) ? data.ok.storage_size + "KB" : "N/A"
    $('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-database"></i></i></div>Formatted Capacity: <span id = storageSizeId>' + formattedCapacity + '</span></div>');
    $('#deviceStatusBox').append('</br>');
    $('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-database"></i></i></div>Available for User Files: <span id = actualUserFilesSizeId>' + data.ok.available_user_files_size + 'KB</span></div>');
    
    $('#deviceStatusBox').append('</br>');
    
    //$('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-database"></i></i></div>SFLASH manufacturer code: <span id = jdecManu>' + data.ok.jdec[0] + '</span></div>');
    //$('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-database"></i></i></div>SFLASH model code: <span id = jdecModel>' + data.ok.jdec[1] + '</span></div>');    
    $('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-database"></i></i></div>SFLASH codes: <span id = sflashCodes>' + data.ok.sflash_codes + '</span></div>');    
    
    $('#deviceStatusBox').append('</br>');
    
    $('#deviceStatusBox').append('<div class="uText jslink"><div class="uIcon"><i class="fa fa-exclamation"></i></i></div>Security Alerts: <span id = alertsId>' + data.ok.alerts[0] + " / " +  data.ok.alerts[1] + '</span></div>');
    
    $('#deviceStatusBox').append('</div>');

    $('#connectSpan').text("Disconnect");
     
    //try to update the general setting page
    $('select#3200Select').val(secured ? "2" : "1");
    project.header.IsTheDeviceSecure = secured;
    
    $('select#ProductionNWP').val(data.ok.z_device ? "0" : "1");
    project.header.ProductionNWP = data.ok.z_device ? "0" : "1";
    
    if (data.ok.capacity) {
        project.header.StorageCapacityBytes = (data.ok.capacity * 1024 * 1024).toString();
        $('select#capacitySelect').val(data.ok.capacity * 1024 * 1024);
    }

    //set connected as true
    localStorage.setItem('isconnected', 'true'); 
    
    try {
        loadMode(false);
    } catch (e) {
        
    }
    
    changeCapacity(false);
    changeSecureDevice(false);
    changeProductionDevice(false);

    changeFactorySOP(false);
    changeFactoryConfig(false);
    
    loadMacAddress();
    loadDevelopeMacAddress();
    changeDevelopeMacAddress(false);
    updateBrowseBtn();
    
    saveProjectAPI();
    
    progressModalClose();
}

function connectDeviceAPI(disconnect, done, fail) {
    if ($('#connectSpan').text() == "Disconnect" || disconnect) {
        $('#deviceStatusBox').empty();
        
        $('#deviceStatusBox').append('<div class="uStatus"><div class="uText jslink"><div class="uIcon"><i class="fa fa-unlink uRed" ></i></div>Connected: Off</div></div>');

        $('#connectSpan').text("Connect");
        
        localStorage.setItem('isconnected', 'false');
        
        //changeMode(false);
        try {
            loadMode(false);
        } catch (e) {
            
        }
        
        changeCapacity(false);
        changeSecureDevice(false);
        changeProductionDevice(false);
        
        changeFactorySOP(false);     /*! place important*/
        changeFactoryConfig(false); /*! place important*/
        loadMacAddress();
        updateBrowseBtn();
        
        
        $.post("/api/1/flash/disconnectDevice", {})
            .done(function(data) {
                if (done) {
                    done();
                }
            })
            .fail(function(xhr, textStatus, errorThrown) {
                if (done) {
                    done(); // just do it
                }
            });

        saveProjectAPI();
        
        return;
    }
    
    //update status of key source file name
    if($('#KEY_FILE').val() != undefined)
        project.header.KeyFileLocation = $('#KEY_FILE').val();
    
    //$('#deviceStatusBox').append('<div class="uStatus"><div class="uText jslink"><div class="uIcon"><i><img id="progStatusImgJslink" src="images/ajax-loader.gif"/></i></div>Connecting...</div></div>');
    progressModalOpen("Connecting, please wait");
   
    $.post("/api/1/flash/connectDevice",   {"com_port": null})
        .done(function(data) {
            if (data && data.ok) {
                getProgressStatus(100, updateConnectionStatus);
            } else {
                $('#progStatusImg').hide();
                $('#progStatusBtn').text("Close");
                $('#progStatus').text("Error: " + ((data && data.err) ? data.err : "unknown"));
            }
        })
        .fail( function(xhr, textStatus, errorThrown) {
            $('#progStatusImg').hide();
            $('#progStatusBtn').text("Close");
            $('#progStatus').text("Error: disconnected");
        });

}

function progressModalOpen(text, showBar){
    var html='<div class="white-popup-small">'
	html +='	<div class="wrapPopup">';
	html +='		<div class="center-block"><img class="mright10" id="progStatusImg" src="images/ajax-loader.gif" ><span id="progStatus">'+text+'<span></div>';
    //html +='        <div class="center-block"><img class="center-block" id="progStatusImg" src="images/ajax-loader.gif"></div>';
    
    if (showBar)
        html +='        <div><div class="center-block progress"><div id="progStatusBar" class="progress-bar"></div></div>';
        
	html +='		<div class="wrapButtons">';
	html +='			<div id="progStatusBtn" class="regButton mleft59">Abort</div>';
	html +='		</div>';
	html +='	</div>';
	html +='	<div class="clear" />';
	html +='</div>';
    
    $.magnificPopup.open({
        items: {
        src:html,
        type: 'inline',
		},
        
        modal: true,
	});
    
    $('#progStatusBtn').on("click", function(e) {
        if ($('#progStatusBtn').text() == "Abort") {
            $.post("/api/1/flash/prog_abort", {}, function(data, status) {
                if (status == "success" && data.ok) {
                        $('#progStatus').text("Aborted");
                } else {
                    if (data.err)
                        $('#progStatus').text("Abort error: " + data.err);
                    else
                        $('#progStatus').text("Abort error: disconnect");
                }
            });
        } else if ($('#progStatusBtn').text() == "Close") {
            progressModalClose();
            loadGenerateImage();
            if((localStorage.getItem('isconnected') == undefined) || (localStorage.getItem('isconnected') == 'false'))
            {
                $('#deviceStatusBox').empty();
                $('#deviceStatusBox').append('<div class="uStatus"><div class="uText jslink"><div class="uIcon"><i class="fa fa-unlink uRed" ></i></div>Connected: Off</div></div>');
            }
        }
    });
}

function progressModalClose(noClose) 
{    
    //update data
    try {
        changeCapacity();
    } catch (e) {
        console.log(e);
    }
    try {
        changeSecureDevice();
    } catch (e) {
        console.log(e);
    }
    try {
        changeFactorySOP();
    } catch (e) {
        console.log(e);
    }
    try {
        changeFactoryConfig();
    } catch (e) {
        console.log(e);
    }
    try {
        changeProductionDevice();
    } catch (e) {
        console.log(e);
    }
    
    if (noClose) {
        $('#progStatusImg').hide();
        $('#progStatus').text("Done")
        $('#progStatusBtn').text("Close");
    } else {
        $.magnificPopup.close();
    }
}

function getProgressStatus(timeOut, callOnDone, noClose, fail) {
    if (timeOut === undefined)
        timeOut = 100
        
    $.post("/api/1/flash/getProgStatus", {})
        .done(function(data) {
            if (data) {
                if (data.ok || data.err) {                    
                    if (data.ok) {
                        $('#progStatus').text((typeof(data.ok) == "string") ? data.ok : "Operation Completed Successfully");
                        if (callOnDone)
                            callOnDone(data)
                    }                 
                    else if (data.err) {
                        $('#progStatus').css({'color': 'red'});
                        $('#progStatus').css("font-weight","Bold");
                        $('#progStatus').text("Operation failed: " + data.err);
                        $('#progStatusImg').hide();
                        $('#progStatusBtn').text("Close");
                        
                        if (fail) {
                            fail(data);
                        }
                    }
                    
                    if (!noClose) {
                        $('#progStatusImg').hide();
                        $('#progStatusBtn').text("Close");
                    }
                } else {
                    
                    if (data.prog_status.need_ok) {
                        fileBrowser.showInputDialog(data.prog_status.text, 
                        false, 
                        {text: "Program", func: function() { 
                            $.post("/api/1/flash/userOk", {})
                                .done(function(data) {
                                    progressModalOpen("Starting...", true);
                                    window.setTimeout(function() { getProgressStatus(timeOut, callOnDone, noClose, fail) }, timeOut);
                                })
                                .fail(function(data) {
                                    $('#progStatus').css({'color': 'red'});
                                    $('#progStatus').css("font-weight","Bold");
                                    $('#progStatus').text("Operation failed: " + data.err);
                                    $('#progStatusImg').hide();
                                    $('#progStatusBtn').text("Close");
                                });
                        }},
                        {text: "Abort", func: function() {
                            $.post("/api/1/flash/prog_abort", {})
                                .done(function(data) {
                                    
                                })
                                .fail(function(data) {
                                    $('#progStatus').css({'color': 'red'});
                                    $('#progStatus').css("font-weight","Bold");
                                    $('#progStatus').text("Operation failed: " + data.err);
                                    $('#progStatusImg').hide();
                                    $('#progStatusBtn').text("Close");
                                });
                            }
                        });
                        return
                        
                    }
                    
                    $('#progStatus').text(data.prog_status.text);
                    var progress = ((data.prog_status.number * 1.0) / data.prog_status.total * 100) + "%";
                    try {
                            if($('#progStatusBar').prop("style"))
                            {
                                $('#progStatusBar').prop("style").width = progress;
                            }
                    } catch (err) {
                    }
                    
                    window.setTimeout(function() { getProgressStatus(timeOut, callOnDone, noClose, fail) }, timeOut);
                }
            } else {
               
            }
        })
        .fail( function(xhr, textStatus, errorThrown) {
            $('#progStatusImg').hide();
            $('#progStatusBtn').text("Close");
            $('#progStatus').text("Error: disconnected");
        });
}

function programImageFromProject() {
    progressModalOpen("Starting...", true);
    connectDeviceAPI(true, function() {
        $.post("/api/1/flash/programImageFromProject", {name: projectName, com_port: null})
            .done(function(data) {
                if (data && data.ok) {
                    getProgressStatus(100);
                } else {
                    $('#progStatusImg').hide();
                    $('#progStatusBtn').text("Close");
                    $('#progStatus').text("Error: " + ((data && data.err) ? data.err : "unknown"));
                }
            })
            .fail( function(xhr, textStatus, errorThrown) {
                $('#progStatusImg').hide();
                $('#progStatusBtn').text("Close");
                $('#progStatus').text("Error: disconnected");
            });
    });
}

function programImageFromSLI(sliFile, keyFile, use2btldr, use_otp) {
    var formData = new FormData();
    
    if (false) // should be user selected com port in the future
        formData.append("com_port", null);
                
    if (sliFile)
        formData.append("sli_file", sliFile);

    if (keyFile)
        formData.append("key_file", keyFile);

    if (use2btldr)
        formData.append("use2btldr", use2btldr);

    if (use_otp)
        formData.append("use_otp", use_otp);


        
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                getProgressStatus(100)
            }
            else if (data && data.err) {
                $('#progStatusImg').hide();
                $('#progStatusBtn').text("Close");
                $('#progStatus').text("Error: " + data.err);
            }
        } else if (xhr.status > 200) {
            $('#progStatusImg').hide();
            $('#progStatusBtn').text("Close");
            $('#progStatus').text("Error: disconnected"); 
        }
    }
    
    xhr.upload.onprogress = function(e) {
        $('#progStatus').text("Preparing SLI file");
        if (e.lengthComputable) {
            $('#progStatusBar').prop("style").width = ((e.loaded / e.total) * 100) + "%";
        }
    }
    
    progressModalOpen("Starting...", true);
    
    xhr.open("POST", "/api/1/flash/programImageFromSLI", true);
    xhr.send(formData);
}

function loadWelcome()
{
    window.location.href = "/Welcome.htm";
    
    //$('#toolssignfile').remove(backBtn); 
}

function loadGenerataImage()
{
    displayMainContent('programimageconnected');
}

function loadToolsWelcome()
{
    toggleFullOverlay($("#mainContent"), true);
    
    $('#backToolsWelcome').show();
    
    localStorage.setItem('tools' , 'noproject');
}

function loadTools()
{
    toggleFullOverlay($("#mainContent").css("width", "550px"), false);

    displayMainContent('toolssignfile');
    
    $('#backToolsWelcome').hide();
    
    localStorage.setItem('tools' , 'project');
}

function enableSetProjImage(ext)
{
    $('#setProjImage').removeClass('btnDisabled')
    $('#setProjImage').off('click').on('click', setProjImageClicked);
   
    if(!ext){
        $('#keyImgFileInput').val($('#IMG_FILE').val().replace(/^.*[\\\/]/, ''));    
        $('#keyImgKeyFileInput').val($('#PROJ_IMAGE_KEY_FILE_NAME').val().replace(/^.*[\\\/]/, ''));
    }
}
function changeProjImgSecBtldr() {
    if ($('#PROJ_IMAGE_USE_SEC_BTLDR_1').is(':checked')) {
        $('#fieldsetProjImgOTPOut').show();
        changeProjImgOtpFile();
    }
    else {
        $('#fieldsetProjImgOTPOut').hide();
    }

    
}
function changeProjImgOtpFile() {
    if ($('#PROJ_IMAGE_USE_OTP_1').is(':checked')) {
        $('#fieldsetProjImgOTPInn').show();
    }
    else {
        $('#fieldsetProjImgOTPInn').hide();
    }
}
function loadOpenProjImg()
{
    $('#PROJ_IMAGE_USE_SEC_BTLDR_1').unbind('change');
    $('#PROJ_IMAGE_USE_SEC_BTLDR_1').bind('change', changeProjImgSecBtldr);

    $('#PROJ_IMAGE_USE_OTP_1').unbind('change');
    $('#PROJ_IMAGE_USE_OTP_1').bind('change', changeProjImgOtpFile);



    $('#fieldsetProjImgOTPOut').hide();


    if (document.getElementById('IMG_FILE').innerHTML == "")
    {
        $('#setProjImage').addClass('btnDisabled').off('click');
    }

}

function openCreateImageBrowse()
{
    $('#browseKeyImageFile').click();
}

function toggleFullOverlay(activeDiv, enable) {
    var ol = $("#fullOverlay")
    
    if (enable) {
        activeDiv.css("z-index", 200);
        ol.css({ "z-index": 100, "opacity": 0.8 })
        ol.width("100%");
    } else {
        activeDiv.css("z-index", "auto");
        ol.css({ "z-index": 0, "opacity": 0 })
        ol.width("0%");
    }
}

function browse()
{   
    displayMainContent('filesuserfilesonline');
}

function updateBrowseBtn()
{
//if(project){if(project.header){if(project.header.Mode){
    if(localStorage.getItem('isconnected') != 'true'){//disconnected
        $('#BrowseBtn').addClass('btnDisabled').off('click');
        
        //if ($("h1.mainTitle").text() == "Files > Device File Browser") {
        if (gLoadedPage == "filesuserfilesonline") {
            displayMainContent("generalsettings");
        }
    }
    else{
        //if('development' == project.header.Mode){
        if("Development" == $('#currentModeId').html()){
            //enable browse button
            $('#BrowseBtn').removeClass('btnDisabled');
            $('#BrowseBtn').off("click").on('click', browse);
        }
        else {
            $('#BrowseBtn').addClass('btnDisabled').off('click');
            
            if ($("h1.mainTitle").text() == "Files > Device File Browser") {
                displayMainContent("generalsettings");
            }
        }
    }
//}}}
}

function progressModalError(text, done) {
    $('#progStatusImg').hide();
    $('#progStatusBtn').text("Close");
    $('#progStatus').text("Error: " + text);
    
    if (done) {
        done();
    }
}
////////////// OTA ////////////////

function saveOTAPrivateKey(isSecured) {

    if( $('#userOtaKeyFileInput').val().length == 0 ) {
        // empty key
      
        if ( isSecured == true ) {
          alert("Error - cannot create secured OTA without key");
          return;
        }
      
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
        if (xhr.status == 200 && xhr.readyState == 4) {
           var data = JSON.parse(xhr.responseText);
           if (data && data.ok) {
        
           }
           else if (data && data.err) {
               alert(data.err);
           }
           else {
               alert("error upload/delete file");
           }
         }
        }
            
        var formData = new FormData();
        formData.append("name", projectName);
        formData.append("source", "");
        formData.append("dest", "/"+"");
        
        xhr.open("post", "/api/1/flash/uploadOTAPrivateKey", true);
        xhr.send(formData);
    } else {
     
       if ( isSecured == false ) {
          alert("To skip, remove key or create secured OTA");
          return;
       }
    }    
    
    
    $.magnificPopup.close();
    progressModalOpen("Creating OTA", false);

    $.post("/api/1/flash/createBasicOTA", {name: projectName})
        .done(function(data) {
            if (data && data.ok) {
                setTimeout(function() {
                    progressModalClose();
                    saveBasicOTA();
                }, 1000);
            } else if (data && data.err) {
                progressModalError("Create Basic OTA Error:" + data.err);
            } else {
                progressModalError("Create Basic OTA Error");
            }
        })
        .fail(function(data) {
            progressModalError("Create Basic OTA Error");
        });

}


//upload/delete user signature  file
function uploadOTAPrivateKeyBase()
{  
    var apiFuncPath = '';
    if($('#USER_OTA_KEY_FILE'))
    {
        var spf = document.getElementById('USER_OTA_KEY_FILE');
        $('#userOtaKeyFileInput').val(spf.value.replace(/^.*[\\\/]/, ''));
        apiFuncPath= "/api/1/flash/uploadOTAPrivateKey";
   
        // if validity checks are done and failed we will pass  $('#userOtaKeyFileInput').val('');
        uploadSignature(spf.files, "", apiFuncPath);
    }
}
function setAddclstOta() {

    project.header.AddClstOta = $('#ADD_CLST_OTA_1').is(':checked');
    saveProjectAPI();
}

function createBasicOTA() {
    project.header.AddClstOta = false;
    saveProjectAPI();
    // Before starting OTA we need to ask for private key and send it to server
    // this key will be used to encrypt ota.cmd digest
    var html = '<div class="white-popup-small">';
    html += '			<div class="wrapDRS">';
    html += '			<input type="checkbox" id="ADD_CLST_OTA_1" onChange = "setAddclstOta();" data-tooltip="Create OTA bundle with certificate catalog ONLY! Certificate catalog is used in the signature validation process and cannot be updated along with other files." >Certificate catalog OTA bundle only.</>';
    html += '		    <div class="clear"></div>';
    html +='			</div>';
   	html += '		<div class="mbot25 clear"></div>';
   	html += '			<div class="wrapDRS">';
    html +='			<div class="fieldName">OTA Private Key File Name(pem/der):</div>';
	html +='				<div class="floatLeft">';
	html +='					<input type="text" id="userOtaKeyFileInput">';
	html +='				</div>';
	html +='				<div class="regButton mtop0 mLeftS" onClick="$(\'#USER_OTA_KEY_FILE\').click();">';
	html +='					<id="browseUserOtaKeyFile">';
    html +='					Browse';
	html +='				</div>';
	html +='				<div class="hideFile" onChange = "uploadOTAPrivateKeyBase();">';
	html +='					<input type="file" id="USER_OTA_KEY_FILE">';
	html +='				</div>';
	html +='			</div>';
    html +='		<div class="clear"></div>';
    html +='		<div class="regButton" onClick="saveOTAPrivateKey(true);" >Create OTA</div>&nbsp;&nbsp;&nbsp;&nbsp;';
    html +='		<div class="regButton" style="margin-left: 20px;" onClick="saveOTAPrivateKey(false);" >Skip Security</div>&nbsp;&nbsp;&nbsp;&nbsp;';
	//html +='		<div class="regButton" style="margin-left: 20px;" onClick="$.magnificPopup.close();">Cancel</div>';
	html +='		<div class="clearM  mbot25"></div>';
	html +='</div>';	
	
	$.magnificPopup.open({
		items: {
			src:html,
			type: 'inline',
		}			
	});

}

function saveBasicOTA() {
    $('#lastImageLink').html('<a id="lastImageLinkPath" href="/api/1/flash/getBasicOTA?x=' + new Date().getTime() + '&r=' + Math.random() + '"</a>');
    $('#lastImageLinkPath')[0].click();
    //document.getElementById('lastImageLinkPath').click();
}
////// End of OTA /////////////////////

function decryptImage() {
    var keyFile = $("#TOOLS_PROJ_IMAGE_KEY_FILE_NAME")[0];
    if (keyFile.files.length) {
        keyFile = keyFile.files[0];
    } else if (g_tools_img_dec_key_file) {
        keyFile = g_tools_img_dec_key_file;
    } else {
        fileBrowser.showInputDialog('No key file selected', 
                    false, 
                    null,
                    {text: "Close"});
            return;
    }
    
    var formData = new FormData()
    formData.append("key_file", keyFile)
    
    progressModalOpen("Activating Image", false);
        
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                getProgressStatus(100, null);
            } else if (data && data.err) {
                progressModalError("Image Activating Error:" + data.err);
            } else {
                progressModalError("Image Activating Error");
            }
        }
    }
        
    xhr.open("post", "/api/1/flash/activateImage", true);
    xhr.send(formData);    
}

///////////////     CSR    /////////////////////////
function getCSR()
{
   // $.magnificPopup.close();
    progressModalOpen("Getting CSR file", false);

    $.post("/api/1/flash/getCSR", { "com_port": null })
        .done(function (data) {
            if (data && data.ok) {
                setTimeout(function () {
                    progressModalClose();
                    saveCSR();
                }, 1000);
            } else if (data && data.err) {
                progressModalError("Get CSR Error:" + data.err);
            } else {
                progressModalError("Get CSR Error");
            }
        })
        .fail(function (data) {
            progressModalError("Get CSR Errorr");
        });
}

function saveCSR() {
    $('#lastImageLinkCSR').html('<a id="lastImageLinkCSRPath" href="/api/1/flash/saveCSR?x=' + new Date().getTime() + '&r=' + Math.random() + '"</a>');
    $('#lastImageLinkCSRPath')[0].click();
}


function setCSR()
{
    var csrFile = $("#TOOLS_SIGNED_CSR_FILE_NAME")[0];
    if (csrFile.files.length) {
        csrFile = csrFile.files[0];
    } else if (g_tools_csr_file) {
        csrFile = g_tools_csr_file;
    } else {
        fileBrowser.showInputDialog('No input file selected', 
                    false, 
                    null,
                    {text: "Close"});
        return;
    }
    
    var formData = new FormData()
    formData.append("source", csrFile)
    
    progressModalOpen("Setting certificate file", false);
        
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.status == 200 && xhr.readyState == 4) {
            var data = JSON.parse(xhr.responseText);
            if (data && data.ok) {
                getProgressStatus(100, null);
            } else if (data && data.err) {
                progressModalError("Setting certificate file:" + data.err);
            } else {
                progressModalError("Setting certificate file");
            }
        }
    }
        
    xhr.open("post", "/api/1/flash/setCSR", true);
    xhr.send(formData);    

}
////////////// End of CSR /////////////////////
function projectManagementLoad(){
    toggleFullOverlay($("#mainContent").css("width", "611px"), true);
    //toggleFullOverlay($("#mainContent"), true);
    var div = $("#projectList");
    var pl = new projectList.ProjectList(div);
    pl.load(true);
	
}

function openProject(projectname) {
    localStorage.setItem('projectName', projectname);
    window.location.href = '/index.htm?page=simplemode';
    //$('#idmenugeneral0').css('color', 'red');
}
/*
function openProject(projectname) {
    try {
        $.post("/api/1/flash/openProject", { 'name': projectname },
			function (data, status) {
			    //if return sucess and has data
			    if (status == 'success' && data.ok) {
			        //save on localStorage project name
			        localStorage.setItem('projectName', projectname);
			        //window.location.href ='/index.htm?page=programimageconnected';
			        window.location.href = '/index.htm?page=simplemode';
			    }
			    else if (data.err) {
			        alert(data.err);
			    }

			});
    }
    catch (e) { }
}
*/
function verifyDeviceName(e, oldVal)
{
    var str = e.currentTarget.value;
    
    var msg = "Invalid device name: "
    var regexpression = "[^a-zA-Z0-9\\-]+";
    /*TBD!!*/
    if (project.header.IsGen3) {
        regexpression = "[^\u00A1-\uFFFFa-zA-Z0-9\\-]+";
    }
    if (new RegExp(regexpression).test(str)) {
        msg += 'may not contain anything other than letters, digits and dashes (-)';
    } else if (str.trim() === "") {
        msg += 'may not be empty or made out of spaces';
    } else if (str.length > 32) {
        msg += 'may not be longer than 32 characters (currently: ' + str.length + ")";
    } else {
        return true;
    }

    fileBrowser.showInputDialog(msg, 
            false, 
            null,
            {text: "Close"});

    e.currentTarget.value = oldVal;
    return false;
}

function verifyProjectName(projectName) {
    if (projectName.trim() === "" || projectName.indexOf(" ") == 0 || projectName.lastIndexOf(" ") == projectName.length - 1) {
        return {err: "Error: Project Name cannot be: empty, made out of spaces only, start with space/end with space"};
    }

    if (new RegExp("[^a-zA-Z0-9_\\-\\. ]+").test(projectName)) {
        return {err: "Error: Project Name must not contain any characters other than: letters, digits, underscores (_), dashes (-), dots (.) and internal spaces"};
    }

    if (projectName.length > 64) {
        return {err: "Error: Project Name length must be between 1 and 64 (currently: " + projectName.length + ")"};
    }

    return {ok: ""};
}

function clearEncrKeyFile() {
    progressModalOpen("Deleting file, please wait...");
    
    $.post("/api/1/flash/deleteProjectKSFile", {name: projectName})
        .done(function(data) {
            if (data && data.ok) {
                project.header.KeyFileLocation = '';
                $('#keyFileInput').val('');
                saveProjectAPI();
                setTimeout(progressModalClose, 500);
            } else if (data && data.err) {
                progressModalError("Error deleting file: " + data.err);               
            } else {
                progressModalError("Error deleting file");               
            }
        })
        .fail(function(data) {
            progressModalError("Error deleting file");
        });
}

function btnRegDomain24Open() {
    displayMainContent("regulatorydomain24");
}

function regulatoryDomain24Load() {
    toggleFullOverlay($("#mainContent").css("width","1600px"), true);
    loadRegulatoryDomain24();
}

function btnRegDomainOpen() {
    displayMainContent("regulatorydomain");
}

function regulatoryDomainLoad() {
    toggleFullOverlay($("#mainContent").css("width", "1400px"), true);
    loadRegulatoryDomain();
}

function loadRFSettings() {
    toggleFullOverlay($("#mainContent").css("width", "611px"), false); /*width to set back after expand to 2.4 advanced Tx Power tables*/
    displayMainContent("systemsettingsdeviceRadioSettings");
}

function clearProjectFile(func_name, jq_obj, num) {
    progressModalOpen("Deleting file, please wait...");

    $.post(func_name, { name: projectName })
        .done(function (data) {
            if (data && data.ok) {
                jq_obj.val('');
                switch (num) {
                    case 0: project.systemFiles.CONFIG_TYPE_HTTP_SRV.PRIVATEKEY_FILE_NAME = "";
                        break;
                    case 1: project.systemFiles.CONFIG_TYPE_HTTP_SRV.CERTIFICATE_FILE_NAME = "";
                        break;
                    case 2: project.systemFiles.CONFIG_TYPE_HTTP_SRV.CA_CERTIFICATE_FILE_NAME = "";
                        break;
                    case 13: project.systemFiles.FILES.OTP_NAME = "";
                        break;
                }
                saveProjectAPI();
                setTimeout(progressModalClose, 500);
            } else if (data && data.err) {
                progressModalError("Error deleting file: " + data.err);
            } else {
                progressModalError("Error deleting file");
            }
        })
        .fail(function (data) {
            progressModalError("Error deleting file");
        });
}
/*-------------------------------------------DICE--------------------------------------------*/
function ShowDice()
{
    if ((project.header.IsGen3 == true) && (project.header.DeviceType.indexOf('CC31') == -1)) /*3100*/{
        $('#idmenuDiceSettings').show();
        $('#idmenuDiceSettings1').show();
        $('#idmenuDiceSettings2').show();
    }
    else {
        $('#idmenuDiceSettings').hide();
        $('#idmenuDiceSettings1').hide();
        $('#idmenuDiceSettings2').hide();
    }
}

function updateDiceGui() {

    project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.ENABLE_DICE = $('#ENABLE_DICE').val();

    if (project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.ENABLE_DICE == 0) //Disabled
    {
        if (0 == $('#CREATE_CSR option[value=2]').length)
        {
            $('<option>').val('2').text('Disable').appendTo('#CREATE_CSR');
        }
        
        $('#CSR_ISCA').prop("disabled", false);
        $('#randomNumDiceTable').hide();
        $('#csrVendorDiv').hide();
        $('#csrTokenDiv').hide();
        
    }
    else //Enabled
    {
        $('#CSR_ISCA').prop("disabled", true);
        $('#CSR_ISCA').val('true');
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_ISCA = true;
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CREATE_CSR           = true;
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.USE_SELF_SIGNED_CERT = false;

        $("#CREATE_CSR option[value='2']").remove();
        $('#randomNumDiceTable').hide(); // hidden by sdk request - still useless
        $('#csrVendorDiv').show();
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_VENDOR ? $('#csrTokenDiv').show() : $('#csrTokenDiv').hide();
        //$('#csrTokenDiv').show();

    }
    saveProjectAPI();

    updateCSRGUI();
}

function updateUDID()
{
    project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_USE_UDID_AS_COMMON_NAME = $('#UDID_NAME_0').is(':checked'); 
    saveProjectAPI();

    $('#csrNameText').prop("disabled", project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_USE_UDID_AS_COMMON_NAME);

}

function loadDice() {
    toggleFullOverlay($("#mainContent").css("width", "611px"), false);
    $('#ENABLE_DICE').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.ENABLE_DICE);

    if (project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CREATE_CSR) {

        if (project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.USE_SELF_SIGNED_CERT) {
            $('#CREATE_CSR').val(1);
        } else {
            $('#CREATE_CSR').val(0);
        }
        

    } else {
        $('#CREATE_CSR').val(2);
    }

    ShowCSRDateWarning(false);

    {
        $('#RANDOM_NUM_DICE_1').prop('checked', project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.USE_RAND_NUMBER);
        $('#RANDOM_NUM_DICE_1').unbind('change');
        $('#RANDOM_NUM_DICE_1').bind('change', function (e) { project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.USE_RAND_NUMBER = $('#RANDOM_NUM_DICE_1').is(':checked'); saveProjectAPI(); });
    }//RANDOM_NUM_DICE_1

    {
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_VENDOR && project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.ENABLE_DICE ? $('#csrTokenDiv').show() : $('#csrTokenDiv').hide();
        $('#CSR_VENDOR_1').prop('checked', project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_VENDOR);
        $('#CSR_VENDOR_1').unbind('change');
        $('#CSR_VENDOR_1').bind('change', function (e) {
            project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_VENDOR = $('#CSR_VENDOR_1').is(':checked');
            project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_VENDOR ? $('#csrTokenDiv').show() : $('#csrTokenDiv').hide();
            saveProjectAPI();
        });
    }//CSR_VENDOR_1

    {
        $('#CSR_TOKEN').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_TOKEN);
        $('#CSR_TOKEN').unbind('change');
        $('#CSR_TOKEN').bind('change', function (e) {
            if (verifyNumLimits(e, 0, 4294967295, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_TOKEN)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_TOKEN = $('#CSR_TOKEN').val(); saveProjectAPI();
            }
        });
    } //CSR_TOKEN

    {
        $('#UDID_NAME_0').prop('checked', project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_USE_UDID_AS_COMMON_NAME);
        $('#UDID_NAME_0').unbind('change');
        $('#UDID_NAME_0').bind('change', updateUDID);
    }//UDID_NAME_0

    {
        $('#csrCertSerialNameText').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_CERT_NUMBER);
        $('#csrCertSerialNameText').unbind('change');
        $('#csrCertSerialNameText').bind('change', function (e) {
            if (verifyNumLimits(e, 0, 99999999, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_CERT_NUMBER)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_CERT_NUMBER = $('#csrCertSerialNameText').val(); saveProjectAPI();
            }
        });
    } //csrCertSerialNameText
    {
        {
            $('#csr_starts_year').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR);
            $('#csr_starts_year').unbind('change');
            $('#csr_starts_year').bind('change', function (e) {
                if (verifyNumLimits(e, 2013, 2037, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR)) {
                    if (!validateStartDate($('#csr_starts_year').val(), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY)) {    
                        $('#csr_ends_year_select').next().val("");
                        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR = "";

                        ShowCSRDateWarning(true);
                    }
                    else { 
                        // $('#csr_starts_year').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR)
                        ShowCSRDateWarning(false);
                    }

                    $('#csr_starts_year').val().length == 1 ? project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR = "0" + $('#csr_starts_year').val() : project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR = $('#csr_starts_year').val();
                    saveProjectAPI();
                }
            });

            $('#csr_starts_year_select').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR);
            $('#csr_starts_year_select').unbind('change');
            $('#csr_starts_year_select').bind('change', function (e) {
                if (!validateStartDate($('#csr_starts_year_select').val(), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY)) {

                    $('#csr_ends_year_select').next().val("");
                    project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR = "";

                    ShowCSRDateWarning(true);


                }
                else {
                    ShowCSRDateWarning(false);
                }

                $('#csr_starts_year_select').next().val($('#csr_starts_year_select').val());
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR = $('#csr_starts_year_select').val(); saveProjectAPI();
            });
        }//csr_starts_year
        {
            $('#csr_starts_month').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH);
            $('#csr_starts_month').unbind('change');
            $('#csr_starts_month').bind('change', function (e) {
                if (verifyNumLimits(e, 1, 12, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH)) {
                    if (! validateStartDate(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR, $('#csr_starts_month').val(), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY)) {
                        $('#csr_ends_month_select').next().val("");
                        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH = "";

                        ShowCSRDateWarning(true);
                    }
                    else { //set old value
                        //$('#csr_starts_month').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH);
                        ShowCSRDateWarning(false);
                    }
                    $('#csr_starts_month').val().length == 1 ? project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH = "0" + $('#csr_starts_month').val() : project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH = $('#csr_starts_month').val();
                    saveProjectAPI();
                }
            });

            $('#csr_starts_month_select').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH);
            $('#csr_starts_month_select').unbind('change');
            $('#csr_starts_month_select').bind('change', function (e) {
                if (!validateStartDate(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR, $('#csr_starts_month_select').val(), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY)) {

                    ShowCSRDateWarning(true);

                    $('#csr_ends_month_select').next().val("");
                    project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH = "";
                } else {

                    ShowCSRDateWarning(false);
                }

                $('#csr_starts_month_select').next().val($('#csr_starts_month_select').val());
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH = $('#csr_starts_month_select').val(); saveProjectAPI();

                setDays($('#csr_starts_day_select'), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR);
            });
        }//csr_starts_month
        {
            $('#csr_starts_day').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY);
            $('#csr_starts_day').unbind('change');
            $('#csr_starts_day').bind('change', function (e) {
                if (verifyNumLimitsDays(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY)) {
                    if (! validateStartDate(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH, $('#csr_starts_day').val())) {
                        $('#csr_ends_day_select').next().val("");
                        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY = "";

                        ShowCSRDateWarning(true);
                    } else {//set old value
                        // $('#csr_starts_day').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY);
                        ShowCSRDateWarning(false);
                    }
                    $('#csr_starts_day').val().length == 1 ? project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY = "0" + $('#csr_starts_day').val() : project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY = $('#csr_starts_day').val();
                    saveProjectAPI();
                }
            });

            setDays($('#csr_starts_day_select'),project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR);

            $('#csr_starts_day_select').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY);
            $('#csr_starts_day_select').unbind('change');
            $('#csr_starts_day_select').bind('change', function (e) {
                if (! validateStartDate(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH, $('#csr_starts_day_select').val())) {
                    ShowCSRDateWarning(true);

                    $('#csr_ends_day_select').next().val("");
                    project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY = "";
                } else {

                    ShowCSRDateWarning(false);
                
                }
                $('#csr_starts_day_select').next().val($('#csr_starts_day_select').val());
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY = $('#csr_starts_day_select').val(); saveProjectAPI();
            });
        }//csr_starts_day
        {
            $('#csr_ends_year').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR);
            $('#csr_ends_year').unbind('change');
            $('#csr_ends_year').bind('change', function (e) {
                if (verifyNumLimits(e, 2013, 2037, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR)) {
                    if (validateEndDate($('#csr_ends_year').val(), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)) {
                        ShowCSRDateWarning(false);
                        $('#csr_ends_year').val().length == 1 ? project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR = "0" + $('#csr_ends_year').val() : project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR = $('#csr_ends_year').val();
                        saveProjectAPI();
                    } else {
                        $('#csr_ends_year').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR);
                    }
                }
            });

            $('#csr_ends_year_select').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR);
            $('#csr_ends_year_select').unbind('change');
            $('#csr_ends_year_select').bind('change', function (e) {
                if (validateEndDate($('#csr_ends_year_select').val(), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)) {

                    ShowCSRDateWarning(false);

                    $('#csr_ends_year_select').next().val($('#csr_ends_year_select').val());
                    project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR = $('#csr_ends_year_select').val(); saveProjectAPI();
                }
            });
        }//csr_ends_year
        {
            $('#csr_ends_month').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH);
            $('#csr_ends_month').unbind('change');
            $('#csr_ends_month').bind('change', function (e) {
                if (verifyNumLimits(e, 1, 12, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH)) {
                    if (validateEndDate(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR, $('#csr_ends_month').val(), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)) {
                        ShowCSRDateWarning(false);
                        $('#csr_ends_month').val().length == 1 ? project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH = "0" + $('#csr_ends_month').val() : project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH = $('#csr_ends_month').val();
                        saveProjectAPI();
                    } else {
                        $('#csr_ends_month').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH);
                    }
                }
            });

            $('#csr_ends_month_select').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH);
            $('#csr_ends_month_select').unbind('change');
            $('#csr_ends_month_select').bind('change', function (e) {
                if (validateEndDate(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR, $('#csr_ends_month_select').val(), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)) {
                    ShowCSRDateWarning(false);
                    $('#csr_ends_month_select').next().val($('#csr_ends_month_select').val());
                    project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH = $('#csr_ends_month_select').val(); saveProjectAPI();

                    setDays($('#csr_ends_day_select'), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR);
                }
            });
        }//csr_ends_month
        {
            $('#csr_ends_day').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY);
            $('#csr_ends_day').unbind('change');
            $('#csr_ends_day').bind('change', function (e) {
                if (verifyNumLimitsDays(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)) {
                    if (validateEndDate(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH, $('#csr_ends_day').val())) {
                        ShowCSRDateWarning(false);
                        $('#csr_ends_day').val().length == 1 ? project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY = "0" + $('#csr_ends_day').val() : project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY = $('#csr_ends_day').val();
                        saveProjectAPI();
                    } else {
                        $('#csr_ends_day').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)
                    }
                }
            });

            setDays($('#csr_ends_day_select'), project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR);

            $('#csr_ends_day_select').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY);
            $('#csr_ends_day_select').unbind('change');
            $('#csr_ends_day_select').bind('change', function (e) {
                if (validateEndDate(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH, $('#csr_ends_day_select').val())) {
                    ShowCSRDateWarning(false);
                    $('#csr_ends_day_select').next().val($('#csr_ends_day_select').val());
                    project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY = $('#csr_ends_day_select').val(); saveProjectAPI();
                }
            });
        }//csr_ends_day

    } //csrValidityStart -End
    {
        $('#CSR_ISCA').val(String(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_ISCA));
        $('#CSR_ISCA').unbind('change');
        $('#CSR_ISCA').bind('change', function (e) {
            project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_ISCA = $('#CSR_ISCA').val() === "false" ? false : true;
            saveProjectAPI();
        });
    } //CSR_ISCA
    {
        $('#csrCountryCodeSelect').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_COUNTRY_CODE);
        $('#csrCountryCodeSelect').unbind('change');
        $('#csrCountryCodeSelect').bind('change', function (e) {
            $('#csrCountryCodeSelect').next().val($('#csrCountryCodeSelect').val());
            project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_COUNTRY_CODE = $('#csrCountryCodeSelect').val(); saveProjectAPI();
        });
    } //csrCountryCodeSelect
    {
        $('#csrCountryCodeInput').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_COUNTRY_CODE);
        $('#csrCountryCodeInput').unbind('change');
        $('#csrCountryCodeInput').bind('change', function (e) {
            if (verifyStringLength(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_COUNTRY_CODE, 2)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_COUNTRY_CODE = $('#csrCountryCodeInput').val().toUpperCase(); saveProjectAPI();
            }
        });
    } //csrCountryCodeInput
    {
        $('#csrStateNameText').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_STATE);
        $('#csrStateNameText').unbind('change');
        $('#csrStateNameText').bind('change', function (e) {
            if (verifyStringLength(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_STATE, 64)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_STATE = $('#csrStateNameText').val(); saveProjectAPI();
            }
        });
    } //csrStateNameText
    {
        $('#csrLocalityNameText').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_LOCALITY);
        $('#csrLocalityNameText').unbind('change');
        $('#csrLocalityNameText').bind('change', function (e) {
            if (verifyStringLength(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_LOCALITY, 64)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_LOCALITY = $('#csrLocalityNameText').val(); saveProjectAPI();
            }
        });
    } //csrLocalityNameText
    {
        $('#csrSurnameNameText').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_SURNAME);
        $('#csrSurnameNameText').unbind('change');
        $('#csrSurnameNameText').bind('change', function (e) {
            if (verifyStringLength(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_SURNAME, 64)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_SURNAME = $('#csrSurnameNameText').val(); saveProjectAPI();
            }
        });
    } //csrSurnameNameText
    {
        $('#csrOrganizationNameText').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_ORGANIZATION);
        $('#csrOrganizationNameText').unbind('change');
        $('#csrOrganizationNameText').bind('change', function (e) {
            if (verifyStringLength(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_ORGANIZATION, 64)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_ORGANIZATION = $('#csrOrganizationNameText').val(); saveProjectAPI();
            }
        });
    } //csrOrganizationNameText
    {
        $('#csrOrganizationUnitNameText').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_ORGANIZATION_UNIT);
        $('#csrOrganizationUnitNameText').unbind('change');
        $('#csrOrganizationUnitNameText').bind('change', function (e) {
            if (verifyStringLength(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_ORGANIZATION_UNIT, 64)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_ORGANIZATION_UNIT = $('#csrOrganizationUnitNameText').val(); saveProjectAPI();
            }
        });
    } //csrOrganizationUnitNameText
    {
        $('#csrNameText').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_NAME);
        $('#csrNameText').unbind('change');
        $('#csrNameText').bind('change', function (e) {
            if (verifyStringLength(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_NAME, 64)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_NAME = $('#csrNameText').val(); saveProjectAPI();
            }
        });

        $('#csrNameText').prop("disabled", project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_USE_UDID_AS_COMMON_NAME);
    } //csrNameText
    {
        $('#csrEmailText').val(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_EMAIL);
        $('#csrEmailText').unbind('change');
        $('#csrEmailText').bind('change', function (e) {
            if (verifyEmail(e, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_EMAIL)) {
                project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_EMAIL = $('#csrEmailText').val(); saveProjectAPI();
            }
        });
    } //csrEmailText

    updateDiceGui();
}

function updateCSRGUI() {

    var cert_val = $('#CREATE_CSR').val();

    if (cert_val == 2) //Disabled
    {
        $('#fieldsetCSRSettings').hide();
        $('#UDID_name_div').hide();
        $('#csrValidityNameStartDiv').hide();
        $('#csrValidityNameEndDiv').hide();
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CREATE_CSR           = false;
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.USE_SELF_SIGNED_CERT = false;

    }
    else if (cert_val == 1) //SelfCert
    {
        $('#fieldsetCSRSettings').show();
        $('#UDID_name_div').show();
        $('#csrValidityNameStartDiv').show();
        $('#csrValidityNameEndDiv').show();
        //$('#fieldsetCSRSettings').text("DanaDan");
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CREATE_CSR           = true;
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.USE_SELF_SIGNED_CERT = true;
    }
    else
    {
        $('#fieldsetCSRSettings').show();
        $('#UDID_name_div').show();
        $('#csrValidityNameStartDiv').hide();
        $('#csrValidityNameEndDiv').hide();
        //$('#fieldsetCSRSettings').text("CertULul");
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CREATE_CSR           = true;
        project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.USE_SELF_SIGNED_CERT = false;
    }
    //$('#fieldsetCSRSettings').prop("disabled", !project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CREATE_CSR);

    saveProjectAPI();
}

function verifyEmail(e, oldVal) {
    var str = e.currentTarget.value;

    var msg = "Invalid email: "
    //var regexpression = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$";
    var regexpression = "[\\S]+@[\\S]+\\.[a-zA-Z]{2,4}$";

    var b_test = new RegExp(regexpression).test(str);

    if (str.length > 0)//not empty
    {
        if (b_test == false) {
            msg += 'incorrect email address';
        }/* else if (str.trim() === "") {
        msg += 'may not be empty or made out of spaces';
        }*/ else if (str.length > 32) {
            msg += 'may not be longer than 64 characters (currently: ' + str.length + ")";
        } else {
            return true;
        }
        fileBrowser.showInputDialog(msg,
                false,
                null,
                { text: "Close" });

        e.currentTarget.value = oldVal;
        return false;
    }
    else //empty
    {
        return true;
    }
}

function verifyStringLength(e, oldVal, length) {

    var str = e.currentTarget.value;

    var msg = "Invalid string length: "
    if (str.length > 0)//not empty
    {
        if (str.length > length) {
            msg += 'may not be longer than ' + length + ' characters (currently: ' + str.length + ")";

            fileBrowser.showInputDialog(msg,
                false,
                null,
                { text: "Close" });

            e.currentTarget.value = oldVal;
            return false;
        }
        return true;
    }
    return true;
    
}

function setDays(selection, month, year) {

    selection.empty();

    if ((month == 1) || (month == 3) || (month == 5) || (month == 1) || (month == 7) || (month == 8) || (month == 10) || (month == 12))
    {
        $('<option>').val('01').text('01').appendTo(selection);
        $('<option>').val('02').text('02').appendTo(selection);
        $('<option>').val('03').text('03').appendTo(selection);
        $('<option>').val('04').text('04').appendTo(selection);
        $('<option>').val('05').text('05').appendTo(selection);
        $('<option>').val('06').text('06').appendTo(selection);
        $('<option>').val('07').text('07').appendTo(selection);
        $('<option>').val('08').text('08').appendTo(selection);
        $('<option>').val('09').text('09').appendTo(selection);
        $('<option>').val('10').text('10').appendTo(selection);
        $('<option>').val('11').text('11').appendTo(selection);
        $('<option>').val('12').text('12').appendTo(selection);
        $('<option>').val('13').text('13').appendTo(selection);
        $('<option>').val('14').text('14').appendTo(selection);
        $('<option>').val('15').text('15').appendTo(selection);
        $('<option>').val('16').text('16').appendTo(selection);
        $('<option>').val('17').text('17').appendTo(selection);
        $('<option>').val('18').text('18').appendTo(selection);
        $('<option>').val('19').text('19').appendTo(selection);
        $('<option>').val('20').text('20').appendTo(selection);
        $('<option>').val('21').text('21').appendTo(selection);
        $('<option>').val('22').text('22').appendTo(selection);
        $('<option>').val('23').text('23').appendTo(selection);
        $('<option>').val('24').text('24').appendTo(selection);
        $('<option>').val('25').text('25').appendTo(selection);
        $('<option>').val('26').text('26').appendTo(selection);
        $('<option>').val('27').text('27').appendTo(selection);
        $('<option>').val('28').text('28').appendTo(selection);
        $('<option>').val('29').text('29').appendTo(selection);
        $('<option>').val('30').text('30').appendTo(selection);
        $('<option>').val('31').text('31').appendTo(selection);
    }
    else if ((month == 4) || (month == 6) || (month == 9) || (month == 11)) {
        $('<option>').val('01').text('01').appendTo(selection);
        $('<option>').val('02').text('02').appendTo(selection);
        $('<option>').val('03').text('03').appendTo(selection);
        $('<option>').val('04').text('04').appendTo(selection);
        $('<option>').val('05').text('05').appendTo(selection);
        $('<option>').val('06').text('06').appendTo(selection);
        $('<option>').val('07').text('07').appendTo(selection);
        $('<option>').val('08').text('08').appendTo(selection);
        $('<option>').val('09').text('09').appendTo(selection);
        $('<option>').val('10').text('10').appendTo(selection);
        $('<option>').val('11').text('11').appendTo(selection);
        $('<option>').val('12').text('12').appendTo(selection);
        $('<option>').val('13').text('13').appendTo(selection);
        $('<option>').val('14').text('14').appendTo(selection);
        $('<option>').val('15').text('15').appendTo(selection);
        $('<option>').val('16').text('16').appendTo(selection);
        $('<option>').val('17').text('17').appendTo(selection);
        $('<option>').val('18').text('18').appendTo(selection);
        $('<option>').val('19').text('19').appendTo(selection);
        $('<option>').val('20').text('20').appendTo(selection);
        $('<option>').val('21').text('21').appendTo(selection);
        $('<option>').val('22').text('22').appendTo(selection);
        $('<option>').val('23').text('23').appendTo(selection);
        $('<option>').val('24').text('24').appendTo(selection);
        $('<option>').val('25').text('25').appendTo(selection);
        $('<option>').val('26').text('26').appendTo(selection);
        $('<option>').val('27').text('27').appendTo(selection);
        $('<option>').val('28').text('28').appendTo(selection);
        $('<option>').val('29').text('29').appendTo(selection);
        $('<option>').val('30').text('30').appendTo(selection);
    }
    else //February
    {
        if (year % 4 == 0)
        {
            $('<option>').val('01').text('01').appendTo(selection);
            $('<option>').val('02').text('02').appendTo(selection);
            $('<option>').val('03').text('03').appendTo(selection);
            $('<option>').val('04').text('04').appendTo(selection);
            $('<option>').val('05').text('05').appendTo(selection);
            $('<option>').val('06').text('06').appendTo(selection);
            $('<option>').val('07').text('07').appendTo(selection);
            $('<option>').val('08').text('08').appendTo(selection);
            $('<option>').val('09').text('09').appendTo(selection);
            $('<option>').val('10').text('10').appendTo(selection);
            $('<option>').val('11').text('11').appendTo(selection);
            $('<option>').val('12').text('12').appendTo(selection);
            $('<option>').val('13').text('13').appendTo(selection);
            $('<option>').val('14').text('14').appendTo(selection);
            $('<option>').val('15').text('15').appendTo(selection);
            $('<option>').val('16').text('16').appendTo(selection);
            $('<option>').val('17').text('17').appendTo(selection);
            $('<option>').val('18').text('18').appendTo(selection);
            $('<option>').val('19').text('19').appendTo(selection);
            $('<option>').val('20').text('20').appendTo(selection);
            $('<option>').val('21').text('21').appendTo(selection);
            $('<option>').val('22').text('22').appendTo(selection);
            $('<option>').val('23').text('23').appendTo(selection);
            $('<option>').val('24').text('24').appendTo(selection);
            $('<option>').val('25').text('25').appendTo(selection);
            $('<option>').val('26').text('26').appendTo(selection);
            $('<option>').val('27').text('27').appendTo(selection);
            $('<option>').val('28').text('28').appendTo(selection);
            $('<option>').val('29').text('29').appendTo(selection);
        }
        else
        {
            $('<option>').val('01').text('01').appendTo(selection);
            $('<option>').val('02').text('02').appendTo(selection);
            $('<option>').val('03').text('03').appendTo(selection);
            $('<option>').val('04').text('04').appendTo(selection);
            $('<option>').val('05').text('05').appendTo(selection);
            $('<option>').val('06').text('06').appendTo(selection);
            $('<option>').val('07').text('07').appendTo(selection);
            $('<option>').val('08').text('08').appendTo(selection);
            $('<option>').val('09').text('09').appendTo(selection);
            $('<option>').val('10').text('10').appendTo(selection);
            $('<option>').val('11').text('11').appendTo(selection);
            $('<option>').val('12').text('12').appendTo(selection);
            $('<option>').val('13').text('13').appendTo(selection);
            $('<option>').val('14').text('14').appendTo(selection);
            $('<option>').val('15').text('15').appendTo(selection);
            $('<option>').val('16').text('16').appendTo(selection);
            $('<option>').val('17').text('17').appendTo(selection);
            $('<option>').val('18').text('18').appendTo(selection);
            $('<option>').val('19').text('19').appendTo(selection);
            $('<option>').val('20').text('20').appendTo(selection);
            $('<option>').val('21').text('21').appendTo(selection);
            $('<option>').val('22').text('22').appendTo(selection);
            $('<option>').val('23').text('23').appendTo(selection);
            $('<option>').val('24').text('24').appendTo(selection);
            $('<option>').val('25').text('25').appendTo(selection);
            $('<option>').val('26').text('26').appendTo(selection);
            $('<option>').val('27').text('27').appendTo(selection);
            $('<option>').val('28').text('28').appendTo(selection);
        }
    }
   
}

function validateStartDate(year, month, day) {
    if (~~Number(year) < ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR))
        return true;
    if ((~~Number(year) == ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR))
        && (~~Number(month) < ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH)))
        return true;
    if (    (~~Number(year ) == ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_YEAR))
         && (~~Number(month) == ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_MONTH))
         && (~~Number(day  ) <= ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)))
        return true;
    
    fileBrowser.showInputDialog('Start certificate date is invalid (> end) !',
                        false,
                        null,
                        { text: "Close"});
    //{ text: "Close", func: function () {return false;  } });
                        
    return false;
}

function validateEndDate(year, month, day) {
    if (~~Number(year) > ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR))
        return true;
    if ((~~Number(year) == ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR))
        && (~~Number(month) > ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH)))
        return true;
    if ((~~Number(year) == ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_YEAR))
         && (~~Number(month) == ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_MONTH))
         && (~~Number(day) > ~~Number(project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_START_DAY)))
        return true;

    fileBrowser.showInputDialog('End certificate date is invalid (< start) !',
                        false,
                        null,
                        { text: "Close" });
    return false;
}

function verifyNumLimitsDays(e, month, year, oldVal) {

    if ((month == 1) || (month == 3) || (month == 5) || (month == 1) || (month == 7) || (month == 8) || (month == 10) || (month == 12)){
        verifyNumLimits(e, 1, 31, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)
    }
    else if ((month == 4) || (month == 6) || (month == 9) || (month == 11)) {
        verifyNumLimits(e, 1, 30, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)
    }
    else//February
    {
        if (year % 4 == 0)
        {
            verifyNumLimits(e, 1, 29, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)
        }
        else
        {
            verifyNumLimits(e, 1, 28, project.systemFiles.CONFIG_TYPE_CSR_DICE_CFG.CSR_END_DAY)
        }
    }
}

function ShowCSRDateWarning(show) {
    if (show) {
        $('#csr_starts_date_warning').show();
        $('#csr_ends_date_warning').show();
        $('#csr_starts_date_clearM').show();
        $('#csr_ends_date_clearM').show();
        $('#csr_starts_date_clearB').hide();
        $('#csr_ends_date_clearB').hide();
    } else {
        $('#csr_starts_date_warning').hide();
        $('#csr_ends_date_warning').hide();
        $('#csr_starts_date_clearM').hide();
        $('#csr_ends_date_clearM').hide();
        $('#csr_starts_date_clearB').show();
        $('#csr_ends_date_clearB').show();
    }


}

/*
function deviceTypeSelectChange()
{
    if ($('#deviceTypeSelect').val().indexOf('35') != -1) //gen3
    {
        $('#deviceSubTypeInputWrap').show();

    }
    else {
       
        $('#deviceSubTypeInputWrap').hide();
    }
}
*/