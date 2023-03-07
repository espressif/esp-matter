from pyradioconfig.parts.ocelot.calculators.calc_global import CALC_Global_ocelot
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

class Calc_Global_Sol(CALC_Global_ocelot):

    def buildVariables(self, model):

        # Build variables from the Ocelot calculations
        super().buildVariables(model)

        # new actual variables for Ocelot
        self._addModelActual(model,    'srcsrd'            , int,   ModelVariableFormat.DECIMAL)

        # new model variables for Sol
        self._addModelRegister(model, 'RFFPLL0.RFFPLLCTRL1.DIVXADCSEL', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RFFPLL0.RFFPLLCTRL1.DIVXDACSEL', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RFFPLL0.RFFPLLCTRL1.DIVX', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RFFPLL0.RFFPLLCTRL1.DIVY', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RFFPLL0.RFFPLLCTRL1.DIVN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RFFPLL0.RFFPLLCTRL1.DIVXMODEMSEL', int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.CTRL5.INTOSR',              int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.CTRL5.DEC2',              int, ModelVariableFormat.HEX)
        
        self._addModelRegister(model, 'FEFILT0.CF.DEC0'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.CF.DEC1'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.DEC1GAIN'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.BBSS'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.CF.ADCBITORDERI', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CF.ADCBITORDERQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.SRC.SRCRATIO',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.SRC.SRCENABLE',          int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.SRC.SRCSRD',          int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.SRCCHF.SRCDECEN2',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.DIGIGAINEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.DIGIGAIN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.DEC0GAIN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE00.SET0COEFF0', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE00.SET0COEFF1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE00.SET0COEFF2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE01.SET0COEFF3', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE01.SET0COEFF4', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE02.SET0COEFF5', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE02.SET0COEFF6', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE03.SET0COEFF7', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE03.SET0COEFF8', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE04.SET0COEFF9', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE04.SET0COEFF10', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE05.SET0COEFF11', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE05.SET0COEFF12', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE06.SET0COEFF13', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE06.SET0COEFF14', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE10.SET1COEFF0', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE10.SET1COEFF1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE10.SET1COEFF2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE11.SET1COEFF3', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE11.SET1COEFF4', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE12.SET1COEFF5', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE12.SET1COEFF6', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE13.SET1COEFF7', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE13.SET1COEFF8', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE14.SET1COEFF9', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE14.SET1COEFF10', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE15.SET1COEFF11', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE15.SET1COEFF12', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE16.SET1COEFF13', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE16.SET1COEFF14', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCTRL.FWSWCOEFFEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCTRL.FWSELCOEFF', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCTRL.SWCOEFFEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCTRL.CHFLATENCY', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGMIXCTRL.DIGIQSWAPEN'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.DIGMIXCTRL.DIGMIXFREQ',      int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGMIXCTRL.MIXERCONJ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGMIXCTRL.DIGMIXFBENABLE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCGAINGEAREN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCGAINGEAR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCGAINGEARSMPS', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCESTIEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCCOMPEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCRSTEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCCOMPFREEZE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCCOMPGEAR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCLIMIT', int, ModelVariableFormat.HEX)        
        self._addModelRegister(model, 'FEFILT0.DCCOMPFILTINIT.DCCOMPINITVALI', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMPFILTINIT.DCCOMPINITVALQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMPFILTINIT.DCCOMPINIT', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.SRC.UPGAPS', int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.PHDMODANTDIV.ANTDECRSTBYP', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.PHDMODANTDIV.RECHKCORREN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.PHDMODANTDIV.SKIPTHDSEL', int, ModelVariableFormat.HEX)


        self._addModelRegister(model, 'FEFILT1.CF.DEC0'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.CF.DEC1'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.DEC1GAIN'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.BBSS'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.CF.ADCBITORDERI', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CF.ADCBITORDERQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.SRC.SRCRATIO',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.SRC.SRCENABLE',          int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.SRC.SRCSRD',          int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.SRCCHF.SRCDECEN2',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.DIGIGAINEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.DIGIGAIN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.DEC0GAIN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE00.SET0COEFF0', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE00.SET0COEFF1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE00.SET0COEFF2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE01.SET0COEFF3', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE01.SET0COEFF4', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE02.SET0COEFF5', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE02.SET0COEFF6', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE03.SET0COEFF7', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE03.SET0COEFF8', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE04.SET0COEFF9', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE04.SET0COEFF10', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE05.SET0COEFF11', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE05.SET0COEFF12', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE06.SET0COEFF13', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE06.SET0COEFF14', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE10.SET1COEFF0', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE10.SET1COEFF1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE10.SET1COEFF2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE11.SET1COEFF3', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE11.SET1COEFF4', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE12.SET1COEFF5', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE12.SET1COEFF6', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE13.SET1COEFF7', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE13.SET1COEFF8', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE14.SET1COEFF9', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE14.SET1COEFF10', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE15.SET1COEFF11', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE15.SET1COEFF12', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE16.SET1COEFF13', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE16.SET1COEFF14', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCTRL.FWSWCOEFFEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCTRL.FWSELCOEFF', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCTRL.SWCOEFFEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCTRL.CHFLATENCY', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGMIXCTRL.DIGIQSWAPEN'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.DIGMIXCTRL.DIGMIXFREQ',      int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGMIXCTRL.MIXERCONJ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGMIXCTRL.DIGMIXFBENABLE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCGAINGEAREN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCGAINGEAR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCGAINGEARSMPS', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCESTIEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCCOMPEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCRSTEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCCOMPFREEZE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCCOMPGEAR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCLIMIT', int, ModelVariableFormat.HEX)        
        self._addModelRegister(model, 'FEFILT1.DCCOMPFILTINIT.DCCOMPINITVALI', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMPFILTINIT.DCCOMPINITVALQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMPFILTINIT.DCCOMPINIT', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.SRC.UPGAPS', int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'TXFRONT.INT1COEF01.COEFF0', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF01.COEFF1', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF23.COEFF2', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF23.COEFF3', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF45.COEFF4', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF45.COEFF5', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF67.COEFF6', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF67.COEFF7', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF89.COEFF8', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF89.COEFF9', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT1COEF1011.COEFF10', int, ModelVariableFormat.HEX)    
        self._addModelRegister(model, 'TXFRONT.INT1COEF1011.COEFF11', int, ModelVariableFormat.HEX)    
        self._addModelRegister(model, 'TXFRONT.INT1COEF1213.COEFF12', int, ModelVariableFormat.HEX)    
        self._addModelRegister(model, 'TXFRONT.INT1COEF1213.COEFF13', int, ModelVariableFormat.HEX)    
        self._addModelRegister(model, 'TXFRONT.INT1COEF1415.COEFF14', int, ModelVariableFormat.HEX)    
        self._addModelRegister(model, 'TXFRONT.INT1COEF1415.COEFF15', int, ModelVariableFormat.HEX)    
        self._addModelRegister(model, 'TXFRONT.INT1CFG.RATIO', int, ModelVariableFormat.HEX)           
        self._addModelRegister(model, 'TXFRONT.INT1CFG.GAINSHIFT', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.INT2CFG.RATIO', int, ModelVariableFormat.HEX)           
        self._addModelRegister(model, 'TXFRONT.INT2CFG.GAINSHIFT', int, ModelVariableFormat.HEX)       
        self._addModelRegister(model, 'TXFRONT.SRCCFG.RATIO', int, ModelVariableFormat.HEX)                  
        self._addModelRegister(model, 'RAC.TXOFDM.TXENBBREG', int, ModelVariableFormat.HEX)            
        self._addModelRegister(model, 'RAC.TXOFDM.TXENMIX', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.TXMIX.TXSELMIXCTUNE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.TXMIX.TXSELMIXGMSLICEI', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.TXMIX.TXSELMIXGMSLICEQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.TXMIX.TXSELMIXRLOAD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.TXMIX.TXSELMIXBAND', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.TXMIX.TXMIXCAPPULLDOWN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.SCRATCH6.SCRATCH6', int, ModelVariableFormat.HEX)           
        self._addModelRegister(model, 'RAC.SOFTMCTRL.TXMODSEL', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.SOFTMCTRL.CLKEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.RX.FEFILTOUTPUTSEL', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.PATRIM6.TXTRIMBBREGFB', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'RAC.PATRIM6.TXTRIMFILGAIN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'AGC.SETTLINGINDCTRL.EN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'AGC.SETTLINGINDCTRL.POSTHD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'AGC.SETTLINGINDCTRL.NEGTHD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'AGC.SETTLINGINDPER.SETTLEDPERIOD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'AGC.SETTLINGINDPER.DELAYPERIOD', int, ModelVariableFormat.HEX)

        # SEQ Peripheral - MODEMINFO register
        self._addModelRegister(model, 'SEQ.MODEMINFO.LEGACY_EN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SEQ.MODEMINFO.DSA_VITERBI_EN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SEQ.MODEMINFO.TRECS_EN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SEQ.MODEMINFO.BCR_EN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SEQ.MODEMINFO.COHERENT_EN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SEQ.MODEMINFO.HARDMODEM_SPARE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SEQ.MODEMINFO.SOFTMODEM_DEMOD_EN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SEQ.MODEMINFO.SOFTMODEM_MOD_EN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SEQ.MODEMINFO.SOFTMODEM_SPARE', int, ModelVariableFormat.HEX)

        ### Defining SoftModem Regs ###

        # CW Peripheral
        self._addModelRegister(model, 'CW.CFG1.MODULATION', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'CW.CFG1.NFFTLOG2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'CW.CFG1.DUALPATH', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'CW.CFG1.FFTANALYZE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'CW.CFG1.SIGTYPE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'CW.CFG2.FREQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'CW.CFG2.AMP', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'CW.CFG3.XTALFREQ', int, ModelVariableFormat.HEX)

        # SUNOFDM Peripheral
        self._addModelRegister(model, 'SUNOFDM.CFG1.MODULATION', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNOFDM.CFG1.OFDMOPTION', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNOFDM.CFG1.INTERLEAVING', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNOFDM.CFG1.MACFCSTYPE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNOFDM.CFG1.XTALFREQ', int, ModelVariableFormat.HEX)

        # SUNOQPSK Peripheral
        self._addModelRegister(model, 'SUNOQPSK.CFG1.MODULATION', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNOQPSK.CFG1.CHIPRATE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNOQPSK.CFG1.BANDFREQMHZ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNOQPSK.CFG2.MACFCSTYPE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNOQPSK.CFG2.XTALFREQ', int, ModelVariableFormat.HEX)

        # LEGOQPSK Peripheral
        self._addModelRegister(model, 'LEGOQPSK.CFG1.MODULATION', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'LEGOQPSK.CFG1.CHIPRATE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'LEGOQPSK.CFG1.BANDFREQMHZ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'LEGOQPSK.CFG2.XTALFREQ', int, ModelVariableFormat.HEX)

        # SUNFSK Peripheral
        self._addModelRegister(model, 'SUNFSK.CFG1.MODULATION', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG1.FECSEL', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG1.PHYSUNFSKSFD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG1.FSKPREAMBLELENGTH', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG1.MODSCHEME', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG1.BT', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG1.MODINDEX', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG2.VEQEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG2.KSI1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG2.KSI2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG2.KSI3', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG3.KSI3W', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG3.PHASESCALE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG3.OSR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG4.PREAMBLECOSTTHD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG4.PREAMBLECNTWIN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG4.PREAMBLECNTTHD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG5.PREAMBLELEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG5.PREAMBLEERRORS', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG5.PREAMBLETIMEOUT', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG6.PREAMBLEPATT', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG7.PREAMBLEFREQLIM', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG7.PREAMBLECOSTMAX', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG8.SFD1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG9.SFD2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG10.SFDLEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG10.SFDCOSTTHD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG11.SFDTIMEOUT', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG11.SFDERRORS', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG11.TIMINGTRACKGEAR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG12.TIMINGTRACKSYMB', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG12.TIMINGMINTRANS', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG12.REALTIMESEARCH', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG12.NBSYMBBATCH', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG13.CHFILTERSWEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG13.AFCMODE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG13.AFCSCALE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG14.AFCADJLIM', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG15.AFCADJPERIOD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG15.AFCCOSTTHD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG15.XTALFREQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'SUNFSK.CFG16.MODE', int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'FRC.SPARE.SPARE', int, ModelVariableFormat.HEX)


    def _add_modulation_type_variable(self, model):

        #This method is called in the inherited Common calculations
        #Adding the method here overrides the modulation type for Sol

        var = self._addModelVariable(model, 'modulation_type', Enum, ModelVariableFormat.DECIMAL,
                                     'Defines the modulation type.')
        member_data = [
            ['FSK2', 0, 'Frequency Shift Keying on two frequencies'],
            ['FSK4', 1, 'Frequency Shift Keying on four frequencies'],
            ['BPSK', 2,
             'Binary Phase Shift Keying: the 2 symbols are represented by 0 or 180 degree phase shifts wrt the carrier'],
            ['DBPSK', 3,
             'Differential Binary Phase Shift Keying: the 2 symbols are represented by 0 or 180 degree phase shifts wrt the preceding symbol'],
            ['OOK', 4, 'On Off Keying: the 2 symbols are represented by the presence / absence of the carrier'],
            ['ASK', 5,
             'Amplitude Shift Keying: the 2 symbols are represented by two different power levels of the carrier'],
            ['MSK', 6,
             'Minimum Shift Keying: Special case of FSK2 where the phase shift in one symbol is +/- 90 degree'],
            ['OQPSK', 7,
             'Offset Quadrature Phase Shift Keying: 4 state phase modulation with 0, 90, 180 and 270 degrees wrt the carrier. Only +/-90 degree changes are allowed at any one transition that take place at twice the symbol rate.'],
            ['OFDM', 8, 'Orthogonal Frequency-Division Multiplexing'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'ModModeEnum',
            'Defines the modulation type.',
            member_data)

    def _add_MODEM_RXRESTART(self, model):
        self._addModelRegister(model, 'MODEM.RXRESTART.FLTRSTEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.RXRESTART.ANTSWRSTFLTTDIS', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.RXRESTART.RXRESTARTB4PREDET', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.RXRESTART.RXRESTARTUPONMARSSI', int, ModelVariableFormat.HEX)

    def _add_baudrate_variable(self, model):
        self._addModelVariable(model, 'baudrate', float, ModelVariableFormat.DECIMAL)

