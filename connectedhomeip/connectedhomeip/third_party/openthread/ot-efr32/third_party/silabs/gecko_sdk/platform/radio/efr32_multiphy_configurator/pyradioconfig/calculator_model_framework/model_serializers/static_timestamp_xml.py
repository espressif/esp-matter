import datetime
import os

from pyradioconfig.calculator_model_framework.Utils.FileUtilities import FileUtilities


class Static_TimeStamp_XML(object):

    #
    # Print a modem_model object to a XML file, with a static timestamp
    #
    @staticmethod
    def print_modem_model_values_xml(outputfilename, phy_name, model_instance, comment=None):
        phy = getattr(model_instance.phys, phy_name)
        profile = getattr(model_instance.profiles, phy.profile_name)
        static_timestamp = datetime.datetime(1984, 1, 2, 12, 34, 56)
        part_rev = model_instance.part_revision
        result_code = model_instance.result_code
        error_message = model_instance.error_message

        if comment is None:
            comment = phy_name + " instance"

        # create dir path, if not exists
        FileUtilities.makedirs(os.path.dirname(outputfilename))

        model_instance.to_instance_xml(outputfilename, part_rev, comment, True, result_code, error_message, profile, phy, static_timestamp)