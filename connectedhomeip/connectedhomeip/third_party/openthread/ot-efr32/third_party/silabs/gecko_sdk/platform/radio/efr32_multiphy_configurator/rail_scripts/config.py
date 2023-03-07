import copy

# These are registers that should not be touched by radio configs, as they are handled by the RAIL API
PROTECTED_REGS = ['SYNTH.FREQ', 'SYNTH.CHCTRL',
                  'SYNTH.CHSP', 'FRC.BLOCKRAMADDR', 'FRC.CONVRAMADDR']

# These are fields of registers that should not be touched by radio configs, as they are handled by the RAIL API
PROTECTED_FIELDS_BASE = {
    'AGC.CTRL0': ['RSSISHIFT'],
    'FRC.RXCTRL': [
        'BUFRESTORERXABORTED',
        'BUFRESTOREFRAMEERROR',
        'BUFCLEAR',
        'TRACKABFRAME',
        'ACCEPTBLOCKERRORS',
        'ACCEPTCRCERRORS',
        'STORECRC'
    ],
    'HFXO0.TRIM': [
        'VTRREGTRIMANA',
        'VTRCORETRIMANA',
        'SHUNTLVLANA'
    ],
    'HFXO0.LOWPWRCTRL': ['SHUNTBIASANA'],
    'MODEM.IRCALCOEF': [
        'CIV',
        'CRV'
    ],
    'MODEM.IRCAL': [
        'MURSHF',
        'MUISHF'
    ],
    'RAC.AUXADCTRIM': ['AUXADCRCTUNE'],
    'RAC.PATRIM0': [
        'TX0DBMTRIMDUTYCYP',
        'TX0DBMTRIMDUTYCYN',
        'TX0DBMTRIMBIASP',
        'TX0DBMTRIMBIASN'
    ],
    'RAC.PATRIM1': [
        'TX0DBMTRIMREGVREF',
        'TX0DBMTRIMREGFB'
    ],
    'RAC.PATRIM2': [
        'TX6DBMTRIMDUTYCYP',
        'TX6DBMTRIMDUTYCYN',
        'TX6DBMTRIMBIASP',
        'TX6DBMTRIMBIASN'
    ],
    'RAC.PATRIM3': [
        'TX6DBMTRIMREGVREF',
        'TX6DBMTRIMREGFB',
        'TX6DBMTRIMPREDRVREGVREF',
        'TX6DBMTRIMPREDRVREGFB'
    ],
    'RAC.RFBIASCAL': [
        'RFBIASCALVREF',
        'RFBIASCALBIAS'
    ],
    'RAC.LNAMIXTRIM1': [
        'LNAMIXIBIASADJ',
        'LNAMIXRFPKDCALCM'
    ],
    'RAC.PRECTRL': ['PREREGTRIM'],
    'RAC.IFADCCAL': ['IFADCTUNERC'],
    'RAC.PGATRIM': ['PGACTUNE'],
    'RAC.AUXADCTRIM': [
        'AUXADCRCTUNE',
        'AUXADCLDOVREFTRIM'
    ],
    'RAC.SYEN': [
        'SYENVCOBIAS',
        'SYENMMDREG',
        'SYLODIVLDOBIASEN',
        'SYENCHPREG',
        'SYENCHPREPLICA',
        'SYENMMDREPLICA1',
        'SYENVCOPFET',
        'SYENVCOREG',
        'SYLODIVLDOEN',
        'SYCHPEN',
        'SYLODIVEN'
    ],
    'RFSENSE.TRIMBG': ['TRIMREF'],
    'RFSENSE.TRIMPTAT': [
        'TRIMCURR',
        'TRIMRES'
    ],
    'RFSENSE.TRIMDAC': [
        'TRIMDM',
        'CMPOSCAL',
        'TRIMCM'
    ],
    'SYNTH.LPFCTRL2RX': ['CALCRX'],
    'SYNTH.LPFCTRL2TX': ['CALCTX'],
    'SYNTH.VCDACCTRL': ['VCDACVAL'],
    'SYNTH.VCOGAIN': [
        'VCOKVCOARSE',
        'VCOKVFINE'
    ]
}

PROTECTED_FIELDS_OCELOT = {
    'RAC.PATRIM3': [
        'TXTRIMOREG',
        'TXTRIMRREG',
        'TXTRIMDREG'
    ],
    'RAC.IFADCTRIM0': [
        'IFADCLDOSHUNTAMPLVL1',
        'IFADCLDOSHUNTAMPLVL2',
        'IFADCLDOSHUNTCURLVL1'
    ],
    'RAC.SYTRIM1': ['SYLODIVTLOTRIMDELAY'],
    'RAC.LNAMIXTRIM4': ['LNAMIXRFPKDCALCMHI', 'LNAMIXRFPKDCALCMLO'],
    'RAC.PGACTRL': ['PGAPOWERMODE'],
    'RAC.IFADCPLLDCO': ['IFADCPLLDCOTEMPADJ'],
    'SYNTH.LPFCTRL2RX': ['LPFINCAPRX'],
    'SYNTH.LPFCTRL2TX': ['LPFINCAPTX'],
    'MODEM.ANARAMPCTRL': ['ANARAMPLUTODEV'],
}

