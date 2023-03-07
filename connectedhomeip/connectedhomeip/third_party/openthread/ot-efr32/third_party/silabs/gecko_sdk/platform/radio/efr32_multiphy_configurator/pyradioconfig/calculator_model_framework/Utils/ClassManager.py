import inspect
#import md5
import os.path
import imp
import traceback
import os

from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr

__all__ = ['ClassManager']

"""
Common utitly class
"""
class ClassManager(object):
    """
    Loads py file by path
    """
    @staticmethod
    def load_module(full_code_path):
        try:
            try:
                #code_dir = os.path.dirname(full_code_path)
                # code_file = os.path.basename(full_code_path)
                # code_file, ext = os.path.splitext(code_file)

                # Hack:  Can't use "." in class name,
                # so repalced with "_"
                class_name = full_code_path.replace(".", "_")
                class_name = class_name.replace("\\", "_")
                class_name = class_name.replace("/", "_")
                pos = class_name.rfind('.py')
                class_name = class_name[:pos]  # remove .py extention
                pos = class_name.rfind(':_')
                class_name = class_name[pos+2:]  # starting drive

                fin = open(full_code_path, 'rb')

                #return  imp.load_source(md5.new(full_code_path).hexdigest(), full_code_path, fin)
                name, ext = os.path.splitext(full_code_path)
                if ext.upper() == '.PY':
                    return imp.load_source(class_name, full_code_path, fin)
                else:
                    return imp.load_compiled(class_name, full_code_path + "c", fin)
            finally:
                try: fin.close()
                except: pass
        # These blocks appear to be redundant. Is there something above them swallowing the exception?
        except ImportError:
            traceback.print_exc()
            raise
        except:
            traceback.print_exc()
            raise

    """
    Loads itertable classes file by path
    """
    @staticmethod
    def getClasses(path):
        try:
            #path = path + "0" # HACK!!!
            # Import profile modules and classes
            path = os.path.realpath(path)
            if os.path.exists(path):
                foo = imp.load_source('__init__', path)
            else:
                path = path + 'c'  # try .pyc file if .py does nto exists
                if os.path.exists(path):
                    foo = imp.load_compiled('__init__', path)
                else:
                    path = path.replace('.pyc', '$py.class')  # try $py.class file if .pyc does nto exists
                    foo = imp.load_compiled('__init__', path)
            #foo = imp.load_source(md5.new(path).hexdigest() + '__init__', path)
            for file in foo.modules:
                # Load each profile class individually
                fooModule = ClassManager.load_module(file)
                for cls in dir(fooModule):          #<-- Loop over all objects in the module's namespace
                    cls = getattr(fooModule, cls)
                    if inspect.isclass(cls) and inspect.getmodule(cls) == fooModule:
                        # Make sure it is a class
                        # and
                        # Make sure it was defined in module, not just imported
                        yield cls
        except ImportError:
            LogMgr.Error("Unable to import modules at: %s" % (path,))
            raise
        except Exception:
            LogMgr.Error("Path error for: " + path)
            #time.sleep(20)
            traceback.print_exc()
            raise

    @staticmethod
    def getClassListFromPath(path, class_type):
        class_list = []

        # Import profile modules and classes
        classes = ClassManager.getClasses(os.path.realpath(path))
        for cls in classes:
            if issubclass(cls, class_type):  # Make sure it is a Calculator class
                # Add everything to list
                class_list.append(cls())

        return class_list

    @staticmethod
    def merge_lists_classes(common_classes, part_specific_classes):
        # Loop through common calculators and remove and parent classes from part rev specific instance
        for common_class in common_classes[:]:
            common_calculator_name = str(common_class.__class__.__name__)
            for part_specific_class in part_specific_classes:
                if ClassManager.class_is_parent_whole_hierarchy(common_calculator_name, part_specific_class):
                    common_classes.remove(common_class)
                    break

        class_list = []
        class_list.extend(part_specific_classes)
        class_list.extend(common_classes)
        return class_list


    @staticmethod
    def class_is_parent_whole_hierarchy(class_name_to_search_for, class_object):
        try:
            parent_classes = class_object().__class__.__bases__
        except TypeError:
            parent_classes = class_object.__class__.__bases__

        for parent_class in parent_classes:
            parent_class_name = str(parent_class.__name__)
            if parent_class_name == class_name_to_search_for:
                return True

            return ClassManager.class_is_parent_whole_hierarchy(class_name_to_search_for, parent_class)
        return False
