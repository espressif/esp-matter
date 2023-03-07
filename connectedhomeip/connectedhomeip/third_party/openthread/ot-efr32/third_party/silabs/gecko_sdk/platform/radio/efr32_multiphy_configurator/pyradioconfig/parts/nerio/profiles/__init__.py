"""
Nerio specific Profiles

How to add a new Profile:
--------------------------
*  Add a new Python file in this directory (example: Profile_[Name].py)
*  Inside the Py file, create a class that implements IProfile:

    >>> class Profile_Base(IProfile):

*  Implement/override buildProfileModel() function.  This function builds the profile inputs, forced, outputs into modem model.
    Example:

    >>>     def buildProfileModel(self, model):
    >>>         # Build profile
    >>>         profile = self._makeProfile(model)
    >>>
    >>>         profile.inputs.append(ModelInput(model.vars.xtal_frequency_hz,      "crystal",  input_type=ModelInputType.REQUIRED, readable_name="Crystal Frequency",          value_limit_min=38000000,  value_limit_max=40000000))
    >>>         profile.inputs.append(ModelInput(model.vars.rx_xtal_error_ppm,      "crystal",  input_type=ModelInputType.REQUIRED, readable_name="RX Crystal Accuracy",        value_limit_min=0,         value_limit_max=200))
    >>>
    >>>         # Intermediate values
    >>>         IProfile.make_linked_io(profile, model.vars.timing_detection_threshold        , 'Advanced', readable_name='Timing Detection Threshold',                   value_limit_min=0,      value_limit_max=255)
    >>>
    >>>             # Output fields
    >>>         profile.outputs.append(ModelOutput(model.vars.SYNTH_CTRL_PRSMUX1           , '', ModelOutputType.SVD_REG_FIELD, readable_name='SYNTH.CTRL.PRSMUX1'             ))
    >>>
    >>>         return profile

"""
import os
import glob
modules = glob.glob(os.path.dirname(__file__)+"/*.py")
if len(modules) == 0:
    modules = glob.glob(os.path.dirname(__file__)+"/*.pyc")
if len(modules) == 0:
    modules = glob.glob(os.path.dirname(__file__)+"/*$py.class")
__all__ = [ os.path.basename(f)[:-3] for f in modules]