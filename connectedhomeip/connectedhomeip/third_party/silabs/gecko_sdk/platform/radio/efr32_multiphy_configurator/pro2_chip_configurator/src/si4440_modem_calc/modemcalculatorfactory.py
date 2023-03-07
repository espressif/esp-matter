'''
Created on Mar 20, 2013

@author: sesuskic
'''
import csv
import xml.parsers.expat
from .pro2modemcalc import Pro2ModemCalc
from .pro2plusmodemcalc import Pro2PlusModemCalc, Pro2PlusLegacyModemCalc
from .pro2modemcalcinputs import Pro2ModemCalcInputs
from .pro2plusmodemcalcinputs import Pro2PlusModemCalcInputs

__all__ = ["create_modem_calculator"]

def _matlabdatafile(filename, radio):
    inputparams = []
    data_lines = []
    f = open(filename, 'rU')
    for line in f.readlines():
        if (not(line.startswith('%')) or not(line.strip())):
            data_lines += [(line.split('%')[0]).replace(' ', '\t')]
    if len(data_lines) == 0:
        raise RuntimeError('File {:s} has invalid content'.format(f.name))
    matlab_file_data = csv.reader(data_lines, delimiter='\t')
    for row in matlab_file_data:
        for item in filter(None, row):
            try:
                inputparams += [float(item)]
            except ValueError:
                raise RuntimeError('{:s} file has invalid file content'.format(filename))
    # Legacy matlab format input file specified frequency in MHz, but the
    # calculator core works with HZ.
    inputparams[16] *= 1e6
    if radio == 'pro2':
        if not(len(inputparams) in [24, 27]):
            raise RuntimeError('Unknown input format in file {:s}'.format(filename))
        return Pro2ModemCalcInputs(inputparams)
    elif radio in ['pro2plus', 'pro2plus_legacy']:
        if not (len(inputparams) in [24, 27, 29, 48, 49]):
            raise RuntimeError('Unknown input format in file {:s}'.format(filename))
        return Pro2PlusModemCalcInputs(inputparams)
    else:
        raise RuntimeError('Invalid radio')
        

def _xmlparse(filename):
    parser = xml.parsers.expat.ParserCreate()
    parser.ParseFile(open(filename, 'rU'))

def _createmodeminput(testplan, radio):
#     try:
        open(testplan, 'rU')
        try:
            _xmlparse(testplan)
        except Exception:
            pass
#             print('Not XML file')
        
        return _matlabdatafile(testplan, radio)
#     except IOError:
#         pass
#         print('Not a file. Try parsing XML stream')

def create_modem_calculator(testplan, radio='pro2'):
    inputs = _createmodeminput(testplan, radio)
    if radio == 'pro2':
        modem_calc = Pro2ModemCalc(inputs)
    elif radio == 'pro2plus':
        modem_calc = Pro2PlusModemCalc(inputs)
    elif radio == 'pro2plus_legacy':
        modem_calc = Pro2PlusLegacyModemCalc(inputs)
    else:
        raise RuntimeError('{:s}: invalid radio option'.format(radio)) 
    return modem_calc
