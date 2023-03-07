function autocompleteEnable(input) {

    var data = [
                    { value: 'EU', label: 'EU' },
                    { value: 'US', label: 'USA' },
                    { value: 'JP', label: 'Japan' }
                ]
    if (project.header.IsGen3 == true) {
        data = [
                    { value: 'EU', label: 'EU' },
                    { value: 'US', label: 'USA' },
                    { value: 'JP', label: 'Japan' },
                    { value: 'AF', label: 'Afghanistan' },
                    { value: 'AL', label: 'Albania' },
                    { value: 'DZ', label: 'Algeria' },
                    { value: 'AS', label: 'American Samoa' },
                    { value: 'AD', label: 'Andorra' },
                    { value: 'AI', label: 'Anguilla' },
                    { value: 'AR', label: 'Argentina' },
                    { value: 'AM', label: 'Armenia' },
                    { value: 'AW', label: 'Aruba' },
                    { value: 'AU', label: 'Australia' },
                    { value: 'AT', label: 'Austria' },
                    { value: 'AZ', label: 'Azerbaijan' },
                    { value: 'BS', label: 'Bahamas' },
                    { value: 'BH', label: 'Bahrain' },
                    { value: 'BD', label: 'Bangladesh' },
                    { value: 'BB', label: 'Barbados' },
                    { value: 'BY', label: 'Belarus' },
                    { value: 'BE', label: 'Belgium' },
                    { value: 'BZ', label: 'Belize' },
                    { value: 'BM', label: 'Bermuda' },
                    { value: 'BT', label: 'Bhutan' },
                    { value: 'BO', label: 'Bolivia' },
                    { value: 'BA', label: 'Bosnia and Herzegovina' },
                    { value: 'BR', label: 'Brazil' },
                    { value: 'BN', label: 'Brunei Darussalam' },
                    { value: 'BG', label: 'Bulgaria' },
                    { value: 'BF', label: 'Burkina Faso' },
                    { value: 'KH', label: 'Cambodia' },
                    { value: 'CA', label: 'Canada' },
                    { value: 'KY', label: 'Cayman Islands' },
                    { value: 'CF', label: 'Central African Republic' },
                    { value: 'TD', label: 'Chad' },
                    { value: 'CL', label: 'Chile' },
                    { value: 'CN', label: 'China' },
                    { value: 'CX', label: 'Christmas Island' },
                    { value: 'CO', label: 'Colombia' },
                    { value: 'CR', label: 'Costa Rica' },
                    { value: 'CI', label: 'Cote Divoire' },
                    { value: 'HR', label: 'Croatia' },
                    { value: 'CU', label: 'Cuba' },
                    { value: 'CY', label: 'Cyprus' },
                    { value: 'CZ', label: 'Czechia' },
                    { value: 'DK', label: 'Denmark' },
                    { value: 'DM', label: 'Dominica' },
                    { value: 'DO', label: 'Dominican Republic' },
                    { value: 'EC', label: 'Ecuador' },
                    { value: 'EG', label: 'Egypt' },
                    { value: 'SV', label: 'El Salvador' },
                    { value: 'EE', label: 'Estonia' },
                    { value: 'ET', label: 'Ethiopia' },
                    { value: 'FI', label: 'Finland' },
                    { value: 'FR', label: 'France' },
                    { value: 'GF', label: 'French Guiana' },
                    { value: 'PF', label: 'French Polynesia' },
                    { value: 'GE', label: 'Georgia' },
                    { value: 'DE', label: 'Germany' },
                    { value: 'GH', label: 'Ghana' },
                    { value: 'GR', label: 'Greece' },
                    { value: 'GL', label: 'Greenland' },
                    { value: 'GD', label: 'Grenada' },
                    { value: 'GP', label: 'Guadeloupe' },
                    { value: 'GU', label: 'Guam' },
                    { value: 'GT', label: 'Guatemala' },
                    { value: 'GY', label: 'Guyana' },
                    { value: 'HT', label: 'Haiti' },
                    { value: 'HN', label: 'Honduras' },
                    { value: 'HK', label: 'Hong Kong' },
                    { value: 'HU', label: 'Hungary' },
                    { value: 'IS', label: 'Iceland' },
                    { value: 'IN', label: 'India' },
                    { value: 'ID', label: 'Indonesia' },
                    { value: 'IR', label: 'Iran' },
                    { value: 'IE', label: 'Ireland' },
                    { value: 'IL', label: 'Israel' },
                    { value: 'IT', label: 'Italy' },
                    { value: 'JM', label: 'Jamaica' },
                    { value: 'JO', label: 'Jordan' },
                    { value: 'KZ', label: 'Kazakhstan' },
                    { value: 'KE', label: 'Kenya' },
                    { value: 'KP', label: 'Korea North' },
                    { value: 'KR', label: 'Korea South' },
                    { value: 'KW', label: 'Kuwait' },
                    { value: 'LV', label: 'Latvia' },
                    { value: 'LB', label: 'Lebanon' },
                    { value: 'LS', label: 'Lesotho' },
                    { value: 'LI', label: 'Liechtenstein' },
                    { value: 'LT', label: 'Lithuania' },
                    { value: 'LU', label: 'Luxembourg' },
                    { value: 'MO', label: 'Macao' },
                    { value: 'MK', label: 'Macedonia' },
                    { value: 'MG', label: 'Madagascar' },
                    { value: 'MW', label: 'Malawi' },
                    { value: 'MY', label: 'Malaysia' },
                    { value: 'MV', label: 'Maldives' },
                    { value: 'MT', label: 'Malta' },
                    { value: 'MH', label: 'Marshall Islands' },
                    { value: 'MQ', label: 'Martinique' },
                    { value: 'MR', label: 'Mauritania' },
                    { value: 'MU', label: 'Mauritius' },
                    { value: 'YT', label: 'Mayotte' },
                    { value: 'MX', label: 'Mexico' },
                    { value: 'FM', label: 'Micronesia' },
                    { value: 'MD', label: 'Moldova' },
                    { value: 'MC', label: 'Monaco' },
                    { value: 'MN', label: 'Mongolia' },
                    { value: 'ME', label: 'Montenegro' },
                    { value: 'MA', label: 'Morocco' },
                    { value: 'NP', label: 'Nepal' },
                    { value: 'NL', label: 'Netherlands' },
                    { value: 'AN', label: 'Netherlands Antilles' },
                    { value: 'NZ', label: 'New Zealand' },
                    { value: 'NI', label: 'Nicaragua' },
                    { value: 'NG', label: 'Nigeria' },
                    { value: 'MP', label: 'Northern Mariana Islands' },
                    { value: 'NO', label: 'Norway' },
                    { value: 'OM', label: 'Oman' },
                    { value: 'PK', label: 'Pakistan' },
                    { value: 'PW', label: 'Palau' },
                    { value: 'PA', label: 'Panama' },
                    { value: 'PG', label: 'Papua New Guinea' },
                    { value: 'PY', label: 'Paraguay' },
                    { value: 'PE', label: 'Peru' },
                    { value: 'PH', label: 'Philippines' },
                    { value: 'PL', label: 'Poland' },
                    { value: 'PT', label: 'Portugal' },
                    { value: 'PR', label: 'Puerto Rico' },
                    { value: 'QA', label: 'Qatar' },
                    { value: 'RE', label: 'Reunion' },
                    { value: 'RO', label: 'Romania' },
                    { value: 'RU', label: 'Russian Federation' },
                    { value: 'RW', label: 'Rwanda' },
                    { value: 'BL', label: 'Saint Barthélemy' },
                    { value: 'KN', label: 'Saint Kitts and Nevis' },
                    { value: 'LC', label: 'Saint Lucia' },
                    { value: 'PM', label: 'Saint Pierre and Miquelon' },
                    { value: 'VC', label: 'Saint Vincent and The Grenadines' },
                    { value: 'WS', label: 'Samoa' },
                    { value: 'SA', label: 'Saudi Arabia' },
                    { value: 'SN', label: 'Senegal' },
                    { value: 'RS', label: 'Serbia' },
                    { value: 'SG', label: 'Singapore' },
                    { value: 'SK', label: 'Slovakia' },
                    { value: 'SI', label: 'Slovenia' },
                    { value: 'ZA', label: 'South Africa' },
                    { value: 'ES', label: 'Spain' },
                    { value: 'LK', label: 'Sri Lanka' },
                    { value: 'SR', label: 'Suriname' },
                    { value: 'SE', label: 'Sweden' },
                    { value: 'CH', label: 'Switzerland' },
                    { value: 'SY', label: 'Syrian Arab Republic' },
                    { value: 'TW', label: 'Taiwan' },
                    { value: 'TZ', label: 'Tanzania' },
                    { value: 'TH', label: 'Thailand' },
                    { value: 'TG', label: 'Togo' },
                    { value: 'TT', label: 'Trinidad and Tobago' },
                    { value: 'TN', label: 'Tunisia' },
                    { value: 'TR', label: 'Turkey' },
                    { value: 'TC', label: 'Turks and Caicos Islands' },
                    { value: 'UG', label: 'Uganda' },
                    { value: 'UA', label: 'Ukraine' },
                    { value: 'AE', label: 'United Arab Emirates' },
                    { value: 'GB', label: 'United Kingdom' },
                    { value: 'UY', label: 'Uruguay' },
                    { value: 'UZ', label: 'Uzbekistan' },
                    { value: 'VU', label: 'Vanuatu' },
                    { value: 'VE', label: 'Venezuela' },
                    { value: 'VN', label: 'Viet Nam' },
                    { value: 'VI', label: 'Virgin Islands' },
                    { value: 'WF', label: 'Wallis and Futuna' },
                    { value: 'YE', label: 'Yemen' },
                    { value: 'ZW', label: 'Zimbabwe' },
                    { value: 'WW', label: 'WorldWide' }
        ];
    }
    //$("input#CountryCodeInput").autocomplete({
    $(input).autocomplete({

        source: function( request, response ) {
            var matcher = new RegExp( "^" + $.ui.autocomplete.escapeRegex( request.term ), "i" );
            response( $.grep( data, function( item ){
                return matcher.test( item.label );
            }) );
        }
    });
}

