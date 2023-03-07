/*
 *  ======== Utils ========
 *  Common utility functions for /imagecreator
 */
"use strict";
/*global exports, system*/
// var gStartRole = "2"
let Common   = system.getScript("/ti/drivers/Common.js");
/*let family   = Common.device2Family(system.deviceData, "UART");*/
let logError = Common.logError;
const isStandAloneGUI = system.isStandAloneGUI();


exports = {
    
	validateToken   	: validateToken,
	validateIP 			: validateIP,	    /* validate IP address is correct IPv4     */
	validateMacAddr 	: validateMacAddr,	/* validate Mac address is correct     */
	country_codes   	: country_codes,
	wildcard        	: wildcard,
	checkIpaddrInRange	: checkIpaddrInRange,
	atoi				: atoi,
    init				: init
};


function wildcard()
{

	//if (isStandAloneGUI) {//for sanity 1.6.0
		if (system.getOS() === 'mac')
		{
			if (isStandAloneGUI)
				return  "."
			else
			    return  ".*"
		}
			
		else 
			return "*";
	/*} else {
		return ".*"
	}*/
	
}
/*
 *  ======== init ========
 */
function init()
{
}

/*
 *  ======== validateToken ========
 *  validate Token is numeric
 
 *  fieldname 		- fieldname of instance to be validated
 *  field 			- field of instance to be validated
 *  inst 			- instance to be validated
 *  validation 		- Issue reporting object
 */
function validateToken(fieldname, field, inst, validation)
{
	/*var TokenRegex=new RegExp("^[0-9]*$");
	
	if(!TokenRegex.test(field))
	{
		logError(validation, inst, fieldname, "Token has to be a decimal");
	}*/

	if (field >  4294967295){

		logError(validation, inst, fieldname, "Filetoken has to be smaller than 4294967295 (0xFFFFFFFF)")
	}
}

/*
 *  ======== validateIP ========
 *  Validate validate IP address is correct IPv4
 
 *  fieldname 		- fieldname of instance to be validated
 *  field 			- field of instance to be validated
 *  inst 			- instance to be validated
 *  validation 		- Issue reporting object
 */
