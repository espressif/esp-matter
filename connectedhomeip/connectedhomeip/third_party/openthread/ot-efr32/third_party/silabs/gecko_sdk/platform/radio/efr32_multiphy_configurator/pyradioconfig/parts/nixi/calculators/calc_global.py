from pyradioconfig.parts.nerio.calculators.calc_global import CALC_Global_nerio
from pycalcmodel.core.variable import ModelVariableFormat

class CALC_Global_nixi(CALC_Global_nerio):

    def buildVariables(self, model):

        # Build variables from the Nerio calculations
        super().buildVariables(model)

        #Add additional Nerio variables
        self._addModelRegister(model, 'MODEM.VTCORRCFG1.EXPSYNCLEN',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.VTCORRCFG1.BUFFHEAD',             int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.VTCORRCFG1.EXPECTHT',             int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.DSACTRL.AMPJUPTHD',               int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.DSACTRL.GAINREDUCDLY',            int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.COH0.COHCHPWRTH2',                int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH0.COHCHPWRTH1',                int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH0.COHCHPWRTH0',                int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH0.COHDYNAMICSYNCTHRESH',       int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH0.COHDYNAMICBBSSEN',           int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.COH1.SYNCTHRESH0',                int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH1.SYNCTHRESH1',                int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH1.SYNCTHRESH2',                int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH1.SYNCTHRESH3',                int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.COH2.SYNCTHRESHDELTA0',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH2.SYNCTHRESHDELTA1',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH2.SYNCTHRESHDELTA2',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH2.SYNCTHRESHDELTA3',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH2.DSAPEAKCHPWRTH',             int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH2.FIXEDCDTHFORIIR',            int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.COH3.COHDSAEN',                   int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.COHDSAADDWNDSIZE',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.CDSS',                       int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.DSAPEAKCHKEN',               int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.DSAPEAKINDLEN',              int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.DSAPEAKCHPWREN',             int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.LOGICBASEDCOHDEMODGATE',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.DYNIIRCOEFOPTION',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.ONEPEAKQUALEN',              int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.PEAKCHKTIMOUT',              int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.COH3.COHDSADETDIS',               int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.CTRL6.DSSS3SYMBOLSYNCEN',         int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.VTTRACK.HIPWRTHD',                int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'MODEM.DSATHD2.PMDETFORCE',              int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.DSATHD2.INTERFERDET',             int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.DSATHD2.FREQESTTHD',              int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.DSATHD2.PMDETPASSTHD',            int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.DSATHD2.FDADJTHD',                int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.DSATHD2.JUMPDETEN',               int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.DSATHD2.POWABSTHDLOG',            int, ModelVariableFormat.HEX)