function addCountryCode5G(selection) {
    if (project.header.IsGen3 == true) {
        $('<option>').val('AF').text('Afghanistan').appendTo(selection);
        $('<option>').val('AL').text('Albania').appendTo(selection);
        $('<option>').val('DZ').text('Algeria').appendTo(selection);
        $('<option>').val('AS').text('American Samoa').appendTo(selection);
        $('<option>').val('AD').text('Andorra').appendTo(selection);
        $('<option>').val('AI').text('Anguilla').appendTo(selection);
        $('<option>').val('AR').text('Argentina').appendTo(selection);
        $('<option>').val('AM').text('Armenia').appendTo(selection);
        $('<option>').val('AW').text('Aruba').appendTo(selection);
        $('<option>').val('AU').text('Australia').appendTo(selection);
        $('<option>').val('AT').text('Austria').appendTo(selection);
        $('<option>').val('AZ').text('Azerbaijan').appendTo(selection);
        $('<option>').val('BS').text('Bahamas').appendTo(selection);
        $('<option>').val('BH').text('Bahrain').appendTo(selection);
        $('<option>').val('BD').text('Bangladesh').appendTo(selection);
        $('<option>').val('BB').text('Barbados').appendTo(selection);
        $('<option>').val('BY').text('Belarus').appendTo(selection);
        $('<option>').val('BE').text('Belgium').appendTo(selection);
        $('<option>').val('BZ').text('Belize').appendTo(selection);
        $('<option>').val('BM').text('Bermuda').appendTo(selection);
        $('<option>').val('BT').text('Bhutan').appendTo(selection);
        $('<option>').val('BO').text('Bolivia').appendTo(selection);
        $('<option>').val('BA').text('Bosnia and Herzegovina').appendTo(selection);
        $('<option>').val('BR').text('Brazil').appendTo(selection);
        $('<option>').val('BN').text('Brunei Darussalam').appendTo(selection);
        $('<option>').val('BG').text('Bulgaria').appendTo(selection);
        $('<option>').val('BF').text('Burkina Faso').appendTo(selection);
        $('<option>').val('BL').text('Saint Barthélemy').appendTo(selection);
        $('<option>').val('KH').text('Cambodia').appendTo(selection);
        $('<option>').val('CA').text('Canada').appendTo(selection);
        $('<option>').val('KY').text('Cayman Islands').appendTo(selection);
        $('<option>').val('CF').text('Central African Republic').appendTo(selection);
        $('<option>').val('TD').text('Chad').appendTo(selection);
        $('<option>').val('CL').text('Chile').appendTo(selection);
        $('<option>').val('CN').text('China').appendTo(selection);
        $('<option>').val('CX').text('Christmas Island').appendTo(selection);
        $('<option>').val('CO').text('Colombia').appendTo(selection);
        $('<option>').val('CR').text('Costa Rica').appendTo(selection);
        $('<option>').val('CI').text('Cote Divoire').appendTo(selection);
        $('<option>').val('HR').text('Croatia').appendTo(selection);
        $('<option>').val('CU').text('Cuba').appendTo(selection);
        $('<option>').val('CY').text('Cyprus').appendTo(selection);
        $('<option>').val('CZ').text('Czechia').appendTo(selection);
        $('<option>').val('DK').text('Denmark').appendTo(selection);
        $('<option>').val('DM').text('Dominica').appendTo(selection);
        $('<option>').val('DO').text('Dominican Republic').appendTo(selection);
        $('<option>').val('EC').text('Ecuador').appendTo(selection);
        $('<option>').val('EG').text('Egypt').appendTo(selection);
        $('<option>').val('SV').text('El Salvador').appendTo(selection);
        $('<option>').val('EE').text('Estonia').appendTo(selection);
        $('<option>').val('ET').text('Ethiopia').appendTo(selection);
        $('<option>').val('FI').text('Finland').appendTo(selection);
        $('<option>').val('FR').text('France').appendTo(selection);
        $('<option>').val('GF').text('French Guiana').appendTo(selection);
        $('<option>').val('PF').text('French Polynesia').appendTo(selection);
        $('<option>').val('GE').text('Georgia').appendTo(selection);
        $('<option>').val('DE').text('Germany').appendTo(selection);
        $('<option>').val('GH').text('Ghana').appendTo(selection);
        $('<option>').val('GR').text('Greece').appendTo(selection);
        $('<option>').val('GL').text('Greenland').appendTo(selection);
        $('<option>').val('GD').text('Grenada').appendTo(selection);
        $('<option>').val('GP').text('Guadeloupe').appendTo(selection);
        $('<option>').val('GU').text('Guam').appendTo(selection);
        $('<option>').val('GT').text('Guatemala').appendTo(selection);
        $('<option>').val('GY').text('Guyana').appendTo(selection);
        $('<option>').val('HT').text('Haiti').appendTo(selection);
        $('<option>').val('HN').text('Honduras').appendTo(selection);
        $('<option>').val('HK').text('Hong Kong').appendTo(selection);
        $('<option>').val('HU').text('Hungary').appendTo(selection);
        $('<option>').val('IS').text('Iceland').appendTo(selection);
        $('<option>').val('IN').text('India').appendTo(selection);
        $('<option>').val('ID').text('Indonesia').appendTo(selection);
        $('<option>').val('IR').text('Iran').appendTo(selection);
        $('<option>').val('IE').text('Ireland').appendTo(selection);
        $('<option>').val('IL').text('Israel').appendTo(selection);
        $('<option>').val('IT').text('Italy').appendTo(selection);
        $('<option>').val('JM').text('Jamaica').appendTo(selection);
        $('<option>').val('JO').text('Jordan').appendTo(selection);
        $('<option>').val('KZ').text('Kazakhstan').appendTo(selection);
        $('<option>').val('KE').text('Kenya').appendTo(selection);
        $('<option>').val('KP').text('Korea').appendTo(selection);
        $('<option>').val('KR').text('Korea').appendTo(selection);
        $('<option>').val('KW').text('Kuwait').appendTo(selection);
        $('<option>').val('LV').text('Latvia').appendTo(selection);
        $('<option>').val('LB').text('Lebanon').appendTo(selection);
        $('<option>').val('LS').text('Lesotho').appendTo(selection);
        $('<option>').val('LI').text('Liechtenstein').appendTo(selection);
        $('<option>').val('LT').text('Lithuania').appendTo(selection);
        $('<option>').val('LU').text('Luxembourg').appendTo(selection);
        $('<option>').val('MO').text('Macao').appendTo(selection);
        $('<option>').val('MK').text('Macedonia').appendTo(selection);
        $('<option>').val('MF').text('Saint Martin (French part)').appendTo(selection);
        $('<option>').val('MW').text('Malawi').appendTo(selection);
        $('<option>').val('MY').text('Malaysia').appendTo(selection);
        $('<option>').val('MV').text('Maldives').appendTo(selection);
        $('<option>').val('MT').text('Malta').appendTo(selection);
        $('<option>').val('MH').text('Marshall Islands').appendTo(selection);
        $('<option>').val('MQ').text('Martinique').appendTo(selection);
        $('<option>').val('MR').text('Mauritania').appendTo(selection);
        $('<option>').val('MU').text('Mauritius').appendTo(selection);
        $('<option>').val('YT').text('Mayotte').appendTo(selection);
        $('<option>').val('MX').text('Mexico').appendTo(selection);
        $('<option>').val('FM').text('Micronesia').appendTo(selection);
        $('<option>').val('MD').text('Moldova').appendTo(selection);
        $('<option>').val('MC').text('Monaco').appendTo(selection);
        $('<option>').val('MN').text('Mongolia').appendTo(selection);
        $('<option>').val('ME').text('Montenegro').appendTo(selection);
        $('<option>').val('MA').text('Morocco').appendTo(selection);
        $('<option>').val('NP').text('Nepal').appendTo(selection);
        $('<option>').val('NL').text('Netherlands').appendTo(selection);
        $('<option>').val('AN').text('Netherlands Antilles').appendTo(selection);
        $('<option>').val('NZ').text('New Zealand').appendTo(selection);
        $('<option>').val('NI').text('Nicaragua').appendTo(selection);
        $('<option>').val('NG').text('Nigeria').appendTo(selection);
        $('<option>').val('MP').text('Northern Mariana Islands').appendTo(selection);
        $('<option>').val('NO').text('Norway').appendTo(selection);
        $('<option>').val('OM').text('Oman').appendTo(selection);
        $('<option>').val('PK').text('Pakistan').appendTo(selection);
        $('<option>').val('PW').text('Palau').appendTo(selection);
        $('<option>').val('PA').text('Panama').appendTo(selection);
        $('<option>').val('PG').text('Papua New Guinea').appendTo(selection);
        $('<option>').val('PY').text('Paraguay').appendTo(selection);
        $('<option>').val('PE').text('Peru').appendTo(selection);
        $('<option>').val('PH').text('Philippines').appendTo(selection);
        $('<option>').val('PL').text('Poland').appendTo(selection);
        $('<option>').val('PT').text('Portugal').appendTo(selection);
        $('<option>').val('PR').text('Puerto Rico').appendTo(selection);
        $('<option>').val('QA').text('Qatar').appendTo(selection);
        $('<option>').val('RE').text('Reunion').appendTo(selection);
        $('<option>').val('RO').text('Romania').appendTo(selection);
        $('<option>').val('RU').text('Russian Federation').appendTo(selection);
        $('<option>').val('RW').text('Rwanda').appendTo(selection);
        $('<option>').val('KN').text('Saint Kitts and Nevis').appendTo(selection);
        $('<option>').val('LC').text('Saint Lucia').appendTo(selection);
        $('<option>').val('PM').text('Saint Pierre and Miquelon').appendTo(selection);
        $('<option>').val('VC').text('Saint Vincent and The Grenadines').appendTo(selection);
        $('<option>').val('WS').text('Samoa').appendTo(selection);
        $('<option>').val('SA').text('Saudi Arabia').appendTo(selection);
        $('<option>').val('SN').text('Senegal').appendTo(selection);
        $('<option>').val('RS').text('Serbia').appendTo(selection);
        $('<option>').val('SG').text('Singapore').appendTo(selection);
        $('<option>').val('SK').text('Slovakia').appendTo(selection);
        $('<option>').val('SI').text('Slovenia').appendTo(selection);
        $('<option>').val('ZA').text('South Africa').appendTo(selection);
        $('<option>').val('ES').text('Spain').appendTo(selection);
        $('<option>').val('LK').text('Sri Lanka').appendTo(selection);
        $('<option>').val('SR').text('Suriname').appendTo(selection);
        $('<option>').val('SE').text('Sweden').appendTo(selection);
        $('<option>').val('CH').text('Switzerland').appendTo(selection);
        $('<option>').val('SY').text('Syrian Arab Republic').appendTo(selection);
        $('<option>').val('TW').text('Taiwan').appendTo(selection);
        $('<option>').val('TZ').text('Tanzania').appendTo(selection);
        $('<option>').val('TH').text('Thailand').appendTo(selection);
        $('<option>').val('TG').text('Togo').appendTo(selection);
        $('<option>').val('TT').text('Trinidad and Tobago').appendTo(selection);
        $('<option>').val('TN').text('Tunisia').appendTo(selection);
        $('<option>').val('TR').text('Turkey').appendTo(selection);
        $('<option>').val('TC').text('Turks and Caicos Islands').appendTo(selection);
        $('<option>').val('UG').text('Uganda').appendTo(selection);
        $('<option>').val('UA').text('Ukraine').appendTo(selection);
        $('<option>').val('AE').text('United Arab Emirates').appendTo(selection);
        $('<option>').val('GB').text('United Kingdom').appendTo(selection);
        $('<option>').val('UY').text('Uruguay').appendTo(selection);
        $('<option>').val('UZ').text('Uzbekistan').appendTo(selection);
        $('<option>').val('VU').text('Vanuatu').appendTo(selection);
        $('<option>').val('VE').text('Venezuela').appendTo(selection);
        $('<option>').val('VN').text('Viet Nam').appendTo(selection);
        $('<option>').val('VI').text('Virgin Islands').appendTo(selection);
        $('<option>').val('WF').text('Wallis and Futuna').appendTo(selection);
        $('<option>').val('YE').text('Yemen').appendTo(selection);
        $('<option>').val('ZW').text('Zimbabwe').appendTo(selection);
    }
}
