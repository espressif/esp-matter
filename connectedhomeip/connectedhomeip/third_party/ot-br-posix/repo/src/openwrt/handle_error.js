function GetURLParameter(sParam)
{
	var sPageURL = window.location.search.substring(1);
	var sURLVariables = sPageURL.split('&');
	for (var i = 0; i < sURLVariables.length; i++)
	{
		var sParameterName = sURLVariables[i].split('=');
		if (sParameterName[0] == sParam)
		{
			return sParameterName[1];
		}
	}
}
function handle_error(sError)
{
	if(sError && sError != 0)
		alert("Error: " + translateErrorMessage(parseInt(sError)));
}
function translateErrorMessage(sError)
{
	switch(sError)
	{
		case 1:
			return "Failed";
		case 2:
			return "Drop";
		case 3:
			return "NoBufs";
		case 4:
			return "NoRoute";
		case 5:
			return "Busy";
		case 6:
			return "Parse";
		case 7:
			return "InvalidArgs";
		case 8:
			return "Security";
		case 9:
			return "AddressQuery";
		case 10:
			return "NoAddress";
		case 11:
			return "Abort";
		case 12:
			return "NotImplemented";
		case 13:
			return "InvalidState";
		case 14:
			return "NoAck";
		case 15:
			return "ChannelAccessFailure";
		case 16:
			return "Detached";
		case 17:
			return "FcsErr";
		case 18:
			return "NoFrameReceived";
		case 19:
			return "UnknownNeighbor";
		case 20:
			return "InvalidSourceAddress";
		case 21:
			return "AddressFiltered";
		case 22:
			return "DestinationAddressFiltered";
		case 23:
			return "NotFound";
		case 24:
			return "Already";
		case 26:
			return "Ipv5AddressCreationFailure";
		case 27:
			return "NotCapable";
		case 28:
			return "ResponseTimeout";
		case 29:
			return "Duplicated";
		case 30:
			return "ReassemblyTimeout";
		case 31:
			return "NotTmf";
		case 32:
			return "NonLowpanDataFrame";
		case 33:
			return "DisabledFeature";
		case 34:
			return "LinkMarginLow";
		case 255:
			return "GenericError";
		default:
			return "UnknownErrorType";
	}
}