PROTECTED_FIELDS_BOBCAT = {
    'RAC.PATRIM3': [
        'TXTRIMOREG',
        'TXTRIMRREG',
        'TXTRIMDREG'
    ],
    'RAC.IFADCTRIM0': [
        'IFADCLDOSHUNTAMPLVL1',
        'IFADCLDOSHUNTAMPLVL2',
        'IFADCLDOSHUNTCURLVL1'
    ],
    'RAC.SYTRIM1': ['SYLODIVTLOTRIMDELAY', 'SYLODIVTLO20DBM2G4DELAY'],
    'RAC.LNAMIXTRIM4': ['LNAMIXRFPKDCALCMHI', 'LNAMIXRFPKDCALCMLO'],
    'RAC.PGACTRL': ['PGAPOWERMODE'],
}

PROTECTED_FIELDS_SOL = {
    'RAC.PATRIM3': [
        'TXTRIMOREG',
        'TXTRIMRREG',
        'TXTRIMDREG'
    ],
    'RAC.IFADCTRIM0': [
        'IFADCLDOSHUNTAMPLVL1',
        'IFADCLDOSHUNTAMPLVL2',
        'IFADCLDOSHUNTCURLVL1'
    ],
    'RAC.SYTRIM1': [
        'SYLODIVTLOTRIMDELAY',
        'SYLODIVTLO20DBM2G4DELAY'
    ],
    'RAC.LNAMIXTRIM4': [
        'LNAMIXRFPKDCALCMHI',
        'LNAMIXRFPKDCALCMLO'
    ],
    'RAC.PGACTRL': [
        'PGAPOWERMODE'
    ],
    'RAC.SOFTMCTRL': [
        'CLKEN'
    ],
    'RAC.IFADCPLLDCO': [
        'IFADCPLLDCOTEMPADJ'
    ],
    'RFFPLL0.RFFPLLCAL1': [
        'DIVR'
    ],
    'RFFPLL0.RFFPLLCTRL1': [
        'DIVN',
        'DIVX',
        'DIVY',
        'DIVXDACSEL',
        'DIVXMODEMSEL',
        'DIVYSEL'
    ],
    'SYNTH.LPFCTRL2RX': [
        'LPFINCAPRX'
    ],
    'SYNTH.LPFCTRL2TX': [
      'LPFINCAPTX'
    ],
}

def mergeDictionaries(dict1, dict2):
    dict1Copy = copy.deepcopy(dict1)
    # Merge the keys in dict2 into the copy of dict1
    if dict2 != None:
        for key in dict2.keys():
            if key in dict1Copy:
                dict1Copy[key].extend(dict2[key].copy())
            else:
                dict1Copy[key] = dict2[key].copy()
    return dict1Copy


# Create chip specific protected register files
PROTECTED_FIELDS = {
    'dumbo': mergeDictionaries(PROTECTED_FIELDS_BASE, None),
    'jumbo': mergeDictionaries(PROTECTED_FIELDS_BASE, None),
    'nerio': mergeDictionaries(PROTECTED_FIELDS_BASE, None),
    'nixi': mergeDictionaries(PROTECTED_FIELDS_BASE, None),
    'panther': mergeDictionaries(PROTECTED_FIELDS_BASE, None),
    'lynx': mergeDictionaries(PROTECTED_FIELDS_BASE, None),
    'ocelot': mergeDictionaries(PROTECTED_FIELDS_BASE, PROTECTED_FIELDS_OCELOT),
    'bobcat': mergeDictionaries(PROTECTED_FIELDS_BASE, PROTECTED_FIELDS_BOBCAT),
    'sol': mergeDictionaries(PROTECTED_FIELDS_BASE, PROTECTED_FIELDS_SOL),
    'leopard': mergeDictionaries(PROTECTED_FIELDS_BASE, None),
    'margay': mergeDictionaries(PROTECTED_FIELDS_BASE, PROTECTED_FIELDS_OCELOT),
}

# Radio config action "enum"
RADIO_ACTION_WRITE = 0
RADIO_ACTION_AND = 1
RADIO_ACTION_XOR = 2
RADIO_ACTION_OR = 3
RADIO_ACTION_DELAY = 4
RADIO_ACTION_BITSET = 5
RADIO_ACTION_BITCLR = 6
RADIO_ACTION_WAITFORSET = 7
RADIO_ACTION_DONE = 0xFFFFFFFF
