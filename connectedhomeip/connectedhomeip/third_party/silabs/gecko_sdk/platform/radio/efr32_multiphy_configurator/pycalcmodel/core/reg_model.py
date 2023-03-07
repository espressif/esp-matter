from host_py_rm_studio_internal.factory import RM_Factory, RM_ALL_PART_FAMILY_NAMES

#This is a static class that stores the reg model only for parts accessed during execution
class RegModel(object):

    _reg_model_dict = dict()

    @classmethod
    def get_reg_model(cls, part_family):

        part_family = part_family.lower()

        if part_family in cls._reg_model_dict.keys():
            #Already in reg model
            reg_model = cls._reg_model_dict[part_family]
        else:
            if part_family.upper() in RM_ALL_PART_FAMILY_NAMES:
                #Add to reg model
                cls._reg_model_dict[part_family] = RM_Factory(part_family.upper())()
                reg_model = cls._reg_model_dict[part_family]
            else:
                #Can't add to reg model
                reg_model = None

        return reg_model
