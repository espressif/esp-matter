from abc import ABCMeta, abstractmethod
import inspect
import types
from pycalcmodel.core.phy import ModelPhy
import os

"""
Phy interface file

This has been updated to handle 3 scenarios:
1) makePhy can be called inside of PHY_PHY1Example, and then if another PHY wishes to call PHY1Example the phy_name and phy_group_name will actually be set based on the highest level PHY in the call stack.
2) makePhy can be called inside of PHY_PHY1Example, and then if another PHY wishes to call PHY1Example they can optionally pass in the phy_name and phy_group_name as parameters.
3) makePhy can be called inside of a high level PHY_PHY2Example that calls PHY1Example. If makePhy is also called inside PHY1Example, then the lower level call will actually not execute and instead return the phy for PHY2Example.

This allows a lot of flexibility in how PHYs are created, and should make PHYs "just work" in most cases.
"""

class IPhy(object):

    """
    Returns list of callable phy functions
    """
    @abstractmethod
    def getPhyList(self):
        functionList = list()
        for functionName, functionPointer in inspect.getmembers(self, predicate=inspect.ismethod):
                if str(functionName).lower().startswith('phy_'):  # Check if the function name starts with "phy_"
                    if isinstance(functionPointer, types.MethodType):
                        #functionList.append(self.__class__.__name__ + "." + functionName)
                        functionList.append(functionPointer)
        return functionList

    """
    Builds empty phy model
    """
    @abstractmethod
    def _makePhy(self, modem_model, profile, phy_description=None, phy_group_name=None, readable_name=None, act_logic='', tags='', phy_name=None, locked=False):
        #Check to see if a higher-level PHY has already been created higher in the stack
        existing_phy = self.__get_phy_already_created(modem_model)
        if existing_phy == None:
            # Build phy from name, version, and class path
            if phy_name is None:
                #Use name of method if no phy name given
                phy_name = self.__highest_caller_phy()
            if phy_description is None:
                if readable_name is None:
                    phy_description = phy_name
                else:
                    phy_description = readable_name
            if phy_group_name is None:
                # Use file name if no group name given
                phy_group_name = self.__highest_caller_group()
            if readable_name is None:
                readable_name = phy_name.replace("_", " ")
            if tags is None:
                tags = ""
            if '_prod' in phy_name.lower():
                #If this is a PROD PHY, then also get the name of the PHY that the PROD PHY points to
                phy_points_to = self.__get_points_to_phy()
            else:
                phy_points_to = None

            if hasattr(self, 'phy_guids') and (modem_model.part_family, phy_name) in self.phy_guids:
                guid = self.phy_guids[modem_model.part_family, phy_name]
            else:
                guid = None

            phy = ModelPhy(phy_name, phy_description, profile, phy_group_name, readable_name=readable_name, act_logic=act_logic, tags=tags, phy_points_to = phy_points_to, locked=locked, guid=guid)
            #Before appending, make sure that this PHY doesn't already exist in the model
            try:
                duplicate_phy = getattr(modem_model, phy_name)
            except AttributeError:
                #If no duplicates found then go ahead and append the phy
                modem_model.phys.append(phy)
            else:
                raise Exception("Error: duplicate definition found for %s" %phy_name)

            return phy
        else:
            #Return the already existing PHY
            return existing_phy

    """
    Helper methods
    """

    def __highest_caller_phy(self):
        #Get the call stack and go through it in reverse order, looking for any entry with PHY_ in its name
        stack = inspect.stack()
        for entry in reversed(stack):
            caller_name = entry[3]
            if caller_name.startswith("PHY_"):
                return caller_name #Return the highest level caller with PHY_ in its name

        #If we did not find a caller name then raise an exception
        raise AssertionError('Did not find a PHY method to name the PHY, check that the method is named correctly')

    def __highest_caller_group(self):
        #Get the call stack and go through it in reverse order, looking for any entry with PHY_ in its name
        stack = inspect.stack()
        for entry in reversed(stack):
            caller_name = entry[3]
            if caller_name.startswith("PHY_"):
                file_path = entry[1]
                file_name = os.path.basename(file_path)
                file_name_no_ext = os.path.splitext(file_name)[0]
                return file_name_no_ext #Return the file name of the highest level caller with PHY_ in its name

        #If we did not find a caller name then raise an exception
        raise AssertionError('Did not find a PHY method to name the PHY group, check that the method is named correctly')

    def __get_phy_already_created(self, model):

        #This method allows, in certain cases, for an existing PHY object in the model to be reused instead of creating
        #a new one. This is intended to be used when a top-level PHY calls makePhy, and then calls a sub-PHY that
        #also calls makePhy. In this case we want the sub-PHY to act on the top-level PHY object.

        stack_frame = inspect.currentframe()
        last_frame = None
        lowest_phy_frame = None
        highest_phy_frame = None

        while lowest_phy_frame is None or highest_phy_frame is None:
            if stack_frame.f_code.co_name.startswith("PHY_"):
                if lowest_phy_frame is None:
                    lowest_phy_frame = stack_frame
            else:
                if lowest_phy_frame is not None:
                    highest_phy_frame = last_frame
            try:
                last_frame = stack_frame
                stack_frame = stack_frame.f_back
            except AttributeError:
                #End of stack, abort
                break

        if lowest_phy_frame is None:
            # We did not find any PHYs, raise an exception
            raise Exception('Did not find a PHY method to name the PHY, check that the method is named correctly')
        elif lowest_phy_frame == highest_phy_frame:
            # If there are no PHYs calling other PHYs, then we will check for duplicates later when appending the
            # new PHY to the model
            return None
        else:
            # We have one PHY calling another, so we will reuse the parent PHY in the model if it exists
            try:
                highest_phy_name = highest_phy_frame.f_code.co_name
                highest_phy = getattr(model.phys, highest_phy_name)
            except AttributeError:
                # If we can't find a phy in the model that matches the calling PHY, then don't return anything
                return None
            else:
                # If we can find a PHY in the model that matches the calling PHY, we can just use that phy object
                # in place of creating a new phy. The caveat to this is that we are assuming the top-level PHY
                # method name matches its entry in the model -- any renaming is going to break this
                return highest_phy

    def __get_points_to_phy(self):
        # Get the call stack and go through it in reverse order, looking for a PROD PHY
        found_prod_phy = False
        stack = inspect.stack()
        for entry in reversed(stack):
            caller_name = entry[3]
            if caller_name.startswith("PHY_") and "_prod" in caller_name.lower():
                #We found a PROD PHY
                found_prod_phy = True
            elif caller_name.startswith("PHY_") and found_prod_phy:
                #If we previously found a PROD PHY, then the next PHY down in the stack is the "points to" PHY
               return caller_name

        return None
