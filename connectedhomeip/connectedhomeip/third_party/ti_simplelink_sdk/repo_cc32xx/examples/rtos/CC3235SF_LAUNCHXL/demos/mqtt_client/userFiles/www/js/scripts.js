
/////////////////////////////////////////// popup ///////////////////////////////////////////////////////////

var isPopupOpen = false;
var wPopu;
var hPopu;

document.addEventListener("touchstart", function(){}, true);


function unloadPopupBox() {
	isPopupOpen = false;
	$('#popUpcontainerBoxIn').fadeOut("fast");
	$("#popUpcontainer").fadeOut( "slow", function() {
		$('#popUpcontentBox').html("");
	});
	
}	

function loadPopupBox(url ,wPop, hPop) {
	isPopupOpen = true;
	wPopu = wPop;
	hPopu = hPop;
	changePopupSize();
	console.log(url);
	$( "#popUpcontentBox" ).load( url, function() {
	  // alert( "Load was performed." );
	});
	$('#popUpcontainerBoxIn').fadeIn("slow");
	$("#popUpcontainer").fadeIn("slow");
}
function changePopupSize(){
	//alert(wPopu)	
	var wPopIn = wPopu;
	var hPopuIn = hPopu;	
	if(wPopu >  $( window ).width()) { 
		wPopIn = $( window ).width()-30;
	} 

	dWidth = $( window ).width()/2-wPopIn/2;
	dHeight = $( window ).height()/2-hPopu/2;
	
	if(dHeight<10) dHeight = 10;
	
	$('#popUpcontainerBoxIn').css( "width", wPopIn );
	$('#popUpcontainerBoxIn').css( "height", "auto" );
	$('#popUpcontainerBoxIn').css( "left", dWidth );
	$('#popUpcontainerBoxIn').css( "top", dHeight + $(document).scrollTop() );
	$('#popUpcontainer').css( "width", $( document ).width() );
	$('#popUpcontainer').css( "height", $( document ).height() );
}

$.ajaxSetup({ cache: false });

///////////////////////////////////////////end  popup ///////////////////////////////////////////////////////////	



var ledLight = false;

$(document).ready(function() {
	
// index
	$('#startApp').on('click', function(e){
		window.location.href = "ota.html";
	});

// popup
	
	$( window ).resize(function() {
		if(isPopupOpen){
			changePopupSize();
			console.log(isPopupOpen);
		}
	});
	
	$('#help').on('click', function(e){
		loadPopupBox($(this).attr('href'),$( document ).width()-50, $( document ).height()-50);
		e.preventDefault();
	});

	$('#popUpcontainerClose, #popUpcontainer').click( function() {			
		unloadPopupBox();
	});

// menu
	var oddClick = false;
	$(".menuTop").click(function() {
		oddClick = !oddClick;
		oddClick ? $("nav.tmenuNav").slideDown(250) : $("nav.tmenuNav").slideUp(100);
	});


// toggle slide
	
	$(".slideToggle").click(function() {
		ledLight = !ledLight;
		$(".slider").animate({
			right: ledLight ? 85 : -1
		},300, function(){
			setLedStatus((ledLight)?"on":"off");
		});
		// function to change the light on board
	});
	
	$("ul.mMenu").css( "height", ($( document ).height() -  $( ".headerIn" ).outerHeight()));
	
	
	

});