function validateIP(fieldname, field, inst, validation)
{
	var IPRegex=new RegExp("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
	
	if(!IPRegex.test(field))
	{
		logError(validation, inst, fieldname, "You have entered an invalid IP address");
	}
}
/*
 *  ======== validateMacAddr ========
 *  Validate validate IP address is correct IPv4
 
 *  fieldname 		- fieldname of instance to be validated
 *  field 			- field of instance to be validated
 *  inst 			- instance to be validated
 *  validation 		- Issue reporting object
 */
function validateMacAddr(fieldname, field, inst, validation)
{
    var MACRegex=new RegExp("^(?!(?:ff:ff:ff:ff:ff:ff|00:00:00:00:00:00))([0-9a-fA-F][0-9a-fA-F]:){5}([0-9a-fA-F][0-9a-fA-F])$");
    if(!MACRegex.test(field))
    {
		logError(validation, inst, fieldname, 'Wrong Mac Address format. Should be "01:23:45:67:89:AB"\n "00:00:00:00:00:00" and "ff:ff:ff:ff:ff:ff" are not required');
    }
}

function atoi(addr) {
	var parts = addr.split('.').map(function(str) {
	  return parseInt(str); 
	});
  
	return (parts[0] ? parts[0] << 24 : 0) +
		   (parts[1] ? parts[1] << 16 : 0) +
		   (parts[2] ? parts[2] << 8  : 0) +
			parts[3];
  };
  
function checkIpaddrInRange(ipaddr, start, end) {
	var num = atoi(ipaddr);
	return (num >= atoi(start)) && (num <= atoi(end));
  }

function  country_codes()
{ return [
	{ name: 'EU', displayName: 'EU' },
	{ name: 'US', displayName: 'USA' },
	{ name: 'JP', displayName: 'Japan' },
	{ name: 'AF', displayName: 'Afghanistan' },
	{ name: 'AL', displayName: 'Albania' },
	{ name: 'DZ', displayName: 'Algeria' },
	{ name: 'AS', displayName: 'American Samoa' },
	{ name: 'AD', displayName: 'Andorra' },
	{ name: 'AI', displayName: 'Anguilla' },
	{ name: 'AR', displayName: 'Argentina' },
	{ name: 'AM', displayName: 'Armenia' },
	{ name: 'AW', displayName: 'Aruba' },
	{ name: 'AU', displayName: 'Australia' },
	{ name: 'AT', displayName: 'Austria' },
	{ name: 'AZ', displayName: 'Azerbaijan' },
	{ name: 'BS', displayName: 'Bahamas' },
	{ name: 'BH', displayName: 'Bahrain' },
	{ name: 'BD', displayName: 'Bangladesh' },
	{ name: 'BB', displayName: 'Barbados' },
	{ name: 'BY', displayName: 'Belarus' },
	{ name: 'BE', displayName: 'Belgium' },
	{ name: 'BZ', displayName: 'Belize' },
	{ name: 'BM', displayName: 'Bermuda' },
	{ name: 'BT', displayName: 'Bhutan' },
	{ name: 'BO', displayName: 'Bolivia' },
	{ name: 'BA', displayName: 'Bosnia and Herzegovina' },
	{ name: 'BR', displayName: 'Brazil' },
	{ name: 'BN', displayName: 'Brunei Darussalam' },
	{ name: 'BG', displayName: 'Bulgaria' },
	{ name: 'BF', displayName: 'Burkina Faso' },
	{ name: 'KH', displayName: 'Cambodia' },
	{ name: 'CA', displayName: 'Canada' },
	{ name: 'KY', displayName: 'Cayman Islands' },
	{ name: 'CF', displayName: 'Central African Republic' },
	{ name: 'TD', displayName: 'Chad' },
	{ name: 'CL', displayName: 'Chile' },
	{ name: 'CN', displayName: 'China' },
	{ name: 'CX', displayName: 'Christmas Island' },
	{ name: 'CO', displayName: 'Colombia' },
	{ name: 'CR', displayName: 'Costa Rica' },
	{ name: 'CI', displayName: 'Cote Divoire' },
	{ name: 'HR', displayName: 'Croatia' },
	{ name: 'CU', displayName: 'Cuba' },
	{ name: 'CY', displayName: 'Cyprus' },
	{ name: 'CZ', displayName: 'Czechia' },
	{ name: 'DK', displayName: 'Denmark' },
	{ name: 'DM', displayName: 'Dominica' },
	{ name: 'DO', displayName: 'Dominican Republic' },
	{ name: 'EC', displayName: 'Ecuador' },
	{ name: 'EG', displayName: 'Egypt' },
	{ name: 'SV', displayName: 'El Salvador' },
	{ name: 'EE', displayName: 'Estonia' },
	{ name: 'ET', displayName: 'Ethiopia' },
	{ name: 'FI', displayName: 'Finland' },
	{ name: 'FR', displayName: 'France' },
	{ name: 'GF', displayName: 'French Guiana' },
	{ name: 'PF', displayName: 'French Polynesia' },
	{ name: 'GE', displayName: 'Georgia' },
	{ name: 'DE', displayName: 'Germany' },
	{ name: 'GH', displayName: 'Ghana' },
	{ name: 'GR', displayName: 'Greece' },
	{ name: 'GL', displayName: 'Greenland' },
	{ name: 'GD', displayName: 'Grenada' },
	{ name: 'GP', displayName: 'Guadeloupe' },
	{ name: 'GU', displayName: 'Guam' },
	{ name: 'GT', displayName: 'Guatemala' },
	{ name: 'GY', displayName: 'Guyana' },
	{ name: 'HT', displayName: 'Haiti' },
	{ name: 'HN', displayName: 'Honduras' },
	{ name: 'HK', displayName: 'Hong Kong' },
	{ name: 'HU', displayName: 'Hungary' },
	{ name: 'IS', displayName: 'Iceland' },
	{ name: 'IN', displayName: 'India' },
	{ name: 'ID', displayName: 'Indonesia' },
	{ name: 'IR', displayName: 'Iran' },
	{ name: 'IE', displayName: 'Ireland' },
	{ name: 'IL', displayName: 'Israel' },
	{ name: 'IT', displayName: 'Italy' },
	{ name: 'JM', displayName: 'Jamaica' },
	{ name: 'JO', displayName: 'Jordan' },
	{ name: 'KZ', displayName: 'Kazakhstan' },
	{ name: 'KE', displayName: 'Kenya' },
	{ name: 'KP', displayName: 'Korea North' },
	{ name: 'KR', displayName: 'Korea South' },
	{ name: 'KW', displayName: 'Kuwait' },
	{ name: 'LV', displayName: 'Latvia' },
	{ name: 'LB', displayName: 'Lebanon' },
	{ name: 'LS', displayName: 'Lesotho' },
	{ name: 'LI', displayName: 'Liechtenstein' },
	{ name: 'LT', displayName: 'Lithuania' },
	{ name: 'LU', displayName: 'Luxembourg' },
	{ name: 'MO', displayName: 'Macao' },
	{ name: 'MK', displayName: 'Macedonia' },
	{ name: 'MG', displayName: 'Madagascar' },
	{ name: 'MW', displayName: 'Malawi' },
	{ name: 'MY', displayName: 'Malaysia' },
	{ name: 'MV', displayName: 'Maldives' },
	{ name: 'MT', displayName: 'Malta' },
	{ name: 'MH', displayName: 'Marshall Islands' },
	{ name: 'MQ', displayName: 'Martinique' },
	{ name: 'MR', displayName: 'Mauritania' },
	{ name: 'MU', displayName: 'Mauritius' },
	{ name: 'YT', displayName: 'Mayotte' },
	{ name: 'MX', displayName: 'Mexico' },
	{ name: 'FM', displayName: 'Micronesia' },
	{ name: 'MD', displayName: 'Moldova' },
	{ name: 'MC', displayName: 'Monaco' },
	{ name: 'MN', displayName: 'Mongolia' },
	{ name: 'ME', displayName: 'Montenegro' },
	{ name: 'MA', displayName: 'Morocco' },
	{ name: 'NP', displayName: 'Nepal' },
	{ name: 'NL', displayName: 'Netherlands' },
	{ name: 'AN', displayName: 'Netherlands Antilles' },
	{ name: 'NZ', displayName: 'New Zealand' },
	{ name: 'NI', displayName: 'Nicaragua' },
	{ name: 'NG', displayName: 'Nigeria' },
	{ name: 'MP', displayName: 'Northern Mariana Islands' },
	{ name: 'NO', displayName: 'Norway' },
	{ name: 'OM', displayName: 'Oman' },
	{ name: 'PK', displayName: 'Pakistan' },
	{ name: 'PW', displayName: 'Palau' },
	{ name: 'PA', displayName: 'Panama' },
	{ name: 'PG', displayName: 'Papua New Guinea' },
	{ name: 'PY', displayName: 'Paraguay' },
	{ name: 'PE', displayName: 'Peru' },
	{ name: 'PH', displayName: 'Philippines' },
	{ name: 'PL', displayName: 'Poland' },
	{ name: 'PT', displayName: 'Portugal' },
	{ name: 'PR', displayName: 'Puerto Rico' },
	{ name: 'QA', displayName: 'Qatar' },
	{ name: 'RE', displayName: 'Reunion' },
	{ name: 'RO', displayName: 'Romania' },
	{ name: 'RU', displayName: 'Russian Federation' },
	{ name: 'RW', displayName: 'Rwanda' },
	{ name: 'BL', displayName: 'Saint Barthélemy' },
	{ name: 'KN', displayName: 'Saint Kitts and Nevis' },
	{ name: 'LC', displayName: 'Saint Lucia' },
	{ name: 'PM', displayName: 'Saint Pierre and Miquelon' },
	{ name: 'VC', displayName: 'Saint Vincent and The Grenadines' },
	{ name: 'WS', displayName: 'Samoa' },
	{ name: 'SA', displayName: 'Saudi Arabia' },
	{ name: 'SN', displayName: 'Senegal' },
	{ name: 'RS', displayName: 'Serbia' },
	{ name: 'SG', displayName: 'Singapore' },
	{ name: 'SK', displayName: 'Slovakia' },
	{ name: 'SI', displayName: 'Slovenia' },
	{ name: 'ZA', displayName: 'South Africa' },
	{ name: 'ES', displayName: 'Spain' },
	{ name: 'LK', displayName: 'Sri Lanka' },
	{ name: 'SR', displayName: 'Suriname' },
	{ name: 'SE', displayName: 'Sweden' },
	{ name: 'CH', displayName: 'Switzerland' },
	{ name: 'SY', displayName: 'Syrian Arab Republic' },
	{ name: 'TW', displayName: 'Taiwan' },
	{ name: 'TZ', displayName: 'Tanzania' },
	{ name: 'TH', displayName: 'Thailand' },
	{ name: 'TG', displayName: 'Togo' },
	{ name: 'TT', displayName: 'Trinidad and Tobago' },
	{ name: 'TN', displayName: 'Tunisia' },
	{ name: 'TR', displayName: 'Turkey' },
	{ name: 'TC', displayName: 'Turks and Caicos Islands' },
	{ name: 'UG', displayName: 'Uganda' },
	{ name: 'UA', displayName: 'Ukraine' },
	{ name: 'AE', displayName: 'United Arab Emirates' },
	{ name: 'GB', displayName: 'United Kingdom' },
	{ name: 'UY', displayName: 'Uruguay' },
	{ name: 'UZ', displayName: 'Uzbekistan' },
	{ name: 'VU', displayName: 'Vanuatu' },
	{ name: 'VE', displayName: 'Venezuela' },
	{ name: 'VN', displayName: 'Viet Nam' },
	{ name: 'VI', displayName: 'Virgin Islands' },
	{ name: 'WF', displayName: 'Wallis and Futuna' },
	{ name: 'YE', displayName: 'Yemen' },
	{ name: 'ZW', displayName: 'Zimbabwe' },
	{ name: 'WW', displayName: 'WorldWide' }
]
}
